//--------------------------------------------------------------------------------------
// File: ToneMapPostProcess.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"

#include "d3dx12.h"
#include "postprocess.h"
#include "AlignedNew.h"
#include "commonstates.h"
#include "DemandCreate.h"
#include "directxhelpers.h"
#include "EffectPipelineStateDescription.h"
#include "graphicsmemory.h"
#include "SharedResourcePool.h"

using namespace directxtk;

// using Microsoft::WRL::ComPtr;

namespace
{
constexpr int32_t Dirty_ConstantBuffer = 0x01;
constexpr int32_t Dirty_Parameters = 0x02;

#if defined(_XBOX_ONE) && defined(_TITLE)
constexpr int32_t PixelShaderCount = 15;
constexpr int32_t ShaderPermutationCount = 24;
#else
constexpr int32_t PixelShaderCount = 9;
constexpr int32_t ShaderPermutationCount = 12;
#endif

// Constant buffer layout. Must match the shader!
__declspec(align(16)) struct ToneMapConstants
{
	// linearExposure is .x
	// paperWhiteNits is .y
	DirectX::XMVECTOR parameters;
};

static_assert((sizeof(ToneMapConstants) % 16) == 0, "CB size not padded correctly");
} // namespace

// Include the precompiled shader code.
namespace
{
#if defined(_XBOX_ONE) && defined(_TITLE)
#include "Shaders/Compiled/XboxOneToneMap_VSQuad.inc"

#include "Shaders/Compiled/XboxOneToneMap_PSCopy.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSSaturate.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSReinhard.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSACESFilmic.inc"
#include "Shaders/Compiled/XboxOneToneMap_PS_SRGB.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSSaturate_SRGB.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSReinhard_SRGB.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSACESFilmic_SRGB.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSHDR10.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSHDR10_Saturate.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSHDR10_Reinhard.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSHDR10_ACESFilmic.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSHDR10_Saturate_SRGB.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSHDR10_Reinhard_SRGB.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSHDR10_ACESFilmic_SRGB.inc"
#else
#include "Shaders/Compiled/ToneMap_VSQuad.inc"

#include "Shaders/Compiled/ToneMap_PSCopy.inc"
#include "Shaders/Compiled/ToneMap_PSSaturate.inc"
#include "Shaders/Compiled/ToneMap_PSReinhard.inc"
#include "Shaders/Compiled/ToneMap_PSACESFilmic.inc"
#include "Shaders/Compiled/ToneMap_PS_SRGB.inc"
#include "Shaders/Compiled/ToneMap_PSSaturate_SRGB.inc"
#include "Shaders/Compiled/ToneMap_PSReinhard_SRGB.inc"
#include "Shaders/Compiled/ToneMap_PSACESFilmic_SRGB.inc"
#include "Shaders/Compiled/ToneMap_PSHDR10.inc"
#endif
} // namespace

namespace
{
const D3D12_SHADER_BYTECODE vertexShader =
	{ToneMap_VSQuad, sizeof(ToneMap_VSQuad)};

const D3D12_SHADER_BYTECODE pixelShaders[] =
	{
		{ToneMap_PSCopy, sizeof(ToneMap_PSCopy)},
		{ToneMap_PSSaturate, sizeof(ToneMap_PSSaturate)},
		{ToneMap_PSReinhard, sizeof(ToneMap_PSReinhard)},
		{ToneMap_PSACESFilmic, sizeof(ToneMap_PSACESFilmic)},
		{ToneMap_PS_SRGB, sizeof(ToneMap_PS_SRGB)},
		{ToneMap_PSSaturate_SRGB, sizeof(ToneMap_PSSaturate_SRGB)},
		{ToneMap_PSReinhard_SRGB, sizeof(ToneMap_PSReinhard_SRGB)},
		{ToneMap_PSACESFilmic_SRGB, sizeof(ToneMap_PSACESFilmic_SRGB)},
		{ToneMap_PSHDR10, sizeof(ToneMap_PSHDR10)},

#if defined(_XBOX_ONE) && defined(_TITLE)
		// Shaders that generate both HDR10 and GameDVR SDR signals via Multiple Render Targets.
		{ToneMap_PSHDR10_Saturate, sizeof(ToneMap_PSHDR10_Saturate)},
		{ToneMap_PSHDR10_Reinhard, sizeof(ToneMap_PSHDR10_Reinhard)},
		{ToneMap_PSHDR10_ACESFilmic, sizeof(ToneMap_PSHDR10_ACESFilmic)},
		{ToneMap_PSHDR10_Saturate_SRGB, sizeof(ToneMap_PSHDR10_Saturate_SRGB)},
		{ToneMap_PSHDR10_Reinhard_SRGB, sizeof(ToneMap_PSHDR10_Reinhard_SRGB)},
		{ToneMap_PSHDR10_ACESFilmic_SRGB, sizeof(ToneMap_PSHDR10_ACESFilmic_SRGB)},
#endif
};

static_assert(_countof(pixelShaders) == PixelShaderCount, "array/max mismatch");

const int32_t pixelShaderIndices[] =
	{
		// Linear EOTF
		0, // Copy
		1, // Saturate
		2, // Reinhard
		3, // ACES Filmic

		// Gamam22 EOTF
		4, // SRGB
		5, // Saturate_SRGB
		6, // Reinhard_SRGB
		7, // ACES Filmic

		// ST.2084 EOTF
		8, // HDR10
		8, // HDR10
		8, // HDR10
		8, // HDR10

#if defined(_XBOX_ONE) && defined(_TITLE)
		// MRT Linear EOTF
		9, // HDR10+Saturate
		9, // HDR10+Saturate
		10, // HDR10+Reinhard
		11, // HDR10+ACESFilmic

		// MRT Gamma22 EOTF
		12, // HDR10+Saturate_SRGB
		12, // HDR10+Saturate_SRGB
		13, // HDR10+Reinhard_SRGB
		14, // HDR10+ACESFilmic

		// MRT ST.2084 EOTF
		9, // HDR10+Saturate
		9, // HDR10+Saturate
		10, // HDR10+Reinhard
		11, // HDR10+ACESFilmic
#endif
};

static_assert(_countof(pixelShaderIndices) == ShaderPermutationCount, "array/max mismatch");

// Factory for lazily instantiating shared root signatures.
class DeviceResources
{
public:
	DeviceResources(_In_ ID3D12Device* device) noexcept
		: m_device(device)
	{
	}

