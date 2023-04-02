//--------------------------------------------------------------------------------------
// File: BufferHelpers.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#pragma once

#if defined(_XBOX_ONE) && defined(_TITLE)
#include <d3d12_x.h>
#else
#include <d3d12.h>
#endif


namespace directxtk
{
    class ResourceUploadBatch;

    // Helpers for creating initialized Direct3D buffer resources.
    HRESULT __cdecl CreateStaticBuffer(_In_ ID3D12Device* device,
        ResourceUploadBatch& resourceUpload,
        _In_reads_bytes_(count* stride) const void* ptr,
        size_t count,
        size_t stride,
        D3D12_RESOURCE_STATES afterstate,
        _COM_Outptr_ ID3D12Resource** pBuffer,
        D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE) noexcept;

    template<typename T>
    HRESULT CreateStaticBuffer(_In_ ID3D12Device* device,
        ResourceUploadBatch& resourceUpload,
        _In_reads_(count) T const* data,
        size_t count,
        D3D12_RESOURCE_STATES afterstate,
        _COM_Outptr_ ID3D12Resource** pBuffer,
        D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE) noexcept
    {
        return CreateStaticBuffer(device, resourceUpload, data, count, sizeof(T), afterstate, pBuffer, resFlags);
    }

    template<typename T>
    HRESULT CreateStaticBuffer(_In_ ID3D12Device* device,
        ResourceUploadBatch& resourceUpload,
        T const& data,
        D3D12_RESOURCE_STATES afterstate,
        _COM_Outptr_ ID3D12Resource** pBuffer,
        D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE) noexcept
    {
        return CreateStaticBuffer(device, resourceUpload, data.data(), data.size(), sizeof(typename T::value_type),
            afterstate, pBuffer, resFlags);
    }

    // Helpers for creating texture from memory arrays.
    HRESULT __cdecl CreateTextureFromMemory(_In_ ID3D12Device* device,
        ResourceUploadBatch& resourceUpload,
        size_t width,
        DXGI_FORMAT format,
        const D3D12_SUBRESOURCE_DATA& initData,
        _COM_Outptr_ ID3D12Resource** texture,
        D3D12_RESOURCE_STATES afterstate = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE) noexcept;

    HRESULT __cdecl CreateTextureFromMemory(_In_ ID3D12Device* device,
        ResourceUploadBatch& resourceUpload,
        size_t width, size_t height,
        DXGI_FORMAT format,
        const D3D12_SUBRESOURCE_DATA& initData,
        _COM_Outptr_ ID3D12Resource** texture,
        bool generatemips = false,
        D3D12_RESOURCE_STATES afterstate = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE) noexcept;

    HRESULT __cdecl CreateTextureFromMemory(_In_ ID3D12Device* device,
        ResourceUploadBatch& resourceUpload,
        size_t width, size_t height, size_t depth,
        DXGI_FORMAT format,
        const D3D12_SUBRESOURCE_DATA& initData,
        _COM_Outptr_ ID3D12Resource** texture,
        D3D12_RESOURCE_STATES afterstate = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE) noexcept;
}
