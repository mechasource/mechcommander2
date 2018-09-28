//---------------------------------------------------------------------------
//
// celine.h - This file contains the class declarations for the line element
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef CELINE_H
#define CELINE_H
//---------------------------------------------------------------------------
// Include files

#ifndef CELEMENT_H
#include "celement.h"
#endif

#ifndef VFX_H
#include "vfx.h"
#endif

#include <stuff/stuff.hpp>
//---------------------------------------------------------------------------
class LineElement : public Element
{
public:
	Stuff::Vector4D startPos;
	Stuff::Vector4D endPos;
	int32_t color;
	int32_t endColor;
	puint8_t fadeTable;

	LineElement(void) : Element()
	{
		startPos.x = startPos.y = startPos.z = startPos.w = 0.0f;
		endPos.x = endPos.y = endPos.z = endPos.w = 0.0f;
		color									  = 0;
		endColor								  = -1;
		fadeTable								  = nullptr;
	}

	LineElement(Stuff::Vector4D& pos1, Stuff::Vector4D& pos2, int32_t clr, puint8_t fTable,
		int32_t endClr = -1);

	virtual void draw(void);
};

//---------------------------------------------------------------------------
#endif
