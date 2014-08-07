//===========================================================================//
// File:	 ACM.cpp														 //
// Contents: Wrappers for the Audio Compression Manager						 //																		 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#include <msacm.h>

MMRESULT __stdcall wACMStreamPrepareHeader( HACMSTREAM hACMStream, ACMSTREAMHEADER *ACMHeader, ULONG fdwPrepare );
MMRESULT __stdcall wACMStreamConvert( HACMSTREAM hACMStream, ACMSTREAMHEADER *ACMHeader, ULONG flags );
MMRESULT __stdcall wACMStreamUnprepareHeader( HACMSTREAM hACMStream, ACMSTREAMHEADER *ACMHeader, ULONG fdwPrepare );
MMRESULT __stdcall wACMStreamClose( HACMSTREAM hACMStream, ULONG flags );
MMRESULT __stdcall wACMStreamOpen( LPHACMSTREAM phas, HACMDRIVER had, LPWAVEFORMATEX pwfxSrc, LPWAVEFORMATEX pwfxDst, LPWAVEFILTER pwfltr, ULONG dwCallback, ULONG dwInstance, ULONG fdwOpen );
MMRESULT __stdcall wACMStreamSize( HACMSTREAM has, ULONG cbInput, LPDWORD pdwOutputBytes, ULONG fdwSize );
MMRESULT __stdcall wACMFormatSuggest( HACMDRIVER had, LPWAVEFORMATEX pwfxSrc, LPWAVEFORMATEX pwfxDst, ULONG cbwfxDst, ULONG fdwSuggest );
