; gdt.asm
;
; Copyright (c) 2023 Omar Berrow

[BITS 32]

; extern "C" void gdtFlush(UINTPTR_T base);
global gdtFlush
; extern "C" void tssFlush();
global tssFlush

gdtFlush:
    mov eax, [esp+4]
    lgdt [eax]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush
.flush:
    ret
tssFlush:
    mov ax, (5 * 8) | 0
	ltr ax
	ret