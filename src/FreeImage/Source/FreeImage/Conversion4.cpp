// ==========================================================
// Bitmap conversion routines
//
// Design and implementation by
// - Riley McNiff (rmcniff@marexgroup.com)
//
// This file is part of FreeImage 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================

#include "FreeImage.h"
#include "Utilities.h"

// ----------------------------------------------------------
//  internal conversions X to 4 bits
// ----------------------------------------------------------

void DLL_CALLCONV
FreeImage_ConvertLine1To4(uint8_t *target, uint8_t *source, int width_in_pixels) {
	FIBOOL hinibble = TRUE;
	for (int cols = 0; cols < width_in_pixels; cols++){
		if (hinibble == TRUE){
			target[cols >> 1] = ((source[cols >> 3] & (0x80 >> (cols & 0x07))) != 0 ? 15 : 0) << 4;
		} 
		else {
			target[cols >> 1] |= ((source[cols >> 3] & (0x80 >> (cols & 0x07))) != 0 ? 15 : 0);
		}

		hinibble = !hinibble;
	}
}

void DLL_CALLCONV
FreeImage_ConvertLine8To4(uint8_t *target, uint8_t *source, int width_in_pixels, FIRGBA8 *palette) {
	FIBOOL hinibble = TRUE;
	uint8_t index;

	for (int cols = 0; cols < width_in_pixels; cols++){
		index = GREY(palette[source[cols]].red, palette[source[cols]].green, palette[source[cols]].blue);
		if (hinibble) {
			target[cols >> 1] = (index & 0xF0);
		} else {
			target[cols >> 1] |= (index >> 4);
		}

		hinibble = !hinibble;
	}
}

void DLL_CALLCONV
FreeImage_ConvertLine16To4_555(uint8_t *target, uint8_t *source, int width_in_pixels) {
	uint16_t *bits = (uint16_t *)source;
	FIBOOL hinibble = TRUE;

	for (int cols = 0; cols < width_in_pixels; cols++) {
		if (hinibble) {
			target[cols >> 1] = GREY((((bits[cols] & FI16_555_RED_MASK) >> FI16_555_RED_SHIFT) * 0xFF) / 0x1F,
								(((bits[cols] & FI16_555_GREEN_MASK) >> FI16_555_GREEN_SHIFT) * 0xFF) / 0x1F,
								(((bits[cols] & FI16_555_BLUE_MASK) >> FI16_555_BLUE_SHIFT) * 0xFF) / 0x1F)
								& 0xF0;
		} else {
			target[cols >> 1] |= GREY((((bits[cols] & FI16_555_RED_MASK) >> FI16_555_RED_SHIFT) * 0xFF) / 0x1F,
								(((bits[cols] & FI16_555_GREEN_MASK) >> FI16_555_GREEN_SHIFT) * 0xFF) / 0x1F,
								(((bits[cols] & FI16_555_BLUE_MASK) >> FI16_555_BLUE_SHIFT) * 0xFF) / 0x1F)
								>> 4;
		}
		
		hinibble = !hinibble;
	}
}

void DLL_CALLCONV
FreeImage_ConvertLine16To4_565(uint8_t *target, uint8_t *source, int width_in_pixels) {
	uint16_t *bits = (uint16_t *)source;
	FIBOOL hinibble = TRUE;

	for (int cols = 0; cols < width_in_pixels; cols++) {
		if (hinibble) {
			target[cols >> 1] = GREY((((bits[cols] & FI16_565_RED_MASK) >> FI16_565_RED_SHIFT) * 0xFF) / 0x1F,
				        (((bits[cols] & FI16_565_GREEN_MASK) >> FI16_565_GREEN_SHIFT) * 0xFF) / 0x3F,
						(((bits[cols] & FI16_565_BLUE_MASK) >> FI16_565_BLUE_SHIFT) * 0xFF) / 0x1F)
						& 0xF0;
		} else {
			target[cols >> 1] |= GREY((((bits[cols] & FI16_565_RED_MASK) >> FI16_565_RED_SHIFT) * 0xFF) / 0x1F,
				        (((bits[cols] & FI16_565_GREEN_MASK) >> FI16_565_GREEN_SHIFT) * 0xFF) / 0x3F,
						(((bits[cols] & FI16_565_BLUE_MASK) >> FI16_565_BLUE_SHIFT) * 0xFF) / 0x1F)
						>> 4;
		}

		hinibble = !hinibble;
	}
}

void DLL_CALLCONV
FreeImage_ConvertLine24To4(uint8_t *target, uint8_t *source, int width_in_pixels) {
	FIBOOL hinibble = TRUE;

	for (int cols = 0; cols < width_in_pixels; cols++) {
		if (hinibble) {
			target[cols >> 1] = GREY(source[FI_RGBA_RED], source[FI_RGBA_GREEN], source[FI_RGBA_BLUE]) & 0xF0;
		} else {
			target[cols >> 1] |= GREY(source[FI_RGBA_RED], source[FI_RGBA_GREEN], source[FI_RGBA_BLUE]) >> 4;
		}

		source += 3;
		hinibble = !hinibble;
	}
}

