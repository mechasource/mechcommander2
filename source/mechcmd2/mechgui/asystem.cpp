//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
#include "mclib.h"
#include "mechgui/asystem.h"
#include "packet.h"
#include "mechgui/afont.h"
#include "paths.h"
#include "userinput.h"

int32_t helpTextID = 0;
int32_t helpTextHeaderID = 0;

void
aObject::update()
{
	int32_t x = userInput->getMouseX();
	int32_t y = userInput->getMouseY();
	if (pointInside(x, y) && helpid && isShowing())
	{
		helpTextID = helpid;
	}
	// call update for the children
	for (size_t i = 0; i < nNumberOfChildren; i++)
		pChildren[i]->update();
}

// base class aObject methods

aObject::aObject()
{
	nNumberOfChildren = 0;
	pParent = nullptr;
	textureHandle = 0;
	memset(location, 0, sizeof(gos_VERTEX) * 4);
	for (size_t i = 0; i < 4; i++)
		location[i].rhw = .5;
	showWindow = 1;
	helpid = 0;
	filewidth = 0.0f;
}

aObject::~aObject()
{
	destroy(); //	destroy will often have been called already, so better be
		// sure
		//	it's safe to call twice
}

int32_t
aObject::init(int32_t xPos, int32_t yPos, int32_t w, int32_t h)
{
	location[0].x = xPos;
	location[0].y = yPos;
	location[1].x = xPos;
	location[1].y = yPos + h;
	location[2].x = xPos + w;
	location[2].y = yPos + h;
	location[3].x = xPos + w;
	location[3].y = yPos;
	for (size_t i = 0; i < 4; i++)
	{
		location[i].u = 0.f;
		location[i].v = 0.f;
		location[i].z = 0.f;
		location[i].rhw = 0.5f;
		location[i].argb = 0xff000000;
		location[i].frgb = 0;
	}
	showWindow = TRUE;
	nNumberOfChildren = 0;
	pParent = nullptr;
	return (NO_ERROR);
}

void
aObject::init(FitIniFile* file, const std::wstring_view& blockName, uint32_t neverFlush)
{
	memset(location, 0, sizeof(location));
	wchar_t fileName[256];
	textureHandle = 0;
	filewidth = 256.;
	if (NO_ERROR != file->seekBlock(blockName))
	{
		wchar_t errBuffer[256];
		sprintf(errBuffer, "couldn't find static block %s", blockName);
		Assert(0, 0, errBuffer);
		return;
	}
	int32_t x, y, width, height;
	file->readIdLong("XLocation", x);
	file->readIdLong("YLocation", y);
	file->readIdLong("width", width);
	file->readIdLong("height", height);
	file->readIdLong("HelpCaption", helpHeader);
	file->readIdLong("HelpDesc", helpid);
	if (NO_ERROR == file->readIdString("fileName", fileName, 32))
	{
		bool bAlpha = 0;
		file->readIdBoolean("Alpha", bAlpha);
		if (!textureHandle)
		{
			wchar_t buffer[256];
			strcpy(buffer, artPath);
			strcat(buffer, fileName);
			_strlwr(buffer);
			if (!strstr(buffer, ".tga"))
				strcat(buffer, ".tga");
			int32_t ID = mcTextureManager->loadTexture(
				buffer, bAlpha ? gos_Texture_Alpha : gos_Texture_Keyed, 0, 0, 0x2);
			textureHandle = ID;
			uint32_t gosID = mcTextureManager->get_gosTextureHandle(ID);
			TEXTUREPTR textureData;
			gos_LockTexture(gosID, 0, 0, &textureData);
			filewidth = textureData.width;
			gos_UnLockTexture(gosID);
		}
	}
	int32_t u, v, uwidth, vheight;
	bool bRotated = 0;
	file->readIdLong("UNormal", u);
	file->readIdLong("VNormal", v);
	file->readIdLong("Uwidth", uwidth);
	file->readIdLong("Vheight", vheight);
	file->readIdBoolean("texturesRotated", bRotated);
	for (size_t k = 0; k < 4; k++)
	{
		location[k].argb = 0xffffffff;
		location[k].frgb = 0;
		location[k].x = x;
		location[k].y = y;
		location[k].z = 0.f;
		location[k].rhw = .5;
		if (filewidth)
			location[k].u = (float)u / (float)filewidth + (.1f / (float)filewidth);
		if (filewidth)
			location[k].v = (float)v / (float)filewidth + (.1f / (float)filewidth);
	}
	location[3].x = location[2].x = x + width;
	location[2].y = location[1].y = y + height;
	if (filewidth)
		location[2].u = location[3].u =
			((float)(u + uwidth)) / ((float)filewidth) + (.1f / (float)filewidth);
	if (filewidth)
		location[1].v = location[2].v =
			((float)(v + vheight)) / ((float)filewidth) + (.1f / (float)filewidth);
	if (bRotated)
	{
		location[0].u = (u + uwidth) / (float)filewidth + (.1f / (float)filewidth);
		;
		location[1].u = u / (float)filewidth + (.1f / (float)filewidth);
		;
		location[2].u = u / (float)filewidth + (.1f / (float)filewidth);
		location[3].u = (u + uwidth) / (float)filewidth + (.1f / (float)filewidth);
		location[0].v = v / (float)filewidth + (.1f / (float)filewidth);
		;
		location[1].v = v / (float)filewidth + (.1f / (float)filewidth);
		;
		location[2].v = (v + vheight) / (float)filewidth + (.1f / (float)filewidth);
		;
		location[3].v = (v + vheight) / (float)filewidth + (.1f / (float)filewidth);
		;
	}
}

