//--------------------------------------------------------------------------------------
// File: WICTextureLoader.cpp
//
// Function for loading a WIC image and creating a Direct3D runtime texture for it
// (auto-generating mipmaps if possible)
//
// Note: Assumes application has already called CoInitializeEx
//
// Note these functions are useful for images created as simple 2D textures. For
// more complex resources, DDSTextureLoader is an excellent light-weight runtime loader.
// For a full-featured DDS file reader, writer, and texture processing pipeline see
// the 'Texconv' sample and the 'DirectXTex' library.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

// We could load multi-frame images (TIFF/GIF) into a texture array.
// For now, we just load the first frame (note: DirectXTex supports multi-frame images)

#include "stdinc.h"

#include "wictextureloader.h"
#include "d3dx12.h"
#include "directxhelpers.h"
#include "platformhelpers.h"
#include "loaderhelpers.h"
#include "resourceuploadbatch.h"

using namespace directxtk;
// using Microsoft::WRL::ComPtr;

namespace
{
    //-------------------------------------------------------------------------------------
    // WIC Pixel Format Translation Data
    //-------------------------------------------------------------------------------------
    struct WICTranslate
    {
        GUID                wic;
        DXGI_FORMAT         format;
    };

    const WICTranslate g_WICFormats[] =
    {
        { GUID_WICPixelFormat128bppRGBAFloat,       DXGI_FORMAT_R32G32B32A32_FLOAT },

        { GUID_WICPixelFormat64bppRGBAHalf,         DXGI_FORMAT_R16G16B16A16_FLOAT },
        { GUID_WICPixelFormat64bppRGBA,             DXGI_FORMAT_R16G16B16A16_UNORM },

        { GUID_WICPixelFormat32bppRGBA,             DXGI_FORMAT_R8G8B8A8_UNORM },
        { GUID_WICPixelFormat32bppBGRA,             DXGI_FORMAT_B8G8R8A8_UNORM },
        { GUID_WICPixelFormat32bppBGR,              DXGI_FORMAT_B8G8R8X8_UNORM },

        { GUID_WICPixelFormat32bppRGBA1010102XR,    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM },
        { GUID_WICPixelFormat32bppRGBA1010102,      DXGI_FORMAT_R10G10B10A2_UNORM },

        { GUID_WICPixelFormat16bppBGRA5551,         DXGI_FORMAT_B5G5R5A1_UNORM },
        { GUID_WICPixelFormat16bppBGR565,           DXGI_FORMAT_B5G6R5_UNORM },

        { GUID_WICPixelFormat32bppGrayFloat,        DXGI_FORMAT_R32_FLOAT },
        { GUID_WICPixelFormat16bppGrayHalf,         DXGI_FORMAT_R16_FLOAT },
        { GUID_WICPixelFormat16bppGray,             DXGI_FORMAT_R16_UNORM },
        { GUID_WICPixelFormat8bppGray,              DXGI_FORMAT_R8_UNORM },

        { GUID_WICPixelFormat8bppAlpha,             DXGI_FORMAT_A8_UNORM },
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
        { GUID_WICPixelFormat96bppRGBFloat,         DXGI_FORMAT_R32G32B32_FLOAT },
#endif
    };

    //-------------------------------------------------------------------------------------
    // WIC Pixel Format nearest conversion table
    //-------------------------------------------------------------------------------------

    struct WICConvert
    {
        GUID        source;
        GUID        target;
    };

