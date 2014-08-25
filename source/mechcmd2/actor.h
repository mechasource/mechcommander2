//---------------------------------------------------------------------------
//
//	actor.h - This file contains the header for the Actor class
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef ACTOR_H
#define ACTOR_H

//#include <mclib.h>
//#include "appear.h"
//#include "apprtype.h"

//#ifndef TGATXM_H
//#include "tgatxm.h"
//#endif


//***********************************************************************
// Macro Definitions

#define MAX_TREES		20

enum ActorState
{
	ACTOR_STATE_INVALID = -1,
	ACTOR_STATE_NORMAL = 0,
	ACTOR_STATE_BLOWING_UP1,		//From Normal to Damaged 
	ACTOR_STATE_DAMAGED,
	ACTOR_STATE_BLOWING_UP2,		//From Damaged to Destroyed 
	ACTOR_STATE_DESTROYED,
	ACTOR_STATE_FALLEN_DMG,			//Used for Trees
	MAX_ACTOR_STATES
};

struct ActorData
{
	ActorState			state;
	uint8_t 		symmetrical;					// are second-half rotations flip versions of first half?
	uint8_t		numRotations;					// number of rotations (including flips)
	uint32_t		numFrames;						// number of frames for this gesture (if -1, does not exist)
	uint32_t		basePacketNumber;				// Where in packet file does this gesture start.
	float				frameRate;						// intended frame rate of playback
	int32_t				textureSize;					// Length of one edge of texture.
	int32_t				textureHS;						// Where the screen coord should go for texture.
};

//***********************************************************************
//
// VFXAppearanceType
//
//***********************************************************************

//-----------------------------------------------------------------------
class VFXAppearanceType : public AppearanceType
{
	public:
	
		ActorData			*actorStateData;
		TGATexturePtr		*textureList;				//These go NULL when a texture is cached out.
		int32_t				numPackets;
		uint32_t				textureMemoryHandle;
		uint8_t		numStates;

	public:
	
		void init (void)
		{
			actorStateData = NULL;
			textureList = NULL;
			numStates = 0;
			numPackets = 0;

			users = NULL;
			lastUser = NULL;
		}
	
		VFXAppearanceType (void)
		{
			init(void);
		}

		~VFXAppearanceType (void)
		{
			destroy(void);
		}

		void init (FilePtr appearFile, uint32_t fileSize);
		
		//----------------------------------------------
		// This routine is where the magic happens.
		TGATexturePtr getTexture (ActorState shapeId, int32_t rot, int32_t currFrame, float &frameRate, bool &mirror);

		int32_t loadIniFile (FilePtr appearFile, uint32_t fileSize);

		void destroy (void);

		virtual void removeTexture (TGATexture *shape);

		int32_t getNumFrames (ActorState typeId)
		{
			if (actorStateData)
				return actorStateData[typeId].numFrames;
				
			return 0;
		}
};

//***********************************************************************
//
// VFXAppearance
//
//***********************************************************************

//-----------------------------------------------------------------------
class VFXAppearance : public Appearance
{
	public:

		VFXAppearanceType*			appearType;
		TGATexturePtr				currentTexture;		//OK because we make sure each frame before we draw it.
		uint32_t				currentFrame;
		float						currentRotation;
		
		float						lastInView;						//Time since last in view (s)
		float						timeInFrame;
		float						frameInc;
		int32_t						lastWholeFrame;
		
		uint32_t				startFrame;
		uint32_t				endFrame;
			
		ActorState					currentShapeTypeId;
		puint8_t 					fadeTable;
		bool 						realBuildingDamage;
		bool						changedTypeId;
		
		float						lightIntensity;
		float						topZ;
		
		Stuff::Vector2DOf<int32_t>		shapeMin;
		Stuff::Vector2DOf<int32_t>		shapeMax;

		Stuff::Vector3D				position;
		float						rotation;
		int32_t						selected;
		int32_t						alignment;
		
	public:

		virtual void init (AppearanceTypePtr tree = NULL, GameObjectPtr obj = NULL);

		VFXAppearance (void)
		{
			init(void);
		}

		virtual int32_t update (void);
		virtual int32_t render (int32_t depthFixup = 0);

		virtual void destroy (void);

		~VFXAppearance (void)
		{
			destroy(void);
		}

		virtual AppearanceTypePtr getAppearanceType (void)
		{
			return appearType;
		}

		virtual bool recalcBounds (void);
		
		virtual void setTypeId (ActorState typeId)
		{
			if ((typeId < appearType->numStates) && (typeId >= 0))
			{
				currentShapeTypeId = typeId;
				currentFrame = -1;
				timeInFrame = 0.0;
				lastWholeFrame = 0;
			}
			
			changedTypeId = TRUE;
		}

		void loopFrames (int32_t sFrame, int32_t eFrame)
		{
			startFrame = sFrame;
			endFrame = eFrame;
		}
		
		virtual void setDamageLvl (uint32_t damage);
		
		void setFadeTable (puint8_t fTable)
		{
			fadeTable = fTable;
		}
		
		void setObjectParameters (Stuff::Vector3D &pos, float rot, int32_t selected);
		
		int32_t stateExists (ActorState typeId);
		
		void debugUpdate (void);
		
		virtual bool isMouseOver (float px, float py);
};


//***********************************************************************
#endif


