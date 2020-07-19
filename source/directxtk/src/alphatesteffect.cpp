//--------------------------------------------------------------------------------------
// File: AlphaTestEffect.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"

#include "effectcommon.h"

using namespace directxtk;

namespace
{
    // Constant buffer layout. Must match the shader!
    struct AlphaTestEffectConstants
    {
        DirectX::XMVECTOR diffuseColor;
        DirectX::XMVECTOR alphaTest;
        DirectX::XMVECTOR fogColor;
        DirectX::XMVECTOR fogVector;
        DirectX::XMMATRIX worldViewProj;
    };

    static_assert((sizeof(AlphaTestEffectConstants) % 16) == 0, "CB size not padded correctly");


    // Traits type describes our characteristics to the EffectBase template.
    struct AlphaTestEffectTraits
    {
        using ConstantBufferType = AlphaTestEffectConstants;

        static constexpr int32_t VertexShaderCount = 4;
        static constexpr int32_t PixelShaderCount = 4;
        static constexpr int32_t ShaderPermutationCount = 8;
        static constexpr int32_t RootSignatureCount = 1;
    };
}

// Internal AlphaTestEffect implementation class.
class AlphaTestEffect::Impl : public EffectBase<AlphaTestEffectTraits>
{
public:
    Impl(_In_ ID3D12Device* device, uint32_t effectFlags, const EffectPipelineStateDescription& pipelineDescription,
        D3D12_COMPARISON_FUNC alphaFunction);

    enum RootParameterIndex
    {
        ConstantBuffer,
        TextureSRV,
        TextureSampler,
        RootParameterCount
    };

    D3D12_COMPARISON_FUNC mAlphaFunction;
    int32_t referenceAlpha;

    EffectColor color;

    D3D12_GPU_DESCRIPTOR_HANDLE texture;
    D3D12_GPU_DESCRIPTOR_HANDLE textureSampler;
    
    int32_t GetPipelineStatePermutation(bool vertexColorEnabled) const noexcept;

    void Apply(_In_ ID3D12GraphicsCommandList* commandList);
};


// Include the precompiled shader code.
namespace
{
#if defined(_XBOX_ONE) && defined(_TITLE)
    #include "Shaders/Compiled/XboxOneAlphaTestEffect_VSAlphaTest.inc"
    #include "Shaders/Compiled/XboxOneAlphaTestEffect_VSAlphaTestNoFog.inc"
    #include "Shaders/Compiled/XboxOneAlphaTestEffect_VSAlphaTestVc.inc"
    #include "Shaders/Compiled/XboxOneAlphaTestEffect_VSAlphaTestVcNoFog.inc"

    #include "Shaders/Compiled/XboxOneAlphaTestEffect_PSAlphaTestLtGt.inc"
    #include "Shaders/Compiled/XboxOneAlphaTestEffect_PSAlphaTestLtGtNoFog.inc"
    #include "Shaders/Compiled/XboxOneAlphaTestEffect_PSAlphaTestEqNe.inc"
    #include "Shaders/Compiled/XboxOneAlphaTestEffect_PSAlphaTestEqNeNoFog.inc"
#else
    #include "Shaders/Compiled/AlphaTestEffect_VSAlphaTest.inc"
    #include "Shaders/Compiled/AlphaTestEffect_VSAlphaTestNoFog.inc"
    #include "Shaders/Compiled/AlphaTestEffect_VSAlphaTestVc.inc"
    #include "Shaders/Compiled/AlphaTestEffect_VSAlphaTestVcNoFog.inc"

    #include "Shaders/Compiled/AlphaTestEffect_PSAlphaTestLtGt.inc"
    #include "Shaders/Compiled/AlphaTestEffect_PSAlphaTestLtGtNoFog.inc"
    #include "Shaders/Compiled/AlphaTestEffect_PSAlphaTestEqNe.inc"
    #include "Shaders/Compiled/AlphaTestEffect_PSAlphaTestEqNeNoFog.inc"
#endif
}


template<>
const D3D12_SHADER_BYTECODE EffectBase<AlphaTestEffectTraits>::VertexShaderBytecode[] =
{
    { AlphaTestEffect_VSAlphaTest,        sizeof(AlphaTestEffect_VSAlphaTest)        },
    { AlphaTestEffect_VSAlphaTestNoFog,   sizeof(AlphaTestEffect_VSAlphaTestNoFog)   },
    { AlphaTestEffect_VSAlphaTestVc,      sizeof(AlphaTestEffect_VSAlphaTestVc)      },
    { AlphaTestEffect_VSAlphaTestVcNoFog, sizeof(AlphaTestEffect_VSAlphaTestVcNoFog) },
};


