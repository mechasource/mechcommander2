#pragma once
//===========================================================================//
// File:	Time.cpp                                                         //
// Contents: Time Function													 //												 
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//


void __stdcall TimeManagerInstall();
void __stdcall StartTimeManager();
void __stdcall TimeManagerUpdate();
void __stdcall TimeManagerUninstall();
void __stdcall StartPauseTime();
void __stdcall EndPauseTime();

extern DWORD WatchDogFrame;
extern DWORD CurrentFrameNumber;
extern __int64 StartCount;
extern __int64 frequency;
extern __int64 StartOfRenderTime;
