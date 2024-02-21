//===========================================================
// FreeImage Re(surrected)
// Modified fork from the original FreeImage 3.18
// with updated dependencies and extended features.
//===========================================================

#include "FreeImage.h"
#include "SimpleTools.h"
#include <algorithm>
#include <memory>

#if __cplusplus >= 201703L
template <typename YuvStandard_>
FIBOOL TmoLinearImpl(FIBITMAP* dst, FIBITMAP* src, double maxValue, double minBrightness, double maxBrightness)
{
    const float black = static_cast<float>(minBrightness / maxBrightness);
    const float div   = static_cast<float>(1.0 / (1.0 - black));

    auto Clamp = [](auto v) {
        using Ty_ = decltype(v);
        return static_cast<uint8_t>(std::clamp(v * static_cast<Ty_>(256.0), static_cast<Ty_>(0.0), static_cast<Ty_>(255.0)));
    };

    auto ProcessAlpha = [div = static_cast<float>(256.0 / maxValue)](auto a) {
        return static_cast<uint8_t>(std::clamp(a * div, 0.0f, 255.0f));
    };

    auto ProcessRgb = [&, maxf32 = static_cast<float>(maxBrightness)](const auto& p) {
        FIRGBF yuv = RgbToYuv<YuvStandard_>(FIRGBF{ p.red / maxf32, p.green / maxf32, p.blue / maxf32 });
        yuv.red = (yuv.red - black) * div;
        return YuvToRgb<YuvStandard_>(yuv);
    };

    auto ProcessGrey = [&](const auto& p) {
        return (p - minBrightness) / (maxBrightness - minBrightness);
    };

    switch (FreeImage_GetImageType(src)) {
    case FIT_RGBAF:
        BitmapTransform<FIRGBA8, FIRGBAF>(dst, src, [&](const FIRGBAF& p) {
            const auto rgb = ProcessRgb(p);
            return FIRGBA8{ Clamp(rgb.red), Clamp(rgb.green), Clamp(rgb.blue), ProcessAlpha(p.alpha) };
        });
        break;
    case FIT_RGBF:
        BitmapTransform<FIRGB8, FIRGBF>(dst, src, [&](const FIRGBF& p) {
            const auto rgb = ProcessRgb(p);
            return FIRGB8{ Clamp(rgb.red), Clamp(rgb.green), Clamp(rgb.blue) };
        });
        break;
    case FIT_RGBA32:
        BitmapTransform<FIRGBA8, FIRGBA32>(dst, src, [&](const FIRGBA32& p) {
            const auto rgb = ProcessRgb(p);
            return FIRGBA8{ Clamp(rgb.red), Clamp(rgb.green), Clamp(rgb.blue), ProcessAlpha(p.alpha) };
        });
        break;
    case FIT_RGB32:
        BitmapTransform<FIRGB8, FIRGB32>(dst, src, [&](const FIRGB32& p) {
            const auto rgb = ProcessRgb(p);
            return FIRGB8{ Clamp(rgb.red), Clamp(rgb.green), Clamp(rgb.blue) };
        });
        break;
    case FIT_RGBA16:
        BitmapTransform<FIRGBA8, FIRGBA16>(dst, src, [&](const FIRGBA16& p) {
            const auto rgb = ProcessRgb(p);
            return FIRGBA8{ Clamp(rgb.red), Clamp(rgb.green), Clamp(rgb.blue), ProcessAlpha(p.alpha) };
        });
        break;
    case FIT_RGB16:
        BitmapTransform<FIRGB8, FIRGB16>(dst, src, [&](const FIRGB16& p) {
            const auto rgb = ProcessRgb(p);
            return FIRGB8{ Clamp(rgb.red), Clamp(rgb.green), Clamp(rgb.blue) };
        });
        break;
    case FIT_DOUBLE:
        BitmapTransform<uint8_t, double>(dst, src, [&](const double& p) {
            return Clamp(ProcessGrey(p));
        });
        break;
    case FIT_FLOAT:
        BitmapTransform<uint8_t, float>(dst, src, [&](const float& p) {
            return Clamp(ProcessGrey(p));
        });
        break;
    case FIT_UINT32:
        BitmapTransform<uint8_t, uint32_t>(dst, src, [&](const uint32_t& p) {
            return Clamp(ProcessGrey(p));
        });
        break;
    case FIT_UINT16:
        BitmapTransform<uint8_t, uint16_t>(dst, src, [&](const uint16_t& p) {
            return Clamp(ProcessGrey(p));
        });
        break;
    default:
        return FALSE;
    }

    return TRUE;
}


FIBITMAP* FreeImage_TmoLinear(FIBITMAP* src, double max_value, FREE_IMAGE_CVT_COLOR_PARAM yuv_standard)
{
    if (!FreeImage_HasPixels(src)) {
        return nullptr;
    }

    const auto imageType = FreeImage_GetImageType(src);
    if (imageType == FIT_BITMAP) {
        return FreeImage_Clone(src);
    }

    uint32_t dstBpp = 32;
    switch (imageType) {
    case FIT_RGBAF:
    case FIT_RGBA32:
    case FIT_RGBA16:
        dstBpp = 32;
        if (FreeImage_GetColorType2(src) != FIC_RGBALPHA) {
            // Only RGB is supported so far
            return nullptr;
        }
        break;
    case FIT_RGBF:
    case FIT_RGB32:
    case FIT_RGB16:
        dstBpp = 24;
        if (FreeImage_GetColorType2(src) != FIC_RGB) {
            // Only RGB is supported so far
            return nullptr;
        }
        break;
    case FIT_DOUBLE:
    case FIT_FLOAT:
    case FIT_UINT32:
    case FIT_UINT16:
        dstBpp = 8;
        if (FreeImage_GetColorType2(src) != FIC_MINISBLACK) {
            return nullptr;
        }
        break;
    default:
        return nullptr;
    }

    void* minPixel = nullptr, * maxPixel = nullptr;
    double minBrightness = 0.0, maxBrightness = 0.0;
    if (!FreeImage_FindMinMax(src, &minBrightness, &maxBrightness, &minPixel, &maxPixel)) {
        return nullptr;
    }

    if (minBrightness >= maxBrightness) {
        return FreeImage_TmoClamp(src, max_value);
    }

    const unsigned h = FreeImage_GetHeight(src);
    const unsigned w = FreeImage_GetWidth(src);
    std::unique_ptr<FIBITMAP, decltype(&::FreeImage_Unload)> dst(FreeImage_Allocate(w, h, dstBpp), &::FreeImage_Unload);

    FIBOOL status = FALSE;
    switch (yuv_standard) {
    case FICPARAM_YUV_STANDARD_JPEG:
        status = TmoLinearImpl<YuvJPEG>(dst.get(), src, max_value, minBrightness, maxBrightness);
        break;
    default:
        break;
    };

    if (!status) {
        return nullptr;
    }

    return dst.release();

}
#else
FIBITMAP* FreeImage_TmoLinear(FIBITMAP* src, double max_value, FREE_IMAGE_CVT_COLOR_PARAM yuv_standard)
{
	return nullptr;
}
#endif
