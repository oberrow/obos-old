/*
	oboskrnl/arch/x86_64/memory_manager/allocate.cpp

	Copyright (c) 2023 Omar Berrow
*/

#include <limine.h>

#include <int.h>
#include <klog.h>
#include <atomic.h>
#include <memory_manipulation.h>

#include <x86_64-utils/asm.h>

#include <arch/x86_64/memory_manager/physical/allocate.h>

#include <arch/x86_64/memory_manager/virtual/initialize.h>

#include <multitasking/locks/mutex.h>

namespace obos
{
	static volatile limine_memmap_request mmap_request = {
		.id = LIMINE_MEMMAP_REQUEST,
		.revision = 0
	};
	namespace memory
	{
		struct MemoryNode
		{
			MemoryNode *next, *prev;
		};
		MemoryNode *g_memoryHead, *g_memoryTail;
		locks::Mutex g_pmmLock;

#pragma GCC push_options
#pragma GCC optimize("O1")
		void InitializePhysicalMemoryManager()
		{
			for (size_t i = 0; i < mmap_request.response->entry_count; i++)
			{
				if (mmap_request.response->entries[i]->type != LIMINE_MEMMAP_USABLE)
					continue;
				uintptr_t base = mmap_request.response->entries[i]->base;
				if (base < 0x1000)
					base = 0x1000;
				if (base & 0xfff)
					base = (base + 0xfff) & 0xfff;
				size_t size = mmap_request.response->entries[i]->length & ~0xfff;
				for (uintptr_t addr = base; addr < (base + size); addr += 0x1000)
				{
					MemoryNode* newNode = (MemoryNode*)addr;
					if (g_memoryTail)
						g_memoryTail->next = newNode;
					if (!g_memoryHead)
						g_memoryHead = newNode;
					newNode->prev = g_memoryTail;
					newNode->next = nullptr;
					g_memoryTail = newNode;
				}
			}
		}
#pragma GCC pop_options

		uintptr_t allocatePhysicalPage()
		{
			if (!g_memoryTail)
				logger::panic("No more available physical memory left.\n");
			uintptr_t ret = (uintptr_t)g_memoryTail;
			MemoryNode* node = (MemoryNode*)memory::mapPageTable((uintptr_t*)ret);
			MemoryNode* next = node->next;
			MemoryNode* prev = node->prev;
			if (prev)
				((MemoryNode*)memory::mapPageTable((uintptr_t*)prev))->next = next;
			g_memoryTail = prev;
			memory::mapPageTable((uintptr_t*)ret);
			node->next = nullptr;
			node->prev = nullptr;
			return ret;
		}
		bool freePhysicalPage(uintptr_t addr)
		{
			MemoryNode* node = (MemoryNode*)memory::mapPageTable((uintptr_t*)addr);
			node->prev = g_memoryTail;
			node->next = nullptr;
			MemoryNode* lastNode = (MemoryNode*)memory::mapPageTable((uintptr_t*)g_memoryTail);
			g_memoryTail = (MemoryNode*)addr;
			lastNode->next = g_memoryTail;
			return true;
		}
	}
}