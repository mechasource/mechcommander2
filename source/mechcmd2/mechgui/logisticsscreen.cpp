/*************************************************************************************************\
LogisticsScreen.cpp			: Implementation of the LogisticsScreen component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#define LOGISTICSSCREEN_CPP

#include "stdinc.h"
#include "mechgui/logisticsscreen.h"
#include "inifile.h"
#include "mechgui/asystem.h"
#include "mechgui/abutton.h"
#include "mechgui/aedit.h"
#include "err.h"
#include "mechgui/aanimobject.h"

extern int32_t helpTextID;
extern int32_t helpTextHeaderID;
extern float frameLength;

LogisticsScreen::LogisticsScreen()
{
	statics = 0;
	rects = 0;
	buttons = 0;
	edits = 0;
	textObjects = 0;
	animObjects = 0;
	staticCount = rectCount = buttonCount = textCount = editCount = animObjectsCount = 0;
	helpTextArrayID = -1;
	fadeInTime = fadeOutTime = fadeTime = 0;
	fadeOutMaxcolour = 0xff000000;
}

//-------------------------------------------------------------------------------------------------

LogisticsScreen::~LogisticsScreen()
{
	destroy();
}

void LogisticsScreen::destroy()
{
	clear();
}

void LogisticsScreen::clear()
{
	if (statics)
		delete[] statics;
	if (rects)
		delete[] rects;
	if (buttons)
		delete[] buttons;
	if (edits)
		delete[] edits;
	if (textObjects)
		delete[] textObjects;
	if (animObjects)
		delete[] animObjects;
	statics = 0;
	rects = 0;
	buttons = 0;
	edits = 0;
	textObjects = 0;
	animObjects = 0;
	staticCount = 0;
	rectCount = 0;
	buttonCount = 0;
	editCount = 0;
	textCount = 0;
	animObjectsCount = 0;
}

//-------------------------------------------------------------------------------------------------

void LogisticsScreen::init(FitIniFile& file, std::wstring_view staticName, std::wstring_view textName, std::wstring_view rectName,
	std::wstring_view buttonName, std::wstring_view editName, std::wstring_view animObjectName, uint32_t neverFlush)
{
	clear();
	wchar_t blockname[256];
	// init statics
	if (staticName)
	{
		sprintf(blockname, "%s%c", staticName, 's');
		if (NO_ERROR == file.seekBlock(blockname))
		{
			file.readIdLong("staticCount", staticCount);
			if (staticCount)
			{
				statics = new aObject[staticCount];
				wchar_t blockname[128];
				for (size_t i = 0; i < staticCount; i++)
				{
					sprintf(blockname, "%s%ld", staticName, i);
					statics[i].init(&file, blockname);
				}
			}
		}
	}
	if (rectName)
	{
		// init rects
		sprintf(blockname, "%s%c", rectName, 's');
		if (NO_ERROR == file.seekBlock(blockname))
		{
			file.readIdLong("rectCount", rectCount);
			if (rectCount)
			{
				rects = new aRect[rectCount];
				wchar_t blockname[128];
				for (size_t i = 0; i < rectCount; i++)
				{
					sprintf(blockname, "%s%ld", rectName, i);
					rects[i].init(&file, blockname);
				}
			}
		}
	}
	// init buttons
	if (buttonName)
	{
		sprintf(blockname, "%s%c", buttonName, 's');
		if (NO_ERROR == file.seekBlock(blockname))
		{
			file.readIdLong("buttonCount", buttonCount);
			if (buttonCount)
			{
				wchar_t blockname[128];
				buttons = new aAnimButton[buttonCount];
				for (size_t i = 0; i < buttonCount; i++)
				{
					sprintf(blockname, "%s%ld", buttonName, i);
					buttons[i].init(file, blockname);
					addChild(&buttons[i]);
				}
			}
		}
	}
	// init texts
	if (textName)
	{
		sprintf(blockname, "%s%c", textName, 's');
		if (NO_ERROR == file.seekBlock(blockname))
		{
			if (NO_ERROR != file.readIdLong("TextEntryCount", textCount))
				file.readIdLong("TextCount", textCount);
			if (textCount)
			{
				textObjects = new aText[textCount];
				wchar_t blockname[64];
				for (size_t i = 0; i < textCount; i++)
				{
					sprintf(blockname, "%s%ld", textName, i);
					textObjects[i].init(&file, blockname);
				}
			}
		}
	}
	if (editName)
	{
		sprintf(blockname, "%s%c", editName, 's');
		if (NO_ERROR == file.seekBlock(blockname))
		{
			if (NO_ERROR != file.readIdLong("EditCount", editCount))
				file.readIdLong("EditCount", editCount);
			if (editCount)
			{
				edits = new aEdit[editCount];
				wchar_t blockname[64];
				for (size_t i = 0; i < editCount; i++)
				{
					sprintf(blockname, "%s%ld", editName, i);
					edits[i].init(&file, blockname);
				}
			}
		}
	}
	if (animObjectName)
	{
		sprintf(blockname, "%s%c", animObjectName, 's');
		if (NO_ERROR == file.seekBlock(blockname))
		{
			file.readIdLong("Count", animObjectsCount);
			if (animObjectsCount)
			{
				animObjects = new aAnimObject[animObjectsCount];
				wchar_t blockname[64];
				for (size_t i = 0; i < animObjectsCount; i++)
				{
					sprintf(blockname, "%s%ld", animObjectName, i);
					animObjects[i].init(&file, blockname, neverFlush);
				}
			}
		}
	}
}

aButton*
LogisticsScreen::getButton(int32_t who)
{
	for (size_t i = 0; i < buttonCount; i++)
	{
		if (buttons[i].getID() == who)
		{
			return &buttons[i];
		}
	}
	return nullptr;
}

aRect*
LogisticsScreen::getRect(int32_t who)
{
	if ((who >= 0) && (who < rectCount))
	{
		return &rects[who];
	}
	return nullptr;
}

//-------------------------------------------------------------------------------------------------
void LogisticsScreen::update()
{
	int32_t i;
	for (i = 0; i < staticCount; i++)
	{
		statics[i].update();
	}
	for (i = 0; i < buttonCount; i++)
	{
		buttons[i].update();
	}
	for (i = 0; i < textCount; i++)
		textObjects[i].update();
	for (i = 0; i < rectCount; i++)
		rects[i].update();
	// help text
	if (helpTextArrayID != -1)
	{
		if (::helpTextID)
		{
			std::wstring_view helpText;
			wchar_t tmp[1024];
			//	if ( helpTextHeaderID )
			//	{
			//		cLoadString( helpTextHeaderID, tmp, 255 );
			//		helpText = tmp;
			//		helpText.MakeUpper();
			//		helpText += '\n';
			//	}
			cLoadString(helpTextID, tmp, 1024);
			helpText = tmp;
			textObjects[helpTextArrayID].setText(helpText);
		}
		else
			textObjects[helpTextArrayID].setText("");
	}
	for (i = 0; i < editCount; i++)
	{
		edits[i].update();
	}
	for (i = 0; i < animObjectsCount; i++)
		animObjects[i].update();
	//	if ( gos_GetKeyStatus( KEY_RETURN ) == KEY_RELEASED )
	//	{
	//		if ( getButton( 50 /*MB_MSG_NEXT*/ ) )
	//		{
	//			if ( getButton(50 )->isEnabled() )
	//				handleMessage( aMSG_LEFTMOUSEDOWN, 50 );
	//		}
	//	}
	//	if ( gos_GetKeyStatus( KEY_ESCAPE ) == KEY_RELEASED )
	//	{
	//		if ( getButton( 57 /*MB_MSG_MAINMENU*/ ) )
	//		{
	//			if ( getButton(57 )->isEnabled() )
	//				handleMessage( aMSG_LEFTMOUSEDOWN, 57 );
	//		}
	//	}
	helpTextID = 0;
}

