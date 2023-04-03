#define OPTIONSAREA_CPP
/*************************************************************************************************\
OptionsArea.cpp			: Implementation of the OptionsArea component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdinc.h"

#include "OptionsArea.h"
#include "prefs.h"
#include "IniFile.h"
#include "userinput.h"
#include "resource.h"
#include "prefs.h"
#include "missionGui.h"
#include "logisticsdialog.h"
#include "gameSound.h"
#include "LoadScreen.h"

#include "gamesound.h"
static bool bShadows = true;
static bool bDetailTexture = true;
static int32_t objectDetail = 0;
static int32_t difficulty = 0;
static bool bUnLimitedAmmo = false;
static int32_t DigitalMasterVolume = 255;
static int32_t MusicVolume = 64;
static int32_t sfxVolume = 64;
static int32_t RadioVolume = 64;
static int32_t BettyVolume = 64;
CPrefs prefs;
CPrefs originalSettings;

#include "mission.h"

extern SoundSystem* sndSystem;

#define MSB_TAB0 200
#define MSB_TAB1 201
#define MSB_TAB2 202
#define MSB_TAB3 203

#define MSG_NEAR 101
#define MSG_FAR 102
#define MSG_TAB 312

#define MSG_TERRAIN_DETAIL 301
#define MSG_PILOT_VIDS 302
#define MSG_OBJECT_DETAIL 303
#define MSG_SHADOWS 304
#define MSG_NON_WEAPON 305
#define MSG_LOCAL_SHADOWS 306
#define MSG_ASYNC_MOUSE 307
#define MSG_HARDWARE_RASTERIZER 308
#define MSG_RESET 309

#define MSG_GREEN 400
#define MSG_REGULAR 401
#define MSG_VETERAN 402
#define MSG_ELITE 403

#define MSG_BASE 404
#define MSG_ACCENT 405

#define MSG_UNLIMITED_AMMO 406
#define MSG_LEFT_CLICK 407
#define MSG_SAVE_TRANSCRIPT 408
#define MSG_TUTORIALS 409

#define CTRL 0x10000000
#define SHIFT 0x01000000
#define ALT 0x00100000
#define WAYPT 0x20000000

HotKeyListItem* HotKeyListItem::s_item = nullptr;

OptionsXScreen::OptionsXScreen()
{
	statics = 0;
	rects = 0;
	staticCount = rectCount = buttonCount = textCount = 0;
	buttons = 0;
	textObjects = 0;
	bDone = 0;
	curTab = 0;
	helpTextArrayID = 1;
}

OptionsXScreen::~OptionsXScreen()
{
	for (size_t i = 0; i < 4; i++)
		delete tabAreas[i];
}

int32_t
OptionsXScreen::indexOfButtonWithID(int32_t id)
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

void OptionsXScreen::init(FitIniFile* file)
{
	LogisticsScreen::init(*file, "Static", "Text", "Rect", "Button", "Edit");
	std::wstring_view fileNames[4] = {
		"mcl_optionsgraphics", "mcl_optionsaudio", "mcl_optionsgameplay", "mcl_optionshotkeys"};
	OptionsGraphics* pGraphics = new OptionsGraphics;
	tabAreas[0] = pGraphics;
	OptionsAudio* pAudio = new OptionsAudio;
	tabAreas[1] = pAudio;
	OptionsGamePlay* pPlay = new OptionsGamePlay;
	tabAreas[2] = pPlay;
	OptionsHotKeys* pKeys = new OptionsHotKeys;
	tabAreas[3] = pKeys;
	FullPathFileName path;
	for (size_t i = 0; i < 4; i++)
	{
		path.init(artPath, fileNames[i], ".fit");
		FitIniFile tmpFile;
		if (NO_ERROR != tmpFile.open(path))
		{
			wchar_t error[256];
			sprintf(error, "couldn't open file %s", path);
			Assert(0, 0, error);
			return;
		}
		tabAreas[i]->init(tmpFile, "Static", "Text", "Rect", "Button", "Edit");
		tabAreas[i]->moveTo(rects[2].x(), rects[2].y());
	}
	// 640: offset x = -80, y = -90
	// 800: offset none
	// 1024: offset x = 113, y = 54
	// 1280: offset x = 240, y = 182
	// 1600: offset x = 400, y = 270
	int32_t xOffset = 0;
	int32_t yOffset = 0;
	switch (Environment.screenwidth)
	{
	case 640:
		xOffset = -80;
		yOffset = -90;
		break;
	case 1024:
		xOffset = 13;
		yOffset = 54;
		break;
	case 1280:
		xOffset = 240;
		yOffset = 182;
		break;
	case 1600:
		xOffset = 400;
		yOffset = 270;
		break;
	}
	move(xOffset, yOffset);
	pGraphics->init(xOffset, yOffset);
	pAudio->init(xOffset, yOffset);
	pPlay->init(xOffset, yOffset);
	pKeys->init(xOffset, yOffset);
	// Needs to be 8.3 or it won't go on the CD!!
	originalSettings.load("OrgPrefs");
	prefs.load();
	for (i = 0; i < 4; i++)
		tabAreas[i]->begin();
	if (mission && strlen(mission->getMissionFileName()))
	{
		getButton(MSB_TAB2)->disable(true);
	}
	for (i = 0; i < buttonCount; i++)
	{
		if (MSB_TAB0 > buttons[i].getID() || MSB_TAB3 < buttons[i].getID())
			buttons[i].setMessageOnRelease();
	}
	getButton(MSB_TAB0)->press(true);
	bShowWarning = 0;
}

void OptionsXScreen::render()
{
	RECT rect = {0, 0, Environment.screenwidth, Environment.screenheight};
	drawRect(rect, 0xff000000);
	rects[1].setcolour(0xff000000);
	rects[1].render();
	if (curTab < 2)
		tabAreas[curTab]->render();
	rects[1].setcolour(0);
	LogisticsScreen::render();
	getButton(MSB_TAB0 + curTab)->render();
	if (curTab >= 2)
		tabAreas[curTab]->render();
	if (bShowWarning)
	{
		LogisticsOneButtonDialog::instance()->render();
	}
}

int32_t
OptionsXScreen::handleMessage(uint32_t message, uint32_t who)
{
	if (aMSG_LEFTMOUSEDOWN == message)
	{
		switch (who)
		{
		case MSB_TAB0:
		case MSB_TAB1:
		case MSB_TAB2:
		case MSB_TAB3:
		{
			for (size_t i = MSB_TAB0; i < MSB_TAB3 + 1; i++)
				getButton(i)->press(0);
			getButton(who)->press(true);
			curTab = who - MSB_TAB0;
		}
		break;
		case YES:
		{
			int32_t oldRes = prefs.resolution;
			int32_t oldDepth = prefs.bitDepth;
			for (size_t i = 0; i < 4; i++)
				tabAreas[i]->end();
			prefs.save();
			prefs.applyPrefs(0);
			LoadScreenWrapper::changeRes();
			int32_t newRes = prefs.resolution;
			int32_t newDepth = prefs.bitDepth;
			if (newRes != oldRes || newDepth != oldDepth)
			{
				LogisticsOneButtonDialog::instance()->setText(
					IDS_SWAP_RESOLUTION_WARNING, IDS_DIALOG_OK, IDS_DIALOG_OK);
				LogisticsOneButtonDialog::instance()->begin();
				bShowWarning = true;
			}
			else
				bDone = true;
			return 1;
		}
		break;
		case NO:
		{
			(dynamic_cast<OptionsGamePlay*>(tabAreas[2]))->resetCamera();
			prefs.load();
			prefs.applyPrefs(0);
			bDone = true;
			return 1;
		}
		break;
		}
	}
	return 0;
}

bool OptionsXScreen::isDone()
{
	return bDone;
}

void OptionsXScreen::update()
{
	if (bShowWarning)
	{
		LogisticsOneButtonDialog::instance()->update();
		if (LogisticsOneButtonDialog::instance()->isDone())
			bDone = true;
		return;
	}
	else
	{
		LogisticsScreen::update();
		tabAreas[curTab]->update();
	}
}

void OptionsXScreen::updateOptions()
{
}

//////////////////////////////////////////////
typedef struct
{
	int32_t xRes;
	int32_t yRes;
	int32_t bitDepth;
} ResModes;

ResModes resModes[10] = {640, 480, 16, 640, 480, 32, 800, 600, 16, 800, 600, 32, 1024, 768, 16,
	1024, 768, 32, 1280, 1024, 16, 1280, 1024, 32, 1600, 1200, 16, 1600, 1200, 32};

bool availableMode[10] = {true, true, true, true, true, true, true, true, true, true};

void OptionsGraphics::init(int32_t xOffset, int32_t yOffset)
{
	FullPathFileName path;
	path.init(artPath, "mcl_options_combobox0", ".fit");
	FitIniFile file;
	if (NO_ERROR != file.open(path))
	{
		wchar_t error[256];
		sprintf(error, "couldn't open file %s", path);
		Assert(0, 0, error);
		return;
	}
	getButton(MSG_RESET)->setMessageOnRelease();
	resolutionList.init(&file, "PlayerNameComboBox");
	resolutionList.move(globalX(), globalY());
	resolutionList.move(xOffset, yOffset);
	resolutionList.ListBox().setOrange(true);
	for (size_t i = IDS_RESOLUTION0; i < IDS_RESOLUTION9 + 1; i++)
	{
		if (1 != gos_GetMachineInformation(gos_Info_ValidMode, Environment.FullScreenDevice, resModes[i - IDS_RESOLUTION0].xRes, resModes[i - IDS_RESOLUTION0].yRes, resModes[i - IDS_RESOLUTION0].bitDepth))
		{
			availableMode[i - IDS_RESOLUTION0] = false;
		}
		else
		{
			wchar_t cstr[256];
			cLoadString(i, cstr, 255);
			resolutionList.AddItem(cstr, 0xffffffff);
		}
	}
	file.close();
	path.init(artPath, "mcl_options_combobox2", ".fit");
	if (NO_ERROR != file.open(path))
	{
		wchar_t error[256];
		sprintf(error, "couldn't open file %s", path);
		Assert(0, 0, error);
		return;
	}
	getButton(MSG_RESET)->setMessageOnRelease();
	cardList.init(&file, "PlayerNameComboBox");
	cardList.move(globalX(), globalY());
	cardList.move(xOffset, yOffset);
	cardList.ListBox().setOrange(true);
	uint32_t numDevices = 0;
	numDevices = gos_GetMachineInformation(gos_Info_NumberDevices);
	// Theoretically impossible but config would probably like to know if it
	// happens!
	if (numDevices <= 0)
		STOP(("GameOS said there were no video cards in the system!"));
	int32_t usableCardCount = 0;
	for (i = 0; i < numDevices; i++)
	{
		uint32_t minTextureRam = 6291456;
		// If we are a Voodoo 2, we may be a 4/8 or a 4/4.  Try allowing a 4/4
		// to run
		// and see what happens!!  NO good has come of this!
		/*
		if ((gos_GetMachineInformation(gos_Info_GetDeviceVendorID,i) == 0x121a)
		&& (gos_GetMachineInformation(gos_Info_GetDeviceDeviceID,i) == 0x0002))
			minTextureRam = 4096000;
		*/
		if (gos_GetMachineInformation(gos_Info_GetDeviceLocalMemory, i) >= minTextureRam)
		{
			std::wstring_view deviceName = (std::wstring_view)gos_GetMachineInformation(gos_Info_GetDeviceName, i);
			// Save name to other string here.
			cardList.AddItem(deviceName, 0xffffffff);
			if (Environment.FullScreenDevice == i)
				cardList.SelectItem(usableCardCount);
			usableCardCount++;
		}
	}
	// ALL of this is a lie.  Code is now correct but I would ignore this.
	// Sorry Heidi!
	// -fs
	// Turns out, its the same for both.
	// Environment.Renderer tells you what device you are currently using as
	// follows: 	If the value is 0 or 1 you are using the default device or the
	// primary display adapter (One and the same always) 	If the value is 2
	// you are using the second video card. 	If the value is 3 you are using
	// BLADE software renderer.  If you set the values to any of the above, you
	// will switch to that device.
	for (i = 0; i < buttonCount; i++)
	{
		buttons[i].setPressFX(LOG_VIDEOBUTTONS);
		buttons[i].setHighlightFX(LOG_DIGITALHIGHLIGHT);
		buttons[i].setDisabledFX(LOG_WRONGBUTTON);
	}
	move(xOffset, yOffset);
	helpTextArrayID = 1;
}

