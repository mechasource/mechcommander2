//---------------------------------------------------------------------------
//
// csvfile.h - This file contains the class declaration for the CSV Files
//
//				The CSV file is an Excel csv style file.
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef CSVFILE_H
#define CSVFILE_H
//---------------------------------------------------------------------------
// Include files

#ifndef DSTD_H
#include "dstd.h"
#endif

#ifndef DCSVFILE_H
#include "dcsvfile.h"
#endif

#ifndef FILE_H
#include "file.h"
#endif

//---------------------------------------------------------------------------
// Macro Definitions

//---------------------------------------------------------------------------
// Enums

//---------------------------------------------------------------------------
// Structs

//---------------------------------------------------------------------------
//									CSVFile
class CSVFile : public File
{
	// Data Members
	//--------------
	protected:
		ULONG totalRows;			//Number of ROWS CSV file has.
		ULONG totalCols;			//NUmber of COLS CSV file has.
		
		char dataBuffer[2048];

	// Member Functions
	//------------------
	protected:
		int32_t afterOpen (void);
		void atClose (void);
		
		int32_t countRows (void);
		int32_t countCols (void);
		
		int32_t getNextWord (PSTR &line, PSTR buffer, size_t bufLen);

		float textToFloat (PSTR num);
		
		int32_t textToLong (PSTR num);
		size_t textToULong (PSTR num);
		
		short textToShort (PSTR num);
		uint16_t textToUShort (PSTR num);
		
		char textToChar (PSTR num);
		uint8_t textToUCHAR (PSTR num);

		bool booleanToLong (PSTR num);

		int32_t floatToText (PSTR result, float num, size_t bufLen);
		
		int32_t longToTextDec (PSTR result, int32_t num, size_t bufLen);
		int32_t longToTextHex (PSTR result, int32_t num, size_t bufLen);

		int32_t shortToTextDec (PSTR result, short num, size_t bufLen);
		int32_t shortToTextHex (PSTR result, short num, size_t bufLen);

		int32_t byteToTextDec (PSTR result, byte num, size_t bufLen);	
		int32_t byteToTextHex (PSTR result, byte num, size_t bufLen);
		
		int32_t copyString (PSTR dest, PSTR src, size_t bufLen);

	public:
		CSVFile (void);
		~CSVFile (void);

		virtual int32_t open (PCSTR fName, FileMode _mode = READ, int32_t numChildren = 50);
		virtual int32_t open (FilePtr _parent, size_t fileSize, int32_t numChildren = 50);
		
		virtual int32_t create (PSTR fName);

		virtual void close (void);

		virtual FileClass getFileClass (void)
		{
			return CSVFILE;
		}

		int32_t seekRowCol (ULONG row, ULONG col);
		
		int32_t readFloat (ULONG row, ULONG col, float &value);
		
		int32_t readBoolean (ULONG row, ULONG col, bool &value);
		int32_t readLong (ULONG row, ULONG col, int32_t &value);
		int32_t readULong (ULONG row, ULONG col, size_t &value);
		
		int32_t readShort (ULONG row, ULONG col, short &value);
		int32_t readUShort (ULONG row, ULONG col, uint16_t &value);
		
		int32_t readChar (ULONG row, ULONG col, char &value);
		int32_t readUCHAR (ULONG row, ULONG col, uint8_t &value);
		
		int32_t readString (ULONG row, ULONG col, PSTR result, size_t bufferSize);
};

//---------------------------------------------------------------------------
#endif
