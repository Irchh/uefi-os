#include <efihelper.h>
#include <io.h>
#include <mem.h>
#include <gdt.h>
#include <idt.h>
#include <string.h>
#include <console.h>
#include <psf.h>

EFI_STATUS kernel_main()
{
	/*char* str = "Test string";
	printf("[%s] %iman\r\n", str, 'o');
	printf("Length of pointer: %i\r\n", __SIZEOF_POINTER__);
	printf("Size of size_t: %i\r\n", sizeof(size_t));

	printf("length: %i\r\n", strlen(""));

	printf("%i\r\n", sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE));

	printf("mapSize: %i, descriptorSize: %i\r\n", mapSize, descriptorSize);*/
	printModes();

	exit_services();
	init_mem_after_bs();

	setup_gdt();

	print_segment_stuff();

	PrintStr("TR: 0x");
	PrintInt(read_tr(), 16);
	PrintStr("\n");

	PrintStr("mapSize: ");
	PrintInt(mapSize, 10);
	PrintStr(", descriptorSize: ");
	PrintInt(descriptorSize, 10);
	PrintStr("\n");

	//drawTriangle(gop_mode_info->HorizontalResolution / 2, gop_mode_info->VerticalResolution / 2, 200, 0x00ff8800);

	//copyBlock(0, 0, gop_mode_info->HorizontalResolution / 2, 0, gop_mode_info->HorizontalResolution / 2, gop_mode_info->VerticalResolution);

	PrintStr("Welcome to my sh!\n");

	PrintStr("/> _");

	while (1);
	return EFI_SUCCESS;
}