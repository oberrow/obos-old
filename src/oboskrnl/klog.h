/*
	oboskrnl/klog.h

	Copyright (c) 2023 Omar Berrow
*/

#pragma once

#include <int.h>
#include <stdarg.h>

namespace obos
{
	namespace logger
	{
		size_t printf(const char* format, ...);
		size_t vprintf(const char* format, va_list list);
		size_t sprintf(char* dest, const char* format, ...);

		size_t log(const char* format, ...);
		size_t warning(const char* format, ...);
		size_t error(const char* format, ...);
		[[noreturn]] void panic(const char* format, ...);
	}
}