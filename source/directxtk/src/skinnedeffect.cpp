//--------------------------------------------------------------------------------------
// File: SkinnedEffect.cpp
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
    struct SkinnedEffectConstants
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

        DirectX::XMVECTOR bones[SkinnedEffect::MaxBones][3];
    };

    static_assert((sizeof(SkinnedEffectConstants) % 16) == 0, "CB size not padded correctly");


    // Traits type describes our characteristics to the EffectBase template.
    struct SkinnedEffectTraits
    {
        using ConstantBufferType = SkinnedEffectConstants;

        static constexpr int32_t VertexShaderCount = 12;
        static constexpr int32_t PixelShaderCount = 3;
        static constexpr int32_t ShaderPermutationCount = 24;
        static constexpr int32_t RootSignatureCount = 1;
    };
}

// Internal SkinnedEffect implementation class.
class SkinnedEffect::Impl : public EffectBase<SkinnedEffectTraits>
{
public:
    Impl(_In_ ID3D12Device* device, uint32_t effectFlags, const EffectPipelineStateDescription& pipelineDescription,
        int32_t weightsPerVertex);

    enum RootParameterIndex
    {
        ConstantBuffer,
        TextureSRV,
        TextureSampler,
        RootParameterCount
    };

    D3D12_GPU_DESCRIPTOR_HANDLE texture;
    D3D12_GPU_DESCRIPTOR_HANDLE sampler;

    EffectLights lights;

    int32_t GetPipelineStatePermutation(bool preferPerPixelLighting, int32_t weightsPerVertex, bool biasedVertexNormals) const noexcept;

    void Apply(_In_ ID3D12GraphicsCommandList* commandList);
};


// Include the precompiled shader code.
namespace
{
#if defined(_XBOX_ONE) && defined(_TITLE)
    #include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedVertexLightingOneBone.inc"
    #include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedVertexLightingTwoBones.inc"
    #include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedVertexLightingFourBones.inc"

    #include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedPixelLightingOneBone.inc"
    #include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedPixelLightingTwoBones.inc"
    #include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedPixelLightingFourBones.inc"

    #include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedVertexLightingOneBoneBn.inc"
    #include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedVertexLightingTwoBonesBn.inc"
    #include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedVertexLightingFourBonesBn.inc"

    #include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedPixelLightingOneBoneBn.inc"
    #include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedPixelLightingTwoBonesBn.inc"
    #include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedPixelLightingFourBonesBn.inc"

    #include "Shaders/Compiled/XboxOneSkinnedEffect_PSSkinnedVertexLighting.inc"
    #include "Shaders/Compiled/XboxOneSkinnedEffect_PSSkinnedVertexLightingNoFog.inc"
    #include "Shaders/Compiled/XboxOneSkinnedEffect_PSSkinnedPixelLighting.inc"
#else
    #include "Shaders/Compiled/SkinnedEffect_VSSkinnedVertexLightingOneBone.inc"
    #include "Shaders/Compiled/SkinnedEffect_VSSkinnedVertexLightingTwoBones.inc"
    #include "Shaders/Compiled/SkinnedEffect_VSSkinnedVertexLightingFourBones.inc"

    #include "Shaders/Compiled/SkinnedEffect_VSSkinnedPixelLightingOneBone.inc"
    #include "Shaders/Compiled/SkinnedEffect_VSSkinnedPixelLightingTwoBones.inc"
    #include "Shaders/Compiled/SkinnedEffect_VSSkinnedPixelLightingFourBones.inc"

    #include "Shaders/Compiled/SkinnedEffect_VSSkinnedVertexLightingOneBoneBn.inc"
    #include "Shaders/Compiled/SkinnedEffect_VSSkinnedVertexLightingTwoBonesBn.inc"
    #include "Shaders/Compiled/SkinnedEffect_VSSkinnedVertexLightingFourBonesBn.inc"

