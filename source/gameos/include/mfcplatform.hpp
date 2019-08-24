
//===========================================================================//
// File:	 MFCPlatform.hpp												 //
// Contents: Setup GameOS for running DLL's									 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#define WIN32_EXTRA_LEAN // Reduce windows header size
#define VC_EXTRALEAN // Reduce windows header size
#include <windows.h>
#include "platform.hpp"
#include "gameos.hpp"

NTSTATUS __stdcall ProcessException(PEXCEPTION_POINTERS pep);
void __stdcall InitExceptionHandler(PSTR commandline);
void __stdcall InitGameOS(HINSTANCE hinstance, HWND hwnd, PSTR commandline);
LRESULT __stdcall GameOSWinProc(HWND hwnd, uint32_t message, WPARAM wparam, LPARAM lparam);
uint32_t __stdcall RunGameOSLogic(void);
void __stdcall ExitGameOS(void);
