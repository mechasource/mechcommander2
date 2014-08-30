//---------------------------------------------------------------------------
//
// ffile.h - This file contains the class declaration for FastFile
//
// This class will catch a fileOpen and try to find the file in the FastFiles
// open before allowing the call to go through to the system
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef FFILE_H
#define FFILE_H

//---------------------------------------------------------------------------
// Include files

//#include <dstd.h>
//#include <file.h>
//#include <sys/types.h>
//#include <sys/stat.h>

//---------------------------------------------------------------------------

#define FASTFILE_VERSION		0xCADDECAF
#define FASTFILE_VERSION_LZ		0xFADDECAF

#pragma pack(1)
typedef struct 
{
    ptrdiff_t	offset;
    size_t		size;						//LZ Compressed Size
	size_t		realSize;					//Uncompressed Size
	uint32_t	hash;						//Hash Compare to weed out stinky files faster then StrCmp
    char		name[MAX_PATH];
} FILEENTRY;
#pragma pack()

typedef struct FILE_HANDLE {
    int32_t		inuse;
    int32_t		pos;
    FILEENTRY*	pfe;
} FILE_HANDLE;

//---------------------------------------------------------------------------
// Class FastFile
class FastFile
{
	protected:
		size_t			numFiles;
		FILE_HANDLE*	files;
		char*			fileName;
		FILE*			handle;
		size_t 			length;
		size_t 			logicalPosition;
		bool			useLZCompress;

	public:
		FastFile (void);
		~FastFile (void);

		PVOID operator new (size_t mySize);
		void operator delete (PVOID us);

		int32_t open (PSTR fName);
		void close (void);

		bool isOpen (void)
		{
			return (handle != nullptr);
		}

		size_t fileSize (void)
		{
			if (isOpen() && (length == 0))
			{
				struct _stat st;
				_stat(fileName, &st);
				length = size_t(st.st_size);
			}
			return length;			
		}

		size_t getNumFiles (void)
		{
			return numFiles;
		}

		int32_t openFast (uint32_t hash, PSTR fName);

		void closeFast (int32_t localHandle);

		int32_t seekFast (int32_t fastFileHandle, int32_t off, int32_t from = SEEK_SET);
		int32_t readFast (int32_t fastFileHandle, PVOID bfr, int32_t size);
		int32_t readFastRAW (int32_t fastFileHandle, PVOID bfr, int32_t size);
		int32_t tellFast (int32_t fastFileHandle);
		int32_t sizeFast (int32_t fastFileHandle);
		int32_t lzSizeFast (int32_t fastFileHandle);

		bool isLZCompressed (void)
		{
			return useLZCompress;
		}
};

//---------------------------------------------------------------------------
extern FastFile**	fastFiles;
extern size_t		numFastFiles;
extern size_t		maxFastFiles;
//---------------------------------------------------------------------------
#endif

