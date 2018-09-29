//***************************************************************************
//
//	Light.h -- File contains the Explosion Object Definition
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef LIGHT_H
#define LIGHT_H

//---------------------------------------------------------------------------

//#include "dcarnage.h"
//#include "gameobj.h"
//#include "objmgr.h"
//#include "objtype.h"

//---------------------------------------------------------------------------
/*
#define NO_APPEARANCE_TYPE_FOR_EXPL		0xDCDC0003
#define NO_APPEARANCE_FOR_EXPL			0xDCDC0004
#define APPEARANCE_NOT_VFX_XPL			0xDCDC0005
*/
//---------------------------------------------------------------------------

class LightType : public ObjectType
{

public:
	bool oneShotFlag;
	float altitudeOffset;

public:
	void init(void) { ObjectType::init(void); }

	LightType(void) { init(void); }

	virtual int32_t init(FilePtr objFile, uint32_t fileSize);
	int32_t init(FitIniFilePtr objFile);

	~LightType(void) { destroy(void); }

	virtual void destroy(void);
	virtual GameObjectPtr createInstance(void);
	virtual bool handleCollision(GameObjectPtr collidee, GameObjectPtr collider);
	virtual bool handleDestruction(GameObjectPtr collidee, GameObjectPtr collider);
};

//---------------------------------------------------------------------------

class Light : public GameObject
{

public:
	virtual void init(bool create);

	Light(void) : GameObject() { init(true); }

	~Light(void) { destroy(void); }

	virtual void destroy(void);
	virtual int32_t update(void);
	virtual void render(void);
	virtual void init(bool create, ObjectTypePtr _type);
	virtual int32_t kill(void) { return (NO_ERROR); }
};

//***************************************************************************

#endif
