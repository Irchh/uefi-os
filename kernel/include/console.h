#include <efihelper.h>
#include <display.h>

#define BLOCK_W 10
#define BLOCK_H 16

extern UINT32 block[BLOCK_W*BLOCK_H];

void InitCon(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info);
void setFgCol(int c);
void setBgCol(int c);
void PrintChar(char c);
void PrintStr(const char* s);
void PrintInt(uint64_t i, int base);