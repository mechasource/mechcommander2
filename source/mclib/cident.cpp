//---------------------------------------------------------------------------
//
// cident.cpp - This file contains some misc string classes
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//---------------------------------------------------------------------------
// Include files
#include "stdafx.h"
// #include <ctype.h>
// #include "windows.h"

#ifndef CIDENT_H
#include "cident.h"
#endif

#include "heap.h"

#ifndef _MBCS
#include <gameos.hpp>
#else
#include <assert.h>
#define gosASSERT assert
#define gos_Malloc malloc
#define gos_Free free
#endif

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//								FullPathFileName
//---------------------------------------------------------------------------
FullPathFileName::~FullPathFileName (void)
{
	destroy();
}

//---------------------------------------------------------------------------
void FullPathFileName::destroy (void)
{
	if (fullName)
		systemHeap->Free(fullName);

	fullName = NULL;
}

//---------------------------------------------------------------------------
void FullPathFileName::init (PSTR  dir_path, PCSTR  name, PSTR  ext)
{
	destroy();

	size_t total_length = strlen(dir_path);
	total_length += strlen(name);
	total_length += strlen(ext);
	total_length++;


	fullName = (PSTR )systemHeap->Malloc(total_length);
	gosASSERT(fullName != NULL);
	if (fullName == NULL)
		return;
	fullName[0] = 0;

	if ( strstr( name, dir_path ) != name )
		strcpy_s(fullName, total_length, dir_path);

	if (name)
		strcat_s(fullName, total_length, name);

	// don't append if its already there
	if (ext && _stricmp( fullName + strlen( fullName ) - strlen( ext ), ext ) != 0)
		strcat_s(fullName, total_length, ext);

	// CharLowerA(fullName);
	_strlwr_s(fullName, total_length);
}

void FullPathFileName::changeExt (PSTR from, PSTR to)
{
	if (strlen(from) != strlen(to))
		return;

	PSTR ext = strstr(fullName,from);
	if (ext)
	{
		for (ULONG i=0;i<strlen(to);i++)
			ext[i] = to[i];
	}
}
//---------------------------------------------------------------------------
//
// Edit Log
//
// $Log$
//
//---------------------------------------------------------------------------