template<>
const int32_t EffectBase<AlphaTestEffectTraits>::VertexShaderIndices[] =
{
    0,      // lt/gt
    1,      // lt/gt, no fog
    2,      // lt/gt, vertex color
    3,      // lt/gt, vertex color, no fog
    
    0,      // eq/ne
    1,      // eq/ne, no fog
    2,      // eq/ne, vertex color
    3,      // eq/ne, vertex color, no fog
};


template<>
const D3D12_SHADER_BYTECODE EffectBase<AlphaTestEffectTraits>::PixelShaderBytecode[] =
{
    { AlphaTestEffect_PSAlphaTestLtGt,      sizeof(AlphaTestEffect_PSAlphaTestLtGt)      },
    { AlphaTestEffect_PSAlphaTestLtGtNoFog, sizeof(AlphaTestEffect_PSAlphaTestLtGtNoFog) },
    { AlphaTestEffect_PSAlphaTestEqNe,      sizeof(AlphaTestEffect_PSAlphaTestEqNe)      },
    { AlphaTestEffect_PSAlphaTestEqNeNoFog, sizeof(AlphaTestEffect_PSAlphaTestEqNeNoFog) },
};


template<>
const int32_t EffectBase<AlphaTestEffectTraits>::PixelShaderIndices[] =
{
    0,      // lt/gt
    1,      // lt/gt, no fog
    0,      // lt/gt, vertex color
    1,      // lt/gt, vertex color, no fog
    
    2,      // eq/ne
    3,      // eq/ne, no fog
    2,      // eq/ne, vertex color
    3,      // eq/ne, vertex color, no fog
};


// Global pool of per-device AlphaTestEffect resources.
template<>
SharedResourcePool<ID3D12Device*, EffectBase<AlphaTestEffectTraits>::DeviceResources> EffectBase<AlphaTestEffectTraits>::deviceResourcesPool = {};

// Constructor.
AlphaTestEffect::Impl::Impl(
    _In_ ID3D12Device* device,
    uint32_t effectFlags,
    const EffectPipelineStateDescription& pipelineDescription,
    D3D12_COMPARISON_FUNC alphaFunction)
    : EffectBase(device),
        mAlphaFunction(alphaFunction),
        referenceAlpha(0),
        texture{},
        textureSampler{}
{
    static_assert(_countof(EffectBase<AlphaTestEffectTraits>::VertexShaderIndices) == AlphaTestEffectTraits::ShaderPermutationCount, "array/max mismatch");
    static_assert(_countof(EffectBase<AlphaTestEffectTraits>::VertexShaderBytecode) == AlphaTestEffectTraits::VertexShaderCount, "array/max mismatch");
    static_assert(_countof(EffectBase<AlphaTestEffectTraits>::PixelShaderBytecode) == AlphaTestEffectTraits::PixelShaderCount, "array/max mismatch");
    static_assert(_countof(EffectBase<AlphaTestEffectTraits>::PixelShaderIndices) == AlphaTestEffectTraits::ShaderPermutationCount, "array/max mismatch");

    // Create root signature.
    {
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

        CD3DX12_DESCRIPTOR_RANGE textureRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        CD3DX12_DESCRIPTOR_RANGE textureSamplerRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

        CD3DX12_ROOT_PARAMETER rootParameters[RootParameterIndex::RootParameterCount] = {};
        rootParameters[RootParameterIndex::TextureSRV].InitAsDescriptorTable(1, &textureRange, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameterIndex::TextureSampler].InitAsDescriptorTable(1, &textureSamplerRange, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameterIndex::ConstantBuffer].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

        CD3DX12_ROOT_SIGNATURE_DESC rsigDesc = {};
        rsigDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

        m_prootsignature = GetRootSignature(0, rsigDesc);
    }

    assert(m_prootsignature != nullptr);

    fog.enabled = (effectFlags & EffectFlags::Fog) != 0;

    if (effectFlags & EffectFlags::PerPixelLightingBit)
    {
        DebugTrace("ERROR: AlphaTestEffect does not implement EffectFlags::PerPixelLighting\n");
        throw std::invalid_argument("AlphaTestEffect");
    }
    else if (effectFlags & EffectFlags::Lighting)
    {
        DebugTrace("ERROR: DualTextureEffect does not implement EffectFlags::Lighting\n");
        throw std::invalid_argument("AlphaTestEffect");
    }

    // Create pipeline state.
    int32_t sp = GetPipelineStatePermutation(
        (effectFlags & EffectFlags::VertexColor) != 0);
    assert(sp >= 0 && sp < AlphaTestEffectTraits::ShaderPermutationCount);
    _Analysis_assume_(sp >= 0 && sp < AlphaTestEffectTraits::ShaderPermutationCount);

    int32_t vi = EffectBase<AlphaTestEffectTraits>::VertexShaderIndices[sp];
    assert(vi >= 0 && vi < AlphaTestEffectTraits::VertexShaderCount);
    _Analysis_assume_(vi >= 0 && vi < AlphaTestEffectTraits::VertexShaderCount);
    int32_t pi = EffectBase<AlphaTestEffectTraits>::PixelShaderIndices[sp];
    assert(pi >= 0 && pi < AlphaTestEffectTraits::PixelShaderCount);
    _Analysis_assume_(pi >= 0 && pi < AlphaTestEffectTraits::PixelShaderCount);

    pipelineDescription.CreatePipelineState(
        device,
        m_prootsignature,
        EffectBase<AlphaTestEffectTraits>::VertexShaderBytecode[vi],
        EffectBase<AlphaTestEffectTraits>::PixelShaderBytecode[pi],
        m_ppipelinestate.addressof());

    SetDebugObjectName(m_ppipelinestate.get(), L"AlphaTestEffect");
}