void
aObject::destroy()
{
	removeAllChildren();
	if (textureHandle)
	{
		int32_t gosID = mcTextureManager->get_gosTextureHandle(textureHandle);
		if (gosID && gosID != -1)
			mcTextureManager->removeTexture(gosID);
	}
	if (pParent)
	{
		pParent->removeChild(this);
	}
	pParent = nullptr;
}

bool
aObject::pointInside(int32_t mouseX, int32_t mouseY) const
{
	if ((location[0].x) <= mouseX && location[3].x >= mouseX && location[0].y <= mouseY && location[1].y >= mouseY)
		return true;
	return false;
}

bool
aObject::rectIntersect(int32_t left, int32_t top, int32_t right, int32_t bottom) const
{
	if ((right > location[0].x) && (left < location[2].x) && (bottom > location[0].y) && (top < location[2].y))
		return (TRUE);
	else
		return (FALSE);
}

bool
aObject::rectIntersect(const RECT& testRect) const
{
	if ((testRect.right > location[0].x) && (testRect.left < location[0].y) && (testRect.bottom > location[2].y) && (testRect.top < location[2].y))
		return (TRUE);
	else
		return (FALSE);
}

aObject*
aObject::findObject(int32_t xPos, int32_t yPos)
{
	aObject* target;
	if (showWindow)
	{
		for (size_t i = nNumberOfChildren; i > 0; i--)
		{
			target = pChildren[i - 1]->findObject(xPos, yPos);
			if (target)
				return target;
		}
	}
	if (showWindow && pointInside(xPos, yPos))
		return (this);
	return nullptr;
}

void
aObject::setParent(aObject* p)
{
	pParent = p;
}

int32_t
aObject::numberOfChildren(void) const
{
	return nNumberOfChildren;
}

void
aObject::addChild(aObject* c)
{
	Assert(nNumberOfChildren < MAX_CHILDREN, nNumberOfChildren + 1, "Too many children!");
	Assert(c->getParent() == nullptr || c->getParent() == this, 0,
		" Adding child that's someone else's ");
	if (!c)
		return;
	removeChild(c); //	make sure this isn't already my child (Duplicate
		// children screws up bringToFront())
	c->setParent(this);
	pChildren[nNumberOfChildren] = c;
	nNumberOfChildren++;
	c->move(x(), y());
}

