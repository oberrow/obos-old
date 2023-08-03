/*
	mutex.h

	Copyright (c) 2023 Omar Berrow
*/

#pragma	once

#include <multitasking/thread.h>

namespace obos
{
	namespace multitasking
	{
		class Mutex
		{
		public:
			Mutex() = default;
			
			void Lock(bool waitIfLocked = true);
			void Unlock();

			~Mutex();
			friend bool MutexLockCallback(Thread* _this, PVOID _mutex);
		private:
			bool m_locked = false;
			bool m_resume = false;
			Thread::Tid m_lockOwner = ((DWORD)-1);
		};
	}
}