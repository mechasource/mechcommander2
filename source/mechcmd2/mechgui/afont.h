//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef AFONT_H
#define AFONT_H

//#include <gameos.hpp>

namespace mechgui {

// Error codes, local to this file...
typedef enum __afont_constants {
	FONT_NOT_LOADED			= -3,
};

class aFont
{
public:
	aFont();
	~aFont();
	aFont( const aFont& src );
	aFont& operator=( const aFont& src );

	int32_t init( PCSTR fontName);
	int32_t init( int32_t resourceID );
	void destroy();
	void render( PCSTR text, int XPos, int YPos, int areaWidth, 
		int areaHeight, ULONG color, bool bBold, int alignment );

	int32_t load( PCSTR fontName);
	ULONG height() const;
	ULONG width( PCSTR st) const;
	ULONG height( PCSTR st, int areaWidth ) const;
	void		  getSize( ULONG& width, ULONG& height, PCSTR pText );

	static HGOSFONT3D loadFont( int32_t resourceID, int32_t& size );
	int32_t	getSize() { return size; }

	HGOSFONT3D getTempHandle() { return gosFont; } // don't you dare hang on to this
	int			getFontID() { return resID; }
private:
	HGOSFONT3D		gosFont;
	int				resID;
	char			fontName[64]; // so we can copy fonts
	int32_t			size;
	void copyData( const aFont& src );

};

#endif
