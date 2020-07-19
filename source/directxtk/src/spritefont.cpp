//--------------------------------------------------------------------------------------
// File: SpriteFont.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"

#include "d3dx12.h"
#include "spritefont.h"
#include "directxhelpers.h"
#include "binaryreader.h"
#include "loaderhelpers.h"
#include "resourceuploadbatch.h"
#include "descriptorheap.h"

using namespace directxtk;
// using Microsoft::WRL::ComPtr;


// Internal SpriteFont implementation class.
class SpriteFont::Impl
{
public:
    Impl(_In_ ID3D12Device* device,
        ResourceUploadBatch& upload,
        _In_ BinaryReader* reader,
        D3D12_CPU_DESCRIPTOR_HANDLE cpuDesc,
        D3D12_GPU_DESCRIPTOR_HANDLE gpuDesc,
        bool forceSRGB) noexcept(false);
    Impl(D3D12_GPU_DESCRIPTOR_HANDLE texture,
        DirectX::XMUINT2 textureSize,
        _In_reads_(glyphcount) Glyph const* glyphs,
        size_t glyphcount,
        float lineSpacing) noexcept(false);

    Glyph const* FindGlyph(wchar_t character) const;

    void SetDefaultCharacter(wchar_t character);

    template<typename TAction>
    void ForEachGlyph(_In_ const std::wstring_view&  text, TAction action, bool ignorewhitespace) const;

    void CreateTextureResource(_In_ ID3D12Device* device,
        ResourceUploadBatch& upload,
        uint32_t width, uint32_t height,
        DXGI_FORMAT format,
        uint32_t stride, uint32_t rows,
        _In_reads_(stride * rows) const uint8_t* data) noexcept(false);

    const std::wstring_view& ConvertUTF8(_In_z_ const std::string_view& text) noexcept(false);

    // Fields.
    wil::com_ptr<ID3D12Resource> textureResource;
    D3D12_GPU_DESCRIPTOR_HANDLE texture;
    DirectX::XMUINT2 textureSize;
    std::vector<Glyph> glyphs;
    std::vector<uint32_t> glyphsIndex;
    Glyph const* defaultGlyph;
    float lineSpacing;

private:
    size_t m_utfbuffersize;
    std::unique_ptr<wchar_t[]> m_utfbuffer;
};


// Constants.
const DirectX::XMFLOAT2 SpriteFont::Float2Zero(0, 0);

static const char spriteFontMagic[] = "DXTKfont";


// Comparison operators make our sorted glyph vector work with std::binary_search and lower_bound.
namespace directxtk
{
    static inline bool operator< (SpriteFont::Glyph const& left, SpriteFont::Glyph const& right) noexcept
    {
        return left.Character < right.Character;
    }

    static inline bool operator< (wchar_t left, SpriteFont::Glyph const& right) noexcept
    {
        return left < right.Character;
    }

    static inline bool operator< (SpriteFont::Glyph const& left, wchar_t right) noexcept
    {
        return left.Character < right;
    }
}


