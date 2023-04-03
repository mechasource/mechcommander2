//---------------------------------------------------------------------------
//
// inifile.cpp - This file contains the class declaration for the FitIni Files
//
//				The FitIni file is a ini style file.
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

//---------------------------------------------------------------------------

#include "inifile.h"

//#include "heap.H"
//#include "err.h"

//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
//#include <string.h>
//#include <ctype.h>

////#include "gameos.hpp"
//#include <_ASSERT.h>
//#define gosASSERT _ASSERT
//#define gos_Malloc malloc
//#define gos_Free free

enum __inifile_source_constants : uint32_t
{
	BUFFERSIZE = UCHAR_MAX,
};

//---------------------------------------------------------------------------
// Static Globals
constexpr std::wstring_view fitIniHeader = "FITini";
constexpr std::wstring_view fitIniFooter = "FITend";
constexpr std::wstring_view fitIniHeaderLE = "FITini\r\n";
constexpr std::wstring_view fitIniFooterLE = "FITend\r\n";

//---------------------------------------------------------------------------
// class FitIniFile
// FitIniFile::FitIniFile(void) : File()
//{
//	m_totalBlocks = 0;
//	m_fileBlocks = nullptr;
//
//	m_currentBlockId = nullptr;
//	m_currentBlockOffset = 0;
//	m_currentBlockSize = 0;
//}

