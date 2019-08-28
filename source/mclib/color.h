//******************************************************************************************
//	color.h - This file contains the definitions for the colour Tables
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef COLOR_H
#define COLOR_H
//----------------------------------------------------------------------------------
// Include Files
#ifndef DSTD_H
#include "dstd.h"
#endif

//----------------------------------------------------------------------------------
void
initcolourTables(void);

void
destroycolourTables(void);

uint32_t*
getcolourTable(int32_t tableNum);
//----------------------------------------------------------------------------------
#endif