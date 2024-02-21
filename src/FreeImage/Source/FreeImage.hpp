//===========================================================
// FreeImage Re(surrected)
// Modified fork from the original FreeImage 3.18
// with updated dependencies and extended features.
//
// See changes in ReadMe.md
// 
//===========================================================

#ifndef FREEIMAGE_HPP
#define FREEIMAGE_HPP

#include <cassert>
#include <filesystem>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include "FreeImage.h"

namespace fi
{

    namespace details
    {
        template <typename Ty_, typename Uy_>
        struct is_same_signedness
            : std::integral_constant<bool, std::is_signed<Ty_>::value == std::is_signed<Uy_>::value>
        { };

        template <typename Ty_, typename Uy_, typename = void>
        struct check_different_sign_narrowing
        {
            static constexpr
            bool apply(Ty_, Uy_)
            {
                return true;
            }
        };

        template <typename Ty_, typename Uy_>
        struct check_different_sign_narrowing<
            Ty_, Uy_,
            std::enable_if_t<!is_same_signedness<Ty_, Uy_>::value>
        >
        {
            static constexpr
            bool apply(Ty_ t, Uy_ u)
            {
                return ((t < static_cast<Ty_>(0)) == (u < static_cast<Uy_>(0)));
            }
        };

        template <typename TypeTo_, typename TypeFrom_>
        constexpr inline
        auto narrow_cast(const TypeFrom_& val)
            -> std::enable_if_t<std::is_integral<TypeTo_>::value && std::is_integral<TypeFrom_>::value, TypeTo_>
        {
            assert((val == static_cast<TypeFrom_>(static_cast<TypeTo_>(val))) && "yato::narrow_cast failed!");
            assert(((details::check_different_sign_narrowing<TypeTo_, TypeFrom_>::apply(static_cast<TypeTo_>(val), val))) && "yato::narrow_cast failed!");
            return static_cast<TypeTo_>(val);
        }

        template <typename T>
        constexpr inline
        bool is_uninitialized(std::weak_ptr<T> const& weak) {
            using WT_ = std::weak_ptr<T>;
            return !weak.owner_before(WT_{}) && !WT_{}.owner_before(weak);
        }

    } // namesapce details


    enum class ImageFormat
    {
        eUnknown = FIF_UNKNOWN,
        eBmp     = FIF_BMP,
        eIco     = FIF_ICO,
        eJpeg     = FIF_JPEG,
        eJng     = FIF_JNG,
        eKoala   = FIF_KOALA,
        eLbm     = FIF_LBM,
        eIff     = FIF_IFF,
        eMng     = FIF_MNG,
        ePbm     = FIF_PBM,
        ePbmRaw  = FIF_PBMRAW,
        ePcd     = FIF_PCD,
        ePcx     = FIF_PCX,
        ePgm     = FIF_PGM,
        ePgmRaw  = FIF_PGMRAW,
        ePng     = FIF_PNG,
        ePpm     = FIF_PPM,
        ePpmRaw  = FIF_PPMRAW,
        eRas     = FIF_RAS,
        eTarga   = FIF_TARGA,
        eTiff    = FIF_TIFF,
        eWbmp    = FIF_WBMP,
        ePsd     = FIF_PSD,
        eCut     = FIF_CUT,
        eXbm     = FIF_XBM,
        eXpm     = FIF_XPM,
        eDds     = FIF_DDS,
        eGif     = FIF_GIF,
        eHdr     = FIF_HDR,
        eFaxg3   = FIF_FAXG3,
        eSgi     = FIF_SGI,
        eExr     = FIF_EXR,
        eJ2k     = FIF_J2K,
        eJp2     = FIF_JP2,
        ePfm     = FIF_PFM,
        ePict    = FIF_PICT,
        eRaw     = FIF_RAW,
        eWebp    = FIF_WEBP,
        eJxr     = FIF_JXR
    };

    enum class ImageType
    {
        eUnknown  = FIT_UNKNOWN,
        eBitmap   = FIT_BITMAP,
        eUInt16   = FIT_UINT16,
        eInt16    = FIT_INT16,
        eUint32   = FIT_UINT32,
        eInt32    = FIT_INT32,
        eFloat    = FIT_FLOAT,
        eDouble   = FIT_DOUBLE,
        eComplexF = FIT_COMPLEXF,
        eComplex  = FIT_COMPLEX,
        eRgb16    = FIT_RGB16,
        eRgba16   = FIT_RGBA16,
        eRgbF     = FIT_RGBF,
        eRgbaF    = FIT_RGBAF,
        eRgb32    = FIT_RGB32,
        eRgba32   = FIT_RGBA32
    };

    enum class ColorType
    {
        eMinIsWhite = FIC_MINISWHITE,
        eMinIsBlack = FIC_MINISBLACK,
        eRgb        = FIC_RGB,
        ePalette    = FIC_PALETTE,
        eRgbAlpha   = FIC_RGBALPHA,
        eCmyk       = FIC_CMYK,
        eYuv        = FIC_YUV
    };

    enum class QuantizationAlgorithm
    {
        eWu  = FIQ_WUQUANT,
        eNeu = FIQ_NNQUANT,
        eLFP = FIQ_LFPQUANT
    };

    enum class DitherAlgorithm
    {
        eFS           = FID_FS,
        eBayer4x4     = FID_BAYER4x4,
        eBayer8x8     = FID_BAYER8x8,
        eBayer16x16   = FID_BAYER16x16,
        eCluster6x6   = FID_CLUSTER6x6,
        eCluster8x8   = FID_CLUSTER8x8,
        eCluster16x16 = FID_CLUSTER16x16
    };

    enum class ToneMappingAlgorithm
    {
        eClamp      = FITMO_CLAMP,
        eLinear     = FITMO_LINEAR,
        eDrago03    = FITMO_DRAGO03,
        eReinhard05 = FITMO_REINHARD05,
        eFattal02   = FITMO_FATTAL02
    };

    enum class CvtColorParameter
    {
        eYuvStandardDefault = FICPARAM_YUV_STANDARD_DEFAULT,
        eYuvStandardJpeg    = FICPARAM_YUV_STANDARD_JPEG
    };

