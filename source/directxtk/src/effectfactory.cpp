//--------------------------------------------------------------------------------------
// File: EffectFactory.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"

#include <mutex>

#include "effects.h"
#include "commonstates.h"
#include "directxhelpers.h"
#include "platformhelpers.h"
#include "descriptorheap.h"



using namespace directxtk;
// using Microsoft::WRL::ComPtr;

// Internal EffectFactory implementation class. Only one of these helpers is allocated
// per D3D device, even if there are multiple public facing EffectFactory instances.
class EffectFactory::Impl
{
public:
    Impl(_In_ ID3D12Device* device, _In_ ID3D12DescriptorHeap* textureDescriptors, _In_ ID3D12DescriptorHeap* samplerDescriptors) noexcept(false)
        : mTextureDescriptors(nullptr)
        , mSamplerDescriptors(nullptr)
        , mUseNormalMapEffect(true)
        , mEnablePerPixelLighting(true)
        , mEnableFog(false)
        , mDevice(device)
        , mSharing(true)
    { 
        if (textureDescriptors)
            mTextureDescriptors = std::make_unique<DescriptorHeap>(textureDescriptors);
        if (samplerDescriptors)
            mSamplerDescriptors = std::make_unique<DescriptorHeap>(samplerDescriptors);
    }

    std::shared_ptr<IEffect> CreateEffect(
        const EffectInfo& info,
        const EffectPipelineStateDescription& opaquePipelineState,
        const EffectPipelineStateDescription& alphaPipelineState,
        const D3D12_INPUT_LAYOUT_DESC& inputLayout,
        int32_t textureDescriptorOffset,
        int32_t samplerDescriptorOffset);

    void ReleaseCache();
    void SetSharing(bool enabled) noexcept { mSharing = enabled; }

    std::unique_ptr<DescriptorHeap> mTextureDescriptors;
    std::unique_ptr<DescriptorHeap> mSamplerDescriptors;

    bool mUseNormalMapEffect;
    bool mEnablePerPixelLighting;
    bool mEnableFog;

private:
    wil::com_ptr<ID3D12Device> mDevice;

    using EffectCache = std::map< std::wstring, std::shared_ptr<IEffect> >;

    EffectCache  mEffectCache;
    EffectCache  mEffectCacheSkinning;
    EffectCache  mEffectCacheDualTexture;
    EffectCache  mEffectCacheNormalMap;

    bool mSharing;

    std::mutex mutex;
};


