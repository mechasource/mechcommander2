//---------------------------------------------------------------------------
//
// ffile.cpp - This file contains the class functions for FastFile
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
#include <zlib.h>

#include "ffile.h"
//#include "utilities.h"
//#include "lz.h"

// #include <windows.h>
// #include <string.h>
// #include "gameos.hpp"

union {
	uint32_t numval;
	char buffer[4];
} readtype32;

std::unique_ptr<uint8_t> LZPacketBuffer;
size_t LZPacketBufferSize = 512000;

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
#if CONSIDERED_DISABLED
PVOID
FastFile::operator new(size_t mySize)
{
	PVOID result = ::malloc(mySize);
	return (result);
}

//---------------------------------------------------------------------------
void
FastFile::operator delete(PVOID us)
{
	::free(us);
}
#endif

//---------------------------------------------------------------------------
// FastFile::FastFile(void)
//{
//	files			= nullptr;
//	numFiles		= 0;
//	fileName		= nullptr;
//	handle			= nullptr;
//	length			= 0;
//	logicalPosition = 0;
//	useLZCompress   = false;
//}

//---------------------------------------------------------------------------
// FastFile::~FastFile(void)
//{
//	close();
//	if (LZPacketBuffer)
//	{
//		free(LZPacketBuffer);
//		LZPacketBuffer = nullptr;
//	}
//}

//---------------------------------------------------------------------------
HRESULT
FastFile::open(const std::wstring_view& fileName)
{
	//---------------------------------------------
	//-- Read in ALL of the file names and offsets

	m_stream.open(fileName, std::ios::in);
	if ((m_stream.rdstate() & std::ifstream::failbit) != 0)
		throw std::system_error(EIO, std::system_category(), __func__);

	m_fileName = fileName;

	//---------------------------------------------
	//-- First Long is Version Number of FastFile
	m_stream.seekg(std::ios::beg);
	m_stream.read(readtype32.buffer, sizeof(readtype32.numval));

	if ((readtype32.numval != FASTFILE_VERSION) && (readtype32.numval != FASTFILE_VERSION_LZ))
		throw std::system_error(EILSEQ, std::system_category(), __func__);
	if (readtype32.numval == FASTFILE_VERSION_LZ)
		m_useLZCompress = true;

	//---------------------------------------------
	//-- Second Long is number of filenames present.
	m_stream.seekg(sizeof(readtype32.numval));
	m_stream.read(readtype32.buffer, sizeof(readtype32.numval));

	// lets introduce some kind of safety
	if (readtype32.numval > 100)
		throw std::system_error(EOVERFLOW, std::system_category(), __func__);

	m_files.resize(readtype32.numval);

	return S_OK;
}

//---------------------------------------------------------------------------
// void FastFile::close(void)
//{
//	//------------------------------------------------------------------------
//	// The actual stored filename is also in the parent.  Everyone else just has
//	// pointer and, as such, only the parent frees the memory.
//	if (fileName)
//		delete[] fileName; //	this was free, which didn't match the new
//						   //allocation.
//	//	neither new nor free were overridden. Should they have been?
//	fileName = nullptr;
//	length   = 0;
//	if (isOpen())
//	{
//		fclose(stream);
//		stream = nullptr;
//	}
//	//---------------------------------------------
//	//-- First Long is number of filenames present.
//	for (size_t i = 0; i < numFiles; i++)
//	{
//		free(files[i].fentry);
//	}
//	free(files);
//	files	= nullptr;
//	numFiles = 0;
//}

//---------------------------------------------------------------------------
HRESULT
FastFile::openFast(uint32_t hash, const std::wstring_view& fileName, ffindex& index)
{
	//------------------------------------------------------------------
	//-- In order to use this, the file name must be part of the index.
	for (size_t i = 0; i < m_files.size(); i++)
	{
		// wcscmp(m_files[i].fentry.name, fileName)

		if ((m_files[i].fentry.hash == hash) && (fileName.compare(m_files[i].fentry.name) == 0))
		{
			m_files[i].inuse = true;
			m_files[i].pos = 0;
			index = i;

			return S_OK;
		}
	}

	return E_FAIL;

	// the original function wanted to return a file descriptor which
	// somehow would be equal to the index
}

