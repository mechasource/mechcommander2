//--------------------------------------------------------------------------------------
// File: VertexTypes.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#pragma once

#include <d3d12.h>

#include <DirectXMath.h>

namespace DirectX
{
// Vertex struct holding position information.
struct VertexPosition
{
	VertexPosition() = default;

	VertexPosition(const VertexPosition&) = default;
	VertexPosition& operator=(const VertexPosition&) = default;

	VertexPosition(VertexPosition&&) = default;
	VertexPosition& operator=(VertexPosition&&) = default;

	VertexPosition(XMFLOAT3 const& iposition) :
		position(iposition)
	{
	}

	VertexPosition(FXMVECTOR iposition)
	{
		XMStoreFloat3(&this->position, iposition);
	}

	XMFLOAT3 position;

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
	static const int InputElementCount = 1;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

// Vertex struct holding position and color information.
struct VertexPositioncolour
{
	VertexPositioncolour() = default;

	VertexPositioncolour(const VertexPositioncolour&) = default;
	VertexPositioncolour& operator=(const VertexPositioncolour&) = default;

	VertexPositioncolour(VertexPositioncolour&&) = default;
	VertexPositioncolour& operator=(VertexPositioncolour&&) = default;

	VertexPositioncolour(XMFLOAT3 const& iposition, XMFLOAT4 const& icolor) :
		position(iposition),
		color(icolor)
	{
	}

	VertexPositioncolour(FXMVECTOR iposition, FXMVECTOR icolor)
	{
		XMStoreFloat3(&this->position, iposition);
		XMStoreFloat4(&this->color, icolor);
	}

	XMFLOAT3 position;
	XMFLOAT4 color;

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
	static const int InputElementCount = 2;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

// Vertex struct holding position and texture mapping information.
struct VertexPositionTexture
{
	VertexPositionTexture() = default;

	VertexPositionTexture(const VertexPositionTexture&) = default;
	VertexPositionTexture& operator=(const VertexPositionTexture&) = default;

	VertexPositionTexture(VertexPositionTexture&&) = default;
	VertexPositionTexture& operator=(VertexPositionTexture&&) = default;

	VertexPositionTexture(XMFLOAT3 const& iposition, XMFLOAT2 const& itextureCoordinate) :
		position(iposition),
		textureCoordinate(itextureCoordinate)
	{
	}

	VertexPositionTexture(FXMVECTOR iposition, FXMVECTOR itextureCoordinate)
	{
		XMStoreFloat3(&this->position, iposition);
		XMStoreFloat2(&this->textureCoordinate, itextureCoordinate);
	}

	XMFLOAT3 position;
	XMFLOAT2 textureCoordinate;

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
	static const int InputElementCount = 2;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

// Vertex struct holding position and dual texture mapping information.
struct VertexPositionDualTexture
{
	VertexPositionDualTexture() = default;

	VertexPositionDualTexture(const VertexPositionDualTexture&) = default;
	VertexPositionDualTexture& operator=(const VertexPositionDualTexture&) = default;

	VertexPositionDualTexture(VertexPositionDualTexture&&) = default;
	VertexPositionDualTexture& operator=(VertexPositionDualTexture&&) = default;

	VertexPositionDualTexture(XMFLOAT3 const& iposition, XMFLOAT2 const& itextureCoordinate0, XMFLOAT2 const& itextureCoordinate1) :
		position(iposition),
		textureCoordinate0(itextureCoordinate0),
		textureCoordinate1(itextureCoordinate1)
	{
	}

	VertexPositionDualTexture(FXMVECTOR iposition,
		FXMVECTOR itextureCoordinate0,
		FXMVECTOR itextureCoordinate1)
	{
		XMStoreFloat3(&this->position, iposition);
		XMStoreFloat2(&this->textureCoordinate0, itextureCoordinate0);
		XMStoreFloat2(&this->textureCoordinate1, itextureCoordinate1);
	}

	XMFLOAT3 position;
	XMFLOAT2 textureCoordinate0;
	XMFLOAT2 textureCoordinate1;

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
	static const int InputElementCount = 3;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

// Vertex struct holding position and normal vector.
struct VertexPositionNormal
{
	VertexPositionNormal() = default;

	VertexPositionNormal(const VertexPositionNormal&) = default;
	VertexPositionNormal& operator=(const VertexPositionNormal&) = default;

	VertexPositionNormal(VertexPositionNormal&&) = default;
	VertexPositionNormal& operator=(VertexPositionNormal&&) = default;

	VertexPositionNormal(XMFLOAT3 const& iposition, XMFLOAT3 const& inormal) :
		position(iposition),
		normal(inormal)
	{
	}

	VertexPositionNormal(FXMVECTOR iposition, FXMVECTOR inormal)
	{
		XMStoreFloat3(&this->position, iposition);
		XMStoreFloat3(&this->normal, inormal);
	}

