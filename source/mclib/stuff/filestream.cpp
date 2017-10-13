//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"
//#include "stuffheaders.hpp"

#include <gameos.hpp>
#include <toolos.hpp>
#include <stuff/filestream.hpp>

using namespace Stuff;

//#############################################################################
//############################# Directory ###############################
//#############################################################################

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Directory::Directory(PSTR find_files, bool directories)
	: fileEntries(nullptr, nullptr), folderEntries(nullptr, nullptr)
{
	Check_Pointer(find_files);
	//
	//------------------------------------------------
	// Isolate the search path that we are looking for
	//------------------------------------------------
	//
	MString new_directory_string = find_files;
	//
	//---------------------------------------------------------
	// Now look for the files and add each entry into the chain
	//---------------------------------------------------------
	//
	PCSTR file_name = gos_FindFiles(new_directory_string);
	while(file_name)
	{
		MString file_name_string = file_name;
		DirectoryEntry* entry = new DirectoryEntry(file_name_string);
		Check_Object(entry);
		fileEntries.AddValue(entry, file_name_string);
		file_name = gos_FindFilesNext();
	}
	gos_FindFilesClose();
	fileWalker = new SortedChainIteratorOf<DirectoryEntry*, MString>(&fileEntries);
	Check_Object(fileWalker);
	//
	//---------------------------------------------------
	// Look through the directories if we are supposed to
	//---------------------------------------------------
	//
	folderWalker = nullptr;
	if(directories)
	{
		file_name = gos_FindDirectories(new_directory_string);
		while(file_name)
		{
			MString file_name_string = file_name;
			DirectoryFolder* entry = new DirectoryFolder(file_name_string);
			Check_Object(entry);
			folderEntries.AddValue(entry, file_name_string);
			file_name = gos_FindDirectoriesNext();
		}
		gos_FindDirectoriesClose();
		folderWalker = new SortedChainIteratorOf<DirectoryFolder*, MString>(&folderEntries);
		Check_Object(folderWalker);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Directory::~Directory(void)
{
	// Check_Object(this);
	Check_Object(fileWalker);
	fileWalker->DeletePlugs();
	Check_Object(fileWalker);
	delete fileWalker;
	if(folderWalker != nullptr)
	{
		Check_Object(folderWalker);
		folderWalker->DeletePlugs();
		Check_Object(folderWalker);
		delete folderWalker;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
PSTR Directory::GetCurrentFileName(void)
{
	// Check_Object(this);
	Check_Object(fileWalker);
	DirectoryEntry* entry;
	if((entry = fileWalker->GetCurrent()) != nullptr)
	{
		Check_Object(entry);
		return entry->GetItem();
	}
	return nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Directory::AdvanceCurrentFile(void)
{
	// Check_Object(this);
	Check_Object(fileWalker);
	fileWalker->Next();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
PSTR Directory::GetCurrentFolderName(void)
{
	// Check_Object(this);
	if(folderWalker == nullptr)
		STOP(("Directory class was instantiated without directory support:\n Set the <directories> parameter to true to enable."));
	Check_Object(folderWalker);
	DirectoryFolder* entry;
	if((entry = folderWalker->GetCurrent()) != nullptr)
	{
		Check_Object(entry);
		return entry->GetItem();
	}
	return nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Directory::AdvanceCurrentFolder(void)
{
	// Check_Object(this);
	if(folderWalker == nullptr)
		STOP(("Directory class was instantiated without directory support:\n Set the <directories> parameter to true to enable."));
	Check_Object(folderWalker);
	folderWalker->Next();
}

//#############################################################################
//########################### FileStream ################################
//#############################################################################

FileStream::ClassData* FileStream::DefaultData = nullptr;

PCSTR FileStream::WhiteSpace = " \t\n";
char FileStream::RedirectedName[256];
bool FileStream::IsRedirected = false;
bool FileStream::IgnoreReadOnlyFlag = false;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
FileStream::InitializeClass(void)
{
	Verify(!DefaultData);
	DefaultData =
		new ClassData(
		FileStreamClassID,
		"Stuff::FileStream",
		MemoryStream::DefaultData
	);
	Check_Object(DefaultData);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
FileStream::TerminateClass(void)
{
	Check_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
FileStream::TestInstance(void) const
{
	Verify(IsDerivedFrom(DefaultData));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
FileStream::FileStream():
	MemoryStream(DefaultData, nullptr, 0)
{
	fileName = nullptr;
	isOpen = false;
	fileHandle = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
FileStream::FileStream(
	PCSTR file_name,
	WriteStatus writable
):
	MemoryStream(DefaultData, nullptr, 0)
{
	fileName = nullptr;
	isOpen = false;
	Open(file_name, writable);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
FileStream::~FileStream(void)
{
	Close();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
FileStream::Open(
	PCSTR file_name,
	WriteStatus writable
)
{
	// Check_Object(this);
	Check_Pointer(file_name);
	writeEnabled = writable;
	currentPosition = streamStart = nullptr;
	streamSize = 0;
	fileHandle = nullptr;
	//
	//------------------------------------------------
	// If this is a readonly file, have gos read it in
	//------------------------------------------------
	//
	if(writeEnabled == ReadOnly)
	{
		if(IsRedirected)
		{
			void (__stdcall * old_hook)(PCSTR, puint8_t*, uint32_t*) = Environment.HookGetFile;
			Environment.HookGetFile = nullptr;
			gos_GetFile(RedirectedName, &streamStart, &streamSize);
			Environment.HookGetFile = old_hook;
			fileName = _strdup(RedirectedName);
		}
		else
		{
			gos_GetFile(file_name, &streamStart, &streamSize);
			if(IsRedirected)
				fileName = _strdup(RedirectedName);
			else
				fileName = _strdup(file_name);
		}
		isOpen = true;
		currentPosition = streamStart;
	}
	//
	//-----------------------------------------------
	// Write only flags go through standard file open
	//-----------------------------------------------
	//
	else
	{
		writeEnabled = writable;
		if(IgnoreReadOnlyFlag)
		{
			uint8_t (__stdcall * old_hook)(PCSTR) = Environment.HookDoesFileExist;
			Environment.HookDoesFileExist = nullptr;
			if(gos_DoesFileExist(file_name))
				gos_FileSetReadWrite(file_name);
			Environment.HookDoesFileExist = old_hook;
		}
		gos_OpenFile(
			&fileHandle,
			file_name,
			READWRITE
		);
		isOpen = true;
		streamSize = 0xFFFFFFFFU;
		fileName = _strdup(file_name);
	}
	//
	//--------------------------
	// Try and open the filename
	//--------------------------
	//
	IsRedirected = false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
FileStream::Close(void)
{
	// Check_Object(this);
	//
	//---------------------------------
	// If the file was opened, close it
	//---------------------------------
	//
	if(fileHandle)
		gos_CloseFile(fileHandle);
	else if(streamStart != nullptr)
		gos_Free(streamStart);
	//
	//----------------
	// Delete the name
	//----------------
	//
	isOpen = false;
	if(fileName)
	{
		Check_Pointer(fileName);
		free(fileName);
		fileName = nullptr;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
FileStream::SetPointer(size_t index)
{
	// Check_Object(this);
	Verify(IsFileOpened());
	Verify(writeEnabled == ReadOnly);
	MemoryStream::SetPointer(index);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MemoryStream&
FileStream::AdvancePointer(size_t index)
{
	// Check_Object(this);
	Verify(IsFileOpened());
	Verify(writeEnabled == ReadOnly);
	return MemoryStream::AdvancePointer(index);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MemoryStream&
FileStream::RewindPointer(size_t index)
{
	// Check_Object(this);
	Verify(IsFileOpened());
	Verify(writeEnabled == ReadOnly);
	return MemoryStream::RewindPointer(index);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MemoryStream&
FileStream::ReadBytes(
	PVOID ptr,
	size_t number_of_bytes
)
{
	// Check_Object(this);
	Verify(IsFileOpened());
	Verify(writeEnabled == ReadOnly);
	return MemoryStream::ReadBytes(ptr, number_of_bytes);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MemoryStream&
FileStream::WriteBytes(
	PCVOID ptr,
	size_t number_of_bytes
)
{
	// Check_Object(this);
	Verify(IsFileOpened());
	Verify(writeEnabled == WriteOnly);
	size_t written = gos_WriteFile(
						 fileHandle, Cast_Pointer(puint8_t, const_cast<PVOID>(ptr)), number_of_bytes);
	Verify(written == number_of_bytes);
	currentPosition += written;
	return *this;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
FileStream::IsFileOpened(void)
{
	// Check_Object(this);
	return isOpen;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool Stuff::CreateDirectories(PCSTR directory_path)
{
	if(directory_path == nullptr)
		return false;
	Check_Pointer(directory_path);
	PCSTR start_position = directory_path;
	PCSTR current_position = directory_path;
	if(*current_position != '\\')
	{
		if((current_position + 1) != nullptr)
		{
			if((current_position + 2) != nullptr)
			{
				if(*(current_position + 1) == ':' && *(current_position + 2) == '\\')
				{
					current_position += 2;
				}
			}
		}
	}
	if(*current_position != '\\')
		return false;
	while((current_position != nullptr))
	{
		//make a substring with the path...
		PCSTR next_slash;
		next_slash = strchr(current_position + 1, '\\');
		PSTR new_string = nullptr;
		if(next_slash == nullptr)
		{
			//copy the whole string
			size_t length = strlen(start_position) + 1;
			new_string = new char[length];
			Check_Pointer(new_string);
			Str_Copy(new_string, start_position, length);
		}
		else
		{
			if(next_slash - current_position == 0)
			{
				return false;
			}
			if(next_slash - current_position == 1)
			{
				return false;
			}
			//copy the sub string
			size_t length = size_t(next_slash - start_position);
			new_string = new char[length + 1];
			Check_Pointer(new_string);
			strncpy_s(new_string, (length + 1), start_position, length);
			new_string[length] = 0;
		}
		Verify(new_string != nullptr);
		if(!gos_DoesFileExist(new_string))
		{
			gos_CreateDirectory(new_string);
		}
		Check_Pointer(new_string);
		delete[] new_string;
		current_position = next_slash;
	}
	return true;
}