int32_t AlphaTestEffect::Impl::GetPipelineStatePermutation(bool vertexColorEnabled) const noexcept
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

    // Which alpha compare mode?
    if (mAlphaFunction == D3D12_COMPARISON_FUNC_EQUAL ||
        mAlphaFunction == D3D12_COMPARISON_FUNC_NOT_EQUAL)
    {
        permutation += 4;
    }

    return permutation;
}


// Sets our state onto the D3D device.
void AlphaTestEffect::Impl::Apply(_In_ ID3D12GraphicsCommandList* commandList)
{
    // Compute derived parameter values.
    matrices.SetConstants(dirtyFlags, constants.worldViewProj);
    fog.SetConstants(dirtyFlags, matrices.worldView, constants.fogVector);            
    color.SetConstants(dirtyFlags, constants.diffuseColor);

    UpdateConstants();

    // Recompute the alpha test settings?
    if (dirtyFlags & EffectDirtyFlags::AlphaTest)
    {
        // Convert reference alpha from 8 bit integer to 0-1 float format.
        auto reference = static_cast<float>(referenceAlpha) / 255.0f;
                
        // Comparison tolerance of half the 8 bit integer precision.
        const float threshold = 0.5f / 255.0f;

        // What to do if the alpha comparison passes or fails. Positive accepts the pixel, negative clips it.
        static const DirectX::XMVECTORF32 selectIfTrue  = { { {  1, -1 } } };
        static const DirectX::XMVECTORF32 selectIfFalse = { { { -1,  1 } } };
        static const DirectX::XMVECTORF32 selectNever   = { { { -1, -1 } } };
        static const DirectX::XMVECTORF32 selectAlways  = { { {  1,  1 } } };

        float compareTo;
        DirectX::XMVECTOR resultSelector;

        switch (mAlphaFunction)
        {
            case D3D12_COMPARISON_FUNC_LESS:
                // Shader will evaluate: clip((a < x) ? z : w)
                compareTo = reference - threshold;
                resultSelector = selectIfTrue;
                break;

            case D3D12_COMPARISON_FUNC_LESS_EQUAL:
                // Shader will evaluate: clip((a < x) ? z : w)
                compareTo = reference + threshold;
                resultSelector = selectIfTrue;
                break;

            case D3D12_COMPARISON_FUNC_GREATER_EQUAL:
                // Shader will evaluate: clip((a < x) ? z : w)
                compareTo = reference - threshold;
                resultSelector = selectIfFalse;
                break;

            case D3D12_COMPARISON_FUNC_GREATER:
                // Shader will evaluate: clip((a < x) ? z : w)
                compareTo = reference + threshold;
                resultSelector = selectIfFalse;
                break;

            case D3D12_COMPARISON_FUNC_EQUAL:
                // Shader will evaluate: clip((abs(a - x) < y) ? z : w)
                compareTo = reference;
                resultSelector = selectIfTrue;
                break;

            case D3D12_COMPARISON_FUNC_NOT_EQUAL:
                // Shader will evaluate: clip((abs(a - x) < y) ? z : w)
                compareTo = reference;
                resultSelector = selectIfFalse;
                break;

            case D3D12_COMPARISON_FUNC_NEVER:
                // Shader will evaluate: clip((a < x) ? z : w)
                compareTo = 0;
                resultSelector = selectNever;
                break;

            case D3D12_COMPARISON_FUNC_ALWAYS:
                // Shader will evaluate: clip((a < x) ? z : w)
                compareTo = 0;
                resultSelector = selectAlways;
                break;

            default:
                throw std::exception("Unknown alpha test function");
        }

        // x = compareTo, y = threshold, zw = resultSelector.
        constants.alphaTest = DirectX::XMVectorPermute<0, 1, 4, 5>(DirectX::XMVectorSet(compareTo, threshold, 0, 0), resultSelector);
                
        dirtyFlags &= ~EffectDirtyFlags::AlphaTest;
        dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
    }

    // Set the root signature
    commandList->SetGraphicsRootSignature(m_prootsignature);

    // Set the texture
    if (!texture.ptr || !textureSampler.ptr)
    {
        DebugTrace("ERROR: Missing texture or sampler for AlphaTestEffect (texture %llu, sampler %llu)\n", texture.ptr, textureSampler.ptr);
        throw std::exception("AlphaTestEffect");
    }

    // **NOTE** If D3D asserts or crashes here, you probably need to call commandList->SetDescriptorHeaps() with the required descriptor heaps.
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSRV, texture);
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSampler, textureSampler);

    // Set constants
    commandList->SetGraphicsRootConstantBufferView(RootParameterIndex::ConstantBuffer, GetConstantBufferGpuAddress());

    // Set the pipeline state
    commandList->SetPipelineState(EffectBase::m_ppipelinestate.get());
}

