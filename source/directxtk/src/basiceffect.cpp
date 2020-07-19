//--------------------------------------------------------------------------------------
// File: BasicEffect.cpp
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
    struct BasicEffectConstants
    {
        DirectX::XMVECTOR diffuseColor;
        DirectX::XMVECTOR emissiveColor;
        DirectX::XMVECTOR specularColorAndPower;

        DirectX::XMVECTOR lightDirection[IEffectLights::MaxDirectionalLights];
        DirectX::XMVECTOR lightDiffuseColor[IEffectLights::MaxDirectionalLights];
        DirectX::XMVECTOR lightSpecularColor[IEffectLights::MaxDirectionalLights];

        DirectX::XMVECTOR eyePosition;

        DirectX::XMVECTOR fogColor;
        DirectX::XMVECTOR fogVector;

        DirectX::XMMATRIX world;
        DirectX::XMVECTOR worldInverseTranspose[3];
        DirectX::XMMATRIX worldViewProj;
    };

    static_assert((sizeof(BasicEffectConstants) % 16) == 0, "CB size not padded correctly");


    // Traits type describes our characteristics to the EffectBase template.
    struct BasicEffectTraits
    {
        using ConstantBufferType = BasicEffectConstants;

        static constexpr int32_t VertexShaderCount = 24;
        static constexpr int32_t PixelShaderCount = 10;
        static constexpr int32_t ShaderPermutationCount = 40;
        static constexpr int32_t RootSignatureCount = 2;
    };
}

// Internal BasicEffect implementation class.
class BasicEffect::Impl : public EffectBase<BasicEffectTraits>
{
public:
    Impl(_In_ ID3D12Device* device, uint32_t effectFlags, const EffectPipelineStateDescription& pipelineDescription);

    enum RootParameterIndex
    {
        ConstantBuffer,
        TextureSRV,
        TextureSampler,
        RootParameterCount
    };

    bool lightingEnabled;
    bool textureEnabled;

    D3D12_GPU_DESCRIPTOR_HANDLE texture;
    D3D12_GPU_DESCRIPTOR_HANDLE sampler;

    EffectLights lights;

    int32_t GetPipelineStatePermutation(bool preferPerPixelLighting, bool vertexColorEnabled, bool biasedVertexNormals) const noexcept;

    void Apply(_In_ ID3D12GraphicsCommandList* commandList);
};


// Include the precompiled shader code.
namespace
{
#if defined(_XBOX_ONE) && defined(_TITLE)
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasic.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicNoFog.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVc.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVcNoFog.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicTx.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicTxNoFog.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicTxVc.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicTxVcNoFog.inc"

    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLighting.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLightingVc.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLightingTx.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLightingTxVc.inc"

    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLighting.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLightingVc.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLightingTx.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLightingTxVc.inc"

    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLightingBn.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLightingVcBn.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLightingTxBn.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLightingTxVcBn.inc"

    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLightingBn.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLightingVcBn.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLightingTxBn.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLightingTxVcBn.inc"

    #include "Shaders/Compiled/XboxOneBasicEffect_PSBasic.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_PSBasicNoFog.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_PSBasicTx.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_PSBasicTxNoFog.inc"

    #include "Shaders/Compiled/XboxOneBasicEffect_PSBasicVertexLighting.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_PSBasicVertexLightingNoFog.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_PSBasicVertexLightingTx.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_PSBasicVertexLightingTxNoFog.inc"

    #include "Shaders/Compiled/XboxOneBasicEffect_PSBasicPixelLighting.inc"
    #include "Shaders/Compiled/XboxOneBasicEffect_PSBasicPixelLightingTx.inc"
#else
    #include "Shaders/Compiled/BasicEffect_VSBasic.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicNoFog.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicVc.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicVcNoFog.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicTx.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicTxNoFog.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicTxVc.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicTxVcNoFog.inc"

    #include "Shaders/Compiled/BasicEffect_VSBasicVertexLighting.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicVertexLightingVc.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicVertexLightingTx.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicVertexLightingTxVc.inc"

    #include "Shaders/Compiled/BasicEffect_VSBasicPixelLighting.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicPixelLightingVc.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicPixelLightingTx.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicPixelLightingTxVc.inc"

    #include "Shaders/Compiled/BasicEffect_VSBasicVertexLightingBn.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicVertexLightingVcBn.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicVertexLightingTxBn.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicVertexLightingTxVcBn.inc"

