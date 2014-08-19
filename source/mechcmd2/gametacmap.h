/*************************************************************************************************\
gameTacMap.h			: Interface for the gameTacMap component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef GAMETACMAP_H
#define GAMETACMAP_H

#include "tacmap.h"

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
gameTacMap:
**************************************************************************************************/
class GameTacMap: public TacMap
{
	public:

	GameTacMap();
	~GameTacMap()
	{
		if (buildingPoints)
		{
			delete [] buildingPoints;
			buildingPoints = NULL;
		}
	}

	void init( PUCHAR bitmapData, int dataSize );
	void update(); // do not call these two functions from editor
	void render();
	
	bool animate (long objectiveId, long nFlashes);

	// if the world coords do not lie on the map, they will be changed.
	void worldToTacMap( Stuff::Vector3D& world, gos_VERTEX& tac );	


	bool inRegion( int x, int y ){ return x > left && x < right && y > top && y < bottom; }
	
	void initBuildings( PUCHAR data, int size );
	void setPos( const RECT& newPos );
	
	protected:

	int		top;	// position
	int		left;
	int		right;
	int		bottom;
	int		bmpWidth;
	int		bmpHeight;
	int		buildingCount;
	gos_VERTEX* buildingPoints;

	ULONG textureHandle;
	ULONG viewRectHandle;
	ULONG blipHandle;

	Stuff::Vector3D navMarkers[6];
	ULONG navMarkerCount;
	ULONG curNavMarker;

	void drawSensor( const Stuff::Vector3D& pos, float radius, long color);
	void drawBlip( const Stuff::Vector3D& pos, long color, int shape  );

	const static float s_blinkLength;
	static float		s_lastBlinkTime;

	//Tutorial
	long objectiveAnimationId;
	long objectiveNumFlashes;
	float objectiveFlashTime;

};


//*************************************************************************************************
#endif  // end of file ( gameTacMap.h )
