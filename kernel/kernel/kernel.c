#include <efihelper.h>
#include <io.h>
#include <mem.h>
#include <gdt.h>
#include <idt.h>
#include <apic.h>
#include <string.h>
#include <console.h>
#include <keyboard.h>
#include <psf.h>

__attribute__((noreturn)) void goto_user_space();

void Panic(const char* s) {
	PrintStr("Panic: ");
	PrintStr(s);
	PrintStr("! Halting CPU.\n");
	while(1) asm("hlt");
}

int cols[6] = {
	0xFF0000, // RED
	0x00FF00, // GREEN
	0xFFFF00, // YELLOW
	0x0000FF, // BLUE
	0xFF00FF, // MAGENTA
	0x00FFFF, // ORANGE
};

void kmsg(const char* s) {
	int noret = 0;
	if (s[0] == '\042' && s[1] == '[' && s[2] == ';') {
		noret = 1;
		s += 3;
	}
	while (*s) { 
		if (s[0] == '\033' && s[1] == '[') {
			if (s[3] == 'm' && s[4] == ';') {
				setFgCol(0xFFFFFF);
				s += 4;
			}
			else if (s[3] == ';' && s[4] == '3' && s[6] == 'm' && (s[5] > '0' && s[5] < '7')) {
				setFgCol(cols[s[5]-'1']);
				s += 6;
			} else {
				PrintChar('^');
			}
		} else if (s[0] == '\033') {
			PrintChar('^');
		} else {
			PrintChar(*s);
		}
		s++;
	}
	if (!noret)
		PrintChar('\n');
}

#define RSDP1 (EFI_GUID){.Data1 = 0xeb9d2d30, .Data2 = 0x2d88, .Data3 = 0x11d3, .Data4 = {0x9a, 0x16, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}}
#define RSDP1_D1 0xeb9d2d30
#define RSDP1_D2 0x2d88
#define RSDP1_D3 0x11d3
#define RSDP1_D4 (uint8_t[8]){0x9a, 0x16, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}
// 8868e871-e4f1-11d3-bc22-0080c73c8881
#define RSDP2_D1 0x8868e871
#define RSDP2_D2 0xe4f1
#define RSDP2_D3 0x11d3
#define RSDP2_D4 (uint8_t[8]){0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81}

void* find_rsdp1() {
	EFI_CONFIGURATION_TABLE* ect = sysInfo->ST->ConfigurationTable;
	uint64_t len = sysInfo->ST->NumberOfTableEntries;
	for (int i = 0; i < len; ++i)
	{
		if (ect[i].VendorGuid.Data1 == RSDP1_D1 && ect[i].VendorGuid.Data2 == RSDP1_D2 && ect[i].VendorGuid.Data3 == RSDP1_D3) {
			for (int i = 0; i < 8; ++i)
			{
				PrintStr("ACPI 1.0: 0x");
				PrintInt(ect[i].VendorGuid.Data4[i], 16);
				PrintStr(" == 0x");
				PrintInt(RSDP1_D4[i], 16);
				PrintChar('\n');
			}
			kmsg(INFO "Found ACPI 1.0 RSDP");
			return ect[i].VendorTable;
		}
	}
	return NULL;
}

void* find_rsdp2() {
	EFI_CONFIGURATION_TABLE* ect = sysInfo->ST->ConfigurationTable;
	uint64_t len = sysInfo->ST->NumberOfTableEntries;
	int found = 0;
	for (int i = 0; i < len; ++i)
	{
		if (ect[i].VendorGuid.Data1 == RSDP2_D1 && ect[i].VendorGuid.Data2 == RSDP2_D2 && ect[i].VendorGuid.Data3 == RSDP2_D3) {
			found = 1;
			for (int i = 0; i < 8; ++i)
			{
				PrintStr("ACPI 2.0: 0x");
				PrintInt(ect[i].VendorGuid.Data4[i], 16);
				PrintStr(" == 0x");
				PrintInt(RSDP2_D4[i], 16);
				PrintChar('\n');
			}
			kmsg(INFO "Found ACPI 2.0 RSDP");
			return ect[i].VendorTable;
		}
	}
	return NULL;
}

void* find_rsdp(int *ver) {
	void* rsdp_table = find_rsdp2();
	int vers = 0;
	if (!ver) ver = &vers;
	*ver = 1;
	if (!rsdp_table)
		rsdp_table = find_rsdp1();
	else *ver = 2;

	return rsdp_table;
}

extern uint64_t ticks;

