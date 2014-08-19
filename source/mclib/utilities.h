//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// stuff I need all over the place
#ifndef UTILITIES_H
#define UTILITIES_H

#include <gameos.hpp>
#include <string.h>


class FitIniFile;


class StaticInfo
{
public:

	void init( FitIniFile& file, PSTR blockName, long hiResOffsetX = 0, long hiResOffsetY = 0, DWORD neverFlush = 0 );
	void render();
	bool isInside( int mouseX, int mouseY );

	void setLocation( float  newX, float newY );
	void move( float deltaX, float deltaY );

	void setNewUVs( float uLeft, float vTop, float uRight, float vBottom );

	float width(){ return location[2].x - location[0].x; }
	float height(){ return location[2].y - location[0].y; }
	
	void getData(PUCHAR  buffer);

	void showGUIWindow( bool bShow );

	void setColor( long newColor );

	StaticInfo(){}
	~StaticInfo();

	ULONG textureHandle;
	gos_VERTEX	location[4];
	long u, v, uWidth, vHeight;

	ULONG textureWidth; // textures are square
};

#if _CONSIDERED_OBSOLETE
typedef struct _GUI_RECTd
{
	long left;
	long top;
	long right;
	long bottom;
} RECT;
#endif

void drawEmptyRect( const RECT& rect, ULONG leftBorderColor = 0xffffffff,
	 ULONG rightBorderColor = 0xff000000 );


void drawRect( const RECT& rect, ULONG color );

void drawShadowText( long colorTop, long colorShadow, HGOSFONT3D font, 
					long left, long top, bool proportional, PCSTR text, bool bBold, float scale );

void drawShadowText( long colorTop, long colorShadow, HGOSFONT3D font, 
					long left, long top, bool proportional, PCSTR text, bool bold, float scale,
					long xOffset, long yOffset);

void drawShadowText( long colorTop, long colorShadow, HGOSFONT3D font, 
					long left, long top, long right, long bottom, bool proportional, PCSTR text, bool bold, float scale,
					long xOffset, long yOffset);



long interpolateColor( long color1, long color2, float percent );

inline COLORREF reverseRGB(COLORREF oldVal) 
{
	return ( (oldVal & 0xff000000) | ((oldVal & 0xff0000) >> 16) | (oldVal & 0xff00) | ((oldVal & 0xff) << 16) );
}


//-------------------------------------------------------
// Replaces the WINDOWS version because it is a lie!
// There are cases where windows does NOT append a null.
// THANKS!
//
// Replace with GOS String Resource get when available
// Replaced.  Andy wants us to call everytime.  Will try and see if practical.
extern DWORD gosResourceHandle;




#if 1
inline int cLoadString(
  uint32_t uID,             // resource identifier
  PSTR lpBuffer,      // pointer to buffer for resource
  size_t nBufferMax,        // size of buffer
  ULONG handle = gosResourceHandle
  )
{
	memset(lpBuffer,0,nBufferMax);
	PSTR  tmpBuffer = gos_GetResourceString(handle, uID);
	size_t stringLength = strlen(tmpBuffer);
	if (stringLength >= nBufferMax)
		STOP(("String too long for buffer.  String Id %d, bufferLen %d, StringLen %d",uID,nBufferMax,stringLength));
	memcpy(lpBuffer,tmpBuffer,stringLength);
	return stringLength;
}

#else

inline PSTR  cLoadString (uint32_t uID)
{
	return gos_GetResourceString(gosResourceHandle, uID);
}
#endif

#endif