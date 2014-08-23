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

	LogisticsMechIcon();
	~LogisticsMechIcon();
	
	static int init( FitIniFile& file );

	void update();
	void render( int32_t offsetX, int32_t offsetY );

	void setMech( LogisticsMech* pMech );
	LogisticsMech* getMech( ){ return pMech; }
	LogisticsPilot* getPilot();
	void setPilot( LogisticsPilot* pPilot);

	float width(){ return outline.width(); }
	float height(){ return outline.height(); }
	void move( int32_t x, int32_t y );

	bool justSelected(){ return bJustSelected; }
	void select( bool bSelect );
	bool isSelected();

	int32_t globalX() const { return outline.globalX(); }
	int32_t globalY() const { return outline.globalY(); }
	float width() const { return outline.width(); }
	float height() const { return outline.height(); }

	bool pointInside(int32_t x, int32_t y) const { return outline.pointInside( x, y ); }

	void dimPilot( bool bDim );
	void setHelpID( int newID ) { helpID = newID; }

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