    #include "Shaders/Compiled/BasicEffect_VSBasicPixelLightingBn.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicPixelLightingVcBn.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicPixelLightingTxBn.inc"
    #include "Shaders/Compiled/BasicEffect_VSBasicPixelLightingTxVcBn.inc"

    #include "Shaders/Compiled/BasicEffect_PSBasic.inc"
    #include "Shaders/Compiled/BasicEffect_PSBasicNoFog.inc"
    #include "Shaders/Compiled/BasicEffect_PSBasicTx.inc"
    #include "Shaders/Compiled/BasicEffect_PSBasicTxNoFog.inc"

    #include "Shaders/Compiled/BasicEffect_PSBasicVertexLighting.inc"
    #include "Shaders/Compiled/BasicEffect_PSBasicVertexLightingNoFog.inc"
    #include "Shaders/Compiled/BasicEffect_PSBasicVertexLightingTx.inc"
    #include "Shaders/Compiled/BasicEffect_PSBasicVertexLightingTxNoFog.inc"

    #include "Shaders/Compiled/BasicEffect_PSBasicPixelLighting.inc"
    #include "Shaders/Compiled/BasicEffect_PSBasicPixelLightingTx.inc"
#endif
}


template<>
const D3D12_SHADER_BYTECODE EffectBase<BasicEffectTraits>::VertexShaderBytecode[] =
{
    { BasicEffect_VSBasic,                     sizeof(BasicEffect_VSBasic)                     },
    { BasicEffect_VSBasicNoFog,                sizeof(BasicEffect_VSBasicNoFog)                },
    { BasicEffect_VSBasicVc,                   sizeof(BasicEffect_VSBasicVc)                   },
    { BasicEffect_VSBasicVcNoFog,              sizeof(BasicEffect_VSBasicVcNoFog)              },
    { BasicEffect_VSBasicTx,                   sizeof(BasicEffect_VSBasicTx)                   },
    { BasicEffect_VSBasicTxNoFog,              sizeof(BasicEffect_VSBasicTxNoFog)              },
    { BasicEffect_VSBasicTxVc,                 sizeof(BasicEffect_VSBasicTxVc)                 },
    { BasicEffect_VSBasicTxVcNoFog,            sizeof(BasicEffect_VSBasicTxVcNoFog)            },

    { BasicEffect_VSBasicVertexLighting,       sizeof(BasicEffect_VSBasicVertexLighting)       },
    { BasicEffect_VSBasicVertexLightingVc,     sizeof(BasicEffect_VSBasicVertexLightingVc)     },
    { BasicEffect_VSBasicVertexLightingTx,     sizeof(BasicEffect_VSBasicVertexLightingTx)     },
    { BasicEffect_VSBasicVertexLightingTxVc,   sizeof(BasicEffect_VSBasicVertexLightingTxVc)   },

    { BasicEffect_VSBasicPixelLighting,        sizeof(BasicEffect_VSBasicPixelLighting)        },
    { BasicEffect_VSBasicPixelLightingVc,      sizeof(BasicEffect_VSBasicPixelLightingVc)      },
    { BasicEffect_VSBasicPixelLightingTx,      sizeof(BasicEffect_VSBasicPixelLightingTx)      },
    { BasicEffect_VSBasicPixelLightingTxVc,    sizeof(BasicEffect_VSBasicPixelLightingTxVc)    },

    { BasicEffect_VSBasicVertexLightingBn,     sizeof(BasicEffect_VSBasicVertexLightingBn)     },
    { BasicEffect_VSBasicVertexLightingVcBn,   sizeof(BasicEffect_VSBasicVertexLightingVcBn)   },
    { BasicEffect_VSBasicVertexLightingTxBn,   sizeof(BasicEffect_VSBasicVertexLightingTxBn)   },
    { BasicEffect_VSBasicVertexLightingTxVcBn, sizeof(BasicEffect_VSBasicVertexLightingTxVcBn) },

    { BasicEffect_VSBasicPixelLightingBn,      sizeof(BasicEffect_VSBasicPixelLightingBn)      },
    { BasicEffect_VSBasicPixelLightingVcBn,    sizeof(BasicEffect_VSBasicPixelLightingVcBn)    },
    { BasicEffect_VSBasicPixelLightingTxBn,    sizeof(BasicEffect_VSBasicPixelLightingTxBn)    },
    { BasicEffect_VSBasicPixelLightingTxVcBn,  sizeof(BasicEffect_VSBasicPixelLightingTxVcBn)  },
};