std::shared_ptr<IEffect> EffectFactory::Impl::CreateEffect(
    const EffectInfo& info,
    const EffectPipelineStateDescription& opaquePipelineState,
    const EffectPipelineStateDescription& alphaPipelineState,
    const D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc,
    int32_t textureDescriptorOffset,
    int32_t samplerDescriptorOffset)
{
    // If textures are required, make sure we have a descriptor heap
    if (!mTextureDescriptors && (info.diffuseTextureIndex != -1 || info.specularTextureIndex != -1 || info.normalTextureIndex != -1 || info.emissiveTextureIndex != -1))
    {
        DebugTrace("ERROR: EffectFactory created without texture descriptor heap with texture index set (diffuse %d, specular %d, normal %d, emissive %d)!\n",
            info.diffuseTextureIndex, info.specularTextureIndex, info.normalTextureIndex, info.emissiveTextureIndex);
        throw std::exception("EffectFactory");
    }
    if (!mSamplerDescriptors && (info.samplerIndex != -1 || info.samplerIndex2 != -1))
    {
        DebugTrace("ERROR: EffectFactory created without sampler descriptor heap with sampler index set (samplerIndex %d, samplerIndex2 %d)!\n",
            info.samplerIndex, info.samplerIndex2);
        throw std::exception("EffectFactory");
    }

    // If we have descriptors, make sure we have both texture and sampler descriptors
    if ((mTextureDescriptors == nullptr) != (mSamplerDescriptors == nullptr))
    {
        DebugTrace("ERROR: A texture or sampler descriptor heap was provided, but both are required.\n");
        throw std::exception("EffectFactory");
    }

    // Validate the we have either both texture and sampler descriptors, or neither
    if ((info.diffuseTextureIndex == -1) != (info.samplerIndex == -1))
    {
        DebugTrace("ERROR: Material provides either a texture or sampler, but both are required.\n");
        throw std::exception("EffectFactory");
    }

    int32_t diffuseTextureIndex = (info.diffuseTextureIndex != -1 && mTextureDescriptors != nullptr) ? info.diffuseTextureIndex + textureDescriptorOffset : -1;
    int32_t specularTextureIndex = (info.specularTextureIndex != -1 && mTextureDescriptors != nullptr) ? info.specularTextureIndex + textureDescriptorOffset : -1;
    int32_t emissiveTextureIndex = (info.emissiveTextureIndex != -1 && mTextureDescriptors != nullptr) ? info.emissiveTextureIndex + textureDescriptorOffset : -1;
    int32_t normalTextureIndex = (info.normalTextureIndex != -1 && mTextureDescriptors != nullptr) ? info.normalTextureIndex + textureDescriptorOffset : -1;
    int32_t samplerIndex = (info.samplerIndex != -1 && mSamplerDescriptors != nullptr) ? info.samplerIndex + samplerDescriptorOffset : -1;
    int32_t samplerIndex2 = (info.samplerIndex2 != -1 && mSamplerDescriptors != nullptr) ? info.samplerIndex2 + samplerDescriptorOffset : -1;

    // Modify base pipeline state
    EffectPipelineStateDescription derivedPSD = (info.alphaValue < 1.0f) ? alphaPipelineState : opaquePipelineState;
    derivedPSD.inputLayout = inputLayoutDesc;

    std::wstring cachename;
    if (info.enableSkinning)
    {
        // SkinnedEffect
        uint32_t effectflags = (mEnablePerPixelLighting) ? EffectFlags::PerPixelLighting : EffectFlags::Lighting;

        if (mEnableFog)
        {
            effectflags |= EffectFlags::Fog;
        }

        if (info.biasedVertexNormals)
        {
            effectflags |= EffectFlags::BiasedVertexNormals;
        }

        if (mSharing && !info.name.empty())
        {
            uint32_t hash = derivedPSD.ComputeHash();
            cachename = std::to_wstring(effectflags) + info.name + std::to_wstring(hash);

            auto it = mEffectCacheSkinning.find(cachename);
            if (mSharing && it != mEffectCacheSkinning.end())
            {
                return it->second;
            }
        }

        auto effect = std::make_shared<SkinnedEffect>(mDevice.get(), effectflags, derivedPSD);

        effect->EnableDefaultLighting();

        effect->SetAlpha(info.alphaValue);

        // Skinned Effect does not have an ambient material color, or per-vertex color support

        DirectX::XMVECTOR color = XMLoadFloat3(&info.diffuseColor);
        effect->SetDiffuseColor(color);

        if (info.specularColor.x != 0 || info.specularColor.y != 0 || info.specularColor.z != 0)
        {
            color = XMLoadFloat3(&info.specularColor);
            effect->SetSpecularColor(color);
            effect->SetSpecularPower(info.specularPower);
        }
        else
        {
            effect->DisableSpecular();
        }

        if (info.emissiveColor.x != 0 || info.emissiveColor.y != 0 || info.emissiveColor.z != 0)
        {
            color = XMLoadFloat3(&info.emissiveColor);
            effect->SetEmissiveColor(color);
        }

        if (diffuseTextureIndex != -1)
        {
            effect->SetTexture(
                mTextureDescriptors->GetGpuHandle(static_cast<size_t>(diffuseTextureIndex)),
                mSamplerDescriptors->GetGpuHandle(static_cast<size_t>(samplerIndex)));
        }

        if (mSharing && !info.name.empty())
        {
            std::lock_guard<std::mutex> lock(mutex);
            EffectCache::value_type v(cachename, effect);
            mEffectCacheSkinning.insert(v);
        }

        return std::move(effect);
    }
    else if (info.enableDualTexture)
    {
        // DualTextureEffect
        int32_t effectflags = EffectFlags::None;

        if (mEnableFog)
        {
            effectflags |= EffectFlags::Fog;
        }

        if (mSharing && !info.name.empty())
        {
            uint32_t hash = derivedPSD.ComputeHash();
            cachename = std::to_wstring(effectflags) + info.name + std::to_wstring(hash);

            auto it = mEffectCacheDualTexture.find(cachename);
            if (mSharing && it != mEffectCacheDualTexture.end())
            {
                return it->second;
            }
        }

        if (info.perVertexColor)
        {
            effectflags |= EffectFlags::VertexColor;
        }

        auto effect = std::make_shared<DualTextureEffect>(mDevice.get(), effectflags, derivedPSD);

        // Dual texture effect doesn't support lighting (usually it's lightmaps)
        effect->SetAlpha(info.alphaValue);

        DirectX::XMVECTOR color = XMLoadFloat3(&info.diffuseColor);
        effect->SetDiffuseColor(color);

        if (diffuseTextureIndex != -1)
        {
            effect->SetTexture(
                mTextureDescriptors->GetGpuHandle(static_cast<size_t>(diffuseTextureIndex)),
                mSamplerDescriptors->GetGpuHandle(static_cast<size_t>(samplerIndex)));
        }

        if (emissiveTextureIndex != -1)
        {
            if (samplerIndex2 == -1)
            {
                DebugTrace("ERROR: Dual-texture requires a second sampler (emissive %d)\n", emissiveTextureIndex);
                throw std::exception("EffectFactory");
            }

            effect->SetTexture2(
                mTextureDescriptors->GetGpuHandle(static_cast<size_t>(emissiveTextureIndex)),
                mSamplerDescriptors->GetGpuHandle(static_cast<size_t>(samplerIndex2)));
        }
        else if (specularTextureIndex != -1)
        {
            // If there's no emissive texture specified, use the specular texture as the second texture
            if (samplerIndex2 == -1)
            {
                DebugTrace("ERROR: Dual-texture requires a second sampler (specular %d)\n", specularTextureIndex);
                throw std::exception("EffectFactory");
            }

            effect->SetTexture2(
                mTextureDescriptors->GetGpuHandle(static_cast<size_t>(specularTextureIndex)),
                mSamplerDescriptors->GetGpuHandle(static_cast<size_t>(samplerIndex2)));
        }

        if (mSharing && !info.name.empty())
        {
            std::lock_guard<std::mutex> lock(mutex);
            EffectCache::value_type v(cachename, effect);
            mEffectCacheDualTexture.insert(v);
        }

        return std::move(effect);
    }
    else if (info.enableNormalMaps && mUseNormalMapEffect)
    {
        // NormalMapEffect
        int32_t effectflags = EffectFlags::None;

        if (mEnableFog)
        {
            effectflags |= EffectFlags::Fog;
        }

        if (info.perVertexColor)
        {
            effectflags |= EffectFlags::VertexColor;
        }

        if (info.biasedVertexNormals)
        {
            effectflags |= EffectFlags::BiasedVertexNormals;
        }

        if (specularTextureIndex != -1)
        {
            effectflags |= EffectFlags::Specular;
        }

        if (mSharing && !info.name.empty())
        {
            uint32_t hash = derivedPSD.ComputeHash();
            cachename = std::to_wstring(effectflags) + info.name + std::to_wstring(hash);

            auto it = mEffectCacheNormalMap.find(cachename);
            if (mSharing && it != mEffectCacheNormalMap.end())
            {
                return it->second;
            }
        }

        auto effect = std::make_shared<NormalMapEffect>(mDevice.get(), effectflags, derivedPSD);

        effect->EnableDefaultLighting();

        effect->SetAlpha(info.alphaValue);

        // NormalMap Effect does not have an ambient material color

        DirectX::XMVECTOR color = XMLoadFloat3(&info.diffuseColor);
        effect->SetDiffuseColor(color);

        if (info.specularColor.x != 0 || info.specularColor.y != 0 || info.specularColor.z != 0)
        {
            color = XMLoadFloat3(&info.specularColor);
            effect->SetSpecularColor(color);
            effect->SetSpecularPower(info.specularPower);
        }
        else
        {
            effect->DisableSpecular();
        }

        if (info.emissiveColor.x != 0 || info.emissiveColor.y != 0 || info.emissiveColor.z != 0)
        {
            color = XMLoadFloat3(&info.emissiveColor);
            effect->SetEmissiveColor(color);
        }

        if (diffuseTextureIndex != -1)
        {
            effect->SetTexture(
                mTextureDescriptors->GetGpuHandle(static_cast<size_t>(diffuseTextureIndex)),
                mSamplerDescriptors->GetGpuHandle(static_cast<size_t>(samplerIndex)));
        }

        if (specularTextureIndex != -1)
        {
            effect->SetSpecularTexture(mTextureDescriptors->GetGpuHandle(static_cast<size_t>(specularTextureIndex)));
        }

        if (normalTextureIndex != -1)
        {
            effect->SetNormalTexture(mTextureDescriptors->GetGpuHandle(static_cast<size_t>(normalTextureIndex)));
        }

        if (mSharing && !info.name.empty())
        {
            std::lock_guard<std::mutex> lock(mutex);
            EffectCache::value_type v(cachename, effect);
            mEffectCacheNormalMap.insert(v);
        }

        return std::move(effect);
    }
    else
    {
        // set effect flags for creation
        uint32_t effectflags = (mEnablePerPixelLighting) ? EffectFlags::PerPixelLighting : EffectFlags::Lighting;

        if (mEnableFog)
        {
            effectflags |= EffectFlags::Fog;
        }

        if (info.perVertexColor)
        {
            effectflags |= EffectFlags::VertexColor;
        }

        if (diffuseTextureIndex != -1)
        {
            effectflags |= EffectFlags::Texture;
        }

        if (info.biasedVertexNormals)
        {
            effectflags |= EffectFlags::BiasedVertexNormals;
        }

        // BasicEffect
        if (mSharing && !info.name.empty())
        {
            uint32_t hash = derivedPSD.ComputeHash();
            cachename = std::to_wstring(effectflags) + info.name + std::to_wstring(hash);

            auto it = mEffectCache.find(cachename);
            if (mSharing && it != mEffectCache.end())
            {
                return it->second;
            }
        }

        auto effect = std::make_shared<BasicEffect>(mDevice.get(), effectflags, derivedPSD);

        effect->EnableDefaultLighting();

        effect->SetAlpha(info.alphaValue);

        // Basic Effect does not have an ambient material color
        DirectX::XMVECTOR color = XMLoadFloat3(&info.diffuseColor);
        effect->SetDiffuseColor(color);

        if (info.specularColor.x != 0 || info.specularColor.y != 0 || info.specularColor.z != 0)
        {
            color = XMLoadFloat3(&info.specularColor);
            effect->SetSpecularColor(color);
            effect->SetSpecularPower(info.specularPower);
        }
        else
        {
            effect->DisableSpecular();
        }

        if (info.emissiveColor.x != 0 || info.emissiveColor.y != 0 || info.emissiveColor.z != 0)
        {
            color = XMLoadFloat3(&info.emissiveColor);
            effect->SetEmissiveColor(color);
        }

        if (diffuseTextureIndex != -1)
        {
            effect->SetTexture(
                mTextureDescriptors->GetGpuHandle(static_cast<size_t>(diffuseTextureIndex)),
                mSamplerDescriptors->GetGpuHandle(static_cast<size_t>(samplerIndex)));
        }

        if (mSharing && !info.name.empty())
        {
            std::lock_guard<std::mutex> lock(mutex);
            EffectCache::value_type v(cachename, effect);
            mEffectCache.insert(v);
        }

        return std::move(effect);
    }
}

