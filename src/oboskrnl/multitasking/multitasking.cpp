/*
	multitasking.cpp

	Copyright (c) 2023 Omar Berrow
*/

#include <multitasking/multitasking.h>
#include <multitasking/thread.h>

#include <utils/memory.h>

#include <inline-asm.h>

#include <descriptors/idt/idt.h>
#include <descriptors/idt/pic.h>

namespace obos
{
	extern void kmainThr();
	extern char thrstack_top;
	namespace multitasking
	{
		list_t* g_threads = nullptr;
		list_t* g_threadPriorityList[4] = {
			nullptr, nullptr, nullptr, nullptr,
		};
		Thread* g_currentThread = nullptr;

		bool canRanTask(Thread* thread)
		{
			return  thread->iterations < (UINT32_T)thread->priority &&
					thread->status == (UINT32_T)Thread::status_t::RUNNING;
		}

		extern void switchToTaskAsm();

		void switchToTask(Thread* newThread)
		{
			g_currentThread = newThread;
			asm volatile("mov %0, %%eax" :
										 : "memory"(&g_currentThread->frame));
			switchToTaskAsm();
		}

		void findNewTask(const interrupt_frame* frame)
		{
			if (g_currentThread)
				g_currentThread->frame = *frame;
			else
				return;
			
			// Tf all threads were ran, then clear the amount of iterations.

			EnterKernelSection();
			Thread* currentThread = nullptr;
			for (int i = 0; i < 4 && !currentThread; i++)
			{
				list_iterator_t* iter = list_iterator_new(g_threadPriorityList[i], LIST_HEAD);
				for (list_node_t* node = list_iterator_next(iter); node != nullptr; node = list_iterator_next(iter))
				{
					currentThread = (Thread*)node->val;
					break;
				}
				list_iterator_destroy(iter);
			}
			if (currentThread->iterations >= (UINT32_T)currentThread->priority)
			{
				list_iterator_t* iter = list_iterator_new(g_threads, LIST_HEAD);
				for (list_node_t* node = list_iterator_next(iter); node != nullptr; node = list_iterator_next(iter), currentThread = (Thread*)node->val)
					currentThread->iterations = 0;
				list_iterator_destroy(iter);
			}

			findNew:

			list_node_t* currentNode = nullptr;

			// Find a new task.
			for (int i = 3; i > -1 && !currentNode; i--)
			{
				list_iterator_t* iter = list_iterator_new(g_threadPriorityList[i], LIST_TAIL);
				for (list_node_t* node = list_iterator_next(iter); node != nullptr; node = list_iterator_next(iter))
				{
					currentThread = (Thread*)node->val;
					if (utils::testBitInBitfield(currentThread->status, 3))
					{
						if (currentThread->isBlockedCallback)
							if (currentThread->isBlockedCallback(currentThread, currentThread->isBlockedUserdata))
								utils::clearBitInBitfield(currentThread->status, 3);
							else {}
						else
							currentThread->status = (UINT32_T)Thread::status_t::DEAD;
					}
					if (canRanTask(currentThread))
					{
						currentNode = node;
						currentThread->iterations++;
						break;
					}
				}
				list_iterator_destroy(iter);
			}
			if (!currentNode)
				goto findNew;
			LeaveKernelSection();
			if (frame->intNumber != 0x30)
				SendEOI(frame->intNumber - 32);
			if (currentThread == g_currentThread)
				return;
			switchToTask(currentThread);
		}

		void InitializeMultitasking()
		{
			EnterKernelSection();
			g_threads = list_new();
			for(int i = 0; i < 4; g_threadPriorityList[i++] = list_new());
			Thread* kmainThread = g_currentThread = new Thread{};
			kmainThread->tid = 0;
			kmainThread->frame.eip = GET_FUNC_ADDR(kmainThr);
			kmainThread->frame.esp = GET_FUNC_ADDR(&thrstack_top);
			kmainThread->frame.ebp = 0;
			kmainThread->priority = Thread::priority_t::NORMAL;
			kmainThread->status = (UINT32_T)Thread::status_t::RUNNING;
			kmainThread->exitCode = 0;
			kmainThread->iterations = 0;
			kmainThread->lastError = 0;
			list_lpush(g_threads			  , list_node_new(kmainThread));
			list_lpush(g_threadPriorityList[2], list_node_new(kmainThread));

			Pic masterPic{ Pic::PIC1_CMD, Pic::PIC1_DATA };
			// 400 hz, 2.5 ms. (1/400 * 1000)
			UINT32_T divisor = 1193180 / 400;
			
			outb(0x43, 0x36);
			
			UINT8_T l = (UINT8_T)(divisor & 0xFF);
			UINT8_T h = (UINT8_T)((divisor >> 8) & 0xFF);
			
			// Send the frequency divisor.
			outb(0x40, l);
			outb(0x40, h);
			
			masterPic.enableIrq(0);
			RegisterInterruptHandler(0x20, findNewTask);
			RegisterInterruptHandler(0x30, findNewTask);
			LeaveKernelSection();
			// Just in case.
			asm volatile("mov %0, %%eax" :
										: "memory"(&kmainThread->frame));
			switchToTaskAsm();
		}
	}
}