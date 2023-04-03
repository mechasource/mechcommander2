//---------------------------------------------------------------------------
//
// ObjType.cpp -- File contains the Basic Game Object Type code
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"

//---------------------------------------------------------------------------
// Include Files
// #include "mclib.h"

#ifndef OBJTYPE_H
#include "objtype.h"
#endif

#ifndef DOBJNUM_H
#include "dobjnum.h"
#endif

#ifndef GAMEOBJ_H
#include "gameobj.h"
#endif

#ifndef MECH_H
#include "mech.h"
#endif

#ifndef GVEHICL_H
#include "gvehicl.h"
#endif

#ifndef BLDNG_H
#include "bldng.h"
#endif

#ifndef TERROBJ_H
#include "terrobj.h"
#endif

#ifndef WEAPONBOLT_H
#include "weaponbolt.h"
#endif

#ifndef CARNAGE_H
#include "carnage.h"
#endif

#ifndef TURRET_H
#include "turret.h"
#endif

#ifndef GATE_H
#include "gate.h"
#endif

#ifndef ARTLRY_H
#include "artlry.h"
#endif

#ifndef WEAPONFX_H
#include "weaponfx.h"
#endif

//---------------------------------------------------------------------------

PacketFilePtr ObjectTypeManager::objectFile = nullptr;
UserHeapPtr ObjectTypeManager::objectTypeCache = nullptr;
UserHeapPtr ObjectTypeManager::objectCache = nullptr;
int32_t ObjectTypeManager::bridgeTypeHandle = 0xFFFFFFFF;
int32_t ObjectTypeManager::forestTypeHandle = 0xFFFFFFFF;
int32_t ObjectTypeManager::wallHeavyTypeHandle = 0xFFFFFFFF;
int32_t ObjectTypeManager::wallMediumTypeHandle = 0xFFFFFFFF;
int32_t ObjectTypeManager::wallLightTypeHandle = 0xFFFFFFFF;

uint32_t NextIdNumber = 0x30000001; // Big number to indicate
// This object is NOT a mission
// part.  Trust Me. -ffs

//***************************************************************************
//* OBJECTTYPE class
//***************************************************************************

PVOID
ObjectType::operator new(size_t ourSize)
{
	PVOID result = ObjectTypeManager::objectTypeCache->Malloc(ourSize);
	return (result);
}

//---------------------------------------------------------------------------

void ObjectType::operator delete(PVOID us)
{
	if (!((ObjectTypePtr)us)->inUse())
	{
		ObjectTypeManager::objectTypeCache->Free(us);
	}
}

//---------------------------------------------------------------------------

GameObjectPtr
ObjectType::createInstance(void)
{
	GameObjectPtr result = new GameObject;
	result->init(true, this);
	// result->setIdNumber(NextIdNumber++);
	return (result);
}

//---------------------------------------------------------------------------

void ObjectType::destroy(void)
{
	// Nothing at the moment!
	ObjectTypeManager::objectTypeCache->Free(appearName);
	appearName = nullptr;
}

//---------------------------------------------------------------------------