//---------------------------------------------------------------------------
HRESULT
FastFile::closeFast(ffindex index)
{
	_ASSERT(index < m_files.size());
	// if ((index >= 0) && (index < m_files.size()) && (m_files[index].inuse == true))
	{
		m_files[index].inuse = false;
		m_files[index].pos = 0;
	}
}

//---------------------------------------------------------------------------
HRESULT
FastFile::seekFast(ffindex index, size_t position, int32_t how)
{
	if (index < m_files.size())
		throw std::system_error(EINVAL, std::system_category(), __func__);

	if (m_files[index].inuse = false)
		return S_FALSE;

	// This must always be an error, right?
	if (position > m_files[index].fentry.size)
		throw std::system_error(ERANGE, std::system_category(), __func__);

	// makes sense?
	switch (how)
	{
	case SEEK_SET:
		m_files[index].pos = position;
		break;
	case SEEK_END:
		m_files[index].pos = position + m_files[index].fentry.size;
		break;
	case SEEK_CUR:
		m_files[index].pos = position + m_files[index].pos;
		break;
	}

	//-----------------------------------
	//-- Now macro seek the entire file.

	m_stream.seekg(m_files[index].pos);
	m_stream.read(readtype32.buffer, sizeof(readtype32.numval));

	if (fseek(m_stream, m_files[index].pos + m_files[index].fentry.position, SEEK_SET) == 0)
		logicalPosition = ftell(m_stream);
	return (m_files[index].pos);

	return (FILE_NOT_OPEN);
}

