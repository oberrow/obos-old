/*
	arch/x86_64/syscall/register.h

	Copyright (c) 2023-2024 Omar Berrow
*/

#pragma once

#include <int.h>

namespace obos
{
	namespace syscalls
	{
		// Zero-based.
		constexpr size_t g_syscallTableLimit = 0x1fff;
		extern uintptr_t g_syscallTable[g_syscallTableLimit + 1];
		void RegisterSyscalls();
		void RegisterSyscall(uint16_t n, uintptr_t func);

		// These next three functions are never actually defined except for the actual syscall
		// They're just defined for documentation purposes

		/// <summary>
		/// Syscall number: 55<para></para>
		/// Gets the last error for the current thread.
		/// </summary>
		/// <returns>The last error</returns>
		uint32_t SyscallGetLastError();
		/// <summary>
		/// Syscall number: 56<para></para>
		/// Sets the last error for the current thread.
		/// </summary>
		/// <param name="newError">The new error code.</param>
		void SyscallSetLastError(uint32_t newError);

		/// <summary>
		/// Syscall number: 57<para></para>
		/// Loads a kernel module.
		/// </summary>
		/// <param name="data">The beginning of the file.</param>
		/// <param name="size">The size of the file.</param>
		/// <returns>Whether the function succeeded (true) or not (false).</returns>
		bool SyscallLoadModule(const byte* data, size_t size);
	}
}