    enum class MetadataModel
    {
        eNoData        = FIMD_NODATA,
        eComments      = FIMD_COMMENTS,
        eExifMain      = FIMD_EXIF_MAIN,
        eExifExif      = FIMD_EXIF_EXIF,
        eExifGps       = FIMD_EXIF_GPS,
        eExifMakerNote = FIMD_EXIF_MAKERNOTE,
        eExifInterop   = FIMD_EXIF_INTEROP,
        eIptc          = FIMD_IPTC,
        eXmp           = FIMD_XMP,
        eGeoTiff       = FIMD_GEOTIFF,
        eAnimation     = FIMD_ANIMATION,
        eCustom        = FIMD_CUSTOM,
        eExifRaw       = FIMD_EXIF_RAW,
    };

    enum class MetadataType
    {
        eNoType    = FIDT_NOTYPE,
        eByte      = FIDT_BYTE,
        eAscii     = FIDT_ASCII,
        eShort     = FIDT_SHORT,
        eLong      = FIDT_LONG,
        eRational  = FIDT_RATIONAL,
        eSByte     = FIDT_SBYTE,
        eUndefined = FIDT_UNDEFINED,
        eSShort    = FIDT_SSHORT,
        eSLong     = FIDT_SLONG,
        eSRational = FIDT_SRATIONAL,
        eFloat     = FIDT_FLOAT,
        eDouble    = FIDT_DOUBLE,
        eIfd       = FIDT_IFD,
        ePalette   = FIDT_PALETTE,
        eLong8     = FIDT_LONG8,
        eSLong8    = FIDT_SLONG8,
        eIfd8      = FIDT_IFD8
    };

    enum class FilterType
    {
        eBox        = FILTER_BOX,
        eBicubic    = FILTER_BICUBIC,
        eBilinear   = FILTER_BILINEAR,
        eBSpline    = FILTER_BSPLINE,
        eCatmullRom = FILTER_CATMULLROM,
        eLanczos3   = FILTER_LANCZOS3,
    };

    enum class ColorChannel
    {
        eRgb   = FICC_RGB,
        eRed   = FICC_RED,
        eGreen = FICC_GREEN,
        eBlue  = FICC_BLUE,
        eAlpha = FICC_ALPHA,
        eBlack = FICC_BLACK,
        eReal  = FICC_REAL,
        eImag  = FICC_IMAG,
        eMag   = FICC_MAG,
        ePhase = FICC_PHASE
    };

    enum class AlphaOperation
    {
        eSrcAlpha = FIAO_SrcAlpha
    };


    class ImageError
        : public std::runtime_error
    {
    public:
        template <typename Function_, typename... Args_>
        static
        auto CheckedCall(const std::string& name, Function_ f, Args_&&... args) {
            const auto r = f(std::forward<Args_>(args)...);
            if (!r) {
                throw ImageError("FreeImageRe: " + name + " failed.");
            }
            return r;
        }

        ImageError(std::string msg)
            : std::runtime_error(std::move(msg))
        { }
    };

#define FREEIMAGERE_CHECKED_CALL(Func_, ...) fi::ImageError::CheckedCall(#Func_, &::Func_, __VA_ARGS__)


    inline
    ImageFormat DetectFormat(const char* filename)
    {
        auto fif = FreeImage_GetFileType(filename);
        if (fif == FIF_UNKNOWN) {
            fif = FreeImage_GetFIFFromFilename(filename);
        }
        return static_cast<ImageFormat>(fif);
    }

    inline
    ImageFormat DetectFormat(const wchar_t* filename)
    {
        auto fif = FreeImage_GetFileTypeU(filename);
        if (fif == FIF_UNKNOWN) {
            fif = FreeImage_GetFIFFromFilenameU(filename);
        }
        return static_cast<ImageFormat>(fif);
    }

    inline
    ImageFormat DetectFormat(const std::filesystem::path& filename)
    {
        return DetectFormat(filename.c_str());
    }


    inline
    MetadataModel RequireKnownMetadata(MetadataModel model) {
        if (model == MetadataModel::eNoData) {
            throw ImageError("[RequireKnownMetadata] Metadata model must not be FIMD_NODATA");
        }
        return model;
    }

    inline
    ImageType RequireKnownType(ImageType fit) {
        if (fit == ImageType::eUnknown) {
            throw ImageError("[RequireKnownType]: Format must not be FIT_UNKNOWN");
        }
        return fit;
    }

    inline
    ImageFormat RequireKnownFormat(ImageFormat fif) {
        if (fif == ImageFormat::eUnknown) {
            throw ImageError("[RequireKnownFormat]: Format must not be FIF_UNKNOWN");
        }
        return fif;
    }



    class Tag
    {
    public:
        Tag()
            : Tag(FREEIMAGERE_CHECKED_CALL(FreeImage_CreateTag))
        { }

        Tag(const Tag& other)
            : Tag(FREEIMAGERE_CHECKED_CALL(FreeImage_CloneTag, other.NativeHandle_()))
        { }

        Tag(Tag&& other) noexcept
            : Tag(other.mHandle, other.mCallDelete)
        {
            other.mHandle = nullptr;
            other.mCallDelete = false;
        }

        ~Tag()
        {
            if (mCallDelete) {
                FreeImage_DeleteTag(mHandle);
            }
        }

        Tag& operator=(const Tag& other)
        {
            Tag{ other }.Swap(*this);
            return *this;
        }

        Tag& operator=(Tag&& other) noexcept
        {
            if (mCallDelete && mHandle) {
                FreeImage_DeleteTag(mHandle);
            }
            mHandle = other.mHandle;
            mCallDelete = other.mCallDelete;
            other.mHandle = nullptr;
            other.mCallDelete = false;
            return *this;
        }

        void Swap(Tag& other) noexcept
        {
            std::swap(mHandle, other.mHandle);
            std::swap(mCallDelete, other.mCallDelete);
        }

        const char* GetKey() const
        {
            return FreeImage_GetTagKey(NativeHandle_());
        }

