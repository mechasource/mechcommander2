//--------------------------------------------------------------------------------------
// File: SpriteFont.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#pragma once

#include "SpriteBatch.h"


namespace directxtk
{
    class SpriteFont
    {
    public:
        struct Glyph;

        SpriteFont(ID3D12Device* device, ResourceUploadBatch& upload, _In_ const std::wstring_view&  filename, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorDest, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptor, bool forceSRGB = false);
        SpriteFont(ID3D12Device* device, ResourceUploadBatch& upload, _In_reads_bytes_(datasize) uint8_t const* datablob, size_t datasize, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorDest, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptor, bool forceSRGB = false);
        SpriteFont(D3D12_GPU_DESCRIPTOR_HANDLE texture, DirectX::XMUINT2 textureSize, _In_reads_(glyphcount) Glyph const* glyphs, size_t glyphcount, float lineSpacing);

        SpriteFont(SpriteFont&& moveFrom) noexcept;
        SpriteFont& operator= (SpriteFont&& moveFrom) noexcept;

        SpriteFont(SpriteFont const&) = delete;
        SpriteFont& operator= (SpriteFont const&) = delete;

        virtual ~SpriteFont();

        // Wide-character / UTF-16LE
        void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::wstring_view& text, DirectX::XMFLOAT2 const& position, DirectX::FXMVECTOR color = DirectX::Colors::White, float rotation = 0, DirectX::XMFLOAT2 const& origin = Float2Zero, float scale = 1, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;
        void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::wstring_view& text, DirectX::XMFLOAT2 const& position, DirectX::FXMVECTOR color, float rotation, DirectX::XMFLOAT2 const& origin, DirectX::XMFLOAT2 const& scale, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;
        void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::wstring_view& text, DirectX::FXMVECTOR position, DirectX::FXMVECTOR color = DirectX::Colors::White, float rotation = 0, DirectX::FXMVECTOR origin = DirectX::g_XMZero, float scale = 1, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;
        void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::wstring_view& text, DirectX::FXMVECTOR position, DirectX::FXMVECTOR color, float rotation, DirectX::FXMVECTOR origin, DirectX::GXMVECTOR scale, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;

        DirectX::XMVECTOR XM_CALLCONV MeasureString(_In_ const std::wstring_view&  text, bool ignorewhitespace = true) const;

        RECT __cdecl MeasureDrawBounds(_In_ const std::wstring_view&  text, DirectX::XMFLOAT2 const& position, bool ignorewhitespace = true) const;
        RECT XM_CALLCONV MeasureDrawBounds(_In_ const std::wstring_view&  text, DirectX::FXMVECTOR position, bool ignorewhitespace = true) const;

        // UTF-8
        void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::string_view& text, DirectX::XMFLOAT2 const& position, DirectX::FXMVECTOR color = DirectX::Colors::White, float rotation = 0, DirectX::XMFLOAT2 const& origin = Float2Zero, float scale = 1, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;
        void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::string_view& text, DirectX::XMFLOAT2 const& position, DirectX::FXMVECTOR color, float rotation, DirectX::XMFLOAT2 const& origin, DirectX::XMFLOAT2 const& scale, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;
        void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::string_view& text, DirectX::FXMVECTOR position, DirectX::FXMVECTOR color = DirectX::Colors::White, float rotation = 0, DirectX::FXMVECTOR origin = DirectX::g_XMZero, float scale = 1, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;
        void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_ const std::string_view& text, DirectX::FXMVECTOR position, DirectX::FXMVECTOR color, float rotation, DirectX::FXMVECTOR origin, DirectX::GXMVECTOR scale, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;

        DirectX::XMVECTOR XM_CALLCONV MeasureString(_In_ const std::string_view& text, bool ignorewhitespace = true) const;

        RECT __cdecl MeasureDrawBounds(_In_ const std::string_view& text, DirectX::XMFLOAT2 const& position, bool ignorewhitespace = true) const;
        RECT XM_CALLCONV MeasureDrawBounds(_In_ const std::string_view& text, DirectX::FXMVECTOR position, bool ignorewhitespace = true) const;

        // Spacing properties
        float __cdecl GetLineSpacing() const noexcept;
        void __cdecl SetLineSpacing(float spacing);

        // Font properties
        wchar_t __cdecl GetDefaultCharacter() const noexcept;
        void __cdecl SetDefaultCharacter(wchar_t character);

        bool __cdecl ContainsCharacter(wchar_t character) const;

        // Custom layout/rendering
        Glyph const* __cdecl FindGlyph(wchar_t character) const;
        D3D12_GPU_DESCRIPTOR_HANDLE __cdecl GetSpriteSheet() const noexcept;
        DirectX::XMUINT2 __cdecl GetSpriteSheetSize() const noexcept;

        // Describes a single character glyph.
        struct Glyph
        {
            uint32_t Character;
            RECT Subrect;
            float XOffset;
            float YOffset;
            float XAdvance;
        };


    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pimpl;

        static const DirectX::XMFLOAT2 Float2Zero;
    };
}
