//--------------------------------------------------------------------------------------
// File: EnvironmentMapEffect.cpp
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
    struct EnvironmentMapEffectConstants
    {
        DirectX::XMVECTOR environmentMapSpecular;
        float environmentMapAmount;
        float fresnelFactor;
        float pad[2];

        DirectX::XMVECTOR diffuseColor;
        DirectX::XMVECTOR emissiveColor;

        DirectX::XMVECTOR lightDirection[IEffectLights::MaxDirectionalLights];
        DirectX::XMVECTOR lightDiffuseColor[IEffectLights::MaxDirectionalLights];

        DirectX::XMVECTOR eyePosition;

        DirectX::XMVECTOR fogColor;
        DirectX::XMVECTOR fogVector;

        DirectX::XMMATRIX world;
        DirectX::XMVECTOR worldInverseTranspose[3];
        DirectX::XMMATRIX worldViewProj;
    };

    static_assert((sizeof(EnvironmentMapEffectConstants) % 16) == 0, "CB size not padded correctly");


    // Traits type describes our characteristics to the EffectBase template.
    struct EnvironmentMapEffectTraits
    {
        using ConstantBufferType = EnvironmentMapEffectConstants;

        static constexpr int32_t VertexShaderCount = 6;
        static constexpr int32_t PixelShaderCount = 16;
        static constexpr int32_t ShaderPermutationCount = 40;
        static constexpr int32_t RootSignatureCount = 1;
    };
}

// Internal EnvironmentMapEffect implementation class.
class EnvironmentMapEffect::Impl : public EffectBase<EnvironmentMapEffectTraits>
{
public:
    Impl(_In_ ID3D12Device* device,
         uint32_t effectFlags,
         const EffectPipelineStateDescription& pipelineDescription,
         EnvironmentMapEffect::Mapping mapping);

    enum RootParameterIndex
    {
        TextureSRV,
        TextureSampler,
        CubemapSRV,
        CubemapSampler,
        ConstantBuffer,
        RootParameterCount
    };

    EffectLights lights;

    D3D12_GPU_DESCRIPTOR_HANDLE texture;
    D3D12_GPU_DESCRIPTOR_HANDLE textureSampler;
    D3D12_GPU_DESCRIPTOR_HANDLE environmentMap;
    D3D12_GPU_DESCRIPTOR_HANDLE environmentMapSampler;

    int32_t GetPipelineStatePermutation(EnvironmentMapEffect::Mapping mapping, uint32_t effectFlags) const noexcept;

    void Apply(_In_ ID3D12GraphicsCommandList* commandList);
};


// Include the precompiled shader code.
namespace
{
#if defined(_XBOX_ONE) && defined(_TITLE)
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_VSEnvMap.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_VSEnvMapFresnel.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_VSEnvMapPixelLighting.inc"

    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_VSEnvMapBn.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_VSEnvMapFresnelBn.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_VSEnvMapPixelLightingBn.inc"

    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMap.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapNoFog.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapSpecular.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapSpecularNoFog.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapPixelLighting.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapPixelLightingNoFog.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapPixelLightingFresnel.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapPixelLightingFresnelNoFog.inc"

    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapSpherePixelLighting.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapSpherePixelLightingNoFog.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnel.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnelNoFog.inc"

    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapDualParabolaPixelLighting.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingNoFog.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnel.inc"
    #include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnelNoFog.inc"
#else
    #include "Shaders/Compiled/EnvironmentMapEffect_VSEnvMap.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_VSEnvMapFresnel.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_VSEnvMapPixelLighting.inc"

    #include "Shaders/Compiled/EnvironmentMapEffect_VSEnvMapBn.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_VSEnvMapFresnelBn.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_VSEnvMapPixelLightingBn.inc"

    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMap.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapNoFog.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapSpecular.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapSpecularNoFog.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapPixelLighting.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapPixelLightingNoFog.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapPixelLightingFresnel.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapPixelLightingFresnelNoFog.inc"

    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapSpherePixelLighting.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapSpherePixelLightingNoFog.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnel.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnelNoFog.inc"

    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLighting.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingNoFog.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnel.inc"
    #include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnelNoFog.inc"
#endif
}


