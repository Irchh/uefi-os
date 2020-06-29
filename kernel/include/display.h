#include <efihelper.h>

void drawPixel(UINTN x, UINTN y, UINT32 color);
UINT32 readPixel(UINTN x, UINTN y);
void fillScreen(UINT32 color);
void drawTriangle(UINTN center_x, UINTN center_y, UINTN width, UINT32 color);
void copyBlock(UINTN x1, UINTN y1, UINTN x2, UINTN y2, UINTN w, UINTN h);
void drawBlock(const UINT32* arr, UINTN x1, UINTN y1, UINTN w, UINTN h);