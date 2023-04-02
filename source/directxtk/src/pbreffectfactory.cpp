//--------------------------------------------------------------------------------------
// File: PBREffectFactory.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"
#include "effects.h"
#include "commonstates.h"
#include "directxhelpers.h"
#include "platformhelpers.h"
#include "descriptorheap.h"

#include <mutex>


using namespace directxtk;
// using Microsoft::WRL::ComPtr;

// Internal PBREffectFactory implementation class. Only one of these helpers is allocated
// per D3D device, even if there are multiple public facing PBREffectFactory instances.
class PBREffectFactory::Impl
{
public:
    Impl(_In_ ID3D12Device* device, _In_ ID3D12DescriptorHeap* textureDescriptors, _In_ ID3D12DescriptorHeap* samplerDescriptors) noexcept(false)
        : m_texturedescriptors(nullptr)
        , m_samplerdescriptors(nullptr)
        , m_device(device)
        , m_sharing(true)
    { 
        if (textureDescriptors)
            m_texturedescriptors = std::make_unique<DescriptorHeap>(textureDescriptors);
        if (samplerDescriptors)
            m_samplerdescriptors = std::make_unique<DescriptorHeap>(samplerDescriptors);
    }

    std::shared_ptr<IEffect> CreateEffect(
        const EffectInfo& info,
        const EffectPipelineStateDescription& opaquePipelineState,
        const EffectPipelineStateDescription& alphaPipelineState,
        const D3D12_INPUT_LAYOUT_DESC& inputLayout,
        int32_t textureDescriptorOffset,
        int32_t samplerDescriptorOffset);

    void ReleaseCache();
    void SetSharing(bool enabled) noexcept { m_sharing = enabled; }

    std::unique_ptr<DescriptorHeap> m_texturedescriptors;
    std::unique_ptr<DescriptorHeap> m_samplerdescriptors;

private:
    wil::com_ptr<ID3D12Device> m_device;

    using EffectCache = std::map< std::wstring, std::shared_ptr<IEffect> >;

    EffectCache  mEffectCache;

    bool m_sharing;

    std::mutex mutex;
};


std::shared_ptr<IEffect> PBREffectFactory::Impl::CreateEffect(
    const EffectInfo& info,
    const EffectPipelineStateDescription& opaquePipelineState,
    const EffectPipelineStateDescription& alphaPipelineState,
    const D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc,
    int32_t textureDescriptorOffset,
    int32_t samplerDescriptorOffset)
{
    if (!m_texturedescriptors)
    {
        DebugTrace("ERROR: PBREffectFactory created without texture descriptor heap!\n");
        throw std::exception("PBREffectFactory");
    }
    if (!m_samplerdescriptors)
    {
        DebugTrace("ERROR: PBREffectFactory created without sampler descriptor heap!\n");
        throw std::exception("PBREffectFactory");
    }

    int32_t albetoTextureIndex = (info.diffuseTextureIndex != -1) ? info.diffuseTextureIndex + textureDescriptorOffset : -1;
    int32_t rmaTextureIndex = (info.specularTextureIndex != -1) ? info.specularTextureIndex + textureDescriptorOffset : -1;
    int32_t normalTextureIndex = (info.normalTextureIndex != -1) ? info.normalTextureIndex + textureDescriptorOffset : -1;
    int32_t emissiveTextureIndex = (info.emissiveTextureIndex != -1) ? info.emissiveTextureIndex + textureDescriptorOffset : -1;
    int32_t samplerIndex = (info.samplerIndex != -1) ? info.samplerIndex + samplerDescriptorOffset : -1;

    // Modify base pipeline state
    EffectPipelineStateDescription derivedPSD = (info.alphaValue < 1.0f) ? alphaPipelineState : opaquePipelineState;
    derivedPSD.inputLayout = inputLayoutDesc;

    // set effect flags for creation
    int32_t effectflags = EffectFlags::Texture;

    if (info.biasedVertexNormals)
    {
        effectflags |= EffectFlags::BiasedVertexNormals;
    }

    if (emissiveTextureIndex != -1)
    {
        effectflags |= EffectFlags::Emissive;
    }

    std::wstring cachename;
    if (m_sharing && !info.name.empty())
    {
        uint32_t hash = derivedPSD.ComputeHash();
        cachename = std::to_wstring(effectflags) + info.name + std::to_wstring(hash);

        auto it = mEffectCache.find(cachename);
        if (m_sharing && it != mEffectCache.end())
        {
            return it->second;
        }
    }

    auto effect = std::make_shared<PBREffect>(m_device.get(), effectflags, derivedPSD);

    // We don't use EnableDefaultLighting generally for PBR as it uses Image-Based Lighting instead.

    effect->SetAlpha(info.alphaValue);

    effect->SetSurfaceTextures(
        m_texturedescriptors->GetGpuHandle(static_cast<size_t>(albetoTextureIndex)),
        m_texturedescriptors->GetGpuHandle(static_cast<size_t>(normalTextureIndex)),
        m_texturedescriptors->GetGpuHandle(static_cast<size_t>(rmaTextureIndex)),
        m_samplerdescriptors->GetGpuHandle(static_cast<size_t>(samplerIndex)));

    if (emissiveTextureIndex != -1)
    {
        effect->SetEmissiveTexture(m_texturedescriptors->GetGpuHandle(static_cast<size_t>(emissiveTextureIndex)));
    }

    if (m_sharing && !info.name.empty())
    {
        std::lock_guard<std::mutex> lock(mutex);
        EffectCache::value_type v(cachename, effect);
        mEffectCache.insert(v);
    }

    return std::move(effect);
}