//---------------------------------------------------------------------------
int32_t
FastFile::readFast(int32_t fastFileHandle, PVOID bfr, int32_t size)
{
	size;
	int32_t result = 0;
	if ((fastFileHandle >= 0) && (fastFileHandle < numFiles) && m_files[fastFileHandle].inuse)
	{
		logicalPosition = fseek(m_stream,
			m_files[fastFileHandle].pos + m_files[fastFileHandle].fentry->position, SEEK_SET);
		// ALL files in the fast file are now zLib compressed. NO EXCEPTIONS!!
		// This fixes a bug where the zLib Compressed version is the same length
		// as the raw version.  Yikes but this is rare.  Finally happened
		// though! -fs
		/*
		if (files[fastFileHandle].pfe->size ==
		files[fastFileHandle].pfe->realSize)
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
			if (!LZPacketBuffer)
			{
				LZPacketBuffer = (puint8_t)malloc(LZPacketBufferSize);
				if (!LZPacketBuffer)
					return 0;
			}
			if ((int32_t)LZPacketBufferSize < m_files[fastFileHandle].fentry.size)
			{
				LZPacketBufferSize = m_files[fastFileHandle].fentry.size;
				free(LZPacketBuffer);
				LZPacketBuffer = (puint8_t)malloc(LZPacketBufferSize);
				if (!LZPacketBuffer)
					return 0;
			}
			if (LZPacketBuffer)
			{
				result = fread(LZPacketBuffer, 1, m_files[fastFileHandle].fentry.size, m_stream);
				logicalPosition += m_files[fastFileHandle].fentry.size;
				if (result != m_files[fastFileHandle].fentry.size)
				{
					// READ Error.  Maybe the CD is missing?
					bool openFailed = false;
					bool alreadyFullScreen = (Environment.fullScreen != 0);
					while (result != m_files[fastFileHandle].fentry.size)
					{
						openFailed = true;
						EnterWindowMode();
						char data[2048];
						sprintf(data, FileMissingString, m_fileName, CDMissingString);
						uint32_t result1 = MessageBox(
							nullptr, data, MissingTitleString, MB_OKCANCEL | MB_ICONWARNING);
						if (result1 == IDCANCEL)
						{
							ExitGameOS();
							return (2); // File not found.  Never returns
								// though!
						}
						logicalPosition = fseek(m_stream,
							m_files[fastFileHandle].pos + m_files[fastFileHandle].fentry->position,
							SEEK_SET);
						result =
							fread(LZPacketBuffer, 1, m_files[fastFileHandle].fentry.size, m_stream);
						logicalPosition += m_files[fastFileHandle].fentry.size;
					}
					if (openFailed && (Environment.fullScreen == 0) && alreadyFullScreen)
						EnterFullScreenMode();
				}
				//--------------------------------------------------------
				// USED to LZ Compress here.  It is NOW zLib Compression.
				//  We should not try to use old fastfiles becuase version check
				//  above should fail when trying to open!!
				size_t decompLength = 0;
				if (m_useLZCompress)
				{
					decompLength = LZDecomp(
						(puint8_t)bfr, LZPacketBuffer, m_files[fastFileHandle].fentry.size);
				}
				else
				{
					decompLength = m_files[fastFileHandle].fentry->realSize;
					int32_t error = uncompress((puint8_t)bfr, &decompLength, LZPacketBuffer,
						m_files[fastFileHandle].fentry.size);
					if (error != Z_OK)
						STOP(("Error %d UnCompressing File %s from FastFile %s", error,
							m_files[fastFileHandle].fentry->name, m_fileName));
				}
				if ((int32_t)decompLength != m_files[fastFileHandle].fentry->realSize)
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
// This function pulls the raw compressed data out of the file and sticks it in
// the buffer passed in.  This way, we can load the textures directly from file
// to RAM and not have to decompress them!!
int32_t
FastFile::readFastRAW(int32_t fastFileHandle, PVOIDbfr, int32_t size)
{
	size;
	int32_t result = 0;
	if ((fastFileHandle >= 0) && (fastFileHandle < numFiles) && m_files[fastFileHandle].inuse)
	{
		logicalPosition = fseek(m_stream,
			m_files[fastFileHandle].pos + m_files[fastFileHandle].fentry->position, SEEK_SET);
		if (size >= m_files[fastFileHandle].fentry.size)
		{
			result = fread(bfr, 1, m_files[fastFileHandle].fentry.size, m_stream);
			logicalPosition += m_files[fastFileHandle].fentry.size;
		}
		if (result != m_files[fastFileHandle].fentry.size)
		{
			// READ Error.  Maybe the CD is missing?
			bool openFailed = false;
			bool alreadyFullScreen = (Environment.fullScreen != 0);
			while (result != m_files[fastFileHandle].fentry.size)
			{
				openFailed = true;
				EnterWindowMode();
				char data[2048];
				sprintf(data, FileMissingString, m_fileName, CDMissingString);
				uint32_t result1 =
					MessageBox(nullptr, data, MissingTitleString, MB_OKCANCEL | MB_ICONWARNING);
				if (result1 == IDCANCEL)
				{
					ExitGameOS();
					return (2); // File not found.  Never returns though!
				}
				logicalPosition = fseek(m_stream,
					m_files[fastFileHandle].pos + m_files[fastFileHandle].fentry->position,
					SEEK_SET);
				result = fread(bfr, 1, m_files[fastFileHandle].fentry.size, m_stream);
				logicalPosition += m_files[fastFileHandle].fentry.size;
			}
			if (openFailed && (Environment.fullScreen == 0) && alreadyFullScreen)
				EnterFullScreenMode();
		}
		return result;
	}
	return FILE_NOT_OPEN;
}

//---------------------------------------------------------------------------
int32_t
FastFile::tellFast(int32_t fastFileHandle)
{
	if ((fastFileHandle >= 0) && (fastFileHandle < numFiles) && m_files[fastFileHandle].inuse)
		return m_files[fastFileHandle].pos;
	return -1;
}

//---------------------------------------------------------------------------
int32_t
FastFile::sizeFast(int32_t fastFileHandle)
{
	if ((fastFileHandle >= 0) && (fastFileHandle < numFiles) && m_files[fastFileHandle].inuse)
		return m_files[fastFileHandle].fentry->realSize;
	return -1;
}

//---------------------------------------------------------------------------
int32_t
FastFile::lzSizeFast(int32_t fastFileHandle)
{
	if ((fastFileHandle >= 0) && (fastFileHandle < numFiles) && m_files[fastFileHandle].inuse)
		return m_files[fastFileHandle].fentry.size;
	return -1;
}

//---------------------------------------------------------------------------
//
// Edit Log
//
//
//---------------------------------------------------------------------------
