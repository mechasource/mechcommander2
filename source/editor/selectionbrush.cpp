#define SELECTIONBRUSH_CPP
/*************************************************************************************************\
SelectionBrush.cpp			: Implementation of the SelectionBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "SelectionBrush.h"

#ifndef CAMERA_H
#include "Camera.h"
#endif

#ifndef EDITOROBJECTMGR_H
#include "EditorObjectMgr.h"
#endif

#ifndef ACTION_H
#include "Action.h"
#endif

#include "utilities.h"

#include "EditorMessages.h"
#include "EditorInterface.h"


SelectionBrush::SelectionBrush( bool Area, int newRadius )
{ 
	bPainting = false; 
	bArea = Area; 
	bDrag = false; 
	pCurAction = NULL; 
	pCurModifyBuildingAction = NULL;

	lastPos.x = lastPos.y = lastPos.z = lastPos.w = 0.0f;		//Keep the FPU exception from going off!
	smoothRadius = newRadius;
	bFirstClick = false;

	pDragBuilding = NULL;
}

SelectionBrush::~SelectionBrush()
{
	if ( EditorObjectMgr::instance() )
			EditorObjectMgr::instance()->unselectAll();
	if ( land )
		land->unselectAll();
}
bool SelectionBrush::beginPaint()
{
	lastPos.x = lastPos.y = 0.0;
	firstWorldPos.x = lastWorldPos.x = 0.f;
	lastWorldPos.y = firstWorldPos.y = 0.f;

	bPainting = true;
	bFirstClick = !bFirstClick;
	return true;
}
Action* SelectionBrush::endPaint()
{
	bPainting = false;
	Action* pRetAction = NULL;
	if ( pCurAction )
	{
		if ( pCurAction->vertexInfoList.Count() )
		{
			pRetAction = pCurAction;
			pCurAction = NULL;
			land->recalcWater();
//			land->reCalcLight();	
		}

		else
		{
			delete pCurAction;
			pCurAction = NULL;
		}
	}
	else if ( pCurModifyBuildingAction )
	{
		if ( pCurModifyBuildingAction->isNotNull() )
		{
			/*we have to call updateNotedObjectPositions because we use the object's position
			to identify which building to apply the "undo" action to, and the object might have been
			moved since we last noted the object's info*/
			pCurModifyBuildingAction->updateNotedObjectPositions();
			pRetAction = pCurModifyBuildingAction;
			pCurModifyBuildingAction = NULL;
		}

		else
		{
			delete pCurModifyBuildingAction;
			pCurModifyBuildingAction = NULL;
		}
	}

	if ( lastWorldPos != firstWorldPos )
	{
		Stuff::Vector4D lastPos;
		Stuff::Vector4D endPos;
		eye->projectZ( firstWorldPos, endPos );
		eye->projectZ( lastWorldPos, lastPos );
		EditorObjectMgr::instance()->select( lastPos, endPos );
		land->selectVerticesInRect( lastPos, endPos,  (GetAsyncKeyState( VK_CONTROL )) );
	}

	if (EditorInterface::instance()->ObjectSelectOnlyMode())
	{
		ReleaseCapture();
		//EditorData::instance->DoTeamDialog(EditorInterface::instance()->objectivesEditState.alignment);
		EditorInterface::instance()->Team(EditorInterface::instance()->objectivesEditState.alignment);
		//EditorInterface::instance()->Objectives();
	}
	
	return pRetAction;
}


float SelectionBrush::calcNewHeight( int vertexRow, int vertexCol, float deltaScreen )
{
	Stuff::Vector3D world;
	Stuff::Vector3D newWorld;
	Stuff::Vector4D screenVertex;
	Stuff::Vector4D screenNewVertex;


	world.y = newWorld.y = land->tileRowToWorldCoord[vertexRow];
	world.x = newWorld.x = land->tileColToWorldCoord[vertexCol];
	world.z = land->getTerrainElevation( vertexRow, vertexCol );	
	newWorld.z = world.z + 1000.0f;

	eye->projectZ( world, screenVertex );
	eye->projectZ( newWorld, screenNewVertex );

	float ratio = 1000.0f/(screenNewVertex.y - screenVertex.y);
	
	return (ratio * deltaScreen);
	
}

