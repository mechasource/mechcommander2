//---------------------------------------------------------------------------
//
// inifile.h - This file contains the class declaration for the FitIni Files
//
//				The FitIni file is a ini style file.
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef INIFILE_H
#define INIFILE_H

/* !!! to be replaced by XML (using xmllite) !!! */

//#include "dstd.h"
//#include "dinifile.h"
#include "file.h"

//---------------------------------------------------------------------------
// Macro Definitions

//---------------------------------------------------------------------------
// enums
enum __inifile_constants : uint32_t
{
	BLOCK_NOT_FOUND = 0xFADA0000,
	ID_NOT_FOUND = 0xFADA0001,
	DATA_NOT_CORRECT_TYPE = 0xFADA0002,
	BUFFER_TOO_SMALL = 0xFADA0003,
	NOT_A_FITINIFILE = 0xFADA0004,
	NO_RAM_FOR_INI_BLOCKS = 0xFADA0005,
	NO_MORE_BLOCKS = 0xFADA0006,
	TOO_MANY_BLOCKS = 0xFADA0007,
	NOT_ENOUGH_BLOCKS = 0xFADA0008,
	VARIABLE_NOT_FOUND = 0xFADA0009,
	SYNTAX_ERROR = 0xFADA000A,
	NOT_ENOUGH_ELEMENTS_FOR_ARRAY = 0xFADA000B,
	GET_NEXT_LINE = 0xFADA000C,
	USER_ARRAY_TOO_SMALL = 0xFADA000D,
	TOO_MANY_ELEMENTS = 0xFADA000E,
};

/*

!!! NOTE: This is a quick fix for FitIniFile due to its former memory handling
!!! Perhaps write a new parser using std C++ OR replace it with a json parser

*/

//---------------------------------------------------------------------------
// Structs
struct IniBlockNode
{
	wchar_t blockId[50];
	size_t blockOffset;
};

