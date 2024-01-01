/*
	oboskrnl/arch/x86_64/exception_handlers.cpp

	Copyright (c) 2023-2024 Omar Berrow
*/

#include <int.h>
#include <klog.h>
#include <memory_manipulation.h>

#include <arch/x86_64/interrupt.h>

#include <x86_64-utils/asm.h>

#include <multitasking/cpu_local.h>

#include <multitasking/locks/mutex.h>

#include <multitasking/process/process.h>

#include <arch/x86_64/memory_manager/virtual/initialize.h>

#include <arch/x86_64/memory_manager/physical/allocate.h>

#include <arch/x86_64/irq/irq.h>

#include <multitasking/process/signals.h>

extern "C" char _sched_text_start;
extern "C" char _sched_text_end;

namespace obos
{
	namespace memory
	{
		uintptr_t DecodeProtectionFlags(uintptr_t _flags);
		uintptr_t* allocatePagingStructures(uintptr_t address, PageMap* pageMap);
		void* MapEntry(PageMap* pageMap, uintptr_t entry, void* to);
		void UnmapAddress(PageMap* pageMap, void* _addr);
	}
	namespace thread
	{
		extern locks::Mutex g_coreGlobalSchedulerLock;
	}
	bool g_halt;
	void exception14(interrupt_frame* frame)
	{
		uintptr_t entry = 0;
		memory::PageMap* pageMap = memory::getCurrentPageMap();
		uintptr_t faultAddress = (uintptr_t)getCR2() & ~0xfff;
		if (frame->errorCode & 1)
		{
			entry = (uintptr_t)pageMap->getL1PageMapEntryAt(faultAddress);
			if (entry & ((uintptr_t)1 << 9))
			{
				uintptr_t flags = memory::DecodeProtectionFlags(entry >> 52) | 1;
				uintptr_t newEntry = memory::allocatePhysicalPage();
				utils::memcpy(memory::mapPageTable((uintptr_t*)newEntry), (void*)faultAddress, 4096);
				newEntry |= flags;
				memory::UnmapAddress(pageMap, (void*)faultAddress);
				memory::MapEntry(pageMap, newEntry, (void*)faultAddress);
				return;
			}
		}
		const char* action = (frame->errorCode & ((uintptr_t)1 << 1)) ? "write" : "read";
		if (frame->errorCode & ((uintptr_t)1 << 4))
			action = "execute";
		uint32_t cpuId = 0, pid = -1, tid = -1;
		bool whileInScheduler = (frame->rip >= (uintptr_t)&_sched_text_start && frame->rip < (uintptr_t)&_sched_text_end);
		thread::Thread* currentThread = nullptr;
		if (thread::getCurrentCpuLocalPtr())
		{
			cpuId = thread::GetCurrentCpuLocalPtr()->cpuId;
			whileInScheduler = whileInScheduler || thread::GetCurrentCpuLocalPtr()->schedulerLock;
			currentThread = (thread::Thread*)thread::GetCurrentCpuLocalPtr()->currentThread;
			if (!whileInScheduler)
			{
				if (currentThread)
				{
					tid = currentThread->tid;
					if (currentThread->owner)
					{
						process::Process* proc = (process::Process*)currentThread->owner;
						pid = proc->pid;
					}
				}
			}
		}
		if (whileInScheduler)
			tid = pid = (uint32_t)-1;
		// Bug mitigation.
		// Sometimes we page fault while accessing the lapic, even though that's impossible.
		if ((faultAddress >= (uintptr_t)g_localAPICAddr && faultAddress <= 0xfffffffffffff000) && !(frame->errorCode >> 4) && !((frame->errorCode << 2)))
			return;
		// If in user mode...
		if ((frame->errorCode >> 2) & 1)
		{
			// Call SIGPF, or terminate.
			currentThread->context.frame = *frame;
			process::CallSignalOrTerminate(currentThread, process::SIGPF);
			return;
		}
		logger::panic(
			nullptr,
			"Page fault in %s-mode at %p (cpu %d, pid %d, tid %d) while trying to %s a %s page. The address of this page is %p. Error code: %d. whileInScheduler = %s\nPTE: %p, PDE: %p, PDPE: %p, PME: %p.\nDumping registers:\n"
			"\tRDI: %p, RSI: %p, RBP: %p\n"
			"\tRSP: %p, RBX: %p, RDX: %p\n"
			"\tRCX: %p, RAX: %p, RIP: %p\n"
			"\t R8: %p,  R9: %p, R10: %p\n"
			"\tR11: %p, R12: %p, R13: %p\n"
			"\tR14: %p, R15: %p, RFL: %p\n"
			"\t SS: %p,  DS: %p,  CS: %p\n"
			"\tCR0: %p, CR2: %p, CR3: %p\n"
			"\tCR4: %p, CR8: %p, EFER: %p\n",
			(frame->errorCode & ((uintptr_t)1 << 2)) ? "user" : "kernel",
			frame->rip,
			cpuId,
			pid, tid,
			action,
			(frame->errorCode & ((uintptr_t)1 << 0)) ? "present" : "non-present",
			getCR2(),
			frame->errorCode,
			whileInScheduler ? "true" : "false",
			entry,
			pageMap->getL2PageMapEntryAt(faultAddress),
			pageMap->getL3PageMapEntryAt(faultAddress),
			pageMap->getL4PageMapEntryAt(faultAddress),
			frame->rdi, frame->rsi, frame->rbp,
			frame->rsp, frame->rbx, frame->rdx,
			frame->rcx, frame->rax, frame->rip,
			frame->r8, frame->r9, frame->r10,
			frame->r11, frame->r12, frame->r13,
			frame->r14, frame->r15, frame->rflags,
			frame->ss, frame->ds, frame->cs,
			getCR0(), getCR2(), memory::getCurrentPageMap(),
			getCR4(), getCR8(), getEFER()
			);
	}
	void defaultExceptionHandler(interrupt_frame* frame)
	{
		uint32_t cpuId = 0, pid = -1, tid = -1;
		bool whileInScheduler = (frame->rip >= (uintptr_t)&_sched_text_start && frame->rip < (uintptr_t)&_sched_text_end);
		thread::Thread* currentThread = nullptr;
		if (thread::getCurrentCpuLocalPtr())
		{
			cpuId = thread::GetCurrentCpuLocalPtr()->cpuId;
			whileInScheduler = whileInScheduler || thread::GetCurrentCpuLocalPtr()->schedulerLock;
			currentThread = (thread::Thread*)thread::GetCurrentCpuLocalPtr()->currentThread;
			if (!whileInScheduler)
			{
				if (currentThread)
				{
					tid = currentThread->tid;
					if (currentThread->owner)
					{
						process::Process* proc = (process::Process*)currentThread->owner;
						pid = proc->pid;
					}
				}
			}
		}
		if (whileInScheduler)
			tid = pid = (uint32_t)-1;
		// If in user mode...
		if (frame->cs != 0x08 && frame->ds != 0x10)
		{
			constexpr const process::signals intToSignal[] = {
				process::signals::SIGME         ,process::signals::SIGDG         ,process::signals::INVALID_SIGNAL,process::signals::         SIGDG,
				process::signals::SIGOF         ,process::signals::SIGUEXC       ,process::signals::SIGUDOC       ,process::signals::INVALID_SIGNAL,
				process::signals::INVALID_SIGNAL,process::signals::INVALID_SIGNAL,process::signals::INVALID_SIGNAL,process::signals::INVALID_SIGNAL,
				process::signals::INVALID_SIGNAL,process::signals::SIGPM         ,process::signals::SIGPF         ,process::signals::INVALID_SIGNAL,
				process::signals::SIGME         ,process::signals::SIGUEXC       ,process::signals::INVALID_SIGNAL,process::signals::SIGME         ,
				process::signals::INVALID_SIGNAL,process::signals::INVALID_SIGNAL,process::signals::INVALID_SIGNAL,process::signals::INVALID_SIGNAL,
				process::signals::INVALID_SIGNAL,process::signals::INVALID_SIGNAL,process::signals::INVALID_SIGNAL,process::signals::INVALID_SIGNAL,
				process::signals::INVALID_SIGNAL,process::signals::INVALID_SIGNAL,process::signals::INVALID_SIGNAL,process::signals::INVALID_SIGNAL,
			};
			process::signals sig = intToSignal[frame->intNumber];
			if (sig != process::signals::INVALID_SIGNAL)
			{
				currentThread->context.frame = *frame;
				process::CallSignalOrTerminate(currentThread, sig);
				return;
			}
		}
		logger::panic(
			nullptr,
			"Exception %d at %p (cpu %d, pid %d, tid %d). Error code: %d.\nDumping registers:\n"
			"\tRDI: %p, RSI: %p, RBP: %p\n"
			"\tRSP: %p, RBX: %p, RDX: %p\n"
			"\tRCX: %p, RAX: %p, RIP: %p\n"
			"\t R8: %p,  R9: %p, R10: %p\n"
			"\tR11: %p, R12: %p, R13: %p\n"
			"\tR14: %p, R15: %p, RFL: %p\n"
			"\t SS: %p,  DS: %p,  CS: %p\n",
			frame->intNumber,
			frame->rip,
			cpuId,
			pid, tid,
			frame->errorCode,
			frame->rdi, frame->rsi, frame->rbp,
			frame->rsp, frame->rbx, frame->rdx,
			frame->rcx, frame->rax, frame->rip,
			frame->r8, frame->r9, frame->r10,
			frame->r11, frame->r12, frame->r13,
			frame->r14, frame->r15, frame->rflags,
			frame->ss, frame->ds, frame->cs
		);
	}
	[[noreturn]] void nmiHandler(interrupt_frame*)
	{
		if (g_halt)
			haltCPU();
		logger::panic(nullptr, "NMI thrown by the hardware. System control port: %d.\n", inb(0x92) | (inb(0x61) << 8));
	}
	void RegisterExceptionHandlers()
	{
		RegisterInterruptHandler(0, defaultExceptionHandler);
		RegisterInterruptHandler(1, defaultExceptionHandler);
		RegisterInterruptHandler(2, nmiHandler);
		for (int i = 3; i < 14; i++)
			RegisterInterruptHandler(i, defaultExceptionHandler);
		RegisterInterruptHandler(14, exception14);
		for (byte i = 15; i < 32; i++)
			RegisterInterruptHandler(i, defaultExceptionHandler);
	}
}