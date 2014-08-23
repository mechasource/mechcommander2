//===========================================================================//
// File:	filestrm.hpp                                                     //
// Contents: Implementation Details of resource management                   //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _FILESTREAM_HPP_
#define _FILESTREAM_HPP_

#include <stuff/style.hpp>
#include <stuff/mstring.hpp>
#include <stuff/plug.hpp>
#include <stuff/sortedchain.hpp>

#ifndef _GAMEOS_HPP_
typedef struct	gosFileStream*	HGOSFILE;
#endif

namespace Stuff {

	class FileStreamManager;
	class FileStream;

	//##########################################################################
	//##########################    Directory    ###############################
	//##########################################################################

	class Directory
#if defined(_ARMOR)
		: public Stuff::Signature
#endif
	{
	public:
		Directory(
			PSTR find_files,
			bool find_directories=false
			);
		~Directory();

		void TestInstance(void) const {}

		PSTR
			GetCurrentFileName();
		void
			AdvanceCurrentFile();
		PSTR
			GetCurrentFolderName();
		void
			AdvanceCurrentFolder();

	public:
		int32_t 
			fileOk;

		Stuff::MString
			currentDirectoryString;

		typedef Stuff::PlugOf<Stuff::MString>
			DirectoryEntry;
		typedef Stuff::PlugOf<Stuff::MString>
			DirectoryFolder;


		Stuff::SortedChainOf<DirectoryFolder*, Stuff::MString>
			folderEntries;
		Stuff::SortedChainOf<DirectoryEntry*, Stuff::MString>
			fileEntries;
		Stuff::SortedChainIteratorOf<DirectoryFolder*, Stuff::MString>
			*folderWalker;
		Stuff::SortedChainIteratorOf<DirectoryEntry*, Stuff::MString>
			*fileWalker;
	};

	//##########################################################################
	//########################    FileStream    ################################
	//##########################################################################

	class FileStream:
		public MemoryStream
	{
		friend class FileStreamManager;

	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);

		static ClassData* DefaultData;

		static PCSTR WhiteSpace;

		void TestInstance(void) const;
		static bool
			TestClass();

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors
		//
	public:
		enum WriteStatus {
			ReadOnly,
			WriteOnly
		};

		FileStream();
		explicit FileStream(
			PCSTR file_name,
			WriteStatus writable = ReadOnly
			);
		~FileStream();

		void
			Open(
			PCSTR file_name = NULL,
			WriteStatus writable = ReadOnly
			);
		void
			Close();

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Status functions
		//
	public:
		void
			SetPointer(PVOID)
		{
#ifdef _GAMEOS_HPP_
			STOP(("No implementation possible for FileStream::SetPointer(PVOID)"));
#endif
		}
		void
			SetPointer(size_t index);

		MemoryStream&
			AdvancePointer(size_t count);

		MemoryStream&
			RewindPointer(size_t count);

		MemoryStream&
			ReadBytes(
			PVOID ptr,
			size_t number_of_bytes
			);
		MemoryStream&
			WriteBytes(
			PCVOID ptr,
			size_t number_of_bytes
			);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// File functions
		//
	public:
		virtual bool
			IsFileOpened();
		PCSTR
			GetFileName()
		{Check_Object(this); return fileName;}

		enum SeekType {
			FromBeginning,
			FromEnd
		};

		static char
			RedirectedName[256];
		static bool
			IsRedirected;

		static void
			IgnoreReadOnly(bool flag)
		{IgnoreReadOnlyFlag = flag;}


	protected:
		WriteStatus	writeEnabled;
		PSTR		fileName;
		HGOSFILE	fileHandle;
		bool		isOpen;
		static bool	IgnoreReadOnlyFlag;
	};

	MString* __stdcall StripExtension(MString *file_name);
	MString* __stdcall IsolateDirectory(MString *file_name);
	MString* __stdcall StripDirectory(MString *file_name);

	bool CreateDirectories(PCSTR directories);
}
#endif
