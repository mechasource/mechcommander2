//---------------------------------------------------------------------------
//
// cevfx.h - This file contains the class declarations for the VFX Shape Element
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef CEVFX_H
#define CEVFX_H
//---------------------------------------------------------------------------
// Include files

#ifndef CELEMENT_H
#include "celement.h"
#endif

#include <float.h>
#include <gameos.hpp>
//---------------------------------------------------------------------------
class VFXElement : public Element
{
	public:
	
		puint8_t		shapeTable;
		int32_t			frameNum;
		int32_t			x,y;
		bool			reverse;
		puint8_t		fadeTable;
		bool			noScaleDraw;
		bool			scaleUp;

	VFXElement (void)
	{
		shapeTable = NULL;
		frameNum = 0;
		x = y = 0;
		reverse = FALSE;
		
		fadeTable = NULL;
		
		noScaleDraw = FALSE;
		scaleUp = FALSE;
	}

	VFXElement (puint8_t _shape, int32_t _x, int32_t _y, int32_t frame, bool rev, puint8_t fTable = NULL, bool noScale = FALSE, bool scaleUp = FALSE);
	VFXElement (puint8_t _shape, float _x, float _y, int32_t frame, bool rev, puint8_t fTable = NULL, bool noScale = FALSE, bool scaleUp = FALSE);

	virtual void draw (void);
};

#define MAX_ELEMENT_SHAPES		4
//---------------------------------------------------------------------------
class VFXShapeElement : public Element
{
	public:
	
		//---------------------------------------
		// This new element class combines all of
		// the shapes into a single Texture for
		// rendering.  Can be used for vehicles
		// and mechs in MechCmdr 2.
		puint8_t		shapeTable[MAX_ELEMENT_SHAPES];
		int32_t			frameNum[MAX_ELEMENT_SHAPES];
		bool			reverse[MAX_ELEMENT_SHAPES];
		int32_t			x,y,xHS,yHS;
		size_t	*fadeTable;
		uint32_t			textureMemoryHandle;
		int32_t			actualHeight;
		float			textureFactor;
		uint32_t			lightRGB;
		uint32_t			fogRGB;
		float			z,topZ;

	VFXShapeElement (void)
	{
		shapeTable[0] = shapeTable[1] = shapeTable[2] = shapeTable[3] = NULL;
		frameNum[0] = frameNum[1] = frameNum[2] = frameNum[3] = 0;

		x = y = 0;

		reverse[0] = reverse[1] = reverse[2] = reverse[3] = FALSE;
		
		fadeTable = NULL;

		textureMemoryHandle = 0xffffffff;

		actualHeight = -1;
		
		lightRGB = 0xffffffff;		//Fully Lit
		fogRGB = 0xffffffff;		//NO Fog
	}

	void init (puint8_t _shape, int32_t _x, int32_t _y, int32_t frame, bool rev, size_t *fTable = NULL, float _z = 0.0, float tZ = 0.0);

	int32_t getTextureHandle (int32_t height = -1);					//Return the block of memory so I store it for this mech/vehicle,etc.
	void setTextureHandle (uint32_t handle, int32_t height = -1);

	void setLight (uint32_t light)
	{
		lightRGB = light;
	}

	void setFog (uint32_t fog)
	{
		fogRGB = fog;
	}

	void drawShape (void);
		
	virtual void draw (void);
};

//---------------------------------------------------------------------------
class TextureElement : public Element
{
	public:
	
		//-----------------------------------------
		// This just draws a textured face.
		// Texture is passed in when inited.
		// For use with Cards.  Not real 3D faces!
		int32_t			x, y, xHS, yHS;
		float			tWidth;
		float			z,topZ;
		uint32_t			textureMemoryHandle;
		uint32_t			lightRGB;
		uint32_t			fogRGB;

	TextureElement (void)
	{
		x = y = 0;
		topZ = z = 0.0;
		textureMemoryHandle = 0xffffffff;
		lightRGB = 0xffffffff;
	}

	void setLight (uint32_t light)
	{
		lightRGB = light;
	}

	void setFog (uint32_t fog)
	{
		fogRGB = fog;
	}
		
	void init (uint32_t textureHandle, int32_t _x, int32_t _y, int32_t hsx, int32_t hsy, float tWidth, float _z, float tZ);
	virtual void draw (void);
};

//---------------------------------------------------------------------------
class PolygonQuadElement : public Element
{
	public:
		
		//--------------------------------
		// This draws any untextured face.
		// Useful for status bars, etc.
		gos_VERTEX		vertices[4];
		
	PolygonQuadElement (void)
	{
	}
	
	void init (gos_VERTEX *v);
	
	virtual void draw (void);

};

//---------------------------------------------------------------------------
class PolygonTriElement : public Element
{
	public:
		
		//--------------------------------
		// This draws any untextured face.
		// Useful for status bars, etc.
		gos_VERTEX		vertices[3];
		
	PolygonTriElement (void)
	{
	}
	
	void init (gos_VERTEX *v);
	
	virtual void draw (void);

};

//---------------------------------------------------------------------------
class TexturedPolygonQuadElement : public PolygonQuadElement
{
	public:
		
		//--------------------------------
		// This draws any untextured face.
		// Useful everywhere
		uint32_t		textureHandle;
		bool		zWrite;
		bool		zComp;
		
	TexturedPolygonQuadElement (void)
	{
	}
	
	void init (gos_VERTEX *v, uint32_t tHandle, bool writeZ = true, bool compZ = true);
	
	virtual void draw (void);

};

//---------------------------------------------------------------------------
class TexturedPolygonTriElement : public PolygonTriElement
{
	public:
		
		//--------------------------------
		// This draws any textured face.
		// Useful everywhere.
		uint32_t		textureHandle;
		
	TexturedPolygonTriElement (void)
	{
	}
	
	void init (gos_VERTEX *v, uint32_t tHandle);
	
	virtual void draw (void);

};

//---------------------------------------------------------------------------
#endif
