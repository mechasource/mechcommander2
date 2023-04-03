//--------------------------------------------------------------------------------------
// File: DirectXHelpers.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"
#include "directxhelpers.h"
#include "graphicsmemory.h"
#include "platformhelpers.h"

using namespace directxtk;

_Use_decl_annotations_ void directxtk::CreateShaderResourceView(
	ID3D12Device* device,
	ID3D12Resource* tex,
	D3D12_CPU_DESCRIPTOR_HANDLE srvDescriptor,
	bool iscubemap)
{
	const auto desc = tex->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	switch (desc.Dimension)
	{
	case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
		if (desc.DepthOrArraySize > 1)
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
			srvDesc.Texture1DArray.MipLevels = (!desc.MipLevels) ? uint32_t(-1) : desc.MipLevels;
			srvDesc.Texture1DArray.ArraySize = static_cast<uint32_t>(desc.DepthOrArraySize);
		}
		else
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
			srvDesc.Texture1D.MipLevels = (!desc.MipLevels) ? uint32_t(-1) : desc.MipLevels;
		}
		break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
		if (iscubemap)
		{
			if (desc.DepthOrArraySize > 6)
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
				srvDesc.TextureCubeArray.MipLevels = (!desc.MipLevels) ? uint32_t(-1) : desc.MipLevels;
				srvDesc.TextureCubeArray.NumCubes = static_cast<uint32_t>(desc.DepthOrArraySize / 6);
			}
			else
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				srvDesc.TextureCube.MipLevels = (!desc.MipLevels) ? uint32_t(-1) : desc.MipLevels;
			}
		}
		else if (desc.DepthOrArraySize > 1)
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.MipLevels = (!desc.MipLevels) ? uint32_t(-1) : desc.MipLevels;
			srvDesc.Texture2DArray.ArraySize = static_cast<uint32_t>(desc.DepthOrArraySize);
		}
		else
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = (!desc.MipLevels) ? uint32_t(-1) : desc.MipLevels;
		}
		break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
		srvDesc.Texture3D.MipLevels = (!desc.MipLevels) ? uint32_t(-1) : desc.MipLevels;
		break;

	case D3D12_RESOURCE_DIMENSION_BUFFER:
		DebugTrace("ERROR: CreateShaderResourceView cannot be used with DIMENSION_BUFFER.\n");
		throw std::exception("buffer resources not supported");

	case D3D12_RESOURCE_DIMENSION_UNKNOWN:
	default:
		DebugTrace("ERROR: CreateShaderResourceView cannot be used with DIMENSION_UNKNOWN (%d).\n", desc.Dimension);
		throw std::exception("unknown resource dimension");
	}

	device->CreateShaderResourceView(tex, &srvDesc, srvDescriptor);
}
