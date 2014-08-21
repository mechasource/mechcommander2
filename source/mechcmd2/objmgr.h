//***************************************************************************
//
//	objmgr.h - This file contains the GameObject Manager class
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef OBJMGR_H
#define OBJMGR_H

//---------------------------------------------------------------------------

//#include "dobjmgr.h"
//#include "gameobj.h"
//#include "mover.h"
//#include "dmech.h"
//#include "dgvehicl.h"
//#include "delemntl.h"
//#include "dterrobj.h"
//#include "dbldng.h"
//#include "dweaponbolt.h"
//#include "dcarnage.h"
//#include "dartlry.h"
//#include "dobjblck.h"
//#include "dturret.h"
//#include "dgate.h"
//#include "dcollsn.h"

class PacketFile;

//---------------------------------------------------------------------------

#define	MOVERLIST_DELETE	0
#define	MOVERLIST_ADD		1
#define	MOVERLIST_TRADE		2

#define NO_RAM_FOR_TERRAIN_OBJECT_FILE		0xBAAA0014
#define NO_RAM_FOR_TERRAIN_OBJECT_HEAP		0xBAAA0015
#define NO_RAM_FOR_OBJECT_BLOCK_NUM			0xBAAA0016
#define NO_RAM_FOR_OBJECT_LISTS				0xBAAA0017
#define NO_RAM_FOR_OBJECT_DATA_BLOCK		0xBAAA0018
#define NO_RAM_FOR_OBJECT_BLOCK_USER		0xBAAA0019
#define NO_RAM_FOR_LAST_BLOCK				0xBAAA001A
#define NO_RAM_FOR_CENTER_BLOCK				0xBAAA001B
#define OBJECTBLOCK_OUTOFRANGE				0xBAAA001C
#define NO_AVAILABLE_OBJQUEUE				0xBAAA001D
#define COULDNT_MAKE_TERRAIN_OBJECT			0xBAAA001E
#define OBJECTBLOCK_NULL					0xBAAA001F
#define BLOCK_NOT_CACHED					0xBAAA0020
#define COULDNT_CREATE_OBJECT				0xBAAA0021
#define OBJECT_NOT_FOUND					0xBAAA0022

#pragma pack(1)

struct ObjData {
	short				objTypeNum;		//Type number of object
	uint16_t		vertexNumber;	//Vertex Number in Block.
	uint16_t		blockNumber;	//Which terrain Block.
	uint8_t		damage;			//Damage
	int32_t				teamId;
	ULONG		parentId;		//hOW AM i LINKED.
};

struct MiscObjectData {
	int32_t				blockNumber;	//Terrain Block I occupy
	int32_t				vertexNumber;	//Terrain Vertex I occupy
	int32_t				objectTypeNum;	//ObjectTypeNumber for this overlay tile
	int32_t				damaged;		//Is this overlay tile damaged or not
};

struct ObjDataLoader {
		int32_t objTypeNum;
		Stuff::Vector3D vector;

		float rotation;		
		int32_t damage;
		
		int32_t teamId;
		int32_t parentId;

		// convert to block and vertex
		int32_t tileCol;
		int32_t tileRow;
		
		int32_t blockNumber;
		int32_t vertexNumber;
};

#pragma pack()

typedef ObjData *ObjDataPtr;

//---------------------------------------------------------------------------

#define	MAX_REINFORCEMENTS_PER_TEAM		16
#define	MAX_GATE_CONTROLS				50
#define	MAX_TURRET_CONTROLS				50
#define MAX_POWER_GENERATORS			50
#define MAX_SPECIAL_BUILDINGS			200

#pragma pack(1)

typedef struct _TeamObjectInfo {
	short		numMechs;
	short		numVehicles;
	short		numElementals;
	short		mechHandleIndex;
	short		vehicleHandleIndex;
	short		elementalHandleIndex;
} TeamObjectInfo;

#pragma pack()

#define	MAX_REMOVED	20
#define	MAX_WATCHES 4000

#define	MAX_CAPTURES_PER_TEAM	30

typedef struct _RemovedMoverRec {
	ULONG	turn;
	int32_t			partID;
} RemovedMoverRec;

