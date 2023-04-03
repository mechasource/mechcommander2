//--------------------------------------------------------------------------------------
// File: ModelLoadSDKMESH.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"
#include "Model.h"

#include "effects.h"
#include "VertexTypes.h"

#include "directxhelpers.h"
#include "platformhelpers.h"
#include "binaryreader.h"
#include "descriptorheap.h"
#include "commonstates.h"

#include "sdkmesh.h"

using namespace directxtk;
// using Microsoft::WRL::ComPtr;

namespace
{
enum : uint32_t
{
	PER_VERTEX_COLOR = 0x1,
	SKINNING = 0x2,
	DUAL_TEXTURE = 0x4,
	NORMAL_MAPS = 0x8,
	BIASED_VERTEX_NORMALS = 0x10,
	USES_OBSOLETE_DEC3N = 0x20,
};

int32_t GetUniqueTextureIndex(std::wstring_view texturename, std::map<std::wstring, int32_t>& texturedictionary)
{
	if (texturename.empty())
		return -1;

	// !!!!
	auto i = texturedictionary.find(texturename.data());
	if (i == std::cend(texturedictionary))
	{
		int32_t index = static_cast<int32_t>(texturedictionary.size());
		texturedictionary[texturename.data()] = index;
		return index;
	}
	else
	{
		return i->second;
	}
}

inline DirectX::XMFLOAT3 GetMaterialColor(float r, float g, float b, bool srgb) noexcept
{
	if (srgb)
	{
		DirectX::XMVECTOR v = DirectX::XMVectorSet(r, g, b, 1.f);
		v = DirectX::XMColorSRGBToRGB(v);

		DirectX::XMFLOAT3 result;
		XMStoreFloat3(&result, v);
		return result;
	}
	else
	{
		return DirectX::XMFLOAT3(r, g, b);
	}
}

void InitMaterial(
	const DXUT::SDKMESH_MATERIAL& meshmaterial,
	uint32_t flags,
	_Out_ Model::ModelMaterialInfo& materialinfo,
	_Inout_ std::map<std::wstring, int32_t>& texturedictionary,
	bool srgb)
{
	wchar_t materialname[DXUT::MAX_MATERIAL_NAME] = {};
	MultiByteToWideChar(CP_UTF8, 0, meshmaterial.Name, -1, materialname, DXUT::MAX_MATERIAL_NAME);

	wchar_t diffusename[DXUT::MAX_TEXTURE_NAME] = {};
	MultiByteToWideChar(CP_UTF8, 0, meshmaterial.DiffuseTexture, -1, diffusename, DXUT::MAX_TEXTURE_NAME);

	wchar_t specularname[DXUT::MAX_TEXTURE_NAME] = {};
	MultiByteToWideChar(CP_UTF8, 0, meshmaterial.SpecularTexture, -1, specularname, DXUT::MAX_TEXTURE_NAME);

	wchar_t normalname[DXUT::MAX_TEXTURE_NAME] = {};
	MultiByteToWideChar(CP_UTF8, 0, meshmaterial.NormalTexture, -1, normalname, DXUT::MAX_TEXTURE_NAME);

	if ((flags & DUAL_TEXTURE) && !meshmaterial.SpecularTexture[0])
	{
		DebugTrace("WARNING: Material '%s' has multiple texture coords but not multiple textures\n", meshmaterial.Name);
		flags &= ~static_cast<uint32_t>(DUAL_TEXTURE);
	}

	if (flags & NORMAL_MAPS)
	{
		if (!meshmaterial.NormalTexture[0])
		{
			flags &= ~static_cast<uint32_t>(NORMAL_MAPS);
			*normalname = 0;
		}
	}
	else if (meshmaterial.NormalTexture[0])
	{
		DebugTrace("WARNING: Material '%s' has a normal map, but vertex buffer is missing tangents\n", meshmaterial.Name);
		*normalname = 0;
	}

	materialinfo = {};
	materialinfo.name = materialname;
	materialinfo.perVertexColor = (flags & PER_VERTEX_COLOR) != 0;
	materialinfo.enableSkinning = (flags & SKINNING) != 0;
	materialinfo.enableDualTexture = (flags & DUAL_TEXTURE) != 0;
	materialinfo.enableNormalMaps = (flags & NORMAL_MAPS) != 0;
	materialinfo.biasedVertexNormals = (flags & BIASED_VERTEX_NORMALS) != 0;

	if (meshmaterial.Ambient.x == 0 && meshmaterial.Ambient.y == 0 && meshmaterial.Ambient.z == 0 && meshmaterial.Ambient.w == 0
		&& meshmaterial.Diffuse.x == 0 && meshmaterial.Diffuse.y == 0 && meshmaterial.Diffuse.z == 0 && meshmaterial.Diffuse.w == 0)
	{
		// SDKMESH material color block is uninitalized; assume defaults
		materialinfo.diffuseColor = DirectX::XMFLOAT3(1.f, 1.f, 1.f);
		materialinfo.alphaValue = 1.f;
	}
	else
	{
		materialinfo.ambientColor = GetMaterialColor(meshmaterial.Ambient.x, meshmaterial.Ambient.y, meshmaterial.Ambient.z, srgb);
		materialinfo.diffuseColor = GetMaterialColor(meshmaterial.Diffuse.x, meshmaterial.Diffuse.y, meshmaterial.Diffuse.z, srgb);
		materialinfo.emissiveColor = GetMaterialColor(meshmaterial.Emissive.x, meshmaterial.Emissive.y, meshmaterial.Emissive.z, srgb);

		if (meshmaterial.Diffuse.w != 1.f && meshmaterial.Diffuse.w != 0.f)
		{
			materialinfo.alphaValue = meshmaterial.Diffuse.w;
		}
		else
			materialinfo.alphaValue = 1.f;

		if (meshmaterial.Power > 0)
		{
			materialinfo.specularPower = meshmaterial.Power;
			materialinfo.specularColor = DirectX::XMFLOAT3(meshmaterial.Specular.x, meshmaterial.Specular.y, meshmaterial.Specular.z);
		}
	}

	materialinfo.diffuseTextureIndex = GetUniqueTextureIndex(diffusename, texturedictionary);
	materialinfo.specularTextureIndex = GetUniqueTextureIndex(specularname, texturedictionary);
	materialinfo.normalTextureIndex = GetUniqueTextureIndex(normalname, texturedictionary);

	materialinfo.samplerIndex = (materialinfo.diffuseTextureIndex == -1) ? -1 : static_cast<int32_t>(CommonStates::SamplerIndex::AnisotropicWrap);
	materialinfo.samplerIndex2 = (flags & DUAL_TEXTURE) ? static_cast<int32_t>(CommonStates::SamplerIndex::AnisotropicWrap) : -1;
}

void InitMaterial(
	const DXUT::SDKMESH_MATERIAL_V2& mh,
	uint32_t flags,
	_Out_ Model::ModelMaterialInfo& m,
	_Inout_ std::map<std::wstring, int32_t>& texturedictionary)
{
	wchar_t materialname[DXUT::MAX_MATERIAL_NAME] = {};
	MultiByteToWideChar(CP_UTF8, 0, mh.Name, -1, materialname, DXUT::MAX_MATERIAL_NAME);

	wchar_t albetoname[DXUT::MAX_TEXTURE_NAME] = {};
	MultiByteToWideChar(CP_UTF8, 0, mh.AlbetoTexture, -1, albetoname, DXUT::MAX_TEXTURE_NAME);

	wchar_t normalname[DXUT::MAX_TEXTURE_NAME] = {};
	MultiByteToWideChar(CP_UTF8, 0, mh.NormalTexture, -1, normalname, DXUT::MAX_TEXTURE_NAME);

	wchar_t rmaname[DXUT::MAX_TEXTURE_NAME] = {};
	MultiByteToWideChar(CP_UTF8, 0, mh.RMATexture, -1, rmaname, DXUT::MAX_TEXTURE_NAME);

	wchar_t emissivename[DXUT::MAX_TEXTURE_NAME] = {};
	MultiByteToWideChar(CP_UTF8, 0, mh.EmissiveTexture, -1, emissivename, DXUT::MAX_TEXTURE_NAME);

	m = {};
	m.name = materialname;
	m.perVertexColor = false;
	m.enableSkinning = false;
	m.enableDualTexture = false;
	m.enableNormalMaps = true;
	m.biasedVertexNormals = (flags & BIASED_VERTEX_NORMALS) != 0;
	m.alphaValue = (mh.Alpha == 0.f) ? 1.f : mh.Alpha;

	m.diffuseTextureIndex = GetUniqueTextureIndex(albetoname, texturedictionary);
	m.specularTextureIndex = GetUniqueTextureIndex(rmaname, texturedictionary);
	m.normalTextureIndex = GetUniqueTextureIndex(normalname, texturedictionary);
	m.emissiveTextureIndex = GetUniqueTextureIndex(emissivename, texturedictionary);

	m.samplerIndex = m.samplerIndex2 = static_cast<int32_t>(CommonStates::SamplerIndex::AnisotropicWrap);
}

//--------------------------------------------------------------------------------------
// Direct3D 9 Vertex Declaration to Direct3D 12 Input Layout mapping

static_assert(D3D12_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT >= 32, "SDKMESH supports decls up to 32 entries");

uint32_t GetInputLayoutDesc(
	_In_reads_(32) const DXUT::D3DVERTEXELEMENT9 decl[],
	std::vector<D3D12_INPUT_ELEMENT_DESC>& inputDesc)
{
	static const D3D12_INPUT_ELEMENT_DESC s_elements[] =
		{
			{"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BLENDWEIGHT", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

	using namespace DXUT;

	uint32_t offset = 0;
	uint32_t texcoords = 0;
	uint32_t flags = 0;

	bool posfound = false;

	for (uint32_t index = 0; index < DXUT::MAX_VERTEX_ELEMENTS; ++index)
	{
		if (decl[index].Usage == 0xFF)
			break;

		if (decl[index].Type == D3DDECLTYPE_UNUSED)
			break;

		if (decl[index].Offset != offset)
			break;

		if (decl[index].Usage == D3DDECLUSAGE_POSITION)
		{
			if (decl[index].Type == D3DDECLTYPE_FLOAT3)
			{
				inputDesc.push_back(s_elements[0]);
				offset += 12;
				posfound = true;
			}
			else
				break;
		}
		else if (decl[index].Usage == D3DDECLUSAGE_NORMAL
			|| decl[index].Usage == D3DDECLUSAGE_TANGENT
			|| decl[index].Usage == D3DDECLUSAGE_BINORMAL)
		{
			size_t base = 1;
			if (decl[index].Usage == D3DDECLUSAGE_TANGENT)
				base = 3;
			else if (decl[index].Usage == D3DDECLUSAGE_BINORMAL)
				base = 4;

			D3D12_INPUT_ELEMENT_DESC desc = s_elements[base];

			bool unk = false;
			switch (decl[index].Type)
			{
			case D3DDECLTYPE_FLOAT3:
				assert(desc.Format == DXGI_FORMAT_R32G32B32_FLOAT);
				offset += 12;
				break;
			case D3DDECLTYPE_UBYTE4N:
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				flags |= BIASED_VERTEX_NORMALS;
				offset += 4;
				break;
			case D3DDECLTYPE_SHORT4N:
				desc.Format = DXGI_FORMAT_R16G16B16A16_SNORM;
				offset += 8;
				break;
			case D3DDECLTYPE_FLOAT16_4:
				desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				offset += 8;
				break;
			case D3DDECLTYPE_DXGI_R10G10B10A2_UNORM:
				desc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
				flags |= BIASED_VERTEX_NORMALS;
				offset += 4;
				break;
			case D3DDECLTYPE_DXGI_R11G11B10_FLOAT:
				desc.Format = DXGI_FORMAT_R11G11B10_FLOAT;
				flags |= BIASED_VERTEX_NORMALS;
				offset += 4;
				break;
			case D3DDECLTYPE_DXGI_R8G8B8A8_SNORM:
				desc.Format = DXGI_FORMAT_R8G8B8A8_SNORM;
				offset += 4;
				break;

#if defined(_XBOX_ONE) && defined(_TITLE)
			case D3DDECLTYPE_DEC3N:
				desc.Format = DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM;
				offset += 4;
				break;
			case (32 + DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM):
				desc.Format = DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM;
				offset += 4;
				break;
#else
			case D3DDECLTYPE_DEC3N:
				desc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
				flags |= USES_OBSOLETE_DEC3N;
				offset += 4;
				break;
#endif

			default:
				unk = true;
				break;
			}

			if (unk)
				break;

			if (decl[index].Usage == D3DDECLUSAGE_TANGENT)
			{
				flags |= NORMAL_MAPS;
			}

			inputDesc.push_back(desc);
		}
		else if (decl[index].Usage == D3DDECLUSAGE_COLOR)
		{
			D3D12_INPUT_ELEMENT_DESC desc = s_elements[2];

			bool unk = false;
			switch (decl[index].Type)
			{
			case D3DDECLTYPE_FLOAT4:
				desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				offset += 16;
				break;
			case D3DDECLTYPE_D3DCOLOR:
				assert(desc.Format == DXGI_FORMAT_B8G8R8A8_UNORM);
				offset += 4;
				break;
			case D3DDECLTYPE_UBYTE4N:
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				offset += 4;
				break;
			case D3DDECLTYPE_FLOAT16_4:
				desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				offset += 8;
				break;
			case D3DDECLTYPE_DXGI_R10G10B10A2_UNORM:
				desc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
				offset += 4;
				break;
			case D3DDECLTYPE_DXGI_R11G11B10_FLOAT:
				desc.Format = DXGI_FORMAT_R11G11B10_FLOAT;
				offset += 4;
				break;

			default:
				unk = true;
				break;
			}

			if (unk)
				break;

			flags |= PER_VERTEX_COLOR;

			inputDesc.push_back(desc);
		}
		else if (decl[index].Usage == D3DDECLUSAGE_TEXCOORD)
		{
			D3D12_INPUT_ELEMENT_DESC desc = s_elements[5];
			desc.SemanticIndex = decl[index].UsageIndex;

			bool unk = false;
			switch (decl[index].Type)
			{
			case D3DDECLTYPE_FLOAT1:
				desc.Format = DXGI_FORMAT_R32_FLOAT;
				offset += 4;
				break;
			case D3DDECLTYPE_FLOAT2:
				assert(desc.Format == DXGI_FORMAT_R32G32_FLOAT);
				offset += 8;
				break;
			case D3DDECLTYPE_FLOAT3:
				desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				offset += 12;
				break;
			case D3DDECLTYPE_FLOAT4:
				desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				offset += 16;
				break;
			case D3DDECLTYPE_FLOAT16_2:
				desc.Format = DXGI_FORMAT_R16G16_FLOAT;
				offset += 4;
				break;
			case D3DDECLTYPE_FLOAT16_4:
				desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				offset += 8;
				break;

			default:
				unk = true;
				break;
			}

			if (unk)
				break;

			++texcoords;

			inputDesc.push_back(desc);
		}
		else if (decl[index].Usage == D3DDECLUSAGE_BLENDINDICES)
		{
			if (decl[index].Type == D3DDECLTYPE_UBYTE4)
			{
				flags |= SKINNING;
				inputDesc.push_back(s_elements[6]);
				offset += 4;
			}
			else
				break;
		}
		else if (decl[index].Usage == D3DDECLUSAGE_BLENDWEIGHT)
		{
			if (decl[index].Type == D3DDECLTYPE_UBYTE4N)
			{
				flags |= SKINNING;
				inputDesc.push_back(s_elements[7]);
				offset += 4;
			}
			else
				break;
		}
		else
			break;
	}

	if (!posfound)
		throw std::exception("SV_Position is required");

	if (texcoords == 2)
	{
		flags |= DUAL_TEXTURE;
	}

	return flags;
}
} // namespace

//======================================================================================
// Model Loader
//======================================================================================

_Use_decl_annotations_
	std::unique_ptr<Model>
	directxtk::Model::CreateFromSDKMESH(
		ID3D12Device* device,
		const uint8_t* meshData,
		size_t idataSize,
		ModelLoaderFlags flags)
{
	if (!meshData)
		throw std::exception("meshData cannot be null");

	uint64_t datasize = idataSize;

	// File Headers
	if (datasize < sizeof(DXUT::SDKMESH_HEADER))
		throw std::exception("End of file");
	auto header = reinterpret_cast<const DXUT::SDKMESH_HEADER*>(meshData);

	size_t headersize = sizeof(DXUT::SDKMESH_HEADER)
		+ header->NumVertexBuffers * sizeof(DXUT::SDKMESH_VERTEX_BUFFER_HEADER)
		+ header->NumIndexBuffers * sizeof(DXUT::SDKMESH_INDEX_BUFFER_HEADER);
	if (header->HeaderSize != headersize)
		throw std::exception("Not a valid SDKMESH file");

	if (datasize < header->HeaderSize)
		throw std::exception("End of file");

	if (header->Version != DXUT::SDKMESH_FILE_VERSION && header->Version != DXUT::SDKMESH_FILE_VERSION_V2)
		throw std::exception("Not a supported SDKMESH version");

	if (header->IsBigEndian)
		throw std::exception("Loading BigEndian SDKMESH files not supported");

	if (!header->NumMeshes)
		throw std::exception("No meshes found");

	if (!header->NumVertexBuffers)
		throw std::exception("No vertex buffers found");

	if (!header->NumIndexBuffers)
		throw std::exception("No index buffers found");

	if (!header->NumTotalSubsets)
		throw std::exception("No subsets found");

	if (!header->NumMaterials)
		throw std::exception("No materials found");

	// Sub-headers
	if (datasize < header->VertexStreamHeadersOffset
		|| (datasize < (header->VertexStreamHeadersOffset + uint64_t(header->NumVertexBuffers) * sizeof(DXUT::SDKMESH_VERTEX_BUFFER_HEADER))))
		throw std::exception("End of file");
	auto vbArray = reinterpret_cast<const DXUT::SDKMESH_VERTEX_BUFFER_HEADER*>(meshData + header->VertexStreamHeadersOffset);

	if (datasize < header->IndexStreamHeadersOffset
		|| (datasize < (header->IndexStreamHeadersOffset + uint64_t(header->NumIndexBuffers) * sizeof(DXUT::SDKMESH_INDEX_BUFFER_HEADER))))
		throw std::exception("End of file");
	auto ibArray = reinterpret_cast<const DXUT::SDKMESH_INDEX_BUFFER_HEADER*>(meshData + header->IndexStreamHeadersOffset);

	if (datasize < header->MeshDataOffset
		|| (datasize < (header->MeshDataOffset + uint64_t(header->NumMeshes) * sizeof(DXUT::SDKMESH_MESH))))
		throw std::exception("End of file");
	auto meshArray = reinterpret_cast<const DXUT::SDKMESH_MESH*>(meshData + header->MeshDataOffset);

	if (datasize < header->SubsetDataOffset
		|| (datasize < (header->SubsetDataOffset + uint64_t(header->NumTotalSubsets) * sizeof(DXUT::SDKMESH_SUBSET))))
		throw std::exception("End of file");
	auto subsetArray = reinterpret_cast<const DXUT::SDKMESH_SUBSET*>(meshData + header->SubsetDataOffset);

	if (datasize < header->FrameDataOffset
		|| (datasize < (header->FrameDataOffset + uint64_t(header->NumFrames) * sizeof(DXUT::SDKMESH_FRAME))))
		throw std::exception("End of file");
	// TODO - auto frameArray = reinterpret_cast<const DXUT::SDKMESH_FRAME*>( meshData + header->FrameDataOffset );

	if (datasize < header->MaterialDataOffset
		|| (datasize < (header->MaterialDataOffset + uint64_t(header->NumMaterials) * sizeof(DXUT::SDKMESH_MATERIAL))))
		throw std::exception("End of file");

	const DXUT::SDKMESH_MATERIAL* materialArray = nullptr;
	const DXUT::SDKMESH_MATERIAL_V2* materialArray_v2 = nullptr;
	if (header->Version == DXUT::SDKMESH_FILE_VERSION_V2)
	{
		materialArray_v2 = reinterpret_cast<const DXUT::SDKMESH_MATERIAL_V2*>(meshData + header->MaterialDataOffset);
	}
	else
	{
		materialArray = reinterpret_cast<const DXUT::SDKMESH_MATERIAL*>(meshData + header->MaterialDataOffset);
	}

	// Buffer data
	uint64_t bufferDataOffset = header->HeaderSize + header->NonBufferDataSize;
	if ((datasize < bufferDataOffset)
		|| (datasize < bufferDataOffset + header->BufferDataSize))
		throw std::exception("End of file");
	const uint8_t* bufferData = meshData + bufferDataOffset;

	// Create vertex buffers
	std::vector<std::shared_ptr<std::vector<D3D12_INPUT_ELEMENT_DESC>>> vbDecls;
	vbDecls.resize(header->NumVertexBuffers);

	std::vector<uint32_t> materialFlags;
	materialFlags.resize(header->NumVertexBuffers);

	bool dec3nwarning = false;
	for (uint32_t j = 0; j < header->NumVertexBuffers; ++j)
	{
		auto& vh = vbArray[j];

		if (vh.SizeBytes > UINT32_MAX)
			throw std::exception("VB too large");

		if (!(flags & ModelLoader_AllowLargeModels))
		{
			if (vh.SizeBytes > (D3D12_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
				throw std::exception("VB too large for DirectX 12");
		}

		if (datasize < vh.DataOffset
			|| (datasize < vh.DataOffset + vh.SizeBytes))
			throw std::exception("End of file");

		vbDecls[j] = std::make_shared<std::vector<D3D12_INPUT_ELEMENT_DESC>>();
		uint32_t ilflags = GetInputLayoutDesc(vh.Decl, *vbDecls[j].get());

		if (ilflags & SKINNING)
		{
			ilflags &= ~static_cast<uint32_t>(DUAL_TEXTURE | NORMAL_MAPS);
		}
		if (ilflags & DUAL_TEXTURE)
		{
			ilflags &= ~static_cast<uint32_t>(NORMAL_MAPS);
		}

		if (ilflags & USES_OBSOLETE_DEC3N)
		{
			dec3nwarning = true;
		}

		materialFlags[j] = ilflags;
	}

	if (dec3nwarning)
	{
		DebugTrace("WARNING: Vertex declaration uses legacy Direct3D 9 D3DDECLTYPE_DEC3N which has no DXGI equivalent\n"
				   "         (treating as DXGI_FORMAT_R10G10B10A2_UNORM which is not a signed format)\n");
	}

	// Validate index buffers
	for (uint32_t j = 0; j < header->NumIndexBuffers; ++j)
	{
		auto& ih = ibArray[j];

		if (ih.SizeBytes > UINT32_MAX)
			throw std::exception("IB too large");

		if (!(flags & ModelLoader_AllowLargeModels))
		{
			if (ih.SizeBytes > (D3D12_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
				throw std::exception("IB too large for DirectX 12");
		}

		if (datasize < ih.DataOffset
			|| (datasize < ih.DataOffset + ih.SizeBytes))
			throw std::exception("End of file");

		if (ih.IndexType != DXUT::IT_16BIT && ih.IndexType != DXUT::IT_32BIT)
			throw std::exception("Invalid index buffer type found");
	}

	// Create meshes
	std::vector<ModelMaterialInfo> materials;
	materials.resize(header->NumMaterials);

	std::map<std::wstring, int32_t> texturedictionary;

	auto model = std::make_unique<Model>();
	model->meshes.reserve(header->NumMeshes);

	uint32_t partCount = 0u;

	for (auto meshindex = 0u; meshindex < header->NumMeshes; ++meshindex)
	{
		auto& mh = meshArray[meshindex];

		if (!mh.NumSubsets
			|| !mh.NumVertexBuffers
			|| mh.IndexBuffer >= header->NumIndexBuffers
			|| mh.VertexBuffers[0] >= header->NumVertexBuffers)
			throw std::exception("Invalid mesh found");

		// mh.NumVertexBuffers is sometimes not what you'd expect, so we skip validating it

		if (datasize < mh.SubsetOffset
			|| (datasize < mh.SubsetOffset + uint64_t(mh.NumSubsets) * sizeof(uint32_t)))
			throw std::exception("End of file");

		auto subsets = reinterpret_cast<const uint32_t*>(meshData + mh.SubsetOffset);

		if (mh.NumFrameInfluences > 0)
		{
			if (datasize < mh.FrameInfluenceOffset
				|| (datasize < mh.FrameInfluenceOffset + uint64_t(mh.NumFrameInfluences) * sizeof(uint32_t)))
				throw std::exception("End of file");

			// TODO - auto influences = reinterpret_cast<const uint32_t*>( meshData + mh.FrameInfluenceOffset );
		}

		auto mesh = std::make_shared<ModelMesh>();
		wchar_t meshName[DXUT::MAX_MESH_NAME] = {};
		MultiByteToWideChar(CP_UTF8, 0, mh.Name, -1, meshName, DXUT::MAX_MESH_NAME);
		mesh->name = meshName;

		// Extents
		mesh->boundingBox.Center = mh.BoundingBoxCenter;
		mesh->boundingBox.Extents = mh.BoundingBoxExtents;
		DirectX::BoundingSphere::CreateFromBoundingBox(mesh->boundingSphere, mesh->boundingBox);

		// Create subsets
		for (uint32_t j = 0; j < mh.NumSubsets; ++j)
		{
			auto sIndex = subsets[j];
			if (sIndex >= header->NumTotalSubsets)
				throw std::exception("Invalid mesh found");

			auto& subset = subsetArray[sIndex];

			D3D_PRIMITIVE_TOPOLOGY primType;
			switch (subset.primitivetype)
			{
			case DXUT::PT_TRIANGLE_LIST:
				primType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
				break;
			case DXUT::PT_TRIANGLE_STRIP:
				primType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
				break;
			case DXUT::PT_LINE_LIST:
				primType = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
				break;
			case DXUT::PT_LINE_STRIP:
				primType = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
				break;
			case DXUT::PT_POINT_LIST:
				primType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
				break;
			case DXUT::PT_TRIANGLE_LIST_ADJ:
				primType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
				break;
			case DXUT::PT_TRIANGLE_STRIP_ADJ:
				primType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
				break;
			case DXUT::PT_LINE_LIST_ADJ:
				primType = D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
				break;
			case DXUT::PT_LINE_STRIP_ADJ:
				primType = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
				break;

			case DXUT::PT_QUAD_PATCH_LIST:
			case DXUT::PT_TRIANGLE_PATCH_LIST:
				throw std::exception("Direct3D9 era tessellation not supported");

			default:
				throw std::exception("Unknown primitive type");
			}

			if (subset.MaterialID >= header->NumMaterials)
				throw std::exception("Invalid mesh found");

			auto& mat = materials[subset.MaterialID];

			const size_t vi = mh.VertexBuffers[0];
			if (materialArray_v2)
			{
				InitMaterial(
					materialArray_v2[subset.MaterialID],
					materialFlags[vi],
					mat,
					texturedictionary);
			}
			else
			{
				InitMaterial(
					materialArray[subset.MaterialID],
					materialFlags[vi],
					mat,
					texturedictionary,
					(flags & ModelLoader_MaterialColorsSRGB) != 0);
			}

			auto part = new ModelMeshPart(partCount++);

			const auto& vh = vbArray[mh.VertexBuffers[0]];
			const auto& ih = ibArray[mh.IndexBuffer];

			part->indexCount = static_cast<uint32_t>(subset.IndexCount);
			part->startIndex = static_cast<uint32_t>(subset.IndexStart);
			part->vertexOffset = static_cast<int32_t>(subset.VertexStart);
			part->vertexStride = static_cast<uint32_t>(vh.StrideBytes);
			part->vertexCount = static_cast<uint32_t>(subset.VertexCount);
			part->primitiveType = primType;
			part->indexFormat = (ibArray[mh.IndexBuffer].IndexType == DXUT::IT_32BIT) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;

			// Vertex data
			auto verts = bufferData + (vh.DataOffset - bufferDataOffset);
			auto vbytes = static_cast<size_t>(vh.SizeBytes);
			part->vertexBufferSize = static_cast<uint32_t>(vh.SizeBytes);
			part->vertexBuffer = GraphicsMemory::Get(device).Allocate(vbytes);
			memcpy(part->vertexBuffer.Memory(), verts, vbytes);

			// Index data
			auto indices = bufferData + (ih.DataOffset - bufferDataOffset);
			auto ibytes = static_cast<size_t>(ih.SizeBytes);
			part->indexBufferSize = static_cast<uint32_t>(ih.SizeBytes);
			part->indexBuffer = GraphicsMemory::Get(device).Allocate(ibytes);
			memcpy(part->indexBuffer.Memory(), indices, ibytes);

			part->materialIndex = subset.MaterialID;
			part->vbDecl = vbDecls[mh.VertexBuffers[0]];

			if (mat.alphaValue < 1.0f)
				mesh->alphaMeshParts.emplace_back(part);
			else
				mesh->opaqueMeshParts.emplace_back(part);
		}

		model->meshes.emplace_back(mesh);
	}

	// Copy the materials and texture names into contiguous arrays
	model->materials = std::move(materials);
	model->texturenames.resize(texturedictionary.size());
	for (auto texture = std::cbegin(texturedictionary); texture != std::cend(texturedictionary); ++texture)
	{
		model->texturenames[static_cast<size_t>(texture->second)] = texture->first;
	}

	return model;
}

//--------------------------------------------------------------------------------------
_Use_decl_annotations_
	std::unique_ptr<Model>
	directxtk::Model::CreateFromSDKMESH(
		ID3D12Device* device,
		std::wstring_view filename,
		ModelLoaderFlags flags)
{
	size_t datasize = 0;
	std::unique_ptr<uint8_t[]> data;
	HRESULT hr = BinaryReader::ReadEntireFile(filename, data, &datasize);
	if (FAILED(hr))
	{
		DebugTrace("ERROR: CreateFromSDKMESH failed (%08X) loading '%ls'\n",
			static_cast<uint32_t>(hr), filename);
		throw std::exception("CreateFromSDKMESH");
	}

	auto model = CreateFromSDKMESH(device, data.get(), datasize, flags);

	model->name = filename;

	return model;
}
