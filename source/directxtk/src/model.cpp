//--------------------------------------------------------------------------------------
// File: Model.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"

#include "d3dx12.h"
#include "Model.h"
#include "commonstates.h"
#include "descriptorheap.h"
#include "directxhelpers.h"
#include "effects.h"
#include "platformhelpers.h"
#include "resourceuploadbatch.h"

using namespace directxtk;

#ifndef _CPPRTTI
#error Model requires RTTI
#endif

//--------------------------------------------------------------------------------------
// ModelMeshPart
//--------------------------------------------------------------------------------------

ModelMeshPart::ModelMeshPart(uint32_t ipartIndex) noexcept
	: partIndex(ipartIndex)
	, materialIndex(0)
	, indexCount(0)
	, startIndex(0)
	, vertexOffset(0)
	, vertexStride(0)
	, vertexCount(0)
	, indexBufferSize(0)
	, vertexBufferSize(0)
	, primitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, indexFormat(DXGI_FORMAT_R16_UINT)
{
}

ModelMeshPart::~ModelMeshPart()
{
}

_Use_decl_annotations_ void ModelMeshPart::Draw(_In_ ID3D12GraphicsCommandList* commandList) const
{
	if (!indexBufferSize || !vertexBufferSize)
	{
		DebugTrace("ERROR: Model part missing values for vertex and/or index buffer size (indexBufferSize %u, vertexBufferSize %u)!\n", indexBufferSize, vertexBufferSize);
		throw std::exception("ModelMeshPart");
	}

	if (!staticIndexBuffer && !indexBuffer)
	{
		DebugTrace("ERROR: Model part missing index buffer!\n");
		throw std::exception("ModelMeshPart");
	}

	if (!staticVertexBuffer && !vertexBuffer)
	{
		DebugTrace("ERROR: Model part missing vertex buffer!\n");
		throw std::exception("ModelMeshPart");
	}

	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = staticVertexBuffer ? staticVertexBuffer->GetGPUVirtualAddress() : vertexBuffer.GpuAddress();
	vbv.StrideInBytes = vertexStride;
	vbv.SizeInBytes = vertexBufferSize;
	commandList->IASetVertexBuffers(0, 1, &vbv);

	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = staticIndexBuffer ? staticIndexBuffer->GetGPUVirtualAddress() : indexBuffer.GpuAddress();
	ibv.SizeInBytes = indexBufferSize;
	ibv.Format = indexFormat;
	commandList->IASetIndexBuffer(&ibv);

	commandList->IASetPrimitiveTopology(primitiveType);

	commandList->DrawIndexedInstanced(indexCount, 1, startIndex, vertexOffset, 0);
}

_Use_decl_annotations_ void ModelMeshPart::DrawInstanced(_In_ ID3D12GraphicsCommandList* commandList, uint32_t instanceCount, uint32_t startInstanceLocation) const
{
	if (!indexBufferSize || !vertexBufferSize)
	{
		DebugTrace("ERROR: Model part missing values for vertex and/or index buffer size (indexBufferSize %u, vertexBufferSize %u)!\n", indexBufferSize, vertexBufferSize);
		throw std::exception("ModelMeshPart");
	}

	if (!staticIndexBuffer && !indexBuffer)
	{
		DebugTrace("ERROR: Model part missing index buffer!\n");
		throw std::exception("ModelMeshPart");
	}

	if (!staticVertexBuffer && !vertexBuffer)
	{
		DebugTrace("ERROR: Model part missing vertex buffer!\n");
		throw std::exception("ModelMeshPart");
	}

	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = staticVertexBuffer ? staticVertexBuffer->GetGPUVirtualAddress() : vertexBuffer.GpuAddress();
	vbv.StrideInBytes = vertexStride;
	vbv.SizeInBytes = vertexBufferSize;
	commandList->IASetVertexBuffers(0, 1, &vbv);

	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = staticIndexBuffer ? staticIndexBuffer->GetGPUVirtualAddress() : indexBuffer.GpuAddress();
	ibv.SizeInBytes = indexBufferSize;
	ibv.Format = indexFormat;
	commandList->IASetIndexBuffer(&ibv);

	commandList->IASetPrimitiveTopology(primitiveType);

	commandList->DrawIndexedInstanced(indexCount, instanceCount, startIndex, vertexOffset, startInstanceLocation);
}

