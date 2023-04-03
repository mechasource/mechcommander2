#define MPSETUP_CPP
/*************************************************************************************************\
MPSetupArea.cpp			: Implementation of the MPSetupArea component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdinc.h"

#include "MPSetupArea.h"
#include "prefs.h"
#include "IniFile.h"
#include "userinput.h"
#include "resource.h"

#include "gamesound.h"
#define CHECK_BUTTON 200

static int32_t connectionType = 0;

static const int32_t FIRST_BUTTON_ID = 1000010;
static const int32_t OK_BUTTON_ID = 1000001;
static const int32_t CANCEL_BUTTON_ID = 1000002;

MPSetupXScreen::MPSetupXScreen()
{
	bDone = 0;
	basecolour = 0xffff0000;
	stripecolour = 0xff00ff00;
	bPaintSchemeInitialized = false;
	ppConnectionScreen = 0;
	pLocalBrowserScreen = 0;
	pDirectTcpipScreen = 0;
	pModem2ModemScreen = 0;
	status = RUNNING;
}

MPSetupXScreen::~MPSetupXScreen() { }

int32_t
MPSetupXScreen::indexOfButtonWithID(int32_t id)
{
	int32_t i;
	for (i = 0; i < buttonCount; i++)
	{
		if (buttons[i].getID() == id)
		{
			return i;
		}
	}
	return -1;
}

void MPSetupXScreen::init(FitIniFile* file)
{
	LogisticsScreen::init(*file, "Static", "Text", "Rect", "Button");
	if (buttonCount)
	{
		for (size_t i = 0; i < buttonCount; i++)
		{
			buttons[i].setMessageOnRelease();
			if (buttons[i].getID() == 0)
			{
				buttons[i].setID(FIRST_BUTTON_ID + i);
			}
		}
	}
	{
		wchar_t path[256];
		strcpy(path, artPath);
		strcat(path, "mcl_mp_setup_combobox0.fit");
		FitIniFile PNfile;
		if (NO_ERROR != PNfile.open(path))
		{
			wchar_t error[256];
			sprintf(error, "couldn't open file %s", path);
			Assert(0, 0, error);
			return;
		}
		playerNameComboBox.init(&PNfile, "PlayerNameComboBox");
		aStyle1TextListItem* pTmp2 = new aStyle1TextListItem;
		pTmp2->init(&PNfile, "Text0");
		playerNameComboBox.AddItem(pTmp2);
		pTmp2 = new aStyle1TextListItem;
		pTmp2->init(&PNfile, "Text1");
		playerNameComboBox.AddItem(pTmp2);
	}
	{
		wchar_t path[256];
		strcpy(path, artPath);
		strcat(path, "mcl_mp_setup_combobox1.fit");
		FitIniFile PNfile;
		if (NO_ERROR != PNfile.open(path))
		{
			wchar_t error[256];
			sprintf(error, "couldn't open file %s", path);
			Assert(0, 0, error);
			return;
		}
		unitNameComboBox.init(&PNfile, "UnitNameComboBox");
	}
	{
		wchar_t path[256];
		strcpy(path, artPath);
		strcat(path, "mcl_mp_setup_droplist0.fit");
		FitIniFile PNfile;
		if (NO_ERROR != PNfile.open(path))
		{
			wchar_t error[256];
			sprintf(error, "couldn't open file %s", path);
			Assert(0, 0, error);
			return;
		}
		insigniaDropList.init(&PNfile, "InsigniaDropList");
		PNfile.seekBlock("ListItems");
		int32_t listItemCount = 0;
		PNfile.readIdLong("ListItemCount", listItemCount);
		aInsigniaListItem* pTmp2;
		int32_t i;
		for (i = 0; i < listItemCount; i += 1)
		{
			pTmp2 = new aInsigniaListItem;
			std::wstring_view tmpStr;
			tmpStr.Format("ListItem%d", i);
			pTmp2->init(&PNfile, tmpStr.Data());
			insigniaDropList.AddItem(pTmp2);
		}
		insigniaDropList.SelectItem(0);
	}
	colorPicker.init(file, "colourPicker");
	mechCamera.init(337, 297, 464, 512);
	/* these string are to be moved to the string table */
	if (1 <= textCount)
	{
		std::wstring_view str = "MULTIPLAYER SETUP";
		textObjects[0].setText(str);
	}
	if (2 <= textCount)
	{
		std::wstring_view str = "PLAYER NAME";
		textObjects[1].setText(str);
	}
	if (3 <= textCount)
	{
		std::wstring_view str = "UNIT NAME";
		textObjects[2].setText(str);
	}
	if (4 <= textCount)
	{
		std::wstring_view str = "UNIT INSIGNIA";
		textObjects[3].setText(str);
	}
	if (5 <= textCount)
	{
		std::wstring_view str = "PLAYER COLORS";
		textObjects[4].setText(str);
	}
	if (6 <= textCount)
	{
		std::wstring_view str = "HELP TEXT";
		textObjects[5].setText(str);
	}
	if (7 <= textCount)
	{
		std::wstring_view str = "CONNECTION TYPE";
		textObjects[6].setText(str);
	}
	if (8 <= textCount)
	{
		std::wstring_view str = "connection type info";
		textObjects[7].setText(str);
	}
	if (9 <= textCount)
	{
		std::wstring_view str = "PLAYER NAME...";
		textObjects[8].setText(str);
	}
	if (10 <= textCount)
	{
		std::wstring_view str = "UNIT NAME...";
		textObjects[9].setText(str);
	}
	if (11 <= textCount)
	{
		std::wstring_view str = "INSIGNIA NAME...";
		textObjects[10].setText(str);
	}
	unitNameComboBox.setFocus(false);
	playerNameComboBox.setFocus(false);
}

