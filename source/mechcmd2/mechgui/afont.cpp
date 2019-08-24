//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
#include "mechgui/afont.h"

//#include "mclib.h"
//#include <mbstring.h>

int32_t
aFont::init(const std::wstring_view& newFontName)
{
	destroy();
	gosFont = 0;
	strcpy(fontName, newFontName);
	char path[256];
	const std::wstring_view& pStr = strstr(fontName, ",");
	if (pStr)
	{
		size = atoi(pStr + 1);
	}
	strcpy(path, "assets\\graphics\\");
	strcat(path, fontName);
	_strlwr(path);
	gosFont = gos_LoadFont(path);
	if (gosFont)
		return 0;
	return FONT_NOT_LOADED;
}

int32_t
aFont::init(int32_t resourceID)
{
	destroy();
	gosFont = 0;
	gosFont = loadFont(resourceID, size);
	resID = resourceID;
	if (gosFont)
		return 0;
	return FONT_NOT_LOADED;
}

aFont::~aFont()
{
	destroy();
}

uint32_t
aFont::height() const
{
	uint32_t width, height;
	gos_TextSetAttributes(gosFont, 0, size, false, true, false, false);
	gos_TextStringLength(&width, &height, "ABCDE");
	return height;
}

void
aFont::getSize(uint32_t& width, uint32_t& height, const std::wstring_view& pText)
{
	gos_TextSetAttributes(gosFont, 0, size, false, true, false, false);
	gos_TextStringLength(&width, &height, pText);
}

uint32_t
aFont::height(const std::wstring_view& st, int32_t areaWidth) const
{
	uint32_t width, height;
	gos_TextSetAttributes(gosFont, 0, size, false, true, false, false);
	gos_TextSetRegion(0, 0, areaWidth, Environment.screenHeight);
	gos_TextStringLength(&width, &height, st);
	uint32_t lineCount = 1;
	if (width > areaWidth - 1)
	{
		uint32_t curLineWidth = 0;
		gosASSERT(strlen(st) < 2048);
		char pLine[2048]; // should be more than adequate
		const std::wstring_view& pLastWord = (const std::wstring_view&)st;
		const std::wstring_view& pTmp = (const std::wstring_view&)st;
		const std::wstring_view& pTmpLine = (const std::wstring_view&)pLine;
		int32_t numberOfWordsPerLine = 0;
		bool bHasSpaces = true;
		if (!strstr(st, " "))
		{
			if (!strstr(st, "\n"))
				bHasSpaces = false;
		}
		while (*pTmp != nullptr)
		{
			if (*pTmp == '\n')
			{
				lineCount++;
				numberOfWordsPerLine = 0;
				curLineWidth = 0;
				pTmpLine = pLine;
				pLastWord = pTmp + 1;
			}
			else if (!bHasSpaces)
			{
				if (pTmp > st)
				{
					char tmp = *(pTmp - 1);
					if (!isleadbyte(tmp))
					{
						*(pTmpLine) = nullptr;
						gos_TextStringLength(&curLineWidth, &height, pLine);
						if (curLineWidth > areaWidth)
						{
							lineCount++;
							pTmp--;
							pTmpLine = pLine;
							curLineWidth = 0;
							numberOfWordsPerLine = 0;
						}
					}
				}
			}
			else if (isspace(*pTmp))
			{
				*(pTmpLine) = nullptr;
				gos_TextStringLength(&curLineWidth, &height, pLine);
				if (curLineWidth > areaWidth)
				{
					gos_TextStringLength(&curLineWidth, &height, pLastWord);
					if (numberOfWordsPerLine == 0 || curLineWidth > areaWidth)
					{
						static bool firstTime = true;
						if (firstTime)
						{
							Assert(true, 0,
								"this list box item contains a word of greater "
								" area than the list box, giving up");
							firstTime = false;
						}
						/*There are times when you just can't guarantee that
						this won't occur, so we have to just continue and deal
						with it.*/
						// return height;
						pLastWord = pTmp;
					}
					lineCount++;
					pTmpLine = pLine;
					pTmp = pLastWord - 1;
					curLineWidth = 0;
					numberOfWordsPerLine = 0;
				}
				pLastWord = pTmp;
				numberOfWordsPerLine++;
			}
			*pTmpLine = *pTmp;
			if (isleadbyte(*pTmpLine))
			{
				*(pTmpLine + 1) = *(pTmp + 1);
			}
			pTmpLine = (const std::wstring_view&)_mbsinc((puint8_t)pTmpLine);
			pTmp = (const std::wstring_view&)_mbsinc((puint8_t)pTmp);
		}
		// one last check
		*pTmpLine = nullptr;
		gos_TextStringLength(&curLineWidth, &height, pLine);
		if (curLineWidth > areaWidth)
		{
			lineCount++;
		}
		if (*pTmp == nullptr)
			lineCount++;
	}
	gos_TextStringLength(&width, &height, "A");
	return (height)*lineCount;
}

