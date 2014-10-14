//===========================================================================//
// File:     string.hpp                                                      //
// Title:    Definition of MString class.                                    //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _MSTRING_HPP_
#define _MSTRING_HPP_

#include <stuff/memorystream.hpp>
#include <stuff/iterator.hpp>

namespace Stuff
{
	class MString;
	class MStringRepresentation;
}

#if !defined(Spew)
void Spew(PCSTR group, const Stuff::MStringRepresentation& string);
void Spew(PCSTR group, const Stuff::MString& string);
#endif

namespace MemoryStreamIO
{

	Stuff::MemoryStream& Read(Stuff::MemoryStream* stream, Stuff::MStringRepresentation* str);
	Stuff::MemoryStream& Write(Stuff::MemoryStream* stream, const Stuff::MStringRepresentation& str);
	Stuff::MemoryStream& Read(Stuff::MemoryStream* stream, Stuff::MString* str);
	Stuff::MemoryStream& Write(Stuff::MemoryStream* stream, const Stuff::MString* str);

}

namespace GetHashFunctions
{
	Stuff::IteratorPosition GetHashValue(const Stuff::MString& value);
}

namespace Stuff
{

	class MStringRepresentation;
	class MString;

	class MemoryStream;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ASCII Conversions
//
	void Convert_From_Ascii(PCSTR str, pint8_t value);
	void Convert_From_Ascii(PCSTR str, puint8_t value);
	void Convert_From_Ascii(PCSTR str, pint16_t value);
	void Convert_From_Ascii(PCSTR str, puint16_t value);
	void Convert_From_Ascii(PCSTR str, pint32_t value);
	void Convert_From_Ascii(PCSTR str, puint32_t value);
	void Convert_From_Ascii(PCSTR str, plong32_t value);
	void Convert_From_Ascii(PCSTR str, pulong32_t value);

//##########################################################################
//#####################    MStringRepresentation    ########################
//##########################################################################

	class MStringRepresentation
#if defined(_ARMOR)
		: public Stuff::Signature
#endif
	{
		friend MString;

		friend MString operator + (const MString& str1, const MString& str2);
		friend MString operator + (const MString& str1, char ch);

		friend MemoryStream& MemoryStreamIO::Read(MemoryStream* stream, MString* str);
		friend void Convert_From_Ascii(PCSTR str, MString* value);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Construction, Destruction
		//
	private:
		MStringRepresentation(void);
		MStringRepresentation(const MStringRepresentation& str);
		MStringRepresentation(PCSTR cstr);

	public:
		~MStringRepresentation(void);

		void TestInstance(void) const;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Length, Size
		//
	private:
		//
		// Length returns strlen of string
		//
		size_t GetLength(void) const;
		void SetLength(size_t length);
		void AllocateLength(size_t length);

		//
		// Size returns memory allocation size
		//
		size_t GetSize(void) const;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Accesors & Manipulation
		//
	private:
		//
		// create a c-string from MStringRepresentation method
		// HACK - ECH 11/1/95 - Remove const to support 3rd party libs
		//
		operator PSTR(void) const;

		//
		// assignment method
		//
		MStringRepresentation
		operator = (const MStringRepresentation& str);
		MStringRepresentation
		operator = (PCSTR cstr);

		//
		// concatenation methods
		//
		friend MStringRepresentation
		operator + (
			const MStringRepresentation& str1,
			const MStringRepresentation& str2
		);

		friend MStringRepresentation
		operator + (
			const MStringRepresentation& str1,
			char ch
		);

		void
		operator += (const MStringRepresentation& str);
		void
		operator += (char ch);

		//
		// comparison methods
		//
		int32_t
		Compare(const MStringRepresentation& str) const;

		bool
		operator < (const MStringRepresentation& str) const;
		bool
		operator > (const MStringRepresentation& str) const;
		bool
		operator <= (const MStringRepresentation& str) const;
		bool
		operator >= (const MStringRepresentation& str) const;
		bool
		operator == (const MStringRepresentation& str) const;
		bool
		operator != (const MStringRepresentation& str) const;

		bool
		operator == (PCSTR cstr) const;

		//
		// character retrieval method
		//
		char
		operator [](size_t pos) const;

		MStringRepresentation
		GetNthToken(
			size_t nth_token,
			PSTR delimiters = nullptr
		) const;

		//
		// case-modification methods
		//
		void
		ToUpper(void);
		void
		ToLower(void);

		//
		// stream input/output methods
		//
#if !defined(Spew)
		friend void
		::Spew(
			PCSTR  group,
			const MStringRepresentation& string
		);
#endif

		friend MemoryStream&
		MemoryStreamIO::Read(
			MemoryStream* stream,
			MStringRepresentation* str
		);

		friend MemoryStream&
		MemoryStreamIO::Write(
			MemoryStream* stream,
			const MStringRepresentation& str
		);

		friend void
		Convert_From_Ascii(
			PCSTR str,
			MStringRepresentation* value
		);

		IteratorPosition
		GetHashValue(void);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Reference count methods
		//
	private:
		void
		IncrementReferenceCount(void);

		void
		DecrementReferenceCount(void);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Private Data
		//
	private:
		//
		// class constant
		//
		static size_t
		allocationIncrement;

		//
		// calc alloc size for needed bytes
		//
		static size_t
		CalculateSize(size_t needed);

		//
		// instance variables
		//
		size_t
		stringSize;
		size_t
		stringLength;
		char
		* stringText;

		//
		// reference count
		//
		int32_t
		referenceCount;
	};

//~~~~~~~~~~~~~~~~~~~~~~~~ MStringRepresentation inlines ~~~~~~~~~~~~~~~~~~~

// value return methods
	inline size_t
	MStringRepresentation::GetLength(void) const
	{
		Check_Object(this);
		Verify(
			(stringText != nullptr) ?
			(stringLength == strlen(stringText)) :
			(stringLength == 0)
		);
		return stringLength;
	}