int32_t
OptionsGraphics::handleMessage(uint32_t message, uint32_t fromWho)
{
	if (fromWho == MSG_RESET)
		reset(originalSettings);
	return 1;
}

void OptionsGraphics::render()
{
	LogisticsScreen::render();
	resolutionList.render();
	cardList.render();
}

void OptionsGraphics::update()
{
	if (resolutionList.IsExpanded())
	{
		textObjects[helpTextArrayID].setText("");
		helpTextID = 0;
		resolutionList.update();
		if (helpTextID)
			textObjects[helpTextArrayID].setText(helpTextID);
		bExpanded = true;
	}
	else
	{
		if (!bExpanded)
		{
			LogisticsScreen::update();
		}
		if (userInput->leftMouseReleased())
			bExpanded = 0;
	}
	//	if ( cardList.ListBox().GetItemCount() > 1 )
	cardList.update();
	if (!mission || !strlen(mission->getMissionFileName()))
	{
		if (!bExpanded)
			resolutionList.update(); // don't want to call 2x's
	}
	else if (userInput->isLeftClick() && resolutionList.pointInside(userInput->getMouseX(), userInput->getMouseY()))
	{
		soundSystem->playDigitalSample(LOG_WRONGBUTTON);
	}
}

void OptionsGraphics::begin()
{
	helpTextArrayID = 1;
	reset(prefs);
}
void OptionsGraphics::end()
{
	prefs.useWaterInterestTexture = getButton(MSG_TERRAIN_DETAIL)->isPressed();
	prefs.pilotVideos = getButton(MSG_PILOT_VIDS)->isPressed();
	prefs.useHighObjectDetail = getButton(MSG_OBJECT_DETAIL)->isPressed();
	prefs.useShadows = getButton(MSG_SHADOWS)->isPressed();
	prefs.useNonWeaponEffects = getButton(MSG_NON_WEAPON)->isPressed();
	prefs.useLocalShadows = getButton(MSG_LOCAL_SHADOWS)->isPressed();
	prefs.asyncMouse = getButton(MSG_ASYNC_MOUSE)->isPressed();
	prefs.renderer = getButton(MSG_HARDWARE_RASTERIZER)->isPressed() ? 0 : 3;
	int32_t sel = resolutionList.GetSelectedItem();
	if (sel > -1)
	{
		int32_t actualSel = -1;
		for (size_t i = IDS_RESOLUTION0; i < IDS_RESOLUTION9 + 1; i++)
		{
			if (availableMode[i - IDS_RESOLUTION0])
				actualSel++;
			if (actualSel == sel)
			{
				switch (resModes[i - IDS_RESOLUTION0].xRes)
				{
				case 640:
					prefs.resolution = 0;
					break;
				case 800:
					prefs.resolution = 1;
					break;
				case 1024:
					prefs.resolution = 2;
					break;
				case 1280:
					prefs.resolution = 3;
					break;
				case 1600:
					prefs.resolution = 4;
					break;
				}
				switch (resModes[i - IDS_RESOLUTION0].bitDepth)
				{
				case 16:
					prefs.bitDepth = 0;
					break;
				case 32:
					prefs.bitDepth = 1;
					break;
				}
				break;
			}
		}
	}
	int32_t index = cardList.GetSelectedItem();
	if ((index != -1) && (prefs.renderer != 3))
		prefs.renderer = index;
}
void OptionsGraphics::reset(const CPrefs& newPrefs)
{
	getButton(MSG_TERRAIN_DETAIL)->press(newPrefs.useWaterInterestTexture);
	getButton(MSG_PILOT_VIDS)->press(newPrefs.pilotVideos);
	getButton(MSG_OBJECT_DETAIL)->press(newPrefs.useHighObjectDetail);
	getButton(MSG_SHADOWS)->press(newPrefs.useShadows);
	getButton(MSG_NON_WEAPON)->press(newPrefs.useNonWeaponEffects);
	getButton(MSG_LOCAL_SHADOWS)->press(newPrefs.useLocalShadows);
	getButton(MSG_ASYNC_MOUSE)->press(newPrefs.asyncMouse);
	getButton(MSG_HARDWARE_RASTERIZER)->press((newPrefs.renderer != 3));
	if (availableMode[1])
	{
		resolutionList.SelectItem(newPrefs.resolution * 2 + newPrefs.bitDepth);
	}
	else // Assume there are no 32-bit modes available.  Not ideal, but all we
		// can do
	{
		resolutionList.SelectItem(newPrefs.resolution);
	}
}

