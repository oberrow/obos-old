/*
	oboskrnl/boot/kmain.cpp

	Copyright (c) 2023 Omar Berrow
*/

// __ALL__ code in this file must be platform-independent.

#include <limine.h>

#include <int.h>
#include <klog.h>
#include <memory_manipulation.h>

#include <new>

#include <multitasking/scheduler.h>
#include <multitasking/thread.h>
#include <multitasking/arch.h>

#include <multitasking/threadAPI/thrHandle.h>

#include <multitasking/process/process.h>
#include <multitasking/process/x86_64/loader/elf.h>

#include <driverInterface/load.h>
#include <driverInterface/call.h>
#include <driverInterface/interface.h>
#include <driverInterface/struct.h>

#include <vfs/mount/mount.h>
#include <vfs/vfsNode.h>

namespace obos
{
	extern volatile limine_module_request module_request;
	void _thread(uintptr_t)
	{
		logger::log("Hello from tid %d.\n", thread::g_currentThread->tid);
		thread::ExitThread(0);
	}
	void kmain_common()
	{
		thread::g_initialized = true;
		logger::log("Multitasking initialized! In \"%s\" now.\n", __func__);
		
		process::Process* kernelProc = new process::Process;
		kernelProc->pid = 0;
		kernelProc->console = &g_kernelConsole;
		kernelProc->isUsermode = false;
		kernelProc->parent = nullptr;
#ifdef __x86_64__
		kernelProc->context.cr3 = thread::g_currentThread->context.cr3; // Only this time... (reference to line 7)
#endif
		process::g_processes.tail = process::g_processes.head = kernelProc;
		process::g_processes.size++;
		thread::g_currentThread->owner = kernelProc;

		byte* procExecutable = nullptr;
		size_t moduleIndex = 0;

		for (; moduleIndex < module_request.response->module_count; moduleIndex++)
		{
			if (utils::strcmp(module_request.response->modules[moduleIndex]->path, "/obos/initrdDriver"))
			{
				procExecutable = (byte*)module_request.response->modules[moduleIndex]->address;
				break;
			}
		}

		if (!procExecutable)
			logger::panic("Couldn't find the initrd driver.\n");
		
		driverInterface::RegisterSyscall(0, (uintptr_t)driverInterface::SyscallVPrintf);
		driverInterface::RegisterSyscall(1, (uintptr_t)(void(*)(const uint32_t*))[](const uint32_t* exitCode) { 
			thread::ExitThread(*exitCode); 
			});
		driverInterface::RegisterSyscall(2, (uintptr_t)driverInterface::SyscallAllocDriverServer);
		driverInterface::RegisterSyscall(3, (uintptr_t)driverInterface::SyscallFreeDriverServer);
		driverInterface::RegisterSyscall(4, (uintptr_t)driverInterface::SyscallMalloc);
		driverInterface::RegisterSyscall(5, (uintptr_t)driverInterface::SyscallFree);
		driverInterface::RegisterSyscall(6, (uintptr_t)driverInterface::SyscallMapPhysToVirt);
		driverInterface::RegisterSyscall(7, (uintptr_t)driverInterface::SyscallGetInitrdLocation);
		
		driverInterface::LoadModule(procExecutable, module_request.response->modules[moduleIndex]->size, nullptr);

		new (&vfs::g_mountPoints) Vector<vfs::MountPoint*>{};

		uint32_t point = (uint32_t)-1;
		vfs::mount(point, 0); // Mount the initrd.

		thread::ExitThread(0);
	}

}