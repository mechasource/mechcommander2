#pragma once
//===========================================================================//
// File:	 Windows.hpp													 //
// Contents: Window-related routines										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "Globals.hpp"

PSTR __stdcall GetWindowsMessage( ULONG uMsg,WPARAM wParam,LPARAM lParam );
void __stdcall InitializeWindows(void);
void __stdcall DestroyWindows(void);
bool __stdcall AlreadyRunning(void);
void __stdcall SystemCheck(void);
void __stdcall Update(void);

typedef enum Status { 
	BeforeInit,Uninitialized, GameInit, Running, Paused, AfterExit 
} Status;

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
void __stdcall InitLibraries(void);		// Load all libraries GOS depends upon
void __stdcall DestroyLibraries(void);	// Unload any libraries GOS loaded
PSTR __stdcall GetDirectXVersion(void);	// Returns a string describing the current version of DirectX


// Functions only in Win98/2000
typedef BOOL (__stdcall* T_GetFileAttributesEx)( PCSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, PVOID lpFileInformation );
typedef EXECUTION_STATE (__stdcall* T_SetThreadExecutionState)( EXECUTION_STATE esFlags );
typedef BOOL (__stdcall* T_GetDiskFreeSpaceEx)(PCSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

extern T_SetThreadExecutionState _SetThreadExecutionState;
extern T_GetFileAttributesEx _GetFileAttributesEx;
extern T_GetDiskFreeSpaceEx _GetDiskFreeSpaceEx;

extern BOOL (__stdcall* _EmptyWorkingSet)(HANDLE hProcess);
extern BOOL (__stdcall* _InitializeProcessForWsWatch)(HANDLE hProcess);
extern BOOL (__stdcall* _GetWsChanges)(HANDLE hProcess,PPSAPI_WS_WATCH_INFORMATION pWatchInfo,ULONG cb);
extern ULONG (__stdcall* _GetMappedFileName)(HANDLE hProcess,PVOID pv,LPTSTR pFilename,ULONG nSize);


extern HRESULT (__stdcall* _DirectDrawCreateEx)( GUID* lpGUID, void** lplpDD, const IID* iid, LPUNKNOWN pUnkOuter ); 
extern HRESULT (__stdcall* _DirectDrawEnumerate)( LPDDENUMCALLBACK pCallback, PVOID lpContext );
extern HRESULT (__stdcall* _DirectDrawEnumerateEx)( LPDDENUMCALLBACKEX lpCallback, PVOID lpContext, ULONG Flags );
extern HRESULT (__stdcall* _DirectInputCreate)( HINSTANCE hinst, ULONG dwVersion, LPDIRECTINPUT * lplpDirectInput, LPUNKNOWN punkOuter );
extern HRESULT (__stdcall* _DirectSoundCreate)( LPGUID lpGuid, LPDIRECTSOUND * ppDS, LPUNKNOWN  pUnkOuter );
extern HRESULT (__stdcall* _DirectInputCreateEx)( HINSTANCE hinst, ULONG dwVersion, const IID* refID, void** lplpDirectInput, LPUNKNOWN punkOuter );
extern HRESULT (__stdcall* _DirectSoundEnumerate)( LPDSENUMCALLBACK lpDSEnumCallback, PVOID lpContext );
extern HWND (__stdcall* _CallmyHelp)( HWND hWindow, HINSTANCE hInst, bool Window, ULONG HelpItem );
extern void (__stdcall* _FreeHlpLib)(void);
extern ULONG (__stdcall* _AMGetErrorText)( HRESULT hr, TCHAR *pBuffer, ULONG MaxLen );

// WinMain.cpp
extern float frameRate;
extern char AssetsDirectory1[MAX_PATH];
extern char AssetsDirectory2[MAX_PATH];
extern char ImageHelpPath[MAX_PATH];
extern char ApplicationName[256];
extern char RegistryKey[256];
extern ULONG ThreadId;
extern ULONG SkipRendering;
extern bool gNoBlade;
extern void __stdcall RestartGameOS(void);
extern void __stdcall DoGame(void);
extern void __stdcall gos_UpdateDisplay( bool Everything );