// Reads a SpriteFont from the binary format created by the MakeSpriteFont utility.
_Use_decl_annotations_
SpriteFont::Impl::Impl(
    ID3D12Device* device,
    ResourceUploadBatch& upload,
    BinaryReader* reader,
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDesc,
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDesc,
    bool forceSRGB) noexcept(false) :
        texture{},
        textureSize{},
        defaultGlyph(nullptr),
        lineSpacing(0),
        m_utfbuffersize(0)
{
    // Validate the header.
    for (char const* magic = spriteFontMagic; *magic; magic++)
    {
        if (reader->Read<uint8_t>() != *magic)
        {
            DebugTrace("ERROR: SpriteFont provided with an invalid .spritefont file\n");
            throw std::exception("Not a MakeSpriteFont output binary");
        }
    }

    // Read the glyph data.
    auto glyphcount = reader->Read<uint32_t>();
    auto glyphdata = reader->ReadArray<Glyph>(glyphcount);

    glyphs.assign(glyphdata, glyphdata + glyphcount);
    glyphsIndex.reserve(glyphs.size());

    for (auto& glyph : glyphs)
    {
        glyphsIndex.emplace_back(glyph.Character);
    }

    // Read font properties.
    lineSpacing = reader->Read<float>();

    SetDefaultCharacter(static_cast<wchar_t>(reader->Read<uint32_t>()));

    // Read the texture data.
    auto textureWidth = reader->Read<uint32_t>();
    auto textureHeight = reader->Read<uint32_t>();
    auto textureFormat = reader->Read<DXGI_FORMAT>();
    auto textureStride = reader->Read<uint32_t>();
    auto textureRows = reader->Read<uint32_t>();

    uint64_t datasize = uint64_t(textureStride) * uint64_t(textureRows);
    if (datasize > UINT32_MAX)
    {
        DebugTrace("ERROR: SpriteFont provided with an invalid .spritefont file\n");
        throw std::overflow_error("Invalid .spritefont file");
    }

    auto textureData = reader->ReadArray<uint8_t>(static_cast<size_t>(datasize));

    if (forceSRGB)
    {
        textureFormat = LoaderHelpers::MakeSRGB(textureFormat);
    }

    // Create the D3D texture.
    CreateTextureResource(
        device,
        upload,
        textureWidth, textureHeight,
        textureFormat,
        textureStride, textureRows,
        textureData);

    // Create the shader resource view
    CreateShaderResourceView(
        device, textureResource.get(),
        cpuDesc, false);

    // Save off the GPU descriptor pointer and size.
    texture = gpuDesc;
    textureSize = DirectX::XMUINT2(textureWidth, textureHeight);
}


// Constructs a SpriteFont from arbitrary user specified glyph data.
_Use_decl_annotations_
SpriteFont::Impl::Impl(
    D3D12_GPU_DESCRIPTOR_HANDLE itexture,
    DirectX::XMUINT2 itextureSize,
    Glyph const* iglyphs,
    size_t glyphcount,
    float ilineSpacing) noexcept(false) :
        texture(itexture),
        textureSize(itextureSize),
        glyphs(iglyphs, iglyphs + glyphcount),
        defaultGlyph(nullptr),
        lineSpacing(ilineSpacing),
        m_utfbuffersize(0)
{
    if (!std::is_sorted(iglyphs, iglyphs + glyphcount))
    {
        throw std::exception("Glyphs must be in ascending codepoint order");
    }

    glyphsIndex.reserve(glyphs.size());

    for (auto& glyph : glyphs)
    {
        glyphsIndex.emplace_back(glyph.Character);
    }
}


// Looks up the requested glyph, falling back to the default character if it is not in the font.
SpriteFont::Glyph const* SpriteFont::Impl::FindGlyph(wchar_t character) const
{
    // Rather than use std::lower_bound (which includes a slow debug path when built for _DEBUG),
    // we implement a binary search inline to ensure sufficient Debug build performance to be useful
    // for text-heavy applications.

    size_t lower = 0;
    size_t higher = glyphs.size() - 1;
    size_t index = higher / 2;
    const size_t size = glyphs.size();

    while (index < size)
    {
        const auto curChar = glyphsIndex[index];
        if (curChar == character) { return &glyphs[index]; }
        if (curChar < character)
        {
            lower = index + 1;
        }
        else
        {
            higher = index - 1;
        }
        if (higher < lower) { break; }
        else if (higher - lower <= 4)
        {
            for (index = lower; index <= higher; index++)
            {
                if (glyphsIndex[index] == character)
                {
                    return &glyphs[index];
                }
            }
        }
        index = lower + ((higher - lower) / 2);
    }

    if (defaultGlyph)
    {
        return defaultGlyph;
    }

    DebugTrace("ERROR: SpriteFont encountered a character not in the font (%u, %C), and no default glyph was provided\n", character, character);
    throw std::exception("Character not in font");
}


// Sets the missing-character fallback glyph.
void SpriteFont::Impl::SetDefaultCharacter(wchar_t character)
{
    defaultGlyph = nullptr;

    if (character)
    {
        defaultGlyph = FindGlyph(character);
    }
}


