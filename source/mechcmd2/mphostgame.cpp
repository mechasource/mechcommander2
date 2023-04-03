#define MPHOSTGAME_CPP
/*************************************************************************************************\
MPHostGame.cpp			: Implementation of the MPHostGame component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdinc.h"

#include "mphostgame.h"
#include "prefs.h"
#include "IniFile.h"
#include "userinput.h"
#include "resource.h"
#include "multplyr.h"

#include "gamesound.h"
#define CHECK_BUTTON 200

static int32_t connectionType = 0;

static const int32_t FIRST_BUTTON_ID = 1000010;
static const int32_t OK_BUTTON_ID = 1000001;
static const int32_t CANCEL_BUTTON_ID = 1000002;

extern CPrefs prefs;

MPHostGame::MPHostGame()
{
	bDone = 0;
	status = RUNNING;
}

MPHostGame::~MPHostGame() { }

int32_t
MPHostGame::indexOfButtonWithID(int32_t id)
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

void MPHostGame::init()
{
	FullPathFileName path;
	FitIniFile file;
	path.init(artPath, "mcl_mp_hostgamedialog", ".fit");
	if (NO_ERROR != file.open(path))
	{
		wchar_t error[256];
		sprintf(error, "couldn't open file %s", path);
		Assert(0, 0, error);
		return;
	}
	LogisticsScreen::init(file, "Static", "Text", "Rect", "Button");
	file.seekBlock("InAnim");
	enterAnim.init(&file, "");
	file.seekBlock("OutAnim");
	exitAnim.init(&file, "");
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
	edits[0].limitEntry(24);
	edits[0].allowIME(false);
	/*	int32_t tmpX, tmpY;
		file.seekBlock( "ComboBox0" );
		file.readIdLong( "XLocation", tmpX );
		file.readIdLong( "YLocation", tmpY );


		wchar_t tmpStr[256];
		file.readIdString( "filename", tmpStr, 255 );
		{
			FullPathFileName tmpPath;
			tmpPath.init( artPath, tmpStr, ".fit" );

			FitIniFile PNfile;
			if ( NO_ERROR != PNfile.open( tmpPath ) )
			{
				wchar_t error[256];
				sprintf( error, "couldn't open file %s", (std::wstring_view)tmpPath );
				Assert( 0, 0, error );
				return;
			}
			numPlayersDropList.init( &PNfile, "ComboBox0");

			aStyle5TextListItem *pTmp2;
			int32_t i;
			for (i = 2; i < MAX_MC_PLAYERS+1; i += 1)
			{
				pTmp2 = new aStyle5TextListItem;
				pTmp2->init(&PNfile, "Text0");
				wchar_t txt[32];
				sprintf( txt, "%ld", i );
				pTmp2->setText( txt );
				pTmp2->sizeToText();
				pTmp2->resize( numPlayersDropList.width() -
	   numPlayersDropList.ListBox().getScrollBarwidth() + 5, pTmp2->height() );
				numPlayersDropList.AddItem(pTmp2);
			}
			numPlayersDropList.SelectItem(0);
		}

		numPlayersDropList.moveTo( tmpX, tmpY );
		numPlayersDropList.ListBox().setOrange( true );
		numPlayersDropList.EditBox().limitEntry( 1 );
		numPlayersDropList.EditBox().setNumeric( 1 );
		numPlayersDropList.setReadOnly( 0 );*/
}

void MPHostGame::begin()
{
	status = RUNNING;
	LogisticsDialog::begin();
	bShowDlg = 0;
}

void MPHostGame::end()
{
	bShowDlg = 0;
	LogisticsDialog::end();
}

void MPHostGame::render(int32_t xOffset, int32_t yOffset)
{
	LogisticsDialog::render();
	if ((0 == xOffset) && (0 == yOffset) && enterAnim.isDone() && !exitAnim.isAnimating())
	{
		//		numPlayersDropList.render();
		for (size_t i = 0; i < staticCount; i++)
		{
			statics[i].render(); // need to cover up droplist overflow...
		}
	}
	if (bShowDlg)
	{
		LogisticsOneButtonDialog::instance()->render();
	}
}

void MPHostGame::render()
{
	render(0, 0);
}

int32_t
MPHostGame::handleMessage(uint32_t message, uint32_t who)
{
	status = who;
	exitAnim.begin();
	enterAnim.end();
	if (status == YES)
	{
		std::wstring_view tmp;
		//		numPlayersDropList.EditBox().getEntry(tmp);
		//		int32_t maxPlayers = atoi( tmp );
		edits[0].getEntry(tmp);
		MPlayer->setMode(MULTIPLAYER_MODE_PARAMETERS);
		if (!MPlayer->hostSession((std::wstring_view)(std::wstring_view)tmp, &prefs.playerName[0][0], 8))
		{
			MPlayer->setMode(MULTIPLAYER_MODE_NONE);
			// need to pop dlg here
			LogisticsOneButtonDialog::instance()->begin();
			LogisticsOneButtonDialog::instance()->setText(
				IDS_ERROR_NOT_CONNECTED, IDS_DIALOG_OK, IDS_DIALOG_OK);
			LogisticsOneButtonDialog::instance()->setFont(IDS_MP_CONNECT_ERROR_NO_CONNECTION_FONT);
			bShowDlg = true;
		}
	}
	return 0;
}

bool MPHostGame::isDone()
{
	return bDone;
}

void MPHostGame::update()
{
	if (bShowDlg)
	{
		LogisticsOneButtonDialog::instance()->update();
		if (LogisticsOneButtonDialog::instance()->isDone())
		{
			bShowDlg = 0;
			status = NO;
		}
		return;
	}
	LogisticsDialog::update();
	helpTextID = 0;
	helpTextHeaderID = 0;
	std::wstring_view tmp;
	edits[0].getEntry(tmp);
	int32_t len = tmp.Length();
	if (len >= 1)
	{
		getButton(YES)->disable(false);
	}
	else
		getButton(YES)->disable(true);
}

int32_t
aStyle5TextListItem::init(FitIniFile* file, std::wstring_view blockname)
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

void aStyle5TextListItem::render()
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

//////////////////////////////////////////////

// end of file ( MPHostGame.cpp )