void MPSetupXScreen::begin()
{
	status = RUNNING;
	connectionType = 0;
	/* set button states */
	int32_t buttonIndex;
	int32_t buttonIndex2;
	int32_t buttonIndex3;
	int32_t buttonIndex4;
	buttonIndex = indexOfButtonWithID(FIRST_BUTTON_ID + 2);
	buttonIndex2 = indexOfButtonWithID(FIRST_BUTTON_ID + 3);
	buttonIndex3 = indexOfButtonWithID(FIRST_BUTTON_ID + 4);
	buttonIndex4 = indexOfButtonWithID(FIRST_BUTTON_ID + 5);
	if ((0 <= buttonIndex) && (0 <= buttonIndex2) && (0 <= buttonIndex3) && (0 <= buttonIndex4))
	{
		buttons[buttonIndex].press(
			!((1 == connectionType) || (2 == connectionType) || (3 == connectionType)));
		buttons[buttonIndex2].press(1 == connectionType);
		buttons[buttonIndex3].press(2 == connectionType);
		buttons[buttonIndex4].press(3 == connectionType);
	}
	colorPicker.setcolour0(basecolour);
	colorPicker.setcolour1(stripecolour);
	mechCamera.setMech("hunchback");
	bPaintSchemeInitialized = false;
	// mechCamera.getObjectAppearance()->resetPaintScheme(basecolour,
	// stripecolour, 0xfff0f0f0);
}

void MPSetupXScreen::end()
{
	mechCamera.setMech(nullptr);
}

void MPSetupXScreen::render(int32_t xOffset, int32_t yOffset)
{
	LogisticsScreen::render(xOffset, yOffset);
	if ((0 == xOffset) && (0 == yOffset))
	{
		colorPicker.render();
		mechCamera.render();
		if (!bPaintSchemeInitialized)
		{
			bPaintSchemeInitialized = true;
			mechCamera.getObjectAppearance()->resetPaintScheme(basecolour, stripecolour, 0xfff0f0f0);
		}
		/*combo boxes need to be rendered after anything they might obscure*/
		insigniaDropList.render();
		unitNameComboBox.render();
		playerNameComboBox.render();
	}
}

void MPSetupXScreen::render()
{
	render(0, 0);
}