template<>
const D3D12_SHADER_BYTECODE EffectBase<EnvironmentMapEffectTraits>::VertexShaderBytecode[] =
{
    { EnvironmentMapEffect_VSEnvMap,                sizeof(EnvironmentMapEffect_VSEnvMap)                },
    { EnvironmentMapEffect_VSEnvMapFresnel,         sizeof(EnvironmentMapEffect_VSEnvMapFresnel)         },
    { EnvironmentMapEffect_VSEnvMapPixelLighting,   sizeof(EnvironmentMapEffect_VSEnvMapPixelLighting)   },

    { EnvironmentMapEffect_VSEnvMapBn,              sizeof(EnvironmentMapEffect_VSEnvMapBn)              },
    { EnvironmentMapEffect_VSEnvMapFresnelBn,       sizeof(EnvironmentMapEffect_VSEnvMapFresnelBn)       },
    { EnvironmentMapEffect_VSEnvMapPixelLightingBn, sizeof(EnvironmentMapEffect_VSEnvMapPixelLightingBn) },
};


template<>
const int32_t EffectBase<EnvironmentMapEffectTraits>::VertexShaderIndices[] =
{
    0,      // basic
    0,      // basic, no fog
    1,      // fresnel
    1,      // fresnel, no fog
    0,      // specular
    0,      // specular, no fog
    1,      // fresnel + specular
    1,      // fresnel + specular, no fog

    2,      // pixel lighting
    2,      // pixel lighting, no fog
    2,      // pixel lighting, fresnel
    2,      // pixel lighting, fresnel, no fog

    3,      // basic (biased vertex normals)
    3,      // basic (biased vertex normals), no fog
    4,      // fresnel (biased vertex normals)
    4,      // fresnel (biased vertex normals), no fog
    3,      // specular (biased vertex normals)
    3,      // specular (biased vertex normals), no fog
    4,      // fresnel + specular (biased vertex normals)
    4,      // fresnel + specular (biased vertex normals), no fog

    5,      // pixel lighting (biased vertex normals)
    5,      // pixel lighting (biased vertex normals), no fog
    5,      // pixel lighting (biased vertex normals), fresnel
    5,      // pixel lighting (biased vertex normals), fresnel, no fog

    2,      // spheremap pixel lighting
    2,      // spheremap pixel lighting, no fog
    2,      // spheremap pixel lighting, fresnel
    2,      // spheremap pixel lighting, fresnel, no fog

    5,      // spheremap pixel lighting (biased vertex normals)
    5,      // spheremap pixel lighting (biased vertex normals), no fog
    5,      // spheremap pixel lighting (biased vertex normals), fresnel
    5,      // spheremap pixel lighting (biased vertex normals), fresnel, no fog

    2,      // dual-parabola pixel lighting
    2,      // dual-parabola pixel lighting, no fog
    2,      // dual-parabola pixel lighting, fresnel
    2,      // dual-parabola pixel lighting, fresnel, no fog

    5,      // dual-parabola pixel lighting (biased vertex normals)
    5,      // dual-parabola pixel lighting (biased vertex normals), no fog
    5,      // dual-parabola pixel lighting (biased vertex normals), fresnel
    5,      // dual-parabola pixel lighting (biased vertex normals), fresnel, no fog
};


