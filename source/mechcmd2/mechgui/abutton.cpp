//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
#include "mechgui/abutton.h"
#include "mechgui/afont.h"
#include "mclib.h"
#include <windows.h>
#include "soundsys.h"

extern SoundSystem* sndSystem;

aButton::aButton()
{
	toggleButton = 0;
	singlePress = 0;
	messageOnRelease = 0;
	state = ENABLED;
	memset(&data, 0, sizeof(data));
	clickSFX = LOG_CLICKONBUTTON;
	highlightSFX = LOG_HIGHLIGHTBUTTONS;
	disabledSFX = LOG_WRONGBUTTON;
	data.textAlign = 2;
	holdTime = .5f;
}

int32_t
aButton::init(int32_t xPos, int32_t yPos, int32_t w, int32_t h)
{
	int32_t err;
	err = aObject::init(xPos, yPos, w, h);
	if (err)
		return err;
	return (NO_ERROR);
}

void aButton::destroy()
{
	aObject::destroy();
}

aButton&
aButton::operator=(const aButton& src)
{
	copyData(src);
	aObject::operator=(src);
	return *this;
}
aButton::aButton(const aButton& src)
	: aObject(src)
{
	copyData(src);
}
void aButton::copyData(const aButton& src)
{
	if (&src != this)
	{
		data = src.data;
		state = src.state;
	}
}

void aButton::update()
{
	if (!isShowing())
		return;
	int32_t mouseX = userInput->getMouseX();
	int32_t mouseY = userInput->getMouseY();
	if (pointInside(mouseX, mouseY))
	{
		int32_t mouseDragX = userInput->getMouseDragX();
		int32_t mouseDragY = userInput->getMouseDragY();
		if (messageOnRelease && userInput->leftMouseReleased() && pointInside(mouseDragX, mouseDragY))
		{
			press(false);
			if (getParent())
				getParent()->handleMessage(aMSG_LEFTMOUSEDOWN, data.ID);
			if (state != DISABLED && state != HIDDEN)
				sndSystem->playDigitalSample(clickSFX);
			else
				sndSystem->playDigitalSample(disabledSFX);
		}
		if (userInput->isLeftClick())
		{
			press(true);
			if (getParent() && !messageOnRelease && pointInside(userInput->getMouseDragX(), userInput->getMouseDragY()))
				getParent()->handleMessage(aMSG_LEFTMOUSEDOWN, data.ID);
			if (state != DISABLED)
				sndSystem->playDigitalSample(clickSFX);
			else
				sndSystem->playDigitalSample(disabledSFX);
		}
		else if (userInput->getMouseLeftHeld() > holdTime && !messageOnRelease && pointInside(userInput->getMouseDragX(), userInput->getMouseDragY()))
			handleMessage(aMSG_LEFTMOUSEHELD, data.ID);
		else
		{
			if (state != HIGHLIGHT && state != DISABLED && state != HIDDEN)
				sndSystem->playDigitalSample(highlightSFX);
			state = HIGHLIGHT;
			makeUVs(location, state, data);
		}
	}
	else if (state == PRESSED && messageOnRelease)
		state = ENABLED;
	aObject::update();
}

bool aButton::pointInside(int32_t xPos, int32_t yPos) const
{
	if (aObject::pointInside(xPos, yPos))
		return true;
	if (data.textRect.left <= xPos && data.textRect.right >= xPos && data.textRect.top <= yPos && data.textRect.bottom >= yPos)
	{
		return true;
	}
	return 0;
}

/////////////////////////////////////////////////
void aButton::render()
{
	if (state != HIDDEN)
	{
		if (textureHandle)
		{
			uint32_t gosID = mcTextureManager->get_gosTextureHandle(textureHandle);
			gos_SetRenderState(gos_State_Texture, gosID);
		}
		else
			gos_SetRenderState(gos_State_Texture, 0);
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
		gos_SetRenderState(gos_State_AlphaTest, true);
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureClamp);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulateAlpha);
		gos_DrawQuads(location, 4);
		if (data.textID && data.textFont)
		{
			wchar_t buffer[256];
			cLoadString(data.textID, buffer, 256);
			uint32_t width, height;
			gos_TextSetAttributes(data.textFont, data.textcolours[state], data.textSize, true, true,
				false, false, data.textAlign);
			gos_TextSetRegion(
				data.textRect.left, data.textRect.top, data.textRect.right, data.textRect.bottom);
			gos_TextStringLength(&width, &height, buffer);
			gos_TextSetPosition(data.textRect.left,
				(data.textRect.top + data.textRect.bottom) / 2 - height / 2 + 1);
			gos_TextDraw(buffer);
			if (data.outlineText)
			{
				drawEmptyRect(data.textRect, data.textcolours[state], data.textcolours[state]);
			}
		}
		if (data.outline)
		{
			RECT tmp;
			tmp.left = location[0].x;
			tmp.top = location[0].y;
			tmp.right = location[2].x;
			tmp.bottom = location[2].y;
			drawEmptyRect(tmp, location[0].argb, location[0].argb);
		}
		for (size_t i = 0; i < numberOfChildren(); i++)
		{
			pChildren[i]->render();
		}
	}
}
void aButton::press(bool bPress)
{
	if (!isEnabled())
		return;
	if (!bPress && state == HIGHLIGHT)
		return;
	state = bPress ? PRESSED : ENABLED;
	makeUVs(location, state, data);
}

