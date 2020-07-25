#include <apic.h>

bool check_apic() {
	uint32_t eax, edx;
	//cpuid(1, &eax, &edx);
	//return edx & CPUID_FEAT_EDX_APIC;
	return false;
}

// Legacy PIC
#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

void setup_pic() {
	kmsg(INFO "Configuring PIC");
	asm("cli");
	remap_pic(0x20, 0x28); // 32, 40
	mask_pic();
	asm("sti");
	kmsg(DONE "Configured PIC");
}

void remap_pic(int offset1, int offset2) {
	unsigned char a1, a2;
	a1 = inb(PIC1_D);
	a2 = inb(PIC2_D);

	outb(PIC1_C, ICW1_INIT | ICW1_ICW4);
	outb(PIC2_C, ICW1_INIT | ICW1_ICW4);

	outb(PIC1_D, offset1);
	outb(PIC2_D, offset2);

	outb(PIC1_D, 0x04);
	outb(PIC2_D, 0x02);

	outb(PIC1_D, 0x01);
	outb(PIC2_D, 0x01);

	outb(PIC1_D, a1);   // restore saved masks.
	outb(PIC2_D, a2);
}

void IRQ_set_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_D;
    } else {
        port = PIC2_D;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}
 
void IRQ_clear_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_D;
    } else {
        port = PIC2_D;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}

void mask_pic() {
	outb(PIC1_D, 0xFC);
	outb(PIC2_D, 0xFF);
}

void int_end(bool both) {
	if (both)
		outb(PIC2_C, 0x20);
	outb(PIC1_C, 0x20);
}