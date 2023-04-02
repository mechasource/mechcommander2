//--------------------------------------------------------------------------------------
// File: Effects.h
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
#endif

#include <DirectXMath.h>
#include <memory>
#include <string>

#include "rendertargetstate.h"
#include "effectpipelinestatedescription.h"


namespace directxtk
{
    class DescriptorHeap;
    class ResourceUploadBatch;

    //----------------------------------------------------------------------------------
    // Abstract interface representing any effect which can be applied onto a D3D device context.
    class IEffect
    {
    public:
        virtual ~IEffect() = default;

        IEffect(const IEffect&) = delete;
        IEffect& operator=(const IEffect&) = delete;

        IEffect(IEffect&&) = delete;
        IEffect& operator=(IEffect&&) = delete;

        virtual void __cdecl Apply(_In_ ID3D12GraphicsCommandList* commandList) = 0;

    protected:
        IEffect() = default;
    };


    // Abstract interface for effects with world, view, and projection matrices.
    class IEffectMatrices
    {
    public:
        virtual ~IEffectMatrices() = default;

        IEffectMatrices(const IEffectMatrices&) = delete;
        IEffectMatrices& operator=(const IEffectMatrices&) = delete;

        IEffectMatrices(IEffectMatrices&&) = delete;
        IEffectMatrices& operator=(IEffectMatrices&&) = delete;

        virtual void XM_CALLCONV SetWorld(DirectX::FXMMATRIX value) = 0;
        virtual void XM_CALLCONV SetView(DirectX::FXMMATRIX value) = 0;
        virtual void XM_CALLCONV SetProjection(DirectX::FXMMATRIX value) = 0;
        virtual void XM_CALLCONV SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection);