//////////////////////////////////////////////

void OptionsAudio::init(int32_t xOffset, int32_t yOffset)
{
	getButton(MSG_RESET)->setMessageOnRelease();
	for (size_t i = 0; i < buttonCount; i++)
	{
		if (buttons[i].getID() != MSG_RESET)
		{
			buttons[i].setParent(nullptr);
		}
	}
	move(xOffset, yOffset);
	for (i = 0; i < buttonCount; i++)
	{
		buttons[i].setPressFX(LOG_VIDEOBUTTONS);
		buttons[i].setHighlightFX(LOG_DIGITALHIGHLIGHT);
		buttons[i].setDisabledFX(LOG_WRONGBUTTON);
	}
	for (i = 0; i < 5; i++)
	{
		scrollBars[i].init(&buttons[i * 2], &buttons[i * 2 + 1], &buttons[i + 11]);
		addChild(&scrollBars[i]);
		scrollBars[i].move(-x(), -y());
		scrollBars[i].SetScrollMax(255);
	}
	helpTextArrayID = 15;
}

int32_t
OptionsAudio::handleMessage(uint32_t message, uint32_t fromWho)
{
	if (fromWho == MSG_RESET)
		reset(originalSettings);
	return 1;
}

void OptionsAudio::render()
{
	LogisticsScreen::render();
	for (size_t i = 0; i < 5; i++)
	{
		scrollBars[i].render();
	}
}

