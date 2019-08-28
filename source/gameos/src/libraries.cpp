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
 libraries.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

#include "stdinc.h"

#include "gameos.hpp"
#include "toolos.hpp"
#include "windows.hpp"

#pragma warning(disable : 4191) // 'type cast' : unsafe conversion from 'FARPROC'

typedef int32_t(__stdcall* PDIRECTXSETUPGETVERSION)(uint32_t* pdwVersion, uint32_t* pdwMinorVersion);
typedef HRESULT(__stdcall* PDIRECTDRAWCREATEEX)(GUID* pGUID, PVOID* ppDD, REFIID iid, LPUNKNOWN punkouter);
typedef HRESULT(__stdcall* PDIRECTDRAWENUMERATEA)(LPDDENUMCALLBACKA pCallback, PVOID pcontext);
typedef HRESULT(__stdcall* PDIRECTDRAWENUMERATEEXA)(LPDDENUMCALLBACKEXA pCallback, PVOID pcontext, uint32_t flags);
typedef HRESULT(__stdcall* PDIRECTSOUNDCREATE)(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN punkouter);
typedef HRESULT(__stdcall* PDIRECTSOUNDENUMERATEA)(LPDSENUMCALLBACKA pDSEnumCallback, PVOID pcontext);
typedef HRESULT(__stdcall* PDIRECTINPUTCREATEEX)(HINSTANCE hinstance, uint32_t version, REFIID riidltf, PVOID* ppvOut, LPUNKNOWN punkOuter);
typedef LPDIRECT3DVERTEXBUFFER7(__stdcall* GETDXFUNC)(LPDIRECT3DVERTEXBUFFER7, LPDIRECT3DDEVICE7);

enum IJLERR(__stdcall* PIJLINIT)(struct _JPEG_CORE_PROPERTIES* jcprops);
enum IJLERR(__stdcall* PIJLFREE)(struct _JPEG_CORE_PROPERTIES* jcprops);
enum IJLERR(__stdcall* PIJLREAD)(struct _JPEG_CORE_PROPERTIES* jcprops, enum IJLIOTYPE iotype);
enum IJLERR(__stdcall* PIJLWRITE)(struct _JPEG_CORE_PROPERTIES* jcprops, enum IJLIOTYPE iotype);

typedef HIMC(__stdcall* PIMMASSOCIATECONTEXT)(HWND, HIMC);
typedef HIMC(__stdcall* PIMMGETCONTEXT)(HWND);
typedef int32_t(__stdcall* PIMMRELEASECONTEXT)(HWND, HIMC);
typedef int32_t(__stdcall* PIMMGETCOMPOSITIONSTRINGA)(HIMC, uint32_t, PVOID pBuf, uint32_t dwBufLen);
typedef HWND(__stdcall* PIMMGETDEFAULTIMEWND)(HWND);
typedef HWND(__stdcall* PIMMGETIMEFILENAME)(HKL, char*, uint32_t);
typedef PVOID(__stdcall* PIMMLOCKIMCC)(HIMCC);
typedef int32_t(__stdcall* PIMMGETCONVERSIONSTATUS)(HIMC, uint32_t*, uint32_t*);
typedef int32_t(__stdcall* PIMMGETOPENSTATUS)(HIMC);
typedef int32_t(__stdcall* PIMMISIME)(HKL);
typedef int32_t(__stdcall* PIMMNOTIFYIME)(HIMC, uint32_t, uint32_t, uint32_t);
typedef int32_t(__stdcall* PIMMSETCONVERSIONSTATUS)(HIMC, uint32_t, uint32_t);
typedef int32_t(__stdcall* PIMMSETOPENSTATUS)(HIMC, int32_t);
typedef int32_t(__stdcall* PIMMSIMULATEHOTKEY)(HWND, uint32_t);
typedef int32_t(__stdcall* PIMMUNLOCKIMC)(HIMC);
typedef int32_t(__stdcall* PIMMUNLOCKIMCC)(HIMCC);
typedef uint32_t(__stdcall* PIMMGETCANDIDATELISTA)(HIMC, uint32_t deIndex, struct tagCANDIDATELIST* pCandList, uint32_t dwBufLen);
typedef uint32_t(__stdcall* PIMMGETVIRTUALKEY)(HWND);
typedef struct tagINPUTCONTEXT*(__stdcall* PIMMLOCKIMC)(HIMC);

typedef int32_t(__stdcall* PEMPTYWORKINGSET)(HANDLE hProcess);
typedef int32_t(__stdcall* PINITIALIZEPROCESSFORWSWATCH)(HANDLE hProcess);
typedef int32_t(__stdcall* PGETWSCHANGES)(HANDLE hProcess, struct _PSAPI_WS_WATCH_INFORMATION* lpWatchInfo, uint32_t cb);
typedef uint32_t(__stdcall* PGETMAPPEDFILENAMEA)(HANDLE hProcess, PVOID lpv, PSTR lpFilename, uint32_t nSize);

typedef uint32_t(__stdcall* PAMGETERRORTEXTA)(HRESULT hr, PSTR pbuffer, uint32_t MaxLen);

