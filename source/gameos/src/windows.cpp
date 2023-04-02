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
 windows.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

#include "stdinc.h"

#include "gameos.hpp"
#include "globals.hpp"
#include "winproc.hpp"
#include "platform.hpp"
#include "toolos.hpp"
#include "errorhandler.hpp"
#include "windows.hpp"

#pragma warning(disable : 4191) // 'type cast' : unsafe conversion from 'FARPROC'

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

// -----------------------------------------------------------------------------
// Global data exported from this module

MECH_IMPEXP
EXECUTION_STATE(__stdcall* _SetThreadExecutionState)(EXECUTION_STATE);
MECH_IMPEXP
int32_t(__stdcall* _GetFileAttributesEx)(PSTR, GET_FILEEX_INFO_LEVELS, PVOID);
MECH_IMPEXP
int32_t(__stdcall* _GetDiskFreeSpaceEx)(
	PSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

MECH_IMPEXP HANDLE hMappedFile;
MECH_IMPEXP MSG msg;
MECH_IMPEXP WNDCLASSA wndClass;
MECH_IMPEXP HWND hwnd;
MECH_IMPEXP HINSTANCE hinstance;
MECH_IMPEXP uint32_t DesktopBpp;
MECH_IMPEXP uint32_t DesktopRes;
MECH_IMPEXP POINT clientToScreen;
MECH_IMPEXP int32_t widthX;
MECH_IMPEXP int32_t heightY;
MECH_IMPEXP int32_t WindowStartX;
MECH_IMPEXP int32_t WindowStartY;
MECH_IMPEXP PSTR IconName;
MECH_IMPEXP enum Status status;
MECH_IMPEXP HDC DesktopDC;
MECH_IMPEXP uint32_t ForceReStart;
MECH_IMPEXP BOOL bScreenSaver;
MECH_IMPEXP BOOLEAN StoredScreenSaverSetting;
MECH_IMPEXP PSTR commandline;

// -----------------------------------------------------------------------------
// implemented functions

MECH_IMPEXP BOOLEAN __stdcall AlreadyRunning(void);
MECH_IMPEXP void __stdcall InitializeWindows(void);
MECH_IMPEXP void __stdcall DestroyWindows(void);
MECH_IMPEXP void __stdcall Update(void);
MECH_IMPEXP size_t __stdcall gos_GetClipboardText(PSTR Buffer, size_t BufferSize);
MECH_IMPEXP void __stdcall gos_SetClipboardText(PSTR Text);

static BOOL __stdcall EnumIcons(HMODULE hModule, PSTR pszType, PSTR pszName, LONG_PTR lparam);

// -----------------------------------------------------------------------------
// externals not specified in headers
extern BOOLEAN RunFullScreen;
extern BOOLEAN gDumpMachineInfo;
extern BOOLEAN gNoDialogs;
extern uint32_t gLanguageDLL;
extern int32_t LoseFocusBehavior; // enum?
extern BOOLEAN WindowsPause;

void __stdcall InitializeIME(HWND hwnd);
BOOLEAN __stdcall IgnoreImeHotKey(PMSG pmsg);

// -----------------------------------------------------------------------------

#pragma region AlreadyRunning
/*
 */
#pragma endregion AlreadyRunning
/// <summary>
/// <c>AlreadyRunning</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name=""></param>
/// <returns></returns>
MECH_IMPEXP BOOLEAN __stdcall AlreadyRunning(void)
{
	WINDOWPLACEMENT wndpl;
	HWND hwnd;
	hwnd = ::FindWindowA(Environment.applicationName, Environment.applicationName);
	if (hwnd)
	{
		wndpl.length = sizeof(wndpl);
		::GetWindowPlacement(hwnd, &wndpl);
		::SetForegroundWindow(hwnd);
		if (wndpl.showCmd == SW_SHOWMINIMIZED)
			::ShowWindow(hwnd, SW_RESTORE);
		return true;
	}
	hMappedFile = ::CreateFileMappingA(
		INVALID_HANDLE_VALUE, nullptr, SEC_RESERVE | PAGE_READONLY, 0, 4u, ApplicationName);
	if (hMappedFile && (GetLastError() == ERROR_ALREADY_EXISTS))
	{
		::CloseHandle(hMappedFile);
		hMappedFile = nullptr;
		return true;
	}
	return false;
}

static BOOL __stdcall EnumIcons(HMODULE hModule, PSTR pszType, PSTR pszName, LONG_PTR lparam)
{
	(void)hModule;
	(void)pszType;
	(void)lparam;
	IconName = pszName;
	return FALSE;
}

#pragma region InitializeWindows
/*
 */
#pragma endregion InitializeWindows
/// <summary>
/// <c>InitializeWindows</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <returns></returns>
MECH_IMPEXP void __stdcall InitializeWindows(void)
{
	HMODULE hKernel32;
	PSTR message;
	PSTR pszFormat;
	RECT Rect;
	char Buffer2[260] = {0};
	va_list Arguments;
	char Buffer1[256];
	ATOM atom;
	BOOLEAN bSuccess;
	uint32_t dwStyle;
#if CONSIDERED_OBSOLETE
	OSVERSIONINFOA VersionInformation;
#endif

	InternalFunctionSpew("GameOS_Core", "InitializeWindows()");
	ArrowCursor = LoadCursor(
		nullptr, IDC_ARROW); // LoadCursorA(nullptr, MAKEINTRESOURCEA(32512) /*IDC_ARROW*/);
	DesktopBpp = static_cast<uint32_t>(GetDeviceCaps(DesktopDC, BITSPIXEL));
	DesktopRes = static_cast<uint32_t>(
		(GetDeviceCaps(DesktopDC, VERTRES) << 16) + GetDeviceCaps(DesktopDC, HORZRES));

#if CONSIDERED_OBSOLETE
	// _SetThreadExecutionState = nullptr;
	hKernel32 = GetModuleHandleA("kernel32.dll");
	_SetThreadExecutionState = reinterpret_cast<EXECUTION_STATE(__stdcall*)(EXECUTION_STATE)>(
		GetProcAddress(hKernel32, "SetThreadExecutionState"));
	_GetFileAttributesEx =
		reinterpret_cast<int32_t(__stdcall*)(PSTR, GET_FILEEX_INFO_LEVELS, PVOID)>(
			GetProcAddress(hKernel32, "GetFileAttributesExA"));
	_GetDiskFreeSpaceEx = reinterpret_cast<int32_t(__stdcall*)(PSTR, PULARGE_INTEGER,
		PULARGE_INTEGER, PULARGE_INTEGER)>(GetProcAddress(hKernel32, "GetDiskFreeSpaceExA"));

	if (_SetThreadExecutionState)
		_SetThreadExecutionState(ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED | ES_CONTINUOUS);
#endif
	SetThreadExecutionState(ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED | ES_CONTINUOUS);

	SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &bScreenSaver, 0);
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0, nullptr, SPIF_SENDCHANGE);
	StoredScreenSaverSetting = true;

