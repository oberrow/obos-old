/*
	arch/x86_64/syscall/verify_pars.h

	Copyright (c) 2023-2024 Omar Berrow
*/

#include <int.h>

#include <arch/x86_64/syscall/verify_pars.h>

#include <multitasking/scheduler.h>
#include <multitasking/arch.h>
#include <multitasking/cpu_local.h>

#include <multitasking/process/process.h>

#define getCPULocal() ((thread::cpu_local*)thread::getCurrentCpuLocalPtr())

namespace obos
{
	namespace syscalls
	{
		bool canAccessUserMemory(const void* addr, size_t size, bool hasToWrite)
		{
			if (!addr)
				return false;
			memory::VirtualAllocator vallocator = ((process::Process*)getCPULocal()->currentThread->owner);
			bool checkUsermode = ((process::Process*)getCPULocal()->currentThread->owner)->isUsermode;
			if (checkUsermode && (uintptr_t)addr > 0xffff'8000'0000'0000)
				return false;
			size_t nPagesToCheck = ((size + 0xfff) & ~0xfff) / 4096;
			uintptr_t* pageFlags = (uintptr_t*)kmalloc(nPagesToCheck * sizeof(uintptr_t));
			uintptr_t requiredFlags = memory::PROT_IS_PRESENT | ((uintptr_t)checkUsermode * memory::PROT_USER_MODE_ACCESS);
			if(!vallocator.VirtualGetProtection((void*)((uintptr_t)addr & ~0xfff), nPagesToCheck * 4096, pageFlags))
			{
				kfree(pageFlags);
				return false;
			}
			for (size_t i = 0; i < nPagesToCheck; i++)
			{
				if (((pageFlags[i] & requiredFlags) != requiredFlags) || ((pageFlags[0] & memory::PROT_READ_ONLY) && hasToWrite))
				{
					kfree(pageFlags);
					return false;
				}
			}
			kfree(pageFlags);
			return true;
		}
	}
}