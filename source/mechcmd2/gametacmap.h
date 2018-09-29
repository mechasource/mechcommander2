/*************************************************************************************************\
gameTacMap.h			: Interface for the gameTacMap component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef GAMETACMAP_H
#define GAMETACMAP_H

#include <tacmap.h>

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
gameTacMap:
**************************************************************************************************/
class GameTacMap : public TacMap
{
public:
	GameTacMap(void);
	~GameTacMap()
	{
		if (buildingPoints)
		{
			delete[] buildingPoints;
			buildingPoints = nullptr;
		}
	}

	void init(puint8_t bitmapData, size_t dataSize);
	void update(void); // do not call these two functions from editor
	void render(void);

	bool animate(int32_t objectiveId, int32_t nFlashes);

	// if the world coords do not lie on the map, they will be changed.
	void worldToTacMap(Stuff::Vector3D& world, gos_VERTEX& tac);

	bool inRegion(int32_t x, int32_t y) { return x > left && x < right && y > top && y < bottom; }

	void initBuildings(puint8_t data, int32_t size);
	void setPos(const RECT& newPos);

protected:
	int32_t top; // position
	int32_t left;
	int32_t right;
	int32_t bottom;
	int32_t bmpWidth;
	int32_t bmpHeight;
	int32_t buildingCount;
	gos_VERTEX* buildingPoints;

	uint32_t textureHandle;
	uint32_t viewRectHandle;
	uint32_t blipHandle;

	Stuff::Vector3D navMarkers[6];
	uint32_t navMarkerCount;
	uint32_t curNavMarker;

	void drawSensor(const Stuff::Vector3D& pos, float radius, int32_t color);
	void drawBlip(const Stuff::Vector3D& pos, int32_t color, int32_t shape);

	const static float s_blinkLength;
	static float s_lastBlinkTime;

	// Tutorial
	int32_t objectiveAnimationId;
	int32_t objectiveNumFlashes;
	float objectiveFlashTime;
};

//*************************************************************************************************
#endif // end of file ( gameTacMap.h )
