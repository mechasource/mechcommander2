//===========================================================================//
// File:	 Platform.cpp													 //
// Contents: Setup GameOS for running games, MFC or DLL's					 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#pragma once

typedef enum PlatformType
{
	Platform_Game,
	Platform_MFC,
	Platform_DLL
} PlatformType;
extern PlatformType Platform;

extern void _stdcall RunFromOtherApp(HINSTANCE hInstance, HWND hWnd, PSTR pCmdLine);
extern int32_t _stdcall RunFromWinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int32_t nCmdShow);
