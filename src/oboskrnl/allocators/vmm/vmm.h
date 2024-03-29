/*
	oboskrnl/allocators/vmm/vmm.h

	Copyright (c) 2023-2024 Omar Berrow
*/

#pragma once

#include <int.h>
#include <export.h>

#include <vfs/off_t.h>

#define ALLOCATORS_VMM_VMM_H_INCLUDED 1

namespace obos
{
#ifndef MULTIASKING_PROCESS_PROCESS_H_INCLUDED
	namespace process
	{
		struct Process;
	}
#endif
#ifndef VFS_FILEMANIP_FILEHANDLE_H_INCLUDED
	namespace vfs
	{
		class FileHandle;
	}
#endif
	namespace memory
	{
		enum PageProtectionFlags
		{
			/// <summary>
			/// Whether the pages allocated can be read.
			/// </summary>
			PROT_READ_ONLY = 0b1,
			/// <summary>
			/// Whether user mode threads can access the pages allocated. This is the default when using any syscalls.
			/// </summary>
			PROT_USER_MODE_ACCESS = 0b10,
			/// <summary>
			/// Whether the pages can be executed or not. If the processor doesn't support this, all pages are executable
			/// </summary>
			PROT_CAN_EXECUTE = 0b100,
			/// <summary>
			/// Whether to allocate the pages immediately, or to COW (Copy on write) the allocated pages.
			/// </summary>
			PROT_NO_COW_ON_ALLOCATE = 0b1000,
			/// <summary>
			/// Whether to disable cache on the allocated pages.
			/// </summary>
			PROT_DISABLE_CACHE = 0b10000,
			/// <summary>
			/// Whether the pages are present. This is only used in VirtualGetProtection.
			/// </summary>
			PROT_IS_PRESENT = 0b100000,
			PROT_ALL_BITS_SET = 0b111111,
		};

		class VirtualAllocator final
		{
		public:
			VirtualAllocator() = delete;
			// if owner is nullptr, the class should allocate as the current process.
			OBOS_EXPORT VirtualAllocator(process::Process* owner);

			OBOS_EXPORT void Initialize(process::Process* owner);

			/// <summary>
			/// Allocates nPages at base.
			/// </summary>
			/// <param name="base">The base address to allocate at.</param>
			/// <param name="size">The amount of bytes (rounded to the nearest page size) to allocate at base.</param>
			/// <param name="flags">The initial protection flags.</param>
			/// <returns>"base" if base isn't nullptr. If base is nullptr, the function finds a base address. If this function fails, it returns nullptr.</returns>
			OBOS_EXPORT void* VirtualAlloc(void* base, size_t size, uintptr_t flags);
			/// <summary>
			/// Free nPages pages at base.
			/// </summary>
			/// <param name="base">The base address to free.</param>
			/// <param name="size">The amount of bytes (rounded to the nearest page size) to free.</param>
			/// <returns>false on failure, otherwise true. If this function fails, use GetLastError for extra error information.</returns>
			OBOS_EXPORT bool VirtualFree(void* base, size_t size);
			/// <summary>
			/// Sets the protection for the pages at base.
			/// </summary>
			/// <param name="base">The base address to set the protection for.</param>
			/// <param name="size">The amount of bytes (rounded to the nearest page size) to set the protection for.</param>
			/// <param name="flags">The new protection flags</param>
			/// <returns>false on failure, otherwise true. If this function fails, use GetLastError for extra error information.</returns>
			OBOS_EXPORT bool VirtualProtect(void* base, size_t size, uintptr_t flags);
			/// <summary>
			/// Gets the protection for base.
			/// </summary>
			/// <param name="base">The base address to get the protection for.</param>
			/// <param name="size">The amount of bytes (rounded to the nearest page size) to get the protection for.</param>
			/// <param name="flags">A pointer to a buffer of the size "sizeof(PageProtectionFlags) * sizeToPageCount(size)" to store the protection in.</param>
			/// <returns>false on failure, otherwise true. If this function fails, use GetLastError for extra error information.</returns>
			OBOS_EXPORT bool VirtualGetProtection(void* base, size_t size, uintptr_t* flags);
			/// <summary>
			/// Maps a file into memory.
			/// </summary>
			/// <param name="base">The base address to allocate at.</param>
			/// <param name="size">The amount of bytes (rounded to the nearest page size) to allocate at base. This must be within the file limits.</param>
			/// <param name="offset">The offset into the file.</param>
			/// <param name="file">A file handle representing the file to allocate.</param>
			/// <param name="flags">The initial protection flags.</param>
			/// <returns>"base" if base isn't nullptr. If base is nullptr, the function finds a base address. If this function fails, it returns nullptr.</returns>
			OBOS_EXPORT void* VirtualMapFile(void* base, size_t size, vfs::uoff_t offset, vfs::FileHandle* file, uintptr_t flags);

			/// <summary>
			/// Copies a buffer from this program to the other process.
			/// </summary>
			/// <param name="remoteDest">The remote destination's address.</param>
			/// <param name="localSrc">The local source's address.</param>
			/// <param name="size">The size of the buffer.</param>
			/// <returns>remoteDest on success, or nullptr.</returns>
			OBOS_EXPORT void* Memcpy(void* remoteDest, const void* localSrc, size_t size);

			OBOS_EXPORT bool IsUsermodeAllocator();

			OBOS_EXPORT bool FreeUserProcessAddressSpace();

			OBOS_EXPORT ~VirtualAllocator() { m_owner = nullptr; }

			static OBOS_EXPORT size_t GetPageSize();
		private:
			process::Process* m_owner;
			static size_t m_pageSize;
		};
	}
}