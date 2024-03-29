/*
	oboskrnl/klog.h

	Copyright (c) 2023-2024 Omar Berrow
*/

#pragma once

#include <int.h>
#include <export.h>
#include <stdarg.h>

#ifdef OBOS_DEBUG
#define OBOS_ASSERTP(expr, msg, ...) if (!(expr)) { obos::logger::panic(nullptr, "Function %s, File %s, Line %d: Assertion failed, \"%s\". " msg "\n", __func__, __FILE__, __LINE__, #expr __VA_ARGS__); }
#define OBOS_ASSERT(expr, msg, ...) if (!(expr)) { obos::logger::error("Function %s, File %s, Line %d: Assertion failed, \"%s\". " msg "\n", __func__, __FILE__, __LINE__, #expr __VA_ARGS__); }
#else
#define OBOS_ASSERTP(expr, msg, ...)
#define OBOS_ASSERT(expr, msg, ...)
#endif

namespace obos
{
	namespace logger
	{
		enum
		{
			GREY = 0xD3D3D3,
			GREEN = 0x03D12B,
			BLUE = 0x566F84,
			YELLOW = 0xffcc00,
			ERROR_RED = 0xcc3300,
			PANIC_RED = 0xac1616,
		};

		OBOS_EXPORT size_t printf(const char* format, ...);
		OBOS_EXPORT size_t vprintf(const char* format, va_list list);
		OBOS_EXPORT size_t vsprintf(char* dest, const char* format, va_list list);
		OBOS_EXPORT size_t sprintf(char* dest, const char* format, ...);

		constexpr const char* DEBUG_PREFIX_MESSAGE = "[Debug] ";
		constexpr const char* LOG_PREFIX_MESSAGE = "[Log] ";
		constexpr const char* INFO_PREFIX_MESSAGE = "[Log] ";
		constexpr const char* WARNING_PREFIX_MESSAGE = "[Warning] ";
		constexpr const char* ERROR_PREFIX_MESSAGE = "[Error] ";

		OBOS_EXPORT size_t debug(const char* format, ...);
		OBOS_EXPORT size_t log(const char* format, ...);
		OBOS_EXPORT size_t info(const char* format, ...);
		OBOS_EXPORT size_t warning(const char* format, ...);
		OBOS_EXPORT size_t error(const char* format, ...);
		OBOS_EXPORT [[noreturn]] void panic(void* stackTraceParameter, const char* format, ...);
		OBOS_EXPORT [[noreturn]] void panicVariadic(void* stackTraceParameter, const char* format, va_list list);

		OBOS_EXPORT void stackTrace(void* stackTraceParameter);
		void dumpAddr(uint32_t* addr);
	}
}