template<>
const D3D12_SHADER_BYTECODE EffectBase<EnvironmentMapEffectTraits>::PixelShaderBytecode[] =
{
    { EnvironmentMapEffect_PSEnvMap,                          sizeof(EnvironmentMapEffect_PSEnvMap)                          },
    { EnvironmentMapEffect_PSEnvMapNoFog,                     sizeof(EnvironmentMapEffect_PSEnvMapNoFog)                     },
    { EnvironmentMapEffect_PSEnvMapSpecular,                  sizeof(EnvironmentMapEffect_PSEnvMapSpecular)                  },
    { EnvironmentMapEffect_PSEnvMapSpecularNoFog,             sizeof(EnvironmentMapEffect_PSEnvMapSpecularNoFog)             },
    { EnvironmentMapEffect_PSEnvMapPixelLighting,             sizeof(EnvironmentMapEffect_PSEnvMapPixelLighting)             },
    { EnvironmentMapEffect_PSEnvMapPixelLightingNoFog,        sizeof(EnvironmentMapEffect_PSEnvMapPixelLightingNoFog)        },
    { EnvironmentMapEffect_PSEnvMapPixelLightingFresnel,      sizeof(EnvironmentMapEffect_PSEnvMapPixelLightingFresnel)      },
    { EnvironmentMapEffect_PSEnvMapPixelLightingFresnelNoFog, sizeof(EnvironmentMapEffect_PSEnvMapPixelLightingFresnelNoFog) },

    { EnvironmentMapEffect_PSEnvMapSpherePixelLighting,             sizeof(EnvironmentMapEffect_PSEnvMapSpherePixelLighting) },
    { EnvironmentMapEffect_PSEnvMapSpherePixelLightingNoFog,        sizeof(EnvironmentMapEffect_PSEnvMapSpherePixelLightingNoFog) },
    { EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnel,      sizeof(EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnel) },
    { EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnelNoFog, sizeof(EnvironmentMapEffect_PSEnvMapSpherePixelLightingFresnelNoFog) },

    { EnvironmentMapEffect_PSEnvMapDualParabolaPixelLighting,             sizeof(EnvironmentMapEffect_PSEnvMapDualParabolaPixelLighting) },
    { EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingNoFog,        sizeof(EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingNoFog) },
    { EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnel,      sizeof(EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnel) },
    { EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnelNoFog, sizeof(EnvironmentMapEffect_PSEnvMapDualParabolaPixelLightingFresnelNoFog) },
};


template<>
const int32_t EffectBase<EnvironmentMapEffectTraits>::PixelShaderIndices[] =
{
    0,      // basic
    1,      // basic, no fog
    0,      // fresnel
    1,      // fresnel, no fog
    2,      // specular
    3,      // specular, no fog
    2,      // fresnel + specular
    3,      // fresnel + specular, no fog

    4,      // per pixel lighting
    5,      // per pixel lighting, no fog
    6,      // per pixel lighting, fresnel
    7,      // per pixel lighting, fresnel, no fog

    0,      // basic (biased vertex normals)
    1,      // basic (biased vertex normals), no fog
    0,      // fresnel (biased vertex normals)
    1,      // fresnel (biased vertex normals), no fog
    2,      // specular (biased vertex normals)
    3,      // specular (biased vertex normals), no fog
    2,      // fresnel + specular (biased vertex normals)
    3,      // fresnel + specular (biased vertex normals), no fog

    4,      // per pixel lighting (biased vertex normals)
    5,      // per pixel lighting (biased vertex normals), no fog
    6,      // per pixel lighting (biased vertex normals), fresnel
    7,      // per pixel lighting (biased vertex normals), fresnel, no fog

    8,      // spheremap pixel lighting
    9,      // spheremap pixel lighting, no fog
    10,     // spheremap pixel lighting, fresnel
    11,     // spheremap pixel lighting, fresnel, no fog

    8,      // spheremap pixel lighting (biased vertex normals)
    9,      // spheremap pixel lighting (biased vertex normals), no fog
    10,     // spheremap pixel lighting (biased vertex normals), fresnel
    11,     // spheremap pixel lighting (biased vertex normals), fresnel, no fog

    12,     // dual-parabola pixel lighting
    13,     // dual-parabola pixel lighting, no fog
    14,     // dual-parabola pixel lighting, fresnel
    15,     // dual-parabola pixel lighting, fresnel, no fog

    12,     // dual-parabola pixel lighting (biased vertex normals)
    13,     // dual-parabola pixel lighting (biased vertex normals), no fog
    14,     // dual-parabola pixel lighting (biased vertex normals), fresnel
    15,     // dual-parabola pixel lighting (biased vertex normals), fresnel, no fog
};


// Global pool of per-device EnvironmentMapEffect resources.
template<>
SharedResourcePool<ID3D12Device*, EffectBase<EnvironmentMapEffectTraits>::DeviceResources> EffectBase<EnvironmentMapEffectTraits>::deviceResourcesPool = {};


