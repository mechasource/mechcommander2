/*************************************************************************************************\
aEdit.h			: Interface for the aEdit component of the GUI library.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef AEDIT_H
#define AEDIT_H

#ifndef ASYSTEM_H
#include <gui/asystem.h>
#endif

#ifndef ESTRING_H
#include "estring.h"
#endif

namespace mechgui {

//*************************************************************************************************
//Entry field styles
#define ES_EDITALPHA			0x00020000  //Entry field alpha only
#define ES_EDITNUM				0x00040000  //Entry field numeric only
#define ES_EDITALPHANUM   0x00080000  //Entry field alpha or numeric
#define ES_EDITUPPER			0x00100000  //Entry field convert to uppercase
#define ES_EDITREADONLY   0x00200000  //Read only
#define ES_EDITNOBLANK		0x00400000	//Ignore ' '

/**************************************************************************************************
CLASS DESCRIPTION
aEdit:
**************************************************************************************************/
class aEdit: public aObject
{
	public:

		aEdit( int32_t fontID );
		virtual ~aEdit();
		aEdit(); // really need a font for this to work
		aEdit& operator=( const aEdit& );

		virtual void update();
		virtual void render();

		void	renderWithDropShadow();

	
		void getEntry(EString& str);
		void setEntry(const EString& str, UCHAR byHighlight = 0);
		void limitEntry(int nNewLimit) { nLimit = nNewLimit; }
		void setFocus(bool bHasFocus);
		void setFont( int32_t fontID );
		void setTextColor( int32_t color ){ textColor = color; }
		void setSelectedColor( int32_t color ){ selectedColor = color; }
		bool hasFocus()const { return bFocus; }


		void init( FitIniFile* file, PCSTR header );
		int32_t getHighlightColor( ) const { return highlightColor; }
		int32_t getColor() const { return textColor; }
		int32_t getCursorColor() const { return cursorColor; }
		int32_t getSelectedColor() const { return selectedColor; }

		void allowWierdChars( bool bAllow ){ bWierdChars = bAllow; }

		aFont* getFontObject() { return &font; }

		void setReadOnly( bool bReadOnly )
		{ 
			if ( bReadOnly )
				dwStyleFlags |= ES_EDITREADONLY; 
			else 
				dwStyleFlags ^= ES_EDITREADONLY;
		};

		void setNoBlank( bool bNoBlank )
		{
			if ( bNoBlank )
				dwStyleFlags |= ES_EDITNOBLANK; 
			else 
				dwStyleFlags ^= ES_EDITNOBLANK;
		}

		void setNumeric( bool bNoAlpha )
		{
			if ( bNoAlpha )
				dwStyleFlags |= ES_EDITNUM; 
			else 
				dwStyleFlags ^= ES_EDITNUM;
		}

		int getFont() { return font.getFontID(); }

		void initBufferSize( ULONG newSize );
		void setBufferSize( int newSize ) { text.SetBufferSize( newSize ); }

		void	allowIME( bool bAllow ){ bAllowIME = bAllow; }


	private:

		aEdit( const aEdit& );

		// HELPER FUNCTIONS
		bool	clearSelection();
		void	backSpace(int nPosition);
		void	drawCursor();
		void	hideCursor();
		bool	handleFormattingKeys(int key);
		int		charXPos(int nOffset);
		void	makeCursorVisible();
		void	flushCursorRight();
		int		findChar(int nXPos);

		void	handleMouse();
		void	handleKeyboard();

		int charLength( int index );
		

		int32_t cursorColor;
		int32_t highlightColor; // backdrop
		int32_t textColor;
		int32_t selectedColor; // selected text
		int32_t outlineColor;
		int		nLimit;
		int		nInsertion1,nInsertion2; // beginning and ending highlight positions (when equal there is no highlight)
		bool	bCursorVisible;
		int		nLeftOffset;
		float	cursorTime;
		bool	bMouseDown;
		EString	text;
		aFont	font;
		bool	bFocus;
		ULONG dwStyleFlags;
		bool	bAllowIME;
		bool	bWierdChars;

		bool bIMEInitialized;

};


//*************************************************************************************************
#endif  // end of file ( aEdit.h )