//-------------------------------------------------------------------------------------------------
void LogisticsScreen::render()
{
	if (!isShowing())
		return;
	int32_t i;
	for (i = 0; i < rectCount; i++)
	{
		if (!rects[i].bOutline && ((rects[i].getcolour() & 0xff000000) == 0xff000000))
			rects[i].render();
	}
	for (i = 0; i < staticCount; i++)
		statics[i].render();
	for (i = 0; i < rectCount; i++)
	{
		if (rects[i].bOutline)
			rects[i].render();
	}
	// transparencies after statics
	for (i = 0; i < rectCount; i++)
	{
		if ((rects[i].getcolour() & 0xff000000) != 0xff000000)
			rects[i].render();
	}
	for (i = 0; i < buttonCount; i++)
		buttons[i].render();
	for (i = 0; i < textCount; i++)
	{
		textObjects[i].render();
	}
	for (i = 0; i < editCount; i++)
		edits[i].render();
	for (i = 0; i < animObjectsCount; i++)
		animObjects[i].render();
	if (fadeOutTime)
	{
		fadeTime += frameLength;
		int32_t color = interpolatecolour(0, fadeOutMaxcolour, fadeTime / fadeOutTime);
		RECT rect = {0, 0, Environment.screenwidth, Environment.screenheight};
		drawRect(rect, color);
	}
	else if (fadeInTime && fadeInTime > fadeTime)
	{
		fadeTime += frameLength;
		int32_t color = interpolatecolour(fadeOutMaxcolour, 0, fadeTime / fadeInTime);
		RECT rect = {0, 0, Environment.screenwidth, Environment.screenheight};
		drawRect(rect, color);
	}
}

