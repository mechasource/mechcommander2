//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//----------------------------------------------------------------------------
// Global Fast File Code.
//
#include "stdinc.h"
#include "heap.h"
#include "fastfile.h"
//#include <ctype.h>

int32_t ffLastError = 0;

#define NO_ERROR 0
//-----------------------------------------------------------------------------------
bool FastFileInit(PSTR fname)
{
	if (numFastFiles == maxFastFiles)
	{
		ffLastError = -1;
		return false;
	}
	//-----------------------------------------------------------------------------
	//-- Open this fast file, add it to the list O pointers and return TRUE if
	//OK!
	fastFiles[numFastFiles] = new FastFile;
	int32_t result			= fastFiles[numFastFiles]->open(fname);
	if (result == FASTFILE_VERSION)
	{
		ffLastError = result;
		return false;
	}
	numFastFiles++;
	return TRUE;
}

//-----------------------------------------------------------------------------------
void FastFileFini(void)
{
	if (fastFiles)
	{
		int32_t currentFastFile = 0;
		while (currentFastFile < maxFastFiles)
		{
			if (fastFiles[currentFastFile])
			{
				fastFiles[currentFastFile]->close();
				delete fastFiles[currentFastFile];
				fastFiles[currentFastFile] = nullptr;
			}
			currentFastFile++;
		}
	}
	free(fastFiles);
	fastFiles	= nullptr;
	numFastFiles = 0;
}

//-----------------------------------------------------------------------------------
FastFile* FastFileFind(PSTR fname, int32_t& fastFileHandle)
{
	if (fastFiles)
	{
		uint32_t thisHash		= elfHash(fname);
		int32_t currentFastFile = 0;
		int32_t tempHandle		= -1;
		while (currentFastFile < numFastFiles)
		{
			tempHandle = fastFiles[currentFastFile]->openFast(thisHash, fname);
			if (tempHandle != -1)
			{
				fastFileHandle = tempHandle;
				return fastFiles[currentFastFile];
			}
			currentFastFile++;
		}
	}
	return nullptr;
}

//------------------------------------------------------------------
uint32_t elfHash(PSTR name)
{
	uint32_t h = 0, g;
	while (*name)
	{
		h = (h << 4) + *name++;
		if ((g = h & 0xF0000000) != 0)
			h ^= g >> 24;
		h &= ~g;
	}
	return h;
}

//-----------------------------------------------------------------------------------
