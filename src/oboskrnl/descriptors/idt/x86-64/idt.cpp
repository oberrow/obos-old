/*
	oboskrnl/descriptors/idt/idt.cpp

	Copyright (c) 2023 Omar Berrow
*/

#include "idt.h"

#include <driver_api/interrupts.h>
#include <syscalls/syscalls.h>

#include <types.h>
#include <new>

extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();
extern "C" void isr32();
extern "C" void isr33();
extern "C" void isr34();
extern "C" void isr35();
extern "C" void isr36();
extern "C" void isr37();
extern "C" void isr38();
extern "C" void isr39();
extern "C" void isr40();
extern "C" void isr41();
extern "C" void isr42();
extern "C" void isr43();
extern "C" void isr44();
extern "C" void isr45();
extern "C" void isr46();
extern "C" void isr47();
extern "C" void isr48();

namespace obos
{
	extern void idtFlush(UINTPTR_T base);
	void(*g_interruptHandlers[256])(const obos::interrupt_frame* frame);
	struct IdtPointer
	{
		UINT16_T limit;
		UINT64_T base;
	} __attribute__((packed));

	IdtEntry::IdtEntry(UINTPTR_T base, UINT16_T sel, UINT8_T typeAttributes, UINT8_T ist)
	{
		m_offset1 = (UINT16_T)(base & 0xFFFF);
		m_offset2 = (UINT16_T)((base & 0xFFFF0000) >> 16);
		m_offset3 = (UINT32_T)((base & 0xFFFFFFFF00000000) >> 32);

		m_selector = sel;
		m_ist = ist;
		m_typeAttributes = typeAttributes;
		m_resv1 = 0;
	}

#define REGISTER_INTERRUPT(interrupt) new (s_idtEntries + interrupt) IdtEntry((UINTPTR_T)isr ##interrupt, 0x08, 0x8E, 0);

	static IdtPointer s_idtPointer;
	static IdtEntry s_idtEntries[256];

	void InitializeIdt()
	{
		REGISTER_INTERRUPT(0);
		REGISTER_INTERRUPT(1);
		REGISTER_INTERRUPT(2);
		REGISTER_INTERRUPT(3);
		REGISTER_INTERRUPT(4);
		REGISTER_INTERRUPT(5);
		REGISTER_INTERRUPT(6);
		REGISTER_INTERRUPT(7);
		REGISTER_INTERRUPT(8);
		REGISTER_INTERRUPT(9);
		REGISTER_INTERRUPT(10);
		REGISTER_INTERRUPT(11);
		REGISTER_INTERRUPT(12);
		REGISTER_INTERRUPT(13);
		REGISTER_INTERRUPT(14);
		REGISTER_INTERRUPT(15);
		REGISTER_INTERRUPT(16);
		REGISTER_INTERRUPT(17);
		REGISTER_INTERRUPT(18);
		REGISTER_INTERRUPT(19);
		REGISTER_INTERRUPT(20);
		REGISTER_INTERRUPT(21);
		REGISTER_INTERRUPT(22);
		REGISTER_INTERRUPT(23);
		REGISTER_INTERRUPT(24);
		REGISTER_INTERRUPT(25);
		REGISTER_INTERRUPT(26);
		REGISTER_INTERRUPT(27);
		REGISTER_INTERRUPT(28);
		REGISTER_INTERRUPT(29);
		REGISTER_INTERRUPT(30);
		REGISTER_INTERRUPT(31);
		REGISTER_INTERRUPT(32);
		REGISTER_INTERRUPT(33);
		REGISTER_INTERRUPT(34);
		REGISTER_INTERRUPT(35);
		REGISTER_INTERRUPT(36);
		REGISTER_INTERRUPT(37);
		REGISTER_INTERRUPT(38);
		REGISTER_INTERRUPT(39);
		REGISTER_INTERRUPT(40);
		REGISTER_INTERRUPT(41);
		REGISTER_INTERRUPT(42);
		REGISTER_INTERRUPT(43);
		REGISTER_INTERRUPT(44);
		REGISTER_INTERRUPT(45);
		REGISTER_INTERRUPT(46);
		REGISTER_INTERRUPT(47);
		REGISTER_INTERRUPT(48);
		REGISTER_INTERRUPT(64);
		REGISTER_INTERRUPT(80);

		s_idtPointer.limit = sizeof(s_idtEntries) - 1;
		s_idtPointer.base = (UINTPTR_T)&s_idtEntries;
		idtFlush((UINTPTR_T)&s_idtPointer);
	}
	void RegisterInterruptHandler(UINT8_T interrupt, void(*isr)(const interrupt_frame* frame))
	{
		g_interruptHandlers[interrupt] = isr;
	}
}