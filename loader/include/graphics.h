#include <stdint.h>
#include <efi.h>
extern EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info;
extern EFI_PHYSICAL_ADDRESS lfb_base_addr;

void init_gfx();