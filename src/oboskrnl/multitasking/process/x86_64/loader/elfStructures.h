/*
	multitasking/process/x86_64/loader/elfStructures.h

	Copyright (c) 2023 Omar Berrow
*/

#pragma once

#include <int.h>


namespace obos
{
	namespace process
	{
		namespace loader
		{
			constexpr byte ELFMAG0 = 0x7f;
			constexpr byte ELFMAG1 = 'E';
			constexpr byte ELFMAG2 = 'L';
			constexpr byte ELFMAG3 = 'F';

			constexpr byte ELFCLASSNONE = 0;
			constexpr byte ELFCLASS32 = 1;
			constexpr byte ELFCLASS64 = 2;

			constexpr byte ELFDATANONE = 0;
			constexpr byte ELFDATA2LSB = 1;
			constexpr byte ELFDATA2MSB = 2;

			constexpr byte EV_NONE = 0;
			constexpr byte EV_CURRENT = 1;

			constexpr uint32_t EI_NIDENT = 16;
			constexpr uint32_t EI_MAG0 = 0;
			constexpr uint32_t EI_MAG1 = 1;
			constexpr uint32_t EI_MAG2 = 2;
			constexpr uint32_t EI_MAG3 = 3;
			constexpr uint32_t EI_CLASS = 4;
			constexpr uint32_t EI_DATA = 5;
			// Must be EV_CURRENT.
			constexpr uint32_t EI_VERSION = 6;
			constexpr uint32_t EI_PAD = 7;

			constexpr uint32_t PF_R = 0x4;
			constexpr uint32_t PF_W = 0x2;
			constexpr uint32_t PF_X = 0x1;

			typedef uintptr_t Elf64_Addr;
			typedef uintptr_t Elf64_Off;
			typedef uint32_t Elf64_Word;
			typedef uint64_t Elf64_Qword;
			typedef uint16_t Elf64_Half;

			enum
			{
				PT_NULL,
				PT_LOAD,
				PT_DYNAMIC,
				PT_INTERP,
				PT_NOTE,
				PT_SHLIB,
				PT_PHDR
			};

			struct Elf64_Ehdr
			{
				unsigned char e_ident[EI_NIDENT];
				Elf64_Half e_type;
				Elf64_Half e_machine;
				Elf64_Word e_version;
				Elf64_Addr e_entry;
				Elf64_Off  e_phoff;
				Elf64_Off  e_shoff;
				Elf64_Word e_flags;
				Elf64_Half e_ehsize;
				Elf64_Half e_phentsize;
				Elf64_Half e_phnum;
				Elf64_Half e_shentsize;
				Elf64_Half e_shnum;
				Elf64_Half e_shstrndx;
			};

			struct Elf64_Phdr
			{
				Elf64_Word p_type;
				Elf64_Word p_flags;
				Elf64_Off  p_offset;
				Elf64_Addr p_vaddr;
				Elf64_Addr p_paddr;
				Elf64_Qword p_filesz;
				Elf64_Qword p_memsz;
				Elf64_Qword p_align;
			};

			struct Elf64_Shdr
			{
				Elf64_Word 	sh_name;
				Elf64_Word 	sh_type;
				Elf64_Qword sh_flags;
				Elf64_Addr 	sh_addr;
				Elf64_Off 	sh_offset;
				Elf64_Qword sh_size;
				Elf64_Word 	sh_link;
				Elf64_Word 	sh_info;
				Elf64_Qword sh_addralign;
				Elf64_Qword sh_entsize;
			};
		}
	}
}