// The core glyph layout algorithm, shared between DrawString and MeasureString.
template<typename TAction>
void SpriteFont::Impl::ForEachGlyph(_In_ const std::wstring_view& text, TAction action, bool ignorewhitespace) const
{
    float x = 0;
    float y = 0;

	for(wchar_t character : text) 
	{
		switch (character)
        {
            case '\r':
                // Skip carriage returns.
                continue;

            case '\n':
                // New line.
                x = 0;
                y += lineSpacing;
                break;

            default:
                // Output this character.
                auto glyph = FindGlyph(character);

                x += glyph->XOffset;

                if (x < 0)
                    x = 0;

                float advance = float(glyph->Subrect.right) - float(glyph->Subrect.left) + glyph->XAdvance;

                if (!ignorewhitespace
                    || !iswspace(character)
                    || ((glyph->Subrect.right - glyph->Subrect.left) > 1)
                    || ((glyph->Subrect.bottom - glyph->Subrect.top) > 1))
                {
                    action(glyph, x, y, advance);
                }

                x += advance;
                break;
        }
    }
}


_Use_decl_annotations_
void SpriteFont::Impl::CreateTextureResource(
    ID3D12Device* device,
    ResourceUploadBatch& upload,
    uint32_t width, uint32_t height,
    DXGI_FORMAT format,
    uint32_t stride, uint32_t rows,
    const uint8_t* data) noexcept(false)
{
    uint64_t sliceBytes = uint64_t(stride) * uint64_t(rows);
    if (sliceBytes > UINT32_MAX)
    {
        DebugTrace("ERROR: SpriteFont provided with an invalid .spritefont file\n");
        throw std::overflow_error("Invalid .spritefont file");
    }

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = width;
    desc.Height = height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = format;
    desc.SampleDesc.Count = 1;

    CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

    ThrowIfFailed(device->CreateCommittedResource(
        &defaultHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_GRAPHICS_PPV_ARGS(textureResource.put())));

    SetDebugObjectName(textureResource.get(), L"SpriteFont:Texture");

    D3D12_SUBRESOURCE_DATA initData = { data, static_cast<intptr_t>(stride), static_cast<intptr_t>(sliceBytes) };

    upload.Upload(
        textureResource.get(),
        0,
        &initData,
        1);

    upload.Transition(
        textureResource.get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}


const std::wstring_view& SpriteFont::Impl::ConvertUTF8(_In_ const std::string_view& text) noexcept(false)
{
    if (!m_utfbuffer)
    {
        m_utfbuffersize = 1024;
        m_utfbuffer.reset(new wchar_t[1024]);
    }

    int32_t result = MultiByteToWideChar(CP_UTF8, 0, text.data(), -1, m_utfbuffer.get(), static_cast<int32_t>(m_utfbuffersize));
    if (!result && (GetLastError() == ERROR_INSUFFICIENT_BUFFER))
    {
        // Compute required buffer size
        result = MultiByteToWideChar(CP_UTF8, 0, text.data(), -1, nullptr, 0);
        m_utfbuffersize = AlignUp(static_cast<size_t>(result), 1024u);
        m_utfbuffer.reset(new wchar_t[m_utfbuffersize]);

        // Retry conversion
        result = MultiByteToWideChar(CP_UTF8, 0, text.data(), -1, m_utfbuffer.get(), static_cast<int32_t>(m_utfbuffersize));
    }

    if (!result)
    {
        DebugTrace("ERROR: MultiByteToWideChar failed with error %u.\n", GetLastError());
        throw std::exception("MultiByteToWideChar");
    }

    return m_utfbuffer.get();	// C4172: returning address of local variable or temporary
}


// Construct from a binary file created by the MakeSpriteFont utility.
_Use_decl_annotations_
SpriteFont::SpriteFont(ID3D12Device* device, ResourceUploadBatch& upload, const std::wstring_view& filename, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorDest, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorDest, bool forceSRGB)
{
    BinaryReader reader(filename);

    pimpl = std::make_unique<Impl>(device, upload, &reader, cpuDescriptorDest, gpuDescriptorDest, forceSRGB);
}


// Construct from a binary blob created by the MakeSpriteFont utility and already loaded into memory.
_Use_decl_annotations_
SpriteFont::SpriteFont(ID3D12Device* device, ResourceUploadBatch& upload, uint8_t const* datablob, size_t datasize, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorDest, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorDest, bool forceSRGB)
{
    BinaryReader reader(datablob, datasize);

    pimpl = std::make_unique<Impl>(device, upload, &reader, cpuDescriptorDest, gpuDescriptorDest, forceSRGB);
}


// Construct from arbitrary user specified glyph data (for those not using the MakeSpriteFont utility).
_Use_decl_annotations_
SpriteFont::SpriteFont(D3D12_GPU_DESCRIPTOR_HANDLE texture, DirectX::XMUINT2 textureSize, Glyph const* glyphs, size_t glyphcount, float lineSpacing)
    : pimpl(std::make_unique<Impl>(texture, textureSize, glyphs, glyphcount, lineSpacing))
{
}


// Move constructor.
SpriteFont::SpriteFont(SpriteFont&& moveFrom) noexcept
    : pimpl(std::move(moveFrom.pimpl))
{
}


// Move assignment.
SpriteFont& SpriteFont::operator= (SpriteFont&& moveFrom) noexcept
{
    pimpl = std::move(moveFrom.pimpl);
    return *this;
}


// Public destructor.
SpriteFont::~SpriteFont()
{
}


// Wide-character / UTF-16LE
void XM_CALLCONV SpriteFont::DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::wstring_view&  text, DirectX::XMFLOAT2 const& position, DirectX::FXMVECTOR color, float rotation, DirectX::XMFLOAT2 const& origin, float scale, SpriteEffects effects, float layerDepth) const
{
    DrawString(spriteBatch, text, XMLoadFloat2(&position), color, rotation, XMLoadFloat2(&origin), DirectX::XMVectorReplicate(scale), effects, layerDepth);
}


