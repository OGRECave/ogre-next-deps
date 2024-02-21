//===========================================================
// FreeImage Re(surrected)
// Modified fork from the original FreeImage 3.18
// with updated dependencies and extended features.
//===========================================================

#ifndef FREEIMAGE_CONVERSION_YUV_H_
#define FREEIMAGE_CONVERSION_YUV_H_

#include "FreeImage.h"
#include <type_traits>

#if __cplusplus >= 201703L

// ----------------------------------------------------------
//  internal conversions from RGB to YUV
// ----------------------------------------------------------



/**
 * Conversion according to JPEG sstandard, T-REC-T.871
 */
struct YuvJPEG
{
	static constexpr uint32_t kDefaultFractionalBits = 12;

	template <uint32_t Qb_>
	static constexpr int32_t MakeFxPoint(double val)
	{
		static_assert(Qb_ > 0, "Fractional bits number must be more than 0");
		return static_cast<int32_t>(val * (0x1 << Qb_));
	}

	template <uint32_t Qb_>
	static constexpr int32_t FxPointDivide(int32_t val)
	{
		static_assert(Qb_ > 1, "Fractional bits number must be more than 1");
		return (val + (0x1 << (Qb_ - 1))) >> Qb_;
	}

	template <typename Ty_>
	static constexpr std::enable_if_t<std::is_same_v<std::remove_cv_t<Ty_>, uint8_t>, int32_t> MakeChromaOffset()
	{
		return 0x1 << 7;
	}

	template <typename Ty_>
	static constexpr std::enable_if_t<std::is_same_v<std::remove_cv_t<Ty_>, uint16_t>, int32_t> MakeChromaOffset()
	{
		return 0x1 << 15;
	}

	template <uint32_t Qb_ = kDefaultFractionalBits, typename Ty_>
	static constexpr std::enable_if_t<!std::is_floating_point_v<Ty_>, Ty_> R(Ty_ y, Ty_ u, Ty_ v)
	{
		return static_cast<Ty_>(FxPointDivide<Qb_>(MakeFxPoint<Qb_>(y) + MakeFxPoint<Qb_>(1.402) * (v - MakeChromaOffset<Ty_>())));
	}

	template <uint32_t Qb_ = kDefaultFractionalBits, typename Ty_>
	static constexpr std::enable_if_t<!std::is_floating_point_v<Ty_>, Ty_> G(Ty_ y, Ty_ u, Ty_ v)
	{
		return static_cast<Ty_>(FxPointDivide<Qb_>(MakeFxPoint<Qb_>(y) - MakeFxPoint<Qb_>(0.344136) * (u - MakeChromaOffset<Ty_>()) - MakeFxPoint<Qb_>(0.714136) * (v - MakeChromaOffset<Ty_>())));
	}

	template <uint32_t Qb_ = kDefaultFractionalBits, typename Ty_>
	static constexpr std::enable_if_t<!std::is_floating_point_v<Ty_>, Ty_> B(Ty_ y, Ty_ u, Ty_ v)
	{
		return static_cast<Ty_>(FxPointDivide<Qb_>(MakeFxPoint<Qb_>(y) + MakeFxPoint<Qb_>(1.772) * (u - MakeChromaOffset<Ty_>())));
	}


	template <uint32_t Qb_ = kDefaultFractionalBits, typename Ty_>
	static constexpr std::enable_if_t<!std::is_floating_point_v<Ty_>, Ty_> Y(Ty_ r, Ty_ g, Ty_ b)
	{
		return static_cast<Ty_>(FxPointDivide<Qb_>(MakeFxPoint<Qb_>(0.114) * b + MakeFxPoint<Qb_>(0.587) * g + MakeFxPoint<Qb_>(0.299) * r));
	}

	template <uint32_t Qb_ = kDefaultFractionalBits, typename Ty_>
	static constexpr std::enable_if_t<!std::is_floating_point_v<Ty_>, Ty_> U(Ty_ r, Ty_ g, Ty_ b)
	{
		return static_cast<Ty_>(FxPointDivide<Qb_>(MakeFxPoint<Qb_>(0.5) * b - MakeFxPoint<Qb_>(0.331264) * g - MakeFxPoint<Qb_>(0.168736) * r) + MakeChromaOffset<Ty_>());
	}

	template <uint32_t Qb_ = kDefaultFractionalBits, typename Ty_>
	static constexpr std::enable_if_t<!std::is_floating_point_v<Ty_>, Ty_> V(Ty_ r, Ty_ g, Ty_ b)
	{
		return static_cast<Ty_>(FxPointDivide<Qb_>(MakeFxPoint<Qb_>(0.5) * r - MakeFxPoint<Qb_>(0.418688) * g - MakeFxPoint<Qb_>(0.081312) * b) + MakeChromaOffset<Ty_>());
	}


	template <typename FTy_>
	static constexpr std::enable_if_t<std::is_floating_point_v<FTy_>, FTy_> R(FTy_ y, FTy_ u, FTy_ v)
	{
		return y + static_cast<FTy_>(1.402) * (v - static_cast<FTy_>(0.5));
	}

