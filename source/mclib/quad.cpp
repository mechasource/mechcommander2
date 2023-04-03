//---------------------------------------------------------------------------
//
// Quad.cpp -- File contains class code for the Terrain Quads
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//---------------------------------------------------------------------------
// Include Files
#ifndef QUAD_H
#include "quad.h"
#endif

#ifndef VERTEX_H
#include "vertex.h"
#endif

#ifndef TERRAIN_H
#include "terrain.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef DBASEGUI_H
#include "dbasegui.h"
#endif

#ifndef CELINE_H
#include "celine.h"
#endif

#ifndef MOVE_H
#include "move.h"
#endif

#define SELECTION_COLOR 0xffff7fff
#define HIGHLIGHT_COLOR 0xff00ff00

//---------------------------------------------------------------------------
float TerrainQuad::rainLightLevel = 1.0f;
uint32_t TerrainQuad::lighteningLevel = 0;
uint32_t TerrainQuad::mineTextureHandle = 0xffffffff;
uint32_t TerrainQuad::blownTextureHandle = 0xffffffff;

extern bool drawTerrainGrid;
extern bool drawLOSGrid;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Class TerrainQuad
int32_t
TerrainQuad::init(VertexPtr v0, VertexPtr v1, VertexPtr v2, VertexPtr v3)
{
	vertices[0] = v0;
	vertices[1] = v1;
	vertices[2] = v2;
	vertices[3] = v3;
	return (NO_ERROR);
}

float twoFiveFive = 255.0;
#define HUD_DEPTH 0.0001f // HUD Objects draw over everything else.
extern float cosineEyeHalfFOV;
extern uint32_t BaseVertexcolour;
extern bool useShadows;
extern bool useFog;
extern int32_t sprayFrame;
bool useWaterInterestTexture = true;
bool useOverlayTexture = true;
int32_t numTerrainFaces = 0;
extern float MaxMinUV;

extern float leastZ;
extern float leastW;
extern float mostZ;
extern float mostW;
extern float leastWY;
extern float mostWY;

