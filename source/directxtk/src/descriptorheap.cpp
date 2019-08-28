//--------------------------------------------------------------------------------------
// File: DescriptorHeap.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"
#include "platformhelpers.h"
#include "directxhelpers.h"
#include "descriptorheap.h"

using namespace DirectX;

namespace
{
struct DescriptorHeapDesc
{
	D3D12_DESCRIPTOR_HEAP_TYPE Type;
	D3D12_DESCRIPTOR_HEAP_FLAGS flags;
};

static const DescriptorHeapDesc c_DescriptorHeapDescs[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] =
	{
		{D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE},
		{D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE},
		{D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE},
		{D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE}};
} // namespace

_Use_decl_annotations_
DescriptorHeap::DescriptorHeap(
	ID3D12DescriptorHeap* pExistingHeap) :
	m_pHeap(pExistingHeap)
{
	m_hCPU = pExistingHeap->GetCPUDescriptorHandleForHeapStart();
	m_hGPU = pExistingHeap->GetGPUDescriptorHandleForHeapStart();
	m_desc = pExistingHeap->GetDesc();

	wil::com_ptr<ID3D12Device> device;
	pExistingHeap->GetDevice(IID_GRAPHICS_PPV_ARGS(device.addressof()));

	m_increment = device->GetDescriptorHandleIncrementSize(m_desc.Type);
}

_Use_decl_annotations_
DescriptorHeap::DescriptorHeap(
	ID3D12Device* device,
	const D3D12_DESCRIPTOR_HEAP_DESC* pDesc) :
	m_desc{},
	m_hCPU{},
	m_hGPU{},
	m_increment(0)
{
	Create(device, pDesc);
}

_Use_decl_annotations_
DescriptorHeap::DescriptorHeap(
	ID3D12Device* device,
	D3D12_DESCRIPTOR_HEAP_TYPE type,
	D3D12_DESCRIPTOR_HEAP_FLAGS flags,
	size_t count) :
	m_desc{},
	m_hCPU{},
	m_hGPU{},
	m_increment(0)
{
	if (count > UINT32_MAX)
		throw std::exception("Too many descriptors");

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.flags = flags;
	desc.NumDescriptors = static_cast<uint32_t>(count);
	desc.Type = type;
	Create(device, &desc);
}

_Use_decl_annotations_
	D3D12_GPU_DESCRIPTOR_HANDLE
	DescriptorHeap::WriteDescriptors(
		ID3D12Device* device,
		uint32_t offsetIntoHeap,
		uint32_t totalDescriptorCount,
		const D3D12_CPU_DESCRIPTOR_HANDLE* pDescriptorRangeStarts,
		const uint32_t* pDescriptorRangeSizes,
		uint32_t descriptorRangeCount)
{
	_ASSERT((size_t(offsetIntoHeap) + size_t(totalDescriptorCount)) <= size_t(m_desc.NumDescriptors));

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = GetCpuHandle(offsetIntoHeap);

	device->CopyDescriptors(
		1,
		&cpuHandle,
		&totalDescriptorCount,
		descriptorRangeCount,
		pDescriptorRangeStarts,
		pDescriptorRangeSizes,
		m_desc.Type);

	auto gpuHandle = GetGpuHandle(offsetIntoHeap);

	return gpuHandle;
}

_Use_decl_annotations_
	D3D12_GPU_DESCRIPTOR_HANDLE
	DescriptorHeap::WriteDescriptors(
		ID3D12Device* device,
		uint32_t offsetIntoHeap,
		const D3D12_CPU_DESCRIPTOR_HANDLE* pDescriptorRangeStarts,
		const uint32_t* pDescriptorRangeSizes,
		uint32_t descriptorRangeCount)
{
	uint32_t totalDescriptorCount = 0;
	for (uint32_t i = 0; i < descriptorRangeCount; ++i)
		totalDescriptorCount += pDescriptorRangeSizes[i];

	return WriteDescriptors(
		device,
		offsetIntoHeap,
		totalDescriptorCount,
		pDescriptorRangeStarts,
		pDescriptorRangeSizes,
		descriptorRangeCount);
}

_Use_decl_annotations_
	D3D12_GPU_DESCRIPTOR_HANDLE
	DescriptorHeap::WriteDescriptors(
		ID3D12Device* device,
		uint32_t offsetIntoHeap,
		const D3D12_CPU_DESCRIPTOR_HANDLE* pDescriptors,
		uint32_t descriptorCount)
{
	return WriteDescriptors(
		device,
		offsetIntoHeap,
		descriptorCount,
		pDescriptors,
		&descriptorCount,
		1);
}

_Use_decl_annotations_ void
DescriptorHeap::Create(
	ID3D12Device* pDevice,
	const D3D12_DESCRIPTOR_HEAP_DESC* pDesc)
{
	_ASSERT(pDesc != nullptr);

	m_desc = *pDesc;
	m_increment = pDevice->GetDescriptorHandleIncrementSize(pDesc->Type);

	if (pDesc->NumDescriptors == 0)
	{
		m_pHeap.Reset();
		m_hCPU.ptr = 0;
		m_hGPU.ptr = 0;
	}
	else
	{
		ThrowIfFailed(pDevice->CreateDescriptorHeap(
			pDesc,
			IID_GRAPHICS_PPV_ARGS(m_pHeap.ReleaseAndGetAddressOf())));

		SetDebugObjectName(m_pHeap.get(), L"DescriptorHeap");

		m_hCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart();

		if (pDesc->flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
			m_hGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart();
	}
}

_Use_decl_annotations_ void
DescriptorHeap::DefaultDesc(
	D3D12_DESCRIPTOR_HEAP_TYPE type,
	D3D12_DESCRIPTOR_HEAP_DESC* pDesc)
{
	_ASSERT(c_DescriptorHeapDescs[type].Type == type);
	pDesc->flags = c_DescriptorHeapDescs[type].flags;
	pDesc->NumDescriptors = 0;
	pDesc->Type = type;
}

//======================================================================================
// DescriptorPile
//======================================================================================

void
DescriptorPile::AllocateRange(size_t numDescriptors, _Out_ IndexType& start, _Out_ IndexType& end)
{
	// make sure we didn't allocate zero
	if (numDescriptors == 0)
	{
		throw std::out_of_range("Can't allocate zero descriptors");
	}

	// get the current top
	start = m_top;

	// increment top with new request
	m_top += numDescriptors;
	end = m_top;

	// make sure we have enough room
	if (m_top > Count())
	{
		DebugTrace("DescriptorPile has %zu of %zu descriptors; failed request for %zu more\n", start, Count(), numDescriptors);
		throw std::exception("Can't allocate more descriptors");
	}
}