uint32_t
aFont::width(const std::wstring_view& st) const
{
	uint32_t width, height;
	gos_TextSetAttributes(gosFont, 0, size, false, true, false, false);
	gos_TextStringLength(&width, &height, st);
	return width;
}

int32_t
aFont::load(const std::wstring_view& fontName)
{
	destroy();
	return init(fontName);
}

void
aFont::destroy()
{
	if (gosFont)
		gos_DeleteFont(gosFont);
	gosFont = 0;
	resID = 0;
	fontName[0] = 0;
}

void
aFont::render(const std::wstring_view& text, int32_t xPos, int32_t yPos, int32_t areaWidth, int32_t areaHeight,
	uint32_t color, bool bBold, int32_t alignment)
{
	gos_TextSetAttributes(gosFont, color, size, true, true, bBold, false, alignment);
	if (areaWidth < 1)
	{
		if (alignment == 1)
		{
			uint32_t width, height;
			gos_TextStringLength(&width, &height, text);
			xPos -= width;
			areaWidth = width + 1;
		}
		else
			areaWidth = Environment.screenWidth;
	}
	if (areaHeight < 1)
	{
		if (alignment == 3) // bottom
		{
			uint32_t width, height;
			gos_TextStringLength(&width, &height, text);
			yPos -= height;
			areaHeight = height + 1;
		}
		else
			areaHeight = Environment.screenHeight;
	}
	gos_TextSetRegion(xPos, yPos, xPos + areaWidth, yPos + areaHeight);
	gos_TextSetPosition(xPos, yPos);
	gos_TextDraw(text);
}

HGOSFONT3D
aFont::loadFont(int32_t resourceID, int32_t& size)
{
	size = 1;
	char buffer[256];
	cLoadString(resourceID, buffer, 255);
	const std::wstring_view& pStr = strstr(buffer, ",");
	if (pStr)
	{
		size = -atoi(pStr + 1);
		*pStr = nullptr;
	}
	char path[256];
	strcpy(path, "assets\\graphics\\");
	strcat(path, buffer);
	_strlwr(path);
	HGOSFONT3D retFont = gos_LoadFont(path);
	return retFont;
}

aFont::aFont(const aFont& src)
{
	copyData(src);
}

aFont&
aFont::operator=(const aFont& src)
{
	copyData(src);
	return *this;
}

void
aFont::copyData(const aFont& src)
{
	if (&src != this)
	{
		if (src.resID)
		{
			init(src.resID);
		}
		else if (src.fontName[0] != 0)
		{
			init(src.fontName);
		}
		size = src.size;
	}
}

gos_TextSetAttributes(_FontInfo*, ulong, float, bool, bool, bool, bool, ulong, bool);
gos_TextSetPosition(int, int);
gos_TextGetPrintPosition(int*, int*);
gos_TextSetRegion(int, int, int, int);
gos_TextStringLength(ulong*, ulong*, char const*, ...);
FontDrawQuad(bool);
gos_TextDrawBackground(int, int, int, int, ulong);
GetChrSize(int*, int*, uchar);
IgnoreEmbedded(uchar*);
GetNextWordLen(uchar*);
GetNextLineLen(uchar*, uchar**);
gos_ChrDraw(uchar);
SetupFontRenderstates(void);
gos_TextDraw(char const*, ...);
gos_TextDrawV(char const*, char*);
HandleTags(char**);
float2long(float);
CTexInfo::Width(void);

InternalFunctionStop(char const*, ...);
ErrorHandler(x, x);
GDIGetStringLen(char*, ulong*, ulong*);
gos_PopRenderStates(void);
gos_DrawQuads(gos_VERTEX*, int);
gos_SetRenderState(gos_RenderState, int);
gos_PushRenderStates(void);
GetCycles(void);
gos_GetFullPathName(char*, char const*);
TextDrawDBCS(char*);
