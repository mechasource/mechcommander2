//******************************************************************************
//	collsn.cpp - This file contains the Collision Detection System
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"

// Include Files
// #include "mclib.h"

#ifndef COLLSN_H
#include "collsn.h"
#endif

#ifndef GAMEOBJ_H
#include "gameobj.h"
#endif

#ifndef OBJTYPE_H
#include "objtype.h"
#endif

#ifndef OBJMGR_H
#include "objmgr.h"
#endif

#ifndef MOVE_H
#include "move.h"
#endif

//------------------------------------------------------------------------------
// Static globals
// CollisionSystem *collisionSystem = nullptr;
GlobalCollisionAlert* globalCollisionAlert = nullptr;
UserHeapPtr CollisionSystem::collisionHeap = nullptr;
uint32_t CollisionSystem::maxObjects = 0;
uint32_t CollisionSystem::gridRadius = 0;
uint32_t CollisionSystem::xGridSize = 0;
uint32_t CollisionSystem::yGridSize = 0;
uint32_t CollisionSystem::maxCollisions = 0;

//------------------------------------------------------------------------------
// class CollisionGrid
//------------------------------------------------------------------------------
int32_t
GlobalCollisionAlert::init(uint32_t maxCollisionAlerts)
{
	maxAlerts = maxCollisionAlerts;
	collisionAlerts =
		(CollisionAlertRecordPtr)systemHeap->Malloc(sizeof(CollisionAlertRecord) * maxAlerts);
	gosASSERT(collisionAlerts != nullptr);
	purgeRecords();
	return (NO_ERROR);
}

//------------------------------------------------------------------------------
void GlobalCollisionAlert::destroy(void)
{
	systemHeap->Free(collisionAlerts);
	collisionAlerts = nullptr;
	maxAlerts = nextRecord = 0;
}

//------------------------------------------------------------------------------
int32_t
GlobalCollisionAlert::addRecord(
	GameObjectPtr obj1, GameObjectPtr obj2, float distance, float time)
{
	if (nextRecord < maxAlerts)
	{
		collisionAlerts[nextRecord].object1 = obj1;
		collisionAlerts[nextRecord].object2 = obj2;
		collisionAlerts[nextRecord].currentDistance = distance;
		collisionAlerts[nextRecord].timeToImpact = time;
		nextRecord++;
		return (NO_ERROR);
	}
	return (-1);
}

//------------------------------------------------------------------------------
CollisionAlertRecordPtr
GlobalCollisionAlert::findAlert(
	GameObjectPtr object, CollisionAlertRecordPtr startRecord)
{
	uint32_t startIndex = 0;
	if (startRecord)
	{
		for (size_t i = startIndex; i < (int32_t)nextRecord; i++)
		{
			if (&(collisionAlerts[i]) == startRecord)
			{
				break;
			}
		}
		startIndex = i + 1;
	}
	if (startIndex < nextRecord)
	{
		for (size_t i = startIndex; i < (int32_t)nextRecord; i++)
		{
			if (((collisionAlerts[i].object1) == object) || ((collisionAlerts[i].object2) == object))
			{
				return (&(collisionAlerts[i]));
			}
		}
	}
	return (nullptr);
}

//------------------------------------------------------------------------------
void GlobalCollisionAlert::purgeRecords(void)
{
	nextRecord = 0;
	for (size_t i = 0; i < (int32_t)maxAlerts; i++)
	{
		collisionAlerts[i].object1 = nullptr;
		collisionAlerts[i].object2 = nullptr;
		collisionAlerts[i].currentDistance = 0.0;
		collisionAlerts[i].timeToImpact = 0.0;
	}
}

//------------------------------------------------------------------------------
// class CollisionGrid
PVOID
CollisionGrid::operator new(size_t mySize)
{
	PVOID result = nullptr;
	if (CollisionSystem::collisionHeap && CollisionSystem::collisionHeap->heapReady())
		result = CollisionSystem::collisionHeap->Malloc(mySize);
	return (result);
}

//------------------------------------------------------------------------------
void CollisionGrid::operator delete(PVOID us)
{
	if (CollisionSystem::collisionHeap && CollisionSystem::collisionHeap->heapReady())
		CollisionSystem::collisionHeap->Free(us);
}