//---------------------------------------------------------------------------
//									FitIniFile
class FitIniFile
	: public MechFile<FitIniFile>
{
public:
	FitIniFile(void) noexcept
		:
	{
	}
	virtual ~FitIniFile(void) noexcept
	{
		close();
	}

	// Member Functions
	//------------------
public:
	virtual int32_t open(std::wstring_view filename, FileMode _mode = READ, uint32_t numChildren = 50);
	virtual int32_t open(std::unique_ptr<File> _parent, size_t fileSize, uint32_t numChildren = 50);

	virtual int32_t create(std::wstring_view filename);
	virtual int32_t createWithCase(std::wstring_view filename);

	virtual void close(void);

	virtual FileClass getFileClass(void)
	{
		return FileClass::inifile;
	}

	HRESULT seekBlock(std::wstring_view blockId);

	HRESULT readIdFloat(std::wstring_view varName, float& value);
	HRESULT readIdDouble(std::wstring_view varName, double& value);
	HRESULT readIdBoolean(std::wstring_view varName, bool& value);
	HRESULT readIdLong(std::wstring_view varName, long32_t& value);
	HRESULT readIdULong(std::wstring_view varName, ULONG& value);
	HRESULT readIdInt(std::wstring_view varName, int32_t& value);
	HRESULT readIdUInt(std::wstring_view varName, uint32_t& value);
	HRESULT readIdShort(std::wstring_view varName, int16_t& value);
	HRESULT readIdUShort(std::wstring_view varName, uint16_t& value);
	HRESULT readIdChar(std::wstring_view varName, int8_t& value);
	HRESULT readIdUChar(std::wstring_view varName, uint8_t& value);
	HRESULT readIdString(std::wstring_view varName, std::wstring_view result, size_t bufferSize);
	HRESULT getIdStringLength(std::wstring_view varName);
	HRESULT readIdFloatArray(std::wstring_view varName, float* result, size_t numElements);
	HRESULT readIdLongArray(std::wstring_view varName, int32_t* result, size_t numElements);
	HRESULT readIdULongArray(std::wstring_view varName, uint32_t* result, size_t numElements);
	HRESULT readIdShortArray(std::wstring_view varName, int16_t* result, size_t numElements);
	HRESULT readIdUShortArray(std::wstring_view varName, uint16_t* result, size_t numElements);
	HRESULT readIdCharArray(std::wstring_view varName, int8_t* result, size_t numElements);
	HRESULT readIdUCharArray(std::wstring_view varName, uint8_t* result, size_t numElements);

	size_t getIdFloatArrayElements(std::wstring_view varName);
	size_t getIdLongArrayElements(std::wstring_view varName);
	size_t getIdULongArrayElements(std::wstring_view varName);
	size_t getIdShortArrayElements(std::wstring_view varName);
	size_t getIdUShortArrayElements(std::wstring_view varName);
	size_t getIdCharArrayElements(std::wstring_view varName);
	size_t getIdUCharArrayElements(std::wstring_view varName);

	HRESULT writeBlock(std::wstring_view blockId);
	HRESULT writeIdFloat(std::wstring_view varName, float value);
	HRESULT writeIdBoolean(std::wstring_view varName, bool value);
	HRESULT writeIdLong(std::wstring_view varName, int32_t value);
	HRESULT writeIdULong(std::wstring_view varName, uint32_t value);
	HRESULT writeIdShort(std::wstring_view varName, int16_t value);
	HRESULT writeIdUShort(std::wstring_view varName, uint16_t value);
	HRESULT writeIdChar(std::wstring_view varName, int8_t value);
	HRESULT writeIdUChar(std::wstring_view varName, uint8_t value);
	HRESULT writeIdString(std::wstring_view varName, std::wstring_view result);
	HRESULT writeIdFloatArray(std::wstring_view varName, float* parray, size_t numElements);
	HRESULT writeIdLongArray(std::wstring_view varName, int32_t* parray, size_t numElements);
	HRESULT writeIdUShortArray(std::wstring_view varName, uint16_t* parray, size_t numElements);
	HRESULT writeIdUCharArray(std::wstring_view varName, uint8_t* parray, size_t numElements);

protected:
	HRESULT afterOpen(void);
	void atClose(void);

	HRESULT findNextBlockStart(std::wstring_view line = nullptr, size_t lineLen = 0);
	size_t countBlocks(void);

	HRESULT getNextWord(std::wstring_view& line, std::wstring_view buffer, size_t bufLen);

	float textToFloat(std::wstring_view num);
	double textToDouble(std::wstring_view num);

	int32_t textToLong(std::wstring_view num);
	uint32_t textToULong(std::wstring_view num);

	int16_t textToShort(std::wstring_view num);
	uint16_t textToUShort(std::wstring_view num);

	int8_t textToChar(std::wstring_view num);
	uint8_t textToUChar(std::wstring_view num);

	bool booleanToLong(std::wstring_view num);
	float mathToFloat(std::wstring_view num);
	int32_t mathToLong(std::wstring_view num);
	size_t mathToULong(std::wstring_view num);
	int16_t mathToShort(std::wstring_view num);
	uint16_t mathToUShort(std::wstring_view num);
	wchar_t mathToChar(std::wstring_view num);
	uint8_t mathToUChar(std::wstring_view num);
	int32_t floatToText(std::wstring_view result, float num, size_t bufLen);
	int32_t longToTextDec(std::wstring_view result, int32_t num, size_t bufLen);
	int32_t longToTextHex(std::wstring_view result, int32_t num, size_t bufLen);
	int32_t shortToTextDec(std::wstring_view result, int16_t num, size_t bufLen);
	int32_t shortToTextHex(std::wstring_view result, int16_t num, size_t bufLen);
	int32_t byteToTextDec(std::wstring_view result, byte num, size_t bufLen);
	int32_t byteToTextHex(std::wstring_view result, byte num, size_t bufLen);
	int32_t copyString(std::wstring_view dest, std::wstring_view src, size_t bufLen);

	// Data Members
	//--------------
protected:
	std::unique_ptr<IniBlockNode> m_fileBlocks; // Data for blocks to speed up file
	std::wstring_view m_currentBlockId; // Id of current block
	size_t m_totalBlocks = 0; // Total number of blocks in file
	size_t m_currentBlockOffset = 0; // Offset into file of block start
	size_t m_currentBlockSize = 0; // Length of current block
};

//---------------------------------------------------------------------------
#endif
