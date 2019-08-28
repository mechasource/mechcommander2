/*************************************************************************************************\
LogisticsDialog.cpp			: Implementation of the LogisticsDialog component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#define LOGISTICSDIALOG_CPP

#include "stdinc.h"
#include "logisticsdialog.h"
#include "mechgui/abutton.h"
#include "inifile.h"
#include "mclib.h"
#include "windows.h"
#include "resource.h"
#include "mechgui/aedit.h"
#include "logisticsdata.h"
#include <malloc.h>
#include "missionbriefingscreen.h"
#include "sounds.h"
#include "gamesound.h"

LogisticsOKDialog* LogisticsOKDialog::s_instance = nullptr;
LogisticsSaveDialog* LogisticsSaveDialog::s_instance = nullptr;
LogisticsVariantDialog* LogisticsVariantDialog::s_instance = nullptr;
LogisticsOneButtonDialog* LogisticsOneButtonDialog::s_instance = nullptr;
LogisticsLegalDialog* LogisticsLegalDialog::s_instance = nullptr;

extern int32_t SaveGameVersionNumber;

#define DELETE_BUTTON 40
#define MAP_STATIC 10

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void
LogisticsDialog::begin()
{
	enterAnim.begin();
	exitAnim.end();
	status = RUNNING;
	bDone = 0;
	soundSystem->playDigitalSample(LOG_MAINMENUBUTTON);
	gos_KeyboardFlush();
}

LogisticsDialog::LogisticsDialog()
{
	oldFont = -1;
}

//-------------------------------------------------------------------------------------------------

void
LogisticsDialog::end()
{
	enterAnim.end();
	exitAnim.begin();
	gos_EnableIME(false);
	if (oldFont != -1)
	{
		if (textCount)
			textObjects[0].font.init(oldFont);
	}
}

void
LogisticsDialog::setFont(int32_t newFontResID)
{
	if (textCount)
	{
		oldFont = textObjects[0].font.getFontID();
		textObjects[0].font.init(newFontResID);
	}
}

//-------------------------------------------------------------------------------------------------

void
LogisticsDialog::render()
{
	float color = 0x7f000000;
	if (enterAnim.isAnimating() && !enterAnim.isDone())
	{
		float time = enterAnim.getCurrentTime();
		float endTime = enterAnim.getMaxTime();
		if (endTime)
		{
			color = interpolatecolour(0x00000000, 0x7f000000, time / endTime);
		}
	}
	else if (exitAnim.isAnimating() && !exitAnim.isDone())
	{
		float time = exitAnim.getCurrentTime();
		float endTime = exitAnim.getMaxTime();
		if (endTime)
		{
			color = interpolatecolour(0x7f000000, 0x00000000, time / endTime);
		}
	}
	RECT rect = {0, 0, Environment.screenwidth, Environment.screenheight};
	drawRect(rect, color);
	float xOffset = 0;
	float yOffset = 0;
	if (enterAnim.isAnimating())
	{
		xOffset = enterAnim.getXDelta();
		yOffset = enterAnim.getYDelta();
	}
	else if (exitAnim.isAnimating())
	{
		xOffset = exitAnim.getXDelta();
		yOffset = exitAnim.getYDelta();
	}
	LogisticsScreen::render((int32_t)xOffset, (int32_t)yOffset);
}

//-------------------------------------------------------------------------------------------------

void
LogisticsDialog::update()
{
	enterAnim.update();
	exitAnim.update();
	if (exitAnim.isDone())
		bDone = true;
	// hack for some reason we don't get here from the zone
	userInput->mouseOn();
	userInput->setMouseCursor(mState_NORMAL);
	bool bFocus = 0;
	for (size_t i = 0; i < editCount; i++)
	{
		if (edits[i].hasFocus())
			bFocus = true;
	}
	if (enterAnim.isDone())
		LogisticsScreen::update();
	if (userInput->leftMouseReleased() && enterAnim.isDone() && !inside(userInput->getMouseX(), userInput->getMouseY()) && !inside(userInput->getMouseDragX(), userInput->getMouseDragY()))
	{
		handleMessage(0, NO);
	}
	if (!bFocus && enterAnim.isDone())
	{
		if (gos_GetKeyStatus(KEY_RETURN) == KEY_RELEASED)
		{
			if (getButton(YES))
			{
				if (getButton(YES)->isEnabled())
					handleMessage(aMSG_LEFTMOUSEDOWN, YES);
			}
			else
				handleMessage(aMSG_LEFTMOUSEDOWN, YES);
		}
	}
	if (gos_GetKeyStatus(KEY_ESCAPE) == KEY_RELEASED)
	{
		if (getButton(NO /*MB_MSG_MAINMENU*/))
		{
			if (getButton(NO)->isEnabled())
				handleMessage(aMSG_LEFTMOUSEDOWN, NO);
		}
	}
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

LogisticsOKDialog::LogisticsOKDialog() {}

//-------------------------------------------------------------------------------------------------

LogisticsOKDialog::~LogisticsOKDialog() {}

//-------------------------------------------------------------------------------------------------

