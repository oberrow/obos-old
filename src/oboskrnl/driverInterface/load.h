/*
	driverInterface/load.h

	Copyright (c) 2023 Omar Berrow
*/

#pragma once

#include <int.h>

#include <multitasking/threadAPI/thrHandle.h>

namespace obos
{
	namespace driverInterface
	{
		bool LoadModule(byte* file, size_t size, thread::ThreadHandle** mainThread);
	}
}