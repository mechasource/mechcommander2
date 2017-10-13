//---------------------------------------------------------------------------
//
// file.cpp - This file contains the class functions for File
//
//				The File class simply calls the Windows file functions.
//				It is purely a wrapper.
//
//				The mmFile Class is a wrapper for the Win32 Memory Mapped
//				file functionality.  It is used exactly the same as above class.
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"

#include "file.h"
#include "heap.h"
#include "ffile.h"
#include "packet.h"
#include "fastfile.h"
#include "utilities.h"
//#include <string.h>
//#include <io.h>
//#include <ctype.h>
//#include <windows.h>

#ifndef _MBCS
#include <gameos.hpp>
#else
#include <assert.h>
#define gosASSERT assert
#define gos_Malloc malloc
#define gos_Free free
#endif

//-----------------
// Static Variables
uint32_t File::lastError  = NO_ERROR;
bool File::logFileTraffic = FALSE;

FilePtr fileTrafficLog = nullptr;
char CDInstallPath[1024];
void EnterWindowMode();
void EnterFullScreenMode();
void __stdcall ExitGameOS();

extern char FileMissingString[];
extern char CDMissingString[];
extern char MissingTitleString[];

#undef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE -1
//---------------------------------------------------------------------------
void createTrafficLog(void)
{
	if (fileTrafficLog && fileTrafficLog->isOpen())
		return;
	fileTrafficLog = new File;
	fileTrafficLog->create("filetraffic.log");
}

//---------------------------------------------------------------------------
// Global Functions
int32_t __stdcall fileExists(PSTR fName)
{
	struct _stat st;
	if (_stat(fName, &st) != -1)
	{
		return 1;
	}
	int32_t fastFileHandle = -1;
	FastFilePtr fastFile   = FastFileFind(fName, fastFileHandle);
	if (fastFile)
		return 2;
	return 0;
}

//---------------------------------------------------------------------------
int32_t __stdcall fileExistsOnCD(PSTR fName)
{
	// Just add the CD path here and see if its there.
	char bigPath[2048];
	strcpy(bigPath, CDInstallPath);
	strcat(bigPath, fName);
	struct _stat st;
	if (_stat(bigPath, &st) != -1)
	{
		return 1;
	}
	return 0;
}

//---------------------------------------------------------------------------
bool __stdcall file1OlderThan2(PSTR file1, PSTR file2)
{
	if ((fileExists(file1) == 1) && (fileExists(file2) == 1))
	{
		struct _stat st1, st2;
		_stat(file1, &st1);
		_stat(file2, &st2);
		if (st1.st_mtime < st2.st_mtime)
			return true;
	}
	return false;
}

//---------------------------------------------------------------------------
//	class File member functions
PVOID File::operator new(size_t mySize)
{
	PVOID result = nullptr;
	result		 = systemHeap->Malloc(mySize);
	return (result);
}

//---------------------------------------------------------------------------
void File::operator delete(PVOID us) { systemHeap->Free(us); }

//---------------------------------------------------------------------------
File::File(void)
{
	fileName		= nullptr;
	fileMode		= NOMODE;
	handle			= -1;
	length			= 0;
	logicalPosition = 0;
	bufferResult	= 0;
	parent			= nullptr;
	parentOffset	= 0;
	physicalLength  = 0;
	childList		= nullptr;
	numChildren		= 0;
	inRAM			= FALSE;
	fileImage		= nullptr;
	fastFile		= nullptr;
}

//---------------------------------------------------------------------------
inline void File::setup(void)
{
	logicalPosition = 0;
	//----------------------------------------------------------------------
	// This is only called from an open with a filename, not a file pointer.
	// ie. It assumes we are the parent.
	if (isOpen())
		length = fileSize();
	else
		length = 0;
	parent		   = nullptr;
	parentOffset   = 0;
	physicalLength = length;
	childList	  = nullptr;
	numChildren	= 0;
}

//---------------------------------------------------------------------------
File::~File(void) { close(); }

//---------------------------------------------------------------------------
bool File::eof(void) { return (logicalPosition >= getLength()); }

