/*
	multitasking/process/x86_64/loader/elf.cpp

	Copyright (c) 2023-2024 Omar Berrow
*/

#include <multitasking/process/x86_64/loader/elf.h>
#include <multitasking/process/x86_64/loader/elfStructures.h>

#include <int.h>
#include <error.h>
#include <error.h>
#include <memory_manipulation.h>

#include <allocators/vmm/vmm.h>

namespace obos
{
	namespace process
	{
		namespace loader
		{
			static uint32_t load(const byte* startAddress, size_t, uintptr_t& baseAddress, memory::VirtualAllocator& allocator, bool lazyLoad)
			{
				baseAddress = 0;
				Elf64_Ehdr* elfHeader = (Elf64_Ehdr*)startAddress;
				Elf64_Phdr* programHeader = (Elf64_Phdr*)(startAddress + elfHeader->e_phoff);
				for (int i = 0; i < elfHeader->e_phnum; i++, programHeader++)
				{
					if (programHeader->p_type != PT_LOAD)
						continue;
					uintptr_t allocFlags = (uintptr_t)allocator.IsUsermodeAllocator() * memory::PROT_USER_MODE_ACCESS;
					if (programHeader->p_flags & PF_X)
						allocFlags |= memory::PROT_CAN_EXECUTE;
					if (!(programHeader->p_flags & PF_W))
						allocFlags |= memory::PROT_READ_ONLY;
					if (programHeader->p_vaddr > 0xFFFF800000000000 || !programHeader->p_vaddr)
					{
						SetLastError(OBOS_ERROR_BASE_ADDRESS_USED);
						return OBOS_ERROR_BASE_ADDRESS_USED;
					}
					if (!lazyLoad)
					{
						uint32_t nPages = programHeader->p_memsz >> 12;
						if ((programHeader->p_memsz % 4096) != 0)
							nPages++;
						byte* addr = (byte*)allocator.VirtualAlloc(
							(void*)programHeader->p_vaddr, 
							(size_t)nPages * 4096,
							/* allocator.Memcpy requires user mode access to be enabled for the pages if allocator.IsUsermodeAllocator() is true, otherwise it will fail. */
							(uintptr_t)allocator.IsUsermodeAllocator() * memory::PROT_USER_MODE_ACCESS);
						if (programHeader->p_filesz)
						{
							uintptr_t offset = programHeader->p_vaddr - (uintptr_t)addr;
							addr += offset;
							allocator.Memcpy(addr, startAddress + programHeader->p_offset, programHeader->p_filesz);
						}
						allocator.VirtualProtect(addr, static_cast<size_t>(nPages) * 4096, allocFlags);
					}
					if (baseAddress > programHeader->p_vaddr || elfHeader->e_phnum == 1)
						baseAddress = programHeader->p_vaddr;
				}
				return OBOS_SUCCESS;
			}

			uint32_t LoadElfFile(const byte* startAddress, size_t size, uintptr_t& entry, uintptr_t& baseAddress, memory::VirtualAllocator& allocator, bool lazyLoad)
			{
				uint32_t err = CheckElfFile(startAddress, size, true);
				if (err != 0)
					return err;
				Elf64_Ehdr* elfHeader = (Elf64_Ehdr*)startAddress;
				entry = elfHeader->e_entry;
				return load(startAddress, size, baseAddress, allocator, lazyLoad);
			}
			uint32_t CheckElfFile(const byte* startAddress, size_t size, bool setLastError)
			{
				if (size <= sizeof(Elf64_Ehdr))
				{
					if (setLastError)
						SetLastError(OBOS_ERROR_LOADER_INCORRECT_FILE);
					return OBOS_ERROR_LOADER_INCORRECT_FILE;
				}
				Elf64_Ehdr* elfHeader = (Elf64_Ehdr*)startAddress;
				if (elfHeader->e_ident[EI_MAG0] != ELFMAG0 ||
					elfHeader->e_ident[EI_MAG1] != ELFMAG1 ||
					elfHeader->e_ident[EI_MAG2] != ELFMAG2 ||
					elfHeader->e_ident[EI_MAG3] != ELFMAG3)
				{
					if (setLastError)
						SetLastError(OBOS_ERROR_LOADER_INCORRECT_FILE);
					return OBOS_ERROR_LOADER_INCORRECT_FILE;
				}
				if (elfHeader->e_version != EV_CURRENT)
				{
					if (setLastError)
						SetLastError(OBOS_ERROR_LOADER_INCORRECT_FILE);
					return OBOS_ERROR_LOADER_INCORRECT_FILE;
				}
				if (elfHeader->e_ident[EI_CLASS] != ELFCLASS64 || elfHeader->e_ident[EI_DATA] != ELFDATA2LSB)
				{
					if (setLastError)
						SetLastError(OBOS_ERROR_LOADER_INCORRECT_ARCHITECTURE);
					return OBOS_ERROR_LOADER_INCORRECT_ARCHITECTURE;
				}
				return 0;
			}
		}
	}
}
