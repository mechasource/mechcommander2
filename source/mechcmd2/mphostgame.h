#ifndef MPHOSTGAME_H
#define MPHOSTGAME_H
/*************************************************************************************************\
MPHostGame.h			: Interface for the MPHostGame component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

//*************************************************************************************************

#ifndef LOGISTICSDIALOG_H
#include "LogisticsDialog.h"
#endif

#include "aSystem.h"
#include "aListBox.h"
#include "AttributeMeter.h"
#include "SimpleCamera.h"

#ifndef AANIM_H
#include "aAnim.h"
#endif


class aStyle5TextListItem : public aTextListItem
{
public:
	aStyle5TextListItem() { hasAnimation = false; normalColor = 0xff808080; }
	virtual long	init( FitIniFile* file, const char* blockName );
	virtual void		render();

protected:
	bool hasAnimation;
	aAnimGroup animGroup;
	int normalColor;
};

class MPHostGame : public LogisticsDialog
{
public:
	
	MPHostGame();
	virtual ~MPHostGame();
	
	void init();
	bool isDone();
	virtual void		begin();
	virtual void		end();
 	virtual void render( int xOffset, int yOffset );
	virtual void render();
	virtual void update();
	virtual int			handleMessage( unsigned long, unsigned long );


private:
//	aEdit		nameEntry;
//	aRect		nameEntryOutline;
	bool					bShowDlg;

	int indexOfButtonWithID(int id);
};



//*************************************************************************************************
#endif  // end of file ( MPHostGame.h )
