/*
	oboskrnl/arch/x86_64/idt.cpp

	Copyright (c) 2023-2024 Omar Berrow
*/

#include <int.h>
#include <memory_manipulation.h>

#include <arch/x86_64/interrupt.h>

#define DEFINE_ISR_FUNCTION(n) extern "C" void isr ##n();

DEFINE_ISR_FUNCTION(0);
DEFINE_ISR_FUNCTION(1);
DEFINE_ISR_FUNCTION(2);
DEFINE_ISR_FUNCTION(3);
DEFINE_ISR_FUNCTION(4);
DEFINE_ISR_FUNCTION(5);
DEFINE_ISR_FUNCTION(6);
DEFINE_ISR_FUNCTION(7);
DEFINE_ISR_FUNCTION(8);
DEFINE_ISR_FUNCTION(9);
DEFINE_ISR_FUNCTION(10);
DEFINE_ISR_FUNCTION(11);
DEFINE_ISR_FUNCTION(12);
DEFINE_ISR_FUNCTION(13);
DEFINE_ISR_FUNCTION(14);
DEFINE_ISR_FUNCTION(15);
DEFINE_ISR_FUNCTION(16);
DEFINE_ISR_FUNCTION(17);
DEFINE_ISR_FUNCTION(18);
DEFINE_ISR_FUNCTION(19);
DEFINE_ISR_FUNCTION(20);
DEFINE_ISR_FUNCTION(21);
DEFINE_ISR_FUNCTION(22);
DEFINE_ISR_FUNCTION(23);
DEFINE_ISR_FUNCTION(24);
DEFINE_ISR_FUNCTION(25);
DEFINE_ISR_FUNCTION(26);
DEFINE_ISR_FUNCTION(27);
DEFINE_ISR_FUNCTION(28);
DEFINE_ISR_FUNCTION(29);
DEFINE_ISR_FUNCTION(30);
DEFINE_ISR_FUNCTION(31);
DEFINE_ISR_FUNCTION(32);
DEFINE_ISR_FUNCTION(33);
DEFINE_ISR_FUNCTION(34);
DEFINE_ISR_FUNCTION(35);
DEFINE_ISR_FUNCTION(36);
DEFINE_ISR_FUNCTION(37);
DEFINE_ISR_FUNCTION(38);
DEFINE_ISR_FUNCTION(39);
DEFINE_ISR_FUNCTION(40);
DEFINE_ISR_FUNCTION(41);
DEFINE_ISR_FUNCTION(42);
DEFINE_ISR_FUNCTION(43);
DEFINE_ISR_FUNCTION(44);
DEFINE_ISR_FUNCTION(45);
DEFINE_ISR_FUNCTION(46);
DEFINE_ISR_FUNCTION(47);
DEFINE_ISR_FUNCTION(48);
DEFINE_ISR_FUNCTION(49);
DEFINE_ISR_FUNCTION(50);
DEFINE_ISR_FUNCTION(51);
DEFINE_ISR_FUNCTION(52);
DEFINE_ISR_FUNCTION(53);
DEFINE_ISR_FUNCTION(54);
DEFINE_ISR_FUNCTION(55);
DEFINE_ISR_FUNCTION(56);
DEFINE_ISR_FUNCTION(57);
DEFINE_ISR_FUNCTION(58);
DEFINE_ISR_FUNCTION(59);
DEFINE_ISR_FUNCTION(60);
DEFINE_ISR_FUNCTION(61);
DEFINE_ISR_FUNCTION(62);
DEFINE_ISR_FUNCTION(63);
DEFINE_ISR_FUNCTION(64);
DEFINE_ISR_FUNCTION(65);
DEFINE_ISR_FUNCTION(66);
DEFINE_ISR_FUNCTION(67);
DEFINE_ISR_FUNCTION(68);
DEFINE_ISR_FUNCTION(69);
DEFINE_ISR_FUNCTION(70);
DEFINE_ISR_FUNCTION(71);
DEFINE_ISR_FUNCTION(72);
DEFINE_ISR_FUNCTION(73);
DEFINE_ISR_FUNCTION(74);
DEFINE_ISR_FUNCTION(75);
DEFINE_ISR_FUNCTION(76);
DEFINE_ISR_FUNCTION(77);
DEFINE_ISR_FUNCTION(78);
DEFINE_ISR_FUNCTION(79);
DEFINE_ISR_FUNCTION(80);
DEFINE_ISR_FUNCTION(81);
DEFINE_ISR_FUNCTION(82);
DEFINE_ISR_FUNCTION(83);
DEFINE_ISR_FUNCTION(84);
DEFINE_ISR_FUNCTION(85);
DEFINE_ISR_FUNCTION(86);
DEFINE_ISR_FUNCTION(87);
DEFINE_ISR_FUNCTION(88);
DEFINE_ISR_FUNCTION(89);
DEFINE_ISR_FUNCTION(90);
DEFINE_ISR_FUNCTION(91);
DEFINE_ISR_FUNCTION(92);
DEFINE_ISR_FUNCTION(93);
DEFINE_ISR_FUNCTION(94);
DEFINE_ISR_FUNCTION(95);
DEFINE_ISR_FUNCTION(96);
DEFINE_ISR_FUNCTION(97);
DEFINE_ISR_FUNCTION(98);
DEFINE_ISR_FUNCTION(99);
DEFINE_ISR_FUNCTION(100);
DEFINE_ISR_FUNCTION(101);
DEFINE_ISR_FUNCTION(102);
DEFINE_ISR_FUNCTION(103);
DEFINE_ISR_FUNCTION(104);
DEFINE_ISR_FUNCTION(105);
DEFINE_ISR_FUNCTION(106);
DEFINE_ISR_FUNCTION(107);
DEFINE_ISR_FUNCTION(108);
DEFINE_ISR_FUNCTION(109);
DEFINE_ISR_FUNCTION(110);
DEFINE_ISR_FUNCTION(111);
DEFINE_ISR_FUNCTION(112);
DEFINE_ISR_FUNCTION(113);
DEFINE_ISR_FUNCTION(114);
DEFINE_ISR_FUNCTION(115);
DEFINE_ISR_FUNCTION(116);
DEFINE_ISR_FUNCTION(117);
DEFINE_ISR_FUNCTION(118);
DEFINE_ISR_FUNCTION(119);
DEFINE_ISR_FUNCTION(120);
DEFINE_ISR_FUNCTION(121);
DEFINE_ISR_FUNCTION(122);
DEFINE_ISR_FUNCTION(123);
DEFINE_ISR_FUNCTION(124);
DEFINE_ISR_FUNCTION(125);
DEFINE_ISR_FUNCTION(126);
DEFINE_ISR_FUNCTION(127);
DEFINE_ISR_FUNCTION(128);
DEFINE_ISR_FUNCTION(129);
DEFINE_ISR_FUNCTION(130);
DEFINE_ISR_FUNCTION(131);
DEFINE_ISR_FUNCTION(132);
DEFINE_ISR_FUNCTION(133);
DEFINE_ISR_FUNCTION(134);
DEFINE_ISR_FUNCTION(135);
DEFINE_ISR_FUNCTION(136);
DEFINE_ISR_FUNCTION(137);
DEFINE_ISR_FUNCTION(138);
DEFINE_ISR_FUNCTION(139);
DEFINE_ISR_FUNCTION(140);
DEFINE_ISR_FUNCTION(141);
DEFINE_ISR_FUNCTION(142);
DEFINE_ISR_FUNCTION(143);
DEFINE_ISR_FUNCTION(144);
DEFINE_ISR_FUNCTION(145);
DEFINE_ISR_FUNCTION(146);
DEFINE_ISR_FUNCTION(147);
DEFINE_ISR_FUNCTION(148);
DEFINE_ISR_FUNCTION(149);
DEFINE_ISR_FUNCTION(150);
DEFINE_ISR_FUNCTION(151);
DEFINE_ISR_FUNCTION(152);
DEFINE_ISR_FUNCTION(153);
DEFINE_ISR_FUNCTION(154);
DEFINE_ISR_FUNCTION(155);
DEFINE_ISR_FUNCTION(156);
DEFINE_ISR_FUNCTION(157);
DEFINE_ISR_FUNCTION(158);
DEFINE_ISR_FUNCTION(159);
DEFINE_ISR_FUNCTION(160);
DEFINE_ISR_FUNCTION(161);
DEFINE_ISR_FUNCTION(162);
DEFINE_ISR_FUNCTION(163);
DEFINE_ISR_FUNCTION(164);
DEFINE_ISR_FUNCTION(165);
DEFINE_ISR_FUNCTION(166);
DEFINE_ISR_FUNCTION(167);
DEFINE_ISR_FUNCTION(168);
DEFINE_ISR_FUNCTION(169);
DEFINE_ISR_FUNCTION(170);
DEFINE_ISR_FUNCTION(171);
DEFINE_ISR_FUNCTION(172);
DEFINE_ISR_FUNCTION(173);
DEFINE_ISR_FUNCTION(174);
DEFINE_ISR_FUNCTION(175);
DEFINE_ISR_FUNCTION(176);
DEFINE_ISR_FUNCTION(177);
DEFINE_ISR_FUNCTION(178);
DEFINE_ISR_FUNCTION(179);
DEFINE_ISR_FUNCTION(180);
DEFINE_ISR_FUNCTION(181);
DEFINE_ISR_FUNCTION(182);
DEFINE_ISR_FUNCTION(183);
DEFINE_ISR_FUNCTION(184);
DEFINE_ISR_FUNCTION(185);
DEFINE_ISR_FUNCTION(186);
DEFINE_ISR_FUNCTION(187);
DEFINE_ISR_FUNCTION(188);
DEFINE_ISR_FUNCTION(189);
DEFINE_ISR_FUNCTION(190);
DEFINE_ISR_FUNCTION(191);
DEFINE_ISR_FUNCTION(192);
DEFINE_ISR_FUNCTION(193);
DEFINE_ISR_FUNCTION(194);
DEFINE_ISR_FUNCTION(195);
DEFINE_ISR_FUNCTION(196);
DEFINE_ISR_FUNCTION(197);
DEFINE_ISR_FUNCTION(198);
DEFINE_ISR_FUNCTION(199);
DEFINE_ISR_FUNCTION(200);
DEFINE_ISR_FUNCTION(201);
DEFINE_ISR_FUNCTION(202);
DEFINE_ISR_FUNCTION(203);
DEFINE_ISR_FUNCTION(204);
DEFINE_ISR_FUNCTION(205);
DEFINE_ISR_FUNCTION(206);
DEFINE_ISR_FUNCTION(207);
DEFINE_ISR_FUNCTION(208);
DEFINE_ISR_FUNCTION(209);
DEFINE_ISR_FUNCTION(210);
DEFINE_ISR_FUNCTION(211);
DEFINE_ISR_FUNCTION(212);
DEFINE_ISR_FUNCTION(213);
DEFINE_ISR_FUNCTION(214);
DEFINE_ISR_FUNCTION(215);
DEFINE_ISR_FUNCTION(216);
DEFINE_ISR_FUNCTION(217);
DEFINE_ISR_FUNCTION(218);
DEFINE_ISR_FUNCTION(219);
DEFINE_ISR_FUNCTION(220);
DEFINE_ISR_FUNCTION(221);
DEFINE_ISR_FUNCTION(222);
DEFINE_ISR_FUNCTION(223);
DEFINE_ISR_FUNCTION(224);
DEFINE_ISR_FUNCTION(225);
DEFINE_ISR_FUNCTION(226);
DEFINE_ISR_FUNCTION(227);
DEFINE_ISR_FUNCTION(228);
DEFINE_ISR_FUNCTION(229);
DEFINE_ISR_FUNCTION(230);
DEFINE_ISR_FUNCTION(231);
DEFINE_ISR_FUNCTION(232);
DEFINE_ISR_FUNCTION(233);
DEFINE_ISR_FUNCTION(234);
DEFINE_ISR_FUNCTION(235);
DEFINE_ISR_FUNCTION(236);
DEFINE_ISR_FUNCTION(237);
DEFINE_ISR_FUNCTION(238);
DEFINE_ISR_FUNCTION(239);
DEFINE_ISR_FUNCTION(240);
DEFINE_ISR_FUNCTION(241);
DEFINE_ISR_FUNCTION(242);
DEFINE_ISR_FUNCTION(243);
DEFINE_ISR_FUNCTION(244);
DEFINE_ISR_FUNCTION(245);
DEFINE_ISR_FUNCTION(246);
DEFINE_ISR_FUNCTION(247);
DEFINE_ISR_FUNCTION(248);
DEFINE_ISR_FUNCTION(249);
DEFINE_ISR_FUNCTION(250);
DEFINE_ISR_FUNCTION(251);
DEFINE_ISR_FUNCTION(252);
DEFINE_ISR_FUNCTION(253);
DEFINE_ISR_FUNCTION(254);
DEFINE_ISR_FUNCTION(255);

