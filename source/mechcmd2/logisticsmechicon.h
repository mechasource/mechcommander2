//===========================================================================//
//LogisticsMechIcon.h		: Interface for the LogisticsMechIcon component. //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef LOGISTICSMECHICON_H
#define LOGISTICSMECHICON_H

//#include <mechgui/asystem.h>
//#include <mechgui/aanim.h>

class LogisticsMech;
class LogisticsPilot;

#define ICON_ANIM_COUNT 4
/**************************************************************************************************
CLASS DESCRIPTION
LogisticsMechIcon:
**************************************************************************************************/
class LogisticsMechIcon
{

public:

	LogisticsMechIcon(void);
	~LogisticsMechIcon(void);
	
	static int32_t init( FitIniFile& file );

	void update(void);
	void render( int32_t offsetX, int32_t offsetY );

	void setMech( LogisticsMech* pMech );
	LogisticsMech* getMech( ){ return pMech; }
	LogisticsPilot* getPilot(void);
	void setPilot( LogisticsPilot* pPilot);

	float width(){ return outline.width(void); }
	float height(){ return outline.height(void); }
	void move( int32_t x, int32_t y );

	bool justSelected(){ return bJustSelected; }
	void select( bool bSelect );
	bool isSelected(void);

	int32_t globalX(void) const { return outline.globalX(void); }
	int32_t globalY(void) const { return outline.globalY(void); }
	float width(void) const { return outline.width(void); }
	float height(void) const { return outline.height(void); }

	bool pointInside(int32_t x, int32_t y) const { return outline.pointInside( x, y ); }

	void dimPilot( bool bDim );
	void setHelpID( int32_t newID ) { helpID = newID; }

	void disable( bool bdo) { bDisabled = bdo; }

	static LogisticsMechIcon*	s_pTemplateIcon;

private:
	
	LogisticsMechIcon& operator=( const LogisticsMechIcon& src );
	LogisticsMechIcon( const LogisticsMechIcon& src );

	aText		pilotName;
	aText		chassisName;
	aObject		iconConnector;
	aObject		icon;
	aObject		pilotIcon;
	aObject		outline;

	int32_t		pilotID;
	int32_t		chassisNameID;
	int32_t		iconConnectorID;
	int32_t		iconID;
	int32_t		pilotIconID;
	int32_t		outlineID;

	aAnimGroup	animations[ICON_ANIM_COUNT];

	LogisticsMech*	pMech;
	int32_t			state;
	bool			bJustSelected;
	int32_t			helpID;
	bool			bDisabled;

	void renderObject( aObject& obj, int32_t color, int32_t xOffset, int32_t yOffset );
	static void assignAnimation( FitIniFile& file, int32_t& number );




	
	
};


//*************************************************************************************************
#endif  // end of file ( LogisticsMechIcon.h )
