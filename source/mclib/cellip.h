//---------------------------------------------------------------------------
//
// cellip.h - This file contains the class declarations for the ellipse element
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef CELLIP_H
#define CELLIP_H
//---------------------------------------------------------------------------
// Include files

#ifndef CELEMENT_H
#include "celement.h"
#endif

#ifndef VFX_H
#include "vfx.h"
#endif

#include "utilities.h"

#include "stuff/stuff.h"
//---------------------------------------------------------------------------
class EllipseElement : public Element
{
public:
	gos_VERTEX location[5];
	RECT clip;

	EllipseElement(void)
	{
		memset(location, 0, sizeof(location));
		memset(&clip, 0, sizeof(clip));
	}

	EllipseElement(Stuff::Vector2DOf<int32_t>& cnt, Stuff::Vector2DOf<int32_t>& ortho, int32_t clr,
		int32_t depth);

	virtual void draw(void);

	static void init(void); // gotta call this one time before you can draw

	void setClip(const RECT&);

	static void removeTextureHandle(void);

private:
	static size_t s_textureHandle;
};

//---------------------------------------------------------------------------
#endif
