//---------------------------------------------------------------------------
//
// gamelog.h - This file contains the class declaration for GameLog
//
//				The Log File class allows easy dumping to a log file
//				from your application. It handles the string buffer (to
//				prevent writing to file everytime you call it).
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef GAMELOG_H
#define GAMELOG_H
//---------------------------------------------------------------------------
// Include files

//#include "dstd.h"
//#include "dfile.h"
//#include "dgamelog.h"

//---------------------------------------------------------------------------

#define MAX_GAMELOGS 6
#define MAX_GAMELOG_LINES 50
#define MAX_GAMELOG_LINELEN 200
#define MAXLEN_GAMELOG_NAME 128

class GameLog
{

public:
	int32_t handle;
	bool inUse;
	wchar_t fileName[MAXLEN_GAMELOG_NAME];
	MechFile* pfile;
	int32_t numLines;
	int32_t totalLines;
	wchar_t lines[MAX_GAMELOG_LINES][MAX_GAMELOG_LINELEN];

	static GameLogPtr files[MAX_GAMELOGS];

public:
	PVOID operator new(size_t mySize);

	void operator delete(PVOID us);

	void init(void)
	{
		handle = -1;
		inUse = false;
		fileName[0] = nullptr;
		pfile = nullptr;
		numLines = 0;
		totalLines = 0;
		for (size_t i = 0; i < MAX_GAMELOG_LINES; i++)
			lines[i][0] = nullptr;
	}

	GameLog(void) { init(void); }

	void destroy(void);

	~GameLog(void) { destroy(void); }

	void dump(void);

	void close(void);

	int32_t open(const std::wstring_view& fileName);

	void write(const std::wstring_view& s);

	static void setup(void);

	static void cleanup(void);

	static GameLogPtr getNewFile(void);
};

//---------------------------------------------------------------------------
#endif
