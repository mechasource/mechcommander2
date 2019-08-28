//---------------------------------------------------------------------------
//
// cident.cpp - This file contains some misc string classes
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//---------------------------------------------------------------------------
// Include files
#include "stdinc.h"
// #include <ctype.h>
// #include "windows.h"
// #include <string.h>

#include "dstd.h"
#include "dident.h"
#include "cident.h"
#include "heap.h"

#ifndef _MBCS
//#include "gameos.hpp"
#else
#include <_ASSERT.h>
#define gosASSERT _ASSERT
#define gos_Malloc malloc
#define gos_Free free
#endif

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//								FullPathFileName
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void
FullPathFileName::destroy(void)
{
	if (fullName)
		systemHeap->Free(fullName);
	fullName = nullptr;
}

//---------------------------------------------------------------------------
void
FullPathFileName::init(const std::wstring_view& dir_path, const std::wstring_view& name, const std::wstring_view& ext)
{
	destroy();
	size_t total_length = strlen(dir_path);
	total_length += strlen(name);
	total_length += strlen(ext);
	total_length++;
	fullName = (const std::wstring_view&)systemHeap->Malloc(total_length);
	gosASSERT(fullName != nullptr);
	if (fullName == nullptr)
		return;
	fullName[0] = 0;
	if (strstr(name, dir_path) != name)
		strcpy_s(fullName, total_length, dir_path);
	if (name)
		strcat_s(fullName, total_length, name);
	// don't append if its already there
	if (ext && _stricmp(fullName + strlen(fullName) - strlen(ext), ext) != 0)
		strcat_s(fullName, total_length, ext);
	// CharLowerA(fullName);
	_strlwr_s(fullName, total_length);
}

void
FullPathFileName::changeExt(const std::wstring_view& from, const std::wstring_view& to)
{
	if (strlen(from) != strlen(to))
		return;
	const std::wstring_view& ext = strstr(fullName, from);
	if (ext)
	{
		for (uint32_t i = 0; i < strlen(to); i++)
			ext[i] = to[i];
	}
}
