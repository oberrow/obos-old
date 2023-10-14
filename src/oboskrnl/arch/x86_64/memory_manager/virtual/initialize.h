/*
	arch/x86_64/memory_manager/virtual/initialize.h

	Copyright (c) 2023 Omar Berrow
*/

#pragma once

#include <int.h>

namespace obos
{
	namespace memory
	{
		class PageMap
		{
		public:
			PageMap() = delete;

			uintptr_t* getPageMap() { return (uintptr_t*)this; }
			uintptr_t* getL4PageMapEntryAt(uintptr_t at); // pageMap[addressToIndex(at, 3)];
			uintptr_t* getL3PageMapEntryAt(uintptr_t at); // getL4PageMapEntryAt()[addressToIndex(at,2)];
			uintptr_t* getL2PageMapEntryAt(uintptr_t at); // getL3PageMapEntryAt()[addressToIndex(at,1)];
			uintptr_t* getL1PageMapEntryAt(uintptr_t at); // getL2PageMapEntryAt()[addressToIndex(at,0)];

			void switchToThis();

			size_t addressToIndex(uintptr_t address, uint8_t level) { return (address >> (9 * level + 12)) & 0x1FF;  }
		};
		void InitializeVirtualMemoryManager();
	}
}