void
aObject::removeChild(aObject* c)
{
	if (!c) // If this is nullptr, shouldn't we still remove it from the list?
		return;
	if ((c->getParent() == this) || (c->getParent() == nullptr)) // Normal situation
	{
		for (size_t cc = 0; cc < nNumberOfChildren; cc++)
		{
			if (pChildren[cc] == c)
			{
				// found the child, remove it and shift the rest of the children
				// up
				for (size_t sc = cc; sc < nNumberOfChildren - 1; sc++)
					pChildren[sc] = pChildren[sc + 1];
				pChildren[nNumberOfChildren] = nullptr;
				nNumberOfChildren--;
				c->setParent(nullptr);
				return;
			}
		}
	}
	else
	{
		gosASSERT(0);
	}
}

aObject*
aObject::child(int32_t w)
{
	if (w > nNumberOfChildren - 1)
		return nullptr;
	return pChildren[w];
}

float
aObject::width(void) const
{
	return location[2].x - location[0].x;
}

float
aObject::height(void) const
{
	return location[2].y - location[0].y;
}

float
aObject::x(void) const
{
	if (pParent)
		return location[0].x - pParent->globalX();
	else
		return location[0].x;
}

float
aObject::y(void) const
{
	if (pParent)
		return location[0].y - pParent->y();
	else
		return location[0].y;
}

int32_t
aObject::globalX(void) const
{
	return location[0].x;
}

int32_t
aObject::globalY(void) const
{
	return location[0].y;
}

int32_t
aObject::globalRight(void) const
{
	return globalX() + (int32_t)width();
}

int32_t
aObject::globalBottom(void) const
{
	return globalY() + (int32_t)height();
}

void
aObject::moveTo(int32_t xPos, int32_t yPos)
{
	float offsetX = xPos - location[0].x;
	float offsetY = yPos - location[0].y;
	move(offsetX, offsetY);
}

void
aObject::moveToNoRecurse(int32_t xPos, int32_t yPos)
{
	float offsetX = xPos - location[0].x;
	float offsetY = yPos - location[0].y;
	moveNoRecurse(offsetX, offsetY);
}

void
aObject::move(float offsetX, float offsetY)
{
	int32_t i;
	for (i = 0; i < 4; i++)
	{
		location[i].x += offsetX;
		location[i].y += offsetY;
	}
	for (i = 0; i < nNumberOfChildren; i++)
	{
		pChildren[i]->move(offsetX, offsetY);
	}
}

void
aObject::moveNoRecurse(float offsetX, float offsetY)
{
	for (size_t i = 0; i < 4; i++)
	{
		location[i].x += offsetX;
		location[i].y += offsetY;
	}
}
void
aObject::resize(int32_t w, int32_t h)
{
	location[2].x = location[0].x + w;
	location[3].x = location[0].x + w;
	location[1].y = location[0].y + h;
	location[2].y = location[0].y + h;
}

void
aObject::render()
{
	if (showWindow)
	{
		uint32_t gosID = mcTextureManager->get_gosTextureHandle(textureHandle);
		gos_SetRenderState(gos_State_Texture, gosID);
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
		gos_SetRenderState(gos_State_ZCompare, 0);
		gos_SetRenderState(gos_State_ZWrite, 0);
		gos_DrawQuads(location, 4);
		for (size_t i = 0; i < nNumberOfChildren; i++)
		{
			pChildren[i]->render();
		}
	}
}

void
aObject::render(int32_t x, int32_t y)
{
	move(x, y);
	render();
	move(-x, -y);
}

void
aObject::setTexture(const std::wstring_view& fileName)
{
	if (textureHandle)
	{
		int32_t gosID = mcTextureManager->get_gosTextureHandle(textureHandle);
		if (gosID > 0)
			mcTextureManager->removeTexture(gosID);
	}
	textureHandle = mcTextureManager->loadTexture(fileName, gos_Texture_Keyed, 0, 0, 0x2);
	int32_t gosID = mcTextureManager->get_gosTextureHandle(textureHandle);
	if (gosID)
	{
		TEXTUREPTR textureData;
		gos_LockTexture(gosID, 0, 0, &textureData);
		filewidth = textureData.width;
		gos_UnLockTexture(gosID);
	}
	else
		filewidth = 256; // guess
}

