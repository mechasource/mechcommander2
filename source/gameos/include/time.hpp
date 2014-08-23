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

extern ULONG WatchDogFrame;
extern ULONG CurrentFrameNumber;
extern int64_t StartCount;
extern int64_t frequency;
extern int64_t StartOfRenderTime;