template<>
const int32_t EffectBase<BasicEffectTraits>::VertexShaderIndices[] =
{
    0,      // basic
    1,      // no fog
    2,      // vertex color
    3,      // vertex color, no fog
    4,      // texture
    5,      // texture, no fog
    6,      // texture + vertex color
    7,      // texture + vertex color, no fog

    8,      // vertex lighting
    8,      // vertex lighting, no fog
    9,      // vertex lighting + vertex color
    9,      // vertex lighting + vertex color, no fog
    10,     // vertex lighting + texture
    10,     // vertex lighting + texture, no fog
    11,     // vertex lighting + texture + vertex color
    11,     // vertex lighting + texture + vertex color, no fog

    12,     // pixel lighting
    12,     // pixel lighting, no fog
    13,     // pixel lighting + vertex color
    13,     // pixel lighting + vertex color, no fog
    14,     // pixel lighting + texture
    14,     // pixel lighting + texture, no fog
    15,     // pixel lighting + texture + vertex color
    15,     // pixel lighting + texture + vertex color, no fog

    16,     // vertex lighting (biased vertex normals)
    16,     // vertex lighting (biased vertex normals), no fog
    17,     // vertex lighting (biased vertex normals) + vertex color
    17,     // vertex lighting (biased vertex normals) + vertex color, no fog
    18,     // vertex lighting (biased vertex normals) + texture
    18,     // vertex lighting (biased vertex normals) + texture, no fog
    19,     // vertex lighting (biased vertex normals) + texture + vertex color
    19,     // vertex lighting (biased vertex normals) + texture + vertex color, no fog

    20,     // pixel lighting (biased vertex normals)
    20,     // pixel lighting (biased vertex normals), no fog
    21,     // pixel lighting (biased vertex normals) + vertex color
    21,     // pixel lighting (biased vertex normals) + vertex color, no fog
    22,     // pixel lighting (biased vertex normals) + texture
    22,     // pixel lighting (biased vertex normals) + texture, no fog
    23,     // pixel lighting (biased vertex normals) + texture + vertex color
    23,     // pixel lighting (biased vertex normals) + texture + vertex color, no fog
};


template<>
const D3D12_SHADER_BYTECODE EffectBase<BasicEffectTraits>::PixelShaderBytecode[] =
{
    { BasicEffect_PSBasic,                      sizeof(BasicEffect_PSBasic)                      },
    { BasicEffect_PSBasicNoFog,                 sizeof(BasicEffect_PSBasicNoFog)                 },
    { BasicEffect_PSBasicTx,                    sizeof(BasicEffect_PSBasicTx)                    },
    { BasicEffect_PSBasicTxNoFog,               sizeof(BasicEffect_PSBasicTxNoFog)               },

    { BasicEffect_PSBasicVertexLighting,        sizeof(BasicEffect_PSBasicVertexLighting)        },
    { BasicEffect_PSBasicVertexLightingNoFog,   sizeof(BasicEffect_PSBasicVertexLightingNoFog)   },
    { BasicEffect_PSBasicVertexLightingTx,      sizeof(BasicEffect_PSBasicVertexLightingTx)      },
    { BasicEffect_PSBasicVertexLightingTxNoFog, sizeof(BasicEffect_PSBasicVertexLightingTxNoFog) },

    { BasicEffect_PSBasicPixelLighting,         sizeof(BasicEffect_PSBasicPixelLighting)         },
    { BasicEffect_PSBasicPixelLightingTx,       sizeof(BasicEffect_PSBasicPixelLightingTx)       },
};


