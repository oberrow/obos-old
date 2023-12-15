/*
	oboskrnl/multitasking/arch.h

	Copyright (c) 2023 Omar Berrow
*/

#pragma once

// This file defines all platform-specific functions/structures that a port of the kernel to get the scheduler to work.

#include <int.h>

#if defined(__x86_64__) || defined(_WIN64)
#include <multitasking/x86_64/arch_structs.h>
#endif

extern "C" void idleTask();

namespace obos
{
	namespace thread
	{
		void switchToThreadImpl(taskSwitchInfo* info, struct Thread* thread);
		bool callBlockCallbackOnThread(taskSwitchInfo* info, bool(*callback)(void* thread, void* userdata), void* par1, void* par2);
		void setupThreadContext(taskSwitchInfo* info, void* stackInfo, uintptr_t entry, uintptr_t userdata, size_t stackSize, bool isUsermodeProgram);
		void freeThreadStackInfo(void* stackInfo);
		void setupTimerInterrupt();


		uintptr_t stopTimer();
		void startTimer(uintptr_t);
		
		void callScheduler(bool allCores);

		void* getCurrentCpuLocalPtr();
		bool StartCPUs();
		void StopCPUs(bool includingSelf);
	}
}