    #include "Shaders/Compiled/SkinnedEffect_VSSkinnedPixelLightingOneBoneBn.inc"
    #include "Shaders/Compiled/SkinnedEffect_VSSkinnedPixelLightingTwoBonesBn.inc"
    #include "Shaders/Compiled/SkinnedEffect_VSSkinnedPixelLightingFourBonesBn.inc"

    #include "Shaders/Compiled/SkinnedEffect_PSSkinnedVertexLighting.inc"
    #include "Shaders/Compiled/SkinnedEffect_PSSkinnedVertexLightingNoFog.inc"
    #include "Shaders/Compiled/SkinnedEffect_PSSkinnedPixelLighting.inc"
#endif
}


template<>
const D3D12_SHADER_BYTECODE EffectBase<SkinnedEffectTraits>::VertexShaderBytecode[] =
{
    { SkinnedEffect_VSSkinnedVertexLightingOneBone,     sizeof(SkinnedEffect_VSSkinnedVertexLightingOneBone)     },
    { SkinnedEffect_VSSkinnedVertexLightingTwoBones,    sizeof(SkinnedEffect_VSSkinnedVertexLightingTwoBones)    },
    { SkinnedEffect_VSSkinnedVertexLightingFourBones,   sizeof(SkinnedEffect_VSSkinnedVertexLightingFourBones)   },

    { SkinnedEffect_VSSkinnedPixelLightingOneBone,      sizeof(SkinnedEffect_VSSkinnedPixelLightingOneBone)      },
    { SkinnedEffect_VSSkinnedPixelLightingTwoBones,     sizeof(SkinnedEffect_VSSkinnedPixelLightingTwoBones)     },
    { SkinnedEffect_VSSkinnedPixelLightingFourBones,    sizeof(SkinnedEffect_VSSkinnedPixelLightingFourBones)    },

    { SkinnedEffect_VSSkinnedVertexLightingOneBoneBn,   sizeof(SkinnedEffect_VSSkinnedVertexLightingOneBoneBn)   },
    { SkinnedEffect_VSSkinnedVertexLightingTwoBonesBn,  sizeof(SkinnedEffect_VSSkinnedVertexLightingTwoBonesBn)  },
    { SkinnedEffect_VSSkinnedVertexLightingFourBonesBn, sizeof(SkinnedEffect_VSSkinnedVertexLightingFourBonesBn) },

    { SkinnedEffect_VSSkinnedPixelLightingOneBoneBn,    sizeof(SkinnedEffect_VSSkinnedPixelLightingOneBoneBn)    },
    { SkinnedEffect_VSSkinnedPixelLightingTwoBonesBn,   sizeof(SkinnedEffect_VSSkinnedPixelLightingTwoBonesBn)   },
    { SkinnedEffect_VSSkinnedPixelLightingFourBonesBn,  sizeof(SkinnedEffect_VSSkinnedPixelLightingFourBonesBn)  },
};


template<>
const int32_t EffectBase<SkinnedEffectTraits>::VertexShaderIndices[] =
{
    0,      // vertex lighting, one bone
    0,      // vertex lighting, one bone, no fog
    1,      // vertex lighting, two bones
    1,      // vertex lighting, two bones, no fog
    2,      // vertex lighting, four bones
    2,      // vertex lighting, four bones, no fog

    3,      // pixel lighting, one bone
    3,      // pixel lighting, one bone, no fog
    4,      // pixel lighting, two bones
    4,      // pixel lighting, two bones, no fog
    5,      // pixel lighting, four bones
    5,      // pixel lighting, four bones, no fog

    6,      // vertex lighting (biased vertex normals), one bone
    6,      // vertex lighting (biased vertex normals), one bone, no fog
    7,      // vertex lighting (biased vertex normals), two bones
    7,      // vertex lighting (biased vertex normals), two bones, no fog
    8,      // vertex lighting (biased vertex normals), four bones
    8,      // vertex lighting (biased vertex normals), four bones, no fog

    9,      // pixel lighting (biased vertex normals), one bone
    9,      // pixel lighting (biased vertex normals), one bone, no fog
    10,     // pixel lighting (biased vertex normals), two bones
    10,     // pixel lighting (biased vertex normals), two bones, no fog
    11,     // pixel lighting (biased vertex normals), four bones
    11,     // pixel lighting (biased vertex normals), four bones, no fog
};