	ID3D12RootSignature* GetRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc)
	{
		return DemandCreate(m_prootsignature, mMutex, [&](ID3D12RootSignature** pResult) noexcept -> HRESULT
			{
                HRESULT hr = CreateRootSignature(m_device.get(), &desc, pResult);

                if (SUCCEEDED(hr))
                    SetDebugObjectName(*pResult, L"ToneMapPostProcess");

                return hr; });
	}

	ID3D12Device* GetDevice() const noexcept
	{
		return m_device.get();
	}

protected:
	wil::com_ptr<ID3D12Device> m_device;
	wil::com_ptr<ID3D12RootSignature> m_prootsignature;
	std::mutex mMutex;
};
} // namespace

class ToneMapPostProcess::Impl : public AlignedNew<ToneMapConstants>
{
public:
	Impl(_In_ ID3D12Device* device, const RenderTargetState& rtState, Operator op, TransferFunction func, bool mrt = false);

	void Process(_In_ ID3D12GraphicsCommandList* commandList);

	void SetDirtyFlag() noexcept
	{
		mDirtyFlags = INT_MAX;
	}

	enum RootParameterIndex
	{
		TextureSRV,
		ConstantBuffer,
		RootParameterCount
	};

	// Fields.
	ToneMapConstants constants;
	D3D12_GPU_DESCRIPTOR_HANDLE texture;
	float linearExposure;
	float paperWhiteNits;

private:
	int32_t mDirtyFlags;

	// D3D constant buffer holds a copy of the same data as the public 'constants' field.
	GraphicsResource mConstantBuffer;

	// Per instance cache of PSOs, populated with variants for each shader & layout
	wil::com_ptr<ID3D12PipelineState> m_ppipelinestate;

	// Per instance root signature
	ID3D12RootSignature* m_prootsignature;

	// Per-device resources.
	std::shared_ptr<DeviceResources> mDeviceResources;

	static SharedResourcePool<ID3D12Device*, DeviceResources> deviceResourcesPool;
};

// Global pool of per-device ToneMapPostProcess resources.
SharedResourcePool<ID3D12Device*, DeviceResources> ToneMapPostProcess::Impl::deviceResourcesPool;

