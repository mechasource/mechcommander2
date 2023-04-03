//--------------------------------------------------------------------------------------
// File: NormalMapEffect.cpp
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
struct NormalMapEffectConstants
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

static_assert((sizeof(NormalMapEffectConstants) % 16) == 0, "CB size not padded correctly");

// Traits type describes our characteristics to the EffectBase template.
struct NormalMapEffectTraits
{
	using ConstantBufferType = NormalMapEffectConstants;

	static constexpr int32_t VertexShaderCount = 8;
	static constexpr int32_t PixelShaderCount = 4;
	static constexpr int32_t ShaderPermutationCount = 16;
	static constexpr int32_t RootSignatureCount = 2;
};
} // namespace

// Internal NormalMapEffect implementation class.
class NormalMapEffect::Impl : public EffectBase<NormalMapEffectTraits>
{
public:
	Impl(_In_ ID3D12Device* device, uint32_t effectFlags, const EffectPipelineStateDescription& pipelineDescription);

	enum RootParameterIndex
	{
		TextureSRV,
		TextureNormalSRV,
		TextureSampler,
		ConstantBuffer,
		TextureSpecularSRV,
		RootParameterCount
	};

	bool specularMap;

	D3D12_GPU_DESCRIPTOR_HANDLE texture;
	D3D12_GPU_DESCRIPTOR_HANDLE specular;
	D3D12_GPU_DESCRIPTOR_HANDLE normal;
	D3D12_GPU_DESCRIPTOR_HANDLE sampler;

	EffectLights lights;

	int32_t GetPipelineStatePermutation(uint32_t effectFlags) const noexcept;

	void Apply(_In_ ID3D12GraphicsCommandList* commandList);
};

// Include the precompiled shader code.
namespace
{
#if defined(_XBOX_ONE) && defined(_TITLE)
#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTx.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTxVc.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTxNoSpec.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTxVcNoSpec.inc"

#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTxBn.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTxVcBn.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTxNoSpecBn.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTxVcNoSpecBn.inc"

#include "Shaders/Compiled/XboxOneNormalMapEffect_PSNormalPixelLightingTx.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_PSNormalPixelLightingTxNoFog.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_PSNormalPixelLightingTxNoSpec.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_PSNormalPixelLightingTxNoFogSpec.inc"
#else
#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTx.inc"
#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTxVc.inc"
#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTxNoSpec.inc"
#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTxVcNoSpec.inc"

#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTxBn.inc"
#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTxVcBn.inc"
#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTxNoSpecBn.inc"
#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBn.inc"

#include "Shaders/Compiled/NormalMapEffect_PSNormalPixelLightingTx.inc"
#include "Shaders/Compiled/NormalMapEffect_PSNormalPixelLightingTxNoFog.inc"
#include "Shaders/Compiled/NormalMapEffect_PSNormalPixelLightingTxNoSpec.inc"
#include "Shaders/Compiled/NormalMapEffect_PSNormalPixelLightingTxNoFogSpec.inc"
#endif
} // namespace

template <>
const D3D12_SHADER_BYTECODE EffectBase<NormalMapEffectTraits>::VertexShaderBytecode[] =
	{
		{NormalMapEffect_VSNormalPixelLightingTx, sizeof(NormalMapEffect_VSNormalPixelLightingTx)},
		{NormalMapEffect_VSNormalPixelLightingTxVc, sizeof(NormalMapEffect_VSNormalPixelLightingTxVc)},

		{NormalMapEffect_VSNormalPixelLightingTxBn, sizeof(NormalMapEffect_VSNormalPixelLightingTxBn)},
		{NormalMapEffect_VSNormalPixelLightingTxVcBn, sizeof(NormalMapEffect_VSNormalPixelLightingTxVcBn)},

		{NormalMapEffect_VSNormalPixelLightingTxNoSpec, sizeof(NormalMapEffect_VSNormalPixelLightingTxNoSpec)},
		{NormalMapEffect_VSNormalPixelLightingTxVcNoSpec, sizeof(NormalMapEffect_VSNormalPixelLightingTxVcNoSpec)},

		{NormalMapEffect_VSNormalPixelLightingTxNoSpecBn, sizeof(NormalMapEffect_VSNormalPixelLightingTxNoSpecBn)},
		{NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBn, sizeof(NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBn)},
};