//------------------------------------------------------------------------------
int32_t
CollisionGrid::init(Stuff::Vector3D& newOrigin)
{
	//--------------------------------------------------
	// Check if stuff is allocated.  Once this happens
	// it is silly to call destroy each time.  We can
	// simply reinit and check if allocs already go.
	if (!gridIsGo)
	{
		xGridwidth = CollisionSystem::xGridSize;
		yGridwidth = CollisionSystem::yGridSize;
		maxGridRadius = CollisionSystem::gridRadius;
		maxObjects = CollisionSystem::maxObjects;
		gridSize = sizeof(CollisionGridNodePtr) * xGridwidth * yGridwidth;
		nodeSize = sizeof(CollisionGridNode) * maxObjects;
		if (CollisionSystem::collisionHeap && CollisionSystem::collisionHeap->heapReady())
			grid = (CollisionGridNodePtr*)CollisionSystem::collisionHeap->Malloc(gridSize);
		gosASSERT(grid != nullptr);
		if (CollisionSystem::collisionHeap && CollisionSystem::collisionHeap->heapReady())
			nodes = (CollisionGridNodePtr)CollisionSystem::collisionHeap->Malloc(nodeSize);
		gosASSERT(nodes != nullptr);
		gridXOffset = ((xGridwidth + 1) * maxGridRadius) / 2;
		gridYOffset = ((yGridwidth + 1) * maxGridRadius) / 2;
		gridXCheck = xGridwidth * maxGridRadius;
		gridYCheck = yGridwidth * maxGridRadius;
		gridIsGo = TRUE;
	}
	//-----------------------------
	// Reset all of the grid data.
	memset(grid, 0, gridSize);
	memset(nodes, 0, nodeSize);
	nextAvailableNode = 0;
	gridOrigin = newOrigin;
	giantObjects = nullptr;
	return (NO_ERROR);
}

//------------------------------------------------------------------------------
void CollisionGrid::destroy(void)
{
	if (gridIsGo)
	{
		if (CollisionSystem::collisionHeap && CollisionSystem::collisionHeap->heapReady())
		{
			CollisionSystem::collisionHeap->Free(nodes);
			nodes = nullptr;
			CollisionSystem::collisionHeap->Free(grid);
			grid = nullptr;
		}
		gridIsGo = FALSE;
		init();
	}
}

//------------------------------------------------------------------------------
int32_t
CollisionGrid::add(uint32_t gridIndex, GameObjectPtr object)
{
	gosASSERT(nextAvailableNode < maxObjects);
	gosASSERT((gridIndex >= 0) && (gridIndex < (xGridwidth * yGridwidth)));
	CollisionGridNodePtr prev = grid[gridIndex];
	CollisionGridNodePtr node = &nodes[nextAvailableNode++];
	grid[gridIndex] = node;
	node->object = object;
	node->next = prev;
	return (NO_ERROR);
}

//------------------------------------------------------------------------------
int32_t
CollisionGrid::add(GameObjectPtr object)
{
	if (object->getTangible()) // Can anything even hit me?
	{
		gosASSERT(nextAvailableNode < maxObjects);
		float objectRadius = object->getExtentRadius();
		//---------------------------------------------
		// Check if we are a giant Object
		if (objectRadius > maxGridRadius)
		{
			CollisionGridNodePtr node = &nodes[nextAvailableNode++];
			node->object = object;
			node->next = giantObjects;
			giantObjects = node;
			return NO_ERROR;
		}
		float gx, gy;
		gx = object->getPosition().x; // - gridOrigin.x;
		gx += gridXOffset;
		if (gx < 0)
			gx = 0;
		if (gx >= gridXCheck)
			gx = gridXCheck - 1;
		gx /= maxGridRadius;
		gy = object->getPosition().y; // - gridOrigin.y;
		gy += gridYOffset;
		if (gy < 0)
			gy = 0;
		if (gy >= gridYCheck)
			gy = gridYCheck - 1;
		gy /= maxGridRadius;
		uint32_t gridIndex = float2long(gx - 0.5f) + float2long(gy - 0.5f) * xGridwidth;
		int32_t result = add(gridIndex, object);
		return result;
	}
	return NO_ERROR;
}

