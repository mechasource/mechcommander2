/*************************************************************************************************\
KeyboardRef.h			: Interface for the KeyboardRef component.
//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef KEYBOARDREF_H
#define KEYBOARDREF_H

#include <mechgui/logisticsscreen.h>
#include <mechgui/alistbox.h>
#include "missiongui.h"

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
KeyboardRef:
**************************************************************************************************/
class KeyboardRef: public LogisticsScreen
{
	public:

		KeyboardRef();
		virtual ~KeyboardRef();

		KeyboardRef& operator=( const KeyboardRef& eyboardRef );
		int32_t init();

		virtual void update();
		virtual void render();

		void reseed(MissionInterfaceManager::Command* );

		virtual int32_t			handleMessage( uint32_t, uint32_t );


	private:

		KeyboardRef( const KeyboardRef& src );
		// HELPER FUNCTIONS

		aListBox		listBox;
		aTextListItem	listItemTemplate;
		aTextListItem	listItemTemplate2;

};


//*************************************************************************************************
#endif  // end of file ( KeyboardRef.h )