template <>
const int32_t EffectBase<NormalMapEffectTraits>::VertexShaderIndices[] =
	{
		0, // pixel lighting + texture
		0, // pixel lighting + texture, no fog
		1, // pixel lighting + texture + vertex color
		1, // pixel lighting + texture + vertex color, no fog

		4, // pixel lighting + texture, no specular
		4, // pixel lighting + texture, no fog or specular
		5, // pixel lighting + texture + vertex color, no specular
		5, // pixel lighting + texture + vertex color, no fog or specular

		2, // pixel lighting (biased vertex normal) + texture
		2, // pixel lighting (biased vertex normal) + texture, no fog
		3, // pixel lighting (biased vertex normal) + texture + vertex color
		3, // pixel lighting (biased vertex normal) + texture + vertex color, no fog

		6, // pixel lighting (biased vertex normal) + texture, no specular
		6, // pixel lighting (biased vertex normal) + texture, no fog or specular
		7, // pixel lighting (biased vertex normal) + texture + vertex color, no specular
		7, // pixel lighting (biased vertex normal) + texture + vertex color, no fog or specular
};

template <>
const D3D12_SHADER_BYTECODE EffectBase<NormalMapEffectTraits>::PixelShaderBytecode[] =
	{
		{NormalMapEffect_PSNormalPixelLightingTx, sizeof(NormalMapEffect_PSNormalPixelLightingTx)},
		{NormalMapEffect_PSNormalPixelLightingTxNoFog, sizeof(NormalMapEffect_PSNormalPixelLightingTxNoFog)},
		{NormalMapEffect_PSNormalPixelLightingTxNoSpec, sizeof(NormalMapEffect_PSNormalPixelLightingTxNoSpec)},
		{NormalMapEffect_PSNormalPixelLightingTxNoFogSpec, sizeof(NormalMapEffect_PSNormalPixelLightingTxNoFogSpec)},
};

template <>
const int32_t EffectBase<NormalMapEffectTraits>::PixelShaderIndices[] =
	{
		0, // pixel lighting + texture
		1, // pixel lighting + texture, no fog
		0, // pixel lighting + texture + vertex color
		1, // pixel lighting + texture + vertex color, no fog

		2, // pixel lighting + texture, no specular
		3, // pixel lighting + texture, no fog or specular
		2, // pixel lighting + texture + vertex color, no specular
		3, // pixel lighting + texture + vertex color, no fog or specular

		0, // pixel lighting (biased vertex normal) + texture
		1, // pixel lighting (biased vertex normal) + texture, no fog
		0, // pixel lighting (biased vertex normal) + texture + vertex color
		1, // pixel lighting (biased vertex normal) + texture + vertex color, no fog

		2, // pixel lighting (biased vertex normal) + texture, no specular
		3, // pixel lighting (biased vertex normal) + texture, no fog or specular
		2, // pixel lighting (biased vertex normal) + texture + vertex color, no specular
		3, // pixel lighting (biased vertex normal) + texture + vertex color, no fog or specular
};

// Global pool of per-device NormalMapEffect resources.
template <>
SharedResourcePool<ID3D12Device*, EffectBase<NormalMapEffectTraits>::DeviceResources> EffectBase<NormalMapEffectTraits>::deviceResourcesPool = {};