void OptionsAudio::update()
{
	LogisticsScreen::update();
	for (size_t i = 0; i < 5; i++)
	{
		scrollBars[i].update();
	}
	// Lets update these on the fly so they can hear how much better it sounds.
	prefs.DigitalMasterVolume = scrollBars[0].GetScrollPos();
	prefs.MusicVolume = scrollBars[1].GetScrollPos();
	prefs.sfxVolume = scrollBars[2].GetScrollPos();
	prefs.RadioVolume = scrollBars[3].GetScrollPos();
	prefs.BettyVolume = scrollBars[4].GetScrollPos();
	if (sndSystem)
	{
		sndSystem->setDigitalMasterVolume(prefs.DigitalMasterVolume);
		sndSystem->setSFXVolume(prefs.sfxVolume);
		sndSystem->setRadioVolume(prefs.RadioVolume);
		sndSystem->setMusicVolume(prefs.MusicVolume);
		sndSystem->setBettyVolume(prefs.BettyVolume);
	}
}

void OptionsAudio::begin()
{
	helpTextArrayID = 15;
	reset(prefs);
}
void OptionsAudio::end()
{
	prefs.DigitalMasterVolume = scrollBars[0].GetScrollPos();
	prefs.MusicVolume = scrollBars[1].GetScrollPos();
	prefs.sfxVolume = scrollBars[2].GetScrollPos();
	prefs.RadioVolume = scrollBars[3].GetScrollPos();
	prefs.BettyVolume = scrollBars[4].GetScrollPos();
}
void OptionsAudio::reset(const CPrefs& newPrefs)
{
	scrollBars[0].SetScrollPos(newPrefs.DigitalMasterVolume);
	scrollBars[1].SetScrollPos(newPrefs.MusicVolume);
	scrollBars[2].SetScrollPos(newPrefs.sfxVolume);
	scrollBars[3].SetScrollPos(newPrefs.RadioVolume);
	scrollBars[4].SetScrollPos(newPrefs.BettyVolume);
}

//////////////////////////////////////////////
void OptionsGamePlay::resetCamera()
{
	// Do NOT setup a camera in mission.
	// A.  We don't need it cause the option is greyed out.
	// B.  It creates a mech which will NOT be deleted until we
	// come back to logistics, at which point, THESE heaps will be gone!!
	// Then, CRASH!
	if (mission && strlen(mission->getMissionFileName()))
	{
	}
	else
	{
		camera.setMech(nullptr);
	}
}

