#include <gdt.h>
#include <console.h>

#pragma pack (1)

__attribute__((aligned(4096)))
struct tss tss;
__attribute__((aligned(4096)))
struct gdt_table gdt_table = {
	{0, 0, 0, 0x00, 0x00, 0},  /* 0x00 null  */
	{0, 0, 0, 0x92, 0xa0, 0},  /* 0x08 kernel data */
	{0, 0, 0, 0x9a, 0xa0, 0},  /* 0x10 kernel code (kernel base selector) */
	{0, 0, 0, 0x00, 0x00, 0},  /* 0x18 null (user base selector) */
	{0, 0, 0, 0xf2, 0xa0, 0},  /* 0x20 user data */
	{0, 0, 0, 0xfa, 0xa0, 0},  /* 0x28 user code */
	{0, 0, 0, 0x92, 0xa0, 0},  /* 0x30 ovmf data */
	{0, 0, 0, 0x9a, 0xa0, 0},  /* 0x38 ovmf code */
	{0, 0, 0, 0x89, 0xa0, 0},  /* 0x40 tss low */
	{0, 0, 0, 0x00, 0x00, 0},  /* 0x48 tss high */
};

struct table_ptr {
	uint16_t limit;
	uint64_t base;
} gdt_ptr;

#pragma pack ()

void print_segment_stuff() {
	struct gdt_entry* gdt_ent = (struct gdt_entry*)&gdt_table;
	for (int i = 0; i < gdt_ptr.limit/8+1; ++i)
	{
		UINT32 base = gdt_ent[i].base15_0 + (gdt_ent[i].base23_16<<16) + (gdt_ent[i].base31_24<<24);
		UINT32 limit = gdt_ent[i].limit15_0 + ((gdt_ent[i].limit19_16_and_flags&0xff00)<<16);
		PrintChar('[');
		PrintInt(i,10);
		PrintStr("] base: 0x");
		PrintInt(base,16);
		PrintChar('\n');

		PrintChar('[');
		PrintInt(i,10);
		PrintStr("] limit: 0x");
		PrintInt(limit,16);
		PrintChar('\n');

		PrintChar('[');
		PrintInt(i,10);
		PrintStr("] type: 0x");
		PrintInt(gdt_ent[i].type,16);
		PrintChar('\n');
	}
}

extern void load_gdt(struct table_ptr* gdt_ptr);
extern uint64_t read_rsp();

void memzero(void* s, uint64_t n) {
	for (int i = 0; i < n; ++i) ((uint8_t*)s)[i] = 0;
}

void setup_tss(struct tss* t, struct gdt_entry* tlo, struct gdt_entry* thi) {
	memzero((void*)t, sizeof(*t));
	uint64_t tss_base = ((uint64_t)t);
	tlo->base15_0 = tss_base & 0xffff;
	tlo->base23_16 = (tss_base >> 16) & 0xff;
	tlo->base31_24 = (tss_base >> 24) & 0xff;
	tlo->limit15_0 = sizeof(*t);
	thi->limit15_0 = (tss_base >> 32) & 0xffff;
}

void setup_gdt() {
	kmsg(NORET INFO "Load GDT\r");
	setup_tss(&tss, &gdt_table.tss_low, &gdt_table.tss_high);

	gdt_ptr = (struct table_ptr){ sizeof(gdt_table)-1, (UINT64)&gdt_table };
	load_gdt(&gdt_ptr);

	tss.rsp0 = read_rsp();
	tss.rsp1 = tss.rsp0;
	tss.rsp2 = tss.rsp0;
	kmsg(DONE "Load GDT");
}