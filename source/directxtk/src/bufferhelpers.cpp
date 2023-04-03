//------------------------------------- -------------------------------------------------
// File: BufferHelpers.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"

#include "d3dx12.h"
#include "bufferhelpers.h"
#include "directxhelpers.h"
#include "resourceuploadbatch.h"
#include "loaderhelpers.h"
#include "platformhelpers.h"

using namespace directxtk;
// using Microsoft::WRL::ComPtr;

//--------------------------------------------------------------------------------------
_Use_decl_annotations_
	HRESULT
	directxtk::CreateStaticBuffer(
		ID3D12Device* device,
		ResourceUploadBatch& resourceUpload,
		const void* ptr,
		size_t count,
		size_t stride,
		D3D12_RESOURCE_STATES afterstate,
		ID3D12Resource** pBuffer,
		D3D12_RESOURCE_FLAGS resFlags) noexcept
{
	if (!pBuffer)
		return E_INVALIDARG;

	*pBuffer = nullptr;

	if (!device || !ptr || !count || !stride)
		return E_INVALIDARG;

	uint64_t sizeInbytes = uint64_t(count) * uint64_t(stride);

	static constexpr uint64_t c_maxBytes = D3D12_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u;

	if (sizeInbytes > c_maxBytes)
	{
		DebugTrace("ERROR: Resource size too large for DirectX 12 (size %llu)\n", sizeInbytes);
		return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
	}

	auto desc = CD3DX12_RESOURCE_DESC::Buffer(sizeInbytes, resFlags);

	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

	wil::com_ptr<ID3D12Resource> res;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_GRAPHICS_PPV_ARGS(res.addressof()));
	if (FAILED(hr))
		return hr;

	D3D12_SUBRESOURCE_DATA initData = {ptr, 0, 0};

	try
	{
		resourceUpload.Upload(res.get(), 0, &initData, 1);

		resourceUpload.Transition(res.get(), D3D12_RESOURCE_STATE_COPY_DEST, afterstate);
	}
	catch (com_exception e)
	{
		return e.get_result();
	}
	catch (...)
	{
		return E_FAIL;
	}

	*pBuffer = res.detach();

	return S_OK;
}

//--------------------------------------------------------------------------------------
_Use_decl_annotations_
	HRESULT
	directxtk::CreateTextureFromMemory(
		ID3D12Device* device,
		ResourceUploadBatch& resourceUpload,
		size_t width,
		DXGI_FORMAT format,
		const D3D12_SUBRESOURCE_DATA& initData,
		ID3D12Resource** texture,
		D3D12_RESOURCE_STATES afterstate,
		D3D12_RESOURCE_FLAGS resFlags) noexcept
{
	if (!texture)
		return E_INVALIDARG;

	*texture = nullptr;

	if (!device || !width || !initData.pData)
		return E_INVALIDARG;

	static_assert(D3D12_REQ_TEXTURE1D_U_DIMENSION <= UINT64_MAX, "Exceeded integer limits");

	if (width > D3D12_REQ_TEXTURE1D_U_DIMENSION)
	{
		DebugTrace("ERROR: Resource dimensions too large for DirectX 12 (1D: size %zu)\n", width);
		return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
	}

	auto desc = CD3DX12_RESOURCE_DESC::Tex1D(format, static_cast<uint64_t>(width), 1u, 1u, resFlags);

	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

	wil::com_ptr<ID3D12Resource> res;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_GRAPHICS_PPV_ARGS(res.addressof()));
	if (FAILED(hr))
		return hr;

	try
	{
		resourceUpload.Upload(res.get(), 0, &initData, 1);

		resourceUpload.Transition(res.get(), D3D12_RESOURCE_STATE_COPY_DEST, afterstate);
	}
	catch (com_exception e)
	{
		return e.get_result();
	}
	catch (...)
	{
		return E_FAIL;
	}

	*texture = res.detach();

	return S_OK;
}

