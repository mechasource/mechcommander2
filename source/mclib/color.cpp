//******************************************************************************************
//	color.cpp - This file contains the code for the Color Tables
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef PATHS_H
#include "paths.h"
#endif

#ifndef CIDENT_H
#include "cident.h"
#endif

#ifndef INIFILE_H
#include "inifile.h"
#endif

#ifndef HEAP_H
#include "heap.h"
#endif

#include <gameos.hpp>
//----------------------------------------------------------------------------------
ULONG **colorRGBLookup = NULL;
int32_t numColorRGBTables = 0;

#define MAX_COLOR_ENTRIES		56
//----------------------------------------------------------------------------------
void initColorTables (void)
{
	FullPathFileName colorPath;
	colorPath.init(cameraPath,"colors",".fit");

	FitIniFile colorFile;
	int32_t result = colorFile.open(colorPath);
	gosASSERT(result == NO_ERROR);

	result = colorFile.seekBlock("Main");
	gosASSERT(result == NO_ERROR);

	result = colorFile.readIdLong("NumTables",numColorRGBTables);
	gosASSERT(result == NO_ERROR);

	colorRGBLookup = (ULONG **)systemHeap->Malloc(sizeof(ULONG *) * numColorRGBTables);
	gosASSERT(colorRGBLookup != NULL);

	memset(colorRGBLookup,0,sizeof(ULONG *) * numColorRGBTables);

	for (int32_t i=0;i<numColorRGBTables;i++)
	{
		char tableBlock[256];
		sprintf(tableBlock,"Table%d",i);

		result = colorFile.seekBlock(tableBlock);
		gosASSERT(result == NO_ERROR);

		colorRGBLookup[i] = (ULONG *)systemHeap->Malloc(sizeof(ULONG) * MAX_COLOR_ENTRIES);
		gosASSERT(colorRGBLookup[i] != NULL);

		ULONG *table = colorRGBLookup[i];

		for (int32_t j=0;j<MAX_COLOR_ENTRIES;j++)
		{
			char colorBlock[256];
			sprintf(colorBlock,"Color%d",j);

			result = colorFile.readIdULong(colorBlock,table[j]);
			gosASSERT(result == NO_ERROR);
		}
	}

	colorFile.close();
}	

//----------------------------------------------------------------------------------
void destroyColorTables (void)
{
	for (int32_t i=0;i<numColorRGBTables;i++)
	{
		systemHeap->Free(colorRGBLookup[i]);
		colorRGBLookup[i] = NULL;
	}

	if ( colorRGBLookup )
		systemHeap->Free(colorRGBLookup);
	colorRGBLookup = NULL;
}	

//----------------------------------------------------------------------------------
ULONG *getColorTable (int32_t tableNum)
{
	if ((tableNum >= 0) && (tableNum < numColorRGBTables))
	{
		return(colorRGBLookup[tableNum]);
	}

	return(NULL);
}	

//----------------------------------------------------------------------------------
