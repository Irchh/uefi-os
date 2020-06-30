#include <console.h>
#include <io.h>
#include <psf.h>
#include <mem.h>
#include <limits.h>

UINT32 fontW = 0;
UINT32 fontH = 0;

UINT32 termW = 0;
UINT32 termH = 0;
UINT32 termX = 0;
UINT32 termY = 0;

struct psf2_header* psf2_font = NULL;
struct psf1_header* psf1_font = NULL;
UINTN headersize = 0;
UINTN charsize = 0;
UINTN charlength = 0;
UINT16* unicode;

void InitCon(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info) {
	UINTN font_len = load_file_efi(L"EFI\\BOOT\\MyOS\\UbuntuMono-R-8x16.psf", (void*)&psf2_font);
	printf("LENGTH: %i\n", font_len);
	psf1_font = (struct psf1_header*)psf2_font;
	if (PSF2_MAGIC_OK(psf2_font->magic)) {
		printf("PSF2\r\n");
		fontW = psf2_font->width;
		fontH = psf2_font->height;
		headersize = psf2_font->headersize;
		charsize = psf2_font->charsize;
		charsize = fontH * ((fontW + 7) / 8);
		charlength = psf2_font->length;

		UINT16 glyph = 0;
		if (psf2_font->flags) {
			char* s = (char*)(
				(unsigned char*)psf2_font +
				psf2_font->headersize +
				psf2_font->length * psf2_font->charsize
			);
			char* psf2_font_end = (char*)((unsigned char*)psf2_font)+font_len;
			unicode = malloc(USHRT_MAX*2);
			for (int i = 0; i < USHRT_MAX*2; ++i)
				unicode[i] = 0;
			while (s<psf2_font_end) {
				UINT16 uc = (UINT16)((unsigned char*)s)[0];
				if (uc == 0xFF) {
					glyph++;
					s++;
					continue;
				} else if(uc & 128) {
					if ((uc & 32) == 0) {
						uc = ((s[0] & 0x1F)<<6)+(s[1] & 0x3F);
						s++;
					} else
					if((uc & 16) == 0 ) {
						uc = ((((s[0] & 0xF)<<6)+(s[1] & 0x3F))<<6)+(s[2] & 0x3F);
						s+=2;
					} else
					if((uc & 8) == 0 ) {
						uc = ((((((s[0] & 0x7)<<6)+(s[1] & 0x3F))<<6)+(s[2] & 0x3F))<<6)+(s[3] & 0x3F);
						s+=3;
					} else {
						uc = 0;
					}
				}
				unicode[uc] = glyph;
				s++;
			}
		} else {
			unicode = NULL;
		}
	}
	else if (PSF1_MAGIC_OK(psf1_font->magic)) {
		printf("PSF1\r\n");
		fontW = 8;
		fontH = psf1_font->charsize;
		headersize = sizeof(struct psf1_header);
		charsize = psf1_font->charsize;

		if (psf1_font->mode%PSF1_MODE512) charlength = 512;
		else charlength = 256;
	}

	termW = gop_mode_info->HorizontalResolution/fontW;
	termH = gop_mode_info->VerticalResolution/fontH;
}

int abs(int n) {
	if (n < 0)
		return -n;
	else
		return n;
}

void drawDots(n, off) {
	for (int i = 0; i < n; ++i)
		drawPixel(300 + off, 300+i*2, 0x00FF0000);
}

void drawPsfChar(char c, UINTN cx, UINTN cy) {
	int bytesperline = charsize/fontH;

	if (unicode != NULL) c = unicode[(int)c];

	UINT8* glyph = (UINT8*)psf2_font+headersize;
	glyph += (c>0&&c<charlength?c:0)*charsize;

	cx *= fontW;
	cy *= fontH;

	UINTN line, mask;
	for (int y = 0; y < fontH; ++y) {
		int offset = 0;
		for (int b = 0; b < bytesperline; ++b)
		{
			int bits = b == bytesperline-1 ? fontW%8 : 8;
			if (bits == 0) bits = 8;
			mask = 1<<(bits-1);

			for (int x = 0; x < bits; ++x) {
				int pix = ((UINT8)glyph[0]>>(8-bits)) & (mask);
				if (pix)
					drawPixel(x+cx+offset, y+cy, 0x00FFFFFF);
				else
					drawPixel(x+cx+offset, y+cy, 0x00000000);
				mask >>= 1;
			}
			offset += bits;
			glyph++;
		}
	}
}

void PrintChar(char c) {
	switch (c) {
		case '\n': {
			// TODO: Scroll screen
			if (++termY == termH)
				termY = 0;
			termX = 0;
			break;
		}
		case '\r': { termX = 0; break; }
		case '\b': { if (termX > 0) --termX; break; }
		default: {
			drawPsfChar(c, termX, termY);
			if (++termX == termW) {
				termX = 0;
				if (++termY == termH)
					termY = 0;
			}
		}
	}
}