//------------------------------------------------------------------------------
void CollisionGrid::createGrid(void)
{
	//------------------------------------------------
	// This block of code is only necessary if
	// we collide a giantObject against a giantObject.
	CollisionGridNodePtr g = giantObjects;
	uint32_t totalGiantObjects = 0;
	while (g)
	{
		if (g->next)
		{
			checkGrid(g->object, g->next);
			totalGiantObjects++;
		}
		g = g->next;
	}
	for (size_t y = 0; y < (int32_t)yGridwidth; y++)
	{
		for (size_t x = 0; x < (int32_t)xGridwidth; x++)
		{
			uint32_t gridIndex = x + y * xGridwidth;
			CollisionGridNodePtr g = grid[gridIndex];
			while (g)
			{
				GameObjectPtr obj = g->object;
				//--------------------------------------
				// Check against the big things.
				if (giantObjects)
					checkGrid(obj, giantObjects);
				CollisionGridNodePtr area = g->next;
				//---------------------------
				// Check same grid as object
				if (area)
					checkGrid(obj, area);
				if (x < int32_t(xGridwidth - 1))
				{
					//---------------------
					// Check grid at x+1,y
					area = grid[gridIndex + 1];
					if (area)
						checkGrid(obj, area);
					if (y < int32_t(yGridwidth - 1))
					{
						//-----------------------
						// Check grid at x+1,y+1
						area = grid[gridIndex + 1 + xGridwidth];
						if (area)
							checkGrid(obj, area);
					}
				}
				if (y < int32_t(yGridwidth - 1))
				{
					//---------------------
					// Check grid at x,y+1
					area = grid[gridIndex + xGridwidth];
					if (area)
						checkGrid(obj, area);
				}
				g = g->next;
			}
		}
	}
}

//------------------------------------------------------------------------------
void CollisionGrid::checkGrid(GameObjectPtr obj1, CollisionGridNodePtr area)
{
	while (area)
	{
		GameObjectPtr obj2 = area->object;
		area = area->next;
		if (obj1 && obj2)
		{
			//-------------------------------------------------------------
			// CULL collisions between things which can never collide here
			//------------------------------------------------------------
			if ((obj1->getObjectClass() == TURRET) && (obj2->getObjectClass() == TURRET) || (obj1->getObjectClass() == GATE) && (obj2->getObjectClass() == GATE) || (obj1->getObjectClass() == GATE) && (obj2->getObjectClass() == TURRET) || (obj1->getObjectClass() == TURRET) && (obj2->getObjectClass() == GATE) || (obj1->getObjectClass() == TURRET) && (obj2->getObjectClass() == TREE) || (obj1->getObjectClass() == TREE) && (obj2->getObjectClass() == TURRET) || (obj1->getObjectClass() == EXPLOSION) && (obj2->getObjectClass() == EXPLOSION))
			{
			}
			else
			{
				//--------------------------------------------------------
				// At this point, we have two objects in the same area
				// and they can collide.  We now run the bigBoy detection
				CollisionSystem::detectCollision(obj1, obj2);
			}
		}
	}
}

//------------------------------------------------------------------------------
// class CollisionSystem
PVOID
CollisionSystem::operator new(size_t mySize)
{
	PVOID result = nullptr;
	result = systemHeap->Malloc(mySize);
	return (result);
}

//------------------------------------------------------------------------------
void CollisionSystem::operator delete(PVOID us)
{
	systemHeap->Free(us);
}

//------------------------------------------------------------------------------
int32_t
CollisionSystem::init(FitIniFile* scenarioFile)
{
	xGridSize = 24;
	yGridSize = 24;
	gridRadius = 200;
	maxObjects = 300;
	maxCollisions = 100;
	warningDist = 250.0; // This is in world Units!!!
	alertTime = 2.5; // This is in seconds
	collisionHeap = new UserHeap;
	gosASSERT(collisionHeap != nullptr);
	int32_t result = collisionHeap->init(65535);
	gosASSERT(result == NO_ERROR);
	collisionGrid = new CollisionGrid;
	gosASSERT(collisionGrid != nullptr);
	globalCollisionAlert = new GlobalCollisionAlert;
	gosASSERT(globalCollisionAlert);
	result = globalCollisionAlert->init(20);
	gosASSERT(result == NO_ERROR);
	return (NO_ERROR);
}