	inline void
	MStringRepresentation::SetLength(size_t length)
	{
		Check_Object(this);
		Verify(length < GetSize());
		stringLength = length;
		Verify(strlen(stringText) == stringLength);
	}

	inline size_t
	MStringRepresentation::GetSize(void) const
	{
		Check_Object(this);
		return stringSize;
	}

// create a c-string from MStringRepresentation method
// HACK - ECH 11/1/95 - Remove const to support 3rd party libs
	inline
	MStringRepresentation::operator PSTR(void) const
	{
		Check_Object(this);
		//		Verify(stringText != nullptr);
		return stringText;
	}

// concatenation methods
	inline void
	MStringRepresentation::operator += (const MStringRepresentation& str)
	{
		Check_Object(this);
		*this = *this + str;
	}

	inline void
	MStringRepresentation::operator += (char ch)
	{
		Check_Object(this);
		*this = *this + ch;
	}

// comparison methods
	inline bool
	MStringRepresentation::operator < (const MStringRepresentation& str) const
	{
		return (Compare(str) < 0);
	}

	inline bool
	MStringRepresentation::operator > (const MStringRepresentation& str) const
	{
		return (Compare(str) > 0);
	}

	inline bool
	MStringRepresentation::operator <= (const MStringRepresentation& str) const
	{
		return !(Compare(str) > 0);
	}

	inline bool
	MStringRepresentation::operator >= (const MStringRepresentation& str) const
	{
		return !(Compare(str) < 0);
	}

	inline bool
	MStringRepresentation::operator == (const MStringRepresentation& str) const
	{
		return (Compare(str) == 0);
	}

	inline bool
	MStringRepresentation::operator != (const MStringRepresentation& str) const
	{
		return (Compare(str) != 0);
	}

	inline bool
	MStringRepresentation::operator == (PCSTR cstr) const
	{
		return (Compare(cstr) == 0);
	}

// character retrieval method
	inline char
	MStringRepresentation::operator [](size_t pos) const
	{
		Check_Object(this);
		return (pos >= stringLength) ? ('\x00') : (stringText[pos]);
	}

// Reference count methods
	inline void
	MStringRepresentation::IncrementReferenceCount()
	{
		Check_Object(this);
		Verify(referenceCount >= 0);
		referenceCount++;
	}

