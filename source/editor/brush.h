//----------------------------------------------------------------------------
//
// Brush.h - header file for the abstract base class that all brush objects should
//			 derive from
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef BRUSH_H_
#define BRUSH_H_

class Action;
class Map;

#include "mclib.h"

class Brush
{

public:
	Brush(){}
	virtual ~Brush(){}
	virtual bool beginPaint()  = 0;
	virtual Action* endPaint() = 0;
	virtual bool paint( Stuff::Vector3D& worldPos, int screenX, int screenY  )  = 0;
	virtual bool canPaint( Stuff::Vector3D& worldPos, int screenX, int screenY, int flags ) { return true; } 
	virtual bool canPaintSelection( ){ return true; }
	virtual Action* applyToSelection(){ return NULL;}

	virtual void render( int screenX, int screenY ){}
	virtual void update( int screenX, int screenY ){}

private:


};



#endif // BRUSH_H_