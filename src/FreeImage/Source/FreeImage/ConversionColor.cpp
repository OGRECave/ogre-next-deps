//===========================================================
// FreeImage Re(surrected)
// Modified fork from the original FreeImage 3.18
// with updated dependencies and extended features.
//===========================================================


#include "FreeImage.h"
#include "ConversionYUV.h"


FIBITMAP* FreeImage_ConvertToColor(FIBITMAP* dib, FREE_IMAGE_COLOR_TYPE dst_color, int64_t fisrt_param, int64_t second_param)
{
	(void)second_param;
	if (!FreeImage_HasPixels(dib)) {
		return nullptr;
	}
	const auto srcColorType = FreeImage_GetColorType2(dib);
	if (srcColorType == FIC_RGB || srcColorType == FIC_RGBALPHA) {
		switch (dst_color) {
		case FIC_YUV:
			return ConvertRgbToYuv(dib, fisrt_param);
		default:
			return nullptr;
		}
	}
	else if (srcColorType == FIC_YUV) {
		switch (dst_color) {
		case FIC_RGB:
		case FIC_RGBALPHA:
			return ConvertYuvToRgb(dib, fisrt_param);
		default:
			return nullptr;
		}
	}
	return nullptr;
}
