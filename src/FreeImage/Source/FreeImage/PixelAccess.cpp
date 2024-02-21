// ==========================================================
// Pixel access functions
//
// Design and implementation by
// - Floris van den Berg (flvdberg@wxs.nl)
// - Hervé Drolon (drolon@infonie.fr)
// - Ryan Rubley (ryan@lostreality.org)
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

uint8_t * DLL_CALLCONV
FreeImage_GetScanLine(FIBITMAP *dib, int scanline) {
	if(!FreeImage_HasPixels(dib)) {
		return NULL;
	}
	return CalculateScanLine(FreeImage_GetBits(dib), FreeImage_GetPitch(dib), scanline);
}

FIBOOL DLL_CALLCONV
FreeImage_GetPixelIndex(FIBITMAP *dib, unsigned x, unsigned y, uint8_t *value) {
	uint8_t shift;

	if(!FreeImage_HasPixels(dib) || (FreeImage_GetImageType(dib) != FIT_BITMAP))
		return FALSE;

	if((x < FreeImage_GetWidth(dib)) && (y < FreeImage_GetHeight(dib))) {
		uint8_t *bits = FreeImage_GetScanLine(dib, y);

		switch(FreeImage_GetBPP(dib)) {
			case 1:
				*value = (bits[x >> 3] & (0x80 >> (x & 0x07))) != 0;
				break;
			case 4:
				shift = (uint8_t)((1 - x % 2) << 2);
				*value = (bits[x >> 1] & (0x0F << shift)) >> shift;
				break;
			case 8:
				*value = bits[x];
				break;
			default:
				return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

FIBOOL DLL_CALLCONV
FreeImage_GetPixelColor(FIBITMAP *dib, unsigned x, unsigned y, FIRGBA8 *value) {
	if(!FreeImage_HasPixels(dib) || (FreeImage_GetImageType(dib) != FIT_BITMAP))
		return FALSE;

	if((x < FreeImage_GetWidth(dib)) && (y < FreeImage_GetHeight(dib))) {
		uint8_t *bits = FreeImage_GetScanLine(dib, y);

		switch(FreeImage_GetBPP(dib)) {
			case 16:
			{
				bits += 2*x;
				uint16_t *pixel = (uint16_t *)bits;
				if((FreeImage_GetRedMask(dib) == FI16_565_RED_MASK) && (FreeImage_GetGreenMask(dib) == FI16_565_GREEN_MASK) && (FreeImage_GetBlueMask(dib) == FI16_565_BLUE_MASK)) {
					value->blue		= (uint8_t)((((*pixel & FI16_565_BLUE_MASK) >> FI16_565_BLUE_SHIFT) * 0xFF) / 0x1F);
					value->green		= (uint8_t)((((*pixel & FI16_565_GREEN_MASK) >> FI16_565_GREEN_SHIFT) * 0xFF) / 0x3F);
					value->red		= (uint8_t)((((*pixel & FI16_565_RED_MASK) >> FI16_565_RED_SHIFT) * 0xFF) / 0x1F);
					value->alpha	= 0;
				} else {
					value->blue		= (uint8_t)((((*pixel & FI16_555_BLUE_MASK) >> FI16_555_BLUE_SHIFT) * 0xFF) / 0x1F);
					value->green		= (uint8_t)((((*pixel & FI16_555_GREEN_MASK) >> FI16_555_GREEN_SHIFT) * 0xFF) / 0x1F);
					value->red		= (uint8_t)((((*pixel & FI16_555_RED_MASK) >> FI16_555_RED_SHIFT) * 0xFF) / 0x1F);
					value->alpha	= 0;
				}
				break;
			}
			case 24:
				bits += 3*x;
				value->blue		= bits[FI_RGBA_BLUE];	// B
				value->green		= bits[FI_RGBA_GREEN];	// G
				value->red		= bits[FI_RGBA_RED];	// R
				value->alpha	= 0;
				break;
			case 32:
				bits += 4*x;
				value->blue		= bits[FI_RGBA_BLUE];	// B
				value->green		= bits[FI_RGBA_GREEN];	// G
				value->red		= bits[FI_RGBA_RED];	// R
				value->alpha	= bits[FI_RGBA_ALPHA];	// A
				break;
			default:
				return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

FIBOOL DLL_CALLCONV
FreeImage_SetPixelIndex(FIBITMAP *dib, unsigned x, unsigned y, uint8_t *value) {
	uint8_t shift;

	if(!FreeImage_HasPixels(dib) || (FreeImage_GetImageType(dib) != FIT_BITMAP))
		return FALSE;

	if((x < FreeImage_GetWidth(dib)) && (y < FreeImage_GetHeight(dib))) {
		uint8_t *bits = FreeImage_GetScanLine(dib, y);

		switch(FreeImage_GetBPP(dib)) {
			case 1:
				*value ? bits[x >> 3] |= (0x80 >> (x & 0x7)) : bits[x >> 3] &= (0xFF7F >> (x & 0x7));
				break;
			case 4:
				shift = (uint8_t)((1 - x % 2) << 2);
				bits[x >> 1] &= ~(0x0F << shift);
				bits[x >> 1] |= ((*value & 0x0F) << shift);
				break;
			case 8:
				bits[x] = *value;
				break;
			default:
				return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

FIBOOL DLL_CALLCONV
FreeImage_SetPixelColor(FIBITMAP *dib, unsigned x, unsigned y, FIRGBA8 *value) {
	if(!FreeImage_HasPixels(dib) || (FreeImage_GetImageType(dib) != FIT_BITMAP))
		return FALSE;

	if((x < FreeImage_GetWidth(dib)) && (y < FreeImage_GetHeight(dib))) {
		uint8_t *bits = FreeImage_GetScanLine(dib, y);

		switch(FreeImage_GetBPP(dib)) {
			case 16:
			{
				bits += 2*x;
				uint16_t *pixel = (uint16_t *)bits;
				if((FreeImage_GetRedMask(dib) == FI16_565_RED_MASK) && (FreeImage_GetGreenMask(dib) == FI16_565_GREEN_MASK) && (FreeImage_GetBlueMask(dib) == FI16_565_BLUE_MASK)) {
					*pixel = ((value->blue >> 3) << FI16_565_BLUE_SHIFT) |
						((value->green >> 2) << FI16_565_GREEN_SHIFT) |
						((value->red >> 3) << FI16_565_RED_SHIFT);
				} else {
					*pixel = ((value->blue >> 3) << FI16_555_BLUE_SHIFT) |
						((value->green >> 3) << FI16_555_GREEN_SHIFT) |
						((value->red >> 3) << FI16_555_RED_SHIFT);
				}
				break;
			}
			case 24:
				bits += 3*x;
				bits[FI_RGBA_BLUE]	= value->blue;	// B
				bits[FI_RGBA_GREEN] = value->green;	// G
				bits[FI_RGBA_RED]	= value->red;	// R
				break;
			case 32:
				bits += 4*x;
				bits[FI_RGBA_BLUE]	= value->blue;		// B
				bits[FI_RGBA_GREEN] = value->green;		// G
				bits[FI_RGBA_RED]	= value->red;		// R
				bits[FI_RGBA_ALPHA] = value->alpha;	// A
				break;
			default:
				return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

