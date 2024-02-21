//===========================================================
// FreeImage Re(surrected)
// Modified fork from the original FreeImage 3.18
// with updated dependencies and extended features.
//===========================================================

#include "SimpleTools.h"
#include <cstring>
#include <algorithm>

#if __cplusplus >= 201703L
FIBOOL FreeImage_FindMinMax(FIBITMAP* dib, double* min_brightness, double* max_brightness, void** min_ptr, void** max_ptr)
{
	if (!FreeImage_HasPixels(dib)) {
		return FALSE;
	}
	std::tuple<void*, void*, double, double> res{};
	bool success = false;

	const auto colorType = FreeImage_GetColorType2(dib);
	
	switch (FreeImage_GetImageType(dib))
	{
	case FIT_BITMAP: {
			const auto bpp = FreeImage_GetBPP(dib);
			if (bpp == 32) {
				if (colorType == FIC_RGBALPHA) {
					res = FindMinMax<FIRGBA8>(dib);
					success = true;
				}
				else if (colorType == FIC_YUV) {
					res = FindMinMax<FIRGBA8, YuvBrightness>(dib);
					success = true;
				}
			}
			else if (bpp == 24) {
				if (colorType == FIC_RGB) {
					res = FindMinMax<FIRGB8>(dib);
					success = true;
				}
				else if (colorType == FIC_YUV) {
					res = FindMinMax<FIRGB8, YuvBrightness>(dib);
					success = true;
				}
			}
			else if (bpp == 8) {
				if (colorType == FIC_MINISBLACK) {
					res = FindMinMax<uint8_t>(dib);
					success = true;
				}
			}
		}		
		break;
	case FIT_RGBAF:
		if (colorType == FIC_RGBALPHA) {
			res = FindMinMax<FIRGBAF>(dib);
			success = true;
		}
		else if (colorType == FIC_YUV) {
			res = FindMinMax<FIRGBAF, YuvBrightness>(dib);
			success = true;
		}
		break;
	case FIT_RGBF:
		if (colorType == FIC_RGB) {
			res = FindMinMax<FIRGBF>(dib);
			success = true;
		}
		else if (colorType == FIC_YUV) {
			res = FindMinMax<FIRGBF, YuvBrightness>(dib);
			success = true;
		}
		break;
	case FIT_RGBA32:
		res = FindMinMax<FIRGBA32>(dib);
		success = true;
		break;
	case FIT_RGB32:
		res = FindMinMax<FIRGB32>(dib);
		success = true;
		break;
	case FIT_RGBA16:
		res = FindMinMax<FIRGBA16>(dib);
		success = true;
		break;
	case FIT_RGB16:
		res = FindMinMax<FIRGB16>(dib);
		success = true;
		break;
	case FIT_DOUBLE:
		res = FindMinMax<double>(dib);
		success = true;
		break;
	case FIT_FLOAT:
		res = FindMinMax<float>(dib);
		success = true;
		break;
	case FIT_UINT32:
		res = FindMinMax<uint32_t>(dib);
		success = true;
		break;
	case FIT_INT32:
		res = FindMinMax<int32_t>(dib);
		success = true;
		break;
	case FIT_UINT16:
		res = FindMinMax<uint16_t>(dib);
		success = true;
		break;
	case FIT_INT16:
		res = FindMinMax<int32_t>(dib);
		success = true;
		break;
	default:
		break;
	}

	if (success) {
		if (min_brightness) {
			*min_brightness = std::get<2>(res);
		}
		if (max_brightness) {
			*max_brightness = std::get<3>(res);
		}
		if (min_ptr) {
			*min_ptr = std::get<0>(res);
		}
		if (max_ptr) {
			*max_ptr = std::get<1>(res);
		}
	}

	return success ? TRUE : FALSE;
}

namespace
{

	template <typename PixelType_>
	void FindMinMaxValueImpl(FIBITMAP* src, void* out_min_value, void* out_max_value)
	{
		PixelType_ minVal, maxVal;
		PixelFill(minVal, std::numeric_limits<ToValueType<PixelType_>>::max());
		PixelFill(maxVal, std::numeric_limits<ToValueType<PixelType_>>::lowest());

		const unsigned width = FreeImage_GetWidth(src);
		const unsigned height = FreeImage_GetHeight(src);
		const unsigned src_pitch = FreeImage_GetPitch(src);

		uint8_t* src_bits = FreeImage_GetBits(src);
		for (unsigned y = 0; y < height; ++y) {
			auto src_pixel = static_cast<PixelType_*>(static_cast<void*>(src_bits));
			for (unsigned x = 0; x < width; ++x) {
				const PixelType_ val = src_pixel[x];
				SetChannel<0>(minVal, std::min(GetChannel<0>(minVal), GetChannel<0>(val)));
				SetChannel<1>(minVal, std::min(GetChannel<1>(minVal), GetChannel<1>(val)));
				SetChannel<2>(minVal, std::min(GetChannel<2>(minVal), GetChannel<2>(val)));
				SetChannel<3>(minVal, std::min(GetChannel<3>(minVal), GetChannel<3>(val)));
				SetChannel<0>(maxVal, std::max(GetChannel<0>(maxVal), GetChannel<0>(val)));
				SetChannel<1>(maxVal, std::max(GetChannel<1>(maxVal), GetChannel<1>(val)));
				SetChannel<2>(maxVal, std::max(GetChannel<2>(maxVal), GetChannel<2>(val)));
				SetChannel<3>(maxVal, std::max(GetChannel<3>(maxVal), GetChannel<3>(val)));
			}
			src_bits += src_pitch;
		}

		if (out_min_value) {
			*static_cast<PixelType_*>(out_min_value) = minVal;
		}
		if (out_max_value) {
			*static_cast<PixelType_*>(out_max_value) = maxVal;
		}
	}

} // namespace

