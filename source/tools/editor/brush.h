//----------------------------------------------------------------------------
//
// Brush.h - header file for the abstract base class that all brush objects should
//			 derive from
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef BRUSH_H_
#define BRUSH_H_

// #include <mclib.h>

class Action;
class Map;

class Brush
{

public:
	Brush(){}
	virtual ~Brush(){}
	virtual bool beginPaint()  = 0;
	virtual Action* endPaint() = 0;
	virtual bool paint( Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY  )  = 0;
	virtual bool canPaint( Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags ) { return true; } 
	virtual bool canPaintSelection( ){ return true; }
	virtual Action* applyToSelection(){ return NULL;}

	virtual void render( int32_t screenX, int32_t screenY ){}
	virtual void update( int32_t screenX, int32_t screenY ){}

private:


};
#endif // BRUSH_H_
