//===========================================================
// FreeImage Re(surrected)
// Modified fork from the original FreeImage 3.18
// with updated dependencies and extended features.
//===========================================================

#ifndef FREEIMAGE_SIMPLE_TOOLS_H_
#define FREEIMAGE_SIMPLE_TOOLS_H_

#include "FreeImage.h"
#include <cmath>
#include <tuple>
#include <type_traits>
#include "ConversionYUV.h"


template <typename DstPixel_, typename SrcPixel_ = DstPixel_, typename PixelVisitor_>
void BitmapForEach(FIBITMAP* src, PixelVisitor_ vis)
{
    const unsigned width = FreeImage_GetWidth(src);
    const unsigned height = FreeImage_GetHeight(src);
    const unsigned src_pitch = FreeImage_GetPitch(src);

    const uint8_t* src_bits = FreeImage_GetBits(src);
    for (unsigned y = 0; y < height; ++y) {
        auto src_pixel = static_cast<const SrcPixel_*>(static_cast<const void*>(src_bits));
        for (unsigned x = 0; x < width; ++x) {
            vis(src_pixel[x], x, y);
        }
        src_bits += src_pitch;
    }
}

template <typename DstPixel_, typename SrcPixel_ = DstPixel_, typename UnaryOperation_>
void BitmapTransform(FIBITMAP* dst, FIBITMAP* src, UnaryOperation_ unary_op)
{
	const unsigned width = FreeImage_GetWidth(src);
	const unsigned height = FreeImage_GetHeight(src);
	const unsigned src_pitch = FreeImage_GetPitch(src);
	const unsigned dst_pitch = FreeImage_GetPitch(dst);

	const uint8_t* src_bits = FreeImage_GetBits(src);
	uint8_t* dst_bits = FreeImage_GetBits(dst);

	for (unsigned y = 0; y < height; ++y) {
		auto src_pixel = static_cast<const SrcPixel_*>(static_cast<const void*>(src_bits));
		auto dst_pixel = static_cast<DstPixel_*>(static_cast<void*>(dst_bits));
		for (unsigned x = 0; x < width; ++x) {
			dst_pixel[x] = unary_op(src_pixel[x]);
		}
		src_bits += src_pitch;
		dst_bits += dst_pitch;
	}
}

#if __cplusplus >= 201703L

template <typename Ty_>
using IsIntPixelType = std::integral_constant<bool,
    std::is_same_v<Ty_, FIRGB8> ||
    std::is_same_v<Ty_, FIRGBA8> ||
    std::is_same_v<Ty_, FIRGB16> ||
    std::is_same_v<Ty_, FIRGBA16> ||
    std::is_same_v<Ty_, FIRGB32> ||
    std::is_same_v<Ty_, FIRGBA32>
>;

template <typename Ty_>
using IsFloatPixelType = std::integral_constant<bool,
    std::is_same_v<Ty_, FIRGBF> ||
    std::is_same_v<Ty_, FIRGBAF>
>;

template <typename Ty_>
using IsPixelType = std::integral_constant<bool, IsIntPixelType<Ty_>::value || IsFloatPixelType<Ty_>::value>;

namespace details
{
    template <typename PixelType_>
    struct ToValueTypeImpl {};

    template <> struct ToValueTypeImpl<FIRGBAF>    { using type = float;    };
    template <> struct ToValueTypeImpl<FIRGBF>     { using type = float;    };
    template <> struct ToValueTypeImpl<FIRGBA32>   { using type = uint32_t; };
    template <> struct ToValueTypeImpl<FIRGB32>    { using type = uint32_t; };
    template <> struct ToValueTypeImpl<FIRGBA16>   { using type = uint16_t; };
    template <> struct ToValueTypeImpl<FIRGB16>    { using type = uint16_t; };
    template <> struct ToValueTypeImpl<FIRGBA8>    { using type = uint8_t;  };
    template <> struct ToValueTypeImpl<FIRGB8>     { using type = uint8_t;  };
    template <> struct ToValueTypeImpl<FICOMPLEX>  { using type = double;   };
    template <> struct ToValueTypeImpl<FICOMPLEXF> { using type = float;    };
    template <> struct ToValueTypeImpl<double>     { using type = double;   };
    template <> struct ToValueTypeImpl<float>      { using type = float;    };
    template <> struct ToValueTypeImpl<uint32_t>   { using type = uint32_t; };
    template <> struct ToValueTypeImpl<int32_t>    { using type = int32_t;  };
    template <> struct ToValueTypeImpl<uint16_t>   { using type = uint16_t; };
    template <> struct ToValueTypeImpl<int16_t>    { using type = int16_t;  };
    template <> struct ToValueTypeImpl<uint8_t>    { using type = uint8_t;  };
}

