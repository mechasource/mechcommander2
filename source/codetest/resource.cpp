/*******************************************************************************
 module.cpp - implementation of DLL entrypoint and DLL exports

 codetest

 2018-09-03 Jerker Bäck, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

#include "stdinc.h"
#include "resource.h"

#ifndef IN_TEST_MODE
#define IN_TEST_MODE 0
#endif

/*
 An alternative to Win32 function LoadString. 
 We want to mimic a C library function behavior
*/

// Resource string storage - note structure packing!
typedef struct RES_STRINGTABLE
{
	uint16_t count;
#pragma warning(suppress : 4200)
	wchar_t charray[];
} RES_STRINGTABLE;
typedef RES_STRINGTABLE* PRES_STRINGTABLE;

std::wstring_view __stdcall LoadResourceString(
	_In_opt_ HMODULE insthandle,
	_Out_ HRESULT& hr,
	_In_ uint32_t resid,
	_In_ uint16_t langid);

/// <summary>
/// <c>LoadResourceString</c> is a modified version of the LoadString function.
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="insthandle">resource DLL handle</param>
/// <param name="hr">HRESULT</param>
/// <param name="resid">resource ID</param>
/// <param name="langid">resource language</param>
/// <returns>const reference to resource</returns>
std::wstring_view __stdcall LoadResourceString(_In_opt_ HMODULE insthandle, _Out_ HRESULT& hr,
	_In_ uint32_t resid, _In_ uint16_t langid)
{
	HRSRC reshandle = nullptr;
	HGLOBAL memhandle = nullptr;
	PRES_STRINGTABLE pstringtable = nullptr;

	// string tables are counted UTF16 strings grouped together in bundles of sixteen
	// https://blogs.msdn.microsoft.com/oldnewthing/20040130-00/?p=40813/

	const wchar_t* txtid = MAKEINTRESOURCEW(((resid >> 4) + 1) & USHRT_MAX);
	if (langid == USHRT_MAX) // invalid
		langid = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US); // 1033, 0x409

	reshandle = ::FindResourceExW(insthandle, RT_STRING, txtid, langid);
	if (reshandle == nullptr)
	{
		langid = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
		reshandle = ::FindResourceExW(insthandle, RT_STRING, txtid, langid);
		if (reshandle == nullptr)
		{
			// try strings in English as a last resort
			langid = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
			reshandle = ::FindResourceExW(insthandle, RT_STRING, txtid, langid);
			if (reshandle == nullptr)
				throw std::system_error(static_cast<int>(::GetLastError()), std::system_category(), __func__);
		}
	}

	memhandle = ::LoadResource(insthandle, reshandle);
	if (memhandle)
	{
		pstringtable = static_cast<PRES_STRINGTABLE>(::LockResource(memhandle));
	}
	if ((memhandle == nullptr) || (pstringtable == nullptr))
		throw std::system_error(static_cast<int>(::GetLastError()), std::system_category(), __func__);

	// from atlcore.h, see struct _RES_STRINGTABLE
	uint32_t ressize = ::SizeofResource(insthandle, reshandle);
	uintptr_t resptr = reinterpret_cast<uintptr_t>(pstringtable);
	uintptr_t resend = resptr + ressize;

	uint32_t index = resid & 0x000f;
	while ((index > 0) && (resptr < resend))
	{
		resptr += sizeof(*pstringtable) + (pstringtable->count * sizeof(wchar_t));
		pstringtable = reinterpret_cast<PRES_STRINGTABLE>(resptr);
		index--;
	}

	hr = S_OK;
	return std::wstring_view(pstringtable->charray, pstringtable->count);
}

