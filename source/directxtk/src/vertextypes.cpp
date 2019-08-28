//--------------------------------------------------------------------------------------
// File: VertexTypes.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"
#include "vertextypes.h"

using namespace DirectX;

//--------------------------------------------------------------------------------------
// Vertex struct holding position information.
const D3D12_INPUT_ELEMENT_DESC VertexPosition::InputElements[] =
	{
		{"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
};

static_assert(sizeof(VertexPosition) == 12, "Vertex struct/layout mismatch");

const D3D12_INPUT_LAYOUT_DESC VertexPosition::InputLayout =
	{
		VertexPosition::InputElements,
		VertexPosition::InputElementCount};

//--------------------------------------------------------------------------------------
// Vertex struct holding position and color information.
const D3D12_INPUT_ELEMENT_DESC VertexPositioncolour::InputElements[] =
	{
		{"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
};

static_assert(sizeof(VertexPositioncolour) == 28, "Vertex struct/layout mismatch");

const D3D12_INPUT_LAYOUT_DESC VertexPositioncolour::InputLayout =
	{
		VertexPositioncolour::InputElements,
		VertexPositioncolour::InputElementCount};

//--------------------------------------------------------------------------------------
// Vertex struct holding position and texture mapping information.
const D3D12_INPUT_ELEMENT_DESC VertexPositionTexture::InputElements[] =
	{
		{"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
};

static_assert(sizeof(VertexPositionTexture) == 20, "Vertex struct/layout mismatch");

const D3D12_INPUT_LAYOUT_DESC VertexPositionTexture::InputLayout =
	{
		VertexPositionTexture::InputElements,
		VertexPositionTexture::InputElementCount};

//--------------------------------------------------------------------------------------
// Vertex struct holding position and dual texture mapping information.
const D3D12_INPUT_ELEMENT_DESC VertexPositionDualTexture::InputElements[] =
	{
		{"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
};

static_assert(sizeof(VertexPositionDualTexture) == 28, "Vertex struct/layout mismatch");

const D3D12_INPUT_LAYOUT_DESC VertexPositionDualTexture::InputLayout =
	{
		VertexPositionDualTexture::InputElements,
		VertexPositionDualTexture::InputElementCount};

//--------------------------------------------------------------------------------------
// Vertex struct holding position and normal vector.
const D3D12_INPUT_ELEMENT_DESC VertexPositionNormal::InputElements[] =
	{
		{"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
};

static_assert(sizeof(VertexPositionNormal) == 24, "Vertex struct/layout mismatch");

//--------------------------------------------------------------------------------------
// Vertex struct holding position, color, and texture mapping information.
const D3D12_INPUT_ELEMENT_DESC VertexPositioncolourTexture::InputElements[] =
	{
		{"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
};

static_assert(sizeof(VertexPositioncolourTexture) == 36, "Vertex struct/layout mismatch");

const D3D12_INPUT_LAYOUT_DESC VertexPositioncolourTexture::InputLayout =
	{
		VertexPositioncolourTexture::InputElements,
		VertexPositioncolourTexture::InputElementCount};

//--------------------------------------------------------------------------------------
// Vertex struct holding position, normal vector, and color information.
const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalcolour::InputElements[] =
	{
		{"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
};

static_assert(sizeof(VertexPositionNormalcolour) == 40, "Vertex struct/layout mismatch");

const D3D12_INPUT_LAYOUT_DESC VertexPositionNormalcolour::InputLayout =
	{
		VertexPositionNormalcolour::InputElements,
		VertexPositionNormalcolour::InputElementCount};

//--------------------------------------------------------------------------------------
// Vertex struct holding position, normal vector, and texture mapping information.
const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalTexture::InputElements[] =
	{
		{"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
};

static_assert(sizeof(VertexPositionNormalTexture) == 32, "Vertex struct/layout mismatch");

const D3D12_INPUT_LAYOUT_DESC VertexPositionNormalTexture::InputLayout =
	{
		VertexPositionNormalTexture::InputElements,
		VertexPositionNormalTexture::InputElementCount};

//--------------------------------------------------------------------------------------
// Vertex struct holding position, normal vector, color, and texture mapping information.
const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalcolourTexture::InputElements[] =
	{
		{"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
};

static_assert(sizeof(VertexPositionNormalcolourTexture) == 48, "Vertex struct/layout mismatch");

const D3D12_INPUT_LAYOUT_DESC VertexPositionNormalcolourTexture::InputLayout =
	{
		VertexPositionNormalcolourTexture::InputElements,
		VertexPositionNormalcolourTexture::InputElementCount};

//--------------------------------------------------------------------------------------
// VertexPositionNormalTangentcolourTexture, VertexPositionNormalTangentcolourTextureSkinning are not
// supported for DirectX 12 since they were only present for DGSL
