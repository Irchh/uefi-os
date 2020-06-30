#include <efihelper.h>
#include <io.h>
#include <mem.h>
#include <gdt.h>
#include <string.h>
#include <console.h>
#include <psf.h>

void PrintStr(const char* s) {
	while (*s) {
		PrintChar(*s);
		s++;
	}
}

void PrintInt(int i, int base) {
	char str[22];
	itoa(i, str, base);
	PrintStr(str);
}

EFI_STATUS kernel_main()
{
	char* str = "Test string";
	printf("[%s] %iman\r\n", str, 'o');
	printf("Length of pointer: %i\r\n", __SIZEOF_POINTER__);
	printf("Size of size_t: %i\r\n", sizeof(size_t));

	printf("length: %i\r\n", strlen(""));

	printf("%i\r\n", sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE));

	printf("mapSize: %i, descriptorSize: %i\r\n", mapSize, descriptorSize);
	printModes();

	exit_services();
	init_mem_after_bs();
	
	//setup_gdt(); // This dies dont use it

	//while (1);

	PrintStr("mapSize: ");
	PrintInt(mapSize, 10);
	PrintStr(", descriptorSize: ");
	PrintInt(descriptorSize, 10);

	//drawTriangle(gop_mode_info->HorizontalResolution / 2, gop_mode_info->VerticalResolution / 2, 200, 0x00ff8800);

	//copyBlock(0, 0, gop_mode_info->HorizontalResolution / 2, 0, gop_mode_info->HorizontalResolution / 2, gop_mode_info->VerticalResolution);

	for (int i = 0; i < 10*10; ++i)
	{
		//PrintStr("Monker du");
	}

	EFI_GRAPHICS_OUTPUT_BLT_PIXEL p;
	p.Red = 255;
	p.Green = 0;
	p.Blue = 0;

	//gRS->ResetSystem(0,0,0,0);

	//printf("Keycode[%i] = '%c'\r\n", Key.ScanCode, (char)Key.UnicodeChar);

	//printf("%c\r\n", 0x1B);

	while (1);
	return EFI_SUCCESS;
}