void OptionsGamePlay::init(int32_t xOffset, int32_t yOffset)
{
	camera.init(rects[1].globalX() + xOffset, rects[1].globalY() + yOffset,
		rects[1].globalRight() + xOffset, rects[1].globalBottom() + yOffset);
	camera.setInMission();
	getButton(MSG_BASE)->press(true);
	getButton(MSG_ACCENT)->press(0);
	for (size_t i = 0; i < buttonCount; i++)
	{
		buttons[i].setPressFX(LOG_VIDEOBUTTONS);
		buttons[i].setHighlightFX(LOG_DIGITALHIGHLIGHT);
		buttons[i].setDisabledFX(LOG_WRONGBUTTON);
	}
	move(xOffset, yOffset);
	helpTextArrayID = 2;
}

int32_t
OptionsGamePlay::handleMessage(uint32_t message, uint32_t fromWho)
{
	if (fromWho >= MSG_GREEN && fromWho < MSG_ELITE + 1)
	{
		for (size_t i = MSG_GREEN; i < MSG_ELITE + 1; i++)
		{
			getButton(i)->press(0);
		}
		getButton(fromWho)->press(true);
	}
	switch (fromWho)
	{
	case MSG_RESET:
		reset(originalSettings);
		break;
	case MSG_BASE:
	case MSG_ACCENT:
		getButton(MSG_BASE)->press(0);
		getButton(MSG_ACCENT)->press(0);
		getButton(fromWho)->press(true);
		break;
	}
	return 1;
}

void OptionsGamePlay::render()
{
	LogisticsScreen::render();
	int32_t colorToMatch =
		getButton(MSG_BASE)->isPressed() ? rects[36].getcolour() : rects[37].getcolour();
	for (size_t i = 4; i < 36; i++)
	{
		if (rects[i].getcolour() == colorToMatch)
		{
			RECT tmp = {rects[i].globalX() - 2, rects[i].globalY() - 2, rects[i].globalRight() + 1,
				rects[i].globalBottom() + 1};
			drawEmptyRect(tmp, 0xffffffff, 0xffffffff);
			break;
		}
	}
	rects[37].render();
	rects[36].render();
	camera.render();
}

void OptionsGamePlay::update()
{
	camera.update();
	LogisticsScreen::update();
	aRect* pRect = getButton(MSG_BASE)->isPressed() ? &rects[36] : &rects[37];
	bool bChanged = 0;
	if (userInput->isLeftClick())
	{
		for (size_t i = 4; i < 36; i++)
		{
			if (rects[i].pointInside(userInput->getMouseX(), userInput->getMouseY()))
			{
				pRect->setcolour(rects[i].getcolour());
				bChanged = 1;
				break;
			}
		}
	}
	if (bChanged)
		camera.setMech(
			"Bushwacker", rects[36].getcolour(), rects[37].getcolour(), rects[37].getcolour());
}

void OptionsGamePlay::begin()
{
	helpTextArrayID = 2;
	reset(prefs);
	// Do NOT setup a camera in mission.
	// A.  We don't need it cause the option is greyed out.
	// B.  It creates a mech which will NOT be deleted until we
	// come back to logistics, at which point, THESE heaps will be gone!!
	// Then, CRASH!
	if (mission && strlen(mission->getMissionFileName()))
	{
	}
	else
	{
		camera.setMech("Bushwacker", prefs.basecolour, prefs.highlightcolour, prefs.highlightcolour);
	}
}

void OptionsGamePlay::end()
{
	for (size_t i = MSG_GREEN; i < MSG_ELITE + 1; i++)
	{
		if (getButton(i)->isPressed())
			prefs.GameDifficulty = i - MSG_GREEN;
	}
	prefs.basecolour = rects[36].getcolour();
	prefs.highlightcolour = rects[37].getcolour();
	prefs.useUnlimitedAmmo = getButton(MSG_UNLIMITED_AMMO)->isPressed();
	prefs.useLeftRightMouseProfile = getButton(MSG_LEFT_CLICK)->isPressed();
	// prefs.tutorials = getButton( MSG_TUTORIALS )->isPressed();
	// Do NOT setup a camera in mission.
	// A.  We don't need it cause the option is greyed out.
	// B.  It creates a mech which will NOT be deleted until we
	// come back to logistics, at which point, THESE heaps will be gone!!
	// Then, CRASH!
	if (mission && strlen(mission->getMissionFileName()))
	{
	}
	else
	{
		camera.setMech(nullptr);
	}
}
void OptionsGamePlay::reset(const CPrefs& newPrefs)
{
	for (size_t i = MSG_GREEN; i < MSG_ELITE + 1; i++)
	{
		getButton(i)->press(0);
	}
	getButton(MSG_GREEN + newPrefs.GameDifficulty)->press(true);
	rects[36].setcolour(newPrefs.basecolour);
	rects[37].setcolour(newPrefs.highlightcolour);
	getButton(MSG_UNLIMITED_AMMO)->press(newPrefs.useUnlimitedAmmo);
	getButton(MSG_LEFT_CLICK)->press(newPrefs.useLeftRightMouseProfile);
	//	getButton( MSG_TUTORIALS )->press( newPrefs.tutorials );
}

//////////////////////////////////////////////

void OptionsHotKeys::init(int32_t xOffset, int32_t yOffset)
{
	hotKeyList.init(rects[0].x(), rects[0].y(), rects[0].width(), rects[0].height());
	helpTextArrayID = 2;
	HotKeyListItem::init();
	hotKeyList.setOrange(true);
	bShowDlg = 0;
	move(xOffset, yOffset);
	hotKeyList.move(xOffset, yOffset);
	helpTextArrayID = 2;
}

