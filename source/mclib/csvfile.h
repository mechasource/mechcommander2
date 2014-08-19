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
		DWORD totalRows;			//Number of ROWS CSV file has.
		DWORD totalCols;			//NUmber of COLS CSV file has.
		
		char dataBuffer[2048];

	// Member Functions
	//------------------
	protected:
		long afterOpen (void);
		void atClose (void);
		
		long countRows (void);
		long countCols (void);
		
		long getNextWord (PSTR &line, PSTR buffer, size_t bufLen);

		float textToFloat (PSTR num);
		
		long textToLong (PSTR num);
		size_t textToULong (PSTR num);
		
		short textToShort (PSTR num);
		unsigned short textToUShort (PSTR num);
		
		char textToChar (PSTR num);
		uint8_t textToUCHAR (PSTR num);

		bool booleanToLong (PSTR num);

		long floatToText (PSTR result, float num, size_t bufLen);
		
		long longToTextDec (PSTR result, long num, size_t bufLen);
		long longToTextHex (PSTR result, long num, size_t bufLen);

		long shortToTextDec (PSTR result, short num, size_t bufLen);
		long shortToTextHex (PSTR result, short num, size_t bufLen);

		long byteToTextDec (PSTR result, byte num, size_t bufLen);	
		long byteToTextHex (PSTR result, byte num, size_t bufLen);
		
		long copyString (PSTR dest, PSTR src, size_t bufLen);

	public:
		CSVFile (void);
		~CSVFile (void);

		virtual long open (PCSTR fName, FileMode _mode = READ, long numChildren = 50);
		virtual long open (FilePtr _parent, size_t fileSize, long numChildren = 50);
		
		virtual long create (PSTR fName);

		virtual void close (void);

		virtual FileClass getFileClass (void)
		{
			return CSVFILE;
		}

		long seekRowCol (DWORD row, DWORD col);
		
		long readFloat (DWORD row, DWORD col, float &value);
		
		long readBoolean (DWORD row, DWORD col, bool &value);
		long readLong (DWORD row, DWORD col, long &value);
		long readULong (DWORD row, DWORD col, size_t &value);
		
		long readShort (DWORD row, DWORD col, short &value);
		long readUShort (DWORD row, DWORD col, unsigned short &value);
		
		long readChar (DWORD row, DWORD col, char &value);
		long readUCHAR (DWORD row, DWORD col, uint8_t &value);
		
		long readString (DWORD row, DWORD col, PSTR result, size_t bufferSize);
};

//---------------------------------------------------------------------------
#endif