// Constructor.
EnvironmentMapEffect::Impl::Impl(
    _In_ ID3D12Device* device,
    uint32_t effectFlags,
    const EffectPipelineStateDescription& pipelineDescription,
    EnvironmentMapEffect::Mapping mapping)
    : EffectBase(device),
        texture{},
        textureSampler{},
        environmentMap{},
        environmentMapSampler{}
{
    static_assert(_countof(EffectBase<EnvironmentMapEffectTraits>::VertexShaderIndices) == EnvironmentMapEffectTraits::ShaderPermutationCount, "array/max mismatch");
    static_assert(_countof(EffectBase<EnvironmentMapEffectTraits>::VertexShaderBytecode) == EnvironmentMapEffectTraits::VertexShaderCount, "array/max mismatch");
    static_assert(_countof(EffectBase<EnvironmentMapEffectTraits>::PixelShaderBytecode) == EnvironmentMapEffectTraits::PixelShaderCount, "array/max mismatch");
    static_assert(_countof(EffectBase<EnvironmentMapEffectTraits>::PixelShaderIndices) == EnvironmentMapEffectTraits::ShaderPermutationCount, "array/max mismatch");

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
        CD3DX12_DESCRIPTOR_RANGE textureDescriptor(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        CD3DX12_DESCRIPTOR_RANGE textureSamplerDescriptor(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);
        rootParameters[RootParameterIndex::TextureSRV].InitAsDescriptorTable(1, &textureDescriptor, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameterIndex::TextureSampler].InitAsDescriptorTable(1, &textureSamplerDescriptor, D3D12_SHADER_VISIBILITY_PIXEL);

        // Texture 2
        CD3DX12_DESCRIPTOR_RANGE cubemapDescriptor(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
        CD3DX12_DESCRIPTOR_RANGE cubemapSamplerDescriptor(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 1);
        rootParameters[RootParameterIndex::CubemapSRV].InitAsDescriptorTable(1, &cubemapDescriptor, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameterIndex::CubemapSampler].InitAsDescriptorTable(1, &cubemapSamplerDescriptor, D3D12_SHADER_VISIBILITY_PIXEL);

        // Create the root signature
        CD3DX12_ROOT_SIGNATURE_DESC rsigDesc = {};
        rsigDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

        m_prootsignature = GetRootSignature(0, rsigDesc);
    }

    assert(m_prootsignature != nullptr);

    fog.enabled = (effectFlags & EffectFlags::Fog) != 0;

    if (effectFlags & EffectFlags::VertexColor)
    {
        DebugTrace("ERROR: EnvironmentMapEffect does not implement EffectFlags::VertexColor\n");
        throw std::invalid_argument("EnvironmentMapEffect");
    }

    constants.environmentMapAmount = 1;
    constants.fresnelFactor = 1;

    DirectX::XMVECTOR unwantedOutput[MaxDirectionalLights];

    lights.InitializeConstants(unwantedOutput[0], constants.lightDirection, constants.lightDiffuseColor, unwantedOutput);

    // Create pipeline state.
    int32_t sp = GetPipelineStatePermutation(mapping, effectFlags);

    assert(sp >= 0 && sp < EnvironmentMapEffectTraits::ShaderPermutationCount);
    _Analysis_assume_(sp >= 0 && sp < EnvironmentMapEffectTraits::ShaderPermutationCount);
    int32_t vi = EffectBase<EnvironmentMapEffectTraits>::VertexShaderIndices[sp];
    assert(vi >= 0 && vi < EnvironmentMapEffectTraits::VertexShaderCount);
    _Analysis_assume_(vi >= 0 && vi < EnvironmentMapEffectTraits::VertexShaderCount);
    int32_t pi = EffectBase<EnvironmentMapEffectTraits>::PixelShaderIndices[sp];
    assert(pi >= 0 && pi < EnvironmentMapEffectTraits::PixelShaderCount);
    _Analysis_assume_(pi >= 0 && pi < EnvironmentMapEffectTraits::PixelShaderCount);

    pipelineDescription.CreatePipelineState(
        device,
        m_prootsignature,
        EffectBase<EnvironmentMapEffectTraits>::VertexShaderBytecode[vi],
        EffectBase<EnvironmentMapEffectTraits>::PixelShaderBytecode[pi],
        m_ppipelinestate.addressof());

    SetDebugObjectName(m_ppipelinestate.get(), L"EnvironmentMapEffect");
}


int32_t EnvironmentMapEffect::Impl::GetPipelineStatePermutation(
    EnvironmentMapEffect::Mapping mapping,
    uint32_t effectFlags) const noexcept
{
    bool biasedVertexNormals = (effectFlags & EffectFlags::BiasedVertexNormals) != 0;

    int32_t permutation = 0;

    // Use optimized shaders if fog is disabled.
    if (!fog.enabled)
    {
        permutation += 1;
    }

    // Support fresnel?
    if (effectFlags & EffectFlags::Fresnel)
    {
        permutation += 2;
    }

    if (mapping == Mapping_Sphere)
    {
        permutation += 24;

        if (biasedVertexNormals)
        {
            permutation += 4;
        }
    }
    else if (mapping == Mapping_DualParabola)
    {
        permutation += 32;

        if (biasedVertexNormals)
        {
            permutation += 4;
        }
    }
    else // Mapping_Cube
    {
        if (effectFlags & EffectFlags::PerPixelLightingBit)
        {
            permutation += 8;
        }
        else
        {
            if (effectFlags & EffectFlags::Specular)
            {
                permutation += 4;
            }
        }

        if (biasedVertexNormals)
        {
            // Compressed normals need to be scaled and biased in the vertex shader.
            permutation += 12;
        }
    }

    return permutation;
}


// Sets our state onto the D3D device.
void EnvironmentMapEffect::Impl::Apply(_In_ ID3D12GraphicsCommandList* commandList)
{
    // Compute derived parameter values.
    matrices.SetConstants(dirtyFlags, constants.worldViewProj);

    fog.SetConstants(dirtyFlags, matrices.worldView, constants.fogVector);
            
    lights.SetConstants(dirtyFlags, matrices, constants.world, constants.worldInverseTranspose, constants.eyePosition, constants.diffuseColor, constants.emissiveColor, true);

    UpdateConstants();

    // Set the resources and state
    commandList->SetGraphicsRootSignature(m_prootsignature);

    // Set the textures
    if (!texture.ptr || !environmentMap.ptr)
    {
        DebugTrace("ERROR: Missing texture(s) for EnvironmentMapEffect (texture %llu, environmentMap %llu)\n", texture.ptr, environmentMap.ptr);
        throw std::exception("EnvironmentMapEffect");
    }
    if (!textureSampler.ptr || !environmentMapSampler.ptr)
    {
        DebugTrace("ERROR: Missing sampler(s) for EnvironmentMapEffect (sampler %llu, environmentMap %llu)\n", textureSampler.ptr, environmentMapSampler.ptr);
        throw std::exception("EnvironmentMapEffect");
    }

    // **NOTE** If D3D asserts or crashes here, you probably need to call commandList->SetDescriptorHeaps() with the required descriptor heaps.
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSRV, texture);
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSampler, textureSampler);
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::CubemapSRV, environmentMap);
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::CubemapSampler, environmentMapSampler);

    // Set constants
    commandList->SetGraphicsRootConstantBufferView(RootParameterIndex::ConstantBuffer, GetConstantBufferGpuAddress());

    // Set the pipeline state
    commandList->SetPipelineState(EffectBase::m_ppipelinestate.get());
}


