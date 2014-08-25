//===========================================================================//
// File:	 ACM.cpp														 //
// Contents: Wrappers for the Audio Compression Manager						 //																		 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _ACM_HPP_
#define _ACM_HPP_

// #include <msacm.h>

/*
This header is not used outside gameos and the implementation is wrappers of
the ACM functions supplied with logging and error handling.
*/

HRESULT __stdcall wACMStreamPrepareHeader(HACMSTREAM hACMStream, LPACMSTREAMHEADER pACMHeader, uint32_t fdwPrepare);
HRESULT __stdcall wACMStreamConvert(HACMSTREAM hACMStream, LPACMSTREAMHEADER pACMHeader, uint32_t flags);
HRESULT __stdcall wACMStreamUnprepareHeader(HACMSTREAM hACMStream, LPACMSTREAMHEADER pACMHeader, uint32_t fdwPrepare);
HRESULT __stdcall wACMStreamClose(HACMSTREAM hACMStream, uint32_t flags);
HRESULT __stdcall wACMStreamOpen(LPHACMSTREAM phas, HACMDRIVER had, LPWAVEFORMATEX pwfxSrc, LPWAVEFORMATEX pwfxDst, LPWAVEFILTER pwfltr, uint32_t dwCallback, uint32_t dwInstance, uint32_t fdwOpen);
HRESULT __stdcall wACMStreamSize(HACMSTREAM has, uint32_t cbInput, puint32_t pdwOutputBytes, uint32_t fdwSize);
HRESULT __stdcall wACMFormatSuggest(HACMDRIVER had, LPWAVEFORMATEX pwfxSrc, LPWAVEFORMATEX pwfxDst, uint32_t cbwfxDst, uint32_t fdwSuggest);

#endif
