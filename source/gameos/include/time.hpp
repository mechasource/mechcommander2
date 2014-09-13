#pragma once
//===========================================================================//
// File:	Time.cpp                                                         //
// Contents: Time Function													 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//


void __stdcall TimeManagerInstall(void);
void __stdcall StartTimeManager(void);
void __stdcall TimeManagerUpdate(void);
void __stdcall TimeManagerUninstall(void);
void __stdcall StartPauseTime(void);
void __stdcall EndPauseTime(void);

extern uint32_t WatchDogFrame;
extern uint32_t CurrentFrameNumber;
extern int64_t StartCount;
extern int64_t frequency;
extern int64_t StartOfRenderTime;