int32_t
LogisticsScreen::getStatus()
{
	if (status != RUNNING && fadeOutTime)
	{
		if (fadeTime > fadeOutTime)
		{
			return status;
		}
		else
			return RUNNING; // fake it until done fading
	}
	return status;
}

void LogisticsScreen::render(int32_t xOffset, int32_t yOffset)
{
	if (!isShowing())
		return;
	int32_t i;
	for (i = 0; i < rectCount; i++)
	{
		if (!rects[i].bOutline && ((rects[i].getcolour() & 0xff000000) == 0xff000000))
		{
			rects[i].move(xOffset, yOffset);
			rects[i].render();
			rects[i].move(-xOffset, -yOffset);
		}
	}
	for (i = 0; i < staticCount; i++)
	{
		statics[i].move(xOffset, yOffset);
		statics[i].render();
		statics[i].move(-xOffset, -yOffset);
	}
	for (i = 0; i < rectCount; i++)
	{
		if (rects[i].bOutline)
		{
			rects[i].move(xOffset, yOffset);
			rects[i].render();
			rects[i].move(-xOffset, -yOffset);
		}
	}
	// transparencies after statics
	for (i = 0; i < rectCount; i++)
	{
		if ((rects[i].getcolour() & 0xff000000) != 0xff000000)
		{
			rects[i].move(xOffset, yOffset);
			rects[i].render();
			rects[i].move(-xOffset, -yOffset);
		}
	}
	for (i = 0; i < buttonCount; i++)
	{
		buttons[i].move(xOffset, yOffset);
		buttons[i].render();
		buttons[i].move(-xOffset, -yOffset);
	}
	for (i = 0; i < textCount; i++)
	{
		textObjects[i].move(xOffset, yOffset);
		textObjects[i].render();
		textObjects[i].move(-xOffset, -yOffset);
	}
	for (i = 0; i < editCount; i++)
	{
		edits[i].move(xOffset, yOffset);
		edits[i].render();
		edits[i].move(-xOffset, -yOffset);
	}
	for (i = 0; i < animObjectsCount; i++)
	{
		animObjects[i].move(xOffset, yOffset);
		animObjects[i].render();
		animObjects[i].move(-xOffset, -yOffset);
	}
	if (fadeOutTime)
	{
		fadeTime += frameLength;
		int32_t color = interpolatecolour(0, 0xff000000, fadeTime / fadeOutTime);
		RECT rect = {0, 0, Environment.screenwidth, Environment.screenheight};
		drawRect(rect, color);
	}
	else if (fadeInTime && fadeInTime > fadeTime)
	{
		fadeTime += frameLength;
		int32_t color = interpolatecolour(0xff000000, 0, fadeTime / fadeInTime);
		RECT rect = {0, 0, Environment.screenwidth, Environment.screenheight};
		drawRect(rect, color);
	}
}

