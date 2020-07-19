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
        wil::com_ptr<ID3D12Resource> mResource;
        bool mIsCubeMap;
        size_t slot;

        TextureCacheEntry() noexcept : mIsCubeMap(false), slot(0) {}
    };

    using TextureCache = std::map< std::wstring, TextureCacheEntry >;

    Impl(
        _In_ ID3D12Device* device,
        ResourceUploadBatch& resourceUploadBatch,
        _In_ ID3D12DescriptorHeap* descriptorHeap)
        : mPath{}
        , mTextureDescriptorHeap(descriptorHeap)
        , mDevice(device)
        , mResourceUploadBatch(resourceUploadBatch)
        , mSharing(true)
        , mForceSRGB(false)
        , mAutoGenMips(false)
    { 
        *mPath = 0; 
    }

    Impl(
        _In_ ID3D12Device* device,
        ResourceUploadBatch& resourceUploadBatch,
        _In_ size_t numDescriptors,
        _In_ D3D12_DESCRIPTOR_HEAP_FLAGS descriptorHeapFlags)
        : mPath{}
        , mTextureDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, descriptorHeapFlags, numDescriptors)
        , mDevice(device)
        , mResourceUploadBatch(resourceUploadBatch)
        , mSharing(true)
        , mForceSRGB(false)
        , mAutoGenMips(false)
    {
        SetDebugObjectName(mTextureDescriptorHeap.Heap(), L"EffectTextureFactory");
    }

    size_t CreateTexture(_In_ const std::wstring_view& name, int32_t descriptorSlot);

    void ReleaseCache();
    void SetSharing(bool enabled) noexcept { mSharing = enabled; }
    void EnableForceSRGB(bool forceSRGB) noexcept { mForceSRGB = forceSRGB; }
    void EnableAutoGenMips(bool generateMips) noexcept { mAutoGenMips = generateMips; }

    wchar_t mPath[MAX_PATH];

    ::DescriptorHeap               mTextureDescriptorHeap;
    std::vector<TextureCacheEntry> mResources; // flat list of unique resources so we can index into it

private:
    wil::com_ptr<ID3D12Device>           mDevice;
    ResourceUploadBatch&           mResourceUploadBatch;

    TextureCache                   mTextureCache;

    bool                           mSharing;
    bool                           mForceSRGB;
    bool                           mAutoGenMips;

    std::mutex                     mutex;
};


