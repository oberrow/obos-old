/*
	driverInterface/x86_64/scan.cpp

	Copyright (c) 2023-2024 Omar Berrow
*/

#include <int.h>
#include <klog.h>
#include <error.h>

#include <driverInterface/struct.h>
#include <driverInterface/load.h>

#include <driverInterface/x86_64/enumerate_pci.h>

#include <hashmap.h>
#include <vector.h>
#include <string.h>

#include <x86_64-utils/asm.h>

#include <vfs/fileManip/fileHandle.h>
#include <vfs/fileManip/directoryIterator.h>

#include <vfs/vfsNode.h>

#include <vfs/devManip/driveHandle.h>
#include <vfs/devManip/driveIterator.h>

#include <multitasking/process/x86_64/loader/elfStructures.h>

namespace obos
{
	namespace driverInterface
	{
		struct LoadableDriver
		{
			utils::String fullPath;
			driverHeader* header = nullptr;
			LoadableDriver() = default;
			LoadableDriver(const char* path, driverHeader* _header)
				:fullPath{path},header{_header}
			{}
			LoadableDriver(const LoadableDriver& other)
			{
				fullPath = other.fullPath;
				header = other.header;
			}
			void operator=(const LoadableDriver& other)
			{
				fullPath = other.fullPath;
				header = other.header;
			}
			LoadableDriver(LoadableDriver&& other)
			{
				fullPath = other.fullPath;
				header = other.header;
				other.header = nullptr;
				other.fullPath = nullptr;
			}
			void operator=(LoadableDriver&& other)
			{
				fullPath = other.fullPath;
				header = other.header;
				other.header = nullptr;
				other.fullPath = nullptr;
			}

			static size_t hasher(const LoadableDriver& key)
			{
				return utils::defaultHasher<const char*>(key.fullPath.data());
			}
		};
		bool operator==(const LoadableDriver& first, const LoadableDriver& second)
		{
			return utils::strcmp(first.fullPath.data(), second.fullPath.data());
		}
		
