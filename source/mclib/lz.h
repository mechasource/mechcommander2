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
typedef uint8_t* puint8_t;
size_t
LZDecomp(puint8_t dest, puint8_t src, size_t srcLen);
size_t
LZCompress(puint8_t dest, puint8_t src, size_t len);

//---------------------------------------------------------------------------
#endif