_Use_decl_annotations_
	HRESULT
	directxtk::CreateTextureFromMemory(
		ID3D12Device* device,
		ResourceUploadBatch& resourceUpload,
		size_t width,
		size_t height,
		DXGI_FORMAT format,
		const D3D12_SUBRESOURCE_DATA& initData,
		ID3D12Resource** texture,
		bool generatemips,
		D3D12_RESOURCE_STATES afterstate,
		D3D12_RESOURCE_FLAGS resFlags) noexcept
{
	if (!texture)
		return E_INVALIDARG;

	*texture = nullptr;

	if (!device || !width || !height
		|| !initData.pData || !initData.RowPitch)
		return E_INVALIDARG;

	static_assert(D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION <= UINT32_MAX, "Exceeded integer limits");

	if ((width > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION)
		|| (height > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION))
	{
		DebugTrace("ERROR: Resource dimensions too large for DirectX 12 (2D: size %zu by %zu)\n", width, height);
		return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
	}

	uint16_t mipCount = 1;
	if (generatemips)
	{
		generatemips = resourceUpload.IsSupportedForGenerateMips(format);
		if (generatemips)
		{
			mipCount = static_cast<uint16_t>(LoaderHelpers::CountMips(static_cast<uint32_t>(width), static_cast<uint32_t>(height)));
		}
	}

	auto desc = CD3DX12_RESOURCE_DESC::Tex2D(format, static_cast<uint64_t>(width), static_cast<uint32_t>(height),
		1u, mipCount, 1u, 0u, resFlags);

	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

	wil::com_ptr<ID3D12Resource> res;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_GRAPHICS_PPV_ARGS(res.addressof()));
	if (FAILED(hr))
		return hr;

	try
	{
		resourceUpload.Upload(res.get(), 0, &initData, 1);

		resourceUpload.Transition(res.get(), D3D12_RESOURCE_STATE_COPY_DEST, afterstate);

		if (generatemips)
		{
			resourceUpload.GenerateMips(res.get());
		}
	}
	catch (com_exception e)
	{
		return e.get_result();
	}
	catch (...)
	{
		return E_FAIL;
	}

	*texture = res.detach();

	return S_OK;
}

_Use_decl_annotations_
	HRESULT
	directxtk::CreateTextureFromMemory(
		ID3D12Device* device,
		ResourceUploadBatch& resourceUpload,
		size_t width, size_t height, size_t depth,
		DXGI_FORMAT format,
		const D3D12_SUBRESOURCE_DATA& initData,
		ID3D12Resource** texture,
		D3D12_RESOURCE_STATES afterstate,
		D3D12_RESOURCE_FLAGS resFlags) noexcept
{
	if (!texture)
		return E_INVALIDARG;

	*texture = nullptr;

	if (!device || !width || !height || !depth
		|| !initData.pData || !initData.RowPitch || !initData.SlicePitch)
		return E_INVALIDARG;

	static_assert(D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION <= UINT16_MAX, "Exceeded integer limits");

	if ((width > D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION)
		|| (height > D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION)
		|| (depth > D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION))
	{
		DebugTrace("ERROR: Resource dimensions too large for DirectX 12 (3D: size %zu by %zu by %zu)\n", width, height, depth);
		return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
	}

	auto desc = CD3DX12_RESOURCE_DESC::Tex3D(format,
		static_cast<uint64_t>(width), static_cast<uint32_t>(height), static_cast<uint16_t>(depth),
		1u, resFlags);

	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

	wil::com_ptr<ID3D12Resource> res;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_GRAPHICS_PPV_ARGS(res.addressof()));
	if (FAILED(hr))
		return hr;

	try
	{
		resourceUpload.Upload(res.get(), 0, &initData, 1);

		resourceUpload.Transition(res.get(), D3D12_RESOURCE_STATE_COPY_DEST, afterstate);
	}
	catch (com_exception e)
	{
		return e.get_result();
	}
	catch (...)
	{
		return E_FAIL;
	}

	*texture = res.detach();

	return S_OK;
}
