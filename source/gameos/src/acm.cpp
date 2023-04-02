/*******************************************************************************
 This file consist of reversed and interpreted code from object source in the
 x86 debug build gameos.lib. The code in this file can not be used in any way
 other than serve as an information reference to the original released source of
 Mechcommander2. The code is a work of progress and there is no guarantee it is
 complete, accurate or useful in any way. The purpose is instead to make it
 possible to safely remove any dependencies of gameos.lib from Mechcommander2.
 All code is logically copyrighted to Microsoft
*******************************************************************************/
/*******************************************************************************
 acm.cpp - gameos Audio Compression Manager wrappers

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

#include "stdinc.h"

//#include <mmreg.h>
//#include <msacm.h>
#include "gameos.hpp"
#include "errorhandler.hpp"
#include "acm.hpp"

// -----------------------------------------------------------------------------
// Global data exported from this module

// global referenced data not listed in headers

// local data

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers

// global implemented functions not listed in headers

// local functions

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers

#define MMFAILED(x) ((x & 0x80000000u) != 0)

#if _JUST_SOME_INFO
// implemented function declarations
HRESULT __stdcall wACMStreamPrepareHeader(
	HACMSTREAM has, PACMSTREAMHEADER pash, uint32_t fdwPrepare);
HRESULT __stdcall wACMStreamConvert(HACMSTREAM has, PACMSTREAMHEADER pash, uint32_t fdwConvert);
HRESULT __stdcall wACMStreamUnprepareHeader(
	HACMSTREAM has, PACMSTREAMHEADER pash, uint32_t fdwUnprepare);
HRESULT __stdcall wACMStreamClose(HACMSTREAM has, uint32_t fdwClose);
HRESULT __stdcall wACMStreamOpen(PHACMSTREAM phas, HACMDRIVER had, const PWAVEFORMATEX pwfxsrc,
	const PWAVEFORMATEX pwfxDst, PWAVEFILTER pwfltr, ULONG_PTR dwCallback, ULONG_PTR dwInstance,
	uint32_t fdwOpen);
HRESULT __stdcall wACMStreamSize(
	HACMSTREAM has, uint32_t cbInput, uint32_t* pdwOutputBytes, uint32_t fsize);
HRESULT __stdcall wACMFormatSuggest(HACMDRIVER had, const struct tWAVEFORMATEX* pwfxsrc,
	const struct tWAVEFORMATEX* pwfxDst, uint32_t cbwfxDst, uint32_t fdwSuggest);
#endif

#pragma region wACMStreamPrepareHeader
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wACMStreamPrepareHeader</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="has"></param>
/// <param name="pash"></param>
/// <param name="fdwPrepare"></param>
/// <returns></returns>
HRESULT __stdcall wACMStreamPrepareHeader(
	HACMSTREAM has, PACMSTREAMHEADER pash, uint32_t fdwPrepare)
{
	PSTR errormessage;
	MMRESULT hr;
	InternalFunctionSpew(
		"GameOS_DirectSound", "acmStreamPrepareHeader(0x%x, 0x%x, 0x%x)", has, pash, fdwPrepare);
	hr = acmStreamPrepareHeader(has, pash, fdwPrepare);
	if (MMFAILED(hr))
	{
		errormessage = ErrorNumberToMessage(hr); // likely to take a uint32_t
		// PAUSE(
		if (InternalFunctionPause("FAILED (0x%x - %s) - acmStreamPrepareHeader(0x%x, 0x%x, 0x%x)",
				hr, errormessage, has, pash, fdwPrepare))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wACMStreamConvert
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wACMStreamConvert</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="has"></param>
/// <param name="pash"></param>
/// <param name="fdwConvert"></param>
/// <returns></returns>
HRESULT __stdcall wACMStreamConvert(HACMSTREAM has, PACMSTREAMHEADER pash, uint32_t fdwConvert)
{
	PSTR errormessage;
	MMRESULT hr;
	InternalFunctionSpew(
		"GameOS_DirectSound", "acmStreamConvert(0x%x, 0x%x, 0x%x)", has, pash, fdwConvert);
	hr = acmStreamConvert(has, pash, fdwConvert);
	if (MMFAILED(hr))
	{
		errormessage = ErrorNumberToMessage(hr);
		// PAUSE(
		if (InternalFunctionPause("FAILED (0x%x - %s) - acmStreamConvert(0x%x, 0x%x, 0x%x)",
				hr, errormessage, has, pash, fdwConvert))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wACMStreamUnprepareHeader
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wACMStreamUnprepareHeader</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="has"></param>
/// <param name="pash"></param>
/// <param name="fdwUnprepare"></param>
/// <returns></returns>
HRESULT __stdcall wACMStreamUnprepareHeader(
	HACMSTREAM has, PACMSTREAMHEADER pash, uint32_t fdwUnprepare)
{
	PSTR errormessage;
	MMRESULT hr;
	InternalFunctionSpew("GameOS_DirectSound", "acmStreamUnprepareHeader(0x%x, 0x%x, 0x%x)", has,
		pash, fdwUnprepare);
	hr = acmStreamUnprepareHeader(has, pash, fdwUnprepare);
	if (MMFAILED(hr))
	{
		errormessage = ErrorNumberToMessage(hr);
		// PAUSE(
		if (InternalFunctionPause("FAILED (0x%x - %s) - "
								  "acmStreamUnprepareHeader(0x%x, 0x%x, 0x%x)",
				hr, errormessage, has, pash, fdwUnprepare))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wACMStreamClose
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wACMStreamClose</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="has"></param>
/// <param name="fdwClose"></param>
/// <returns></returns>
HRESULT __stdcall wACMStreamClose(HACMSTREAM has, uint32_t fdwClose)
{
	PSTR errormessage;
	MMRESULT hr;
	InternalFunctionSpew("GameOS_DirectSound", "acmStreamClose(0x%x, 0x%x)", has, fdwClose);
	hr = acmStreamClose(has, fdwClose);
	if (MMFAILED(hr))
	{
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - acmStreamClose(0x%x, 0x%x, 0x%x)", hr,
				errormessage, has, fdwClose))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wACMStreamOpen
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wACMStreamOpen</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="phas"></param>
/// <param name="had"></param>
/// <param name="pwfxsrc"></param>
/// <param name="pwfxDst"></param>
/// <param name="pwfltr"></param>
/// <param name="dwCallback"></param>
/// <param name="dwInstance"></param>
/// <param name="fdwOpen"></param>
/// <returns></returns>
HRESULT __stdcall wACMStreamOpen(PHACMSTREAM phas, HACMDRIVER had, const PWAVEFORMATEX pwfxsrc,
	const PWAVEFORMATEX pwfxDst, PWAVEFILTER pwfltr, ULONG_PTR dwCallback, ULONG_PTR dwInstance,
	uint32_t fdwOpen)
{
	PSTR pszwfDst;
	PSTR pszwfSrc;
	PSTR errormessage;
	MMRESULT hr;
	char BUffer2[128];
	char Buffer[128];
	pszwfDst = GetWaveFormat(Buffer, pwfxDst);
	pszwfSrc = GetWaveFormat(BUffer2, pwfxsrc);
	InternalFunctionSpew("GameOS_DirectSound",
		"acmStreamOpen(0x%x, 0x%x, %s, %s, 0x%x,0x%x, 0x%x, 0x%x)", phas, had, pszwfSrc, pszwfDst,
		pwfltr, dwCallback, dwInstance, fdwOpen);
	hr = acmStreamOpen(phas, had, pwfxsrc, pwfxDst, pwfltr, dwCallback, dwInstance, 1u);
	if (MMFAILED(hr))
	{
		pszwfDst = GetWaveFormat(Buffer, pwfxDst);
		pszwfSrc = GetWaveFormat(BUffer2, pwfxsrc);
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - acmStreamOpen(0x%x, "
								  "0x%x, %s, %s, 0x%x,0x%x, 0x%x, 0x%x)",
				hr, errormessage, phas, had, pszwfSrc, pszwfDst, pwfltr, dwCallback,
				dwInstance, fdwOpen))
			ENTER_DEBUGGER;
	}
	hr = acmStreamOpen(phas, had, pwfxsrc, pwfxDst, pwfltr, dwCallback, dwInstance, fdwOpen);
	if (MMFAILED(hr))
	{
		pszwfDst = GetWaveFormat(Buffer, pwfxDst);
		pszwfSrc = GetWaveFormat(BUffer2, pwfxsrc);
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - acmStreamOpen(0x%x, "
								  "0x%x, %s, %s, 0x%x,0x%x, 0x%x, 0x%x)",
				hr, errormessage, phas, had, pszwfSrc, pszwfDst, pwfltr, dwCallback,
				dwInstance, fdwOpen))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wACMStreamSize
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wACMStreamSize</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="has"></param>
/// <param name="cbInput"></param>
/// <param name="pdwOutputBytes"></param>
/// <param name="fsize"></param>
/// <returns></returns>
HRESULT __stdcall wACMStreamSize(
	HACMSTREAM has, uint32_t cbInput, uint32_t* pdwOutputBytes, uint32_t fsize)
{
	PSTR errormessage;
	MMRESULT hr;
	InternalFunctionSpew("GameOS_DirectSound", "acmStreamSize(0x%x, 0x%x, 0x%x, 0x%x)", has,
		cbInput, pdwOutputBytes, fsize);
	hr = acmStreamSize(has, cbInput, pdwOutputBytes, fsize);
	if (MMFAILED(hr))
	{
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - acmStreamSize(0x%x, 0x%x, 0x%x, 0x%x)",
				hr, errormessage, has, cbInput, pdwOutputBytes, fsize))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wACMFormatSuggest
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wACMFormatSuggest</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="had"></param>
/// <param name="pwfxsrc"></param>
/// <param name="pwfxDst"></param>
/// <param name="cbwfxDst"></param>
/// <param name="fdwSuggest"></param>
/// <returns></returns>
HRESULT __stdcall wACMFormatSuggest(HACMDRIVER had, const PWAVEFORMATEX pwfxsrc,
	const PWAVEFORMATEX pwfxDst, uint32_t cbwfxDst, uint32_t fdwSuggest)
{
	PSTR pszwfDst;
	PSTR pszwfSrc;
	PSTR errormessage;
	char Buffer2[128];
	char Buffer1[128];
	MMRESULT hr;
	pszwfDst = GetWaveFormat(Buffer1, pwfxDst);
	pszwfSrc = GetWaveFormat(Buffer2, pwfxsrc);
	InternalFunctionSpew("GameOS_DirectSound", "acmFormatSuggest(0x%x, %s, %s, 0x%x, 0x%x)", had,
		pszwfSrc, pszwfDst, cbwfxDst, fdwSuggest);
	hr = acmFormatSuggest(had, pwfxsrc, pwfxDst, cbwfxDst, fdwSuggest);
	if (MMFAILED(hr))
	{
		pszwfDst = GetWaveFormat(Buffer1, pwfxDst);
		pszwfSrc = GetWaveFormat(Buffer2, pwfxsrc);
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - acmFormatSuggest(0x%x, "
								  "%s, %s, 0x%x, 0x%x)",
				hr, errormessage, had, pszwfSrc, pszwfDst, cbwfxDst, fdwSuggest))
			ENTER_DEBUGGER;
	}
	return hr;
}