void XM_CALLCONV SpriteFont::DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::wstring_view& text, DirectX::XMFLOAT2 const& position, DirectX::FXMVECTOR color, float rotation, DirectX::XMFLOAT2 const& origin, DirectX::XMFLOAT2 const& scale, SpriteEffects effects, float layerDepth) const
{
    DrawString(spriteBatch, text, XMLoadFloat2(&position), color, rotation, XMLoadFloat2(&origin), XMLoadFloat2(&scale), effects, layerDepth);
}


void XM_CALLCONV SpriteFont::DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::wstring_view& text, DirectX::FXMVECTOR position, DirectX::FXMVECTOR color, float rotation, DirectX::FXMVECTOR origin, float scale, SpriteEffects effects, float layerDepth) const
{
    DrawString(spriteBatch, text, position, color, rotation, origin, DirectX::XMVectorReplicate(scale), effects, layerDepth);
}


void XM_CALLCONV SpriteFont::DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::wstring_view& text, DirectX::FXMVECTOR position, DirectX::FXMVECTOR color, float rotation, DirectX::FXMVECTOR origin, DirectX::GXMVECTOR scale, SpriteEffects effects, float layerDepth) const
{
    static_assert(SpriteEffects_FlipHorizontally == 1 &&
                  SpriteEffects_FlipVertically == 2, "If you change these enum values, the following tables must be updated to match");

    // Lookup table indicates which way to move along each axis per SpriteEffects enum value.
    static DirectX::XMVECTORF32 axisDirectionTable[4] =
    {
        { { { -1, -1, 0, 0 } } },
        { { {  1, -1, 0, 0 } } },
        { { { -1,  1, 0, 0 } } },
        { { {  1,  1, 0, 0 } } },
    };

    // Lookup table indicates which axes are mirrored for each SpriteEffects enum value.
    static DirectX::XMVECTORF32 axisIsMirroredTable[4] =
    {
        { { { 0, 0, 0, 0 } } },
        { { { 1, 0, 0, 0 } } },
        { { { 0, 1, 0, 0 } } },
        { { { 1, 1, 0, 0 } } },
    };

    DirectX::XMVECTOR baseOffset = origin;

    // If the text is mirrored, offset the start position accordingly.
    if (effects)
    {
        baseOffset = XMVectorNegativeMultiplySubtract(
            MeasureString(text),
            axisIsMirroredTable[effects & 3],
            baseOffset);
    }

    // Draw each character in turn.
    pimpl->ForEachGlyph(text, [&](Glyph const* glyph, float x, float y, float advance)
    {
        UNREFERENCED_PARAMETER(advance);

        DirectX::XMVECTOR offset = DirectX::XMVectorMultiplyAdd(DirectX::XMVectorSet(x, y + glyph->YOffset, 0, 0), axisDirectionTable[effects & 3], baseOffset);

        if (effects)
        {
            // For mirrored characters, specify bottom and/or right instead of top left.
            DirectX::XMVECTOR glyphRect = DirectX::XMConvertVectorIntToFloat(DirectX::XMLoadInt4(reinterpret_cast<uint32_t const*>(&glyph->Subrect)), 0);

            // xy = glyph width/height.
            glyphRect = DirectX::XMVectorSubtract(DirectX::XMVectorSwizzle<2, 3, 0, 1>(glyphRect), glyphRect);

            offset = DirectX::XMVectorMultiplyAdd(glyphRect, axisIsMirroredTable[effects & 3], offset);
        }

        spriteBatch->Draw(pimpl->texture, pimpl->textureSize, position, &glyph->Subrect, color, rotation, offset, scale, effects, layerDepth);
    }, true);
}