_Use_decl_annotations_ void ModelMeshPart::DrawMeshParts(ID3D12GraphicsCommandList* commandList, const ModelMeshPart::Collection& meshParts)
{
	for (auto it = meshParts.cbegin(); it != meshParts.cend(); ++it)
	{
		auto part = (*it).get();
		assert(part != nullptr);

		part->Draw(commandList);
	}
}

_Use_decl_annotations_ void ModelMeshPart::DrawMeshParts(
	ID3D12GraphicsCommandList* commandList,
	const ModelMeshPart::Collection& meshParts,
	ModelMeshPart::DrawCallback callback)
{
	for (auto it = meshParts.cbegin(); it != meshParts.cend(); ++it)
	{
		auto part = (*it).get();
		assert(part != nullptr);

		callback(commandList, *part);
		part->Draw(commandList);
	}
}

_Use_decl_annotations_ void ModelMeshPart::DrawMeshParts(ID3D12GraphicsCommandList* commandList,
	const ModelMeshPart::Collection& meshParts,
	IEffect* effect)
{
	effect->Apply(commandList);
	DrawMeshParts(commandList, meshParts);
}

//--------------------------------------------------------------------------------------
// ModelMesh
//--------------------------------------------------------------------------------------

ModelMesh::ModelMesh() noexcept
{
}

ModelMesh::~ModelMesh()
{
}

// Draw the mesh
void __cdecl ModelMesh::DrawOpaque(_In_ ID3D12GraphicsCommandList* commandList) const
{
	ModelMeshPart::DrawMeshParts(commandList, opaqueMeshParts);
}

void __cdecl ModelMesh::DrawAlpha(_In_ ID3D12GraphicsCommandList* commandList) const
{
	ModelMeshPart::DrawMeshParts(commandList, alphaMeshParts);
}

// Draw the mesh with an effect
void __cdecl ModelMesh::DrawOpaque(_In_ ID3D12GraphicsCommandList* commandList, _In_ IEffect* effect) const
{
	ModelMeshPart::DrawMeshParts(commandList, opaqueMeshParts, effect);
}

void __cdecl ModelMesh::DrawAlpha(_In_ ID3D12GraphicsCommandList* commandList, _In_ IEffect* effect) const
{
	ModelMeshPart::DrawMeshParts(commandList, alphaMeshParts, effect);
}

// Draw the mesh with a callback for each mesh part
void __cdecl ModelMesh::DrawOpaque(_In_ ID3D12GraphicsCommandList* commandList, ModelMeshPart::DrawCallback callback) const
{
	ModelMeshPart::DrawMeshParts(commandList, opaqueMeshParts, callback);
}

void __cdecl ModelMesh::DrawAlpha(_In_ ID3D12GraphicsCommandList* commandList, ModelMeshPart::DrawCallback callback) const
{
	ModelMeshPart::DrawMeshParts(commandList, alphaMeshParts, callback);
}

//--------------------------------------------------------------------------------------
// Model
//--------------------------------------------------------------------------------------
Model::Model() noexcept
{
}

Model::~Model()
{
}

// Load texture resources
int32_t Model::LoadTextures(IEffectTextureFactory& texFactory, int32_t destinationDescriptorOffset) const
{
	for (auto i = 0u; i < texturenames.size(); ++i)
	{
		texFactory.CreateTexture(texturenames[i].c_str(), destinationDescriptorOffset + static_cast<int32_t>(i));
	}

	return static_cast<int32_t>(texturenames.size());
}