// Public constructor.
EnvironmentMapEffect::EnvironmentMapEffect(
    _In_ ID3D12Device* device,
    uint32_t effectFlags,
    const EffectPipelineStateDescription& pipelineDescription,
    EnvironmentMapEffect::Mapping mapping)
    : pimpl(std::make_unique<Impl>(device, effectFlags, pipelineDescription, mapping))
{
}


// Move constructor.
EnvironmentMapEffect::EnvironmentMapEffect(EnvironmentMapEffect&& moveFrom) noexcept
    : pimpl(std::move(moveFrom.pimpl))
{
}


// Move assignment.
EnvironmentMapEffect& EnvironmentMapEffect::operator= (EnvironmentMapEffect&& moveFrom) noexcept
{
    pimpl = std::move(moveFrom.pimpl);
    return *this;
}


// Public destructor.
EnvironmentMapEffect::~EnvironmentMapEffect()
{
}


// IEffect methods.
void EnvironmentMapEffect::Apply(
    _In_ ID3D12GraphicsCommandList* cmdList)
{
    pimpl->Apply(cmdList);
}


// Camera settings.
void XM_CALLCONV EnvironmentMapEffect::SetWorld(DirectX::FXMMATRIX value)
{
    pimpl->matrices.world = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose | EffectDirtyFlags::FogVector;
}


