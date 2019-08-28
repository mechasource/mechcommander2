//===========================================================================//
// File: color.hpp //
// Title: Declaration of colour classes. //
// Purpose: Stores color information. //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved. //
//===========================================================================//

#pragma once

#ifndef _COLOR_HPP_
#define _COLOR_HPP_

// #include "stuff/memorystream.h"

namespace Stuff
{
class RGBcolour;
class RGBAcolour;
class HSVcolour;
class HSVAcolour;
} // namespace Stuff

#if !defined(Spew)
void
Spew(const std::wstring_view& group, const Stuff::RGBcolour& color);
void
Spew(const std::wstring_view& group, const Stuff::RGBAcolour& color);
void
Spew(const std::wstring_view& group, const Stuff::HSVcolour& color);
void
Spew(const std::wstring_view& group, const Stuff::HSVAcolour& color);
#endif

namespace Stuff
{

//##########################################################################
//############## RGBcolour ############################################
//##########################################################################

class RGBcolour
{
	//	friend class RGBAcolour;

public:
	RGBcolour(void) { red = green = blue = -1.0f; }

	RGBcolour(float r, float g, float b)
	{
		red = r;
		green = g;
		blue = b;
	}

	friend bool Close_Enough(const RGBcolour& c1, const RGBcolour& c2, float e = SMALL);
	bool operator==(const RGBcolour& color) const
	{
		// Check_Object(this);
		return Close_Enough(*this, color, SMALL);
	}
	bool operator!=(const RGBcolour& color) const
	{
		// Check_Object(this);
		return !Close_Enough(*this, color, SMALL);
	}
	RGBcolour& operator=(const RGBcolour& color)
	{
		// Check_Object(this);
		Check_Object(&color);
		red = color.red;
		green = color.green;
		blue = color.blue;
		return *this;
	}
	RGBcolour& operator=(const HSVcolour& color);

	void TestInstance(void) const {}

	float Infrared(void) const
	{
		// Check_Object(this);
		return 0.3f * red + 0.5f * green + 0.2f * blue;
	}

	RGBcolour& Combine(const RGBcolour& c1, float t1, const RGBcolour& c2, float t2)
	{
		// Check_Pointer(this);
		Check_Object(&c1);
		Check_Object(&c2);
		red = c1.red * t1 + c2.red * t2;
		green = c1.green * t1 + c2.green * t2;
		blue = c1.blue * t1 + c2.blue * t2;
		return *this;
	}

	RGBcolour& Lerp(const RGBcolour& c1, const RGBcolour& c2, float t)
	{
		// Check_Pointer(this);
		Check_Object(&c1);
		Check_Object(&c2);
		red = c1.red + t * (c2.red - c1.red);
		green = c1.green + t * (c2.green - c1.green);
		blue = c1.blue + t * (c2.blue - c1.blue);
		return *this;
	}

	//
	// Support functions
	//
#if !defined(Spew)
	friend void ::Spew(const std::wstring_view& group, const RGBcolour& color);
#endif

protected:
	// static const RGBcolour Unassigned;
	float red, green, blue;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~ RGBcolour functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void
Convert_From_Ascii(const std::wstring_view& str, RGBcolour* color);

//##########################################################################
//############## RGBAcolour ###########################################
//##########################################################################

class RGBAcolour : public RGBcolour
{
public:
	RGBAcolour(void) :
		RGBcolour() { alpha = -1.0f; }
	RGBAcolour(float r, float g, float b, float a) :
		RGBcolour(r, g, b) { alpha = a; }

	friend bool Close_Enough(const RGBAcolour& c1, const RGBAcolour& c2, float e = SMALL);
	bool operator==(const RGBAcolour& color) const { return Close_Enough(*this, color, SMALL); }
	bool operator!=(const RGBAcolour& color) const { return !Close_Enough(*this, color, SMALL); }

