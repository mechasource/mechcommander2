//===========================================================================//
// File:     color.cpp                                                       //
// Title:    Definition of colour classes.                                    //
// Contents: Implementation details for color classes                        //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
//#include "stuff/stuffheaders.h"

////#include "gameos.hpp"
#include "stuff/scalar.h"
// #include "stuff/mstring.h"
#include "stuff/color.h"

// using namespace Stuff;

//~~~~~~~~~~~~~~~~~~~~~~~~~~ RGBcolour functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
const RGBcolour RGBcolour::Unassigned(-1.0f, -1.0f, -1.0f);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// (friend)
bool
Stuff::Close_Enough(const RGBcolour& c1, const RGBcolour& c2,
	float e // = SMALL
)
{
	Check_Object(&c1);
	Check_Object(&c2);
	return Close_Enough(c1.red, c2.red, e) && Close_Enough(c1.green, c2.green, e) && Close_Enough(c1.blue, c2.blue, e);
}

//
//###########################################################################
//###########################################################################
//
RGBcolour&
RGBcolour::operator=(const HSVcolour& color)
{
	// Check_Object(this);
	Check_Object(&color);
	_ASSERT(color.saturation >= 0.0 && color.saturation <= 1.0f);
	//
	//----------------
	// Check for black
	//----------------
	//
	if (color.saturation <= SMALL)
	{
		red = green = blue = 0.0f;
		return *this;
	}
	//
	//-----------------------------
	// find the sextant for the hue
	//-----------------------------
	//
	_ASSERT(color.hue >= 0.0 && color.hue <= 1.0f);
	float hue = (color.hue == 1.0f) ? 0.0f : color.hue;
	hue *= 6.0f;
	int32_t sextant = Truncate_Float_To_Byte(hue);
	_ASSERT(static_cast<uint32_t>(sextant) < 6);
	float remainder = hue - static_cast<float>(sextant);
	//
	//--------------------
	// Build the RGB color
	//--------------------
	//
	_ASSERT(color.value >= 0.0f && color.value <= 1.0f);
	float a = color.value * (1.0f - color.saturation);
	_ASSERT(a >= 0.0f && a < 1.0f);
	switch (sextant)
	{
	case 0:
		red = color.value;
		green = color.value * (1.0f - color.saturation * (1.0f - remainder));
		_ASSERT(green >= 0.0f && green <= 1.0f);
		blue = a;
		break;
	case 1:
		red = color.value * (1.0f - color.saturation * remainder);
		_ASSERT(red >= 0.0f && red <= 1.0f);
		green = color.value;
		blue = a;
		break;
	case 2:
		red = a;
		green = color.value;
		blue = color.value * (1.0f - color.saturation * (1.0f - remainder));
		_ASSERT(blue >= 0.0f && blue <= 1.0f);
		break;
	case 3:
		red = a;
		green = color.value * (1.0f - color.saturation * remainder);
		_ASSERT(green >= 0.0f && green <= 1.0f);
		blue = color.value;
		break;
	case 4:
		red = color.value * (1.0f - color.saturation * (1.0f - remainder));
		_ASSERT(red >= 0.0f && red <= 1.0f);
		green = a;
		blue = color.value;
		break;
	case 5:
		red = color.value;
		green = a;
		blue = color.value * (1.0f - color.saturation * remainder);
		_ASSERT(blue >= 0.0f && blue <= 1.0f);
		break;
	}
	return *this;
}