void aButton::makeAmbiguous(bool bAmbiguous)
{
	state = bAmbiguous ? AMBIGUOUS : ENABLED;
	makeUVs(location, state, data);
}

void aButton::disable(bool bDisable)
{
	if (!bDisable)
	{
		if (state == DISABLED)
			state = ENABLED;
	}
	else
		state = DISABLED;
	makeUVs(location, state, data);
}

void aButton::hide(bool bHide)
{
	state = bHide ? HIDDEN : ENABLED;
	if (state != HIDDEN)
		aButton::makeUVs(location, state, data);
}

bool aButton::isEnabled()
{
	return state == ENABLED || state == PRESSED || state == AMBIGUOUS || state == HIGHLIGHT;
}

int32_t
aButton::getID()
{
	return data.ID;
}

void aButton::setID(int32_t newID)
{
	data.ID = newID;
}

void aButton::makeUVs(gos_VERTEX* vertices, int32_t State, aButton::aButtonData& data)
{
	float left = data.stateCoords[State][0];
	float top = data.stateCoords[State][1];
	if (left == -1 || top == -1)
	{
		SPEW((0, "makeUVs given an Invalid state\n"));
	}
	float width = data.texturewidth;
	float height = data.textureheight;
	float right = left + width;
	float bottom = top + height;
	if (data.filewidth && data.fileheight) // will crash if 0
	{
		if (data.textureRotated)
		{
			vertices[0].u = right / (float)data.filewidth + (.1f / (float)data.filewidth);
			;
			vertices[1].u = left / (float)data.filewidth + (.1f / (float)data.filewidth);
			;
			vertices[2].u = left / (float)data.filewidth + (.1f / (float)data.filewidth);
			vertices[3].u = right / (float)data.filewidth + (.1f / (float)data.filewidth);
			vertices[0].v = top / (float)data.fileheight + (.1f / (float)data.filewidth);
			;
			vertices[1].v = top / (float)data.fileheight + (.1f / (float)data.filewidth);
			;
			vertices[2].v = bottom / (float)data.fileheight + (.1f / (float)data.fileheight);
			;
			vertices[3].v = bottom / (float)data.fileheight + (.1f / (float)data.fileheight);
			;
		}
		else
		{
			{
				vertices[0].u = vertices[1].u =
					left / (float)data.filewidth + (.1f / (float)data.filewidth);
				;
				vertices[2].u = vertices[3].u =
					right / (float)data.filewidth + (.1f / (float)data.filewidth);
				vertices[0].v = vertices[3].v =
					top / (float)data.fileheight + (.1f / (float)data.filewidth);
				;
				vertices[1].v = vertices[2].v =
					bottom / (float)data.fileheight + (.1f / (float)data.fileheight);
			}
		}
	}
}

