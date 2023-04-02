#pragma once
//===========================================================================//
// File:	 Windows.hpp													 //
// Contents: Window-related routines										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "globals.hpp"

// clang-format off

#ifndef MECH_IMPEXP
#define MECH_IMPEXP extern
#endif

// windows.cpp
MECH_IMPEXP BOOLEAN __stdcall AlreadyRunning(void);
MECH_IMPEXP void __stdcall InitializeWindows(void);
MECH_IMPEXP void __stdcall DestroyWindows(void);
MECH_IMPEXP void __stdcall Update(void);

// windowsdebugging.cpp
MECH_IMPEXP PSTR __stdcall GetWindowsMessage(uint32_t message, WPARAM wparam, LPARAM lparam);

void __stdcall SystemCheck(void);

enum class Status
{
	BeforeInit,
	Uninitialized,
	GameInit,
	Running,
	Paused,
	AfterExit
} Status;

extern HDC DesktopDC;
extern HWND hwnd;
extern HINSTANCE hinstance;
extern WNDCLASSA wndClass;
extern PSTR commandline;
extern MSG msg;
extern Status status;
extern BOOL fullScreen;
extern POINT clientToScreen;
extern int32_t widthX;
extern int32_t heightY;
extern int32_t WindowStartX;
extern int32_t WindowStartY;
extern BOOLEAN WindowsNT;
extern uint32_t HitScrollLock;
extern uint32_t ForceReStart;
extern uint32_t DesktopBpp;
extern uint32_t DesktopRes;
extern BOOLEAN PerfCounters;
extern BOOLEAN WindowClosed;

// Libraries.cpp routines
void __stdcall InitLibraries(void);	// Load all libraries GOS depends upon
void __stdcall DestroyLibraries(void); // Unload any libraries GOS loaded
PSTR __stdcall GetDirectXVersion(
	void); // Returns a string describing the current version of DirectX

// Functions only in Win98/2000
typedef BOOL(__stdcall* T_GetFileAttributesEx)(PSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, PVOID lpFileInformation);
typedef EXECUTION_STATE(__stdcall* T_SetThreadExecutionState)(EXECUTION_STATE esFlags);
typedef BOOL(__stdcall* T_GetDiskFreeSpaceEx)(PSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

extern T_SetThreadExecutionState _SetThreadExecutionState;
extern T_GetFileAttributesEx _GetFileAttributesEx;
extern T_GetDiskFreeSpaceEx _GetDiskFreeSpaceEx;

extern BOOL(__stdcall* _EmptyWorkingSet)(HANDLE hProcess);
extern BOOL(__stdcall* _InitializeProcessForWsWatch)(HANDLE hProcess);
extern BOOL(__stdcall* _GetWsChanges)(HANDLE hProcess, struct _PSAPI_WS_WATCH_INFORMATION* pWatchInfo, uint32_t cb);
extern uint32_t(__stdcall* _GetMappedFileName)(HANDLE hProcess, PVOID pv, PSTR pFilename, uint32_t nSize);

typedef int32_t(__stdcall* PDIRECTXSETUPGETVERSION)(uint32_t* pdwVersion, uint32_t* pdwMinorVersion);
typedef HRESULT(__stdcall* PDIRECTDRAWCREATEEX)(GUID* pGUID, PVOID* ppDD, REFIID iid, LPUNKNOWN punkouter);
typedef HRESULT(__stdcall* PDIRECTDRAWENUMERATEA)(LPDDENUMCALLBACKA pCallback, PVOID pcontext);
typedef HRESULT(__stdcall* PDIRECTDRAWENUMERATEEXA)(LPDDENUMCALLBACKEXA pCallback, PVOID pcontext, uint32_t flags);
typedef HRESULT(__stdcall* PDIRECTSOUNDCREATE)(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN punkouter);
typedef HRESULT(__stdcall* PDIRECTSOUNDENUMERATEA)(LPDSENUMCALLBACKA pDSEnumCallback, PVOID pcontext);
typedef HRESULT(__stdcall* PDIRECTINPUTCREATEEX)(HINSTANCE hinstance, uint32_t version, REFIID riidltf, PVOID* ppvOut, LPUNKNOWN punkOuter);

extern HRESULT(__stdcall* _DirectDrawCreateEx)(GUID* pGUID, PVOID* ppDD, REFIID iid, LPUNKNOWN punkouter);
extern HRESULT(__stdcall* _DirectDrawEnumerate)(LPDDENUMCALLBACKA pCallback, PVOID pcontext);
extern HRESULT(__stdcall* _DirectDrawEnumerateEx)(LPDDENUMCALLBACKEXA pCallback, PVOID pcontext, uint32_t flags);
extern HRESULT(__stdcall* _DirectSoundCreate)(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN punkouter);
extern HRESULT(__stdcall* _DirectSoundEnumerate)(LPDSENUMCALLBACKA pDSEnumCallback, PVOID pcontext);
extern HRESULT(__stdcall* _DirectInputCreate)(HINSTANCE hinst, uint32_t version, LPDIRECTINPUT* lplpDirectInput, LPUNKNOWN punkOuter);
extern HRESULT(__stdcall* _DirectInputCreateEx)(HINSTANCE hinstance, uint32_t version, REFIID riidltf, PVOID* ppvOut, LPUNKNOWN punkOuter);
extern HWND(__stdcall* _CallmyHelp)(HWND hwnd, HINSTANCE hinstance, BOOLEAN Window, uint32_t HelpItem);
extern void(__stdcall* _FreeHlpLib)(void);
extern uint32_t(__stdcall* _AMGetErrorText)(HRESULT hr, PSTR pbuffer, uint32_t MaxLen);

// WinMain.cpp
extern float frameRate;
extern char AssetsDirectory1[MAX_PATH];
extern char AssetsDirectory2[MAX_PATH];
extern char ImageHelpPath[MAX_PATH];
extern char ApplicationName[256];
extern char RegistryKey[256];
extern uint32_t ThreadId;
extern uint32_t SkipRendering;
extern BOOLEAN gNoBlade;
extern void __stdcall RestartGameOS(void);
extern void __stdcall DoGame(void);
extern void __stdcall gos_UpdateDisplay(BOOLEAN everything);

// clang-format on