uint64_t read_pstruct(struct PML4T *pml4t, uint8_t* index, int level) {
	if (index == 0 | level > 2)
		return 0;

	uint64_t value = 0;
	struct PDPT* pdpt = NULL;
	struct PDT* pdt = NULL;
	struct PT* pt = NULL;

	switch (level) {
		case 0:
			pdpt = (struct PDPT*)((uint64_t)pml4t->pdpt&(~0xFFF0000000000FFF));
		case 1:
			pdt = (struct PDT*)((uint64_t)&pdpt->pdt&(~0xFFF0000000000FFF));
		case 2:
			pt = (struct PT*)((uint64_t)&pdt->pt&(~0xFFF0000000000FFF));
			break;
		default:
			return 0;
	}

	if (level == 0)
		value = (uint64_t)&pdpt[index[0]]&(~0xFFF0000000000FFF);
	else if (level == 1)
		value = (uint64_t)&pdt[index[1]]&(~0xFFF0000000000FFF);
	else if (level == 2)
		value = (uint64_t)&pt[index[2]]&(~0xFFF0000000000FFF);
	return value;
}

EFI_STATUS kernel_main()
{
	kmsg(INFO "Kernel init");
	//printModes();

	init_mem_after_bs(400000);

	setup_gdt();

	//print_segment_stuff();

	PrintStr("TR: 0x");
	PrintInt(read_tr(), 16);
	PrintStr("\n");

	PrintStr("mapSize: ");
	PrintInt(sysInfo->mapSize, 10);
	PrintStr(", descriptorSize: ");
	PrintInt(sysInfo->descriptorSize, 10);
	PrintStr("\n");


	uint8_t	*startOfMemoryMap = (uint8_t *)sysInfo->memoryMap;
	uint8_t *endOfMemoryMap = startOfMemoryMap + sysInfo->mapSize;

	uint8_t *offset = startOfMemoryMap;

	uint64_t totalPages = 0;

	EFI_MEMORY_DESCRIPTOR *desc = NULL;

	while(offset < endOfMemoryMap)
	{
		desc = (EFI_MEMORY_DESCRIPTOR *)offset;

		totalPages += desc->NumberOfPages;

		offset += sysInfo->descriptorSize;
	}

	// (totalPages)*4096 could also work
	//uint64_t memSize = desc->PhysicalStart+(desc->NumberOfPages*4096);
	uint64_t memSize = (totalPages)*4096;
	//memSize = 4*(memSize<<3);
	int pages = memSize/0x1000;
	setup_pfa(pages);
	kmsg(NORET INFO "Memory size: ");
	PrintInt(memSize/1048576, 10);
	PrintStr(" MiB, ");
	PrintInt(pages, 10);
	PrintStr(" Pages\n");

	PrintStr("First free frame: "); PrintInt(first_free_frame(), 10);
	PrintStr(", addr: 0x"); PrintInt(pf2addr(first_free_frame()), 16);
	PrintStr(", end: 0x"); PrintInt(pf2addr(first_free_frame())+0xFFF, 16);
	PrintChar('\n');

	setup_idt();

	debug_idt();

	if (!setup_apic()) {
		kmsg(WARN "APIC not supported, using legacy PIC.");
		setup_pic();
	}

	int ver = 0;
	void* rsdp = find_rsdp(&ver);

	kmsg( NORET INFO "RSDP VER ");
	PrintInt(ver, 10);
	PrintStr(". RSDP = 0x");
	PrintInt((uint64_t)rsdp, 16);
	PrintChar('\n');

	PrintStr("Size of struct PML4T: ");
	PrintInt(sizeof(struct PML4T), 10);
	PrintChar('\n');

	struct PML4T *pml4t = (struct PML4T*)(read_cr3()&(~0xFFF));

	setup_paging(pml4t);
	first_n_pages(20);

	syscall_test();

	//goto_user_space();
	int8_t level = 0;
	uint8_t index[4];
	for (int i = 0; i < 4; ++i) index[i] = 0;
	uint64_t oldTicks = 0;
	while (1) {
		char key = ReadKey();
		if (key) {
			switch (key) {
				case 72: { index[level]--; break; }
				case 80: { index[level]++; break; }
				case 75: { level--; break; }
				case 77: { level++; break; }
				default: {
					char c = FromKeyCode(key);
					if (c)
						PrintChar(c);
					goto endif;
				}
			}
			if (level < 0) level = 0;
			else if (level > 2) level = 2;

			uint64_t value = read_pstruct(pml4t, index, level);
			
			int x = 0;
			int y = 0;
			getCursorPos(&x, &y);

			clearLine(y);
			PrintStr("\rValue: 0x"); PrintInt(value, 16);
			setCursorPos(26, y);
			PrintStr("Index: "); PrintInt(index[level], 10);
			setCursorPos(37, y);
			PrintStr("Level: "); PrintInt(level, 10);

			setCursorPos(x, y);
endif:;
			//PrintStr("Key: ");
			//PrintInt(key, 10);
			//PrintChar('\n');
		}
		if (!((int64_t)(ticks-oldTicks) < 1 && oldTicks != 0)) {
			oldTicks = ticks;
			int x = 0;
			int y = 0;
			getCursorPos(&x, &y);
			setCursorPos(80, 0);
			PrintStr("Seconds: ");
			PrintInt(ticks/100, 10);
			setCursorPos(x, y);
		}
	}
	return EFI_SUCCESS;
}