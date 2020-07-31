#ifndef __EFIHELPER_H_
#define __EFIHELPER_H_

#include <stdint.h>
#include <efi.h>
#include <stdbool.h>
#include <stddef.h>

struct SysInfo
{
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info;
	EFI_PHYSICAL_ADDRESS lfb_base_addr;
	void* font;
	uint64_t font_len;
	size_t mapSize;
	size_t descriptorSize;
	EFI_MEMORY_DESCRIPTOR *memoryMap;
	EFI_RUNTIME_SERVICES *RS;
	EFI_SYSTEM_TABLE *ST;
};

extern UINTN size_of_info;
extern EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info;
extern EFI_PHYSICAL_ADDRESS lfb_base_addr;

extern struct SysInfo* sysInfo;

#define INFO "[\033[0;36m INFO \033[0m;] "
#define DONE "[\033[0;32m DONE \033[0m;] "
#define WARN "[\033[0;33m WARN \033[0m;] "
#define ERR "[\033[0;31m ERROR \033[0m;] "
#define NORET "\042[;"

void Panic(const char* s);
void kmsg(const char* s);

#endif // __EFIHELPER_H_