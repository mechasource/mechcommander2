//--------------------------------------------------------------------------------------
// File: EffectTextureFactory.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"

#include "effects.h"
#include "directxhelpers.h"
#include "ddstextureloader.h"
#include "descriptorheap.h"
#include "platformhelpers.h"
#include "resourceuploadbatch.h"
#include "wictextureloader.h"

#include <mutex>

using namespace directxtk;
// using Microsoft::WRL::ComPtr;

class EffectTextureFactory::Impl
{
public:
	struct TextureCacheEntry
	{
		wil::com_ptr<ID3D12Resource> m_resource;
		bool m_iscubemap = false;
		size_t slot = 0;

		TextureCacheEntry(void) noexcept { }
	};

	using TextureCache = std::map<std::wstring, TextureCacheEntry>;

	Impl(
		_In_ ID3D12Device* device,
		ResourceUploadBatch& resourceuploadbatch,
		_In_ ID3D12DescriptorHeap* descriptorheap)
		: m_texturedescriptorheap(descriptorheap)
		, m_device(device)
		, m_resourceuploadbatch(resourceuploadbatch)
	{
	}

	Impl(
		_In_ ID3D12Device* device,
		ResourceUploadBatch& resourceuploadbatch,
		_In_ size_t numdescriptors,
		_In_ D3D12_DESCRIPTOR_HEAP_FLAGS descriptorheapflags)
		: m_texturedescriptorheap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, descriptorheapflags, numdescriptors)
		, m_device(device)
		, m_resourceuploadbatch(resourceuploadbatch)
	{
		SetDebugObjectName(m_texturedescriptorheap.Heap(), L"EffectTextureFactory");
	}

	size_t CreateTexture(_In_ std::wstring_view name, int32_t descriptorslot);

	void ReleaseCache(void);
	void SetSharing(bool enablesharing) noexcept
	{
		m_sharing = enablesharing;
	}
	void EnableForceSRGB(bool forcesrgb) noexcept
	{
		m_forcesrgb = forcesrgb;
	}
	void EnableAutoGenMips(bool generatemips) noexcept
	{
		m_autogenmips = generatemips;
	}

protected:
	wchar_t m_path[MAX_PATH] {};
	::DescriptorHeap m_texturedescriptorheap;
	std::vector<TextureCacheEntry> m_resources; // flat list of unique resources so we can index into it

private:
	wil::com_ptr<ID3D12Device> m_device;
	ResourceUploadBatch& m_resourceuploadbatch;
	TextureCache m_texturecache;
	bool m_sharing = true;
	bool m_forcesrgb = false;
	bool m_autogenmips = false;

	std::mutex mutex;
};