//---------------------------------------------------------------------------
// FitIniFile::~FitIniFile (void)
//{
//	close();
//}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::findNextBlockStart(std::wstring_view line, size_t lineLen)
{
	wchar_t thisLine[BUFFERSIZE];
	std::wstring_view common = nullptr;
	do
	{
		if (line)
		{
			readLine((uint8_t*)line, lineLen);
			common = line;
		}
		else
		{
			readLine((uint8_t*)thisLine, BUFFERSIZE - 1);
			common = thisLine;
		}
	} while (!eof() && (common[0] != '['));
	if (eof())
	{
		return (NO_MORE_BLOCKS);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
size_t
FitIniFile::countBlocks(void)
{
	size_t count = 0;
	size_t oldPosition = logicalPosition;
	while (findNextBlockStart() != NO_MORE_BLOCKS)
	{
		count++;
	}
	//----------------------------------
	// Move back to where we were.
	seek(oldPosition);
	return (count);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::getNextWord(std::wstring_view& line, std::wstring_view buffer, size_t bufLen)
{
	//--------------------------------------------------
	// Check to see if we are at end of line
	if (*line == '\0')
		return (GET_NEXT_LINE);
	//--------------------------------------------------
	// Check to see if the rest of the line is comments
	if (*line == '/')
		return (GET_NEXT_LINE);
	//------------------------------------------
	// Find start of word from current location
	while ((*line != '\0') && ((*line == ' ') || (*line == '\t') || (*line == ',')))
	{
		line++;
	}
	//--------------------------------------------------
	// Check to see if we are at end of line
	if (*line == '\0')
		return (GET_NEXT_LINE);
	//--------------------------------------------------
	// Check to see if the rest of the line is comments
	if (*line == '/')
		return (GET_NEXT_LINE);
	//-------------------------------------------
	// Find length of word from current location
	std::wstring_view startOfWord = line;
	size_t wordLength = 0;
	while ((*line != '\0') && ((*line != ' ') && (*line != '\t') && (*line != ',')))
	{
		line++;
		wordLength++;
	}
	if (wordLength > bufLen)
		return (BUFFER_TOO_SMALL);
	strncpy(buffer, startOfWord, wordLength);
	buffer[wordLength] = '\0';
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::afterOpen(void)
{
	// wchar_t line[BUFFERSIZE];
	//-------------------------------------------------------
	// Check if we opened this with CREATE and write the
	// FITini Header and position to Write Start.
	if (fileMode == CREATE && m_parent == nullptr)
	{
		// sprintf_s(line, BUFFERSIZE, "%s \r\n", fitIniHeader);
		write(const_cast<uint8_t*>(fitIniHeaderLE), strlen(fitIniHeaderLE));
		m_totalBlocks = 0;
	}
	else
	{
		//------------------------------------------------------
		// Check if this is a FitIniFile by looking for header.
		wchar_t chkHeader[12];
		readLine(static_cast<uint8_t*>(chkHeader), 11);
		if (strstr(chkHeader, fitIniHeader) == nullptr)
			return (NOT_A_FITINIFILE);
		//------------------------------------------------------
		// Find out how many blocks we have
		m_totalBlocks = countBlocks();
		//--------------------------------------------------------------------------
		// Allocate RAM for the BlockInfoNodes.  Check if system Heap is
		// available
		m_fileBlocks = (IniBlockNode*)systemHeap->Malloc(sizeof(IniBlockNode) * m_totalBlocks);
		// auto fileBlocks = make_unique<IniBlockNode>(L"Mr. Children",
		// L"Namonaki Uta");
		ATLASSERT(m_fileBlocks != nullptr);
		memset(m_fileBlocks, 0, sizeof(IniBlockNode) * m_totalBlocks);
		//--------------------------------------------------------------------------
		// Put Info into fileBlocks.
		uint32_t currentBlockNum = 0;
		while (findNextBlockStart(line, BUFFERSIZE - 1) != NO_MORE_BLOCKS)
		{
			//----------------------------------------------------
			// If we write too many fileBlocks, we will trash RAM
			// Shouldn't be able to happen but...
			if (currentBlockNum == m_totalBlocks)
				return (TOO_MANY_BLOCKS);
			int32_t count = 1;
			while (line[count] != ']' && line[count] != '\n')
			{
				m_fileBlocks[currentBlockNum].blockId[count - 1] = line[count];
				count++;
			}
			if (count >= 49)
				STOP(("BlockId To large in Fit File %s", m_fileName));
			if (line[count] == '\n')
			{
				wchar_t error[BUFFERSIZE + 1];
				sprintf_s(error, BUFFERSIZE + 1, "couldn't resolve block %s in file %s", line,
					getFilename());
				Assert(0, 0, error);
				return SYNTAX_ERROR;
			}
			m_fileBlocks[currentBlockNum].blockId[count - 1] = '\0';
			//----------------------------------------------------------------------
			// Since we just read all of last line, we now point to start of
			// data
			m_fileBlocks[currentBlockNum].blockOffset = logicalPosition;
			currentBlockNum++;
		}
		//------------------------------------------------------
		// If we didn't read in enough, CD-ROM error?
		if (currentBlockNum != m_totalBlocks)
			return (NOT_ENOUGH_BLOCKS);
	}
	return (NO_ERROR);
}

#if CONSIDERED_OBSOLETE
//---------------------------------------------------------------------------
void FitIniFile::atClose(void)
{
	//------------------------------------------------------------
	// Check if we are in create mode and if so, write the footer
	wchar_t line[BUFFERSIZE];
	if (fileMode == CREATE)
	{
		seek(0, SEEK_END);
		sprintf_s(line, BUFFERSIZE, "%s \r\n", fitIniFooter);
		write((uint8_t*)line, strlen(line));
	}
	//-----------------------------
	// Free up the fileBlocks
	systemHeap->Free(m_fileBlocks);
	m_fileBlocks = nullptr;
}
#endif

//---------------------------------------------------------------------------
float FitIniFile::textToFloat(std::wstring_view num)
{
	float result = atof(num);
	return (result);
}

//---------------------------------------------------------------------------
double
FitIniFile::textToDouble(std::wstring_view num)
{
	double result = atof(num);
	return (result);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::textToLong(std::wstring_view num)
{
	int32_t result = 0;
	//------------------------------------
	// Check if Hex Number
	std::wstring_view hexOffset = (std::wstring_view)strstr(num, "0x");
	if (hexOffset == nullptr)
	{
		result = atol(num);
	}
	else
	{
		hexOffset += 2;
		int32_t numDigits = strlen(hexOffset) - 1;
		for (size_t i = 0; i <= numDigits; i++)
		{
			if (!isalnum(hexOffset[i]) || (isalpha(hexOffset[i]) && toupper(hexOffset[i]) > 'F'))
			{
				hexOffset[i] = 0; // we've reach a "wrong" character. Either
					// start of a comment or something illegal.
					// Either way, stop evaluation here.
				break;
			}
		}
		numDigits = strlen(hexOffset) - 1;
		int32_t power = 0;
		for (size_t count = numDigits; count >= 0; count--, power++)
		{
			uint8_t currentDigit = toupper(hexOffset[count]);
			if (currentDigit >= 'A' && currentDigit <= 'F')
			{
				result += (currentDigit - 'A' + 10) << (4 * power);
			}
			else if (currentDigit >= '0' && currentDigit <= '9')
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
	return (result);
}

//---------------------------------------------------------------------------
int16_t
FitIniFile::textToShort(std::wstring_view num)
{
	int16_t result = 0;
	//------------------------------------
	// Check if Hex Number
	std::wstring_view hexOffset = (std::wstring_view)strstr(num, "0x");
	if (hexOffset == nullptr)
	{
		result = atol(num);
	}
	else
	{
		hexOffset += 2;
		int32_t numDigits = strlen(hexOffset) - 1;
		for (size_t i = 0; i <= numDigits; i++)
		{
			if (!isalnum(hexOffset[i]) || (isalpha(hexOffset[i]) && toupper(hexOffset[i]) > 'F'))
			{
				hexOffset[i] = 0; // we've reach a "wrong" character. Either
					// start of a comment or something illegal.
					// Either way, stop evaluation here.
				break;
			}
		}
		numDigits = strlen(hexOffset) - 1;
		int32_t power = 0;
		for (size_t count = numDigits; count >= 0; count--, power++)
		{
			uint8_t currentDigit = toupper(hexOffset[count]);
			if (currentDigit >= 'A' && currentDigit <= 'F')
			{
				result += (currentDigit - 'A' + 10) << (4 * power);
			}
			else if (currentDigit >= '0' && currentDigit <= '9')
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
	return (result);
}

//---------------------------------------------------------------------------
wchar_t
FitIniFile::textToChar(std::wstring_view num)
{
	wchar_t result = 0;
	//------------------------------------
	// Check if Hex Number
	std::wstring_view hexOffset = (std::wstring_view)strstr(num, "0x");
	if (hexOffset == nullptr)
	{
		result = atol(num);
	}
	else
	{
		hexOffset += 2;
		int32_t numDigits = strlen(hexOffset) - 1;
		for (size_t i = 0; i <= numDigits; i++)
		{
			if (!isalnum(hexOffset[i]) || (isalpha(hexOffset[i]) && toupper(hexOffset[i]) > 'F'))
			{
				hexOffset[i] = 0; // we've reach a "wrong" character. Either
					// start of a comment or something illegal.
					// Either way, stop evaluation here.
				break;
			}
		}
		numDigits = strlen(hexOffset) - 1;
		int32_t power = 0;
		for (size_t count = numDigits; count >= 0; count--, power++)
		{
			uint8_t currentDigit = toupper(hexOffset[count]);
			if (currentDigit >= 'A' && currentDigit <= 'F')
			{
				result += (currentDigit - 'A' + 10) << (4 * power);
			}
			else if (currentDigit >= '0' && currentDigit <= '9')
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
	return (result);
}

//---------------------------------------------------------------------------
uint32_t
FitIniFile::textToULong(std::wstring_view num)
{
	uint32_t result = 0;
	//------------------------------------
	// Check if Hex Number
	std::wstring_view hexOffset = (std::wstring_view)strstr(num, "0x");
	if (hexOffset == nullptr)
	{
		result = atol(num);
	}
	else
	{
		hexOffset += 2;
		int32_t numDigits = strlen(hexOffset) - 1;
		for (size_t i = 0; i <= numDigits; i++)
		{
			if (!isalnum(hexOffset[i]) || (isalpha(hexOffset[i]) && toupper(hexOffset[i]) > 'F'))
			{
				hexOffset[i] = 0; // we've reach a "wrong" character. Either
					// start of a comment or something illegal.
					// Either way, stop evaluation here.
				break;
			}
		}
		numDigits = strlen(hexOffset) - 1;
		int32_t power = 0;
		for (size_t count = numDigits; count >= 0; count--, power++)
		{
			uint8_t currentDigit = toupper(hexOffset[count]);
			if (currentDigit >= 'A' && currentDigit <= 'F')
			{
				result += (currentDigit - 'A' + 10) << (4 * power);
			}
			else if (currentDigit >= '0' && currentDigit <= '9')
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
	return (result);
}

//---------------------------------------------------------------------------
uint16_t
FitIniFile::textToUShort(std::wstring_view num)
{
	uint16_t result = 0;
	//------------------------------------
	// Check if Hex Number
	std::wstring_view hexOffset = (std::wstring_view)strstr(num, "0x");
	if (hexOffset == nullptr)
	{
		result = atol(num);
	}
	else
	{
		hexOffset += 2;
		int32_t numDigits = strlen(hexOffset) - 1;
		for (size_t i = 0; i <= numDigits; i++)
		{
			if (!isalnum(hexOffset[i]) || (isalpha(hexOffset[i]) && toupper(hexOffset[i]) > 'F'))
			{
				hexOffset[i] = 0; // we've reach a "wrong" character. Either
					// start of a comment or something illegal.
					// Either way, stop evaluation here.
				break;
			}
		}
		numDigits = strlen(hexOffset) - 1;
		int32_t power = 0;
		for (size_t count = numDigits; count >= 0; count--, power++)
		{
			uint8_t currentDigit = toupper(hexOffset[count]);
			if (currentDigit >= 'A' && currentDigit <= 'F')
			{
				result += (currentDigit - 'A' + 10) << (4 * power);
			}
			else if (currentDigit >= '0' && currentDigit <= '9')
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
	return (result);
}

//---------------------------------------------------------------------------
uint8_t
FitIniFile::textToUChar(std::wstring_view num)
{
	uint8_t result = 0;
	//------------------------------------
	// Check if Hex Number
	std::wstring_view hexOffset = (std::wstring_view)strstr(num, "0x");
	if (hexOffset == nullptr)
	{
		result = atol(num);
	}
	else
	{
		hexOffset += 2;
		int32_t numDigits = strlen(hexOffset) - 1;
		for (size_t i = 0; i <= numDigits; i++)
		{
			if (!isalnum(hexOffset[i]) || (isalpha(hexOffset[i]) && toupper(hexOffset[i]) > 'F'))
			{
				hexOffset[i] = 0; // we've reach a "wrong" character. Either
					// start of a comment or something illegal.
					// Either way, stop evaluation here.
				break;
			}
		}
		numDigits = strlen(hexOffset) - 1;
		int32_t power = 0;
		for (size_t count = numDigits; count >= 0; count--, power++)
		{
			uint8_t currentDigit = toupper(hexOffset[count]);
			if (currentDigit >= 'A' && currentDigit <= 'F')
			{
				result += (currentDigit - 'A' + 10) << (4 * power);
			}
			else if (currentDigit >= '0' && currentDigit <= '9')
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
	return (result);
}

//---------------------------------------------------------------------------
bool FitIniFile::booleanToLong(std::wstring_view num)
{
	wchar_t testChar = 0;
	while (num[testChar] && isspace(num[testChar]))
		testChar++;
	if ((toupper(num[testChar]) == 'F') || (toupper(num[testChar]) == '0'))
		return false;
	else
		return (TRUE);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::floatToText(std::wstring_view result, float num, uint32_t bufLen)
{
	wchar_t temp[250];
	sprintf(temp, "%f4", num);
	uint32_t numLength = strlen(temp);
	if (numLength >= bufLen)
		return (BUFFER_TOO_SMALL);
	strncpy(result, temp, numLength);
	result[numLength] = '\0';
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::longToTextDec(std::wstring_view result, int32_t num, uint32_t bufLen)
{
	wchar_t temp[250];
	sprintf(temp, "%d", num);
	uint32_t numLength = strlen(temp);
	if (numLength >= bufLen)
		return (BUFFER_TOO_SMALL);
	strncpy(result, temp, numLength);
	result[numLength] = '\0';
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::longToTextHex(std::wstring_view result, int32_t num, uint32_t bufLen)
{
	wchar_t temp[250];
	sprintf(temp, "0x%x", num);
	uint32_t numLength = strlen(temp);
	if (numLength >= bufLen)
		return (BUFFER_TOO_SMALL);
	strncpy(result, temp, numLength);
	result[numLength] = '\0';
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::shortToTextDec(std::wstring_view result, int16_t num, uint32_t bufLen)
{
	wchar_t temp[250];
	sprintf(temp, "%d", num);
	uint32_t numLength = strlen(temp);
	if (numLength >= bufLen)
		return (BUFFER_TOO_SMALL);
	strncpy(result, temp, numLength);
	result[numLength] = '\0';
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::shortToTextHex(std::wstring_view result, int16_t num, uint32_t bufLen)
{
	wchar_t temp[250];
	sprintf(temp, "0x%x", num);
	uint32_t numLength = strlen(temp);
	if (numLength >= bufLen)
		return (BUFFER_TOO_SMALL);
	strncpy(result, temp, numLength);
	result[numLength] = '\0';
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::byteToTextDec(std::wstring_view result, byte num, uint32_t bufLen)
{
	wchar_t temp[250];
	sprintf(temp, "%d", num);
	uint32_t numLength = strlen(temp);
	if (numLength >= bufLen)
		return (BUFFER_TOO_SMALL);
	strncpy(result, temp, numLength);
	result[numLength] = '\0';
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::byteToTextHex(std::wstring_view result, byte num, uint32_t bufLen)
{
	wchar_t temp[250];
	sprintf(temp, "0x%x", num);
	uint32_t numLength = strlen(temp);
	if (numLength >= bufLen)
		return (BUFFER_TOO_SMALL);
	strncpy(result, temp, numLength);
	result[numLength] = '\0';
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::open(std::wstring_view filename, FileMode _mode, int32_t numChild)
{
	int32_t result = MechFile::open(filename, _mode, numChild);
	if (result != NO_ERROR)
		return (result);
	seek(0);
	result = afterOpen();
	return (result);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::open(std::unique_ptr<File> _parent, uint32_t fileSize, int32_t numChild)
{
	numChild = -1; // Force all parented FitINIs to load from RAM.
	int32_t result = MechFile::open(_parent, fileSize, numChild);
	if (result != NO_ERROR)
		return (result);
	result = afterOpen();
	return (result);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::create(std::wstring_view filename)
{
	int32_t result = MechFile::create(filename);
	afterOpen();
	return (result);
}

int32_t
FitIniFile::createWithCase(std::wstring_view filename)
{
	int32_t result = MechFile::createWithCase(filename);
	afterOpen();
	return result;
}

//---------------------------------------------------------------------------
void FitIniFile::close(void)
{
	if (isOpen())
	{
		atClose();
		MechFile::close();
	}
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::seekBlock(std::wstring_view blockId)
{
	uint32_t blockNum = 0;
	while ((blockNum < m_totalBlocks) && (strcmp(m_fileBlocks[blockNum].blockId, blockId) != 0))
	{
		blockNum++;
	}
	if (blockNum == m_totalBlocks)
	{
		return (BLOCK_NOT_FOUND);
	}
	//----------------------------------------
	// Block was found, seek to that position
	seek(m_fileBlocks[blockNum].blockOffset);
	//----------------------------------------
	// Setup all current Block Info
	m_currentBlockId = m_fileBlocks[blockNum].blockId;
	m_currentBlockOffset = m_fileBlocks[blockNum].blockOffset;
	blockNum++;
	if (blockNum == m_totalBlocks)
	{
		m_currentBlockSize = getLength() - m_currentBlockOffset;
	}
	else
	{
		m_currentBlockSize = m_fileBlocks[blockNum].blockOffset - m_currentBlockOffset;
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::readIdFloat(std::wstring_view varName, float& value)
{
	wchar_t line[BUFFERSIZE];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------
	// Put prefix on varName.
	sprintf(searchString, "f %s", varName);
	//--------------------------------
	// Search line by line for varName
	int32_t testy;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		testy = strnicmp(line, searchString, strlen(searchString));
		if (testy == 0)
		{
			std::wstring_view tc = &line[strlen(searchString)];
			while (isspace(*tc))
				tc++;
			if (*tc != '=')
				testy = 1;
		}
	} while ((testy != 0) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		value = 0.0;
		return (VARIABLE_NOT_FOUND);
	}
	std::wstring_view equalSign = strstr(line, "=");
	if (equalSign)
	{
		equalSign++;
		value = textToFloat(equalSign);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::readIdDouble(std::wstring_view varName, double& value)
{
	wchar_t line[BUFFERSIZE];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------
	// Put prefix on varName.
	sprintf(searchString, "f %s", varName);
	//--------------------------------
	// Search line by line for varName
	int32_t testy;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		testy = strnicmp(line, searchString, strlen(searchString));
		if (testy == 0)
		{
			std::wstring_view tc = &line[strlen(searchString)];
			while (isspace(*tc))
				tc++;
			if (*tc != '=')
				testy = 1;
		}
	} while ((testy != 0) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		value = 0.0;
		return (VARIABLE_NOT_FOUND);
	}
	std::wstring_view equalSign = strstr(line, "=");
	if (equalSign)
	{
		equalSign++;
		value = textToDouble(equalSign);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::readIdInt(std::wstring_view varName, int32_t& value) {
	return readIdLong(varName, static_cast<long32_t>(value))}

HRESULT FitIniFile::readIdUInt(std::wstring_view varName, uint32_t& value) {
	return readIdULong(varName, static_cast<ULONG>(value))}

//---------------------------------------------------------------------------
HRESULT FitIniFile::readIdLong(std::wstring_view varName, long32_t& value)
{
	wchar_t line[BUFFERSIZE];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------
	// Put prefix on varName.
	sprintf(searchString, "l %s", varName);
	//--------------------------------
	// Search line by line for varName
	int32_t testy;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		testy = strnicmp(line, searchString, strlen(searchString));
		if (testy == 0)
		{
			std::wstring_view tc = &line[strlen(searchString)];
			while (isspace(*tc))
				tc++;
			if (*tc != '=')
				testy = 1;
		}
	} while ((testy != 0) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		value = 0;
		return (VARIABLE_NOT_FOUND);
	}
	std::wstring_view equalSign = strstr(line, "=");
	if (equalSign)
	{
		equalSign++;
		value = textToLong(equalSign);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::readIdBoolean(std::wstring_view varName, bool& value)
{
	wchar_t line[BUFFERSIZE];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------
	// Put prefix on varName.
	sprintf(searchString, "b %s", varName);
	//--------------------------------
	// Search line by line for varName
	int32_t testy;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		testy = strnicmp(line, searchString, strlen(searchString));
		if (testy == 0)
		{
			std::wstring_view tc = &line[strlen(searchString)];
			while (isspace(*tc))
				tc++;
			if (*tc != '=')
				testy = 1;
		}
	} while ((testy != 0) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		value = 0;
		return (VARIABLE_NOT_FOUND);
	}
	std::wstring_view equalSign = strstr(line, "=");
	if (equalSign)
	{
		equalSign++;
		value = booleanToLong(equalSign);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::readIdShort(std::wstring_view varName, int16_t& value)
{
	wchar_t line[BUFFERSIZE];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------
	// Put prefix on varName.
	sprintf(searchString, "s %s", varName);
	//--------------------------------
	// Search line by line for varName
	int32_t testy;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		testy = strnicmp(line, searchString, strlen(searchString));
		if (testy == 0)
		{
			std::wstring_view tc = &line[strlen(searchString)];
			while (isspace(*tc))
				tc++;
			if (*tc != '=')
				testy = 1;
		}
	} while ((testy != 0) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		value = 0;
		return (VARIABLE_NOT_FOUND);
	}
	std::wstring_view equalSign = strstr(line, "=");
	if (equalSign)
	{
		equalSign++;
		value = textToShort(equalSign);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::readIdChar(std::wstring_view varName, wchar_t& value)
{
	wchar_t line[BUFFERSIZE];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------
	// Put prefix on varName.
	sprintf(searchString, "c %s", varName);
	//--------------------------------
	// Search line by line for varName
	int32_t testy;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		testy = strnicmp(line, searchString, strlen(searchString));
		if (testy == 0)
		{
			std::wstring_view tc = &line[strlen(searchString)];
			while (isspace(*tc))
				tc++;
			if (*tc != '=')
				testy = 1;
		}
	} while ((testy != 0) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		value = 0;
		return (VARIABLE_NOT_FOUND);
	}
	std::wstring_view equalSign = strstr(line, "=");
	if (equalSign)
	{
		equalSign++;
		value = textToChar(equalSign);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::readIdULong(std::wstring_view varName, ULONG& value)
{
	wchar_t line[BUFFERSIZE];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------
	// Put prefix on varName.
	sprintf(searchString, "ul %s", varName);
	//--------------------------------
	// Search line by line for varName
	int32_t testy;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		testy = strnicmp(line, searchString, strlen(searchString));
		if (testy == 0)
		{
			std::wstring_view tc = &line[strlen(searchString)];
			while (isspace(*tc))
				tc++;
			if (*tc != '=')
				testy = 1;
		}
	} while ((testy != 0) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		value = 0;
		return (VARIABLE_NOT_FOUND);
	}
	std::wstring_view equalSign = strstr(line, "=");
	if (equalSign)
	{
		equalSign++;
		value = textToULong(equalSign);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::readIdUShort(std::wstring_view varName, uint16_t& value)
{
	wchar_t line[BUFFERSIZE];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------
	// Put prefix on varName.
	sprintf(searchString, "us %s", varName);
	//--------------------------------
	// Search line by line for varName
	int32_t testy;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		testy = strnicmp(line, searchString, strlen(searchString));
		if (testy == 0)
		{
			std::wstring_view tc = &line[strlen(searchString)];
			while (isspace(*tc))
				tc++;
			if (*tc != '=')
				testy = 1;
		}
	} while ((testy != 0) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		value = 0;
		return (VARIABLE_NOT_FOUND);
	}
	std::wstring_view equalSign = strstr(line, "=");
	if (equalSign)
	{
		equalSign++;
		value = textToUShort(equalSign);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::readIdUChar(std::wstring_view varName, uint8_t& value)
{
	wchar_t line[BUFFERSIZE];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------
	// Put prefix on varName.
	sprintf(searchString, "uc %s", varName);
	//--------------------------------
	// Search line by line for varName
	int32_t testy;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		testy = strnicmp(line, searchString, strlen(searchString));
		if (testy == 0)
		{
			std::wstring_view tc = &line[strlen(searchString)];
			while (isspace(*tc))
				tc++;
			if (*tc != '=')
				testy = 1;
		}
	} while ((testy != 0) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		value = 0;
		return (VARIABLE_NOT_FOUND);
	}
	std::wstring_view equalSign = strstr(line, "=");
	if (equalSign)
	{
		equalSign++;
		value = textToUChar(equalSign);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::copyString(std::wstring_view dest, std::wstring_view src, uint32_t bufLen)
{
	uint32_t offset = 0;
	//---------------------
	// Find starting Quote
	while (*src != '"')
	{
		src++;
	}
	src++; // One more to get to first character past quote.
	//---------------------------------------
	// Copy each character until close quote
	while (*src != '"' && offset < bufLen)
	{
		dest[offset] = *src;
		src++;
		offset++;
	}
	//----------------------------------------------------
	// If this string is longer than buffer, let em know.
	if (offset == bufLen)
	{
		return (BUFFER_TOO_SMALL);
	}
	//---------------------------------
	// otherwise, nullptr term and return
	dest[offset] = '\0';
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::readIdString(std::wstring_view varName, std::wstring_view result, uint32_t bufferSize)
{
	wchar_t line[2048];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------
	// Put prefix on varName.
	sprintf(searchString, "st %s", varName);
	//--------------------------------
	// Search line by line for varName
	int32_t testy;
	do
	{
		readLine((uint8_t*)line, 2047);
		testy = strnicmp(line, searchString, strlen(searchString));
		if (testy == 0)
		{
			std::wstring_view tc = &line[strlen(searchString)];
			while (isspace(*tc))
				tc++;
			if (*tc != '=')
				testy = 1;
		}
	} while ((testy != 0) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	std::wstring_view equalSign = strstr(line, "=");
	if (equalSign)
	{
		equalSign++;
		std::wstring_view pFound = nullptr;
		std::wstring_view pFirstEqual = strstr(equalSign, "\"");
		// strings can span more than one line, make sure there is another equal
		// sign
		if (pFirstEqual && !strstr(pFirstEqual + 1, "\""))
		{
			int32_t curLen = strlen(equalSign);
			wchar_t tmpLine[2047];
			do
			{
				readLine((uint8_t*)tmpLine, 2047);
				pFound = strstr(tmpLine, "\"");
				int32_t addedLen = strlen(tmpLine) + 1;
				if (curLen + addedLen < 2048)
				{
					strcat(equalSign, tmpLine);
					strcat(equalSign, "\n");
					curLen += addedLen;
				}
				else
					break;
			} while (!pFound && (logicalPosition < endOfBlock));
		}
		int32_t errorCode = copyString(result, equalSign, bufferSize);
		if (errorCode != NO_ERROR)
			return (errorCode);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::getIdStringLength(std::wstring_view varName)
{
	wchar_t line[BUFFERSIZE];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------
	// Put prefix on varName.
	sprintf(searchString, "st %s", varName);
	//--------------------------------
	// Search line by line for varName
	int32_t testy;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		testy = strnicmp(line, searchString, strlen(searchString));
		if (testy == 0)
		{
			std::wstring_view tc = &line[strlen(searchString)];
			while (isspace(*tc))
				tc++;
			if (*tc != '=')
				testy = 1;
		}
	} while ((testy != 0) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	std::wstring_view equalSign = strchr(line, '"');
	if (equalSign)
	{
		equalSign++;
		std::wstring_view end = equalSign;
		while (*end != '"' && *end != 0)
			end++;
		if (*end)
			return (end - equalSign + 1); // + for terminator
		else
			return (SYNTAX_ERROR);
	}
	return (SYNTAX_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::readIdFloatArray(std::wstring_view varName, float* result, uint32_t numElements)
{
	wchar_t line[BUFFERSIZE];
	wchar_t frontSearch[10];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------------------------------------------------
	// Create two search strings so that we can match any number in []
	sprintf(frontSearch, "f[");
	sprintf(searchString, "] %s", varName);
	//--------------------------------
	// Search line by line for varName
	std::wstring_view fSearch = nullptr;
	std::wstring_view bSearch = nullptr;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		fSearch = strstr(line, frontSearch);
		bSearch = strstr(line, searchString);
	} while (((fSearch == nullptr) || (bSearch == nullptr)) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	//--------------------------------------
	// Get number of elements in array.
	wchar_t elementString[10];
	uint32_t actualElements;
	fSearch += 2; // Move pointer to first number in brackets.
	int32_t numDigits = bSearch - fSearch;
	if (numDigits > 9)
		return (TOO_MANY_ELEMENTS);
	strncpy(elementString, fSearch, numDigits);
	elementString[numDigits] = '\0';
	actualElements = textToULong(elementString);
	if (actualElements > numElements)
		return (USER_ARRAY_TOO_SMALL);
	//------------------------------
	// Parse out the elements here.
	std::wstring_view equalSign = strstr(line, "=");
	uint32_t elementsRead = 0;
	if (equalSign)
	{
		equalSign++; // Move to wchar_t past equal sign.
		//--------------------------------------------------------------------------------
		// Now, loop until we reach the end of block or we've read in all of the
		// elements
		while ((logicalPosition < endOfBlock) && (elementsRead < actualElements))
		{
			int32_t errorCode = getNextWord(equalSign, elementString, 9);
			if (errorCode == GET_NEXT_LINE)
			{
				readLine((uint8_t*)line, BUFFERSIZE - 1);
				equalSign = line;
				continue;
			}
			if (errorCode != NO_ERROR)
			{
				return (errorCode);
			}
			result[elementsRead] = textToFloat(elementString);
			elementsRead++;
		}
		if (logicalPosition >= endOfBlock && elementsRead < actualElements)
			return (NOT_ENOUGH_ELEMENTS_FOR_ARRAY);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::readIdLongArray(std::wstring_view varName, int32_t* result, uint32_t numElements)
{
	wchar_t line[BUFFERSIZE];
	wchar_t frontSearch[10];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------------------------------------------------
	// Create two search strings so that we can match any number in []
	sprintf(frontSearch, "l[");
	sprintf(searchString, "] %s", varName);
	//--------------------------------
	// Search line by line for varName
	std::wstring_view fSearch = nullptr;
	std::wstring_view bSearch = nullptr;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		fSearch = strstr(line, frontSearch);
		bSearch = strstr(line, searchString);
	} while (((fSearch == nullptr) || (bSearch == nullptr)) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	//--------------------------------------
	// Get number of elements in array.
	wchar_t elementString[10];
	uint32_t actualElements;
	fSearch += 2; // Move pointer to first number in brackets.
	int32_t numDigits = bSearch - fSearch;
	if (numDigits > 9)
		return (TOO_MANY_ELEMENTS);
	strncpy(elementString, fSearch, numDigits);
	elementString[numDigits] = '\0';
	actualElements = textToULong(elementString);
	if (actualElements > numElements)
		return (USER_ARRAY_TOO_SMALL);
	//------------------------------
	// Parse out the elements here.
	std::wstring_view equalSign = strstr(line, "=");
	uint32_t elementsRead = 0;
	if (equalSign)
	{
		equalSign++; // Move to wchar_t past equal sign.
		//--------------------------------------------------------------------------------
		// Now, loop until we reach the end of block or we've read in all of the
		// elements
		while ((logicalPosition < endOfBlock) && (elementsRead < actualElements))
		{
			int32_t errorCode = getNextWord(equalSign, elementString, 9);
			if (errorCode == GET_NEXT_LINE)
			{
				readLine((uint8_t*)line, BUFFERSIZE - 1);
				equalSign = line;
				continue;
			}
			if (errorCode != NO_ERROR)
			{
				return (errorCode);
			}
			result[elementsRead] = textToLong(elementString);
			elementsRead++;
		}
		if (logicalPosition >= endOfBlock && elementsRead < actualElements)
			return (NOT_ENOUGH_ELEMENTS_FOR_ARRAY);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::readIdULongArray(std::wstring_view varName, uint32_t* result, uint32_t numElements)
{
	wchar_t line[BUFFERSIZE];
	wchar_t frontSearch[10];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------------------------------------------------
	// Create two search strings so that we can match any number in []
	sprintf(frontSearch, "ul[");
	sprintf(searchString, "] %s", varName);
	//--------------------------------
	// Search line by line for varName
	std::wstring_view fSearch = nullptr;
	std::wstring_view bSearch = nullptr;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		fSearch = strstr(line, frontSearch);
		bSearch = strstr(line, searchString);
	} while (((fSearch == nullptr) || (bSearch == nullptr)) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	//--------------------------------------
	// Get number of elements in array.
	wchar_t elementString[10];
	uint32_t actualElements;
	fSearch += 3; // Move pointer to first number in brackets.
	int32_t numDigits = bSearch - fSearch;
	if (numDigits > 9)
		return (TOO_MANY_ELEMENTS);
	strncpy(elementString, fSearch, numDigits);
	elementString[numDigits] = '\0';
	actualElements = textToULong(elementString);
	if (actualElements > numElements)
		return (USER_ARRAY_TOO_SMALL);
	//------------------------------
	// Parse out the elements here.
	std::wstring_view equalSign = strstr(line, "=");
	uint32_t elementsRead = 0;
	if (equalSign)
	{
		equalSign++; // Move to wchar_t past equal sign.
		//--------------------------------------------------------------------------------
		// Now, loop until we reach the end of block or we've read in all of the
		// elements
		while ((logicalPosition < endOfBlock) && (elementsRead < actualElements))
		{
			int32_t errorCode = getNextWord(equalSign, elementString, 9);
			if (errorCode == GET_NEXT_LINE)
			{
				readLine((uint8_t*)line, BUFFERSIZE - 1);
				equalSign = line;
				continue;
			}
			if (errorCode != NO_ERROR)
			{
				return (errorCode);
			}
			result[elementsRead] = textToULong(elementString);
			elementsRead++;
		}
		if (logicalPosition >= endOfBlock && elementsRead < actualElements)
			return (NOT_ENOUGH_ELEMENTS_FOR_ARRAY);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::readIdShortArray(std::wstring_view varName, int16_t* result, uint32_t numElements)
{
	wchar_t line[BUFFERSIZE];
	wchar_t frontSearch[10];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------------------------------------------------
	// Create two search strings so that we can match any number in []
	sprintf(frontSearch, "s[");
	sprintf(searchString, "] %s", varName);
	//--------------------------------
	// Search line by line for varName
	std::wstring_view fSearch = nullptr;
	std::wstring_view bSearch = nullptr;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		fSearch = strstr(line, frontSearch);
		bSearch = strstr(line, searchString);
	} while (((fSearch == nullptr) || (bSearch == nullptr)) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	//--------------------------------------
	// Get number of elements in array.
	wchar_t elementString[10];
	uint32_t actualElements;
	fSearch += 2; // Move pointer to first number in brackets.
	int32_t numDigits = bSearch - fSearch;
	if (numDigits > 9)
		return (TOO_MANY_ELEMENTS);
	strncpy(elementString, fSearch, numDigits);
	elementString[numDigits] = '\0';
	actualElements = textToULong(elementString);
	if (actualElements > numElements)
		return (USER_ARRAY_TOO_SMALL);
	//------------------------------
	// Parse out the elements here.
	std::wstring_view equalSign = strstr(line, "=");
	uint32_t elementsRead = 0;
	if (equalSign)
	{
		equalSign++; // Move to wchar_t past equal sign.
		//--------------------------------------------------------------------------------
		// Now, loop until we reach the end of block or we've read in all of the
		// elements
		while ((logicalPosition < endOfBlock) && (elementsRead < actualElements))
		{
			int32_t errorCode = getNextWord(equalSign, elementString, 9);
			if (errorCode == GET_NEXT_LINE)
			{
				readLine((uint8_t*)line, BUFFERSIZE - 1);
				equalSign = line;
				continue;
			}
			if (errorCode != NO_ERROR)
			{
				return (errorCode);
			}
			result[elementsRead] = textToShort(elementString);
			elementsRead++;
		}
		if (logicalPosition >= endOfBlock && elementsRead < actualElements)
			return (NOT_ENOUGH_ELEMENTS_FOR_ARRAY);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::readIdUShortArray(std::wstring_view varName, uint16_t* result, uint32_t numElements)
{
	wchar_t line[BUFFERSIZE];
	wchar_t frontSearch[10];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------------------------------------------------
	// Create two search strings so that we can match any number in []
	sprintf(frontSearch, "us[");
	sprintf(searchString, "] %s", varName);
	//--------------------------------
	// Search line by line for varName
	std::wstring_view fSearch = nullptr;
	std::wstring_view bSearch = nullptr;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		fSearch = strstr(line, frontSearch);
		bSearch = strstr(line, searchString);
	} while (((fSearch == nullptr) || (bSearch == nullptr)) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	//--------------------------------------
	// Get number of elements in array.
	wchar_t elementString[10];
	uint32_t actualElements;
	fSearch += 3; // Move pointer to first number in brackets.
	int32_t numDigits = bSearch - fSearch;
	if (numDigits > 9)
		return (TOO_MANY_ELEMENTS);
	strncpy(elementString, fSearch, numDigits);
	elementString[numDigits] = '\0';
	actualElements = textToULong(elementString);
	if (actualElements > numElements)
		return (USER_ARRAY_TOO_SMALL);
	//------------------------------
	// Parse out the elements here.
	std::wstring_view equalSign = strstr(line, "=");
	uint32_t elementsRead = 0;
	if (equalSign)
	{
		equalSign++; // Move to wchar_t past equal sign.
		//--------------------------------------------------------------------------------
		// Now, loop until we reach the end of block or we've read in all of the
		// elements
		while ((logicalPosition < endOfBlock) && (elementsRead < actualElements))
		{
			int32_t errorCode = getNextWord(equalSign, elementString, 9);
			if (errorCode == GET_NEXT_LINE)
			{
				readLine((uint8_t*)line, BUFFERSIZE - 1);
				equalSign = line;
				continue;
			}
			if (errorCode != NO_ERROR)
			{
				return (errorCode);
			}
			result[elementsRead] = textToUShort(elementString);
			elementsRead++;
		}
		if (logicalPosition >= endOfBlock && elementsRead < actualElements)
			return (NOT_ENOUGH_ELEMENTS_FOR_ARRAY);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::readIdCharArray(std::wstring_view varName, std::wstring_view result, uint32_t numElements)
{
	wchar_t line[BUFFERSIZE];
	wchar_t frontSearch[10];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------------------------------------------------
	// Create two search strings so that we can match any number in []
	sprintf(frontSearch, "c[");
	sprintf(searchString, "] %s", varName);
	//--------------------------------
	// Search line by line for varName
	std::wstring_view fSearch = nullptr;
	std::wstring_view bSearch = nullptr;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		fSearch = strstr(line, frontSearch);
		bSearch = strstr(line, searchString);
	} while (((fSearch == nullptr) || (bSearch == nullptr)) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	//--------------------------------------
	// Get number of elements in array.
	wchar_t elementString[10];
	uint32_t actualElements;
	fSearch += 2; // Move pointer to first number in brackets.
	int32_t numDigits = bSearch - fSearch;
	if (numDigits > 9)
		return (TOO_MANY_ELEMENTS);
	strncpy(elementString, fSearch, numDigits);
	elementString[numDigits] = '\0';
	actualElements = textToULong(elementString);
	if (actualElements > numElements)
		return (USER_ARRAY_TOO_SMALL);
	//------------------------------
	// Parse out the elements here.
	std::wstring_view equalSign = strstr(line, "=");
	uint32_t elementsRead = 0;
	if (equalSign)
	{
		equalSign++; // Move to wchar_t past equal sign.
		//--------------------------------------------------------------------------------
		// Now, loop until we reach the end of block or we've read in all of the
		// elements
		while ((logicalPosition < endOfBlock) && (elementsRead < actualElements))
		{
			int32_t errorCode = getNextWord(equalSign, elementString, 9);
			if (errorCode == GET_NEXT_LINE)
			{
				readLine((uint8_t*)line, BUFFERSIZE - 1);
				equalSign = line;
				continue;
			}
			if (errorCode != NO_ERROR)
			{
				return (errorCode);
			}
			result[elementsRead] = textToChar(elementString);
			elementsRead++;
		}
		if (logicalPosition >= endOfBlock && elementsRead < actualElements)
			return (NOT_ENOUGH_ELEMENTS_FOR_ARRAY);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::readIdUCharArray(std::wstring_view varName, uint8_t* result, uint32_t numElements)
{
	wchar_t line[BUFFERSIZE];
	wchar_t frontSearch[10];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------------------------------------------------
	// Create two search strings so that we can match any number in []
	sprintf(frontSearch, "uc[");
	sprintf(searchString, "] %s", varName);
	//--------------------------------
	// Search line by line for varName
	std::wstring_view fSearch = nullptr;
	std::wstring_view bSearch = nullptr;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		fSearch = strstr(line, frontSearch);
		bSearch = strstr(line, searchString);
	} while (((fSearch == nullptr) || (bSearch == nullptr)) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	//--------------------------------------
	// Get number of elements in array.
	wchar_t elementString[10];
	uint32_t actualElements;
	fSearch += 3; // Move pointer to first number in brackets.
	int32_t numDigits = bSearch - fSearch;
	if (numDigits > 9)
		return (TOO_MANY_ELEMENTS);
	strncpy(elementString, fSearch, numDigits);
	elementString[numDigits] = '\0';
	actualElements = textToULong(elementString);
	if (actualElements > numElements)
		return (USER_ARRAY_TOO_SMALL);
	//------------------------------
	// Parse out the elements here.
	std::wstring_view equalSign = strstr(line, "=");
	uint32_t elementsRead = 0;
	if (equalSign)
	{
		equalSign++; // Move to wchar_t past equal sign.
		//--------------------------------------------------------------------------------
		// Now, loop until we reach the end of block or we've read in all of the
		// elements
		while ((logicalPosition < endOfBlock) && (elementsRead < actualElements))
		{
			int32_t errorCode = getNextWord(equalSign, elementString, 9);
			if (errorCode == GET_NEXT_LINE)
			{
				readLine((uint8_t*)line, BUFFERSIZE - 1);
				equalSign = line;
				continue;
			}
			if (errorCode != NO_ERROR)
			{
				return (errorCode);
			}
			result[elementsRead] = textToUChar(elementString);
			elementsRead++;
		}
		if (logicalPosition >= endOfBlock && elementsRead < actualElements)
			return (NOT_ENOUGH_ELEMENTS_FOR_ARRAY);
	}
	else
	{
		return (SYNTAX_ERROR);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
uint32_t
FitIniFile::getIdFloatArrayElements(std::wstring_view varName)
{
	wchar_t line[BUFFERSIZE];
	wchar_t frontSearch[10];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------------------------------------------------
	// Create two search strings so that we can match any number in []
	sprintf(frontSearch, "f[");
	sprintf(searchString, "] %s", varName);
	//--------------------------------
	// Search line by line for varName
	std::wstring_view fSearch = nullptr;
	std::wstring_view bSearch = nullptr;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		fSearch = strstr(line, frontSearch);
		bSearch = strstr(line, searchString);
	} while (((fSearch == nullptr) || (bSearch == nullptr)) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	//--------------------------------------
	// Get number of elements in array.
	wchar_t elementString[10];
	uint32_t actualElements;
	fSearch += 2; // Move pointer to first number in brackets.
	int32_t numDigits = bSearch - fSearch;
	if (numDigits > 9)
		return (TOO_MANY_ELEMENTS);
	strncpy(elementString, fSearch, numDigits);
	elementString[numDigits] = '\0';
	actualElements = textToULong(elementString);
	return (actualElements);
}

//---------------------------------------------------------------------------
uint32_t
FitIniFile::getIdLongArrayElements(std::wstring_view varName)
{
	wchar_t line[BUFFERSIZE];
	wchar_t frontSearch[10];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------------------------------------------------
	// Create two search strings so that we can match any number in []
	sprintf(frontSearch, "l[");
	sprintf(searchString, "] %s", varName);
	//--------------------------------
	// Search line by line for varName
	std::wstring_view fSearch = nullptr;
	std::wstring_view bSearch = nullptr;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		fSearch = strstr(line, frontSearch);
		bSearch = strstr(line, searchString);
	} while (((fSearch == nullptr) || (bSearch == nullptr)) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	//--------------------------------------
	// Get number of elements in array.
	wchar_t elementString[10];
	uint32_t actualElements;
	fSearch += 2; // Move pointer to first number in brackets.
	int32_t numDigits = bSearch - fSearch;
	if (numDigits > 9)
		return (TOO_MANY_ELEMENTS);
	strncpy(elementString, fSearch, numDigits);
	elementString[numDigits] = '\0';
	actualElements = textToULong(elementString);
	return (actualElements);
}

//---------------------------------------------------------------------------
uint32_t
FitIniFile::getIdULongArrayElements(std::wstring_view varName)
{
	wchar_t line[BUFFERSIZE];
	wchar_t frontSearch[10];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------------------------------------------------
	// Create two search strings so that we can match any number in []
	sprintf(frontSearch, "ul[");
	sprintf(searchString, "] %s", varName);
	//--------------------------------
	// Search line by line for varName
	std::wstring_view fSearch = nullptr;
	std::wstring_view bSearch = nullptr;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		fSearch = strstr(line, frontSearch);
		bSearch = strstr(line, searchString);
	} while (((fSearch == nullptr) || (bSearch == nullptr)) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	//--------------------------------------
	// Get number of elements in array.
	wchar_t elementString[10];
	uint32_t actualElements;
	fSearch += 3; // Move pointer to first number in brackets.
	int32_t numDigits = bSearch - fSearch;
	if (numDigits > 9)
		return (TOO_MANY_ELEMENTS);
	strncpy(elementString, fSearch, numDigits);
	elementString[numDigits] = '\0';
	actualElements = textToULong(elementString);
	return (actualElements);
}

//---------------------------------------------------------------------------
uint32_t
FitIniFile::getIdShortArrayElements(std::wstring_view varName)
{
	wchar_t line[BUFFERSIZE];
	wchar_t frontSearch[10];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------------------------------------------------
	// Create two search strings so that we can match any number in []
	sprintf(frontSearch, "s[");
	sprintf(searchString, "] %s", varName);
	//--------------------------------
	// Search line by line for varName
	std::wstring_view fSearch = nullptr;
	std::wstring_view bSearch = nullptr;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		fSearch = strstr(line, frontSearch);
		bSearch = strstr(line, searchString);
	} while (((fSearch == nullptr) || (bSearch == nullptr)) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	//--------------------------------------
	// Get number of elements in array.
	wchar_t elementString[10];
	uint32_t actualElements;
	fSearch += 2; // Move pointer to first number in brackets.
	int32_t numDigits = bSearch - fSearch;
	if (numDigits > 9)
		return (TOO_MANY_ELEMENTS);
	strncpy(elementString, fSearch, numDigits);
	elementString[numDigits] = '\0';
	actualElements = textToULong(elementString);
	return (actualElements);
}

//---------------------------------------------------------------------------
uint32_t
FitIniFile::getIdUShortArrayElements(std::wstring_view varName)
{
	wchar_t line[BUFFERSIZE];
	wchar_t frontSearch[10];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------------------------------------------------
	// Create two search strings so that we can match any number in []
	sprintf(frontSearch, "us[");
	sprintf(searchString, "] %s", varName);
	//--------------------------------
	// Search line by line for varName
	std::wstring_view fSearch = nullptr;
	std::wstring_view bSearch = nullptr;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		fSearch = strstr(line, frontSearch);
		bSearch = strstr(line, searchString);
	} while (((fSearch == nullptr) || (bSearch == nullptr)) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	//--------------------------------------
	// Get number of elements in array.
	wchar_t elementString[10];
	uint32_t actualElements;
	fSearch += 3; // Move pointer to first number in brackets.
	int32_t numDigits = bSearch - fSearch;
	if (numDigits > 9)
		return (TOO_MANY_ELEMENTS);
	strncpy(elementString, fSearch, numDigits);
	elementString[numDigits] = '\0';
	actualElements = textToULong(elementString);
	return (actualElements);
}

//---------------------------------------------------------------------------
uint32_t
FitIniFile::getIdCharArrayElements(std::wstring_view varName)
{
	wchar_t line[BUFFERSIZE];
	wchar_t frontSearch[10];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------------------------------------------------
	// Create two search strings so that we can match any number in []
	sprintf(frontSearch, "c[");
	sprintf(searchString, "] %s", varName);
	//--------------------------------
	// Search line by line for varName
	std::wstring_view fSearch = nullptr;
	std::wstring_view bSearch = nullptr;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		fSearch = strstr(line, frontSearch);
		bSearch = strstr(line, searchString);
	} while (((fSearch == nullptr) || (bSearch == nullptr)) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	//--------------------------------------
	// Get number of elements in array.
	wchar_t elementString[10];
	uint32_t actualElements;
	fSearch += 2; // Move pointer to first number in brackets.
	int32_t numDigits = bSearch - fSearch;
	if (numDigits > 9)
		return (TOO_MANY_ELEMENTS);
	strncpy(elementString, fSearch, numDigits);
	elementString[numDigits] = '\0';
	actualElements = textToULong(elementString);
	return (actualElements);
}

//---------------------------------------------------------------------------
uint32_t
FitIniFile::getIdUCharArrayElements(std::wstring_view varName)
{
	wchar_t line[BUFFERSIZE];
	wchar_t frontSearch[10];
	wchar_t searchString[BUFFERSIZE];
	//--------------------------------
	// Always read from top of Block.
	seek(m_currentBlockOffset);
	uint32_t endOfBlock = m_currentBlockOffset + m_currentBlockSize;
	//------------------------------------------------------------------
	// Create two search strings so that we can match any number in []
	sprintf(frontSearch, "uc[");
	sprintf(searchString, "] %s", varName);
	//--------------------------------
	// Search line by line for varName
	std::wstring_view fSearch = nullptr;
	std::wstring_view bSearch = nullptr;
	do
	{
		readLine((uint8_t*)line, BUFFERSIZE - 1);
		fSearch = strstr(line, frontSearch);
		bSearch = strstr(line, searchString);
	} while (((fSearch == nullptr) || (bSearch == nullptr)) && (logicalPosition < endOfBlock));
	if (logicalPosition >= endOfBlock)
	{
		return (VARIABLE_NOT_FOUND);
	}
	//--------------------------------------
	// Get number of elements in array.
	wchar_t elementString[10];
	uint32_t actualElements;
	fSearch += 3; // Move pointer to first number in brackets.
	int32_t numDigits = bSearch - fSearch;
	if (numDigits > 9)
		return (TOO_MANY_ELEMENTS);
	strncpy(elementString, fSearch, numDigits);
	elementString[numDigits] = '\0';
	actualElements = textToULong(elementString);
	return (actualElements);
}

auto format = "your %x format %d string %s";
auto size = std::snprintf(nullptr, 0, format /* Arguments go here*/);
std::string output(size + 1, '\0');
std::sprintf(&output[0], format, /* Arguments go here*/);

int sprintf_s(wchar_t* buffer, size_t sizeOfBuffer, const wchar_t* format[, argument]...);

//---------------------------------------------------------------------------

template <typename TVAR>
static HRESULT
writeformatted(std::wstring_view varName, std::wstring_view format, TVAR value)
{
	uint8_t thisLine[BUFFERSIZE];
	int lenght = sprintf_s(static_cast<wchar_t*>(thisLine), BUFFERSIZE - 1, format, value);
	if (lenght == -1)
		throw std::system_error(errno, std::system_category(), __func__);

	return write(thisLine, lenght);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::writeBlock(std::wstring_view blockId)
{
	constexpr const wchar_t* format = "\r\n[%s]\r\n";

	return writeformatted<wchar_t>(blockId, format, 0);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::writeIdBoolean(std::wstring_view varName, bool value)
{
	constexpr const wchar_t* formattrue = "b %s = true\r\n";
	constexpr const wchar_t* formatfalse = "b %s = false\r\n";

	return (value == true) ? writeformatted<wchar_t>(varName, formattrue, 0)
						   : writeformatted<bool>(varName, formatfalse, 0);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::writeIdFloat(std::wstring_view varName, float value)
{
	constexpr const wchar_t* format = "f %s = %f\r\n";

	return writeformatted<float>(varName, format, value);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::writeIdLong(std::wstring_view varName, long32_t value)
{
	constexpr const wchar_t* format = "l %s = %d\r\n";

	return writeformatted<long32_t>(varName, format, value);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::writeIdULong(std::wstring_view varName, ULONG value)
{
	constexpr const wchar_t* format = "ul %s = %d\r\n";

	return writeformatted<ULONG>(varName, format, value);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::writeIdShort(std::wstring_view varName, int16_t value)
{
	constexpr const wchar_t* format = "s %s = %d\r\n";

	return writeformatted<int16_t>(varName, format, value);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::writeIdUShort(std::wstring_view varName, uint16_t value)
{
	constexpr const wchar_t* format = "us %s = %d\r\n";

	return writeformatted<uint16_t>(varName, format, value);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::writeIdChar(std::wstring_view varName, wchar_t value)
{
	constexpr const wchar_t* format = "c %s = %d\r\n";

	return writeformatted<wchar_t>(varName, format, value);
}

//---------------------------------------------------------------------------
HRESULT
FitIniFile::writeIdUChar(std::wstring_view varName, uint8_t value)
{
	constexpr const wchar_t* format = "uc %s = %d\r\n";

	return writeformatted<uint8_t>(varName, format, value);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::writeIdString(std::wstring_view varName, std::wstring_view result)
{
	wchar_t thisLine[4096];
	wchar_t tmpString[4000];
	memset(tmpString, 0, 4000);
	strncpy(tmpString, result, 3999);
	if (strlen(result) >= 4000)
		PAUSE(("string passed to WriteIdString is longer then 4000 characters"));
	sprintf(thisLine, "st %s = \"%s\"\r\n", varName, result);
	int32_t bytesWritten = write((uint8_t*)thisLine, strlen(thisLine));
	return (bytesWritten);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::writeIdUShortArray(std::wstring_view varName, uint16_t* array, uint32_t numElements)
{
	wchar_t thisLine[BUFFERSIZE];
	sprintf(thisLine, "us[%d] %s = %d,", numElements, varName, array[0]);
	int32_t bytesWritten = write((uint8_t*)thisLine, strlen(thisLine));
	for (size_t i = 1; i < (int32_t)numElements; i++)
	{
		sprintf(thisLine, "%d,", array[i]);
		bytesWritten += write((uint8_t*)thisLine, strlen(thisLine));
	}
	sprintf(thisLine, "\r\n");
	bytesWritten += write((uint8_t*)thisLine, strlen(thisLine));
	return (bytesWritten);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::writeIdLongArray(std::wstring_view varName, int32_t* array, uint32_t numElements)
{
	wchar_t thisLine[BUFFERSIZE];
	sprintf(thisLine, "l[%d] %s = %d,", numElements, varName, array[0]);
	int32_t bytesWritten = write((uint8_t*)thisLine, strlen(thisLine));
	for (size_t i = 1; i < (int32_t)numElements; i++)
	{
		sprintf(thisLine, "%d,", array[i]);
		bytesWritten += write((uint8_t*)thisLine, strlen(thisLine));
	}
	sprintf(thisLine, "\r\n");
	bytesWritten += write((uint8_t*)thisLine, strlen(thisLine));
	return (bytesWritten);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::writeIdFloatArray(std::wstring_view varName, float* array, uint32_t numElements)
{
	wchar_t thisLine[BUFFERSIZE];
	sprintf(thisLine, "f[%d] %s = %.2f,", numElements, varName, array[0]);
	int32_t bytesWritten = write((uint8_t*)thisLine, strlen(thisLine));
	for (size_t i = 1; i < (int32_t)numElements; i++)
	{
		sprintf(thisLine, " %.2f,", array[i]);
		bytesWritten += write((uint8_t*)thisLine, strlen(thisLine));
	}
	sprintf(thisLine, "\r\n");
	bytesWritten += write((uint8_t*)thisLine, strlen(thisLine));
	return (bytesWritten);
}

//---------------------------------------------------------------------------
int32_t
FitIniFile::writeIdUCharArray(std::wstring_view varName, uint8_t* array, uint32_t numElements)
{
	wchar_t thisLine[BUFFERSIZE];
	sprintf(thisLine, "uc[%d] %s = %d,", numElements, varName, array[0]);
	int32_t bytesWritten = write((uint8_t*)thisLine, strlen(thisLine));
	for (size_t i = 1; i < (int32_t)numElements; i++)
	{
		sprintf(thisLine, " %d,", array[i]);
		bytesWritten += write((uint8_t*)thisLine, strlen(thisLine));
	}
	sprintf(thisLine, "\r\n");
	bytesWritten += write((uint8_t*)thisLine, strlen(thisLine));
	return (bytesWritten);
}

//---------------------------------------------------------------------------
//
// Edit log
//
//---------------------------------------------------------------------------
