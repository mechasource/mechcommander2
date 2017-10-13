/*******************************************************************************
 Copyright (c) 2011-2014, Jerker Back. All rights reserved.

 Permission to use, copy, modify, and distribute this software for any
 purpose with or without fee is hereby granted, provided that the following
 conditions are met (OSI approved BSD 2-clause license):

 1. Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
/*******************************************************************************
 windows.cpp - gameos reference pseudo code

 MechCommander 2 source code

 2014-07-24 jerker_back, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

#include "stdafx.h"

#include <gameos.hpp>
#include <globals.hpp>
#include <winproc.hpp>
#include <platform.hpp>
#include <toolos.hpp>
#include <errorhandler.hpp>
#include <windows.hpp>

#pragma warning(                                                               \
	disable : 4191) // 'type cast' : unsafe conversion from 'FARPROC'

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

MECH_IMPEXP EXECUTION_STATE(__stdcall* _SetThreadExecutionState)(
	EXECUTION_STATE);
MECH_IMPEXP int32_t(__stdcall* _GetFileAttributesEx)(
	PCSTR, GET_FILEEX_INFO_LEVELS, PVOID);
MECH_IMPEXP int32_t(__stdcall* _GetDiskFreeSpaceEx)(
	PCSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

MECH_IMPEXP HANDLE hMappedFile;
MECH_IMPEXP MSG msg;
MECH_IMPEXP WNDCLASSA wndClass;
MECH_IMPEXP HWND hWindow;
MECH_IMPEXP HINSTANCE hInstance;
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
MECH_IMPEXP bool StoredScreenSaverSetting;
MECH_IMPEXP PSTR CommandLine;

// -----------------------------------------------------------------------------
// implemented functions

MECH_IMPEXP bool __stdcall AlreadyRunning(void);
MECH_IMPEXP void __stdcall InitializeWindows(void);
MECH_IMPEXP void __stdcall DestroyWindows(void);
MECH_IMPEXP void __stdcall Update(void);
MECH_IMPEXP size_t __stdcall gos_GetClipboardText(
	PSTR Buffer, size_t BufferSize);
MECH_IMPEXP void __stdcall gos_SetClipboardText(PSTR Text);

static BOOL __stdcall EnumIcons(
	HMODULE hModule, PCSTR pszType, PSTR pszName, LONG_PTR lParam);

// -----------------------------------------------------------------------------
// externals not specified in headers
extern bool RunFullScreen;
extern bool gDumpMachineInfo;
extern bool gNoDialogs;
extern uint32_t gLanguageDLL;
extern int32_t LoseFocusBehavior; // enum?
extern bool WindowsPause;

void __stdcall InitializeIME(HWND hWnd);
bool __stdcall IgnoreImeHotKey(PMSG pmsg);

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
MECH_IMPEXP bool __stdcall AlreadyRunning(void)
{
	WINDOWPLACEMENT wndpl;
	HWND hWnd;
	hWnd =
		::FindWindowA(Environment.applicationName, Environment.applicationName);
	if (hWnd)
	{
		wndpl.length = sizeof(wndpl);
		::GetWindowPlacement(hWnd, &wndpl);
		::SetForegroundWindow(hWnd);
		if (wndpl.showCmd == SW_SHOWMINIMIZED)
			::ShowWindow(hWnd, SW_RESTORE);
		return true;
	}
	hMappedFile = ::CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr,
		SEC_RESERVE | PAGE_READONLY, 0, 4u, ApplicationName);
	if (hMappedFile && (GetLastError() == ERROR_ALREADY_EXISTS))
	{
		::CloseHandle(hMappedFile);
		hMappedFile = nullptr;
		return true;
	}
	return false;
}

static BOOL __stdcall EnumIcons(
	HMODULE hModule, PCSTR pszType, PSTR pszName, LONG_PTR lParam)
{
	(void)hModule;
	(void)pszType;
	(void)lParam;
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
	PCSTR pszMessage;
	PCSTR pszFormat;
	RECT Rect;
	char Buffer2[260] = {0};
	va_list Arguments;
	char Buffer1[256];
	ATOM atom;
	bool bSuccess;
	uint32_t dwStyle;
#if _CONSIDERED_OBSOLETE
	OSVERSIONINFOA VersionInformation;
#endif
	InternalFunctionSpew("GameOS_Core", "InitializeWindows()");
	ArrowCursor = LoadCursor(nullptr, IDC_ARROW); // LoadCursorA(nullptr,
												  // MAKEINTRESOURCEA(32512)
												  // /*IDC_ARROW*/);
	DesktopBpp = static_cast<uint32_t>(GetDeviceCaps(DesktopDC, BITSPIXEL));
	DesktopRes =
		static_cast<uint32_t>((GetDeviceCaps(DesktopDC, VERTRES) << 16) +
							  GetDeviceCaps(DesktopDC, HORZRES));
	// _SetThreadExecutionState = nullptr;
	hKernel32 = GetModuleHandleA("kernel32.dll");
	_SetThreadExecutionState =
		reinterpret_cast<EXECUTION_STATE(__stdcall*)(EXECUTION_STATE)>(
			GetProcAddress(hKernel32, "SetThreadExecutionState"));
	_GetFileAttributesEx =
		reinterpret_cast<int32_t(__stdcall*)(PCSTR, GET_FILEEX_INFO_LEVELS,
			PVOID)>(GetProcAddress(hKernel32, "GetFileAttributesExA"));
	_GetDiskFreeSpaceEx = reinterpret_cast<int32_t(__stdcall*)(
		PCSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER)>(
		GetProcAddress(hKernel32, "GetDiskFreeSpaceExA"));
	if (_SetThreadExecutionState)
		_SetThreadExecutionState(
			ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED | ES_CONTINUOUS);
	SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &bScreenSaver, 0);
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0, nullptr, SPIF_SENDCHANGE);
	StoredScreenSaverSetting = true;