bool SelectionBrush::paint( Stuff::Vector3D& worldPos, int screenX, int screenY )
{
	Stuff::Vector4D endPos;
	endPos.x = (float)screenX;
	endPos.y = (float)screenY;
	
	if ( pDragBuilding )
	{
		const EditorObject* pObject = pDragBuilding;
		if ( pObject )
		{
			if ( !pCurModifyBuildingAction )
				pCurModifyBuildingAction = new ModifyBuildingAction();

			long newCellI, newCellJ;
			land->worldToCell( worldPos, newCellJ, newCellI );

			//if ( lastRow  != 0 && lastCol != 0 )
			if ( true )
			{
				pCurModifyBuildingAction->addBuildingInfo(*(const_cast<EditorObject*>(pObject)));
				EditorObjectMgr::instance()->moveBuilding( const_cast<EditorObject*>(pObject),
					newCellJ, newCellI );
			}
			lastRow = newCellI;
			lastCol = newCellJ;
		}
		return true;
	}
	
	else if ( bDrag ) // if we are dragging vertex heights, do this
	{
		if ( endPos != lastPos )
		{
			if ( !pCurAction )
				pCurAction = new ActionPaintTile();

			if ( smoothRadius != -1 )
			{
				return paintSmooth( worldPos, screenX, screenY, smoothRadius );
			}

			if ( lastPos.x  != 0.0 && lastPos.y != 0.0 )
			{
				//return paintSmooth( worldPos, screenX, screenY, 6 );
				float delta = calcNewHeight( lastRow, lastCol, endPos.y - lastPos.y );
				for ( int j = 0; j < land->realVerticesMapSide; ++j )
				{
					for ( int i = 0; i < land->realVerticesMapSide; ++i )
					{
						if ( land->isVertexSelected( j, i ) )
						{
							pCurAction->addChangedVertexInfo( j, i );
							float oldHeight = land->getVertexHeight( j * land->realVerticesMapSide + i);
							land->setVertexHeight( j * land->realVerticesMapSide + i, oldHeight + delta );
						}
					}
				}
			}
		}	

		lastPos = endPos;
		return true;
	}
	
	else //if ( bFirstClick ) // otherwise, do a new area select
	{
		long bShift = GetAsyncKeyState( VK_SHIFT );
		long bCtrl = GetAsyncKeyState( VK_CONTROL );
	
		Stuff::Vector2DOf<long> screenPos;
		screenPos.x = screenX;
		screenPos.y = screenY;

		// select the objects
		if ( lastPos.x != 0.0 && lastPos.y != 0.0 )
		{
			if ( !bShift && !bCtrl )
			{
				land->unselectAll();
				EditorObjectMgr::instance()->unselectAll();
			}
			
			eye->inverseProject( screenPos, lastWorldPos );
		
		}
		else
		{
			if ( lastPos != endPos )
			{
				if ( !bShift && !bCtrl )
				{
					land->unselectAll();
					EditorObjectMgr::instance()->unselectAll();			
				}
			}
			lastPos = endPos;
	
			if ( firstWorldPos.x == 0.f && firstWorldPos.y == 0.f )
				eye->inverseProject( screenPos, firstWorldPos );

			eye->inverseProject( screenPos, lastWorldPos );

			const EditorObject* pInfo = EditorObjectMgr::instance()->getObjectAtPosition( worldPos );
			if ( pInfo )
			{
				if ( !bCtrl || (bCtrl && pInfo->isSelected() == false ) )
					(const_cast<EditorObject*>(pInfo))->select( true );
				else
					(const_cast<EditorObject*>(pInfo))->select( false );
			}
			else
			{
				long tileR, tileC;
				land->worldToTile( worldPos, tileR, tileC );
				if ( tileR > -1 && tileR < land->realVerticesMapSide
					&& tileC > -1 && tileC < land->realVerticesMapSide )
				{
					// figure out which vertex is closest
					if ( fabs(worldPos.x - land->tileColToWorldCoord[tileC]) >= land->worldUnitsPerVertex/2 )
						tileC++;

					if ( fabs(worldPos.y - land->tileRowToWorldCoord[tileR]) >= land->worldUnitsPerVertex/2 )
						tileR++;

					if (!bCtrl || (bCtrl && !land->isVertexSelected( tileR, tileC ) ) )
						land->selectVertex( tileR, tileC );

					else // shift key, object is selected
						land->selectVertex( tileR, tileC, false );
				}
			}
		}
	}


	return true;
}

