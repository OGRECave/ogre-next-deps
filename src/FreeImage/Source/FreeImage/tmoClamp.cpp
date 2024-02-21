//===========================================================
// FreeImage Re(surrected)
// Modified fork from the original FreeImage 3.18
// with updated dependencies and extended features.
//===========================================================

#include "FreeImage.h"
#include "SimpleTools.h"
#include <algorithm>
#include <memory>
#include <limits>

FIBITMAP* FreeImage_TmoClamp(FIBITMAP* src, double max_value)
{
#if __cplusplus >= 201703L
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
        break;
    case FIT_RGBF:
    case FIT_RGB32:
    case FIT_RGB16:
        dstBpp = 24;
        break;
    case FIT_DOUBLE:
    case FIT_FLOAT:
    case FIT_UINT32:
    case FIT_UINT16:
        dstBpp = 8;
        break;
    default:
        return nullptr;
    }

    if (max_value <= 0.0) {
        switch (imageType) {
        case FIT_RGBAF:
        case FIT_RGBF:
        case FIT_DOUBLE:
        case FIT_FLOAT:
            max_value = 1.0;
            break;
        case FIT_RGBA32:
        case FIT_RGB32:
        case FIT_UINT32:
            max_value = static_cast<double>(std::numeric_limits<uint32_t>::max());
            break;
        case FIT_RGBA16:
        case FIT_RGB16:
        case FIT_UINT16:
            max_value = static_cast<double>(std::numeric_limits<uint16_t>::max());
            break;
        case FIT_BITMAP:
            max_value = std::pow(2.0, FreeImage_GetBPP(src)) - 1.0;
            break;
        default:
            return nullptr;
        }
    }

    auto ClampFloat = [](auto v) {
        return static_cast<uint8_t>(std::clamp(v * 256.0, 0.0, 255.0));
    };

    auto ClampInt = [div = 256.0 / max_value](auto v) {
        return static_cast<uint8_t>(std::clamp(v * div, 0.0, 255.0));
    };

    const unsigned h = FreeImage_GetHeight(src);
    const unsigned w = FreeImage_GetWidth(src);
    std::unique_ptr<FIBITMAP, decltype(&::FreeImage_Unload)> dst(FreeImage_Allocate(w, h, dstBpp), &::FreeImage_Unload);

    switch (imageType) {
    case FIT_RGBAF:
        BitmapTransform<FIRGBA8, FIRGBAF>(dst.get(), src, [&](const FIRGBAF& p) {
            return FIRGBA8{ ClampFloat(p.red), ClampFloat(p.green), ClampFloat(p.blue), ClampFloat(p.alpha) };
        });
        break;
    case FIT_RGBF:
        BitmapTransform<FIRGB8, FIRGBF>(dst.get(), src, [&](const FIRGBF& p) {
            return FIRGB8{ ClampFloat(p.red), ClampFloat(p.green), ClampFloat(p.blue) };
        });
        break;
    case FIT_RGBA32:
        BitmapTransform<FIRGBA8, FIRGBA32>(dst.get(), src, [&](const FIRGBA32& p) {
            return FIRGBA8{ ClampInt(p.red), ClampInt(p.green), ClampInt(p.blue), ClampInt(p.alpha) };
        });
        break;
    case FIT_RGB32:
        BitmapTransform<FIRGB8, FIRGB32>(dst.get(), src, [&](const FIRGB32& p) {
            return FIRGB8{ ClampInt(p.red), ClampInt(p.green), ClampInt(p.blue) };
        });
        break;
    case FIT_RGBA16:
        BitmapTransform<FIRGBA8, FIRGBA16>(dst.get(), src, [&](const FIRGBA16& p) {
            return FIRGBA8{ ClampInt(p.red), ClampInt(p.green), ClampInt(p.blue), ClampInt(p.alpha) };
        });
        break;
    case FIT_RGB16:
        BitmapTransform<FIRGB8, FIRGB16>(dst.get(), src, [&](const FIRGB16& p) {
            return FIRGB8{ ClampInt(p.red), ClampInt(p.green), ClampInt(p.blue) };
        });
        break;
    case FIT_DOUBLE:
        BitmapTransform<uint8_t, double>(dst.get(), src, [&](const double& p) {
            return ClampFloat(p);
        });
        break;
    case FIT_FLOAT:
        BitmapTransform<uint8_t, float>(dst.get(), src, [&](const float& p) {
            return ClampFloat(p);
        });
        break;
    case FIT_UINT32:
        BitmapTransform<uint8_t, uint32_t>(dst.get(), src, [&](const uint32_t& p) {
            return ClampInt(p);
        });
        break;
    case FIT_UINT16:
        BitmapTransform<uint8_t, uint16_t>(dst.get(), src, [&](const uint16_t& p) {
            return ClampInt(p);
        });
        break;
    default:
        return nullptr;
    }

    return dst.release();
#else
	return nullptr;
#endif
}