template<>
const D3D12_SHADER_BYTECODE EffectBase<SkinnedEffectTraits>::PixelShaderBytecode[] =
{
    { SkinnedEffect_PSSkinnedVertexLighting,      sizeof(SkinnedEffect_PSSkinnedVertexLighting)      },
    { SkinnedEffect_PSSkinnedVertexLightingNoFog, sizeof(SkinnedEffect_PSSkinnedVertexLightingNoFog) },
    { SkinnedEffect_PSSkinnedPixelLighting,       sizeof(SkinnedEffect_PSSkinnedPixelLighting)       },
};


template<>
const int32_t EffectBase<SkinnedEffectTraits>::PixelShaderIndices[] =
{
    0,      // vertex lighting, one bone
    1,      // vertex lighting, one bone, no fog
    0,      // vertex lighting, two bones
    1,      // vertex lighting, two bones, no fog
    0,      // vertex lighting, four bones
    1,      // vertex lighting, four bones, no fog

    2,      // pixel lighting, one bone
    2,      // pixel lighting, one bone, no fog
    2,      // pixel lighting, two bones
    2,      // pixel lighting, two bones, no fog
    2,      // pixel lighting, four bones
    2,      // pixel lighting, four bones, no fog

    0,      // vertex lighting (biased vertex normals), one bone
    1,      // vertex lighting (biased vertex normals), one bone, no fog
    0,      // vertex lighting (biased vertex normals), two bones
    1,      // vertex lighting (biased vertex normals), two bones, no fog
    0,      // vertex lighting (biased vertex normals), four bones
    1,      // vertex lighting (biased vertex normals), four bones, no fog

    2,      // pixel lighting (biased vertex normals), one bone
    2,      // pixel lighting (biased vertex normals), one bone, no fog
    2,      // pixel lighting (biased vertex normals), two bones
    2,      // pixel lighting (biased vertex normals), two bones, no fog
    2,      // pixel lighting (biased vertex normals), four bones
    2,      // pixel lighting (biased vertex normals), four bones, no fog
};


// Global pool of per-device SkinnedEffect resources.
template<>
SharedResourcePool<ID3D12Device*, EffectBase<SkinnedEffectTraits>::DeviceResources> EffectBase<SkinnedEffectTraits>::deviceResourcesPool = {};