    protected:
        IEffectMatrices() = default;
    };


    // Abstract interface for effects which support directional lighting.
    class IEffectLights
    {
    public:
        virtual ~IEffectLights() = default;

        IEffectLights(const IEffectLights&) = delete;
        IEffectLights& operator=(const IEffectLights&) = delete;

        IEffectLights(IEffectLights&&) = delete;
        IEffectLights& operator=(IEffectLights&&) = delete;

        virtual void XM_CALLCONV SetAmbientLightColor(DirectX::FXMVECTOR value) = 0;

        virtual void __cdecl SetLightEnabled(int32_t whichLight, bool value) = 0;
        virtual void XM_CALLCONV SetLightDirection(int32_t whichLight, DirectX::FXMVECTOR value) = 0;
        virtual void XM_CALLCONV SetLightDiffuseColor(int32_t whichLight, DirectX::FXMVECTOR value) = 0;
        virtual void XM_CALLCONV SetLightSpecularColor(int32_t whichLight, DirectX::FXMVECTOR value) = 0;

        virtual void __cdecl EnableDefaultLighting() = 0;

        static constexpr int32_t MaxDirectionalLights = 3;

    protected:
        IEffectLights() = default;
    };


    // Abstract interface for effects which support fog.
    class IEffectFog
    {
    public:
        virtual ~IEffectFog() = default;

        IEffectFog(const IEffectFog&) = delete;
        IEffectFog& operator=(const IEffectFog&) = delete;

        IEffectFog(IEffectFog&&) = delete;
        IEffectFog& operator=(IEffectFog&&) = delete;

        virtual void __cdecl SetFogStart(float value) = 0;
        virtual void __cdecl SetFogEnd(float value) = 0;
        virtual void XM_CALLCONV SetFogColor(DirectX::FXMVECTOR value) = 0;

    protected:
        IEffectFog() = default;
    };


    // Abstract interface for effects which support skinning
    class IEffectSkinning
    {
    public:
        virtual ~IEffectSkinning() = default;

        IEffectSkinning(const IEffectSkinning&) = delete;
        IEffectSkinning& operator=(const IEffectSkinning&) = delete;

        IEffectSkinning(IEffectSkinning&&) = delete;
        IEffectSkinning& operator=(IEffectSkinning&&) = delete;

        virtual void __cdecl SetBoneTransforms(_In_reads_(count) DirectX::XMMATRIX const* value, size_t count) = 0;
        virtual void __cdecl ResetBoneTransforms() = 0;

        static constexpr int32_t MaxBones = 72;

    protected:
        IEffectSkinning() = default;
    };


    //----------------------------------------------------------------------------------
    namespace EffectFlags
    {
        constexpr uint32_t None                = 0x00;
        constexpr uint32_t Fog                 = 0x01;
        constexpr uint32_t Lighting            = 0x02;
        constexpr uint32_t PerPixelLighting    = 0x04 | Lighting; // per pixel lighting implies lighting enabled
        constexpr uint32_t VertexColor         = 0x08;
        constexpr uint32_t Texture             = 0x10;

        constexpr uint32_t Specular            = 0x100; // enable optional specular/specularMap feature
        constexpr uint32_t Emissive            = 0x200; // enable optional emissive/emissiveMap feature
        constexpr uint32_t Fresnel             = 0x400; // enable optional Fresnel feature
        constexpr uint32_t Velocity            = 0x800; // enable optional velocity feature

        constexpr uint32_t BiasedVertexNormals = 0x10000; // compressed vertex normals need x2 bias
    }


    //----------------------------------------------------------------------------------
    // Built-in shader supports optional texture mapping, vertex coloring, directional lighting, and fog.
    class BasicEffect : public IEffect, public IEffectMatrices, public IEffectLights, public IEffectFog
    {
    public:
        BasicEffect(_In_ ID3D12Device* device, uint32_t effectFlags, const EffectPipelineStateDescription& pipelineDescription);
        BasicEffect(BasicEffect&& moveFrom) noexcept;
        BasicEffect& operator= (BasicEffect&& moveFrom) noexcept;

        BasicEffect(BasicEffect const&) = delete;
        BasicEffect& operator= (BasicEffect const&) = delete;

        ~BasicEffect() override;

        // IEffect methods.
        void __cdecl Apply(_In_ ID3D12GraphicsCommandList* commandList) override;

        // Camera settings.
        void XM_CALLCONV SetWorld(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetView(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetProjection(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection) override;

        // Material settings.
        void XM_CALLCONV SetDiffuseColor(DirectX::FXMVECTOR value);
        void XM_CALLCONV SetEmissiveColor(DirectX::FXMVECTOR value);
        void XM_CALLCONV SetSpecularColor(DirectX::FXMVECTOR value);
        void __cdecl SetSpecularPower(float value);
        void __cdecl DisableSpecular();
        void __cdecl SetAlpha(float value);
        void XM_CALLCONV SetColorAndAlpha(DirectX::FXMVECTOR value);

        // Light settings.
        void XM_CALLCONV SetAmbientLightColor(DirectX::FXMVECTOR value) override;

        void __cdecl SetLightEnabled(int32_t whichLight, bool value) override;
        void XM_CALLCONV SetLightDirection(int32_t whichLight, DirectX::FXMVECTOR value) override;
        void XM_CALLCONV SetLightDiffuseColor(int32_t whichLight, DirectX::FXMVECTOR value) override;
        void XM_CALLCONV SetLightSpecularColor(int32_t whichLight, DirectX::FXMVECTOR value) override;

        void __cdecl EnableDefaultLighting() override;

        // Fog settings.
        void __cdecl SetFogStart(float value) override;
        void __cdecl SetFogEnd(float value) override;
        void XM_CALLCONV SetFogColor(DirectX::FXMVECTOR value) override;

        // Texture setting.
        void __cdecl SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor);

    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pimpl;
    };


    // Built-in shader supports per-pixel alpha testing.
    class AlphaTestEffect : public IEffect, public IEffectMatrices, public IEffectFog
    {
    public:
        AlphaTestEffect(_In_ ID3D12Device* device, uint32_t effectFlags, const EffectPipelineStateDescription& pipelineDescription,
            D3D12_COMPARISON_FUNC alphaFunction = D3D12_COMPARISON_FUNC_GREATER);
        AlphaTestEffect(AlphaTestEffect&& moveFrom) noexcept;
        AlphaTestEffect& operator= (AlphaTestEffect&& moveFrom) noexcept;

        AlphaTestEffect(AlphaTestEffect const&) = delete;
        AlphaTestEffect& operator= (AlphaTestEffect const&) = delete;

        ~AlphaTestEffect() override;

        // IEffect methods.
        void __cdecl Apply(_In_ ID3D12GraphicsCommandList* commandList) override;

        // Camera settings.
        void XM_CALLCONV SetWorld(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetView(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetProjection(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection) override;

        // Material settings.
        void XM_CALLCONV SetDiffuseColor(DirectX::FXMVECTOR value);
        void __cdecl SetAlpha(float value);
        void XM_CALLCONV SetColorAndAlpha(DirectX::FXMVECTOR value);
        
        // Fog settings.
        void __cdecl SetFogStart(float value) override;
        void __cdecl SetFogEnd(float value) override;
        void XM_CALLCONV SetFogColor(DirectX::FXMVECTOR value) override;

        // Texture setting.
        void __cdecl SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor);

        // Alpha test settings.
        void __cdecl SetReferenceAlpha(int32_t value);

    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pimpl;
    };


    // Built-in shader supports two layer multitexturing (eg. for lightmaps or detail textures).
    class DualTextureEffect : public IEffect, public IEffectMatrices, public IEffectFog
    {
    public:
        DualTextureEffect(_In_ ID3D12Device* device, uint32_t effectFlags, const EffectPipelineStateDescription& pipelineDescription);
        DualTextureEffect(DualTextureEffect&& moveFrom) noexcept;
        DualTextureEffect& operator= (DualTextureEffect&& moveFrom) noexcept;

        DualTextureEffect(DualTextureEffect const&) = delete;
        DualTextureEffect& operator= (DualTextureEffect const&) = delete;

        ~DualTextureEffect() override;

        // IEffect methods.
        void __cdecl Apply(_In_ ID3D12GraphicsCommandList* commandList) override;

        // Camera settings.
        void XM_CALLCONV SetWorld(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetView(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetProjection(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection) override;

        // Material settings.
        void XM_CALLCONV SetDiffuseColor(DirectX::FXMVECTOR value);
        void __cdecl SetAlpha(float value);
        void XM_CALLCONV SetColorAndAlpha(DirectX::FXMVECTOR value);
        
        // Fog settings.
        void __cdecl SetFogStart(float value) override;
        void __cdecl SetFogEnd(float value) override;
        void XM_CALLCONV SetFogColor(DirectX::FXMVECTOR value) override;

        // Texture settings.
        void __cdecl SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor);
        void __cdecl SetTexture2(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor);
        
    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pimpl;
    };


    // Built-in shader supports cubic environment mapping.
    class EnvironmentMapEffect : public IEffect, public IEffectMatrices, public IEffectLights, public IEffectFog
    {
    public:
        enum Mapping
        {
            Mapping_Cube = 0,       // Cubic environment map
            Mapping_Sphere,         // Spherical environment map
            Mapping_DualParabola,   // Dual-parabola environment map (requires Feature Level 10.0)
        };

        EnvironmentMapEffect(_In_ ID3D12Device* device, uint32_t effectFlags, const EffectPipelineStateDescription& pipelineDescription,
            Mapping mapping = Mapping_Cube);
        EnvironmentMapEffect(EnvironmentMapEffect&& moveFrom) noexcept;
        EnvironmentMapEffect& operator= (EnvironmentMapEffect&& moveFrom) noexcept;

        EnvironmentMapEffect(EnvironmentMapEffect const&) = delete;
        EnvironmentMapEffect& operator= (EnvironmentMapEffect const&) = delete;

        ~EnvironmentMapEffect() override;

        // IEffect methods.
        void __cdecl Apply(_In_ ID3D12GraphicsCommandList* commandList) override;

        // Camera settings.
        void XM_CALLCONV SetWorld(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetView(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetProjection(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection) override;

        // Material settings.
        void XM_CALLCONV SetDiffuseColor(DirectX::FXMVECTOR value);
        void XM_CALLCONV SetEmissiveColor(DirectX::FXMVECTOR value);
        void __cdecl SetAlpha(float value);
        void XM_CALLCONV SetColorAndAlpha(DirectX::FXMVECTOR value);
        
        // Light settings.
        void XM_CALLCONV SetAmbientLightColor(DirectX::FXMVECTOR value) override;

        void __cdecl SetLightEnabled(int32_t whichLight, bool value) override;
        void XM_CALLCONV SetLightDirection(int32_t whichLight, DirectX::FXMVECTOR value) override;
        void XM_CALLCONV SetLightDiffuseColor(int32_t whichLight, DirectX::FXMVECTOR value) override;

        void __cdecl EnableDefaultLighting() override;

        // Fog settings.
        void __cdecl SetFogStart(float value) override;
        void __cdecl SetFogEnd(float value) override;
        void XM_CALLCONV SetFogColor(DirectX::FXMVECTOR value) override;

        // Texture setting.
        void __cdecl SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE texture, D3D12_GPU_DESCRIPTOR_HANDLE sampler);

        // Environment map settings.
        void __cdecl SetEnvironmentMap(D3D12_GPU_DESCRIPTOR_HANDLE texture, D3D12_GPU_DESCRIPTOR_HANDLE sampler);
        void __cdecl SetEnvironmentMapAmount(float value);
        void XM_CALLCONV SetEnvironmentMapSpecular(DirectX::FXMVECTOR value);
        void __cdecl SetFresnelFactor(float value);
        
    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pimpl;

        // Unsupported interface methods.
        void XM_CALLCONV SetLightSpecularColor(int32_t whichLight, DirectX::FXMVECTOR value) override;
    };


    // Built-in shader supports skinned animation.
    class SkinnedEffect : public IEffect, public IEffectMatrices, public IEffectLights, public IEffectFog, public IEffectSkinning
    {
    public:
        SkinnedEffect(_In_ ID3D12Device* device, uint32_t effectFlags, const EffectPipelineStateDescription& pipelineDescription,
            int32_t weightsPerVertex = 4);
        SkinnedEffect(SkinnedEffect&& moveFrom) noexcept;
        SkinnedEffect& operator= (SkinnedEffect&& moveFrom) noexcept;

        SkinnedEffect(SkinnedEffect const&) = delete;
        SkinnedEffect& operator= (SkinnedEffect const&) = delete;

        ~SkinnedEffect() override;

        // IEffect methods.
        void __cdecl Apply(_In_ ID3D12GraphicsCommandList* commandList) override;

        // Camera settings.
        void XM_CALLCONV SetWorld(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetView(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetProjection(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection) override;

        // Material settings.
        void XM_CALLCONV SetDiffuseColor(DirectX::FXMVECTOR value);
        void XM_CALLCONV SetEmissiveColor(DirectX::FXMVECTOR value);
        void XM_CALLCONV SetSpecularColor(DirectX::FXMVECTOR value);
        void __cdecl SetSpecularPower(float value);
        void __cdecl DisableSpecular();
        void __cdecl SetAlpha(float value);
        void XM_CALLCONV SetColorAndAlpha(DirectX::FXMVECTOR value);
        
        // Light settings.
        void XM_CALLCONV SetAmbientLightColor(DirectX::FXMVECTOR value) override;

        void __cdecl SetLightEnabled(int32_t whichLight, bool value) override;
        void XM_CALLCONV SetLightDirection(int32_t whichLight, DirectX::FXMVECTOR value) override;
        void XM_CALLCONV SetLightDiffuseColor(int32_t whichLight, DirectX::FXMVECTOR value) override;
        void XM_CALLCONV SetLightSpecularColor(int32_t whichLight, DirectX::FXMVECTOR value) override;

        void __cdecl EnableDefaultLighting() override;

        // Fog settings.
        void __cdecl SetFogStart(float value) override;
        void __cdecl SetFogEnd(float value) override;
        void XM_CALLCONV SetFogColor(DirectX::FXMVECTOR value) override;

        // Texture setting.
        void __cdecl SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor);
        
        // Animation settings.
        void __cdecl SetBoneTransforms(_In_reads_(count) DirectX::XMMATRIX const* value, size_t count) override;
        void __cdecl ResetBoneTransforms() override;

    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pimpl;
    };


    //----------------------------------------------------------------------------------
    // Built-in shader extends BasicEffect with normal map and optional specular map
    class NormalMapEffect : public IEffect, public IEffectMatrices, public IEffectLights, public IEffectFog
    {
    public:
        NormalMapEffect(_In_ ID3D12Device* device, uint32_t effectFlags, const EffectPipelineStateDescription& pipelineDescription);
        NormalMapEffect(NormalMapEffect&& moveFrom) noexcept;
        NormalMapEffect& operator= (NormalMapEffect&& moveFrom) noexcept;

        NormalMapEffect(NormalMapEffect const&) = delete;
        NormalMapEffect& operator= (NormalMapEffect const&) = delete;

        ~NormalMapEffect() override;

        // IEffect methods.
        void __cdecl Apply(_In_ ID3D12GraphicsCommandList* commandList) override;

        // Camera settings.
        void XM_CALLCONV SetWorld(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetView(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetProjection(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection) override;

        // Material settings.
        void XM_CALLCONV SetDiffuseColor(DirectX::FXMVECTOR value);
        void XM_CALLCONV SetEmissiveColor(DirectX::FXMVECTOR value);
        void XM_CALLCONV SetSpecularColor(DirectX::FXMVECTOR value);
        void __cdecl SetSpecularPower(float value);
        void __cdecl DisableSpecular();
        void __cdecl SetAlpha(float value);
        void XM_CALLCONV SetColorAndAlpha(DirectX::FXMVECTOR value);

        // Light settings.
        void XM_CALLCONV SetAmbientLightColor(DirectX::FXMVECTOR value) override;

        void __cdecl SetLightEnabled(int32_t whichLight, bool value) override;
        void XM_CALLCONV SetLightDirection(int32_t whichLight, DirectX::FXMVECTOR value) override;
        void XM_CALLCONV SetLightDiffuseColor(int32_t whichLight, DirectX::FXMVECTOR value) override;
        void XM_CALLCONV SetLightSpecularColor(int32_t whichLight, DirectX::FXMVECTOR value) override;

        void __cdecl EnableDefaultLighting() override;

        // Fog settings.
        void __cdecl SetFogStart(float value) override;
        void __cdecl SetFogEnd(float value) override;
        void XM_CALLCONV SetFogColor(DirectX::FXMVECTOR value) override;

        // Texture setting - albedo, normal and specular intensity
        void __cdecl SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor);
        void __cdecl SetNormalTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor);
        void __cdecl SetSpecularTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor);

    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pimpl;
    };


    //----------------------------------------------------------------------------------
    // Built-in shader for Physically-Based Rendering (Roughness/Metalness) with Image-based lighting
    class PBREffect : public IEffect, public IEffectMatrices, public IEffectLights
    {
    public:
        PBREffect(_In_ ID3D12Device* device, uint32_t effectFlags, const EffectPipelineStateDescription& pipelineDescription);
        PBREffect(PBREffect&& moveFrom) noexcept;
        PBREffect& operator= (PBREffect&& moveFrom) noexcept;

        PBREffect(PBREffect const&) = delete;
        PBREffect& operator= (PBREffect const&) = delete;

        ~PBREffect() override;

        // IEffect methods.
        void __cdecl Apply(_In_ ID3D12GraphicsCommandList* commandList) override;

        // Camera settings.
        void XM_CALLCONV SetWorld(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetView(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetProjection(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection) override;

        // Light settings.
        void __cdecl SetLightEnabled(int32_t whichLight, bool value) override;
        void XM_CALLCONV SetLightDirection(int32_t whichLight, DirectX::FXMVECTOR value) override;
        void XM_CALLCONV SetLightDiffuseColor(int32_t whichLight, DirectX::FXMVECTOR value) override;

        void __cdecl EnableDefaultLighting() override;

        // PBR Settings.
        void __cdecl SetAlpha(float value);
        void XM_CALLCONV SetConstantAlbedo(DirectX::FXMVECTOR value);
        void __cdecl SetConstantMetallic(float value);
        void __cdecl SetConstantRoughness(float value);

        // Texture settings.
        void __cdecl SetAlbedoTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor);
        void __cdecl SetNormalTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor);
        void __cdecl SetRMATexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor);

        void __cdecl SetEmissiveTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor);

        void __cdecl SetSurfaceTextures(
            D3D12_GPU_DESCRIPTOR_HANDLE albedo,
            D3D12_GPU_DESCRIPTOR_HANDLE normal,
            D3D12_GPU_DESCRIPTOR_HANDLE roughnessMetallicAmbientOcclusion,
            D3D12_GPU_DESCRIPTOR_HANDLE sampler);

        void __cdecl SetIBLTextures(
            D3D12_GPU_DESCRIPTOR_HANDLE radiance,
            int32_t numRadianceMips,
            D3D12_GPU_DESCRIPTOR_HANDLE irradiance,
            D3D12_GPU_DESCRIPTOR_HANDLE sampler);

        // Render target size, required for velocity buffer output.
        void __cdecl SetRenderTargetSizeInPixels(int32_t width, int32_t height);

    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pimpl;

        // Unsupported interface methods.
        void XM_CALLCONV SetAmbientLightColor(DirectX::FXMVECTOR value) override;
        void XM_CALLCONV SetLightSpecularColor(int32_t whichLight, DirectX::FXMVECTOR value) override;
    };


    //----------------------------------------------------------------------------------
    // Built-in shader for debug visualization of normals, tangents, etc.
    class DebugEffect : public IEffect, public IEffectMatrices
    {
    public:
        enum Mode
        {
            Mode_Default = 0,   // Hemispherical ambient lighting
            Mode_Normals,       // RGB normals
            Mode_Tangents,      // RGB tangents
            Mode_BiTangents,    // RGB bi-tangents
        };

        DebugEffect(_In_ ID3D12Device* device, uint32_t effectFlags, const EffectPipelineStateDescription& pipelineDescription,
            Mode debugMode = Mode_Default);
        DebugEffect(DebugEffect&& moveFrom) noexcept;
        DebugEffect& operator= (DebugEffect&& moveFrom) noexcept;

        DebugEffect(DebugEffect const&) = delete;
        DebugEffect& operator= (DebugEffect const&) = delete;

        ~DebugEffect() override;

        // IEffect methods.
        void __cdecl Apply(_In_ ID3D12GraphicsCommandList* commandList) override;

        // Camera settings.
        void XM_CALLCONV SetWorld(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetView(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetProjection(DirectX::FXMMATRIX value) override;
        void XM_CALLCONV SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection) override;

        // Debug Settings.
        void XM_CALLCONV SetHemisphericalAmbientColor(DirectX::FXMVECTOR upper, DirectX::FXMVECTOR lower);
        void __cdecl SetAlpha(float value);

    private:
        // Private implementation.
        class Impl;

        std::unique_ptr<Impl> pimpl;
    };


    //----------------------------------------------------------------------------------
    // Abstract interface to factory texture resources
    class IEffectTextureFactory
    {
    public:
        virtual ~IEffectTextureFactory() = default;

        IEffectTextureFactory(const IEffectTextureFactory&) = delete;
        IEffectTextureFactory& operator=(const IEffectTextureFactory&) = delete;

        IEffectTextureFactory(IEffectTextureFactory&&) = delete;
        IEffectTextureFactory& operator=(IEffectTextureFactory&&) = delete;

        virtual size_t __cdecl CreateTexture(_In_ const std::wstring_view& texturename, int32_t descriptorindex) = 0;

    protected:
        IEffectTextureFactory() = default;
    };


    // Factory for sharing texture resources
    class EffectTextureFactory : public IEffectTextureFactory
    {
    public:
        EffectTextureFactory(
            _In_ ID3D12Device* device,
            ResourceUploadBatch& resourceuploadbatch,
            _In_ ID3D12DescriptorHeap* descriptorheap) noexcept(false);

        EffectTextureFactory(
            _In_ ID3D12Device* device,
            ResourceUploadBatch& resourceuploadbatch,
            _In_ size_t numdescriptors,
            _In_ D3D12_DESCRIPTOR_HEAP_FLAGS descriptorheapflags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) noexcept(false);

        EffectTextureFactory(EffectTextureFactory&& moveFrom) noexcept;
        EffectTextureFactory& operator= (EffectTextureFactory&& moveFrom) noexcept;

        EffectTextureFactory(EffectTextureFactory const&) = delete;
        EffectTextureFactory& operator= (EffectTextureFactory const&) = delete;

        ~EffectTextureFactory() override;

        size_t __cdecl CreateTexture(_In_ const std::wstring_view& texturename, int32_t descriptorindex) override;

        ID3D12DescriptorHeap* __cdecl Heap(void) const noexcept;

        // Shorthand accessors for the descriptor heap
        D3D12_CPU_DESCRIPTOR_HANDLE __cdecl GetCpuDescriptorHandle(size_t index) const;
        D3D12_GPU_DESCRIPTOR_HANDLE __cdecl GetGpuDescriptorHandle(size_t index) const;

        // How many textures are there in this factory?
        size_t __cdecl ResourceCount(void) const noexcept;

        // Get a resource in a specific slot (note: increases reference count on resource)
        void __cdecl GetResource(size_t slotindex, _Out_ ID3D12Resource** resource, _Out_opt_ bool* iscubemap = nullptr);

        // Settings.
        void __cdecl ReleaseCache(void);

        void __cdecl SetSharing(bool enableshared) noexcept;

        void __cdecl EnableForceSRGB(bool forcesrgb) noexcept;
        void __cdecl EnableAutoGenMips(bool generatemips) noexcept;

        void __cdecl SetDirectory(_In_opt_ const std::wstring_view& pathname) noexcept;

    private:
        // Private implementation
        class Impl;

        std::unique_ptr<Impl> pimpl;
    };


    //----------------------------------------------------------------------------------
    // Abstract interface to factory for sharing effects
    class IEffectFactory
    {
    public:
        virtual ~IEffectFactory() = default;

        IEffectFactory(const IEffectFactory&) = delete;
        IEffectFactory& operator=(const IEffectFactory&) = delete;

        IEffectFactory(IEffectFactory&&) = delete;
        IEffectFactory& operator=(IEffectFactory&&) = delete;

        struct EffectInfo
        {
            std::wstring        name;
            bool                perVertexColor = false;
            bool                enableSkinning = false;
            bool                enableDualTexture = false;
            bool                enableNormalMaps = false;
            bool                biasedVertexNormals = false;
			float               specularPower{};
            float               alphaValue{};
            DirectX::XMFLOAT3   ambientColor{};
            DirectX::XMFLOAT3   diffuseColor{};
            DirectX::XMFLOAT3   specularColor{};
            DirectX::XMFLOAT3   emissiveColor{};
            int32_t                 diffuseTextureIndex = -1;
            int32_t                 specularTextureIndex = -1;
            int32_t                 normalTextureIndex = -1;
            int32_t                 emissiveTextureIndex = -1;
            int32_t                 samplerIndex = -1;
            int32_t                 samplerIndex2 = -1;

            EffectInfo() noexcept  = default;
        };

        virtual std::shared_ptr<IEffect> __cdecl CreateEffect(
            const EffectInfo& info, 
            const EffectPipelineStateDescription& opaquePipelineState,
            const EffectPipelineStateDescription& alphaPipelineState,
            const D3D12_INPUT_LAYOUT_DESC& inputLayout, 
            int32_t textureDescriptorOffset = 0,
            int32_t samplerDescriptorOffset = 0) = 0;

    protected:
        IEffectFactory() = default;
    };


    // Factory for sharing effects
    class EffectFactory : public IEffectFactory
    {
    public:
        EffectFactory(_In_ ID3D12Device* device);
        EffectFactory(
            _In_ ID3D12DescriptorHeap* textureDescriptors,
            _In_ ID3D12DescriptorHeap* samplerDescriptors);

        EffectFactory(EffectFactory&& moveFrom) noexcept;
        EffectFactory& operator= (EffectFactory&& moveFrom) noexcept;

        EffectFactory(EffectFactory const&) = delete;
        EffectFactory& operator= (EffectFactory const&) = delete;

        ~EffectFactory() override;

        // IEffectFactory methods.
        virtual std::shared_ptr<IEffect> __cdecl CreateEffect(
            const EffectInfo& info,
            const EffectPipelineStateDescription& opaquePipelineState,
            const EffectPipelineStateDescription& alphaPipelineState,
            const D3D12_INPUT_LAYOUT_DESC& inputLayout,
            int32_t textureDescriptorOffset = 0,
            int32_t samplerDescriptorOffset = 0) override;

        // Settings.
        void __cdecl ReleaseCache();

        void __cdecl SetSharing(bool enabled) noexcept;

        void __cdecl EnablePerPixelLighting(bool enabled) noexcept;

        void __cdecl EnableNormalMapEffect(bool enabled) noexcept;

        void __cdecl EnableFogging(bool enabled) noexcept;

    private:
        // Private implementation.
        class Impl;

        std::shared_ptr<Impl> pimpl;
    };


    // Factory for Physically Based Rendering (PBR)
    class PBREffectFactory : public IEffectFactory
    {
    public:
        PBREffectFactory(_In_ ID3D12Device* device) noexcept(false);
        PBREffectFactory(
            _In_ ID3D12DescriptorHeap* textureDescriptors,
            _In_ ID3D12DescriptorHeap* samplerDescriptors) noexcept(false);

        PBREffectFactory(PBREffectFactory&& moveFrom) noexcept;
        PBREffectFactory& operator= (PBREffectFactory&& moveFrom) noexcept;

        PBREffectFactory(PBREffectFactory const&) = delete;
        PBREffectFactory& operator= (PBREffectFactory const&) = delete;

        ~PBREffectFactory() override;

        // IEffectFactory methods.
        virtual std::shared_ptr<IEffect> __cdecl CreateEffect(
            const EffectInfo& info,
            const EffectPipelineStateDescription& opaquePipelineState,
            const EffectPipelineStateDescription& alphaPipelineState,
            const D3D12_INPUT_LAYOUT_DESC& inputLayout,
            int32_t textureDescriptorOffset = 0,
            int32_t samplerDescriptorOffset = 0) override;

        // Settings.
        void __cdecl ReleaseCache();

        void __cdecl SetSharing(bool enabled) noexcept;

    private:
        // Private implementation.
        class Impl;

        std::shared_ptr<Impl> pimpl;
    };
}