int32_t
MPSetupXScreen::handleMessage(uint32_t message, uint32_t who)
{
	if (RUNNING == status)
	{
		switch (who)
		{
		case 50 /*MB_MSG_NEXT*/:
		{
			switch (connectionType)
			{
			case 0 /*internet*/:
			{
				(*ppConnectionScreen) = pLocalBrowserScreen;
			}
			break;
			case 1 /*local browser*/:
			{
				(*ppConnectionScreen) = pLocalBrowserScreen;
			}
			break;
			case 2 /*direct tcp/ip*/:
			{
				(*ppConnectionScreen) = pDirectTcpipScreen;
			}
			break;
			case 3 /*modem2modem*/:
			{
				(*ppConnectionScreen) = pLocalBrowserScreen;
			}
			break;
			}
			getButton(50 /*MB_MSG_NEXT*/)->press(0);
			status = NEXT;
		}
		break;
		case 57 /*MB_MSG_MAINMENU*/:
		{
			getButton(57 /*MB_MSG_MAINMENU*/)->press(0);
			status = MAINMENU;
		}
		break;
		case FIRST_BUTTON_ID + 2:
		{
			getButton(FIRST_BUTTON_ID + 2)->press(0);
			connectionType = 0;
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 2)].press(
				!((1 == connectionType) || (2 == connectionType) || (3 == connectionType)));
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 3)].press(1 == connectionType);
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 4)].press(2 == connectionType);
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 5)].press(3 == connectionType);
			return 1;
		}
		break;
		case FIRST_BUTTON_ID + 3:
		{
			connectionType = 1;
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 2)].press(
				!((1 == connectionType) || (2 == connectionType) || (3 == connectionType)));
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 3)].press(1 == connectionType);
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 4)].press(2 == connectionType);
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 5)].press(3 == connectionType);
			return 1;
		}
		break;
		case FIRST_BUTTON_ID + 4:
		{
			connectionType = 2;
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 2)].press(
				!((1 == connectionType) || (2 == connectionType) || (3 == connectionType)));
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 3)].press(1 == connectionType);
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 4)].press(2 == connectionType);
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 5)].press(3 == connectionType);
			return 1;
		}
		break;
		case FIRST_BUTTON_ID + 5:
		{
			connectionType = 3;
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 2)].press(
				!((1 == connectionType) || (2 == connectionType) || (3 == connectionType)));
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 3)].press(1 == connectionType);
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 4)].press(2 == connectionType);
			buttons[indexOfButtonWithID(FIRST_BUTTON_ID + 5)].press(3 == connectionType);
			return 1;
		}
		break;
		}
	}
	return 0;
}

bool MPSetupXScreen::isDone()
{
	return bDone;
}

void MPSetupXScreen::update()
{
	LogisticsScreen::update();
	helpTextID = 0;
	helpTextHeaderID = 0;
	/*
	for ( int32_t i = 0; i < buttonCount; i++ )
	{
		buttons[i].update();
		if ( buttons[i].pointInside( userInput->getMouseX(),
	userInput->getMouseY() )
			&& userInput->isLeftClick() )
		{
			handleMessage( buttons[i].getID(), buttons[i].getID() );
		}

	}
	*/
	insigniaDropList.update();
	unitNameComboBox.update();
	playerNameComboBox.update();
	colorPicker.update();
	if ((colorPicker.getcolour0() != basecolour) || (colorPicker.getcolour1() != stripecolour))
	{
		basecolour = colorPicker.getcolour0();
		stripecolour = colorPicker.getcolour1();
		mechCamera.getObjectAppearance()->resetPaintScheme(basecolour, stripecolour, 0xfff0f0f0);
	}
	mechCamera.update();
	if (userInput->isLeftClick())
	{
		int32_t mx = userInput->getMouseX();
		int32_t my = userInput->getMouseY();
		if (playerNameComboBox.EditBox().pointInside(mx, my))
		{
			playerNameComboBox.EditBox().setFocus(true);
			unitNameComboBox.EditBox().setFocus(false);
		}
		else if (unitNameComboBox.EditBox().pointInside(mx, my))
		{
			playerNameComboBox.EditBox().setFocus(false);
			unitNameComboBox.EditBox().setFocus(true);
		}
	}
}

void MPSetupXScreen::updateMPSetup()
{
}

