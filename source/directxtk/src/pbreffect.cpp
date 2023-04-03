//--------------------------------------------------------------------------------------
// File: PBREffect.cpp
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
struct PBREffectConstants
{
	DirectX::XMVECTOR eyePosition;
	DirectX::XMMATRIX world;
	DirectX::XMVECTOR worldInverseTranspose[3];
	DirectX::XMMATRIX worldViewProj;
	DirectX::XMMATRIX prevWorldViewProj; // for velocity generation

	DirectX::XMVECTOR lightDirection[IEffectLights::MaxDirectionalLights];
	DirectX::XMVECTOR lightDiffuseColor[IEffectLights::MaxDirectionalLights];

	// PBR Parameters
	DirectX::XMVECTOR Albedo;
	float Metallic;
	float Roughness;
	int32_t numRadianceMipLevels;

	// Size of render target
	float targetWidth;
	float targetHeight;
};

static_assert((sizeof(PBREffectConstants) % 16) == 0, "CB size not padded correctly");

// Traits type describes our characteristics to the EffectBase template.
struct PBREffectTraits
{
	using ConstantBufferType = PBREffectConstants;

	static constexpr int32_t VertexShaderCount = 4;
	static constexpr int32_t PixelShaderCount = 5;
	static constexpr int32_t ShaderPermutationCount = 10;
	static constexpr int32_t RootSignatureCount = 1;
};
} // namespace

// Internal PBREffect implementation class.
class PBREffect::Impl : public EffectBase<PBREffectTraits>
{
public:
	Impl(_In_ ID3D12Device* device,
		uint32_t effectFlags,
		const EffectPipelineStateDescription& pipelineDescription);

	void Apply(_In_ ID3D12GraphicsCommandList* commandList);

	int32_t GetPipelineStatePermutation(uint32_t effectFlags) const noexcept;

	bool textureEnabled;
	bool emissiveMap;

	enum RootParameterIndex
	{
		AlbedoTexture,
		NormalTexture,
		RMATexture,
		EmissiveTexture,
		RadianceTexture,
		IrradianceTexture,
		SurfaceSampler,
		RadianceSampler,
		ConstantBuffer,
		RootParametersCount
	};

	D3D12_GPU_DESCRIPTOR_HANDLE descriptors[RootParametersCount];

	DirectX::XMVECTOR lightColor[MaxDirectionalLights];
};

// Include the precompiled shader code.
namespace
{
#if defined(_XBOX_ONE) && defined(_TITLE)
#include "Shaders/Compiled/XboxOnePBREffect_VSConstant.inc"
#include "Shaders/Compiled/XboxOnePBREffect_VSConstantVelocity.inc"
#include "Shaders/Compiled/XboxOnePBREffect_VSConstantBn.inc"
#include "Shaders/Compiled/XboxOnePBREffect_VSConstantVelocityBn.inc"

#include "Shaders/Compiled/XboxOnePBREffect_PSConstant.inc"
#include "Shaders/Compiled/XboxOnePBREffect_PSTextured.inc"
#include "Shaders/Compiled/XboxOnePBREffect_PSTexturedEmissive.inc"
#include "Shaders/Compiled/XboxOnePBREffect_PSTexturedVelocity.inc"
#include "Shaders/Compiled/XboxOnePBREffect_PSTexturedEmissiveVelocity.inc"
#else
#include "Shaders/Compiled/PBREffect_VSConstant.inc"
#include "Shaders/Compiled/PBREffect_VSConstantVelocity.inc"
#include "Shaders/Compiled/PBREffect_VSConstantBn.inc"
#include "Shaders/Compiled/PBREffect_VSConstantVelocityBn.inc"

#include "Shaders/Compiled/PBREffect_PSConstant.inc"
#include "Shaders/Compiled/PBREffect_PSTextured.inc"
#include "Shaders/Compiled/PBREffect_PSTexturedEmissive.inc"
#include "Shaders/Compiled/PBREffect_PSTexturedVelocity.inc"
#include "Shaders/Compiled/PBREffect_PSTexturedEmissiveVelocity.inc"
#endif
} // namespace