//---------------------------------------------------------------------------
int32_t File::open(PCSTR fName, FileMode _mode, int32_t numChild)
{
	gosASSERT(!isOpen());
	//-------------------------------------------------------------
	int32_t fNameLength = strlen(fName);
	fileName			= (PSTR)systemHeap->Malloc(fNameLength + 1);
	gosASSERT(fileName != nullptr);
	strncpy(fileName, fName, fNameLength + 1);
	fileMode = _mode;
	_fmode   = _O_BINARY;
	_strlwr(fileName);
	if (fileMode == CREATE)
	{
		handle = _creat(fileName, _S_IWRITE);
		if (handle == INVALID_HANDLE_VALUE)
		{
			lastError = errno;
			return lastError;
		}
	}
	else
	{
		//----------------------------------------------------------------
		//-- First, see if file is in normal place.  Useful for patches!!
		handle = _open(fileName, _O_RDONLY);
		//------------------------------------------
		//-- Next, see if file is in fastFile.
		if (handle == INVALID_HANDLE_VALUE)
		{
			lastError = errno;
			fastFile  = FastFileFind(fileName, fastFileHandle);
			if (!fastFile)
			{
				// Not in main installed directory and not in fastfile.  Look on
				// CD.
				char actualPath[2048];
				strcpy(actualPath, CDInstallPath);
				strcat(actualPath, fileName);
				handle = _open(actualPath, _O_RDONLY);
				if (handle == INVALID_HANDLE_VALUE)
				{
					bool openFailed		   = false;
					bool alreadyFullScreen = (Environment.fullScreen != 0);
					while (handle == INVALID_HANDLE_VALUE)
					{
						openFailed = true;
						// OK, check to see if the CD is actually present.
						// Do this by checking for tgl.fst on the CD Path.
						// If its there, the CD is present BUT the file is
						// missing. MANY files in MechCommander 2 are LEGALLY
						// missing! Tell it to the art staff.
						char testCDPath[2048];
						strcpy(testCDPath, CDInstallPath);
						strcat(testCDPath, "tgl.fst");
						uint32_t findCD = fileExists(testCDPath);
						if (findCD == 1) // File exists. CD is in drive.  Return
										 // 2 to indicate file not found.
							return 2;
						EnterWindowMode();
						char data[2048];
						sprintf(
							data, FileMissingString, fileName, CDMissingString);
						uint32_t result1 = MessageBox(nullptr, data,
							MissingTitleString, MB_OKCANCEL | MB_ICONWARNING);
						if (result1 == IDCANCEL)
						{
							ExitGameOS();
							return (2); // File not found.  Never returns
										// though!
						}
						handle = _open(actualPath, _O_RDONLY);
					}
					if (openFailed && (Environment.fullScreen == 0) &&
						alreadyFullScreen)
						EnterFullScreenMode();
				}
				else
				{
					if (logFileTraffic && (handle != INVALID_HANDLE_VALUE))
					{
						if (!fileTrafficLog)
						{
							createTrafficLog();
						}
						char msg[300];
						sprintf(msg, "CFHandle  Length: %010d    File: %s",
							fileSize(), fileName);
						fileTrafficLog->writeLine(msg);
					}
					setup();
					//------------------------------------------------------------
					// NEW FUNCTIONALITY!!!
					//
					// Each file may have a number of files open as children
					// which use the parent's handle for reads and writes.  This
					// would allow us to open a packet file and read a packet as
					// a fitIni or allow us to write a packet as a fit ini and
					// so forth.
					//
					// It also allows us to use the packet file extensions as
					// tree files to avoid the ten thousand file syndrome.
					//
					// There is now an open which takes a FilePtr and a size.
					maxChildren = numChild;
					childList   = (FilePtr*)systemHeap->Malloc(
						  sizeof(FilePtr) * maxChildren);
					if (!childList)
					{
						return (NO_RAM_FOR_CHILD_LIST);
					}
					numChildren = 0;
					for (size_t i = 0; i < (int32_t)maxChildren; i++)
					{
						childList[i] = nullptr;
					}
					return (NO_ERROR);
				}
			}
			if (logFileTraffic)
			{
				if (!fileTrafficLog)
				{
					createTrafficLog();
				}
				char msg[300];
				sprintf(msg, "FASTF     Length: %010d    File: %s", fileSize(),
					fileName);
				fileTrafficLog->writeLine(msg);
			}
			//---------------------------------------------------------------------
			//-- FastFiles are all compressed.  Must read in entire chunk into
			//RAM
			//-- Then close fastfile!!!!!
			inRAM	 = TRUE;
			fileImage = (puint8_t)malloc(fileSize());
			if (fileImage)
			{
				fastFile->readFast(fastFileHandle, fileImage, fileSize());
				physicalLength = getLength();
				//------------------------------------
				//-- Image is in RAM.  Shut the file.
				// fastFile->closeFast(fastFileHandle);
				// fastFile = nullptr;
				// fastFileHandle = -1;
				logicalPosition = 0;
			}
			return NO_ERROR;
		}
		else
		{
			if (logFileTraffic && (handle != INVALID_HANDLE_VALUE))
			{
				if (!fileTrafficLog)
				{
					createTrafficLog();
				}
				char msg[300];
				sprintf(msg, "CFHandle  Length: %010d    File: %s", fileSize(),
					fileName);
				fileTrafficLog->writeLine(msg);
			}
			setup();
			//------------------------------------------------------------
			// NEW FUNCTIONALITY!!!
			//
			// Each file may have a number of files open as children which
			// use the parent's handle for reads and writes.  This would
			// allow us to open a packet file and read a packet as a fitIni
			// or allow us to write a packet as a fit ini and so forth.
			//
			// It also allows us to use the packet file extensions as tree
			// files to avoid the ten thousand file syndrome.
			//
			// There is now an open which takes a FilePtr and a size.
			maxChildren = numChild;
			childList =
				(FilePtr*)systemHeap->Malloc(sizeof(FilePtr) * maxChildren);
			if (!childList)
			{
				return (NO_RAM_FOR_CHILD_LIST);
			}
			numChildren = 0;
			for (size_t i = 0; i < (int32_t)maxChildren; i++)
			{
				childList[i] = nullptr;
			}
			return (NO_ERROR);
		}
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t File::open(FilePtr _parent, uint32_t fileSize, int32_t numChild)
{
	if (_parent && (_parent->fastFile == nullptr))
	{
		parent = _parent;
		if (parent->getFileMode() != READ)
		{
			return (CANT_WRITE_TO_CHILD);
		}
		physicalLength  = fileSize;
		parentOffset	= parent->getLogicalPosition();
		logicalPosition = 0;
		//-------------------------------------------------------------
		fileName = parent->getFilename();
		fileMode = parent->getFileMode();
		handle   = parent->getFileHandle();
		if (logFileTraffic)
		{
			if (!fileTrafficLog)
			{
				createTrafficLog();
			}
			char msg[300];
			sprintf(msg, "CHILD     Length: %010d    File: %s", fileSize,
				_parent->getFilename());
			fileTrafficLog->writeLine(msg);
		}
		int32_t result = parent->addChild(this);
		if (result != NO_ERROR)
			return (result);
		//------------------------------------------------------------
		// NEW FUNCTIONALITY!!!
		//
		// Each file may have a number of files open as children which
		// use the parent's handle for reads and writes.  This would
		// allow us to open a packet file and read a packet as a fitIni
		// or allow us to write a packet as a fit ini and so forth.
		//
		// It also allows us to use the packet file extensions as tree
		// files to avoid the ten thousand file syndrome.
		//
		// There is now an open which takes a FilePtr and a size.
		//
		// IF a numChild parameter is passed in as -1, we want this file in
		// RAM!! This means NO CHILDREN!!!!!!!!!!!!!
		if (numChild != -1)
		{
			maxChildren = numChild;
			childList =
				(FilePtr*)systemHeap->Malloc(sizeof(FilePtr) * maxChildren);
			gosASSERT(childList != nullptr);
			numChildren = 0;
			for (size_t i = 0; i < (int32_t)maxChildren; i++)
			{
				childList[i] = nullptr;
			}
		}
		else
		{
			maxChildren		= 0;
			inRAM			= TRUE;
			uint32_t result = 0;
			fileImage		= (puint8_t)malloc(fileSize);
			if (!fileImage)
				inRAM = FALSE;
			if (_parent->getFileClass() == PACKETFILE)
			{
				result = ((PacketFilePtr)_parent)
							 ->readPacket(
								 ((PacketFilePtr)_parent)->getCurrentPacket(),
								 fileImage);
			}
			else
			{
				result = _read(handle, fileImage, fileSize);
				if (result != fileSize)
					lastError = errno;
			}
		}
	}
	else
	{
		return (PARENT_NULL);
	}
	return (NO_ERROR);
}

int32_t File::open(PCSTR buffer, int32_t bufferLength)
{
	if (buffer && bufferLength > 0)
	{
		fileImage		= (puint8_t)buffer;
		physicalLength  = bufferLength;
		logicalPosition = 0;
		fileMode		= RDWRITE;
		inRAM			= true;
	}
	else // fail on nullptr
	{
		return FILE_NOT_OPEN;
	}
	return NO_ERROR;
}

//---------------------------------------------------------------------------
int32_t File::create(PCSTR fName) { return (open(fName, CREATE)); }

int32_t File::createWithCase(PSTR fName)
{
	gosASSERT(!isOpen());
	//-------------------------------------------------------------
	int32_t fNameLength = strlen(fName);
	fileName			= (PSTR)systemHeap->Malloc(fNameLength + 1);
	gosASSERT(fileName != nullptr);
	strncpy(fileName, fName, fNameLength + 1);
	fileMode = CREATE;
	_fmode   = _O_BINARY;
	handle   = _creat(fileName, _S_IWRITE);
	if (handle == INVALID_HANDLE_VALUE)
	{
		lastError = errno;
		return lastError;
	}
	return 0;
}
//---------------------------------------------------------------------------
int32_t File::addChild(FilePtr child)
{
	if (maxChildren)
	{
		for (size_t i = 0; i < (int32_t)maxChildren; i++)
		{
			if (childList[i] == nullptr)
			{
				childList[i] = child;
				return NO_ERROR;
			}
		}
	}
	return (TOO_MANY_CHILDREN);
}

//---------------------------------------------------------------------------
void File::removeChild(FilePtr child)
{
	if (maxChildren)
	{
		if (childList)
		{
			for (size_t i = 0; i < (int32_t)maxChildren; i++)
			{
				if (childList[i] == child)
				{
					childList[i] = nullptr;
					break;
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
void File::close(void)
{
	//------------------------------------------------------------------------
	// First, close us if we are the parent.  Otherwise, just nullptr the handle
	// DO NOT CALL CLOSE IF WE ARE A CHILD!!
	//
	// The actual stored filename is also in the parent.  Everyone else just has
	// pointer and, as such, only the parent frees the memory.
	bool bFast = false;
	if ((parent == nullptr) && (fileName != nullptr))
	{
		systemHeap->Free(fileName);
	}
	fileName = nullptr;
	length   = 0;
	if (isOpen())
	{
		if ((parent == nullptr) && (handle != nullptr) && (-1 != handle))
			_close(handle);
		handle = nullptr;
		if (fastFile)
		{
			fastFile->closeFast(fastFileHandle);
			bFast = true; // save that it was a fast file
		}
		fastFile	   = nullptr; // DO NOT DELETE THE FASTFILE!!!!!!!!!!!!!
		fastFileHandle = -1;
	}
	//---------------------------------------------------------------------
	// Check if we have any children and close them.  This will set their
	// handle to nullptr and their filename to nullptr.  It will also close any
	// of THEIR children.
	if (maxChildren)
	{
		if (childList)
		{
			for (size_t i = 0; i < (int32_t)maxChildren; i++)
			{
				if (childList[i])
					childList[i]->close();
			}
		}
		if (childList)
			systemHeap->Free(childList);
	}
	if (parent != nullptr)
		parent->removeChild(this);
	childList   = nullptr;
	numChildren = 0;
	if (inRAM && (bFast || parent)) // don't want to delete memFiles
	{
		if (fileImage)
			free(fileImage);
		fileImage = nullptr;
		inRAM	 = FALSE;
	}
}

//---------------------------------------------------------------------------
void File::deleteFile(void)
{
	//--------------------------------------------------------------
	// Must be the ultimate parent to delete this file.  Close will
	// make sure all of the children close themselves.
	if (isOpen() && (parent == nullptr))
		close();
}

int32_t newPosition = 0;
//---------------------------------------------------------------------------
int32_t File::seek(int32_t pos, int32_t from)
{
	switch (from)
	{
	case SEEK_SET:
		if (pos > (int32_t)getLength())
		{
			return READ_PAST_EOF_ERR;
		}
		break;
	case SEEK_END:
		if ((abs(pos) > (int32_t)getLength()) || (pos > 0))
		{
			return READ_PAST_EOF_ERR;
		}
		break;
	case SEEK_CUR:
		if (pos + logicalPosition > getLength())
		{
			return READ_PAST_EOF_ERR;
		}
		break;
	}
	if (inRAM && fileImage)
	{
		if (parent)
		{
			switch (from)
			{
			case SEEK_SET:
				newPosition = pos;
				break;
			case SEEK_END:
				newPosition = getLength() + parentOffset;
				newPosition += pos;
				break;
			case SEEK_CUR:
				newPosition += pos;
				break;
			}
		}
		else
		{
			switch (from)
			{
			case SEEK_SET:
				newPosition = pos;
				break;
			case SEEK_END:
				newPosition = getLength() + pos;
				break;
			case SEEK_CUR:
				newPosition += pos;
				break;
			}
		}
		if (newPosition == -1)
		{
			return (INVALID_SEEK_ERR);
		}
		logicalPosition = newPosition;
	}
	else if (fastFile)
	{
		newPosition		= fastFile->seekFast(fastFileHandle, pos, from);
		logicalPosition = newPosition;
	}
	else
	{
		if (parent)
		{
			switch (from)
			{
			case SEEK_SET:
				_lseek(handle, pos + parentOffset, SEEK_SET);
				newPosition = pos;
				break;
			case SEEK_END:
				_lseek(handle, getLength() + parentOffset, SEEK_SET);
				_lseek(handle, pos, SEEK_CUR);
				newPosition = getLength() + pos;
				break;
			case SEEK_CUR:
				_lseek(handle, pos, SEEK_CUR);
				newPosition = logicalPosition + pos;
				break;
			}
		}
		else
		{
			newPosition = _lseek(handle, pos, from);
		}
		if (newPosition == -1)
		{
			return (INVALID_SEEK_ERR);
		}
		logicalPosition = newPosition;
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t File::read(uint32_t pos, puint8_t buffer, int32_t length)
{
	int32_t result = 0;
	if (inRAM && fileImage)
	{
		PSTR readAddress = ((PSTR)fileImage) + pos;
		memcpy((PSTR)buffer, readAddress, length);
		return (length);
	}
	else if (fastFile)
	{
		if (logicalPosition != pos)
			fastFile->seekFast(fastFileHandle, pos);
		result = fastFile->readFast(fastFileHandle, buffer, length);
	}
	else
	{
		if (isOpen())
		{
			if (logicalPosition != pos)
				seek(pos);
			result = _read(handle, buffer, length);
			if (result != length)
				lastError = errno;
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	return (result);
}

//---------------------------------------------------------------------------
uint8_t File::readByte(void)
{
	uint8_t value  = 0;
	int32_t result = 0;
	if (inRAM && fileImage)
	{
		PSTR readAddress = (PSTR)fileImage + logicalPosition;
		memcpy((PSTR)&value, readAddress, sizeof(value));
		logicalPosition += sizeof(value);
	}
	else if (fastFile)
	{
		result =
			fastFile->readFast(fastFileHandle, (PSTR)&value, sizeof(value));
		logicalPosition += sizeof(value);
	}
	else
	{
		if (isOpen())
		{
			result = _read(handle, (&value), sizeof(value));
			logicalPosition += sizeof(value);
			if (result != sizeof(value))
				lastError = errno;
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	return value;
}

//---------------------------------------------------------------------------
int16_t File::readWord(void)
{
	int16_t value  = 0;
	int32_t result = 0;
	if (inRAM && fileImage)
	{
		PSTR readAddress = (PSTR)fileImage + logicalPosition;
		memcpy((PSTR)(&value), readAddress, sizeof(value));
		logicalPosition += sizeof(value);
	}
	else if (fastFile)
	{
		result =
			fastFile->readFast(fastFileHandle, (PSTR)&value, sizeof(value));
		logicalPosition += sizeof(value);
	}
	else
	{
		if (isOpen())
		{
			result = _read(handle, (&value), sizeof(value));
			logicalPosition += sizeof(value);
			if (result != sizeof(value))
				lastError = errno;
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	return value;
}

//---------------------------------------------------------------------------
int16_t File::readShort(void) { return (readWord()); }

//---------------------------------------------------------------------------
int32_t File::readLong(void)
{
	int32_t value   = 0;
	uint32_t result = 0;
	if (inRAM && fileImage)
	{
		PSTR readAddress = (PSTR)fileImage + logicalPosition;
		memcpy((PSTR)(&value), readAddress, sizeof(value));
		logicalPosition += sizeof(value);
	}
	else if (fastFile)
	{
		result =
			fastFile->readFast(fastFileHandle, (PSTR)&value, sizeof(value));
		logicalPosition += sizeof(value);
	}
	else
	{
		if (isOpen())
		{
			result = _read(handle, (&value), sizeof(value));
			logicalPosition += sizeof(value);
			if (result != sizeof(value))
				lastError = errno;
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	return value;
}

bool isNAN(float* pFloat)
{
	/* We're assuming ansi/ieee 754 floating point representation. See
	 * http://www.research.microsoft.com/~hollasch/cgindex/coding/ieeefloat.html.
	 */
	puint8_t byteArray = (puint8_t)pFloat;
	if ((0x7f == (0x7f & byteArray[3])) && (0x80 == (0x80 & byteArray[2])))
	{
		if (0x80 == (0x80 & byteArray[3]))
		{
			/* if the mantissa is a 1 followed by all zeros in this case then it
			is technically "Indeterminate" rather than an NaN, but we'll just
			count it as a NaN here. */
			return true;
		}
		else
		{
			return true;
		}
	}
	return false;
}

float File::readFloat(void)
{
	float value		= 0;
	uint32_t result = 0;
	if (inRAM && fileImage)
	{
		PSTR readAddress = (PSTR)fileImage + logicalPosition;
		memcpy((PSTR)(&value), readAddress, sizeof(value));
		logicalPosition += sizeof(value);
	}
	else if (fastFile)
	{
		result =
			fastFile->readFast(fastFileHandle, (PSTR)&value, sizeof(value));
		logicalPosition += sizeof(value);
	}
	else
	{
		if (isOpen())
		{
			result = _read(handle, (&value), sizeof(value));
			logicalPosition += sizeof(value);
			if (result != sizeof(value))
				lastError = errno;
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	if (isNAN(&value))
	{
		gosASSERT(false);
		value = 1.0 /*arbitrary value that seems safe*/;
	}
	return value;
}

//---------------------------------------------------------------------------
int32_t File::readString(puint8_t buffer)
{
	int32_t last = 0;
	if (isOpen())
	{
		for (;;)
		{
			byte ch		 = readByte();
			buffer[last] = ch;
			if (ch)
				++last;
			else
				break;
		}
	}
	else
	{
		lastError = FILE_NOT_OPEN;
	}
	return last;
}

//---------------------------------------------------------------------------
int32_t File::read(puint8_t buffer, int32_t length)
{
	int32_t result = 0;
	if (inRAM && fileImage)
	{
		PSTR readAddress = (PSTR)fileImage + logicalPosition;
		memcpy((PSTR)buffer, readAddress, length);
		logicalPosition += length;
		return (length);
	}
	else if (fastFile)
	{
		result = fastFile->readFast(fastFileHandle, buffer, length);
		logicalPosition += result;
	}
	else
	{
		if (isOpen())
		{
			result = _read(handle, buffer, length);
			if (result != length)
				lastError = errno;
			else
				logicalPosition += result;
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	return result;
}

//---------------------------------------------------------------------------
int32_t File::readRAW(uint32_t*& buffer, UserHeapPtr heap)
{
	int32_t result = 0;
	if (fastFile && heap && fastFile->isLZCompressed())
	{
		int32_t lzSizeNeeded = fastFile->lzSizeFast(fastFileHandle);
		buffer				 = (uint32_t*)heap->Malloc(lzSizeNeeded);
		result = fastFile->readFastRAW(fastFileHandle, buffer, lzSizeNeeded);
		logicalPosition += result;
	}
	return result;
}

//---------------------------------------------------------------------------
int32_t File::readLine(puint8_t buffer, int32_t maxLength)
{
	int32_t i = 0;
	if (inRAM && fileImage)
	{
		if (isOpen())
		{
			puint8_t readAddress = (puint8_t)fileImage + logicalPosition;
			while ((i < maxLength) && ((i + logicalPosition) < fileSize()) &&
				   readAddress[i] != '\r')
				i++;
			memcpy(buffer, readAddress, i);
			buffer[i++] = 0;
			logicalPosition += i;
			if (logicalPosition > fileSize())
				return READ_PAST_EOF_ERR;
			if (readAddress[i] == '\n')
				logicalPosition += 1;
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	else if (fastFile)
	{
		int32_t bytesread;
		bytesread = fastFile->readFast(fastFileHandle, buffer, maxLength);
		if (maxLength > bytesread)
			maxLength = bytesread;
		while ((i < maxLength) && (buffer[i] != '\r'))
			i++;
		buffer[i++] = 0;
		logicalPosition += i;
		if (buffer[i] == '\n')
			logicalPosition += 1;
		fastFile->seekFast(fastFileHandle, logicalPosition);
	}
	else
	{
		if (isOpen())
		{
			int32_t bytesread;
			bytesread = _read(handle, buffer, maxLength);
			if (maxLength > bytesread)
				maxLength = bytesread;
			while (i < maxLength && buffer[i] != '\r')
				i++;
			buffer[i++] = 0;
			logicalPosition += i;
			if (buffer[i] == '\n')
				logicalPosition += 1;
			seek(logicalPosition);
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	return i;
}

//---------------------------------------------------------------------------
int32_t File::readLineEx(puint8_t buffer, int32_t maxLength)
{
	int32_t i = 0;
	if (inRAM && fileImage)
	{
		if (isOpen())
		{
			puint8_t readAddress = (puint8_t)fileImage + logicalPosition;
			while (i < maxLength && readAddress[i] != '\n')
				i++;
			i++; // Include Newline
			memcpy(buffer, readAddress, i);
			buffer[i++] = 0;
			logicalPosition += (i - 1);
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	else if (fastFile)
	{
		int32_t bytesread;
		bytesread = fastFile->readFast(fastFileHandle, buffer, maxLength);
		if (maxLength > bytesread)
			maxLength = bytesread;
		while ((i < maxLength) && (buffer[i] != '\n'))
			i++;
		i++; // Include Newline
		buffer[i++] = 0;
		logicalPosition += (i - 1);
		fastFile->seekFast(fastFileHandle, logicalPosition);
	}
	else
	{
		if (isOpen())
		{
			int32_t bytesread = _read(handle, buffer, maxLength);
			if (maxLength > bytesread)
				maxLength = bytesread;
			while (i < maxLength && buffer[i] != '\n')
				i++;
			i++;
			buffer[i++] = 0;
			logicalPosition += (i - 1);
			seek(logicalPosition);
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	return i;
}

//---------------------------------------------------------------------------
int32_t File::write(uint32_t pos, puint8_t buffer, int32_t bytes)
{
	uint32_t result = 0;
	if (parent == nullptr)
	{
		if (isOpen())
		{
			if (logicalPosition != pos)
				seek(pos);
			if (inRAM)
			{
				if (logicalPosition + bytes > physicalLength)
					return BAD_WRITE_ERR;
				memcpy(fileImage + logicalPosition, buffer, bytes);
				result = bytes;
			}
			else
			{
				result = _write(handle, buffer, bytes);
				if (result != length)
					lastError = errno;
			}
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	else
	{
		lastError = CANT_WRITE_TO_CHILD;
	}
	return (result);
}

//---------------------------------------------------------------------------
int32_t File::writeByte(byte value)
{
	int32_t result = 0;
	if (parent == nullptr)
	{
		if (isOpen())
		{
			if (inRAM)
			{
				if (logicalPosition + sizeof(byte) > physicalLength)
					return BAD_WRITE_ERR;
				memcpy(fileImage + logicalPosition, &value, sizeof(byte));
				result = sizeof(byte);
			}
			else
				result = _write(handle, (&value), sizeof(value));
			if (result == sizeof(value))
			{
				logicalPosition += sizeof(value);
				result = NO_ERROR;
			}
			else
			{
				result = BAD_WRITE_ERR;
			}
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	else
	{
		lastError = CANT_WRITE_TO_CHILD;
	}
	return (result);
}

//---------------------------------------------------------------------------
int32_t File::writeWord(int16_t value)
{
	uint32_t result = 0;
	if (parent == nullptr)
	{
		if (isOpen())
		{
			if (inRAM)
			{
				if (logicalPosition + sizeof(int16_t) > physicalLength)
					return BAD_WRITE_ERR;
				memcpy(fileImage + logicalPosition, &value, sizeof(int16_t));
				result = sizeof(value);
			}
			else
				result = _write(handle, (&value), sizeof(value));
			if (result == sizeof(value))
			{
				logicalPosition += sizeof(value);
				result = NO_ERROR;
			}
			else
			{
				result = BAD_WRITE_ERR;
			}
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	else
	{
		lastError = CANT_WRITE_TO_CHILD;
	}
	return (result);
}

//---------------------------------------------------------------------------
int32_t File::writeShort(int16_t value)
{
	int32_t result = writeWord(value);
	return (result);
}

//---------------------------------------------------------------------------
int32_t File::writeLong(int32_t value)
{
	uint32_t result = 0;
	if (parent == nullptr)
	{
		if (isOpen())
		{
			if (inRAM)
			{
				if (logicalPosition + sizeof(value) > physicalLength)
					return BAD_WRITE_ERR;
				memcpy(fileImage + logicalPosition, &value, sizeof(value));
				result = sizeof(value);
			}
			else
				result = _write(handle, (&value), sizeof(value));
			if (result == sizeof(value))
			{
				logicalPosition += sizeof(value);
				result = NO_ERROR;
			}
			else
			{
				result = BAD_WRITE_ERR;
			}
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	else
	{
		lastError = CANT_WRITE_TO_CHILD;
	}
	return (result);
}

//---------------------------------------------------------------------------
int32_t File::writeFloat(float value)
{
	uint32_t result = 0;
	gosASSERT(!isNAN(&value));
	if (parent == nullptr)
	{
		if (isOpen())
		{
			if (inRAM)
			{
				if (logicalPosition + sizeof(value) > physicalLength)
					return BAD_WRITE_ERR;
				memcpy(fileImage + logicalPosition, &value, sizeof(value));
				result = sizeof(value);
			}
			else
				result = _write(handle, (&value), sizeof(float));
			if (result == sizeof(float))
			{
				logicalPosition += sizeof(float);
				result = NO_ERROR;
			}
			else
			{
				result = BAD_WRITE_ERR;
			}
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	else
	{
		lastError = CANT_WRITE_TO_CHILD;
	}
	return (result);
}

//---------------------------------------------------------------------------

int32_t File::writeString(PSTR buffer)
{
	int32_t result = -1;
	if (parent == nullptr)
	{
		if (isOpen())
		{
			PSTR ch = buffer;
			for (; *ch; ++ch)
				writeByte((byte)*ch);
			return ch - buffer;
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	else
	{
		lastError = CANT_WRITE_TO_CHILD;
	}
	return (result);
}

//---------------------------------------------------------------------------
int32_t File::writeLine(PSTR buffer)
{
	int32_t result = -1;
	if (parent == nullptr)
	{
		if (isOpen())
		{
			PSTR ch = buffer;
			for (; *ch; ++ch)
				writeByte((byte)*ch);
			writeByte('\r');
			writeByte('\n');
			return ch - buffer;
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	else
	{
		lastError = CANT_WRITE_TO_CHILD;
	}
	return (result);
}

//---------------------------------------------------------------------------
int32_t File::write(puint8_t buffer, int32_t bytes)
{
	int32_t result = 0;
	if (parent == nullptr)
	{
		if (isOpen())
		{
			if (inRAM)
			{
				if (logicalPosition + bytes > physicalLength)
					return BAD_WRITE_ERR;
				memcpy(fileImage + logicalPosition, buffer, bytes);
				result = bytes;
			}
			else
			{
				result = _write(handle, buffer, bytes);
				if (result != bytes)
				{
					lastError = errno;
					return result;
				}
			}
			logicalPosition += result;
		}
		else
		{
			lastError = FILE_NOT_OPEN;
		}
	}
	else
	{
		lastError = CANT_WRITE_TO_CHILD;
	}
	return result;
}

//---------------------------------------------------------------------------
bool File::isOpen(void)
{
	return ((handle != nullptr && handle != -1) || (fileImage != nullptr));
}

//---------------------------------------------------------------------------
PSTR File::getFilename(void) { return (fileName); }

//---------------------------------------------------------------------------
time_t File::getFileMTime(void)
{
	time_t mTime = 0;
	if (isOpen())
	{
		struct _stat st;
		_fstat(handle, &st);
		mTime = st.st_mtime;
		// Time\Date Stamp is WAY out of line.
		// Return January 1, 1970
		if (mTime == -1)
			mTime = 0;
	}
	return mTime;
}

//---------------------------------------------------------------------------
uint32_t File::getLength(void)
{
	if (fastFile && (length == 0))
	{
		length = fastFile->sizeFast(fastFileHandle);
	}
	else if ((length == 0) && (parent || inRAM))
	{
		length = physicalLength;
	}
	else if (isOpen() && ((length == 0) || (fileMode > READ) && !inRAM))
	{
		/* _fstat() was being used to get the length of the file, but it was
		   wrong. It was not giving the *logical* size, which is what we want.
		 */
		length = _filelength(handle);
	}
	return length;
}

//---------------------------------------------------------------------------
uint32_t File::fileSize(void) { return getLength(); }

//---------------------------------------------------------------------------
uint32_t File::getNumLines(void)
{
	uint32_t currentPos = logicalPosition;
	uint32_t numLines   = 0;
	seek(0);
	for (uint32_t i = 0; i < getLength(); i++)
	{
		uint8_t check1 = readByte();
		if (check1 == '\n')
			numLines++;
	}
	seek(currentPos);
	return numLines;
}

//---------------------------------------------------------------------------
void File::seekEnd(void) { seek(0, SEEK_END); }

//---------------------------------------------------------------------------
void File::skip(int32_t bytesToSkip)
{
	if (bytesToSkip)
	{
		seek(logicalPosition + bytesToSkip);
	}
}
//---------------------------------------------------------------------------
