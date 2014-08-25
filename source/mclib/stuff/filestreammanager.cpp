//===========================================================================//
// File:	filestrmmgr.cpp                                                  //
// Contents: Implementation Details of resource management                   //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"
//#include "stuffheaders.hpp"

#include <gameos.hpp>
#include <toolos.hpp>
#include <stuff/filestreammanager.hpp>

using namespace Stuff;


//#############################################################################
//##########################    FileDependencies    ###########################
//#############################################################################

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
FileDependencies::FileDependencies(void) : Plug(DefaultData)
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
FileDependencies::~FileDependencies(void)
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
FileDependencies& FileDependencies::operator=(const FileDependencies &dependencies)
{
	Check_Pointer(this);
	Check_Object(&dependencies);

	m_fileNameStream.Rewind();
	size_t len = dependencies.m_fileNameStream.GetBytesUsed();
	if (len)
	{
		MemoryStream scanner(
			static_cast<puint8_t>(dependencies.m_fileNameStream.GetPointer())-len,
			len
		);
		m_fileNameStream.AllocateBytes(len);
		m_fileNameStream << scanner;
	}
	return *this;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void FileDependencies::AddDependency(FileStream *stream)
{
	Check_Object(this);
	Check_Object(stream);

	//
	//---------------------
	// Get the new filename
	//---------------------
	//
	PCSTR new_file = stream->GetFileName();
	Check_Pointer(new_file);

	//
	//---------------------------------------------------
	// Make a new memorystream that wraps our current one
	//---------------------------------------------------
	//
	puint8_t end = Cast_Pointer(puint8_t, m_fileNameStream.GetPointer());
	size_t len = m_fileNameStream.GetBytesUsed();
	MemoryStream scanner(end-len, len);

	//
	//--------------------------------------
	// See if the new file is already inside
	//--------------------------------------
	//
	while (scanner.GetBytesRemaining() > 0)
	{
		PCSTR old_name = Cast_Pointer(PCSTR, scanner.GetPointer());
		len = strlen(old_name);
		if (!_stricmp(new_file, old_name))
			return;
		scanner.AdvancePointer(len+1);
	}
	m_fileNameStream.WriteBytes(new_file, strlen(new_file)+1);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void FileDependencies::AddDependencies(MemoryStream *dependencies)
{
	Check_Object(this);
	Check_Object(dependencies);

	size_t old_len = m_fileNameStream.GetBytesUsed();

	dependencies->Rewind();
	while (dependencies->GetBytesRemaining() > 0)
	{
		PCSTR new_name =
			Cast_Pointer(PCSTR, dependencies->GetPointer());
		size_t new_len = strlen(new_name);

		//
		//---------------------------------------------------
		// Make a new memorystream that wraps our current one
		//---------------------------------------------------
		//
		puint8_t end = static_cast<puint8_t>(m_fileNameStream.GetPointer());
		MemoryStream scanner(end-m_fileNameStream.GetBytesUsed(), old_len);

		//
		//--------------------------------------
		// See if the new file is already inside
		//--------------------------------------
		//
		while (scanner.GetBytesRemaining() > 0)
		{
			PCSTR old_name = Cast_Pointer(PCSTR, scanner.GetPointer());
			size_t len = strlen(old_name);
			if (!_stricmp(old_name, new_name))
				break;
			scanner.AdvancePointer(len+1);
		}
		if (!scanner.GetBytesRemaining())
			m_fileNameStream.WriteBytes(new_name, new_len+1);
		dependencies->AdvancePointer(new_len+1);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void FileDependencies::AddDependencies(const FileDependencies *dependencies)
{
	Check_Object(this);
	Check_Object(dependencies);

	AddDependencies((MemoryStream*)&dependencies->m_fileNameStream);
}

//#############################################################################
//#########################    FileStreamManager    ###########################
//#############################################################################

FileStreamManager* FileStreamManager::Instance = NULL;
	
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
FileStreamManager::FileStreamManager() : compareCache(NULL, true)
{	
}	

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
FileStreamManager::~FileStreamManager(void)
{
	PurgeFileCompareCache();
}	

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool FileStreamManager::CompareModificationDate(
	const MString &file_name, int64_t time_stamp)
{
	Check_Object(this);
	Check_Object(&file_name);

	//
	//------------------------------
	// Check the compare cache first
	//------------------------------
	//
	FileStatPlug *stat_plug;
	
	if ((stat_plug = compareCache.Find(file_name)) != NULL)
	{
		Check_Object(stat_plug);
		Check_Pointer(stat_plug->GetPointer());
		if (*stat_plug->GetPointer() > time_stamp)
			return true;
		return false;
	}

	//
	//-------------------------------------------------------------
	// Get the statistics about the file.  If the file isn't there, 
	// return false == "file older than time stamp"
	//-------------------------------------------------------------
	//
	int64_t file_stats = gos_FileTimeStamp(file_name);
	if (file_stats == -1)
		return false;

	//
	//-------------
	// Add to cache
	//-------------
	//
	stat_plug = new FileStatPlug(file_stats);
	Register_Object(stat_plug);
	compareCache.AddValue(stat_plug, file_name);

	//
	// Return, "is file newer than time stamp"?
	//
	if (file_stats > time_stamp)
		return true;
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void FileStreamManager::PurgeFileCompareCache(void)
{
	Check_Object(this);
	TreeIteratorOf<FileStatPlug*, MString> cache(&compareCache);
	cache.DeletePlugs();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MString* __stdcall Stuff::StripExtension(MString* file_name)
{
	Check_Object(file_name);

	if(file_name->GetLength() == 0)
	{
		return file_name;
	}

	PSTR p = strrchr(*file_name, '.');
	if (p)
	{
		*p = '\0';
		file_name->SetLength(size_t(p - (PSTR)*file_name));
	}

	Check_Object(file_name);
	return file_name;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MString* __stdcall Stuff::IsolateDirectory(MString* file_name)
{
	Check_Object(file_name);

	if(file_name->GetLength() == 0)
	{
		return file_name;
	}

	PSTR p = strrchr(*file_name, '\\');
	if (p)
	{
		*++p = '\0';
	}
	else
	{
		p = *file_name;
		*p = '\0';
	}
	file_name->SetLength(size_t(p - (PSTR )*file_name));
	Check_Object(file_name);
	return file_name;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MString* __stdcall Stuff::StripDirectory(MString* file_name)
{
	Check_Object(file_name);
	PSTR p = strrchr(*file_name, '\\');
	if (p)
	{
		PSTR q = *file_name;
		do
		{
			*q++ = *++p;
		}
		while (*p);
		// The following does not handle all cases...
		// file_name->SetLength(p - (PSTR )*file_name);
		file_name->SetLength(strlen(*file_name));
	}
	Check_Object(file_name);
	return file_name;
}