// Constructor.
NormalMapEffect::Impl::Impl(
	_In_ ID3D12Device* device,
	uint32_t effectFlags,
	const EffectPipelineStateDescription& pipelineDescription)
	: EffectBase(device)
	, specularMap(effectFlags & EffectFlags::Specular)
	, texture {}
	, specular {}
	, normal {}
	, sampler {}
{
	static_assert(_countof(EffectBase<NormalMapEffectTraits>::VertexShaderIndices) == NormalMapEffectTraits::ShaderPermutationCount, "array/max mismatch");
	static_assert(_countof(EffectBase<NormalMapEffectTraits>::VertexShaderBytecode) == NormalMapEffectTraits::VertexShaderCount, "array/max mismatch");
	static_assert(_countof(EffectBase<NormalMapEffectTraits>::PixelShaderBytecode) == NormalMapEffectTraits::PixelShaderCount, "array/max mismatch");
	static_assert(_countof(EffectBase<NormalMapEffectTraits>::PixelShaderIndices) == NormalMapEffectTraits::ShaderPermutationCount, "array/max mismatch");

	lights.InitializeConstants(constants.specularColorAndPower, constants.lightDirection, constants.lightDiffuseColor, constants.lightSpecularColor);

	// Create root signature.
	{
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

		CD3DX12_ROOT_PARAMETER rootParameters[RootParameterIndex::RootParameterCount] = {};
		CD3DX12_DESCRIPTOR_RANGE textureSRV(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		rootParameters[RootParameterIndex::TextureSRV].InitAsDescriptorTable(1, &textureSRV, D3D12_SHADER_VISIBILITY_PIXEL);

		CD3DX12_DESCRIPTOR_RANGE textureSRV2(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
		rootParameters[RootParameterIndex::TextureNormalSRV].InitAsDescriptorTable(1, &textureSRV2, D3D12_SHADER_VISIBILITY_PIXEL);

		CD3DX12_DESCRIPTOR_RANGE textureSampler(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);
		rootParameters[RootParameterIndex::TextureSampler].InitAsDescriptorTable(1, &textureSampler, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[RootParameterIndex::ConstantBuffer].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

		CD3DX12_ROOT_SIGNATURE_DESC rsigDesc = {};

		if (specularMap)
		{
			CD3DX12_DESCRIPTOR_RANGE textureSRV3(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
			rootParameters[RootParameterIndex::TextureSpecularSRV].InitAsDescriptorTable(1, &textureSRV3, D3D12_SHADER_VISIBILITY_PIXEL);

			rsigDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

			m_prootsignature = GetRootSignature(1, rsigDesc);
		}
		else
		{
			rsigDesc.Init(_countof(rootParameters) - 1, rootParameters, 0, nullptr, rootSignatureFlags);

			m_prootsignature = GetRootSignature(0, rsigDesc);
		}
	}

	assert(m_prootsignature != nullptr);

	fog.enabled = (effectFlags & EffectFlags::Fog) != 0;

	// Create pipeline state.
	int32_t sp = GetPipelineStatePermutation(effectFlags);
	assert(sp >= 0 && sp < NormalMapEffectTraits::ShaderPermutationCount);
	_Analysis_assume_(sp >= 0 && sp < NormalMapEffectTraits::ShaderPermutationCount);

	int32_t vi = EffectBase<NormalMapEffectTraits>::VertexShaderIndices[sp];
	assert(vi >= 0 && vi < NormalMapEffectTraits::VertexShaderCount);
	_Analysis_assume_(vi >= 0 && vi < NormalMapEffectTraits::VertexShaderCount);
	int32_t pi = EffectBase<NormalMapEffectTraits>::PixelShaderIndices[sp];
	assert(pi >= 0 && pi < NormalMapEffectTraits::PixelShaderCount);
	_Analysis_assume_(pi >= 0 && pi < NormalMapEffectTraits::PixelShaderCount);

	pipelineDescription.CreatePipelineState(
		device,
		m_prootsignature,
		EffectBase<NormalMapEffectTraits>::VertexShaderBytecode[vi],
		EffectBase<NormalMapEffectTraits>::PixelShaderBytecode[pi],
		m_ppipelinestate.addressof());

	SetDebugObjectName(m_ppipelinestate.get(), L"NormalMapEffect");
}

int32_t NormalMapEffect::Impl::GetPipelineStatePermutation(uint32_t effectFlags) const noexcept
{
	int32_t permutation = 0;

	// Use optimized shaders if fog is disabled.
	if (!fog.enabled)
	{
		permutation += 1;
	}

	// Support vertex coloring?
	if (effectFlags & EffectFlags::VertexColor)
	{
		permutation += 2;
	}

	if (!specularMap)
	{
		permutation += 4;
	}

	if (effectFlags & EffectFlags::BiasedVertexNormals)
	{
		// Compressed normals need to be scaled and biased in the vertex shader.
		permutation += 8;
	}

	return permutation;
}

// Sets our state onto the D3D device.
void NormalMapEffect::Impl::Apply(_In_ ID3D12GraphicsCommandList* commandList)
{
	// Compute derived parameter values.
	matrices.SetConstants(dirtyFlags, constants.worldViewProj);
	fog.SetConstants(dirtyFlags, matrices.worldView, constants.fogVector);
	lights.SetConstants(dirtyFlags, matrices, constants.world, constants.worldInverseTranspose, constants.eyePosition, constants.diffuseColor, constants.emissiveColor, true);

	UpdateConstants();

	// Set the root signature
	commandList->SetGraphicsRootSignature(m_prootsignature);

	// Set the texture
	if (!texture.ptr || !sampler.ptr || !normal.ptr)
	{
		DebugTrace("ERROR: Missing texture(s) or sampler for NormalMapEffect (texture %llu, normal %llu, sampler %llu)\n", texture.ptr, normal.ptr, sampler.ptr);
		throw std::exception("NormalMapEffect");
	}

	// **NOTE** If D3D asserts or crashes here, you probably need to call commandList->SetDescriptorHeaps() with the required descriptor heaps.
	commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSRV, texture);
	commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureNormalSRV, normal);
	commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSampler, sampler);

	if (specularMap)
	{
		if (!specular.ptr)
		{
			DebugTrace("ERROR: Missing specular texure NormalMapEffect (texture %llu)\n", specular.ptr);
			throw std::exception("NormalMapEffect");
		}
		commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSpecularSRV, specular);
	}

	// Set constants
	commandList->SetGraphicsRootConstantBufferView(RootParameterIndex::ConstantBuffer, GetConstantBufferGpuAddress());

	// Set the pipeline state
	commandList->SetPipelineState(EffectBase::m_ppipelinestate.get());
}

