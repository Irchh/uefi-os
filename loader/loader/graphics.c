#include <graphics.h>
#include <efi.h>
#include <stdbool.h>
#include <loader.h>

extern void bdgprnt(CHAR16* s);

EFI_GRAPHICS_OUTPUT_PROTOCOL *gGP = NULL;
UINTN size_of_info;
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info;
EFI_PHYSICAL_ADDRESS lfb_base_addr = 0;

UINTN handleCount;
EFI_HANDLE *handleBuffer;

static EFI_GUID GraphicsOutputProtocolGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

void init_gfx() {
	EFI_STATUS status;
	if (gGP == NULL) {
		bdgprnt(L"LocateHandleBuffer\r\n");
		status = gBS->LocateHandleBuffer(
						ByProtocol,
						&GraphicsOutputProtocolGUID,
						NULL,
						&handleCount,
						&handleBuffer);
		bdgprnt(L"LocateHandleBuffer end\r\n");
		if (EFI_ERROR(status)) {
			bdgprnt(L"error: LocateHandleBuffer() failed\r\n");
			while(1);
		}
		status = gBS->HandleProtocol(
						handleBuffer[0], &GraphicsOutputProtocolGUID, (void**)&gGP);
		if (EFI_ERROR(status) || gGP == NULL) {
			bdgprnt(L"error: HandleProtocol() failed\r\n");
			while(1);
		}
	}

	UINTN mode_num;
	/*
	UINTN size_of_info;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info;
	for (mode_num = 0;
		 (status = gGP->QueryMode( gGP, mode_num, &size_of_info, &gop_mode_info )) == EFI_SUCCESS;
		 mode_num++) {
		if (gop_mode_info->HorizontalResolution == 1920 &&
			  gop_mode_info->VerticalResolution == 1080 &&
			  gop_mode_info->PixelFormat        == PixelBlueGreenRedReserved8BitPerColor)
			break;
	}

	if (status != EFI_SUCCESS)//*/
		mode_num = gGP->Mode->Mode;

	status = gGP->SetMode(gGP, mode_num);
	if (EFI_ERROR(status)) {
		bdgprnt(L"error: failed to set default mode\r\n");
	}

	//bdgprnt(L"Current mode: %i\r\n\r", gGP->Mode->Mode);

	gGP->QueryMode(gGP, gGP->Mode->Mode, &size_of_info, &gop_mode_info);
	lfb_base_addr = gGP->Mode->FrameBufferBase;
}