DirectX::XMVECTOR XM_CALLCONV SpriteFont::MeasureString(_In_ const std::wstring_view& text, bool ignorewhitespace) const
{
    DirectX::XMVECTOR result = DirectX::XMVectorZero();

    pimpl->ForEachGlyph(text, [&](Glyph const* glyph, float x, float y, float advance)
        {
            UNREFERENCED_PARAMETER(advance);

            auto w = static_cast<float>(glyph->Subrect.right - glyph->Subrect.left);
            auto h = static_cast<float>(glyph->Subrect.bottom - glyph->Subrect.top) + glyph->YOffset;

            h = iswspace(wchar_t(glyph->Character)) ?
                pimpl->lineSpacing :
                std::max(h, pimpl->lineSpacing);

            result = DirectX::XMVectorMax(result, DirectX::XMVectorSet(x + w, y + h, 0, 0));
        }, ignorewhitespace);

    return result;
}


RECT SpriteFont::MeasureDrawBounds(_In_ const std::wstring_view& text, DirectX::XMFLOAT2 const& position, bool ignorewhitespace) const
{
    RECT result = { LONG_MAX, LONG_MAX, 0, 0 };

    pimpl->ForEachGlyph(text, [&](Glyph const* glyph, float x, float y, float advance) noexcept
        {
            auto isWhitespace = iswspace(wchar_t(glyph->Character));
            auto w = static_cast<float>(glyph->Subrect.right - glyph->Subrect.left);
            auto h = isWhitespace ?
                pimpl->lineSpacing :
                static_cast<float>(glyph->Subrect.bottom - glyph->Subrect.top);

            float minX = position.x + x;
            float minY = position.y + y + (isWhitespace ? 0.0f : glyph->YOffset);

            float maxX = std::max(minX + advance, minX + w);
            float maxY = minY + h;

            if (minX < float(result.left))
                result.left = int32_t(minX);

            if (minY < float(result.top))
                result.top = int32_t(minY);

            if (float(result.right) < maxX)
                result.right = int32_t(maxX);

            if (float(result.bottom) < maxY)
                result.bottom = int32_t(maxY);
        }, ignorewhitespace);

    if (result.left == LONG_MAX)
    {
        result.left = 0;
        result.top = 0;
    }

    return result;
}


RECT XM_CALLCONV SpriteFont::MeasureDrawBounds(_In_ const std::wstring_view& text, DirectX::FXMVECTOR position, bool ignorewhitespace) const
{
    DirectX::XMFLOAT2 pos;
    XMStoreFloat2(&pos, position);

    return MeasureDrawBounds(text, pos, ignorewhitespace);
}


