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

//#include "dstd.h"
//#include "dcsvfile.h"
//#include "file.h"

//---------------------------------------------------------------------------
// Macro Definitions

//---------------------------------------------------------------------------
// Enums

//---------------------------------------------------------------------------
// Structs

//---------------------------------------------------------------------------
//									CSVFile
class CSVFile
	: public MechFile<CSVFile>
{

public:
	CSVFile(void);
	~CSVFile(void);

	virtual int32_t open(std::wstring_view filename, FileMode _mode = READ, int32_t numChildren = 50);
	virtual int32_t open(std::unique_ptr<File> _parent, size_t fileSize, int32_t numChildren = 50);

	virtual int32_t create(std::wstring_view filename);

	virtual void close(void);

	virtual FileClass getFileClass(void)
	{
		return FileClass::csvfile;
	}

	int32_t seekRowCol(uint32_t row, uint32_t col);

	int32_t readFloat(uint32_t row, uint32_t col, float& value);

	int32_t readBoolean(uint32_t row, uint32_t col, bool& value);
	int32_t readLong(uint32_t row, uint32_t col, int32_t& value);
	int32_t readULong(uint32_t row, uint32_t col, size_t& value);

	int32_t readShort(uint32_t row, uint32_t col, int16_t& value);
	int32_t readUShort(uint32_t row, uint32_t col, uint16_t& value);

	int32_t readChar(uint32_t row, uint32_t col, wchar_t& value);
	int32_t readUCHAR(uint32_t row, uint32_t col, uint8_t& value);

	int32_t readString(uint32_t row, uint32_t col, std::wstring_view result, size_t bufferSize);

protected:
	int32_t afterOpen(void);
	void atClose(void);

	int32_t countRows(void);
	int32_t countCols(void);

	int32_t getNextWord(std::wstring_view& line, std::wstring_view buffer, size_t bufLen);

	float textToFloat(std::wstring_view num);

	int32_t textToLong(std::wstring_view num);
	size_t textToULong(std::wstring_view num);

	int16_t textToShort(std::wstring_view num);
	uint16_t textToUShort(std::wstring_view num);

	wchar_t textToChar(std::wstring_view num);
	uint8_t textToUCHAR(std::wstring_view num);

	bool booleanToLong(std::wstring_view num);

	int32_t floatToText(std::wstring_view result, float num, size_t bufLen);

	int32_t longToTextDec(std::wstring_view result, int32_t num, size_t bufLen);
	int32_t longToTextHex(std::wstring_view result, int32_t num, size_t bufLen);

	int32_t shortToTextDec(std::wstring_view result, int16_t num, size_t bufLen);
	int32_t shortToTextHex(std::wstring_view result, int16_t num, size_t bufLen);

	int32_t byteToTextDec(std::wstring_view result, byte num, size_t bufLen);
	int32_t byteToTextHex(std::wstring_view result, byte num, size_t bufLen);

	int32_t copyString(std::wstring_view dest, std::wstring_view src, size_t bufLen);

protected:
	uint32_t totalRows; // Number of ROWS CSV file has.
	uint32_t totalCols; // NUmber of COLS CSV file has.

	wchar_t dataBuffer[2048];
};

//---------------------------------------------------------------------------
#endif