typedef struct _ObjectManagerData
{
	int32_t					maxObjects;
	int32_t					numElementals;
	int32_t					numTerrainObjects;
	int32_t					numBuildings;
	int32_t					numTurrets;
	int32_t					numWeapons;
	int32_t					numCarnage;
	int32_t					numLights;
	int32_t					numArtillery;
	int32_t					numGates;
	int32_t					maxMechs;
	int32_t					maxVehicles;
	int32_t					numMechs;
	int32_t					numVehicles;
	int32_t					nextWatchId;
} ObjectManagerData;

class GameObjectManager {

	public:

		ObjectTypeManagerPtr	objTypeManager;

		int32_t					numMechs;
		int32_t					numVehicles;
		int32_t					numElementals;
		int32_t					numTerrainObjects;
		int32_t					numBuildings;
		int32_t					numTurrets;
		int32_t					numWeapons;
		int32_t					numCarnage;
		int32_t					numLights;
		int32_t					numArtillery;
		int32_t					numGates;
		int32_t					maxMechs;
		int32_t					maxVehicles;
		int32_t					maxMovers;

		BattleMechPtr*			mechs;
		GroundVehiclePtr*		vehicles;
		ElementalPtr*			elementals;
		TerrainObjectPtr*		terrainObjects;
		BuildingPtr*			buildings;
		WeaponBoltPtr*			weapons;
		LightPtr*				lights;
		CarnagePtr*				carnage;
		ArtilleryPtr*			artillery;
		TurretPtr*				turrets;
		GatePtr*				gates;
		
		BuildingPtr				gateControls[MAX_GATE_CONTROLS];
		BuildingPtr				turretControls[MAX_TURRET_CONTROLS];
		BuildingPtr				powerGenerators[MAX_POWER_GENERATORS];
		BuildingPtr				specialBuildings[MAX_SPECIAL_BUILDINGS];		//These must be updated EVERY FRAME.  Perimeter alarms, etc.
		int32_t					numGateControls;
		int32_t					numTurretControls;
		int32_t					numPowerGenerators;
		int32_t					numSpecialBuildings;

		PSTR					moverLineOfSightTable;
		bool					useMoverLineOfSightTable;

		GameObjectPtr*			objList;
		GameObjectPtr*			collidableList;
		MoverPtr				moverList[MAX_MOVERS];
		MoverPtr				goodMoverList[MAX_MOVERS];
		MoverPtr				badMoverList[MAX_MOVERS];
		int32_t					numCollidables;
		int32_t					numGoodMovers;
		int32_t					numBadMovers;
		int32_t					numMovers;
		int32_t					nextReinforcementPartId;
		int32_t					numRemoved;
		RemovedMoverRec			moversRemoved[MAX_REMOVED];
		ULONG			nextWatchID;
		GameObjectPtr			*watchList;

		int32_t					currentWeaponsIndex;			//points to next entry in rotating weapon array.
		int32_t					currentCarnageIndex;			//points to next entry in rotating carnage list		
		int32_t					currentLightIndex;
		int32_t					currentArtilleryIndex;
		
		bool					rebuildCollidableList;

		int32_t					numCaptures[MAX_TEAMS];
		GameObjectWatchID		captureList[MAX_TEAMS][MAX_CAPTURES_PER_TEAM];

		int32_t					totalObjCount;
		ObjDataLoader			*objData;						//Used to keep from loading twice!!
		CollisionSystemPtr		collisionSystem;

	private:

		GameObjectPtr findObjectByMouse (int32_t mouseX,
										 int32_t mouseY,
										 GameObjectPtr* searchList,
										 int32_t listSize,
										 bool skipDisabled = false);

		GameObjectPtr findMoverByMouse (int32_t mouseX,
										int32_t mouseY,
										int32_t teamId,
										bool skipDisabled = false);

		GameObjectPtr findTerrainObjectByMouse (int32_t mouseX,
												int32_t mouseY,
												bool skipDisabled = false);

		void addObject(ObjDataLoader *objData, int32_t& objIndex, int32_t& buildIndex, 
			int32_t &turretIndex, int32_t &gateIndex, int32_t& curCollideHandle, int32_t& curNonCollideHandle);

		void countObject( ObjDataLoader* objType);


	public:

		PVOID operator new (size_t mySize);

		void operator delete (PVOID us);
		
		void init (void);
		
