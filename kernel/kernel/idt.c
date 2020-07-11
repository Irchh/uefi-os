#include <idt.h>

#pragma pack (1)
struct idt_entry idt[IDT_ENTRIES];
struct table_ptr idtr = {.limit = 255, .base = (uint64_t)idt};
#pragma pack ()
uint64_t int_handlers[256];

void setup_idt() {
	set_int_handl();
	for (int i = 0; i < 256; ++i)
	{
		idt[i].offset15_0 = low_16(int_handlers[i]);
		idt[i].offset31_16 = mid_16(int_handlers[i]);
		idt[i].offset63_32 = high_32(int_handlers[i]);
		idt[i].selector = 0x8; // text segment
		idt[i].ist = 0; // no IST (yet)
		idt[i].type_attr = 0x8e; // Present + interrupt gate
	}
}

void interrupt_handler() {

}

void general_protection_fault_handler() {

}

void pagefault_handler() {

}

void keyboard_handler() {

}