	inline void
	MStringRepresentation::DecrementReferenceCount()
	{
		Check_Object(this);
		Verify(referenceCount > 0);
		if(--referenceCount == 0)
		{
			Unregister_Object(this);
			delete this;
		}
	}

#if USE_ASSEMBLER_CODE
	inline IteratorPosition
	MStringRepresentation::GetHashValue()
	{
		// Verify that the IteratorPosition is 32 bits wide
		// Hash value is first 16 bits of fileID and first 16 bits of recordID
		Verify(sizeof(IteratorPosition) == sizeof(uint32_t));
		static int32_t andAway[3] = {0x000000ff, 0x0000ffff, 0x00ffffff };
		IteratorPosition ret = 0;
		register size_t i, r, len = stringLength >> 2;
		for(i = 0; i < len; i++)
		{
			r = ((size_t*)stringText)[i];
			_asm mov ecx, i
			_asm ror r, cl
			ret += r;
		}
		ret += stringLength & 0x3 ?
			   ((size_t*)stringText)[i] & andAway[stringLength & 0x3 - 1] : 0;
		return (ret & 0x7fffffff);
	}

#endif

//##########################################################################
//############################    MString    ###############################
//##########################################################################

	class MString
#if defined(_ARMOR)
		: public Stuff::Signature
#endif
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Construction, Destruction
		//
	public:
		MString(void);
		MString(const MString& str);
		MString(PCSTR cstr);

		~MString(void);

		void TestInstance(void) const;
		static bool TestClass(void);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Length, Size
		//
	public:
		//
		// Length returns strlen of string
		//
		size_t
		GetLength(void) const;
		void
		SetLength(size_t length);
		void
		AllocateLength(size_t length);

		//
		// Size returns memory allocation size
		//
		size_t
		GetSize(void) const;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Accesors & Manipulation
		//
	public:
		//
		// create a c-string from MString method
		// HACK - ECH 11/1/95 - Remove const to support 3rd party libs
		//
		operator PSTR(void) const;

		//
		// assignment method
		//
		MString&
		operator = (const MString& str);
		MString&
		operator = (PCSTR cstr);

		//
		// concatenation methods
		//
		friend MString
		operator + (
			const MString& str1,
			const MString& str2
		);

		friend MString
		operator + (
			const MString& str1,
			char ch
		);

		friend bool
		Close_Enough(
			PCSTR str1,
			PCSTR str2,
			float e = SMALL
		);

		void
		operator += (const MString& str);
		void
		operator += (char ch);

		//
		// comparison methods
		//
		int32_t
		Compare(const MString& str) const;

		bool
		operator < (const MString& str) const;
		bool
		operator > (const MString& str) const;
		bool
		operator <= (const MString& str) const;
		bool
		operator >= (const MString& str) const;
		bool
		operator == (const MString& str) const;
		bool
		operator != (const MString& str) const;

		bool
		operator == (PCSTR cstr) const;

		//
		// character retrieval method
		//
		char
		operator [](size_t pos) const;

		MString
		GetNthToken(
			size_t nth_token,
			PSTR delimiters = nullptr
		) const;

		//
		// case-modification methods
		//
		void
		ToUpper(void);
		void
		ToLower(void);


		//
		// stream input/output methods
		//
#if !defined(Spew)
		friend void
		::Spew(
			PCSTR  group,
			const MString& string
		);
#endif

		friend MemoryStream&
		MemoryStreamIO::Read(
			MemoryStream* stream,
			MString* str
		);

		friend MemoryStream&
		MemoryStreamIO::Write(
			MemoryStream* stream,
			const MString* str
		);

		friend void
		Convert_From_Ascii(
			PCSTR str,
			MString* value
		);

		IteratorPosition
		GetHashValue(void) const
		{
			return representation->GetHashValue();
		}

