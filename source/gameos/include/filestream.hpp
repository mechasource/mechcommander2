//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
// FileStream.h: interface for the FileStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(FILESTREAM_HPP)
#define FILESTREAM_HPP

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/*
//#pragma warning(push,3)
#include <fstream.h>
//#pragma warning(pop)

//#ifdef _ARMOR
//#if 0
//#define DebugFileStream ofstream
//
//#else

class DebugFileStream
{
public:
	DebugFileStream(PSTR )
	{}

	DebugFileStream()
	{}

	inline  DebugFileStream& operator<<(ostream& (__cdecl * _f)(ostream&)){return *this;}
	inline  DebugFileStream& operator<<(ios& (__cdecl * _f)(ios&)){return *this;}
	inline  DebugFileStream& operator<<(PCSTR ){return *this;}
	inline  DebugFileStream& operator<<(pcuint8_t){return *this;}
	inline  DebugFileStream& operator<<(const signed PSTR ){return *this;}
	inline  DebugFileStream& operator<<(char){return *this;}
	inline  DebugFileStream& operator<<(uint8_t){return *this;}
	inline  DebugFileStream& operator<<(signed char){return *this;}
	inline  DebugFileStream& operator<<(short){return *this;}
	inline  DebugFileStream& operator<<(uint16_t){return *this;}
	inline  DebugFileStream& operator<<(int){return *this;}
	inline  DebugFileStream& operator<<(uint32_t){return *this;}
	inline  DebugFileStream& operator<<(int32_t){return *this;}
	inline  DebugFileStream& operator<<(ULONG){return *this;}
	inline  DebugFileStream& operator<<(float){return *this;}
	inline	DebugFileStream& operator<<(double){return *this;}
	inline  DebugFileStream& operator<<(int32_t double){return *this;}
	inline  DebugFileStream& operator<<(PCVOID ){return *this;}
	inline  DebugFileStream& operator<<(streambuf*){return *this;}

};

//#endif

*/

#endif

