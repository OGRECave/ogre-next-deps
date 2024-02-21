// ==========================================================
// Display routines
//
// Design and implementation by
// - Hervé Drolon (drolon@infonie.fr)
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


/**
@brief Composite a foreground image against a background color or a background image.

The equation for computing a composited sample value is:<br>
output = alpha * foreground + (1-alpha) * background<br>
where alpha and the input and output sample values are expressed as fractions in the range 0 to 1. 
For colour images, the computation is done separately for R, G, and B samples.

@param fg Foreground image
@param useFileBkg If TRUE and a file background is present, use it as the background color
@param appBkColor If not equal to NULL, and useFileBkg is FALSE, use this color as the background color
@param bg If not equal to NULL and useFileBkg is FALSE and appBkColor is NULL, use this as the background image
@return Returns the composite image if successful, returns NULL otherwise
@see FreeImage_IsTransparent, FreeImage_HasBackgroundColor
*/
FIBITMAP * DLL_CALLCONV
FreeImage_Composite(FIBITMAP *fg, FIBOOL useFileBkg, FIRGBA8 *appBkColor, FIBITMAP *bg) {
	if(!FreeImage_HasPixels(fg)) return NULL;

	int width  = FreeImage_GetWidth(fg);
	int height = FreeImage_GetHeight(fg);
	int bpp    = FreeImage_GetBPP(fg);

	if((bpp != 8) && (bpp != 32))
		return NULL;

	if(bg) {
		int bg_width  = FreeImage_GetWidth(bg);
		int bg_height = FreeImage_GetHeight(bg);
		int bg_bpp    = FreeImage_GetBPP(bg);
		if((bg_width != width) || (bg_height != height) || (bg_bpp != 24))
			return NULL;
	}

	int bytespp = (bpp == 8) ? 1 : 4;

	
	int x, y, c;
	uint8_t alpha = 0, not_alpha;
	uint8_t index;
	FIRGBA8 fgc;	// foreground color
	FIRGBA8 bkc;	// background color

	memset(&fgc, 0, sizeof(FIRGBA8));
	memset(&bkc, 0, sizeof(FIRGBA8));

	// allocate the composite image
	FIBITMAP *composite = FreeImage_Allocate(width, height, 24, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
	if(!composite) return NULL;

	// get the palette
	FIRGBA8 *pal = FreeImage_GetPalette(fg);

	// retrieve the alpha table from the foreground image
	FIBOOL bIsTransparent = FreeImage_IsTransparent(fg);
	uint8_t *trns = FreeImage_GetTransparencyTable(fg);

	// retrieve the background color from the foreground image
	FIBOOL bHasBkColor = FALSE;

	if(useFileBkg && FreeImage_HasBackgroundColor(fg)) {
		FreeImage_GetBackgroundColor(fg, &bkc);
		bHasBkColor = TRUE;
	} else {
		// no file background color
		// use application background color ?
		if(appBkColor) {
			memcpy(&bkc, appBkColor, sizeof(FIRGBA8));
			bHasBkColor = TRUE;
		}
		// use background image ?
		else if(bg) {
			bHasBkColor = FALSE;
		}
	}

	for(y = 0; y < height; y++) {
		// foreground
		uint8_t *fg_bits = FreeImage_GetScanLine(fg, y);
		// background
		uint8_t *bg_bits = FreeImage_GetScanLine(bg, y);
		// composite image
		uint8_t *cp_bits = FreeImage_GetScanLine(composite, y);

		for(x = 0; x < width; x++) {

			// foreground color + alpha

			if(bpp == 8) {
				// get the foreground color
				index = fg_bits[0];
				memcpy(&fgc, &pal[index], sizeof(FIRGBA8));
				// get the alpha
				if(bIsTransparent) {
					alpha = trns[index];
				} else {
					alpha = 255;
				}
			}
			else if(bpp == 32) {
				// get the foreground color
				fgc.blue  = fg_bits[FI_RGBA_BLUE];
				fgc.green = fg_bits[FI_RGBA_GREEN];
				fgc.red   = fg_bits[FI_RGBA_RED];
				// get the alpha
				alpha = fg_bits[FI_RGBA_ALPHA];
			}

			// background color

			if(!bHasBkColor) {
				if(bg) {
					// get the background color from the background image
					bkc.blue  = bg_bits[FI_RGBA_BLUE];
					bkc.green = bg_bits[FI_RGBA_GREEN];
					bkc.red   = bg_bits[FI_RGBA_RED];
				}
				else {
					// use a checkerboard pattern
					c = (((y & 0x8) == 0) ^ ((x & 0x8) == 0)) * 192;
					c = c ? c : 255;
					bkc.blue  = (uint8_t)c;
					bkc.green = (uint8_t)c;
					bkc.red   = (uint8_t)c;
				}
			}

			// composition

			if(alpha == 0) {
				// output = background
				cp_bits[FI_RGBA_BLUE] = bkc.blue;
				cp_bits[FI_RGBA_GREEN] = bkc.green;
				cp_bits[FI_RGBA_RED] = bkc.red;
			}
			else if(alpha == 255) {
				// output = foreground
				cp_bits[FI_RGBA_BLUE] = fgc.blue;
				cp_bits[FI_RGBA_GREEN] = fgc.green;
				cp_bits[FI_RGBA_RED] = fgc.red;
			}
			else {
				// output = alpha * foreground + (1-alpha) * background
				not_alpha = (uint8_t)~alpha;
				cp_bits[FI_RGBA_BLUE] = (uint8_t)((alpha * (uint16_t)fgc.blue  + not_alpha * (uint16_t)bkc.blue) >> 8);
				cp_bits[FI_RGBA_GREEN] = (uint8_t)((alpha * (uint16_t)fgc.green + not_alpha * (uint16_t)bkc.green) >> 8);
				cp_bits[FI_RGBA_RED] = (uint8_t)((alpha * (uint16_t)fgc.red   + not_alpha * (uint16_t)bkc.red) >> 8);
			}

			fg_bits += bytespp;
			bg_bits += 3;
			cp_bits += 3;
		}
	}

	// copy metadata from src to dst
	FreeImage_CloneMetadata(composite, fg);
	
	return composite;	
}

/**
Pre-multiplies a 32-bit image's red-, green- and blue channels with it's alpha channel 
for to be used with e.g. the Windows GDI function AlphaBlend(). 
The transformation changes the red-, green- and blue channels according to the following equation:  
channel(x, y) = channel(x, y) * alpha_channel(x, y) / 255  
@param dib Input/Output dib to be premultiplied
@return Returns TRUE on success, FALSE otherwise (e.g. when the bitdepth of the source dib cannot be handled). 
*/
FIBOOL DLL_CALLCONV 
FreeImage_PreMultiplyWithAlpha(FIBITMAP *dib) {
	if (!FreeImage_HasPixels(dib)) return FALSE;
	
	if ((FreeImage_GetBPP(dib) != 32) || (FreeImage_GetImageType(dib) != FIT_BITMAP)) {
		return FALSE;
	}

	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);

	for(int y = 0; y < height; y++) {
		uint8_t *bits = FreeImage_GetScanLine(dib, y);
		for (int x = 0; x < width; x++, bits += 4) {
			const uint8_t alpha = bits[FI_RGBA_ALPHA];
			// slightly faster: care for two special cases
			if(alpha == 0x00) {
				// special case for alpha == 0x00
				// color * 0x00 / 0xFF = 0x00
				bits[FI_RGBA_BLUE] = 0x00;
				bits[FI_RGBA_GREEN] = 0x00;
				bits[FI_RGBA_RED] = 0x00;
			} else if(alpha == 0xFF) {
				// nothing to do for alpha == 0xFF
				// color * 0xFF / 0xFF = color
				continue;
			} else {
				bits[FI_RGBA_BLUE] = (uint8_t)( (alpha * (uint16_t)bits[FI_RGBA_BLUE] + 127) / 255 );
				bits[FI_RGBA_GREEN] = (uint8_t)( (alpha * (uint16_t)bits[FI_RGBA_GREEN] + 127) / 255 );
				bits[FI_RGBA_RED] = (uint8_t)( (alpha * (uint16_t)bits[FI_RGBA_RED] + 127) / 255 );
			}
		}
	}
	return TRUE;
}



