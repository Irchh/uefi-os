#include <efihelper.h>

#pragma pack (1)

struct gdt_entry {
	uint16_t limit15_0;            uint16_t base15_0;
	uint8_t  base23_16;            uint8_t  type;
	uint8_t  limit19_16_and_flags; uint8_t  base31_24;
} __attribute__((packed));

struct tss {
	uint32_t reserved0; uint64_t rsp0;      uint64_t rsp1;
	uint64_t rsp2;      uint64_t reserved1; uint64_t ist1;
	uint64_t ist2;      uint64_t ist3;      uint64_t ist4;
	uint64_t ist5;      uint64_t ist6;      uint64_t ist7;
	uint64_t reserved2; uint16_t reserved3; uint16_t iopb_offset;
} __attribute__((packed));

struct gdt_table {
	struct gdt_entry null;
	struct gdt_entry kernel_code;
	struct gdt_entry kernel_data;
	struct gdt_entry null2;
	struct gdt_entry user_data;
	struct gdt_entry user_code;
	struct gdt_entry ovmf_data;
	struct gdt_entry ovmf_code;
	struct gdt_entry tss_low;
	struct gdt_entry tss_high;
} __attribute__((packed));

extern struct tss tss;
extern struct gdt_table gdt_table;

uint64_t read_tr();
void setup_gdt();
void print_segment_stuff();

#pragma pack ()