_Use_decl_annotations_
	size_t
	EffectTextureFactory::Impl::CreateTexture(_In_ std::wstring_view texturename, int32_t descriptorslot)
{
	if (texturename.empty())
		throw std::exception("invalid arguments");

	auto it = m_texturecache.find(texturename.data());

	TextureCacheEntry textureentry = {};

	if (m_sharing && it != m_texturecache.end())
	{
		textureentry = it->second;
	}
	else
	{
		wchar_t fullname[MAX_PATH] = {};
		wcscpy_s(fullname, m_path);
		wcscat_s(fullname, texturename.data());

		WIN32_FILE_ATTRIBUTE_DATA fileattr = {};
		if (!GetFileAttributesExW(fullname, GetFileExInfoStandard, &fileattr))
		{
			// Try Current Working Directory (CWD)
			wcscpy_s(fullname, texturename.data());
			if (!GetFileAttributesExW(fullname, GetFileExInfoStandard, &fileattr))
			{
				DebugTrace("ERROR: EffectTextureFactory could not find texture file '%ls'\n", texturename);
				throw std::exception("CreateTexture");
			}
		}

		wchar_t ext[_MAX_EXT];
		_wsplitpath_s(texturename.data(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

		DDS_LOADER_FLAGS loadFlags = DDS_LOADER_DEFAULT;
		if (m_forcesrgb)
			loadFlags |= DDS_LOADER_FORCE_SRGB;
		if (m_autogenmips)
			loadFlags |= DDS_LOADER_MIP_AUTOGEN;

		static_assert(static_cast<int32_t>(DDS_LOADER_DEFAULT) == static_cast<int32_t>(WIC_LOADER_DEFAULT), "DDS/WIC Load flags mismatch");
		static_assert(static_cast<int32_t>(DDS_LOADER_FORCE_SRGB) == static_cast<int32_t>(WIC_LOADER_FORCE_SRGB), "DDS/WIC Load flags mismatch");
		static_assert(static_cast<int32_t>(DDS_LOADER_MIP_AUTOGEN) == static_cast<int32_t>(WIC_LOADER_MIP_AUTOGEN), "DDS/WIC Load flags mismatch");
		static_assert(static_cast<int32_t>(DDS_LOADER_MIP_RESERVE) == static_cast<int32_t>(WIC_LOADER_MIP_RESERVE), "DDS/WIC Load flags mismatch");

		if (_wcsicmp(ext, L".dds") == 0)
		{
			HRESULT hr = CreateDDSTextureFromFileEx(
				m_device.get(),
				m_resourceuploadbatch,
				fullname,
				0u,
				D3D12_RESOURCE_FLAG_NONE,
				loadFlags,
				textureentry.m_resource.put(),
				nullptr,
				&textureentry.m_iscubemap);
			if (FAILED(hr))
			{
				DebugTrace("ERROR: CreateDDSTextureFromFile failed (%08X) for '%ls'\n",
					static_cast<uint32_t>(hr), fullname);
				throw std::exception("CreateDDSTextureFromFile");
			}
		}
		else
		{
			textureentry.m_iscubemap = false;

			HRESULT hr = CreateWICTextureFromFileEx(
				m_device.get(),
				m_resourceuploadbatch,
				fullname,
				0u,
				D3D12_RESOURCE_FLAG_NONE,
				static_cast<WIC_LOADER_FLAGS>(loadFlags),
				textureentry.m_resource.put());
			if (FAILED(hr))
			{
				DebugTrace("ERROR: CreateWICTextureFromFile failed (%08X) for '%ls'\n",
					static_cast<uint32_t>(hr), fullname);
				throw std::exception("CreateWICTextureFromFile");
			}
		}

		std::lock_guard<std::mutex> lock(mutex);
		textureentry.slot = m_resources.size();
		if (m_sharing)
		{
			TextureCache::value_type v(texturename, textureentry);
			m_texturecache.insert(v);
		}
		m_resources.push_back(textureentry);
	}

	assert(textureentry.m_resource != nullptr);

	// bind a new descriptor in slot
	auto textureDescriptor = m_texturedescriptorheap.GetCpuHandle(static_cast<size_t>(descriptorslot));
	directxtk::CreateShaderResourceView(m_device.get(), textureentry.m_resource.get(), textureDescriptor, textureentry.m_iscubemap);

	return textureentry.slot;
}

void EffectTextureFactory::Impl::ReleaseCache()
{
	std::lock_guard<std::mutex> lock(mutex);
	m_texturecache.clear();
}

//--------------------------------------------------------------------------------------
// EffectTextureFactory
//--------------------------------------------------------------------------------------

_Use_decl_annotations_
EffectTextureFactory::EffectTextureFactory(
	ID3D12Device* device,
	ResourceUploadBatch& resourceuploadbatch,
	ID3D12DescriptorHeap* descriptorheap) noexcept(false)
{
	pimpl = std::make_unique<Impl>(device, resourceuploadbatch, descriptorheap);
}

_Use_decl_annotations_
EffectTextureFactory::EffectTextureFactory(
	ID3D12Device* device,
	ResourceUploadBatch& resourceuploadbatch,
	size_t numdescriptors,
	D3D12_DESCRIPTOR_HEAP_FLAGS descriptorheapflags) noexcept(false)
{
	pimpl = std::make_unique<Impl>(device, resourceuploadbatch, numdescriptors, descriptorheapflags);
}

EffectTextureFactory::~EffectTextureFactory()
{
}

EffectTextureFactory::EffectTextureFactory(EffectTextureFactory&& moveFrom) noexcept
	: pimpl(std::move(moveFrom.pimpl))
{
}

EffectTextureFactory& EffectTextureFactory::operator=(EffectTextureFactory&& moveFrom) noexcept
{
	pimpl = std::move(moveFrom.pimpl);
	return *this;
}

_Use_decl_annotations_
	size_t
	EffectTextureFactory::CreateTexture(_In_ std::wstring_view name, int32_t descriptorindex)
{
	return pimpl->CreateTexture(name, descriptorindex);
}

void EffectTextureFactory::ReleaseCache()
{
	pimpl->ReleaseCache();
}

void EffectTextureFactory::SetSharing(bool enabled) noexcept
{
	pimpl->SetSharing(enabled);
}

void EffectTextureFactory::EnableForceSRGB(bool forceSRGB) noexcept
{
	pimpl->EnableForceSRGB(forceSRGB);
}

void EffectTextureFactory::EnableAutoGenMips(bool generatemips) noexcept
{
	pimpl->EnableAutoGenMips(generatemips);
}

void EffectTextureFactory::SetDirectory(_In_opt_ std::wstring_view path) noexcept
{
	if (!path.empty())
	{
		wcscpy_s(pimpl->m_path, path.data());
		size_t len = wcsnlen(pimpl->m_path, MAX_PATH);
		if (len > 0 && len < (MAX_PATH - 1))
		{
			// Ensure it has a trailing slash
			if (pimpl->m_path[len - 1] != L'\\')
			{
				pimpl->m_path[len] = L'\\';
				pimpl->m_path[len + 1] = 0;
			}
		}
	}
	else
		*pimpl->m_path = 0;
}

ID3D12DescriptorHeap* EffectTextureFactory::Heap(void) const noexcept
{
	return pimpl->m_texturedescriptorheap.Heap();
}

// Shorthand accessors for the descriptor heap
D3D12_CPU_DESCRIPTOR_HANDLE EffectTextureFactory::GetCpuDescriptorHandle(size_t index) const
{
	return pimpl->m_texturedescriptorheap.GetCpuHandle(index);
}

D3D12_GPU_DESCRIPTOR_HANDLE EffectTextureFactory::GetGpuDescriptorHandle(size_t index) const
{
	return pimpl->m_texturedescriptorheap.GetGpuHandle(index);
}

size_t EffectTextureFactory::ResourceCount() const noexcept
{
	return pimpl->m_resources.size();
}

_Use_decl_annotations_ void __cdecl directxtk::EffectTextureFactory::GetResource(size_t slotindex, _Out_ ID3D12Resource** resource, _Out_opt_ bool* iscubemap /*= nullptr*/)
{
	if (slotindex >= pimpl->m_resources.size())
		throw std::exception("Accessing resource out of range.");

	const auto& textureentry = pimpl->m_resources[slotindex];

	textureentry.m_resource.copy_to(resource);

	if (iscubemap)
	{
		*iscubemap = textureentry.m_iscubemap;
	}
}