// Load texture resources (helper function)
_Use_decl_annotations_
	std::unique_ptr<EffectTextureFactory>
	Model::LoadTextures(
		ID3D12Device* device,
		ResourceUploadBatch& resourceuploadbatch,
		std::wstring_view texturespath,
		D3D12_DESCRIPTOR_HEAP_FLAGS flags) const
{
	if (texturenames.empty())
		return nullptr;

	std::unique_ptr<EffectTextureFactory> texFactory = std::make_unique<EffectTextureFactory>(
		device,
		resourceuploadbatch,
		texturenames.size(),
		flags);
	if (!texturespath.empty())
	{
		texFactory->SetDirectory(texturespath);
	}

	LoadTextures(*texFactory);

	return texFactory;
}

// Load VB/IB resources for static geometry
_Use_decl_annotations_ void Model::LoadStaticBuffers(
	ID3D12Device* device,
	ResourceUploadBatch& resourceuploadbatch,
	bool keepMemory)
{
	// Gather all unique parts
	std::set<ModelMeshPart*> uniqueParts;
	for (const auto& mesh : meshes)
	{
		for (const auto& part : mesh->opaqueMeshParts)
		{
			uniqueParts.insert(part.get());
		}
		for (const auto& part : mesh->alphaMeshParts)
		{
			uniqueParts.insert(part.get());
		}
	}

	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

	for (auto it = uniqueParts.cbegin(); it != uniqueParts.cend(); ++it)
	{
		auto part = *it;

		// Convert dynamic VB to static VB
		if (!part->staticVertexBuffer)
		{
			if (!part->vertexBuffer)
			{
				DebugTrace("ERROR: Model part missing vertex buffer!\n");
				throw std::exception("ModelMeshPart");
			}

			part->vertexBufferSize = static_cast<uint32_t>(part->vertexBuffer.Size());

			auto desc = CD3DX12_RESOURCE_DESC::Buffer(part->vertexBuffer.Size());

			ThrowIfFailed(device->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_GRAPHICS_PPV_ARGS(part->staticVertexBuffer.addressof())));

			SetDebugObjectName(part->staticVertexBuffer.get(), L"ModelMeshPart");

			resourceuploadbatch.Upload(part->staticVertexBuffer.get(), part->vertexBuffer);

			resourceuploadbatch.Transition(part->staticVertexBuffer.get(),
				D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

			// Scan for any other part with the same vertex buffer for sharing
			for (auto sit = std::next(it); sit != uniqueParts.cend(); ++sit)
			{
				auto sharePart = *sit;
				assert(sharePart != part);

				if (sharePart->staticVertexBuffer)
					continue;

				if (sharePart->vertexBuffer == part->vertexBuffer)
				{
					sharePart->vertexBufferSize = part->vertexBufferSize;
					sharePart->staticVertexBuffer = part->staticVertexBuffer;

					if (!keepMemory)
					{
						sharePart->vertexBuffer.reset();
					}
				}
			}

			if (!keepMemory)
			{
				part->vertexBuffer.reset();
			}
		}

		// Convert dynamic IB to static IB
		if (!part->staticIndexBuffer)
		{
			if (!part->indexBuffer)
			{
				DebugTrace("ERROR: Model part missing index buffer!\n");
				throw std::exception("ModelMeshPart");
			}

			part->indexBufferSize = static_cast<uint32_t>(part->indexBuffer.Size());

			auto desc = CD3DX12_RESOURCE_DESC::Buffer(part->indexBuffer.Size());

			ThrowIfFailed(device->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_GRAPHICS_PPV_ARGS(part->staticIndexBuffer.addressof())));

			SetDebugObjectName(part->staticIndexBuffer.get(), L"ModelMeshPart");

			resourceuploadbatch.Upload(part->staticIndexBuffer.get(), part->indexBuffer);

			resourceuploadbatch.Transition(part->staticIndexBuffer.get(),
				D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);

			// Scan for any other part with the same index buffer for sharing
			for (auto sit = std::next(it); sit != uniqueParts.cend(); ++sit)
			{
				auto sharePart = *sit;
				assert(sharePart != part);

				if (sharePart->staticIndexBuffer)
					continue;

				if (sharePart->indexBuffer == part->indexBuffer)
				{
					sharePart->indexBufferSize = part->indexBufferSize;
					sharePart->staticIndexBuffer = part->staticIndexBuffer;

					if (!keepMemory)
					{
						sharePart->indexBuffer.reset();
					}
				}
			}

			if (!keepMemory)
			{
				part->indexBuffer.reset();
			}
		}
	}
}