void aButton::init(FitIniFile& buttonFile, std::wstring_view str, HGOSFONT3D font)
{
	textureHandle = 0;
	int32_t result = buttonFile.seekBlock(str);
	if (result != NO_ERROR)
	{
		wchar_t errorStr[256];
		sprintf(errorStr, "couldn't find button %s", str);
		Assert(0, 0, errorStr);
		return;
	}
	buttonFile.readIdLong("ID", data.ID);
	buttonFile.readIdString("filename", data.fileName, 32);
	buttonFile.readIdLong("HelpCaption", helpHeader);
	buttonFile.readIdLong("HelpDesc", helpid);
	buttonFile.readIdLong("TextID", data.textID);
	buttonFile.readIdLong("TextNormal", data.textcolours[0]);
	buttonFile.readIdLong("TextPressed", data.textcolours[1]);
	buttonFile.readIdLong("TextDisabled", data.textcolours[2]);
	buttonFile.readIdBoolean("Toggle", toggleButton);
	buttonFile.readIdBoolean("outline", data.outline);
	int32_t fontID;
	buttonFile.readIdLong("Font", fontID);
	if (fontID)
		data.textFont = aFont::loadFont(fontID, data.textSize);
	else
		data.textFont = 0;
	int32_t x, y, width, height;
	buttonFile.readIdLong("XLocation", x);
	buttonFile.readIdLong("YLocation", y);
	buttonFile.readIdLong("width", width);
	buttonFile.readIdLong("height", height);
	buttonFile.readIdLong("HelpCaption", helpHeader);
	buttonFile.readIdLong("HelpDesc", helpid);
	buttonFile.readIdBoolean("texturesRotated", data.textureRotated);
	if (NO_ERROR != buttonFile.readIdLong("Alignment", data.textAlign))
		data.textAlign = 2;
	location[0].x = location[1].x = x;
	location[0].y = location[3].y = y;
	location[2].x = location[3].x = x + width;
	location[1].y = location[2].y = y + height;
	for (size_t j = 0; j < 4; j++)
	{
		location[j].argb = 0xffffffff;
		location[j].frgb = 0;
		location[j].rhw = .5;
		location[j].u = 0.f;
		location[j].v = 0.f;
		location[j].z = 0.f;
	}
	if (0 == textureHandle && data.fileName && strlen(data.fileName))
	{
		wchar_t file[256];
		strcpy(file, artPath);
		strcat(file, data.fileName);
		_strlwr(file);
		if (!strstr(data.fileName, ".tga"))
			strcat(file, ".tga");
		int32_t ID = mcTextureManager->loadTexture(file, gos_Texture_Alpha, 0, 0, 0x2);
		int32_t gosID = mcTextureManager->get_gosTextureHandle(ID);
		TEXTUREPTR textureData;
		gos_LockTexture(gosID, 0, 0, &textureData);
		gos_UnLockTexture(gosID);
		textureHandle = ID;
		data.filewidth = textureData.width;
		data.fileheight = data.filewidth;
	}
	if (NO_ERROR != buttonFile.readIdLong("UNormal", data.stateCoords[0][0]))
		data.stateCoords[0][0] = -1.f;
	if (NO_ERROR != buttonFile.readIdLong("VNormal", data.stateCoords[0][1]))
		data.stateCoords[0][1] = -1.f;
	if (NO_ERROR != buttonFile.readIdLong("UPressed", data.stateCoords[1][0]))
		data.stateCoords[1][0] = -1.f;
	if (NO_ERROR != buttonFile.readIdLong("VPressed", data.stateCoords[1][1]))
		data.stateCoords[1][1] = -1.f;
	if (NO_ERROR != buttonFile.readIdLong("UDisabled", data.stateCoords[2][0]))
		data.stateCoords[2][0] = -1.f;
	if (NO_ERROR != buttonFile.readIdLong("VDisabled", data.stateCoords[2][1]))
		data.stateCoords[2][1] = -1.f;
	if (NO_ERROR != buttonFile.readIdLong("UAmbiguous", data.stateCoords[3][0]))
		data.stateCoords[3][0] = -1.f;
	if (NO_ERROR != buttonFile.readIdLong("VAmbiguous", data.stateCoords[3][1]))
		data.stateCoords[3][1] = -1.f;
	if (NO_ERROR != buttonFile.readIdLong("UHighlight", data.stateCoords[4][0]))
	{
		data.stateCoords[4][0] = data.stateCoords[0][0];
	}
	if (NO_ERROR != buttonFile.readIdLong("VHighlight", data.stateCoords[4][1]))
	{
		data.stateCoords[4][1] = data.stateCoords[0][1];
	}
	buttonFile.readIdLong("Uwidth", data.texturewidth);
	buttonFile.readIdLong("Vheight", data.textureheight);
	if (data.textID)
		buttonFile.readIdBoolean("TextOutline", data.outlineText);
	if (NO_ERROR == buttonFile.readIdLong("XTextLocation", data.textRect.left))
	{
		buttonFile.readIdLong("YTextLocation", data.textRect.top);
		buttonFile.readIdLong("Textwidth", width);
		buttonFile.readIdLong("Textheight", height);
		data.textRect.right = data.textRect.left + width;
		data.textRect.bottom = data.textRect.top + height;
		buttonFile.readIdBoolean("TextOutline", data.outlineText);
	}
	else
	{
		data.textRect.left = x;
		data.textRect.right = x + width;
		data.textRect.top = y;
		data.textRect.bottom = y + height;
	}
	wchar_t bmpName[256];
	strcpy(bmpName, str);
	strcat(bmpName, "Bmp");
	wchar_t finalName[256];
	int32_t counter = 0;
	while (true)
	{
		sprintf(finalName, "%s%ld", bmpName, counter);
		if (NO_ERROR != buttonFile.seekBlock(finalName))
			break;
		aObject* pObject = new aObject;
		pObject->init(&buttonFile, finalName);
		// Dorje is doing this in global coords
		pObject->move(-globalX(), -globalY());
		addChild(pObject);
		counter++;
	}
	buttonFile.seekBlock(str);
	disable(0);
	press(0);
}

