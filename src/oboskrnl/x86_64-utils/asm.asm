; oboskrnl/x86_64-utils/asm.asm

; Copyright (c) 2023 Omar Berrow

[BITS 64]

global _ZN4obos4outbEth
global _ZN4obos4outwEtt
global _ZN4obos4outdEtj
global _ZN4obos3inbEt
global _ZN4obos3inwEt
global _ZN4obos3indEt
global _ZN4obos3cliEv
global _ZN4obos3stiEv
global _ZN4obos3hltEv
global _ZN4obos6getCR2Ev
global _ZN4obos6memory17getCurrentPageMapEv
global _ZN4obos6memory7PageMap12switchToThisEv
global _ZN4obos7getEFEREv
global _ZN4obos6invlpgEm
global _ZN4obos5rdmsrEj
global _ZN4obos5wrmsrEjm
global _ZN4obos15saveFlagsAndCLIEv
global _ZN4obos30restorePreviousInterruptStatusEm
global _ZN4obos7haltCPUEv
global _ZN4obos9__cpuid__EmmPmS0_S0_S0_
global _ZN4obos5rdtscEv
global _ZN4obos11set_if_zeroEPmm

_ZN4obos4outbEth:
	mov dx, di
	mov al, sil
	out dx, al
	ret
_ZN4obos4outwEtt:
	mov dx, di
	mov ax, si
	out dx, ax
	ret
_ZN4obos4outdEtj:
	mov dx, di
	mov eax, esi
	out dx, eax
	ret
_ZN4obos3inbEt:
	mov eax, 0
	mov dx, di
	in al, dx
	ret
_ZN4obos3inwEt:
	mov eax, 0
	mov dx, di
	in ax, dx
	ret
_ZN4obos3indEt:
	mov dx, di
	in eax, dx
	ret
_ZN4obos3cliEv:
	cli
	ret
_ZN4obos3stiEv:
	sti
	ret
_ZN4obos3hltEv:
	hlt
	ret
_ZN4obos6getCR2Ev:
	mov rax, cr2
	ret
_ZN4obos6memory17getCurrentPageMapEv:
	mov rax, cr3
	ret
_ZN4obos6memory7PageMap12switchToThisEv:
	mov cr3, rdi
	ret
_ZN4obos7getEFEREv:
	mov ecx, 0xC0000080
    rdmsr
	ret
_ZN4obos6invlpgEm:
	invlpg [rdi]
	ret

_ZN4obos5rdmsrEj:
	push rbp
	mov rbp, rsp

	xor rax,rax
	xor rdx,rdx
	xor rcx,rcx

	; Load the msr address.
	mov ecx, edi

	rdmsr

	shl rdx, 32
	or rax, rdx

	leave
	ret
_ZN4obos5wrmsrEjm:
	push rbp
	mov rbp, rsp

;	Setup the registers for wrmsr.

	mov ecx, edi

	mov eax, esi
	mov rdx, rsi
	sar rdx, 32

	wrmsr

	leave
	ret
_ZN4obos15saveFlagsAndCLIEv:
	pushfq
	pop rax
	cli
	ret
_ZN4obos30restorePreviousInterruptStatusEm:
	push rdi
	popfq
	ret
_ZN4obos7haltCPUEv:
	cli
.loop:
	hlt
	jmp .loop
_ZN4obos9__cpuid__EmmPmS0_S0_S0_:
	push rbp
	mov rbp, rsp
	push rdx

	mov r10, rcx
	mov r11, rdx

	mov eax, edi
	mov ecx, esi
	cpuid

	mov [r11], rax
	mov [r10], rbx
	mov [r8], rcx
	mov [r9], rdx

	pop rdx
	leave
	ret
_ZN4obos5rdtscEv:
	push rbp
	mov rbp, rsp
	
	rdtsc

	shl rdx, 32
	or rax, rdx

	leave
	ret
global _ZN4obos10atomic_setEPb
_ZN4obos10atomic_setEPb:
	mov al, [rdi]
	mov sil, 1
	lock cmpxchg byte [rdi], sil
	ret
global _ZN4obos12atomic_clearEPb
_ZN4obos12atomic_clearEPb:
	mov al, [rdi]
	mov sil, 0
	lock cmpxchg byte [rdi], sil
	ret
global _ZN4obos11atomic_testEPb
_ZN4obos11atomic_testEPb:
	mov al, byte [rdi]
	ret
global _ZN4obos6getCR4Ev
_ZN4obos6getCR4Ev:
	mov rax, cr4
	ret
global _ZN4obos6setCR4Em
_ZN4obos6setCR4Em:
	mov cr4, rdi
	ret
_ZN4obos11set_if_zeroEPmm:
	pushfq
	pop rax
	and rax, ~(1<<6)
	push rax
	popfq
	mov rax, [rdi]
	test rax, rax
	cmovz rax, rsi
	mov [rdi], rax
	ret