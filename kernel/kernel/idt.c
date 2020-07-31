#include <idt.h>
#include <syscall.h>
#include <io.h>
#include <console.h>
#include <apic.h>

#pragma pack (1)
struct idt_entry idt[IDT_ENTRIES];
struct table_ptr idtr = {.limit = sizeof(struct idt_entry)*255, .base = (uint64_t)idt};
#pragma pack ()
uint64_t int_handlers[256];

extern void load_idt(struct table_ptr*);
extern void div_by_0();

void debug_idt() {
	PrintStr("Pos of idtr: 0x");
	PrintInt((uint64_t)&idtr, 16);
	PrintStr("\n");

	PrintStr("Pos of idt table: 0x");
	PrintInt((uint64_t)idt, 16);
	PrintStr("\n");

	PrintStr("Pos of int_handlers: 0x");
	PrintInt((uint64_t)int_handlers, 16);
	PrintStr("\n");
}

void setup_idt() {
	kmsg(NORET INFO "Load IDT\r");
	set_int_handl(int_handlers);
	for (int i = 0; i < 256; ++i)
	{
		idt[i].offset15_0 = low_16(int_handlers[i]);
		idt[i].offset31_16 = mid_16(int_handlers[i]);
		idt[i].offset63_32 = high_32(int_handlers[i]);
		idt[i].selector = 0x10; // text segment
		idt[i].ist = 0; // no IST (yet)
		if (i < 32)
			idt[i].type_attr = 0x8f; // Present + trap gate
		else
			idt[i].type_attr = 0x8e; // Present + interrupt gate
		idt[i].zero = 0;
	}

	load_idt(&idtr);
	kmsg(DONE "Load IDT");
}

enum Exception {
	DIVZ = 0,
	DEBUG,
	INVL = 6,
	DOUBLEF = 8,
};

enum Int {
	TICK = 32,
	KEYB = 33,

};

void Print2(const char* s, uint64_t i) {
	PrintStr(s);
	PrintInt(i, 10);
}


uint64_t ticks = 0;
void interrupt_handler(uint64_t n, uint64_t err, uint64_t SP) {
	switch (n) {
		case DIVZ: {
			Panic("Division by zero");
			break;
		}
		case DEBUG: {
			PrintStr("Debug");
			break;
		}
		case DOUBLEF: {
			Panic("Double fault");
			break;
		}
		case INVL: {
			struct reg* regs = (struct reg*)SP;
			uint8_t* inst = (uint8_t*)((uint64_t*)&regs->rax)[3];
			
			if (inst[0] == 0x0F && inst[1] == 0x05) {
				syscall(regs);
			}else
				Panic("Invalid instruction");
			break;
		}
		case TICK: {
			//PrintChar('.');
			ticks++;
			int_end(false);
			break;
		}
		default: {
			PrintStr("INT: ");
			PrintInt(n, 10);
			PrintStr("\n");
			Panic("Unknown interrupt");
		}
	}
}

void general_protection_fault_handler(uint64_t rip, uint64_t err, uint64_t SP) {
	int ext = err&1;
	int table = (err&6)>>1;
	int index = (err&0xFFF8)>>4;

	PrintStr("General Protection fault!\n");
	PrintStr("RIP: 0x");
	PrintInt(rip, 16);
	PrintStr("\n");
	if (ext)
		PrintStr("External fault\n");
	else
		PrintStr("Internal fault\n");
	if (table == 0) PrintStr("Error in GDT\n");
	else if (table == 1 || table == 3) PrintStr("Error in IDT\n");
	else if (table == 2) PrintStr("Error in LDT\n");

	PrintStr("Error index: 0x");
	PrintInt(index, 16);

	PrintStr("\nHalting!\n");
	while(1) asm("hlt");
}

void pagefault_handler(uint64_t n, uint64_t err, uint64_t SP) {
	PrintStr("PAGEFAULT: ");
	PrintInt(err, 10);
	PrintStr("\nHalting!\n");
	while(1) asm("hlt");
}

#include <keyboard.h>
void keyboard_handler(uint64_t n, uint64_t key, uint64_t SP) {
	KeyPressed(key);
	if (!(key&128))
		AddKey(key);
	int_end(false);
}