typedef int16_t(__stdcall* PSQLALLOCHANDLE)(int16_t, void*, void**);
typedef int16_t(__stdcall* PSQLBINDCOL)(void*, uint16_t, int16_t, void*, int32_t, int32_t*);
typedef int16_t(__stdcall* PSQLBINDPARAMETER)(void*, uint16_t, int16_t, int16_t, int16_t, uint32_t, int16_t, void*, int32_t, int32_t*);
typedef int16_t(__stdcall* PSQLDISCONNECT)(void*);
typedef int16_t(__stdcall* PSQLDRIVERCONNECT)(void*, HWND, uint8_t*, int16_t, uint8_t*, int16_t, int16_t*, uint16_t);
typedef int16_t(__stdcall* PSQLEXECDIRECT)(void*, uint8_t*, int32_t);
typedef int16_t(__stdcall* PSQLFETCH)(void*);
typedef int16_t(__stdcall* PSQLFREEHANDLE)(int16_t, void*);
typedef int16_t(__stdcall* PSQLFREESTMT)(void*, uint16_t);
typedef int16_t(__stdcall* PSQLGETDIAGFIELD)(int16_t, void*, int16_t, int16_t, void*, int16_t, int16_t*);
typedef int16_t(__stdcall* PSQLGETDIAGREC)(int16_t, void*, int16_t, uint8_t*, int32_t*, uint8_t*, int16_t, int16_t*);
typedef int16_t(__stdcall* PSQLNUMRESULTCOLS)(void*, int16_t*);
typedef int16_t(__stdcall* PSQLSETENVATTR)(void*, int32_t, void*, int32_t);

typedef SOCKET(__stdcall* PACCEPT)(SOCKET s, struct sockaddr* paddr, int32_t* addrlen);
typedef int32_t(__stdcall* PWSACLEANUP)(void);
typedef int32_t(__stdcall* PWSAGETLASTERROR)(void);
typedef int32_t(__stdcall* PWSASTARTUP)(uint16_t, struct WSAData*);
typedef int32_t(__stdcall* PBIND)(uint32_t, const struct sockaddr*, int32_t);
typedef int32_t(__stdcall* PCLOSESOCKET)(uint32_t);
typedef int32_t(__stdcall* PGETHOSTNAME)(char*, int32_t);
typedef int32_t(__stdcall* PLISTEN)(uint32_t, int32_t);
typedef int32_t(__stdcall* PRECV)(uint32_t, char*, int32_t, int32_t);
typedef int32_t(__stdcall* PRECVFROM)(uint32_t, char*, int32_t, int32_t, struct sockaddr*, int32_t*);
typedef int32_t(__stdcall* PSEND)(uint32_t, char*, int32_t, int32_t);
typedef int32_t(__stdcall* PSENDTO)(uint32_t, const char*, int32_t, int32_t, const struct sockaddr*, int32_t);
typedef struct hostent*(__stdcall* PGETHOSTBYNAME)(const char* name);
typedef struct servent*(__stdcall* PGETSERVBYNAME)(char*, char*);
typedef uint16_t(__stdcall* PHTONS)(uint16_t);
typedef uint32_t(__stdcall* PHTONL)(uint32_t);
typedef uint32_t(__stdcall* PINET_ADDR)(const char*);
typedef uint32_t(__stdcall* PNTOHL)(uint32_t);
typedef uint32_t(__stdcall* PSOCKET)(int32_t, int32_t, int32_t);

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

// data
// =============================================================================
extern PDIRECTDRAWCREATEEX _DirectDrawCreateEx;
extern PDIRECTDRAWENUMERATEA _DirectDrawEnumerate;
extern PDIRECTDRAWENUMERATEEXA _DirectDrawEnumerateEx;
extern PDIRECTSOUNDCREATE _DirectSoundCreate;
extern PDIRECTSOUNDENUMERATEA _DirectSoundEnumerate;
extern PDIRECTINPUTCREATEEX _DirectInputCreateEx;
extern GETDXFUNC g_pGetDXVB;

extern PIJLINIT _ijlInit;
extern PIJLFREE _ijlFree;
extern PIJLREAD _ijlRead;
extern PIJLWRITE _ijlWrite;

extern PIMMGETCONTEXT _ImmGetContext;
extern PIMMRELEASECONTEXT _ImmReleaseContext;
extern PIMMASSOCIATECONTEXT _ImmAssociateContext;
extern PIMMGETCOMPOSITIONSTRINGA _ImmGetCompositionString;
extern PIMMGETOPENSTATUS _ImmGetOpenStatus;
extern PIMMSETOPENSTATUS _ImmSetOpenStatus;
extern PIMMGETCONVERSIONSTATUS _ImmGetConversionStatus;
extern PIMMGETCANDIDATELISTA _ImmGetCandidateList;
extern PIMMGETVIRTUALKEY _ImmGetVirtualKey;
extern PIMMLOCKIMC _ImmLockIMC;
extern PIMMUNLOCKIMC _ImmUnlockIMC;
extern PIMMLOCKIMCC _ImmLockIMCC;
extern PIMMUNLOCKIMCC _ImmUnlockIMCC;
extern PIMMGETDEFAULTIMEWND _ImmGetDefaultIMEWnd;
extern PIMMGETIMEFILENAME _ImmGetIMEFileName;
extern PIMMNOTIFYIME _ImmNotifyIME;
extern PIMMSETCONVERSIONSTATUS _ImmSetConversionStatus;
extern PIMMSIMULATEHOTKEY _ImmSimulateHotKey;
extern PIMMISIME _ImmIsIME;