// UTF-8
void XM_CALLCONV SpriteFont::DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::string_view& text, DirectX::XMFLOAT2 const& position, DirectX::FXMVECTOR color, float rotation, DirectX::XMFLOAT2 const& origin, float scale, SpriteEffects effects, float layerDepth) const
{
    DrawString(spriteBatch, pimpl->ConvertUTF8(text), XMLoadFloat2(&position), color, rotation, XMLoadFloat2(&origin), DirectX::XMVectorReplicate(scale), effects, layerDepth);
}


void XM_CALLCONV SpriteFont::DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::string_view& text, DirectX::XMFLOAT2 const& position, DirectX::FXMVECTOR color, float rotation, DirectX::XMFLOAT2 const& origin, DirectX::XMFLOAT2 const& scale, SpriteEffects effects, float layerDepth) const
{
    DrawString(spriteBatch, pimpl->ConvertUTF8(text), XMLoadFloat2(&position), color, rotation, XMLoadFloat2(&origin), XMLoadFloat2(&scale), effects, layerDepth);
}


void XM_CALLCONV SpriteFont::DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::string_view& text, DirectX::FXMVECTOR position, DirectX::FXMVECTOR color, float rotation, DirectX::FXMVECTOR origin, float scale, SpriteEffects effects, float layerDepth) const
{
    DrawString(spriteBatch, pimpl->ConvertUTF8(text), position, color, rotation, origin, DirectX::XMVectorReplicate(scale), effects, layerDepth);
}


void XM_CALLCONV SpriteFont::DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::string_view& text, DirectX::FXMVECTOR position, DirectX::FXMVECTOR color, float rotation, DirectX::FXMVECTOR origin, DirectX::GXMVECTOR scale, SpriteEffects effects, float layerDepth) const
{
    DrawString(spriteBatch, pimpl->ConvertUTF8(text), position, color, rotation, origin, scale, effects, layerDepth);
}


DirectX::XMVECTOR XM_CALLCONV SpriteFont::MeasureString(_In_ const std::string_view& text, bool ignorewhitespace) const
{
    return MeasureString(pimpl->ConvertUTF8(text), ignorewhitespace);
}


RECT SpriteFont::MeasureDrawBounds(_In_ const std::string_view& text, DirectX::XMFLOAT2 const& position, bool ignorewhitespace) const
{
    return MeasureDrawBounds(pimpl->ConvertUTF8(text), position, ignorewhitespace);
}


RECT XM_CALLCONV SpriteFont::MeasureDrawBounds(_In_ const std::string_view& text, DirectX::FXMVECTOR position, bool ignorewhitespace) const
{
    DirectX::XMFLOAT2 pos;
    XMStoreFloat2(&pos, position);

    return MeasureDrawBounds(pimpl->ConvertUTF8(text), pos, ignorewhitespace);
}


// Spacing properties
float SpriteFont::GetLineSpacing() const noexcept
{
    return pimpl->lineSpacing;
}


void SpriteFont::SetLineSpacing(float spacing)
{
    pimpl->lineSpacing = spacing;
}


// Font properties
wchar_t SpriteFont::GetDefaultCharacter() const noexcept
{
    return static_cast<wchar_t>(pimpl->defaultGlyph ? pimpl->defaultGlyph->Character : 0);
}


void SpriteFont::SetDefaultCharacter(wchar_t character)
{
    pimpl->SetDefaultCharacter(character);
}


bool SpriteFont::ContainsCharacter(wchar_t character) const
{
    return std::binary_search(pimpl->glyphs.begin(), pimpl->glyphs.end(), character);
}


// Custom layout/rendering
SpriteFont::Glyph const* SpriteFont::FindGlyph(wchar_t character) const
{
    return pimpl->FindGlyph(character);
}


D3D12_GPU_DESCRIPTOR_HANDLE SpriteFont::GetSpriteSheet() const noexcept
{
    return pimpl->texture;
}


DirectX::XMUINT2 SpriteFont::GetSpriteSheetSize() const noexcept
{
    return pimpl->textureSize;
}