#if _CONSIDERED_OBSOLETE
	VersionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	GetVersionExA(&VersionInformation);
	if (VersionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT &&
		VersionInformation.dwMajorVersion == 5 &&
		VersionInformation.dwBuildNumber < 0x893 &&
		InternalFunctionPause(
			"Please upgrade to Windows 2000 RTM (Build 2195)"))
		ENTER_DEBUGGER;
#else
	if (IsWindowsVersionOrGreater(5, 0, 0) == FALSE)
	{
		pszMessage = "Please upgrade to Windows 2000 RTM (Build 2195)";
		MessageBoxA(0, pszMessage, ApplicationName, MB_ICONEXCLAMATION);
		status = AfterExit;
		ExitGameOS();
	}
#endif
	if (Platform == Platform_Game)
	{
		if ((DesktopBpp == 1) || ((RunFullScreen == false) &&
									 (DesktopBpp != 16) && (DesktopBpp != 32)))
		{
			// pszMessage = gos_GetResourceString(gLanguageDLL, 1020u);
			pszMessage = "Please select a color depth of 16 bit or 32 bit from "
						 "the settings tab of the display properties dialog "
						 "located in the control panel.";
			MessageBoxA(0, pszMessage, ApplicationName, MB_ICONEXCLAMATION);
			status = AfterExit;
			ExitGameOS();
			// ??? the code below must surely be dead
			Arguments = Buffer2;
			// pszFormat = gos_GetResourceString(gLanguageDLL, 1021u);
			pszFormat = "Desktop is in %1 mode. Hardware acceleration will not "
						"be available when running in a window";
			FormatMessageA(
				FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_FROM_STRING,
				pszMessage, 0, 0, Buffer1, _countof(Buffer1), &Arguments);
#if _CONSIDERED_UNSUPPORTED
			// Pointer to local array Buffer1 is stored outside the scope of
			// this array. Such a pointer will become invalid
			ErrorMessageTitle = Buffer1;
#endif
			if (gNoDialogs == false)
			{
				DoColorDialog();
				if (DesktopBpp == 1)
					ExitGameOS();
				if (ErrorReturn == -1)
					ExitGameOS();
				if (ErrorReturn == 1)
					RunFullScreen = true;
			}
		}
		widthX  = Environment.screenWidth;
		heightY = Environment.screenHeight;
		wndClass.style =
			CS_BYTEALIGNCLIENT | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		if (Environment.allowDoubleClicks)
			wndClass.style |= CS_DBLCLKS;
		wndClass.lpfnWndProc = (WNDPROC)&GameOSWinProc;
		wndClass.cbClsExtra  = 0;
		wndClass.cbWndExtra  = 0;
		wndClass.hInstance   = hInstance;
		EnumResourceNamesA(hInstance, "Windows()", EnumIcons, 0);
		if (IconName)
			wndClass.hIcon =
				LoadIconA(hInstance, /*static_cast<PCSTR>*/ (IconName));
		else
			wndClass.hIcon = nullptr;
		wndClass.hCursor = nullptr;
		wndClass.hbrBackground =
			static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		wndClass.lpszMenuName  = Environment.applicationName;
		wndClass.lpszClassName = Environment.applicationName;
		atom				   = RegisterClassA(&wndClass);
		Rect.left			   = 0;
		Rect.top			   = 0;
		Rect.right			   = widthX;
		Rect.bottom			   = heightY;
		dwStyle  = WS_CAPTION | WS_SYSMENU | WS_GROUP | WS_TABSTOP;
		bSuccess = AdjustWindowRect(&Rect, dwStyle, FALSE) == TRUE;
		if (atom && bSuccess)
		{
			hWindow = CreateWindowExA(0, Environment.applicationName,
				Environment.applicationName, dwStyle, WindowStartX,
				WindowStartY, Rect.right - Rect.left, Rect.bottom - Rect.top,
				nullptr, nullptr, hInstance, nullptr);
			if (hWindow)
			{
				if (gDumpMachineInfo == false)
					ShowWindow(hWindow, SW_SHOWDEFAULT);
				InitializeIME(hWindow);
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
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,
			static_cast<uint32_t>(bScreenSaver), nullptr, SPIF_SENDCHANGE);
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
/// <param name=""></param>
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
			if (hDlg != hWindow)
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
			timenow		= gos_GetHiResTime();
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
MECH_IMPEXP size_t __stdcall gos_GetClipboardText(
	PSTR Buffer, size_t BufferSize)
{
	size_t nstringsize;
	PCSTR Source;
	HANDLE hClipboardData;
	if (Buffer == nullptr)
		return 0;
	Buffer[0] = 0;
	if (IsClipboardFormatAvailable(CF_TEXT))
	{
		if (OpenClipboard(hWindow))
		{
			hClipboardData = GetClipboardData(CF_TEXT);
			if (hClipboardData)
			{
				Source = (PCSTR)GlobalLock(hClipboardData);
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
	if (OpenClipboard(hWindow))
	{
		EmptyClipboard();
		if (*pszText)
		{
			nsizetext	  = strlen(pszText);
			hClipboardData = GlobalAlloc(GMEM_SHARE, nsizetext + 1);
			if (hClipboardData)
			{
				ClipboardData = GlobalLock(hClipboardData);
				nsizetext	 = strlen(pszText);
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