		friend IteratorPosition
		GetHashFunctions::GetHashValue(const MString& value);


		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Private Data
		//
	private:
		MStringRepresentation
		* representation;
	};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~ MString inlines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// construction, destruction, testing
	inline
	MString::MString()
	{
		representation = new MStringRepresentation;
		Register_Object(representation);
		representation->IncrementReferenceCount();
		Verify(representation->referenceCount == 1);
	}

	inline
	MString::MString(const MString& str)
	{
		Check_Object(&str);
		representation = str.representation;
		Check_Object(representation);
		representation->IncrementReferenceCount();
	}

	inline
	MString::MString(PCSTR cstr)
	{
		representation = new MStringRepresentation(cstr);
		Register_Object(representation);
		representation->IncrementReferenceCount();
		Verify(representation->referenceCount == 1);
	}

	inline
	MString::~MString()
	{
		Check_Object(representation);
		representation->DecrementReferenceCount();
	}

	inline void
	MString::TestInstance(void) const
	{
		Check_Object(representation);
	}

// length, size
	inline size_t
	MString::GetLength(void) const
	{
		Check_Object(representation);
		return representation->GetLength();
	}

	inline void
	MString::SetLength(size_t length)
	{
		Check_Object(representation);
		representation->SetLength(length);
	}

	inline void
	MString::AllocateLength(size_t length)
	{
		Check_Object(representation);
		representation->AllocateLength(length);
	}

	inline size_t
	MString::GetSize(void) const
	{
		Check_Object(representation);
		return representation->GetSize();
	}

// create a c-string from MString method
// HACK - ECH 11/1/95 - Remove const to support 3rd party libs
	inline
	MString::operator PSTR(void) const
	{
		Check_Object(representation);
		//		Verify(representation->stringText != nullptr);
		return representation->stringText;
	}

// concatenation methods
	inline MString
	operator + (
		const MString& str1,
		const MString& str2
	)
	{
		Check_Object(&str1);
		Check_Object(&str2);
		MStringRepresentation temp = *str1.representation + *str2.representation;
		return MString(temp.stringText);
	}

	inline MString
	operator + (
		const MString& str1,
		char ch
	)
	{
		Check_Object(&str1);
		MStringRepresentation temp = *str1.representation + ch;
		return MString(temp.stringText);
	}

	inline void
	MString::operator += (const MString& str)
	{
		Check_Object(this);
		Check_Object(&str);
		*this = *this + str;
	}

	inline void
	MString::operator += (char ch)
	{
		Check_Object(this);
		*this = *this + ch;
	}

// comparison methods
	inline int32_t
	MString::Compare(const MString& str) const
	{
		Check_Object(&str);
		Check_Object(representation);
		return representation->Compare(*str.representation);
	}

	inline bool
	MString::operator < (const MString& str) const
	{
		return (Compare(str) < 0);
	}

	inline bool
	MString::operator > (const MString& str) const
	{
		return (Compare(str) > 0);
	}

	inline bool
	MString::operator <= (const MString& str) const
	{
		return !(Compare(str) > 0);
	}

	inline bool
	MString::operator >= (const MString& str) const
	{
		return !(Compare(str) < 0);
	}

	inline bool
	MString::operator == (const MString& str) const
	{
		return (Compare(str) == 0);
	}

	inline bool
	MString::operator != (const MString& str) const
	{
		return (Compare(str) != 0);
	}

	inline bool
	MString::operator == (PCSTR cstr) const
	{
		return (Compare(cstr) == 0);
	}

// character retrieval method
	inline char
	MString::operator[](size_t pos) const
	{
		Check_Object(representation);
		return (*representation)[pos];
	}

	inline MString
	MString::GetNthToken(
		size_t nth_token,
		PSTR delimiters
	) const
	{
		Check_Object(representation);
		MStringRepresentation temp =
			representation->GetNthToken(nth_token, delimiters);
		return MString(temp.stringText);
	}

}

// stream input/output methods
#if !defined(Spew)
inline void
Spew(
	PCSTR  group,
	const Stuff::MStringRepresentation& string
)
{
	Check_Object(&string);
	SPEW((group, string.stringText));
}

inline void
Spew(
	PCSTR  group,
	const Stuff::MString& string
)
{
	Check_Object(&string);
	Spew(group, *string.representation);
}
#endif

namespace MemoryStreamIO
{

	inline Stuff::MemoryStream&
	Write(
		Stuff::MemoryStream* stream,
		const Stuff::MString* str
	)
	{
		return Write(stream, *str->representation);
	}

}
#endif
