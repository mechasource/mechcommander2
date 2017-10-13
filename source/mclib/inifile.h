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
#include <file.h>

//---------------------------------------------------------------------------
// Macro Definitions

//---------------------------------------------------------------------------
// enums
typedef enum __inifile_constants
{
	BLOCK_NOT_FOUND						= 0xFADA0000,
	ID_NOT_FOUND						= 0xFADA0001,
	DATA_NOT_CORRECT_TYPE				= 0xFADA0002,
	BUFFER_TOO_SMALL					= 0xFADA0003,
	NOT_A_FITINIFILE					= 0xFADA0004,
	NO_RAM_FOR_INI_BLOCKS				= 0xFADA0005,
	NO_MORE_BLOCKS						= 0xFADA0006,
	TOO_MANY_BLOCKS						= 0xFADA0007,
	NOT_ENOUGH_BLOCKS					= 0xFADA0008,
	VARIABLE_NOT_FOUND					= 0xFADA0009,
	SYNTAX_ERROR						= 0xFADA000A,
	NOT_ENOUGH_ELEMENTS_FOR_ARRAY		= 0xFADA000B,
	GET_NEXT_LINE						= 0xFADA000C,
	USER_ARRAY_TOO_SMALL				= 0xFADA000D,
	TOO_MANY_ELEMENTS					= 0xFADA000E,
};

/*

!!! NOTE: This is a quick fix for FitIniFile due to its former memory handling
!!! Perhaps write a new parser using std C++ OR replace it with a json parser

*/

//---------------------------------------------------------------------------
// Structs
struct IniBlockNode
{
	char blockId[50];
	size_t blockOffset;
};

//---------------------------------------------------------------------------
//									FitIniFile
class FitIniFile : public File
{
	// Data Members
	//--------------
protected:
	std::unique_ptr<IniBlockNode>	m_fileBlocks;			//Data for blocks to speed up file
	PSTR							m_currentBlockId;		//Id of current block
	size_t							m_totalBlocks;			//Total number of blocks in file
	size_t 							m_currentBlockOffset;	//Offset into file of block start
	size_t 							m_currentBlockSize;		//Length of current block

public:
	FitIniFile(void) : m_currentBlockId(nullptr),
		m_totalBlocks(0), m_currentBlockOffset(0), m_currentBlockSize(0)
	{
	}
	virtual ~FitIniFile(void)
	{
		close();
	}

	// Member Functions
	//------------------
protected:
	HRESULT afterOpen(void);
	void atClose(void);

	HRESULT findNextBlockStart(PSTR line = nullptr, size_t lineLen = 0);
	size_t countBlocks(void);

	HRESULT getNextWord(PSTR& line, PSTR buffer, size_t bufLen);

	float textToFloat(PCSTR num);
	double textToDouble(PCSTR num);

	int32_t textToLong(PCSTR num);
	uint32_t textToULong(PCSTR num);

	int16_t textToShort(PCSTR num);
	uint16_t textToUShort(PCSTR num);

	int8_t textToChar(PCSTR num);
	uint8_t textToUChar(PCSTR num);

	bool booleanToLong(PCSTR num);

	float mathToFloat(PCSTR num);

	int32_t mathToLong(PCSTR num);
	size_t mathToULong(PCSTR num);

	int16_t mathToShort(PCSTR num);
	uint16_t mathToUShort(PCSTR num);

	char mathToChar(PCSTR num);
	uint8_t mathToUChar(PCSTR num);

	int32_t floatToText(PSTR result, float num, size_t bufLen);

	int32_t longToTextDec(PSTR result, int32_t num, size_t bufLen);
	int32_t longToTextHex(PSTR result, int32_t num, size_t bufLen);

	int32_t shortToTextDec(PSTR result, int16_t num, size_t bufLen);
	int32_t shortToTextHex(PSTR result, int16_t num, size_t bufLen);

	int32_t byteToTextDec(PSTR result, byte num, size_t bufLen);
	int32_t byteToTextHex(PSTR result, byte num, size_t bufLen);

	int32_t copyString(PSTR dest, PSTR src, size_t bufLen);

public:

	virtual int32_t open(PCSTR fName, FileMode _mode = READ, uint32_t numChildren = 50);
	virtual int32_t open(FilePtr _parent, size_t fileSize, uint32_t numChildren = 50);

	virtual int32_t create(PCSTR fName);
	virtual int32_t createWithCase(PSTR fName);

	virtual void close(void);

	virtual FileClass getFileClass(void)
	{
		return INIFILE;
	}

	HRESULT seekBlock(PCSTR blockId);

	HRESULT readIdFloat(PCSTR varName, float& value);
	HRESULT readIdDouble(PCSTR varName, double& value);

	HRESULT readIdBoolean(PCSTR varName, bool& value);
	HRESULT readIdLong(PCSTR varName, int32_t& value);
	HRESULT readIdULong(PCSTR varName, uint32_t& value);

	HRESULT readIdShort(PCSTR varName, int16_t& value);
	HRESULT readIdUShort(PCSTR varName, uint16_t& value);

	HRESULT readIdChar(PCSTR varName, int8_t& value);
	HRESULT readIdUChar(PCSTR varName, uint8_t& value);

	HRESULT readIdString(PCSTR varName, PSTR result, size_t bufferSize);

	HRESULT getIdStringLength(PCSTR varName);

	HRESULT readIdFloatArray(PCSTR varName, float* result, size_t numElements);

	HRESULT readIdLongArray(PCSTR varName, pint32_t result, size_t numElements);
	HRESULT readIdULongArray(PCSTR varName, puint32_t result, size_t numElements);

	HRESULT readIdShortArray(PCSTR varName, pint16_t result, size_t numElements);
	HRESULT readIdUShortArray(PCSTR varName, puint16_t result, size_t numElements);

	HRESULT readIdCharArray(PCSTR varName, pint8_t result, size_t numElements);
	HRESULT readIdUCharArray(PCSTR varName, puint8_t result, size_t numElements);

	size_t getIdFloatArrayElements(PCSTR varName);

	size_t getIdLongArrayElements(PCSTR varName);
	size_t getIdULongArrayElements(PCSTR varName);

	size_t getIdShortArrayElements(PCSTR varName);
	size_t getIdUShortArrayElements(PCSTR varName);

	size_t getIdCharArrayElements(PCSTR varName);
	size_t getIdUCharArrayElements(PCSTR varName);

	HRESULT writeBlock(PCSTR blockId);

	HRESULT writeIdFloat(PCSTR varName, float value);

	HRESULT writeIdBoolean(PCSTR varName, bool value);
	HRESULT writeIdLong(PCSTR varName, int32_t value);
	HRESULT writeIdULong(PCSTR varName, uint32_t value);

	HRESULT writeIdShort(PCSTR varName, int16_t value);
	HRESULT writeIdUShort(PCSTR varName, uint16_t value);

	HRESULT writeIdChar(PCSTR varName, int8_t value);
	HRESULT writeIdUChar(PCSTR varName, uint8_t value);

	HRESULT writeIdString(PCSTR varName, PCSTR result);

	HRESULT writeIdFloatArray(PCSTR varName, float* parray, size_t numElements);
	HRESULT writeIdLongArray(PCSTR varName, pint32_t parray, size_t numElements);
	HRESULT writeIdUShortArray(PCSTR varName, puint16_t parray, size_t numElements);
	HRESULT writeIdUCharArray(PCSTR varName, puint8_t parray, size_t numElements);
};

//---------------------------------------------------------------------------
#endif
