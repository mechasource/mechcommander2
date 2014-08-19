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
#ifndef 	NO_ERR
#define	NO_ERR						0x00000000
#endif

#define BLOCK_NOT_FOUND						0xFADA0000
#define ID_NOT_FOUND							0xFADA0001
#define DATA_NOT_CORRECT_TYPE				0xFADA0002
#define BUFFER_TOO_SMALL					0xFADA0003
#define NOT_A_FITINIFILE					0xFADA0004
#define NO_RAM_FOR_INI_BLOCKS				0xFADA0005
#define NO_MORE_BLOCKS						0xFADA0006
#define TOO_MANY_BLOCKS						0xFADA0007
#define NOT_ENOUGH_BLOCKS					0xFADA0008
#define VARIABLE_NOT_FOUND					0xFADA0009
#define SYNTAX_ERROR							0xFADA000A
#define NOT_ENOUGH_ELEMENTS_FOR_ARRAY	0xFADA000B
#define GET_NEXT_LINE						0xFADA000C
#define USER_ARRAY_TOO_SMALL				0xFADA000D
#define TOO_MANY_ELEMENTS					0xFADA000E

//---------------------------------------------------------------------------
// Enums

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
	long afterOpen (void);
	void atClose (void);

	long findNextBlockStart (PSTR line = NULL, size_t lineLen = 0);
	long countBlocks (void);

	long getNextWord (PSTR &line, PSTR buffer, size_t bufLen);

	float textToFloat (PCSTR num);
	double textToDouble (PCSTR num);

	long textToLong (PCSTR num);
	size_t textToULong (PCSTR num);

	short textToShort (PCSTR num);
	unsigned short textToUShort (PCSTR num);

	char textToChar (PCSTR num);
	uint8_t textToUChar (PCSTR num);

	bool booleanToLong (PCSTR num);

	float mathToFloat (PCSTR num);

	long mathToLong (PCSTR num);
	size_t mathToULong (PCSTR num);

	short mathToShort (PCSTR num);
	unsigned short mathToUShort (PCSTR num);

	char mathToChar (PCSTR num);
	uint8_t mathToUChar (PCSTR num);

	long floatToText (PSTR result, float num, size_t bufLen);

	long longToTextDec (PSTR result, long num, size_t bufLen);
	long longToTextHex (PSTR result, long num, size_t bufLen);

	long shortToTextDec (PSTR result, short num, size_t bufLen);
	long shortToTextHex (PSTR result, short num, size_t bufLen);

	long byteToTextDec (PSTR result, byte num, size_t bufLen);	
	long byteToTextHex (PSTR result, byte num, size_t bufLen);

	long copyString (PSTR dest, PSTR src, size_t bufLen);

public:
	FitIniFile (void);
	~FitIniFile (void);

	virtual long open (PCSTR fName, FileMode _mode = READ, long numChildren = 50);
	virtual long open (FilePtr _parent, size_t fileSize, long numChildren = 50);

	virtual long create (PSTR fName);
	virtual long createWithCase( PSTR fName );


	virtual void close (void);

	virtual FileClass getFileClass (void)
	{
		return INIFILE;
	}

	long seekBlock (PCSTR blockId);

	long readIdFloat (PCSTR varName, float &value);
	long readIdDouble (PCSTR varName, double &value);

	long readIdBoolean (PCSTR varName, bool &value);
	long readIdLong (PCSTR varName, long &value);
	long readIdULong (PCSTR varName, size_t &value);

	long readIdShort (PCSTR varName, short &value);
	long readIdUShort (PCSTR varName, unsigned short &value);

	long readIdChar (PCSTR varName, char &value);
	long readIdUChar (PCSTR varName, uint8_t &value);

	long readIdString (PCSTR varName, PSTR result, size_t bufferSize);

	long getIdStringLength (PCSTR varName);

	long readIdFloatArray (PCSTR varName, float *result, size_t numElements);

	long readIdLongArray (PCSTR varName, long *result, size_t numElements);
	long readIdULongArray (PCSTR varName, size_t *result, size_t numElements);

	long readIdShortArray (PCSTR varName, short *result, size_t numElements);
	long readIdUShortArray (PCSTR varName, unsigned short *result, size_t numElements);

	long readIdCharArray (PCSTR varName, PSTR result, size_t numElements);
	long readIdUCHARArray (PCSTR varName, PUCHAR result, size_t numElements);

	size_t getIdFloatArrayElements (PCSTR varName);

	size_t getIdLongArrayElements (PCSTR varName);
	size_t getIdULongArrayElements (PCSTR varName);

	size_t getIdShortArrayElements (PCSTR varName);
	size_t getIdUShortArrayElements (PCSTR varName);

	size_t getIdCharArrayElements (PCSTR varName);
	size_t getIdUCHARArrayElements (PCSTR varName);

	long writeBlock (PCSTR blockId);

	long writeIdFloat (PCSTR varName, float value);

	long writeIdBoolean (PCSTR varName, bool value);
	long writeIdLong (PCSTR varName, long value);
	long writeIdULong (PCSTR varName, size_t value);

	long writeIdShort (PCSTR varName, short value);
	long writeIdUShort (PCSTR varName, unsigned short value);

	long writeIdChar (PCSTR varName, char value);
	long writeIdUCHAR (PCSTR varName, uint8_t value);

	long writeIdString (PCSTR varName, PCSTR result);

	long writeIdFloatArray (PCSTR varName, float *array, size_t numElements);
	long writeIdLongArray (PCSTR varName, long *array, size_t numElements);
	long writeIdUShortArray (PCSTR varName, unsigned short *array, size_t numElements);
	long writeIdUCHARArray (PCSTR varName, PUCHAR array, size_t numElements);
};

//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
//
// Edit Log
//
//
//---------------------------------------------------------------------------