int32_t
OptionsHotKeys::handleMessage(uint32_t message, uint32_t fromWho)
{
	switch (fromWho)
	{
	case MSG_RESET:
		reset(true);
		break;
	}
	return 1;
}

void OptionsHotKeys::render()
{
	hotKeyList.render();
	LogisticsScreen::render();
	if (bShowDlg)
	{
		LogisticsOKDialog::instance()->render();
	}
}

void OptionsHotKeys::update()
{
	if (bShowDlg)
	{
		LogisticsOKDialog::instance()->update();
		if (LogisticsOKDialog::instance()->isDone())
		{
			bShowDlg = 0;
			if (LogisticsDialog::YES == LogisticsOKDialog::instance()->getStatus())
			{
				wchar_t keysString[256];
				keysString[0] = 0;
				makeKeyString(curHotKey, keysString);
				int32_t index = hotKeyList.GetSelectedItem();
				int32_t oldKey = -1;
				if (index > -1)
				{
					HotKeyListItem* pItemToSet = (HotKeyListItem*)hotKeyList.GetItem(index);
					// now I've got to find the other one with th new key and
					// set it to the old key
					for (size_t i = 0; i < hotKeyList.GetItemCount(); i++)
					{
						HotKeyListItem* pTmpItem = (HotKeyListItem*)hotKeyList.GetItem(i);
						if (pTmpItem->getHotKey() == curHotKey && pTmpItem != pItemToSet)
						{
							// first we've got to see if we can set to the
							// default
							int32_t* defaultKeys = MissionInterfaceManager::getOldKeys();
							int32_t defaultKey = defaultKeys[pTmpItem->getCommand()];
							for (size_t j = 0; j < hotKeyList.GetItemCount(); j++)
							{
								HotKeyListItem* pCheckItem = (HotKeyListItem*)hotKeyList.GetItem(j);
								if (pCheckItem->getHotKey() == defaultKey)
								{
									defaultKey = -1;
									break;
								}
							}
							if (defaultKey != -1)
								oldKey = defaultKey;
							else if (pItemToSet)
								oldKey = pItemToSet->getHotKey();
							wchar_t tmpKeyStr[256];
							tmpKeyStr[0] = 0;
							makeKeyString(oldKey, tmpKeyStr);
							pTmpItem->setHotKey(oldKey);
							pTmpItem->setKey(tmpKeyStr);
						}
					}
					pItemToSet->setKey(keysString);
					pItemToSet->setHotKey(curHotKey);
					hotKeyList.SelectItem(-1);
				}
			}
		}
		return;
	}
	LogisticsScreen::update();
	hotKeyList.update();
	int32_t tmpKey = 1;
	while (tmpKey) // empty out keyboard buffers...
	{
		int32_t index = hotKeyList.GetSelectedItem();
		tmpKey = 0;
		if (index > -1)
		{
			HotKeyListItem* pItem = (HotKeyListItem*)hotKeyList.GetItem(index);
			tmpKey = gos_GetKey();
			if (tmpKey)
			{
				wchar_t hotKeyString[256];
				hotKeyString[0] = 0;
				if (0 != makeInputKeyString(tmpKey, hotKeyString))
					return;
				curHotKey = tmpKey;
				// check and see if anyone else is using this one...
				for (size_t i = 0; i < hotKeyList.GetItemCount(); i++)
				{
					HotKeyListItem* pTmpItem = (HotKeyListItem*)hotKeyList.GetItem(i);
					if (pTmpItem->getHotKey() == curHotKey && pTmpItem != pItem)
					{
						LogisticsOKDialog::instance()->setText(
							IDS_OPTIONS_HOTKEY_ERROR, IDS_DIALOG_NO, IDS_DIALOG_YES);
						LogisticsOKDialog::instance()->begin();
						bShowDlg = true;
					}
				}
				if (!bShowDlg)
				{
					pItem->setHotKey(tmpKey);
					pItem->setKey(hotKeyString);
					hotKeyList.SelectItem(-1);
				}
			}
		}
	}
}

void OptionsHotKeys::makeKeyString(int32_t newKey, std::wstring_view keysString)
{
	wchar_t shift[32];
	wchar_t control[32];
	wchar_t alt[32];
	cLoadString(IDS_SHIFT, shift, 31);
	cLoadString(IDS_CONTROL, control, 31);
	cLoadString(IDS_ALT, alt, 31);
	int32_t key = newKey;
	std::wstring_view pKey = gos_DescribeKey((key & 0x000fffff) << 8);
	if (((key & SHIFT)))
	{
		strcat(keysString, shift);
		strcat(keysString, " + ");
	}
	if (((key & CTRL)))
	{
		strcat(keysString, control);
		strcat(keysString, " + ");
	}
	if (((key & ALT)))
	{
		strcat(keysString, alt);
		strcat(keysString, " + ");
	}
	strcat(keysString, pKey);
}