#if IN_TEST_MODE
HRESULT testresource()
{
	HRESULT hr = EXIT_SUCCESS;

	HMODULE modulehandle = ::LoadLibraryW(L"mc2res.dll");
	HMODULE modulehandle2 = ::LoadLibraryW(L"mc2res.dll");
	_ASSERT(modulehandle == modulehandle2);

	if (modulehandle == nullptr)
		return EXIT_FAILURE;

	// MSDN: If nBufferMax is zero, then lpBuffer receives a read-only pointer to the resource itself.
	PWSTR bufferptr = nullptr;
	hr = ::LoadStringW(modulehandle, IDS_LARGE_PULSE_DESC, reinterpret_cast<PWSTR>(&bufferptr), 0);
	std::wstring_view resstring1(bufferptr, static_cast<size_t>(hr));

	std::wstring_view resstring2 = LoadResourceString(modulehandle, hr, IDS_LARGE_PULSE_DESC, USHRT_MAX);

	_ASSERT(resstring1.compare(resstring2) == 0);

	return (SUCCEEDED(hr)) ? EXIT_SUCCESS : EXIT_FAILURE;
}
#endif

#if 0
int LoadStringOrError(_In_opt_ HMODULE hModule,_Out_ std::wstring_view& pBuffer,_In_ uint32_t uID,_In_ uint16_t wLanguage = USHRT_MAX)
{
	HRSRC		hResource;
	HGLOBAL		hGlobal;
	PVOID		pStringResource;
	uint32_t	nResourceSize;
	uint32_t	iIndex;

	//_In_opt_ HINSTANCE hinstance,
	//_In_ uint32_t uID,
	//_Out_writes_to_(cchBufferMax,return + 1) LPWSTR lpBuffer,
	//_In_ int cchBufferMax

	//HANDLE	hStringSeg;
	std::wstring_view		pszResource;
	size_t		ncount;
	
	(void)hModule;(void)uID;(void)wLanguage;(void)pBuffer;

	//const ATLSTRINGRESOURCEIMAGE* pImage;
	//const ATLSTRINGRESOURCEIMAGE* pImageEnd;
	//ULONG nResourceSize;
	//HGLOBAL hGlobal;
	//uint32_t iIndex;



	/*
	 * Make sure the parms are valid.
	 */
	ncount = 0;
	/*
	 * string Tables are broken up into 16 string segments. Find the segment
	 * containing the string we are interested in.
	 */


	std::wstring_view pszID = MAKEINTRESOURCEW(((uID >> 4) + 1) & USHRT_MAX);
	if (wLanguage == USHRT_MAX)	// invalid
		wLanguage = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);	// 1033, 0x409

	hResource = ::FindResourceExW(hModule, RT_STRING, pszID, wLanguage);
	if (hResource == nullptr)
	{
		// Mechcommander strings are in English (if not translated)
		wLanguage = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);	// 1033, 0x409
		hResource = ::FindResourceExW(hModule, RT_STRING, pszID, wLanguage);
		if (hResource == nullptr)
		{
			wLanguage = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
			hResource = ::FindResourceExW(hModule, RT_STRING, pszID, wLanguage);
			if (hResource == nullptr)
				throw std::system_error(static_cast<int>(::GetLastError()), std::system_category(), __func__);
		}
	}

	hGlobal = ::LoadResource(hModule, hResource);
	if (hGlobal == nullptr)
		throw std::system_error(static_cast<int>(::GetLastError()), std::system_category(), __func__);

	pStringResource = ::LockResource(hGlobal);
	if (pStringResource == nullptr)
		throw std::system_error(static_cast<int>(::GetLastError()), std::system_category(), __func__);

	/*
	* Move past the other strings in this segment.
	* (16 strings in a segment -> & 0x0F)
	*/
	pszResource = static_cast<std::wstring_view>(pStringResource);
	iIndex = uID & 0x000f;
	while (true)
	{
		ncount = *((wchar_t*)pszResource++);       // PASCAL like string count
		// first UTCHAR is count if TCHARs
		if (iIndex-- == 0) break;
		pszResource += ncount;                    // Step to start if next string
	}

	std::wstring_view winbasestring(pszResource, ncount);

#if 0
	struct ATLSTRINGRESOURCEIMAGE
	{
		uint16_t nLength;
		wchar_t achString[];
	};
	typedef struct _UNICODE_STRING {
		USHORT Length;
		USHORT MaximumLength;
		PWCH   Buffer;
	} UNICODE_STRING;