void XM_CALLCONV EnvironmentMapEffect::SetView(DirectX::FXMMATRIX value)
{
    pimpl->matrices.view = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::EyePosition | EffectDirtyFlags::FogVector;
}


void XM_CALLCONV EnvironmentMapEffect::SetProjection(DirectX::FXMMATRIX value)
{
    pimpl->matrices.projection = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj;
}


void XM_CALLCONV EnvironmentMapEffect::SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
{
    pimpl->matrices.world = world;
    pimpl->matrices.view = view;
    pimpl->matrices.projection = projection;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose | EffectDirtyFlags::EyePosition | EffectDirtyFlags::FogVector;
}


// Material settings.
void XM_CALLCONV EnvironmentMapEffect::SetDiffuseColor(DirectX::FXMVECTOR value)
{
    pimpl->lights.diffuseColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void XM_CALLCONV EnvironmentMapEffect::SetEmissiveColor(DirectX::FXMVECTOR value)
{
    pimpl->lights.emissiveColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void EnvironmentMapEffect::SetAlpha(float value)
{
    pimpl->lights.alpha = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void XM_CALLCONV EnvironmentMapEffect::SetColorAndAlpha(DirectX::FXMVECTOR value)
{
    pimpl->lights.diffuseColor = value;
    pimpl->lights.alpha = DirectX::XMVectorGetW(value);

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


// Light settings.
void XM_CALLCONV EnvironmentMapEffect::SetAmbientLightColor(DirectX::FXMVECTOR value)
{
    pimpl->lights.ambientLightColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void EnvironmentMapEffect::SetLightEnabled(int32_t whichLight, bool value)
{
    DirectX::XMVECTOR unwantedOutput[MaxDirectionalLights] = {};

    pimpl->dirtyFlags |= pimpl->lights.SetLightEnabled(whichLight, value, pimpl->constants.lightDiffuseColor, unwantedOutput);
}


void XM_CALLCONV EnvironmentMapEffect::SetLightDirection(int32_t whichLight, DirectX::FXMVECTOR value)
{
    EffectLights::ValidateLightIndex(whichLight);

    pimpl->constants.lightDirection[whichLight] = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


void XM_CALLCONV EnvironmentMapEffect::SetLightDiffuseColor(int32_t whichLight, DirectX::FXMVECTOR value)
{
    pimpl->dirtyFlags |= pimpl->lights.SetLightDiffuseColor(whichLight, value, pimpl->constants.lightDiffuseColor);
}


void XM_CALLCONV EnvironmentMapEffect::SetLightSpecularColor(int32_t, DirectX::FXMVECTOR)
{
    // Unsupported interface method.
}


void EnvironmentMapEffect::EnableDefaultLighting()
{
    EffectLights::EnableDefaultLighting(this);
}


// Fog settings.
void EnvironmentMapEffect::SetFogStart(float value)
{
    pimpl->fog.start = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::FogVector;
}


void EnvironmentMapEffect::SetFogEnd(float value)
{
    pimpl->fog.end = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::FogVector;
}


void XM_CALLCONV EnvironmentMapEffect::SetFogColor(DirectX::FXMVECTOR value)
{
    pimpl->constants.fogColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


// Texture settings.
void EnvironmentMapEffect::SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE texture, D3D12_GPU_DESCRIPTOR_HANDLE sampler)
{
    pimpl->texture = texture;
    pimpl->textureSampler = sampler;
}


void EnvironmentMapEffect::SetEnvironmentMap(D3D12_GPU_DESCRIPTOR_HANDLE texture, D3D12_GPU_DESCRIPTOR_HANDLE sampler)
{
    pimpl->environmentMap = texture;
    pimpl->environmentMapSampler = sampler;
}


// Additional settings.
void EnvironmentMapEffect::SetEnvironmentMapAmount(float value)
{
    pimpl->constants.environmentMapAmount = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


void XM_CALLCONV EnvironmentMapEffect::SetEnvironmentMapSpecular(DirectX::FXMVECTOR value)
{
    pimpl->constants.environmentMapSpecular = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


void EnvironmentMapEffect::SetFresnelFactor(float value)
{
    pimpl->constants.fresnelFactor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}