int32_t
OptionsHotKeys::makeInputKeyString(int32_t& tmpKey, std::wstring_view hotKeyString)
{
	std::wstring_view pText = gos_DescribeKey(tmpKey & 0x0001ff00);
	int32_t tmp = (tmpKey >> 8) & 0x01ff;
	if (tmp == KEY_LSHIFT || tmp == KEY_LMENU || tmp == KEY_LCONTROL)
		return -1;
	int32_t hotKey = tmp;
	bool shiftDn = userInput->shift();
	if (shiftDn)
	{
		wchar_t shift[32];
		cLoadString(IDS_SHIFT, shift, 31);
		hotKey |= SHIFT;
		strcat(hotKeyString, shift);
		strcat(hotKeyString, " + ");
	}
	bool ctrlDn = userInput->ctrl();
	if (ctrlDn)
	{
		wchar_t control[32];
		cLoadString(IDS_CONTROL, control, 31);
		hotKey |= CTRL;
		strcat(hotKeyString, control);
		strcat(hotKeyString, " + ");
	}
	bool altDn = userInput->alt();
	if (altDn)
	{
		wchar_t alt[32];
		cLoadString(IDS_ALT, alt, 31);
		hotKey |= ALT;
		strcat(hotKeyString, alt);
		strcat(hotKeyString, " + ");
	}
	strcat(hotKeyString, pText);
	tmpKey = hotKey;
	return 0;
}

void OptionsHotKeys::begin()
{
	helpTextArrayID = 2;
	reset(0);
}
void OptionsHotKeys::end()
{
	for (size_t i = 0; i < hotKeyList.GetItemCount(); i++)
	{
		HotKeyListItem* pItem = (HotKeyListItem*)hotKeyList.GetItem(i);
		if (pItem)
		{
			int32_t Command = pItem->getCommand();
			int32_t Key = pItem->getHotKey();
			MissionInterfaceManager::setHotKey(
				Command, (gosEnum_KeyIndex)(Key & 0x000ffff), Key & SHIFT, Key & CTRL, Key & ALT);
		}
	}
	bShowDlg = 0;
	hotKeyList.removeAllItems(true);
	//	MissionInterfaceManager::setHotKey(
}
void OptionsHotKeys::reset(bool useOld)
{
	hotKeyList.removeAllItems(true);
	wchar_t shift[32];
	wchar_t control[32];
	wchar_t alt[32];
	wchar_t descText[128];
	wchar_t keysString[128];
	cLoadString(IDS_SHIFT, shift, 31);
	cLoadString(IDS_CONTROL, control, 31);
	cLoadString(IDS_ALT, alt, 31);
	MissionInterfaceManager::Command* commands = MissionInterfaceManager::getCommands();
	int32_t* oldKeys = MissionInterfaceManager::getOldKeys();
	for (size_t i = 0; i < MAX_COMMAND; i++)
	{
		if (commands[i].hotKeyDescriptionText != -1)
		{
			keysString[0] = 0;
			cLoadString(commands[i].hotKeyDescriptionText, descText, 127);
			int32_t key = useOld ? oldKeys[i] : commands[i].key;
			makeKeyString(key, keysString);
			HotKeyListItem* item = new HotKeyListItem();
			item->setDescription(descText);
			item->setKey(keysString);
			item->setHotKey(key);
			item->setCommand(i);
			hotKeyList.AddItem(item);
		}
	}
}

ScrollX::ScrollX()
{
	scrollMax = 0;
	scrollPos = 0;
	lastX = 0;
	scrollInc = 1;
	pageInc = 5;
}

int32_t
ScrollX::init(aButton* pLeft, aButton* pRight, aButton* pTab)
{
	aObject::init(pLeft->globalX(), pLeft->top(), pRight->globalRight() - pLeft->globalX(),
		pRight->bottom() - pRight->top());
	buttons[0] = pLeft;
	buttons[1] = pRight;
	buttons[2] = pTab;
	pLeft->setPressFX(-1);
	pLeft->setHighlightFX(-1);
	pLeft->setDisabledFX(-1);
	for (size_t i = 0; i < 3; i++)
		buttons[i]->setHoldTime(.01f);
	pLeft->moveTo(0, 0);
	pRight->moveTo(width() - pRight->width(), 0);
	pTab->moveTo(pLeft->width() + 1, -1);
	addChild(pLeft);
	addChild(pRight);
	addChild(pTab);
	setcolour(0);
	return (NO_ERROR);
}

void ScrollX::SetScrollMax(float newMax)
{
	scrollMax = newMax;
	buttons[2]->showGUIWindow(newMax != 0);
	ResizeAreas();
}

void ScrollX::SetScrollPos(float newPos)
{
	if (newPos < 0)
		newPos = 0;
	if (newPos > scrollMax)
		newPos = scrollMax;
	scrollPos = newPos;
	ResizeAreas();
}

