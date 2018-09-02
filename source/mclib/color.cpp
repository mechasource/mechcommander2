//******************************************************************************************
//	color.cpp - This file contains the code for the Color Tables
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

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
puint32_t* colorRGBLookup = nullptr;
int32_t numColorRGBTables = 0;

#define MAX_COLOR_ENTRIES 56
//----------------------------------------------------------------------------------
void initColorTables(void)
{
	stdfs::path colorPath;		// FullPathFileName colorPath;
	GetMcDataPath(colorPath, data_paths::cameraPath);
	colorPath.append(L"colors.fit");

	FitIniFile colorFile;
	int32_t result = colorFile.open(colorPath);
	gosASSERT(result == NO_ERROR);
	result = colorFile.seekBlock("Main");
	gosASSERT(result == NO_ERROR);
	result = colorFile.readIdLong("NumTables", numColorRGBTables);
	gosASSERT(result == NO_ERROR);
	colorRGBLookup =
		(puint32_t*)systemHeap->Malloc(sizeof(uint32_t*) * numColorRGBTables);
	gosASSERT(colorRGBLookup != nullptr);
	memset(colorRGBLookup, 0, sizeof(uint32_t*) * numColorRGBTables);
	for (size_t i = 0; i < numColorRGBTables; i++)
	{
		char tableBlock[256];
		sprintf(tableBlock, "Table%d", i);
		result = colorFile.seekBlock(tableBlock);
		gosASSERT(result == NO_ERROR);
		colorRGBLookup[i] =
			(uint32_t*)systemHeap->Malloc(sizeof(uint32_t) * MAX_COLOR_ENTRIES);
		gosASSERT(colorRGBLookup[i] != nullptr);
		uint32_t* table = colorRGBLookup[i];
		for (size_t j = 0; j < MAX_COLOR_ENTRIES; j++)
		{
			char colorBlock[256];
			sprintf(colorBlock, "Color%d", j);
			result = colorFile.readIdULong(colorBlock, table[j]);
			gosASSERT(result == NO_ERROR);
		}
	}
	colorFile.close();
}

//----------------------------------------------------------------------------------
void destroyColorTables(void)
{
	for (size_t i = 0; i < numColorRGBTables; i++)
	{
		systemHeap->Free(colorRGBLookup[i]);
		colorRGBLookup[i] = nullptr;
	}
	if (colorRGBLookup)
		systemHeap->Free(colorRGBLookup);
	colorRGBLookup = nullptr;
}

//----------------------------------------------------------------------------------
uint32_t* getColorTable(int32_t tableNum)
{
	if ((tableNum >= 0) && (tableNum < numColorRGBTables))
	{
		return (colorRGBLookup[tableNum]);
	}
	return (nullptr);
}

//----------------------------------------------------------------------------------
