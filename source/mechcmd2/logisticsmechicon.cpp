#define LOGISTICSMECHICON_CPP
//===========================================================================//
//LogisticsMechIcon.cpp	: Implementation of the LogisticsMechIcon component. //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdafx.h"

#include "LogisticsMechIcon.h"
#include "MechIcon.h"
#include "LogisticsMech.h"
#include "LogisticsPilot.h"
#include "..\resource.h"
#include <mechgui/alistbox.h>
#include "LogisticsPilotListBox.h"
#include "gameSound.h"

LogisticsMechIcon*	LogisticsMechIcon::s_pTemplateIcon = nullptr;


LogisticsMechIcon::LogisticsMechIcon( )
{
	if( s_pTemplateIcon )
		*this = *s_pTemplateIcon;

	pMech = 0;
	state = 0;
	bJustSelected = 0;
	bDisabled = 0;
}

LogisticsMechIcon& LogisticsMechIcon::operator =( const LogisticsMechIcon& src )
{
	if ( &src != this )
	{
		pilotName = src.pilotName;
		chassisName = src.chassisName;
		iconConnector = src.iconConnector;
		icon = src.icon;
		pilotIcon = src.pilotIcon;
		outline = src.outline;
		for ( int32_t i = 0; i < ICON_ANIM_COUNT; i++ )
		{
			animations[i] = src.animations[i];
		}

		pilotID = src.pilotID;
		chassisNameID = src.chassisNameID;
		iconConnectorID = src.iconConnectorID;
		iconID = src.iconID;
		pilotIconID = src.pilotIconID;
		outlineID = src.outlineID;
		helpID = src.helpID;

	}

	return *this;
}

//-------------------------------------------------------------------------------------------------

LogisticsMechIcon::~LogisticsMechIcon()
{

}



int32_t LogisticsMechIcon::init( FitIniFile& file )
{
	if ( !s_pTemplateIcon )
	{
		s_pTemplateIcon = new LogisticsMechIcon;
		s_pTemplateIcon->pilotName.init( &file, "PilotNameText" );
		assignAnimation( file, s_pTemplateIcon->pilotID );

		s_pTemplateIcon->chassisName.init( &file, "MechNameText" );
		assignAnimation( file, s_pTemplateIcon->chassisNameID );

		s_pTemplateIcon->iconConnector.init( &file, "IconConnector" );
		assignAnimation( file, s_pTemplateIcon->iconConnectorID );

		s_pTemplateIcon->outline.init( &file, "BoxOutline" );
		assignAnimation( file, s_pTemplateIcon->outlineID );

		s_pTemplateIcon->icon.init( &file, "MechEntryIcon" );
		assignAnimation( file, s_pTemplateIcon->iconID );

		s_pTemplateIcon->pilotIcon.init( &file, "PilotIcon" );
		assignAnimation( file, s_pTemplateIcon->pilotIconID );



		char blockName[64];
		for ( int32_t i = 0; i < ICON_ANIM_COUNT; i++ )
		{
			sprintf( blockName, "Animation%ld", i );
			s_pTemplateIcon->animations[i].init( &file, blockName );
		}
	}
	

	return true;
}

void LogisticsMechIcon::assignAnimation( FitIniFile& file, int32_t& number )
{
	number = -1;
	char buffer[64];
	if ( NO_ERROR == file.readIdString( "Animation", buffer, 63 ) )
	{
		for ( int32_t i = 0; i < strlen( buffer ); i++ )
		{
			if ( isdigit( buffer[i] ) )
			{
				buffer[i+1] = 0;
				number = atoi( &buffer[i] );
			}
		}
	}
}

void LogisticsMechIcon::setMech( LogisticsMech* pNewMech )
{
	pMech = pNewMech;

	if ( pMech )
	{
		// need to set the uv's of the mech icon
		int32_t index = pMech->getIconIndex();
		int32_t xIndex = index % 10;
		int32_t yIndex = index / 10;

		float fX = xIndex;
		float fY = yIndex;

		float width = icon.width();
		float height = icon.height();

		float u = (fX * width);
		float v = (fY * height);

		fX += 1.f;
		fY += 1.f;
		
		float u2 = (fX * width);
		float v2 = (fY * height);

		icon.setFileWidth(256.f);
		icon.setUVs( u, v, u2, v2 );

		chassisName.setText( pMech->getChassisName() );

		iconConnector.showGUIWindow( true );

		if ( pMech->getPilot() )
		{
			pilotName.setText( pMech->getPilot()->getName() );
			
		}
		else
		{
			pilotName.setText( IDS_NOPILOT );
			
		}
	}
	else
		iconConnector.showGUIWindow( 0 );

		

}