// Constructor.
ToneMapPostProcess::Impl::Impl(_In_ ID3D12Device* device, const RenderTargetState& rtState, Operator op, TransferFunction func, bool mrt)
	: constants {}
	, texture {}
	, linearExposure(1.f)
	, paperWhiteNits(200.f)
	, mDirtyFlags(INT_MAX)
	, mDeviceResources(deviceResourcesPool.DemandCreate(device))
{
	if (op >= Operator_Max)
		throw std::out_of_range("Tonemap operator not defined");

	if (func > TransferFunction_Max)
		throw std::out_of_range("Transfer function not defined");

	// Create root signature.
	{
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

		CD3DX12_DESCRIPTOR_RANGE textureSRVs(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		// Same as CommonStates::StaticPointClamp
		CD3DX12_STATIC_SAMPLER_DESC sampler(
			0, // register
			D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			0.f,
			16,
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
			0.f,
			D3D12_FLOAT32_MAX,
			D3D12_SHADER_VISIBILITY_PIXEL);

		CD3DX12_ROOT_PARAMETER rootParameters[RootParameterIndex::RootParameterCount] = {};

		CD3DX12_DESCRIPTOR_RANGE texture1Range(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		rootParameters[RootParameterIndex::TextureSRV].InitAsDescriptorTable(1, &textureSRVs, D3D12_SHADER_VISIBILITY_PIXEL);

		// Root parameter descriptor
		CD3DX12_ROOT_SIGNATURE_DESC rsigDesc = {};

		// Constant buffer
		rootParameters[RootParameterIndex::ConstantBuffer].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_PIXEL);

		rsigDesc.Init(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

		m_prootsignature = mDeviceResources->GetRootSignature(rsigDesc);
	}

	assert(m_prootsignature != nullptr);

	// Determine shader permutation.
#if defined(_XBOX_ONE) && defined(_TITLE)
	int32_t permutation = (mrt) ? 12 : 0;
	permutation += (static_cast<int32_t>(func) * static_cast<int32_t>(Operator_Max)) + static_cast<int32_t>(op);
#else
	UNREFERENCED_PARAMETER(mrt);
	int32_t permutation = (static_cast<int32_t>(func) * static_cast<int32_t>(Operator_Max)) + static_cast<int32_t>(op);
#endif

	assert(permutation >= 0 && permutation < ShaderPermutationCount);
	_Analysis_assume_(permutation >= 0 && permutation < ShaderPermutationCount);

	int32_t shaderIndex = pixelShaderIndices[permutation];
	assert(shaderIndex >= 0 && shaderIndex < PixelShaderCount);
	_Analysis_assume_(shaderIndex >= 0 && shaderIndex < PixelShaderCount);

	// Create pipeline state.
	EffectPipelineStateDescription psd(nullptr,
		CommonStates::Opaque,
		CommonStates::DepthNone,
		CommonStates::CullNone,
		rtState,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	psd.CreatePipelineState(
		device,
		m_prootsignature,
		vertexShader,
		pixelShaders[shaderIndex],
		m_ppipelinestate.addressof());

	SetDebugObjectName(m_ppipelinestate.get(), L"ToneMapPostProcess");
}

// Sets our state onto the D3D device.
void ToneMapPostProcess::Impl::Process(_In_ ID3D12GraphicsCommandList* commandList)
{
	// Set the root signature.
	commandList->SetGraphicsRootSignature(m_prootsignature);

	// Set the texture.
	if (!texture.ptr)
	{
		DebugTrace("ERROR: Missing texture for ToneMapPostProcess (texture %llu)\n", texture.ptr);
		throw std::exception("ToneMapPostProcess");
	}
	commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSRV, texture);

	// Set constants.
	if (mDirtyFlags & Dirty_Parameters)
	{
		mDirtyFlags &= ~Dirty_Parameters;
		mDirtyFlags |= Dirty_ConstantBuffer;

		constants.parameters = DirectX::XMVectorSet(linearExposure, paperWhiteNits, 0.f, 0.f);
	}

	if (mDirtyFlags & Dirty_ConstantBuffer)
	{
		mDirtyFlags &= ~Dirty_ConstantBuffer;
		mConstantBuffer = GraphicsMemory::Get(mDeviceResources->GetDevice()).AllocateConstant(constants);
	}

	commandList->SetGraphicsRootConstantBufferView(RootParameterIndex::ConstantBuffer, mConstantBuffer.GpuAddress());

	// Set the pipeline state.
	commandList->SetPipelineState(m_ppipelinestate.get());

	// Draw quad.
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->DrawInstanced(3, 1, 0, 0);
}

// Public constructor.
#if defined(_XBOX_ONE) && defined(_TITLE)
ToneMapPostProcess::ToneMapPostProcess(_In_ ID3D12Device* device, const RenderTargetState& rtState, Operator op, TransferFunction func, bool mrt)
	: pimpl(std::make_unique<Impl>(device, rtState, op, func, mrt))
#else
ToneMapPostProcess::ToneMapPostProcess(_In_ ID3D12Device* device, const RenderTargetState& rtState, Operator op, TransferFunction func)
	: pimpl(std::make_unique<Impl>(device, rtState, op, func))
#endif
{
}

// Move constructor.
ToneMapPostProcess::ToneMapPostProcess(ToneMapPostProcess&& moveFrom) noexcept
	: pimpl(std::move(moveFrom.pimpl))
{
}

// Move assignment.
ToneMapPostProcess& ToneMapPostProcess::operator=(ToneMapPostProcess&& moveFrom) noexcept
{
	pimpl = std::move(moveFrom.pimpl);
	return *this;
}

// Public destructor.
ToneMapPostProcess::~ToneMapPostProcess()
{
}

// IPostProcess methods.
void ToneMapPostProcess::Process(_In_ ID3D12GraphicsCommandList* commandList)
{
	pimpl->Process(commandList);
}

// Properties
void ToneMapPostProcess::SetHDRSourceTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor)
{
	pimpl->texture = srvDescriptor;
}

void ToneMapPostProcess::SetExposure(float exposureValue)
{
	pimpl->linearExposure = powf(2.f, exposureValue);
	pimpl->SetDirtyFlag();
}

void ToneMapPostProcess::SetST2084Parameter(float paperWhiteNits)
{
	pimpl->paperWhiteNits = paperWhiteNits;
	pimpl->SetDirtyFlag();
}
