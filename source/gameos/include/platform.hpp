//===========================================================================//
// File:	 Platform.cpp													 //
// Contents: Setup GameOS for running games, MFC or DLL's					 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#pragma once


typedef enum PlatformType {
	Platform_Game,
	Platform_MFC,
	Platform_DLL
} PlatformType;
extern PlatformType Platform;

extern void _stdcall RunFromOtherApp(HINSTANCE hInstance, HWND hWnd, PSTR CommandLine);
extern int _stdcall RunFromWinMain( HINSTANCE hInst, HINSTANCE hPrevInst, PSTR lpCmdLine, int nCmdShow );