template<>
const int32_t EffectBase<BasicEffectTraits>::PixelShaderIndices[] =
{
    0,      // basic
    1,      // no fog
    0,      // vertex color
    1,      // vertex color, no fog
    2,      // texture
    3,      // texture, no fog
    2,      // texture + vertex color
    3,      // texture + vertex color, no fog

    4,      // vertex lighting
    5,      // vertex lighting, no fog
    4,      // vertex lighting + vertex color
    5,      // vertex lighting + vertex color, no fog
    6,      // vertex lighting + texture
    7,      // vertex lighting + texture, no fog
    6,      // vertex lighting + texture + vertex color
    7,      // vertex lighting + texture + vertex color, no fog

    8,      // pixel lighting
    8,      // pixel lighting, no fog
    8,      // pixel lighting + vertex color
    8,      // pixel lighting + vertex color, no fog
    9,      // pixel lighting + texture
    9,      // pixel lighting + texture, no fog
    9,      // pixel lighting + texture + vertex color
    9,      // pixel lighting + texture + vertex color, no fog

    4,      // vertex lighting (biased vertex normals)
    5,      // vertex lighting (biased vertex normals), no fog
    4,      // vertex lighting (biased vertex normals) + vertex color
    5,      // vertex lighting (biased vertex normals) + vertex color, no fog
    6,      // vertex lighting (biased vertex normals) + texture
    7,      // vertex lighting (biased vertex normals) + texture, no fog
    6,      // vertex lighting (biased vertex normals) + texture + vertex color
    7,      // vertex lighting (biased vertex normals) + texture + vertex color, no fog

    8,      // pixel lighting (biased vertex normals)
    8,      // pixel lighting (biased vertex normals), no fog
    8,      // pixel lighting (biased vertex normals) + vertex color
    8,      // pixel lighting (biased vertex normals) + vertex color, no fog
    9,      // pixel lighting (biased vertex normals) + texture
    9,      // pixel lighting (biased vertex normals) + texture, no fog
    9,      // pixel lighting (biased vertex normals) + texture + vertex color
    9,      // pixel lighting (biased vertex normals) + texture + vertex color, no fog
};

// Global pool of per-device BasicEffect resources.
template<>
SharedResourcePool<ID3D12Device*, EffectBase<BasicEffectTraits>::DeviceResources> EffectBase<BasicEffectTraits>::deviceResourcesPool = {};