	template <typename FTy_>
	static constexpr std::enable_if_t<std::is_floating_point_v<FTy_>, FTy_> G(FTy_ y, FTy_ u, FTy_ v)
	{
		return y - static_cast<FTy_>(0.344136) * (u - static_cast<FTy_>(0.5)) - static_cast<FTy_>(0.714136) * (v - static_cast<FTy_>(0.5));
	}

	template <typename FTy_>
	static constexpr std::enable_if_t<std::is_floating_point_v<FTy_>, FTy_> B(FTy_ y, FTy_ u, FTy_ v)
	{
		return y + static_cast<FTy_>(1.772) * (u - static_cast<FTy_>(0.5));
	}


	template <typename FTy_>
	static constexpr std::enable_if_t<std::is_floating_point_v<FTy_>, FTy_> Y(FTy_ r, FTy_ g, FTy_ b)
	{
		return static_cast<FTy_>(0.114) * b + static_cast<FTy_>(0.587) * g + static_cast<FTy_>(0.299) * r;
	}

	template <typename FTy_>
	static constexpr std::enable_if_t<std::is_floating_point_v<FTy_>, FTy_> U(FTy_ r, FTy_ g, FTy_ b)
	{
		return static_cast<FTy_>(0.5) * b - static_cast<FTy_>(0.331264) * g - static_cast<FTy_>(0.168736) * r + static_cast<FTy_>(0.5);
	}

	template <typename FTy_>
	static constexpr std::enable_if_t<std::is_floating_point_v<FTy_>, FTy_> V(FTy_ r, FTy_ g, FTy_ b)
	{
		return static_cast<FTy_>(0.5) * r - static_cast<FTy_>(0.418688) * g - static_cast<FTy_>(0.081312) * b + static_cast<FTy_>(0.5);
	}

};


template <typename YuvStandard_>
constexpr FIRGBAF YuvToRgb(const FIRGBAF& p) {
	FIRGBAF res{
		YuvStandard_::R(p.red, p.green, p.blue),
		YuvStandard_::G(p.red, p.green, p.blue),
		YuvStandard_::B(p.red, p.green, p.blue),
		p.alpha
	};
	return res;
}

template <typename YuvStandard_>
constexpr FIRGBF YuvToRgb(const FIRGBF& p) {
	FIRGBF res{
		YuvStandard_::R(p.red, p.green, p.blue),
		YuvStandard_::G(p.red, p.green, p.blue),
		YuvStandard_::B(p.red, p.green, p.blue)
	};
	return res;
}

template <typename YuvStandard_>
constexpr FIRGBA8 YuvToRgb(const FIRGBA8& p) {
	FIRGBA8 res{
		YuvStandard_::R(p.red, p.green, p.blue),
		YuvStandard_::G(p.red, p.green, p.blue),
		YuvStandard_::B(p.red, p.green, p.blue),
		p.alpha
	};
	return res;
}

template <typename YuvStandard_>
constexpr FIRGB8 YuvToRgb(const FIRGB8& p) {
	FIRGB8 res{
		YuvStandard_::R(p.red, p.green, p.blue),
		YuvStandard_::G(p.red, p.green, p.blue),
		YuvStandard_::B(p.red, p.green, p.blue)
	};
	return res;
}

template <typename YuvStandard_>
constexpr FIRGBAF RgbToYuv(const FIRGBAF& p) {
	FIRGBAF res{
		YuvStandard_::Y(p.red, p.green, p.blue),
		YuvStandard_::U(p.red, p.green, p.blue),
		YuvStandard_::V(p.red, p.green, p.blue),
		p.alpha
	};
	return res;
}

template <typename YuvStandard_>
constexpr FIRGBF RgbToYuv(const FIRGBF& p) {
	FIRGBF res{
		YuvStandard_::Y(p.red, p.green, p.blue),
		YuvStandard_::U(p.red, p.green, p.blue),
		YuvStandard_::V(p.red, p.green, p.blue)
	};
	return res;
}

template <typename YuvStandard_>
constexpr FIRGBA8 RgbToYuv(const FIRGBA8& p) {
	FIRGBA8 res{
		YuvStandard_::Y(p.red, p.green, p.blue),
		YuvStandard_::U(p.red, p.green, p.blue),
		YuvStandard_::V(p.red, p.green, p.blue),
		p.alpha
	};
	return res;
}

template <typename YuvStandard_>
constexpr FIRGB8 RgbToYuv(const FIRGB8& p) {
	FIRGB8 res{
		YuvStandard_::Y(p.red, p.green, p.blue),
		YuvStandard_::U(p.red, p.green, p.blue),
		YuvStandard_::V(p.red, p.green, p.blue)
	};
	return res;
}



#endif // __cplusplus >= 201703L

FIBITMAP* ConvertRgbToYuv(FIBITMAP* dib, int64_t standard_version);

FIBITMAP* ConvertYuvToRgb(FIBITMAP* dib, int64_t standard_version);

#endif //FREEIMAGE_CONVERSION_YUV_H_
