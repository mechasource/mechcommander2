//===========================================================================//
// File:	 ACM.cpp														 //
// Contents: Wrappers for the Audio Compression Manager						 //																		 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#include <msacm.h>

/*
This header is not used outside gameos and the implementation is wrappers of
the ACM functions supplied with logging and error handling.
*/

HRESULT __stdcall wACMStreamPrepareHeader(HACMSTREAM hACMStream, LPACMSTREAMHEADER pACMHeader, ULONG fdwPrepare);
HRESULT __stdcall wACMStreamConvert(HACMSTREAM hACMStream, LPACMSTREAMHEADER pACMHeader, ULONG flags);
HRESULT __stdcall wACMStreamUnprepareHeader(HACMSTREAM hACMStream, LPACMSTREAMHEADER pACMHeader, ULONG fdwPrepare);
HRESULT __stdcall wACMStreamClose(HACMSTREAM hACMStream, ULONG flags);
HRESULT __stdcall wACMStreamOpen(LPHACMSTREAM phas, HACMDRIVER had, LPWAVEFORMATEX pwfxSrc, LPWAVEFORMATEX pwfxDst, LPWAVEFILTER pwfltr, ULONG dwCallback, ULONG dwInstance, ULONG fdwOpen);
HRESULT __stdcall wACMStreamSize(HACMSTREAM has, ULONG cbInput, PULONG pdwOutputBytes, ULONG fdwSize);
HRESULT __stdcall wACMFormatSuggest(HACMDRIVER had, LPWAVEFORMATEX pwfxSrc, LPWAVEFORMATEX pwfxDst, ULONG cbwfxDst, ULONG fdwSuggest);
