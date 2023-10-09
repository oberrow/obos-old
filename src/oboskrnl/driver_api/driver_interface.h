/*
	oboskrnl/driver_api/driver_interface.h

	Copyright (c) 2023 Omar Berrow
*/

#pragma once

#include <types.h>
#include <handle.h>

#include <driver_api/enums.h>
#include <driver_api/syscalls.h>

#include <utils/list.h>

#include <multitasking/mutex/mutexHandle.h>

namespace obos
{
	namespace driverAPI
	{
		struct connection_buffer
		{
			multitasking::MutexHandle mutex{};
			PBYTE buffer = nullptr;
			PBYTE bufferPosition = nullptr;
			SIZE_T szBuffer = 0;
		};

		class DriverConnectionHandle final
		{
		public:
			bool SendData(PBYTE buffer, SIZE_T size, bool failIfConnectionMutexLocked = true);
			bool RecvData(PBYTE data, SIZE_T size, bool waitForData = true, bool peek = false, bool failIfConnectionMutexLocked = true);

			bool CloseConnection(bool setLastError = true);
			~DriverConnectionHandle();
			
			driverIdentification* GetDriverIdentification() { return m_driverIdentity; }

			friend class DriverClientConnectionHandle;
			friend DriverConnectionHandle* Listen();
		private:
			DriverConnectionHandle() = default;
			bool SendDataImpl(connection_buffer& conn_buffer, PBYTE buffer, SIZE_T size, bool failIfConnectionMutexLocked);
			bool RecvDataImpl(connection_buffer& conn_buffer, PBYTE data, SIZE_T size, bool waitForData, bool peek, bool failIfConnectionMutexLocked);
			driverIdentification* m_driverIdentity = nullptr;
			bool m_isClosingConnection = false;
			connection_buffer m_inputBuffer;
			connection_buffer m_outputBuffer;
		};
		class DriverClientConnectionHandle final
		{
		public:
			DriverClientConnectionHandle() = default;
			
			bool OpenConnection(DWORD driverId, DWORD connectionTimeoutMilliseconds = 5000);
			bool CloseConnection(bool setLastError = true);

			bool SendData(PBYTE buffer, SIZE_T size, bool failIfConnectionMutexLocked = true);
			bool RecvData(PBYTE data, SIZE_T size, bool waitForData = true, bool peek = false, bool failIfConnectionMutexLocked = true);

			virtual ~DriverClientConnectionHandle();
		private:
			DriverConnectionHandle* m_driverConnection = nullptr;
		};

		DriverConnectionHandle* Listen();
	}
}