// Constructor.
SkinnedEffect::Impl::Impl(
    _In_ ID3D12Device* device,
    uint32_t effectFlags,
    const EffectPipelineStateDescription& pipelineDescription,
    int32_t weightsPerVertex)
    : EffectBase(device),
        texture{},
        sampler{}
{
    static_assert(_countof(EffectBase<SkinnedEffectTraits>::VertexShaderIndices) == SkinnedEffectTraits::ShaderPermutationCount, "array/max mismatch");
    static_assert(_countof(EffectBase<SkinnedEffectTraits>::VertexShaderBytecode) == SkinnedEffectTraits::VertexShaderCount, "array/max mismatch");
    static_assert(_countof(EffectBase<SkinnedEffectTraits>::PixelShaderBytecode) == SkinnedEffectTraits::PixelShaderCount, "array/max mismatch");
    static_assert(_countof(EffectBase<SkinnedEffectTraits>::PixelShaderIndices) == SkinnedEffectTraits::ShaderPermutationCount, "array/max mismatch");

    if ((weightsPerVertex != 1) &&
        (weightsPerVertex != 2) &&
        (weightsPerVertex != 4))
    {
        DebugTrace("ERROR: SkinnedEffect's weightsPerVertex parameter must be 1, 2, or 4");
        throw std::out_of_range("weightsPerVertex must be 1, 2, or 4");
    }

    lights.InitializeConstants(constants.specularColorAndPower, constants.lightDirection, constants.lightDiffuseColor, constants.lightSpecularColor);

    for (int32_t i = 0; i < MaxBones; i++)
    {
        constants.bones[i][0] = DirectX::g_XMIdentityR0;
        constants.bones[i][1] = DirectX::g_XMIdentityR1;
        constants.bones[i][2] = DirectX::g_XMIdentityR2;
    }

    // Create root signature.
    {
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

        CD3DX12_DESCRIPTOR_RANGE textureSrvDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        CD3DX12_DESCRIPTOR_RANGE textureSamplerDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

        CD3DX12_ROOT_PARAMETER rootParameters[RootParameterIndex::RootParameterCount] = {};
        rootParameters[RootParameterIndex::TextureSRV].InitAsDescriptorTable(1, &textureSrvDescriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameterIndex::TextureSampler].InitAsDescriptorTable(1, &textureSamplerDescriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameterIndex::ConstantBuffer].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

        CD3DX12_ROOT_SIGNATURE_DESC rsigDesc = {};
        rsigDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

        m_prootsignature = GetRootSignature(0, rsigDesc);
    }

    assert(m_prootsignature != nullptr);

    fog.enabled = (effectFlags & EffectFlags::Fog) != 0;

    if (effectFlags & EffectFlags::VertexColor)
    {
        DebugTrace("ERROR: SkinnedEffect does not implement EffectFlags::VertexColor\n");
        throw std::invalid_argument("SkinnedEffect");
    }

    // Create pipeline state.
    int32_t sp = GetPipelineStatePermutation(
        (effectFlags & EffectFlags::PerPixelLightingBit) != 0,
        weightsPerVertex,
        (effectFlags & EffectFlags::BiasedVertexNormals) != 0);
    assert(sp >= 0 && sp < SkinnedEffectTraits::ShaderPermutationCount);
    _Analysis_assume_(sp >= 0 && sp < SkinnedEffectTraits::ShaderPermutationCount);

    int32_t vi = EffectBase<SkinnedEffectTraits>::VertexShaderIndices[sp];
    assert(vi >= 0 && vi < SkinnedEffectTraits::VertexShaderCount);
    _Analysis_assume_(vi >= 0 && vi < SkinnedEffectTraits::VertexShaderCount);
    int32_t pi = EffectBase<SkinnedEffectTraits>::PixelShaderIndices[sp];
    assert(pi >= 0 && pi < SkinnedEffectTraits::PixelShaderCount);
    _Analysis_assume_(pi >= 0 && pi < SkinnedEffectTraits::PixelShaderCount);

    pipelineDescription.CreatePipelineState(
        device,
        m_prootsignature,
        EffectBase<SkinnedEffectTraits>::VertexShaderBytecode[vi],
        EffectBase<SkinnedEffectTraits>::PixelShaderBytecode[pi],
        m_ppipelinestate.addressof());

    SetDebugObjectName(m_ppipelinestate.get(), L"SkinnedEffect");
}


int32_t SkinnedEffect::Impl::GetPipelineStatePermutation(bool preferPerPixelLighting, int32_t weightsPerVertex, bool biasedVertexNormals) const noexcept
{
    int32_t permutation = 0;

    // Use optimized shaders if fog is disabled.
    if (!fog.enabled)
    {
        permutation += 1;
    }

    // Evaluate 1, 2, or 4 weights per vertex?
    if (weightsPerVertex == 2)
    {
        permutation += 2;
    }
    else if (weightsPerVertex == 4)
    {
        permutation += 4;
    }

    if (preferPerPixelLighting)
    {
        // Do lighting in the pixel shader.
        permutation += 6;
    }

    if (biasedVertexNormals)
    {
        // Compressed normals need to be scaled and biased in the vertex shader.
        permutation += 12;
    }

    return permutation;
}


