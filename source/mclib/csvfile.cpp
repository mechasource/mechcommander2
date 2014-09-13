//---------------------------------------------------------------------------
//
// csvfile.cpp - This file contains the class declaration for the CSV Files
//
//				The CSV file is an Excel csv style file.
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"

#ifndef CSVFILE_H
#include "csvfile.h"
#endif

#ifndef HEAP_H
#include "heap.H"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#ifndef _MBCS
#include <gameos.hpp>
#else
#include <assert.h>
#define gosASSERT assert
#define gos_Malloc malloc
#define gos_Free free
#endif

//---------------------------------------------------------------------------
// Static Globals

#undef isspace		//Macro Chokes under Intel Compiler!!

//---------------------------------------------------------------------------
// class CSVIniFile
CSVFile::CSVFile(void) : File()
{
	totalRows = totalCols = 0L;
}

//---------------------------------------------------------------------------
CSVFile::~CSVFile(void)
{
	close();
}

//---------------------------------------------------------------------------
int32_t CSVFile::countRows(void)
{
	int32_t count = 0;
	int32_t oldPosition = logicalPosition;
	seek(0);		//Start at the top.
	char tmp[2048];
	readLine((puint8_t)tmp, 2047);
	while(!eof())
	{
		count++;
		readLine((puint8_t)tmp, 2047);
	}
	//----------------------------------
	// Move back to where we were.
	seek(oldPosition);
	return(count);
}

//---------------------------------------------------------------------------
int32_t CSVFile::countCols(void)
{
	int32_t count = 0, maxCols = 0;
	int32_t oldPosition = logicalPosition;
	seek(0);		//Start at the top.
	char tmp[2048];
	PSTR currentChk = tmp;
	readLine((puint8_t)tmp, 2047);
	currentChk = strstr(tmp, ",");
	while(currentChk && (*currentChk != '\n') && (*currentChk != '\r'))
	{
		count++;
		currentChk++;
		currentChk = strstr(currentChk, ",");
	}
	if(count > maxCols)
		maxCols = count;
	readLine((puint8_t)tmp, 2047);
	//----------------------------------
	// Move back to where we were.
	seek(oldPosition);
	return(maxCols);
}

