//--------------------------------------------------------------------------------------
// File: WaveBankReader.h
//
// Functions for loading audio data from Wave Banks
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//-------------------------------------------------------------------------------------

#pragma once

#include <stdint.h>
#include <objbase.h>
#include <memory>
#include <mmreg.h>

namespace DirectX
{
class WaveBankReader
{
public:
	WaveBankReader() noexcept(false);

	WaveBankReader(WaveBankReader&&) = default;
	WaveBankReader& operator=(WaveBankReader&&) = default;

	WaveBankReader(WaveBankReader const&) = delete;
	WaveBankReader& operator=(WaveBankReader const&) = delete;

	~WaveBankReader();

	HRESULT Open(_In_z_ const std::wstring_view& szFileName);

	uint32_t Find(_In_z_ const std::string_view& name) const;

	bool IsPrepared();
	void WaitOnPrepare();

	bool HasNames(void) const;
	bool IsStreamingBank(void) const;

	const std::string_view& BankName(void) const;

	uint32_t Count(void) const;

	uint32_t BankAudioSize(void) const;

	HRESULT GetFormat(_In_ uint32_t index, _Out_writes_bytes_(maxsize) WAVEFORMATEX* pFormat, _In_ size_t maxsize) const;

	HRESULT GetWaveData(_In_ uint32_t index, _Outptr_ const uint8_t** pData, _Out_ uint32_t& dataSize) const;

	HRESULT GetSeekTable(_In_ uint32_t index, _Out_ const uint32_t** pData, _Out_ uint32_t& dataCount, _Out_ uint32_t& tag) const;

	HANDLE GetAsyncHandle(void) const;

	struct Metadata
	{
		uint32_t duration;
		uint32_t loopStart;
		uint32_t loopLength;
		uint32_t offsetBytes;
		uint32_t lengthBytes;
	};
	HRESULT GetMetadata(_In_ uint32_t index, _Out_ Metadata& metadata) const;

private:
	// Private implementation.
	class Impl;

	std::unique_ptr<Impl> pImpl;
};
} // namespace DirectX