void
aObject::setTexture(uint32_t newHandle)
{
	// Gotta check handle.  If its the same as the new one,
	// We don't gotta delete the old one.  The texture manager already did!!
	if (textureHandle && textureHandle != newHandle)
	{
		int32_t gosID = mcTextureManager->get_gosTextureHandle(textureHandle);
		if (gosID > 0)
			mcTextureManager->removeTexture(gosID);
	}
	textureHandle = newHandle;
	if (newHandle)
	{
		int32_t gosID = mcTextureManager->get_gosTextureHandle(newHandle);
		TEXTUREPTR textureData;
		gos_LockTexture(gosID, 0, 0, &textureData);
		filewidth = textureData.width;
		gos_UnLockTexture(gosID);
	}
}

void
aObject::setcolour(uint32_t newcolour, bool bRecurse)
{
	for (size_t i = 0; i < 4; i++)
	{
		location[i].argb = newcolour;
	}
	// set the kids?
	if (bRecurse)
	{
		for (size_t i = 0; i < this->pNumberOfChildren; i++)
		{
			pChildren[i]->setcolour(newcolour, 1);
		}
	}
}

void
aObject::setUVs(float u1, float v1, float u2, float v2)
{
	location[0].u = location[1].u = u1 / filewidth + (.1f / (float)filewidth);
	location[2].u = location[3].u = u2 / filewidth + (.1f / (float)filewidth);
	location[0].v = location[3].v = v1 / filewidth + (.1f / (float)filewidth);
	location[1].v = location[2].v = v2 / filewidth + (.1f / (float)filewidth);
}

void
aObject::removeAllChildren(bool bDelete)
{
	for (size_t i = 0; i < nNumberOfChildren; i++)
	{
		pChildren[i]->setParent(0);
		if (bDelete)
			delete pChildren[i];
	}
	memset(pChildren, 0, sizeof(aObject*) * MAX_CHILDREN);
	nNumberOfChildren = 0;
}

void
aObject::copyData(const aObject& src)
{
	if (&src != this)
	{
		if (src.textureHandle)
			textureHandle = mcTextureManager->copyTexture(src.textureHandle);
		for (size_t i = 0; i < 4; i++)
			location[i] = src.location[i];
		filewidth = src.filewidth;
		showWindow = src.showWindow;
		nNumberOfChildren = 0; // not copying the kids.
		ID = src.ID;
	}
}

aObject::aObject(const aObject& src)
{
	copyData(src);
}
aObject&
aObject::operator=(const aObject& src)
{
	if (&src != this)
		copyData(src);
	return *this;
}

/////////////////////////////////////////////////////////////

aRect::aRect()
{
	bOutline = 0;
}

void
aRect::render()
{
	int32_t color = getcolour();
	if (isShowing())
		// bOutline ? drawEmptyRect( getGUI_RECT(), color, color ) : drawRect(
		// getGUI_RECT(), color );
		bOutline ? drawEmptyRect(getGlobalGUI_RECT(), color, color)
				 : drawRect(getGlobalGUI_RECT(), color);
}

void
aRect::render(int32_t x, int32_t y)
{
	// RECT tmpRect = getGUI_RECT();
	RECT tmpRect = getGlobalGUI_RECT();
	tmpRect.left += x;
	tmpRect.right += x;
	tmpRect.top += y;
	tmpRect.bottom += y;
	int32_t color = getcolour();
	bOutline ? drawEmptyRect(tmpRect, color, color) : drawRect(tmpRect, color);
}

void
aRect::init(FitIniFile* file, const std::wstring_view& blockName)
{
	if (NO_ERROR != file->seekBlock(blockName))
	{
		wchar_t errorStr[256];
		sprintf(errorStr, "couldn't find block %s in file %s", blockName, file->getFilename());
		Assert(0, 0, errorStr);
	}
	int32_t left;
	if (NO_ERROR == file->readIdLong("left", left))
	{
		int32_t right, top, bottom;
		file->readIdLong("top", top);
		file->readIdLong("right", right);
		file->readIdLong("bottom", bottom);
		aObject::init(left, top, right - left, bottom - top);
	}
	else
	{
		// aObject::init(file, blockName);
		/*we're not using */
		int32_t x, y, width, height;
		file->readIdLong("XLocation", x);
		file->readIdLong("YLocation", y);
		file->readIdLong("width", width);
		file->readIdLong("height", height);
		aObject::init(x, y, width, height);
	}
	int32_t color = 0xff000000;
	file->readIdLong("color", color);
	setcolour(color);
	file->readIdBoolean("outline", bOutline);
	file->readIdLong("HelpCaption", helpHeader);
	file->readIdLong("HelpDesc", helpid);
}

