//---------------------------------------------------------------------------
//
// ffile.cpp - This file contains the class functions for FastFile
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"

#ifndef FFILE_H
#include "ffile.h"
#endif

#ifndef UTILITIES_H
#include "utilities.h"
#endif

#include "lz.h"
#include <zlib.h>
// #include <windows.h>
// #include <string.h>
#include <gameos.hpp>

puint8_t 		LZPacketBuffer = nullptr;
size_t			LZPacketBufferSize = 512000;

extern char CDInstallPath[];
void __stdcall EnterWindowMode();
void __stdcall EnterFullScreenMode();
void __stdcall ExitGameOS();

char FileMissingString[512];
char CDMissingString[1024];
char MissingTitleString[256];
//---------------------------------------------------------------------------
//	class FastFile member functions
//---------------------------------------------------------------------------
PVOIDFastFile::operator new(size_t mySize)
{
	PVOID result = nullptr;
	result = malloc(mySize);
	return(result);
}

//---------------------------------------------------------------------------
void FastFile::operator delete(PVOID us)
{
	::free(us);
}

//---------------------------------------------------------------------------
FastFile::FastFile(void)
{
	files = nullptr;
	numFiles = 0;
	fileName = nullptr;
	handle = nullptr;
	length = 0;
	logicalPosition = 0;
	useLZCompress = false;
}

//---------------------------------------------------------------------------
FastFile::~FastFile(void)
{
	close();
	if(LZPacketBuffer)
	{
		free(LZPacketBuffer);
		LZPacketBuffer = nullptr;
	}
}

//---------------------------------------------------------------------------
int32_t FastFile::open(PSTR fName)
{
	//-------------------------------------------------------------
	int32_t fNameLength = strlen(fName);
	fileName = new char [fNameLength + 1];
	if(!fileName)
		return(NO_RAM_FOR_FILENAME);
	strncpy(fileName, fName, fNameLength + 1);
	handle = fopen(fileName, "r");
	if(handle != nullptr)
	{
		logicalPosition = 0;
		fileSize();				//Sets Length
	}
	else
	{
		//Check on CD.  They may not have been installed.
		char actualPath[2048];
		strcpy(actualPath, CDInstallPath);
		strcat(actualPath, fileName);
		handle = fopen(actualPath, "r");
		if(handle == nullptr)
		{
			//OPEN Error.  Maybe the CD is missing?
			bool openFailed = false;
			bool alreadyFullScreen = (Environment.fullScreen != 0);
			while(handle == nullptr)
			{
				openFailed = true;
				EnterWindowMode();
				char data[2048];
				sprintf(data, FileMissingString, fileName, CDMissingString);
				uint32_t result1 = MessageBox(nullptr, data, MissingTitleString, MB_OKCANCEL | MB_ICONWARNING);
				if(result1 == IDCANCEL)
				{
					ExitGameOS();
					return (2);		//File not found.  Never returns though!
				}
				handle = fopen(actualPath, "r");
			}
			if(openFailed && (Environment.fullScreen == 0) && alreadyFullScreen)
				EnterFullScreenMode();
		}
		else
		{
			logicalPosition = 0;
			fileSize();				//Sets Length
		}
	}
	//---------------------------------------------
	//-- Read in ALL of the file names and offsets
	logicalPosition = fseek(handle, 0, SEEK_SET);
	//---------------------------------------------
	//-- First Long is Version Number of FastFile
	uint32_t result = 0;
	uint32_t version = 0;
	result = fread((&version), 1, sizeof(int32_t), handle);
	logicalPosition += sizeof(int32_t);
	if(result != sizeof(int32_t))
	{
		int32_t lastError = errno;
		return lastError;
	}
	if(version != FASTFILE_VERSION && version != FASTFILE_VERSION_LZ)
		return FASTFILE_VERSION;
	if(version == FASTFILE_VERSION_LZ)
		useLZCompress = true;
	//---------------------------------------------
	//-- Second Long is number of filenames present.
	result = fread((&numFiles), 1, sizeof(int32_t), handle);
	logicalPosition += sizeof(int32_t);
	if(result != sizeof(int32_t))
	{
		int32_t lastError = errno;
		return lastError;
	}
	files = (FILE_HANDLE*)malloc(sizeof(FILE_HANDLE) * numFiles);
	for(size_t i = 0; i < numFiles; i++)
	{
		files[i].pfe = (FILEENTRY*)malloc(sizeof(FILEENTRY));
		memset(files[i].pfe, 0, sizeof(FILEENTRY));
		uint32_t result = 0;
		result = fread(files[i].pfe, 1, sizeof(FILEENTRY), handle);
		files[i].inuse = FALSE;
		files[i].pos = 0;
	}
	return (result);
}

