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

//#include "dstd.h"
//#include "dinifile.h"
//#include "file.h"

//---------------------------------------------------------------------------
// Macro Definitions

//---------------------------------------------------------------------------
// enums
typedef enum __inifile_constants {
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
	size_t 			totalBlocks;			//Total number of blocks in file
	IniBlockNode*	fileBlocks;				//Data for blocks to speed up file
	PSTR			currentBlockId;			//Id of current block
	size_t 			currentBlockOffset;		//Offset into file of block start
	size_t 			currentBlockSize;		//Length of current block

	// Member Functions
	//------------------
protected:
	int32_t afterOpen (void);
	void atClose (void);

	int32_t findNextBlockStart (PSTR line = NULL, size_t lineLen = 0);
	int32_t countBlocks (void);

	int32_t getNextWord (PSTR& line, PSTR buffer, size_t bufLen);

	float textToFloat (PCSTR num);
	double textToDouble (PCSTR num);

	int32_t textToLong (PCSTR num);
	size_t textToULong (PCSTR num);

	int16_t textToShort (PCSTR num);
	uint16_t textToUShort (PCSTR num);

	char textToChar (PCSTR num);
	uint8_t textToUChar (PCSTR num);

	bool booleanToLong (PCSTR num);

	float mathToFloat (PCSTR num);

	int32_t mathToLong (PCSTR num);
	size_t mathToULong (PCSTR num);

	int16_t mathToShort (PCSTR num);
	uint16_t mathToUShort (PCSTR num);

	char mathToChar (PCSTR num);
	uint8_t mathToUChar (PCSTR num);

	int32_t floatToText (PSTR result, float num, size_t bufLen);

	int32_t longToTextDec (PSTR result, int32_t num, size_t bufLen);
	int32_t longToTextHex (PSTR result, int32_t num, size_t bufLen);

	int32_t shortToTextDec (PSTR result, int16_t num, size_t bufLen);
	int32_t shortToTextHex (PSTR result, int16_t num, size_t bufLen);

	int32_t byteToTextDec (PSTR result, byte num, size_t bufLen);	
	int32_t byteToTextHex (PSTR result, byte num, size_t bufLen);

	int32_t copyString (PSTR dest, PSTR src, size_t bufLen);

public:
	FitIniFile (void);
	~FitIniFile (void);

	virtual int32_t open(PCSTR fName, FileMode _mode = READ, uint32_t numChildren = 50);
	virtual int32_t open(FilePtr _parent, size_t fileSize, uint32_t numChildren = 50);

	virtual int32_t create(PCSTR fName);
	virtual int32_t createWithCase(PSTR fName);

	virtual void close (void);

	virtual FileClass getFileClass (void)
	{
		return INIFILE;
	}

	HRESULT seekBlock(PCSTR blockId);

	int32_t readIdFloat (PCSTR varName, float &value);
	int32_t readIdDouble (PCSTR varName, double &value);

	int32_t readIdBoolean (PCSTR varName, bool& value);
	int32_t readIdLong (PCSTR varName, int32_t& value);
	int32_t readIdULong (PCSTR varName, uint32_t& value);

	int32_t readIdShort (PCSTR varName, int16_t& value);
	int32_t readIdUShort (PCSTR varName, uint16_t& value);

	int32_t readIdChar (PCSTR varName, int8_t& value);
	int32_t readIdUChar (PCSTR varName, uint8_t& value);

	int32_t readIdString (PCSTR varName, PSTR result, size_t bufferSize);

	int32_t getIdStringLength (PCSTR varName);

	int32_t readIdFloatArray (PCSTR varName, float* result, size_t numElements);

	int32_t readIdLongArray (PCSTR varName, pint32_t result, size_t numElements);
	int32_t readIdULongArray (PCSTR varName, puint32_t result, size_t numElements);

	int32_t readIdShortArray (PCSTR varName, pint16_t result, size_t numElements);
	int32_t readIdUShortArray (PCSTR varName, puint16_t result, size_t numElements);

	int32_t readIdCharArray (PCSTR varName, PSTR result, size_t numElements);
	int32_t readIdUCharArray (PCSTR varName, puint8_t result, size_t numElements);

	size_t getIdFloatArrayElements (PCSTR varName);

	size_t getIdLongArrayElements (PCSTR varName);
	size_t getIdULongArrayElements (PCSTR varName);

	size_t getIdShortArrayElements (PCSTR varName);
	size_t getIdUShortArrayElements (PCSTR varName);

	size_t getIdCharArrayElements (PCSTR varName);
	size_t getIdUCharArrayElements (PCSTR varName);

	int32_t writeBlock (PCSTR blockId);

	int32_t writeIdFloat (PCSTR varName, float value);

	int32_t writeIdBoolean (PCSTR varName, bool value);
	int32_t writeIdLong (PCSTR varName, int32_t value);
	int32_t writeIdULong (PCSTR varName, uint32_t value);

	int32_t writeIdShort (PCSTR varName, int16_t value);
	int32_t writeIdUShort (PCSTR varName, uint16_t value);

	int32_t writeIdChar (PCSTR varName, int8_t value);
	int32_t writeIdUChar (PCSTR varName, uint8_t value);

	int32_t writeIdString (PCSTR varName, PCSTR result);

	int32_t writeIdFloatArray (PCSTR varName, float* array, size_t numElements);
	int32_t writeIdLongArray (PCSTR varName, pint32_t array, size_t numElements);
	int32_t writeIdUShortArray (PCSTR varName, puint16_t array, size_t numElements);
	int32_t writeIdUCharArray (PCSTR varName, puint8_t array, size_t numElements);
};

//---------------------------------------------------------------------------
#endif
