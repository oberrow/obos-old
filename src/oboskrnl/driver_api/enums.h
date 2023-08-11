/*
	driver_api/enums.h

	Copyright (c) 2023 Omar Berrow
*/

#pragma once

#ifdef __cplusplus
#define DEFINE_ENUM enum class
#else
#define DEFINE_ENUM enum
#endif

#ifdef __cplusplus

namespace obos
{
	namespace driverAPI
	{

#endif
		DEFINE_ENUM serviceType
		{
			// Ex: A driver that hasn't been registered yet.
			SERVICE_TYPE_INVALID = -1,
			// Ex: fat driver, ext2 driver.
			SERVICE_TYPE_FILESYSTEM,
			// Ex: SATA driver.
			SERVICE_TYPE_STORAGE_DEVICE,
			// Ex: iso mounter.
			SERVICE_TYPE_VIRTUAL_STORAGE_DEVICE,
			// Ex: PS/2 Keyboard driver, PS/2 Mouse driver.
			SERVICE_TYPE_USER_INPUT_DEVICE,
			// Ex: On-screen keyboard.
			SERVICE_TYPE_VIRTUAL_USER_INPUT_DEVICE,
			// Ex: GPU driver, VGA driver.
			SERVICE_TYPE_GRAPHICS_DEVICE,
			// Ex: A background process that shuts down the computer when the cpu temperature is too high.
			SERVICE_TYPE_MONITOR,
			// Ex: A driver that allows pe files to be run.
			SERVICE_TYPE_KERNEL_EXTENSION,
			// Ex: A serial driver, a network card driver.
			SERVICE_TYPE_COMMUNICATION,
			// Ex: A virtual serial driver (see com0com on windows to understand this).
			SERVICE_TYPE_VIRTUAL_COMMUNICATION,
		};
		DEFINE_ENUM exitStatus
		{
			EXIT_STATUS_INVALID_SYSCALL = -1,
			EXIT_STATUS_SUCCESS,
			EXIT_STATUS_ALREADY_REGISTERED,
			EXIT_STATUS_ACCESS_DENIED,
			EXIT_STATUS_NO_SUCH_DRIVER,
			EXIT_STATUS_INVALID_PARAMETER,
		};

		struct interrupt_frame
		{
			// +0		
			UINT32_T ds;
			//		  +4,  +8,  +12, +16, +20, +24, +28, +32
			UINT32_T edi, esi, ebp, esp, ebx, edx, ecx, eax;
			// +36
			UINT8_T intNumber;
			// +40 (Padding)
			UINT32_T errorCode;
			//		 +44,+48,    +52,	   +56,+60
			UINT32_T eip, cs, eflags, useresp, ss;
			// +64 (End)
		};

#ifdef __cplusplus
	}
}

#endif