int32_t
LogisticsOKDialog::init(FitIniFile& file)
{
	if (!s_instance)
	{
		s_instance = new LogisticsOKDialog;
		((LogisticsScreen*)s_instance)->init(file, "Static", "Text", "Rect", "Button", "Edit");
		file.seekBlock("EnterAnim");
		s_instance->enterAnim.init(&file, "");
		file.seekBlock("LeaveAnim");
		s_instance->exitAnim.init(&file, "");
		for (size_t i = 0; i < s_instance->buttonCount; i++)
			s_instance->buttons[i].setMessageOnRelease();
		s_instance->oldFont = s_instance->textObjects[0].font.getFontID();
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------

int32_t
LogisticsOKDialog::handleMessage(uint32_t, uint32_t who)
{
	status = who;
	exitAnim.begin();
	enterAnim.end();
	return 0;
}

//-------------------------------------------------------------------------------------------------

void
LogisticsOKDialog::setText(int32_t textID, int32_t CancelButton, int32_t OKButton)
{
	textObjects[0].setText(textID);
	buttons[0].setText(CancelButton);
	buttons[1].setText(OKButton);
}

void
LogisticsOKDialog::setText(const std::wstring_view& mainText)
{
	textObjects[0].setText(mainText);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

LogisticsOneButtonDialog::LogisticsOneButtonDialog() {}

//-------------------------------------------------------------------------------------------------

LogisticsOneButtonDialog::~LogisticsOneButtonDialog() {}

//-------------------------------------------------------------------------------------------------

int32_t
LogisticsOneButtonDialog::init(FitIniFile& file)
{
	if (!s_instance)
	{
		s_instance = new LogisticsOneButtonDialog;
		((LogisticsScreen*)s_instance)->init(file, "Static", "Text", "Rect", "Button", "Edit");
		file.seekBlock("EnterAnim");
		s_instance->enterAnim.init(&file, "");
		file.seekBlock("LeaveAnim");
		s_instance->exitAnim.init(&file, "");
		for (size_t i = 0; i < s_instance->buttonCount; i++)
			s_instance->buttons[i].setMessageOnRelease();
		s_instance->oldFont = s_instance->textObjects[0].font.getFontID();
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------

int32_t
LogisticsLegalDialog::init(FitIniFile& file)
{
	if (!s_instance)
	{
		s_instance = new LogisticsLegalDialog;
		((LogisticsScreen*)s_instance)->init(file, "Static", "Text", "Rect", "Button", "Edit");
		file.seekBlock("EnterAnim");
		s_instance->enterAnim.init(&file, "");
		file.seekBlock("LeaveAnim");
		s_instance->exitAnim.init(&file, "");
		for (size_t i = 0; i < s_instance->buttonCount; i++)
			s_instance->buttons[i].setMessageOnRelease();
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------

int32_t
LogisticsOneButtonDialog::handleMessage(uint32_t, uint32_t who)
{
	status = who;
	exitAnim.begin();
	enterAnim.end();
	return 0;
}

//-------------------------------------------------------------------------------------------------

void
LogisticsOneButtonDialog::setText(int32_t textID, int32_t CancelButton, int32_t OKButton)
{
	textObjects[0].setText(textID);
	buttons[0].setText(CancelButton);
	//	buttons[1].setText( OKButton );
}

void
LogisticsOneButtonDialog::setText(const std::wstring_view& mainText)
{
	textObjects[0].setText(mainText);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

LogisticsSaveDialog::LogisticsSaveDialog() {}

//-------------------------------------------------------------------------------------------------

LogisticsSaveDialog::~LogisticsSaveDialog()
{
	gameListBox.destroy();
}

//-------------------------------------------------------------------------------------------------

int32_t
LogisticsSaveDialog::init(FitIniFile& file)
{
	if (!s_instance)
	{
		s_instance = new LogisticsSaveDialog();
		((LogisticsScreen*)s_instance)->init(file, "Static", "Text", "Rect", "Button");
		file.seekBlock("InAnim");
		s_instance->enterAnim.init(&file, "");
		file.seekBlock("OutAnim");
		s_instance->exitAnim.init(&file, "");
		s_instance->gameListBox.init(s_instance->rects[1].globalX(), s_instance->rects[1].globalY(),
			s_instance->rects[1].width(), s_instance->rects[1].height());
		s_instance->gameListBox.setTopSkip(3);
		s_instance->gameListBox.setOrange(true);
		for (size_t i = 0; i < s_instance->buttonCount; i++)
			s_instance->buttons[i].setMessageOnRelease();
		s_instance->templateItem.init(&file, "AnimText");
		s_instance->templateItem.resize(
			s_instance->gameListBox.width() - 30, s_instance->templateItem.height());
		for (i = 0; i < s_instance->editCount; i++)
		{
			s_instance->edits[i].allowWierdChars(0);
		}
	}
	s_instance->getButton(YES)->setPressFX(LOG_NEXTBACKBUTTONS);
	return 0;
}

void
LogisticsSaveDialog::begin()
{
	beginFadeIn(0); // turn off any fades...
	edits[0].setFocus(true);
	initDialog(savePath, 0);
	status = RUNNING;
	bPromptOverwrite = 0;
	bDeletePrompt = 0;
	bLoad = 0;
	bCampaign = 0;
	// need to change all the texts
	textObjects[0].setText(IDS_DIALOG_SAVE_GAME);
	textObjects[1].setText(IDS_DIALOG_GAME_LIST);
	textObjects[2].setText("");
	buttons[2].setText(IDS_DIALOG_SAVE);
	edits[0].setEntry("");
	edits[0].limitEntry(20);
	statics[MAP_STATIC].setTexture((uint32_t)0);
	aListItem* pItem = gameListBox.GetItem(0);
	if (pItem)
	{
		pItem->select();
		edits[0].setEntry(((aTextListItem*)pItem)->getText());
		selectedName = (((aLocalizedListItem*)pItem)->getHiddenText());
	}
	updateMissionInfo();
	LogisticsDialog::begin();
}

void
LogisticsSaveDialog::beginLoad()
{
	beginFadeIn(0);
	edits[0].setFocus(true);
	initDialog(savePath, 0);
	status = RUNNING;
	bPromptOverwrite = 0;
	bDeletePrompt = 0;
	bLoad = 1;
	bCampaign = 0;
	// need to change all the texts...
	textObjects[0].setText(IDS_DIALOG_LOAD_GAME);
	textObjects[1].setText(IDS_DIALOG_GAME_LIST);
	textObjects[2].setText("");
	buttons[2].setText(IDS_LOAD);
	edits[0].setEntry("");
	edits[0].limitEntry(20);
	aListItem* pItem = gameListBox.GetItem(0);
	if (pItem)
	{
		pItem->select();
		edits[0].setEntry(((aTextListItem*)pItem)->getText());
		selectedName = (((aLocalizedListItem*)pItem)->getHiddenText());
	}
	updateMissionInfo();
	LogisticsDialog::begin();
}

void
LogisticsSaveDialog::beginCampaign()
{
	beginLoad();
	edits[0].limitEntry(20);
	bCampaign = true;
	initDialog(campaignPath, 1);
	if (!gameListBox.GetItemCount())
	{
		selectedName = "campaign";
		bDone = true;
		return;
	}
	else
	{
		aListItem* pItem = gameListBox.GetItem(0);
		if (pItem)
		{
			pItem->select();
			edits[0].setEntry(((aTextListItem*)pItem)->getText());
			selectedName = (((aLocalizedListItem*)pItem)->getHiddenText());
		}
	}
	updateMissionInfo();
}

void
LogisticsSaveDialog::initDialog(const std::wstring_view& path, bool bCampaign)
{
	gameListBox.removeAllItems(true);
	// need to add items to the save game list
	wchar_t findString[512];
	sprintf(findString, "%s*.fit", path);
	WIN32_FIND_DATA findResult;
	HANDLE searchHandle = FindFirstFile(findString, &findResult);
	if (searchHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((findResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				aLocalizedListItem* pEntry = new aLocalizedListItem();
				*pEntry = s_instance->templateItem;
				const std::wstring_view& pExt = strstr(findResult.cFileName, ".fit");
				if (!pExt)
				{
					pExt = (strstr(findResult.cFileName, ".FIT"));
				}
				if (pExt)
					*pExt = nullptr;
				if (!bCampaign && isCorrectVersionSaveGame(findResult.cFileName))
					pEntry->setText(findResult.cFileName);
				else if (bCampaign)
					pEntry->setText(findResult.cFileName);
				if (bCampaign)
				{
					wchar_t campaignName[1024];
					readCampaignNameFromFile(findResult.cFileName, campaignName, 1023);
					pEntry->setText(campaignName);
				}
				if (bCampaign || isCorrectVersionSaveGame(findResult.cFileName))
				{
					pEntry->setHiddenText(findResult.cFileName);
					pEntry->setcolour(edits[0].getcolour());
					gameListBox.AddItem(pEntry);
				}
			}
		} while (FindNextFile(searchHandle, &findResult) != 0);
		FindClose(searchHandle);
	}
	// Check for the two we KNOW are there but may be in fast files.
	// Campaign and tutorial
	if (bCampaign)
	{
		FullPathFileName path;
		path.init(campaignPath, "campaign", ".fit");
		// All of this will work in the fastfiles.
		if (fileExists(path))
		{
			aLocalizedListItem* pEntry = new aLocalizedListItem();
			*pEntry = s_instance->templateItem;
			wchar_t campaignName[1024];
			readCampaignNameFromFile("campaign", campaignName, 1023);
			pEntry->setText(campaignName);
			pEntry->setHiddenText("campaign");
			pEntry->setcolour(edits[0].getcolour());
			gameListBox.AddItem(pEntry);
		}
		path.init(campaignPath, "tutorial", ".fit");
		// All of this will work in the fastfiles.
		if (fileExists(path))
		{
			aLocalizedListItem* pEntry = new aLocalizedListItem();
			*pEntry = s_instance->templateItem;
			wchar_t campaignName[1024];
			readCampaignNameFromFile("tutorial", campaignName, 1023);
			pEntry->setText(campaignName);
			pEntry->setHiddenText("tutorial");
			pEntry->setcolour(edits[0].getcolour());
			gameListBox.AddItem(pEntry);
		}
	}
}

bool
LogisticsSaveDialog::isCorrectVersionSaveGame(const std::wstring_view& fileName)
{
	FullPathFileName path;
	path.init(savePath, fileName, ".fit");
	FitIniFile file;
	if (NO_ERROR != file.open((const std::wstring_view&)(const std::wstring_view&)path))
	{
		wchar_t errorStr[256];
		sprintf(errorStr, "couldn't open file %s", path);
		STOP((errorStr));
	}
	int32_t result = file.seekBlock("Version");
	if (result != NO_ERROR)
		return false;
	int32_t testVersionNum = 0;
	result = file.readIdLong("VersionNumber", testVersionNum);
	if ((result == NO_ERROR) && (testVersionNum == SaveGameVersionNumber))
		return true;
	return false;
}

void
LogisticsSaveDialog::readCampaignNameFromFile(const std::wstring_view& fileName, const std::wstring_view& resultName, int32_t len)
{
	FullPathFileName path;
	path.init(campaignPath, fileName, ".fit");
	FitIniFile file;
	if (NO_ERROR != file.open((const std::wstring_view&)(const std::wstring_view&)path))
	{
		wchar_t errorStr[256];
		sprintf(errorStr, "couldn't open file %s", path);
		Assert(0, 0, errorStr);
	}
	int32_t result = file.seekBlock("Campaign");
	Assert(result == NO_ERROR, 0, "Coudln't find the mission settings block in the mission file");
	resultName[0] = 0;
	int32_t lName = 0;
	result = file.readIdLong("NameID", lName);
	if (result == NO_ERROR)
	{
		cLoadString(lName, resultName, len);
	}
	else
	{
		result = file.readIdString("CampaignName", resultName, len);
		Assert(result == NO_ERROR, 0, "couldn't find the missionName");
	}
}

void
LogisticsSaveDialog::end()
{
	statics[MAP_STATIC].setTexture((uint32_t)0);
	bCampaign = 0;
	LogisticsDialog::end();
}
void
LogisticsSaveDialog::update()
{
	if (!bPromptOverwrite && !bDeletePrompt)
		LogisticsDialog::update();
	if (exitAnim.isDone())
		bDone = true;
	buttons[2].disable(0);
	buttons[0].disable(0);
	const std::wstring_view& fileName;
	edits[0].getEntry(fileName);
	if (bCampaign)
	{
		fileName.Empty();
		const std::wstring_view& displayName;
		edits[0].getEntry(displayName);
		{
			/*if there is a selected item and it matches the text in the
			 * editbox, then use that selected item*/
			aLocalizedListItem* pSelectedItem = 0;
			int32_t selectedIndex = gameListBox.GetSelectedItem();
			if ((0 <= selectedIndex) && (gameListBox.GetItemCount() > selectedIndex))
			{
				pSelectedItem =
					dynamic_cast<aLocalizedListItem*>(gameListBox.GetItem(selectedIndex));
			}
			if (pSelectedItem)
			{
				if (0 == displayName.Compare(pSelectedItem->getText(), false /*not case sensitive*/))
				{
					fileName = pSelectedItem->getHiddenText();
				}
			}
		}
		if (fileName.IsEmpty())
		{
			/* No item is selected or the selected item doesn't match the
			 * editbox. */
			// loop through the entries in the game list box, and fine the same
			// one, and find its hidden text
			for (size_t i = 0; i < gameListBox.GetItemCount(); i++)
			{
				aLocalizedListItem* pItem =
					dynamic_cast<aLocalizedListItem*>(gameListBox.GetItem(i));
				if (displayName.Compare(pItem->getText(), false /*not case sensitive*/) == 0)
				{
					fileName = pItem->getHiddenText();
					break;
				}
			}
		}
	}
	if ((bLoad || bCampaign) && fileName.Length())
	{
		FullPathFileName path;
		path.init(bCampaign ? campaignPath : savePath, fileName, ".fit");
		if (fileExists(path))
			getButton(YES)->disable(false);
		else
			getButton(YES)->disable(true);
	}
	else if (!fileName.Length())
	{
		getButton(YES)->disable(true);
	}
	else
		getButton(YES)->disable(false);
	if (bPromptOverwrite)
	{
		LogisticsOKDialog::instance()->update();
		if (LogisticsOKDialog::instance()->isDone())
		{
			if (YES == LogisticsOKDialog::instance()->getStatus())
			{
				status = YES;
				bPromptOverwrite = 0;
				end();
			}
			else if (NO == LogisticsOKDialog::instance()->getStatus())
			{
				bPromptOverwrite = 0;
			}
		}
	}
	else if (bDeletePrompt)
	{
		LogisticsOKDialog::instance()->update();
		if (YES == LogisticsOKDialog::instance()->getStatus())
		{
			FullPathFileName name;
			if (bCampaign)
			{
				name.init(campaignPath, selectedName, ".fit");
			}
			else
			{
				name.init(savePath, selectedName, ".fit");
			}
			// delete the file
			DeleteFile(name);
			initDialog(bCampaign ? campaignPath : savePath, bCampaign ? 1 : 0);
			bDeletePrompt = 0;
			edits[0].getEntry(selectedName);
			edits[0].setEntry("");
			selectedName = "";
			aListItem* pItem = gameListBox.GetItem(0);
			if (pItem)
			{
				pItem->select();
				edits[0].setEntry(((aTextListItem*)pItem)->getText());
				selectedName = (((aLocalizedListItem*)pItem)->getHiddenText());
			}
		}
		else if (NO == LogisticsOKDialog::instance()->getStatus())
		{
			bDeletePrompt = 0;
		}
	}
	gameListBox.update();
	if (userInput->isLeftClick())
	{
		if (gameListBox.pointInside(userInput->getMouseX(), userInput->getMouseY()))
		{
			// get selected item
			int32_t item = gameListBox.GetSelectedItem();
			if (item != -1)
			{
				for (size_t i = 0; i < gameListBox.GetItemCount(); i++)
				{
					gameListBox.GetItem(i)->setcolour(edits[0].getcolour());
				}
				gameListBox.GetItem(item)->setcolour(edits[0].getHighlightcolour());
				const std::wstring_view& text = ((aTextListItem*)gameListBox.GetItem(item))->getText();
				edits[0].setEntry(text);
				selectedName = ((aLocalizedListItem*)gameListBox.GetItem(item))->getHiddenText();
				// update mission info
				updateMissionInfo();
			}
		}
		else if (edits[0].pointInside(userInput->getMouseX(), userInput->getMouseY()))
		{
			gameListBox.SelectItem(-1);
			for (size_t i = 0; i < gameListBox.GetItemCount(); i++)
			{
				gameListBox.GetItem(i)->setcolour(edits[0].getcolour());
			}
		}
	}
}

void
LogisticsSaveDialog::updateCampaignMissionInfo()
{
	FitIniFile file;
	FullPathFileName path;
	path.init(campaignPath, selectedName, ".fit");
	if (NO_ERROR != file.open(path))
	{
		Assert(0, 0, "coudln't find the campaign file\n");
	}
	wchar_t fileName[256];
	if (NO_ERROR == file.seekBlock("Group0Mission0"))
	{
		if (NO_ERROR != file.readIdString("FileName", fileName, 1023))
		{
			setMission("");
		}
		else
		{
			setMission(fileName);
		}
	}
	else
		setMission("");
}
void
LogisticsSaveDialog::updateMissionInfo()
{
	if (!selectedName.Length())
	{
		statics[MAP_STATIC].setcolour(0);
		return;
	}
	if (bCampaign)
	{
		updateCampaignMissionInfo();
		return;
	}
	FullPathFileName fileName;
	fileName.init(savePath, selectedName, ".fit");
	FitIniFile file;
	if (NO_ERROR == file.open(fileName))
	{
		wchar_t tmp[256];
		wchar_t tmp2[256];
		wchar_t real[1024];
		file.seekBlock("General");
		tmp[0] = 0;
		file.readIdString("CampaignName", tmp, 255);
		_splitpath(tmp, nullptr, nullptr, tmp2, nullptr);
		strcpy(real, tmp2);
		strcat(real, "\n");
		if (NO_ERROR == file.readIdString("MissionName", tmp, 255))
		{
			strcat(real, tmp);
			strcat(real, "\n");
		}
		if (NO_ERROR == file.readIdString("MissionFileName", tmp, 255))
		{
			int32_t textureHandle = MissionBriefingScreen::getMissionTGA(tmp);
			statics[MAP_STATIC].setTexture(textureHandle);
			statics[MAP_STATIC].setUVs(0, 127, 127, 0);
			statics[MAP_STATIC].setcolour(0xffffffff);
		}
		else
			statics[MAP_STATIC].setcolour(0);
		int32_t cBills;
		file.readIdLong("CBills", cBills);
		cLoadString(IDS_DIALOG_CBILLS, tmp2, 255);
		sprintf(tmp, tmp2, cBills);
		strcat(real, tmp);
		strcat(real, "\n");
		time_t time = file.getFileMTime();
		struct tm* ptm = localtime(&time);
		const std::wstring_view& timeStr = gos_GetFormattedTime(ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
		const std::wstring_view& dateStr = gos_GetFormattedDate(0, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
		cLoadString(IDS_DIALOG_TIME, tmp2, 255);
		sprintf(tmp, tmp2, dateStr, timeStr);
		strcat(real, tmp);
		textObjects[2].setText(real);
	}
	else
	{
		textObjects[2].setText("");
		statics[MAP_STATIC].setcolour(0);
	}
}

void
LogisticsSaveDialog::setMission(const std::wstring_view& fileName)
{
	if (strlen(fileName))
	{
		int32_t textureHandle = MissionBriefingScreen::getMissionTGA(fileName);
		statics[MAP_STATIC].setTexture(textureHandle);
		statics[MAP_STATIC].setUVs(0, 127, 127, 0);
		statics[MAP_STATIC].setcolour(0xffffffff);
		FullPathFileName path;
		FitIniFile file;
		path.init(missionPath, fileName, ".fit");
		if (NO_ERROR == file.open(path))
		{
			wchar_t missionName[256];
			missionName[0] = 0;
			bool bRes = 0;
			file.seekBlock("MissionSettings");
			file.readIdBoolean("MissionNameUseResourceString", bRes);
			if (bRes)
			{
				uint32_t lRes;
				file.readIdULong("MissionNameResourceStringID", lRes);
				cLoadString(lRes, missionName, 255);
			}
			else
			{
				file.readIdString("MissionName", missionName, 255);
			}
			textObjects[2].setText(missionName);
		}
		else
		{
			textObjects[2].setText("");
		}
	}
	else
	{
		statics[MAP_STATIC].setcolour(0);
		textObjects[2].setText("");
	}
}
void
LogisticsSaveDialog::render()
{
	float xOffset = 0;
	float yOffset = 0;
	if (enterAnim.isAnimating())
	{
		xOffset = enterAnim.getXDelta();
		yOffset = enterAnim.getYDelta();
	}
	else if (exitAnim.isAnimating())
	{
		xOffset = exitAnim.getXDelta();
		yOffset = exitAnim.getYDelta();
	}
	float color = 0x7f000000;
	if (enterAnim.isAnimating() && !enterAnim.isDone())
	{
		float time = enterAnim.getCurrentTime();
		float endTime = enterAnim.getMaxTime();
		if (endTime)
		{
			color = interpolatecolour(0x00000000, 0x7f000000, time / endTime);
		}
	}
	else if (exitAnim.isAnimating() && !exitAnim.isDone() && !bLoad)
	{
		float time = exitAnim.getCurrentTime();
		float endTime = exitAnim.getMaxTime();
		if (endTime)
		{
			color = interpolatecolour(0x7f000000, 0x00000000, time / endTime);
		}
	}
	if (fadeOutTime)
		color = 0;
	RECT rect = {0, 0, Environment.screenwidth, Environment.screenheight};
	drawRect(rect, color);
	if (xOffset || yOffset)
	{
		gameListBox.move(xOffset, yOffset);
		gameListBox.render();
		gameListBox.move(-xOffset, -yOffset);
	}
	else
	{
		gameListBox.render();
	}
	LogisticsScreen::render((int32_t)xOffset, (int32_t)yOffset);
	if (bPromptOverwrite || bDeletePrompt)
	{
		LogisticsOKDialog::instance()->render();
	}
}

int32_t
LogisticsSaveDialog::handleMessage(uint32_t what, uint32_t who)
{
	if (YES == who)
	{
		const std::wstring_view& str;
		edits[0].getEntry(str);
		bool bFound = 0;
		// look and see if you are overwriting anything here...
		for (size_t i = 0; i < gameListBox.GetItemCount(); i++)
		{
			if (str.Compare(((aLocalizedListItem*)gameListBox.GetItem(i))->getText(), 0) == 0)
			{
				// do prompt here
				selectedName = ((aLocalizedListItem*)gameListBox.GetItem(i))->getHiddenText();
				wchar_t str[256];
				cLoadString(IDS_DIALOG_OVERWRITE_PROMPT, str, 255);
				wchar_t promptString[256];
				sprintf(promptString, str, (const std::wstring_view&)selectedName);
				LogisticsOKDialog::instance()->setText(
					IDS_DIALOG_QUIT_PROMPT, IDS_DIALOG_NO, IDS_DIALOG_YES);
				LogisticsOKDialog::instance()->setText(promptString);
				LogisticsOKDialog::instance()->begin();
				bPromptOverwrite = true;
				bFound = 1;
				break;
			}
		}
		if ((!bFound || bLoad))
		{
			status = YES;
			bPromptOverwrite = 0;
			if (bLoad)
				beginFadeOut(1.0);
			// Selected Name is coming back in SAVE with the default savegame.
			// NOT with what I typed in the box.  Maybe this will fix?  -fs
			if (!bCampaign)
				selectedName = str;
			end();
		}
	}
	if (NO == who)
	{
		selectedName = "";
		status = NO;
		end();
	}
	if (DELETE_BUTTON == who)
	{
		if (gameListBox.GetItemCount())
		{
			const std::wstring_view& tmpName;
			edits[0].getEntry(tmpName);
			for (size_t i = 0; i < gameListBox.GetItemCount(); i++)
			{
				const std::wstring_view& pFileName = ((aLocalizedListItem*)gameListBox.GetItem(i))->getHiddenText();
				if (tmpName.Compare(pFileName, 0) == 0)
				{
					selectedName = pFileName;
					break;
				}
			}
			wchar_t str[256];
			cLoadString(IDS_DIALOG_DELETE_PROMPT, str, 255);
			wchar_t promptString[256];
			sprintf(promptString, str, (const std::wstring_view&)tmpName);
			LogisticsOKDialog::instance()->setText(
				IDS_DIALOG_QUIT_PROMPT, IDS_DIALOG_NO, IDS_DIALOG_YES);
			LogisticsOKDialog::instance()->setText(promptString);
			LogisticsOKDialog::instance()->begin();
			bDeletePrompt = true;
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

LogisticsVariantDialog::LogisticsVariantDialog() :
	templateItem(IDS_DIALOG_LB_FONT)
{
	if (!s_instance)
		s_instance = this;
}

//-------------------------------------------------------------------------------------------------

LogisticsVariantDialog::~LogisticsVariantDialog()
{
	gameListBox.destroy();
}

//-------------------------------------------------------------------------------------------------

int32_t
LogisticsVariantDialog::init(FitIniFile& file)
{
	LogisticsScreen::init(file, "Static", "Text", "Rect", "Button", "Edit");
	file.seekBlock("InAnim");
	enterAnim.init(&file, "");
	file.seekBlock("OutAnim");
	exitAnim.init(&file, "");
	gameListBox.init(rects[1].globalX(), rects[1].globalY(), rects[1].width(), rects[1].height());
	gameListBox.setOrange(true);
	for (size_t i = 0; i < buttonCount; i++)
		buttons[i].setMessageOnRelease();
	edits[0].limitEntry(14);
	templateItem.init(file, "AnimText");
	for (i = 0; i < editCount; i++)
	{
		edits[i].allowWierdChars(0);
	}
	return 0;
}

void
LogisticsVariantDialog::begin()
{
	edits[0].allowIME(false);
	edits[0].setFocus(true);
	edits[0].setEntry("");
	initVariantList();
	status = RUNNING;
	bPromptOverwrite = 0;
	bDeletePrompt = 0;
	bTranscript = 0;
	// change all the texts
	textObjects[0].setText(IDS_DIALOG_VARIANT_SAVE);
	textObjects[1].setText(IDS_DIALOG_VARIANT_LIST);
	buttons[2].setText(IDS_DIALOG_SAVE);
	LogisticsDialog::begin();
}

void
LogisticsVariantDialog::beginTranscript()
{
	edits[0].allowIME(true);
	edits[0].setFocus(true);
	status = RUNNING;
	bPromptOverwrite = 0;
	bDeletePrompt = 0;
	bTranscript = 1;
	initTranscript();
	// change all the texts
	textObjects[0].setText(IDS_DIALOG_TRANSCRIPT_SAVE);
	textObjects[1].setText(IDS_DIALOG_TRANSCRIPT_LIST);
	buttons[2].setText(IDS_DIALOG_SAVE);
	LogisticsDialog::begin();
}

void
LogisticsVariantDialog::initTranscript()
{
	wchar_t findString[512];
	sprintf(findString, "%s*.txt", "data\\multiplayer\\transcripts\\");
	gameListBox.removeAllItems(true);
	WIN32_FIND_DATA findResult;
	HANDLE searchHandle = FindFirstFile(findString, &findResult);
	if (searchHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((findResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 && _stricmp(findResult.cFileName, "transcript.txt") != 0)
			{
				aAnimTextListItem* pEntry = new aAnimTextListItem(IDS_DIALOG_LIST_FONT);
				*pEntry = s_instance->templateItem;
				pEntry->setText(findResult.cFileName);
				pEntry->setcolour(edits[0].getcolour());
				gameListBox.AddItem(pEntry);
			}
		} while (FindNextFile(searchHandle, &findResult) != 0);
	}
	edits[0].setEntry("");
}
void
LogisticsVariantDialog::initVariantList()
{
	gameListBox.removeAllItems(true);
	int32_t count = 0;
	LogisticsData::instance->getPlayerVariantNames(nullptr, count);
	if (count)
	{
		const std::wstring_view&* pNames = (const std::wstring_view&*)_alloca(count * sizeof(const std::wstring_view&));
		LogisticsData::instance->getPlayerVariantNames(pNames, count);
		int32_t bValid = 0;
		for (size_t i = 0; i < count; i++)
		{
			aAnimTextListItem* pEntry = new aAnimTextListItem(IDS_DIALOG_LIST_FONT);
			*pEntry = s_instance->templateItem;
			pEntry->setText(pNames[i]);
			pEntry->setcolour(edits[0].getcolour());
			gameListBox.AddItem(pEntry);
			if (!LogisticsData::instance->canReplaceVariant(pNames[i]))
			{
				pEntry->setState(aListItem::DISABLED);
				pEntry->setcolour(0xff373737);
			}
			else if (!bValid)
			{
				pEntry->select();
				edits[0].setEntry(((aTextListItem*)pEntry)->getText());
				selectedName = (((aTextListItem*)pEntry)->getText());
				bValid = true;
			}
		}
	}
}

void
LogisticsVariantDialog::end()
{
	LogisticsDialog::end();
	bTranscript = 0;
}
void
LogisticsVariantDialog::update()
{
	if (bPromptOverwrite)
	{
		LogisticsOKDialog::instance()->update();
		if (LogisticsOKDialog::instance()->isDone())
		{
			if (YES == LogisticsOKDialog::instance()->getStatus())
			{
				status = YES;
				end();
				bPromptOverwrite = 0;
			}
			else if (NO == LogisticsOKDialog::instance()->getStatus())
			{
				bPromptOverwrite = 0;
			}
		}
		return;
	}
	else if (bDeletePrompt)
	{
		LogisticsOKDialog::instance()->update();
		if (YES == LogisticsOKDialog::instance()->getStatus())
		{
			if (bTranscript)
			{
				FullPathFileName path;
				path.init("data\\multiplayer\\transcripts\\", selectedName, ".txt");
				DeleteFile(path);
				initTranscript();
			}
			else
			{
				LogisticsData::instance->removeVariant(selectedName);
				initVariantList();
				edits[0].getEntry(selectedName);
			}
			bDeletePrompt = 0;
			edits[0].setEntry("");
		}
		else if (NO == LogisticsOKDialog::instance()->getStatus())
		{
			bDeletePrompt = 0;
		}
		return;
	}
	LogisticsDialog::update();
	gameListBox.update();
	if (userInput->isLeftClick())
	{
		if (gameListBox.pointInside(userInput->getMouseX(), userInput->getMouseY()))
		{
			// get selected item
			int32_t item = gameListBox.GetSelectedItem();
			if (item != -1)
			{
				for (size_t i = 0; i < gameListBox.GetItemCount(); i++)
				{
					gameListBox.GetItem(i)->setcolour(edits[0].getcolour());
				}
				gameListBox.GetItem(item)->setcolour(edits[0].getHighlightcolour());
				const std::wstring_view& text = ((aTextListItem*)gameListBox.GetItem(item))->getText();
				edits[0].setEntry(text);
				selectedName = text;
			}
		}
		else if (edits[0].pointInside(userInput->getMouseX(), userInput->getMouseY()))
		{
			gameListBox.SelectItem(-1);
			for (size_t i = 0; i < gameListBox.GetItemCount(); i++)
			{
				gameListBox.GetItem(i)->setcolour(edits[0].getcolour());
			}
		}
	}
	const std::wstring_view& text;
	edits[0].getEntry(text);
	if ((!bTranscript && !LogisticsData.instance->canReplaceVariant(text)) || !text.Length())
	{
		buttons[2].disable(1);
	}
	else
	{
		buttons[2].disable(0);
	}
	if (bTranscript || LogisticsData::instance->canDeleteVariant(text))
	{
		if (bTranscript)
		{
			if (!text.Length())
				buttons[0].disable(true);
			else
			{
				FullPathFileName path;
				path.init("data\\multiplayer\\transcripts\\", text, ".txt");
				if (fileExists(path))
				{
					buttons[0].disable(0);
				}
				else
					buttons[0].disable(1);
			}
		}
		else
			buttons[0].disable(0);
	}
	else
		buttons[0].disable(1);
}

void
LogisticsVariantDialog::render()
{
	float color = 0x7f000000;
	if (enterAnim.isAnimating() && !enterAnim.isDone())
	{
		float time = enterAnim.getCurrentTime();
		float endTime = enterAnim.getMaxTime();
		if (endTime)
		{
			color = interpolatecolour(0x00000000, 0x7f000000, time / endTime);
		}
	}
	else if (exitAnim.isAnimating() && !exitAnim.isDone())
	{
		float time = exitAnim.getCurrentTime();
		float endTime = exitAnim.getMaxTime();
		if (endTime)
		{
			color = interpolatecolour(0x7f000000, 0x00000000, time / endTime);
		}
	}
	RECT rect = {0, 0, Environment.screenwidth, Environment.screenheight};
	drawRect(rect, color);
	float xOffset = 0;
	float yOffset = 0;
	if (enterAnim.isAnimating())
	{
		xOffset = enterAnim.getXDelta();
		yOffset = enterAnim.getYDelta();
	}
	else if (exitAnim.isAnimating())
	{
		xOffset = exitAnim.getXDelta();
		yOffset = exitAnim.getYDelta();
		if (exitAnim.isDone())
			bDone = true;
	}
	gameListBox.move(xOffset, yOffset);
	gameListBox.render();
	gameListBox.move(-xOffset, -yOffset);
	LogisticsScreen::render((int32_t)xOffset, (int32_t)yOffset);
	if (bPromptOverwrite || bDeletePrompt)
	{
		LogisticsOKDialog::instance()->render();
	}
}

int32_t
LogisticsVariantDialog::handleMessage(uint32_t what, uint32_t who)
{
	if (YES == who)
	{
		edits[0].getEntry(selectedName);
		bool bFound = 0;
		// look and see if you are overwriting anything here...
		for (size_t i = 0; i < gameListBox.GetItemCount(); i++)
		{
			if (selectedName.Compare(((aTextListItem*)gameListBox.GetItem(i))->getText(), 0) == 0)
			{
				// do prompt here
				wchar_t str[256];
				cLoadString(IDS_DIALOG_OVERWRITE_PROMPT, str, 255);
				wchar_t promptString[256];
				sprintf(promptString, str, (const std::wstring_view&)selectedName);
				LogisticsOKDialog::instance()->setText(
					IDS_DIALOG_QUIT_PROMPT, IDS_DIALOG_NO, IDS_DIALOG_YES);
				LogisticsOKDialog::instance()->setText(promptString);
				LogisticsOKDialog::instance()->begin();
				bPromptOverwrite = true;
				bFound = 1;
				break;
			}
		}
		if (!bFound)
		{
			status = YES;
			end();
		}
	}
	if (NO == who)
	{
		selectedName = "";
		status = NO;
		end();
	}
	if (DELETE_BUTTON == who)
	{
		edits[0].getEntry(selectedName);
		wchar_t str[256];
		cLoadString(IDS_DIALOG_DELETE_PROMPT, str, 255);
		wchar_t promptString[256];
		sprintf(promptString, str, (const std::wstring_view&)selectedName);
		LogisticsOKDialog::instance()->setText(
			IDS_DIALOG_QUIT_PROMPT, IDS_DIALOG_NO, IDS_DIALOG_YES);
		LogisticsOKDialog::instance()->setText(promptString);
		LogisticsOKDialog::instance()->begin();
		bDeletePrompt = true;
	}
	return 0;
}

LogisticsAcceptVariantDialog::LogisticsAcceptVariantDialog() {}
LogisticsAcceptVariantDialog::~LogisticsAcceptVariantDialog() {}
void
LogisticsAcceptVariantDialog::begin()
{
	status = RUNNING;
	bPromptOverwrite = 0;
	bDeletePrompt = 0;
	edits[0].allowIME(false);
	edits[0].setFocus(true);
	LogisticsDialog::begin();
}
void
LogisticsAcceptVariantDialog::end()
{
	LogisticsVariantDialog::end();
}
void
LogisticsAcceptVariantDialog::update()
{
	LogisticsDialog::update();
	buttons[1].disable(0);
	if (bPromptOverwrite)
	{
		LogisticsOKDialog::instance()->update();
		if (YES == LogisticsOKDialog::instance()->getStatus())
		{
			status = YES;
			bPromptOverwrite = 0;
		}
		else if (NO == LogisticsOKDialog::instance()->getStatus())
		{
			bPromptOverwrite = 0;
		}
	}
	else if (bDeletePrompt)
	{
		LogisticsOKDialog::instance()->update();
		if (YES == LogisticsOKDialog::instance()->getStatus())
		{
			LogisticsData::instance->removeVariant(selectedName);
			initVariantList();
			bDeletePrompt = 0;
			edits[0].getEntry(selectedName);
			edits[0].setEntry("");
		}
		else if (NO == LogisticsOKDialog::instance()->getStatus())
		{
			bDeletePrompt = 0;
		}
	}
	gameListBox.update();
	if (userInput->isLeftClick())
	{
		if (gameListBox.pointInside(userInput->getMouseX(), userInput->getMouseY()))
		{
			// get selected item
			int32_t item = gameListBox.GetSelectedItem();
			if (item != -1)
			{
				for (size_t i = 0; i < gameListBox.GetItemCount(); i++)
				{
					gameListBox.GetItem(i)->setcolour(edits[0].getcolour());
				}
				gameListBox.GetItem(item)->setcolour(edits[0].getHighlightcolour());
				const std::wstring_view& text = ((aTextListItem*)gameListBox.GetItem(item))->getText();
				edits[0].setEntry(text);
				selectedName = text;
			}
		}
		else if (edits[0].pointInside(userInput->getMouseX(), userInput->getMouseY()))
		{
			gameListBox.SelectItem(-1);
			for (size_t i = 0; i < gameListBox.GetItemCount(); i++)
			{
				gameListBox.GetItem(i)->setcolour(edits[0].getcolour());
			}
		}
	}
	const std::wstring_view& text;
	edits[0].getEntry(text);
	if (!LogisticsData.instance->canReplaceVariant(text))
	{
		buttons[1].disable(1);
	}
	else
	{
		buttons[1].disable(0);
	}
}
void
LogisticsAcceptVariantDialog::render()
{
	float color = 0x7f000000;
	if (enterAnim.isAnimating() && !enterAnim.isDone())
	{
		float time = enterAnim.getCurrentTime();
		float endTime = enterAnim.getMaxTime();
		if (endTime)
		{
			color = interpolatecolour(0x00000000, 0x7f000000, time / endTime);
		}
	}
	else if (exitAnim.isAnimating() && !exitAnim.isDone())
	{
		float time = enterAnim.getCurrentTime();
		float endTime = enterAnim.getMaxTime();
		if (endTime)
		{
			color = interpolatecolour(0x7f000000, 0x00000000, time / endTime);
		}
	}
	RECT rect = {0, 0, Environment.screenwidth, Environment.screenheight};
	drawRect(rect, color);
	float xOffset = 0;
	float yOffset = 0;
	if (enterAnim.isAnimating())
	{
		xOffset = enterAnim.getXDelta();
		yOffset = enterAnim.getYDelta();
	}
	else if (exitAnim.isAnimating())
	{
		xOffset = enterAnim.getXDelta();
		yOffset = enterAnim.getYDelta();
		if (exitAnim.isDone())
			bDone = true;
	}
	LogisticsScreen::render((int32_t)xOffset, (int32_t)yOffset);
	if (bPromptOverwrite || bDeletePrompt)
	{
		LogisticsOKDialog::instance()->render();
	}
}

int32_t
LogisticsAcceptVariantDialog::init(FitIniFile& file)
{
	LogisticsScreen::init(file, "Static", "Text", "Rect", "Button", "Edit");
	file.seekBlock("InAnim");
	enterAnim.init(&file, "");
	file.seekBlock("OutAnim");
	exitAnim.init(&file, "");
	for (size_t i = 0; i < buttonCount; i++)
		buttons[i].setMessageOnRelease();
	edits[0].limitEntry(14);
	for (i = 0; i < editCount; i++)
	{
		edits[i].allowWierdChars(0);
	}
	return 0;
}

int32_t
LogisticsAcceptVariantDialog::handleMessage(uint32_t p1, uint32_t p2)
{
	return LogisticsVariantDialog::handleMessage(p1, p2);
}
//-------------------------------------------------------------------------------------------------

LogisticsMapInfoDialog::LogisticsMapInfoDialog() {}
LogisticsMapInfoDialog::~LogisticsMapInfoDialog() {}
void
LogisticsMapInfoDialog::end()
{
	statics[10].setTexture((uint32_t)0);
	statics[10].setcolour((int32_t)0);
}
void
LogisticsMapInfoDialog::setMap(const std::wstring_view& pFileName)
{
	int32_t textureHandle = MissionBriefingScreen::getMissionTGA(pFileName);
	statics[10].setTexture(textureHandle);
	statics[10].setUVs(0, 127, 127, 0);
	statics[10].setcolour(0xffffffff);
	// need to set all the map info too....
	FitIniFile file;
	FullPathFileName path;
	path.init(missionPath, pFileName, ".fit");
	if (NO_ERROR == file.open(path))
	{
		wchar_t missionName[256];
		wchar_t text[1024];
		missionName[0] = 0;
		wchar_t text2[1024];
		bool bRes = 0;
		wchar_t totalText[1024];
		file.seekBlock("MissionSettings");
		file.readIdBoolean("MissionNameUseResourceString", bRes);
		if (bRes)
		{
			uint32_t lRes;
			file.readIdULong("MissionNameResourceStringID", lRes);
			cLoadString(lRes, missionName, 255);
		}
		else
		{
			file.readIdString("MissionName", missionName, 255);
		}
		cLoadString(IDS_MP_LM_MAP_LIST_MAP_NAME, text, 255);
		sprintf(text2, text, missionName);
		sprintf(totalText, text2);
		strcat(totalText, "\n");
		uint32_t type = 0;
		file.readIdULong("MissionType", type);
		cLoadString(IDS_MP_LM_MAP_LIST_TYPE, text, 255);
		wchar_t mType[128];
		cLoadString(IDS_MP_LM_TYPE0 + type, mType, 127);
		sprintf(text2, text, mType);
		strcat(totalText, text2);
		strcat(totalText, "\n");
		uint32_t numPlayers = 2;
		file.readIdULong("MaximumNumberOfPlayers", numPlayers);
		cLoadString(IDS_MP_LM_MAP_LIST_MAX_PLAYERS, text, 255);
		sprintf(text2, text, numPlayers);
		strcat(totalText, text2);
		strcat(totalText, "\n");
		textObjects[1].setText(totalText);
		wchar_t blurb[1024];
		int32_t result = file.readIdString("Blurb2", blurb, 1023);
		bool tmpBool = false;
		result = file.readIdBoolean("Blurb2UseResourceString", tmpBool);
		if (NO_ERROR == result && tmpBool)
		{
			uint32_t tmpInt = 0;
			result = file.readIdULong("Blurb2ResourceStringID", tmpInt);
			if (NO_ERROR == result)
			{
				cLoadString(tmpInt, blurb, 1024);
			}
		}
		textObjects[2].setText(blurb);
	}
}

int32_t
LogisticsMapInfoDialog::init()
{
	FitIniFile file;
	FullPathFileName path;
	path.init(artPath, "mcl_mp_mapinfo", ".fit");
	if (NO_ERROR != file.open(path))
	{
		wchar_t buffer2[512];
		sprintf(buffer2, "couldn't open file %s", (const std::wstring_view&)path);
		Assert(0, 0, buffer2);
		return false;
	}
	LogisticsScreen::init(file, "Static", "Text", "Rect", "Button", "Edit");
	file.seekBlock("InAnim");
	enterAnim.init(&file, "");
	file.seekBlock("OutAnim");
	exitAnim.init(&file, "");
	return 1;
}

int32_t
LogisticsMapInfoDialog::handleMessage(uint32_t, uint32_t who)
{
	exitAnim.begin();
	enterAnim.end();
	status = who;
	return 1;
}
//-------------------------------------------------------------------------------------------------

// end of file ( LogisticsDialog.cpp )