template <>
const D3D12_SHADER_BYTECODE EffectBase<PBREffectTraits>::VertexShaderBytecode[] =
	{
		{PBREffect_VSConstant, sizeof(PBREffect_VSConstant)},
		{PBREffect_VSConstantVelocity, sizeof(PBREffect_VSConstantVelocity)},
		{PBREffect_VSConstantBn, sizeof(PBREffect_VSConstantBn)},
		{PBREffect_VSConstantVelocityBn, sizeof(PBREffect_VSConstantVelocityBn)},
};

template <>
const int32_t EffectBase<PBREffectTraits>::VertexShaderIndices[] =
	{
		0, // constant
		0, // textured
		0, // textured + emissive
		1, // textured + velocity
		1, // textured + emissive + velocity

		2, // constant (biased vertex normals)
		2, // textured (biased vertex normals)
		2, // textured + emissive (biased vertex normals)
		3, // textured + velocity (biased vertex normals)
		3, // textured + emissive + velocity (biasoed vertex normals)
};

template <>
const D3D12_SHADER_BYTECODE EffectBase<PBREffectTraits>::PixelShaderBytecode[] =
	{
		{PBREffect_PSConstant, sizeof(PBREffect_PSConstant)},
		{PBREffect_PSTextured, sizeof(PBREffect_PSTextured)},
		{PBREffect_PSTexturedEmissive, sizeof(PBREffect_PSTexturedEmissive)},
		{PBREffect_PSTexturedVelocity, sizeof(PBREffect_PSTexturedVelocity)},
		{PBREffect_PSTexturedEmissiveVelocity, sizeof(PBREffect_PSTexturedEmissiveVelocity)}};

template <>
const int32_t EffectBase<PBREffectTraits>::PixelShaderIndices[] =
	{
		0, // constant
		1, // textured
		2, // textured + emissive
		3, // textured + velocity
		4, // textured + emissive + velocity

		0, // constant (biased vertex normals)
		1, // textured (biased vertex normals)
		2, // textured + emissive (biased vertex normals)
		3, // textured + velocity (biased vertex normals)
		4, // textured + emissive + velocity (biased vertex normals)
};

// Global pool of per-device PBREffect resources. Required by EffectBase<>, but not used.
template <>
SharedResourcePool<ID3D12Device*, EffectBase<PBREffectTraits>::DeviceResources> EffectBase<PBREffectTraits>::deviceResourcesPool = {};

