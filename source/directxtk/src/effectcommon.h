//--------------------------------------------------------------------------------------
// File: EffectCommon.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#pragma once

#include <memory>

#include "d3dx12.h"
#include "effects.h"
#include "platformhelpers.h"
#include "sharedresourcepool.h"
#include "alignednew.h"
#include "descriptorheap.h"
#include "graphicsmemory.h"
#include "directxhelpers.h"
#include "rendertargetstate.h"

// BasicEffect, SkinnedEffect, et al, have many things in common, but also significant
// differences (for instance, not all the effects support lighting). This header breaks
// out common functionality into a set of helpers which can be assembled in different
// combinations to build up whatever subset is needed by each effect.

namespace directxtk
{
// Internal effect flags
namespace EffectFlags
{
constexpr int32_t PerPixelLightingBit = 0x04;
}

static_assert(((EffectFlags::PerPixelLighting)&EffectFlags::PerPixelLightingBit) != 0, "PerPixelLighting enum flags mismatch");

// Bitfield tracks which derived parameter values need to be recomputed.
namespace EffectDirtyFlags
{
constexpr int32_t ConstantBuffer = 0x01;
constexpr int32_t WorldViewProj = 0x02;
constexpr int32_t WorldInverseTranspose = 0x04;
constexpr int32_t EyePosition = 0x08;
constexpr int32_t MaterialColor = 0x10;
constexpr int32_t FogVector = 0x20;
constexpr int32_t FogEnable = 0x40;
constexpr int32_t AlphaTest = 0x80;
} // namespace EffectDirtyFlags

// Helper stores matrix parameter values, and computes derived matrices.
struct EffectMatrices
{
	EffectMatrices() noexcept;

	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX worldView;

	void SetConstants(_Inout_ int32_t& dirtyFlags, _Inout_ DirectX::XMMATRIX& worldViewProjConstant);
};

// Helper stores the current fog settings, and computes derived shader parameters.
struct EffectFog
{
	EffectFog() noexcept;

	bool enabled;
	float start;
	float end;

	void XM_CALLCONV SetConstants(_Inout_ int32_t& dirtyFlags, _In_ DirectX::FXMMATRIX worldView, _Inout_ DirectX::XMVECTOR& fogVectorConstant);
};

// Helper stores material color settings, and computes derived parameters for shaders that do not support realtime lighting.
struct EffectColor
{
	EffectColor() noexcept;

	DirectX::XMVECTOR diffuseColor;
	float alpha;

	void SetConstants(_Inout_ int32_t& dirtyFlags, _Inout_ DirectX::XMVECTOR& diffuseColorConstant);
};

// Helper stores the current light settings, and computes derived shader parameters.
struct EffectLights : public EffectColor
{
	EffectLights() noexcept;

	static constexpr int32_t MaxDirectionalLights = IEffectLights::MaxDirectionalLights;

	// Fields.
	DirectX::XMVECTOR emissiveColor;
	DirectX::XMVECTOR ambientLightColor;

	bool lightEnabled[MaxDirectionalLights];
	DirectX::XMVECTOR lightDiffuseColor[MaxDirectionalLights];
	DirectX::XMVECTOR lightSpecularColor[MaxDirectionalLights];

	// Methods.
	void InitializeConstants(_Out_ DirectX::XMVECTOR& specularColorAndPowerConstant, _Out_writes_all_(MaxDirectionalLights) DirectX::XMVECTOR* lightDirectionConstant, _Out_writes_all_(MaxDirectionalLights) DirectX::XMVECTOR* lightDiffuseConstant, _Out_writes_all_(MaxDirectionalLights) DirectX::XMVECTOR* lightSpecularConstant) const;
	void SetConstants(_Inout_ int32_t& dirtyFlags, _In_ EffectMatrices const& matrices, _Inout_ DirectX::XMMATRIX& worldConstant, _Inout_updates_(3) DirectX::XMVECTOR worldInverseTransposeConstant[3], _Inout_ DirectX::XMVECTOR& eyePositionConstant, _Inout_ DirectX::XMVECTOR& diffuseColorConstant, _Inout_ DirectX::XMVECTOR& emissiveColorConstant, bool lightingEnabled);

	int32_t SetLightEnabled(int32_t whichLight, bool value, _Inout_updates_(MaxDirectionalLights) DirectX::XMVECTOR* lightDiffuseConstant, _Inout_updates_(MaxDirectionalLights) DirectX::XMVECTOR* lightSpecularConstant);
	int32_t XM_CALLCONV SetLightDiffuseColor(int32_t whichLight, DirectX::FXMVECTOR value, _Inout_updates_(MaxDirectionalLights) DirectX::XMVECTOR* lightDiffuseConstant);
	int32_t XM_CALLCONV SetLightSpecularColor(int32_t whichLight, DirectX::FXMVECTOR value, _Inout_updates_(MaxDirectionalLights) DirectX::XMVECTOR* lightSpecularConstant);

