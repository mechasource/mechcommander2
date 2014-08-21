//******************************************************************************
//	collsn.h - This file contains the Collision Detection System
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef COLLSN_H
#define COLLSN_H
//------------------------------------------------------------------------------

//#include <mclib.h>
//#include "dcollsn.h"
//#include "dgameobj.h"

//------------------------------------------------------------------------------
// Macro Definitions
#ifndef NO_ERROR
#define NO_ERROR	0
#endif

//------------------------------------------------------------------------------
// classes
struct CollisionGridNode
{
	GameObjectPtr			object;
	CollisionGridNodePtr	next;
};

//------------------------------------------------------------------------------
struct CollisionAlertRecord
{
	GameObjectPtr			object1;
	GameObjectPtr			object2;
	float					currentDistance;
	float					timeToImpact;
};

typedef CollisionAlertRecord *CollisionAlertRecordPtr;
//------------------------------------------------------------------------------
class GlobalCollisionAlert
{
	//Data Members
	//-------------
	protected:
		CollisionAlertRecordPtr		collisionAlerts;
		ULONG				maxAlerts;
		ULONG				nextRecord;
		
	public:
		GlobalCollisionAlert (void)
		{
			collisionAlerts = NULL;
			maxAlerts = 0;
		}

		int32_t init (ULONG maxCollisionAlerts);
		
		void destroy (void);
		
		~GlobalCollisionAlert (void)
		{
			destroy();
		}
		
		int32_t addRecord (GameObjectPtr obj1, GameObjectPtr obj2, float distance, float time);
		
		CollisionAlertRecordPtr findAlert (GameObjectPtr object, CollisionAlertRecordPtr startRecord = NULL);
		
		void purgeRecords (void);
};

extern GlobalCollisionAlert *globalCollisionAlert;

//------------------------------------------------------------------------------
class CollisionGrid
{
	//Data Members
	//-------------
	protected:
		ULONG			xGridWidth;			//Number of gridNodes in x direction
		ULONG			yGridWidth;			//Number of gridNodes in y direction
													//In theory we would need a z but not for a mech game!!
		
		ULONG			maxGridRadius;		//Max radius in (m) of each grid node.
		
		ULONG			maxObjects;			//Max number of objects in world.
		
		CollisionGridNodePtr	giantObjects;		//Collection of objects larger than maxGridRadius
		CollisionGridNodePtr	*grid;				//Pointer to array of gridNodes layed out in space
		CollisionGridNodePtr	nodes;				//Actual grid nodes available to layout in space.
		
		ULONG			nextAvailableNode;	//next node in nodes which can be used.
		Stuff::Vector3D			gridOrigin;			//Center point of the grid.
		
		bool					gridIsGo;			//Have we already allocated everything?
		
		ULONG			gridSize;
		ULONG			nodeSize;
		
		float					gridXOffset;
		float 					gridYOffset;
		
		float 					gridXCheck;
		float 					gridYCheck;
		
	//Member Functions
	//-----------------
	public:

		PVOID operator new (size_t mySize);
		void operator delete (PVOID us);
		
		void init (void)
		{
			giantObjects = NULL;
			grid = NULL;
			nodes = NULL;
			
			xGridWidth = yGridWidth = 0;
			
			maxGridRadius = 0;
			
			nextAvailableNode = 0;
			
			gridOrigin.Zero();
			
			gridIsGo = FALSE;
		}
		
		CollisionGrid (void)
		{
			init();
		}
		
		int32_t init (Stuff::Vector3D &newOrigin);
		
		void destroy (void);
		
		~CollisionGrid (void)
		{
			destroy();
		}
		
		int32_t add (ULONG gridIndex, GameObjectPtr object);
		int32_t add (GameObjectPtr object);
		
		void createGrid (void);		//Put all objects in world into grids
		
		void checkGrid (GameObjectPtr object, CollisionGridNodePtr area);	//Check each object against grid
};

//------------------------------------------------------------------------------
struct CollisionRecord
{
	GameObjectPtr		obj1;		//Which objects hit each other
	GameObjectPtr		obj2;
	float				time;		//When did they do it relative to current frame.
};
	
//------------------------------------------------------------------------------
class CollisionSystem
{
	//Data Members
	//-------------
	protected:

		CollisionGridPtr		collisionGrid;
		
	public:
	
		static ULONG	xGridSize;
		static ULONG	yGridSize;
		static ULONG	gridRadius;
		static ULONG	maxObjects;
		static ULONG	maxCollisions;
		static ULONG	numCollisions;
		
		float					warningDist;		//Distance to worry about short term collision avoidance (in World Units!!)
		float					alertTime;			//Time to worry about short term collision avoidance.
		
		static UserHeapPtr		collisionHeap;
	
	//Member Functions
	//-----------------
	protected:

		CollisionRecordPtr findNextPending (void);

	public:
	
		PVOID operator new (size_t mySize);
		void operator delete (PVOID us);
		
		void init (void)
		{
			collisionGrid = NULL;
		}
		
		CollisionSystem (void)
		{
			init();
		}

		int32_t init (FitIniFile *scenarioFile);
				
		void destroy (void);
		
		~CollisionSystem (void)
		{
			destroy();
		}
		
		void checkObjects (void);
		
		static void detectCollision (GameObjectPtr obj1, GameObjectPtr obj2);
		
		void detectStaticCollision (GameObjectPtr obj1, GameObjectPtr obj2);

		float timeToImpact (GameObjectPtr obj1, GameObjectPtr obj2);
		
		static void checkExtents (GameObjectPtr obj1, GameObjectPtr obj2, float time);
};

extern CollisionSystem *collisionSystem;
//------------------------------------------------------------------------------
#endif