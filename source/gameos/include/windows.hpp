#pragma once
//===========================================================================//
// File:	 Windows.hpp													 //
// Contents: Window-related routines										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "Globals.hpp"

PSTR __stdcall GetWindowsMessage( uint32_t uMsg,WPARAM wParam,LPARAM lParam );
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
extern int32_t			widthX;
extern int32_t			heightY;
extern int32_t			WindowStartX;
extern int32_t			WindowStartY;
extern bool			WindowsNT;
extern uint32_t		HitScrollLock;
extern uint32_t		ForceReStart;
extern uint32_t		DesktopBpp;
extern uint32_t		DesktopRes;
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
extern BOOL (__stdcall* _GetWsChanges)(HANDLE hProcess,PPSAPI_WS_WATCH_INFORMATION pWatchInfo,uint32_t cb);
extern uint32_t (__stdcall* _GetMappedFileName)(HANDLE hProcess,PVOID pv,PSTR pFilename,uint32_t nSize);


extern HRESULT (__stdcall* _DirectDrawCreateEx)( GUID* lpGUID, PVOID* lplpDD, const IID* iid, LPUNKNOWN pUnkOuter ); 
extern HRESULT (__stdcall* _DirectDrawEnumerate)( LPDDENUMCALLBACK pCallback, PVOID lpContext );
extern HRESULT (__stdcall* _DirectDrawEnumerateEx)( LPDDENUMCALLBACKEX lpCallback, PVOID lpContext, uint32_t Flags );
extern HRESULT (__stdcall* _DirectInputCreate)( HINSTANCE hinst, uint32_t dwVersion, LPDIRECTINPUT * lplpDirectInput, LPUNKNOWN punkOuter );
extern HRESULT (__stdcall* _DirectSoundCreate)( LPGUID lpGuid, LPDIRECTSOUND * ppDS, LPUNKNOWN  pUnkOuter );
extern HRESULT (__stdcall* _DirectInputCreateEx)( HINSTANCE hinst, uint32_t dwVersion, const IID* refID, PVOID* lplpDirectInput, LPUNKNOWN punkOuter );
extern HRESULT (__stdcall* _DirectSoundEnumerate)( LPDSENUMCALLBACK lpDSEnumCallback, PVOID lpContext );
extern HWND (__stdcall* _CallmyHelp)( HWND hWindow, HINSTANCE hInst, bool Window, uint32_t HelpItem );
extern void (__stdcall* _FreeHlpLib)(void);
extern uint32_t (__stdcall* _AMGetErrorText)( HRESULT hr, TCHAR *pBuffer, uint32_t MaxLen );

// WinMain.cpp
extern float frameRate;
extern char AssetsDirectory1[MAX_PATH];
extern char AssetsDirectory2[MAX_PATH];
extern char ImageHelpPath[MAX_PATH];
extern char ApplicationName[256];
extern char RegistryKey[256];
extern uint32_t ThreadId;
extern uint32_t SkipRendering;
extern bool gNoBlade;
extern void __stdcall RestartGameOS(void);
extern void __stdcall DoGame(void);
extern void __stdcall gos_UpdateDisplay( bool Everything );



