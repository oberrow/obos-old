/*
	oboskrnl/arch/x86_64/irq/irq.h

	Copyright (c) 2023-2024 Omar Berrow
*/

#pragma once

#include <int.h>
#include <export.h>

namespace obos
{	struct LAPIC
	{
		alignas(0x10) const uint8_t resv1[0x20];
		alignas(0x10) uint32_t lapicID;
		alignas(0x10) uint32_t lapicVersion;
		alignas(0x10) const uint8_t resv2[0x40];
		alignas(0x10) uint32_t taskPriority;
		alignas(0x10) const uint32_t arbitrationPriority;
		alignas(0x10) const uint32_t processorPriority;
		alignas(0x10) uint32_t eoi; // write zero to send eoi.
		alignas(0x10) uint32_t remoteRead;
		alignas(0x10) uint32_t logicalDestination;
		alignas(0x10) uint32_t destinationFormat;
		alignas(0x10) uint32_t spuriousInterruptVector;
		alignas(0x10) const uint32_t inService0_31;
		alignas(0x10) const uint32_t inService32_63;
		alignas(0x10) const uint32_t inService64_95;
		alignas(0x10) const uint32_t inService96_127;
		alignas(0x10) const uint32_t inService128_159;
		alignas(0x10) const uint32_t inService160_191;
		alignas(0x10) const uint32_t inService192_223;
		alignas(0x10) const uint32_t inService224_255;
		alignas(0x10) const uint32_t triggerMode0_31;
		alignas(0x10) const uint32_t triggerMode32_63;
		alignas(0x10) const uint32_t triggerMode64_95;
		alignas(0x10) const uint32_t triggerMode96_127;
		alignas(0x10) const uint32_t triggerMode128_159;
		alignas(0x10) const uint32_t triggerMode160_191;
		alignas(0x10) const uint32_t triggerMode192_223;
		alignas(0x10) const uint32_t triggerMode224_255;
		alignas(0x10) const uint32_t interruptRequest0_31;
		alignas(0x10) const uint32_t interruptRequest32_63;
		alignas(0x10) const uint32_t interruptRequest64_95;
		alignas(0x10) const uint32_t interruptRequest96_127;
		alignas(0x10) const uint32_t interruptRequest128_159;
		alignas(0x10) const uint32_t interruptRequest160_191;
		alignas(0x10) const uint32_t interruptRequest192_223;
		alignas(0x10) const uint32_t interruptRequest224_255;
		alignas(0x10) uint32_t errorStatus;
		alignas(0x10) const uint8_t resv3[0x60];
		alignas(0x10) uint32_t lvtCMCI;
		alignas(0x10) uint32_t interruptCommand0_31;
		alignas(0x10) uint32_t interruptCommand32_63;
		alignas(0x10) uint32_t lvtTimer;
		alignas(0x10) uint32_t lvtThermalSensor;
		alignas(0x10) uint32_t lvtPerformanceMonitoringCounters;
		alignas(0x10) uint32_t lvtLINT0;
		alignas(0x10) uint32_t lvtLINT1;
		alignas(0x10) uint32_t lvtError;
		alignas(0x10) uint32_t initialCount;
		alignas(0x10) const uint32_t currentCount;
		alignas(0x10) const uint8_t resv4[0x40];
		alignas(0x10) uint32_t divideConfig;
		alignas(0x10) const uint8_t resv5[0x10];
	};
	struct HPET_Timer
	{
		/*struct
		{
			bool resv1 : 1;
			bool intTypeCnf : 1;
			bool intEnbCnf : 1;
			bool perINTCap : 1;
			bool sizeCap : 1;
			bool valSetCnf : 1;
			bool resv2 : 1;
			uint8_t _32ModeCnf : 5;
			bool intRouteCnf : 1;
			bool fsbEnCnf : 1;
			bool fsbIntDelCap : 1;
			uint16_t resv3;
			uint32_t intRouteCap;
		} timerConfigAndCapabilities;*/
		uint64_t timerConfigAndCapabilities;
		uint64_t timerComparatorValue;
		struct
		{
			uint32_t fsbIntVal;
			uint32_t fsbIntAddr;
		} timerFSBInterruptRoute;
		const uint64_t resv;
	};
	struct HPET
	{
		const struct {
			uint8_t revisionId;
			uint8_t numTimCap : 4;
			bool countSizeCap : 1;
			bool resv1 : 1;
			bool legRouteCap : 1;
			uint16_t vendorID;
			uint32_t counterCLKPeriod;
		} __attribute__((packed)) generalCapabilitiesAndID;
		const uint64_t resv1;
		/*struct
		{
			bool enableCFG : 1;
			bool legRtCNF : 1;
			uint8_t resv1 : 6;
			uint8_t vendorSpecific;
			uint64_t resv2 : 48;
		} __attribute__((packed)) generalConfig;*/
		uint64_t generalConfig;
		const uint64_t resv2;
		/*struct
		{
			bool t0IntSTS : 1;
			bool t1IntSTS : 1;
			bool t2IntSTS : 1;
			uint32_t tNIntSTS : 29;
			uint32_t resv1;
		} __attribute__((packed))  generalInterruptStatus;*/
		uint64_t generalInterruptStatus;
		const uint64_t resv3[0x19];
		uint64_t mainCounterValue;
		const uint64_t resv4;
		HPET_Timer timer0, timer1, timer2;
		// 0x160-0x400 are for timers 0-31
	};
	struct IOAPIC_RedirectionEntry
	{
		uint8_t vector;
		uint8_t delMod : 3;
		bool destMode : 1;
		const bool delivStatus : 1;
		bool intPol : 1;
		const bool remoteIRR : 1;
		bool triggerMode : 1;
		bool mask : 1;
		const uint64_t padding : 39;
		union
		{
			struct
			{
				uint8_t setOfProcessors;
			} __attribute__((packed)) logical;
			struct
			{
				uint8_t resv1 : 4;
				uint8_t lapicId : 4;
			} __attribute__((packed)) physical;
		} __attribute__((packed)) destination;
	} __attribute__((packed));
	struct IOAPIC_Registers
	{
		struct {
			const uint32_t resv1 : 24;
			uint8_t ioapicID : 4;
			const uint8_t resv2 : 4;
		} __attribute__((packed)) ioapicId;
		struct
		{
			const uint8_t version;
			const uint8_t resv1;
			const uint8_t maximumRedirectionEntries;
			const uint8_t resv2;
		} __attribute__((packed)) ioapicVersion;
		struct
		{
			const uint32_t resv1 : 24;
			const uint8_t ioapicID : 4;
			const uint8_t resv2 : 4;
		} __attribute__((packed)) ioapicArbitrationID;
		uint32_t resv1[13];
		IOAPIC_RedirectionEntry redirectionEntries[];
	} __attribute__((packed));
	struct IOAPIC
	{
		alignas(0x10) uint8_t ioregsel;
		alignas(0x10) uint32_t iowin;
	};
	static_assert(sizeof(LAPIC) == 0x400, "struct LAPIC has an invalid size.");
	static_assert(sizeof(IOAPIC_RedirectionEntry) == 8, "struct IOAPIC_RedirectionEntry has an invalid size.");
	static_assert(sizeof(HPET) == 0x160, "struct HPET has an invalid size.");
	static_assert(sizeof(HPET_Timer) == 32, "struct HPET_Timer has an invalid size.");
	
	extern OBOS_EXPORT volatile LAPIC* g_localAPICAddr; 
	extern OBOS_EXPORT volatile IOAPIC* g_ioAPICAddr;
	extern OBOS_EXPORT volatile HPET* g_HPETAddr;
	extern uint64_t g_hpetFrequency;

	enum class DestinationShorthand
	{
		None,
		Self,
		All,
		All_Except_Self,
		HighestValue = All_Except_Self
	};
	enum class DeliveryMode
	{
		Fixed, Default = Fixed,
		Fixed_LowestPriority,
		SMI,
		NMI = 4,
		INIT,
		SIPI,
	};

	void InitializeIrq(bool isBSP);
	OBOS_EXPORT void SendIPI(DestinationShorthand shorthand, DeliveryMode deliveryMode, uint8_t vector = 0, uint8_t _destination = 0);
	OBOS_EXPORT void SendEOI();
	OBOS_EXPORT bool MapIRQToVector(uint8_t irq, uint8_t vector);
	OBOS_EXPORT bool MaskIRQ(uint8_t irq, bool mask = false);
}