void EffectFactory::Impl::ReleaseCache()
{
    std::lock_guard<std::mutex> lock(mutex);
    mEffectCache.clear();
    mEffectCacheSkinning.clear();
    mEffectCacheDualTexture.clear();
    mEffectCacheNormalMap.clear();
}



//--------------------------------------------------------------------------------------
// EffectFactory
//--------------------------------------------------------------------------------------

EffectFactory::EffectFactory(_In_ ID3D12Device* device)
{
    pimpl = std::make_shared<Impl>(device, nullptr, nullptr);
}

EffectFactory::EffectFactory(_In_ ID3D12DescriptorHeap* textureDescriptors, _In_ ID3D12DescriptorHeap* samplerDescriptors)
{
    if (!textureDescriptors)
    {
        throw std::exception("Texture descriptor heap cannot be null if no device is provided. Use the alternative EffectFactory constructor instead.");
    }
    if (!samplerDescriptors)
    {
        throw std::exception("Descriptor heap cannot be null if no device is provided. Use the alternative EffectFactory constructor instead.");
    }

    if (textureDescriptors->GetDesc().Type != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
    {
        throw std::exception("EffectFactory::CreateEffect requires a CBV_SRV_UAV descriptor heap for textureDescriptors.");
    }
    if (samplerDescriptors->GetDesc().Type != D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
    {
        throw std::exception("EffectFactory::CreateEffect requires a SAMPLER descriptor heap for samplerDescriptors.");
    }

    wil::com_ptr<ID3D12Device> device;
#if defined(_XBOX_ONE) && defined(_TITLE)
    textureDescriptors->GetDevice(IID_GRAPHICS_PPV_ARGS(device.addressof()));
#else
    HRESULT hresult = textureDescriptors->GetDevice(IID_PPV_ARGS(device.addressof()));
    if (FAILED(hresult))
    {
        throw com_exception(hresult);
    }
#endif

    pimpl = std::make_shared<Impl>(device.get(), textureDescriptors, samplerDescriptors);
}

EffectFactory::~EffectFactory()
{
}


EffectFactory::EffectFactory(EffectFactory&& moveFrom) noexcept
    : pimpl(std::move(moveFrom.pimpl))
{
}

EffectFactory& EffectFactory::operator= (EffectFactory&& moveFrom) noexcept
{
    pimpl = std::move(moveFrom.pimpl);
    return *this;
}

std::shared_ptr<IEffect> EffectFactory::CreateEffect(
    const EffectInfo& info, 
    const EffectPipelineStateDescription& opaquePipelineState,
    const EffectPipelineStateDescription& alphaPipelineState,
    const D3D12_INPUT_LAYOUT_DESC& inputLayout, 
    int32_t textureDescriptorOffset,
    int32_t samplerDescriptorOffset)
{
    return pimpl->CreateEffect(info, opaquePipelineState, alphaPipelineState, inputLayout, textureDescriptorOffset, samplerDescriptorOffset);
}

void EffectFactory::ReleaseCache()
{
    pimpl->ReleaseCache();
}

void EffectFactory::SetSharing(bool enabled) noexcept
{
    pimpl->SetSharing(enabled);
}

void EffectFactory::EnablePerPixelLighting(bool enabled) noexcept
{
    pimpl->mEnablePerPixelLighting = enabled;
}

void EffectFactory::EnableFogging(bool enabled) noexcept
{
    pimpl->mEnableFog = enabled;
}

void EffectFactory::EnableNormalMapEffect(bool enabled) noexcept
{
    pimpl->mUseNormalMapEffect = enabled;
}