_Use_decl_annotations_
size_t EffectTextureFactory::Impl::CreateTexture(_In_ const std::wstring_view& name, int32_t descriptorSlot)
{
    if (!name)
        throw std::exception("invalid arguments");

    auto it = mTextureCache.find(name);

    TextureCacheEntry textureEntry = {};

    if (mSharing && it != mTextureCache.end())
    {
        textureEntry = it->second;
    }
    else
    {
        wchar_t fullName[MAX_PATH] = {};
        wcscpy_s(fullName, mPath);
        wcscat_s(fullName, name);

        WIN32_FILE_ATTRIBUTE_DATA fileAttr = {};
        if (!GetFileAttributesExW(fullName, GetFileExInfoStandard, &fileAttr))
        {
            // Try Current Working Directory (CWD)
            wcscpy_s(fullName, name);
            if (!GetFileAttributesExW(fullName, GetFileExInfoStandard, &fileAttr))
            {
                DebugTrace("ERROR: EffectTextureFactory could not find texture file '%ls'\n", name);
                throw std::exception("CreateTexture");
            }
        }

        wchar_t ext[_MAX_EXT];
        _wsplitpath_s(name, nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

        DDS_LOADER_FLAGS loadFlags = DDS_LOADER_DEFAULT;
        if (mForceSRGB)
            loadFlags |= DDS_LOADER_FORCE_SRGB;
        if (mAutoGenMips)
            loadFlags |= DDS_LOADER_MIP_AUTOGEN;

        static_assert(static_cast<int32_t>(DDS_LOADER_DEFAULT) == static_cast<int32_t>(WIC_LOADER_DEFAULT), "DDS/WIC Load flags mismatch");
        static_assert(static_cast<int32_t>(DDS_LOADER_FORCE_SRGB) == static_cast<int32_t>(WIC_LOADER_FORCE_SRGB), "DDS/WIC Load flags mismatch");
        static_assert(static_cast<int32_t>(DDS_LOADER_MIP_AUTOGEN) == static_cast<int32_t>(WIC_LOADER_MIP_AUTOGEN), "DDS/WIC Load flags mismatch");
        static_assert(static_cast<int32_t>(DDS_LOADER_MIP_RESERVE) == static_cast<int32_t>(WIC_LOADER_MIP_RESERVE), "DDS/WIC Load flags mismatch");

        if (_wcsicmp(ext, L".dds") == 0)
        {
            HRESULT hr = CreateDDSTextureFromFileEx(
                mDevice.get(),
                mResourceUploadBatch,
                fullName,
                0u,
                D3D12_RESOURCE_FLAG_NONE,
                loadFlags,
                textureEntry.mResource.put(),
                nullptr,
                &textureEntry.mIsCubeMap);
            if (FAILED(hr))
            {
                DebugTrace("ERROR: CreateDDSTextureFromFile failed (%08X) for '%ls'\n",
                    static_cast<uint32_t>(hr), fullName);
                throw std::exception("CreateDDSTextureFromFile");
            }
        }
        else
        {
            textureEntry.mIsCubeMap = false;

            HRESULT hr = CreateWICTextureFromFileEx(
                mDevice.get(),
                mResourceUploadBatch,
                fullName,
                0u,
                D3D12_RESOURCE_FLAG_NONE,
                static_cast<WIC_LOADER_FLAGS>(loadFlags),
                textureEntry.mResource.put());
            if (FAILED(hr))
            {
                DebugTrace("ERROR: CreateWICTextureFromFile failed (%08X) for '%ls'\n",
                    static_cast<uint32_t>(hr), fullName);
                throw std::exception("CreateWICTextureFromFile");
            }
        }

        std::lock_guard<std::mutex> lock(mutex);
        textureEntry.slot = mResources.size();
        if (mSharing)
        {
            TextureCache::value_type v(name, textureEntry);
            mTextureCache.insert(v);
        }
        mResources.push_back(textureEntry);
    }

    assert(textureEntry.mResource != nullptr);

    // bind a new descriptor in slot 
    auto textureDescriptor = mTextureDescriptorHeap.GetCpuHandle(static_cast<size_t>(descriptorSlot));
    directxtk::CreateShaderResourceView(mDevice.get(), textureEntry.mResource.get(), textureDescriptor, textureEntry.mIsCubeMap);

    return textureEntry.slot;
}

void EffectTextureFactory::Impl::ReleaseCache()
{
    std::lock_guard<std::mutex> lock(mutex);
    mTextureCache.clear();
}



//--------------------------------------------------------------------------------------
// EffectTextureFactory
//--------------------------------------------------------------------------------------

_Use_decl_annotations_
EffectTextureFactory::EffectTextureFactory(
    ID3D12Device* device,
    ResourceUploadBatch& resourceUploadBatch,
    ID3D12DescriptorHeap* descriptorHeap) noexcept(false)
{
    pimpl = std::make_unique<Impl>(device, resourceUploadBatch, descriptorHeap);
}

_Use_decl_annotations_
EffectTextureFactory::EffectTextureFactory(
    ID3D12Device* device,
    ResourceUploadBatch& resourceUploadBatch,
    size_t numDescriptors,
    D3D12_DESCRIPTOR_HEAP_FLAGS descriptorHeapFlags) noexcept(false)
{
    pimpl = std::make_unique<Impl>(device, resourceUploadBatch, numDescriptors, descriptorHeapFlags);
}

EffectTextureFactory::~EffectTextureFactory()
{
}


EffectTextureFactory::EffectTextureFactory(EffectTextureFactory&& moveFrom) noexcept
    : pimpl(std::move(moveFrom.pimpl))
{
}

EffectTextureFactory& EffectTextureFactory::operator= (EffectTextureFactory&& moveFrom) noexcept
{
    pimpl = std::move(moveFrom.pimpl);
    return *this;
}

_Use_decl_annotations_
size_t EffectTextureFactory::CreateTexture(_In_ const std::wstring_view& name, int32_t descriptorIndex)
{
    return pimpl->CreateTexture(name, descriptorIndex);
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

void EffectTextureFactory::EnableAutoGenMips(bool generateMips) noexcept
{
    pimpl->EnableAutoGenMips(generateMips);
}

void EffectTextureFactory::SetDirectory(_In_opt_ const std::wstring_view& path) noexcept
{
    if (path && *path != 0)
    {
        wcscpy_s(pimpl->mPath, path);
        size_t len = wcsnlen(pimpl->mPath, MAX_PATH);
        if (len > 0 && len < (MAX_PATH - 1))
        {
            // Ensure it has a trailing slash
            if (pimpl->mPath[len - 1] != L'\\')
            {
                pimpl->mPath[len] = L'\\';
                pimpl->mPath[len + 1] = 0;
            }
        }
    }
    else
        *pimpl->mPath = 0;
}

ID3D12DescriptorHeap* EffectTextureFactory::Heap() const noexcept
{
    return pimpl->mTextureDescriptorHeap.Heap();
}

// Shorthand accessors for the descriptor heap
D3D12_CPU_DESCRIPTOR_HANDLE EffectTextureFactory::GetCpuDescriptorHandle(size_t index) const
{
    return pimpl->mTextureDescriptorHeap.GetCpuHandle(index);
}

D3D12_GPU_DESCRIPTOR_HANDLE EffectTextureFactory::GetGpuDescriptorHandle(size_t index) const
{
    return pimpl->mTextureDescriptorHeap.GetGpuHandle(index);
}

size_t EffectTextureFactory::ResourceCount() const noexcept
{
    return pimpl->mResources.size();
}

_Use_decl_annotations_
void EffectTextureFactory::GetResource(size_t slot, ID3D12Resource** resource, bool* isCubeMap)
{
    if (slot >= pimpl->mResources.size())
        throw std::exception("Accessing resource out of range.");

    const auto& textureEntry = pimpl->mResources[slot];

    textureEntry.mResource.copy_to(resource);

    if (isCubeMap)
    {
        *isCubeMap = textureEntry.mIsCubeMap;
    }
}
