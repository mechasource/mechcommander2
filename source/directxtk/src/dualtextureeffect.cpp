//--------------------------------------------------------------------------------------
// File: DualTextureEffect.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"
#include "effectcommon.h"

using namespace directxtk;
// using Microsoft::WRL::ComPtr;

namespace
{
    // Constant buffer layout. Must match the shader!
    struct DualTextureEffectConstants
    {
        DirectX::XMVECTOR diffuseColor;
        DirectX::XMVECTOR fogColor;
        DirectX::XMVECTOR fogVector;
        DirectX::XMMATRIX worldViewProj;
    };

    static_assert((sizeof(DualTextureEffectConstants) % 16) == 0, "CB size not padded correctly");


    // Traits type describes our characteristics to the EffectBase template.
    struct DualTextureEffectTraits
    {
        using ConstantBufferType = DualTextureEffectConstants;

        static constexpr int32_t VertexShaderCount = 4;
        static constexpr int32_t PixelShaderCount = 2;
        static constexpr int32_t ShaderPermutationCount = 4;
        static constexpr int32_t RootSignatureCount = 1;
    };
}

// Internal DualTextureEffect implementation class.
class DualTextureEffect::Impl : public EffectBase<DualTextureEffectTraits>
{
public:
    Impl(_In_ ID3D12Device* device, uint32_t effectFlags, const EffectPipelineStateDescription& pipelineDescription);
    
    enum RootParameterIndex
    {
        Texture1SRV,
        Texture1Sampler,
        Texture2SRV,
        Texture2Sampler,
        ConstantBuffer,
        RootParameterCount
    };

    EffectColor color;

    D3D12_GPU_DESCRIPTOR_HANDLE texture1;
    D3D12_GPU_DESCRIPTOR_HANDLE texture1Sampler;
    D3D12_GPU_DESCRIPTOR_HANDLE texture2;
    D3D12_GPU_DESCRIPTOR_HANDLE texture2Sampler;

    int32_t GetPipelineStatePermutation(bool vertexColorEnabled) const noexcept;

    void Apply(_In_ ID3D12GraphicsCommandList* commandList);
};


// Include the precompiled shader code.
namespace
{
#if defined(_XBOX_ONE) && defined(_TITLE)
    #include "Shaders/Compiled/XboxOneDualTextureEffect_VSDualTexture.inc"
    #include "Shaders/Compiled/XboxOneDualTextureEffect_VSDualTextureNoFog.inc"
    #include "Shaders/Compiled/XboxOneDualTextureEffect_VSDualTextureVc.inc"
    #include "Shaders/Compiled/XboxOneDualTextureEffect_VSDualTextureVcNoFog.inc"

    #include "Shaders/Compiled/XboxOneDualTextureEffect_PSDualTexture.inc"
    #include "Shaders/Compiled/XboxOneDualTextureEffect_PSDualTextureNoFog.inc"
#else
    #include "Shaders/Compiled/DualTextureEffect_VSDualTexture.inc"
    #include "Shaders/Compiled/DualTextureEffect_VSDualTextureNoFog.inc"
    #include "Shaders/Compiled/DualTextureEffect_VSDualTextureVc.inc"
    #include "Shaders/Compiled/DualTextureEffect_VSDualTextureVcNoFog.inc"

    #include "Shaders/Compiled/DualTextureEffect_PSDualTexture.inc"
    #include "Shaders/Compiled/DualTextureEffect_PSDualTextureNoFog.inc"
#endif
}


template<>
const D3D12_SHADER_BYTECODE EffectBase<DualTextureEffectTraits>::VertexShaderBytecode[] =
{
    { DualTextureEffect_VSDualTexture,        sizeof(DualTextureEffect_VSDualTexture)        },
    { DualTextureEffect_VSDualTextureNoFog,   sizeof(DualTextureEffect_VSDualTextureNoFog)   },
    { DualTextureEffect_VSDualTextureVc,      sizeof(DualTextureEffect_VSDualTextureVc)      },
    { DualTextureEffect_VSDualTextureVcNoFog, sizeof(DualTextureEffect_VSDualTextureVcNoFog) },

};


template<>
const int32_t EffectBase<DualTextureEffectTraits>::VertexShaderIndices[] =
{
    0,      // basic
    1,      // no fog
    2,      // vertex color
    3,      // vertex color, no fog
};


template<>
const D3D12_SHADER_BYTECODE EffectBase<DualTextureEffectTraits>::PixelShaderBytecode[] =
{
    { DualTextureEffect_PSDualTexture,        sizeof(DualTextureEffect_PSDualTexture)        },
    { DualTextureEffect_PSDualTextureNoFog,   sizeof(DualTextureEffect_PSDualTextureNoFog)   },

};


template<>
const int32_t EffectBase<DualTextureEffectTraits>::PixelShaderIndices[] =
{
    0,      // basic
    1,      // no fog
    0,      // vertex color
    1,      // vertex color, no fog
};