//
//###########################################################################
//###########################################################################
//
void
Stuff::Convert_From_Ascii(const std::wstring_view& str, RGBcolour* color)
{
	Check_Pointer(str);
	Check_Object(color);
	const std::wstring_view& parse_string(str);
	const std::wstring_view& token = parse_string.GetNthToken(0);
	Check_Pointer(token);
	color->red = AtoF(token);
	token = parse_string.GetNthToken(1);
	Check_Pointer(token);
	color->green = AtoF(token);
	token = parse_string.GetNthToken(2);
	Check_Pointer(token);
	color->blue = AtoF(token);
	Check_Object(color);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~ RGBAcolour functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const RGBAcolour RGBAcolour::Unassigned(-1.0f, -1.0f, -1.0f, -1.0f);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// (friend)
bool
Stuff::Close_Enough(const RGBAcolour& c1, const RGBAcolour& c2,
	float e // = SMALL
)
{
	Check_Object(&c1);
	Check_Object(&c2);
	return Close_Enough(c1.red, c2.red, e) && Close_Enough(c1.green, c2.green, e) && Close_Enough(c1.blue, c2.blue, e) && Close_Enough(c1.alpha, c2.alpha, e);
}

//
//###########################################################################
//###########################################################################
//
void
Stuff::Convert_From_Ascii(const std::wstring_view& str, RGBAcolour* color)
{
	Check_Pointer(str);
	Check_Object(color);
	const std::wstring_view& parse_string(str);
	const std::wstring_view& token = parse_string.GetNthToken(0);
	Check_Pointer(token);
	color->red = AtoF(token);
	token = parse_string.GetNthToken(1);
	Check_Pointer(token);
	color->green = AtoF(token);
	token = parse_string.GetNthToken(2);
	Check_Pointer(token);
	color->blue = AtoF(token);
	token = parse_string.GetNthToken(3);
	Check_Pointer(token);
	color->alpha = AtoF(token);
	Check_Object(color);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~ HSVcolour functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
const HSVcolour HSVcolour::Unassigned(-1.0f, -1.0f, -1.0f);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// (friend)
bool
Stuff::Close_Enough(const HSVcolour& c1, const HSVcolour& c2,
	float e // = SMALL
)
{
	Check_Object(&c1);
	Check_Object(&c2);
	return Close_Enough(c1.hue, c2.hue, e) && Close_Enough(c1.saturation, c2.saturation, e) && Close_Enough(c1.value, c2.value, e);
}

//
//###########################################################################
//###########################################################################
//
HSVcolour&
HSVcolour::operator=(const RGBcolour& color)
{
	// Check_Object(this);
	Check_Object(&color);
	_ASSERT(color.red >= 0.0f && color.red <= 1.0f);
	_ASSERT(color.green >= 0.0f && color.green <= 1.0f);
	_ASSERT(color.blue >= 0.0f && color.blue <= 1.0f);
	//
	//--------------------
	// Set the color value
	//--------------------
	//
	value = Max(color.red, Max(color.green, color.blue));
	//
	//-------------------------
	// Set the saturation value
	//-------------------------
	//
	float delta = value - Min(color.red, Min(color.green, color.blue));
	if (value > SMALL)
	{
		saturation = delta / value;
		_ASSERT(saturation > 0.0f && saturation <= 1.0f);
	}
	else
	{
		saturation = 0.0f;
	}
	//
	//------------
	// Set the hue
	//------------
	//
	if (saturation <= SMALL)
	{
		hue = 0.0f;
	}
	else
	{
		_ASSERT(delta > SMALL);
		if (color.red == value)
		{
			hue = (color.green - color.blue) / delta;
		}
		else if (color.green == value)
		{
			hue = 2.0f + (color.blue - color.red) / delta;
		}
		else
		{
			hue = 4.0f + (color.red - color.green) / delta;
		}
		if (hue < 0.0f)
		{
			hue += 6.0f;
		}
		hue *= 1.0f / 6.0f;
		_ASSERT(hue >= 0.0f && hue <= 1.0f);
	}
	return *this;
}

//
//###########################################################################
//###########################################################################
//
void
Stuff::Convert_From_Ascii(const std::wstring_view& str, HSVcolour* color)
{
	Check_Pointer(str);
	Check_Object(color);
	const std::wstring_view& parse_string(str);
	const std::wstring_view& token = parse_string.GetNthToken(0);
	Check_Pointer(token);
	color->hue = AtoF(token);
	token = parse_string.GetNthToken(1);
	Check_Pointer(token);
	color->saturation = AtoF(token);
	token = parse_string.GetNthToken(2);
	Check_Pointer(token);
	color->value = AtoF(token);
	Check_Object(color);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~ HSVAcolour functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const HSVAcolour HSVAcolour::Unassigned(-1.0f, -1.0f, -1.0f, -1.0f);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// (friend)
bool
Stuff::Close_Enough(const HSVAcolour& c1, const HSVAcolour& c2,
	float e // = SMALL
)
{
	Check_Object(&c1);
	Check_Object(&c2);
	return Close_Enough(c1.hue, c2.hue, e) && Close_Enough(c1.saturation, c2.saturation, e) && Close_Enough(c1.value, c2.value, e) && Close_Enough(c1.alpha, c2.alpha, e);
}

//
//###########################################################################
//###########################################################################
//
void
Stuff::Convert_From_Ascii(const std::wstring_view& str, HSVAcolour* color)
{
	Check_Pointer(str);
	Check_Object(color);
	const std::wstring_view& parse_string(str);
	const std::wstring_view& token = parse_string.GetNthToken(0);
	Check_Pointer(token);
	color->hue = AtoF(token);
	token = parse_string.GetNthToken(1);
	Check_Pointer(token);
	color->saturation = AtoF(token);
	token = parse_string.GetNthToken(2);
	Check_Pointer(token);
	color->value = AtoF(token);
	token = parse_string.GetNthToken(3);
	Check_Pointer(token);
	color->alpha = AtoF(token);
	Check_Object(color);
}
