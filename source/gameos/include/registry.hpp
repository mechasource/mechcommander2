//==========================================================================//
// File:	 Registry.cpp													//
// Contents: Registry routines												//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#pragma once

#include "String.hpp"

void __stdcall RegistryManagerInstall(void);
void __stdcall RegistryManagerUninstall(void);
PSTR __stdcall ReadRegistry( PSTR KeyName, PSTR ValueName, bool LocalMachine );
PSTR __stdcall ReadRegistryHKCU( PSTR KeyName, PSTR ValueName, bool LocalMachine );

extern uint32_t UpdatedExe;					// Has the exe been updated since last run?

