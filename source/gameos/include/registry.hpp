//==========================================================================//
// File:	 Registry.cpp													//
// Contents: Registry routines												//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#include "string.hpp"

void __stdcall RegistryManagerInstall(void);
void __stdcall RegistryManagerUninstall(void);
PSTR __stdcall ReadRegistry(PSTR keyname, PSTR valuename, bool ishklm);
PSTR __stdcall ReadRegistryHKCU(PSTR keyname, PSTR valuename, bool ishklm);

extern uint32_t UpdatedExe; // Has the exe been updated since last run?