FIBOOL FreeImage_DrawBitmap(FIBITMAP* dst, FIBITMAP* src, FREE_IMAGE_ALPHA_OPERATION alpha, int32_t left, int32_t top)
{
	if (!FreeImage_HasPixels(dst) || !FreeImage_HasPixels(src)) {
		return FALSE;
	}

	if (FreeImage_GetImageType(dst) != FIT_BITMAP || FreeImage_GetColorType2(dst) != FIC_RGBALPHA || FreeImage_GetBPP(dst) != 32 ||
			FreeImage_GetImageType(src) != FIT_BITMAP || FreeImage_GetColorType2(src) != FIC_RGBALPHA || FreeImage_GetBPP(src) != 32) {
		return FALSE;
	}

	const int32_t dstW = static_cast<int32_t>(FreeImage_GetWidth(dst));
	const int32_t dstH = static_cast<int32_t>(FreeImage_GetHeight(dst));
	const int32_t srcW = static_cast<int32_t>(FreeImage_GetWidth(src));
	const int32_t srcH = static_cast<int32_t>(FreeImage_GetHeight(src));

	if (left + srcW <= 0 || top + srcH <= 0) {
		return TRUE;
	}

	const int32_t roiLeft   = std::max(0, left);
	const int32_t roiTop    = std::max(0, top);
	const int32_t roiRight  = std::min(left + srcW, dstW);
	const int32_t roiBottom = std::min(top + srcH, dstH);

	const int32_t offsetX = roiLeft - left;
	const int32_t offsetY = roiTop - top;

	if (alpha != FIAO_SrcAlpha) {
		// not supported
		return FALSE;
	}

	// Y axis is flipped in FI
	for (int32_t y = roiBottom - roiTop; y > 0; --y) {
		const auto srcLine = static_cast<const FIRGBA8*>(static_cast<const void*>(FreeImage_GetScanLine(src, srcH - y - offsetY))) + offsetX;
		const auto dstLine = static_cast<FIRGBA8*>(static_cast<void*>(FreeImage_GetScanLine(dst, dstH - roiTop - y))) + roiLeft;
		for (int32_t x = 0; x < roiRight - roiLeft; ++x) {
			const uint8_t alpha = srcLine[x].alpha;
			if (alpha == 255) {
				dstLine[x].red   = srcLine[x].red;
				dstLine[x].green = srcLine[x].green;
				dstLine[x].blue  = srcLine[x].blue;
			}
			else if (alpha > 0) {
				const uint8_t notAlpha = ~alpha;
				dstLine[x].red   = static_cast<uint8_t>((alpha * srcLine[x].red   + notAlpha * dstLine[x].red)   / 255);
				dstLine[x].green = static_cast<uint8_t>((alpha * srcLine[x].green + notAlpha * dstLine[x].green) / 255);
				dstLine[x].blue  = static_cast<uint8_t>((alpha * srcLine[x].blue  + notAlpha * dstLine[x].blue)  / 255);
			}
		}
	}

	return TRUE;
}