extern PEMPTYWORKINGSET _EmptyWorkingSet;
extern PINITIALIZEPROCESSFORWSWATCH _InitializeProcessForWsWatch;
extern PGETWSCHANGES _GetWsChanges;
extern PGETMAPPEDFILENAMEA _GetMappedFileName;
extern PAMGETERRORTEXTA _AMGetErrorText;

extern PSQLALLOCHANDLE _SQLAllocHandle;
extern PSQLBINDCOL _SQLBindCol;
extern PSQLBINDPARAMETER _SQLBindParameter;
extern PSQLDISCONNECT _SQLDisconnect;
extern PSQLDRIVERCONNECT _SQLDriverConnect;
extern PSQLEXECDIRECT _SQLExecDirect;
extern PSQLFETCH _SQLFetch;
extern PSQLFREEHANDLE _SQLFreeHandle;
extern PSQLFREESTMT _SQLFreeStmt;
extern PSQLGETDIAGFIELD _SQLGetDiagField;
extern PSQLGETDIAGREC _SQLGetDiagRec;
extern PSQLNUMRESULTCOLS _SQLNumResultCols;
extern PSQLSETENVATTR _SQLSetEnvAttr;

extern PACCEPT _accept;
extern PBIND _bind;
extern PCLOSESOCKET _closesocket;
extern PGETHOSTBYNAME _gethostbyname;
extern PGETHOSTNAME _gethostname;
extern PGETSERVBYNAME _getservbyname;
extern PHTONL _htonl;
extern PHTONS _htons;
extern PINET_ADDR _inet_addr;
extern PLISTEN _listen;
extern PNTOHL _ntohl;
extern PRECV _recv;
extern PRECVFROM _recvfrom;
extern PSEND _send;
extern PSENDTO _sendto;
extern PSOCKET _socket;
extern PWSACLEANUP _WSACleanup;
extern PWSAGETLASTERROR _WSAGetLastError;
extern PWSASTARTUP _WSAStartup;

HMODULE LibAmstream;
HMODULE LibQuartz;
HMODULE LibMSADP32;
HMODULE LibBlade;
HMODULE LibDinput;
HMODULE LibDsound;
HMODULE LibIME;
HMODULE LibJpeg;
HMODULE LibPSAPI;
HMODULE LibODBC;
HMODULE LibWinSock;

char BladePath[MAX_PATH];
uint32_t DXdwRevision;
uint32_t DXdwVersion;

// externals
// =============================================================================
extern uint32_t gDirectX7;
extern uint32_t gEnableGosView;
extern bool OnNetwork;
PSTR __stdcall GetDLLInfo(char*, int32_t);

// implemented functions
// =============================================================================
void __stdcall InitLibraries(void);
void __stdcall DestroyLibraries(void);
PSTR __stdcall GetDirectXVersion(void);

#pragma region InitLibraries
/*
 */