//---------------------------------------------------------------------------
void FastFile::close(void)
{
	//------------------------------------------------------------------------
	// The actual stored filename is also in the parent.  Everyone else just has
	// pointer and, as such, only the parent frees the memory.
	if(fileName)
		delete [] fileName;	//	this was free, which didn't match the new allocation.
	//	neither new nor free were overridden. Should they have been?
	fileName = nullptr;
	length = 0;
	if(isOpen())
	{
		fclose(handle);
		handle = nullptr;
	}
	//---------------------------------------------
	//-- First Long is number of filenames present.
	for(size_t i = 0; i < numFiles; i++)
	{
		free(files[i].pfe);
	}
	free(files);
	files = nullptr;
	numFiles = 0;
}

//---------------------------------------------------------------------------
int32_t FastFile::openFast(uint32_t hash, PSTR fName)
{
	//------------------------------------------------------------------
	//-- In order to use this, the file name must be part of the index.
	for(size_t i = 0; i < numFiles; i++)
	{
		if((hash == files[i].pfe->hash) && (_stricmp(files[i].pfe->name, fName) == 0))
		{
			files[i].inuse = TRUE;
			files[i].pos = 0;
			return i;
		}
	}
	return -1;
}

//---------------------------------------------------------------------------
void FastFile::closeFast(int32_t fastFileHandle)
{
	if((fastFileHandle >= 0) && (fastFileHandle < numFiles) && files[fastFileHandle].inuse)
	{
		files[fastFileHandle].inuse = FALSE;
		files[fastFileHandle].pos = 0;
	}
}

//---------------------------------------------------------------------------
int32_t FastFile::seekFast(int32_t fastFileHandle, int32_t off, int32_t how)
{
	if((fastFileHandle >= 0) && (fastFileHandle < numFiles) && files[fastFileHandle].inuse)
	{
		switch(how)
		{
			case SEEK_SET:
				if(off > files[fastFileHandle].pfe->size)
				{
					return READ_PAST_EOF_ERR;
				}
				break;
			case SEEK_END:
				if((abs(off) > files[fastFileHandle].pfe->size) || (off > 0))
				{
					return READ_PAST_EOF_ERR;
				}
				break;
			case SEEK_CUR:
				if(off + files[fastFileHandle].pos > files[fastFileHandle].pfe->size)
				{
					return READ_PAST_EOF_ERR;
				}
				break;
		}
		int32_t newPosition = 0;
		switch(how)
		{
			case SEEK_SET:
				newPosition = off;
				break;
			case SEEK_END:
				newPosition = files[fastFileHandle].pfe->size;
				newPosition += off;
				break;
			case SEEK_CUR:
				newPosition = off + files[fastFileHandle].pos;
				break;
		}
		if(newPosition == -1)
		{
			return (INVALID_SEEK_ERR);
		}
		files[fastFileHandle].pos = newPosition;
		//-----------------------------------
		//-- Now macro seek the entire file.
		if(fseek(handle, files[fastFileHandle].pos + files[fastFileHandle].pfe->offset, SEEK_SET) == 0)
			logicalPosition = ftell(handle);
		return (files[fastFileHandle].pos);
	}
	return (FILE_NOT_OPEN);
}