// Constructor.
PBREffect::Impl::Impl(_In_ ID3D12Device* device,
	uint32_t effectFlags,
	const EffectPipelineStateDescription& pipelineDescription)
	: EffectBase(device)
	, emissiveMap(effectFlags & EffectFlags::Emissive)
	, descriptors {}
	, lightColor {}
{
	static_assert(_countof(EffectBase<PBREffectTraits>::VertexShaderIndices) == PBREffectTraits::ShaderPermutationCount, "array/max mismatch");
	static_assert(_countof(EffectBase<PBREffectTraits>::VertexShaderBytecode) == PBREffectTraits::VertexShaderCount, "array/max mismatch");
	static_assert(_countof(EffectBase<PBREffectTraits>::PixelShaderBytecode) == PBREffectTraits::PixelShaderCount, "array/max mismatch");
	static_assert(_countof(EffectBase<PBREffectTraits>::PixelShaderIndices) == PBREffectTraits::ShaderPermutationCount, "array/max mismatch");

	// Lighting
	static const DirectX::XMVECTORF32 defaultLightDirection = {{{0, -1, 0, 0}}};
	for (int32_t i = 0; i < MaxDirectionalLights; i++)
	{
		lightColor[i] = DirectX::g_XMOne;
		constants.lightDirection[i] = defaultLightDirection;
		constants.lightDiffuseColor[i] = DirectX::g_XMZero;
	}

	if (effectFlags & EffectFlags::Texture)
	{
		textureEnabled = true;
	}
	else
	{
		textureEnabled = false;

		if (effectFlags & (EffectFlags::Emissive | EffectFlags::Velocity))
		{
			DebugTrace("ERROR: PBREffect does not support emissive or velocity without surface textures\n");
			throw std::invalid_argument("PBREffect");
		}
	}

	// Default PBR values
	constants.Albedo = DirectX::g_XMOne;
	constants.Metallic = 0.5f;
	constants.Roughness = 0.2f;
	constants.numRadianceMipLevels = 1;

	// Create root signature
	{
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // Only the input assembler stage needs access to the constant buffer.
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

		CD3DX12_ROOT_PARAMETER rootParameters[RootParametersCount] = {};
		CD3DX12_DESCRIPTOR_RANGE textureSRV[6] = {
			CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0),
			CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1),
			CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2),
			CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3),
			CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4),
			CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5),
		};

		CD3DX12_DESCRIPTOR_RANGE textureSampler[2] = {
			CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0),
			CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 1)};

		for (auto i = 0u; i < _countof(textureSRV); i++)
		{
			rootParameters[i].InitAsDescriptorTable(1, &textureSRV[i]);
		}

		for (auto i = 0u; i < _countof(textureSampler); i++)
		{
			rootParameters[i + SurfaceSampler].InitAsDescriptorTable(1, &textureSampler[i]);
		}

		rootParameters[ConstantBuffer].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

		CD3DX12_ROOT_SIGNATURE_DESC rsigDesc;
		rsigDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

		m_prootsignature = GetRootSignature(0, rsigDesc);
	}

	assert(m_prootsignature != nullptr);

	if (effectFlags & EffectFlags::Fog)
	{
		DebugTrace("ERROR: PBEffect does not implement EffectFlags::Fog\n");
		throw std::invalid_argument("PBREffect");
	}
	else if (effectFlags & EffectFlags::VertexColor)
	{
		DebugTrace("ERROR: PBEffect does not implement EffectFlags::VertexColor\n");
		throw std::invalid_argument("PBREffect");
	}

	// Create pipeline state.
	int32_t sp = GetPipelineStatePermutation(effectFlags);
	assert(sp >= 0 && sp < PBREffectTraits::ShaderPermutationCount);
	_Analysis_assume_(sp >= 0 && sp < PBREffectTraits::ShaderPermutationCount);

	int32_t vi = EffectBase<PBREffectTraits>::VertexShaderIndices[sp];
	assert(vi >= 0 && vi < PBREffectTraits::VertexShaderCount);
	_Analysis_assume_(vi >= 0 && vi < PBREffectTraits::VertexShaderCount);
	int32_t pi = EffectBase<PBREffectTraits>::PixelShaderIndices[sp];
	assert(pi >= 0 && pi < PBREffectTraits::PixelShaderCount);
	_Analysis_assume_(pi >= 0 && pi < PBREffectTraits::PixelShaderCount);

	pipelineDescription.CreatePipelineState(
		device,
		m_prootsignature,
		EffectBase<PBREffectTraits>::VertexShaderBytecode[vi],
		EffectBase<PBREffectTraits>::PixelShaderBytecode[pi],
		m_ppipelinestate.put());

	SetDebugObjectName(m_ppipelinestate.get(), L"PBREffect");
}

int32_t PBREffect::Impl::GetPipelineStatePermutation(uint32_t effectFlags) const noexcept
{
	int32_t permutation = 0;

	// Textured RMA vs. constant albedo/roughness/metalness?
	if (effectFlags & EffectFlags::Velocity)
	{
		// Optional velocity buffer (implies textured RMA)?
		permutation = 3;
	}
	else if (textureEnabled)
	{
		permutation = 1;
	}

	// Using an emissive texture?
	if (emissiveMap)
	{
		permutation += 1;
	}

	if (effectFlags & EffectFlags::BiasedVertexNormals)
	{
		// Compressed normals need to be scaled and biased in the vertex shader.
		permutation += 5;
	}

	return permutation;
}

