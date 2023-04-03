//---------------------------------------------------------------------------
//
// file.h - This file contains the class declaration for File
//
//				The File class simply calls the Windows 32bit file functions.
//				It is purely a wrapper.
//
//				The mmFile Class is a wrapper for the Win32 Memory Mapped
//				file functionality.  It is used exactly the same as above class.
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

//---------------------------------------------------------------------------

#if CONSIDERED_OBSOLETE
enum class FileMode : uint8_t
{
	NOMODE = 0, // notused
	READ,
	CREATE,
	MC2_APPEND, // notused
	WRITE,
	RDWRITE
};
#endif

enum class FileClass : uint8_t
{
	basefile,
	inifile,
	packetfile,
	csvfile
};

//---------------------------------------------------------------------------
// Macro Definitions

enum __file_constants : uint32_t
{
	DISK_FULL_ERR = 0xBADF0001,
	SHARE_ERR = 0xBADF0002,
	FILE_NOT_FOUND_ERR = 0xBADF0003,
	PACKET_OUT_OF_RANGE = 0xBADF0004,
	PACKET_WRONG_SIZE = 0xBADF0005,
	READ_ONLY_ERR = 0xBADF0006,
	TOO_MANY_FILES_ERR = 0xBADF0007,
	READ_PAST_EOF_ERR = 0xBADF0008,
	INVALID_SEEK_ERR = 0xBADF0009,
	BAD_WRITE_ERR = 0xBADF000A,
	BAD_PACKET_VERSION = 0xBADF000B,
	NO_RAM_FOR_SEEK_TABLE = 0xBADF000C,
	NO_RAM_FOR_FILENAME = 0xBADF000D,
	PARENT_NULL = 0xBADF000E,
	TOO_MANY_CHILDREN = 0xBADF000F,
	FILE_NOT_OPEN = 0xBADF0010,
	CANT_WRITE_TO_CHILD = 0xBADF0011,
	NO_RAM_FOR_CHILD_LIST = 0xBADF0012,
	MAPPED_WRITE_NOT_SUPPORTED = 0xBADF0013,
	COULD_NOT_MAP_FILE = 0xBADF0014,
};

//---------------------------------------------------------------------------
// Function Declarations
// Returns 1 if file is on HardDrive and 2 if file is in FastFile
// int32_t __stdcall fileExists(std::wstring_view filename);
// int32_t __stdcall fileExistsOnCD(std::wstring_view filename);
// bool __stdcall file1OlderThan2(std::wstring_view file1, std::wstring_view file2);

// class UserHeap;
// class FastFile;
// class File;
// typedef File* std::unique_ptr<File>;

//---------------------------------------------------------------------------
//									File
//---------------------------------------------------------------------------

template <class Tderived>
class ATL_NO_VTABLE MechFile
{

public:
	// static bool logFileTraffic;
	// static HRESULT lastError;

	// Member Functions
	//------------------
public:
	// PVOID operator new(size_t mySize);
	// void operator delete(PVOID us);

	MechFile(void) noexcept
	{
	}
	virtual ~MechFile(void) noexcept = default;

	virtual HRESULT open(stdfs::path& path);
	virtual HRESULT open(std::wstring_view filename)
	{
		stdfs::path path(filename);
		return open(path);
	}

	// virtual HRESULT open(std::wstring_view filename, FileMode _mode = READ, uint32_t numChildren = 50);
	// virtual int32_t open(std::unique_ptr<File> _parent, size_t fileSize, uint32_t numChildren = 50);
	///*virtual*/ int32_t open(std::wstring_view buffer, size_t bufferLength); // for streaming from memory

	virtual int32_t create(std::wstring_view filename);
	virtual int32_t createWithCase(std::wstring_view filename);

	virtual void close(void);

	void deleteFile(void);

	int32_t seek(int32_t pos, int32_t from = SEEK_SET);
	void seekEnd(void);
	void skip(int32_t bytesToSkip);
	// bool eof(void) { return (logicalPosition >= getLength()); }

	int32_t read(size_t pos, uint8_t* buffer, size_t length);
	int32_t read(uint8_t* buffer, size_t length);

	// Used to dig the LZ data directly out of the fastfiles.
	// For textures.
	int32_t readRAW(size_t*& buffer /*, UserHeap* heap*/);

	uint8_t readByte(void);
	int16_t readWord(void);
	int16_t readShort(void);
	int32_t readLong(void);
	float readFloat(void);

	int32_t readString(uint8_t* buffer);
	int32_t readLine(uint8_t* buffer, size_t maxLength);
	int32_t readLineEx(uint8_t* buffer, size_t maxLength);

	int32_t write(size_t pos, uint8_t* buffer, size_t bytes);
	int32_t write(uint8_t* buffer, size_t bytes);

	int32_t writeByte(uint8_t value);
	int32_t writeWord(int16_t value);
	int32_t writeShort(int16_t value);
	int32_t writeLong(int32_t value);
	int32_t writeFloat(float value);

	int32_t writeString(std::wstring_view buffer);
	int32_t writeLine(std::wstring_view buffer);

	bool isOpen(void);

	virtual FileClass getFileClass(void)
	{
		return FileClass::basefile;
	}

	std::wstring_view getFilename(void);

	size_t getLength(void);
	size_t fileSize(void);
	size_t getNumLines(void);

	// HRESULT getLastError(void) { return (lastError); }
	// size_t getLogicalPosition(void) { return logicalPosition; }
	// std::unique_ptr<File> getParent(void) { return m_parent; }
	// FileMode getFileMode(void) { return (fileMode); }
	// int32_t getFileHandle(void) { return (handle); }
	time_t getFileMTime(void);
	// int32_t addChild(std::unique_ptr<File> child);
	// void removeChild(std::unique_ptr<File> child);

protected:
	void setup(void);

protected:
	std::wstring_view m_fileName;
	stdfs::path m_path;
	std::fstream m_stream;
	std::unique_ptr<MechFile> m_parent;

	// fileMode		= NOMODE;
	// handle			= -1;
	// length			= 0;
	// logicalPosition = 0;
	// bufferResult	= 0;
	// parent			= nullptr;
	// parentOffset	= 0;
	// physicalLength  = 0;
	// childList		= nullptr;
	// numChildren		= 0;
	// inRAM			= false;
	// fileImage		= nullptr;
	// fastFile		= nullptr;

	// FileMode fileMode;
	// int32_t handle;
	// FastFile* fastFile;
	// int32_t fastFileHandle;
	// size_t length;
	// size_t logicalPosition;
	// size_t bufferResult;
	// std::unique_ptr<File>* childList;
	// size_t numChildren;
	// size_t maxChildren;
	// std::unique_ptr<File> parent;
	// ptrdiff_t parentOffset;
	// size_t physicalLength;
	// bool inRAM;
	// uint8_t* fileImage;
};
