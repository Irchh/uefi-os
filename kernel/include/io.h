#ifndef __IO_H_
#define __IO_H_

#include <efi.h>
#include <stdarg.h>

extern EFI_SYSTEM_TABLE *gST;

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
	uint8_t ret;
	asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
	return ret;
}

void printStrW(CHAR16* str);
void printStr(char* str);

char* format(const char* fmt, ...);
char* vformat(const char* fmt, va_list args);

void printf(const char* fmt, ...);
void vprintf(const char* fmt, va_list args);

#endif // __IO_H_