// Public constructor.
NormalMapEffect::NormalMapEffect(
	_In_ ID3D12Device* device,
	uint32_t effectFlags,
	const EffectPipelineStateDescription& pipelineDescription)
	: pimpl(std::make_unique<Impl>(device, effectFlags, pipelineDescription))
{
}

// Move constructor.
NormalMapEffect::NormalMapEffect(NormalMapEffect&& moveFrom) noexcept
	: pimpl(std::move(moveFrom.pimpl))
{
}

// Move assignment.
NormalMapEffect& NormalMapEffect::operator=(NormalMapEffect&& moveFrom) noexcept
{
	pimpl = std::move(moveFrom.pimpl);
	return *this;
}

// Public destructor.
NormalMapEffect::~NormalMapEffect()
{
}

// IEffect methods
void NormalMapEffect::Apply(_In_ ID3D12GraphicsCommandList* commandList)
{
	pimpl->Apply(commandList);
}

// Camera settings
void XM_CALLCONV NormalMapEffect::SetWorld(DirectX::FXMMATRIX value)
{
	pimpl->matrices.world = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose | EffectDirtyFlags::FogVector;
}

void XM_CALLCONV NormalMapEffect::SetView(DirectX::FXMMATRIX value)
{
	pimpl->matrices.view = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::EyePosition | EffectDirtyFlags::FogVector;
}

void XM_CALLCONV NormalMapEffect::SetProjection(DirectX::FXMMATRIX value)
{
	pimpl->matrices.projection = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj;
}

void XM_CALLCONV NormalMapEffect::SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
{
	pimpl->matrices.world = world;
	pimpl->matrices.view = view;
	pimpl->matrices.projection = projection;

	pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose | EffectDirtyFlags::EyePosition | EffectDirtyFlags::FogVector;
}