#define MAX_LISTS_TO_CHECK 3
//------------------------------------------------------------------------------
void CollisionSystem::checkObjects(void)
{
	Stuff::Vector3D gridCenter(0L, 0L, 0L);
	collisionGrid->init(gridCenter);
	//-----------------------------------------------------------
	// Reset the Collision Alerts
	globalCollisionAlert->purgeRecords();
#if 1
	GameObjectPtr* objList = nullptr;
	int32_t numCollidables = ObjectManager->getCollidableList(objList);
	for (size_t i = 0; i < numCollidables; i++)
	{
		if (objList[i] && objList[i]->getExists() && objList[i]->getTangible())
		{
#ifdef _DEBUG
			int32_t result =
#endif
				collisionGrid->add(objList[i]);
			gosASSERT(result == NO_ERROR);
			objList[i]->handleStaticCollision();
		}
	}
#else
	//---------------------------------------------------------
	// Convert to Glenn's Magical New Object System!
	ObjectQueueNodePtr objList = objectList->getHeadList(); // Start with the default list.
	ObjectNodePtr objNode = nullptr;
	uint32_t objectsPerList[MAX_LISTS_TO_CHECK] = {0, 0, 0};
	while (objList && (currentList < MAX_LISTS_TO_CHECK))
	{
		//----------------------------------------------------
		// if getObjectType returns nullptr we are a baseObject.
		// Something like the sun or terrain.  DO NOT CHECK!!
		// We cast as a GameObject and that will seriously
		// mess up if we are just a baseObject!
		if (objNode && objNode->getObjectType() != nullptr)
		{
			result = collisionGrid->add((GameObjectPtr)objNode);
			objectsPerList[currentList]++;
			//----------------------------------------------------------
			// This can only happen if we are out of nodes.
			// Just break out of loop and check what we have, cause
			// no more are getting added!  We should probably Inform
			// the debug crowd of this!!
			gosASSERT(result == NO_ERROR);
			//-----------------------------------------------------
			//-- For right now, just explosions and artillery
			//-- Movers need a revised check for the code to work.
			//-- Code is in.  Away we go.
			objNode->handleStaticCollision();
			objNode = objNode->next;
		}
		else
		{
			objNode = objList->head;
		}
		if (objNode == nullptr)
		{
			objList = objList->next;
			currentList++;
		}
	}
#endif
	collisionGrid->createGrid();
}

//------------------------------------------------------------------------------

void CollisionSystem::detectCollision(GameObjectPtr obj1, GameObjectPtr obj2)
{
	float timeOfClosest = 0.0;
	//---------------------------------------------------------
	// Convert to Glenn's Magical New Object System!
	// Need some way to know this is a MOVER/Collider!
	if ((obj1->getObjectClass() < EXPLOSION) && (obj2->getObjectClass() < EXPLOSION))
	{
		//---------------------------------------------------------------
		// Objects have to be in the same move plane in order to collide.
		// Thus, a copter can't collide with a tree...
		int32_t obj1Pos[2] = {0, 0};
		int32_t obj2Pos[2] = {0, 0};
		obj1->getCellPosition(obj1Pos[0], obj1Pos[1]);
		obj2->getCellPosition(obj2Pos[0], obj2Pos[1]);
		//-------------------------------------------
		// Do a cell based collision.  Ignore Radii.
		int32_t obj1Block, obj2Block;
		int32_t obj1Vertex, obj2Vertex;
		obj1->getBlockAndVertexNumber(obj1Block, obj1Vertex);
		obj2->getBlockAndVertexNumber(obj2Block, obj2Vertex);
		if ((obj1Block == obj2Block) && (obj1Vertex == obj2Vertex))
			if ((obj1Pos[0] == obj2Pos[0]) && (obj1Pos[1] == obj2Pos[1]))
			{
				//------------------------------------------------
				// OK, we now need to check extents to determine
				// If collision happened.  May not need to go any
				// further for Honor Bound.
				checkExtents(obj1, obj2, timeOfClosest);
			}
	}
	else
	{
		//--------------------------------------------------------
		// First we check and see if we are already colliding.
		// If so, dump us to checkExtents.
		Stuff::Vector3D obj1Pos = obj1->getPosition();
		Stuff::Vector3D obj2Pos = obj2->getPosition();
		Stuff::Vector3D pos;
		pos.Subtract(obj2Pos, obj1Pos);
		pos.z = 0.0; // Ignore Elevation.  May cause explosion/artillery f-ups!
		float distMag = pos.x * pos.x + pos.y * pos.y; // pos.magnitude();
		float dist0 = obj1->getExtentRadius();
		float dist1 = obj2->getExtentRadius();
		float maxDist = dist1 + dist0;
		maxDist *= maxDist;
		if (distMag < maxDist)
		{
			//------------------------------------------------
			// OK, we now need to check extents to determine
			// If collision happened.  May not need to go any
			// further for Honor Bound.
			checkExtents(obj1, obj2, timeOfClosest);
		}
	}
}