#pragma endregion InitLibraries
/// <summary>
/// <c>InitLibraries</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <returns></returns>
void __stdcall InitLibraries(void)
{
	PSTR format;
	PSTR message;
	size_t buffersize;
	char szAssetsPathMessage[MAX_PATH * 2];
	bool bbladepath1;
	bool bbladepath2;
	// char szDdrawMessage[MAX_PATH * 2];
	char szDirectXVersionMessage2[128];
	char szDirectXVersionMessage1[128];
	char szDsetupMessage[MAX_PATH * 2];
	HMODULE hModule_dsetup;
	char szAssetsPath[MAX_PATH];
	PDIRECTXSETUPGETVERSION pDirectXSetupGetVersion;
#if CONSIDERED_OBSOLETE
	OSVERSIONINFOA VersionInformation;
	char szDirBuffer[MAX_PATH];
	char szInstallMessage[256];
#endif
	gos_MathExceptions(false, false);
	// C6031: Return value ignored: 'CoInitialize'
	CoInitialize(nullptr);
	// dsetup.dll
	strcpy_s(szAssetsPath, _countof(szAssetsPath), AssetsDirectory2);
	strcat_s(szAssetsPath, _countof(szAssetsPath), "\\dsetup.dll");
	hModule_dsetup = LoadLibraryA(szAssetsPath);
	if (hModule_dsetup == nullptr)
	{
		strcpy_s(szAssetsPath, _countof(szAssetsPath), AssetsDirectory1);
		strcat_s(szAssetsPath, _countof(szAssetsPath), "\\assets\\binaries\\dsetup.dll");
		hModule_dsetup = LoadLibraryA(szAssetsPath);
		if (hModule_dsetup == nullptr)
		{
			// pszFormat = gos_GetResourceString(gLanguageDLL, 10051u);
			format = "Cannot find dsetup.dll in %s\\assets\\binaries or %s, "
						"this file is required";
			sprintf_s(szDsetupMessage, _countof(szDsetupMessage), format, AssetsDirectory1,
				AssetsDirectory2);
			MessageBoxA(nullptr, szDsetupMessage, ApplicationName, MB_ICONEXCLAMATION);
			status = AfterExit;
			_exit(EXIT_FAILURE);
		}
	}
	pDirectXSetupGetVersion = reinterpret_cast<PDIRECTXSETUPGETVERSION>(
		GetProcAddress(hModule_dsetup, "DirectXSetupGetVersion"));
	if (pDirectXSetupGetVersion == nullptr || pDirectXSetupGetVersion(&DXdwVersion, &DXdwRevision) == 0)
	{
		DXdwVersion = 0;
		DXdwRevision = 0;
	}
	FreeLibrary(hModule_dsetup);
	// ddraw.dll
	if (gDirectX7)
	{
		if ((WindowsNT == false) && ((DXdwVersion < 7) || ((DXdwVersion == 7) && (DXdwRevision < 716))))
		{
			format = "Requires DirectX 7.0a or later, found %s";
			message = GetDirectXVersion();
			sprintf_s(szDirectXVersionMessage1, _countof(szDirectXVersionMessage1), format,
				message);
			MessageBoxA(nullptr, szDirectXVersionMessage1, ApplicationName, MB_ICONEXCLAMATION);
			status = AfterExit;
			_exit(EXIT_FAILURE);
		}
	}
	else
	{
		if ((DXdwVersion < 8) || ((DXdwVersion == 8) && (DXdwRevision < 400)))
		{
			// pszFormat = gos_GetResourceString(gLanguageDLL, 10030u);	//
			// language.dll : "Requires DirectX 8 or later, found %s"
			format = "Requires DirectX 8 or later, found %s";
			message = GetDirectXVersion();
			sprintf_s(szDirectXVersionMessage2, _countof(szDirectXVersionMessage2), format,
				message);
			MessageBoxA(nullptr, szDirectXVersionMessage2, ApplicationName, MB_ICONEXCLAMATION);
			status = AfterExit;
			_exit(EXIT_FAILURE);
		}
	}
	if (gNoBlade == true)
	{
		LibBlade = LoadLibraryA("ddraw.dll");
		if (LibBlade == nullptr)
		{
			// message = gos_GetResourceString(gLanguageDLL, 10031u);
			message = "Cannot find ddraw.dll, this file is required";
			// sprintf_s(szDdrawMessage, _countof(szDdrawMessage), message);
			MessageBoxA(nullptr, message, ApplicationName, MB_ICONEXCLAMATION);
			status = AfterExit;
			_exit(EXIT_FAILURE);
		}
		_DirectDrawCreateEx =
			reinterpret_cast<PDIRECTDRAWCREATEEX>(GetProcAddress(LibBlade, "DirectDrawCreateEx"));
		_DirectDrawEnumerate = reinterpret_cast<PDIRECTDRAWENUMERATEA>(
			GetProcAddress(LibBlade, "DirectDrawEnumerateA"));
		_DirectDrawEnumerateEx = reinterpret_cast<PDIRECTDRAWENUMERATEEXA>(
			GetProcAddress(LibBlade, "DirectDrawEnumerateExA"));
		g_pGetDXVB = nullptr;
		if ((_DirectDrawCreateEx == nullptr) || (_DirectDrawEnumerate == nullptr) ||
			/* typo bug (_DirectDrawEnumerate == nullptr)*/
			(_DirectDrawEnumerateEx == nullptr))
		{
			message = "Cannot find ddraw functions";
			MessageBoxA(nullptr, message, ApplicationName, MB_ICONEXCLAMATION);
			status = AfterExit;
			_exit(EXIT_FAILURE);
		}
	}
	else
	{
		// this special dll must surely be considered obsolete whatever the
		// advantages there were to use a custom ddraw library.
		bbladepath2 = false;
		bbladepath1 = true;
		strcpy_s(BladePath, _countof(BladePath), AssetsDirectory2);
		strcat_s(BladePath, _countof(BladePath), "\\bladed.dll");
		LibBlade = LoadLibraryA(BladePath);
		if (LibBlade == nullptr)
		{
			strcpy_s(BladePath, _countof(BladePath), AssetsDirectory1);
			strcat_s(BladePath, _countof(BladePath), "\\assets\\binaries\\bladed.dll");
			LibBlade = LoadLibraryA(BladePath);
			if (LibBlade == nullptr)
			{
				if (strlen(AssetsDirectory2) >= 16)
				{
					buffersize = strlen(AssetsDirectory2);
					if (!_strnicmp(
							&AssetsDirectory2[buffersize - 16], "\\assets\\binaries", 0x10u))
						bbladepath2 = true;
				}
				bbladepath1 = false;
				strcpy_s(BladePath, _countof(BladePath), AssetsDirectory2);
				strcat_s(BladePath, _countof(BladePath), "\\blade.dll");
				LibBlade = LoadLibraryA(BladePath);
				if (LibBlade == nullptr)
				{
					strcpy_s(BladePath, _countof(BladePath), AssetsDirectory1);
					strcat_s(BladePath, _countof(BladePath), "\\assets\\binaries\\blade.dll");
					LibBlade = LoadLibraryA(BladePath);
					bbladepath2 = true;
					if (LibBlade == nullptr)
					{
						// pszFormat = gos_GetResourceString(gLanguageDLL,
						// 10033u);
						format = "Cannot find blade.dll in "
									"%s\\assets\\binaries or %s, this file is "
									"required";
						sprintf_s(szAssetsPathMessage, _countof(szAssetsPathMessage), format,
							AssetsDirectory1, AssetsDirectory2);
						MessageBoxA(
							nullptr, szAssetsPathMessage, ApplicationName, MB_ICONEXCLAMATION);
						status = AfterExit;
						_exit(EXIT_FAILURE);
					}
				}
			}
		}
		if ((bbladepath2 == false) && _stricmp(ApplicationName, "Test Blade"))
		{
			if (bbladepath1 == true)
			{
				if (InternalFunctionPause("Warning, '%s' was used", BladePath))
					ENTER_DEBUGGER;
			}
			else
			{
				if (gos_DoesFileExist("assets\\binaries") && InternalFunctionPause("Warning, '%s' was used", BladePath))
					ENTER_DEBUGGER;
			}
		}
		_DirectDrawCreateEx =
			reinterpret_cast<PDIRECTDRAWCREATEEX>(GetProcAddress(LibBlade, "DirectDrawCreateEx"));
		_DirectDrawEnumerate = reinterpret_cast<PDIRECTDRAWENUMERATEA>(
			GetProcAddress(LibBlade, "DirectDrawEnumerateA"));
		_DirectDrawEnumerateEx = reinterpret_cast<PDIRECTDRAWENUMERATEEXA>(
			GetProcAddress(LibBlade, "DirectDrawEnumerateExA"));
		g_pGetDXVB = reinterpret_cast<GETDXFUNC>(GetProcAddress(LibBlade, "GetDXVB"));
		if ((_DirectDrawCreateEx == nullptr) || (_DirectDrawEnumerate == nullptr) ||
			/* typo bug (_DirectDrawEnumerate == nullptr)*/
			(_DirectDrawEnumerateEx == nullptr) || (g_pGetDXVB == nullptr))
		{
			message = "Cannot find Blade functions";
			MessageBoxA(nullptr, message, ApplicationName, MB_ICONEXCLAMATION);
			status = AfterExit;
			_exit(EXIT_FAILURE);
		}
	}
	// ijl10.dll
	strcpy_s(szAssetsPath, _countof(szAssetsPath), AssetsDirectory2);
	strcat_s(szAssetsPath, _countof(szAssetsPath), "\\ijl10.dll");
	LibJpeg = LoadLibraryA(szAssetsPath);
	if (LibJpeg == nullptr)
	{
		strcpy_s(szAssetsPath, _countof(szAssetsPath), AssetsDirectory1);
		strcat_s(szAssetsPath, _countof(szAssetsPath), "\\assets\\binaries\\ijl10.dll");
		LibJpeg = LoadLibraryA(szAssetsPath);
	}
	if (LibJpeg)
	{
		_ijlInit = reinterpret_cast<PIJLINIT>(GetProcAddress(LibJpeg, "ijlInit"));
		_ijlFree = reinterpret_cast<PIJLFREE>(GetProcAddress(LibJpeg, "ijlFree"));
		_ijlRead = reinterpret_cast<PIJLREAD>(GetProcAddress(LibJpeg, "ijlRead"));
		_ijlWrite = reinterpret_cast<PIJLWRITE>(GetProcAddress(LibJpeg, "ijlWrite"));
	}
	// dsound.dll
	LibDsound = LoadLibraryA("dsound.dll");
	if (LibDsound == nullptr)
	{
		// message = gos_GetResourceString(gLanguageDLL, 10036u);
		message = "Cannot load DirectSound dll";
		MessageBoxA(nullptr, message, ApplicationName, MB_ICONEXCLAMATION);
		status = AfterExit;
		_exit(EXIT_FAILURE);
	}
	_DirectSoundCreate =
		reinterpret_cast<PDIRECTSOUNDCREATE>(GetProcAddress(LibDsound, "DirectSoundCreate"));
	_DirectSoundEnumerate = reinterpret_cast<PDIRECTSOUNDENUMERATEA>(
		GetProcAddress(LibDsound, "DirectSoundEnumerateA"));
	if ((_DirectSoundCreate == nullptr) && (_DirectSoundEnumerate == nullptr))
	{
		message = "Cannot find DirectSound functions";
		MessageBoxA(nullptr, message, ApplicationName, MB_ICONEXCLAMATION);
		status = AfterExit;
		_exit(EXIT_FAILURE);
	}
	// dinput.dll
	LibDinput = LoadLibraryA("dinput.dll");
	if (LibDinput == nullptr)
	{
		// message = gos_GetResourceString(gLanguageDLL, 10038u);
		message = "Cannot load DirectInput dll";
		MessageBoxA(nullptr, message, ApplicationName, MB_ICONEXCLAMATION);
		status = AfterExit;
		_exit(EXIT_FAILURE);
	}
	_DirectInputCreateEx =
		reinterpret_cast<PDIRECTINPUTCREATEEX>(GetProcAddress(LibDinput, "DirectInputCreateEx"));
	if (_DirectInputCreateEx == nullptr)
	{
		message = "Cannot find DirectInput functions";
		MessageBoxA(nullptr, message, ApplicationName, MB_ICONEXCLAMATION);
		status = AfterExit;
		_exit(EXIT_FAILURE);
	}
	// imm32.dll
	LibIME = LoadLibraryA("imm32.dll");
	if (LibIME)
	{
		_ImmGetContext = reinterpret_cast<PIMMGETCONTEXT>(GetProcAddress(LibIME, "ImmGetContext"));
		_ImmReleaseContext =
			reinterpret_cast<PIMMRELEASECONTEXT>(GetProcAddress(LibIME, "ImmReleaseContext"));
		_ImmAssociateContext =
			reinterpret_cast<PIMMASSOCIATECONTEXT>(GetProcAddress(LibIME, "ImmAssociateContext"));
		_ImmGetCompositionString = reinterpret_cast<PIMMGETCOMPOSITIONSTRINGA>(
			GetProcAddress(LibIME, "ImmGetCompositionStringA"));
		_ImmGetOpenStatus =
			reinterpret_cast<PIMMGETOPENSTATUS>(GetProcAddress(LibIME, "ImmGetOpenStatus"));
		_ImmSetOpenStatus =
			reinterpret_cast<PIMMSETOPENSTATUS>(GetProcAddress(LibIME, "ImmSetOpenStatus"));
		_ImmGetConversionStatus = reinterpret_cast<PIMMGETCONVERSIONSTATUS>(
			GetProcAddress(LibIME, "ImmGetConversionStatus"));
		_ImmGetCandidateList =
			reinterpret_cast<PIMMGETCANDIDATELISTA>(GetProcAddress(LibIME, "ImmGetCandidateListA"));
		_ImmGetVirtualKey =
			reinterpret_cast<PIMMGETVIRTUALKEY>(GetProcAddress(LibIME, "ImmGetVirtualKey"));
		_ImmLockIMC = reinterpret_cast<PIMMLOCKIMC>(GetProcAddress(LibIME, "ImmLockIMC"));
		_ImmUnlockIMC = reinterpret_cast<PIMMUNLOCKIMC>(GetProcAddress(LibIME, "ImmUnlockIMCC"));
		_ImmLockIMCC = reinterpret_cast<PIMMLOCKIMCC>(GetProcAddress(LibIME, "ImmLockIMCC"));
		_ImmUnlockIMCC = reinterpret_cast<PIMMUNLOCKIMCC>(GetProcAddress(LibIME, "ImmUnlockIMC"));
		_ImmGetDefaultIMEWnd =
			reinterpret_cast<PIMMGETDEFAULTIMEWND>(GetProcAddress(LibIME, "ImmGetDefaultIMEWnd"));
		_ImmGetIMEFileName =
			reinterpret_cast<PIMMGETIMEFILENAME>(GetProcAddress(LibIME, "ImmGetIMEFileNameA"));
		_ImmNotifyIME = reinterpret_cast<PIMMNOTIFYIME>(GetProcAddress(LibIME, "ImmNotifyIME"));
		_ImmSetConversionStatus = reinterpret_cast<PIMMSETCONVERSIONSTATUS>(
			GetProcAddress(LibIME, "ImmSetConversionStatus"));
		_ImmSimulateHotKey =
			reinterpret_cast<PIMMSIMULATEHOTKEY>(GetProcAddress(LibIME, "ImmSimulateHotKey"));
		_ImmIsIME = reinterpret_cast<PIMMISIME>(GetProcAddress(LibIME, "ImmIsIME"));
	}
	// psapi.dll
	if (WindowsNT)
		LibPSAPI = LoadLibraryA("psapi.dll");
	else
		LibPSAPI = nullptr;
	if (LibPSAPI)
	{
		_EmptyWorkingSet =
			reinterpret_cast<PEMPTYWORKINGSET>(GetProcAddress(LibPSAPI, "EmptyWorkingSet"));
		_InitializeProcessForWsWatch = reinterpret_cast<PINITIALIZEPROCESSFORWSWATCH>(
			GetProcAddress(LibPSAPI, "InitializeProcessForWsWatch"));
		_GetWsChanges = reinterpret_cast<PGETWSCHANGES>(GetProcAddress(LibPSAPI, "GetWsChanges"));
		_GetMappedFileName =
			reinterpret_cast<PGETMAPPEDFILENAMEA>(GetProcAddress(LibPSAPI, "GetMappedFileNameA"));
	}
	// ws2_32.dll
	if (Environment.NetworkGame || gEnableGosView)
	{
		LibWinSock = LoadLibraryA("ws2_32.dll");
		if (LibWinSock == nullptr)
			LibWinSock = LoadLibraryA("wsock32.dll");
		if (LibWinSock)
		{
			_accept = reinterpret_cast<PACCEPT>(GetProcAddress(LibWinSock, "accept"));
			_bind = reinterpret_cast<PBIND>(GetProcAddress(LibWinSock, "bind"));
			_closesocket =
				reinterpret_cast<PCLOSESOCKET>(GetProcAddress(LibWinSock, "closesocket"));
			_gethostbyname =
				reinterpret_cast<PGETHOSTBYNAME>(GetProcAddress(LibWinSock, "gethostbyname"));
			_gethostname =
				reinterpret_cast<PGETHOSTNAME>(GetProcAddress(LibWinSock, "gethostname"));
			_getservbyname =
				reinterpret_cast<PGETSERVBYNAME>(GetProcAddress(LibWinSock, "getservbyname"));
			_htonl = reinterpret_cast<PHTONL>(GetProcAddress(LibWinSock, "htonl"));
			_htons = reinterpret_cast<PHTONS>(GetProcAddress(LibWinSock, "htons"));
			_inet_addr = reinterpret_cast<PINET_ADDR>(GetProcAddress(LibWinSock, "inet_addr"));
			_listen = reinterpret_cast<PLISTEN>(GetProcAddress(LibWinSock, "listen"));
			_ntohl = reinterpret_cast<PNTOHL>(GetProcAddress(LibWinSock, "ntohl"));
			_recv = reinterpret_cast<PRECV>(GetProcAddress(LibWinSock, "recv"));
			_recvfrom = reinterpret_cast<PRECVFROM>(GetProcAddress(LibWinSock, "recvfrom"));
			_send = reinterpret_cast<PSEND>(GetProcAddress(LibWinSock, "send"));
			_sendto = reinterpret_cast<PSENDTO>(GetProcAddress(LibWinSock, "sendto"));
			_socket = reinterpret_cast<PSOCKET>(GetProcAddress(LibWinSock, "socket"));
			_WSACleanup = reinterpret_cast<PWSACLEANUP>(GetProcAddress(LibWinSock, "WSACleanup"));
			_WSAGetLastError =
				reinterpret_cast<PWSAGETLASTERROR>(GetProcAddress(LibWinSock, "WSAGetLastError"));
			_WSAStartup = reinterpret_cast<PWSASTARTUP>(GetProcAddress(LibWinSock, "WSAStartup"));
		}
	}
	// amstream.dll quartz.dll msadp32.acm
	LibAmstream = LoadLibraryA("amstream.dll");
	LibQuartz = LoadLibraryA("quartz.dll");
	LibMSADP32 = LoadLibraryA("msadp32.acm");
	if ((LibAmstream == nullptr) || (LibQuartz == nullptr))
	{
		// message = gos_GetResourceString(gLanguageDLL, 10040u);
		message = "DirectX Media not installed";
		MessageBoxA(nullptr, message, ApplicationName, MB_ICONEXCLAMATION);
		status = AfterExit;
		_exit(EXIT_FAILURE);
	}
	if (LibMSADP32 == nullptr)
	{
		// message = gos_GetResourceString(gLanguageDLL, 10042Au);
		message = "Microsoft audio compression not installed";
		MessageBoxA(nullptr, message, ApplicationName, MB_ICONEXCLAMATION);
		status = AfterExit;
		_exit(EXIT_FAILURE);
	}
	// NOTE: useless code => meant to be moved up?
	if (LibAmstream == nullptr)
	{
		buffersize = strlen("amstream.dll  - Version 6.01");
		message = GetDLLInfo("amstream.dll", 0);
		if (strncmp(message, "amstream.dll  - Version 6.01", buffersize) < 0)
		{
			// message = gos_GetResourceString(gLanguageDLL, 10043u);
			message = "Please install DirectX Media 6.0 or later";
			MessageBoxA(nullptr, message, ApplicationName, MB_ICONEXCLAMATION);
			status = AfterExit;
			_exit(EXIT_FAILURE);
		}
	}
	_AMGetErrorText =
		reinterpret_cast<PAMGETERRORTEXTA>(GetProcAddress(LibQuartz, "AMGetErrorTextA"));
	if (_AMGetErrorText == nullptr)
	{
		message = "Cannot find DirectShow functions";
		MessageBoxA(nullptr, message, ApplicationName, MB_ICONEXCLAMATION);
		status = AfterExit;
		_exit(EXIT_FAILURE);
	}
#if CONSIDERED_OBSOLETE
	// Windows 9X vmcpd.vxd
	VersionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	GetVersionExA(&VersionInformation);
	if (WindowsNT == false)
	{
		if (HasKatmai)
		{
			if (LOWORD(VersionInformation.dwBuildNumber) == 1998)
			{
				GetSystemDirectoryA(szDirBuffer, MAX_PATH);
				strcat(szDirBuffer, "\\vmm32\\vmcpd.vxd");
				if (gos_DoesFileExist(szDirBuffer) == false)
				{
					strcpy_s(szInstallMessage, _countof(szInstallMessage),
						"You need to install \"vmcpd.vxd\" This can be found "
						"in DirectX 7.0 SDK \"DXF\\extras\\win98 piii dbg\" "
						"folder.");
					if (OnNetwork == true)
						strcat_s(szInstallMessage, _countof(szInstallMessage),
							"\n\n(Microsoft Internal) - This can be found at "
							"\\\\dxrelease\\release\\dx7\\DXF\\extras\\win98 "
							"piii dbg");
					MessageBoxA(nullptr, szInstallMessage, ApplicationName, MB_ICONEXCLAMATION);
				}
			}
		}
	}
#endif
	// odbc32.dll
	LibODBC = LoadLibraryA("odbc32.dll");
	if (LibODBC)
	{
		_SQLAllocHandle =
			reinterpret_cast<PSQLALLOCHANDLE>(GetProcAddress(LibODBC, "SQLAllocHandle"));
		_SQLBindCol = reinterpret_cast<PSQLBINDCOL>(GetProcAddress(LibODBC, "SQLBindCol"));
		_SQLBindParameter =
			reinterpret_cast<PSQLBINDPARAMETER>(GetProcAddress(LibODBC, "SQLBindParameter"));
		_SQLDisconnect = reinterpret_cast<PSQLDISCONNECT>(GetProcAddress(LibODBC, "SQLDisconnect"));
		_SQLDriverConnect =
			reinterpret_cast<PSQLDRIVERCONNECT>(GetProcAddress(LibODBC, "SQLDriverConnect"));
		_SQLExecDirect = reinterpret_cast<PSQLEXECDIRECT>(GetProcAddress(LibODBC, "SQLExecDirect"));
		_SQLFetch = reinterpret_cast<PSQLFETCH>(GetProcAddress(LibODBC, "SQLFetch"));
		_SQLFreeHandle = reinterpret_cast<PSQLFREEHANDLE>(GetProcAddress(LibODBC, "SQLFreeHandle"));
		_SQLFreeStmt = reinterpret_cast<PSQLFREESTMT>(GetProcAddress(LibODBC, "SQLFreeStmt"));
		_SQLGetDiagField =
			reinterpret_cast<PSQLGETDIAGFIELD>(GetProcAddress(LibODBC, "SQLGetDiagField"));
		_SQLGetDiagRec = reinterpret_cast<PSQLGETDIAGREC>(GetProcAddress(LibODBC, "SQLGetDiagRec"));
		_SQLNumResultCols =
			reinterpret_cast<PSQLNUMRESULTCOLS>(GetProcAddress(LibODBC, "SQLNumResultCols"));
		_SQLSetEnvAttr = reinterpret_cast<PSQLSETENVATTR>(GetProcAddress(LibODBC, "SQLSetEnvAttr"));
	}
	gos_MathExceptions(true, false);
}