FIBOOL FreeImage_FindMinMaxValue(FIBITMAP* dib, void* min_value, void* max_value)
{
	if (!FreeImage_HasPixels(dib)) {
		return FALSE;
	}

	switch (FreeImage_GetImageType(dib)) {
	case FIT_BITMAP: {
			const auto bpp = FreeImage_GetBPP(dib);
			const auto colorType = FreeImage_GetColorType2(dib);
			if (bpp == 32) {
				if (colorType == FIC_RGBALPHA || colorType == FIC_YUV) {
					FindMinMaxValueImpl<FIRGBA8>(dib, min_value, max_value);
				}
				else {
					return FALSE;
				}
			}
			else if (bpp == 24) {
				if (colorType == FIC_RGB || colorType == FIC_YUV) {
					FindMinMaxValueImpl<FIRGB8>(dib, min_value, max_value);
				}
				else {
					return FALSE;
				}
			}
			else if (bpp == 8) {
				if (colorType == FIC_MINISBLACK) {
					FindMinMaxValueImpl<uint8_t>(dib, min_value, max_value);
				}
				else {
					return FALSE;
				}
			}
			else {
				return FALSE;
			}
		}
		break;
	case FIT_RGBAF:
		FindMinMaxValueImpl<FIRGBAF>(dib, min_value, max_value);
		break;
	case FIT_RGBF:
		FindMinMaxValueImpl<FIRGBF>(dib, min_value, max_value);
		break;
	case FIT_RGBA32:
		FindMinMaxValueImpl<FIRGBA32>(dib, min_value, max_value);
		break;
	case FIT_RGB32:
		FindMinMaxValueImpl<FIRGB32>(dib, min_value, max_value);
		break;
	case FIT_RGBA16:
		FindMinMaxValueImpl<FIRGBA16>(dib, min_value, max_value);
		break;
	case FIT_RGB16:
		FindMinMaxValueImpl<FIRGB16>(dib, min_value, max_value);
		break;
	case FIT_DOUBLE:
		FindMinMaxValueImpl<double>(dib, min_value, max_value);
		break;
	case FIT_FLOAT:
		FindMinMaxValueImpl<float>(dib, min_value, max_value);
		break;
	case FIT_UINT32:
		FindMinMaxValueImpl<uint32_t>(dib, min_value, max_value);
		break;
	case FIT_INT32:
		FindMinMaxValueImpl<int32_t>(dib, min_value, max_value);
		break;
	case FIT_UINT16:
		FindMinMaxValueImpl<uint16_t>(dib, min_value, max_value);
		break;
	case FIT_INT16:
		FindMinMaxValueImpl<uint16_t>(dib, min_value, max_value);
		break;
	case FIT_COMPLEX:
		FindMinMaxValueImpl<FICOMPLEX>(dib, min_value, max_value);
		break;
	case FIT_COMPLEXF:
		FindMinMaxValueImpl<FICOMPLEXF>(dib, min_value, max_value);
		break;
	default:
		return FALSE;
	}

	return TRUE;
}
#else
FIBOOL FreeImage_FindMinMax(FIBITMAP* dib, double* min_brightness, double* max_brightness, void** min_ptr, void** max_ptr)
{
	return FALSE;
}

FIBOOL FreeImage_FindMinMaxValue(FIBITMAP* dib, void* min_value, void* max_value)
{
	return FALSE;
}
#endif


FIBOOL FreeImage_Fill(FIBITMAP* dib, const void* value_ptr, size_t value_size)
{
	if (!FreeImage_HasPixels(dib)) {
		return FALSE;
	}
	if (FreeImage_GetBPP(dib) != 8 * value_size) {
		return FALSE;
	}

	const unsigned width  = FreeImage_GetWidth(dib);
	const unsigned height = FreeImage_GetHeight(dib);
	const unsigned pitch  = FreeImage_GetPitch(dib);

	uint8_t* dst_line = FreeImage_GetBits(dib);
	for (unsigned y = 0; y < height; ++y, dst_line += pitch) {
		uint8_t* dst_pixel = dst_line;
		for (unsigned x = 0; x < width; ++x, dst_pixel += value_size) {
			std::memcpy(dst_pixel, value_ptr, value_size);
		}
	}

	return TRUE;
}

