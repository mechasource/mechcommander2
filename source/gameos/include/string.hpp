#pragma once
//===========================================================================//
// File:	 String.hpp														 //
// Contents: String Routines												 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//



//
// Fixed length strings are used for error messages. No GOS memory is used and
// and functions that would cause the string to overrun are simply truncated.
//
// Can be initialized, appended to and passed as a PSTR
//
class FixedLengthString
{
private:
//
// Data
//
	size_t MaximumLength;
	size_t CurrentSize;
	PSTR Text;
//
// Visible stuff
//
public:
//
// Constructor
//
	inline FixedLengthString(size_t Length)
	{
		gosASSERT(Length != 0);
		Text = (PSTR)malloc(Length);
		MaximumLength = Length;
		CurrentSize = 0;
		if(Text) *Text = 0;
	}
//
// Destructor
//
	inline ~FixedLengthString()
	{
		free(Text);
	}
//
// FixedLengthString can be referenced as a PSTR
//
	inline operator PSTR(void) const
	{
		return Text;
	}
//
// Individual chars can be referenced
//
	inline char operator [](size_t Offset) const
	{
		if(Offset <= CurrentSize)
			return *(Text + Offset);
		else
			return 0;
	}
//
// Get Length
//
	inline size_t Length()
	{
		return CurrentSize;
	}
//
// Reset to null
//
	inline void Reset()
	{
		CurrentSize = 0;
		*Text = 0;
	}
//
// Strings can be initialized by other FixedLengthStrings
//
	inline FixedLengthString& operator = (const FixedLengthString&)
	{
		return *this;
	}
//
// Strings can be initialized by PSTR strings
//
	inline FixedLengthString& operator = (PCSTR Source)
	{
		if(Source)
		{
			size_t Length = strlen(Source) + 1;
			if(Length > MaximumLength)
				Length = MaximumLength;
			memcpy(Text, Source, Length);
			CurrentSize = Length - 1;
			*(Text + CurrentSize) = 0;
		}
		return *this;
	}
//
// Strings can be appended with PSTR strings
//
	inline FixedLengthString& operator += (PSTR Source)
	{
		size_t Length = strlen(Source) + 1;
		if(CurrentSize + Length > MaximumLength)
			Length = MaximumLength - CurrentSize;
		memcpy(Text + CurrentSize, Source, Length);
		CurrentSize += Length - 1;
		*(Text + CurrentSize) = 0;
		return *this;
	}
//
// Strings can be appended with PSTR strings
//
	inline FixedLengthString& operator << (PSTR Source)
	{
		if(Source)
		{
			size_t Length = strlen(Source) + 1;
			if(CurrentSize + Length > MaximumLength)
				Length = MaximumLength - CurrentSize;
			memcpy(Text + CurrentSize, Source, Length);
			CurrentSize += Length - 1;
			*(Text + CurrentSize) = 0;
		}
		return *this;
	}
//
// Strings can be appended with integers
//
	inline FixedLengthString& operator << (int32_t Value)
	{
		char Source[30];
		_itoa(Value, Source, 10);
		size_t Length = strlen(Source) + 1;
		if(CurrentSize + Length > MaximumLength)
			Length = MaximumLength - CurrentSize;
		memcpy(Text + CurrentSize, Source, Length);
		CurrentSize += Length - 1;
		*(Text + CurrentSize) = 0;
		return *this;
	}
//
// Strings can be appended with int16_t integers
//
	inline FixedLengthString& operator << (int16_t int32_t Value)
	{
		char Source[30];
		_itoa(Value, Source, 10);
		size_t Length = strlen(Source) + 1;
		if(CurrentSize + Length > MaximumLength)
			Length = MaximumLength - CurrentSize;
		memcpy(Text + CurrentSize, Source, Length);
		CurrentSize += Length - 1;
		*(Text + CurrentSize) = 0;
		return *this;
	}
};





