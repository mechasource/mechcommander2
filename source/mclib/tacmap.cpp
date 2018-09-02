#define TACMAP_CPP
/*************************************************************************************************\
TacMap.cpp			: Implementation of the TacMap component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include <tacmap.h>
#include "TGAInfo.h"
#include "gameOS.hpp"

extern uint8_t godMode;

TacMap::TacMap() {}

void TacMap::worldToTacMap(Stuff::Vector3D& world, int32_t xOffset,
	int32_t yOffset, int32_t xSize, int32_t ySize, gos_VERTEX& tac)
{
	int32_t tacX;
	int32_t tacY;
	land->worldToCell(world, tacY, tacX);
	if (tacX < 0)
		tacX = 0;
	if (tacY < 0)
		tacY = 0;
	if (tacX > land->realVerticesMapSide * terrain_const::MAPCELL_DIM)
		tacX = land->realVerticesMapSide * terrain_const::MAPCELL_DIM;
	if (tacY > land->realVerticesMapSide * terrain_const::MAPCELL_DIM)
		tacY = land->realVerticesMapSide * terrain_const::MAPCELL_DIM;
	tac.x = (float)tacX * (float)xSize /
			((float)land->realVerticesMapSide * terrain_const::MAPCELL_DIM);
	tac.y = (float)tacY * (float)ySize /
			((float)land->realVerticesMapSide * terrain_const::MAPCELL_DIM);
	tac.x += xOffset;
	tac.y += yOffset;
}

void TacMap::tacMapToWorld(const Stuff::Vector2DOf<int32_t>& screen,
	int32_t xSize, int32_t ySize, Stuff::Vector3D& world)
{
	// turn screen into cells
	int32_t cellX = screen.x / (float)xSize *
					((float)land->realVerticesMapSide * terrain_const::MAPCELL_DIM);
	int32_t cellY = screen.y / (float)ySize *
					((float)land->realVerticesMapSide * terrain_const::MAPCELL_DIM);
	if ((cellX < 0) || (cellX >= land->realVerticesMapSide * terrain_const::MAPCELL_DIM) ||
		(cellY < 0) || (cellY >= land->realVerticesMapSide * terrain_const::MAPCELL_DIM))
	{
		world = eye->getPosition();
	}
	else
	{
		world.x = land->cellColToWorldCoord[cellX];
		world.y = land->cellRowToWorldCoord[cellY];
		world.z = land->getTerrainElevation(world);
	}
}