// Create effects for each mesh piece
std::vector<std::shared_ptr<IEffect>> Model::CreateEffects(
	IEffectFactory& fxFactory,
	const EffectPipelineStateDescription& opaquePipelineState,
	const EffectPipelineStateDescription& alphaPipelineState,
	int32_t textureDescriptorOffset,
	int32_t samplerDescriptorOffset) const
{
	if (materials.empty())
	{
		DebugTrace("ERROR: Model has no material information to create effects!\n");
		throw std::exception("CreateEffects");
	}

	std::vector<std::shared_ptr<IEffect>> effects;

	// Count the number of parts
	uint32_t partCount = 0;
	for (const auto& mesh : meshes)
	{
		for (const auto& part : mesh->opaqueMeshParts)
			partCount = std::max(part->partIndex + 1, partCount);
		for (const auto& part : mesh->alphaMeshParts)
			partCount = std::max(part->partIndex + 1, partCount);
	}

	if (partCount == 0)
		return effects;

	// Create an array of effects for each part. We need to have an effect per part because the part's vertex layout
	// combines with the material spec to create a unique effect. We rely on the EffectFactory to de-duplicate if it
	// wants to.
	effects.resize(partCount);

	for (const auto& mesh : meshes)
	{
		assert(mesh != nullptr);

		for (const auto& part : mesh->opaqueMeshParts)
		{
			assert(part != nullptr);

			if (part->materialIndex == uint32_t(-1))
				continue;

			// If this fires, you have multiple parts with the same unique ID
			assert(effects[part->partIndex] == nullptr);

			effects[part->partIndex] = CreateEffectForMeshPart(fxFactory, opaquePipelineState, alphaPipelineState, textureDescriptorOffset, samplerDescriptorOffset, part.get());
		}

		for (const auto& part : mesh->alphaMeshParts)
		{
			assert(part != nullptr);

			if (part->materialIndex == uint32_t(-1))
				continue;

			// If this fires, you have multiple parts with the same unique ID
			assert(effects[part->partIndex] == nullptr);

			effects[part->partIndex] = CreateEffectForMeshPart(fxFactory, opaquePipelineState, alphaPipelineState, textureDescriptorOffset, samplerDescriptorOffset, part.get());
		}
	}

	return effects;
}

// Creates an effect for a mesh part
_Use_decl_annotations_
	std::shared_ptr<IEffect>
	Model::CreateEffectForMeshPart(
		IEffectFactory& fxFactory,
		const EffectPipelineStateDescription& opaquePipelineState,
		const EffectPipelineStateDescription& alphaPipelineState,
		int32_t textureDescriptorOffset,
		int32_t samplerDescriptorOffset,
		const ModelMeshPart* part) const
{
	assert(part->materialIndex < materials.size());
	const auto& m = materials[part->materialIndex];

	if (!part->vbDecl || part->vbDecl->empty())
		throw std::exception("Model mesh part missing vertex buffer input elements data");

	if (part->vbDecl->size() > D3D12_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT)
		throw std::exception("Model mesh part input layout size is too large for DirectX 12");

	D3D12_INPUT_LAYOUT_DESC il = {};
	il.NumElements = static_cast<uint32_t>(part->vbDecl->size());
	il.pInputElementDescs = part->vbDecl->data();

	return fxFactory.CreateEffect(m, opaquePipelineState, alphaPipelineState, il, textureDescriptorOffset, samplerDescriptorOffset);
}