        const char* GetDescription() const
        {
            return FreeImage_GetTagDescription(NativeHandle_());
        }

        uint16_t GetID() const
        {
            return FreeImage_GetTagID(NativeHandle_());
        }

        MetadataType GetType() const
        {
            return static_cast<MetadataType>(FreeImage_GetTagType(NativeHandle_()));
        }

        uint32_t GetCount() const
        {
            return FreeImage_GetTagCount(NativeHandle_());
        }

        uint32_t GetLength() const
        {
            return FreeImage_GetTagLength(NativeHandle_());
        }

        const void* GetValue() const
        {
            return FreeImage_GetTagValue(NativeHandle_());
        }

        bool SetKey(const char* key)
        {
            return FreeImage_SetTagKey(NativeHandle_(), key);
        }

        bool SetDescription(const char* description)
        {
            return FreeImage_SetTagDescription(NativeHandle_(), description);
        }

        bool SetID(uint16_t id)
        {
            return FreeImage_SetTagID(NativeHandle_(), id);
        }

        bool SetType(MetadataType type)
        {
            return FreeImage_SetTagType(NativeHandle_(), static_cast<FREE_IMAGE_MDTYPE>(type));
        }

        bool SetCount(uint32_t count)
        {
            return FreeImage_SetTagCount(NativeHandle_(), count);
        }

        bool SetLength(uint32_t length)
        {
            return FreeImage_SetTagLength(NativeHandle_(), length);
        }

        bool SetValue(const void* value)
        {
            return FreeImage_SetTagValue(NativeHandle_(), value);
        }

    private:
        template <bool>
        friend class BasicMetadataIterator;
        friend class Bitmap;

        Tag(FITAG* handle, bool callDelete = true)
            : mHandle(handle), mCallDelete(callDelete)
        { }

        FITAG* NativeHandle_() const noexcept
        {
            return mHandle;
        }

        FITAG** NativeHandlePtr_() noexcept
        {
            return &mHandle;
        }

        FITAG* mHandle;
        bool mCallDelete;
    };


    template <bool IsConst_>
    class BasicMetadataIterator
    {
        using ThisType = BasicMetadataIterator<IsConst_>;
    public:

        BasicMetadataIterator(const BasicMetadataIterator&) = delete;
        BasicMetadataIterator(BasicMetadataIterator&&) noexcept = default;

        ~BasicMetadataIterator() = default;

        BasicMetadataIterator& operator=(const BasicMetadataIterator&) = delete;
        BasicMetadataIterator& operator=(BasicMetadataIterator&&) noexcept = default;

        ThisType& Next()
        {
            if (mHandlePtr) {
                if (!FreeImage_FindNextMetadata(mHandlePtr.get(), mCurrTag.NativeHandlePtr_())) {
                    // end
                    mHandlePtr.reset();
                }
            }
            return *this;
        }

        bool HasNext() const
        {
            return (mHandlePtr != nullptr);
        }

        ThisType& operator++()
        {
            return Next();
        }

        const Tag& operator*() const
        {
            assert(mHandlePtr && "End iterator cannot be dereferneced");
            return mCurrTag;
        }

        template <bool WasConst_ = IsConst_>
        std::enable_if_t<!WasConst_, Tag&> operator*()
        {
            assert(mHandlePtr && "End iterator cannot be dereferneced");
            return mCurrTag;
        }

        MetadataModel GetModel() const
        {
            return mModel;
        }

        friend bool operator==(const ThisType& lhs, const ThisType& rhs)
        {
            assert(SameOrigin(lhs, rhs) && "Do not compare iterators from different metadata models");
            return lhs.mCurrTag.NativeHandle_() == rhs.mCurrTag.NativeHandle_();
        }

        friend bool operator!=(const ThisType& lhs, const ThisType& rhs)
        {
            assert(SameOrigin(lhs, rhs) && "Do not compare iterators from different metadata models");
            return lhs.mCurrTag.NativeHandle_() == rhs.mCurrTag.NativeHandle_();
        }

    private:
        friend class Bitmap;

        struct BeginIterTag {};
        struct EndIterTag {};

        class Deleter
        {
        public:
            void operator()(FIMETADATA* ptr) const noexcept
            {
                FreeImage_FindCloseMetadata(ptr);
            }
        };

        static
        bool SameOrigin(const ThisType& lhs, const ThisType& rhs)
        {
            return (lhs.mParent == rhs.mParent) && (lhs.mModel == rhs.mModel);
        }

        BasicMetadataIterator(BeginIterTag, std::shared_ptr<FIBITMAP> parent, MetadataModel model)
            : mParent(std::move(parent))
            , mModel(RequireKnownMetadata(model))
            , mCurrTag(nullptr, false)
            , mHandlePtr(FreeImage_FindFirstMetadata(static_cast<FREE_IMAGE_MDMODEL>(model), mParent.get(), mCurrTag.NativeHandlePtr_()), Deleter{ })
        { }

        BasicMetadataIterator(EndIterTag, std::shared_ptr<FIBITMAP> parent, MetadataModel model)
            : mParent(std::move(parent))
            , mModel(RequireKnownMetadata(model))
            , mCurrTag(nullptr, false)
            , mHandlePtr(nullptr, Deleter{ })
        { }

        std::shared_ptr<FIBITMAP> mParent;
        MetadataModel mModel;
        Tag mCurrTag;
        std::unique_ptr<FIMETADATA, Deleter> mHandlePtr;
    };



    class Bitmap
    {
        class BitmapDeleter;
    public:
        using MetadataIterator = BasicMetadataIterator<false>;
        using MetadataConstIterator = BasicMetadataIterator<true>;

