// ==========================================================
// Bitmap conversion routines
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
#include "SimpleTools.h"

// ----------------------------------------------------------
//   smart convert X to Float
// ----------------------------------------------------------


FIBITMAP * DLL_CALLCONV
FreeImage_ConvertToFloat(FIBITMAP *dib, FIBOOL scale_linear) {
	FIBITMAP *src = NULL;
	FIBITMAP *dst = NULL;

	if(!FreeImage_HasPixels(dib)) return NULL;

	FREE_IMAGE_TYPE src_type = FreeImage_GetImageType(dib);

	// check for allowed conversions 
	switch(src_type) {
		case FIT_BITMAP:
		{
			// allow conversion from 8-bit
			if((FreeImage_GetBPP(dib) == 8) && (FreeImage_GetColorType(dib) == FIC_MINISBLACK)) {
				src = dib;
			} else {
				src = FreeImage_ConvertToGreyscale(dib);
				if(!src) return NULL;
			}
			break;
		}
		case FIT_DOUBLE:
		case FIT_UINT32:
		case FIT_INT32:
		case FIT_UINT16:
		case FIT_INT16:
		case FIT_RGB32:
		case FIT_RGBA32:
		case FIT_RGB16:
		case FIT_RGBA16:
		case FIT_RGBF:
		case FIT_RGBAF:
			src = dib;
			break;
		case FIT_FLOAT:
			// float type : clone the src
			return FreeImage_Clone(dib);
		default:
			return NULL;
	}

	// allocate dst image

	const unsigned width = FreeImage_GetWidth(src);
	const unsigned height = FreeImage_GetHeight(src);

	dst = FreeImage_AllocateT(FIT_FLOAT, width, height);
	if(!dst) {
		if(src != dib) {
			FreeImage_Unload(src);
		}
		return NULL;
	}

	// copy metadata from src to dst
	FreeImage_CloneMetadata(dst, src);

	// convert from src type to float
	switch(src_type) {
		case FIT_BITMAP:
			if (scale_linear) {
				BitmapTransform<float, uint8_t>(dst, src, [](uint8_t v) { 
					return static_cast<float>(v) / static_cast<float>(std::numeric_limits<uint8_t>::max()); });
			}
			else {
				BitmapTransform<float, uint8_t>(dst, src, [](uint8_t v) { return static_cast<float>(v); });
			}
			break;

		case FIT_UINT16:
			if (scale_linear) {
				BitmapTransform<float, uint16_t>(dst, src, [](uint16_t v) { 
					return static_cast<float>(v) / static_cast<float>(std::numeric_limits<uint16_t>::max()); });
			}
			else {
				BitmapTransform<float, uint16_t>(dst, src, [](uint16_t v) { return static_cast<float>(v); });
			}
			break;

		case FIT_INT16:
			if (scale_linear) {
				BitmapTransform<float, int16_t>(dst, src, [](int16_t v) {
					return static_cast<float>(v) / static_cast<float>(std::numeric_limits<int16_t>::max()); });
			}
			else {
				BitmapTransform<float, int16_t>(dst, src, [](int16_t v) { return static_cast<float>(v); });
			}
			break;

		case FIT_UINT32:
			if (scale_linear) {
				BitmapTransform<float, uint32_t>(dst, src, [](uint32_t v) {
					return static_cast<float>(static_cast<double>(v) / static_cast<double>(std::numeric_limits<uint32_t>::max())); });
			}
			else {
				BitmapTransform<float, uint32_t>(dst, src, [](uint32_t v) { return static_cast<float>(v); });
			}
			break;

		case FIT_INT32:
			if (scale_linear) {
				BitmapTransform<float, int32_t>(dst, src, [](int32_t v) { 
					return static_cast<float>(static_cast<double>(v) / static_cast<double>(std::numeric_limits<int32_t>::max())); });
			}
			else {
				BitmapTransform<float, int32_t>(dst, src, [](int32_t v) { return static_cast<float>(v); });
			}
			break;

		case FIT_RGB32:
			if (scale_linear) {
				BitmapTransform<float, FIRGB32>(dst, src, [](const FIRGB32& p) {
					return static_cast<float>(LUMA_REC709(p.red, p.green, p.blue) / static_cast<double>(std::numeric_limits<uint32_t>::max())); });
			}
			else {
				BitmapTransform<float, FIRGB32>(dst, src, [](const FIRGB32& p) {
					return LUMA_REC709(p.red, p.green, p.blue); });
			}
			break;

		case FIT_RGBA32:
			if (scale_linear) {
				BitmapTransform<float, FIRGBA32>(dst, src, [](const FIRGBA32& p) {
					return static_cast<float>(LUMA_REC709(p.red, p.green, p.blue) / static_cast<double>(std::numeric_limits<uint32_t>::max())); });
			}
			else {
				BitmapTransform<float, FIRGBA32>(dst, src, [](const FIRGBA32& p) {
					return LUMA_REC709(p.red, p.green, p.blue); });
			}
			break;

		case FIT_RGB16:
			if (scale_linear) {
				BitmapTransform<float, FIRGB16>(dst, src, [](const FIRGB16& p) {
					return LUMA_REC709(p.red, p.green, p.blue) / static_cast<float>(std::numeric_limits<uint16_t>::max()); });
			}
			else {
				BitmapTransform<float, FIRGB16>(dst, src, [](const FIRGB16& p) {
					return LUMA_REC709(p.red, p.green, p.blue); });
			}
			break;

		case FIT_RGBA16:
			if (scale_linear) {
				BitmapTransform<float, FIRGBA16>(dst, src, [](const FIRGBA16& p) {
					return LUMA_REC709(p.red, p.green, p.blue) / static_cast<float>(std::numeric_limits<uint16_t>::max()); });
			}
			else {
				BitmapTransform<float, FIRGBA16>(dst, src, [](const FIRGBA16& p) {
					return LUMA_REC709(p.red, p.green, p.blue); });
			}
			break;

		case FIT_RGBF:
			if (scale_linear) {
				BitmapTransform<float, FIRGBF>(dst, src, [](const FIRGBF& p) {
					return CLAMP(LUMA_REC709(p.red, p.green, p.blue), 0.0F, 1.0F); });
			}
			else {
				BitmapTransform<float, FIRGBF>(dst, src, [](const FIRGBF& p) {
					return LUMA_REC709(p.red, p.green, p.blue); });
			}
			break;

		case FIT_RGBAF:
			if (scale_linear) {
				BitmapTransform<float, FIRGBAF>(dst, src, [](const FIRGBAF& p) {
					return CLAMP(LUMA_REC709(p.red, p.green, p.blue), 0.0F, 1.0F); });
			}
			else {
				BitmapTransform<float, FIRGBAF>(dst, src, [](const FIRGBAF& p) {
					return LUMA_REC709(p.red, p.green, p.blue); });
			}
			break;

		case FIT_DOUBLE:
			BitmapTransform<float, double>(dst, src, [](double v) { return static_cast<float>(v); });
			break;
	}

	if(src != dib) {
		FreeImage_Unload(src);
	}

	return dst;
}