	RGBAcolour& operator=(const RGBAcolour& color)
	{
		// Check_Object(this);
		Check_Object(&color);
		red = color.red;
		green = color.green;
		blue = color.blue;
		alpha = color.alpha;
		return *this;
	}
	RGBAcolour& operator=(const HSVAcolour& color);

	RGBAcolour& Combine(const RGBAcolour& c1, float t1, const RGBAcolour& c2, float t2)
	{
		// Check_Pointer(this);
		Check_Object(&c1);
		Check_Object(&c2);
		red = c1.red * t1 + c2.red * t2;
		green = c1.green * t1 + c2.green * t2;
		blue = c1.blue * t1 + c2.blue * t2;
		alpha = c1.alpha * t1 + c2.alpha * t2;
		return *this;
	}

	RGBAcolour& Lerp(const RGBAcolour& c1, const RGBAcolour& c2, float t)
	{
		// Check_Pointer(this);
		Check_Object(&c1);
		Check_Object(&c2);
		red = c1.red + t * (c2.red - c1.red);
		green = c1.green + t * (c2.green - c1.green);
		blue = c1.blue + t * (c2.blue - c1.blue);
		alpha = c1.alpha + t * (c2.alpha - c1.alpha);
		return *this;
	}

	float GetAlpha(void) const
	{
		return alpha;
	}

	//
	// Support functions
	//
#if !defined(Spew)
	friend void ::Spew(const std::wstring_view& group, const RGBAcolour& color);
#endif

protected:
	//static const RGBAcolour Unassigned;
	float alpha;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~ RGBAcolour functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void
Convert_From_Ascii(const std::wstring_view& str, RGBAcolour* color);

//##########################################################################
//############## HSVcolour ############################################
//##########################################################################

class HSVcolour
{
	friend class HSVAcolour;

public:
	HSVcolour(void) { hue = saturation = value = -1.0f; }

	HSVcolour(float h, float s, float v)
	{
		hue = h;
		saturation = s;
		value = v;
	}

	friend bool Close_Enough(const HSVcolour& c1, const HSVcolour& c2, float e = SMALL);
	bool operator==(const HSVcolour& color) const
	{
		// Check_Object(this);
		return Close_Enough(*this, color, SMALL);
	}
	bool operator!=(const HSVcolour& color) const
	{
		// Check_Object(this);
		return !Close_Enough(*this, color, SMALL);
	}

	HSVcolour& operator=(const RGBcolour& color);
	HSVcolour& operator=(const HSVcolour& color)
	{
		// Check_Object(this);
		Check_Object(&color);
		hue = color.hue;
		saturation = color.saturation;
		value = color.value;
		return *this;
	}

	void TestInstance(void) const {}

	HSVcolour& Combine(const HSVcolour& c1, float t1, const HSVcolour& c2, float t2)
	{
		// Check_Pointer(this);
		Check_Object(&c1);
		Check_Object(&c2);
		hue = c1.hue * t1 + c2.hue * t2;
		saturation = c1.saturation * t1 + c2.saturation * t2;
		value = c1.value * t1 + c2.value * t2;
		return *this;
	}

	HSVcolour& Lerp(const HSVcolour& c1, const HSVcolour& c2, float t)
	{
		// Check_Pointer(this);
		Check_Object(&c1);
		Check_Object(&c2);
		hue = c1.hue + t * (c2.hue - c1.hue);
		saturation = c1.saturation + t * (c2.saturation - c1.saturation);
		value = c1.value + t * (c2.value - c1.value);
		return *this;
	}

	//
	// Support functions
	//
#if !defined(Spew)
	friend void ::Spew(const std::wstring_view& group, const HSVcolour& color);
#endif

protected:
	//static const HSVcolour Unassigned;
	float hue, saturation, value;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~ HSVcolour functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void
Convert_From_Ascii(const std::wstring_view& str, HSVcolour* color);

//##########################################################################
//############## HSVAcolour ###########################################
//##########################################################################

class HSVAcolour : public HSVcolour
{
public:
	static const HSVAcolour Unassigned;