void DLL_CALLCONV
FreeImage_ConvertLine32To4(uint8_t *target, uint8_t *source, int width_in_pixels) {
	FIBOOL hinibble = TRUE;

	for (int cols = 0; cols < width_in_pixels; cols++) {
		if (hinibble) {
			target[cols >> 1] = GREY(source[FI_RGBA_RED], source[FI_RGBA_GREEN], source[FI_RGBA_BLUE]) & 0xF0;
		} else {
			target[cols >> 1] |= GREY(source[FI_RGBA_RED], source[FI_RGBA_GREEN], source[FI_RGBA_BLUE]) >> 4;
		}

		source += 4;
		hinibble = !hinibble;
	}
}

// ----------------------------------------------------------
//   smart convert X to 4 bits
// ----------------------------------------------------------

FIBITMAP * DLL_CALLCONV
FreeImage_ConvertTo4Bits(FIBITMAP *dib) {
	if(!FreeImage_HasPixels(dib)) return NULL;

	const int bpp = FreeImage_GetBPP(dib);

	if(bpp != 4) {
		const int width  = FreeImage_GetWidth(dib);
		const int height = FreeImage_GetHeight(dib);
		FIBITMAP *new_dib = FreeImage_Allocate(width, height, 4);

		if(new_dib == NULL) {
			return NULL;
		}

		// copy metadata from src to dst
		FreeImage_CloneMetadata(new_dib, dib);

		// Build a greyscale palette (*always* needed for image processing)

		FIRGBA8 *new_pal = FreeImage_GetPalette(new_dib);

		for(int i = 0; i < 16; i++) {
			new_pal[i].red	= (uint8_t)((i << 4) + i);
			new_pal[i].green = (uint8_t)((i << 4) + i);
			new_pal[i].blue	= (uint8_t)((i << 4) + i);
		}

		switch(bpp) {
			case 1:
			{
				if(FreeImage_GetColorType(dib) == FIC_PALETTE) {

					// Copy the palette

					FIRGBA8 *old_pal = FreeImage_GetPalette(dib);
					memcpy(&new_pal[0], &old_pal[0], sizeof(FIRGBA8));
					memcpy(&new_pal[15], &old_pal[1], sizeof(FIRGBA8));

				}
				else if(FreeImage_GetColorType(dib) == FIC_MINISWHITE) {
					
					// Reverse the grayscale palette

					for(int i = 0; i < 16; i++) {
						new_pal[i].red = new_pal[i].green = new_pal[i].blue = (uint8_t)(255 - ((i << 4) + i));
					}
				}

				// Expand and copy the bitmap data

				for (int rows = 0; rows < height; rows++) {
					FreeImage_ConvertLine1To4(FreeImage_GetScanLine(new_dib, rows), FreeImage_GetScanLine(dib, rows), width);
				}
				return new_dib;
			}

			case 8 :
			{
				// Expand and copy the bitmap data

				for (int rows = 0; rows < height; rows++) {
					FreeImage_ConvertLine8To4(FreeImage_GetScanLine(new_dib, rows), FreeImage_GetScanLine(dib, rows), width, FreeImage_GetPalette(dib));
				}
				return new_dib;
			}

			case 16 :
			{
				// Expand and copy the bitmap data

				for (int rows = 0; rows < height; rows++) {
					if ((FreeImage_GetRedMask(dib) == FI16_565_RED_MASK) && (FreeImage_GetGreenMask(dib) == FI16_565_GREEN_MASK) && (FreeImage_GetBlueMask(dib) == FI16_565_BLUE_MASK)) {
						FreeImage_ConvertLine16To4_565(FreeImage_GetScanLine(new_dib, rows), FreeImage_GetScanLine(dib, rows), width);
					} else {
						FreeImage_ConvertLine16To4_555(FreeImage_GetScanLine(new_dib, rows), FreeImage_GetScanLine(dib, rows), width);
					}
				}
				
				return new_dib;
			}

			case 24 :
			{
				// Expand and copy the bitmap data

				for (int rows = 0; rows < height; rows++) {
					FreeImage_ConvertLine24To4(FreeImage_GetScanLine(new_dib, rows), FreeImage_GetScanLine(dib, rows), width);					
				}
				return new_dib;
			}

			case 32 :
			{
				// Expand and copy the bitmap data

				for (int rows = 0; rows < height; rows++) {
					FreeImage_ConvertLine32To4(FreeImage_GetScanLine(new_dib, rows), FreeImage_GetScanLine(dib, rows), width);
				}
				return new_dib;
			}
		}
	}

	return FreeImage_Clone(dib);
}