        static
        Bitmap FromRawBits(uint8_t* bits, uint32_t width, uint32_t height, uint32_t pitch, uint32_t bpp, uint32_t redMask, uint32_t greenMask, uint32_t blueMask, bool topdown = false)
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertFromRawBits, bits, details::narrow_cast<int32_t>(width), details::narrow_cast<int32_t>(height), details::narrow_cast<int32_t>(pitch), bpp, redMask, greenMask, blueMask, topdown));
        }

        static
        Bitmap FromRawBits(bool copySource, uint8_t* bits, ImageType type, uint32_t width, uint32_t height, uint32_t pitch, uint32_t bpp, uint32_t redMask, uint32_t greenMask, uint32_t blueMask, bool topdown = false)
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertFromRawBitsEx, copySource, bits, static_cast<FREE_IMAGE_TYPE>(RequireKnownType(type)), details::narrow_cast<int32_t>(width), details::narrow_cast<int32_t>(height), details::narrow_cast<int32_t>(pitch), bpp, redMask, greenMask, blueMask, topdown));
        }

        Bitmap(uint32_t width, uint32_t height, uint32_t bpp, uint16_t = 0, uint32_t redMask = 0, uint32_t greenMask = 0, uint32_t blueMask = 0)
            : Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_Allocate, details::narrow_cast<int32_t>(width), details::narrow_cast<int32_t>(height), details::narrow_cast<int32_t>(bpp), redMask, greenMask, blueMask))
        { }

        Bitmap(ImageType type, uint32_t width, uint32_t height, uint32_t bpp, uint16_t = 0, uint32_t redMask = 0, uint32_t greenMask = 0, uint32_t blueMask = 0)
            : Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_AllocateT, static_cast<FREE_IMAGE_TYPE>(type), details::narrow_cast<int32_t>(width), details::narrow_cast<int32_t>(height), details::narrow_cast<int32_t>(bpp), redMask, greenMask, blueMask))
        { }

        Bitmap(ImageFormat fif, const char* filename, int flags = 0)
            : Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_Load, static_cast<FREE_IMAGE_FORMAT>(RequireKnownFormat(fif)), filename, flags))
        { }

        Bitmap(ImageFormat fif, const wchar_t* filename, int flags = 0)
            : Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_LoadU, static_cast<FREE_IMAGE_FORMAT>(RequireKnownFormat(fif)), filename, flags))
        { }

        Bitmap(ImageFormat fif, const std::filesystem::path& filename, int flags = 0)
            : Bitmap(fif, filename.c_str(), flags)
        { }

        Bitmap(ImageFormat fif, FreeImageIO* io, fi_handle handle, int flags = 0)
            : Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_LoadFromHandle, static_cast<FREE_IMAGE_FORMAT>(RequireKnownFormat(fif)), io, handle, flags))
        { }

        explicit
        Bitmap(const char* filename, int flags = 0)
            : Bitmap(DetectFormat(filename), filename, flags)
        { }

        explicit
        Bitmap(const wchar_t* filename, int flags = 0)
            : Bitmap(DetectFormat(filename), filename, flags)
        { }

        explicit
        Bitmap(const std::filesystem::path& filename, int flags = 0)
            : Bitmap(DetectFormat(filename.c_str()), filename.c_str(), flags)
        { }

        Bitmap(FreeImageIO* io, fi_handle handle, int flags = 0)
            : Bitmap(static_cast<ImageFormat>(FreeImage_GetFileTypeFromHandle(io, handle, 0)), io, handle, flags)
        { }

        template <typename Deleter_ = BitmapDeleter>
        explicit
        Bitmap(FIBITMAP* handle, Deleter_&& deleter = BitmapDeleter{})
            : mHandlePtr(handle, std::move(deleter))
        {
            if (!mHandlePtr) {
                throw ImageError("Bitmap[ctor]: FIBITMAP handle is null");
            }
        }

        Bitmap(const Bitmap& other)
            : Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_Clone, other.NativeHandle_()))
        { }

        Bitmap(Bitmap&& other) noexcept = default;

        ~Bitmap() noexcept = default;

        Bitmap& operator=(const Bitmap& other)
        {
            Bitmap copy(other);
            Swap(copy);
            return *this;
        }

        Bitmap& operator=(Bitmap&& other) noexcept = default;

        explicit
        operator FIBITMAP*()
        {
            return NativeHandle_();
        }

        explicit
        operator const FIBITMAP*() const noexcept
        {
            return NativeHandle_();
        }

        void Swap(Bitmap& other) noexcept
        {
            std::swap(mHandlePtr, other.mHandlePtr);
        }

        bool HasPixels() const
        {
            return FreeImage_HasPixels(NativeHandle_());
        }

        uint32_t GetBPP() const
        {
            return FreeImage_GetBPP(NativeHandle_());
        }

        uint32_t GetWidth() const
        {
            return FreeImage_GetWidth(NativeHandle_());
        }

        uint32_t GetHeight() const
        {
            return FreeImage_GetHeight(NativeHandle_());
        }

        uint32_t GetChannelsNumber() const
        {
            return FreeImage_GetChannelsNumber(NativeHandle_());
        }

        uint32_t GetLine() const
        {
            return FreeImage_GetLine(NativeHandle_());
        }

        uint32_t GetPitch() const
        {
            return FreeImage_GetPitch(NativeHandle_());
        }

        uint32_t GetMemorySize() const
        {
            return FreeImage_GetMemorySize(NativeHandle_());
        }

        uint8_t* GetBits()
        {
            return FreeImage_GetBits(NativeHandle_());
        }

        const uint8_t* GetBits() const
        {
            return FreeImage_GetBits(NativeHandle_());
        }

        uint8_t* GetScanLine(uint32_t scanline)
        {
            return FreeImage_GetScanLine(NativeHandle_(), details::narrow_cast<int>(scanline));
        }

        const uint8_t* GetScanLine(uint32_t scanline) const
        {
            return FreeImage_GetScanLine(NativeHandle_(), details::narrow_cast<int>(scanline));
        }

        template <typename Ty_>
        Ty_* GetScanLineAs(uint32_t scanline)
        {
            return static_cast<Ty_*>(static_cast<void*>(GetScanLine(scanline)));
        }

        template <typename Ty_>
        std::add_const_t<Ty_>* GetScanLineAs(uint32_t scanline) const
        {
            return static_cast<std::add_const_t<Ty_>*>(static_cast<const void*>(GetScanLine(scanline)));
        }

        ColorType GetColorType(bool scanAlpha = false) const
        {
            return static_cast<ColorType>(FreeImage_GetColorType2(NativeHandle_(), scanAlpha));
        }

        void ConvertToRawBits(uint8_t* bits, uint32_t pitch, uint32_t bpp, uint32_t redMask, uint32_t greenMask, uint32_t blueMask, bool topdown = false) const
        {
            FreeImage_ConvertToRawBits(bits, NativeHandle_(), details::narrow_cast<int>(pitch), bpp, redMask, greenMask, blueMask, topdown);
        }

        Bitmap ConvertTo4Bits() const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertTo4Bits, NativeHandle_()));
        }

        Bitmap ConvertTo8Bits() const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertTo8Bits, NativeHandle_()));
        }

        Bitmap ConvertToGreyscale() const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertToGreyscale, NativeHandle_()));
        }

        Bitmap ConvertTo16Bits555() const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertTo16Bits555, NativeHandle_()));
        }

        Bitmap ConvertTo16Bits565() const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertTo16Bits565, NativeHandle_()));
        }

        Bitmap ConvertTo24Bits() const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertTo24Bits, NativeHandle_()));
        }

        Bitmap ConvertTo32Bits() const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertTo32Bits, NativeHandle_()));
        }

        Bitmap ConvertToFloat(bool scaleLinear = true) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertToFloat, NativeHandle_(), scaleLinear));
        }

        Bitmap ConvertToRGBF() const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertToRGBF, NativeHandle_()));
        }

        Bitmap ConvertToRGBAF() const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertToRGBAF, NativeHandle_()));
        }

        Bitmap ConvertToUINT16() const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertToUINT16, NativeHandle_()));
        }

        Bitmap ConvertToRGB16() const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertToRGB16, NativeHandle_()));
        }

        Bitmap ConvertToRGBA16() const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertToRGBA16, NativeHandle_()));
        }

        Bitmap ConvertToStandardType(bool scaleLinear = true) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertToStandardType, NativeHandle_(), scaleLinear));
        }

        Bitmap ConvertToType(ImageType dstType, bool scaleLinear = true) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertToType, NativeHandle_(), static_cast<FREE_IMAGE_TYPE>(RequireKnownType(dstType)), scaleLinear));
        }

        Bitmap ConvertToColor(ColorType dstColor, int64_t firstParam = 0, int64_t secondParam = 0) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ConvertToColor, NativeHandle_(), static_cast<FREE_IMAGE_COLOR_TYPE>(dstColor), firstParam, secondParam));
        }

        Bitmap ColorQuantize(QuantizationAlgorithm quantize = QuantizationAlgorithm::eWu, uint32_t paletteSize = 256u, uint32_t reserveSize = 0u, FIRGBA8* reservePalette = nullptr)
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ColorQuantizeEx, NativeHandle_(), static_cast<FREE_IMAGE_QUANTIZE>(quantize), details::narrow_cast<int>(paletteSize), details::narrow_cast<int>(reserveSize), reservePalette));
        }

        Bitmap Threshold(uint8_t t) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_Threshold, NativeHandle_(), t));
        }

        Bitmap Dither(DitherAlgorithm algorithm) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_Dither, NativeHandle_(), static_cast<FREE_IMAGE_DITHER>(algorithm)));
        }

        Bitmap ToneMapping(ToneMappingAlgorithm tmo, double firstParam = 0.0, double secondParam = 0.0) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_ToneMapping, NativeHandle_(), static_cast<FREE_IMAGE_TMO>(tmo), firstParam, secondParam));
        }

        Bitmap TmoDrago03(ToneMappingAlgorithm tmo, double gamma = 2.2, double exposure = 0.0) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_TmoDrago03, NativeHandle_(), gamma, exposure));
        }

        Bitmap TmoReinhard05(ToneMappingAlgorithm tmo, double intensity = 0.0, double contrast = 0.0, double adaptation = 1.0, double colorCorrection = 0.0) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_TmoReinhard05Ex, NativeHandle_(), intensity, contrast, adaptation, colorCorrection));
        }

        Bitmap TmoFattal02(ToneMappingAlgorithm tmo, double colorSaturation = 0.5, double attenuation = 0.85) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_TmoFattal02, NativeHandle_(), colorSaturation, attenuation));
        }

        Bitmap TmoClamp(ToneMappingAlgorithm tmo, double maxValue = 0.0) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_TmoClamp, NativeHandle_(), maxValue));
        }

        Bitmap TmoLinear(ToneMappingAlgorithm tmo, double maxValue = 0.0, CvtColorParameter yuvStandard = CvtColorParameter::eYuvStandardDefault) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_TmoLinear, NativeHandle_(), maxValue, static_cast<FREE_IMAGE_CVT_COLOR_PARAM>(yuvStandard)));
        }

        bool HasThumbnail() const
        {
            return (FreeImage_GetThumbnail(NativeHandle_()) != nullptr);
        }

        Bitmap GetThumbnail() const
        {
            // Make copy of small thumbnail image in order to avoid possible pointer invalidation after SetThumbnail
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_Clone, FreeImage_GetThumbnail(NativeHandle_())));
        }

        bool SetThumbnail(const Bitmap& thumbnail) {
            return FreeImage_SetThumbnail(NativeHandle_(), thumbnail.NativeHandle_());
        }

        bool SetThumbnail() {
            return FreeImage_SetThumbnail(NativeHandle_(), nullptr);
        }

        MetadataIterator MetadataBegin(MetadataModel model)
        {
            return MetadataIterator(MetadataIterator::BeginIterTag{}, mHandlePtr, model);
        }

        MetadataIterator MetadataEnd(MetadataModel model)
        {
            return MetadataIterator(MetadataIterator::EndIterTag{}, mHandlePtr, model);
        }

        MetadataConstIterator MetadataCBegin(MetadataModel model) const
        {
            return MetadataConstIterator(MetadataConstIterator::BeginIterTag{}, mHandlePtr, model);
        }

        MetadataConstIterator MetadataCEnd(MetadataModel model) const
        {
            return MetadataConstIterator(MetadataConstIterator::EndIterTag{}, mHandlePtr, model);
        }

        bool SetMetadata(MetadataModel model, std::string_view key, const Tag& tag)
        {
            return FreeImage_SetMetadata(static_cast<FREE_IMAGE_MDMODEL>(RequireKnownMetadata(model)), NativeHandle_(), key.data(), tag.NativeHandle_());
        }

        bool SetMetadata(MetadataModel model, std::string_view key, const char* value)
        {
            return FreeImage_SetMetadataKeyValue(static_cast<FREE_IMAGE_MDMODEL>(RequireKnownMetadata(model)), NativeHandle_(), key.data(), value);
        }

        // returns tag object attached to internal tag inside bitmap metadata, that can be changed
        std::optional<Tag> GetMetadata(MetadataModel model, std::string_view key)
        {
            Tag tag{ nullptr, false };
            if (!FreeImage_GetMetadata(static_cast<FREE_IMAGE_MDMODEL>(RequireKnownMetadata(model)), NativeHandle_(), key.data(), tag.NativeHandlePtr_())) {
                return std::nullopt;
            }
            return std::make_optional(std::move(tag));
        }

        std::optional<Tag> GetMetadata(MetadataModel model, std::string_view key) const
        {
            const auto opt = const_cast<Bitmap*>(this)->GetMetadata(model, std::move(key));
            return opt; // deep copy not allowing to change stored metadata
        }

        uint32_t GetMetadataCount(MetadataModel model) const
        {
            return FreeImage_GetMetadataCount(static_cast<FREE_IMAGE_MDMODEL>(RequireKnownMetadata(model)), NativeHandle_());
        }

        bool CloneMetadata(const Bitmap* src)
        {
            return FreeImage_CloneMetadata(NativeHandle_(), src->NativeHandle_());
        }

        Bitmap Rotate(double angle, const void* bkcolor = nullptr) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_Rotate, NativeHandle_(), angle, bkcolor));
        }

        Bitmap Rotate(double angle, double xShift, double yShift, double xOrigin, double yOrigin, bool useMask) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_RotateEx, NativeHandle_(), angle, xShift, yShift, xOrigin, yOrigin, useMask));
        }

        Bitmap& FlipHorizontal()
        {
            FREEIMAGERE_CHECKED_CALL(FreeImage_FlipHorizontal, NativeHandle_());
            return *this;
        }

        Bitmap& FlipVertical()
        {
            FREEIMAGERE_CHECKED_CALL(FreeImage_FlipVertical, NativeHandle_());
            return *this;
        }

        Bitmap Rescale(uint32_t dstWidth, uint32_t dstHeight, FilterType filter = FilterType::eCatmullRom) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_Rescale, NativeHandle_(), details::narrow_cast<int>(dstWidth), details::narrow_cast<int>(dstHeight), static_cast<FREE_IMAGE_FILTER>(filter)));
        }

        Bitmap RescaleRect(uint32_t dstWidth, uint32_t dstHeight, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom, FilterType filter = FilterType::eCatmullRom, uint32_t flags = 0)
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_RescaleRect, NativeHandle_(), details::narrow_cast<int>(dstWidth), details::narrow_cast<int>(dstHeight), 
                details::narrow_cast<int>(left), details::narrow_cast<int>(top), details::narrow_cast<int>(right), details::narrow_cast<int>(bottom), static_cast<FREE_IMAGE_FILTER>(filter), flags));
        }

        Bitmap MakeThumbnail(uint32_t maxPixelSize, bool convert = true) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_MakeThumbnail, NativeHandle_(), maxPixelSize, convert));
        }

        Bitmap& AdjustCurve(const uint8_t* lut, ColorChannel channel)
        {
            FREEIMAGERE_CHECKED_CALL(FreeImage_AdjustCurve, NativeHandle_(), const_cast<uint8_t*>(lut), static_cast<FREE_IMAGE_COLOR_CHANNEL>(channel));
            return *this;
        }

        Bitmap& AdjustGamma(double gamma)
        {
            FREEIMAGERE_CHECKED_CALL(FreeImage_AdjustGamma, NativeHandle_(), gamma);
            return *this;
        }

        Bitmap& AdjustBrightness(double percentage)
        {
            FREEIMAGERE_CHECKED_CALL(FreeImage_AdjustBrightness, NativeHandle_(), percentage);
            return *this;
        }

        Bitmap& AdjustContrast(double percentage)
        {
            FREEIMAGERE_CHECKED_CALL(FreeImage_AdjustContrast, NativeHandle_(), percentage);
            return *this;
        }

        Bitmap& Invert()
        {
            FREEIMAGERE_CHECKED_CALL(FreeImage_Invert, NativeHandle_());
            return *this;
        }

        bool MakeHistogram(uint32_t bins, void* minVal, void* maxVal, uint32_t* histR, uint32_t strideR = 1u,
            uint32_t* histG = nullptr, uint32_t strideG = 1u, uint32_t* histB = nullptr, uint32_t strideB = 1u, uint32_t* histL = nullptr, uint32_t strideL = 1u) const
        {
            return FreeImage_MakeHistogram(NativeHandle_(), bins, minVal, maxVal, histR, strideR, histG, strideG, histB, strideB, histL, strideL);
        }

        bool AdjustColors(double brightness, double contrast, double gamma, bool invert = false)
        {
            return FreeImage_AdjustColors(NativeHandle_(), brightness, contrast, gamma, invert);
        }

        uint32_t ApplyColorMapping(const FIRGBA8* srccolors, const FIRGBA8* dstcolors, uint32_t count, bool ignoreAlpha, bool swap)
        {
            return FreeImage_ApplyColorMapping(NativeHandle_(), const_cast<FIRGBA8*>(srccolors), const_cast<FIRGBA8*>(dstcolors), count, ignoreAlpha, swap);
        }

        uint32_t SwapColors(const FIRGBA8* colorA, const FIRGBA8* colorB, bool ignoreAlpha)
        {
            return FreeImage_SwapColors(NativeHandle_(), const_cast<FIRGBA8*>(colorA), const_cast<FIRGBA8*>(colorB), ignoreAlpha);
        }

        uint32_t ApplyPaletteIndexMapping(const uint8_t* srcindices, const uint8_t* dstindices, uint32_t count, bool swap)
        {
            return FreeImage_ApplyPaletteIndexMapping(NativeHandle_(), const_cast<uint8_t*>(srcindices), const_cast<uint8_t*>(dstindices), count, swap);
        }

        uint32_t SwapPaletteIndices(const uint8_t* index_a, const uint8_t* index_b)
        {
            return FreeImage_SwapPaletteIndices(NativeHandle_(), const_cast<uint8_t*>(index_a), const_cast<uint8_t*>(index_b));
        }

        Bitmap GetChannel(ColorChannel channel) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_GetChannel, NativeHandle_(), static_cast<FREE_IMAGE_COLOR_CHANNEL>(channel)));
        }

        bool SetChannel(const Bitmap& src, ColorChannel channel)
        {
            return FreeImage_SetChannel(NativeHandle_(), src.NativeHandle_(), static_cast<FREE_IMAGE_COLOR_CHANNEL>(channel));
        }

        Bitmap GetComplexChannel(ColorChannel channel) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_GetComplexChannel, NativeHandle_(), static_cast<FREE_IMAGE_COLOR_CHANNEL>(channel)));
        }

        bool SetComplexChannel(const Bitmap& src, ColorChannel channel)
        {
            return FreeImage_SetComplexChannel(NativeHandle_(), src.NativeHandle_(), static_cast<FREE_IMAGE_COLOR_CHANNEL>(channel));
        }

        Bitmap Copy(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_Copy, NativeHandle_(), details::narrow_cast<int>(left), details::narrow_cast<int>(top), details::narrow_cast<int>(right), details::narrow_cast<int>(bottom)));
        }

        bool Paste(const Bitmap& src, uint32_t left, uint32_t top, uint32_t alpha)
        {
            return FreeImage_Paste(NativeHandle_(), src.NativeHandle_(), details::narrow_cast<int>(left), details::narrow_cast<int>(top), details::narrow_cast<int>(alpha));
        }

        Bitmap CreateView(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_CreateView, NativeHandle_(), details::narrow_cast<int>(left), details::narrow_cast<int>(top), details::narrow_cast<int>(right), details::narrow_cast<int>(bottom)));
        }

        Bitmap& PreMultiplyWithAlpha()
        {
            FREEIMAGERE_CHECKED_CALL(FreeImage_PreMultiplyWithAlpha, NativeHandle_());
            return *this;
        }

        Bitmap Composite(bool useFileBkg = false, const FIRGBA8* appBkColor = nullptr, const Bitmap* bg = nullptr) const
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_Composite, NativeHandle_(), useFileBkg, const_cast<FIRGBA8*>(appBkColor), (bg ? bg->NativeHandle_() : nullptr)));
        }

        Bitmap& DrawBitmap(const Bitmap& src, AlphaOperation alpha, int32_t left = 0, int32_t top = 0)
        {
            FREEIMAGERE_CHECKED_CALL(FreeImage_DrawBitmap, NativeHandle_(), src.NativeHandle_(), static_cast<FREE_IMAGE_ALPHA_OPERATION>(alpha), left, top);
            return *this;
        }

        const Bitmap& FindMinMax(double* minBrightness, double* maxBrightness, void** minPtr = nullptr, void** maxPtr = nullptr) const
        {
            FREEIMAGERE_CHECKED_CALL(FreeImage_FindMinMax, NativeHandle_(), minBrightness, maxBrightness, minPtr, maxPtr);
            return *this;
        }

        Bitmap& Fill(const void* valuePtr, size_t valueSize)
        {
            FREEIMAGERE_CHECKED_CALL(FreeImage_Fill, NativeHandle_(), valuePtr, valueSize);
            return *this;
        }

        Bitmap& SwapRedBlue32()
        {
            FREEIMAGERE_CHECKED_CALL(SwapRedBlue32, NativeHandle_());
            return *this;
        }

        bool Save(ImageFormat fif, const char* filename, int flags = 0) const
        {
            return FreeImage_Save(static_cast<FREE_IMAGE_FORMAT>(fif), NativeHandle_(), filename, flags);
        }

        bool Save(ImageFormat fif, const wchar_t* filename, int flags = 0) const
        {
            return FreeImage_SaveU(static_cast<FREE_IMAGE_FORMAT>(fif), NativeHandle_(), filename, flags);
        }

        bool Save(ImageFormat fif, const std::filesystem::path& filename, int flags = 0) const
        {
            return Save(fif, filename.c_str(), flags);
        }

        bool Save(ImageFormat fif, FreeImageIO* io, fi_handle handle, int flags = 0) const
        {
            return FreeImage_SaveToHandle(static_cast<FREE_IMAGE_FORMAT>(fif), NativeHandle_(), io, handle, flags);
        }


        /**
         * Returns native FIBIMAP handle and disabples ownership. Deleter won't be called.
         * Can be used only for standalone Bitmaps. Not for images with a parent: locked pages, thumbnails, etc.
         */
        FIBITMAP* Release() &&
        {
            if (mHandlePtr.use_count() != 1) {
                throw ImageError("Bitmap[Release]: Cannot release image with shared ownership");
            }
            BitmapDeleter* deleter = std::get_deleter<BitmapDeleter>(mHandlePtr);
            if (!deleter) {
                throw ImageError("Bitmap[Release]: Cannot release image with a parent deleter");
            }
            deleter->Disable();
            return NativeHandle_();
        }

        static
        int AdjustColorsLookupTable(uint8_t* lut, double brightness, double contrast, double gamma, bool invert)
        {
            return FreeImage_GetAdjustColorsLookupTable(lut, brightness, contrast, gamma, invert);
        }

    private:
        friend class MultiBitmap;

        class BitmapDeleter
        {
        public:
            void operator()(FIBITMAP* bmp) const noexcept {
                if (mCallUnload) {
                    FreeImage_Unload(bmp);
                }
            }

            void Disable() {
                mCallUnload = false;
            }

        private:
            bool mCallUnload = true;
        };

        FIBITMAP* NativeHandle_() const noexcept
        {
            return mHandlePtr.get();
        }

        std::shared_ptr<FIBITMAP> mHandlePtr{ nullptr };
    };



    class MultiBitmap
    {
        class MultiBitmapDeleter;
        class PageDeleter;
    public:
        MultiBitmap(ImageFormat fif, const char* filename, bool createNew, bool readOnly, bool keepCacheInMemory = false, int flags = 0)
            : MultiBitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_OpenMultiBitmap, static_cast<FREE_IMAGE_FORMAT>(fif), filename, createNew, readOnly, keepCacheInMemory, flags))
        { }

        MultiBitmap(ImageFormat fif, const std::filesystem::path& filename, bool createNew, bool readOnly, bool keepCacheInMemory = false, int flags = 0)
            : MultiBitmap(fif, filename.string().c_str(), createNew, readOnly, keepCacheInMemory, flags)
        { }

        MultiBitmap(ImageFormat fif, FreeImageIO* io, fi_handle handle, int flags = 0)
            : MultiBitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_OpenMultiBitmapFromHandle, static_cast<FREE_IMAGE_FORMAT>(fif), io, handle, flags))
        { }

        template <typename Deleter_ = MultiBitmapDeleter>
        explicit
        MultiBitmap(FIMULTIBITMAP* handle, Deleter_&& deleter = MultiBitmapDeleter{})
            : mHandlePtr(handle, std::move(deleter))
        {
            if (!mHandlePtr) {
                throw ImageError("MultiBitmap[ctor]: FIMULTIBITMAP handle is null");
            }
        }

        MultiBitmap(const MultiBitmap& other) = delete;

        MultiBitmap(MultiBitmap&& other) noexcept = default;

        ~MultiBitmap() noexcept = default;

        MultiBitmap& operator=(const MultiBitmap& other) = delete;

        MultiBitmap& operator=(MultiBitmap&& other) noexcept = default;

        explicit
        operator FIMULTIBITMAP* ()
        {
            return NativeHandle_();
        }

        explicit
        operator const FIMULTIBITMAP* () const
        {
            return NativeHandle_();
        }

        void Swap(MultiBitmap& other) noexcept
        {
            std::swap(mHandlePtr, other.mHandlePtr);
        }

        uint32_t GetPagesCount() const
        {
            return details::narrow_cast<uint32_t>(FreeImage_GetPageCount(NativeHandle_()));
        }

        void AppendPage(const Bitmap& bmp)
        {
            FreeImage_AppendPage(NativeHandle_(), bmp.NativeHandle_());
        }

        void InsertPage(uint32_t page, const Bitmap& bmp)
        {
            FreeImage_InsertPage(NativeHandle_(), details::narrow_cast<int>(page), bmp.NativeHandle_());
        }

        void DeletePage(uint32_t page)
        {
            FreeImage_DeletePage(NativeHandle_(), details::narrow_cast<int>(page));
        }

        Bitmap LockPage(uint32_t page)
        {
            return Bitmap(FREEIMAGERE_CHECKED_CALL(FreeImage_LockPage, NativeHandle_(), details::narrow_cast<int32_t>(page)), PageDeleter(mHandlePtr));
        }

        void UnlockPage(Bitmap&& bmp, bool changed = false)
        {
            PageDeleter* deleter = std::get_deleter<PageDeleter>(bmp.mHandlePtr);
            if (!deleter) {
                throw ImageError("MultiBitmap[UnlockPage]: Image is not a page.");
            }
            if (deleter->GetParent() != mHandlePtr) {
                throw ImageError("MultiBitmap[UnlockPage]: Wrong parent.");
            }
            if (changed) {
                deleter->WriteChanges();
            }
            Bitmap toBeDeleted{ std::move(bmp) };
        }

        bool MovePage(uint32_t target, uint32_t source)
        {
            return FreeImage_MovePage(NativeHandle_(), details::narrow_cast<int>(target), details::narrow_cast<int>(source));
        }

        std::vector<uint32_t> GetLockedPageNumbers() const
        {
            int count = 0;
            if (!FreeImage_GetLockedPageNumbers(NativeHandle_(), nullptr, &count)) {
                return { };
            }
            std::vector<uint32_t> res(count, 0);
            if (!FreeImage_GetLockedPageNumbers(NativeHandle_(), reinterpret_cast<int32_t*>(res.data()), &count)) {
                return { };
            }
            return res;
        }

        bool Save(ImageFormat fif, FreeImageIO* io, fi_handle handle, int flags = 0) const
        {
            return FreeImage_SaveMultiBitmapToHandle(static_cast<FREE_IMAGE_FORMAT>(fif), NativeHandle_(), io, handle, flags);
        }

    private:
        class MultiBitmapDeleter
        {
        public:
            void operator()(FIMULTIBITMAP* bmp) const noexcept {
                FreeImage_CloseMultiBitmap(bmp);
            }
        };

        class PageDeleter
        {
        public:
            PageDeleter(std::shared_ptr<FIMULTIBITMAP> parent)
                : mParent(std::move(parent))
            {
                if (!mParent) {
                    throw std::logic_error("MultiBitmap[PageDeleter]: Parent can't be null");
                }
            }

            void WriteChanges()
            {
                mWriteChanges = true;
            }

            const std::shared_ptr<FIMULTIBITMAP>& GetParent() const
            {
                return mParent;
            }

            void operator()(FIBITMAP* bmp) const noexcept
            {
                FreeImage_UnlockPage(mParent.get(), bmp, mWriteChanges);
            }

        private:
            std::shared_ptr<FIMULTIBITMAP> mParent;
            bool mWriteChanges = false;
        };

        FIMULTIBITMAP* NativeHandle_() const
        {
            return mHandlePtr.get();
        }

        std::shared_ptr<FIMULTIBITMAP> mHandlePtr;
    };



} // fi

#endif // FREEIMAGE_HPP

