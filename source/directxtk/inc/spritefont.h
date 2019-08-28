//--------------------------------------------------------------------------------------
// File: SpriteFont.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#pragma once

#include "spritebatch.h"

namespace DirectX
{
class SpriteFont
{
public:
	struct Glyph;

	SpriteFont(ID3D12Device* device, ResourceUploadBatch& upload, _In_z_ const std::wstring_view& fileName, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorDest, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptor, bool forceSRGB = false);
	SpriteFont(ID3D12Device* device, ResourceUploadBatch& upload, _In_reads_bytes_(dataSize) uint8_t const* dataBlob, size_t dataSize, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorDest, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptor, bool forceSRGB = false);
	SpriteFont(D3D12_GPU_DESCRIPTOR_HANDLE texture, XMUINT2 textureSize, _In_reads_(glyphCount) Glyph const* glyphs, size_t glyphCount, float lineSpacing);

	SpriteFont(SpriteFont&& moveFrom) noexcept;
	SpriteFont& operator=(SpriteFont&& moveFrom) noexcept;

	SpriteFont(SpriteFont const&) = delete;
	SpriteFont& operator=(SpriteFont const&) = delete;

	virtual ~SpriteFont();

	// Wide-character / UTF-16LE
	void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_z_ const std::wstring_view& text, XMFLOAT2 const& position, FXMVECTOR color = colours::White, float rotation = 0, XMFLOAT2 const& origin = Float2Zero, float scale = 1, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;
	void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_z_ const std::wstring_view& text, XMFLOAT2 const& position, FXMVECTOR color, float rotation, XMFLOAT2 const& origin, XMFLOAT2 const& scale, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;
	void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_z_ const std::wstring_view& text, FXMVECTOR position, FXMVECTOR color = colours::White, float rotation = 0, FXMVECTOR origin = g_XMZero, float scale = 1, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;
	void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_z_ const std::wstring_view& text, FXMVECTOR position, FXMVECTOR color, float rotation, FXMVECTOR origin, GXMVECTOR scale, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;

	XMVECTOR XM_CALLCONV MeasureString(_In_z_ const std::wstring_view& text) const;

	RECT __cdecl MeasureDrawBounds(_In_z_ const std::wstring_view& text, XMFLOAT2 const& position) const;
	RECT XM_CALLCONV MeasureDrawBounds(_In_z_ const std::wstring_view& text, FXMVECTOR position) const;

	// UTF-8
	void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_z_ const std::string_view& text, XMFLOAT2 const& position, FXMVECTOR color = colours::White, float rotation = 0, XMFLOAT2 const& origin = Float2Zero, float scale = 1, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;
	void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_z_ const std::string_view& text, XMFLOAT2 const& position, FXMVECTOR color, float rotation, XMFLOAT2 const& origin, XMFLOAT2 const& scale, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;
	void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_z_ const std::string_view& text, FXMVECTOR position, FXMVECTOR color = colours::White, float rotation = 0, FXMVECTOR origin = g_XMZero, float scale = 1, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;
	void XM_CALLCONV DrawString(_In_ SpriteBatch* spriteBatch, _In_z_ const std::string_view& text, FXMVECTOR position, FXMVECTOR color, float rotation, FXMVECTOR origin, GXMVECTOR scale, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0) const;

	XMVECTOR XM_CALLCONV MeasureString(_In_z_ const std::string_view& text) const;

	RECT __cdecl MeasureDrawBounds(_In_z_ const std::string_view& text, XMFLOAT2 const& position) const;
	RECT XM_CALLCONV MeasureDrawBounds(_In_z_ const std::string_view& text, FXMVECTOR position) const;

	// Spacing properties
	float __cdecl GetLineSpacing(void) const;
	void __cdecl SetLineSpacing(float spacing);

	// Font properties
	wchar_t __cdecl GetDefaultCharacter(void) const;
	void __cdecl SetDefaultCharacter(wchar_t character);

	bool __cdecl ContainsCharacter(wchar_t character) const;

	// Custom layout/rendering
	Glyph const* __cdecl FindGlyph(wchar_t character) const;
	D3D12_GPU_DESCRIPTOR_HANDLE __cdecl GetSpriteSheet(void) const;
	XMUINT2 __cdecl GetSpriteSheetSize(void) const;

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

	std::unique_ptr<Impl> pImpl;

	static const XMFLOAT2 Float2Zero;
};
} // namespace DirectX
