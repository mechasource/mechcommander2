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
//---------------------------------------------------------------------------
// Include files

#ifndef DSTD_H
#include "dstd.h"
#endif

#ifndef DINIFILE_H
#include "dinifile.h"
#endif

#ifndef FILE_H
#include "file.h"
#endif

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
		size_t 	totalBlocks;					//Total number of blocks in file
		IniBlockNode 	*fileBlocks;					//Data for blocks to speed up file
		char 				*currentBlockId;				//Id of current block
		size_t 	currentBlockOffset;			//Offset into file of block start
		size_t 	currentBlockSize;				//Length of current block

	// Member Functions
	//------------------
	protected:
		long afterOpen (void);
		void atClose (void);
		
		long findNextBlockStart (char *line = NULL, size_t lineLen = 0);
		long countBlocks (void);
		
		long getNextWord (char *&line, char *buffer, size_t bufLen);

		float textToFloat (const char *num);
		double textToDouble (const char *num);
		
		long textToLong (const char *num);
		size_t textToULong (const char *num);
		
		short textToShort (const char *num);
		unsigned short textToUShort (const char *num);
		
		char textToChar (const char *num);
		unsigned char textToUChar (const char *num);

		bool booleanToLong (const char *num);

		float mathToFloat (const char *num);
		
		long mathToLong (const char *num);
		size_t mathToULong (const char *num);
		
		short mathToShort (const char *num);
		unsigned short mathToUShort (const char *num);
		
		char mathToChar (const char *num);
		unsigned char mathToUChar (const char *num);

		long floatToText (char *result, float num, size_t bufLen);
		
		long longToTextDec (char *result, long num, size_t bufLen);
		long longToTextHex (char *result, long num, size_t bufLen);

		long shortToTextDec (char *result, short num, size_t bufLen);
		long shortToTextHex (char *result, short num, size_t bufLen);

		long byteToTextDec (char *result, byte num, size_t bufLen);	
		long byteToTextHex (char *result, byte num, size_t bufLen);
		
		long copyString (char* dest, char *src, size_t bufLen);

	public:
		FitIniFile (void);
		~FitIniFile (void);

		virtual long open (const char* fName, FileMode _mode = READ, long numChildren = 50);
		virtual long open (FilePtr _parent, size_t fileSize, long numChildren = 50);
		
		virtual long create (char* fName);
		virtual long createWithCase( char* fName );


		virtual void close (void);

		virtual FileClass getFileClass (void)
		{
			return INIFILE;
		}

		long seekBlock (const char *blockId);

		long readIdFloat (const char *varName, float &value);
		long readIdDouble (const char *varName, double &value);
		
		long readIdBoolean (const char *varName, bool &value);
		long readIdLong (const char *varName, long &value);
		long readIdULong (const char *varName, size_t &value);
		
		long readIdShort (const char *varName, short &value);
		long readIdUShort (const char *varName, unsigned short &value);
		
		long readIdChar (const char *varName, char &value);
		long readIdUChar (const char *varName, unsigned char &value);
		
		long readIdString (const char *varName, char *result, size_t bufferSize);

		long getIdStringLength (const char *varName);
		
		long readIdFloatArray (const char *varName, float *result, size_t numElements);
		
		long readIdLongArray (const char *varName, long *result, size_t numElements);
		long readIdULongArray (const char *varName, size_t *result, size_t numElements);
		
		long readIdShortArray (const char *varName, short *result, size_t numElements);
		long readIdUShortArray (const char *varName, unsigned short *result, size_t numElements);
		
		long readIdCharArray (const char *varName, char *result, size_t numElements);
		long readIdUCharArray (const char *varName, unsigned char *result, size_t numElements);
		
		size_t getIdFloatArrayElements (const char *varName);
		
		size_t getIdLongArrayElements (const char *varName);
		size_t getIdULongArrayElements (const char *varName);
											 
		size_t getIdShortArrayElements (const char *varName);
		size_t getIdUShortArrayElements (const char *varName);
		
		size_t getIdCharArrayElements (const char *varName);
		size_t getIdUCharArrayElements (const char *varName);
		
		long writeBlock (const char *blockId);

		long writeIdFloat (const char *varName, float value);
		
		long writeIdBoolean (const char *varName, bool value);
		long writeIdLong (const char *varName, long value);
		long writeIdULong (const char *varName, size_t value);
		
		long writeIdShort (const char *varName, short value);
		long writeIdUShort (const char *varName, unsigned short value);
		
		long writeIdChar (const char *varName, char value);
		long writeIdUChar (const char *varName, unsigned char value);
		
		long writeIdString (const char *varName, const char *result);

		long writeIdFloatArray (const char *varName, float *array, size_t numElements);
		long writeIdLongArray (const char *varName, long *array, size_t numElements);
		long writeIdUShortArray (const char *varName, unsigned short *array, size_t numElements);
		long writeIdUCharArray (const char *varName, unsigned char *array, size_t numElements);
};

//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
//
// Edit Log
//
//
//---------------------------------------------------------------------------
