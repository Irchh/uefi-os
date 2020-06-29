#ifndef __EFIHELPER_H_
#define __EFIHELPER_H_

#include <stdint.h>
#include <efi.h>
#include <stdbool.h>

extern EFI_HANDLE gIH;
extern EFI_SYSTEM_TABLE *gST;
extern EFI_BOOT_SERVICES *gBS;
extern EFI_RUNTIME_SERVICES *gRS;

extern EFI_GRAPHICS_OUTPUT_PROTOCOL *gGP;

extern UINTN size_of_info;
extern EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info;
extern EFI_PHYSICAL_ADDRESS lfb_base_addr;

extern bool init;
extern bool exit;

EFI_STATUS init_efi(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);
void init_gfx();
EFI_STATUS Init();
EFI_STATUS exit_services();
void printModes();
UINTN load_file_efi(CHAR16* file, void** dest);

#endif // __EFIHELPER_H_