RECT
aRect::getGUI_RECT()
{
	RECT rect;
	rect.left = left();
	rect.right = right();
	rect.top = top();
	rect.bottom = bottom();
	if (pParent)
	{
		/* if there is a parent then we have to translate from relative to
		 * absolute coordinates */
		rect.left += pParent->x();
		rect.right += pParent->x();
		rect.top += pParent->top();
		rect.bottom += pParent->top();
	}
	return rect;
}

RECT
aRect::getGlobalGUI_RECT()
{
	RECT rect;
	rect.left = globalLeft();
	rect.right = globalRight();
	rect.top = globalTop();
	rect.bottom = globalBottom();
	return rect;
}

//////////////////////////////////////////////////////////

aText::aText()
{
	alignment = 0;
}

aText::~aText() {}

void
aText::init(FitIniFile* file, const std::wstring_view& header)
{
	int32_t result = file->seekBlock(header);
	if (result != NO_ERROR)
	{
		wchar_t errorStr[256];
		sprintf(errorStr, "couldn't find the text block%s", header);
		Assert(result == NO_ERROR, 0, errorStr);
		return;
	}
	int32_t lfont;
	file->readIdLong("Font", lfont);
	font.init(lfont);
	int32_t left, top, width, height;
	file->readIdLong("XLocation", left);
	file->readIdLong("YLocation", top);
	file->readIdLong("width", width);
	file->readIdLong("height", height);
	aObject::init(left, top, width, height);
	int32_t color;
	file->readIdLong("colour", color);
	for (size_t i = 0; i < 4; i++)
		location[i].argb = color;
	file->readIdLong("Alignment", alignment);
	int32_t textID;
	if (NO_ERROR == file->readIdLong("TextID", textID))
	{
		// WAY too small.  Good crash.  Only crashes in profile.
		// cLoadString now checks buffer length and keeps game from crashing!!
		// -fs
		wchar_t tmp[1024];
		cLoadString(textID, tmp, 1023);
		text = tmp;
	}
	file->readIdLong("HelpCaption", helpHeader);
	file->readIdLong("HelpDesc", helpid);
}

void
aText::setText(const std::wstring_view& str)
{
	text = str;
}

void
aText::render()
{
	if (showWindow)
		font.render(text, location[0].x, location[0].y, location[2].x - location[0].x,
			location[2].y - location[0].y, location[0].argb, 0, alignment);
}

void
aText::render(int32_t x, int32_t y)
{
	move(x, y);
	render();
	move(-x, -y);
}

void
aText::setText(int32_t resID)
{
	wchar_t tmp[1280];
	cLoadString(resID, tmp, 1279);
	if (tmp[0] != 0)
		text = tmp;
	else
	{
		wchar_t tmpy[1280];
		memset(tmpy, 0, 1280);
		sprintf(tmpy, "nullptr for ID: %d", resID);
		text = tmpy;
	}
}

aText::aText(const aText& src)
{
	CopyData(src);
}

aText&
aText::operator=(const aText& src)
{
	CopyData(src);
	return *this;
}

void
aText::CopyData(const aText& src)
{
	if (&src != this)
	{
		text = src.text;
		alignment = src.alignment;
		font = src.font;
		aObject::operator=(src);
	}
}

bool
aText::pointInside(int32_t xPos, int32_t yPos) const
{
	if (!width() || !height())
	{
		int32_t left = location[0].x;
		int32_t top = location[0].y;
		int32_t width = font.width(text);
		int32_t height = font.height();
		if (alignment == 1) // right aligned
		{
			left -= width;
		}
		int32_t mouseX = userInput->getMouseX();
		;
		int32_t mouseY = userInput->getMouseY();
		if (left <= mouseX && left + width >= mouseX && top <= mouseY && top + height >= mouseY)
			return true;
	}
	return aObject::pointInside(xPos, yPos);
}