	float alpha;

	HSVAcolour(void) :
		HSVcolour() { alpha = -1.0f; }
	HSVAcolour(float h, float s, float v, float a) :
		HSVcolour(h, s, v) { alpha = a; }

	friend bool Close_Enough(const HSVAcolour& c1, const HSVAcolour& c2, float e = SMALL);
	bool operator==(const HSVAcolour& color) const { return Close_Enough(*this, color, SMALL); }
	bool operator!=(const HSVAcolour& color) const { return !Close_Enough(*this, color, SMALL); }

	HSVAcolour& operator=(const RGBAcolour& color)
	{
		// Check_Object(this);
		Check_Object(&color);
		HSVcolour::operator=(color);
		alpha = color.GetAlpha();
		return *this;
	}
	HSVAcolour& operator=(const HSVAcolour& color)
	{
		// Check_Object(this);
		Check_Object(&color);
		hue = color.hue;
		saturation = color.saturation;
		value = color.value;
		alpha = color.alpha;
		return *this;
	}

	HSVAcolour& Combine(const HSVAcolour& c1, float t1, const HSVAcolour& c2, float t2)
	{
		// Check_Pointer(this);
		Check_Object(&c1);
		Check_Object(&c2);
		hue = c1.hue * t1 + c2.hue * t2;
		saturation = c1.saturation * t1 + c2.saturation * t2;
		value = c1.value * t1 + c2.value * t2;
		alpha = c1.alpha * t1 + c2.alpha * t2;
		return *this;
	}

	HSVAcolour& Lerp(const HSVAcolour& c1, const HSVAcolour& c2, float t)
	{
		// Check_Pointer(this);
		Check_Object(&c1);
		Check_Object(&c2);
		hue = c1.hue + t * (c2.hue - c1.hue);
		saturation = c1.saturation + t * (c2.saturation - c1.saturation);
		value = c1.value + t * (c2.value - c1.value);
		alpha = c1.alpha + t * (c2.alpha - c1.alpha);
		return *this;
	}

	//
	// Support functions
	//
#if !defined(Spew)
	friend void ::Spew(const std::wstring_view& group, const HSVAcolour& color);
#endif
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~ HSVAcolour functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void
Convert_From_Ascii(const std::wstring_view& str, HSVAcolour* color);

inline RGBAcolour&
RGBAcolour::operator=(const HSVAcolour& color)
{
	// Check_Object(this);
	Check_Object(&color);
	RGBcolour::operator=(color);
	alpha = color.alpha;
	return *this;
}
} // namespace Stuff

namespace MemoryStreamIO
{
#if CONSIDERED_DISABLED
inline std::istream&
Read(std::istream& stream, Stuff::RGBcolour* output)
{
	return stream.read(output, sizeof(*output));
}
inline std::ostream&
Write(std::ostream& stream, const Stuff::RGBcolour* input)
{
	return stream.write(input, sizeof(*input));
}

inline std::istream&
Read(std::istream& stream, Stuff::RGBAcolour* output)
{
	return stream.read(output, sizeof(*output));
}
inline std::ostream&
Write(std::ostream& stream, const Stuff::RGBAcolour* input)
{
	return stream.write(input, sizeof(*input));
}

inline std::istream&
Read(std::istream& stream, Stuff::HSVcolour* output)
{
	return stream.read(output, sizeof(*output));
}
inline std::ostream&
Write(std::ostream& stream, const Stuff::HSVcolour* input)
{
	return stream.write(input, sizeof(*input));
}

inline std::istream&
Read(std::istream& stream, Stuff::HSVAcolour* output)
{
	return stream.read(output, sizeof(*output));
}
inline std::ostream&
Write(std::ostream& stream, const Stuff::HSVAcolour* input)
{
	return stream.write(input, sizeof(*input));
}
#endif
} // namespace MemoryStreamIO
#endif
