/*
	oboskrnl/arch/x86_64/interrupt_frame.h

	Copyright (c) 2023-2024 Omar Berrow
*/

#pragma once

#include <int.h>
#include <export.h>

#define BITFIELD_FROM_BIT(n) (1<<n)

namespace obos
{
	struct x86_64_flags
	{
		enum
		{
			RFLAGS_CARRY = BITFIELD_FROM_BIT(0),
			RFLAGS_PARITY = BITFIELD_FROM_BIT(2),
			RFLAGS_AUXILLARY_CARRY = BITFIELD_FROM_BIT(4),
			RFLAGS_ZERO = BITFIELD_FROM_BIT(6),
			RFLAGS_SIGN = BITFIELD_FROM_BIT(7),
			RFLAGS_TRAP = BITFIELD_FROM_BIT(8),
			RFLAGS_INTERRUPT_ENABLE = BITFIELD_FROM_BIT(9),
			RFLAGS_DIRECTION = BITFIELD_FROM_BIT(10),
			RFLAGS_OVERFLOW = BITFIELD_FROM_BIT(11),
			RFLAGS_IOPL_3 = BITFIELD_FROM_BIT(12) | BITFIELD_FROM_BIT(13),
			RFLAGS_NESTED_TASK = BITFIELD_FROM_BIT(14),
			RFLAGS_RESUME = BITFIELD_FROM_BIT(16),
			RFLAGS_VIRTUAL8086 = BITFIELD_FROM_BIT(17),
			RFLAGS_ALIGN_CHECK = BITFIELD_FROM_BIT(18),
			RFLAGS_VINTERRUPT_FLAG = BITFIELD_FROM_BIT(19),
			RFLAGS_VINTERRUPT_PENDING = BITFIELD_FROM_BIT(20),
			RFLAGS_CPUID = BITFIELD_FROM_BIT(21),
		};
		explicit operator uintptr_t() const { return m_bitfield; }
		uintptr_t get() const 
		{
			return m_bitfield;
		}
		x86_64_flags() = default;
		x86_64_flags(uintptr_t flags)
		{
			m_bitfield = flags | 2;
		}
		void setBit(uintptr_t bitmask)
		{
			bitmask |= 0b10;
			m_bitfield |= bitmask;
		}
		void clearBit(uintptr_t bitmask)
		{
			bitmask &= ~(0b10);
			m_bitfield &= (~bitmask);
		}
		bool getBit(uintptr_t bitmask) const
		{
			return (m_bitfield & bitmask) == bitmask;
		}

		bool operator[](uintptr_t bitmask) const
		{
			return getBit(bitmask);
		}
	private:
		uintptr_t m_bitfield;
	};

	struct interrupt_frame
	{
		// 0x00
		uintptr_t ds;
		// 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50
		// 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88
		uintptr_t rbp, ignored1, r8, r9, r10, r11, r12, r13, r14, r15,
				 rdi, rsi, ignored2, rbx, rdx, rcx, rax;
		// 0x90, 0x98
		uintptr_t intNumber, errorCode;
		// 0x100, 0x108
		uintptr_t rip, cs;
		// 0x110
		x86_64_flags rflags;
		// 0x118, 0x120
		uintptr_t rsp, ss;
	};

	OBOS_EXPORT void RegisterInterruptHandler(byte interrupt, void(*handler)(interrupt_frame* frame));
	void RegisterInterruptInIDT(int n, void(*handler)());
}