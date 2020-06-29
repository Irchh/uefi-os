#include <display.h>

void drawPixel(UINTN x, UINTN y, UINT32 color) {
	UINT32* at = (UINT32*)lfb_base_addr;
	at[(gop_mode_info->PixelsPerScanLine)*y+x] = color;
}

UINT32 readPixel(UINTN x, UINTN y) {
	UINT32* at = (UINT32*)lfb_base_addr;
	return at[(gop_mode_info->PixelsPerScanLine)*y+x];
}

void drawPixel24(UINTN x, UINTN y, UINT32 color) {
	UINT8* at = (UINT8*)lfb_base_addr;
	at += ((gop_mode_info->PixelsPerScanLine)*y+x)*3;
	at[0] = (color>>8)&0xFF;
	at[1] = (color>>4)&0xFF;
	at[2] = color&0xFF;
}

void fillScreen(UINT32 color) {
	for (int i = 0; i < gop_mode_info->HorizontalResolution; ++i)
		for (int j = 0; j < gop_mode_info->VerticalResolution; ++j)
			drawPixel(i, i, color);
}

void drawTriangle(UINTN center_x, UINTN center_y, UINTN width, UINT32 color) {
	UINT32* at = (UINT32*)lfb_base_addr;
	UINTN row, col;

	at += (gop_mode_info->PixelsPerScanLine * (center_y - width / 2) + center_x - width / 2);

	for (row = 0; row < width / 2; row++) {
		for (col = 0; col < width - row * 2; col++)
			*at++ = color;
		at += (gop_mode_info->PixelsPerScanLine - col);
		for (col = 0; col < width - row * 2; col++)
			*at++ = color;
		at += (gop_mode_info->PixelsPerScanLine - col + 1);
	}
}

void copyBlock(UINTN x1, UINTN y1, UINTN x2, UINTN y2, UINTN w, UINTN h) {
	UINT32* at = (UINT32*)lfb_base_addr;
	for (int x = 0; x < w; ++x)
		for (int y = 0; y < h; ++y)
			drawPixel(x+x2, y+y2, readPixel(w-(x+x1), y+y1));
}

void drawBlock(const UINT32* arr, UINTN x1, UINTN y1, UINTN w, UINTN h) {
	for (int x = 0; x < w; ++x)
		for (int y = 0; y < h; ++y)
			drawPixel(x+x1, y+y1, arr[x+w*y]);
}