		GameObjectManager (void) {
			init();
		}

		void destroy (void);

		~GameObjectManager (void) {
			destroy();
		}
		
		void init (PSTR objTypeDataFile, int32_t objTypeCacheSize, int32_t objCacheSize);

		void setNumObjects (int32_t nMechs,
							int32_t nVehicles,
							int32_t nElementals,
							int32_t nTerrainObjects,
							int32_t nBuildings,
							int32_t nTurrets,
							int32_t nWeapons,
							int32_t nCarnage,
							int32_t nLights,
							int32_t nArtillery,
							int32_t nGates);

		int32_t getNumObjects (void) {
			return(numMechs +
				   numVehicles +
				   numElementals +
				   numTerrainObjects +
				   numBuildings +
				   numTurrets +
				   numWeapons +
				   numCarnage +
				   numLights +
				   numArtillery + 
				   numGates);
		}

		int32_t getMaxObjects (void) {
			return(maxMechs +
				   maxVehicles +
				   numElementals +
				   numTerrainObjects +
				   numBuildings +
				   numTurrets +
				   numWeapons +
				   numCarnage +
				   numLights +
				   numArtillery + 
				   numGates);
		}

		int32_t load (File* gameFile);

		int32_t save (File* gameFile);

		BattleMechPtr newMech (void);

		GroundVehiclePtr newVehicle (void);

		void freeMover (MoverPtr mover);
		
		void tradeMover (MoverPtr mover, int32_t newTeamID, int32_t newCommanderID);

#ifdef USE_ELEMENTAL
		BattleMechPtr addElemental (void);
#endif

		BattleMechPtr getMech (int32_t mechIndex);

		GroundVehiclePtr getVehicle (int32_t vehicleIndex);

		GroundVehiclePtr getOpenVehicle (void);

		ElementalPtr getElemental (int32_t elementalIndex);

		TerrainObjectPtr getTerrainObject (int32_t terrainObjectIndex);

		BuildingPtr getBuilding (int32_t buildingIndex);

		TurretPtr getTurret (int32_t turretIndex);
		
		GatePtr	getGate (int32_t gateIndex);
		
		WeaponBoltPtr getWeapon (void);				//gets next available weapon.

		CarnagePtr getCarnage (CarnageEnumType carnageType);

		void releaseCarnage (CarnagePtr obj);

		LightPtr getLight (void);

		void releaseLight (LightPtr obj);

		ArtilleryPtr getArtillery (void);

		int32_t getNumArtillery (void) {
			return(numArtillery);
		}

		ArtilleryPtr getArtillery (int32_t artilleryIndex) {
			return(artillery[artilleryIndex]);
		}

		int32_t getNumGateControls (void) {
			return(numGateControls);
		}

		int32_t getNumTurretControls (void) {
			return(numTurretControls);
		}

		BuildingPtr getGateControl (int32_t index) {
			return(gateControls[index]);
		}

		BuildingPtr getTurretControl (int32_t index) {
			return(turretControls[index]);
		}

		void countTerrainObjects (PacketFile* pFile, int32_t firstHandle);

		void loadTerrainObjects ( PacketFile* pFile, volatile float& progress, float progressRange );

		int32_t getSpecificObjects (int32_t objClass, int32_t objSubType, GameObjectPtr* objects, int32_t maxObjects);

		GameObjectPtr getObjBlockObject (int32_t blockNumber, int32_t objLocalIndex) {
			return(objList[Terrain::objBlockInfo[blockNumber].firstHandle + objLocalIndex]);
		}

		int32_t getObjBlockNumObjects (int32_t blockNumber) {
			return(Terrain::objBlockInfo[blockNumber].numObjects);
		}

		int32_t getObjBlockNumCollidables (int32_t blockNumber) {
			return(Terrain::objBlockInfo[blockNumber].numCollidableObjects);
		}

		int32_t getObjBlockFirstHandle (int32_t blockNumber) {
			return(Terrain::objBlockInfo[blockNumber].firstHandle);
		}

		int32_t getNumMechs (void) {
			return(numMechs);
		}

		int32_t getNumVehicles (void) {
			return(numVehicles);
		}

		int32_t getNumMovers (void) {
			return(numMovers);
		}

		int32_t getMaxMovers (void) {
			return(maxMovers);
		}