// Sets our state onto the D3D device.
void PBREffect::Impl::Apply(_In_ ID3D12GraphicsCommandList* commandList)
{
	// Store old wvp for velocity calculation in shader
	constants.prevWorldViewProj = constants.worldViewProj;

	// Compute derived parameter values.
	matrices.SetConstants(dirtyFlags, constants.worldViewProj);

	// World inverse transpose matrix.
	if (dirtyFlags & EffectDirtyFlags::WorldInverseTranspose)
	{
		constants.world = XMMatrixTranspose(matrices.world);

		DirectX::XMMATRIX worldInverse = XMMatrixInverse(nullptr, matrices.world);

		constants.worldInverseTranspose[0] = worldInverse.r[0];
		constants.worldInverseTranspose[1] = worldInverse.r[1];
		constants.worldInverseTranspose[2] = worldInverse.r[2];

		dirtyFlags &= ~EffectDirtyFlags::WorldInverseTranspose;
		dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
	}

	// Eye position vector.
	if (dirtyFlags & EffectDirtyFlags::EyePosition)
	{
		DirectX::XMMATRIX viewInverse = XMMatrixInverse(nullptr, matrices.view);

		constants.eyePosition = viewInverse.r[3];

		dirtyFlags &= ~EffectDirtyFlags::EyePosition;
		dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
	}

	// Set constants to GPU
	UpdateConstants();

	// Set the root signature
	commandList->SetGraphicsRootSignature(m_prootsignature);

	if (!descriptors[RadianceTexture].ptr || !descriptors[RadianceSampler].ptr)
	{
		DebugTrace("ERROR: Missing radiance texture or sampler for PBREffect (texture %llu, sampler %llu)\n", descriptors[RadianceTexture].ptr, descriptors[RadianceSampler].ptr);
		throw std::exception("PBREffect");
	}

	if (!descriptors[IrradianceTexture].ptr)
	{
		DebugTrace("ERROR: Missing irradiance texture for PBREffect (texture %llu)\n", descriptors[IrradianceTexture].ptr);
		throw std::exception("PBREffect");
	}

	// Set the root parameters
	if (!textureEnabled)
	{
		// only update radiance/irradiance texture and samplers

		// **NOTE** If D3D asserts or crashes here, you probably need to call commandList->SetDescriptorHeaps() with the required descriptor heaps.
		commandList->SetGraphicsRootDescriptorTable(RadianceTexture, descriptors[RadianceTexture]);
		commandList->SetGraphicsRootDescriptorTable(IrradianceTexture, descriptors[IrradianceTexture]);
		commandList->SetGraphicsRootDescriptorTable(RadianceSampler, descriptors[RadianceSampler]);

		// Bind 'empty' textures to avoid warnings on PC
		commandList->SetGraphicsRootDescriptorTable(AlbedoTexture, descriptors[RadianceTexture]);
		commandList->SetGraphicsRootDescriptorTable(NormalTexture, descriptors[RadianceTexture]);
		commandList->SetGraphicsRootDescriptorTable(RMATexture, descriptors[RadianceTexture]);
		commandList->SetGraphicsRootDescriptorTable(EmissiveTexture, descriptors[RadianceTexture]);
		commandList->SetGraphicsRootDescriptorTable(SurfaceSampler, descriptors[RadianceSampler]);
	}
	else
	{
		if (!descriptors[AlbedoTexture].ptr || !descriptors[SurfaceSampler].ptr)
		{
			DebugTrace("ERROR: Missing albedo texture or sampler for PBREffect (texture %llu, sampler %llu)\n", descriptors[AlbedoTexture].ptr, descriptors[SurfaceSampler].ptr);
			throw std::exception("PBREffect");
		}

		if (!descriptors[NormalTexture].ptr)
		{
			DebugTrace("ERROR: Missing normal map texture for PBREffect (texture %llu)\n", descriptors[NormalTexture].ptr);
			throw std::exception("PBREffect");
		}

		if (!descriptors[RMATexture].ptr)
		{
			DebugTrace("ERROR: Missing roughness/metalness texture for PBREffect (texture %llu)\n", descriptors[RMATexture].ptr);
			throw std::exception("PBREffect");
		}

		for (auto i = 0u; i < ConstantBuffer; i++)
		{
			if (i == EmissiveTexture)
				continue;

			// **NOTE** If D3D asserts or crashes here, you probably need to call commandList->SetDescriptorHeaps() with the required descriptor heaps.
			commandList->SetGraphicsRootDescriptorTable(i, descriptors[i]);
		}

		if (emissiveMap)
		{
			if (!descriptors[EmissiveTexture].ptr)
			{
				DebugTrace("ERROR: Missing emissive map texture for PBREffect (texture %llu)\n", descriptors[NormalTexture].ptr);
				throw std::exception("PBREffect");
			}

			commandList->SetGraphicsRootDescriptorTable(EmissiveTexture, descriptors[EmissiveTexture]);
		}
		else
		{
			// Bind 'empty' textures to avoid warnings on PC
			commandList->SetGraphicsRootDescriptorTable(EmissiveTexture, descriptors[AlbedoTexture]);
		}
	}

	// Set constants
	commandList->SetGraphicsRootConstantBufferView(ConstantBuffer, GetConstantBufferGpuAddress());

	// Set the pipeline state
	commandList->SetPipelineState(EffectBase::m_ppipelinestate.get());
}

