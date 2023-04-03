//===========================================================================//
// File:	 Platform.cpp													 //
// Contents: Setup GameOS for running games, MFC or DLL's					 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#pragma once

enum class PlatformType : uint8_t
{
	Platform_Game,
	Platform_MFC,
	Platform_DLL
};
extern PlatformType Platform;

extern void _stdcall RunFromOtherApp(HINSTANCE hinstance, HWND hwnd, PSTR commandline);
extern int32_t _stdcall RunFromWinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, PSTR commandline, int32_t nCmdShow);