//---------------------------------------------------------------------------
int32_t FastFile::readFast(int32_t fastFileHandle, PVOIDbfr, int32_t size)
{
	size;
	int32_t result = 0;
	if((fastFileHandle >= 0) && (fastFileHandle < numFiles) && files[fastFileHandle].inuse)
	{
		logicalPosition = fseek(handle, files[fastFileHandle].pos + files[fastFileHandle].pfe->offset, SEEK_SET);
		//ALL files in the fast file are now zLib compressed. NO EXCEPTIONS!!
		// This fixes a bug where the zLib Compressed version is the same length
		// as the raw version.  Yikes but this is rare.  Finally happened though!
		// -fs
		/*
		if (files[fastFileHandle].pfe->size == files[fastFileHandle].pfe->realSize)
		{
			result = fread(bfr,1,size,handle);
			logicalPosition += size;

			if (result != size)
			{
				int32_t lastError = errno;
				return lastError;
			}
		}
		else			//File is NOW zLib Compressed.  Read In Appropriately
		*/
		{
			if(!LZPacketBuffer)
			{
				LZPacketBuffer = (puint8_t)malloc(LZPacketBufferSize);
				if(!LZPacketBuffer)
					return 0;
			}
			if((int32_t)LZPacketBufferSize < files[fastFileHandle].pfe->size)
			{
				LZPacketBufferSize = files[fastFileHandle].pfe->size;
				free(LZPacketBuffer);
				LZPacketBuffer = (puint8_t)malloc(LZPacketBufferSize);
				if(!LZPacketBuffer)
					return 0;
			}
			if(LZPacketBuffer)
			{
				result = fread(LZPacketBuffer, 1, files[fastFileHandle].pfe->size, handle);
				logicalPosition += files[fastFileHandle].pfe->size;
				if(result != files[fastFileHandle].pfe->size)
				{
					//READ Error.  Maybe the CD is missing?
					bool openFailed = false;
					bool alreadyFullScreen = (Environment.fullScreen != 0);
					while(result != files[fastFileHandle].pfe->size)
					{
						openFailed = true;
						EnterWindowMode();
						char data[2048];
						sprintf(data, FileMissingString, fileName, CDMissingString);
						uint32_t result1 = MessageBox(nullptr, data, MissingTitleString, MB_OKCANCEL | MB_ICONWARNING);
						if(result1 == IDCANCEL)
						{
							ExitGameOS();
							return (2);		//File not found.  Never returns though!
						}
						logicalPosition = fseek(handle, files[fastFileHandle].pos + files[fastFileHandle].pfe->offset, SEEK_SET);
						result = fread(LZPacketBuffer, 1, files[fastFileHandle].pfe->size, handle);
						logicalPosition += files[fastFileHandle].pfe->size;
					}
					if(openFailed && (Environment.fullScreen == 0) && alreadyFullScreen)
						EnterFullScreenMode();
				}
				//--------------------------------------------------------
				//USED to LZ Compress here.  It is NOW zLib Compression.
				//  We should not try to use old fastfiles becuase version check above should fail when trying to open!!
				size_t decompLength = 0;
				if(useLZCompress)
				{
					decompLength = LZDecomp((puint8_t)bfr, LZPacketBuffer, files[fastFileHandle].pfe->size);
				}
				else
				{
					decompLength = files[fastFileHandle].pfe->realSize;
					int32_t error = uncompress((puint8_t)bfr, &decompLength, LZPacketBuffer, files[fastFileHandle].pfe->size);
					if(error != Z_OK)
						STOP(("Error %d UnCompressing File %s from FastFile %s", error, files[fastFileHandle].pfe->name, fileName));
				}
				if((int32_t)decompLength != files[fastFileHandle].pfe->realSize)
					result = 0;
				else
					result = decompLength;
			}
		}
		return result;
	}
	return FILE_NOT_OPEN;
}

//---------------------------------------------------------------------------
// This function pulls the raw compressed data out of the file and sticks it in the buffer
// passed in.  This way, we can load the textures directly from file to RAM and not
// have to decompress them!!
int32_t FastFile::readFastRAW(int32_t fastFileHandle, PVOIDbfr, int32_t size)
{
	size;
	int32_t result = 0;
	if((fastFileHandle >= 0) && (fastFileHandle < numFiles) && files[fastFileHandle].inuse)
	{
		logicalPosition = fseek(handle, files[fastFileHandle].pos + files[fastFileHandle].pfe->offset, SEEK_SET);
		if(size >= files[fastFileHandle].pfe->size)
		{
			result = fread(bfr, 1, files[fastFileHandle].pfe->size, handle);
			logicalPosition += files[fastFileHandle].pfe->size;
		}
		if(result != files[fastFileHandle].pfe->size)
		{
			//READ Error.  Maybe the CD is missing?
			bool openFailed = false;
			bool alreadyFullScreen = (Environment.fullScreen != 0);
			while(result != files[fastFileHandle].pfe->size)
			{
				openFailed = true;
				EnterWindowMode();
				char data[2048];
				sprintf(data, FileMissingString, fileName, CDMissingString);
				uint32_t result1 = MessageBox(nullptr, data, MissingTitleString, MB_OKCANCEL | MB_ICONWARNING);
				if(result1 == IDCANCEL)
				{
					ExitGameOS();
					return (2);		//File not found.  Never returns though!
				}
				logicalPosition = fseek(handle, files[fastFileHandle].pos + files[fastFileHandle].pfe->offset, SEEK_SET);
				result = fread(bfr, 1, files[fastFileHandle].pfe->size, handle);
				logicalPosition += files[fastFileHandle].pfe->size;
			}
			if(openFailed && (Environment.fullScreen == 0) && alreadyFullScreen)
				EnterFullScreenMode();
		}
		return result;
	}
	return FILE_NOT_OPEN;
}

//---------------------------------------------------------------------------
int32_t FastFile::tellFast(int32_t fastFileHandle)
{
	if((fastFileHandle >= 0) && (fastFileHandle < numFiles) && files[fastFileHandle].inuse)
		return files[fastFileHandle].pos;
	return -1;
}

//---------------------------------------------------------------------------
int32_t FastFile::sizeFast(int32_t fastFileHandle)
{
	if((fastFileHandle >= 0) && (fastFileHandle < numFiles) && files[fastFileHandle].inuse)
		return files[fastFileHandle].pfe->realSize;
	return -1;
}

//---------------------------------------------------------------------------
int32_t FastFile::lzSizeFast(int32_t fastFileHandle)
{
	if((fastFileHandle >= 0) && (fastFileHandle < numFiles) && files[fastFileHandle].inuse)
		return files[fastFileHandle].pfe->size;
	return -1;
}

//---------------------------------------------------------------------------
//
// Edit Log
//
//
//---------------------------------------------------------------------------