	XMFLOAT3 position;
	XMFLOAT3 normal;

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
	static const int InputElementCount = 2;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

// Vertex struct holding position, color, and texture mapping information.
struct VertexPositioncolourTexture
{
	VertexPositioncolourTexture() = default;

	VertexPositioncolourTexture(const VertexPositioncolourTexture&) = default;
	VertexPositioncolourTexture& operator=(const VertexPositioncolourTexture&) = default;

	VertexPositioncolourTexture(VertexPositioncolourTexture&&) = default;
	VertexPositioncolourTexture& operator=(VertexPositioncolourTexture&&) = default;

	VertexPositioncolourTexture(XMFLOAT3 const& iposition, XMFLOAT4 const& icolor, XMFLOAT2 const& itextureCoordinate) :
		position(iposition),
		color(icolor),
		textureCoordinate(itextureCoordinate)
	{
	}

	VertexPositioncolourTexture(FXMVECTOR iposition, FXMVECTOR icolor, FXMVECTOR itextureCoordinate)
	{
		XMStoreFloat3(&this->position, iposition);
		XMStoreFloat4(&this->color, icolor);
		XMStoreFloat2(&this->textureCoordinate, itextureCoordinate);
	}

	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT2 textureCoordinate;

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
	static const int InputElementCount = 3;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

// Vertex struct holding position, normal vector, and color information.
struct VertexPositionNormalcolour
{
	VertexPositionNormalcolour() = default;

	VertexPositionNormalcolour(const VertexPositionNormalcolour&) = default;
	VertexPositionNormalcolour& operator=(const VertexPositionNormalcolour&) = default;

	VertexPositionNormalcolour(VertexPositionNormalcolour&&) = default;
	VertexPositionNormalcolour& operator=(VertexPositionNormalcolour&&) = default;

	VertexPositionNormalcolour(XMFLOAT3 const& iposition, XMFLOAT3 const& inormal, XMFLOAT4 const& icolor) :
		position(iposition),
		normal(inormal),
		color(icolor)
	{
	}

	VertexPositionNormalcolour(FXMVECTOR iposition, FXMVECTOR inormal, FXMVECTOR icolor)
	{
		XMStoreFloat3(&this->position, iposition);
		XMStoreFloat3(&this->normal, inormal);
		XMStoreFloat4(&this->color, icolor);
	}

	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
	static const int InputElementCount = 3;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

// Vertex struct holding position, normal vector, and texture mapping information.
struct VertexPositionNormalTexture
{
	VertexPositionNormalTexture() = default;

	VertexPositionNormalTexture(const VertexPositionNormalTexture&) = default;
	VertexPositionNormalTexture& operator=(const VertexPositionNormalTexture&) = default;

	VertexPositionNormalTexture(VertexPositionNormalTexture&&) = default;
	VertexPositionNormalTexture& operator=(VertexPositionNormalTexture&&) = default;

	VertexPositionNormalTexture(XMFLOAT3 const& iposition, XMFLOAT3 const& inormal, XMFLOAT2 const& itextureCoordinate) :
		position(iposition),
		normal(inormal),
		textureCoordinate(itextureCoordinate)
	{
	}

	VertexPositionNormalTexture(FXMVECTOR iposition, FXMVECTOR inormal, FXMVECTOR itextureCoordinate)
	{
		XMStoreFloat3(&this->position, iposition);
		XMStoreFloat3(&this->normal, inormal);
		XMStoreFloat2(&this->textureCoordinate, itextureCoordinate);
	}

	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 textureCoordinate;

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
	static const int InputElementCount = 3;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

// Vertex struct holding position, normal vector, color, and texture mapping information.
struct VertexPositionNormalcolourTexture
{
	VertexPositionNormalcolourTexture() = default;

	VertexPositionNormalcolourTexture(const VertexPositionNormalcolourTexture&) = default;
	VertexPositionNormalcolourTexture& operator=(const VertexPositionNormalcolourTexture&) = default;

	VertexPositionNormalcolourTexture(VertexPositionNormalcolourTexture&&) = default;
	VertexPositionNormalcolourTexture& operator=(VertexPositionNormalcolourTexture&&) = default;

	VertexPositionNormalcolourTexture(XMFLOAT3 const& iposition, XMFLOAT3 const& inormal, XMFLOAT4 const& icolor, XMFLOAT2 const& itextureCoordinate) :
		position(iposition),
		normal(inormal),
		color(icolor),
		textureCoordinate(itextureCoordinate)
	{
	}

	VertexPositionNormalcolourTexture(FXMVECTOR iposition, FXMVECTOR inormal, FXMVECTOR icolor, CXMVECTOR itextureCoordinate)
	{
		XMStoreFloat3(&this->position, iposition);
		XMStoreFloat3(&this->normal, inormal);
		XMStoreFloat4(&this->color, icolor);
		XMStoreFloat2(&this->textureCoordinate, itextureCoordinate);
	}

	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	XMFLOAT2 textureCoordinate;

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
	static const int InputElementCount = 4;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};
} // namespace DirectX
