//--------------------------------------------------------------------------------------
// File: DualPostProcess.cpp
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
constexpr int32_t c_MaxSamples = 16;

constexpr int32_t Dirty_ConstantBuffer = 0x01;
constexpr int32_t Dirty_Parameters = 0x02;

// Constant buffer layout. Must match the shader!
__declspec(align(16)) struct PostProcessConstants
{
	DirectX::XMVECTOR sampleOffsets[c_MaxSamples];
	DirectX::XMVECTOR sampleWeights[c_MaxSamples];
};

static_assert((sizeof(PostProcessConstants) % 16) == 0, "CB size not padded correctly");
} // namespace

// Include the precompiled shader code.
namespace
{
#if defined(_XBOX_ONE) && defined(_TITLE)
#include "Shaders/Compiled/XboxOnePostProcess_VSQuadDual.inc"

#include "Shaders/Compiled/XboxOnePostProcess_PSMerge.inc"
#include "Shaders/Compiled/XboxOnePostProcess_PSBloomCombine.inc"
#else
#include "Shaders/Compiled/PostProcess_VSQuadDual.inc"

#include "Shaders/Compiled/PostProcess_PSMerge.inc"
#include "Shaders/Compiled/PostProcess_PSBloomCombine.inc"
#endif
} // namespace

namespace
{
const D3D12_SHADER_BYTECODE vertexShader =
	{PostProcess_VSQuadDual, sizeof(PostProcess_VSQuadDual)};

const D3D12_SHADER_BYTECODE pixelShaders[] =
	{
		{PostProcess_PSMerge, sizeof(PostProcess_PSMerge)},
		{PostProcess_PSBloomCombine, sizeof(PostProcess_PSBloomCombine)},
};

static_assert(_countof(pixelShaders) == DualPostProcess::Effect_Max, "array/max mismatch");

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
                    SetDebugObjectName(*pResult, L"DualPostProcess");

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

class DualPostProcess::Impl : public AlignedNew<PostProcessConstants>
{
public:
	Impl(_In_ ID3D12Device* device, const RenderTargetState& rtState, Effect ifx);

	void Process(_In_ ID3D12GraphicsCommandList* commandList);

	void SetDirtyFlag() noexcept
	{
		mDirtyFlags = INT_MAX;
	}

	enum RootParameterIndex
	{
		TextureSRV,
		TextureSRV2,
		ConstantBuffer,
		RootParameterCount
	};

	// Fields.
	DualPostProcess::Effect fx;
	PostProcessConstants constants;
	D3D12_GPU_DESCRIPTOR_HANDLE texture;
	D3D12_GPU_DESCRIPTOR_HANDLE texture2;
	float mergeWeight1;
	float mergeWeight2;
	float bloomIntensity;
	float bloomBaseIntensity;
	float bloomSaturation;
	float bloomBaseSaturation;

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

// Global pool of per-device DualPostProcess resources.
SharedResourcePool<ID3D12Device*, DeviceResources> DualPostProcess::Impl::deviceResourcesPool;

// Constructor.
DualPostProcess::Impl::Impl(_In_ ID3D12Device* device, const RenderTargetState& rtState, Effect ifx)
	: fx(ifx)
	, constants {}
	, texture {}
	, texture2 {}
	, mergeWeight1(0.5f)
	, mergeWeight2(0.5f)
	, bloomIntensity(1.25f)
	, bloomBaseIntensity(1.f)
	, bloomSaturation(1.f)
	, bloomBaseSaturation(1.f)
	, mDirtyFlags(INT_MAX)
	, mDeviceResources(deviceResourcesPool.DemandCreate(device))
{
	if (ifx >= Effect_Max)
		throw std::out_of_range("Effect not defined");

	// Create root signature.
	{
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

		// Same as CommonStates::StaticLinearClamp
		CD3DX12_STATIC_SAMPLER_DESC sampler(
			0, // register
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
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
		rootParameters[RootParameterIndex::TextureSRV].InitAsDescriptorTable(1, &texture1Range, D3D12_SHADER_VISIBILITY_PIXEL);

		CD3DX12_DESCRIPTOR_RANGE texture2Range(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
		rootParameters[RootParameterIndex::TextureSRV2].InitAsDescriptorTable(1, &texture2Range, D3D12_SHADER_VISIBILITY_PIXEL);

		// Root parameter descriptor
		CD3DX12_ROOT_SIGNATURE_DESC rsigDesc = {};

		// Constant buffer
		rootParameters[RootParameterIndex::ConstantBuffer].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_PIXEL);

		rsigDesc.Init(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

		m_prootsignature = mDeviceResources->GetRootSignature(rsigDesc);
	}

	assert(m_prootsignature != nullptr);

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
		pixelShaders[ifx],
		m_ppipelinestate.addressof());

	SetDebugObjectName(m_ppipelinestate.get(), L"DualPostProcess");
}

// Sets our state onto the D3D device.
void DualPostProcess::Impl::Process(_In_ ID3D12GraphicsCommandList* commandList)
{
	// Set the root signature.
	commandList->SetGraphicsRootSignature(m_prootsignature);

	// Set the texture.
	if (!texture.ptr || !texture2.ptr)
	{
		DebugTrace("ERROR: Missing texture(s) for DualPostProcess (%llu, %llu)\n", texture.ptr, texture2.ptr);
		throw std::exception("DualPostProcess");
	}
	commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSRV, texture);
	commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSRV2, texture2);