/// <summary>
/// <c>DestroyLibraries</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <returns></returns>
void __stdcall DestroyLibraries()
{
	if (LibJpeg)
	{
		FreeLibrary(LibJpeg);
		LibJpeg = nullptr;
	}
	if (LibMSADP32)
	{
		FreeLibrary(LibMSADP32);
		LibMSADP32 = nullptr;
	}
	if (LibQuartz)
	{
		FreeLibrary(LibQuartz);
		LibQuartz = nullptr;
	}
	if (LibAmstream)
	{
		FreeLibrary(LibAmstream);
		LibAmstream = nullptr;
	}
	if (LibBlade)
	{
		FreeLibrary(LibBlade);
		LibBlade = nullptr;
	}
	if (LibDinput)
	{
		FreeLibrary(LibDinput);
		LibDinput = nullptr;
	}
	if (LibDsound)
	{
		FreeLibrary(LibDsound);
		LibDsound = nullptr;
	}
	if (LibIME)
	{
		FreeLibrary(LibIME);
		LibIME = nullptr;
	}
	if (LibPSAPI)
	{
		FreeLibrary(LibPSAPI);
		LibPSAPI = nullptr;
	}
	if (LibWinSock)
	{
		FreeLibrary(LibWinSock);
		LibWinSock = nullptr;
	}
	if (LibODBC)
	{
		FreeLibrary(LibODBC);
		LibODBC = nullptr;
	}
	CoUninitialize();
}