//---------------------------------------------------------------------------
int32_t CSVFile::getNextWord(PSTR& line, PSTR buffer, uint32_t bufLen)
{
	//--------------------------------------------------
	// Check to see if we are at end of line
	if(*line == '\0')
		return(-1);
	//--------------------------------------------------
	// Check to see if the rest of the line is comments
	if(*line == '/')
		return(-1);
	if(*line == ',')    // empty column, move on
		return -1;
	//------------------------------------------
	// Find start of word from current location
	while((*line != '\0') && ((*line == ' ') || (*line == '\t') || (*line == ',')))
	{
		line++;
	}
	//--------------------------------------------------
	// Check to see if we are at end of line
	if(*line == '\0')
		return(-1);
	//--------------------------------------------------
	// Check to see if the rest of the line is comments
	if(*line == '/')
		return(-1);
	//-------------------------------------------
	// Find length of word from current location
	PSTR startOfWord = line;
	uint32_t wordLength = 0;
	while((*line != '\0') && ((*line != ',')))
	{
		line++;
		wordLength++;
	}
	if(wordLength > bufLen)
		return(-2);
	strncpy(buffer, startOfWord, wordLength);
	buffer[wordLength] = '\0';
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::afterOpen(void)
{
	//-------------------------------------------------------
	// Check if we opened this with CREATE and write the
	// FITini Header and position to Write Start.
	if(fileMode == CREATE && parent == nullptr)
	{
		STOP(("Cannot write CSV files at present."));
	}
	else
	{
		//------------------------------------------------------
		// Find out how many Rows and cols we have
		totalRows = countRows();
		totalCols = countCols();
	}
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
void CSVFile::atClose(void)
{
	//------------------------------------------------------------
	// Check if we are in create mode and if so, write the footer
	if(fileMode == CREATE)
	{
		STOP(("Cannot write CSV files at present."));
	}
	totalRows = totalCols = 0;
}

//---------------------------------------------------------------------------
float CSVFile::textToFloat(PSTR num)
{
	float result = atof(num);
	return(result);
}

//---------------------------------------------------------------------------
int32_t CSVFile::textToLong(PSTR num)
{
	int32_t result = 0;
	//------------------------------------
	// Check if Hex Number
	PSTR hexOffset = strstr(num, "0x");
	if(hexOffset == nullptr)
	{
		result = atol(num);
	}
	else
	{
		hexOffset += 2;
		int32_t numDigits = strlen(hexOffset) - 1;
		for(size_t i = 0; i <= numDigits; i++)
		{
			if(!isalnum(hexOffset[i]) || (isalpha(hexOffset[i]) && toupper(hexOffset[i]) > 'F'))
			{
				hexOffset[i] = 0;	// we've reach a "wrong" character. Either start of a comment or something illegal. Either way, stop evaluation here.
				break;
			}
		}
		numDigits = strlen(hexOffset) - 1;
		int32_t power = 0;
		for(int32_t count = numDigits; count >= 0; count--, power++)
		{
			uint8_t currentDigit = toupper(hexOffset[count]);
			if(currentDigit >= 'A' && currentDigit <= 'F')
			{
				result += (currentDigit - 'A' + 10) << (4 * power);
			}
			else if(currentDigit >= '0' && currentDigit <= '9')
			{
				result += (currentDigit - '0') << (4 * power);
			}
			else
			{
				//---------------------------------------------------------
				// There is a digit in here I don't understand.  Return 0.
				result = 0;
				break;
			}
		}
	}
	return(result);
}


//---------------------------------------------------------------------------
int16_t CSVFile::textToShort(PSTR num)
{
	int16_t result = 0;
	//------------------------------------
	// Check if Hex Number
	PSTR hexOffset = strstr(num, "0x");
	if(hexOffset == nullptr)
	{
		result = atol(num);
	}
	else
	{
		hexOffset += 2;
		int32_t numDigits = strlen(hexOffset) - 1;
		for(size_t i = 0; i <= numDigits; i++)
		{
			if(!isalnum(hexOffset[i]) || (isalpha(hexOffset[i]) && toupper(hexOffset[i]) > 'F'))
			{
				hexOffset[i] = 0;	// we've reach a "wrong" character. Either start of a comment or something illegal. Either way, stop evaluation here.
				break;
			}
		}
		numDigits = strlen(hexOffset) - 1;
		int32_t power = 0;
		for(int32_t count = numDigits; count >= 0; count--, power++)
		{
			uint8_t currentDigit = toupper(hexOffset[count]);
			if(currentDigit >= 'A' && currentDigit <= 'F')
			{
				result += (currentDigit - 'A' + 10) << (4 * power);
			}
			else if(currentDigit >= '0' && currentDigit <= '9')
			{
				result += (currentDigit - '0') << (4 * power);
			}
			else
			{
				//---------------------------------------------------------
				// There is a digit in here I don't understand.  Return 0.
				result = 0;
				break;
			}
		}
	}
	return(result);
}


//---------------------------------------------------------------------------
char CSVFile::textToChar(PSTR num)
{
	char result = 0;
	//------------------------------------
	// Check if Hex Number
	PSTR hexOffset = strstr(num, "0x");
	if(hexOffset == nullptr)
	{
		result = atol(num);
	}
	else
	{
		hexOffset += 2;
		int32_t numDigits = strlen(hexOffset) - 1;
		for(size_t i = 0; i <= numDigits; i++)
		{
			if(!isalnum(hexOffset[i]) || (isalpha(hexOffset[i]) && toupper(hexOffset[i]) > 'F'))
			{
				hexOffset[i] = 0;	// we've reach a "wrong" character. Either start of a comment or something illegal. Either way, stop evaluation here.
				break;
			}
		}
		numDigits = strlen(hexOffset) - 1;
		int32_t power = 0;
		for(int32_t count = numDigits; count >= 0; count--, power++)
		{
			uint8_t currentDigit = toupper(hexOffset[count]);
			if(currentDigit >= 'A' && currentDigit <= 'F')
			{
				result += (currentDigit - 'A' + 10) << (4 * power);
			}
			else if(currentDigit >= '0' && currentDigit <= '9')
			{
				result += (currentDigit - '0') << (4 * power);
			}
			else
			{
				//---------------------------------------------------------
				// There is a digit in here I don't understand.  Return 0.
				result = 0;
				break;
			}
		}
	}
	return(result);
}

//---------------------------------------------------------------------------
uint32_t CSVFile::textToULong(PSTR num)
{
	uint32_t result = 0;
	//------------------------------------
	// Check if Hex Number
	PSTR hexOffset = strstr(num, "0x");
	if(hexOffset == nullptr)
	{
		result = atol(num);
	}
	else
	{
		hexOffset += 2;
		int32_t numDigits = strlen(hexOffset) - 1;
		for(size_t i = 0; i <= numDigits; i++)
		{
			if(!isalnum(hexOffset[i]) || (isalpha(hexOffset[i]) && toupper(hexOffset[i]) > 'F'))
			{
				hexOffset[i] = 0;	// we've reach a "wrong" character. Either start of a comment or something illegal. Either way, stop evaluation here.
				break;
			}
		}
		numDigits = strlen(hexOffset) - 1;
		int32_t power = 0;
		for(int32_t count = numDigits; count >= 0; count--, power++)
		{
			uint8_t currentDigit = toupper(hexOffset[count]);
			if(currentDigit >= 'A' && currentDigit <= 'F')
			{
				result += (currentDigit - 'A' + 10) << (4 * power);
			}
			else if(currentDigit >= '0' && currentDigit <= '9')
			{
				result += (currentDigit - '0') << (4 * power);
			}
			else
			{
				//---------------------------------------------------------
				// There is a digit in here I don't understand.  Return 0.
				result = 0;
				break;
			}
		}
	}
	return(result);
}


//---------------------------------------------------------------------------
uint16_t CSVFile::textToUShort(PSTR num)
{
	uint16_t result = 0;
	//------------------------------------
	// Check if Hex Number
	PSTR hexOffset = strstr(num, "0x");
	if(hexOffset == nullptr)
	{
		result = atol(num);
	}
	else
	{
		hexOffset += 2;
		int32_t numDigits = strlen(hexOffset) - 1;
		for(size_t i = 0; i <= numDigits; i++)
		{
			if(!isalnum(hexOffset[i]) || (isalpha(hexOffset[i]) && toupper(hexOffset[i]) > 'F'))
			{
				hexOffset[i] = 0;	// we've reach a "wrong" character. Either start of a comment or something illegal. Either way, stop evaluation here.
				break;
			}
		}
		numDigits = strlen(hexOffset) - 1;
		int32_t power = 0;
		for(int32_t count = numDigits; count >= 0; count--, power++)
		{
			uint8_t currentDigit = toupper(hexOffset[count]);
			if(currentDigit >= 'A' && currentDigit <= 'F')
			{
				result += (currentDigit - 'A' + 10) << (4 * power);
			}
			else if(currentDigit >= '0' && currentDigit <= '9')
			{
				result += (currentDigit - '0') << (4 * power);
			}
			else
			{
				//---------------------------------------------------------
				// There is a digit in here I don't understand.  Return 0.
				result = 0;
				break;
			}
		}
	}
	return(result);
}


//---------------------------------------------------------------------------
uint8_t CSVFile::textToUCHAR(PSTR num)
{
	uint8_t result = 0;
	//------------------------------------
	// Check if Hex Number
	PSTR hexOffset = strstr(num, "0x");
	if(hexOffset == nullptr)
	{
		result = atol(num);
	}
	else
	{
		hexOffset += 2;
		int32_t numDigits = strlen(hexOffset) - 1;
		for(size_t i = 0; i <= numDigits; i++)
		{
			if(!isalnum(hexOffset[i]) || (isalpha(hexOffset[i]) && toupper(hexOffset[i]) > 'F'))
			{
				hexOffset[i] = 0;	// we've reach a "wrong" character. Either start of a comment or something illegal. Either way, stop evaluation here.
				break;
			}
		}
		numDigits = strlen(hexOffset) - 1;
		int32_t power = 0;
		for(int32_t count = numDigits; count >= 0; count--, power++)
		{
			uint8_t currentDigit = toupper(hexOffset[count]);
			if(currentDigit >= 'A' && currentDigit <= 'F')
			{
				result += (currentDigit - 'A' + 10) << (4 * power);
			}
			else if(currentDigit >= '0' && currentDigit <= '9')
			{
				result += (currentDigit - '0') << (4 * power);
			}
			else
			{
				//---------------------------------------------------------
				// There is a digit in here I don't understand.  Return 0.
				result = 0;
				break;
			}
		}
	}
	return(result);
}

//---------------------------------------------------------------------------
bool CSVFile::booleanToLong(PSTR num)
{
	char testChar = 0;
	while(num[testChar] && isspace(num[testChar]))
		testChar++;
	// 'N' == NO if you can believe that
	if((toupper(num[testChar]) == 'F') || (toupper(num[testChar]) == '0') || (toupper(num[testChar]) == 'N'))
		return FALSE;
	else
		return(TRUE);
}

//---------------------------------------------------------------------------
int32_t CSVFile::floatToText(PSTR result, float num, uint32_t bufLen)
{
	char temp[250];
	sprintf(temp, "%f4", num);
	uint32_t numLength = strlen(temp);
	if(numLength >= bufLen)
		return(-2);
	strncpy(result, temp, numLength);
	result[numLength] = '\0';
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::longToTextDec(PSTR result, int32_t num, uint32_t bufLen)
{
	char temp[250];
	sprintf(temp, "%d", num);
	uint32_t numLength = strlen(temp);
	if(numLength >= bufLen)
		return(-2);
	strncpy(result, temp, numLength);
	result[numLength] = '\0';
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::longToTextHex(PSTR result, int32_t num, uint32_t bufLen)
{
	char temp[250];
	sprintf(temp, "0x%x", num);
	uint32_t numLength = strlen(temp);
	if(numLength >= bufLen)
		return(-2);
	strncpy(result, temp, numLength);
	result[numLength] = '\0';
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::shortToTextDec(PSTR result, int16_t num, uint32_t bufLen)
{
	char temp[250];
	sprintf(temp, "%d", num);
	uint32_t numLength = strlen(temp);
	if(numLength >= bufLen)
		return(-2);
	strncpy(result, temp, numLength);
	result[numLength] = '\0';
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::shortToTextHex(PSTR result, int16_t num, uint32_t bufLen)
{
	char temp[250];
	sprintf(temp, "0x%x", num);
	uint32_t numLength = strlen(temp);
	if(numLength >= bufLen)
		return(-2);
	strncpy(result, temp, numLength);
	result[numLength] = '\0';
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::byteToTextDec(PSTR result, byte num, uint32_t bufLen)
{
	char temp[250];
	sprintf(temp, "%d", num);
	uint32_t numLength = strlen(temp);
	if(numLength >= bufLen)
		return(-2);
	strncpy(result, temp, numLength);
	result[numLength] = '\0';
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::byteToTextHex(PSTR result, byte num, uint32_t bufLen)
{
	char temp[250];
	sprintf(temp, "0x%x", num);
	uint32_t numLength = strlen(temp);
	if(numLength >= bufLen)
		return(-2);
	strncpy(result, temp, numLength);
	result[numLength] = '\0';
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::open(PCSTR fName, FileMode _mode, int32_t numChild)
{
	int32_t result = File::open(fName, _mode, numChild);
	if(result != NO_ERROR)
		return(result);
	seek(0);
	result = afterOpen();
	return(result);
}

//---------------------------------------------------------------------------
int32_t CSVFile::open(FilePtr _parent, uint32_t fileSize, int32_t numChild)
{
	numChild = -1;		//Force all parented CSVs to load from RAM.
	int32_t result = File::open(_parent, fileSize, numChild);
	if(result != NO_ERROR)
		return(result);
	result = afterOpen();
	return(result);
}

//---------------------------------------------------------------------------
int32_t CSVFile::create(PSTR fName)
{
	fName;
	//STOP(("CSV file write is not supported %s",fName));
	return(-1);
}

//---------------------------------------------------------------------------
void CSVFile::close(void)
{
	if(isOpen())
	{
		atClose();
		File::close();
	}
}

//---------------------------------------------------------------------------
int32_t CSVFile::seekRowCol(uint32_t row, uint32_t col)
{
	if((row > totalRows) || (col > totalCols))
		return -1;
	uint32_t rowCount = 0;
	seek(0);		//Start at the top.
	char tmp[2048];
	do
	{
		rowCount++;
		readLine((puint8_t)tmp, 2047);
	}
	while(rowCount != row);
	PSTR currentChk = tmp;
	if(col)
	{
		uint32_t colCount = 1;
		while(currentChk && (colCount != col))
		{
			colCount++;
			currentChk = strstr(currentChk, ",");
			if(currentChk)    // if we increment to one, bad things happen
				currentChk++;
		}
	}
	//---------------------------------------------------
	// We are now pointing at the row and col specified.
	if(currentChk)
	{
		PSTR data = dataBuffer;
		return getNextWord(currentChk, data, 2047);
	}
	else
	{
		return -1;
		// CAN'T do this, for some reason excel writes out empty rows.
		//STOP(("Unable to parse CSV %s, ROW %d, COL %D, ERROR: nullptr",getFilename(),row,col));
	}
	//return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::readFloat(uint32_t row, uint32_t col, float& value)
{
	int32_t result = seekRowCol(row, col);
	if(result == NO_ERROR)
	{
		value = textToFloat(dataBuffer);
	}
	else
		value = 0.0f;
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::readLong(uint32_t row, uint32_t col, int32_t& value)
{
	int32_t result = seekRowCol(row, col);
	if(result == NO_ERROR)
	{
		value = textToLong(dataBuffer);
	}
	else
		value = 0.0f;
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::readBoolean(uint32_t row, uint32_t col, bool& value)
{
	int32_t result = seekRowCol(row, col);
	if(result == NO_ERROR)
	{
		value = booleanToLong(dataBuffer);
	}
	else
		value = 0;
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::readShort(uint32_t row, uint32_t col, int16_t& value)
{
	int32_t result = seekRowCol(row, col);
	if(result == NO_ERROR)
	{
		value = textToShort(dataBuffer);
	}
	else
		value = 0.0f;
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::readChar(uint32_t row, uint32_t col, char& value)
{
	int32_t result = seekRowCol(row, col);
	if(result == NO_ERROR)
	{
		value = textToChar(dataBuffer);
	}
	else
		value = 0.0f;
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::readULong(uint32_t row, uint32_t col, uint32_t& value)
{
	int32_t result = seekRowCol(row, col);
	if(result == NO_ERROR)
	{
		value = textToULong(dataBuffer);
	}
	else
		value = 0.0f;
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::readUShort(uint32_t row, uint32_t col, uint16_t& value)
{
	int32_t result = seekRowCol(row, col);
	if(result == NO_ERROR)
	{
		value = textToUShort(dataBuffer);
	}
	else
		value = 0.0f;
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::readUCHAR(uint32_t row, uint32_t col, uint8_t& value)
{
	int32_t result = seekRowCol(row, col);
	if(result == NO_ERROR)
	{
		value = textToUCHAR(dataBuffer);
	}
	else
		value = 0.0f;
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::copyString(PSTR dest, PSTR src, uint32_t bufLen)
{
	uint32_t offset = 0;
	//---------------------------------------
	// Copy each character until close quote
	while(*src != '"' && *src != '\0' && offset < bufLen)
	{
		dest[offset] = *src;
		src++;
		offset++;
	}
	//----------------------------------------------------
	// If this string is longer than buffer, let em know.
	if(offset == bufLen)
	{
		return(-2);
	}
	//---------------------------------
	// otherwise, nullptr term and return
	dest[offset] = '\0';
	return(NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t CSVFile::readString(uint32_t row, uint32_t col, PSTR result, uint32_t bufferSize)
{
	int32_t res = seekRowCol(row, col);
	if(res == NO_ERROR)
	{
		int32_t errorCode = copyString(result, dataBuffer, bufferSize);
		if(errorCode != NO_ERROR)
			return(errorCode);
	}
	else
		return 1;
	return(0); // gotta return some kind of error!
}

//---------------------------------------------------------------------------
//
// Edit log
//
//---------------------------------------------------------------------------