// Material settings
void XM_CALLCONV NormalMapEffect::SetDiffuseColor(DirectX::FXMVECTOR value)
{
	pimpl->lights.diffuseColor = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}

void XM_CALLCONV NormalMapEffect::SetEmissiveColor(DirectX::FXMVECTOR value)
{
	pimpl->lights.emissiveColor = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}

void XM_CALLCONV NormalMapEffect::SetSpecularColor(DirectX::FXMVECTOR value)
{
	// Set xyz to new value, but preserve existing w (specular power).
	pimpl->constants.specularColorAndPower = DirectX::XMVectorSelect(pimpl->constants.specularColorAndPower, value, DirectX::g_XMSelect1110);

	pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}

void NormalMapEffect::SetSpecularPower(float value)
{
	// Set w to new value, but preserve existing xyz (specular color).
	pimpl->constants.specularColorAndPower = DirectX::XMVectorSetW(pimpl->constants.specularColorAndPower, value);

	pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}

void NormalMapEffect::DisableSpecular()
{
	// Set specular color to black, power to 1
	// Note: Don't use a power of 0 or the shader will generate strange highlights on non-specular materials

	pimpl->constants.specularColorAndPower = DirectX::g_XMIdentityR3;

	pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}

void NormalMapEffect::SetAlpha(float value)
{
	pimpl->lights.alpha = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}

void XM_CALLCONV NormalMapEffect::SetColorAndAlpha(DirectX::FXMVECTOR value)
{
	pimpl->lights.diffuseColor = value;
	pimpl->lights.alpha = DirectX::XMVectorGetW(value);

	pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}

// Light settings
void XM_CALLCONV NormalMapEffect::SetAmbientLightColor(DirectX::FXMVECTOR value)
{
	pimpl->lights.ambientLightColor = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
}

void NormalMapEffect::SetLightEnabled(int32_t whichLight, bool value)
{
	pimpl->dirtyFlags |= pimpl->lights.SetLightEnabled(whichLight, value, pimpl->constants.lightDiffuseColor, pimpl->constants.lightSpecularColor);
}

void XM_CALLCONV NormalMapEffect::SetLightDirection(int32_t whichLight, DirectX::FXMVECTOR value)
{
	EffectLights::ValidateLightIndex(whichLight);

	pimpl->constants.lightDirection[whichLight] = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}

void XM_CALLCONV NormalMapEffect::SetLightDiffuseColor(int32_t whichLight, DirectX::FXMVECTOR value)
{
	pimpl->dirtyFlags |= pimpl->lights.SetLightDiffuseColor(whichLight, value, pimpl->constants.lightDiffuseColor);
}

void XM_CALLCONV NormalMapEffect::SetLightSpecularColor(int32_t whichLight, DirectX::FXMVECTOR value)
{
	pimpl->dirtyFlags |= pimpl->lights.SetLightSpecularColor(whichLight, value, pimpl->constants.lightSpecularColor);
}

void NormalMapEffect::EnableDefaultLighting()
{
	EffectLights::EnableDefaultLighting(this);
}

// Fog settings.
void NormalMapEffect::SetFogStart(float value)
{
	pimpl->fog.start = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::FogVector;
}

void NormalMapEffect::SetFogEnd(float value)
{
	pimpl->fog.end = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::FogVector;
}

void XM_CALLCONV NormalMapEffect::SetFogColor(DirectX::FXMVECTOR value)
{
	pimpl->constants.fogColor = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}

// Texture settings.
void NormalMapEffect::SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor)
{
	pimpl->texture = srvDescriptor;
	pimpl->sampler = samplerDescriptor;
}

void NormalMapEffect::SetNormalTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor)
{
	pimpl->normal = srvDescriptor;
}

void NormalMapEffect::SetSpecularTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor)
{
	if (!pimpl->specularMap)
	{
		DebugTrace("WARNING: Specular texture set on NormalMapEffect instance created without specular shader (texture %llu)\n", srvDescriptor.ptr);
	}

	pimpl->specular = srvDescriptor;
}