aAnimButton::aAnimButton()
{
	animateText = 1;
	animateBmp = 1;
	bAnimateChildren = 1;
}

aAnimButton&
aAnimButton::operator=(const aAnimButton& src)
{
	copyData(src);
	aButton::operator=(src);
	return *this;
}
aAnimButton::aAnimButton(const aAnimButton& src)
	: aButton(src)
{
	copyData(src);
}

void aAnimButton::copyData(const aAnimButton& src)
{
	if (&src != this)
	{
		animateBmp = src.animateBmp;
		animateText = src.animateText;
		highlightData = src.highlightData;
		disabledData = src.disabledData;
		pressedData = src.pressedData;
		normalData = src.normalData;
		toggleButton = src.toggleButton;
		bAnimateChildren = src.bAnimateChildren;
	}
}
void aAnimButton::destroy()
{
	normalData.destroy();
	highlightData.destroy();
	pressedData.destroy();
	disabledData.destroy();
	aButton::destroy();
}

void aAnimButton::init(FitIniFile& file, std::wstring_view headerName, HGOSFONT3D font)
{
	if (NO_ERROR != file.seekBlock(headerName))
	{
		wchar_t errorStr[256];
		sprintf(errorStr, "couldn't find block %s in file %s", headerName, file.getFilename());
		Assert(0, 0, errorStr);
		animateBmp = 0;
		animateText = 0;
		return;
	}
	aButton::init(file, headerName, font);
	normalData.init(&file, "Normal");
	pressedData.init(&file, "Pressed");
	highlightData.init(&file, "Highlight");
	disabledData.init(&file, "Disabled");
	normalData.begin();
	if (NO_ERROR != file.readIdBoolean("AnimateBmp", animateBmp))
		animateBmp = 1;
	if (NO_ERROR != file.readIdBoolean("AnimateText", animateText))
		animateText = 1;
	bool bTmp = 0;
	if (NO_ERROR == file.readIdBoolean("AnimateChildren", bTmp))
	{
		bAnimateChildren = bTmp;
	}
}

