#ifndef OBJECTAPPEARANCE_H
#define OBJECTAPPEARANCE_H
/*************************************************************************************************\
ObjectAppearance.h	: Interface for the ObjectAppearance component, building and tree appearance
							keep their common elements here
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef APPEARANCE_H
#include "Appear.h"
#endif

#ifndef DSTD_H
#include "dstd.h"
#endif

#ifndef OBJSTATUS_H
#include "objstatus.h"
#endif

//*************************************************************************************************

// WEAPON TYPES
#define MECH3D_WEAPONTYPE_NONE				-1
#define MECH3D_WEAPONTYPE_ANY				0
#define MECH3D_WEAPONTYPE_MISSILE			1
#define MECH3D_WEAPONTYPE_BALLISTIC			2
#define MECH3D_WEAPONTYPE_NONENERGY			3
#define MECH3D_WEAPONTYPE_ENERGY			4
#define MECH3D_WEAPONTYPE_DIRECT			5
// NOTES:
// Each mech MUST have a place for each kind of weapon to fire.
// I.e. There must be at least one zero OR at least a 3 and 4 OR at least a 1, 2 and 4 or its an ERROR!!!!!!!!

typedef struct _NodeData
{
	char*			 nodeId;				//Used to determine where jumpjets, smoke and weapon fire come from.
	int32_t			weaponType;				//What kind of weapon can use this node
	bool			isRArmNode;				//Used to stop firing from node when RArm is gone.
	bool			isLArmNode;				//Used to stop firing from node when LArm is gone.
} NodeData;

/**************************************************************************************************
CLASS DESCRIPTION
ObjectAppearance:
**************************************************************************************************/
class ObjectAppearance: public Appearance
{
public:

	float										lightIntensity;

	Stuff::Vector2DOf<int32_t>						shapeMin;
	Stuff::Vector2DOf<int32_t>						shapeMax;

	Stuff::Vector3D								position;
	float										rotation;
	int32_t										selected; // actually a bit field...
	int32_t										teamId;
	int32_t										homeTeamRelationship;
	float										actualRotation;
	int32_t										objectNameId;
	int32_t										damage;
	int32_t										pilotNameID;
	char										pilotName[32];


	int32_t										paintScheme;
	puint8_t									fadeTable;

	void setDamage(int32_t Damage)   // editor uses this... nobody else should
	{
		damage = Damage;
		barStatus = Damage ? 0 : 1;
		if(Damage)
			setObjStatus(OBJECT_STATUS_DESTROYED);		//Change the shape!!
		else
			setObjStatus(OBJECT_STATUS_NORMAL);		//Change the shape!!
	}

	virtual void setHighlightColor(int32_t argb)
	{
		highlightColor = argb;
	}

	virtual int32_t getObjectNameId()
	{
		return objectNameId;
	}

	virtual ~ObjectAppearance() {}

protected:

	int32_t	highlightColor;
	ObjectAppearance()
	{
		highlightColor = 0;
		damage = 0;
		lightIntensity = rotation = 0.0;
		selected = 0;
		pilotName[0] = 0;
	}


	/*
	private:

	ObjectAppearance( const ObjectAppearance& bjectAppearance );
	ObjectAppearance& operator=( const ObjectAppearance& bjectAppearance );
	*/


};


//*************************************************************************************************
#endif  // end of file ( ObjectAppearance.h )