// Create effects for each mesh piece with the default factory
_Use_decl_annotations_
	std::vector<std::shared_ptr<IEffect>>
	Model::CreateEffects(
		const EffectPipelineStateDescription& opaquePipelineState,
		const EffectPipelineStateDescription& alphaPipelineState,
		ID3D12DescriptorHeap* textureDescriptorHeap,
		ID3D12DescriptorHeap* samplerDescriptorHeap,
		int32_t textureDescriptorOffset,
		int32_t samplerDescriptorOffset) const
{
	EffectFactory fxFactory(textureDescriptorHeap, samplerDescriptorHeap);
	return CreateEffects(fxFactory, opaquePipelineState, alphaPipelineState, textureDescriptorOffset, samplerDescriptorOffset);
}

// Updates effect matrices (if applicable)
void XM_CALLCONV Model::UpdateEffectMatrices(
	_In_ std::vector<std::shared_ptr<IEffect>>& effectList,
	DirectX::FXMMATRIX world,
	DirectX::CXMMATRIX view,
	DirectX::CXMMATRIX proj)
{
	for (auto& fx : effectList)
	{
		auto matFx = dynamic_cast<IEffectMatrices*>(fx.get());
		if (matFx)
		{
			matFx->SetMatrices(world, view, proj);
		}
	}
}

// Transition static VB/IB resources (if applicable)
void Model::Transition(
	_In_ ID3D12GraphicsCommandList* commandList,
	D3D12_RESOURCE_STATES stateBeforeVB,
	D3D12_RESOURCE_STATES stateAfterVB,
	D3D12_RESOURCE_STATES stateBeforeIB,
	D3D12_RESOURCE_STATES stateAfterIB)
{
	uint32_t count = 0;
	D3D12_RESOURCE_BARRIER barrier[64] = {};

	for (auto& mit : meshes)
	{
		for (auto& pit : mit->opaqueMeshParts)
		{
			assert(count < _countof(barrier));
			_Analysis_assume_(count < _countof(barrier));

			if (stateBeforeIB != stateAfterIB && pit->staticIndexBuffer)
			{
				barrier[count].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier[count].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				barrier[count].Transition.pResource = pit->staticIndexBuffer.get();
				barrier[count].Transition.StateBefore = stateBeforeIB;
				barrier[count].Transition.StateAfter = stateAfterIB;
				++count;

				if (count >= _countof(barrier))
				{
					commandList->ResourceBarrier(count, barrier);
					count = 0;
				}
			}

			if (stateBeforeVB != stateAfterVB && pit->staticVertexBuffer)
			{
				barrier[count].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier[count].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				barrier[count].Transition.pResource = pit->staticVertexBuffer.get();
				barrier[count].Transition.StateBefore = stateBeforeVB;
				barrier[count].Transition.StateAfter = stateAfterVB;
				++count;

				if (count >= _countof(barrier))
				{
					commandList->ResourceBarrier(count, barrier);
					count = 0;
				}
			}
		}

		for (auto& pit : mit->alphaMeshParts)
		{
			assert(count < _countof(barrier));
			_Analysis_assume_(count < _countof(barrier));

			if (stateBeforeIB != stateAfterIB && pit->staticIndexBuffer)
			{
				barrier[count].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier[count].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				barrier[count].Transition.pResource = pit->staticIndexBuffer.get();
				barrier[count].Transition.StateBefore = stateBeforeIB;
				barrier[count].Transition.StateAfter = stateAfterIB;
				++count;

				if (count >= _countof(barrier))
				{
					commandList->ResourceBarrier(count, barrier);
					count = 0;
				}
			}

			if (stateBeforeVB != stateAfterVB && pit->staticVertexBuffer)
			{
				barrier[count].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier[count].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				barrier[count].Transition.pResource = pit->staticVertexBuffer.get();
				barrier[count].Transition.StateBefore = stateBeforeVB;
				barrier[count].Transition.StateAfter = stateAfterVB;
				++count;

				if (count >= _countof(barrier))
				{
					commandList->ResourceBarrier(count, barrier);
					count = 0;
				}
			}
		}
	}

	if (count > 0)
	{
		commandList->ResourceBarrier(count, barrier);
	}
}
