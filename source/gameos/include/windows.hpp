#pragma once
//===========================================================================//
// File:	 Windows.hpp													 //
// Contents: Window-related routines										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "Globals.hpp"

PSTR __stdcall GetWindowsMessage( ULONG uMsg,WPARAM wParam,LPARAM lParam );
void __stdcall InitializeWindows();
void __stdcall DestroyWindows();
bool __stdcall AlreadyRunning();
void __stdcall SystemCheck();
void __stdcall Update();

enum Status { BeforeInit,Uninitialized, GameInit, Running, Paused, AfterExit };

extern HDC			DesktopDC;
extern HWND			hWindow;
extern HINSTANCE	hInstance;
extern WNDCLASS		wndClass;
extern PSTR 		CommandLine;
extern MSG			msg;
extern Status		status;
extern BOOL			fullScreen;
extern POINT		clientToScreen;
extern int			widthX;
extern int			heightY;
extern int			WindowStartX;
extern int			WindowStartY;
extern bool			WindowsNT;
extern ULONG		HitScrollLock;
extern ULONG		ForceReStart;
extern ULONG		DesktopBpp;
extern ULONG		DesktopRes;
extern bool			PerfCounters;
extern bool			WindowClosed;

// Libraries.cpp routines
void __stdcall InitLibraries();		// Load all libraries GOS depends upon
void __stdcall DestroyLibraries();	// Unload any libraries GOS loaded
PSTR __stdcall GetDirectXVersion();	// Returns a string describing the current version of DirectX


// Functions only in Win98/2000
typedef BOOL (__stdcall* T_GetFileAttributesEx)( PCSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, PVOID lpFileInformation );
typedef EXECUTION_STATE (__stdcall* T_SetThreadExecutionState)( EXECUTION_STATE esFlags );
typedef BOOL (__stdcall* T_GetDiskFreeSpaceEx)(PCSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

extern T_SetThreadExecutionState _SetThreadExecutionState;
extern T_GetFileAttributesEx _GetFileAttributesEx;
extern T_GetDiskFreeSpaceEx _GetDiskFreeSpaceEx;

extern BOOL (__stdcall* _EmptyWorkingSet)(HANDLE);
extern BOOL (__stdcall* _InitializeProcessForWsWatch)(HANDLE);
extern BOOL (__stdcall* _GetWsChanges)(HANDLE,PPSAPI_WS_WATCH_INFORMATION,ULONG);
extern ULONG (__stdcall* _GetMappedFileName)(HANDLE,PVOID,LPTSTR,ULONG);

DirectDrawCreateEx

extern HRESULT (__stdcall* _DirectDrawCreateEx)( GUID* lpGUID, void** lplpDD, const IID* iid, LPUNKNOWN pUnkOuter ); 
extern HRESULT (__stdcall* _DirectDrawEnumerate)( LPDDENUMCALLBACK pCallback, PVOID lpContext );
extern HRESULT (__stdcall* _DirectDrawEnumerateEx)( LPDDENUMCALLBACKEX lpCallback, PVOID lpContext, ULONG Flags );
extern HRESULT (__stdcall* _DirectInputCreate)( HINSTANCE hinst, ULONG dwVersion, LPDIRECTINPUT * lplpDirectInput, LPUNKNOWN punkOuter );
extern HRESULT (__stdcall* _DirectSoundCreate)( LPGUID lpGuid, LPDIRECTSOUND * ppDS, LPUNKNOWN  pUnkOuter );
extern HRESULT (__stdcall* _DirectInputCreateEx)( HINSTANCE hinst, ULONG dwVersion, const IID* refID, void** lplpDirectInput, LPUNKNOWN punkOuter );
extern HRESULT (__stdcall* _DirectSoundEnumerate)( LPDSENUMCALLBACK lpDSEnumCallback, PVOID lpContext );
extern HWND (__stdcall* _CallmyHelp)( HWND hWindow, HINSTANCE hInst, bool Window, ULONG HelpItem );
extern void (__stdcall* _FreeHlpLib)();
extern ULONG (__stdcall* _AMGetErrorText)( HRESULT hr, TCHAR *pBuffer, ULONG MaxLen );

// WinMain.cpp
extern float frameRate;
extern char AssetsDirectory1[MAX_PATH];
extern char AssetsDirectory2[MAX_PATH];
extern char ImageHelpPath[MAX_PATH];
extern char ApplicationName[256];
extern void RestartGameOS();
extern char RegistryKey[256];
extern ULONG ThreadId;
extern ULONG SkipRendering;
extern bool gNoBlade;
extern void __stdcall DoGame(void);
extern void __stdcall gos_UpdateDisplay( bool Everything );