#if CONSIDERED_OBSOLETE
	VersionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	GetVersionExA(&VersionInformation);
	if (VersionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT && VersionInformation.dwMajorVersion == 5 && VersionInformation.dwBuildNumber < 0x893 && InternalFunctionPause("Please upgrade to Windows 2000 RTM (Build 2195)"))
		ENTER_DEBUGGER;
#else
#if _CONSIDERED_OUTDATED
	if (IsWindowsVersionOrGreater(5, 0, 0) == FALSE)
	{
		message = "Please upgrade to Windows 2000 RTM (Build 2195)";
		MessageBoxA(0, message, ApplicationName, MB_ICONEXCLAMATION);
		status = AfterExit;
		ExitGameOS();
	}
#endif
#endif
	if (Platform == Platform_Game)
	{
		if ((DesktopBpp == 1) || ((RunFullScreen == false) && (DesktopBpp != 16) && (DesktopBpp != 32)))
		{
			// message = gos_GetResourceString(gLanguageDLL, 1020u);
			message = "Please select a color depth of 16 bit or 32 bit from "
						 "the settings tab of the display properties dialog "
						 "located in the control panel.";
			MessageBoxA(0, message, ApplicationName, MB_ICONEXCLAMATION);
			status = AfterExit;
			ExitGameOS();

			// ??? the code below must surely be dead
			Arguments = Buffer2;
			// pszFormat = gos_GetResourceString(gLanguageDLL, 1021u);
			pszFormat = "Desktop is in %1 mode. Hardware acceleration will not "
						"be available when running in a window";
			FormatMessageA(FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_FROM_STRING, message,
				0, 0, Buffer1, _countof(Buffer1), &Arguments);
#if CONSIDERED_UNSUPPORTED
			// Pointer to local array Buffer1 is stored outside the scope of
			// this array. Such a pointer will become invalid
			ErrorMessageTitle = Buffer1;
#endif
			if (gNoDialogs == false)
			{
				DocolourDialog();
				if (DesktopBpp == 1)
					ExitGameOS();
				if (ErrorReturn == -1)
					ExitGameOS();
				if (ErrorReturn == 1)
					RunFullScreen = true;
			}
		}
		widthX = Environment.screenwidth;
		heightY = Environment.screenheight;
		wndClass.style = CS_BYTEALIGNCLIENT | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		if (Environment.allowDoubleClicks)
			wndClass.style |= CS_DBLCLKS;
		wndClass.lpfnWndProc = (WNDPROC)&GameOSWinProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hinstance = hinstance;
		EnumResourceNamesA(hinstance, "Windows()", EnumIcons, 0);
		if (IconName)
			wndClass.hIcon = LoadIconA(hinstance, /*static_cast<PSTR>*/ (IconName));
		else
			wndClass.hIcon = nullptr;
		wndClass.hCursor = nullptr;
		wndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		wndClass.lpszMenuName = Environment.applicationName;
		wndClass.lpszClassName = Environment.applicationName;
		atom = RegisterClassA(&wndClass);
		Rect.left = 0;
		Rect.top = 0;
		Rect.right = widthX;
		Rect.bottom = heightY;
		dwStyle = WS_CAPTION | WS_SYSMENU | WS_GROUP | WS_TABSTOP;
		bSuccess = AdjustWindowRect(&Rect, dwStyle, FALSE) == TRUE;
		if (atom && bSuccess)
		{
			hwnd = CreateWindowExA(0, Environment.applicationName, Environment.applicationName,
				dwStyle, WindowStartX, WindowStartY, Rect.right - Rect.left, Rect.bottom - Rect.top,
				nullptr, nullptr, hinstance, nullptr);
			if (hwnd)
			{
				if (gDumpMachineInfo == false)
					ShowWindow(hwnd, SW_SHOWDEFAULT);
				InitializeIME(hwnd);
				gos_EnableIME(false);
			}
			else
			{
				if (InternalFunctionStop("Cannot initialize windows"))
					ENTER_DEBUGGER;
			}
		}
	}
}

