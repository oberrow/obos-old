/*
	driver_api/syscalls.h

	Copyright (c) 2023 Omar Berrow
*/

#pragma once

#include <types.h>
#include <driver_api/enums.h>

#include <process/process.h>

namespace obos
{
	namespace driverAPI
	{
		struct driverIdentification
		{
			DWORD driverId = 0;
			process::Process* process;
			serviceType service_type = serviceType::OBOS_SERVICE_TYPE_INVALID;
			list_t* driverConnections;
		};
		extern driverIdentification** g_registeredDrivers;
		extern SIZE_T g_registeredDriversCapacity;
		void RegisterSyscalls();
	}
}