void aAnimButton::update()
{
	if (!isShowing())
		return;
	int32_t mouseX = userInput->getMouseX();
	int32_t mouseY = userInput->getMouseY();
	bool bInside = pointInside(mouseX, mouseY);
	if (bInside && state == DISABLED)
	{
		::helpTextID = this->helpid;
		::helpTextHeaderID = this->helpHeader;
	}
	if (bInside && state != DISABLED && state != HIDDEN)
	{
		::helpTextID = this->helpid;
		::helpTextHeaderID = this->helpHeader;
		if (userInput->isLeftClick())
		{
			if (toggleButton)
			{
				if (state == PRESSED)
				{
					press(0);
					pressedData.end();
				}
				else
				{
					pressedData.begin();
					highlightData.end();
					press(1);
				}
				sndSystem->playDigitalSample(clickSFX);
			}
			else
			{
				press(true);
				pressedData.begin();
				highlightData.end();
				sndSystem->playDigitalSample(clickSFX);
			}
			if (getParent() && !messageOnRelease && pointInside(userInput->getMouseDragX(), userInput->getMouseDragY()))
				getParent()->handleMessage(aMSG_LEFTMOUSEDOWN, data.ID);
		}
		else if (userInput->getMouseLeftHeld() > holdTime && getParent() && !messageOnRelease && pointInside(userInput->getMouseDragX(), userInput->getMouseDragY()))
			getParent()->handleMessage(aMSG_LEFTMOUSEHELD, data.ID);
		else if (userInput->leftMouseReleased() && getParent() && messageOnRelease)
		{
			int32_t mouseDragX = userInput->getMouseDragX();
			int32_t mouseDragY = userInput->getMouseDragY();
			if (pointInside(mouseDragX, mouseDragY))
			{
				getParent()->handleMessage(aMSG_LEFTMOUSEDOWN, data.ID);
				if (!toggleButton)
					state = ENABLED;
			}
			//		sndSystem->playDigitalSample( clickSFX );
		}
		else if (state != PRESSED)
		{
			if (state != HIGHLIGHT && state != DISABLED && state != HIDDEN && isShowing())
				sndSystem->playDigitalSample(highlightSFX);
			if (!highlightData.isAnimating())
				highlightData.begin();
			state = HIGHLIGHT;
			makeUVs(location, state, data);
		}
	}
	else if (state == PRESSED)
	{
		// if clicked inside and release outside
		if (userInput->leftMouseReleased() && (messageOnRelease || singlePress) && pointInside(userInput->getMouseDragX(), userInput->getMouseDragY()))
		{
			state = ENABLED;
		}
		else
		{
			if (!pressedData.isAnimating())
				pressedData.begin();
			if (singlePress && pressedData.isDone())
				press(0);
		}
	}
	else if (state == DISABLED)
	{
		if (pointInside(mouseX, mouseY) && userInput->isLeftClick())
		{
			sndSystem->playDigitalSample(disabledSFX);
		}
		if (!disabledData.isAnimating())
			disabledData.begin();
		disabledData.update();
	}
	else
	{
		highlightData.end();
		if (state == HIGHLIGHT)
			state = ENABLED;
		press(0);
	}
	if (pressedData.isAnimating() && state != PRESSED)
	{
		pressedData.end();
		press(0);
	}
	if (state != DISABLED)
		disabledData.end();
	highlightData.update();
	pressedData.update();
	normalData.update();
}
void aAnimButton::render()
{
	if (!isShowing())
		return;
	if (disabledData.isAnimating())
	{
		update(disabledData);
	}
	else if (pressedData.isAnimating())
	{
		update(pressedData);
	}
	else if (highlightData.isAnimating())
	{
		update(highlightData);
	}
	else
	{
		update(normalData);
	}
}

void aAnimButton::update(const aAnimation& animData)
{
	if (!isShowing())
		return;
	int32_t color = animData.getcolour();
	if (animateBmp)
		setcolour(color, 0);
	if (animateText)
		data.textcolours[state] = color;
	int32_t xPos = animData.getXDelta();
	int32_t yPos = animData.getYDelta();
	move(xPos, yPos);
	if (bAnimateChildren)
	{
		for (size_t i = 0; i < numberOfChildren(); i++)
			pChildren[i]->setcolour(color);
	}
	float fXcaleX = animData.getScaleX();
	float fScaleY = animData.getScaleX();
	if (fXcaleX != 1.0 && fScaleY != 1.0)
	{
		float oldwidth = width();
		float oldheight = height();
		float oldLeft = globalX();
		float oldTop = globalY();
		float scaleX = .5 * fXcaleX * width();
		float scaleY = .5 * fScaleY * height();
		float midX = globalX() + .5 * width();
		float midY = globalY() + .5 * height();
		float newLeft = midX - scaleX;
		float newTop = midY - scaleY;
		moveToNoRecurse(newLeft, newTop);
		resize(width() * scaleX, height() * scaleY);
		aButton::render();
		resize(oldwidth, oldheight);
		moveToNoRecurse(oldLeft, oldTop);
	}
	else
		aButton::render();
}

void aButton::move(float offsetX, float offsetY)
{
	aObject::move(offsetX, offsetY);
	data.textRect.left += offsetX;
	data.textRect.right += offsetX;
	data.textRect.top += offsetY;
	data.textRect.bottom += offsetY;
}

void aAnimButton::setAnimationInfo(
	aAnimation* normal, aAnimation* highlight, aAnimation* pressed, aAnimation* disabled)
{
	if (normal)
	{
		normalData.destroy();
		normalData = *normal;
	}
	else
		normalData.destroy();
	if (highlight)
	{
		highlightData.destroy();
		highlightData = *highlight;
	}
	else
		highlightData.destroy();
	if (pressed)
	{
		pressedData.destroy();
		pressedData = *pressed;
	}
	else
		pressedData.destroy();
	if (disabled)
	{
		disabledData.destroy();
		disabledData = *disabled;
	}
	else
		disabledData.destroy();
}