#pragma region DestroyWindows
/*
 */
#pragma endregion DestroyWindows
/// <summary>
/// <c>DestroyWindows</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name=""></param>
/// <returns></returns>
MECH_IMPEXP void __stdcall DestroyWindows(void)
{
	if (_SetThreadExecutionState)
		_SetThreadExecutionState(ES_CONTINUOUS);
	if (StoredScreenSaverSetting)
	{
		SystemParametersInfo(
			SPI_SETSCREENSAVEACTIVE, static_cast<uint32_t>(bScreenSaver), nullptr, SPIF_SENDCHANGE);
		StoredScreenSaverSetting = false;
	}
	if (wndClass.hIcon)
		DestroyIcon(wndClass.hIcon);
	if (hMappedFile)
	{
		CloseHandle(hMappedFile);
		hMappedFile = nullptr;
	}
}

#pragma region Update
/*
 */
#pragma endregion Update
/// <summary>
/// <c>Update</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <returns></returns>
MECH_IMPEXP void __stdcall Update(void)
{
	HWND hDlg;
	if (WindowsPause)
	{
		WaitMessage();
	}
	while (PeekMessageA(&msg, 0, 0, 0, 1u) && msg.message != WM_QUIT)
	{
		if (Platform == Platform_MFC)
		{
			hDlg = GetForegroundWindow();
			if (hDlg != hwnd)
			{
				if (IsDialogMessageA(hDlg, &msg))
					continue;
			}
		}
		if (!IgnoreImeHotKey(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}
	if (ForceReStart)
	{
		ForceReStart = 0;
		RestartGameOS();
	}
	if (msg.message == WM_QUIT)
		WindowClosed = true;
	if (gActive && gGotFocus)
	{
		if (LoseFocusBehavior == 3)
		{
#if _CONSIDERED_UNCLEAR_IMPLEMENTATION
			static uint64_t timeLastTime;
			uint64_t timeelapsed;
			uint64_t timenow;
			// ????
			timenow = gos_GetHiResTime();
			timeelapsed = (0.01666666666666667 - (timenow - timeLastTime));
			if (timeelapsed > 0)
				Sleep((timeelapsed * 1000u));
			timeLastTime = timenow;
#endif
		}
	}
	else
	{
		if (LoseFocusBehavior != 0)
		{
			if (LoseFocusBehavior == 1)
				WaitMessage();
		}
	}
}

#pragma region gos_GetClipboardText
/*
Gets a pointer to text data in the windows clip board (0=No text)
*/
#pragma endregion gos_GetClipboardText
/// <summary>
/// <c>gos_GetClipboardText</c>
/// </summary>
/// <remarks>
/// return type changed from uint32_t to size_t
/// </remarks>
/// <param name="Buffer"></param>
/// <param name="BufferSize"></param>
/// <returns></returns>
MECH_IMPEXP size_t __stdcall gos_GetClipboardText(PSTR Buffer, size_t BufferSize)
{
	size_t nstringsize;
	PSTR Source;
	HANDLE hClipboardData;
	if (Buffer == nullptr)
		return 0;
	Buffer[0] = 0;
	if (IsClipboardFormatAvailable(CF_TEXT))
	{
		if (OpenClipboard(hwnd))
		{
			hClipboardData = GetClipboardData(CF_TEXT);
			if (hClipboardData)
			{
				Source = (PSTR)GlobalLock(hClipboardData);
				if (Source)
				{
					strncpy_s(Buffer, BufferSize, Source, BufferSize - 1);
					Buffer[BufferSize - 1] = 0;
					GlobalUnlock(hClipboardData);
				}
				CloseClipboard();
				nstringsize = strlen(Buffer);
				return nstringsize;
			}
			else
			{
				CloseClipboard();
			}
		}
	}
	return 0u;
}

#pragma region gos_SetClipboardText
/*
Sets the windows clipboard to the current text string
*/
#pragma endregion gos_SetClipboardText
/// <summary>
/// <c>gos_SetClipboardText</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pszText"></param>
/// <returns></returns>
MECH_IMPEXP void __stdcall gos_SetClipboardText(PSTR pszText)
{
	size_t nsizetext;
	PVOID ClipboardData;
	HGLOBAL hClipboardData;
	if (OpenClipboard(hwnd))
	{
		EmptyClipboard();
		if (*pszText)
		{
			nsizetext = strlen(pszText);
			hClipboardData = GlobalAlloc(GMEM_SHARE, nsizetext + 1);
			if (hClipboardData)
			{
				ClipboardData = GlobalLock(hClipboardData);
				nsizetext = strlen(pszText);
				if (ClipboardData)
					memcpy(ClipboardData, pszText, nsizetext + 1);
				GlobalUnlock(hClipboardData);
				SetClipboardData(CF_TEXT, hClipboardData);
				CloseClipboard();
			}
			else
			{
				CloseClipboard();
			}
		}
	}
}
