/*
	oboskrnl/arch/x86_64/irq/timer.cpp

	Copyright (c) 2023-2024 Omar Berrow
*/

#include <x86_64-utils/asm.h>

#include <arch/x86_64/interrupt.h>

#include <arch/x86_64/irq/timer.h>
#include <arch/x86_64/irq/irq.h>

#include <klog.h>

namespace obos
{
	void(*g_currentTimerHandler)(interrupt_frame*);
	static void IntermediateTimerIntHandler(interrupt_frame* frame)
	{
		if(g_currentTimerHandler)
			g_currentTimerHandler(frame);
		SendEOI();
	}
	void ConfigureAPICTimer(void(*handler)(interrupt_frame* frame), byte isr, uint32_t initialCount, TimerConfig timerConfig, TimerDivisor divisor, bool mask)
	{
		uintptr_t savedFlags = saveFlagsAndCLI();
		RegisterInterruptHandler(isr, IntermediateTimerIntHandler);
		g_currentTimerHandler = handler;
		divisor = (TimerDivisor)((int)divisor & 0b1101);
		if (timerConfig != 0 && timerConfig != TIMER_CONFIG_PERIODIC)
			timerConfig = TIMER_CONFIG_PERIODIC;
		g_localAPICAddr->divideConfig = divisor;
		g_localAPICAddr->lvtTimer = isr | timerConfig;
		MaskTimer(mask);
		g_localAPICAddr->initialCount = initialCount;
		restorePreviousInterruptStatus(savedFlags);
	}
	void MaskTimer(bool mask)
	{
		if(!mask)
			g_localAPICAddr->lvtTimer |= (1 << 16);
		else
			g_localAPICAddr->lvtTimer &= ~(1 << 16);
	}
}