void LogisticsMechIcon::render(int32_t xOffset, int32_t yOffset )
{

	if ( bDisabled )
	{
		RECT tmprect = { outline.left() + xOffset, outline.top() + yOffset,
			outline.right() + xOffset, outline.bottom() + yOffset };

		drawRect( tmprect, 0xff000000 );

		return;

	}

	if ( !pMech )
		return;


	int32_t color = animations[outlineID].getCurrentColor(animations[outlineID].getState());

	outline.setColor( color );
	outline.render(xOffset, yOffset);	

	xOffset += outline.globalX();
	yOffset += outline.globalY();

	
	renderObject( icon, iconID, xOffset, yOffset );
	renderObject( chassisName, chassisNameID, xOffset, yOffset );
	renderObject( pilotName, pilotID, xOffset, yOffset );
	renderObject( iconConnector, iconConnectorID, xOffset, yOffset );
	renderObject( icon, iconID, xOffset, yOffset );

	if ( pMech && pMech->getPilot() )
		pilotIcon.render(xOffset, yOffset );
	
}

void LogisticsMechIcon::renderObject( aObject& obj, int32_t animIndex, int32_t xOffset, int32_t yOffset )
{
	int32_t color = 0xffffffff;

	if ( animIndex != -1 )
	{
		color = animations[animIndex].getCurrentColor(animations[animIndex].getState());
	}

	obj.setColor( color );
	obj.render( xOffset, yOffset );
}

void LogisticsMechIcon::update()
{
	bJustSelected = 0;

	if ( !pMech )
		return;

	int32_t x = userInput->getMouseX();
	int32_t y = userInput->getMouseY();

	for ( int32_t i = 0; i < ICON_ANIM_COUNT; i++ )
	{
		animations[i].update();
	}

	if ( outline.pointInside( x, y ) )
	{
		if ( (userInput->isLeftClick() || userInput->isLeftDoubleClick())
			&& getMech() )
		{
		
			for ( int32_t i = 0; i < ICON_ANIM_COUNT; i++ )
			{
				animations[i].setState( aAnimGroup::PRESSED );
			}

			bJustSelected = true;
			if ( state != aListItem::SELECTED )
				soundSystem->playDigitalSample( LOG_SELECT );
			state = aListItem::SELECTED;
		}

		else if ( state == aListItem::ENABLED || state == aListItem::HIGHLITE )
		{
			for ( int32_t i = 0; i < ICON_ANIM_COUNT; i++ )
			{
				animations[i].setState( aAnimGroup::HIGHLIGHT );
			}
			if ( state != aListItem::HIGHLITE )
			{
				soundSystem->playDigitalSample( LOG_HIGHLIGHTBUTTONS );
			}
			state = aListItem::HIGHLITE;
		}

		::helpTextID = helpID;

	}
	else if ( state != aListItem::SELECTED )
	{
		for ( int32_t i = 0; i < ICON_ANIM_COUNT; i++ )
		{
			animations[i].setState( aAnimGroup::NORMAL );
		}

		state = aListItem::ENABLED;
	}


	
}

void LogisticsMechIcon::move( int32_t x, int32_t y )
{
	outline.move(x, y);	
}

void LogisticsMechIcon::select( bool bSelect )
{
	if ( !bSelect || bDisabled )
	{
		state = aListItem::ENABLED;
		for ( int32_t i = 0; i < ICON_ANIM_COUNT; i++ )
			animations[i].setState( aAnimGroup::NORMAL );
	}
	else
	{
		state = aListItem::SELECTED;
		for ( int32_t i = 0; i < ICON_ANIM_COUNT; i++ )
			animations[i].setState( aAnimGroup::PRESSED );

		
		bJustSelected = true;
	}
}

bool LogisticsMechIcon::isSelected()
{
	return state == aListItem::SELECTED;
}

LogisticsPilot* LogisticsMechIcon::getPilot()
{
	if ( pMech )
		return pMech->getPilot();

	return nullptr;
}

void LogisticsMechIcon::setPilot( LogisticsPilot* pPilot )
{
	if ( pMech )
	{
		pMech->setPilot( pPilot );
		
		if ( pPilot )
		{
			int32_t x = pilotIcon.globalX();
			int32_t y = pilotIcon.globalY();
			LogisticsPilotListBox::makeUVs( pPilot, pilotIcon );
			pilotIcon.moveTo( x, y );
			pilotName.setText( pPilot->getName() );
			pPilot->setUsed( true );

		}
		else
		{
			pilotIcon.setUVs( 0, 0, 0, 0 );
			pilotName.setText( IDS_NOPILOT );
		}
	}
	else
		pilotIcon.setUVs( 0, 0, 0, 0 );
}

void LogisticsMechIcon::dimPilot( bool bDim )
{
	if ( bDim )
	{
		pilotIcon.setColor( 0x7f000000 );
	}
	else
		pilotIcon.setColor( 0xffffffff );
}

//*************************************************************************************************
// end of file ( LogisticsMechIcon.cpp )
