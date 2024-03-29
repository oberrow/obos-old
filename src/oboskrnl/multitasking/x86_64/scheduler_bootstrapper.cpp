/*
	oboskrnl/multitasking/x86_64/scheduler_bootstrapper.cpp

	Copyright (c) 2023-2024 Omar Berrow
*/

#include <int.h>
#include <klog.h>
#include <memory_manipulation.h>
#include <utils/hashmap.h>

#include <arch/interrupt.h>

#include <arch/x86_64/irq/timer.h>
#include <arch/x86_64/irq/irq.h>

#include <multitasking/scheduler.h>
#include <multitasking/arch.h>
#include <multitasking/cpu_local.h>

#include <multitasking/locks/mutex.h>

#include <x86_64-utils/asm.h>

#define GS_BASE 0xC0000101

#define getCPULocal() GetCurrentCpuLocalPtr()

extern "C" uint64_t calibrateTimer(uint64_t femtoseconds);
extern "C" void _fxsave(byte(*context)[512]);
extern "C" void _callScheduler();

extern "C" char _sched_text_start;
extern "C" char _sched_text_end;

namespace obos
{
	namespace thread
	{
		extern void schedule();

		// Do not make static, as this function is used in drivers/generic/acpi/impl.cpp:uacpi_kernel_stall() and calibrateTimer()
		uint64_t configureHPET(uint64_t freq)
		{
			g_HPETAddr->generalConfig &= ~(1<<0);
			uint64_t comparatorValue = g_HPETAddr->mainCounterValue + (g_hpetFrequency / freq);
			g_HPETAddr->timer0.timerComparatorValue = comparatorValue;
			if (g_HPETAddr->timer0.timerConfigAndCapabilities & (1<<3))
				g_HPETAddr->timer0.timerConfigAndCapabilities &= ~(1<<2);
			g_HPETAddr->generalConfig |= (1<<0);
			return comparatorValue;
		}

		static utils::Hashmap<uint64_t, uint64_t> s_freqToCounterValue;
		static locks::Mutex s_hpetLock;
		// Assumes the timer divisor is one.
		static uint64_t FindCounterValueFromFrequency(uint64_t freq)
		{
			s_hpetLock.Lock();
			if (s_freqToCounterValue.contains(freq))
			{
				s_hpetLock.Unlock();
				return s_freqToCounterValue.at(freq);
			}
			logger::debug("%s, cpu %d: Calibrating the timer at a frequency of %d.\n", __func__, getCPULocal()->cpuId, freq);
			uint64_t ret = calibrateTimer(freq);
			logger::debug("%s, cpu %d: Timer count is %d.\n", __func__, getCPULocal()->cpuId, ret);
			s_freqToCounterValue.emplace_at(freq, ret);
			s_hpetLock.Unlock();
			return ret;
		}

		void scheduler_bootstrap(interrupt_frame* frame)
		{
			if (!g_initialized)
				return;
			volatile Thread* currentThread = getCPULocal()->currentThread;
			if (!getCPULocal()->schedulerLock && currentThread)
			{
				utils::memcpy((void*)&currentThread->context.frame, frame, sizeof(interrupt_frame)); // save the interrupt frame
				_fxsave((byte(*)[512])currentThread->context.fpuState);
				currentThread->context.fsbase = rdmsr(0xC0000100 /*FS.Base*/);
				// Use KernelGSBase so that we get the gs base saved by the user, and not the kernel's gs base.
				currentThread->context.gsbase = rdmsr(0xC0000102 /*KernelGSBase*/);
			}
			_callScheduler();
		}


		void setupTimerInterrupt()
		{
			uint64_t counter = FindCounterValueFromFrequency(g_schedulerFrequency);
			ConfigureAPICTimer(scheduler_bootstrap, 0x20, counter, TIMER_CONFIG_PERIODIC, TIMER_DIVISOR_ONE);
			RegisterInterruptHandler(0x30, scheduler_bootstrap);
		}
		uintptr_t stopTimer()
		{
			uintptr_t ret = saveFlagsAndCLI();
			if(g_localAPICAddr)
				MaskTimer(false);
			return ret;
		}
		void startTimer(uintptr_t flags)
		{
			if (g_localAPICAddr)
				MaskTimer(true);
			restorePreviousInterruptStatus(flags);
		}
		void callScheduler(bool allCores)
		{
			if (allCores)
				SendIPI(DestinationShorthand::All, DeliveryMode::Default, 0x30);
			else
				asm volatile("int $0x30");
		}
	
		bool inSchedulerFunction(struct Thread* thr)
		{
			return thr->context.frame.rip >= (uintptr_t)&_sched_text_start && thr->context.frame.rip < (uintptr_t)&_sched_text_end;
		}
	}
}