/// <summary>
/// <c>GetDirectXVersion</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <returns></returns>
PSTR __stdcall GetDirectXVersion(void)
{
	static char Buffer[64];
	char Buffer2[256];
	sprintf_s(Buffer, _countof(Buffer),
		"%u.%02u.%02u.%04u", // "%d.%02d.%02d.%04d",
		(uint16_t)DXdwVersion, HIWORD(DXdwVersion), HIWORD(DXdwRevision), (uint16_t)DXdwRevision);
	if (DXdwVersion == 0x40005 && DXdwRevision == 155)
		strcat_s(Buffer, _countof(Buffer), " (DirectX 5.0)");
	if (DXdwVersion == 0x40005 && DXdwRevision == 0x10640)
		strcat_s(Buffer, _countof(Buffer), " (DirectX 5.2)");
	if (DXdwVersion == 0x40006 && DXdwRevision == 318)
		strcat_s(Buffer, _countof(Buffer), " (DirectX 6.0)");
	if (DXdwVersion == 0x40006 && DXdwRevision == 0x201B4)
		strcat_s(Buffer, _countof(Buffer), " (DirectX 6.1)");
	if (DXdwVersion == 0x40006 && DXdwRevision == 0x30206)
		strcat_s(Buffer, _countof(Buffer), " (DirectX 6.2)");
	if (DXdwVersion == 0x40007 && DXdwRevision == 166)
		strcat_s(Buffer, _countof(Buffer), " (DirectX 7 Beta 1)");
	if (DXdwVersion == 0x40007 && DXdwRevision == 201)
		strcat_s(Buffer, _countof(Buffer), " (DirectX 7 Beta 2)");
	if (DXdwVersion == 0x40007 && DXdwRevision == 224)
		strcat_s(Buffer, _countof(Buffer), " (DirectX 7 Build 224)");
	if (DXdwVersion == 0x40007 && DXdwRevision == 242)
		strcat_s(Buffer, _countof(Buffer), " (DirectX 7 RC3)");
	if (DXdwVersion == 0x40007 && DXdwRevision == 700)
		strcat_s(Buffer, _countof(Buffer), " (DirectX 7.0)");
	if (DXdwVersion == 0x40007 && DXdwRevision == 716)
		strcat_s(Buffer, _countof(Buffer), " (DirectX 7.0a)");
	if (DXdwVersion == 0x40007 && DXdwRevision == 7096)
		strcat_s(Buffer, _countof(Buffer), " (DirectX 7.1)");
	if (DXdwVersion == 0x40008 && DXdwRevision == 183)
		strcat_s(Buffer, _countof(Buffer), " (DirectX 8 RC0)");
	if (DXdwVersion == 0x40008 && DXdwRevision == 219)
		strcat_s(Buffer, _countof(Buffer), " (DirectX 8 RC1)");
	if (DXdwVersion == 0x40008)
	{
		if (DXdwRevision == 400)
		{
			strcat_s(Buffer, _countof(Buffer), " (DirectX 8.0)");
			GetSystemDirectoryA(Buffer2, 0x100u);
			strcat_s(Buffer2, _countof(Buffer2), "\\ddraw.dll");
			if (gos_FileSize(Buffer2) == 0x71800 || gos_FileSize(Buffer2) == 0xEF200)
				strcat_s(Buffer, _countof(Buffer), " DEBUG DLL's");
		}
	}
	return Buffer;
}