// Sets our state onto the D3D device.
void SkinnedEffect::Impl::Apply(_In_ ID3D12GraphicsCommandList* commandList)
{
    // Compute derived parameter values.
    matrices.SetConstants(dirtyFlags, constants.worldViewProj);
    fog.SetConstants(dirtyFlags, matrices.worldView, constants.fogVector);
    lights.SetConstants(dirtyFlags, matrices, constants.world, constants.worldInverseTranspose, constants.eyePosition, constants.diffuseColor, constants.emissiveColor, true);

    UpdateConstants();

    // Set the root signature
    commandList->SetGraphicsRootSignature(m_prootsignature);

    // Set the texture
    if (!texture.ptr || !sampler.ptr)
    {
        DebugTrace("ERROR: Missing texture or sampler for SkinnedEffect (texture %llu, sampler %llu)\n", texture.ptr, sampler.ptr);
        throw std::exception("SkinnedEffect");
    }

    // **NOTE** If D3D asserts or crashes here, you probably need to call commandList->SetDescriptorHeaps() with the required descriptor heaps.
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSRV, texture);
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSampler, sampler);
    
    // Set constants
    commandList->SetGraphicsRootConstantBufferView(RootParameterIndex::ConstantBuffer, GetConstantBufferGpuAddress());

    // Set the pipeline state
    commandList->SetPipelineState(EffectBase::m_ppipelinestate.get());
}


// Public constructor.
SkinnedEffect::SkinnedEffect(
    _In_ ID3D12Device* device,
    uint32_t effectFlags,
    const EffectPipelineStateDescription& pipelineDescription,
    int32_t weightsPerVertex)
    : pimpl(std::make_unique<Impl>(device, effectFlags, pipelineDescription, weightsPerVertex))
{
}


// Move constructor.
SkinnedEffect::SkinnedEffect(SkinnedEffect&& moveFrom) noexcept
    : pimpl(std::move(moveFrom.pimpl))
{
}


// Move assignment.
SkinnedEffect& SkinnedEffect::operator= (SkinnedEffect&& moveFrom) noexcept
{
    pimpl = std::move(moveFrom.pimpl);
    return *this;
}


// Public destructor.
SkinnedEffect::~SkinnedEffect()
{
}


// IEffect methods.
void SkinnedEffect::Apply(_In_ ID3D12GraphicsCommandList* commandList)
{
    pimpl->Apply(commandList);
}


// Camera settings.
void XM_CALLCONV SkinnedEffect::SetWorld(DirectX::FXMMATRIX value)
{
    pimpl->matrices.world = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose | EffectDirtyFlags::FogVector;
}


void XM_CALLCONV SkinnedEffect::SetView(DirectX::FXMMATRIX value)
{
    pimpl->matrices.view = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::EyePosition | EffectDirtyFlags::FogVector;
}


void XM_CALLCONV SkinnedEffect::SetProjection(DirectX::FXMMATRIX value)
{
    pimpl->matrices.projection = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj;
}


void XM_CALLCONV SkinnedEffect::SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
{
    pimpl->matrices.world = world;
    pimpl->matrices.view = view;
    pimpl->matrices.projection = projection;

    pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose | EffectDirtyFlags::EyePosition | EffectDirtyFlags::FogVector;
}