acolourPicker::acolourPicker()
{
	color0 = 0xffffffff;
	color1 = 0xff000000;
	activeTab = 0;
}

int32_t
acolourPicker::init(int32_t xPos, int32_t yPos, int32_t w, int32_t h)
{
	int32_t err;
	err = aObject::init(xPos, yPos, w, h);
	if (err)
		return err;
	addChild(&mainRect);
	addChild(&colorPlaneStatic);
	addChild(&intensityGradientRect);
	addChild(&intesitySliderScrollBar);
	addChild(&colorPlaneCursorStatic);
	addChild(&tab0Button);
	addChild(&tab0colourRect);
	addChild(&tab0colourOutlineRect);
	addChild(&tab0text);
	addChild(&tab1Button);
	addChild(&tab1colourRect);
	addChild(&tab1colourOutlineRect);
	addChild(&tab1text);
	return (NO_ERROR);
}

void acolourPicker::init(FitIniFile* file, std::wstring_view blockname)
{
	file->seekBlock(blockname);
	int32_t x, y, width, height;
	file->readIdLong("XLocation", x);
	file->readIdLong("YLocation", y);
	file->readIdLong("width", width);
	file->readIdLong("height", height);
	init(x, y, width, height);
	std::wstring_view blockname;
	blockname = "colourPickerMainRect";
	mainRect.init(file, blockname.Data());
	file->seekBlock("colourPickerTab0");
	blockname = "Tab0Text";
	tab0text.init(file, blockname.Data());
	{
		std::wstring_view str = "BASE COLOR";
		tab0text.setText(str);
	}
	activeTab = 0;
	blockname = "Tab0colourOutlineRect";
	tab0colourOutlineRect.init(file, blockname.Data());
	blockname = "Tab0colourRect";
	tab0colourRect.init(file, blockname.Data());
	tab0colourRect.setcolour(color0);
	blockname = "Tab0Button";
	tab0Button.init(*file, blockname.Data());
	tab0Button.press(true);
	file->seekBlock("colourPickerTab1");
	blockname = "Tab1Text";
	tab1text.init(file, blockname.Data());
	{
		std::wstring_view str = "STRIPE COLOR";
		tab1text.setText(str);
	}
	blockname = "Tab1colourOutlineRect";
	tab1colourOutlineRect.init(file, blockname.Data());
	blockname = "Tab1colourRect";
	tab1colourRect.init(file, blockname.Data());
	tab1colourRect.setcolour(color1);
	blockname = "Tab1Button";
	tab1Button.init(*file, blockname.Data());
	tab1Button.press(false);
	blockname = "colourPickerCircleStatic";
	colorPlaneStatic.init(file, blockname.Data());
	blockname = "colourPickerValueGradient";
	intensityGradientRect.init(file, blockname.Data());
	{
		blockname = "GradientSlider";
		int32_t x, y, w, h;
		file->readIdLong("XLocation", x);
		file->readIdLong("YLocation", y);
		file->readIdLong("width", w);
		file->readIdLong("height", h);
		intesitySliderScrollBar.init(x, y, w, h);
	}
	blockname = "HueCirclePicker";
	colorPlaneCursorStatic.init(file, blockname.Data());
}

void acolourPicker::destroy()
{
	aObject::destroy();
}

static const float rhatx = 1.0; // cos(0/*degrees*/ * DEGREES_TO_RADS);
static const float rhaty = 0.0; // sin(0/*degrees*/ * DEGREES_TO_RADS);
static const float ghatx = -0.5; // cos(120/*degrees*/ * DEGREES_TO_RADS);
static const float ghaty = 0.86602540378443864676372317075294f; // sin(120/*degrees*/ *
	// DEGREES_TO_RADS);
static const float bhatx = -0.5; // cos(240/*degrees*/ * DEGREES_TO_RADS);
static const float bhaty = -0.86602540378443864676372317075294f; // sin(240/*degrees*/ *
	// DEGREES_TO_RADS);
static const float two_pi = 6.283185307179586476925286766559f;

