; driver_api/syscall_interface.asm
;
; Copyright (c) 2023 Omar Berrow

; global RegisterDriver
; global RegisterInterruptHandler
; global PicSendEoi
; global EnableIRQ
; global DisableIRQ
; global RegisterReadCallback
; global PrintChar
; global GetMultibootModule
; global RegisterFileReadCallback
; global CallSyscall

%macro SYSCALL_DEFINE 2
[global %1]
%1:
%assign current_syscall current_syscall + 1
	mov eax, %2
	int 0x50
	ret
%endmacro

%assign current_syscall 0

SYSCALL_DEFINE RegisterDriver, current_syscall
SYSCALL_DEFINE RegisterInterruptHandler, current_syscall
SYSCALL_DEFINE PicSendEoi, current_syscall
SYSCALL_DEFINE EnableIRQ, current_syscall
SYSCALL_DEFINE DisableIRQ, current_syscall
SYSCALL_DEFINE RegisterReadCallback, current_syscall
SYSCALL_DEFINE PrintChar, current_syscall
SYSCALL_DEFINE GetMultibootModule, current_syscall
SYSCALL_DEFINE RegisterFileReadCallback, current_syscall
SYSCALL_DEFINE RegisterFileExistsCallback, current_syscall
SYSCALL_DEFINE MapPhysicalTo, current_syscall
SYSCALL_DEFINE UnmapPhysicalTo, current_syscall
SYSCALL_DEFINE Printf, current_syscall

; All syscalls must be before this.
SYSCALL_DEFINE CallSyscall, [esp+8]