// Global pool of per-device DualTextureEffect resources.
template<>
SharedResourcePool<ID3D12Device*, EffectBase<DualTextureEffectTraits>::DeviceResources> EffectBase<DualTextureEffectTraits>::deviceResourcesPool = {};


// Constructor.
DualTextureEffect::Impl::Impl(
    _In_ ID3D12Device* device,
    uint32_t effectFlags,
    const EffectPipelineStateDescription& pipelineDescription)
    : EffectBase(device),
        texture1{},
        texture1Sampler{},
        texture2{},
        texture2Sampler{}
{
    static_assert(_countof(EffectBase<DualTextureEffectTraits>::VertexShaderIndices) == DualTextureEffectTraits::ShaderPermutationCount, "array/max mismatch");
    static_assert(_countof(EffectBase<DualTextureEffectTraits>::VertexShaderBytecode) == DualTextureEffectTraits::VertexShaderCount, "array/max mismatch");
    static_assert(_countof(EffectBase<DualTextureEffectTraits>::PixelShaderBytecode) == DualTextureEffectTraits::PixelShaderCount, "array/max mismatch");
    static_assert(_countof(EffectBase<DualTextureEffectTraits>::PixelShaderIndices) == DualTextureEffectTraits::ShaderPermutationCount, "array/max mismatch");

    // Create root signature.
    {
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

        CD3DX12_ROOT_PARAMETER rootParameters[RootParameterIndex::RootParameterCount] = {};
        rootParameters[RootParameterIndex::ConstantBuffer].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

        // Texture 1
        CD3DX12_DESCRIPTOR_RANGE texture1Range(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        CD3DX12_DESCRIPTOR_RANGE texture1SamplerRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);
        rootParameters[RootParameterIndex::Texture1SRV].InitAsDescriptorTable(1, &texture1Range, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameterIndex::Texture1Sampler].InitAsDescriptorTable(1, &texture1SamplerRange, D3D12_SHADER_VISIBILITY_PIXEL);

        // Texture 2
        CD3DX12_DESCRIPTOR_RANGE texture2Range(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
        CD3DX12_DESCRIPTOR_RANGE texture2SamplerRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 1);
        rootParameters[RootParameterIndex::Texture2SRV].InitAsDescriptorTable(1, &texture2Range, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameterIndex::Texture2Sampler].InitAsDescriptorTable(1, &texture2SamplerRange, D3D12_SHADER_VISIBILITY_PIXEL);

        // Create the root signature
        CD3DX12_ROOT_SIGNATURE_DESC rsigDesc = {};
        rsigDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

        m_prootsignature = GetRootSignature(0, rsigDesc);
    }

    assert(m_prootsignature != nullptr);

    // Validate flags & state.
    fog.enabled = (effectFlags & EffectFlags::Fog) != 0;

    if (effectFlags & EffectFlags::PerPixelLightingBit)
    {
        DebugTrace("ERROR: DualTextureEffect does not implement EffectFlags::PerPixelLighting\n");
        throw std::invalid_argument("DualTextureEffect");
    }
    else if (effectFlags & EffectFlags::Lighting)
    {
        DebugTrace("ERROR: DualTextureEffect does not implement EffectFlags::Lighting\n");
        throw std::invalid_argument("DualTextureEffect");
    }

    // Create pipeline state.
    int32_t sp = GetPipelineStatePermutation(
        (effectFlags & EffectFlags::VertexColor) != 0);
    assert(sp >= 0 && sp < DualTextureEffectTraits::ShaderPermutationCount);
    _Analysis_assume_(sp >= 0 && sp < DualTextureEffectTraits::ShaderPermutationCount);

    int32_t vi = EffectBase<DualTextureEffectTraits>::VertexShaderIndices[sp];
    assert(vi >= 0 && vi < DualTextureEffectTraits::VertexShaderCount);
    _Analysis_assume_(vi >= 0 && vi < DualTextureEffectTraits::VertexShaderCount);
    int32_t pi = EffectBase<DualTextureEffectTraits>::PixelShaderIndices[sp];
    assert(pi >= 0 && pi < DualTextureEffectTraits::PixelShaderCount);
    _Analysis_assume_(pi >= 0 && pi < DualTextureEffectTraits::PixelShaderCount);

    pipelineDescription.CreatePipelineState(
        device,
        m_prootsignature,
        EffectBase<DualTextureEffectTraits>::VertexShaderBytecode[vi],
        EffectBase<DualTextureEffectTraits>::PixelShaderBytecode[pi],
        m_ppipelinestate.addressof());

    SetDebugObjectName(m_ppipelinestate.get(), L"DualTextureEffect");
}


int32_t DualTextureEffect::Impl::GetPipelineStatePermutation(bool vertexColorEnabled) const noexcept
{
    int32_t permutation = 0;

    // Use optimized shaders if fog is disabled.
    if (!fog.enabled)
    {
        permutation += 1;
    }

    // Support vertex coloring?
    if (vertexColorEnabled)
    {
        permutation += 2;
    }

    return permutation;
}


