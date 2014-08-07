//===========================================================================//
// File:	 DLLPlatform.hpp												 //
// Contents: Setup GameOS for running DLL's									 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#define WIN32_EXTRA_LEAN			// Reduce windows header size
#include <windows.h>
#include <gameos\platform.hpp>
#include <gameos\gameos.hpp>


NTSTATUS _stdcall ProcessException( PEXCEPTION_POINTERS pep);
void _stdcall InitExceptionHandler( PSTR CommandLine);
void _stdcall InitGameOS( HINSTANCE hInstance, HWND hWindow, PSTR CommandLine);
LRESULT _stdcall GameOSWinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
ULONG _stdcall RunGameOSLogic(void);
void _stdcall ExitGameOS(void);

