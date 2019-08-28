//---------------------------------------------------------------------------
//
// LZ Compress/Decompress Headers
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef LZ_H
#define LZ_H

//---------------------------------------------------------------------------
typedef uint8_t* uint8_t*;
size_t
LZDecomp(uint8_t* dest, uint8_t* src, size_t srcLen);
size_t
LZCompress(uint8_t* dest, uint8_t* src, size_t len);

//---------------------------------------------------------------------------
#endif