LogisticsScreen::LogisticsScreen(const LogisticsScreen& src)
{
	copyData(src);
}
LogisticsScreen&
LogisticsScreen::operator=(const LogisticsScreen& src)
{
	copyData(src);
	return *this;
}

void LogisticsScreen::copyData(const LogisticsScreen& src)
{
	if (&src != this)
	{
		destroy();
		rectCount = src.rectCount;
		if (rectCount)
		{
			rects = new aRect[rectCount];
			for (size_t i = 0; i < src.rectCount; i++)
			{
				rects[i] = (src.rects[i]);
			}
		}
		staticCount = src.staticCount;
		if (staticCount)
		{
			statics = new aObject[staticCount];
			for (size_t i = 0; i < staticCount; i++)
			{
				statics[i] = src.statics[i];
			}
		}
		buttonCount = src.buttonCount;
		if (buttonCount)
		{
			buttons = new aAnimButton[buttonCount];
			for (size_t i = 0; i < buttonCount; i++)
			{
				buttons[i] = src.buttons[i];
			}
		}
		textCount = src.textCount;
		if (textCount)
		{
			textObjects = new aText[textCount];
			for (size_t i = 0; i < textCount; i++)
			{
				textObjects[i] = src.textObjects[i];
			}
		}
		animObjectsCount = src.animObjectsCount;
		if (animObjectsCount)
		{
			animObjects = new aAnimObject[animObjectsCount];
			for (size_t i = 0; i < animObjectsCount; i++)
				animObjects[i] = src.animObjects[i];
		}
		editCount = src.editCount;
		if (editCount)
		{
			edits = new aEdit[editCount];
			for (size_t i = 0; i < editCount; i++)
				edits[i] = src.edits[i];
		}
	}
}

void LogisticsScreen::moveTo(int32_t xPos, int32_t yPos)
{
	int32_t xOffset = xPos - globalX();
	int32_t yOffset = yPos - globalY();
	aObject::init(xPos, yPos, 800, 600);
	move(xOffset, yOffset);
}

void LogisticsScreen::move(int32_t xOffset, int32_t yOffset)
{
	int32_t i;
	for (i = 0; i < rectCount; i++)
	{
		rects[i].move(xOffset, yOffset);
	}
	for (i = 0; i < staticCount; i++)
	{
		statics[i].move(xOffset, yOffset);
	}
	for (i = 0; i < buttonCount; i++)
	{
		buttons[i].move(xOffset, yOffset);
	}
	for (i = 0; i < textCount; i++)
	{
		textObjects[i].move(xOffset, yOffset);
	}
	for (i = 0; i < editCount; i++)
		edits[i].move(xOffset, yOffset);
	for (i = 0; i < animObjectsCount; i++)
		animObjects[i].move(xOffset, yOffset);
}

bool LogisticsScreen::inside(int32_t x, int32_t y)
{
	int32_t i;
	for (i = 0; i < staticCount; i++)
	{
		if (statics[i].pointInside(x, y))
			return true;
	}
	for (i = 0; i < buttonCount; i++)
	{
		if (buttons[i].pointInside(x, y))
			return true;
	}
	for (i = 0; i < textCount; i++)
	{
		if (textObjects[i].pointInside(x, y))
			return true;
	}
	for (i = 0; i < rectCount; i++)
	{
		if (rects[i].pointInside(x, y))
			return true;
	}
	for (i = 0; i < animObjectsCount; i++)
	{
		if (animObjects[i].pointInside(x, y))
			return true;
	}
	return false;
}

void LogisticsScreen::begin()
{
	for (size_t i = 0; i < animObjectsCount; i++)
		animObjects[i].begin();
	status = RUNNING;
	gos_KeyboardFlush();
}

// end of file ( LogisticsScreen.cpp )