	static void ValidateLightIndex(int32_t whichLight);
	static void EnableDefaultLighting(_In_ IEffectLights* effect);
};

// Factory for lazily instantiating shared root signatures.
class EffectDeviceResources
{
public:
	EffectDeviceResources(_In_ ID3D12Device* pdevice) noexcept
		: m_pdevice(pdevice)
	{
	}

	ID3D12RootSignature* DemandCreateRootSig(_Inout_ wil::com_ptr<ID3D12RootSignature>& prootsign, D3D12_ROOT_SIGNATURE_DESC const& desc);

protected:
	wil::com_ptr<ID3D12Device> m_pdevice;
	std::mutex m_mutex;
};

// Templated base class provides functionality common to all the built-in effects.
template <typename Traits>
class EffectBase : public AlignedNew<typename Traits::ConstantBufferType>
{
public:
	typename Traits::ConstantBufferType constants;

	// Constructor.
	EffectBase(_In_ ID3D12Device* device)
		: constants {}
		, dirtyFlags(INT_MAX)
		, m_prootsignature(nullptr)
		, mDeviceResources(deviceResourcesPool.DemandCreate(device))
	{
		// Initialize the constant buffer data
		mConstantBuffer = GraphicsMemory::Get(device).AllocateConstant(constants);
	}

	// Commits constants to the constant buffer memory
	void UpdateConstants()
	{
		// Make sure the constant buffer is up to date.
		if (dirtyFlags & EffectDirtyFlags::ConstantBuffer)
		{
			mConstantBuffer = GraphicsMemory::Get(mDeviceResources->GetDevice()).AllocateConstant(constants);

			dirtyFlags &= ~EffectDirtyFlags::ConstantBuffer;
		}
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetConstantBufferGpuAddress() noexcept
	{
		return mConstantBuffer.GpuAddress();
	}

	ID3D12RootSignature* GetRootSignature(int32_t slot, CD3DX12_ROOT_SIGNATURE_DESC const& rootSig)
	{
		return mDeviceResources->GetRootSignature(slot, rootSig);
	}

	// Fields.
	EffectMatrices matrices;
	EffectFog fog;
	int32_t dirtyFlags;

protected:
	// Static arrays hold all the precompiled shader permutations.
	static const D3D12_SHADER_BYTECODE VertexShaderBytecode[Traits::VertexShaderCount];
	static const D3D12_SHADER_BYTECODE PixelShaderBytecode[Traits::PixelShaderCount];
	// .. and shader entry points
	static const int32_t VertexShaderIndices[Traits::ShaderPermutationCount];
	static const int32_t PixelShaderIndices[Traits::ShaderPermutationCount];
	// ... and vertex layout tables
	static const D3D12_INPUT_LAYOUT_DESC VertexShaderInputLayouts[Traits::ShaderPermutationCount];

	// Per instance cache of PSOs, populated with variants for each shader & layout
	wil::com_ptr<ID3D12PipelineState> m_ppipelinestate;

	// Per instance root signature
	ID3D12RootSignature* m_prootsignature;

private:
	// D3D constant buffer holds a copy of the same data as the public 'constants' field.
	GraphicsResource mConstantBuffer;

	// Only one of these helpers is allocated per D3D device, even if there are multiple effect instances.
	class DeviceResources : public EffectDeviceResources
	{
	public:
		DeviceResources(_In_ ID3D12Device* device) noexcept
			: EffectDeviceResources(device)
			, m_prootsignature {}
		{
		}

		// Gets or lazily creates the specified root signature
		ID3D12RootSignature* GetRootSignature(int32_t slot, D3D12_ROOT_SIGNATURE_DESC const& desc)
		{
			assert(slot >= 0 && slot < Traits::RootSignatureCount);
			_Analysis_assume_(slot >= 0 && slot < Traits::RootSignatureCount);

			return DemandCreateRootSig(m_prootsignature[slot], desc);
		}

		ID3D12Device* GetDevice() const noexcept
		{
			return m_pdevice.get();
		}

	private:
		wil::com_ptr<ID3D12RootSignature> m_prootsignature[Traits::RootSignatureCount];
	};

	// Per-device resources.
	std::shared_ptr<DeviceResources> mDeviceResources;

	static SharedResourcePool<ID3D12Device*, DeviceResources> deviceResourcesPool;
};
} // namespace directxtk