namespace obos
{
	struct idtEntry
	{
		uint16_t offset1;
		uint16_t selector;
		uint8_t ist;
		uint8_t typeAttributes;
		uint16_t offset2;
		uint32_t offset3;
		uint32_t resv1;
	};
	struct idtPointer
	{
		uint16_t size;
		uintptr_t idt;
	} __attribute__((packed));

	extern "C" void idtFlush(idtPointer* idtptr);

	idtEntry g_idtEntries[256];
	idtPointer g_idtPointer = { sizeof(g_idtEntries) - 1, (uintptr_t)&g_idtEntries };
	void(*g_handlers[256])(interrupt_frame* frame);

	enum
	{
		DEFAULT_TYPE_ATTRIBUTE = 0x8E,
		TYPE_ATTRIBUTE_USER_MODE = 0x60
	};

	void _RegisterInterruptInIDT(int n, void(*handler)(), uint8_t typeAttributes)
	{
		utils::memzero(&g_idtEntries[n], sizeof(g_idtEntries[n]));
		g_idtEntries[n].ist = n == 14 || n == 8 || n == 2 || n == 1;
		g_idtEntries[n].typeAttributes = typeAttributes;
		g_idtEntries[n].selector = 0x08; // Kernel-Mode Code Segment
		
		uintptr_t base = (uintptr_t)handler;
		
		g_idtEntries[n].offset1 = base & 0xffff;
		g_idtEntries[n].offset2 = base >> 16;
		g_idtEntries[n].offset3 = base >> 32;
	}
	void RegisterInterruptInIDT(int n, void(*handler)())
	{
		_RegisterInterruptInIDT(n, handler, DEFAULT_TYPE_ATTRIBUTE | TYPE_ATTRIBUTE_USER_MODE);
	}
#define RegisterInterruptInIDT(n) _RegisterInterruptInIDT(n, isr ##n, DEFAULT_TYPE_ATTRIBUTE | TYPE_ATTRIBUTE_USER_MODE)
#define RegisterInterruptInIDT_NoUserMode(n) _RegisterInterruptInIDT(n, isr ##n, DEFAULT_TYPE_ATTRIBUTE)
	void InitializeIDT_CPU()
	{
		idtFlush(&g_idtPointer);
	}
	void InitializeIdt()
	{
		RegisterInterruptInIDT(0);
		RegisterInterruptInIDT(1);
		RegisterInterruptInIDT(2);
		RegisterInterruptInIDT(3);
		RegisterInterruptInIDT(4);
		RegisterInterruptInIDT(5);
		RegisterInterruptInIDT(6);
		RegisterInterruptInIDT(7);
		RegisterInterruptInIDT(8);
		RegisterInterruptInIDT(9);
		RegisterInterruptInIDT(10);
		RegisterInterruptInIDT(11);
		RegisterInterruptInIDT(12);
		RegisterInterruptInIDT(13);
		RegisterInterruptInIDT(14);
		RegisterInterruptInIDT(15);
		RegisterInterruptInIDT(16);
		RegisterInterruptInIDT(17);
		RegisterInterruptInIDT(18);
		RegisterInterruptInIDT(19);
		RegisterInterruptInIDT(20);
		RegisterInterruptInIDT(21);
		RegisterInterruptInIDT(22);
		RegisterInterruptInIDT(23);
		RegisterInterruptInIDT(24);
		RegisterInterruptInIDT(25);
		RegisterInterruptInIDT(26);
		RegisterInterruptInIDT(27);
		RegisterInterruptInIDT(28);
		RegisterInterruptInIDT(29);
		RegisterInterruptInIDT(30);
		RegisterInterruptInIDT(31);
		RegisterInterruptInIDT(32);
		RegisterInterruptInIDT(33);
		RegisterInterruptInIDT(34);
		RegisterInterruptInIDT(35);
		RegisterInterruptInIDT(36);
		RegisterInterruptInIDT(37);
		RegisterInterruptInIDT(38);
		RegisterInterruptInIDT(39);
		RegisterInterruptInIDT(40);
		RegisterInterruptInIDT(41);
		RegisterInterruptInIDT(42);
		RegisterInterruptInIDT(43);
		RegisterInterruptInIDT(44);
		RegisterInterruptInIDT(45);
		RegisterInterruptInIDT(46);
		RegisterInterruptInIDT(47);
		RegisterInterruptInIDT(48);
		RegisterInterruptInIDT_NoUserMode(49);
		RegisterInterruptInIDT(50);
		RegisterInterruptInIDT(51);
		RegisterInterruptInIDT(52);
		RegisterInterruptInIDT(53);
		RegisterInterruptInIDT(54);
		RegisterInterruptInIDT(55);
		RegisterInterruptInIDT(56);
		RegisterInterruptInIDT(57);
		RegisterInterruptInIDT(58);
		RegisterInterruptInIDT(59);
		RegisterInterruptInIDT(60);
		RegisterInterruptInIDT(61);
		RegisterInterruptInIDT(62);
		RegisterInterruptInIDT(63);
		RegisterInterruptInIDT(64);
		RegisterInterruptInIDT(65);
		RegisterInterruptInIDT(66);
		RegisterInterruptInIDT(67);
		RegisterInterruptInIDT(68);
		RegisterInterruptInIDT(69);
		RegisterInterruptInIDT(70);
		RegisterInterruptInIDT(71);
		RegisterInterruptInIDT(72);
		RegisterInterruptInIDT(73);
		RegisterInterruptInIDT(74);
		RegisterInterruptInIDT(75);
		RegisterInterruptInIDT(76);
		RegisterInterruptInIDT(77);
		RegisterInterruptInIDT(78);
		RegisterInterruptInIDT(79);
		RegisterInterruptInIDT(80);
		RegisterInterruptInIDT(81);
		RegisterInterruptInIDT(82);
		RegisterInterruptInIDT(83);
		RegisterInterruptInIDT(84);
		RegisterInterruptInIDT(85);
		RegisterInterruptInIDT(86);
		RegisterInterruptInIDT(87);
		RegisterInterruptInIDT(88);
		RegisterInterruptInIDT(89);
		RegisterInterruptInIDT(90);
		RegisterInterruptInIDT(91);
		RegisterInterruptInIDT(92);
		RegisterInterruptInIDT(93);
		RegisterInterruptInIDT(94);
		RegisterInterruptInIDT(95);
		RegisterInterruptInIDT(96);
		RegisterInterruptInIDT(97);
		RegisterInterruptInIDT(98);
		RegisterInterruptInIDT(99);
		RegisterInterruptInIDT(100);
		RegisterInterruptInIDT(101);
		RegisterInterruptInIDT(102);
		RegisterInterruptInIDT(103);
		RegisterInterruptInIDT(104);
		RegisterInterruptInIDT(105);
		RegisterInterruptInIDT(106);
		RegisterInterruptInIDT(107);
		RegisterInterruptInIDT(108);
		RegisterInterruptInIDT(109);
		RegisterInterruptInIDT(110);
		RegisterInterruptInIDT(111);
		RegisterInterruptInIDT(112);
		RegisterInterruptInIDT(113);
		RegisterInterruptInIDT(114);
		RegisterInterruptInIDT(115);
		RegisterInterruptInIDT(116);
		RegisterInterruptInIDT(117);
		RegisterInterruptInIDT(118);
		RegisterInterruptInIDT(119);
		RegisterInterruptInIDT(120);
		RegisterInterruptInIDT(121);
		RegisterInterruptInIDT(122);
		RegisterInterruptInIDT(123);
		RegisterInterruptInIDT(124);
		RegisterInterruptInIDT(125);
		RegisterInterruptInIDT(126);
		RegisterInterruptInIDT(127);
		RegisterInterruptInIDT(128);
		RegisterInterruptInIDT(129);
		RegisterInterruptInIDT(130);
		RegisterInterruptInIDT(131);
		RegisterInterruptInIDT(132);
		RegisterInterruptInIDT(133);
		RegisterInterruptInIDT(134);
		RegisterInterruptInIDT(135);
		RegisterInterruptInIDT(136);
		RegisterInterruptInIDT(137);
		RegisterInterruptInIDT(138);
		RegisterInterruptInIDT(139);
		RegisterInterruptInIDT(140);
		RegisterInterruptInIDT(141);
		RegisterInterruptInIDT(142);
		RegisterInterruptInIDT(143);
		RegisterInterruptInIDT(144);
		RegisterInterruptInIDT(145);
		RegisterInterruptInIDT(146);
		RegisterInterruptInIDT(147);
		RegisterInterruptInIDT(148);
		RegisterInterruptInIDT(149);
		RegisterInterruptInIDT(150);
		RegisterInterruptInIDT(151);
		RegisterInterruptInIDT(152);
		RegisterInterruptInIDT(153);
		RegisterInterruptInIDT(154);
		RegisterInterruptInIDT(155);
		RegisterInterruptInIDT(156);
		RegisterInterruptInIDT(157);
		RegisterInterruptInIDT(158);
		RegisterInterruptInIDT(159);
		RegisterInterruptInIDT(160);
		RegisterInterruptInIDT(161);
		RegisterInterruptInIDT(162);
		RegisterInterruptInIDT(163);
		RegisterInterruptInIDT(164);
		RegisterInterruptInIDT(165);
		RegisterInterruptInIDT(166);
		RegisterInterruptInIDT(167);
		RegisterInterruptInIDT(168);
		RegisterInterruptInIDT(169);
		RegisterInterruptInIDT(170);
		RegisterInterruptInIDT(171);
		RegisterInterruptInIDT(172);
		RegisterInterruptInIDT(173);
		RegisterInterruptInIDT(174);
		RegisterInterruptInIDT(175);
		RegisterInterruptInIDT(176);
		RegisterInterruptInIDT(177);
		RegisterInterruptInIDT(178);
		RegisterInterruptInIDT(179);
		RegisterInterruptInIDT(180);
		RegisterInterruptInIDT(181);
		RegisterInterruptInIDT(182);
		RegisterInterruptInIDT(183);
		RegisterInterruptInIDT(184);
		RegisterInterruptInIDT(185);
		RegisterInterruptInIDT(186);
		RegisterInterruptInIDT(187);
		RegisterInterruptInIDT(188);
		RegisterInterruptInIDT(189);
		RegisterInterruptInIDT(190);
		RegisterInterruptInIDT(191);
		RegisterInterruptInIDT(192);
		RegisterInterruptInIDT(193);
		RegisterInterruptInIDT(194);
		RegisterInterruptInIDT(195);
		RegisterInterruptInIDT(196);
		RegisterInterruptInIDT(197);
		RegisterInterruptInIDT(198);
		RegisterInterruptInIDT(199);
		RegisterInterruptInIDT(200);
		RegisterInterruptInIDT(201);
		RegisterInterruptInIDT(202);
		RegisterInterruptInIDT(203);
		RegisterInterruptInIDT(204);
		RegisterInterruptInIDT(205);
		RegisterInterruptInIDT(206);
		RegisterInterruptInIDT(207);
		RegisterInterruptInIDT(208);
		RegisterInterruptInIDT(209);
		RegisterInterruptInIDT(210);
		RegisterInterruptInIDT(211);
		RegisterInterruptInIDT(212);
		RegisterInterruptInIDT(213);
		RegisterInterruptInIDT(214);
		RegisterInterruptInIDT(215);
		RegisterInterruptInIDT(216);
		RegisterInterruptInIDT(217);
		RegisterInterruptInIDT(218);
		RegisterInterruptInIDT(219);
		RegisterInterruptInIDT(220);
		RegisterInterruptInIDT(221);
		RegisterInterruptInIDT(222);
		RegisterInterruptInIDT(223);
		RegisterInterruptInIDT(224);
		RegisterInterruptInIDT(225);
		RegisterInterruptInIDT(226);
		RegisterInterruptInIDT(227);
		RegisterInterruptInIDT(228);
		RegisterInterruptInIDT(229);
		RegisterInterruptInIDT(230);
		RegisterInterruptInIDT(231);
		RegisterInterruptInIDT(232);
		RegisterInterruptInIDT(233);
		RegisterInterruptInIDT(234);
		RegisterInterruptInIDT(235);
		RegisterInterruptInIDT(236);
		RegisterInterruptInIDT(237);
		RegisterInterruptInIDT(238);
		RegisterInterruptInIDT(239);
		RegisterInterruptInIDT(240);
		RegisterInterruptInIDT(241);
		RegisterInterruptInIDT(242);
		RegisterInterruptInIDT(243);
		RegisterInterruptInIDT(244);
		RegisterInterruptInIDT(245);
		RegisterInterruptInIDT(246);
		RegisterInterruptInIDT(247);
		RegisterInterruptInIDT(248);
		RegisterInterruptInIDT(249);
		RegisterInterruptInIDT(250);
		RegisterInterruptInIDT(251);
		RegisterInterruptInIDT(252);
		RegisterInterruptInIDT(253);
		RegisterInterruptInIDT(254);
		RegisterInterruptInIDT(255);

		InitializeIDT_CPU();
	}
	void RegisterInterruptHandler(byte interrupt, void(*handler)(interrupt_frame* frame))
	{
		g_handlers[interrupt] = handler;
	}
}