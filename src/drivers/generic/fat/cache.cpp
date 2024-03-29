/*
	drivers/generic/fat/cache.cpp

	Copyright (c) 2023-2024 Omar Berrow
*/

#include <int.h>
#include <klog.h>
#include <utils/hashmap.h>
#include <utils/vector.h>
#include <utils/string.h>

#include <vfs/devManip/driveHandle.h>
#include <vfs/devManip/driveIterator.h>
#include <vfs/devManip/sectorStore.h>

#include <vfs/vfsNode.h>

#include <driverInterface/struct.h>

#include <multitasking/cpu_local.h>

#include <allocators/vmm/vmm.h>

#include "cache.h"
#include "fat_structs.h"

using namespace obos;

namespace fatDriver
{
	bool operator==(const partition& first, const partition& second)
	{
		return
			first.driveId == second.driveId &&
			first.partitionId == second.partitionId;
	}
	utils::Vector<partition> g_partitions;
	obos::utils::Hashmap<partitionIdPair, ::size_t> g_partitionToIndex;
	static bool Probe(vfs::DriveHandle& handle, generic_bpb** _bpb)
	{
		generic_bpb* bpb = new generic_bpb{};
		if (!handle.ReadSectors(bpb, nullptr, 0, 1))
			return false;
		bool ret = 
			utils::memcmp((uint8_t*)bpb + 0x36, "FAT", 3) ^
			utils::memcmp((uint8_t*)bpb + 0x52, "FAT", 3);
		if (bpb->totalSectorCountOnVolume16 > 0 && bpb->totalSectorCountOnVolume32 > 0)
			ret = false;
		if (_bpb)
			*_bpb = ret ? bpb : nullptr;
		if (!ret || !bpb)
			delete bpb;
		return ret;
	}
	static fatType GetFatType(generic_bpb* bpb)
	{
		fatType type = fatType::INVALID;
		// Get the FAT type. We might need to infer it from the cluster count if we don't have a trusted OEM string.
		logger::debug("FAT: Partition OEM Identifier: %c%c%c%c%c%c%c%c\n",
			bpb->oem_identifer[0], bpb->oem_identifer[1], bpb->oem_identifer[2], bpb->oem_identifer[3],
			bpb->oem_identifer[4], bpb->oem_identifer[5], bpb->oem_identifer[6], bpb->oem_identifer[7]);
		// TODO: Add ourselves to this list when we support making partitions.
		if (utils::memcmp(bpb->oem_identifer, "MSDOS5.1", 8) || 
			utils::memcmp(bpb->oem_identifer, "MSWIN4.1", 8) ||
			utils::memcmp(bpb->oem_identifer,  "mkdosfs", 7) ||
			utils::memcmp(bpb->oem_identifer, "mkfs.fat", 8) ||
			utils::memcmp(bpb->oem_identifer, "FRDOS5.1", 8))
		{
			logger::debug("FAT: Trusted OEM Identifier. Inferring FAT type from system identifier string.\n");
			// We should be able to trust the ebpb's system identifier string.
			if (utils::memcmp((char*)bpb + 0x36, "FAT12", 5))
				type = fatType::FAT12;
			else if (utils::memcmp((char*)bpb + 0x36, "FAT16", 5))
				type = fatType::FAT16;
			else if (utils::memcmp(bpb->ebpb.fat32_ebpb.sysIdentifierString, "FAT32", 5))
				type = fatType::FAT32;
		}
		else
		{
			// This is an untrustworthy FAT partition, infer from cluster count.
			logger::debug("FAT: Untrusted OEM Identifier. Inferring FAT type from cluster count.\n");
			auto RootDirSectors = ((bpb->nRootDirectoryEntries * 32) + (bpb->bytesPerSector - 1)) / bpb->bytesPerSector;
			uint16_t FatSz = 0;
			uint32_t TotSec = 0;
			if (bpb->sectorsPerFAT != 0)
				FatSz = bpb->sectorsPerFAT;
			else
				FatSz = bpb->ebpb.fat32_ebpb.sectorsPerFAT;

			if (bpb->totalSectorCountOnVolume16 != 0)
				TotSec = bpb->totalSectorCountOnVolume16;
			else
				TotSec = bpb->totalSectorCountOnVolume32;
			auto DataSec = TotSec - (bpb->nResvSectors + (bpb->nFats * FatSz) + RootDirSectors);
			auto clusterCount = DataSec / bpb->sectorsPerCluster;
			if (clusterCount < 4085)
				type = fatType::FAT12;
			else if (clusterCount < 65525)
				type = fatType::FAT16;
			else
				type = fatType::FAT32;
		}
		return type;
	}
	struct temp_directoryEntryCache
	{
		cacheEntry* _cacheEntry;
		fat_dirEntry* directoryEntry;
	};
	static char16_t lfn_at(const fat_lfn* lfn, size_t i)
	{
		char16_t ret = 0;
		if (i < 5)
			ret = lfn->name1_5[i];
		else if (i >= 5 && i < 11)
			ret = lfn->name6_11[i - 5];
		else if (i == 11 || i == 12)
			ret = lfn->name12_13[i - 11];
		return ret;
	}
	static size_t lfn_strlen(const fat_lfn* lfn)
	{
		size_t ret = 0;
		for (; lfn_at(lfn, ret) && ret < 13; ret++);
		return ret;
	}
	static bool FAT32LookForEntriesInDirectory(
		const vfs::DriveHandle& ,
		partition& , 
		generic_bpb* bpb, 
		const fat_dirEntry* directory, 
		const utils::String &initPath,
		utils::Vector<temp_directoryEntryCache*>& cacheEntries)
	{
		utils::String curPath = initPath;
		bool isLFN = false;
		utils::Vector<const fat_lfn*> lfnEntriesInOrder;
		for (const fat_dirEntry* curEnt = directory; curEnt->fname[0]; curEnt++)
		{
			if (curEnt->fname[0] == 0 || curEnt->fname[0] == -27 /* 0xE5 */)
				continue;
			if (curEnt->fAttribs == fat_dirEntry::LFN)
			{
				isLFN = true;
				const fat_lfn* lfn = (fat_lfn*)curEnt;
				size_t lfnIndex = (lfn->order & ~0x40) - 1;
				if (lfnIndex == lfnEntriesInOrder.length())
				{
					lfnEntriesInOrder.push_back(lfn);
					continue;
				}
				if (lfnIndex < lfnEntriesInOrder.length())
				{
					lfnEntriesInOrder[lfnIndex] = lfn;
					continue;
				}
				for (size_t i = lfnEntriesInOrder.length(); i < lfnIndex; i++)
					lfnEntriesInOrder.push_back(nullptr);
				lfnEntriesInOrder.push_back(lfn);
				continue;
			}
			if (curEnt->fAttribs & fat_dirEntry::VOLUME_ID)
				continue;
			if (isLFN)
			{
				for (auto lfn : lfnEntriesInOrder)
				{
					if (!lfn)
						continue;
					size_t len = lfn_strlen(lfn);
					for (size_t i = 0; i < len; i++)
						curPath.push_back((char)lfn_at(lfn, i));
				}
			}
			if (utils::memcmp(curEnt->fname, '.', 2))
				continue;
			if (curEnt->fname[0] == '.')
				continue;
			if (!isLFN)
			{
				if (curEnt->fAttribs & fat_dirEntry::DIRECTORY)
				{
					curPath.append(curEnt->fname, utils::strCountToChar(curEnt->fname, ' ') % 11);
				}
				else
				{
					curPath.append(curEnt->fname, utils::strCountToChar(curEnt->fname, ' ') % 8);
					curPath.push_back('.');
					curPath.append(curEnt->fname + 8, utils::strCountToChar(curEnt->fname + 8, ' ') % 3);
				}
			}
			temp_directoryEntryCache* cache = new temp_directoryEntryCache;
			cache->_cacheEntry = new cacheEntry{};
			if (curEnt->fAttribs & fat_dirEntry::READ_ONLY)
				cache->_cacheEntry->fileAttributes |= driverInterface::FILE_ATTRIBUTES_READ_ONLY;
			if (curEnt->fAttribs & fat_dirEntry::DIRECTORY)
				cache->_cacheEntry->fileAttributes |= driverInterface::FILE_ATTRIBUTES_DIRECTORY;
			else
				cache->_cacheEntry->fileAttributes |= driverInterface::FILE_ATTRIBUTES_FILE;
			if (curEnt->fAttribs & fat_dirEntry::HIDDEN)
				cache->_cacheEntry->fileAttributes |= driverInterface::FILE_ATTRIBUTES_HIDDEN;
			cache->_cacheEntry->path = (char*)utils::memcpy(new char[curPath.length() + 1], curPath.data(), curPath.length());
			uint32_t baseCluster = ((uint32_t)curEnt->cluster0_15 | (uint32_t)(curEnt->cluster16_31 << 16));
			size_t bytesPerCluster = (bpb->sectorsPerCluster * bpb->bytesPerSector);
			uint32_t lastCluster = baseCluster + curEnt->filesize / bytesPerCluster + ((curEnt->filesize % bytesPerCluster) != 0);
			if (curEnt->fAttribs & fat_dirEntry::DIRECTORY)
				lastCluster++; // The driver however, needs to find out the amount of clusters the directory takes
			for (uint32_t curCluster = baseCluster; curCluster < lastCluster; curCluster++)
				cache->_cacheEntry->clusters.push_back(curCluster);
			cache->directoryEntry = new fat_dirEntry{ *curEnt };
			curPath = initPath;
			cacheEntries.push_back(cache);
			lfnEntriesInOrder.clear();
			isLFN = false;
		}
		return true;
	}
	static bool InitializeCacheForFAT32Partition(const vfs::DriveHandle& handle, partition& part, generic_bpb* bpb)
	{
		const auto& ebpb = bpb->ebpb.fat32_ebpb;
		auto RootDirSectors = ((bpb->nRootDirectoryEntries * 32) + (bpb->bytesPerSector - 1)) / bpb->bytesPerSector;
		uint32_t FatSz = bpb->sectorsPerFAT == 0 ? ebpb.sectorsPerFAT : bpb->sectorsPerFAT;
		uint32_t TotSec = bpb->totalSectorCountOnVolume16 == 0 ? bpb->totalSectorCountOnVolume32 : bpb->totalSectorCountOnVolume16;
		auto DataSec = TotSec - (bpb->nResvSectors + (bpb->nFats * FatSz) + RootDirSectors);
		auto ClusterCount = DataSec / bpb->sectorsPerCluster;
		part.ClusterCount = ClusterCount;
		part.DataSec = DataSec;
		part.FirstDataSec = (bpb->nResvSectors + (bpb->nFats * FatSz) + RootDirSectors);
		part.TotSec = TotSec;
		part.FatSz = FatSz;
		part.RootDirSectors = RootDirSectors;
		size_t bytesPerSector = 0;
		size_t partLBAOffset = 0;
		handle.QueryInfo(nullptr, &bytesPerSector, nullptr);
		handle.QueryPartitionInfo(nullptr, &partLBAOffset, nullptr);
		// TODO: Change from Vector to a specialized class specifically made for holding sectors.
		// This will allow for less heap fragmentation.
		// Omar Berrow - I'm sure I'll do it tomorrow (January 15, 2024)
		// We'll see...
		// Omar Berrow - (4:45 PM, January 14 2024) I ended up doing it now and not forgetting!
		// Anyway Imma leave these comments for the next person to see them.
		// Omar Berrow - (6:37 PM, January 18 2024) Lol it's kind of surprising how I didn't forget. I also decided to move the class into vfs/devManip/ so it can be used by all.
		utils::SectorStorage currentCluster{ bpb->sectorsPerCluster * bytesPerSector }, otherCluster{ bpb->sectorsPerCluster * bytesPerSector };
		bytesPerSector = bytesPerSector > bpb->bytesPerSector ? bytesPerSector : bpb->bytesPerSector;
		auto sector = fat32FirstSectorOfCluster(ebpb.rootDirectoryCluster, *bpb, part.FirstDataSec);
		//sector -= partLBAOffset;
		if (!handle.ReadSectors(currentCluster.data(), nullptr, sector, bpb->sectorsPerCluster))
			return false;
		const fat_dirEntry* entry = (fat_dirEntry*)currentCluster.data();
		{
			const fat_dirEntry* lastElement = entry + ((bpb->sectorsPerCluster * bytesPerSector - 32) / 32);
			for (size_t cluster = 1; lastElement->fname[0] != 0; cluster++)
			{
				size_t oldLength = currentCluster.length();
				currentCluster.resize(oldLength + bpb->sectorsPerCluster * bytesPerSector);
				entry = (fat_dirEntry*)currentCluster.data();
				lastElement = entry + (oldLength / sizeof(fat_dirEntry)) + ((bpb->sectorsPerCluster * bytesPerSector - 32) / 32);
				if (!handle.ReadSectors(otherCluster.data() + oldLength, nullptr, sector + (cluster * bpb->sectorsPerCluster), bpb->sectorsPerCluster))
					return false;
			}
		}
		utils::Vector<temp_directoryEntryCache*> cacheEntries;
		// Look in the root directory.
		FAT32LookForEntriesInDirectory(handle,part,bpb, entry,"",cacheEntries);
		for (size_t i = 0; i < cacheEntries.length(); i++)
		{
			auto ent = cacheEntries[i];
			if (ent->_cacheEntry->fileAttributes & driverInterface::FILE_ATTRIBUTES_FILE)
				continue;
			const fat_dirEntry* child = (fat_dirEntry*)otherCluster.data();
			const fat_dirEntry* lastElement = child + ((bpb->sectorsPerCluster * bytesPerSector - 32) / 32);
			sector = fat32FirstSectorOfCluster(ent->_cacheEntry->clusters.at(0), *bpb, part.FirstDataSec);
			if (!handle.ReadSectors(otherCluster.data(), nullptr, sector, bpb->sectorsPerCluster))
				return false;
			for (size_t cluster = 1; lastElement->fname[0] != 0; cluster++)
			{
				size_t oldLength = otherCluster.length();
				otherCluster.resize(oldLength + bpb->sectorsPerCluster * bytesPerSector);
				child = (fat_dirEntry*)otherCluster.data();
				lastElement = child + (oldLength / sizeof(fat_dirEntry)) + ((bpb->sectorsPerCluster * bytesPerSector - sizeof(fat_dirEntry)) / sizeof(fat_dirEntry));
				if (!handle.ReadSectors(otherCluster.data() + oldLength, nullptr, sector + cluster * bpb->sectorsPerCluster, bpb->sectorsPerCluster))
					return false;
			}
			utils::String initPath = ent->_cacheEntry->path;
			initPath.push_back('/');
 			FAT32LookForEntriesInDirectory(handle,part,bpb, child,initPath,cacheEntries);
			otherCluster.resize(bpb->sectorsPerCluster * bytesPerSector);
		}
		for (size_t i = 0; i < cacheEntries.length(); i++)
		{
			auto ent = cacheEntries[i];
			ent->_cacheEntry->filesize = ent->directoryEntry->filesize;
			if (part.tail)
				part.tail->next = ent->_cacheEntry;
			if(!part.head)
				part.head = ent->_cacheEntry;
			ent->_cacheEntry->prev = part.tail;
			part.tail = ent->_cacheEntry;
			part.nCacheEntries++;
			// Omar Berrow, January 19 2024 at 8:31 PM:
			// If anyone read the comment in interface.cpp today at 8:12 PM and is wondering, "but wait, the cache DOES set it," I had done that after writing the comment.
			// Decided to say it so it's clear.
			ent->_cacheEntry->owner = &part;
			delete ent->directoryEntry;
			delete ent;
		}
		return true;
	}
	static bool InitializeCacheForPartition(const vfs::DriveHandle& handle, partition& part, generic_bpb* bpb)
	{
		bool ret = false;
		switch (part.fat_type)
		{
		case fatType::FAT32:
		{
			ret = InitializeCacheForFAT32Partition(handle, part, bpb);
			break;
		}
		default:
		case fatType::FAT12:
		case fatType::FAT16:
			break;
		}
		if (ret)
		{
			// FIXME: part.owner.*id is always zero.
			part.bpb = bpb;
			g_partitions.push_back(part);
			auto& owner = g_partitions[g_partitions.length() - 1];
			for (auto node = owner.head; node;)
			{
				node->owner = &owner;
				node = node->next;
			}
			partitionIdPair p;
			utils::memzero(&p, sizeof(p));
			p.first = part.driveId;
			p.second = part.partitionId;
			g_partitionToIndex.emplace_at(p, g_partitions.length() - 1);
		}
		return ret;
	}
	void ProbeDrives()
	{	
		vfs::DriveHandle drvHandle, partHandle;
		for (vfs::DriveIterator iter; iter; )
		{
			// Loop over the drive's partitions.
			const char* path = iter++;
			drvHandle.OpenDrive(path);
			size_t nPartitions = 0;
			drvHandle.QueryInfo(nullptr, nullptr, &nPartitions);
			utils::String dPath;
			dPath.initialize(path, utils::strlen(path) - 2);
			delete[] path;
			for (size_t part = 0; part < nPartitions; part++)
			{
				char* partPath = new char[logger::sprintf(nullptr, "%*sP%d:/", dPath.length(), dPath.data(), part)];
				logger::sprintf(partPath, "%*sP%d:/", dPath.length(), dPath.data(), part);
				partHandle.OpenDrive(partPath);
				logger::log("FAT Driver: Probing partition at %s.\n", partPath);
				delete[] partPath;
				generic_bpb* bpb = nullptr;
				if (Probe(partHandle, &bpb))
				{
					partition _part;
					utils::memzero(&_part, sizeof(_part));
					_part.driveId = partHandle.GetDriveId();
					_part.partitionId = partHandle.GetPartitionId();
					_part.fat_type = GetFatType(bpb);
					logger::log("FAT Driver: Partition at D%dP%d:/ contains a FAT%d filesystem (we hope). Initializing cache for partition.\n", 
						_part.driveId,
						_part.partitionId, 
						_part.fat_type);
					vfs::PartitionEntry* entry = (vfs::PartitionEntry*)partHandle.GetNode();
					entry->filesystemDriver = (driverInterface::driverIdentity*)thread::GetCurrentCpuLocalPtr()->currentThread->driverIdentity;
					switch (_part.fat_type)
					{
					case fatType::FAT12:
						entry->friendlyFilesystemName = "FAT12";
						break;
					case fatType::FAT16:
						entry->friendlyFilesystemName = "FAT16";
						break;
					case fatType::FAT32:
						entry->friendlyFilesystemName = "FAT32";
						break;
					default:
						break;
					}
					InitializeCacheForPartition(partHandle, _part, bpb);
					// Don't free the bpb, it's used in the cache entries.
					//delete bpb;
				}
				partHandle.Close();
			}
			drvHandle.Close();
		}
	}
}