    const WICConvert g_WICConvert[] =
    {
        // Note target GUID in this conversion table must be one of those directly supported formats (above).

        { GUID_WICPixelFormatBlackWhite,            GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM

        { GUID_WICPixelFormat1bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
        { GUID_WICPixelFormat2bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
        { GUID_WICPixelFormat4bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
        { GUID_WICPixelFormat8bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 

        { GUID_WICPixelFormat2bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 
        { GUID_WICPixelFormat4bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 

        { GUID_WICPixelFormat16bppGrayFixedPoint,   GUID_WICPixelFormat16bppGrayHalf }, // DXGI_FORMAT_R16_FLOAT 
        { GUID_WICPixelFormat32bppGrayFixedPoint,   GUID_WICPixelFormat32bppGrayFloat }, // DXGI_FORMAT_R32_FLOAT 

        { GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat16bppBGRA5551 }, // DXGI_FORMAT_B5G5R5A1_UNORM

        { GUID_WICPixelFormat32bppBGR101010,        GUID_WICPixelFormat32bppRGBA1010102 }, // DXGI_FORMAT_R10G10B10A2_UNORM

        { GUID_WICPixelFormat24bppBGR,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
        { GUID_WICPixelFormat24bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
        { GUID_WICPixelFormat32bppPBGRA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
        { GUID_WICPixelFormat32bppPRGBA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 

        { GUID_WICPixelFormat48bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
        { GUID_WICPixelFormat48bppBGR,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
        { GUID_WICPixelFormat64bppBGRA,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
        { GUID_WICPixelFormat64bppPRGBA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
        { GUID_WICPixelFormat64bppPBGRA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

        { GUID_WICPixelFormat48bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
        { GUID_WICPixelFormat48bppBGRFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
        { GUID_WICPixelFormat64bppRGBAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
        { GUID_WICPixelFormat64bppBGRAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
        { GUID_WICPixelFormat64bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
        { GUID_WICPixelFormat64bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
        { GUID_WICPixelFormat48bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 

        { GUID_WICPixelFormat128bppPRGBAFloat,      GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
        { GUID_WICPixelFormat128bppRGBFloat,        GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
        { GUID_WICPixelFormat128bppRGBAFixedPoint,  GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
        { GUID_WICPixelFormat128bppRGBFixedPoint,   GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
        { GUID_WICPixelFormat32bppRGBE,             GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 

        { GUID_WICPixelFormat32bppCMYK,             GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
        { GUID_WICPixelFormat64bppCMYK,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
        { GUID_WICPixelFormat40bppCMYKAlpha,        GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
        { GUID_WICPixelFormat80bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
        { GUID_WICPixelFormat32bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
        { GUID_WICPixelFormat64bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
        { GUID_WICPixelFormat64bppPRGBAHalf,        GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
        { GUID_WICPixelFormat96bppRGBFixedPoint,   GUID_WICPixelFormat96bppRGBFloat }, // DXGI_FORMAT_R32G32B32_FLOAT
#endif
        // We don't support n-channel formats
    };

    BOOL WINAPI InitializeWICFactory(PINIT_ONCE, PVOID, PVOID *ifactory) noexcept
    {
        return SUCCEEDED(CoCreateInstance(
            CLSID_WICImagingFactory2,
            nullptr,
            CLSCTX_INPROC_SERVER,
            __uuidof(IWICImagingFactory2),
            ifactory)) ? TRUE : FALSE;
    }
}

//--------------------------------------------------------------------------------------
namespace directxtk
{
    IWICImagingFactory2* _GetWIC() noexcept;
        // Also used by ScreenGrab

    IWICImagingFactory2* _GetWIC() noexcept
    {
        static INIT_ONCE s_initOnce = INIT_ONCE_STATIC_INIT;

        IWICImagingFactory2* factory = nullptr;
        if (!InitOnceExecuteOnce(
            &s_initOnce,
            InitializeWICFactory,
            nullptr,
            reinterpret_cast<PVOID*>(&factory)))
        {
            return nullptr;
        }

        return factory;
    }
} // namespace directxtk


namespace
{
    //---------------------------------------------------------------------------------
    DXGI_FORMAT _WICToDXGI(const GUID& guid) noexcept
    {
        for (auto i = 0u; i < _countof(g_WICFormats); ++i)
        {
            if (memcmp(&g_WICFormats[i].wic, &guid, sizeof(GUID)) == 0)
                return g_WICFormats[i].format;
        }

        return DXGI_FORMAT_UNKNOWN;
    }

    //---------------------------------------------------------------------------------
    size_t _WICBitsPerPixel(REFGUID targetGuid) noexcept
    {
        auto pWIC = _GetWIC();
        if (!pWIC)
            return 0;

        wil::com_ptr_nothrow<IWICComponentInfo> cinfo;
        if (FAILED(pWIC->CreateComponentInfo(targetGuid, cinfo.addressof())))
            return 0;

        WICComponentType type;
        if (FAILED(cinfo->GetComponentType(&type)))
            return 0;

        if (type != WICPixelFormat)
            return 0;

        wil::com_ptr_nothrow<IWICPixelFormatInfo> pfinfo;
        if (FAILED(cinfo.query_to(&pfinfo)))
            return 0;

        uint32_t bpp;
        if (FAILED(pfinfo->GetBitsPerPixel(&bpp)))
            return 0;

        return bpp;
    }

    //---------------------------------------------------------------------------------
    HRESULT CreateTextureFromWIC(_In_ ID3D12Device* d3dDevice,
        _In_ IWICBitmapFrameDecode *frame,
        size_t maxsize,
        D3D12_RESOURCE_FLAGS resFlags,
        WIC_LOADER_FLAGS loadFlags,
        _Outptr_ ID3D12Resource** texture,
        std::unique_ptr<uint8_t[]>& decodedData,
        D3D12_SUBRESOURCE_DATA& subresource) noexcept
    {
        uint32_t width, height;
        HRESULT hr = frame->GetSize(&width, &height);
        if (FAILED(hr))
            return hr;

        assert(width > 0 && height > 0);

        if (maxsize > UINT32_MAX)
            return E_INVALIDARG;

        if (!maxsize)
        {
            maxsize = size_t(D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION);
        }

        uint32_t twidth = width;
        uint32_t theight = height;
        if (loadFlags & WIC_LOADER_FIT_POW2)
        {
            LoaderHelpers::FitPowerOf2(width, height, twidth, theight, maxsize);
        }
        else if (width > maxsize || height > maxsize)
        {
            float ar = static_cast<float>(height) / static_cast<float>(width);
            if (width > height)
            {
                twidth = static_cast<uint32_t>(maxsize);
                theight = std::max<uint32_t>(1, static_cast<uint32_t>(static_cast<float>(maxsize) * ar));
            }
            else
            {
                theight = static_cast<uint32_t>(maxsize);
                twidth = std::max<uint32_t>(1, static_cast<uint32_t>(static_cast<float>(maxsize) / ar));
            }
            assert(twidth <= maxsize && theight <= maxsize);
        }

        if (loadFlags & WIC_LOADER_MAKE_SQUARE)
        {
            twidth = std::max<uint32_t>(twidth, theight);
            theight = twidth;
        }

        // Determine format
        WICPixelFormatGUID pixelFormat;
        hr = frame->GetPixelFormat(&pixelFormat);
        if (FAILED(hr))
            return hr;

        WICPixelFormatGUID convertGUID;
        memcpy_s(&convertGUID, sizeof(WICPixelFormatGUID), &pixelFormat, sizeof(GUID));

        size_t bpp = 0;

        DXGI_FORMAT format = _WICToDXGI(pixelFormat);
        if (format == DXGI_FORMAT_UNKNOWN)
        {
            for (auto i = 0u; i < _countof(g_WICConvert); ++i)
            {
                if (memcmp(&g_WICConvert[i].source, &pixelFormat, sizeof(WICPixelFormatGUID)) == 0)
                {
                    memcpy_s(&convertGUID, sizeof(WICPixelFormatGUID), &g_WICConvert[i].target, sizeof(GUID));

                    format = _WICToDXGI(g_WICConvert[i].target);
                    assert(format != DXGI_FORMAT_UNKNOWN);
                    bpp = _WICBitsPerPixel(convertGUID);
                    break;
                }
            }

            if (format == DXGI_FORMAT_UNKNOWN)
            {
                DebugTrace("ERROR: WICTextureLoader does not support all DXGI formats (WIC GUID {%8.8lX-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}). Consider using DirectXTex.\n",
                    pixelFormat.Data1, pixelFormat.Data2, pixelFormat.Data3,
                    pixelFormat.Data4[0], pixelFormat.Data4[1], pixelFormat.Data4[2], pixelFormat.Data4[3],
                    pixelFormat.Data4[4], pixelFormat.Data4[5], pixelFormat.Data4[6], pixelFormat.Data4[7]);
                return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
            }
        }
        else
        {
            bpp = _WICBitsPerPixel(pixelFormat);
        }

        if (loadFlags & WIC_LOADER_FORCE_RGBA32)
        {
            memcpy_s(&convertGUID, sizeof(WICPixelFormatGUID), &GUID_WICPixelFormat32bppRGBA, sizeof(GUID));
            format = DXGI_FORMAT_R8G8B8A8_UNORM;
            bpp = 32;
        }

        if (!bpp)
            return E_FAIL;

        // Handle sRGB formats
        if (loadFlags & WIC_LOADER_FORCE_SRGB)
        {
            format = LoaderHelpers::MakeSRGB(format);
        }
        else if (!(loadFlags & WIC_LOADER_IGNORE_SRGB))
        {
            wil::com_ptr<IWICMetadataQueryReader> metareader;
            if (SUCCEEDED(frame->GetMetadataQueryReader(metareader.addressof())))
            {
                GUID containerFormat;
                if (SUCCEEDED(metareader->GetContainerFormat(&containerFormat)))
                {
                    bool sRGB = false;

                    PROPVARIANT value;
                    PropVariantInit(&value);

                    // Check for colorspace chunks
                    if (memcmp(&containerFormat, &GUID_ContainerFormatPng, sizeof(GUID)) == 0)
                    {
                        // Check for sRGB chunk
                        if (SUCCEEDED(metareader->GetMetadataByName(L"/sRGB/RenderingIntent", &value)) && value.vt == VT_UI1)
                        {
                            sRGB = true;
                        }
                        else if (SUCCEEDED(metareader->GetMetadataByName(L"/gAMA/ImageGamma", &value)) && value.vt == VT_UI4)
                        {
                            sRGB = (value.uintVal == 45455);
                        }
                        else
                        {
                            sRGB = (loadFlags & WIC_LOADER_SRGB_DEFAULT) != 0;
                        }
                    }
                #if defined(_XBOX_ONE) && defined(_TITLE)
                    else if (memcmp(&containerFormat, &GUID_ContainerFormatJpeg, sizeof(GUID)) == 0)
                    {
                        if (SUCCEEDED(metareader->GetMetadataByName(L"/app1/ifd/exif/{ushort=40961}", &value)) && value.vt == VT_UI2)
                        {
                            sRGB = (value.uiVal == 1);
                        }
                        else
                        {
                            sRGB = (loadFlags & WIC_LOADER_SRGB_DEFAULT) != 0;
                        }
                    }
                    else if (memcmp(&containerFormat, &GUID_ContainerFormatTiff, sizeof(GUID)) == 0)
                    {
                        if (SUCCEEDED(metareader->GetMetadataByName(L"/ifd/exif/{ushort=40961}", &value)) && value.vt == VT_UI2)
                        {
                            sRGB = (value.uiVal == 1);
                        }
                        else
                        {
                            sRGB = (loadFlags & WIC_LOADER_SRGB_DEFAULT) != 0;
                        }
                    }
                #else
                    else if (SUCCEEDED(metareader->GetMetadataByName(L"System.Image.ColorSpace", &value)) && value.vt == VT_UI2)
                    {
                        sRGB = (value.uiVal == 1);
                    }
                    else
                    {
                        sRGB = (loadFlags & WIC_LOADER_SRGB_DEFAULT) != 0;
                    }
                #endif

                    (void)PropVariantClear(&value);

                    if (sRGB)
                        format = LoaderHelpers::MakeSRGB(format);
                }
            }
        }

        // Allocate memory for decoded image
        uint64_t rowBytes = (uint64_t(twidth) * uint64_t(bpp) + 7u) / 8u;
        uint64_t numBytes = rowBytes * uint64_t(height);

        if (rowBytes > UINT32_MAX || numBytes > UINT32_MAX)
            return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);

        auto rowpitch = static_cast<size_t>(rowBytes);
        auto imageSize = static_cast<size_t>(numBytes);

        decodedData.reset(new (std::nothrow) uint8_t[imageSize]);
        if (!decodedData)
            return E_OUTOFMEMORY;

        // Load image data
        if (memcmp(&convertGUID, &pixelFormat, sizeof(GUID)) == 0
            && twidth == width
            && theight == height)
        {
            // No format conversion or resize needed
            hr = frame->CopyPixels(nullptr, static_cast<uint32_t>(rowpitch), static_cast<uint32_t>(imageSize), decodedData.get());
            if (FAILED(hr))
                return hr;
        }
        else if (twidth != width || theight != height)
        {
            // Resize
            auto pWIC = _GetWIC();
            if (!pWIC)
                return E_NOINTERFACE;

            wil::com_ptr<IWICBitmapScaler> scaler;
            hr = pWIC->CreateBitmapScaler(scaler.addressof());
            if (FAILED(hr))
                return hr;

            hr = scaler->Initialize(frame, twidth, theight, WICBitmapInterpolationModeFant);
            if (FAILED(hr))
                return hr;

            WICPixelFormatGUID pfScaler;
            hr = scaler->GetPixelFormat(&pfScaler);
            if (FAILED(hr))
                return hr;

            if (memcmp(&convertGUID, &pfScaler, sizeof(GUID)) == 0)
            {
                // No format conversion needed
                hr = scaler->CopyPixels(nullptr, static_cast<uint32_t>(rowpitch), static_cast<uint32_t>(imageSize), decodedData.get());
                if (FAILED(hr))
                    return hr;
            }
            else
            {
                wil::com_ptr<IWICFormatConverter> FC;
                hr = pWIC->CreateFormatConverter(FC.addressof());
                if (FAILED(hr))
                    return hr;

                BOOL canConvert = FALSE;
                hr = FC->CanConvert(pfScaler, convertGUID, &canConvert);
                if (FAILED(hr) || !canConvert)
                {
                    return E_UNEXPECTED;
                }

                hr = FC->Initialize(scaler.get(), convertGUID, WICBitmapDitherTypeErrorDiffusion, nullptr, 0, WICBitmapPaletteTypeMedianCut);
                if (FAILED(hr))
                    return hr;

                hr = FC->CopyPixels(nullptr, static_cast<uint32_t>(rowpitch), static_cast<uint32_t>(imageSize), decodedData.get());
                if (FAILED(hr))
                    return hr;
            }
        }
        else
        {
            // Format conversion but no resize
            auto pWIC = _GetWIC();
            if (!pWIC)
                return E_NOINTERFACE;

            wil::com_ptr<IWICFormatConverter> FC;
            hr = pWIC->CreateFormatConverter(FC.addressof());
            if (FAILED(hr))
                return hr;

            BOOL canConvert = FALSE;
            hr = FC->CanConvert(pixelFormat, convertGUID, &canConvert);
            if (FAILED(hr) || !canConvert)
            {
                return E_UNEXPECTED;
            }

            hr = FC->Initialize(frame, convertGUID, WICBitmapDitherTypeErrorDiffusion, nullptr, 0, WICBitmapPaletteTypeMedianCut);
            if (FAILED(hr))
                return hr;

            hr = FC->CopyPixels(nullptr, static_cast<uint32_t>(rowpitch), static_cast<uint32_t>(imageSize), decodedData.get());
            if (FAILED(hr))
                return hr;
        }

        // Count the number of mips
        uint32_t mipCount = (loadFlags & (WIC_LOADER_MIP_AUTOGEN | WIC_LOADER_MIP_RESERVE))
            ? LoaderHelpers::CountMips(twidth, theight) : 1u;

        // Create texture
        D3D12_RESOURCE_DESC desc = {};
        desc.Width = twidth;
        desc.Height = theight;
        desc.MipLevels = static_cast<uint16_t>(mipCount);
        desc.DepthOrArraySize = 1;
        desc.Format = format;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Flags = resFlags;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

        CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

        ID3D12Resource* tex = nullptr;
        hr = d3dDevice->CreateCommittedResource(
            &defaultHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_GRAPHICS_PPV_ARGS(&tex));

        if (FAILED(hr))
        {
            return hr;
        }

        _Analysis_assume_(tex != nullptr);

        subresource.pData = decodedData.get();
        subresource.RowPitch = static_cast<int32_t>(rowpitch);
        subresource.SlicePitch = static_cast<int32_t>(imageSize);

        *texture = tex;
        return hr;
    }

    //--------------------------------------------------------------------------------------
    void SetDebugTextureInfo(
        _In_ const std::wstring_view& filename,
        _In_ ID3D12Resource** texture) noexcept
    {
#if !defined(NO_D3D12_DEBUG_NAME) && ( defined(_DEBUG) || defined(PROFILE) )
        if (texture && *texture && !filename.empty())
        {
			size_t pos = filename.find_last_of('\\');
			if (pos < filename.length())
			{
				(*texture)->SetName(filename.substr(pos + 1).data());
			}
			else
			{
				(*texture)->SetName(filename.data());
			}
        }
#else
        UNREFERENCED_PARAMETER(filename);
        UNREFERENCED_PARAMETER(texture);
#endif
    }

    //--------------------------------------------------------------------------------------
    DXGI_FORMAT GetPixelFormat(_In_ IWICBitmapFrameDecode* frame) noexcept
    {
        WICPixelFormatGUID pixelFormat;
        if (FAILED(frame->GetPixelFormat(&pixelFormat)))
            return DXGI_FORMAT_UNKNOWN;

        DXGI_FORMAT format = _WICToDXGI(pixelFormat);
        if (format == DXGI_FORMAT_UNKNOWN)
        {
            for (auto i = 0u; i < _countof(g_WICConvert); ++i)
            {
                if (memcmp(&g_WICConvert[i].source, &pixelFormat, sizeof(WICPixelFormatGUID)) == 0)
                {
                    return _WICToDXGI(g_WICConvert[i].target);
                }
            }
        }

        return format;
    }
} // anonymous namespace


//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT directxtk::LoadWICTextureFromMemory(
    ID3D12Device* d3dDevice,
    const uint8_t* wicData,
    size_t wicDataSize,
    ID3D12Resource** texture,
    std::unique_ptr<uint8_t[]>& decodedData,
    D3D12_SUBRESOURCE_DATA& subresource,
    size_t maxsize) noexcept
{
    return LoadWICTextureFromMemoryEx(
        d3dDevice,
        wicData,
        wicDataSize,
        maxsize,
        D3D12_RESOURCE_FLAG_NONE,
        WIC_LOADER_DEFAULT,
        texture,
        decodedData,
        subresource);
}

_Use_decl_annotations_
HRESULT directxtk::CreateWICTextureFromMemory(
    ID3D12Device* d3dDevice,
    ResourceUploadBatch& resourceUpload,
    const uint8_t* wicData,
    size_t wicDataSize,
    ID3D12Resource** texture,
    bool generatemips,
    size_t maxsize)
{
    return CreateWICTextureFromMemoryEx(
        d3dDevice,
        resourceUpload,
        wicData,
        wicDataSize,
        maxsize,
        D3D12_RESOURCE_FLAG_NONE,
        (generatemips) ? WIC_LOADER_MIP_AUTOGEN : WIC_LOADER_DEFAULT,
        texture);
}


//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT directxtk::LoadWICTextureFromMemoryEx(
    ID3D12Device* d3dDevice,
    const uint8_t* wicData,
    size_t wicDataSize,
    size_t maxsize,
    D3D12_RESOURCE_FLAGS resFlags,
    WIC_LOADER_FLAGS loadFlags,
    ID3D12Resource** texture,
    std::unique_ptr<uint8_t[]>& decodedData,
    D3D12_SUBRESOURCE_DATA& subresource) noexcept
{
    if (texture)
    {
        *texture = nullptr;
    }

    if (!d3dDevice || !wicData || !texture)
        return E_INVALIDARG;

    if (!wicDataSize)
        return E_FAIL;

    if (wicDataSize > UINT32_MAX)
        return HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE);

    auto pWIC = _GetWIC();
    if (!pWIC)
        return E_NOINTERFACE;

    // Create input stream for memory
    wil::com_ptr<IWICStream> stream;
    HRESULT hr = pWIC->CreateStream(stream.addressof());
    if (FAILED(hr))
        return hr;

    hr = stream->InitializeFromMemory(const_cast<uint8_t*>(wicData), static_cast<uint32_t>(wicDataSize));
    if (FAILED(hr))
        return hr;

    // Initialize WIC
    wil::com_ptr<IWICBitmapDecoder> decoder;
    hr = pWIC->CreateDecoderFromStream(stream.get(), nullptr, WICDecodeMetadataCacheOnDemand, decoder.addressof());
    if (FAILED(hr))
        return hr;

    wil::com_ptr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, frame.addressof());
    if (FAILED(hr))
        return hr;

    hr = CreateTextureFromWIC(d3dDevice,
        frame.get(), maxsize,
        resFlags, loadFlags,
        texture, decodedData, subresource);
    if (FAILED(hr))
        return hr;

    _Analysis_assume_(*texture != nullptr);
    SetDebugObjectName(*texture, L"WICTextureLoader");

    return hr;
}

_Use_decl_annotations_
HRESULT directxtk::CreateWICTextureFromMemoryEx(
    ID3D12Device* d3dDevice,
    ResourceUploadBatch& resourceUpload,
    const uint8_t* wicData,
    size_t wicDataSize,
    size_t maxsize,
    D3D12_RESOURCE_FLAGS resFlags,
    WIC_LOADER_FLAGS loadFlags,
    ID3D12Resource** texture)
{
    if (texture)
    {
        *texture = nullptr;
    }

    if (!d3dDevice || !wicData || !texture)
        return E_INVALIDARG;

    if (!wicDataSize)
        return E_FAIL;

    if (wicDataSize > UINT32_MAX)
        return HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE);

    auto pWIC = _GetWIC();
    if (!pWIC)
        return E_NOINTERFACE;

    // Create input stream for memory
    wil::com_ptr<IWICStream> stream;
    HRESULT hr = pWIC->CreateStream(stream.addressof());
    if (FAILED(hr))
        return hr;

    hr = stream->InitializeFromMemory(const_cast<uint8_t*>(wicData), static_cast<uint32_t>(wicDataSize));
    if (FAILED(hr))
        return hr;

    // Initialize WIC
    wil::com_ptr<IWICBitmapDecoder> decoder;
    hr = pWIC->CreateDecoderFromStream(stream.get(), nullptr, WICDecodeMetadataCacheOnDemand, decoder.addressof());
    if (FAILED(hr))
        return hr;

    wil::com_ptr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, frame.addressof());
    if (FAILED(hr))
        return hr;

    if (loadFlags & WIC_LOADER_MIP_AUTOGEN)
    {
        DXGI_FORMAT fmt = GetPixelFormat(frame.get());
        if (!resourceUpload.IsSupportedForGenerateMips(fmt))
        {
            DebugTrace("WARNING: Autogen of mips ignored (device doesn't support this format (%d) or trying to use a copy queue)\n", static_cast<int32_t>(fmt));
            loadFlags &= ~WIC_LOADER_MIP_AUTOGEN;
        }
    }

    std::unique_ptr<uint8_t[]> decodedData;
    D3D12_SUBRESOURCE_DATA initData;
    hr = CreateTextureFromWIC(d3dDevice,
        frame.get(), maxsize,
        resFlags, loadFlags,
        texture, decodedData, initData);

    if (SUCCEEDED(hr))
    {
        assert(texture != nullptr && *texture != nullptr);
        _Analysis_assume_(texture != nullptr && *texture != nullptr);
        SetDebugObjectName(*texture, L"WICTextureLoader");

        resourceUpload.Upload(
            *texture,
            0,
            &initData,
            1);

        resourceUpload.Transition(
            *texture,
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        // Generate mips?
        if (loadFlags & WIC_LOADER_MIP_AUTOGEN)
        {
            resourceUpload.GenerateMips(*texture);
        }
    }

    return hr;
}


//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT directxtk::LoadWICTextureFromFile(
    ID3D12Device* d3dDevice,
    const std::wstring_view& filename,
    ID3D12Resource** texture,
    std::unique_ptr<uint8_t[]>& wicData,
    D3D12_SUBRESOURCE_DATA& subresource,
    size_t maxsize) noexcept
{
    return LoadWICTextureFromFileEx(
        d3dDevice,
        filename,
        maxsize,
        D3D12_RESOURCE_FLAG_NONE,
        WIC_LOADER_DEFAULT,
        texture,
        wicData,
        subresource);
}

_Use_decl_annotations_
HRESULT directxtk::CreateWICTextureFromFile(
    ID3D12Device* d3dDevice,
    ResourceUploadBatch& resourceUpload,
    const std::wstring_view& filename,
    ID3D12Resource** texture,
    bool generatemips,
    size_t maxsize)
{
    return CreateWICTextureFromFileEx(
        d3dDevice,
        resourceUpload,
        filename,
        maxsize,
        D3D12_RESOURCE_FLAG_NONE,
        generatemips ? WIC_LOADER_MIP_AUTOGEN : WIC_LOADER_DEFAULT,
        texture);
}


//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT directxtk::LoadWICTextureFromFileEx(
    ID3D12Device* d3dDevice,
    const std::wstring_view& filename,
    size_t maxsize,
    D3D12_RESOURCE_FLAGS resFlags,
    WIC_LOADER_FLAGS loadFlags,
    ID3D12Resource** texture,
    std::unique_ptr<uint8_t[]>& decodedData,
    D3D12_SUBRESOURCE_DATA& subresource) noexcept
{
    if (texture)
    {
        *texture = nullptr;
    }

    if (!d3dDevice || filename.empty() || !texture)
        return E_INVALIDARG;

    auto pWIC = _GetWIC();
    if (!pWIC)
        return E_NOINTERFACE;

    // Initialize WIC
    wil::com_ptr<IWICBitmapDecoder> decoder;
    HRESULT hr = pWIC->CreateDecoderFromFilename(filename.data(),
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnDemand,
        decoder.addressof());
    if (FAILED(hr))
        return hr;

    wil::com_ptr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, frame.addressof());
    if (FAILED(hr))
        return hr;

    hr = CreateTextureFromWIC(d3dDevice, frame.get(), maxsize,
        resFlags, loadFlags,
        texture, decodedData, subresource);

    if (SUCCEEDED(hr))
    {
        SetDebugTextureInfo(filename, texture);
    }

    return hr;
}

_Use_decl_annotations_
HRESULT directxtk::CreateWICTextureFromFileEx(
    ID3D12Device* d3dDevice,
    ResourceUploadBatch& resourceUpload,
    const std::wstring_view& filename,
    size_t maxsize,
    D3D12_RESOURCE_FLAGS resFlags,
    WIC_LOADER_FLAGS loadFlags,
    ID3D12Resource** texture)
{
    if (texture)
    {
        *texture = nullptr;
    }

    if (!d3dDevice || filename.empty() || !texture)
        return E_INVALIDARG;

    auto pWIC = _GetWIC();
    if (!pWIC)
        return E_NOINTERFACE;

    // Initialize WIC
    wil::com_ptr<IWICBitmapDecoder> decoder;
    HRESULT hr = pWIC->CreateDecoderFromFilename(filename.data(),
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnDemand,
        decoder.addressof());
    if (FAILED(hr))
        return hr;

    wil::com_ptr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, frame.addressof());
    if (FAILED(hr))
        return hr;

    if (loadFlags & WIC_LOADER_MIP_AUTOGEN)
    {
        DXGI_FORMAT fmt = GetPixelFormat(frame.get());
        if (!resourceUpload.IsSupportedForGenerateMips(fmt))
        {
            DebugTrace("WARNING: Autogen of mips ignored (device doesn't support this format (%d) or trying to use a copy queue)\n", static_cast<int32_t>(fmt));
            loadFlags &= ~WIC_LOADER_MIP_AUTOGEN;
        }
    }

    std::unique_ptr<uint8_t[]> decodedData;
    D3D12_SUBRESOURCE_DATA initData;
    hr = CreateTextureFromWIC(d3dDevice, frame.get(), maxsize,
        resFlags, loadFlags,
        texture, decodedData, initData);

    if (SUCCEEDED(hr))
    {
        SetDebugTextureInfo(filename, texture);

        resourceUpload.Upload(
            *texture,
            0,
            &initData,
            1);

        resourceUpload.Transition(
            *texture,
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        // Generate mips?
        if (loadFlags & WIC_LOADER_MIP_AUTOGEN)
        {
            resourceUpload.GenerateMips(*texture);
        }
    }

    return hr;
}
