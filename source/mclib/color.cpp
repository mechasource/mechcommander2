//******************************************************************************************
//	color.cpp - This file contains the code for the colour Tables
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

//#include "gameos.hpp"
//----------------------------------------------------------------------------------
uint32_t** colorRGBLookup = nullptr;
int32_t numcolourRGBTables = 0;

#define MAX_COLOR_ENTRIES 56
//----------------------------------------------------------------------------------
void initcolourTables(void)
{
	stdfs::path colorPath; // FullPathFileName colorPath;
	GetMcDataPath(colorPath, data_paths::cameraPath);
	colorPath.append(L"colors.fit");

	FitIniFile colorFile;
	int32_t result = colorFile.open(colorPath);
	gosASSERT(result == NO_ERROR);
	result = colorFile.seekBlock("Main");
	gosASSERT(result == NO_ERROR);
	result = colorFile.readIdLong("NumTables", numcolourRGBTables);
	gosASSERT(result == NO_ERROR);
	colorRGBLookup = (uint32_t**)systemHeap->Malloc(sizeof(uint32_t*) * numcolourRGBTables);
	gosASSERT(colorRGBLookup != nullptr);
	memset(colorRGBLookup, 0, sizeof(uint32_t*) * numcolourRGBTables);
	for (size_t i = 0; i < numcolourRGBTables; i++)
	{
		wchar_t tableBlock[256];
		sprintf(tableBlock, "Table%d", i);
		result = colorFile.seekBlock(tableBlock);
		gosASSERT(result == NO_ERROR);
		colorRGBLookup[i] = (uint32_t*)systemHeap->Malloc(sizeof(uint32_t) * MAX_COLOR_ENTRIES);
		gosASSERT(colorRGBLookup[i] != nullptr);
		uint32_t* table = colorRGBLookup[i];
		for (size_t j = 0; j < MAX_COLOR_ENTRIES; j++)
		{
			wchar_t colorBlock[256];
			sprintf(colorBlock, "colour%d", j);
			result = colorFile.readIdULong(colorBlock, table[j]);
			gosASSERT(result == NO_ERROR);
		}
	}
	colorFile.close();
}

//----------------------------------------------------------------------------------
void destroycolourTables(void)
{
	for (size_t i = 0; i < numcolourRGBTables; i++)
	{
		systemHeap->Free(colorRGBLookup[i]);
		colorRGBLookup[i] = nullptr;
	}
	if (colorRGBLookup)
		systemHeap->Free(colorRGBLookup);
	colorRGBLookup = nullptr;
}

//----------------------------------------------------------------------------------
uint32_t*
getcolourTable(int32_t tableNum)
{
	if ((tableNum >= 0) && (tableNum < numcolourRGBTables))
	{
		return (colorRGBLookup[tableNum]);
	}
	return (nullptr);
}

//----------------------------------------------------------------------------------
