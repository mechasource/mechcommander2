//--------------------------------------------------------------------------------------
// File: SpriteBatch.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#pragma once

#if defined(_XBOX_ONE) && defined(_TITLE)
#include <d3d12_x.h>
#else
#include <d3d12.h>
#include <dxgi.h>
#endif

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <functional>
#include <memory>

#include "rendertargetstate.h"


namespace directxtk
{
    class ResourceUploadBatch;

    enum SpriteSortMode
    {
        SpriteSortMode_Deferred,
        SpriteSortMode_Immediate,
        SpriteSortMode_Texture,
        SpriteSortMode_BackToFront,
        SpriteSortMode_FrontToBack,
    };

    enum SpriteEffects : uint32_t
    {
        SpriteEffects_None = 0,
        SpriteEffects_FlipHorizontally = 1,
        SpriteEffects_FlipVertically = 2,
        SpriteEffects_FlipBoth = SpriteEffects_FlipHorizontally | SpriteEffects_FlipVertically,
    };

    class SpriteBatchPipelineStateDescription
    {
    public:
        explicit SpriteBatchPipelineStateDescription(
            const RenderTargetState& renderTarget,
            _In_opt_ const D3D12_BLEND_DESC* blend = nullptr,
            _In_opt_ const D3D12_DEPTH_STENCIL_DESC* depthStencil = nullptr,
            _In_opt_ const D3D12_RASTERIZER_DESC* rasterizer = nullptr,
            _In_opt_ const D3D12_GPU_DESCRIPTOR_HANDLE* isamplerDescriptor = nullptr) noexcept
            :
            blendDesc(blend ? *blend : s_DefaultBlendDesc),
            depthStencilDesc(depthStencil ? *depthStencil : s_DefaultDepthStencilDesc),
            rasterizerDesc(rasterizer ? *rasterizer : s_DefaultRasterizerDesc),
            renderTargetState(renderTarget),
            samplerDescriptor{},
            customRootSignature(nullptr),
            customVertexShader{},
            customPixelShader{}
        {
            if (isamplerDescriptor)
                this->samplerDescriptor = *isamplerDescriptor;
        }

        D3D12_BLEND_DESC            blendDesc;
        D3D12_DEPTH_STENCIL_DESC    depthStencilDesc;
        D3D12_RASTERIZER_DESC       rasterizerDesc;
        RenderTargetState           renderTargetState;
        D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor;
        ID3D12RootSignature*        customRootSignature;
        D3D12_SHADER_BYTECODE       customVertexShader;
        D3D12_SHADER_BYTECODE       customPixelShader;

    private:
        static const D3D12_BLEND_DESC           s_DefaultBlendDesc;
        static const D3D12_RASTERIZER_DESC      s_DefaultRasterizerDesc;
        static const D3D12_DEPTH_STENCIL_DESC   s_DefaultDepthStencilDesc;
    };

    class SpriteBatch
    {
    public:
        SpriteBatch(_In_ ID3D12Device* device, ResourceUploadBatch& upload, const SpriteBatchPipelineStateDescription& psoDesc, _In_opt_ const D3D12_VIEWPORT* viewport = nullptr);
        SpriteBatch(SpriteBatch&& moveFrom) noexcept;
        SpriteBatch& operator= (SpriteBatch&& moveFrom) noexcept;

        SpriteBatch(SpriteBatch const&) = delete;
        SpriteBatch& operator= (SpriteBatch const&) = delete;

        virtual ~SpriteBatch();

        // Begin/End a batch of sprite drawing operations.
        void XM_CALLCONV Begin(
            _In_ ID3D12GraphicsCommandList* commandList,
            SpriteSortMode sortMode = SpriteSortMode_Deferred,
            DirectX::FXMMATRIX transformMatrix = MatrixIdentity);
        void XM_CALLCONV Begin(
            _In_ ID3D12GraphicsCommandList* commandList,
            D3D12_GPU_DESCRIPTOR_HANDLE sampler,
            SpriteSortMode sortMode = SpriteSortMode_Deferred,
            DirectX::FXMMATRIX transformMatrix = MatrixIdentity);
        void __cdecl End();

