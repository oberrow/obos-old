/*
	oboskrnl/arch/x86_64/irq/timer.h

	Copyright (c) 2023 Omar Berrow
*/

#include <int.h>

#include <arch/x86_64/interrupt.h>

namespace obos
{
	enum TimerConfig
	{
		TIMER_CONFIG_ONE_SHOT,
		TIMER_CONFIG_PERIODIC = 0x20000,
	};
	enum TimerDivisor
	{
		TIMER_DIVISOR_TWO = 0b0000,
		TIMER_DIVISOR_FOUR = 0b0001,
		TIMER_DIVISOR_EIGHT = 0b0100,
		TIMER_DIVISOR_SIXTEEN = 0b0101,
		TIMER_DIVISOR_THIRTY_TWO = 0b1000,
		TIMER_DIVISOR_SIXTY_FOUR = 0b1001,
		TIMER_DIVISOR_ONE_HUNDERED_TWENTY_EIGHT = 0b1100,
		TIMER_DIVISOR_ONE = 0b1101,
	};
	void ConfigureAPICTimer(void(*handler)(interrupt_frame* frame), byte isr, uint32_t initialCount, TimerConfig timerConfig, TimerDivisor divisor);
	void MaskTimer(bool mask); // Masks the timer interrupt. If !mask, the timer interrupt is disabled.
}