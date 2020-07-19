//--------------------------------------------------------------------------------------
// File: BinaryReader.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#pragma once

#include <memory>
#include <exception>
#include <stdexcept>
#include <type_traits>

#include "platformhelpers.h"


namespace directxtk
{
    // Helper for reading binary data, either from the filesystem a memory buffer.
    class BinaryReader
    {
    public:
        explicit BinaryReader(_In_ const std::wstring_view&  filename) noexcept(false);
        BinaryReader(_In_reads_bytes_(datasize) uint8_t const* datablob, size_t datasize) noexcept;

        BinaryReader(BinaryReader const&) = delete;
        BinaryReader& operator= (BinaryReader const&) = delete;
        
        // Reads a single value.
        template<typename T> T const& Read()
        {
            return *ReadArray<T>(1);
        }


        // Reads an array of values.
        template<typename T> T const* ReadArray(size_t elementCount)
        {
            static_assert(std::is_standard_layout<T>::value, "Can only read plain-old-data types");
            uint8_t const* newPos = mPos + sizeof(T) * elementCount;

            if (newPos < mPos)
                throw std::overflow_error("ReadArray");

            if (newPos > mEnd)
                throw std::exception("End of file");

            auto result = reinterpret_cast<T const*>(mPos);

            mPos = newPos;

            return result;
        }


        // Lower level helper reads directly from the filesystem into memory.
        static HRESULT ReadEntireFile(_In_ const std::wstring_view&  filename, _Inout_ std::unique_ptr<uint8_t[]>& data, _Out_ size_t* datasize);


    private:
        // The data currently being read.
        uint8_t const* mPos;
        uint8_t const* mEnd;

        std::unique_ptr<uint8_t[]> mOwnedData;
    };
}