// Constructor.
BasicEffect::Impl::Impl(
    _In_ ID3D12Device* device,
    uint32_t effectFlags,
    const EffectPipelineStateDescription& pipelineDescription)
    : EffectBase(device),
        texture{},
        sampler{}
{
    static_assert(_countof(EffectBase<BasicEffectTraits>::VertexShaderIndices) == BasicEffectTraits::ShaderPermutationCount, "array/max mismatch");
    static_assert(_countof(EffectBase<BasicEffectTraits>::VertexShaderBytecode) == BasicEffectTraits::VertexShaderCount, "array/max mismatch");
    static_assert(_countof(EffectBase<BasicEffectTraits>::PixelShaderBytecode) == BasicEffectTraits::PixelShaderCount, "array/max mismatch");
    static_assert(_countof(EffectBase<BasicEffectTraits>::PixelShaderIndices) == BasicEffectTraits::ShaderPermutationCount, "array/max mismatch");

    lights.InitializeConstants(constants.specularColorAndPower, constants.lightDirection, constants.lightDiffuseColor, constants.lightSpecularColor);

    fog.enabled = (effectFlags & EffectFlags::Fog) != 0;
    lightingEnabled = (effectFlags & EffectFlags::Lighting) != 0;
    textureEnabled = (effectFlags & EffectFlags::Texture) != 0;

    // Create root signature.
    {
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

        // Create root parameters and initialize first (constants)
        CD3DX12_ROOT_PARAMETER rootParameters[RootParameterIndex::RootParameterCount] = {};
        rootParameters[RootParameterIndex::ConstantBuffer].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

        // Root parameter descriptor - conditionally initialized
        CD3DX12_ROOT_SIGNATURE_DESC rsigDesc = {};

        if (textureEnabled)
        {
            // Include texture and srv
            CD3DX12_DESCRIPTOR_RANGE textureSRV(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
            CD3DX12_DESCRIPTOR_RANGE textureSampler(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

            rootParameters[RootParameterIndex::TextureSRV].InitAsDescriptorTable(1, &textureSRV, D3D12_SHADER_VISIBILITY_PIXEL);
            rootParameters[RootParameterIndex::TextureSampler].InitAsDescriptorTable(1, &textureSampler, D3D12_SHADER_VISIBILITY_PIXEL);

            // use all parameters
            rsigDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

            m_prootsignature = GetRootSignature(1, rsigDesc);
        }
        else
        {
            // only use constant
            rsigDesc.Init(1, rootParameters, 0, nullptr, rootSignatureFlags);

            m_prootsignature = GetRootSignature(0, rsigDesc);
        }
    }

    assert(m_prootsignature != nullptr);

    // Create pipeline state.
    int32_t sp = GetPipelineStatePermutation(
        (effectFlags & EffectFlags::PerPixelLightingBit) != 0,
        (effectFlags & EffectFlags::VertexColor) != 0,
        (effectFlags & EffectFlags::BiasedVertexNormals) != 0);
    assert(sp >= 0 && sp < BasicEffectTraits::ShaderPermutationCount);
    _Analysis_assume_(sp >= 0 && sp < BasicEffectTraits::ShaderPermutationCount);

    int32_t vi = EffectBase<BasicEffectTraits>::VertexShaderIndices[sp];
    assert(vi >= 0 && vi < BasicEffectTraits::VertexShaderCount);
    _Analysis_assume_(vi >= 0 && vi < BasicEffectTraits::VertexShaderCount);
    int32_t pi = EffectBase<BasicEffectTraits>::PixelShaderIndices[sp];
    assert(pi >= 0 && pi < BasicEffectTraits::PixelShaderCount);
    _Analysis_assume_(pi >= 0 && pi < BasicEffectTraits::PixelShaderCount);

    pipelineDescription.CreatePipelineState(
        device,
        m_prootsignature,
        EffectBase<BasicEffectTraits>::VertexShaderBytecode[vi],
        EffectBase<BasicEffectTraits>::PixelShaderBytecode[pi],
        m_ppipelinestate.addressof());

    SetDebugObjectName(m_ppipelinestate.get(), L"BasicEffect");
}


int32_t BasicEffect::Impl::GetPipelineStatePermutation(bool preferPerPixelLighting, bool vertexColorEnabled, bool biasedVertexNormals) const noexcept
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

    // Support texturing?
    if (textureEnabled)
    {
        permutation += 4;
    }

    if (lightingEnabled)
    {
        if (preferPerPixelLighting)
        {
            // Do lighting in the pixel shader.
            permutation += 16;
        }
        else
        {
            permutation += 8;
        }

        if (biasedVertexNormals)
        {
            // Compressed normals need to be scaled and biased in the vertex shader.
            permutation += 16;
        }
    }

    return permutation;
}


// Sets our state onto the D3D device.
void BasicEffect::Impl::Apply(_In_ ID3D12GraphicsCommandList* commandList)
{
    // Compute derived parameter values.
    matrices.SetConstants(dirtyFlags, constants.worldViewProj);
    fog.SetConstants(dirtyFlags, matrices.worldView, constants.fogVector);
    lights.SetConstants(dirtyFlags, matrices, constants.world, constants.worldInverseTranspose, constants.eyePosition, constants.diffuseColor, constants.emissiveColor, lightingEnabled);

    UpdateConstants();

    // Set the root signature
    commandList->SetGraphicsRootSignature(m_prootsignature);

    // Set the texture
    if (textureEnabled)
    {
        if (!texture.ptr || !sampler.ptr)
        {
            DebugTrace("ERROR: Missing texture or sampler for BasicEffect (texture %llu, sampler %llu)\n", texture.ptr, sampler.ptr);
            throw std::exception("BasicEffect");
        }

        // **NOTE** If D3D asserts or crashes here, you probably need to call commandList->SetDescriptorHeaps() with the required descriptor heaps.
        commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSRV, texture);
        commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSampler, sampler);
    }

    // Set constants
    commandList->SetGraphicsRootConstantBufferView(RootParameterIndex::ConstantBuffer, GetConstantBufferGpuAddress());

    // Set the pipeline state
    commandList->SetPipelineState(EffectBase::m_ppipelinestate.get());
}


// Public constructor.
BasicEffect::BasicEffect(
    _In_ ID3D12Device* device,
    uint32_t effectFlags,
    const EffectPipelineStateDescription& pipelineDescription)
  : pimpl(std::make_unique<Impl>(device, effectFlags, pipelineDescription))
{
}


// Move constructor.
BasicEffect::BasicEffect(BasicEffect&& moveFrom) noexcept
  : pimpl(std::move(moveFrom.pimpl))
{
}


// Move assignment.
BasicEffect& BasicEffect::operator= (BasicEffect&& moveFrom) noexcept
{
    pimpl = std::move(moveFrom.pimpl);
    return *this;
}


// Public destructor.
BasicEffect::~BasicEffect()
{
}