		driverHeader* CheckModule(byte* file, size_t size);
		struct pciDevice
		{
			uint8_t classCode : 8;
			uint8_t subclass : 8;
			uint8_t progIF : 8;
		};
		void ScanPCIBus(const utils::Vector<LoadableDriver>& driverList)
		{
			utils::Hashmap<
				LoadableDriver, pciDevice, utils::defaultEquals<LoadableDriver>, LoadableDriver::hasher> driversToLoad;
			uintptr_t udata[2] = { (uintptr_t)&driverList, (uintptr_t)&driversToLoad };
			for (uint16_t bus = 0; bus < 256; bus++)
				enumerateBus((uint8_t)bus, [](
					void* userData, uint8_t, uint8_t, uint8_t,
					uint8_t classCode,
					uint8_t subclass,
					uint8_t progIF)->bool
					{
						uintptr_t* udata = (uintptr_t*)userData;
						const utils::Vector<LoadableDriver>& driverList = *(utils::Vector<LoadableDriver>*)udata[0];
						utils::Hashmap<
							LoadableDriver, pciDevice>& driversToLoad = *((utils::Hashmap<
								LoadableDriver, pciDevice>*)udata[1]);
						pciDevice dev{
							classCode,subclass,progIF
						};
						for (auto& driver : driverList)
						{
							if (driversToLoad.contains(driver))
								continue;
							if (driver.header->howToIdentifyDevice & 1)
							{
								if (driver.header->pciInfo.classCode == dev.classCode)
								{
									bool found = true;
									__uint128_t drv_subclass = driver.header->pciInfo.subclass;
									for (uint8_t csubclass = bsf(drv_subclass); 
												 csubclass != subclass;
												 csubclass = bsf(drv_subclass))
									{
										if (!(drv_subclass & ((__uint128_t)1<<csubclass)))
										{
											found = false;
											break;
										}
										drv_subclass &= ~((__uint128_t)1<<csubclass);
									}
									if (!found)
										continue;
									__uint128_t drv_progIf = driver.header->pciInfo.progIf;
									for (uint8_t cprogIF = bsf(drv_progIf); 
												 cprogIF != progIF;
												 cprogIF = bsf(drv_progIf))
									{
										if (!(drv_progIf & ((__uint128_t)1<<cprogIF)))
										{
											found = false;
											break;
										}
										drv_progIf &= ~((__uint128_t)1<<cprogIF);
									}
									if (!found)
										continue;
									driversToLoad.emplace_at(driver, dev);
								}
							}
						}
						return true;
					}, &udata);
			for(auto iter = driversToLoad.begin(); iter; iter++)
			{
				auto&[driver, unused1, unused2, unused3] = *iter;
				(unused1 = unused1);
				(unused2 = unused2);
				(unused3 = unused3);
				vfs::FileHandle file;
				file.Open(driver->fullPath.data(), vfs::FileHandle::OPTIONS_READ_ONLY);
				// Load the driver's data.
				size_t size = file.GetFileSize();
				char* data = new char[size + 1];
				file.Read(data, size);
				file.Close();
				// Load the driver.
				if (!LoadModule((byte*)data, size, nullptr))
				{
					// Oh no! Anyway...
					delete[] data;
					continue;
				}
				delete[] data;
			}
		}
		static void ScanDrivesForPartitions(const utils::Vector<LoadableDriver>&)
		{
			bool loadMBR = false, loadGPT = false;
			size_t nSectorsRead = 0;
			utils::Vector<uint32_t> gptDrives;
			utils::Vector<uint32_t> mbrDrives;
			for (vfs::DriveIterator drvI; drvI; delete[] (drvI++))
			{
				if (loadGPT && loadMBR)
					break;

				vfs::DriveHandle drv;
				auto path = *drvI;
				drv.OpenDrive(path);
				delete[] path;

				size_t sectorSize = 0, nSectors = 0;
				drv.QueryInfo(&nSectors, &sectorSize, nullptr);
				if (nSectors <= 2)
				{
					loadMBR = true;
					drv.Close();
					continue;
				}

				utils::Vector<byte> currentSector;
				currentSector.resize(sectorSize);

				if (!drv.ReadSectors((void*)currentSector.data(), &nSectorsRead, 1, 1))
					continue;
				if (nSectorsRead != 1)
					continue;
				if (utils::memcmp(currentSector.data(), "EFI PART", 8))
				{
					gptDrives.push_back(drv.GetDriveId());
					continue;
				}
				if (!drv.ReadSectors(currentSector.data(), &nSectorsRead, 0, 1))
					continue;
				if (nSectorsRead != 1)
					continue;
				if (utils::memcmp(&currentSector[510], "\x55\xAA", 2))
				{
					mbrDrives.push_back(drv.GetDriveId());
					continue;
				}
				drv.Close();
				// This drive is... special.
				// Ignore it.
			}
			if (mbrDrives.length())
			{
				if (g_driverInterfaces.contains(2))
					goto next;
				vfs::FileHandle mbrDriver;
				if (!mbrDriver.Open("0:/mbrDriver"))
					logger::panic(nullptr, "Could not load the MBR driver. GetLastError: %d\n", GetLastError());
				size_t filesize = mbrDriver.GetFileSize();
				if (!filesize)
					logger::panic(nullptr, "Could not load the MBR driver. GetLastError: %d\n", GetLastError());
				byte* fdata = new byte[filesize];
				mbrDriver.Read((char*)fdata, filesize);
				mbrDriver.Close();
				if (!LoadModule(fdata, filesize, nullptr))
					logger::panic(nullptr, "Could not load the MBR driver. GetLastError: %d\n", GetLastError());
			}
			next:
			if (gptDrives.length())
			{
				if (g_driverInterfaces.contains(3))
					goto done;
				vfs::FileHandle gptDriver;
				if (!gptDriver.Open("0:/gptDriver"))
					logger::panic(nullptr, "Could not load the GPT driver. GetLastError: %d\n", GetLastError());
				size_t filesize = gptDriver.GetFileSize();
				if (!filesize)
					logger::panic(nullptr, "Could not load the GPT driver. GetLastError: %d\n", GetLastError());
				byte* fdata = new byte[filesize];
				gptDriver.Read((char*)fdata, filesize);
				gptDriver.Close();
				if (!LoadModule(fdata, filesize, nullptr))
					logger::panic(nullptr, "Could not load the GPT driver. GetLastError: %d\n", GetLastError());
			}
		done:
			driverIdentity *gptDriver = nullptr, *mbrDriver = nullptr;
			if (&g_driverInterfaces.at(2))
				mbrDriver = g_driverInterfaces.at(2);
			if (&g_driverInterfaces.at(3))
				gptDriver = g_driverInterfaces.at(3);
			auto iterateDrivesAndRegisterPartitions = [](utils::Vector<uint32_t>& drives, driverIdentity* driver)
				{
					for (auto& driveId : drives)
					{
						size_t nPartitions = 0;
						partitionInfo* partitions;
						driver->functionTable.serviceSpecific.partitionManager.RegisterPartitionsOnDrive(driveId, &nPartitions, &partitions);
						vfs::DriveEntry* cdrive = vfs::g_drives.head;
						vfs::DriveEntry* drive = nullptr;
						while (cdrive)
						{
							if (cdrive->driveId == driveId)
							{
								drive = cdrive;
								break;
							}

							cdrive = cdrive->next;
						}
						// Free all partition entries.
						for (auto part = drive->firstPartition; part; )
						{
							auto next = part->next;
							delete[] part;

							part = next;
						}
						for (size_t i = 0; i < nPartitions; i++)
						{
							vfs::PartitionEntry* partEntry = new vfs::PartitionEntry{};
							partEntry->drive = drive;
							partEntry->lbaOffset = partitions[i].lbaOffset;
							partEntry->sizeSectors = partitions[i].sizeSectors;
							partEntry->partitionId = partitions[i].id;
							if(!drive->firstPartition)
								drive->firstPartition = partEntry;
							if (drive->lastPartition)
								drive->lastPartition->next = partEntry;
							partEntry->prev = drive->lastPartition;
							drive->lastPartition = partEntry;
							drive->nPartitions++;
						}
						delete[] partitions;
					}
				};
			iterateDrivesAndRegisterPartitions(mbrDrives, mbrDriver);
			iterateDrivesAndRegisterPartitions(gptDrives, gptDriver);
			return;
		}
		void ScanAndLoadModules(const char* root)
		{
			utils::Vector<LoadableDriver> drivers;
			// Look for loadable drivers in "root"
			vfs::DirectoryIterator iter;
			iter.OpenAt(root);
			for (const char* path = *iter; iter; path = ++iter)
			{
				vfs::FileHandle file;
				file.Open(path);
				if (file.GetFileSize() < sizeof(process::loader::Elf64_Ehdr))
				{
					file.Close();
					delete[] path;
					continue;
				}
				char header[4];
				file.Read(header, 4, true);
				if (utils::memcmp(header, "\177ELF", 4))
				{
					// Read the entire file and verify it is a module.
					char* _file = new char[file.GetFileSize()];
					file.Read(_file, file.GetFileSize());
					driverHeader* fheader = CheckModule((byte*)_file, file.GetFileSize());
					if (!fheader)
					{
						delete[] _file;
						delete[] path;
						file.Close();
						continue;
					}
					// If it is, add it to the list.
					driverHeader* header = new driverHeader;
					utils::memcpy(header, fheader, sizeof(driverHeader));
					drivers.push_back(LoadableDriver{ path, header });
					delete[] _file;
				}
				delete[] path;
				file.Close();
			}
			ScanPCIBus(drivers);
			// Look for partitions, and load the partition drivers.
			ScanDrivesForPartitions(drivers);
			for (auto& driver : drivers)
				delete driver.header;
		}
	}
}