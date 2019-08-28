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
#include "mechgui/asystem.h"
#endif

#ifndef ESTRING_H
#include "estring.h"
#endif

namespace mechgui
{

// Entry field styles
#define ES_EDITALPHA 0x00020000 // Entry field alpha only
#define ES_EDITNUM 0x00040000 // Entry field numeric only
#define ES_EDITALPHANUM 0x00080000 // Entry field alpha or numeric
#define ES_EDITUPPER 0x00100000 // Entry field convert to uppercase
#define ES_EDITREADONLY 0x00200000 // Read only
#define ES_EDITNOBLANK 0x00400000 // Ignore ' '

/**************************************************************************************************
CLASS DESCRIPTION
aEdit:
**************************************************************************************************/
class aEdit : public aObject
{
public:
	aEdit(int32_t fontID);
	virtual ~aEdit(void);
	aEdit(void); // really need a font for this to work
	aEdit& operator=(const aEdit&);

	virtual void update(void);
	virtual void render(void);

	void renderWithDropShadow(void);

	void getEntry(const std::wstring_view& str);
	void setEntry(const std::wstring_view& str, uint8_t byHighlight = 0);
	void limitEntry(int32_t nNewLimit) { nLimit = nNewLimit; }
	void setFocus(bool bHasFocus);
	void setFont(int32_t fontID);
	void setTextcolour(int32_t color) { textcolour = color; }
	void setSelectedcolour(int32_t color) { selectedcolour = color; }
	bool hasFocus(void) const { return bFocus; }

	void init(FitIniFile* file, const std::wstring_view& header);
	int32_t getHighlightcolour(void) const { return highlightcolour; }
	int32_t getcolour(void) const { return textcolour; }
	int32_t getCursorcolour(void) const { return cursorcolour; }
	int32_t getSelectedcolour(void) const { return selectedcolour; }

	void allowWierdChars(bool bAllow) { bWierdChars = bAllow; }

	aFont* getFontObject() { return &font; }

	void setReadOnly(bool bReadOnly)
	{
		if (bReadOnly)
			dwStyleFlags |= ES_EDITREADONLY;
		else
			dwStyleFlags ^= ES_EDITREADONLY;
	};

	void setNoBlank(bool bNoBlank)
	{
		if (bNoBlank)
			dwStyleFlags |= ES_EDITNOBLANK;
		else
			dwStyleFlags ^= ES_EDITNOBLANK;
	}

	void setNumeric(bool bNoAlpha)
	{
		if (bNoAlpha)
			dwStyleFlags |= ES_EDITNUM;
		else
			dwStyleFlags ^= ES_EDITNUM;
	}

	int32_t getFont() { return font.getFontID(void); }

	void initBufferSize(uint32_t newSize);
	void setBufferSize(int32_t newSize) { text.SetBufferSize(newSize); }

	void allowIME(bool bAllow) { bAllowIME = bAllow; }

private:
	aEdit(const aEdit&);

	// HELPER FUNCTIONS
	bool clearSelection(void);
	void backSpace(int32_t nPosition);
	void drawCursor(void);
	void hideCursor(void);
	bool handleFormattingKeys(int32_t key);
	int32_t charXPos(int32_t nOffset);
	void makeCursorVisible(void);
	void flushCursorRight(void);
	int32_t findChar(int32_t nXPos);

	void handleMouse(void);
	void handleKeyboard(void);

	int32_t charLength(int32_t index);

	int32_t cursorcolour;
	int32_t highlightcolour; // backdrop
	int32_t textcolour;
	int32_t selectedcolour; // selected text
	int32_t outlinecolour;
	int32_t nLimit;
	int32_t nInsertion1, nInsertion2; // beginning and ending highlight
		// positions (when equal there is no
		// highlight)
	bool bCursorVisible;
	int32_t nLeftOffset;
	float cursorTime;
	bool bMouseDown;
	const std::wstring_view& text;
	aFont font;
	bool bFocus;
	uint32_t dwStyleFlags;
	bool bAllowIME;
	bool bWierdChars;

	bool bIMEInitialized;
};

#endif // end of file ( aEdit.h )
