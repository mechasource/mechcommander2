//--------------------------------------------------------------------------------------
// File: BinaryReader.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"

#include "binaryreader.h"

using namespace directxtk;


// Constructor reads from the filesystem.
BinaryReader::BinaryReader(_In_ const std::wstring_view&  filename) noexcept(false) :
    mPos(nullptr),
    mEnd(nullptr)
{
    size_t datasize;

    HRESULT hr = ReadEntireFile(filename, mOwnedData, &datasize);
    if (FAILED(hr))
    {
        DebugTrace("ERROR: BinaryReader failed (%08X) to load '%ls'\n",
            static_cast<uint32_t>(hr), filename);
        throw std::exception("BinaryReader");
    }

    mPos = mOwnedData.get();
    mEnd = mOwnedData.get() + datasize;
}


// Constructor reads from an existing memory buffer.
BinaryReader::BinaryReader(_In_reads_bytes_(datasize) uint8_t const* datablob, size_t datasize) noexcept :
    mPos(datablob),
    mEnd(datablob + datasize)
{
}


// Reads from the filesystem into memory.
HRESULT BinaryReader::ReadEntireFile(_In_ const std::wstring_view&  filename, _Inout_ std::unique_ptr<uint8_t[]>& data, _Out_ size_t* datasize)
{
    // Open the file.
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
    wil::unique_hfile filehandle(::CreateFile2(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr));
#else
    wil::unique_hfile filehandle(::CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
#endif
    if (!filehandle)
        return HRESULT_FROM_WIN32(GetLastError());

    // Get the file size.
    FILE_STANDARD_INFO fileinfo;
    if (!::GetFileInformationByHandleEx(filehandle.get(), FileStandardInfo, &fileinfo, sizeof(fileinfo)))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // File is too big for 32-bit allocation, so reject read.
    if (fileinfo.EndOfFile.HighPart > 0)
        return E_FAIL;

    // Create enough space for the file data.
    data.reset(new uint8_t[fileinfo.EndOfFile.LowPart]);
    if (!data)
        return E_OUTOFMEMORY;

    // Read the data in.
    uint32_t bytesread = 0;

    if (!::ReadFile(filehandle.get(), data.get(), fileinfo.EndOfFile.LowPart, reinterpret_cast<PULONG>(&bytesread), nullptr))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    if (bytesread < fileinfo.EndOfFile.LowPart)
        return E_FAIL;

    *datasize = bytesread;

    return S_OK;
}