//---------------------------------------------------------------------------
void TerrainQuad::setupTextures(void)
{
	if (mineTextureHandle == 0xffffffff)
	{
		FullPathFileName mineTextureName;
		mineTextureName.init(texturePath, "defaults\\mine_00", ".tga");
		mineTextureHandle = mcTextureManager->loadTexture(
			mineTextureName, gos_Texture_Alpha, gosHint_DisableMipmap | gosHint_DontShrink);
	}
	if (blownTextureHandle == 0xffffffff)
	{
		FullPathFileName mineTextureName;
		mineTextureName.init(texturePath, "defaults\\minescorch_00", ".tga");
		blownTextureHandle = mcTextureManager->loadTexture(
			mineTextureName, gos_Texture_Alpha, gosHint_DisableMipmap | gosHint_DontShrink);
	}
	if (!Terrain::terrainTextures2)
	{
		if (uvMode == BOTTOMRIGHT)
		{
			int32_t clipped1 =
				vertices[0]->clipInfo + vertices[1]->clipInfo + vertices[2]->clipInfo;
			int32_t clipped2 =
				vertices[0]->clipInfo + vertices[2]->clipInfo + vertices[3]->clipInfo;
			if (clipped1 || clipped2)
			{
				{
					terrainHandle = Terrain::terrainTextures->getTextureHandle(
						(vertices[0]->pVertex->textureData & 0x0000ffff));
					uint32_t terrainDetailData = Terrain::terrainTextures->setDetail(1, 0);
					if (terrainDetailData != 0xfffffff)
						terrainDetailHandle =
							Terrain::terrainTextures->getTextureHandle(terrainDetailData);
					else
						terrainDetailHandle = 0xffffffff;
					overlayHandle = 0xffffffff;
					mcTextureManager->addTriangle(terrainHandle, MC2_ISTERRAIN | MC2_DRAWSOLID);
					mcTextureManager->addTriangle(terrainHandle, MC2_ISTERRAIN | MC2_DRAWSOLID);
					mcTextureManager->addTriangle(
						terrainDetailHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
					mcTextureManager->addTriangle(
						terrainDetailHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
				}
				//--------------------------------------------------------------------
				// Mine Information
				int32_t rowCol = vertices[0]->posTile;
				int32_t tileR = rowCol >> 16;
				int32_t tileC = rowCol & 0x0000ffff;
				if (GameMap)
				{
					mineResult.init();
					int32_t cellPos = 0;
					for (size_t cellR = 0; cellR < terrain_const::MAPCELL_DIM; cellR++)
					{
						for (size_t cellC = 0; cellC < terrain_const::MAPCELL_DIM;
							 cellC++, cellPos++)
						{
							int32_t actualCellRow = tileR * terrain_const::MAPCELL_DIM + cellR;
							int32_t actualCellCol = tileC * terrain_const::MAPCELL_DIM + cellC;
							uint32_t localResult = 0;
							if (GameMap->inBounds(actualCellRow, actualCellCol))
								localResult = GameMap->getMine(actualCellRow, actualCellCol);
							if (localResult == 1)
							{
								mcTextureManager->get_gosTextureHandle(mineTextureHandle);
								mcTextureManager->addTriangle(mineTextureHandle, MC2_DRAWALPHA);
								mcTextureManager->addTriangle(mineTextureHandle, MC2_DRAWALPHA);
								mineResult.setMine(cellPos, localResult);
							}
							else if (localResult == 2)
							{
								mcTextureManager->get_gosTextureHandle(blownTextureHandle);
								mcTextureManager->addTriangle(blownTextureHandle, MC2_DRAWALPHA);
								mcTextureManager->addTriangle(blownTextureHandle, MC2_DRAWALPHA);
								mineResult.setMine(cellPos, localResult);
							}
						}
					}
				}
			}
			else
			{
				terrainHandle = 0xffffffff;
				waterHandle = 0xffffffff;
				waterDetailHandle = 0xffffffff;
				terrainDetailHandle = 0xffffffff;
				overlayHandle = 0xffffffff;
			}
		}
		else
		{
			int32_t clipped1 =
				vertices[0]->clipInfo + vertices[1]->clipInfo + vertices[3]->clipInfo;
			int32_t clipped2 =
				vertices[1]->clipInfo + vertices[2]->clipInfo + vertices[3]->clipInfo;
			if (clipped1 || clipped2)
			{
				{
					terrainHandle = Terrain::terrainTextures->getTextureHandle(
						(vertices[0]->pVertex->textureData & 0x0000ffff));
					uint32_t terrainDetailData = Terrain::terrainTextures->setDetail(1, 0);
					if (terrainDetailData != 0xfffffff)
						terrainDetailHandle =
							Terrain::terrainTextures->getTextureHandle(terrainDetailData);
					else
						terrainDetailHandle = 0xffffffff;
					overlayHandle = 0xffffffff;
					mcTextureManager->addTriangle(terrainHandle, MC2_ISTERRAIN | MC2_DRAWSOLID);
					mcTextureManager->addTriangle(terrainHandle, MC2_ISTERRAIN | MC2_DRAWSOLID);
					mcTextureManager->addTriangle(
						terrainDetailHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
					mcTextureManager->addTriangle(
						terrainDetailHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
				}
				//--------------------------------------------------------------------
				// Mine Information
				int32_t rowCol = vertices[0]->posTile;
				int32_t tileR = rowCol >> 16;
				int32_t tileC = rowCol & 0x0000ffff;
				if (GameMap)
				{
					int32_t cellPos = 0;
					mineResult.init();
					for (size_t cellR = 0; cellR < terrain_const::MAPCELL_DIM; cellR++)
					{
						for (size_t cellC = 0; cellC < terrain_const::MAPCELL_DIM;
							 cellC++, cellPos++)
						{
							int32_t actualCellRow = tileR * terrain_const::MAPCELL_DIM + cellR;
							int32_t actualCellCol = tileC * terrain_const::MAPCELL_DIM + cellC;
							uint32_t localResult = 0;
							if (GameMap->inBounds(actualCellRow, actualCellCol))
								localResult = GameMap->getMine(actualCellRow, actualCellCol);
							if (localResult == 1)
							{
								mcTextureManager->get_gosTextureHandle(mineTextureHandle);
								mcTextureManager->addTriangle(mineTextureHandle, MC2_DRAWALPHA);
								mcTextureManager->addTriangle(mineTextureHandle, MC2_DRAWALPHA);
								mineResult.setMine(cellPos, localResult);
							}
							else if (localResult == 2)
							{
								mcTextureManager->get_gosTextureHandle(blownTextureHandle);
								mcTextureManager->addTriangle(blownTextureHandle, MC2_DRAWALPHA);
								mcTextureManager->addTriangle(blownTextureHandle, MC2_DRAWALPHA);
								mineResult.setMine(cellPos, localResult);
							}
						}
					}
				}
			}
			else
			{
				terrainHandle = 0xffffffff;
				waterHandle = 0xffffffff;
				waterDetailHandle = 0xffffffff;
				terrainDetailHandle = 0xffffffff;
				overlayHandle = 0xffffffff;
			}
		}
	}
	else // New single bitmap on the terrain.
	{
		if (uvMode == BOTTOMRIGHT)
		{
			int32_t clipped1 =
				vertices[0]->clipInfo + vertices[1]->clipInfo + vertices[2]->clipInfo;
			int32_t clipped2 =
				vertices[0]->clipInfo + vertices[2]->clipInfo + vertices[3]->clipInfo;
			if (clipped1 || clipped2)
			{
				isCement = Terrain::terrainTextures->isCement(
					vertices[0]->pVertex->textureData & 0x0000ffff);
				bool isAlpha = Terrain::terrainTextures->isAlpha(
					vertices[0]->pVertex->textureData & 0x0000ffff);
				if (!isCement)
				{
					terrainHandle = Terrain::terrainTextures2->getTextureHandle(
						vertices[0], vertices[2], &uvData);
					terrainDetailHandle = Terrain::terrainTextures2->getDetailHandle();
					overlayHandle = 0xffffffff;
					mcTextureManager->addTriangle(terrainHandle, MC2_ISTERRAIN | MC2_DRAWSOLID);
					mcTextureManager->addTriangle(terrainHandle, MC2_ISTERRAIN | MC2_DRAWSOLID);
					if (terrainDetailHandle != 0xffffffff)
					{
						mcTextureManager->addTriangle(
							terrainDetailHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
						mcTextureManager->addTriangle(
							terrainDetailHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
					}
				}
				else // This cement in the old Universe, pull the old texture
					// and display it
				{
					if (isAlpha) // This should be treated as an overlay.  Do
						// so.  Draw overlay AFTER new terrain in same
						// square!!
					{
						overlayHandle = Terrain::terrainTextures->getTextureHandle(
							vertices[0]->pVertex->textureData & 0x0000ffff);
						terrainHandle = Terrain::terrainTextures2->getTextureHandle(
							vertices[1], vertices[3], &uvData);
						terrainDetailHandle = Terrain::terrainTextures2->getDetailHandle();
						mcTextureManager->addTriangle(terrainHandle, MC2_ISTERRAIN | MC2_DRAWSOLID);
						mcTextureManager->addTriangle(terrainHandle, MC2_ISTERRAIN | MC2_DRAWSOLID);
						if (terrainDetailHandle != 0xffffffff)
						{
							mcTextureManager->addTriangle(
								terrainDetailHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
							mcTextureManager->addTriangle(
								terrainDetailHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
						}
						mcTextureManager->addTriangle(
							overlayHandle, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
						mcTextureManager->addTriangle(
							overlayHandle, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
					}
					else // Otherwise, its solid cement.  Save some draw
						// cycles!!
					{
						terrainHandle = Terrain::terrainTextures->getTextureHandle(
							vertices[0]->pVertex->textureData & 0x0000ffff);
						terrainDetailHandle = 0xffffffff; // Cement has NO detail!!
						overlayHandle = 0xffffffff;
						mcTextureManager->addTriangle(
							terrainHandle, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
						mcTextureManager->addTriangle(
							terrainHandle, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
					}
				}
				//--------------------------------------------------------------------
				// Mine Information
				int32_t rowCol = vertices[0]->posTile;
				int32_t tileR = rowCol >> 16;
				int32_t tileC = rowCol & 0x0000ffff;
				if (GameMap)
				{
					int32_t cellPos = 0;
					mineResult.init();
					for (size_t cellR = 0; cellR < terrain_const::MAPCELL_DIM; cellR++)
					{
						for (size_t cellC = 0; cellC < terrain_const::MAPCELL_DIM;
							 cellC++, cellPos++)
						{
							int32_t actualCellRow = tileR * terrain_const::MAPCELL_DIM + cellR;
							int32_t actualCellCol = tileC * terrain_const::MAPCELL_DIM + cellC;
							uint32_t localResult = 0;
							if (GameMap->inBounds(actualCellRow, actualCellCol))
								localResult = GameMap->getMine(actualCellRow, actualCellCol);
							if (localResult == 1)
							{
								mcTextureManager->get_gosTextureHandle(mineTextureHandle);
								mcTextureManager->addTriangle(mineTextureHandle, MC2_DRAWALPHA);
								mcTextureManager->addTriangle(mineTextureHandle, MC2_DRAWALPHA);
								mineResult.setMine(cellPos, localResult);
							}
							else if (localResult == 2)
							{
								mcTextureManager->get_gosTextureHandle(blownTextureHandle);
								mcTextureManager->addTriangle(blownTextureHandle, MC2_DRAWALPHA);
								mcTextureManager->addTriangle(blownTextureHandle, MC2_DRAWALPHA);
								mineResult.setMine(cellPos, localResult);
							}
						}
					}
				}
			}
			else
			{
				overlayHandle = 0xffffffff;
				terrainHandle = 0xffffffff;
				waterHandle = 0xffffffff;
				waterDetailHandle = 0xffffffff;
				terrainDetailHandle = 0xffffffff;
			}
		}
		else
		{
			int32_t clipped1 =
				vertices[0]->clipInfo + vertices[1]->clipInfo + vertices[3]->clipInfo;
			int32_t clipped2 =
				vertices[1]->clipInfo + vertices[2]->clipInfo + vertices[3]->clipInfo;
			if (clipped1 || clipped2)
			{
				isCement = Terrain::terrainTextures->isCement(
					vertices[0]->pVertex->textureData & 0x0000ffff);
				bool isAlpha = Terrain::terrainTextures->isAlpha(
					vertices[0]->pVertex->textureData & 0x0000ffff);
				if (!isCement)
				{
					terrainHandle = Terrain::terrainTextures2->getTextureHandle(
						vertices[1], vertices[3], &uvData);
					terrainDetailHandle = Terrain::terrainTextures2->getDetailHandle();
					overlayHandle = 0xffffffff;
					mcTextureManager->addTriangle(terrainHandle, MC2_ISTERRAIN | MC2_DRAWSOLID);
					mcTextureManager->addTriangle(terrainHandle, MC2_ISTERRAIN | MC2_DRAWSOLID);
					mcTextureManager->addTriangle(
						terrainDetailHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
					mcTextureManager->addTriangle(
						terrainDetailHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
				}
				else // This cement in the old Universe, pull the old texture
					// and display it
				{
					if (isAlpha) // This should be treated as an overlay.  Do
						// so.  Draw overlay AFTER new terrain in same
						// square!!
					{
						overlayHandle = Terrain::terrainTextures->getTextureHandle(
							vertices[0]->pVertex->textureData & 0x0000ffff);
						terrainHandle = Terrain::terrainTextures2->getTextureHandle(
							vertices[1], vertices[3], &uvData);
						terrainDetailHandle = Terrain::terrainTextures2->getDetailHandle();
						mcTextureManager->addTriangle(terrainHandle, MC2_ISTERRAIN | MC2_DRAWSOLID);
						mcTextureManager->addTriangle(terrainHandle, MC2_ISTERRAIN | MC2_DRAWSOLID);
						mcTextureManager->addTriangle(
							terrainDetailHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
						mcTextureManager->addTriangle(
							terrainDetailHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
						mcTextureManager->addTriangle(
							overlayHandle, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
						mcTextureManager->addTriangle(
							overlayHandle, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
					}
					else // Otherwise, its solid cement.  Save some draw
						// cycles!!
					{
						terrainHandle = Terrain::terrainTextures->getTextureHandle(
							vertices[0]->pVertex->textureData & 0x0000ffff);
						terrainDetailHandle = 0xffffffff; // Cement has NO detail!!
						overlayHandle = 0xffffffff;
						mcTextureManager->addTriangle(
							terrainHandle, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
						mcTextureManager->addTriangle(
							terrainHandle, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
					}
				}
				//--------------------------------------------------------------------
				// Mine Information
				int32_t rowCol = vertices[0]->posTile;
				int32_t tileR = rowCol >> 16;
				int32_t tileC = rowCol & 0x0000ffff;
				if (GameMap)
				{
					int32_t cellPos = 0;
					mineResult.init();
					for (size_t cellR = 0; cellR < terrain_const::MAPCELL_DIM; cellR++)
					{
						for (size_t cellC = 0; cellC < terrain_const::MAPCELL_DIM;
							 cellC++, cellPos++)
						{
							int32_t actualCellRow = tileR * terrain_const::MAPCELL_DIM + cellR;
							int32_t actualCellCol = tileC * terrain_const::MAPCELL_DIM + cellC;
							uint32_t localResult = 0;
							if (GameMap->inBounds(actualCellRow, actualCellCol))
								localResult = GameMap->getMine(actualCellRow, actualCellCol);
							if (localResult == 1)
							{
								mcTextureManager->get_gosTextureHandle(mineTextureHandle);
								mcTextureManager->addTriangle(mineTextureHandle, MC2_DRAWALPHA);
								mcTextureManager->addTriangle(mineTextureHandle, MC2_DRAWALPHA);
								mineResult.setMine(cellPos, localResult);
							}
							else if (localResult == 2)
							{
								mcTextureManager->get_gosTextureHandle(blownTextureHandle);
								mcTextureManager->addTriangle(blownTextureHandle, MC2_DRAWALPHA);
								mcTextureManager->addTriangle(blownTextureHandle, MC2_DRAWALPHA);
								mineResult.setMine(cellPos, localResult);
							}
						}
					}
				}
			}
			else
			{
				terrainHandle = 0xffffffff;
				waterHandle = 0xffffffff;
				waterDetailHandle = 0xffffffff;
				terrainDetailHandle = 0xffffffff;
				overlayHandle = 0xffffffff;
			}
		}
	}
	//-----------------------------------------
	// NEW(tm) water texture code here.
	if ((vertices[0]->pVertex->water & 1) || (vertices[1]->pVertex->water & 1) || (vertices[2]->pVertex->water & 1) || (vertices[3]->pVertex->water & 1))
	{
		Stuff::Vector3D vertex3D(vertices[0]->vx, vertices[0]->vy, Terrain::waterElevation);
		Stuff::Vector4D screenPos;
		int32_t clipped1 = vertices[0]->clipInfo + vertices[1]->clipInfo + vertices[2]->clipInfo;
		int32_t clipped2 = vertices[0]->clipInfo + vertices[2]->clipInfo + vertices[3]->clipInfo;
		if (uvMode != BOTTOMRIGHT)
		{
			clipped1 = vertices[0]->clipInfo + vertices[1]->clipInfo + vertices[3]->clipInfo;
			clipped2 = vertices[1]->clipInfo + vertices[2]->clipInfo + vertices[3]->clipInfo;
		}
		float negCos = Terrain::frameCos * -1.0;
		float ourCos = Terrain::frameCos;
		if (!(vertices[0]->calcThisFrame & 2))
		{
			if (clipped1 || clipped2)
			{
				if (vertices[0]->pVertex->water & 128)
				{
					vertices[0]->wAlpha = -Terrain::frameCosAlpha;
					ourCos = negCos;
				}
				else if (vertices[0]->pVertex->water & 64)
				{
					vertices[0]->wAlpha = Terrain::frameCosAlpha;
					ourCos = Terrain::frameCos;
				}
				vertex3D.z = ourCos + Terrain::waterElevation;
				bool clipData = false;
				clipData = eye->projectZ(vertex3D, screenPos);
				bool isVisible = Terrain::IsGameSelectTerrainPosition(vertex3D) || drawTerrainGrid;
				if (!isVisible)
				{
					clipData = false;
					vertices[0]->hazeFactor = 1.0f;
				}
				if (eye->usePerspective)
					vertices[0]->clipInfo = clipData;
				else
					vertices[0]->clipInfo = clipData;
				vertices[0]->wx = screenPos.x;
				vertices[0]->wy = screenPos.y;
				vertices[0]->wz = screenPos.z;
				vertices[0]->ww = screenPos.w;
				vertices[0]->calcThisFrame |= 2;
				if (clipData)
				{
					if (screenPos.z < leastZ)
					{
						leastZ = screenPos.z;
					}
					if (screenPos.z > mostZ)
					{
						mostZ = screenPos.z;
					}
					if (screenPos.w < leastW)
					{
						leastW = screenPos.w;
						leastWY = screenPos.y;
					}
					if (screenPos.w > mostW)
					{
						mostW = screenPos.w;
						mostWY = screenPos.y;
					}
				}
			}
		}
		if (!(vertices[1]->calcThisFrame & 2))
		{
			if (clipped1 || clipped2)
			{
				if (vertices[1]->pVertex->water & 128)
				{
					vertices[1]->wAlpha = -Terrain::frameCosAlpha;
					ourCos = negCos;
				}
				else if (vertices[1]->pVertex->water & 64)
				{
					vertices[1]->wAlpha = Terrain::frameCosAlpha;
					ourCos = Terrain::frameCos;
				}
				vertex3D.z = ourCos + Terrain::waterElevation;
				vertex3D.x = vertices[1]->vx;
				vertex3D.y = vertices[1]->vy;
				bool clipData = false;
				clipData = eye->projectZ(vertex3D, screenPos);
				bool isVisible = Terrain::IsGameSelectTerrainPosition(vertex3D) || drawTerrainGrid;
				if (!isVisible)
				{
					clipData = false;
					vertices[1]->hazeFactor = 1.0f;
				}
				if (eye->usePerspective)
					vertices[1]->clipInfo = clipData; // onScreen;
				else
					vertices[1]->clipInfo = clipData;
				vertices[1]->wx = screenPos.x;
				vertices[1]->wy = screenPos.y;
				vertices[1]->wz = screenPos.z;
				vertices[1]->ww = screenPos.w;
				vertices[1]->calcThisFrame |= 2;
				if (clipData)
				{
					if (screenPos.z < leastZ)
					{
						leastZ = screenPos.z;
					}
					if (screenPos.z > mostZ)
					{
						mostZ = screenPos.z;
					}
					if (screenPos.w < leastW)
					{
						leastW = screenPos.w;
						leastWY = screenPos.y;
					}
					if (screenPos.w > mostW)
					{
						mostW = screenPos.w;
						mostWY = screenPos.y;
					}
				}
			}
		}
		if (!(vertices[2]->calcThisFrame & 2))
		{
			if (clipped1 || clipped2)
			{
				if (vertices[2]->pVertex->water & 128)
				{
					vertices[2]->wAlpha = -Terrain::frameCosAlpha;
					ourCos = negCos;
				}
				else if (vertices[2]->pVertex->water & 64)
				{
					vertices[2]->wAlpha = Terrain::frameCosAlpha;
					ourCos = Terrain::frameCos;
				}
				vertex3D.z = ourCos + Terrain::waterElevation;
				vertex3D.x = vertices[2]->vx;
				vertex3D.y = vertices[2]->vy;
				bool clipData = false;
				clipData = eye->projectZ(vertex3D, screenPos);
				bool isVisible = Terrain::IsGameSelectTerrainPosition(vertex3D) || drawTerrainGrid;
				if (!isVisible)
				{
					clipData = false;
					vertices[2]->hazeFactor = 1.0f;
				}
				if (eye->usePerspective)
					vertices[2]->clipInfo = clipData; // onScreen;
				else
					vertices[2]->clipInfo = clipData;
				vertices[2]->wx = screenPos.x;
				vertices[2]->wy = screenPos.y;
				vertices[2]->wz = screenPos.z;
				vertices[2]->ww = screenPos.w;
				vertices[2]->calcThisFrame |= 2;
				if (clipData)
				{
					if (screenPos.z < leastZ)
					{
						leastZ = screenPos.z;
					}
					if (screenPos.z > mostZ)
					{
						mostZ = screenPos.z;
					}
					if (screenPos.w < leastW)
					{
						leastW = screenPos.w;
						leastWY = screenPos.y;
					}
					if (screenPos.w > mostW)
					{
						mostW = screenPos.w;
						mostWY = screenPos.y;
					}
				}
			}
		}
		if (!(vertices[3]->calcThisFrame & 2))
		{
			if (clipped1 || clipped2)
			{
				if (vertices[3]->pVertex->water & 128)
				{
					vertices[3]->wAlpha = -Terrain::frameCosAlpha;
					ourCos = negCos;
				}
				else if (vertices[3]->pVertex->water & 64)
				{
					vertices[3]->wAlpha = Terrain::frameCosAlpha;
					ourCos = Terrain::frameCos;
				}
				vertex3D.z = ourCos + Terrain::waterElevation;
				vertex3D.x = vertices[3]->vx;
				vertex3D.y = vertices[3]->vy;
				bool clipData = false;
				clipData = eye->projectZ(vertex3D, screenPos);
				bool isVisible = Terrain::IsGameSelectTerrainPosition(vertex3D) || drawTerrainGrid;
				if (!isVisible)
				{
					clipData = false;
					vertices[3]->hazeFactor = 1.0f;
				}
				if (eye->usePerspective)
					vertices[3]->clipInfo = clipData; // onScreen;
				else
					vertices[3]->clipInfo = clipData;
				vertices[3]->wx = screenPos.x;
				vertices[3]->wy = screenPos.y;
				vertices[3]->wz = screenPos.z;
				vertices[3]->ww = screenPos.w;
				vertices[3]->calcThisFrame |= 2;
				if (clipData)
				{
					if (screenPos.z < leastZ)
					{
						leastZ = screenPos.z;
					}
					if (screenPos.z > mostZ)
					{
						mostZ = screenPos.z;
					}
					if (screenPos.w < leastW)
					{
						leastW = screenPos.w;
						leastWY = screenPos.y;
					}
					if (screenPos.w > mostW)
					{
						mostW = screenPos.w;
						mostWY = screenPos.y;
					}
				}
			}
		}
		if (clipped1 || clipped2)
		{
			if (!Terrain::terrainTextures2)
			{
				uint32_t waterDetailData = Terrain::terrainTextures->setDetail(0, sprayFrame);
				waterHandle =
					Terrain::terrainTextures->getTextureHandle(MapData::WaterTXMData & 0x0000ffff);
				waterDetailHandle =
					Terrain::terrainTextures->getDetailHandle(waterDetailData & 0x0000ffff);
			}
			else
			{
				waterHandle = Terrain::terrainTextures2->getWaterTextureHandle();
				waterDetailHandle = Terrain::terrainTextures2->getWaterDetailHandle(sprayFrame);
			}
			mcTextureManager->addTriangle(waterHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
			mcTextureManager->addTriangle(waterHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
			mcTextureManager->addTriangle(waterDetailHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
			mcTextureManager->addTriangle(waterDetailHandle, MC2_ISTERRAIN | MC2_DRAWALPHA);
		}
		else
		{
			waterHandle = 0xffffffff;
			waterDetailHandle = 0xffffffff;
		}
	}
	else
	{
		waterHandle = 0xffffffff;
		waterDetailHandle = 0xffffffff;
	}
	if (terrainHandle != 0xffffffff)
	{
		//-----------------------------------------------------
		// FOG time.
		float fogStart = eye->fogStart;
		float fogFull = eye->fogFull;
		//-----------------------------------------------------
		// Process Vertex 0 if not already done
		if (!(vertices[0]->calcThisFrame & 1))
		{
			uint32_t specR = 0, specG = 0, specB = 0;
			uint32_t lightr = 0xff, lightg = 0xff, lightb = 0xff;
			if (Environment.Renderer != 3)
			{
				//------------------------------------------------------------
				float lightIntensity = vertices[0]->pVertex->vertexNormal * eye->lightDirection;
				uint8_t shadow = vertices[0]->pVertex->shadow;
				if (shadow && lightIntensity > 0.2f)
				{
					lightIntensity = 0.2f;
				}
				lightr = eye->getLightRed(lightIntensity);
				lightg = eye->getLightGreen(lightIntensity);
				lightb = eye->getLightBlue(lightIntensity);
				if (BaseVertexcolour)
				{
					lightr += ((BaseVertexcolour >> 16) & 0x000000ff);
					if (lightr > 0xff)
						lightr = 0xff;
					lightg += ((BaseVertexcolour >> 8) & 0x000000ff);
					if (lightg > 0xff)
						lightg = 0xff;
					lightb += (BaseVertexcolour & 0x000000ff);
					if (lightb > 0xff)
						lightb = 0xff;
				}
				if (rainLightLevel < 1.0f)
				{
					lightr = (float)lightr * rainLightLevel;
					lightb = (float)lightb * rainLightLevel;
					lightg = (float)lightg * rainLightLevel;
				}
				if (lighteningLevel > 0x0)
				{
					specR = specG = specB = lighteningLevel;
				}
				vertices[0]->lightRGB = lightb + (lightr << 16) + (lightg << 8) + (0xff << 24);
				// First two light are already factored into the above
				// equations!
				for (size_t i = 2; i < eye->getNumTerrainLights(); i++)
				{
					TG_LightPtr thisLight = eye->getTerrainLight(i);
					if (thisLight)
					{
						//					if (useShadows)
						{
							if ((thisLight->lightType == TG_LIGHT_POINT) || (thisLight->lightType == TG_LIGHT_SPOT) || (thisLight->lightType == TG_LIGHT_TERRAIN))
							{
								Stuff::Point3D vertexToLight;
								vertexToLight.x = vertices[0]->vx;
								vertexToLight.y = vertices[0]->vy;
								vertexToLight.z = vertices[0]->pVertex->elevation;
								vertexToLight -= thisLight->position;
								float length = vertexToLight.GetApproximateLength();
								float falloff = 1.0f;
								if (thisLight->GetFalloff(length, falloff))
								{
									float red, green, blue;
									red =
										float((thisLight->GetaRGB() >> 16) & 0x000000ff) * falloff;
									green =
										float((thisLight->GetaRGB() >> 8) & 0x000000ff) * falloff;
									blue = float((thisLight->GetaRGB()) & 0x000000ff) * falloff;
									specR += (uint32_t)red;
									specG += (uint32_t)green;
									specB += (uint32_t)blue;
									if (specR > 255)
										specR = 255;
									if (specG > 255)
										specG = 255;
									if (specB > 255)
										specB = 255;
								}
							}
						}
					}
				}
			}
			vertices[0]->lightRGB = lightb + (lightr << 16) + (lightg << 8) + (0xff << 24);
			vertices[0]->fogRGB = (0xff << 24) + (specR << 16) + (specG << 8) + (specB);
			// Fog
			uint32_t fogResult = 0xff;
			if (!(vertices[0]->calcThisFrame & 1))
			{
				if (useFog)
				{
					if (vertices[0]->pVertex->elevation < fogStart)
					{
						float fogFactor = fogStart - vertices[0]->pVertex->elevation;
						if (fogFactor < 0.0)
						{
							fogResult = 0xff;
						}
						else
						{
							fogFactor /= (fogStart - fogFull);
							if (fogFactor <= 1.0)
							{
								fogFactor *= fogFactor;
								fogFactor = 1.0 - fogFactor;
								fogFactor *= 256.0;
							}
							else
							{
								fogFactor = 256.0;
							}
							fogResult = float2long(fogFactor);
						}
					}
					else
					{
						fogResult = 0xff;
					}
					vertices[0]->fogRGB =
						(fogResult << 24) + (specR << 16) + (specG << 8) + (specB);
				}
			}
			//-------------------
			// Distance FOG now.
			if (vertices[0]->hazeFactor != 0.0f)
			{
				float fogFactor = 1.0 - vertices[0]->hazeFactor;
				uint32_t distFog = float2long(fogFactor * 255.0f);
				if (distFog < fogResult)
					fogResult = distFog;
				vertices[0]->fogRGB = (fogResult << 24) + (specR << 16) + (specG << 8) + (specB);
			}
			vertices[0]->calcThisFrame |= 1;
		}
		//-----------------------------------------------------
		// Process Vertex 1 if not already done
		if (!(vertices[1]->calcThisFrame & 1))
		{
			uint32_t specR = 0, specG = 0, specB = 0;
			uint32_t lightr = 0xff, lightg = 0xff, lightb = 0xff;
			if (Environment.Renderer != 3)
			{
				float lightIntensity = vertices[1]->pVertex->vertexNormal * eye->lightDirection;
				uint8_t shadow = vertices[1]->pVertex->shadow;
				if (shadow && lightIntensity > 0.2f)
				{
					lightIntensity = 0.2f;
				}
				lightr = eye->getLightRed(lightIntensity);
				lightg = eye->getLightGreen(lightIntensity);
				lightb = eye->getLightBlue(lightIntensity);
				if (BaseVertexcolour)
				{
					lightr += ((BaseVertexcolour >> 16) & 0x000000ff);
					if (lightr > 0xff)
						lightr = 0xff;
					lightg += ((BaseVertexcolour >> 8) & 0x000000ff);
					if (lightg > 0xff)
						lightg = 0xff;
					lightb += (BaseVertexcolour & 0x000000ff);
					if (lightb > 0xff)
						lightb = 0xff;
				}
				if (rainLightLevel < 1.0f)
				{
					lightr = (float)lightr * rainLightLevel;
					lightb = (float)lightb * rainLightLevel;
					lightg = (float)lightg * rainLightLevel;
				}
				if (lighteningLevel > 0x0)
				{
					specR = specG = specB = lighteningLevel;
				}
				vertices[1]->lightRGB = lightb + (lightr << 16) + (lightg << 8) + (0xff << 24);
				// First two light are already factored into the above
				// equations!
				for (size_t i = 2; i < eye->getNumTerrainLights(); i++)
				{
					TG_LightPtr thisLight = eye->getTerrainLight(i);
					if (thisLight)
					{
						//					if (useShadows)
						{
							if ((thisLight->lightType == TG_LIGHT_POINT) || (thisLight->lightType == TG_LIGHT_SPOT) || (thisLight->lightType == TG_LIGHT_TERRAIN))
							{
								Stuff::Point3D vertexToLight;
								vertexToLight.x = vertices[1]->vx;
								vertexToLight.y = vertices[1]->vy;
								vertexToLight.z = vertices[1]->pVertex->elevation;
								vertexToLight -= thisLight->position;
								float length = vertexToLight.GetApproximateLength();
								float falloff = 1.0f;
								if (thisLight->GetFalloff(length, falloff))
								{
									float red, green, blue;
									red =
										float((thisLight->GetaRGB() >> 16) & 0x000000ff) * falloff;
									green =
										float((thisLight->GetaRGB() >> 8) & 0x000000ff) * falloff;
									blue = float((thisLight->GetaRGB()) & 0x000000ff) * falloff;
									specR += (uint32_t)red;
									specG += (uint32_t)green;
									specB += (uint32_t)blue;
									if (specR > 255)
										specR = 255;
									if (specG > 255)
										specG = 255;
									if (specB > 255)
										specB = 255;
								}
							}
						}
					}
				}
			}
			vertices[1]->lightRGB = lightb + (lightr << 16) + (lightg << 8) + (0xff << 24);
			vertices[1]->fogRGB = (0xff << 24) + (specR << 16) + (specG << 8) + (specB);
			// Fog
			uint32_t fogResult = 0xff;
			if (Environment.Renderer != 3)
			{
				if (useFog)
				{
					if (vertices[1]->pVertex->elevation < fogStart)
					{
						float fogFactor = fogStart - vertices[1]->pVertex->elevation;
						if (fogFactor < 0.0)
						{
							fogResult = 0xff;
						}
						else
						{
							fogFactor /= (fogStart - fogFull);
							if (fogFactor <= 1.0)
							{
								fogFactor *= fogFactor;
								fogFactor = 1.0 - fogFactor;
								fogFactor *= 256.0;
							}
							else
							{
								fogFactor = 256.0;
							}
							fogResult = float2long(fogFactor);
						}
					}
					else
					{
						fogResult = 0xff;
					}
					vertices[1]->fogRGB =
						(fogResult << 24) + (specR << 16) + (specG << 8) + (specB);
				}
			}
			//-------------------
			// Distance FOG now.
			if (vertices[1]->hazeFactor != 0.0f)
			{
				float fogFactor = 1.0 - vertices[1]->hazeFactor;
				uint32_t distFog = float2long(fogFactor * 255.0);
				if (distFog < fogResult)
					fogResult = distFog;
				vertices[1]->fogRGB = (fogResult << 24) + (specR << 16) + (specG << 8) + (specB);
			}
			vertices[1]->calcThisFrame |= 1;
		}
		//-----------------------------------------------------
		// Process Vertex 2 if not already done
		if (!(vertices[2]->calcThisFrame & 1))
		{
			uint32_t specR = 0, specG = 0, specB = 0;
			uint32_t lightr = 0xff, lightg = 0xff, lightb = 0xff;
			if (Environment.Renderer != 3)
			{
				float lightIntensity = vertices[2]->pVertex->vertexNormal * eye->lightDirection;
				uint8_t shadow = vertices[2]->pVertex->shadow;
				if (shadow && lightIntensity > 0.2f)
				{
					lightIntensity = 0.2f;
				}
				lightr = eye->getLightRed(lightIntensity);
				lightg = eye->getLightGreen(lightIntensity);
				lightb = eye->getLightBlue(lightIntensity);
				if (BaseVertexcolour)
				{
					lightr += ((BaseVertexcolour >> 16) & 0x000000ff);
					if (lightr > 0xff)
						lightr = 0xff;
					lightg += ((BaseVertexcolour >> 8) & 0x000000ff);
					if (lightg > 0xff)
						lightg = 0xff;
					lightb += (BaseVertexcolour & 0x000000ff);
					if (lightb > 0xff)
						lightb = 0xff;
				}
				if (rainLightLevel < 1.0f)
				{
					lightr = (float)lightr * rainLightLevel;
					lightb = (float)lightb * rainLightLevel;
					lightg = (float)lightg * rainLightLevel;
				}
				if (lighteningLevel > 0x0)
				{
					specR = specG = specB = lighteningLevel;
				}
				vertices[2]->lightRGB = lightb + (lightr << 16) + (lightg << 8) + (0xff << 24);
				// First two light are already factored into the above
				// equations!
				for (size_t i = 2; i < eye->getNumTerrainLights(); i++)
				{
					TG_LightPtr thisLight = eye->getTerrainLight(i);
					if (thisLight)
					{
						//					if (useShadows)
						{
							if ((thisLight->lightType == TG_LIGHT_POINT) || (thisLight->lightType == TG_LIGHT_SPOT) || (thisLight->lightType == TG_LIGHT_TERRAIN))
							{
								Stuff::Point3D vertexToLight;
								vertexToLight.x = vertices[2]->vx;
								vertexToLight.y = vertices[2]->vy;
								vertexToLight.z = vertices[2]->pVertex->elevation;
								vertexToLight -= thisLight->position;
								float length = vertexToLight.GetApproximateLength();
								float falloff = 1.0f;
								if (thisLight->GetFalloff(length, falloff))
								{
									float red, green, blue;
									red =
										float((thisLight->GetaRGB() >> 16) & 0x000000ff) * falloff;
									green =
										float((thisLight->GetaRGB() >> 8) & 0x000000ff) * falloff;
									blue = float((thisLight->GetaRGB()) & 0x000000ff) * falloff;
									specR += (uint32_t)red;
									specG += (uint32_t)green;
									specB += (uint32_t)blue;
									if (specR > 255)
										specR = 255;
									if (specG > 255)
										specG = 255;
									if (specB > 255)
										specB = 255;
								}
							}
						}
					}
				}
			}
			vertices[2]->lightRGB = lightb + (lightr << 16) + (lightg << 8) + (0xff << 24);
			vertices[2]->fogRGB = (0xff << 24) + (specR << 16) + (specG << 8) + (specB);
			// Fog
			uint32_t fogResult = 0xff;
			if (Environment.Renderer != 3)
			{
				if (useFog)
				{
					if (vertices[2]->pVertex->elevation < fogStart)
					{
						float fogFactor = fogStart - vertices[2]->pVertex->elevation;
						if ((fogFactor < 0.0) || (0.0 == (fogStart - fogFull)))
						{
							fogResult = 0xff;
						}
						else
						{
							fogFactor /= (fogStart - fogFull);
							if (fogFactor <= 1.0)
							{
								fogFactor *= fogFactor;
								fogFactor = 1.0 - fogFactor;
								fogFactor *= 256.0;
							}
							else
							{
								fogFactor = 256.0;
							}
							fogResult = float2long(fogFactor);
						}
					}
					else
					{
						fogResult = 0xff;
					}
					vertices[2]->fogRGB =
						(fogResult << 24) + (specR << 16) + (specG << 8) + (specB);
				}
			}
			//-------------------
			// Distance FOG now.
			if (vertices[2]->hazeFactor != 0.0f)
			{
				float fogFactor = 1.0 - vertices[2]->hazeFactor;
				uint32_t distFog = float2long(fogFactor * 255.0f);
				if (distFog < fogResult)
					fogResult = distFog;
				vertices[2]->fogRGB = (fogResult << 24) + (specR << 16) + (specG << 8) + (specB);
			}
			vertices[2]->calcThisFrame |= 1;
		}
		//-----------------------------------------------------
		// Process Vertex 3 if not already done
		if (!(vertices[3]->calcThisFrame & 1))
		{
			uint32_t specR = 0, specG = 0, specB = 0;
			uint32_t lightr = 0xff, lightg = 0xff, lightb = 0xff;
			if (Environment.Renderer != 3)
			{
				float lightIntensity = vertices[3]->pVertex->vertexNormal * eye->lightDirection;
				uint8_t shadow = vertices[3]->pVertex->shadow;
				if (shadow && lightIntensity > 0.2f)
				{
					lightIntensity = 0.2f;
				}
				lightr = eye->getLightRed(lightIntensity);
				lightg = eye->getLightGreen(lightIntensity);
				lightb = eye->getLightBlue(lightIntensity);
				if (BaseVertexcolour)
				{
					lightr += ((BaseVertexcolour >> 16) & 0x000000ff);
					if (lightr > 0xff)
						lightr = 0xff;
					lightg += ((BaseVertexcolour >> 8) & 0x000000ff);
					if (lightg > 0xff)
						lightg = 0xff;
					lightb += (BaseVertexcolour & 0x000000ff);
					if (lightb > 0xff)
						lightb = 0xff;
				}
				if (rainLightLevel < 1.0f)
				{
					lightr = (float)lightr * rainLightLevel;
					lightb = (float)lightb * rainLightLevel;
					lightg = (float)lightg * rainLightLevel;
				}
				if (lighteningLevel > 0x0)
				{
					specR = specG = specB = lighteningLevel;
				}
				vertices[3]->lightRGB = lightb + (lightr << 16) + (lightg << 8) + (0xff << 24);
				// First two light are already factored into the above
				// equations!
				for (size_t i = 2; i < eye->getNumTerrainLights(); i++)
				{
					TG_LightPtr thisLight = eye->getTerrainLight(i);
					if (thisLight)
					{
						//					if (useShadows)
						{
							if ((thisLight->lightType == TG_LIGHT_POINT) || (thisLight->lightType == TG_LIGHT_SPOT) || (thisLight->lightType == TG_LIGHT_TERRAIN))
							{
								Stuff::Point3D vertexToLight;
								vertexToLight.x = vertices[3]->vx;
								vertexToLight.y = vertices[3]->vy;
								vertexToLight.z = vertices[3]->pVertex->elevation;
								vertexToLight -= thisLight->position;
								float length = vertexToLight.GetApproximateLength();
								float falloff = 1.0f;
								if (thisLight->GetFalloff(length, falloff))
								{
									float red, green, blue;
									red =
										float((thisLight->GetaRGB() >> 16) & 0x000000ff) * falloff;
									green =
										float((thisLight->GetaRGB() >> 8) & 0x000000ff) * falloff;
									blue = float((thisLight->GetaRGB()) & 0x000000ff) * falloff;
									specR += (uint32_t)red;
									specG += (uint32_t)green;
									specB += (uint32_t)blue;
									if (specR > 255)
										specR = 255;
									if (specG > 255)
										specG = 255;
									if (specB > 255)
										specB = 255;
								}
							}
						}
					}
				}
			}
			vertices[3]->lightRGB = lightb + (lightr << 16) + (lightg << 8) + (0xff << 24);
			vertices[3]->fogRGB = (0xff << 24) + (specR << 16) + (specG << 8) + (specB);
			// Fog
			uint32_t fogResult = 0xff;
			if (Environment.Renderer != 3)
			{
				if (useFog)
				{
					if (vertices[3]->pVertex->elevation < fogStart)
					{
						float fogFactor = fogStart - vertices[3]->pVertex->elevation;
						if (fogFactor < 0.0)
						{
							fogResult = 0xff;
						}
						else
						{
							fogFactor /= (fogStart - fogFull);
							if (fogFactor <= 1.0)
							{
								fogFactor *= fogFactor;
								fogFactor = 1.0 - fogFactor;
								fogFactor *= 256.0;
							}
							else
							{
								fogFactor = 256.0;
							}
							fogResult = float2long(fogFactor);
						}
					}
					else
					{
						fogResult = 0xff;
					}
				}
			}
			//-------------------
			// Distance FOG now.
			if (vertices[3]->hazeFactor != 0.0f)
			{
				float fogFactor = 1.0 - vertices[3]->hazeFactor;
				uint32_t distFog = float2long(fogFactor * 255.0f);
				if (distFog < fogResult)
					fogResult = distFog;
				vertices[3]->fogRGB = (fogResult << 24) + (specR << 16) + (specG << 8) + (specB);
			}
			vertices[3]->calcThisFrame |= 1;
		}
	}
}

#define TERRAIN_DEPTH_FUDGE 0.001f
//---------------------------------------------------------------------------
void TerrainQuad::draw(void)
{
	if (terrainHandle != 0xffffffff)
	{
		numTerrainFaces++;
		//---------------------------------------
		// GOS 3D draw Calls now!
		gos_VERTEX gVertex[3];
		float minU = 0.00f;
		float maxU = 0.9999f;
		float minV = 0.00f;
		float maxV = 0.9999f;
		float oldminU = 0.0078125f;
		float oldmaxU = 0.9921875f;
		float oldminV = 0.0078125f;
		float oldmaxV = 0.9921875f;
		if (Terrain::terrainTextures2 && !(overlayHandle == 0xffffffff && isCement))
		{
			minU = uvData.minU;
			minV = uvData.minV;
			maxU = uvData.maxU;
			maxV = uvData.maxV;
		}
		Stuff::Point3D camPosition;
		camPosition = *TG_Shape::cameraOrigin;
		if (uvMode == BOTTOMRIGHT)
		{
			//--------------------------
			// Top Triangle
			uint32_t lightRGB0 = vertices[0]->lightRGB;
			uint32_t lightRGB1 = vertices[1]->lightRGB;
			uint32_t lightRGB2 = vertices[2]->lightRGB;
			if (Terrain::terrainTextures2 && (!Terrain::terrainTextures->isCement(vertices[0]->pVertex->textureData & 0x0000ffff) || Terrain::terrainTextures->isAlpha(vertices[0]->pVertex->textureData & 0x0000ffff)))
				lightRGB0 = lightRGB1 = lightRGB2 = 0xffffffff;
			lightRGB0 = vertices[0]->pVertex->selected ? SELECTION_COLOR : lightRGB0;
			lightRGB1 = vertices[1]->pVertex->selected ? SELECTION_COLOR : lightRGB1;
			lightRGB2 = vertices[2]->pVertex->selected ? SELECTION_COLOR : lightRGB2;
			gVertex[0].x = vertices[0]->px;
			gVertex[0].y = vertices[0]->py;
			gVertex[0].z = vertices[0]->pz + TERRAIN_DEPTH_FUDGE;
			gVertex[0].rhw = vertices[0]->pw;
			gVertex[0].u = minU;
			gVertex[0].v = minV;
			gVertex[0].argb = lightRGB0;
			gVertex[0].frgb = vertices[0]->fogRGB;
			gVertex[1].x = vertices[1]->px;
			gVertex[1].y = vertices[1]->py;
			gVertex[1].z = vertices[1]->pz + TERRAIN_DEPTH_FUDGE;
			gVertex[1].rhw = vertices[1]->pw;
			gVertex[1].u = maxU;
			gVertex[1].v = minV;
			gVertex[1].argb = lightRGB1;
			gVertex[1].frgb = vertices[1]->fogRGB;
			gVertex[2].x = vertices[2]->px;
			gVertex[2].y = vertices[2]->py;
			gVertex[2].z = vertices[2]->pz + TERRAIN_DEPTH_FUDGE;
			gVertex[2].rhw = vertices[2]->pw;
			gVertex[2].u = maxU;
			gVertex[2].v = maxV;
			gVertex[2].argb = lightRGB2;
			gVertex[2].frgb = vertices[2]->fogRGB;
			if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) && (gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
			{
				{
					if ((terrainDetailHandle == 0xffffffff) && (overlayHandle == 0xffffffff) && isCement)
						mcTextureManager->addVertices(
							terrainHandle, gVertex, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
					else
						mcTextureManager->addVertices(
							terrainHandle, gVertex, MC2_ISTERRAIN | MC2_DRAWSOLID);
					//--------------------------------------------------------------
					// Draw the Overlay Texture if it exists.
					if (useOverlayTexture && (overlayHandle != 0xffffffff))
					{
						// Uses EXACT same coords as the above normal texture.
						// Just replace the UVs and the texture handle!!
						gos_VERTEX oVertex[3];
						memcpy(oVertex, gVertex, sizeof(gos_VERTEX) * 3);
						oVertex[0].u = oldminU;
						oVertex[0].v = oldminV;
						oVertex[1].u = oldmaxU;
						oVertex[1].v = oldminV;
						oVertex[2].u = oldmaxU;
						oVertex[2].v = oldmaxV;
						// Light the overlays!!
						oVertex[0].argb = vertices[0]->lightRGB;
						oVertex[1].argb = vertices[1]->lightRGB;
						oVertex[2].argb = vertices[2]->lightRGB;
						mcTextureManager->addVertices(
							overlayHandle, oVertex, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
					}
					//----------------------------------------------------
					// Draw the detail Texture
					if (useWaterInterestTexture && (terrainDetailHandle != 0xffffffff))
					{
						gos_VERTEX sVertex[3];
						memcpy(sVertex, gVertex, sizeof(gos_VERTEX) * 3);
						float tilingFactor = Terrain::terrainTextures->getDetailTilingFactor(1);
						if (Terrain::terrainTextures2)
							tilingFactor = Terrain::terrainTextures2->getDetailTilingFactor();
						float oneOverTf = tilingFactor / Terrain::worldUnitsMapSide;
						sVertex[0].u = (vertices[0]->vx - Terrain::mapTopLeft3d.x) * oneOverTf;
						sVertex[0].v = (Terrain::mapTopLeft3d.y - vertices[0]->vy) * oneOverTf;
						sVertex[1].u = (vertices[1]->vx - Terrain::mapTopLeft3d.x) * oneOverTf;
						sVertex[1].v = (Terrain::mapTopLeft3d.y - vertices[1]->vy) * oneOverTf;
						sVertex[2].u = (vertices[2]->vx - Terrain::mapTopLeft3d.x) * oneOverTf;
						sVertex[2].v = (Terrain::mapTopLeft3d.y - vertices[2]->vy) * oneOverTf;
						if ((sVertex[0].u > MaxMinUV) || (sVertex[0].v > MaxMinUV) || (sVertex[1].u > MaxMinUV) || (sVertex[1].v > MaxMinUV) || (sVertex[2].u > MaxMinUV) || (sVertex[2].v > MaxMinUV))
						{
							// If any are out range, move 'em back in range by
							// adjustfactor.
							float maxU = fmax(sVertex[0].u, fmax(sVertex[1].u, sVertex[2].u));
							maxU = floor(maxU - (MaxMinUV - 1.0f));
							float maxV = fmax(sVertex[0].v, fmax(sVertex[1].v, sVertex[2].v));
							maxV = floor(maxV - (MaxMinUV - 1.0f));
							sVertex[0].u -= maxU;
							sVertex[1].u -= maxU;
							sVertex[2].u -= maxU;
							sVertex[0].v -= maxV;
							sVertex[1].v -= maxV;
							sVertex[2].v -= maxV;
						}
						// Light the Detail Texture
						if (Terrain::terrainTextures2)
						{
							sVertex[0].argb = sVertex[1].argb = sVertex[2].argb = 0xffffffff;
						}
						mcTextureManager->addVertices(
							terrainDetailHandle, sVertex, MC2_ISTERRAIN | MC2_DRAWALPHA);
					}
				}
			}
			//--------------------------
			// Bottom Triangle
			//
			// gVertex[0] same as above gVertex[0].
			// gVertex[1] is same as above gVertex[2].
			// gVertex[2] is calced from vertex[3].
			uint32_t lightRGB3 = vertices[3]->lightRGB;
			if (Terrain::terrainTextures2 && (!Terrain::terrainTextures->isCement(vertices[0]->pVertex->textureData & 0x0000ffff) || Terrain::terrainTextures->isAlpha(vertices[0]->pVertex->textureData & 0x0000ffff)))
				lightRGB3 = 0xffffffff;
			lightRGB3 = vertices[3]->pVertex->selected ? SELECTION_COLOR : lightRGB3;
			gVertex[1].x = gVertex[2].x;
			gVertex[1].y = gVertex[2].y;
			gVertex[1].z = gVertex[2].z;
			gVertex[1].rhw = gVertex[2].rhw;
			gVertex[1].u = gVertex[2].u;
			gVertex[1].v = gVertex[2].v;
			gVertex[1].argb = gVertex[2].argb;
			gVertex[1].frgb = gVertex[2].frgb;
			gVertex[2].x = vertices[3]->px;
			gVertex[2].y = vertices[3]->py;
			gVertex[2].z = vertices[3]->pz + TERRAIN_DEPTH_FUDGE;
			gVertex[2].rhw = vertices[3]->pw;
			gVertex[2].u = minU;
			gVertex[2].v = maxV;
			gVertex[2].argb = lightRGB3;
			gVertex[2].frgb = vertices[3]->fogRGB;
			if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) && (gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
			{
				{
					if ((terrainDetailHandle == 0xffffffff) && (overlayHandle == 0xffffffff) && isCement)
						mcTextureManager->addVertices(
							terrainHandle, gVertex, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
					else
						mcTextureManager->addVertices(
							terrainHandle, gVertex, MC2_ISTERRAIN | MC2_DRAWSOLID);
					//--------------------------------------------------------------
					// Draw the Overlay Texture if it exists.
					if (useOverlayTexture && (overlayHandle != 0xffffffff))
					{
						// Uses EXACT same coords as the above normal texture.
						// Just replace the UVs and the texture handle!!
						// Uses EXACT same coords as the above normal texture.
						// Just replace the UVs and the texture handle!!
						gos_VERTEX oVertex[3];
						memcpy(oVertex, gVertex, sizeof(gos_VERTEX) * 3);
						oVertex[0].u = oldminU;
						oVertex[0].v = oldminV;
						oVertex[1].u = oldmaxU;
						oVertex[1].v = oldmaxV;
						oVertex[2].u = oldminU;
						oVertex[2].v = oldmaxV;
						// Light the overlays!!
						oVertex[0].argb = vertices[0]->lightRGB;
						oVertex[1].argb = vertices[2]->lightRGB;
						oVertex[2].argb = vertices[3]->lightRGB;
						mcTextureManager->addVertices(
							overlayHandle, oVertex, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
					}
					//----------------------------------------------------
					// Draw the detail Texture
					if (useWaterInterestTexture && (terrainDetailHandle != 0xffffffff))
					{
						gos_VERTEX sVertex[3];
						memcpy(sVertex, gVertex, sizeof(gos_VERTEX) * 3);
						float tilingFactor = Terrain::terrainTextures->getDetailTilingFactor(1);
						if (Terrain::terrainTextures2)
							tilingFactor = Terrain::terrainTextures2->getDetailTilingFactor();
						float oneOverTF = tilingFactor / Terrain::worldUnitsMapSide;
						sVertex[0].u = (vertices[0]->vx - Terrain::mapTopLeft3d.x) * oneOverTF;
						sVertex[0].v = (Terrain::mapTopLeft3d.y - vertices[0]->vy) * oneOverTF;
						sVertex[1].u = (vertices[2]->vx - Terrain::mapTopLeft3d.x) * oneOverTF;
						sVertex[1].v = (Terrain::mapTopLeft3d.y - vertices[2]->vy) * oneOverTF;
						sVertex[2].u = (vertices[3]->vx - Terrain::mapTopLeft3d.x) * oneOverTF;
						sVertex[2].v = (Terrain::mapTopLeft3d.y - vertices[3]->vy) * oneOverTF;
						if ((sVertex[0].u > MaxMinUV) || (sVertex[0].v > MaxMinUV) || (sVertex[1].u > MaxMinUV) || (sVertex[1].v > MaxMinUV) || (sVertex[2].u > MaxMinUV) || (sVertex[2].v > MaxMinUV))
						{
							// If any are out range, move 'em back in range by
							// adjustfactor.
							float maxU = fmax(sVertex[0].u, fmax(sVertex[1].u, sVertex[2].u));
							maxU = floor(maxU - (MaxMinUV - 1.0f));
							float maxV = fmax(sVertex[0].v, fmax(sVertex[1].v, sVertex[2].v));
							maxV = floor(maxV - (MaxMinUV - 1.0f));
							sVertex[0].u -= maxU;
							sVertex[1].u -= maxU;
							sVertex[2].u -= maxU;
							sVertex[0].v -= maxV;
							sVertex[1].v -= maxV;
							sVertex[2].v -= maxV;
						}
						// Light the Detail Texture
						if (Terrain::terrainTextures2)
						{
							sVertex[0].argb = sVertex[1].argb = sVertex[2].argb = 0xffffffff;
						}
						mcTextureManager->addVertices(
							terrainDetailHandle, sVertex, MC2_ISTERRAIN | MC2_DRAWALPHA);
					}
				}
			}
		}
		else if (uvMode == BOTTOMLEFT)
		{
			//------------------------------
			// Top Triangle.
			uint32_t lightRGB0 = vertices[0]->lightRGB;
			uint32_t lightRGB1 = vertices[1]->lightRGB;
			uint32_t lightRGB3 = vertices[3]->lightRGB;
			if (Terrain::terrainTextures2 && (!Terrain::terrainTextures->isCement(vertices[0]->pVertex->textureData & 0x0000ffff) || Terrain::terrainTextures->isAlpha(vertices[0]->pVertex->textureData & 0x0000ffff)))
				lightRGB0 = lightRGB1 = lightRGB3 = 0xffffffff;
			lightRGB0 = vertices[0]->pVertex->selected ? SELECTION_COLOR : lightRGB0;
			lightRGB1 = vertices[1]->pVertex->selected ? SELECTION_COLOR : lightRGB1;
			lightRGB3 = vertices[3]->pVertex->selected ? SELECTION_COLOR : lightRGB3;
			gVertex[0].x = vertices[0]->px;
			gVertex[0].y = vertices[0]->py;
			gVertex[0].z = vertices[0]->pz + TERRAIN_DEPTH_FUDGE;
			gVertex[0].rhw = vertices[0]->pw;
			gVertex[0].u = minU;
			gVertex[0].v = minV;
			gVertex[0].argb = lightRGB0;
			gVertex[0].frgb = vertices[0]->fogRGB;
			gVertex[1].x = vertices[1]->px;
			gVertex[1].y = vertices[1]->py;
			gVertex[1].z = vertices[1]->pz + TERRAIN_DEPTH_FUDGE;
			gVertex[1].rhw = vertices[1]->pw;
			gVertex[1].u = maxU;
			gVertex[1].v = minV;
			gVertex[1].argb = lightRGB1;
			gVertex[1].frgb = vertices[1]->fogRGB;
			gVertex[2].x = vertices[3]->px;
			gVertex[2].y = vertices[3]->py;
			gVertex[2].z = vertices[3]->pz + TERRAIN_DEPTH_FUDGE;
			gVertex[2].rhw = vertices[3]->pw;
			gVertex[2].u = minU;
			gVertex[2].v = maxV;
			gVertex[2].argb = lightRGB3;
			gVertex[2].frgb = vertices[3]->fogRGB;
			if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) && (gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
			{
				{
					if ((terrainDetailHandle == 0xffffffff) && (overlayHandle == 0xffffffff) && isCement)
						mcTextureManager->addVertices(
							terrainHandle, gVertex, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
					else
						mcTextureManager->addVertices(
							terrainHandle, gVertex, MC2_ISTERRAIN | MC2_DRAWSOLID);
					//----------------------------------------------------
					// Draw the detail Texture
					if (useWaterInterestTexture && (terrainDetailHandle != 0xffffffff))
					{
						gos_VERTEX sVertex[3];
						memcpy(sVertex, gVertex, sizeof(gos_VERTEX) * 3);
						float tilingFactor = Terrain::terrainTextures->getDetailTilingFactor(1);
						if (Terrain::terrainTextures2)
							tilingFactor = Terrain::terrainTextures2->getDetailTilingFactor();
						float oneOverTF = tilingFactor / Terrain::worldUnitsMapSide;
						sVertex[0].u = (vertices[0]->vx - Terrain::mapTopLeft3d.x) * oneOverTF;
						sVertex[0].v = (Terrain::mapTopLeft3d.y - vertices[0]->vy) * oneOverTF;
						sVertex[1].u = (vertices[1]->vx - Terrain::mapTopLeft3d.x) * oneOverTF;
						sVertex[1].v = (Terrain::mapTopLeft3d.y - vertices[1]->vy) * oneOverTF;
						sVertex[2].u = (vertices[3]->vx - Terrain::mapTopLeft3d.x) * oneOverTF;
						sVertex[2].v = (Terrain::mapTopLeft3d.y - vertices[3]->vy) * oneOverTF;
						if ((sVertex[0].u > MaxMinUV) || (sVertex[0].v > MaxMinUV) || (sVertex[1].u > MaxMinUV) || (sVertex[1].v > MaxMinUV) || (sVertex[2].u > MaxMinUV) || (sVertex[2].v > MaxMinUV))
						{
							// If any are out range, move 'em back in range by
							// adjustfactor.
							float maxU = fmax(sVertex[0].u, fmax(sVertex[1].u, sVertex[2].u));
							maxU = floor(maxU - (MaxMinUV - 1.0f));
							float maxV = fmax(sVertex[0].v, fmax(sVertex[1].v, sVertex[2].v));
							maxV = floor(maxV - (MaxMinUV - 1.0f));
							sVertex[0].u -= maxU;
							sVertex[1].u -= maxU;
							sVertex[2].u -= maxU;
							sVertex[0].v -= maxV;
							sVertex[1].v -= maxV;
							sVertex[2].v -= maxV;
						}
						// Light the Detail Texture
						if (Terrain::terrainTextures2)
						{
							sVertex[0].argb = sVertex[1].argb = sVertex[2].argb = 0xffffffff;
						}
						mcTextureManager->addVertices(
							terrainDetailHandle, sVertex, MC2_ISTERRAIN | MC2_DRAWALPHA);
					}
					//--------------------------------------------------------------
					// Draw the Overlay Texture if it exists.
					if (useOverlayTexture && (overlayHandle != 0xffffffff))
					{
						// Uses EXACT same coords as the above normal texture.
						// Just replace the UVs and the texture handle!!
						gos_VERTEX oVertex[3];
						memcpy(oVertex, gVertex, sizeof(gos_VERTEX) * 3);
						oVertex[0].u = oldminU;
						oVertex[0].v = oldminV;
						oVertex[1].u = oldmaxU;
						oVertex[1].v = oldminV;
						oVertex[2].u = oldminU;
						oVertex[2].v = oldmaxV;
						// Light the overlays!!
						oVertex[0].argb = vertices[0]->lightRGB;
						oVertex[1].argb = vertices[1]->lightRGB;
						oVertex[2].argb = vertices[3]->lightRGB;
						mcTextureManager->addVertices(
							overlayHandle, oVertex, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
					}
				}
			}
			//---------------------------------------
			// Bottom Triangle.
			// gVertex[0] is same as above gVertex[1]
			// gVertex[1] is new and calced from vertex[2].
			// gVertex[2] is same as above.
			uint32_t lightRGB2 = vertices[2]->lightRGB;
			if (Terrain::terrainTextures2 && (!Terrain::terrainTextures->isCement(vertices[0]->pVertex->textureData & 0x0000ffff) || Terrain::terrainTextures->isAlpha(vertices[0]->pVertex->textureData & 0x0000ffff)))
				lightRGB2 = 0xffffffff;
			lightRGB2 = vertices[2]->pVertex->selected ? SELECTION_COLOR : lightRGB2;
			gVertex[0].x = gVertex[1].x;
			gVertex[0].y = gVertex[1].y;
			gVertex[0].z = gVertex[1].z;
			gVertex[0].rhw = gVertex[1].rhw;
			gVertex[0].u = gVertex[1].u;
			gVertex[0].v = gVertex[1].v;
			gVertex[0].argb = gVertex[1].argb;
			gVertex[0].frgb = gVertex[1].frgb;
			gVertex[1].x = vertices[2]->px;
			gVertex[1].y = vertices[2]->py;
			gVertex[1].z = vertices[2]->pz + TERRAIN_DEPTH_FUDGE;
			gVertex[1].rhw = vertices[2]->pw;
			gVertex[1].u = maxU;
			gVertex[1].v = maxV;
			gVertex[1].argb = lightRGB2;
			gVertex[1].frgb = vertices[2]->fogRGB;
			if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) && (gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
			{
				{
					if ((terrainDetailHandle == 0xffffffff) && (overlayHandle == 0xffffffff) && isCement)
						mcTextureManager->addVertices(
							terrainHandle, gVertex, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
					else
						mcTextureManager->addVertices(
							terrainHandle, gVertex, MC2_ISTERRAIN | MC2_DRAWSOLID);
					//----------------------------------------------------
					// Draw the detail Texture
					if (useWaterInterestTexture && (terrainDetailHandle != 0xffffffff))
					{
						gos_VERTEX sVertex[3];
						memcpy(sVertex, gVertex, sizeof(gos_VERTEX) * 3);
						float tilingFactor = Terrain::terrainTextures->getDetailTilingFactor(1);
						if (Terrain::terrainTextures2)
							tilingFactor = Terrain::terrainTextures2->getDetailTilingFactor();
						float oneOverTf = tilingFactor / Terrain::worldUnitsMapSide;
						sVertex[0].u = (vertices[1]->vx - Terrain::mapTopLeft3d.x) * oneOverTf;
						sVertex[0].v = (Terrain::mapTopLeft3d.y - vertices[1]->vy) * oneOverTf;
						sVertex[1].u = (vertices[2]->vx - Terrain::mapTopLeft3d.x) * oneOverTf;
						sVertex[1].v = (Terrain::mapTopLeft3d.y - vertices[2]->vy) * oneOverTf;
						sVertex[2].u = (vertices[3]->vx - Terrain::mapTopLeft3d.x) * oneOverTf;
						sVertex[2].v = (Terrain::mapTopLeft3d.y - vertices[3]->vy) * oneOverTf;
						if ((sVertex[0].u > MaxMinUV) || (sVertex[0].v > MaxMinUV) || (sVertex[1].u > MaxMinUV) || (sVertex[1].v > MaxMinUV) || (sVertex[2].u > MaxMinUV) || (sVertex[2].v > MaxMinUV))
						{
							// If any are out range, move 'em back in range by
							// adjustfactor.
							float maxU = fmax(sVertex[0].u, fmax(sVertex[1].u, sVertex[2].u));
							maxU = floor(maxU - (MaxMinUV - 1.0f));
							float maxV = fmax(sVertex[0].v, fmax(sVertex[1].v, sVertex[2].v));
							maxV = floor(maxV - (MaxMinUV - 1.0f));
							sVertex[0].u -= maxU;
							sVertex[1].u -= maxU;
							sVertex[2].u -= maxU;
							sVertex[0].v -= maxV;
							sVertex[1].v -= maxV;
							sVertex[2].v -= maxV;
						}
						// Light the Detail Texture
						if (Terrain::terrainTextures2)
						{
							sVertex[0].argb = sVertex[1].argb = sVertex[2].argb = 0xffffffff;
						}
						mcTextureManager->addVertices(
							terrainDetailHandle, sVertex, MC2_ISTERRAIN | MC2_DRAWALPHA);
					}
					//--------------------------------------------------------------
					// Draw the Overlay Texture if it exists.
					if (useOverlayTexture && (overlayHandle != 0xffffffff))
					{
						// Uses EXACT same coords as the above normal texture.
						// Just replace the UVs and the texture handle!!
						gos_VERTEX oVertex[3];
						memcpy(oVertex, gVertex, sizeof(gos_VERTEX) * 3);
						oVertex[0].u = oldmaxU;
						oVertex[0].v = oldminV;
						oVertex[1].u = oldmaxU;
						oVertex[1].v = oldmaxV;
						oVertex[2].u = oldminU;
						oVertex[2].v = oldmaxV;
						// Light the overlays!!
						oVertex[0].argb = vertices[1]->lightRGB;
						oVertex[1].argb = vertices[2]->lightRGB;
						oVertex[2].argb = vertices[3]->lightRGB;
						mcTextureManager->addVertices(
							overlayHandle, oVertex, MC2_ISTERRAIN | MC2_DRAWALPHA | MC2_ISCRATERS);
					}
				}
			}
		}
	}
#ifdef _DEBUG
	if (selected)
	{
		drawLine();
		selected = FALSE;
	}
#endif
}

extern float elevationAnimation;
#define SKY_FUDGE (1.0f / 60000.0f)
#define WATER_ALPHA 0x1fffffff
extern float cloudScrollX;
extern float cloudScrollY;

//---------------------------------------------------------------------------
void TerrainQuad::drawWater(void)
{
	float cloudOffsetX = cos(360.0f * DEGREES_TO_RADS * 32.0f * cloudScrollX) * 0.1f;
	float cloudOffsetY = sin(360.0f * DEGREES_TO_RADS * 32.0f * cloudScrollY) * 0.1f;
	float sprayOffsetX = cloudScrollX * 10.0f;
	float sprayOffsetY = cloudScrollY * 10.0f;
	// Gotta be able to run the untextured maps!!!
	float oneOverWaterTF = 1.0f / 64.0f;
	float oneOverTF = 1.0f / 64.0f;
	if (Terrain::terrainTextures2)
	{
		oneOverWaterTF =
			(Terrain::terrainTextures2->getWaterDetailTilingFactor() / Terrain::worldUnitsMapSide);
		oneOverTF =
			(Terrain::terrainTextures2->getWaterTextureTilingFactor() / Terrain::worldUnitsMapSide);
	}
	if (waterHandle != 0xffffffff)
	{
		numTerrainFaces++;
		//---------------------------------------
		// GOS 3D draw Calls now!
		float minU = 0.0f;
		float maxU = 0.9999999f;
		float minV = 0.0f;
		float maxV = 0.9999999f;
		//-----------------------------------------------------
		// FOG time.  Set Render state to FOG on!
		if (useFog)
		{
			uint32_t fogcolour = eye->fogcolour;
			gos_SetRenderState(gos_State_Fog, (int32_t)&fogcolour);
		}
		else
		{
			gos_SetRenderState(gos_State_Fog, 0);
		}
		gos_VERTEX gVertex[3];
		if (uvMode == BOTTOMRIGHT)
		{
			//--------------------------
			// Top Triangle
			gVertex[0].x = vertices[0]->wx;
			gVertex[0].y = vertices[0]->wy;
			gVertex[0].z = vertices[0]->wz + TERRAIN_DEPTH_FUDGE;
			gVertex[0].rhw = vertices[0]->ww;
			gVertex[0].u = minU + cloudOffsetX;
			gVertex[0].v = minV + cloudOffsetY;
			gVertex[0].argb =
				vertices[0]->pVertex->selected ? SELECTION_COLOR : vertices[0]->lightRGB;
			gVertex[0].frgb = vertices[0]->fogRGB;
			gVertex[1].x = vertices[1]->wx;
			gVertex[1].y = vertices[1]->wy;
			gVertex[1].z = vertices[1]->wz + TERRAIN_DEPTH_FUDGE;
			gVertex[1].rhw = vertices[1]->ww;
			gVertex[1].u = maxU + cloudOffsetX;
			gVertex[1].v = minV + cloudOffsetY;
			gVertex[1].argb =
				vertices[1]->pVertex->selected ? SELECTION_COLOR : vertices[1]->lightRGB;
			gVertex[1].frgb = vertices[1]->fogRGB;
			gVertex[2].x = vertices[2]->wx;
			gVertex[2].y = vertices[2]->wy;
			gVertex[2].z = vertices[2]->wz + TERRAIN_DEPTH_FUDGE;
			gVertex[2].rhw = vertices[2]->ww;
			gVertex[2].u = maxU + cloudOffsetX;
			gVertex[2].v = maxV + cloudOffsetY;
			gVertex[2].argb =
				vertices[2]->pVertex->selected ? SELECTION_COLOR : vertices[2]->lightRGB;
			gVertex[2].frgb = vertices[2]->fogRGB;
			gVertex[0].u = (vertices[0]->vx - Terrain::mapTopLeft3d.x) * oneOverTF + cloudOffsetX;
			gVertex[0].v = (Terrain::mapTopLeft3d.y - vertices[0]->vy) * oneOverTF + cloudOffsetY;
			gVertex[1].u = (vertices[1]->vx - Terrain::mapTopLeft3d.x) * oneOverTF + cloudOffsetX;
			gVertex[1].v = (Terrain::mapTopLeft3d.y - vertices[1]->vy) * oneOverTF + cloudOffsetY;
			gVertex[2].u = (vertices[2]->vx - Terrain::mapTopLeft3d.x) * oneOverTF + cloudOffsetX;
			gVertex[2].v = (Terrain::mapTopLeft3d.y - vertices[2]->vy) * oneOverTF + cloudOffsetY;
			if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) && (gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
			{
				{
					//-----------------------------------------------------------------------------
					// Reject Any triangle which has vertices off screeen in
					// software for now. Do real cliping in geometry layer for
					// software and hardware that needs it!
					if (waterHandle != 0xffffffff)
					{
						uint32_t alphaMode0 = Terrain::alphaMiddle;
						uint32_t alphaMode1 = Terrain::alphaMiddle;
						uint32_t alphaMode2 = Terrain::alphaMiddle;
						if (vertices[0]->pVertex->elevation >= (Terrain::waterElevation - MapData::alphaDepth))
						{
							alphaMode0 = Terrain::alphaEdge;
						}
						if (vertices[1]->pVertex->elevation >= (Terrain::waterElevation - MapData::alphaDepth))
						{
							alphaMode1 = Terrain::alphaEdge;
						}
						if (vertices[2]->pVertex->elevation >= (Terrain::waterElevation - MapData::alphaDepth))
						{
							alphaMode2 = Terrain::alphaEdge;
						}
						if (vertices[0]->pVertex->elevation <= (Terrain::waterElevation - (MapData::alphaDepth * 3.0f)))
						{
							alphaMode0 = Terrain::alphaDeep;
						}
						if (vertices[1]->pVertex->elevation <= (Terrain::waterElevation - (MapData::alphaDepth * 3.0f)))
						{
							alphaMode1 = Terrain::alphaDeep;
						}
						if (vertices[2]->pVertex->elevation <= (Terrain::waterElevation - (MapData::alphaDepth * 3.0f)))
						{
							alphaMode2 = Terrain::alphaDeep;
						}
						gVertex[0].argb = (vertices[0]->lightRGB & 0x00ffffff) + alphaMode0;
						gVertex[1].argb = (vertices[1]->lightRGB & 0x00ffffff) + alphaMode1;
						gVertex[2].argb = (vertices[2]->lightRGB & 0x00ffffff) + alphaMode2;
						if ((gVertex[0].u > MaxMinUV) || (gVertex[0].v > MaxMinUV) || (gVertex[1].u > MaxMinUV) || (gVertex[1].v > MaxMinUV) || (gVertex[2].u > MaxMinUV) || (gVertex[2].v > MaxMinUV))
						{
							// If any are out range, move 'em back in range by
							// adjustfactor.
							float maxU = fmax(gVertex[0].u, fmax(gVertex[1].u, gVertex[2].u));
							maxU = floor(maxU - (MaxMinUV - 1.0f));
							float maxV = fmax(gVertex[0].v, fmax(gVertex[1].v, gVertex[2].v));
							maxV = floor(maxV - (MaxMinUV - 1.0f));
							gVertex[0].u -= maxU;
							gVertex[1].u -= maxU;
							gVertex[2].u -= maxU;
							gVertex[0].v -= maxV;
							gVertex[1].v -= maxV;
							gVertex[2].v -= maxV;
						}
						if (alphaMode0 + alphaMode1 + alphaMode2)
						{
							mcTextureManager->addVertices(
								waterHandle, gVertex, MC2_ISTERRAIN | MC2_DRAWALPHA);
						}
					}
					//----------------------------------------------------
					// Draw the sky reflection on the water.
					if (useWaterInterestTexture && (waterDetailHandle != 0xffffffff))
					{
						gos_VERTEX sVertex[3];
						memcpy(sVertex, gVertex, sizeof(gos_VERTEX) * 3);
						sVertex[0].u =
							((vertices[0]->vx - Terrain::mapTopLeft3d.x) * oneOverWaterTF) + sprayOffsetX;
						sVertex[0].v =
							((Terrain::mapTopLeft3d.y - vertices[0]->vy) * oneOverWaterTF) + sprayOffsetY;
						sVertex[1].u =
							((vertices[1]->vx - Terrain::mapTopLeft3d.x) * oneOverWaterTF) + sprayOffsetX;
						sVertex[1].v =
							((Terrain::mapTopLeft3d.y - vertices[1]->vy) * oneOverWaterTF) + sprayOffsetY;
						sVertex[2].u =
							((vertices[2]->vx - Terrain::mapTopLeft3d.x) * oneOverWaterTF) + sprayOffsetX;
						sVertex[2].v =
							((Terrain::mapTopLeft3d.y - vertices[2]->vy) * oneOverWaterTF) + sprayOffsetY;
						sVertex[0].argb = (sVertex[0].argb & 0xff000000) + 0xffffff;
						sVertex[1].argb = (sVertex[1].argb & 0xff000000) + 0xffffff;
						sVertex[2].argb = (sVertex[2].argb & 0xff000000) + 0xffffff;
						mcTextureManager->addVertices(
							waterDetailHandle, sVertex, MC2_ISTERRAIN | MC2_DRAWALPHA);
					}
				}
			}
			//--------------------------
			// Bottom Triangle
			//
			// gVertex[0] same as above gVertex[0].
			// gVertex[1] is same as above gVertex[2].
			// gVertex[2] is calced from vertex[3].
			gVertex[1].x = gVertex[2].x;
			gVertex[1].y = gVertex[2].y;
			gVertex[1].z = gVertex[2].z;
			gVertex[1].rhw = gVertex[2].rhw;
			gVertex[1].u = gVertex[2].u;
			gVertex[1].v = gVertex[2].v;
			gVertex[1].argb = gVertex[2].argb;
			gVertex[1].frgb = gVertex[2].frgb;
			gVertex[2].x = vertices[3]->wx;
			gVertex[2].y = vertices[3]->wy;
			gVertex[2].z = vertices[3]->wz + TERRAIN_DEPTH_FUDGE;
			gVertex[2].rhw = vertices[3]->ww;
			gVertex[2].u = minU + cloudOffsetX;
			gVertex[2].v = maxV + cloudOffsetY;
			gVertex[2].argb =
				vertices[3]->pVertex->selected ? SELECTION_COLOR : vertices[3]->lightRGB;
			gVertex[2].frgb = vertices[3]->fogRGB;
			gVertex[0].u = (vertices[0]->vx - Terrain::mapTopLeft3d.x) * oneOverTF + cloudOffsetX;
			gVertex[0].v = (Terrain::mapTopLeft3d.y - vertices[0]->vy) * oneOverTF + cloudOffsetY;
			gVertex[1].u = (vertices[2]->vx - Terrain::mapTopLeft3d.x) * oneOverTF + cloudOffsetX;
			gVertex[1].v = (Terrain::mapTopLeft3d.y - vertices[2]->vy) * oneOverTF + cloudOffsetY;
			gVertex[2].u = (vertices[3]->vx - Terrain::mapTopLeft3d.x) * oneOverTF + cloudOffsetX;
			gVertex[2].v = (Terrain::mapTopLeft3d.y - vertices[3]->vy) * oneOverTF + cloudOffsetY;
			if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) && (gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
			{
				{
					//-----------------------------------------------------------------------------
					// Reject Any triangle which has vertices off screeen in
					// software for now. Do real cliping in geometry layer for
					// software and hardware that needs it!
					if (waterHandle != 0xffffffff)
					{
						uint32_t alphaMode0 = Terrain::alphaMiddle;
						uint32_t alphaMode1 = Terrain::alphaMiddle;
						uint32_t alphaMode2 = Terrain::alphaMiddle;
						if (vertices[0]->pVertex->elevation >= (Terrain::waterElevation - MapData::alphaDepth))
						{
							alphaMode0 = Terrain::alphaEdge;
						}
						if (vertices[2]->pVertex->elevation >= (Terrain::waterElevation - MapData::alphaDepth))
						{
							alphaMode1 = Terrain::alphaEdge;
						}
						if (vertices[3]->pVertex->elevation >= (Terrain::waterElevation - MapData::alphaDepth))
						{
							alphaMode2 = Terrain::alphaEdge;
						}
						if (vertices[0]->pVertex->elevation <= (Terrain::waterElevation - (MapData::alphaDepth * 3.0f)))
						{
							alphaMode0 = Terrain::alphaDeep;
						}
						if (vertices[2]->pVertex->elevation <= (Terrain::waterElevation - (MapData::alphaDepth * 3.0f)))
						{
							alphaMode1 = Terrain::alphaDeep;
						}
						if (vertices[3]->pVertex->elevation <= (Terrain::waterElevation - (MapData::alphaDepth * 3.0f)))
						{
							alphaMode2 = Terrain::alphaDeep;
						}
						gVertex[0].argb = (vertices[0]->lightRGB & 0x00ffffff) + alphaMode0;
						gVertex[1].argb = (vertices[2]->lightRGB & 0x00ffffff) + alphaMode1;
						gVertex[2].argb = (vertices[3]->lightRGB & 0x00ffffff) + alphaMode2;
						if ((gVertex[0].u > MaxMinUV) || (gVertex[0].v > MaxMinUV) || (gVertex[1].u > MaxMinUV) || (gVertex[1].v > MaxMinUV) || (gVertex[2].u > MaxMinUV) || (gVertex[2].v > MaxMinUV))
						{
							// If any are out range, move 'em back in range by
							// adjustfactor.
							float maxU = fmax(gVertex[0].u, fmax(gVertex[1].u, gVertex[2].u));
							maxU = floor(maxU - (MaxMinUV - 1.0f));
							float maxV = fmax(gVertex[0].v, fmax(gVertex[1].v, gVertex[2].v));
							maxV = floor(maxV - (MaxMinUV - 1.0f));
							gVertex[0].u -= maxU;
							gVertex[1].u -= maxU;
							gVertex[2].u -= maxU;
							gVertex[0].v -= maxV;
							gVertex[1].v -= maxV;
							gVertex[2].v -= maxV;
						}
						if (alphaMode0 + alphaMode1 + alphaMode2)
						{
							mcTextureManager->addVertices(
								waterHandle, gVertex, MC2_ISTERRAIN | MC2_DRAWALPHA);
						}
					}
					//----------------------------------------------------
					// Draw the sky reflection on the water.
					if (useWaterInterestTexture && (waterDetailHandle != 0xffffffff))
					{
						gos_VERTEX sVertex[3];
						memcpy(sVertex, gVertex, sizeof(gos_VERTEX) * 3);
						sVertex[0].u =
							((vertices[0]->vx - Terrain::mapTopLeft3d.x) * oneOverWaterTF) + sprayOffsetX;
						sVertex[0].v =
							((Terrain::mapTopLeft3d.y - vertices[0]->vy) * oneOverWaterTF) + sprayOffsetY;
						sVertex[1].u =
							((vertices[2]->vx - Terrain::mapTopLeft3d.x) * oneOverWaterTF) + sprayOffsetX;
						sVertex[1].v =
							((Terrain::mapTopLeft3d.y - vertices[2]->vy) * oneOverWaterTF) + sprayOffsetY;
						sVertex[2].u =
							((vertices[3]->vx - Terrain::mapTopLeft3d.x) * oneOverWaterTF) + sprayOffsetX;
						sVertex[2].v =
							((Terrain::mapTopLeft3d.y - vertices[3]->vy) * oneOverWaterTF) + sprayOffsetY;
						sVertex[0].argb = (sVertex[0].argb & 0xff000000) + 0xffffff;
						sVertex[1].argb = (sVertex[1].argb & 0xff000000) + 0xffffff;
						sVertex[2].argb = (sVertex[2].argb & 0xff000000) + 0xffffff;
						mcTextureManager->addVertices(
							waterDetailHandle, sVertex, MC2_ISTERRAIN | MC2_DRAWALPHA);
					}
				}
			}
		}
		else if (uvMode == BOTTOMLEFT)
		{
			//------------------------------
			// Top Triangle.
			gVertex[0].x = vertices[0]->wx;
			gVertex[0].y = vertices[0]->wy;
			gVertex[0].z = vertices[0]->wz + TERRAIN_DEPTH_FUDGE;
			gVertex[0].rhw = vertices[0]->ww;
			gVertex[0].u = minU + cloudOffsetX;
			;
			gVertex[0].v = minV + cloudOffsetY;
			;
			gVertex[0].argb =
				vertices[0]->pVertex->selected ? SELECTION_COLOR : vertices[0]->lightRGB;
			gVertex[0].frgb = vertices[0]->fogRGB;
			gVertex[1].x = vertices[1]->wx;
			gVertex[1].y = vertices[1]->wy;
			gVertex[1].z = vertices[1]->wz + TERRAIN_DEPTH_FUDGE;
			gVertex[1].rhw = vertices[1]->ww;
			gVertex[1].u = maxU + cloudOffsetX;
			;
			gVertex[1].v = minV + cloudOffsetY;
			;
			gVertex[1].argb =
				vertices[1]->pVertex->selected ? SELECTION_COLOR : vertices[1]->lightRGB;
			gVertex[1].frgb = vertices[1]->fogRGB;
			gVertex[2].x = vertices[3]->wx;
			gVertex[2].y = vertices[3]->wy;
			gVertex[2].z = vertices[3]->wz + TERRAIN_DEPTH_FUDGE;
			gVertex[2].rhw = vertices[3]->ww;
			gVertex[2].u = minU + cloudOffsetX;
			;
			gVertex[2].v = maxV + cloudOffsetY;
			;
			gVertex[2].argb =
				vertices[3]->pVertex->selected ? SELECTION_COLOR : vertices[3]->lightRGB;
			gVertex[2].frgb = vertices[3]->fogRGB;
			gVertex[0].u = (vertices[0]->vx - Terrain::mapTopLeft3d.x) * oneOverTF + cloudOffsetX;
			gVertex[0].v = (Terrain::mapTopLeft3d.y - vertices[0]->vy) * oneOverTF + cloudOffsetY;
			gVertex[1].u = (vertices[1]->vx - Terrain::mapTopLeft3d.x) * oneOverTF + cloudOffsetX;
			gVertex[1].v = (Terrain::mapTopLeft3d.y - vertices[1]->vy) * oneOverTF + cloudOffsetY;
			gVertex[2].u = (vertices[3]->vx - Terrain::mapTopLeft3d.x) * oneOverTF + cloudOffsetX;
			gVertex[2].v = (Terrain::mapTopLeft3d.y - vertices[3]->vy) * oneOverTF + cloudOffsetY;
			if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) && (gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
			{
				{
					//-----------------------------------------------------------------------------
					// Reject Any triangle which has vertices off screeen in
					// software for now. Do real cliping in geometry layer for
					// software and hardware that needs it!
					if (waterHandle != 0xffffffff)
					{
						uint32_t alphaMode0 = Terrain::alphaMiddle;
						uint32_t alphaMode1 = Terrain::alphaMiddle;
						uint32_t alphaMode2 = Terrain::alphaMiddle;
						if (vertices[0]->pVertex->elevation >= (Terrain::waterElevation - MapData::alphaDepth))
						{
							alphaMode0 = Terrain::alphaEdge;
						}
						if (vertices[1]->pVertex->elevation >= (Terrain::waterElevation - MapData::alphaDepth))
						{
							alphaMode1 = Terrain::alphaEdge;
						}
						if (vertices[3]->pVertex->elevation >= (Terrain::waterElevation - MapData::alphaDepth))
						{
							alphaMode2 = Terrain::alphaEdge;
						}
						if (vertices[0]->pVertex->elevation <= (Terrain::waterElevation - (MapData::alphaDepth * 3.0f)))
						{
							alphaMode0 = Terrain::alphaDeep;
						}
						if (vertices[1]->pVertex->elevation <= (Terrain::waterElevation - (MapData::alphaDepth * 3.0f)))
						{
							alphaMode1 = Terrain::alphaDeep;
						}
						if (vertices[3]->pVertex->elevation <= (Terrain::waterElevation - (MapData::alphaDepth * 3.0f)))
						{
							alphaMode2 = Terrain::alphaDeep;
						}
						gVertex[0].argb = (vertices[0]->lightRGB & 0x00ffffff) + alphaMode0;
						gVertex[1].argb = (vertices[1]->lightRGB & 0x00ffffff) + alphaMode1;
						gVertex[2].argb = (vertices[3]->lightRGB & 0x00ffffff) + alphaMode2;
						if ((gVertex[0].u > MaxMinUV) || (gVertex[0].v > MaxMinUV) || (gVertex[1].u > MaxMinUV) || (gVertex[1].v > MaxMinUV) || (gVertex[2].u > MaxMinUV) || (gVertex[2].v > MaxMinUV))
						{
							// If any are out range, move 'em back in range by
							// adjustfactor.
							float maxU = fmax(gVertex[0].u, fmax(gVertex[1].u, gVertex[2].u));
							maxU = floor(maxU - (MaxMinUV - 1.0f));
							float maxV = fmax(gVertex[0].v, fmax(gVertex[1].v, gVertex[2].v));
							maxV = floor(maxV - (MaxMinUV - 1.0f));
							gVertex[0].u -= maxU;
							gVertex[1].u -= maxU;
							gVertex[2].u -= maxU;
							gVertex[0].v -= maxV;
							gVertex[1].v -= maxV;
							gVertex[2].v -= maxV;
						}
						if (alphaMode0 + alphaMode1 + alphaMode2)
						{
							mcTextureManager->addVertices(
								waterHandle, gVertex, MC2_ISTERRAIN | MC2_DRAWALPHA);
						}
					}
					//----------------------------------------------------
					// Draw the sky reflection on the water.
					if (useWaterInterestTexture && (waterDetailHandle != 0xffffffff))
					{
						gos_VERTEX sVertex[3];
						memcpy(sVertex, gVertex, sizeof(gos_VERTEX) * 3);
						sVertex[0].u =
							((vertices[0]->vx - Terrain::mapTopLeft3d.x) * oneOverWaterTF) + sprayOffsetX;
						sVertex[0].v =
							((Terrain::mapTopLeft3d.y - vertices[0]->vy) * oneOverWaterTF) + sprayOffsetY;
						sVertex[1].u =
							((vertices[1]->vx - Terrain::mapTopLeft3d.x) * oneOverWaterTF) + sprayOffsetX;
						sVertex[1].v =
							((Terrain::mapTopLeft3d.y - vertices[1]->vy) * oneOverWaterTF) + sprayOffsetY;
						sVertex[2].u =
							((vertices[3]->vx - Terrain::mapTopLeft3d.x) * oneOverWaterTF) + sprayOffsetX;
						sVertex[2].v =
							((Terrain::mapTopLeft3d.y - vertices[3]->vy) * oneOverWaterTF) + sprayOffsetY;
						sVertex[0].argb = (sVertex[0].argb & 0xff000000) + 0xffffff;
						sVertex[1].argb = (sVertex[1].argb & 0xff000000) + 0xffffff;
						sVertex[2].argb = (sVertex[2].argb & 0xff000000) + 0xffffff;
						mcTextureManager->addVertices(
							waterDetailHandle, sVertex, MC2_ISTERRAIN | MC2_DRAWALPHA);
					}
				}
			}
			//---------------------------------------
			// Bottom Triangle.
			// gVertex[0] is same as above gVertex[1]
			// gVertex[1] is new and calced from vertex[2].
			// gVertex[2] is same as above.
			gVertex[0].x = gVertex[1].x;
			gVertex[0].y = gVertex[1].y;
			gVertex[0].z = gVertex[1].z;
			gVertex[0].rhw = gVertex[1].rhw;
			gVertex[0].u = gVertex[1].u;
			gVertex[0].v = gVertex[1].v;
			gVertex[0].argb = gVertex[1].argb;
			gVertex[0].frgb = gVertex[1].frgb;
			gVertex[1].x = vertices[2]->wx;
			gVertex[1].y = vertices[2]->wy;
			gVertex[1].z = vertices[2]->wz + TERRAIN_DEPTH_FUDGE;
			gVertex[1].rhw = vertices[2]->ww;
			gVertex[1].u = maxU + cloudOffsetX;
			;
			gVertex[1].v = maxV + cloudOffsetY;
			;
			gVertex[1].argb =
				vertices[2]->pVertex->selected ? SELECTION_COLOR : vertices[2]->lightRGB;
			gVertex[1].frgb = vertices[2]->fogRGB;
			gVertex[0].u = (vertices[1]->vx - Terrain::mapTopLeft3d.x) * oneOverTF + cloudOffsetX;
			gVertex[0].v = (Terrain::mapTopLeft3d.y - vertices[1]->vy) * oneOverTF + cloudOffsetY;
			gVertex[1].u = (vertices[2]->vx - Terrain::mapTopLeft3d.x) * oneOverTF + cloudOffsetX;
			gVertex[1].v = (Terrain::mapTopLeft3d.y - vertices[2]->vy) * oneOverTF + cloudOffsetY;
			gVertex[2].u = (vertices[3]->vx - Terrain::mapTopLeft3d.x) * oneOverTF + cloudOffsetX;
			gVertex[2].v = (Terrain::mapTopLeft3d.y - vertices[3]->vy) * oneOverTF + cloudOffsetY;
			if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) && (gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
			{
				{
					//-----------------------------------------------------------------------------
					// Reject Any triangle which has vertices off screeen in
					// software for now. Do real cliping in geometry layer for
					// software and hardware that needs it!
					if (waterHandle != 0xffffffff)
					{
						uint32_t alphaMode0 = Terrain::alphaMiddle;
						uint32_t alphaMode1 = Terrain::alphaMiddle;
						uint32_t alphaMode2 = Terrain::alphaMiddle;
						if (vertices[1]->pVertex->elevation >= (Terrain::waterElevation - MapData::alphaDepth))
						{
							alphaMode0 = Terrain::alphaEdge;
						}
						if (vertices[2]->pVertex->elevation >= (Terrain::waterElevation - MapData::alphaDepth))
						{
							alphaMode1 = Terrain::alphaEdge;
						}
						if (vertices[3]->pVertex->elevation >= (Terrain::waterElevation - MapData::alphaDepth))
						{
							alphaMode2 = Terrain::alphaEdge;
						}
						if (vertices[1]->pVertex->elevation <= (Terrain::waterElevation - (MapData::alphaDepth * 3.0f)))
						{
							alphaMode0 = Terrain::alphaDeep;
						}
						if (vertices[2]->pVertex->elevation <= (Terrain::waterElevation - (MapData::alphaDepth * 3.0f)))
						{
							alphaMode1 = Terrain::alphaDeep;
						}
						if (vertices[3]->pVertex->elevation <= (Terrain::waterElevation - (MapData::alphaDepth * 3.0f)))
						{
							alphaMode2 = Terrain::alphaDeep;
						}
						gVertex[0].argb = (vertices[1]->lightRGB & 0x00ffffff) + alphaMode0;
						gVertex[1].argb = (vertices[2]->lightRGB & 0x00ffffff) + alphaMode1;
						gVertex[2].argb = (vertices[3]->lightRGB & 0x00ffffff) + alphaMode2;
						if ((gVertex[0].u > MaxMinUV) || (gVertex[0].v > MaxMinUV) || (gVertex[1].u > MaxMinUV) || (gVertex[1].v > MaxMinUV) || (gVertex[2].u > MaxMinUV) || (gVertex[2].v > MaxMinUV))
						{
							// If any are out range, move 'em back in range by
							// adjustfactor.
							float maxU = fmax(gVertex[0].u, fmax(gVertex[1].u, gVertex[2].u));
							maxU = floor(maxU - (MaxMinUV - 1.0f));
							float maxV = fmax(gVertex[0].v, fmax(gVertex[1].v, gVertex[2].v));
							maxV = floor(maxV - (MaxMinUV - 1.0f));
							gVertex[0].u -= maxU;
							gVertex[1].u -= maxU;
							gVertex[2].u -= maxU;
							gVertex[0].v -= maxV;
							gVertex[1].v -= maxV;
							gVertex[2].v -= maxV;
						}
						if (alphaMode0 + alphaMode1 + alphaMode2)
						{
							mcTextureManager->addVertices(
								waterHandle, gVertex, MC2_ISTERRAIN | MC2_DRAWALPHA);
						}
					}
					//----------------------------------------------------
					// Draw the sky reflection on the water.
					if (useWaterInterestTexture && (waterDetailHandle != 0xffffffff))
					{
						gos_VERTEX sVertex[3];
						memcpy(sVertex, gVertex, sizeof(gos_VERTEX) * 3);
						sVertex[0].u =
							((vertices[1]->vx - Terrain::mapTopLeft3d.x) * oneOverWaterTF) + sprayOffsetX;
						sVertex[0].v =
							((Terrain::mapTopLeft3d.y - vertices[1]->vy) * oneOverWaterTF) + sprayOffsetY;
						sVertex[1].u =
							((vertices[2]->vx - Terrain::mapTopLeft3d.x) * oneOverWaterTF) + sprayOffsetX;
						sVertex[1].v =
							((Terrain::mapTopLeft3d.y - vertices[2]->vy) * oneOverWaterTF) + sprayOffsetY;
						sVertex[2].u =
							((vertices[3]->vx - Terrain::mapTopLeft3d.x) * oneOverWaterTF) + sprayOffsetX;
						sVertex[2].v =
							((Terrain::mapTopLeft3d.y - vertices[3]->vy) * oneOverWaterTF) + sprayOffsetY;
						sVertex[0].argb = (sVertex[0].argb & 0xff000000) + 0xffffff;
						sVertex[1].argb = (sVertex[1].argb & 0xff000000) + 0xffffff;
						sVertex[2].argb = (sVertex[2].argb & 0xff000000) + 0xffffff;
						mcTextureManager->addVertices(
							waterDetailHandle, sVertex, MC2_ISTERRAIN | MC2_DRAWALPHA);
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
int32_t DrawDebugCells = 0;

void TerrainQuad::drawLine(void)
{
	int32_t clipped1 = vertices[0]->clipInfo + vertices[1]->clipInfo + vertices[2]->clipInfo;
	int32_t clipped2 = vertices[0]->clipInfo + vertices[2]->clipInfo + vertices[3]->clipInfo;
	if (uvMode == BOTTOMLEFT)
	{
		clipped1 = vertices[0]->clipInfo + vertices[1]->clipInfo + vertices[3]->clipInfo;
		clipped2 = vertices[1]->clipInfo + vertices[2]->clipInfo + vertices[3]->clipInfo;
	}
	//------------------------------------------------------------
	// Draw the Tile block lines at depth just above base tiles.
	int32_t color = XP_WHITE;
	if (uvMode == BOTTOMRIGHT)
	{
		if (clipped1 != 0)
		{
			Stuff::Vector4D pos1(
				vertices[0]->px, vertices[0]->py, vertices[0]->pz - 0.002f, 1.0f / vertices[0]->pw);
			Stuff::Vector4D pos2(
				vertices[1]->px, vertices[1]->py, vertices[1]->pz - 0.002f, 1.0f / vertices[1]->pw);
			{
				LineElement newElement(pos1, pos2, color, nullptr);
				newElement.draw();
			}
			pos1.x = vertices[1]->px;
			pos1.y = vertices[1]->py;
			pos2.x = vertices[2]->px;
			pos2.y = vertices[2]->py;
			{
				LineElement newElement(pos1, pos2, color, nullptr);
				newElement.draw();
			}
			pos1.x = vertices[2]->px;
			pos1.y = vertices[2]->py;
			pos1.z = vertices[2]->pz - 0.002f;
			pos2.x = vertices[0]->px;
			pos2.y = vertices[0]->py;
			pos2.z = vertices[0]->pz - 0.002f;
			{
				LineElement newElement(pos1, pos2, color, nullptr);
				newElement.draw();
			}
		}
		if (clipped2 != 0)
		{
			Stuff::Vector4D pos1(
				vertices[0]->px, vertices[0]->py, vertices[0]->pz - 0.002f, 1.0f / vertices[0]->pw);
			Stuff::Vector4D pos2(
				vertices[2]->px, vertices[2]->py, vertices[2]->pz - 0.002f, 1.0f / vertices[2]->pw);
			{
				LineElement newElement(pos1, pos2, color, nullptr);
				newElement.draw();
			}
			pos1.x = vertices[2]->px;
			pos1.y = vertices[2]->py;
			pos1.z = vertices[2]->pz - 0.002f;
			pos2.x = vertices[3]->px;
			pos2.y = vertices[3]->py;
			pos2.z = vertices[3]->pz - 0.002f;
			{
				LineElement newElement(pos1, pos2, color, nullptr);
				newElement.draw();
			}
			pos1.x = vertices[3]->px;
			pos1.y = vertices[3]->py;
			pos1.z = vertices[3]->pz - 0.002f;
			pos2.x = vertices[0]->px;
			pos2.y = vertices[0]->py;
			pos2.z = vertices[0]->pz - 0.002f;
			{
				LineElement newElement(pos1, pos2, color, nullptr);
				newElement.draw();
			}
		}
	}
	else
	{
		if (clipped1 != 0)
		{
			Stuff::Vector4D pos1(
				vertices[0]->px, vertices[0]->py, vertices[0]->pz - 0.002f, 1.0f / vertices[0]->pw);
			Stuff::Vector4D pos2(
				vertices[1]->px, vertices[1]->py, vertices[1]->pz - 0.002f, 1.0f / vertices[1]->pw);
			{
				LineElement newElement(pos1, pos2, color, nullptr);
				newElement.draw();
			}
			pos1.x = vertices[1]->px;
			pos1.y = vertices[1]->py;
			pos1.z = vertices[1]->pz - 0.002f;
			pos2.x = vertices[3]->px;
			pos2.y = vertices[3]->py;
			pos2.z = vertices[3]->pz - 0.002f;
			{
				LineElement newElement(pos1, pos2, color, nullptr);
				newElement.draw();
			}
			pos1.x = vertices[3]->px;
			pos1.y = vertices[3]->py;
			pos1.z = vertices[3]->pz - 0.002f;
			pos2.x = vertices[0]->px;
			pos2.y = vertices[0]->py;
			pos2.z = vertices[0]->pz - 0.002f;
			{
				LineElement newElement(pos1, pos2, color, nullptr);
				newElement.draw();
			}
		}
		if (clipped2 != 0)
		{
			Stuff::Vector4D pos1(
				vertices[1]->px, vertices[1]->py, vertices[1]->pz - 0.002f, 1.0f / vertices[1]->pw);
			Stuff::Vector4D pos2(
				vertices[2]->px, vertices[2]->py, vertices[2]->pz - 0.002f, 1.0f / vertices[2]->pw);
			{
				LineElement newElement(pos1, pos2, color, nullptr);
				newElement.draw();
			}
			pos1.x = vertices[2]->px;
			pos1.y = vertices[2]->py;
			pos1.z = vertices[2]->pz - 0.002f;
			pos2.x = vertices[3]->px;
			pos2.y = vertices[3]->py;
			pos2.z = vertices[3]->pz - 0.002f;
			{
				LineElement newElement(pos1, pos2, color, nullptr);
				newElement.draw();
			}
			pos1.x = vertices[3]->px;
			pos1.y = vertices[3]->py;
			pos1.z = vertices[3]->pz - 0.002f;
			pos2.x = vertices[1]->px;
			pos2.y = vertices[1]->py;
			pos2.z = vertices[1]->pz - 0.002f;
			{
				LineElement newElement(pos1, pos2, color, nullptr);
				newElement.draw();
			}
		}
	}
	//------------------------------------------------------------
	// Draw Movement Map Grid.
	//		Once movement is split up, turn this back on for editor -fs
	// I need ALL cells drawn to check elevation Code
	if (clipped1 != 0)
	{
		//--------------------------------------------------------------------
		// Display the ScenarioMap cell grid, as well, displaying open\blocked
		// states...
		float cellwidth = Terrain::worldUnitsPerVertex / terrain_const::MAPCELL_DIM;
		// cellwidth -= 5.0;
		int32_t rowCol = vertices[0]->posTile;
		int32_t tileR = rowCol >> 16;
		int32_t tileC = rowCol & 0x0000ffff;
		if (GameMap)
		{
			for (size_t cellR = 0; cellR < terrain_const::MAPCELL_DIM; cellR++)
			{
				for (size_t cellC = 0; cellC < terrain_const::MAPCELL_DIM; cellC++)
				{
					int32_t actualCellRow = tileR * terrain_const::MAPCELL_DIM + cellR;
					int32_t actualCellCol = tileC * terrain_const::MAPCELL_DIM + cellC;
					MapCellPtr curCell = nullptr;
					if (GameMap->inBounds(actualCellRow, actualCellCol))
						curCell = GameMap->getCell(actualCellRow, actualCellCol);
					if (!curCell || curCell->getDebug() || !curCell->getPassable() || curCell->getPathlock(0) || curCell->getDeepWater() || curCell->getShallowWater() || curCell->getForest())
					{
						Stuff::Vector4D pos1;
						Stuff::Vector4D pos2;
						Stuff::Vector4D pos3;
						Stuff::Vector4D pos4;
						Stuff::Vector3D thePoint(
							vertices[0]->vx, vertices[0]->vy, vertices[0]->pVertex->elevation);
						thePoint.x += (cellC)*cellwidth;
						thePoint.y -= (cellR)*cellwidth;
						thePoint.z = land->getTerrainElevation(thePoint);
						eye->projectZ(thePoint, pos4);
						thePoint.x += cellwidth;
						thePoint.z = land->getTerrainElevation(thePoint);
						eye->projectZ(thePoint, pos1);
						thePoint.y -= cellwidth;
						thePoint.z = land->getTerrainElevation(thePoint);
						eye->projectZ(thePoint, pos2);
						thePoint.x -= cellwidth;
						thePoint.z = land->getTerrainElevation(thePoint);
						eye->projectZ(thePoint, pos3);
						pos1.z -= 0.002f;
						pos2.z -= 0.002f;
						pos3.z -= 0.002f;
						pos4.z -= 0.002f;
						uint32_t color = XP_RED;
						if (!curCell)
						{
							color = XP_GREEN;
						}
						else if (curCell->getDebug() && DrawDebugCells)
						{
							color = XP_YELLOW;
						}
						else if (curCell->getPathlock(0))
						{
							color = XP_YELLOW;
						}
						else if (curCell->getForest())
						{
							color = SB_ORANGE;
						}
						else if (!curCell->getPassable())
						{
							color = SB_RED;
						}
						else if (curCell->getDeepWater())
						{
							color = SB_ORANGE;
						}
						else if (curCell->getShallowWater())
						{
							color = XP_BLUE;
						}
						{
							LineElement newElement(pos1, pos2, color, nullptr);
							newElement.draw();
						}
						{
							LineElement newElement(pos2, pos3, color, nullptr);
							newElement.draw();
						}
						{
							LineElement newElement(pos3, pos4, color, nullptr);
							newElement.draw();
						}
						{
							LineElement newElement(pos1, pos4, color, nullptr);
							newElement.draw();
						}
					}
				}
			}
		}
	}
	if (GlobalMoveMap[0]->badLoad)
		return;
	if (clipped1 != 0)
	{
		float cellwidth = Terrain::worldUnitsPerVertex / terrain_const::MAPCELL_DIM;
		cellwidth -= 5.0;
		int32_t rowCol = vertices[0]->posTile;
		int32_t tileR = rowCol >> 16;
		int32_t tileC = rowCol & 0x0000ffff;
		int32_t cellR = tileR * terrain_const::MAPCELL_DIM;
		int32_t cellC = tileC * terrain_const::MAPCELL_DIM;
		for (size_t currentDoor = 0; currentDoor < GlobalMoveMap[0]->numDoors; currentDoor++)
		{
			if ((GlobalMoveMap[0]->doors[currentDoor].row >= cellR) && (GlobalMoveMap[0]->doors[currentDoor].row < (cellR + terrain_const::MAPCELL_DIM)) && (GlobalMoveMap[0]->doors[currentDoor].col >= cellC) && (GlobalMoveMap[0]->doors[currentDoor].col < (cellC + terrain_const::MAPCELL_DIM)))
			{
				Stuff::Vector4D pos1;
				Stuff::Vector4D pos2;
				Stuff::Vector4D pos3;
				Stuff::Vector4D pos4;
				int32_t xLength = 1;
				int32_t yLength = 1;
				if (GlobalMoveMap[0]->doors[currentDoor].direction[0] == 1)
				{
					yLength = GlobalMoveMap[0]->doors[currentDoor].length;
				}
				if (GlobalMoveMap[0]->doors[currentDoor].direction[0] == 2)
				{
					xLength = GlobalMoveMap[0]->doors[currentDoor].length;
				}
				Stuff::Vector3D thePoint(
					vertices[0]->vx, vertices[0]->vy, vertices[0]->pVertex->elevation);
				thePoint.x += (GlobalMoveMap[0]->doors[currentDoor].col - cellC) * cellwidth;
				thePoint.y -= (GlobalMoveMap[0]->doors[currentDoor].row - cellR) * cellwidth;
				thePoint.z = land->getTerrainElevation(thePoint);
				eye->projectZ(thePoint, pos4);
				thePoint.x += (xLength)*cellwidth;
				thePoint.z = land->getTerrainElevation(thePoint);
				eye->projectZ(thePoint, pos1);
				thePoint.y -= (yLength)*cellwidth;
				thePoint.z = land->getTerrainElevation(thePoint);
				eye->projectZ(thePoint, pos2);
				thePoint.x -= (xLength)*cellwidth;
				thePoint.z = land->getTerrainElevation(thePoint);
				eye->projectZ(thePoint, pos3);
				pos1.z -= 0.002f;
				pos2.z -= 0.002f;
				pos3.z -= 0.002f;
				pos4.z -= 0.002f;
				{
					LineElement newElement(pos1, pos2, XP_GREEN, nullptr);
					newElement.draw();
				}
				{
					LineElement newElement(pos2, pos3, XP_GREEN, nullptr);
					newElement.draw();
				}
				{
					LineElement newElement(pos3, pos4, XP_GREEN, nullptr);
					newElement.draw();
				}
				{
					LineElement newElement(pos1, pos4, XP_GREEN, nullptr);
					newElement.draw();
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void TerrainQuad::drawLOSLine(void)
{
	int32_t clipped1 = vertices[0]->clipInfo + vertices[1]->clipInfo + vertices[2]->clipInfo;
	int32_t clipped2 = vertices[0]->clipInfo + vertices[2]->clipInfo + vertices[3]->clipInfo;
	if (uvMode == BOTTOMLEFT)
	{
		clipped1 = vertices[0]->clipInfo + vertices[1]->clipInfo + vertices[3]->clipInfo;
		clipped2 = vertices[1]->clipInfo + vertices[2]->clipInfo + vertices[3]->clipInfo;
	}
	//------------------------------------------------------------
	// Draw the Tile block lines at depth just above base tiles.
	//------------------------------------------------------------
	// Draw LOS Map Grid.
	// Draw a color for LOS cell height data based on height.  Draw NOTHING if
	// cell height is ZERO!!
	if (clipped1 != 0)
	{
		//--------------------------------------------------------------------
		float cellwidth = Terrain::worldUnitsPerVertex / terrain_const::MAPCELL_DIM;
		int32_t rowCol = vertices[0]->posTile;
		int32_t tileR = rowCol >> 16;
		int32_t tileC = rowCol & 0x0000ffff;
		if (GameMap)
		{
			for (size_t cellR = 0; cellR < terrain_const::MAPCELL_DIM; cellR++)
			{
				for (size_t cellC = 0; cellC < terrain_const::MAPCELL_DIM; cellC++)
				{
					int32_t actualCellRow = tileR * terrain_const::MAPCELL_DIM + cellR;
					int32_t actualCellCol = tileC * terrain_const::MAPCELL_DIM + cellC;
					MapCellPtr curCell = nullptr;
					if (GameMap->inBounds(actualCellRow, actualCellCol))
						curCell = GameMap->getCell(actualCellRow, actualCellCol);
					if (curCell && curCell->getLocalheight())
					{
						Stuff::Vector4D pos1;
						Stuff::Vector4D pos2;
						Stuff::Vector4D pos3;
						Stuff::Vector4D pos4;
						Stuff::Vector3D thePoint(
							vertices[0]->vx, vertices[0]->vy, vertices[0]->pVertex->elevation);
						thePoint.x += (cellC)*cellwidth;
						thePoint.y -= (cellR)*cellwidth;
						thePoint.z = land->getTerrainElevation(thePoint);
						eye->projectZ(thePoint, pos4);
						thePoint.x += cellwidth;
						thePoint.z = land->getTerrainElevation(thePoint);
						eye->projectZ(thePoint, pos1);
						thePoint.y -= cellwidth;
						thePoint.z = land->getTerrainElevation(thePoint);
						eye->projectZ(thePoint, pos2);
						thePoint.x -= cellwidth;
						thePoint.z = land->getTerrainElevation(thePoint);
						eye->projectZ(thePoint, pos3);
						pos1.z -= 0.002f;
						pos2.z -= 0.002f;
						pos3.z -= 0.002f;
						pos4.z -= 0.002f;
						uint32_t color = XP_BLACK;
						if (curCell->getLocalheight() < 2)
						{
							color = XP_BLACK;
						}
						else if (curCell->getLocalheight() < 4)
						{
							color = XP_GRAY;
						}
						else if (curCell->getLocalheight() < 6)
						{
							color = XP_RED;
						}
						else if (curCell->getLocalheight() < 8)
						{
							color = XP_ORANGE;
						}
						else if (curCell->getLocalheight() < 10)
						{
							color = XP_YELLOW;
						}
						else if (curCell->getLocalheight() < 12)
						{
							color = XP_GREEN;
						}
						else if (curCell->getLocalheight() < 14)
						{
							color = XP_BLUE;
						}
						else if (curCell->getLocalheight() <= 16)
						{
							color = XP_WHITE;
						}
						{
							LineElement newElement(pos1, pos2, color, nullptr);
							newElement.draw();
						}
						{
							LineElement newElement(pos2, pos3, color, nullptr);
							newElement.draw();
						}
						{
							LineElement newElement(pos3, pos4, color, nullptr);
							newElement.draw();
						}
						{
							LineElement newElement(pos1, pos4, color, nullptr);
							newElement.draw();
						}
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
void TerrainQuad::drawDebugCellLine(void)
{
	int32_t clipped1 = vertices[0]->clipInfo + vertices[1]->clipInfo + vertices[2]->clipInfo;
	int32_t clipped2 = vertices[0]->clipInfo + vertices[2]->clipInfo + vertices[3]->clipInfo;
	if (uvMode == BOTTOMLEFT)
	{
		clipped1 = vertices[0]->clipInfo + vertices[1]->clipInfo + vertices[3]->clipInfo;
		clipped2 = vertices[1]->clipInfo + vertices[2]->clipInfo + vertices[3]->clipInfo;
	}
	//------------------------------------------------------------
	// Draw the Tile block lines at depth just above base tiles.
	if (uvMode == BOTTOMRIGHT)
	{
		if (clipped1 != 0)
		{
			Stuff::Vector4D pos1(
				vertices[0]->px, vertices[0]->py, HUD_DEPTH, 1.0f / vertices[0]->pw);
			Stuff::Vector4D pos2(
				vertices[1]->px, vertices[1]->py, HUD_DEPTH, 1.0f / vertices[1]->pw);
			{
				// LineElement newElement(pos1,pos2,color,nullptr);
				// newElement.draw();
			}
			pos1.x = vertices[1]->px;
			pos1.y = vertices[1]->py;
			pos2.x = vertices[2]->px;
			pos2.y = vertices[2]->py;
			{
				// LineElement newElement(pos1,pos2,color,nullptr);
				// newElement.draw();
			}
			pos1.x = vertices[2]->px;
			pos1.y = vertices[2]->py;
			pos2.x = vertices[0]->px;
			pos2.y = vertices[0]->py;
			{
				// LineElement newElement(pos1,pos2,color,nullptr);
				// newElement.draw();
			}
		}
		if (clipped2 != 0)
		{
			Stuff::Vector4D pos1(
				vertices[0]->px, vertices[0]->py, HUD_DEPTH, 1.0f / vertices[0]->pw);
			Stuff::Vector4D pos2(
				vertices[2]->px, vertices[2]->py, HUD_DEPTH, 1.0f / vertices[2]->pw);
			{
				// LineElement newElement(pos1,pos2,color,nullptr);
				// newElement.draw();
			}
			pos1.x = vertices[2]->px;
			pos1.y = vertices[2]->py;
			pos2.x = vertices[3]->px;
			pos2.y = vertices[3]->py;
			{
				// LineElement newElement(pos1,pos2,color,nullptr);
				// newElement.draw();
			}
			pos1.x = vertices[3]->px;
			pos1.y = vertices[3]->py;
			pos2.x = vertices[0]->px;
			pos2.y = vertices[0]->py;
			{
				// LineElement newElement(pos1,pos2,color,nullptr);
				// newElement.draw();
			}
		}
	}
	else
	{
		if (clipped1 != 0)
		{
			Stuff::Vector4D pos1(
				vertices[0]->px, vertices[0]->py, HUD_DEPTH, 1.0f / vertices[0]->pw);
			Stuff::Vector4D pos2(
				vertices[1]->px, vertices[1]->py, HUD_DEPTH, 1.0f / vertices[1]->pw);
			{
				// LineElement newElement(pos1,pos2,color,nullptr);
				// newElement.draw();
			}
			pos1.x = vertices[1]->px;
			pos1.y = vertices[1]->py;
			pos2.x = vertices[3]->px;
			pos2.y = vertices[3]->py;
			{
				// LineElement newElement(pos1,pos2,color,nullptr);
				// newElement.draw();
			}
			pos1.x = vertices[3]->px;
			pos1.y = vertices[3]->py;
			pos2.x = vertices[0]->px;
			pos2.y = vertices[0]->py;
			{
				// LineElement newElement(pos1,pos2,color,nullptr);
				// newElement.draw();
			}
		}
		if (clipped2 != 0)
		{
			Stuff::Vector4D pos1(
				vertices[1]->px, vertices[1]->py, HUD_DEPTH, 1.0f / vertices[1]->pw);
			Stuff::Vector4D pos2(
				vertices[2]->px, vertices[2]->py, HUD_DEPTH, 1.0f / vertices[2]->pw);
			{
				// LineElement newElement(pos1,pos2,color,nullptr);
				// newElement.draw();
			}
			pos1.x = vertices[2]->px;
			pos1.y = vertices[2]->py;
			pos2.x = vertices[3]->px;
			pos2.y = vertices[3]->py;
			{
				// LineElement newElement(pos1,pos2,color,nullptr);
				// newElement.draw();
			}
			pos1.x = vertices[3]->px;
			pos1.y = vertices[3]->py;
			pos2.x = vertices[1]->px;
			pos2.y = vertices[1]->py;
			{
				// LineElement newElement(pos1,pos2,color,nullptr);
				// newElement.draw();
			}
		}
	}
	//------------------------------------------------------------
	// Draw Movement Map Grid.
	//		Once movement is split up, turn this back on for editor -fs
	// I need ALL cells drawn to check elevation Code
	if (clipped1 != 0)
	{
		//--------------------------------------------------------------------
		// Display the ScenarioMap cell grid, as well, displaying open\blocked
		// states...
		float cellwidth = Terrain::worldUnitsPerVertex / terrain_const::MAPCELL_DIM;
		// cellwidth -= 5.0;
		int32_t rowCol = vertices[0]->posTile;
		int32_t tileR = rowCol >> 16;
		int32_t tileC = rowCol & 0x0000ffff;
		if (GameMap)
		{
			for (size_t cellR = 0; cellR < terrain_const::MAPCELL_DIM; cellR++)
			{
				for (size_t cellC = 0; cellC < terrain_const::MAPCELL_DIM; cellC++)
				{
					int32_t actualCellRow = tileR * terrain_const::MAPCELL_DIM + cellR;
					int32_t actualCellCol = tileC * terrain_const::MAPCELL_DIM + cellC;
					MapCellPtr curCell = nullptr;
					if (GameMap->inBounds(actualCellRow, actualCellCol))
						curCell = GameMap->getCell(actualCellRow, actualCellCol);
					if (!curCell || curCell->getDebug())
					{
						Stuff::Vector4D pos1;
						Stuff::Vector4D pos2;
						Stuff::Vector4D pos3;
						Stuff::Vector4D pos4;
						Stuff::Vector3D thePoint(
							vertices[0]->vx, vertices[0]->vy, vertices[0]->pVertex->elevation);
						thePoint.x += (cellC)*cellwidth;
						thePoint.y -= (cellR)*cellwidth;
						thePoint.z = land->getTerrainElevation(thePoint);
						eye->projectZ(thePoint, pos4);
						thePoint.x += cellwidth;
						thePoint.z = land->getTerrainElevation(thePoint);
						eye->projectZ(thePoint, pos1);
						thePoint.y -= cellwidth;
						thePoint.z = land->getTerrainElevation(thePoint);
						eye->projectZ(thePoint, pos2);
						thePoint.x -= cellwidth;
						thePoint.z = land->getTerrainElevation(thePoint);
						eye->projectZ(thePoint, pos3);
						pos1.z = pos2.z = pos3.z = pos4.z = HUD_DEPTH;
						uint32_t color = XP_RED;
						if (!curCell)
						{
							color = XP_GREEN;
						}
						else
						{
							static uint32_t debugcolours[4] = {0, XP_RED, XP_WHITE, XP_BLUE};
							uint32_t cellDebugValue = curCell->getDebug();
							if (cellDebugValue)
								color = debugcolours[cellDebugValue];
						}
						/*						else if (curCell->getPathlock())
												{
														color = XP_YELLOW;
												}
												else if
						   (!curCell->getLineOfSight())
												{
													color = XP_BLUE;
												}
												else if
						   (!curCell->getPassable())
												{
													color = SB_RED;
												}
						*/
						{
							LineElement newElement(pos1, pos2, color, nullptr);
							newElement.draw();
						}
						{
							LineElement newElement(pos2, pos3, color, nullptr);
							newElement.draw();
						}
						{
							LineElement newElement(pos3, pos4, color, nullptr);
							newElement.draw();
						}
						{
							LineElement newElement(pos1, pos4, color, nullptr);
							newElement.draw();
						}
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
void TerrainQuad::drawMine(void)
{
	int32_t clipped1 = vertices[0]->clipInfo + vertices[1]->clipInfo + vertices[2]->clipInfo;
	int32_t clipped2 = vertices[0]->clipInfo + vertices[2]->clipInfo + vertices[3]->clipInfo;
	if (uvMode == BOTTOMLEFT)
	{
		clipped1 = vertices[0]->clipInfo + vertices[1]->clipInfo + vertices[3]->clipInfo;
		clipped2 = vertices[1]->clipInfo + vertices[2]->clipInfo + vertices[3]->clipInfo;
	}
	//------------------------------------------------------------
	// Draw Mines.
	// All mines are visible all the time!
	if ((clipped1 != 0) || (clipped2 != 0))
	{
		int32_t cellPos = 0;
		float cellwidth = Terrain::worldUnitsPerCell;
		for (size_t cellR = 0; cellR < terrain_const::MAPCELL_DIM; cellR++)
		{
			for (size_t cellC = 0; cellC < terrain_const::MAPCELL_DIM; cellC++, cellPos++)
			{
				//--------------------------------------------------------------------
				bool drawMine = false;
				bool drawBlownMine = false;
				if (mineResult.getMine(cellPos) == 1)
					drawMine = true;
				if (mineResult.getMine(cellPos) == 2)
					drawBlownMine = true;
				if (drawMine || drawBlownMine)
				{
					Stuff::Vector4D pos1;
					Stuff::Vector4D pos2;
					Stuff::Vector4D pos3;
					Stuff::Vector4D pos4;
					//------------------------------------------------------------------------------------
					// Dig the actual Vertex information out of the projected
					// vertices already done. In this way, the draw requires
					// only interpolation and not Giant Matrix multiplies.
					Stuff::Vector3D thePoint(
						vertices[0]->vx, vertices[0]->vy, vertices[0]->pVertex->elevation);
					thePoint.x += (cellC)*cellwidth;
					thePoint.y -= (cellR)*cellwidth;
					thePoint.z = land->getTerrainElevation(thePoint);
					eye->projectZ(thePoint, pos4);
					thePoint.x += cellwidth;
					thePoint.z = land->getTerrainElevation(thePoint);
					eye->projectZ(thePoint, pos1);
					thePoint.y -= cellwidth;
					thePoint.z = land->getTerrainElevation(thePoint);
					eye->projectZ(thePoint, pos2);
					thePoint.x -= cellwidth;
					thePoint.z = land->getTerrainElevation(thePoint);
					eye->projectZ(thePoint, pos3);
					//------------------------------------
					// Replace with New RIA code
					gos_VERTEX gVertex[3];
					gos_VERTEX sVertex[3];
					gVertex[0].x = sVertex[0].x = pos1.x;
					gVertex[0].y = sVertex[0].y = pos1.y;
					gVertex[0].z = sVertex[0].z = pos1.z;
					gVertex[0].rhw = sVertex[0].rhw = pos1.w;
					gVertex[0].u = sVertex[0].u = 0.0f;
					gVertex[0].v = sVertex[0].v = 0.0f;
					gVertex[0].argb = sVertex[0].argb = vertices[0]->lightRGB;
					gVertex[0].frgb = sVertex[0].frgb = vertices[0]->fogRGB;
					gVertex[1].x = pos2.x;
					gVertex[1].y = pos2.y;
					gVertex[1].z = pos2.z;
					gVertex[1].rhw = pos2.w;
					gVertex[1].u = 0.999999999f;
					gVertex[1].v = 0.0f;
					gVertex[1].argb = vertices[1]->lightRGB;
					gVertex[1].frgb = vertices[1]->fogRGB;
					gVertex[2].x = sVertex[1].x = pos3.x;
					gVertex[2].y = sVertex[1].y = pos3.y;
					gVertex[2].z = sVertex[1].z = pos3.z;
					gVertex[2].rhw = sVertex[1].rhw = pos3.w;
					gVertex[2].u = sVertex[1].u = 0.999999999f;
					gVertex[2].v = sVertex[1].v = 0.999999999f;
					gVertex[2].argb = sVertex[1].argb = vertices[2]->lightRGB;
					gVertex[2].frgb = sVertex[1].frgb = vertices[2]->fogRGB;
					sVertex[2].x = pos4.x;
					sVertex[2].y = pos4.y;
					sVertex[2].z = pos4.z;
					sVertex[2].rhw = pos4.w;
					sVertex[2].u = 0.0f;
					sVertex[2].v = 0.999999999f;
					sVertex[2].argb = vertices[3]->lightRGB;
					sVertex[2].frgb = vertices[3]->fogRGB;
					if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) && (gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f) && (sVertex[0].z >= 0.0f) && (sVertex[0].z < 1.0f) && (sVertex[1].z >= 0.0f) && (sVertex[1].z < 1.0f) && (sVertex[2].z >= 0.0f) && (sVertex[2].z < 1.0f))
					{
						if (drawBlownMine)
						{
							mcTextureManager->addVertices(
								blownTextureHandle, gVertex, MC2_DRAWALPHA);
							mcTextureManager->addVertices(
								blownTextureHandle, sVertex, MC2_DRAWALPHA);
						}
						else
						{
							mcTextureManager->addVertices(
								mineTextureHandle, gVertex, MC2_DRAWALPHA);
							mcTextureManager->addVertices(
								mineTextureHandle, sVertex, MC2_DRAWALPHA);
						}
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