// IEffect methods
void BasicEffect::Apply(_In_ ID3D12GraphicsCommandList* commandList)
{
    pimpl->Apply(commandList);
}


// Camera settings
void XM_CALLCONV BasicEffect::SetWorld(DirectX::FXMMATRIX value)
{
    pimpl->matrices.world = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose | EffectDirtyFlags::FogVector;
}


void XM_CALLCONV BasicEffect::SetView(DirectX::FXMMATRIX value)
{
    pimpl->matrices.view = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::EyePosition | EffectDirtyFlags::FogVector;
}


void XM_CALLCONV BasicEffect::SetProjection(DirectX::FXMMATRIX value)
{
    pimpl->matrices.projection = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj;
}


void XM_CALLCONV BasicEffect::SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
{
    pimpl->matrices.world = world;
    pimpl->matrices.view = view;
    pimpl->matrices.projection = projection;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose | EffectDirtyFlags::EyePosition | EffectDirtyFlags::FogVector;
}


// Material settings
void XM_CALLCONV BasicEffect::SetDiffuseColor(DirectX::FXMVECTOR value)
{
    pimpl->lights.diffuseColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void XM_CALLCONV BasicEffect::SetEmissiveColor(DirectX::FXMVECTOR value)
{
    pimpl->lights.emissiveColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void XM_CALLCONV BasicEffect::SetSpecularColor(DirectX::FXMVECTOR value)
{
    // Set xyz to new value, but preserve existing w (specular power).
    pimpl->constants.specularColorAndPower = DirectX::XMVectorSelect(pimpl->constants.specularColorAndPower, value, DirectX::g_XMSelect1110);

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


void BasicEffect::SetSpecularPower(float value)
{
    // Set w to new value, but preserve existing xyz (specular color).
    pimpl->constants.specularColorAndPower = DirectX::XMVectorSetW(pimpl->constants.specularColorAndPower, value);

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


void BasicEffect::DisableSpecular()
{
    // Set specular color to black, power to 1
    // Note: Don't use a power of 0 or the shader will generate strange highlights on non-specular materials

    pimpl->constants.specularColorAndPower = DirectX::g_XMIdentityR3; 

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


void BasicEffect::SetAlpha(float value)
{
    pimpl->lights.alpha = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void XM_CALLCONV BasicEffect::SetColorAndAlpha(DirectX::FXMVECTOR value)
{
    pimpl->lights.diffuseColor = value;
    pimpl->lights.alpha = DirectX::XMVectorGetW(value);

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


// Light settings
void XM_CALLCONV BasicEffect::SetAmbientLightColor(DirectX::FXMVECTOR value)
{
    pimpl->lights.ambientLightColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void BasicEffect::SetLightEnabled(int32_t whichLight, bool value)
{
    pimpl->dirtyFlags |= pimpl->lights.SetLightEnabled(whichLight, value, pimpl->constants.lightDiffuseColor, pimpl->constants.lightSpecularColor);
}


void XM_CALLCONV BasicEffect::SetLightDirection(int32_t whichLight, DirectX::FXMVECTOR value)
{
    EffectLights::ValidateLightIndex(whichLight);

    pimpl->constants.lightDirection[whichLight] = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


void XM_CALLCONV BasicEffect::SetLightDiffuseColor(int32_t whichLight, DirectX::FXMVECTOR value)
{
    pimpl->dirtyFlags |= pimpl->lights.SetLightDiffuseColor(whichLight, value, pimpl->constants.lightDiffuseColor);
}


void XM_CALLCONV BasicEffect::SetLightSpecularColor(int32_t whichLight, DirectX::FXMVECTOR value)
{
    pimpl->dirtyFlags |= pimpl->lights.SetLightSpecularColor(whichLight, value, pimpl->constants.lightSpecularColor);
}


void BasicEffect::EnableDefaultLighting()
{
    EffectLights::EnableDefaultLighting(this);
}


// Fog settings.
void BasicEffect::SetFogStart(float value)
{
    pimpl->fog.start = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::FogVector;
}


void BasicEffect::SetFogEnd(float value)
{
    pimpl->fog.end = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::FogVector;
}


void XM_CALLCONV BasicEffect::SetFogColor(DirectX::FXMVECTOR value)
{
    pimpl->constants.fogColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


// Texture settings.
void BasicEffect::SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor)
{
    pimpl->texture = srvDescriptor;
    pimpl->sampler = samplerDescriptor;
}
