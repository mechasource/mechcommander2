//--------------------------------------------------------------------------------------
// File: DemandCreate.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#pragma once

#include "platformhelpers.h"


namespace directxtk
{
    // Helper for lazily creating a D3D resource.
    template<typename T, typename TCreateFunc>
    inline T* DemandCreate(wil::com_ptr<T>& iptr, std::mutex& mutex, TCreateFunc createFunc)
    {
        T* result = iptr.get();

        // Double-checked lock pattern.
        MemoryBarrier();

        if (!result)
        {
            std::lock_guard<std::mutex> lock(mutex);

            result = iptr.get();
        
            if (!result)
            {
                // Create the new object.
                ThrowIfFailed(
                    createFunc(&result)
                );

                MemoryBarrier();

                iptr.attach(result);
            }
        }

        return result;
    }
}