#endif

	// from atlcore 
	nResourceSize = ::SizeofResource(hModule, hResource);
	PRES_STRINGTABLE pImage = static_cast<PRES_STRINGTABLE>(pStringResource);
	PRES_STRINGTABLE pImageEnd = reinterpret_cast<PRES_STRINGTABLE>(reinterpret_cast<PBYTE>(pImage) + nResourceSize);

	size_t nResourceptr = reinterpret_cast<size_t>(pImage);
	size_t nResourceEnd = nResourceptr + nResourceSize;

	iIndex = uID & 0x000f;
	// while((iIndex > 0) && (pImage < pImageEnd))
	while((iIndex > 0) && (nResourceptr < nResourceEnd))
	{
		nResourceptr += sizeof(*pImage) + (pImage->count * sizeof(wchar_t));
		pImage = reinterpret_cast<PRES_STRINGTABLE>(nResourceptr);
		iIndex--;
	}

	std::wstring_view atlcorestring(pImage->charray, pImage->count);

#if 0
	if( pImage >= pImageEnd )
	{
		return( NULL );
	}
	if( pImage->nLength == 0 )
	{
		return( NULL );
	}

	return( pImage );
#endif

#if 0
	LPCWSTR FindStringResourceEx(HINSTANCE hinst,
		uint32_t uId, uint32_t langId)
	{
		// Convert the string ID into a bundle number
		LPCWSTR pwsz = NULL;
		HRSRC hrsrc = FindResourceEx(hinst, RT_STRING,
			MAKEINTRESOURCE(uId / 16 + 1),
			langId);
		if (hrsrc) {
			HGLOBAL hglob = LoadResource(hinst, hrsrc);
			if (hglob) {
				pwsz = reinterpret_cast<LPCWSTR>
					(LockResource(hglob));
				if (pwsz) {
					// okay now walk the string table
					for (int i = 0; i < uId & 15; i++) {
						pwsz += 1 + (uint32_t)*pwsz;
					}
					UnlockResource(pwsz);
				}
				FreeResource(hglob);
			}
		}
		return pwsz;
	}
#endif

#if 0
	if (hResInfo = FINDRESOURCEEXW(hModule, (std::wstring_view)ULongToPtr(((long32_t)(((USHORT)wID >> 4) + 1))), RT_STRING, wLangId))
	{
		/*
		 * Load that segment.
		 */
		hGlobal = ::LoadResource( hinstance, hResource );
		hStringSeg = LOADRESOURCE(hModule, hResInfo);
		/*
		 * Lock the resource.
		 */
		if (lpsz = (std::wstring_view)LOCKRESOURCE(hStringSeg, hModule))
		{
			/*
			 * Move past the other strings in this segment.
			 * (16 strings in a segment -> & 0x0F)
			 */
			wID &= 0x0F;
			while (TRUE)
			{
				cch = *((UTCHAR*)lpsz++);       // PASCAL like string count
				// first UTCHAR is count if TCHARs
				if (wID-- == 0) break;
				lpsz += cch;                    // Step to start if next string
			}
			/*
			 * chhBufferMax == 0 means return a pointer to the read-only resource buffer.
			 */
			if (cchBufferMax == 0)
			{
				*(std::wstring_view*)lpBuffer = lpsz;
			}
			else
			{
				/*
				 * Account for the NULL
				 */
				cchBufferMax--;
				/*
				 * Don't copy more than the max allowed.
				 */
				if (cch > cchBufferMax)
					cch = cchBufferMax;
				/*
				 * Copy the string into the buffer.
				 */
				RtlCopyMemory(lpBuffer, lpsz, cch * sizeof(wchar_t));
			}
			/*
			 * Unlock resource, but don't free it - better performance this
			 * way.
			 */
			UNLOCKRESOURCE(hStringSeg, hModule);
		}
	}
	/*
	 * Append a NULL.
	 */
	if (cchBufferMax != 0)
	{
		lpBuffer[cch] = 0;
	}

#endif

#if CONSIDERED_OBSOLETE
	UnlockResource(pStringResource);
	FreeResource(hGlobal);
#endif

	return ncount;
}
#endif