int32_t
ObjectType::init(std::unique_ptr<File> objFile, uint32_t fileSize)
{
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
ObjectType::init(FitIniFilePtr objFile)
{
	int32_t result = 0;
	result = objFile->seekBlock("ObjectType");
	if (result != NO_ERROR)
		return (result);
	numUsers = 0;
	//--------------------------------------------------------------------
	// Read in the rest of the Object type Data here.  This includes
	// exposions, collision extents, etc.
	wchar_t apprName[512];
	result = objFile->readIdString("AppearanceName", apprName, 511);
	if (result == NO_ERROR)
	{
		appearName = (std::wstring_view)ObjectTypeManager::objectTypeCache->Malloc(strlen(apprName) + 1);
		strcpy(appearName, apprName);
	}
	result = objFile->readIdLong("ExplosionObject", explosionObject);
	if (result != NO_ERROR)
		return (result);
	result = objFile->readIdLong("DestroyedObject", destroyedObject);
	if (result != NO_ERROR)
		return (result);
	result = objFile->readIdFloat("ExtentRadius", extentRadius);
	if (result != NO_ERROR)
		extentRadius = 0.0f;
	result = objFile->readIdLong("IconNumber", iconNumber);
	if (result != NO_ERROR)
		iconNumber = -1;
	keepMe = true; // Never cache out anymore!
	teamId = -1; // Everything starts out Neutral now.
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

void ObjectType::createExplosion(Stuff::Vector3D& position, float dmg, float rad)
{
	int32_t effectId = weaponEffects->GetEffectObjNum(explosionObject);
	if (explosionObject >= 0)
	{
		if (rad == 0.0)
			rad = getExtentRadius();
		ObjectManager->createExplosion(effectId, nullptr, position, dmg, rad);
	}
}

//---------------------------------------------------------------------------

bool ObjectType::handleCollision(GameObjectPtr collidee, GameObjectPtr collider)
{
	//---------------------------------------------
	// The default reaction of any object in the world
	// is to simply explode.  This just returns TRUE
	// to facilitate this behaviour.
	return (true);
}

//---------------------------------------------------------------------------

bool ObjectType::handleDestruction(GameObjectPtr collidee, GameObjectPtr collider)
{
	//---------------------------------------------
	// The default reaction of any object in the world
	// is to simply explode.  This routine will create
	// the associated explosions, debris, smoke, fire, etc.
	// and then return TRUE, to indicate that the object
	// has, in fact, gone south.  The object is NOT
	// removed from the object list here!!  Please don't
	// do it here either since it will screw up much code.
	Stuff::Vector3D pos = collidee->getPosition();
	createExplosion(pos);
	return (true);
}

//***************************************************************************
//* GAMEOBJECT TYPE MANAGER class
//***************************************************************************

int32_t
ObjectTypeManager::init(std::wstring_view objectFileName, int32_t objectTypeCacheSize,
	int32_t objectCacheSize, int32_t maxObjectTypes)
{
	FullPathFileName objectName;
	objectName.init(objectPath, objectFileName, ".pak");
	objectFile = new PacketFile;
	if (!objectFile)
		return NO_RAM_FOR_OBJECT_TYPE_FILE;
	int32_t result = objectFile->open(objectName);
	if (result != NO_ERROR)
		return (result);
	objectTypeCache = new UserHeap;
	if (!objectTypeCache)
		return NO_RAM_FOR_OBJECT_TYPE_CACHE;
	result = objectTypeCache->init(objectTypeCacheSize, "ObjectTypeHeap");
	if (result != NO_ERROR)
		return (result);
	objectCache = new UserHeap;
	if (!objectCache)
		return NO_RAM_FOR_OBJECT_CACHE;
	result = objectCache->init(objectCacheSize, "ObjectHeap");
	if (result != NO_ERROR)
		return (result);
	numObjectTypes = maxObjectTypes;
	table = (ObjectTypePtr*)objectTypeCache->Malloc(sizeof(ObjectTypePtr) * numObjectTypes);
	if (!table)
		Fatal(0, " ObjectTypeManager.init: unable to create table ");
	for (size_t i = 0; i < numObjectTypes; i++)
		table[i] = nullptr;
	//---------------------------------------------------------------------------
	// Since MC1 handled all of these MiscTerrainObjectTypes with one
	// ObjectType, we set aside 5 separate typeHandles so they can be more
	// logically managed. If we never have to load MC1/MCX missions, we can
	// simply init the objectType packfile with 5 sep. types to begin with and
	// avoid this hack-like "fix"...
	bridgeTypeHandle = numObjectTypes - 5;
	forestTypeHandle = numObjectTypes - 4;
	wallHeavyTypeHandle = numObjectTypes - 3;
	wallMediumTypeHandle = numObjectTypes - 2;
	wallLightTypeHandle = numObjectTypes - 1;
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

void ObjectTypeManager::destroy(void)
{
	if (table)
	{
		objectTypeCache->Free(table);
		table = nullptr;
		numObjectTypes = 0;
	}
	if (objectFile)
	{
		objectFile->close();
		delete objectFile;
		objectFile = nullptr;
	}
	if (objectTypeCache)
	{
		delete objectTypeCache;
		objectTypeCache = nullptr;
	}
	if (objectCache)
	{
		delete objectCache;
		objectCache = nullptr;
	}
}

//---------------------------------------------------------------------------

ObjectTypePtr
ObjectTypeManager::load(ObjectTypeNumber objTypeNum, bool noCacheOut, bool forceLoad)
{
	//-----------------------------------------------------------------------
	// NOTE: This function attempts to load the objectType into the table. If
	// the object type is ALREADY loaded, it simply returns nullptr (indicating
	// it's already been loaded, so no problem). Otherwise, it returns the
	// newly loaded object type. What I'm say'n here is--this function
	// CANNOT fatal out, because any calling function will want to know
	// if the object type had to be loaded or not. However, it will fatal
	// if it's unable to load the object type from the packet file.
	//--------------------------------------------------------
	// If we are going to disk to get the object, be sure the
	// frame length knows to force itself into load mode.
	dynamicFrameTiming = false;
	if ((objTypeNum < 0) || (objTypeNum >= numObjectTypes))
		Fatal(objTypeNum, " ObjectTypeManager.load: bad objTypeNum ");
	if (objTypeNum == 0) // First Object always nullptr!
		return nullptr;
	if (!forceLoad && get(objTypeNum, false))
		return (nullptr);
	bool isMiscTerrObj = false;
	int32_t objectTypeNum = -1;
	ObjectTypePtr objType = nullptr;
	if ((objTypeNum == bridgeTypeHandle) || (objTypeNum == forestTypeHandle) || (objTypeNum == wallHeavyTypeHandle) || (objTypeNum == wallMediumTypeHandle) || (objTypeNum == wallLightTypeHandle))
	{
		//----------------------------------------------------------
		// MiscTerrainObject "hack" to maintain MC1 compatibility...
		objectTypeNum = TERRAINOBJECT_TYPE;
		isMiscTerrObj = true;
	}
	else if (objectFile->seekPacket(objTypeNum) == NO_ERROR)
	{
		if (objTypeNum == 268)
		{
			gosASSERT(objTypeNum == 268);
		}
		//--------------------------------------------------------
		// All new code here.  This will ask the objectType it is
		// loading what kind of objectType it is and create it
		// based on that instead of objTypeNum.
		FitIniFile objTypeFile;
		int32_t result = objTypeFile.open(objectFile, objectFile->getPacketSize());
		if (result != NO_ERROR)
			Fatal(objTypeNum, " ObjectTypeManager.load: can't create object ");
		result = objTypeFile.seekBlock("ObjectClass");
		if (result != NO_ERROR)
			Fatal(objTypeNum, " ObjectTypeManager.load: can't create object ");
		result = objTypeFile.readIdLong("ObjectTypeNum", objectTypeNum);
		if (result != NO_ERROR)
			Fatal(objTypeNum, " ObjectTypeManager.load: can't create object ");
		objTypeFile.close();
		objectFile->seekPacket(objTypeNum);
	}
	else
		Fatal(objTypeNum, " ObjectTypeManager.load: can't create object ");
	//-----------------------------------------------------
	// Now that we know what type it is, let's create it...
	switch (objectTypeNum)
	{
	case CRAPPY_OBJECT:
		//----------------------------------------------
		// In theory we can't ever get here.
		// Because we did our jobs correctly!!
		Fatal(CANT_LOAD_INVALID_OBJECT, " ObjectTypeManager.load: can't create object ");
		break;
	case BATTLEMECH_TYPE:
	{
		objType = new BattleMechType;
		objType->setObjTypeNum(objTypeNum);
		if (objType->init(objectFile, objectFile->getPacketSize()) != NO_ERROR)
			Fatal(objectTypeNum, " ObjectTypeManager.load: unable to init Mech type ");
	}
	break;
	case VEHICLE_TYPE:
	{
		objType = new GroundVehicleType;
		objType->setObjTypeNum(objTypeNum);
		if (objType->init(objectFile, objectFile->getPacketSize()) != NO_ERROR)
			Fatal(objectTypeNum, " ObjectTypeManager.load: unable to init Vehicle type ");
	}
	break;
	case TREEBUILDING_TYPE:
	case BUILDING_TYPE:
	{
		objType = new BuildingType;
		objType->setObjTypeNum(objTypeNum);
		if (objType->init(objectFile, objectFile->getPacketSize()) != NO_ERROR)
			Fatal(objectTypeNum, " ObjectTypeManager.load: unable to init Building type ");
	}
	break;
	case TREE_TYPE:
	{
		objType = new TerrainObjectType;
		objType->setObjTypeNum(objTypeNum);
		if (objType->init(objectFile, objectFile->getPacketSize()) != NO_ERROR)
			Fatal(objectTypeNum,
				" ObjectTypeManager.load: unable to init "
				"TerrainObject:Tree type ");
	}
	break;
	case TERRAINOBJECT_TYPE:
	{
		objType = new TerrainObjectType;
		objType->setObjTypeNum(objTypeNum);
		if (isMiscTerrObj)
			((TerrainObjectTypePtr)objType)->initMiscTerrObj(objTypeNum);
		else if (objType->init(objectFile, objectFile->getPacketSize()) != NO_ERROR)
			Fatal(objectTypeNum, " ObjectTypeManager.load: unable to init TerrainObject type ");
	}
	break;
	case WEAPONBOLT_TYPE:
	{
		objType = new WeaponBoltType;
		objType->setObjTypeNum(objTypeNum);
		if (objType->init(objectFile, objectFile->getPacketSize()) != NO_ERROR)
			Fatal(objectTypeNum, " ObjectTypeManager.load: unable to init WeaponBolt type ");
	}
	break;
	case TURRET_TYPE:
	{
		objType = new TurretType;
		objType->setObjTypeNum(objTypeNum);
		if (objType->init(objectFile, objectFile->getPacketSize()) != NO_ERROR)
			Fatal(objectTypeNum, " ObjectTypeManager.load: unable to init Turret type ");
	}
	break;
	case EXPLOSION_TYPE:
	{
		objType = new ExplosionType;
		objType->setObjTypeNum(objTypeNum);
		if (objType->init(objectFile, objectFile->getPacketSize()) != NO_ERROR)
			Fatal(objectTypeNum, " ObjectTypeManager.load: unable to init Explosion type ");
	}
	break;
	case FIRE_TYPE:
	{
		objType = new FireType;
		objType->setObjTypeNum(objTypeNum);
		if (objType->init(objectFile, objectFile->getPacketSize()) != NO_ERROR)
			Fatal(objectTypeNum, " ObjectTypeManager.load: unable to init Fire type ");
	}
	break;
	case GATE_TYPE:
	{
		objType = new GateType;
		objType->setObjTypeNum(objTypeNum);
		if (objType->init(objectFile, objectFile->getPacketSize()) != NO_ERROR)
			Fatal(objectTypeNum, " ObjectTypeManager.load: unable to init Gate type ");
	}
	break;
	case ARTILLERY_TYPE:
	{
		objType = new ArtilleryType;
		objType->setObjTypeNum(objTypeNum);
		if (objType->init(objectFile, objectFile->getPacketSize()) != NO_ERROR)
			Fatal(objectTypeNum, " ObjectTypeManager.load: unable to init Artillery type ");
	}
	break;
	default:
		// return(nullptr);
		// Fatal(OBJECT_TYPE_NUMBER_UNDEFINED, " ObjectTypeManager.load:
		// undefined objType ");
		NODEFAULT;
	}
	if (noCacheOut)
	{
		//---------------------------------------------------
		// Do NOT EVER cache this object type out.  FXs, etc.
		// This means I'm preloading it!!
		objType->makeLovable();
		if (objType->getExplosionObject() > 0)
			load(objType->getExplosionObject());
	}
	table[objTypeNum] = objType;
	return (objType);
}

//---------------------------------------------------------------------------

void ObjectTypeManager::remove(int32_t objTypeNum)
{
	if ((objTypeNum <= 0) || (objTypeNum >= numObjectTypes))
		Fatal(objTypeNum, " ObjectTypeManager.remove: bad objTypeNum ");
	if (table[objTypeNum])
	{
		table[objTypeNum]->removeUser();
		if (!table[objTypeNum]->inUse() && !table[objTypeNum]->lovable())
		{
			delete table[objTypeNum];
			table[objTypeNum] = nullptr;
		}
	}
}

//---------------------------------------------------------------------------

void ObjectTypeManager::remove(ObjectTypePtr objTypePtr)
{
	remove(objTypePtr->whatAmI());
}

//---------------------------------------------------------------------------

ObjectTypePtr
ObjectTypeManager::get(ObjectTypeNumber objTypeNum, bool loadIt)
{
	if ((objTypeNum < 0) || (objTypeNum >= numObjectTypes))
		Fatal(objTypeNum, " ObjectTypeManager.find: bad objTypeNum ");
	//---------------------------
	// If not, cache it in now...
	if (!table[objTypeNum] && loadIt)
	{
		if (!load(objTypeNum, true, true))
		{
			//			Fatal(objTypeNum, " ObjectTypeManager.get: unable to load
			// object type ");
		}
	}
	return (table[objTypeNum]);
}

//---------------------------------------------------------------------------

GameObjectPtr
ObjectTypeManager::create(ObjectTypeNumber objTypeNum)
{
	if ((objTypeNum < 0) || (objTypeNum >= numObjectTypes))
		Fatal(objTypeNum, " ObjectTypeManager.get: bad objTypeNum ");
	//----------------------------------------------------------
	// First, check if the objectType has already been loaded...
	ObjectTypePtr objType = get(objTypeNum);
	//---------------------------
	// If not, cache it in now...
	if (!objType)
		objType = load(objTypeNum);
	//--------------------------------------------------------------------
	// By now, it should be cached in (otherwise, some error has occured).
	// Now, create an instance of it and send it back (after we increment
	// the number of gameobjects of this type)...
	if (objType)
	{
		GameObjectPtr obj = objType->createInstance();
		if (!obj)
			Fatal(objTypeNum, " ObjectTypeManager.create: unable to create instance ");
		objType->addUser();
		return (obj);
	}
	Fatal(objTypeNum, " ObjectTypeManager.create: unable to load object type ");
	return (nullptr);
}

//***************************************************************************