void SelectionBrush::render( int screenX, int screenY )
{
	
	if ( bPainting && !bDrag && !pDragBuilding)
	{
		//------------------------------------------
		Stuff::Vector4D Screen;
		eye->projectZ( firstWorldPos, Screen );
		
		GUI_RECT rect = { screenX, screenY, (long)Screen.x, (long)Screen.y };
		drawRect( rect, 0x30ffffff );
		drawEmptyRect( rect, 0xff000000, 0xff000000 );
	}


	else if ( lastPos.x != 0.0 && lastPos.y != 0.0 && 
		!GetAsyncKeyState( KEY_LSHIFT ) && !GetAsyncKeyState( KEY_LCONTROL ) )
	{
		
		if ( EditorObjectMgr::instance()->getSelectionCount() == 1 )
		{
			// only drag move one building at a time
			Stuff::Vector2DOf<long> screenPos;
			screenPos.x = screenX;
			screenPos.y = screenY;

			Stuff::Vector3D worldPos;

			eye->inverseProject( screenPos, worldPos );

			const EditorObject* pObject = 
				EditorObjectMgr::instance()->getObjectAtPosition( worldPos );

			if ( pObject && pObject->isSelected() )
			{
				EditorInterface::instance()->ChangeCursor( IDC_HAND );
				pDragBuilding = (EditorObject*)pObject;
				pObject->getCells( (long&)lastRow, (long&)lastCol );
				lastPos.x = (float)screenX;
				lastPos.y = (float)screenY;
				return;
			}

		}
		
		// not movinve a building, figure out if we are moving a vertex
		Stuff::Vector3D world;
		Stuff::Vector4D screen;

		if ( 	!GetAsyncKeyState( KEY_LSHIFT ) && !GetAsyncKeyState( KEY_LCONTROL ) )
		{
			// figure out if there is a selected vertex near here
			for ( int i = 0; i < land->realVerticesMapSide; ++i )
			{
				for ( int j = 0; j < land->realVerticesMapSide; ++j )
				{
					if ( land->isVertexSelected( j, i ) )
					{
						world.y = land->tileRowToWorldCoord[j];
						world.x = land->tileColToWorldCoord[i];
						world.z = land->getTerrainElevation( j, i );


						eye->projectZ( world, screen );
						if ( (fabs(screen.x - screenX) < 20 && fabs(screen.y - screenY) < 20) )
						{
							EditorInterface::instance()->ChangeCursor( smoothRadius == -1 ? IDC_UP : IDC_HILLS );
							bDrag = true;
							lastRow = j;
							lastCol = i;
							return;
						}
					}
				}
			}
		}
	}

	if ( !bPainting )
	{
		EditorInterface::instance()->ChangeCursor( IDC_MC2ARROW );
		bDrag = false;
		pDragBuilding = NULL;
	}
}


bool SelectionBrush::paintSmooth( Stuff::Vector3D& worldPos, int screenX, int screenY, int radius )
{
	int minI = INT_MAX;
	int maxI = 0;
	int minJ = INT_MAX;
	int maxJ = 0;
	for ( int j = 0; j < land->realVerticesMapSide; ++j )
	{
		for ( int i = 0; i < land->realVerticesMapSide; ++i )
		{
			if ( land->isVertexSelected( j, i ) )
			{
				if ( i > maxI )
					maxI = i;
				if ( i < minI )
					minI = i;
				if ( j > maxJ )
					maxJ = j;
				if ( j < minJ )
					minJ = j;
			}
		}
	}

	if ( maxI == minI && maxJ == minJ )
	{
		return paintSmoothArea( worldPos, screenX, screenY, (float)radius, (float)radius, minJ, minI );
	}
	else 
		return paintSmoothArea( worldPos, screenX, screenY, float((maxJ - minJ + 1)>>1), float((maxI - minI + 1)>>1),
							minJ + ((maxJ - minJ)/2), minI + ((maxI - minI)/2) );

	return false;
}

