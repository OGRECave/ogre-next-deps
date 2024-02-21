//===========================================================
// FreeImage Re(surrected)
// Modified fork from the original FreeImage 3.18
// with updated dependencies and extended features.
//===========================================================


#include "ConversionYUV.h"

#include <memory>
#include "SimpleTools.h"

#if __cplusplus >= 201703L


namespace {

	template <typename YuvStandard_>
	struct RgbToYuvConverter
	{
		template <typename Ty_>
		constexpr Ty_ operator()(const Ty_& p) {
			return RgbToYuv<YuvStandard_>(p);
		}
	};

	template <typename YuvStandard_>
	struct YuvToRgbConverter
	{
		template <typename Ty_>
		constexpr Ty_ operator()(const Ty_& p) {
			return YuvToRgb<YuvStandard_>(p);
		}
	};

} // namespace

template <typename Converter_>
static std::unique_ptr<FIBITMAP, decltype(&::FreeImage_Unload)> ConvertImpl(FIBITMAP* src, Converter_ cvt)
{
	std::unique_ptr<FIBITMAP, decltype(&::FreeImage_Unload)> dst{
		FreeImage_AllocateT(FreeImage_GetImageType(src), FreeImage_GetWidth(src), FreeImage_GetHeight(src), FreeImage_GetBPP(src)), &::FreeImage_Unload };

	switch (FreeImage_GetImageType(src)) {
	case FIT_BITMAP: {
			const auto bpp = FreeImage_GetBPP(src);
			if (bpp == 32) {
				BitmapTransform<FIRGBA8>(dst.get(), src, cvt);
			}
			else if (bpp == 24) {
				BitmapTransform<FIRGB8>(dst.get(), src, cvt);
			}
			else {
				std::unique_ptr<FIBITMAP, decltype(&::FreeImage_Unload)>{ nullptr, & ::FreeImage_Unload };
			}
		}
		break;
	case FIT_RGBF:
		BitmapTransform<FIRGBF>(dst.get(), src, cvt);
		break;
	case FIT_RGBAF:
		BitmapTransform<FIRGBAF>(dst.get(), src, cvt);
		break;
	default:
		// ToDo: Add all other types here...
		return std::unique_ptr<FIBITMAP, decltype(&::FreeImage_Unload)>{ nullptr, &::FreeImage_Unload };
	}

	return dst;
}

FIBITMAP* ConvertRgbToYuv(FIBITMAP* dib, int64_t standard_version)
{
	std::unique_ptr<FIBITMAP, decltype(&::FreeImage_Unload)> result{ nullptr,  &::FreeImage_Unload };

	switch (standard_version) {
	case FICPARAM_YUV_STANDARD_JPEG:
		result = ConvertImpl(dib, RgbToYuvConverter<YuvJPEG>{});
		break;
	default:
		return nullptr;
	}

	if (result) {
		auto icc = FreeImage_CreateICCProfile(result.get(), nullptr, 0u);
		icc->flags = FIICC_COLOR_IS_YUV;
	}

	return result.release();
}


FIBITMAP* ConvertYuvToRgb(FIBITMAP* dib, int64_t standard_version)
{
	std::unique_ptr<FIBITMAP, decltype(&::FreeImage_Unload)> result{ nullptr,  &::FreeImage_Unload };

	switch (standard_version) {
	case FICPARAM_YUV_STANDARD_JPEG:
		result = ConvertImpl(dib, YuvToRgbConverter<YuvJPEG>{});
		break;
	default:
		return nullptr;
	}

	return result.release();
}
#else
FIBITMAP* ConvertRgbToYuv(FIBITMAP* dib, int64_t standard_version)
{
	return nullptr;
}


FIBITMAP* ConvertYuvToRgb(FIBITMAP* dib, int64_t standard_version)
{
	return nullptr;
}
#endif
