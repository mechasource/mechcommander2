//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef AFONT_H
#define AFONT_H

#include <gameos.hpp>

class aFont;



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
		int areaHeight, unsigned long color, bool bBold, int alignment );

	long load( PCSTR fontName);
	unsigned long height() const;
	unsigned long width( PCSTR st) const;
	unsigned long height( PCSTR st, int areaWidth ) const;
	void		  getSize( unsigned long& width, unsigned long& height, PCSTR pText );
	
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

// Error codes, local to this file...
#define	FONT_NOT_LOADED			-3

#endif