// Public constructor.
AlphaTestEffect::AlphaTestEffect(
    _In_ ID3D12Device* device,
    uint32_t effectFlags,
    const EffectPipelineStateDescription& pipelineDescription,
    D3D12_COMPARISON_FUNC alphaFunction)
    : pimpl(std::make_unique<Impl>(device, effectFlags, pipelineDescription, alphaFunction))
{
}


// Move constructor.
AlphaTestEffect::AlphaTestEffect(AlphaTestEffect&& moveFrom) noexcept
    : pimpl(std::move(moveFrom.pimpl))
{
}


// Move assignment.
AlphaTestEffect& AlphaTestEffect::operator= (AlphaTestEffect&& moveFrom) noexcept
{
    pimpl = std::move(moveFrom.pimpl);
    return *this;
}


// Public destructor.
AlphaTestEffect::~AlphaTestEffect()
{
}


// IEffect methods
void AlphaTestEffect::Apply(_In_ ID3D12GraphicsCommandList* commandList)
{
    pimpl->Apply(commandList);
}


// Camera settings
void XM_CALLCONV AlphaTestEffect::SetWorld(DirectX::FXMMATRIX value)
{
    pimpl->matrices.world = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose | EffectDirtyFlags::FogVector;
}


void XM_CALLCONV AlphaTestEffect::SetView(DirectX::FXMMATRIX value)
{
    pimpl->matrices.view = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::EyePosition | EffectDirtyFlags::FogVector;
}


void XM_CALLCONV AlphaTestEffect::SetProjection(DirectX::FXMMATRIX value)
{
    pimpl->matrices.projection = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj;
}


void XM_CALLCONV AlphaTestEffect::SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
{
    pimpl->matrices.world = world;
    pimpl->matrices.view = view;
    pimpl->matrices.projection = projection;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose | EffectDirtyFlags::EyePosition | EffectDirtyFlags::FogVector;
}


// Material settings
void XM_CALLCONV AlphaTestEffect::SetDiffuseColor(DirectX::FXMVECTOR value)
{
    pimpl->color.diffuseColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void AlphaTestEffect::SetAlpha(float value)
{
    pimpl->color.alpha = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void XM_CALLCONV AlphaTestEffect::SetColorAndAlpha(DirectX::FXMVECTOR value)
{
    pimpl->color.diffuseColor = value;
    pimpl->color.alpha = DirectX::XMVectorGetW(value);

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


// Fog settings.
void AlphaTestEffect::SetFogStart(float value)
{
    pimpl->fog.start = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::FogVector;
}


void AlphaTestEffect::SetFogEnd(float value)
{
    pimpl->fog.end = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::FogVector;
}


void XM_CALLCONV AlphaTestEffect::SetFogColor(DirectX::FXMVECTOR value)
{
    pimpl->constants.fogColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


// Texture settings.
void AlphaTestEffect::SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor)
{
    pimpl->texture = srvDescriptor;
    pimpl->textureSampler = samplerDescriptor;
}


void AlphaTestEffect::SetReferenceAlpha(int32_t value)
{
    pimpl->referenceAlpha = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::AlphaTest;
}
