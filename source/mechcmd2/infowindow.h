/*************************************************************************************************\
InfoWindow.h			: Interface for the InfoWindow component.
//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef INFOWINDOW_H
#define INFOWINDOW_H

#include <mclib.h>
#include "controlgui.h"
#include <mechgui/afont.h>

class Mover;
class ForceGroupIcon;

#define SCROLLUP	1
#define SCROLLDOWN	2

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
InfoWindow:
**************************************************************************************************/
class InfoWindow
{
	public:

		InfoWindow();
		~InfoWindow();

		void setUnit( Mover* pNewUnit );
		void update();
		void render();

	private:

		float		scrollPos;
		float		scrollLength;
		float		infoLength;

		float		lastYClick;

		// DATA
		Mover*		pUnit;

		static ButtonData	buttonData[2];
		static StaticInfo*	skillInfos;

		uint32_t		backgroundTexture;
		aFont				nameFont;
		aFont				componentFont;

		static ControlButton		buttons[2];

		bool		bUnitChanged;

		friend class ControlGui;

		// HELPER FUNCTIONS
		void drawScrollingStuff();
		void handleClick( int32_t ID );
		void drawDivider( float yVal );
		void drawSkillBar( int32_t skill, float yVal, float height );
		void setScrollPos( int32_t where );
		void drawName( PCSTR name );

		ForceGroupIcon* icon;

		static  int32_t SCROLLLEFT;
		static  int32_t SCROLLRIGHT;
		static  int32_t SCROLLTOP;
		static  int32_t SCROLLBOTTOM;
		static int32_t	 SCROLLMIN;
		static int32_t SCROLLMAX;
		static int32_t SCROLLBUTTONU;
		static int32_t SCROLLBUTTONV;
		static int32_t SCROLLBUTTONWIDTH;
		static int32_t SCROLLBUTTONHEIGHT;
		static  int32_t SECTIONSKIP;
		static  int32_t NAMELEFT;
		static  int32_t NAMERIGHT;
		static  int32_t NAMETOP;
		static  int32_t NAMEBOTTOM;
		static  int32_t HEALTHLEFT;
		static	int32_t HEALTHRIGHT;
		static int32_t HEALTHTOP;
		static int32_t HEALTHBOTTOM;
		static int32_t DIVIDERCOLOR;
		static int32_t DIVIDERLEFT;
		static int32_t DIVIDERRIGHT;
		static int32_t	PILOTLEFT;
		static int32_t PILOTRIGHT;
		static int32_t PILOTHEIGHT;
		static int32_t MECHLEFT;
		static int32_t MECHRIGHT;
		static int32_t MECHHEIGHT;
		static int32_t MECHBACKLEFT;
		static int32_t MECHBACKRIGHT;
		static int32_t MECHBACKHEIGHT;
		static int32_t SKILLLEFT;
		static int32_t SKILLHEIGHT;
		static int32_t SKILLSKIP;
		static int32_t SKILLRIGHT;
		static int32_t SKILLUNITWIDTH;
		static int32_t NUMBERSKILLBARS;
		static int32_t INFOLEFT;
		static int32_t INFOTOP;
		static int32_t INFOWIDTH;
		static int32_t INFOHEIGHT;
		static int32_t SCROLLCOLOR;
		static int32_t SCROLLBUTTONX;
		static int32_t COMPONENTLEFT;
		static int32_t SCROLLBOXLEFT;
		static int32_t SCROLLBOXRIGHT;
		static int32_t SCROLLBOXTOP;
		static int32_t SCROLLBOXBOTTOM;
		static int32_t PILOTNAMELEFT;

		static RECT NameRect;

		static void InfoWindow::init( FitIniFile& file );

		static InfoWindow* s_instance;



};


//*************************************************************************************************
#endif  // end of file ( InfoWindow.h )