//------------------------------------------------------------------------------
void CollisionSystem::detectStaticCollision(GameObjectPtr obj1, GameObjectPtr obj2)
{
	float timeOfClosest = 0.0;
	//--------------------------------------------------------
	// First we check and see if we are already colliding.
	// If so, dump us to checkExtents.
	Stuff::Vector3D obj1Pos = obj1->getPosition();
	Stuff::Vector3D obj2Pos = obj2->getPosition();
	Stuff::Vector3D pos;
	pos.Subtract(obj2Pos, obj1Pos);
	float distMag = pos.x * pos.x + pos.y * pos.y;
	float dist0 = obj1->getExtentRadius();
	float dist1 = obj2->getExtentRadius();
	float maxDist = dist1 + dist0;
	maxDist *= maxDist;
	if (distMag < maxDist)
	{
		//-------------------------------------------------------------------------------------------------------
		// If we are inside the extent radius AND standing on impassable
		// terrain, we hit the static object. ONLY if we are a mover.  We hit
		// them if we are not a mover and we are an artillery or turret or gate.
		if (obj1->isMover() && (obj2->getObjectClass() != TREE) && (obj2->getObjectClass() != TERRAINOBJECT))
		{
			if (!(obj2->isSpecialBuilding()) && (obj2->getObjectClass() != BRIDGE))
			{
				int32_t obj1Pos[2] = {0, 0};
				obj1->getCellPosition(obj1Pos[0], obj1Pos[1]);
				if (GameMap->getPassable(obj1Pos[0], obj1Pos[1]))
					return;
			}
		}
		checkExtents(obj1, obj2, timeOfClosest);
	}
}

//------------------------------------------------------------------------------

void CollisionSystem::checkExtents(GameObjectPtr obj1, GameObjectPtr obj2, float time)
{
	//---------------------------------------------------------
	// We may not need any more information for MechCommander2
	// At this point, simply build a CollisionRecord.
	// If we do need more info, add extent checks here.
	//------------------------------------------------------------
	// If we are an explosion, handle our collisions here and
	// DO NOT add our record.  Fuel Tank Farms will completely
	// swamp this here otherwise.
	//
	// In fact, I can think of NO reason why we wouldn't just
	// handle all of the collisions here anyway.  Saves us the
	// trouble of ever running out of collision records and since
	// we run process collisions afterwards anyway and nothing
	// in MechCommander can ever go away, why not?
	//
	// We handle all here.  NO more collision records.
	//
	// Wow, does this not work.
	// Helicopters still need to collide with turrets and explosions and
	// artillery. Make it so. -fs
	if (!obj1->getExists() || !obj2->getExists())
		return;
	if ((obj1->getMoveLevel() == 2) || (obj2->getMoveLevel() == 2))
	{
		if ((obj1->getMoveLevel() != obj2->getMoveLevel()) && (obj1->getObjectClass() != TURRET) && (obj2->getObjectClass() != TURRET) && (obj1->getObjectClass() != ARTILLERY) && (obj2->getObjectClass() != ARTILLERY) && (obj1->getObjectClass() != EXPLOSION) && (obj2->getObjectClass() != EXPLOSION))
			return;
	}
	ObjectTypePtr obj1Type = obj1->getObjectType();
	ObjectTypePtr obj2Type = obj2->getObjectType();
	bool obj1Result = obj1Type->handleCollision(obj1, obj2);
	bool obj2Result = obj2Type->handleCollision(obj2, obj1);
	//-----------------------------------------
	// If we came back TRUE, whack the object.
	if (obj1Result)
	{
		bool removeObject1 = obj1Type->handleDestruction(obj1, obj2);
		//------------------------------------------------------------
		// Pull the object from the objList here if this flag is set.
		// Otherwise, the object will still be around for a little
		// while as it destroys itself over the next several frames.
		if (removeObject1)
		{
			obj1->setExists(false);
		}
	}
	//-----------------------------------------
	// If we came back TRUE, whack the object.
	if (obj2Result)
	{
		bool removeObject2 = obj2Type->handleDestruction(obj2, obj1);
		//------------------------------------------------------------
		// Pull the object from the objList here if this flag is set.
		// Otherwise, the object will still be around for a little
		// while as it destroys itself over the next several frames.
		if (removeObject2)
		{
			obj2->setExists(false);
		}
	}
}