	// Set constants.
	if (mDirtyFlags & Dirty_Parameters)
	{
		mDirtyFlags &= ~Dirty_Parameters;
		mDirtyFlags |= Dirty_ConstantBuffer;

		switch (fx)
		{
		case Merge:
			constants.sampleWeights[0] = DirectX::XMVectorReplicate(mergeWeight1);
			constants.sampleWeights[1] = DirectX::XMVectorReplicate(mergeWeight2);
			break;

		case BloomCombine:
			constants.sampleWeights[0] = DirectX::XMVectorSet(bloomBaseSaturation, bloomSaturation, 0.f, 0.f);
			constants.sampleWeights[1] = DirectX::XMVectorReplicate(bloomBaseIntensity);
			constants.sampleWeights[2] = DirectX::XMVectorReplicate(bloomIntensity);
			break;

		default:
			break;
		}
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
DualPostProcess::DualPostProcess(_In_ ID3D12Device* device, const RenderTargetState& rtState, Effect fx)
	: pimpl(std::make_unique<Impl>(device, rtState, fx))
{
}

// Move constructor.
DualPostProcess::DualPostProcess(DualPostProcess&& moveFrom) noexcept
	: pimpl(std::move(moveFrom.pimpl))
{
}

// Move assignment.
DualPostProcess& DualPostProcess::operator=(DualPostProcess&& moveFrom) noexcept
{
	pimpl = std::move(moveFrom.pimpl);
	return *this;
}

// Public destructor.
DualPostProcess::~DualPostProcess()
{
}

// IPostProcess methods.
void DualPostProcess::Process(_In_ ID3D12GraphicsCommandList* commandList)
{
	pimpl->Process(commandList);
}

// Properties
void DualPostProcess::SetSourceTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor)
{
	pimpl->texture = srvDescriptor;
}

void DualPostProcess::SetSourceTexture2(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor)
{
	pimpl->texture2 = srvDescriptor;
}

void DualPostProcess::SetMergeParameters(float weight1, float weight2)
{
	pimpl->mergeWeight1 = weight1;
	pimpl->mergeWeight2 = weight2;
	pimpl->SetDirtyFlag();
}

void DualPostProcess::SetBloomCombineParameters(float bloom, float base, float bloomSaturation, float baseSaturation)
{
	pimpl->bloomIntensity = bloom;
	pimpl->bloomBaseIntensity = base;
	pimpl->bloomSaturation = bloomSaturation;
	pimpl->bloomBaseSaturation = baseSaturation;
	pimpl->SetDirtyFlag();
}