void PBREffectFactory::Impl::ReleaseCache()
{
    std::lock_guard<std::mutex> lock(mutex);
    mEffectCache.clear();
}



//--------------------------------------------------------------------------------------
// PBREffectFactory
//--------------------------------------------------------------------------------------

PBREffectFactory::PBREffectFactory(_In_ ID3D12Device* device) noexcept(false)
{
    pimpl = std::make_shared<Impl>(device, nullptr, nullptr);
}

PBREffectFactory::PBREffectFactory(_In_ ID3D12DescriptorHeap* textureDescriptors, _In_ ID3D12DescriptorHeap* samplerDescriptors) noexcept(false)
{
    if (!textureDescriptors)
    {
        throw std::exception("Texture descriptor heap cannot be null if no device is provided. Use the alternative PBREffectFactory constructor instead.");
    }
    if (!samplerDescriptors)
    {
        throw std::exception("Descriptor heap cannot be null if no device is provided. Use the alternative PBREffectFactory constructor instead.");
    }

    if (textureDescriptors->GetDesc().Type != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
    {
        throw std::exception("PBREffectFactory::CreateEffect requires a CBV_SRV_UAV descriptor heap for textureDescriptors.");
    }
    if (samplerDescriptors->GetDesc().Type != D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
    {
        throw std::exception("PBREffectFactory::CreateEffect requires a SAMPLER descriptor heap for samplerDescriptors.");
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

PBREffectFactory::~PBREffectFactory()
{
}


PBREffectFactory::PBREffectFactory(PBREffectFactory&& moveFrom) noexcept
    : pimpl(std::move(moveFrom.pimpl))
{
}

PBREffectFactory& PBREffectFactory::operator= (PBREffectFactory&& moveFrom) noexcept
{
    pimpl = std::move(moveFrom.pimpl);
    return *this;
}

std::shared_ptr<IEffect> PBREffectFactory::CreateEffect(
    const EffectInfo& info, 
    const EffectPipelineStateDescription& opaquePipelineState,
    const EffectPipelineStateDescription& alphaPipelineState,
    const D3D12_INPUT_LAYOUT_DESC& inputLayout, 
    int32_t textureDescriptorOffset,
    int32_t samplerDescriptorOffset)
{
    return pimpl->CreateEffect(info, opaquePipelineState, alphaPipelineState, inputLayout, textureDescriptorOffset, samplerDescriptorOffset);
}

void PBREffectFactory::ReleaseCache()
{
    pimpl->ReleaseCache();
}

void PBREffectFactory::SetSharing(bool enabled) noexcept
{
    pimpl->SetSharing(enabled);
}