//------------------------------------------------------------------------------
float CollisionSystem::timeToImpact(GameObjectPtr obj1, GameObjectPtr obj2)
{
	float timeOfClosest = -1.0;
	//--------------------------------------------------------
	// First we check and see if we are already colliding.
	// If so, dump us to checkExtents.
	Stuff::Vector3D obj1Pos = obj1->getPosition();
	Stuff::Vector3D obj2Pos = obj2->getPosition();
	Stuff::Vector3D pos;
	pos.Subtract(obj2Pos, obj1Pos);
	float distMag = pos.x * pos.x + pos.y * pos.y; // pos.magnitude();
	float dist0 = obj1->getExtentRadius();
	float dist1 = obj2->getExtentRadius();
	float maxDist = dist1 + dist0;
	maxDist *= maxDist;
	if (distMag < maxDist)
	{
		//------------------------------------------------
		// OK, we now need to check extents to determine
		// If collision happened.  May not need to go any
		// further for Honor Bound.
		timeOfClosest = 0.0;
		return (timeOfClosest);
	}
	//--------------------------------------------------------
	// Then we check the current positions against the final
	// positions for the this frame and determine if the
	// objects are moving toward or away from each other.
	// If away, we're done.  If not, next step.
	Stuff::Vector3D obj1Vel = obj1->getVelocity();
	Stuff::Vector3D obj2Vel = obj2->getVelocity();
	Stuff::Vector3D obj1FPos;
	obj1Vel *= frameLength;
	obj1FPos.Add(obj1Pos, obj1Vel);
	Stuff::Vector3D obj2FPos;
	obj2Vel *= frameLength;
	obj2FPos.Add(obj2Pos, obj2Vel);
	Stuff::Vector3D obj1RPos;
	obj1RPos.Subtract(obj1FPos, obj1Pos);
	Stuff::Vector3D obj2RPos;
	obj2RPos.Subtract(obj2FPos, obj2Pos);
	float headingResult = obj1RPos * obj2RPos;
	//--------------------------------------------------------------
	// If this is negative, objects are heading towards each other.
	if (headingResult < 0)
	{
		//--------------------------------------------------------------
		// Now find the time of closest approach.  This is given by the
		// formula (pos DOT vel) / (vel DOT vel) = -timeOfClosest.
		Stuff::Vector3D vel;
		vel.Subtract(obj2Vel, obj1Vel);
		float timeOfClosest = -((pos * vel) / (vel * vel));
		if (timeOfClosest <= frameLength)
		{
			//------------------------------------------------
			// They are closest during this frame.
			// Find out what distance this is.
			Stuff::Vector3D dist;
			vel *= timeOfClosest;
			dist.Add(pos, vel);
			float distMag = dist.x * dist.x + dist.y * dist.y; // dist.magnitude();
			if (distMag < maxDist)
			{
				//------------------------------------------------
				// OK, we now need to check extents to determine
				// If collision happened.  May not need to go any
				// further for Honor Bound.
				return (timeOfClosest);
			}
		}
	}
	return (timeOfClosest);
}

//------------------------------------------------------------------------------
void CollisionSystem::destroy(void)
{
	delete collisionGrid;
	collisionGrid = nullptr;
	delete collisionHeap;
	collisionHeap = nullptr;
	delete globalCollisionAlert;
	globalCollisionAlert = nullptr;
}

//------------------------------------------------------------------------------