// Sets our state onto the D3D device.
void DualTextureEffect::Impl::Apply(_In_ ID3D12GraphicsCommandList* commandList)
{
    // Compute derived parameter values.
    matrices.SetConstants(dirtyFlags, constants.worldViewProj);

    fog.SetConstants(dirtyFlags, matrices.worldView, constants.fogVector);

    color.SetConstants(dirtyFlags, constants.diffuseColor);

    UpdateConstants();

    // Set the root signature
    commandList->SetGraphicsRootSignature(m_prootsignature);

    // Set the textures
    if (!texture1.ptr || !texture2.ptr)
    {
        DebugTrace("ERROR: Missing texture(s) for DualTextureEffect (texture1 %llu, texture2 %llu)\n", texture1.ptr, texture2.ptr);
        throw std::exception("DualTextureEffect");
    }
    if (!texture1Sampler.ptr || !texture2Sampler.ptr)
    {
        DebugTrace("ERROR: Missing sampler(s) for DualTextureEffect (samplers1 %llu, samplers2 %llu)\n", texture2Sampler.ptr, texture2Sampler.ptr);
        throw std::exception("DualTextureEffect");
    }

    // **NOTE** If D3D asserts or crashes here, you probably need to call commandList->SetDescriptorHeaps() with the required descriptor heaps.
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::Texture1SRV, texture1);
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::Texture1Sampler, texture1Sampler);
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::Texture2SRV, texture2);
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::Texture2Sampler, texture2Sampler);

    // Set constants
    commandList->SetGraphicsRootConstantBufferView(RootParameterIndex::ConstantBuffer, GetConstantBufferGpuAddress());

    // Set the pipeline state
    commandList->SetPipelineState(EffectBase::m_ppipelinestate.get());
}


// Public constructor.
DualTextureEffect::DualTextureEffect(
    _In_ ID3D12Device* device,
    uint32_t effectFlags,
    const EffectPipelineStateDescription& pipelineDescription)
    : pimpl(std::make_unique<Impl>(device, effectFlags, pipelineDescription))
{
}


// Move constructor.
DualTextureEffect::DualTextureEffect(DualTextureEffect&& moveFrom) noexcept
    : pimpl(std::move(moveFrom.pimpl))
{
}


// Move assignment.
DualTextureEffect& DualTextureEffect::operator= (DualTextureEffect&& moveFrom) noexcept
{
    pimpl = std::move(moveFrom.pimpl);
    return *this;
}


// Public destructor.
DualTextureEffect::~DualTextureEffect()
{
}


// IEffect methods
void DualTextureEffect::Apply(_In_ ID3D12GraphicsCommandList* commandList)
{
    pimpl->Apply(commandList);
}


// Camera settings
void XM_CALLCONV DualTextureEffect::SetWorld(DirectX::FXMMATRIX value)
{
    pimpl->matrices.world = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose | EffectDirtyFlags::FogVector;
}


void XM_CALLCONV DualTextureEffect::SetView(DirectX::FXMMATRIX value)
{
    pimpl->matrices.view = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::EyePosition | EffectDirtyFlags::FogVector;
}


void XM_CALLCONV DualTextureEffect::SetProjection(DirectX::FXMMATRIX value)
{
    pimpl->matrices.projection = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj;
}


void XM_CALLCONV DualTextureEffect::SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
{
    pimpl->matrices.world = world;
    pimpl->matrices.view = view;
    pimpl->matrices.projection = projection;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose | EffectDirtyFlags::EyePosition | EffectDirtyFlags::FogVector;
}


// Material settings
void XM_CALLCONV DualTextureEffect::SetDiffuseColor(DirectX::FXMVECTOR value)
{
    pimpl->color.diffuseColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void DualTextureEffect::SetAlpha(float value)
{
    pimpl->color.alpha = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void XM_CALLCONV DualTextureEffect::SetColorAndAlpha(DirectX::FXMVECTOR value)
{
    pimpl->color.diffuseColor = value;
    pimpl->color.alpha = DirectX::XMVectorGetW(value);

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


// Fog settings.
void DualTextureEffect::SetFogStart(float value)
{
    pimpl->fog.start = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::FogVector;
}


void DualTextureEffect::SetFogEnd(float value)
{
    pimpl->fog.end = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::FogVector;
}


void XM_CALLCONV DualTextureEffect::SetFogColor(DirectX::FXMVECTOR value)
{
    pimpl->constants.fogColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


// Texture settings.
void DualTextureEffect::SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor)
{
    pimpl->texture1 = srvDescriptor;
    pimpl->texture1Sampler = samplerDescriptor;
}


void DualTextureEffect::SetTexture2(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor)
{
    pimpl->texture2 = srvDescriptor;
    pimpl->texture2Sampler = samplerDescriptor;
}