// Material settings.
void XM_CALLCONV SkinnedEffect::SetDiffuseColor(DirectX::FXMVECTOR value)
{
    pimpl->lights.diffuseColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void XM_CALLCONV SkinnedEffect::SetEmissiveColor(DirectX::FXMVECTOR value)
{
    pimpl->lights.emissiveColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void XM_CALLCONV SkinnedEffect::SetSpecularColor(DirectX::FXMVECTOR value)
{
    // Set xyz to new value, but preserve existing w (specular power).
    pimpl->constants.specularColorAndPower = DirectX::XMVectorSelect(pimpl->constants.specularColorAndPower, value, DirectX::g_XMSelect1110);

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


void SkinnedEffect::SetSpecularPower(float value)
{
    // Set w to new value, but preserve existing xyz (specular color).
    pimpl->constants.specularColorAndPower = DirectX::XMVectorSetW(pimpl->constants.specularColorAndPower, value);

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


void SkinnedEffect::DisableSpecular()
{
    // Set specular color to black, power to 1
    // Note: Don't use a power of 0 or the shader will generate strange highlights on non-specular materials

    pimpl->constants.specularColorAndPower = DirectX::g_XMIdentityR3; 

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


void SkinnedEffect::SetAlpha(float value)
{
    pimpl->lights.alpha = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void XM_CALLCONV SkinnedEffect::SetColorAndAlpha(DirectX::FXMVECTOR value)
{
    pimpl->lights.diffuseColor = value;
    pimpl->lights.alpha = DirectX::XMVectorGetW(value);

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


// Light settings.
void XM_CALLCONV SkinnedEffect::SetAmbientLightColor(DirectX::FXMVECTOR value)
{
    pimpl->lights.ambientLightColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}


void SkinnedEffect::SetLightEnabled(int32_t whichLight, bool value)
{
    pimpl->dirtyFlags |= pimpl->lights.SetLightEnabled(whichLight, value, pimpl->constants.lightDiffuseColor, pimpl->constants.lightSpecularColor);
}


void XM_CALLCONV SkinnedEffect::SetLightDirection(int32_t whichLight, DirectX::FXMVECTOR value)
{
    EffectLights::ValidateLightIndex(whichLight);

    pimpl->constants.lightDirection[whichLight] = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


void XM_CALLCONV SkinnedEffect::SetLightDiffuseColor(int32_t whichLight, DirectX::FXMVECTOR value)
{
    pimpl->dirtyFlags |= pimpl->lights.SetLightDiffuseColor(whichLight, value, pimpl->constants.lightDiffuseColor);
}


void XM_CALLCONV SkinnedEffect::SetLightSpecularColor(int32_t whichLight, DirectX::FXMVECTOR value)
{
    pimpl->dirtyFlags |= pimpl->lights.SetLightSpecularColor(whichLight, value, pimpl->constants.lightSpecularColor);
}


void SkinnedEffect::EnableDefaultLighting()
{
    EffectLights::EnableDefaultLighting(this);
}


// Fog settings.
void SkinnedEffect::SetFogStart(float value)
{
    pimpl->fog.start = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::FogVector;
}


void SkinnedEffect::SetFogEnd(float value)
{
    pimpl->fog.end = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::FogVector;
}


void XM_CALLCONV SkinnedEffect::SetFogColor(DirectX::FXMVECTOR value)
{
    pimpl->constants.fogColor = value;

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


// Texture settings.
void SkinnedEffect::SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor)
{
    pimpl->texture = srvDescriptor;
    pimpl->sampler = samplerDescriptor;
}


// Animation settings.
void SkinnedEffect::SetBoneTransforms(_In_reads_(count) DirectX::XMMATRIX const* value, size_t count)
{
    if (count > MaxBones)
        throw std::out_of_range("count parameter out of range");

    auto boneConstant = pimpl->constants.bones;

    for (auto i = 0u; i < count; i++)
    {
        DirectX::XMMATRIX boneMatrix = XMMatrixTranspose(value[i]);

        boneConstant[i][0] = boneMatrix.r[0];
        boneConstant[i][1] = boneMatrix.r[1];
        boneConstant[i][2] = boneMatrix.r[2];
    }

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}


void SkinnedEffect::ResetBoneTransforms()
{
    auto boneConstant = pimpl->constants.bones;

    for(auto i = 0u; i < MaxBones; ++i)
    {
        boneConstant[i][0] = DirectX::g_XMIdentityR0;
        boneConstant[i][1] = DirectX::g_XMIdentityR1;
        boneConstant[i][2] = DirectX::g_XMIdentityR2;
    }

    pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}