// Public constructor.
PBREffect::PBREffect(_In_ ID3D12Device* device,
	uint32_t effectFlags,
	const EffectPipelineStateDescription& pipelineDescription)
	: pimpl(std::make_unique<Impl>(device, effectFlags, pipelineDescription))
{
}

// Move constructor.
PBREffect::PBREffect(PBREffect&& moveFrom) noexcept
	: pimpl(std::move(moveFrom.pimpl))
{
}

// Move assignment.
PBREffect& PBREffect::operator=(PBREffect&& moveFrom) noexcept
{
	pimpl = std::move(moveFrom.pimpl);
	return *this;
}

// Public destructor.
PBREffect::~PBREffect()
{
}

// IEffect methods.
void PBREffect::Apply(_In_ ID3D12GraphicsCommandList* commandList)
{
	pimpl->Apply(commandList);
}

// Camera settings.
void XM_CALLCONV PBREffect::SetWorld(DirectX::FXMMATRIX value)
{
	pimpl->matrices.world = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose;
}

void XM_CALLCONV PBREffect::SetView(DirectX::FXMMATRIX value)
{
	pimpl->matrices.view = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::EyePosition;
}

void XM_CALLCONV PBREffect::SetProjection(DirectX::FXMMATRIX value)
{
	pimpl->matrices.projection = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj;
}

void XM_CALLCONV PBREffect::SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
{
	pimpl->matrices.world = world;
	pimpl->matrices.view = view;
	pimpl->matrices.projection = projection;

	pimpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose | EffectDirtyFlags::EyePosition;
}

// Light settings
void XM_CALLCONV PBREffect::SetAmbientLightColor(DirectX::FXMVECTOR)
{
	// Unsupported interface.
}

void PBREffect::SetLightEnabled(int32_t whichLight, bool value)
{
	EffectLights::ValidateLightIndex(whichLight);

	pimpl->constants.lightDiffuseColor[whichLight] = (value) ? pimpl->lightColor[whichLight] : DirectX::g_XMZero;

	pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}

void XM_CALLCONV PBREffect::SetLightDirection(int32_t whichLight, DirectX::FXMVECTOR value)
{
	EffectLights::ValidateLightIndex(whichLight);

	pimpl->constants.lightDirection[whichLight] = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}

