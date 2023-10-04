/*
	oboskrnl/error.h

	Copyright (c) 2023 Omar Berrow
*/

#pragma once

#include <types.h>

#ifdef __cplusplus
namespace obos
{
#endif
	enum errors
	{
		OBOS_ERROR_NO_ERROR,
		OBOS_ERROR_INVALID_PARAMETER,
		OBOS_ERROR_NO_MEMORY,
		OBOS_ERROR_ALREADY_EXISTS,
		OBOS_ERROR_DRIVER_NOT_LOADED,
		OBOS_ERROR_NOT_A_FILESYSTEM_DRIVER,
		OBOS_ERROR_FILE_NOT_FOUND,
		OBOS_ERROR_MOUNTPOINT_NOT_FOUND,
		OBOS_ERROR_FILESYSTEM_ERROR,
		OBOS_ERROR_UNOPENED_HANDLE,
		OBOS_ERROR_NO_SUCH_OBJECT,
		OBOS_ERROR_THREAD_DIED,
		OBOS_ERROR_HANDLE_ALREADY_OPENED,
		OBOS_ERROR_ACCESS_DENIED,
		OBOS_ERROR_AVOIDED_DEADLOCK,
		OBOS_ERROR_ELF_INCORRECT_FILE,
		OBOS_ERROR_ELF_INCORRECT_ARCHITECTURE,
		OBOS_ERROR_BASE_ADDRESS_USED,
		OBOS_ERROR_BUFFER_TOO_SMALL,
		OBOS_ERROR_NOT_A_DRIVER,
		OBOS_ERROR_PREMATURE_PROCESS_EXIT,
		OBOS_ERROR_MUTEX_NOT_LOCKED,
		
		OBOS_HIGHEST_ERROR,
	};
	DWORD GetLastError();
	void SetLastError(DWORD err);
#ifdef __cplusplus
}
#endif