#include <efihelper.h>

#define IDT_ENTRIES 256

#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define mid_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)
#define high_32(address) (uint32_t)(((address) >> 32) & 0xFFFFFFFF)

struct idt_entry {
	uint16_t offset15_0;  uint16_t selector;
	uint8_t  ist;         uint8_t  type_attr;
	uint16_t offset31_16; uint32_t offset63_32;
	uint32_t zero;
} __attribute__((packed));

struct table_ptr {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed));

extern int set_int_handl(uint64_t*);
extern void syscall_test();

void debug_idt();
void setup_idt();