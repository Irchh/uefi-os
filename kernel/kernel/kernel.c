#include <efihelper.h>
#include <io.h>
#include <mem.h>
#include <gdt.h>
#include <idt.h>
#include <apic.h>
#include <string.h>
#include <console.h>
#include <psf.h>

__attribute__((noreturn)) void goto_user_space();

void Panic(const char* s) {
	PrintStr("Panic: ");
	PrintStr(s);
	PrintStr("! Halting CPU.\n");
	while(1);
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

EFI_STATUS kernel_main()
{
	kmsg(INFO "Kernel init");
	//printModes();

	exit_services();
	init_mem_after_bs();

	setup_gdt();

	//print_segment_stuff();

	PrintStr("TR: 0x");
	PrintInt(read_tr(), 16);
	PrintStr("\n");

	PrintStr("mapSize: ");
	PrintInt(mapSize, 10);
	PrintStr(", descriptorSize: ");
	PrintInt(descriptorSize, 10);
	PrintStr("\n");


	uint8_t	*startOfMemoryMap = (uint8_t *)memoryMap;
	uint8_t *endOfMemoryMap = startOfMemoryMap + mapSize;

	uint8_t *offset = startOfMemoryMap;

	uint32_t counter = 0;
	uint64_t totalPages = 0;

	EFI_MEMORY_DESCRIPTOR *desc = NULL;


	while(offset < endOfMemoryMap)
	{
		desc = (EFI_MEMORY_DESCRIPTOR *)offset;

		totalPages += desc->NumberOfPages;

		offset += descriptorSize;
		counter++;
	}

	// eventually: (totalPages)*4096
	int memSize = desc->PhysicalStart+(desc->NumberOfPages*4096);
	kmsg(NORET INFO "Memory size: ");
	PrintInt(memSize/1048576, 10);
	PrintStr(" MiB\n");

	setup_idt();

	debug_idt();

	setup_pic();

	kmsg( INFO "TEST");
	kmsg( WARN "TEST");
	kmsg( ERR "TEST");
	kmsg( DONE "TEST");

	syscall_test();

	PrintStr("Welcome to my sh!\n");

	PrintStr("/> _");

	//goto_user_space();

	while (1);
	return EFI_SUCCESS;
}