        // Draw overloads specifying position, origin and scale as DirectX::XMFLOAT2.
        void XM_CALLCONV Draw(D3D12_GPU_DESCRIPTOR_HANDLE textureSRV, DirectX::XMUINT2 const& textureSize, DirectX::XMFLOAT2 const& position, DirectX::FXMVECTOR color = DirectX::Colors::White);
        void XM_CALLCONV Draw(D3D12_GPU_DESCRIPTOR_HANDLE textureSRV, DirectX::XMUINT2 const& textureSize, DirectX::XMFLOAT2 const& position, _In_opt_ RECT const* sourceRectangle, DirectX::FXMVECTOR color = DirectX::Colors::White, float rotation = 0, DirectX::XMFLOAT2 const& origin = Float2Zero, float scale = 1, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0);
        void XM_CALLCONV Draw(D3D12_GPU_DESCRIPTOR_HANDLE textureSRV, DirectX::XMUINT2 const& textureSize, DirectX::XMFLOAT2 const& position, _In_opt_ RECT const* sourceRectangle, DirectX::FXMVECTOR color, float rotation, DirectX::XMFLOAT2 const& origin, DirectX::XMFLOAT2 const& scale, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0);

        // Draw overloads specifying position, origin and scale via the first two components of an DirectX::XMVECTOR.
        void XM_CALLCONV Draw(D3D12_GPU_DESCRIPTOR_HANDLE textureSRV, DirectX::XMUINT2 const& textureSize, DirectX::FXMVECTOR position, DirectX::FXMVECTOR color = DirectX::Colors::White);
        void XM_CALLCONV Draw(D3D12_GPU_DESCRIPTOR_HANDLE textureSRV, DirectX::XMUINT2 const& textureSize, DirectX::FXMVECTOR position, _In_opt_ RECT const* sourceRectangle, DirectX::FXMVECTOR color = DirectX::Colors::White, float rotation = 0, DirectX::FXMVECTOR origin = DirectX::g_XMZero, float scale = 1, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0);
        void XM_CALLCONV Draw(D3D12_GPU_DESCRIPTOR_HANDLE textureSRV, DirectX::XMUINT2 const& textureSize, DirectX::FXMVECTOR position, _In_opt_ RECT const* sourceRectangle, DirectX::FXMVECTOR color, float rotation, DirectX::FXMVECTOR origin, DirectX::GXMVECTOR scale, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0);

        // Draw overloads specifying position as a RECT.
        void XM_CALLCONV Draw(D3D12_GPU_DESCRIPTOR_HANDLE textureSRV, DirectX::XMUINT2 const& textureSize, RECT const& destinationRectangle, DirectX::FXMVECTOR color = DirectX::Colors::White);
        void XM_CALLCONV Draw(D3D12_GPU_DESCRIPTOR_HANDLE textureSRV, DirectX::XMUINT2 const& textureSize, RECT const& destinationRectangle, _In_opt_ RECT const* sourceRectangle, DirectX::FXMVECTOR color = DirectX::Colors::White, float rotation = 0, DirectX::XMFLOAT2 const& origin = Float2Zero, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0);

        // Rotation mode to be applied to the sprite transformation
        void __cdecl SetRotation(DXGI_MODE_ROTATION mode);
        DXGI_MODE_ROTATION __cdecl GetRotation() const noexcept;

        // Set viewport for sprite transformation
        void __cdecl SetViewport(const D3D12_VIEWPORT& viewPort);

    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pimpl;

        static const DirectX::XMMATRIX MatrixIdentity;
        static const DirectX::XMFLOAT2 Float2Zero;
    };
}