template <typename PixelType_>
using ToValueType = typename details::ToValueTypeImpl<PixelType_>::type;


namespace details
{
    template <typename Ty_>
    struct ToWiderTypeImpl {};

    template <> struct ToWiderTypeImpl<uint64_t> { using type = uint64_t; };
    template <> struct ToWiderTypeImpl<int64_t>  { using type = int64_t; };
    template <> struct ToWiderTypeImpl<uint32_t> { using type = uint64_t; };
    template <> struct ToWiderTypeImpl<int32_t>  { using type = int64_t;  };
    template <> struct ToWiderTypeImpl<uint16_t> { using type = uint32_t; };
    template <> struct ToWiderTypeImpl<int16_t>  { using type = int32_t;  };
    template <> struct ToWiderTypeImpl<uint8_t>  { using type = uint32_t;  };
    template <> struct ToWiderTypeImpl<int8_t>   { using type = int32_t; };
}

template <typename Ty_>
using ToWiderType = typename details::ToWiderTypeImpl<Ty_>::type;


namespace details
{
    template <typename Ty_>
    struct ToUnsignedTypeImpl {};

    template <> struct ToUnsignedTypeImpl<uint64_t> { using type = uint64_t; };
    template <> struct ToUnsignedTypeImpl<int64_t>  { using type = uint64_t; };
    template <> struct ToUnsignedTypeImpl<uint32_t> { using type = uint32_t; };
    template <> struct ToUnsignedTypeImpl<int32_t>  { using type = uint32_t; };
    template <> struct ToUnsignedTypeImpl<uint16_t> { using type = uint16_t; };
    template <> struct ToUnsignedTypeImpl<int16_t>  { using type = uint16_t; };
    template <> struct ToUnsignedTypeImpl<uint8_t>  { using type = uint8_t;  };
    template <> struct ToUnsignedTypeImpl<int8_t>   { using type = uint8_t;  };
}

template <typename Ty_>
using ToUnsignedType = typename details::ToUnsignedTypeImpl<Ty_>::type;



template <uint32_t Value_>
using uint32_constant = std::integral_constant<uint32_t, Value_>;

template <typename PixelType_>
struct PixelChannelsNumber : public uint32_constant<1>{};

template <> struct PixelChannelsNumber<FIRGBAF>  : public uint32_constant<4> {};
template <> struct PixelChannelsNumber<FIRGBF>   : public uint32_constant<3> {};
template <> struct PixelChannelsNumber<FIRGBA32> : public uint32_constant<4> {};
template <> struct PixelChannelsNumber<FIRGB32>  : public uint32_constant<3> {};
template <> struct PixelChannelsNumber<FIRGBA16> : public uint32_constant<4> {};
template <> struct PixelChannelsNumber<FIRGB16>  : public uint32_constant<3> {};
template <> struct PixelChannelsNumber<FIRGBA8>  : public uint32_constant<4> {};
template <> struct PixelChannelsNumber<FIRGB8>   : public uint32_constant<3> {};
template <> struct PixelChannelsNumber<FICOMPLEX> : public uint32_constant<2> {};
template <> struct PixelChannelsNumber<FICOMPLEXF> : public uint32_constant<2> {};

namespace details
{
    template <typename PixelType_>
    struct ToNoAlphaTypeImpl
    {
        using type = PixelType_;
    };

    template <> struct ToNoAlphaTypeImpl<FIRGBAF>  { using type = FIRGBF; };
    template <> struct ToNoAlphaTypeImpl<FIRGBA32> { using type = FIRGB32; };
    template <> struct ToNoAlphaTypeImpl<FIRGBA16> { using type = FIRGB16; };
    template <> struct ToNoAlphaTypeImpl<FIRGBA8>  { using type = FIRGB8; };

}

template <typename PixelType_>
using ToNoAlphaType = typename details::ToNoAlphaTypeImpl<PixelType_>::type;



template <uint32_t ChannelIndex_, typename PixelType_>
inline
void SetChannel(PixelType_& p, ToValueType<PixelType_> v)
{
    if constexpr (ChannelIndex_ < PixelChannelsNumber<PixelType_>::value) {
        static_cast<ToValueType<PixelType_>*>(static_cast<void*>(&p))[ChannelIndex_] = std::move(v);
    }
}

template <uint32_t ChannelIndex_, typename PixelType_>
inline
ToValueType<PixelType_> GetChannel(const PixelType_& p)
{
    if constexpr (ChannelIndex_ < PixelChannelsNumber<PixelType_>::value) {
        return static_cast<const ToValueType<PixelType_>*>(static_cast<const void*>(&p))[ChannelIndex_];
    }
    else {
        return ToValueType<PixelType_>{};
    }
}

