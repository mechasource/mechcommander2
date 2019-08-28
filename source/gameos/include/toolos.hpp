//===========================================================================//
// File:	 ToolOS.hpp														 //
// Contents: External API not for Game use, but for tools					 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _TOOLOS_HPP_
#define _TOOLOS_HPP_

// #include "gameos.hpp"

#ifndef MECH_IMPEXP
#define MECH_IMPEXP extern
#endif

//
// Returns the highest res timer possible (in seconds from start of game)
//
int64_t __stdcall gos_GetHiResTime(void);

//
// Creates a 'worker thread' - this API returns a handle to the thread.
//
// You must pass the address of the worker thread routine to this function.
//
uint32_t __stdcall gos_CreateThread(void(__stdcall* ThreadRoutine)(PVOID));

//
// Waits until the thread is not executing anymore and then deletes the thread
//
void __stdcall gos_DeleteThread(uint32_t ThreadHandle);

enum gosThreadPriority : uint32_t
{
	ThreadPri_Lowest = 1,
	ThreadPri_BelowNormal = 2,
	ThreadPri_Normal = 3,
	ThreadPri_AboveNormal = 4,
	ThreadPri_Highest = 5
};

//
// Set the thread priority
//
void __stdcall gos_SetThreadPriority(uint32_t ThreadHandle, gosThreadPriority Priority);

//
// Starts the created thread executing
//
// 'ThreadFinished' will be set when the thread has finished executing
// 'Context' will be passed to the thread function
//
void __stdcall gos_TriggerThread(uint32_t ThreadHandle, bool* ThreadFinished, PVOID Context);

//
//
// LZ Compression - returns length of compressed destination buffer
//
//
uint32_t __stdcall gos_LZCompress(uint8_t* dest, uint8_t* src, size_t srcLen, size_t destLen = 0);
//
//
// LZ Decompression routine, returns length to decompressed output in dest
// buffer
//
//
uint32_t __stdcall gos_LZDecompress(uint8_t* dest, uint8_t* src, size_t srcLen);

//
// Gets a pointer to text data in the windows clip board (nullptr=No text)
//
MECH_IMPEXP size_t __stdcall gos_GetClipboardText(PSTR Buffer, size_t BufferSize);

//
// Sets the windows clipboard to the current text string
//
MECH_IMPEXP void __stdcall gos_SetClipboardText(PSTR Text);

//
// Gets a list of valid drives, returned as a list of strings, ending in 0,0
//
// eg:  "a:\","c:\","d:\","",""
//
// Returns the length of the string
//
uint32_t __stdcall gos_GetValidDrives(PSTR Buffer, size_t buf_len);

//
// Returns the drive label for a root directory specified. eg:  "c:\" might
// return "DriveC"
//
bool __stdcall gos_GetDriveLabel(PSTR RootName, PSTR DriveLabel, size_t DriveLabelBufferLen);

//
// Get the space available on the drive specified, either "x:" or "x:\"
//
int64_t __stdcall gos_GetDriveFreeSpace(PSTR Path);

//
// Gets the current path
//
void __stdcall gos_GetCurrentPath(PSTR Buffer, size_t buf_len);

//
// Create directory. - Returns TRUE for sucess
//
bool __stdcall gos_CreateDirectory(PSTR FileName);
//
// Delete directory. - Returns TRUE for sucess  (Directories must be empty)
//
bool __stdcall gos_DeleteDirectory(PSTR DirectoryName);

//
// Rename file/directory. - Returns TRUE for sucess
//
bool __stdcall gos_RenameFile(PSTR FileNameFrom, PSTR FileNameTo);
//
// Delete file. - Returns TRUE for sucess
//
bool __stdcall gos_DeleteFile(PSTR FileName);

//
// Find files matching pattern - returns nullptr when no more files (Can be used
// too see if a directory is empty : ie: 0==gos_FindFiles( "\\graphics\\*.*" )
//
PSTR __stdcall gos_FindFiles(PSTR PathFileName);
//
// Continues the previous gos_FindFiles
//
PSTR __stdcall gos_FindFilesNext(void);
//
// Stop the previous gos_FindFiles
//
void __stdcall gos_FindFilesClose(void);

//
// Find directories matching pattern - returns nullptr when no more directories
//
PSTR __stdcall gos_FindDirectories(PSTR DirectoryName);
//
// Continues the previous gos_FindDirectoriesNext
//
PSTR __stdcall gos_FindDirectoriesNext(void);
//
// Stop the previous gos_FindDirectories
//
void __stdcall gos_FindDirectoriesClose(void);

//
// Return full path name of file - FullPath should point to a 256 byte buffer
//
void __stdcall gos_GetFullPathName(PSTR FullPath, PSTR FileName);

//
// Get file size information (-1 if error)
//
uint32_t __stdcall gos_FileSize(PSTR FileName);
//
// Get file date/time information (-1 if error) - this can be compared directly,
// and decoded using gos_FileTimeString
//
int64_t __stdcall gos_FileTimeStamp(PSTR FileName);
//
// Get current date/time information (only updates one per game logic)
//
int64_t __stdcall gos_GetTimeDate(void);
//
// Returns an ASCII time/date string from a file time
//
PSTR __stdcall gos_FileTimeString(int64_t Time);

//
// Get file read only attribute information
//
bool __stdcall gos_FileReadOnly(PSTR FileName);
//
// Set file to read/write
//
void __stdcall gos_FileSetReadWrite(PSTR FileName);
//
// Set file to read only
//
void __stdcall gos_FileSetReadOnly(PSTR FileName);

//////////////////////////////////////////////////////////////////////////////////
// Ways to specify how to open file.
enum gosEnum_FileWriteStatus : uint32_t
{
	READONLY, // Open for READ
	READWRITE, // Open for READ and WRITE - but clears the file on open
	APPEND // Open for READ and WRITE - leaves the file alone if it already exists
};

//////////////////////////////////////////////////////////////////////////////////
// Different ways to seek within a file.
enum gosEnum_FileSeekType : uint32_t
{
	FROMSTART,
	FROMCURRENT,
	FROMEND
};

//////////////////////////////////////////////////////////////////////////////////
// If the file or directory exists, return TRUE
bool __stdcall gos_DoesFileExist(PSTR FileName);

//////////////////////////////////////////////////////////////////////////////////
// Open the file found at <path> with the method <writeable> and return a handle
// hfile to it.
void __stdcall gos_OpenFile(HGOSFILE* hfile, PSTR path, gosEnum_FileWriteStatus);

//////////////////////////////////////////////////////////////////////////////////
// Close the file specified by handle <hfile>.
void __stdcall gos_CloseFile(HGOSFILE hfile);

//////////////////////////////////////////////////////////////////////////////////
// Read <size> bytes from the file specified by handle <hfile> into the buffer
// pointed to by <buf>.
uint32_t __stdcall gos_ReadFile(HGOSFILE hfile, PVOID buf, size_t size);

//////////////////////////////////////////////////////////////////////////////////
// Write <size> bytes to the file specified by handle <hfile> from the buffer
// pointed to by <buf>.
uint32_t __stdcall gos_WriteFile(HGOSFILE hfile, PCVOID buf, size_t size);

//////////////////////////////////////////////////////////////////////////////////
// Move the current file position in filestream <hfile> to offset <offset> using
// the seek type specified by <type>.
uint32_t __stdcall gos_SeekFile(HGOSFILE hfile, gosEnum_FileSeekType type, ptrdiff_t offset);

#endif
