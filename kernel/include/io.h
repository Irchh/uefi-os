#ifndef __IO_H_
#define __IO_H_

#include <efi.h>
#include <stdarg.h>

extern EFI_SYSTEM_TABLE *gST;

void printStrW(CHAR16* str);
void printStr(char* str);

char* format(const char* fmt, ...);
char* vformat(const char* fmt, va_list args);

void printf(const char* fmt, ...);
void vprintf(const char* fmt, va_list args);

#endif // __IO_H_