#include <efihelper.h>
#include <io.h>
#include <mem.h>
#include <console.h>
#include <keyboard.h>

EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info;
EFI_PHYSICAL_ADDRESS lfb_base_addr;

struct SysInfo* sysInfo = NULL;

uint64_t rsp;
uint64_t rbp;
EFI_STATUS init_efi(struct SysInfo *sys_info, uint64_t RSP, uint64_t RBP) {
	rsp = RSP;
	rbp = RBP;
	sysInfo = sys_info;

	gop_mode_info = sys_info->gop_mode_info;
	lfb_base_addr = sys_info->lfb_base_addr;

	InitCon(sys_info->font_len, sys_info->font);
	InitKeyboard();

	kmsg(INFO "InitCon");

	return EFI_SUCCESS;
}