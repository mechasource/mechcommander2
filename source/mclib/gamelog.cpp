//---------------------------------------------------------------------------
//
// gamelog.cpp - This file contains the class functions for GameLog
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//---------------------------------------------------------------------------
// Include files
#ifndef FILE_H
#include <file.h>
#endif

#ifndef GAMELOG_H
#include "gamelog.h"
#endif

#ifndef HEAP_H
#include <heap.h>
#endif

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gameos.hpp>

bool isSetup = false;

//*****************************************************************************

GameLogPtr GameLog::files[MAX_GAMELOGS] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

PVOID GameLog::operator new(size_t mySize)
{
	PVOID result = nullptr;
	result		 = gos_Malloc(mySize);
	return (result);
}

//---------------------------------------------------------------------------

void GameLog::operator delete(PVOID us) { gos_Free(us); }

//---------------------------------------------------------------------------

void GameLog::dump(void)
{
	//----------------
	// Dump to file...
	for (size_t i = 0; i < numLines; i++)
		filePtr->writeString(lines[i]);
	numLines = 0;
}

//---------------------------------------------------------------------------

void GameLog::close(void)
{
	if (filePtr && inUse)
	{
		dump();
		char s[512];
		sprintf(s, "\nNum Total Lines = %d\n", totalLines);
		filePtr->writeString(s);
		filePtr->close();
		inUse	  = false;
		numLines   = 0;
		totalLines = 0;
	}
}

//---------------------------------------------------------------------------

void GameLog::destroy(void) { close(); }

//---------------------------------------------------------------------------

int32_t GameLog::open(PSTR fileName)
{
	numLines   = 0;
	totalLines = 0;
	if (filePtr->create(fileName) != NO_ERROR)
		return (-1);
	inUse = true;
	return (0);
}

//---------------------------------------------------------------------------

void GameLog::write(PSTR s)
{
	static char buffer[MAX_GAMELOG_LINELEN];
	if (numLines == MAX_GAMELOG_LINES)
		dump();
	if (strlen(s) > (MAX_GAMELOG_LINELEN - 1))
		s[MAX_GAMELOG_LINELEN - 1] = nullptr;
	sprintf(buffer, "%s\n", s);
	strncpy(lines[numLines], buffer, MAX_GAMELOG_LINELEN - 1);
	numLines++;
	totalLines++;
}

//---------------------------------------------------------------------------

GameLog* GameLog::getNewFile(void)
{
	if (!isSetup)
		setup();
	int32_t fileHandle = -1;
	int32_t i;
	for (i = 0; i < MAX_GAMELOGS; i++)
		if (!files[i]->inUse)
		{
			fileHandle = i;
			break;
		}
	return (files[i]);
}

//---------------------------------------------------------------------------

void GameLog::setup(void)
{
	if (isSetup)
		return;
	isSetup = true;
	for (size_t i = 0; i < MAX_GAMELOGS; i++)
	{
		files[i] = new GameLog;
		files[i]->init();
		files[i]->handle  = i;
		files[i]->inUse   = false;
		files[i]->filePtr = new MechFile;
		gosASSERT(files[i]->filePtr != nullptr);
	}
}

//---------------------------------------------------------------------------

void GameLog::cleanup(void)
{
	if (!isSetup)
		return;
	for (size_t i = 0; i < MAX_GAMELOGS; i++)
	{
		if (files[i] && files[i]->inUse)
		{
			files[i]->close();
			files[i]->filePtr->close();
			delete files[i]->filePtr;
			files[i]->filePtr = nullptr;
			delete files[i];
			files[i] = nullptr;
		}
	}
}
