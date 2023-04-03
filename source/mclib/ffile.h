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

//#include "dstd.h"
//#include "file.h"
//#include <sys/types.h>
//#include <sys/stat.h>

//---------------------------------------------------------------------------
enum fastfile_versions : uint32_t
{
	FASTFILE_VERSION = 0xCADDECAF,
	FASTFILE_VERSION_LZ = 0xFADDECAF
};

//#pragma pack(1)
typedef struct FILEENTRY
{
	size_t position;
	size_t size; // LZ Compressed Size
	size_t realSize; // Uncompressed Size
	size_t hash; // Hash Compare to weed out stinky files faster then StrCmp
	// std::wstring_view name;
	wchar_t name[UCHAR_MAX];
} FILEENTRY;
//#pragma pack()

typedef struct FILE_HANDLE
{
	FILEENTRY fentry;
	size_t pos;
	bool inuse;
} FILE_HANDLE;

typedef size_t ffindex;

//---------------------------------------------------------------------------
// Class FastFile
class FastFile
{
protected:
	std::vector<FILE_HANDLE> m_files;
	std::fstream m_stream;
	std::wstring_view m_fileName;

	size_t numFiles;
	size_t length;
	size_t logicalPosition;
	bool m_useLZCompress;

public:
	FastFile(void)
		: numFiles(0)
		, length(0)
		, logicalPosition(0)
		, m_useLZCompress(false)
	{
	}
	~FastFile(void)
	{
		close();
		// if (LZPacketBuffer)
		//{
		//	free(LZPacketBuffer);
		//	LZPacketBuffer = nullptr;
		//}
	}

	PVOID operator new(size_t mySize);
	void operator delete(PVOID us);

	HRESULT open(std::wstring_view fileName);
	HRESULT open(std::wstring_view filename)
	{
		std::wstring_view fileName(filename);
		return open(fileName);
	};
	HRESULT openFast(uint32_t hash, std::wstring_view fileName, ffindex& index);
	HRESULT closeFast(ffindex index);

	void close(void)
	{
		return m_stream.close();
	}
	bool isOpen(void)
	{
		return m_stream.is_open();
	}

	size_t getNumFiles(void)
	{
		return m_files.size();
	}

	HRESULT seekFast(ffindex index, size_t position, int32_t how = SEEK_SET);
	int32_t readFast(int32_t fastFileHandle, PVOID bfr, int32_t size);
	int32_t readFastRAW(int32_t fastFileHandle, PVOID bfr, int32_t size);
	int32_t tellFast(int32_t fastFileHandle);
	int32_t sizeFast(int32_t fastFileHandle);
	int32_t lzSizeFast(int32_t fastFileHandle);

	bool isLZCompressed(void)
	{
		return m_useLZCompress;
	}
};

//---------------------------------------------------------------------------
extern FastFile** fastFiles;
extern uint32_t numFastFiles;
extern uint32_t maxFastFiles;
//---------------------------------------------------------------------------
#endif
