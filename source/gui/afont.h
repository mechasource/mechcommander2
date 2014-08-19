//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef AFONT_H
#define AFONT_H

//#include <gameos.hpp>

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

	long init( PCSTR fontName);
	long init( long resourceID );
	void destroy();
	void render( PCSTR text, int XPos, int YPos, int areaWidth, 
		int areaHeight, ULONG color, bool bBold, int alignment );

	long load( PCSTR fontName);
	ULONG height() const;
	ULONG width( PCSTR st) const;
	ULONG height( PCSTR st, int areaWidth ) const;
	void		  getSize( ULONG& width, ULONG& height, PCSTR pText );

	static HGOSFONT3D loadFont( long resourceID, long& size );
	long	getSize() { return size; }

	HGOSFONT3D getTempHandle() { return gosFont; } // don't you dare hang on to this
	int			getFontID() { return resID; }
private:
	HGOSFONT3D		gosFont;
	int				resID;
	char			fontName[64]; // so we can copy fonts
	long			size;
	void copyData( const aFont& src );

};

#endif