/* all params range from 0.0 to 1.0 */
static void
rgb2hsi(float r, float g, float b, float& hue, float& saturation, float& intensity)
{
	intensity = (r + g + b) / 3.0;
	if (0.0 >= intensity)
	{
		saturation = 0.0;
		hue = 0.0;
		return;
	}
	float min = r;
	if (g < min)
	{
		min = g;
	}
	if (b < min)
	{
		min = b;
	}
	saturation = 1.0 - (min / intensity);
	float cx = r * rhatx + g * ghatx + b * bhatx;
	float cy = r * rhaty + g * ghaty + b * bhaty;
	hue = atan2(cy, cx);
	if (0.0 > hue)
	{
		hue += two_pi;
	}
	hue /= two_pi;
}

static void
hsi2rgb(float hue, float saturation, float intensity, float& r, float& g, float& b)
{
	float thue = hue;
	if ((1.0f / 3.0f) > hue)
	{
	}
	else if ((2.0f / 3.0f) > hue)
	{
		thue -= 1.0f / 3.0f;
	}
	else
	{
		thue -= 2.0f / 3.0f;
	}
	float chatx = cos(two_pi * thue);
	float chaty = sin(two_pi * thue);
	float ti = 3.0f * intensity;
	{
		b = (1.0f - saturation) * intensity;
		float tib = ti - b;
		float denominator = (chaty * (ghatx - rhatx) - chatx * (ghaty - rhaty));
		if (0.0f == denominator)
		{
			/* I suspect it's mathematically impossible to get here, but I'm too
			 * lazy to make sure. */
			denominator = 0.000001f;
		}
		g = ((b * bhaty + tib * rhaty) * chatx - (b * bhatx + tib * rhatx) * chaty) / denominator;
		r = tib - g;
	}
	if ((1.0f / 3.0f) > hue)
	{
	}
	else if ((2.0f / 3.0f) > hue)
	{
		float swap = b;
		b = g;
		g = r;
		r = swap;
	}
	else
	{
		float swap = b;
		b = r;
		r = g;
		g = swap;
	}
}

void acolourPicker::update()
{
	aObject::update();
	if (userInput->isLeftClick() || userInput->isLeftDrag())
	{
		int32_t cx = userInput->getMouseX();
		int32_t cy = userInput->getMouseY();
		if (userInput->isLeftClick())
		{
			if (tab0Button.pointInside(cx, cy))
			{
				handleMessage(aMSG_BUTTONCLICKED, (uint32_t)(&tab0Button));
			}
			else if (tab1Button.pointInside(cx, cy))
			{
				handleMessage(aMSG_BUTTONCLICKED, (uint32_t)(&tab1Button));
			}
		}
		if (colorPlaneStatic.pointInside(cx, cy))
		{
			handleMessage(aMSG_LEFTMOUSEDOWN, (uint32_t)(&colorPlaneStatic));
		}
	}
}

int32_t
acolourPicker::handleMessage(uint32_t message, uint32_t who)
{
	{
		if ((uint32_t)(&tab0Button) == who)
		{
			if (aMSG_BUTTONCLICKED == message)
			{
				tab0Button.press(true);
				tab1Button.press(false);
				activeTab = 0;
				return 1;
			}
		}
		else if ((uint32_t)(&tab1Button) == who)
		{
			if (aMSG_BUTTONCLICKED == message)
			{
				tab0Button.press(false);
				tab1Button.press(true);
				activeTab = 1;
				return 1;
			}
		}
		else if ((uint32_t)(&colorPlaneStatic) == who)
		{
			if ((aMSG_LEFTMOUSEDOWN == message) || ((aMSG_MOUSEMOVE == message) && (userInput->isLeftDrag())))
			{
				int32_t cx = userInput->getMouseX();
				int32_t cy = userInput->getMouseY();
				int32_t colorPlaneRadius = 0.5 * colorPlaneStatic.width();
				int32_t colorPlaneCenterX = colorPlaneStatic.x() + colorPlaneRadius;
				int32_t colorPlaneCenterY = colorPlaneStatic.y() + colorPlaneRadius;
				float dx = cx - colorPlaneCenterX;
				float dy = cy - colorPlaneCenterY;
				int32_t d2 = dx * dx + dy * dy;
				if ((colorPlaneRadius * colorPlaneRadius) > d2)
				{
					float saturation = sqrt((float)d2) / (float)colorPlaneRadius;
					float hue = (atan2(-dy, dx) / two_pi) + 0.5;
					float intensity = 0.99f;
					float R, G, B;
					hsi2rgb(hue, saturation, intensity, R, G, B);
					if (R > 0.99f)
					{
						R = 0.99f;
					}
					if (G > 0.99f)
					{
						G = 0.99f;
					}
					if (B > 0.99f)
					{
						B = 0.99f;
					}
					if (R < 0.0f)
					{
						R = 0.0f;
					}
					if (G < 0.0f)
					{
						G = 0.0f;
					}
					if (B < 0.0f)
					{
						B = 0.0f;
					}
					int32_t newcolour = 0xff000000 | (((int32_t)(R * 255.0)) << 16) | (((int32_t)(G * 255.0)) << 8) | (((int32_t)(B * 255.0)) << 0);
					// if (tab0Button.isPressed()) {
					if (0 == activeTab)
					{
						setcolour0(newcolour);
					}
					else
					{
						setcolour1(newcolour);
					}
				}
				return 1;
			}
		}
	}
	return 0;
}