bool   SelectionBrush::paintSmoothArea( Stuff::Vector3D& worldPos, int screenX, int screenY, float radY, float radX,
									   int j, int i)
{
	Stuff::Vector4D endPos;
	endPos.x = (float)screenX;
	endPos.y = (float)screenY;
	endPos.z = endPos.w = 0.0f;

	float radiusX = radX;
	float radiusY = radY;

	if ( endPos != lastPos )
	{
		if ( !pCurAction )
			pCurAction = new ActionPaintTile();

		if ( lastPos.x  != 0.0 && lastPos.y != 0.0 )
		{
			float delta = calcNewHeight( lastRow, lastCol, endPos.y - lastPos.y );
			pCurAction->addChangedVertexInfo( j, i );
			float oldHeight = land->getVertexHeight( j * land->realVerticesMapSide + i);
			float newHeight = oldHeight + delta;
			land->setVertexHeight( j * land->realVerticesMapSide + i, newHeight );

			float midX = land->tileColToWorldCoord[i];
			float midY = land->tileRowToWorldCoord[j];

			if ( radiusX == 0 )
				radiusX = .5;
			if ( radiusY == 0 )
				radiusY = .5;
			
			float a = radiusX * radiusX * land->worldUnitsPerVertex * land->worldUnitsPerVertex;
			float b = radiusY * radiusY * land->worldUnitsPerVertex * land->worldUnitsPerVertex;
	
			// now set surrounding vertices within radius
			for ( int k = j - (long)radiusY; k < j + (long)radiusY + 1; ++k )
			{
				if ( k > -1 && k < land->realVerticesMapSide )
				{
					for ( int l = i - (long)radiusX; l < i + (long)radiusX + 1; ++l )
					{
						if ( l > -1 && l < land->realVerticesMapSide )
						{
							if ( radX == radY || ( land->isVertexSelected( k, l ) ) )
							{

								// make sure vertex is within radius
								float deltaY = ( (k - j) ) * -land->worldUnitsPerVertex;
								float deltaX = ( (l - i) ) * land->worldUnitsPerVertex;

								if ( (deltaX * deltaX)/a + (deltaY * deltaY)/b <= 1 )
								{
									Stuff::Vector3D edge;

									if ( deltaX == 0 )
									{
										edge.x = 0.f;
										edge.y = deltaY < 0 ? -radY * land->worldUnitsPerVertex : 
										radY * land->worldUnitsPerVertex;
									}
									else if ( deltaY == 0 )
									{
										edge.y = 0.f;
										edge.x = deltaX < 0 ? -radX * land->worldUnitsPerVertex : 
										radX * land->worldUnitsPerVertex;

									}
									else
									{
	//									if ( fabs( deltaX ) > .1 )
	//									{
	//										theta = atan( fabs(deltaY)/fabs(deltaX) );
	//									}

										float tangent = deltaY/deltaX;
										
										float tmp = 1/a + tangent * tangent/b;
										
									
										edge.x = (float)sqrt( 1/tmp );

										if ( deltaX < 0 && edge.x > 0 )
										{
											edge.x = -edge.x;
										}
										
										edge.y = edge.x * tangent;

										if ( deltaY < 0 && edge.y > 0 || deltaY > 0 && edge.y < 0 )
										{
											edge.y = -edge.y;
										}
									}
									
															
									float r = (float)sqrt( edge.x * edge.x + edge.y * edge.y );
									float delta = (float)sqrt( deltaX * deltaX + deltaY * deltaY );
									
									
									edge.x += midX;
									edge.y += midY;

									edge.z = land->getTerrainElevation( edge );							
									
									
									float deltaZ = newHeight - edge.z;

									if ( deltaZ > .1 || deltaZ < -.1 )
									{					
										float z = (float)fabs(deltaZ/2) + (float)fabs(deltaZ/2) * (float)cos( PI * delta/r );
										if ( fabs( z ) > .1f )
										{
											if ( deltaZ <= 0 )
												z = -z;
											pCurAction->addChangedVertexInfo( k, l );
											z += edge.z;
											
											land->setVertexHeight( k * land->realVerticesMapSide + l, z );
										}
									}
								}
							}
						}
					}
				}
			}
			
			lastPos = endPos;
			return true;

		}
	}	

	lastPos = endPos;
	return true;
}


//*************************************************************************************************
// end of file ( SelectionBrush.cpp )
 