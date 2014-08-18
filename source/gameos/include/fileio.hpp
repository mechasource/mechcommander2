//==========================================================================//
// File:	 FileIO.hpp														//
// Contents: file i/o routines												//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

void __stdcall Init_FileSystem(void);
void __stdcall Destory_FileSystem( bool NoErrors );




#if defined(LAB_ONLY)

//
// Debugging information
//
extern char FileInfo[32][MAX_PATH+32];
extern ULONG CurrentFileInfo;
#endif



//
// Memory mapped file structure
//
typedef struct _MemoryMappedFiles
{

	ULONG Magic;						// Identify this structure
	_MemoryMappedFiles*	pNext;			// Pointer to next structure
	HANDLE hFile;						// File handle
	HANDLE hFileMapping;				// Mapping handle
	PUCHAR  pFile;						// Pointer to start of data
	ULONG  Size;						// Size of data
	ULONG  RefCount;					// Reference count (number of times a file is opened)
	char   Name[MAX_PATH];				// Copy of the file name

} MemoryMappedFiles;

typedef struct gosFileStream
{
	gosEnum_FileWriteStatus m_writeEnabled;
	HANDLE m_hFile;
	char m_Filename[MAX_PATH];
	struct gosFileStream* pNext;

	gosFileStream( PCSTR FileName, gosEnum_FileWriteStatus fwstatus );
	~gosFileStream(void);

	ULONG Seek( int where, gosEnum_FileSeekType from_end );
	ULONG Read( void *buffer, ULONG length );
	ULONG Write( const void *buffer, ULONG length );

	ULONG BytesTransfered;
} gosFileStream;



extern MemoryMappedFiles*	ListofMemoryMappedFiles;
extern gosFileStream* ListOfFiles;
extern ULONG NumberMemoryMappedFilesOpen;
extern ULONG NumberFilesOpen;
