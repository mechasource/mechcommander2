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
typedef uint8_t* PUCHAR;
size_t LZDecomp (PUCHAR dest, PUCHAR src, size_t srcLen);
size_t LZCompress (PUCHAR dest,  PUCHAR src, size_t len);

//---------------------------------------------------------------------------
#endif
