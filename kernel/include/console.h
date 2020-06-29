#include <efihelper.h>
#include <display.h>

#define BLOCK_W 10
#define BLOCK_H 16

extern UINT32 block[BLOCK_W*BLOCK_H];

void InitCon(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info);
void PrintChar(char c);