void XM_CALLCONV PBREffect::SetLightDiffuseColor(int32_t whichLight, DirectX::FXMVECTOR value)
{
	EffectLights::ValidateLightIndex(whichLight);

	pimpl->lightColor[whichLight] = value;
	pimpl->constants.lightDiffuseColor[whichLight] = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}

void XM_CALLCONV PBREffect::SetLightSpecularColor(int32_t, DirectX::FXMVECTOR)
{
	// Unsupported interface.
}

void PBREffect::EnableDefaultLighting()
{
	EffectLights::EnableDefaultLighting(this);
}

// PBR Settings
void PBREffect::SetAlpha(float value)
{
	// Set w to new value, but preserve existing xyz (constant albedo).
	pimpl->constants.Albedo = DirectX::XMVectorSetW(pimpl->constants.Albedo, value);

	pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}

void PBREffect::SetConstantAlbedo(DirectX::FXMVECTOR value)
{
	// Set xyz to new value, but preserve existing w (alpha).
	pimpl->constants.Albedo = DirectX::XMVectorSelect(pimpl->constants.Albedo, value, DirectX::g_XMSelect1110);

	pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}

void PBREffect::SetConstantMetallic(float value)
{
	pimpl->constants.Metallic = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}

void PBREffect::SetConstantRoughness(float value)
{
	pimpl->constants.Roughness = value;

	pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}

// Texture settings.
void PBREffect::SetAlbedoTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor)
{
	pimpl->descriptors[Impl::RootParameterIndex::AlbedoTexture] = srvDescriptor;
	pimpl->descriptors[Impl::RootParameterIndex::SurfaceSampler] = samplerDescriptor;
}

void PBREffect::SetNormalTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor)
{
	pimpl->descriptors[Impl::RootParameterIndex::NormalTexture] = srvDescriptor;
}

void PBREffect::SetRMATexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor)
{
	pimpl->descriptors[Impl::RootParameterIndex::RMATexture] = srvDescriptor;
}

void PBREffect::SetEmissiveTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor)
{
	if (!pimpl->emissiveMap)
	{
		DebugTrace("WARNING: Emissive texture set on PBREffect instance created without emissive shader (texture %llu)\n", srvDescriptor.ptr);
	}

	pimpl->descriptors[Impl::RootParameterIndex::EmissiveTexture] = srvDescriptor;
}

void PBREffect::SetSurfaceTextures(
	D3D12_GPU_DESCRIPTOR_HANDLE albedo,
	D3D12_GPU_DESCRIPTOR_HANDLE normal,
	D3D12_GPU_DESCRIPTOR_HANDLE roughnessMetallicAmbientOcclusion,
	D3D12_GPU_DESCRIPTOR_HANDLE sampler)
{
	pimpl->descriptors[Impl::RootParameterIndex::AlbedoTexture] = albedo;
	pimpl->descriptors[Impl::RootParameterIndex::NormalTexture] = normal;
	pimpl->descriptors[Impl::RootParameterIndex::RMATexture] = roughnessMetallicAmbientOcclusion;
	pimpl->descriptors[Impl::RootParameterIndex::SurfaceSampler] = sampler;
}

void PBREffect::SetIBLTextures(
	D3D12_GPU_DESCRIPTOR_HANDLE radiance,
	int32_t numRadianceMips,
	D3D12_GPU_DESCRIPTOR_HANDLE irradiance,
	D3D12_GPU_DESCRIPTOR_HANDLE sampler)
{
	pimpl->descriptors[Impl::RootParameterIndex::RadianceTexture] = radiance;
	pimpl->descriptors[Impl::RootParameterIndex::RadianceSampler] = sampler;
	pimpl->constants.numRadianceMipLevels = numRadianceMips;

	pimpl->descriptors[Impl::RootParameterIndex::IrradianceTexture] = irradiance;

	pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}

// Additional settings.
void PBREffect::SetRenderTargetSizeInPixels(int32_t width, int32_t height)
{
	pimpl->constants.targetWidth = static_cast<float>(width);
	pimpl->constants.targetHeight = static_cast<float>(height);

	pimpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
}