void ScrollX::SetScroll(int32_t newScrollPos)
{
	if (newScrollPos < 0)
		newScrollPos = 0;
	if (newScrollPos > scrollMax)
		newScrollPos = scrollMax;
	if (getParent())
		getParent()->handleMessage(aMSG_SCROLLTO, newScrollPos);
	SetScrollPos(newScrollPos);
}
void ScrollX::update()
{
	int32_t mouseX = userInput->getMouseX();
	int32_t mouseY = userInput->getMouseY();
	if (userInput->isLeftDrag() && lastX) // dragging the little tab
	{
		float delta = (float)mouseX - (buttons[0]->globalX() + buttons[0]->width());
		// figure out what this translates to
		float physicalRange =
			width() - buttons[0]->width() - buttons[1]->width() - buttons[2]->width();
		float RealRange = scrollMax;
		if (!physicalRange)
			physicalRange = RealRange;
		float newScrollPos = .5 + (delta)*RealRange / physicalRange;
		if (newScrollPos < 0)
			newScrollPos = 0;
		if (newScrollPos > scrollMax)
			newScrollPos = scrollMax;
		getParent()->handleMessage(aMSG_SCROLLTO, newScrollPos);
		SetScrollPos(newScrollPos);
	}
	else if (pointInside(mouseX, mouseY))
	{
		if (userInput->isLeftClick() || gos_GetKeyStatus(KEY_LMOUSE) == KEY_HELD)
		{
			lastX = 0;
			if (buttons[2]->pointInside(mouseX, mouseY) && buttons[2]->pointInside(userInput->getMouseDragX(), userInput->getMouseDragY()))
				lastX = mouseX;
			else if (getParent())
			{
				buttons[2]->press(0);
				if (!buttons[0]->pointInside(mouseX, mouseY) && !buttons[1]->pointInside(mouseX, mouseY) && pointInside(userInput->getMouseDragX(), userInput->getMouseDragY()))
				{
					float physicalRange =
						width() - buttons[0]->width() - buttons[1]->width() - buttons[2]->width();
					float RealRange = scrollMax;
					float delta = (float)mouseX - (buttons[0]->globalX() + buttons[0]->width());
					// if above the thumb, page up, otherwise page down
					if (mouseY < buttons[2]->top())
					{
						float newScrollPos = (delta)*RealRange / physicalRange;
						if (scrollPos - newScrollPos > pageInc)
						{
							newScrollPos = scrollPos - pageInc;
						}
						getParent()->handleMessage(aMSG_SCROLLTO, newScrollPos);
						SetScrollPos(newScrollPos);
					}
					else
					{
						float newScrollPos = (delta)*RealRange / physicalRange;
						if (newScrollPos - scrollPos > pageInc)
						{
							newScrollPos = scrollPos + pageInc;
						}
						getParent()->handleMessage(aMSG_SCROLLTO, newScrollPos);
						SetScrollPos(newScrollPos);
					}
				}
			}
		}
		else
			buttons[2]->press(0);
	}
	else
		buttons[2]->press(0);
	if (userInput->leftMouseReleased())
		lastX = 0;
	aObject::update();
}

int32_t
ScrollX::handleMessage(uint32_t message, uint32_t who)
{
	switch (who)
	{
	case aMSG_SCROLLUP:
		SetScrollPos(scrollPos - scrollInc);
		break;
	case aMSG_SCROLLDOWN:
		SetScrollPos(scrollPos + scrollInc);
		break;
	}
	return getParent()->handleMessage(who, who);
}

void ScrollX::ResizeAreas(void)
{
	float range, position;
	if (scrollMax == 0)
		return;
	range = width() - buttons[0]->width() - buttons[1]->width() - buttons[2]->width() - 4.f; // one scrollwidth for buttons, one for
		// tab. 2 for lines at either end.
	position = range * scrollPos / scrollMax; //	center of scroll tab;
	buttons[2]->moveTo(globalX() + position + buttons[0]->width() + 2, globalY() + 1);
}

void ScrollX::Enable(bool enable)
{
	buttons[2]->disable(!enable);
	buttons[0]->disable(!enable);
	buttons[1]->disable(!enable);
}

void HotKeyListItem::init()
{
	if (s_item)
		return;
	s_item = new HotKeyListItem();
	FitIniFile file;
	FullPathFileName path;
	path.init(artPath, "mcl_options_combobox1", ".fit");
	if (NO_ERROR != file.open(path))
	{
		wchar_t error[256];
		sprintf(error, "couldn't open file %s", path);
		Assert(0, 0, error);
		return;
	}
	s_item->description.init(&file, "Text0");
	s_item->text.init(&file, "Text1");
	s_item->animations[0].init(&file, "Animation0");
	s_item->animations[1].init(&file, "Animation1");
	s_item->animations[2].init(&file, "Animation2");
	s_item->rects[0].init(&file, "Rect0");
	s_item->rects[1].init(&file, "Rect1");
}
void HotKeyListItem::render()
{
	aAnimGroup::STATE curState = (aAnimGroup::STATE)getState();
	for (size_t i = 0; i < 3; i++)
	{
		animations[i].setState(curState);
		animations[i].update();
	}
	text.setcolour(animations[1].getCurrentcolour(curState));
	description.setcolour(animations[0].getCurrentcolour(curState));
	rects[1].setcolour(animations[2].getCurrentcolour(curState));
	aObject::render();
}

void HotKeyListItem::update()
{
}

void HotKeyListItem::setDescription(std::wstring_view pText)
{
	description.setText(pText);
}
void HotKeyListItem::setKey(std::wstring_view pText)
{
	text.setText(pText);
}
HotKeyListItem::~HotKeyListItem()
{
	removeAllChildren(0);
}
HotKeyListItem::HotKeyListItem()
{
	if (s_item)
	{
		description = s_item->description;
		text = s_item->text;
		for (size_t i = 0; i < 3; i++)
			animations[i] = s_item->animations[i];
		rects[0] = s_item->rects[0];
		rects[1] = s_item->rects[1];
		aObject::init(0, 0, rects[1].right(), rects[0].bottom() + 1);
		addChild(&rects[1]);
		addChild(&description);
		addChild(&text);
		addChild(&rects[0]);
	}
}