void acolourPicker::render()
{
	aObject::render();
}

void acolourPicker::move(float offsetX, float offsetY)
{
	aObject::move(offsetX, offsetY);
}

void acolourPicker::setcolour0(int32_t color)
{
	color0 = color;
	tab0colourRect.setcolour(color);
}

void acolourPicker::setcolour1(int32_t color)
{
	color1 = color;
	tab1colourRect.setcolour(color);
}

int32_t
aStyle1TextListItem::init(FitIniFile* file, std::wstring_view blockname)
{
	file->seekBlock(blockname);
	int32_t fontResID = 0;
	file->readIdLong("Font", fontResID);
	int32_t textID = 0;
	file->readIdLong("TextID", textID);
	aTextListItem::init(fontResID);
	setText(textID);
	int32_t color = 0xff808080;
	file->readIdLong("colour", color);
	normalcolour = color;
	setcolour(color);
	wchar_t tmpStr[64];
	strcpy(tmpStr, "");
	file->readIdString("Animation", tmpStr, 63);
	if (0 == strcmp("", tmpStr))
	{
		hasAnimation = false;
	}
	else
	{
		hasAnimation = true;
		animGroup.init(file, tmpStr);
	}
	return 0;
}

void aStyle1TextListItem::render()
{
	float color;
	if (aListItem::SELECTED == getState())
	{
		color = 0.33 * ((uint32_t)normalcolour) + 0.67 * ((uint32_t)0xffffffff);
	}
	else if (aListItem::HIGHLITE == getState())
	{
		color = 0.67 * ((uint32_t)normalcolour) + 0.33 * ((uint32_t)0xffffffff);
	}
	else
	{
		color = normalcolour;
	}
	aTextListItem::setcolour((uint32_t)color);
	aTextListItem::render();
}

int32_t
aInsigniaListItem::init(FitIniFile* file, std::wstring_view blockname)
{
	file->seekBlock(blockname);
	int32_t width, height;
	file->readIdLong("width", width);
	file->readIdLong("height", height);
	std::wstring_view graphicBlockName = blockname;
	graphicBlockName += "Static";
	graphic.init(file, graphicBlockName.Data());
	if (graphic.height() > height)
	{
		height = graphic.height();
	}
	if (graphic.width() > width)
	{
		width = graphic.width();
	}
	std::wstring_view textBlockName = blockname;
	textBlockName += "Text";
	text.init(file, textBlockName.Data());
	if (text.height() > height)
	{
		height = text.height();
	}
	if (graphic.width() + text.width() > width)
	{
		width = graphic.width() + text.width();
	}
	aObject::init(0, 0, width, height);
	addChild(&graphic);
	addChild(&text);
	text.move(graphic.width(), 0);
	return 0;
}

void aInsigniaListItem::update()
{
	text.setState(getState());
	aListItem::update();
}

//////////////////////////////////////////////

// end of file ( MPSetupArea.cpp )