		int32_t getNumGoodMovers (void) {
			return(numGoodMovers);
		}

		int32_t getNumBadMovers (void) {
			return(numBadMovers);
		}

		int32_t getNumTerrainObjects (void) {
			return(numTerrainObjects);
		}

		int32_t getNumBuildings (void) {
			return(numBuildings);
		}

		int32_t getNumTurrets (void) {
			return(numTurrets);
		}

		int32_t getNumGates (void) {
			return(numGates);
		}

		void render (bool terrain, bool movers, bool other);

		void renderShadows (bool terrain, bool movers, bool other);

		void update (bool terrain, bool movers, bool other);
		void updateAppearancesOnly( bool terrain, bool mover, bool other);

		GameObjectPtr get (int32_t handle);

		GameObjectPtr getByWatchID (ULONG watchID) {
			if ((watchID > 0) && (watchID < nextWatchID))
				return(watchList[watchID]);
			return(NULL);
		}

		int32_t buildMoverLists (void);

		bool modifyMoverLists (MoverPtr mover, int32_t action);

		MoverPtr getMover (int32_t index) {
			return(moverList[index]);
		}

		MoverPtr getGoodMover (int32_t index) {
			return(goodMoverList[index]);
		}

		MoverPtr getBadMover (int32_t index) {
			return(badMoverList[index]);
		}

		GameObjectPtr findObject (Stuff::Vector3D position);

		GameObjectPtr findByPartId (int32_t partId);

		GameObjectPtr findByBlockVertex (int32_t blockNum, int32_t vertex);

		GameObjectPtr findByCellPosition (int32_t row, int32_t col);

		GameObjectPtr findByUnitInfo (int32_t commander, int32_t group, int32_t mate);

		GameObjectPtr findObjectByTypeHandle (int32_t typeHandle);

		GameObjectPtr findObjectByMouse (int32_t mouseX, int32_t mouseY);
		
		bool moverInRect(int32_t index, Stuff::Vector3D &dStart, Stuff::Vector3D &dEnd);

		ObjectTypePtr loadObjectType (ObjectTypeNumber typeHandle);

		ObjectTypePtr getObjectType (ObjectTypeNumber typeHandle);

		void removeObjectType (ObjectTypeNumber typeHandle);

		GameObjectHandle getHandle (GameObjectPtr obj);

		int32_t calcPartId (int32_t objectClass, int32_t param1 = 0, int32_t param2 = 0, int32_t param3 = 0);

		void setPartId (GameObjectPtr obj, int32_t param1 = 0, int32_t param2 = 0, int32_t param3 = 0);

		int32_t buildCollidableList (void);

		int32_t initCollisionSystem (FitIniFile* missionFile);

		int32_t getCollidableList (GameObjectPtr*& objList);

		int32_t updateCollisions (void);

		void detectStaticCollision (GameObjectPtr obj1, GameObjectPtr obj2);
		
		void updateCaptureList (void);

		bool GameObjectManager::isTeamCapturing (TeamPtr team, GameObjectWatchID targetWID);

		CarnagePtr createFire (ObjectTypeNumber fireObjTypeHandle,
							   GameObjectPtr owner,
							   Stuff::Vector3D& position,
							   float tonnage);

		CarnagePtr createExplosion (int32_t effectId,
									GameObjectPtr owner,
									Stuff::Vector3D& position,
									float dmg = 0.0,
									float radius = 0.0);

		LightPtr createLight (ObjectTypeNumber lightObjTypeHandle);

		WeaponBoltPtr createWeaponBolt (int32_t effectId);

		ArtilleryPtr createArtillery (int32_t artilleryType, Stuff::Vector3D& position);

		RemovedMoverRec* getRemovedMovers (int32_t& numRemovedMovers) {
			numRemovedMovers = numRemoved;
			return(moversRemoved);
		}

		bool isRemoved (MoverPtr mover);

		void setWatchID (GameObjectPtr obj);

		void CopyTo (ObjectManagerData *data);
		void CopyFrom (ObjectManagerData *data);

		int32_t Save (PacketFilePtr file, int32_t packetNum);
		int32_t Load (PacketFilePtr file, int32_t packetNum);
};

extern GameObjectManagerPtr ObjectManager;

//***************************************************************************

#endif