template <typename PixelType_>
inline
void PixelFill(PixelType_& p, const ToValueType<PixelType_>& v)
{
    SetChannel<0>(p, v);
    SetChannel<1>(p, v);
    SetChannel<2>(p, v);
    SetChannel<3>(p, v);
}

template <typename PixelType_, typename BinaryOperation_>
inline
auto PixelReduce(const PixelType_& p, ToValueType<PixelType_> init, BinaryOperation_&& op)
{
    constexpr uint32_t channelsNumber = PixelChannelsNumber<PixelType_>::value;
    if constexpr (channelsNumber > 0) {
        init = op(std::move(init), GetChannel<0>(p));
    }
    if constexpr (channelsNumber > 1) {
        init = op(std::move(init), GetChannel<1>(p));
    }
    if constexpr (channelsNumber > 2) {
        init = op(std::move(init), GetChannel<2>(p));
    }
    if constexpr (channelsNumber > 3) {
        init = op(std::move(init), GetChannel<3>(p));
    }
    return init;
}

template <typename PixelType_>
inline
auto PixelMin(const PixelType_& p, ToValueType<PixelType_> init = std::numeric_limits<ToValueType<PixelType_>>::max())
{
    return PixelReduce(p, init, [](const auto& lhs, const auto& rhs) { return std::min(lhs, rhs); });
}

template <typename PixelType_>
inline
auto PixelMax(const PixelType_& p, ToValueType<PixelType_> init = std::numeric_limits<ToValueType<PixelType_>>::lowest())
{
    return PixelReduce(p, init, [](const auto& lhs, const auto& rhs) { return std::max(lhs, rhs); });
}

template <typename PixelType_>
inline
auto StripAlpha(PixelType_&& p)
{
    return ToNoAlphaType<PixelType_>(std::forward<PixelType_>(p));
}





inline
bool IsNan(float p) {
    return std::isnan(p);
}

inline
bool IsNan(double p) {
    return std::isnan(p);
}

inline
bool IsNan(FIRGBF p) {
    return std::isnan(p.red) || std::isnan(p.green) || std::isnan(p.blue);
}

inline
bool IsNan(FIRGBAF p) {
    // Ignore alpha channel, since color can be computed without it...
    return std::isnan(p.red) || std::isnan(p.green) || std::isnan(p.blue);
}

template <typename Ty_>
inline constexpr
std::enable_if_t<std::is_integral_v<Ty_> || IsIntPixelType<Ty_>::value, bool> IsNan(Ty_ p)
{
    return false;
}



struct Brightness
{
    template <typename Ty_>
    inline
    auto operator()(const Ty_& p, std::enable_if_t<std::is_floating_point_v<Ty_> || std::is_integral_v<Ty_>, void*> = nullptr)
    {
        return p;
    }

    template <typename Ty_>
    inline
    auto operator()(const Ty_& p, std::enable_if_t<IsPixelType<Ty_>::value, void*> = nullptr)
    {
        return YuvJPEG::Y(p.red, p.green, p.blue);
    }
};

struct YuvBrightness
{
    template <typename Ty_>
    inline
    auto operator()(const Ty_& p, std::enable_if_t<IsPixelType<Ty_>::value, void*> = nullptr)
    {
        return p.red;
    }
};


template <typename PixelTy_, typename BrightnessOp_ = Brightness>
std::tuple<PixelTy_*, PixelTy_*, double, double> FindMinMax(FIBITMAP* src, BrightnessOp_ brightnessOp = BrightnessOp_{})
{
    PixelTy_* minIt = nullptr, * maxIt = nullptr;
    double minVal = 0.0, maxVal = 0.0;
    if (src) {
        const unsigned h = FreeImage_GetHeight(src);
        const unsigned w = FreeImage_GetWidth(src);
        const unsigned pitch = FreeImage_GetPitch(src);
        auto srcLine = static_cast<uint8_t*>(static_cast<void*>(FreeImage_GetBits(src)));
        for (unsigned j = 0; j < h; ++j, srcLine += pitch) {
            auto pixIt = static_cast<PixelTy_*>(static_cast<void*>(srcLine));
            for (unsigned i = 0; i < w; ++i, ++pixIt) {
                if (IsNan(*pixIt)) {
                    continue;
                }
                const auto b = static_cast<double>(brightnessOp(*pixIt));
                if (minIt == nullptr || maxIt == nullptr) {
                    minIt  = maxIt = pixIt;
                    minVal = maxVal = b;
                }
                else {
                    if (b < minVal) {
                        minIt = pixIt;
                        minVal = b;
                    }
                    if (maxVal < b) {
                        maxIt = pixIt;
                        maxVal = b;
                    }
                }
            }
        }
    }
    return std::make_tuple(minIt, maxIt, minVal, maxVal);
}

#endif


#endif //FREEIMAGE_SIMPLE_TOOLS_H_
