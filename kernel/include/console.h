#include <efihelper.h>
#include <display.h>
#include <psf.h>

#define BLOCK_W 10
#define BLOCK_H 16

extern UINT32 block[BLOCK_W*BLOCK_H];

void InitCon(uint64_t len, struct psf2_header* font);
void setFgCol(int c);
void setBgCol(int c);
void setCursorPos(int x, int y);
void getCursorPos(int *x, int *y);
void PrintChar(char c);
void PrintStr(const char* s);
void PrintInt(uint64_t i, int base);
void clearLine(int y);