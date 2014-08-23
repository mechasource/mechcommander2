//******************************************************************************************
//
//	Radio.h -- File contains the Radio Definition
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef RADIO_H
#define RADIO_H

//#include <mclib.h>
//#include "dradio.h"
//#include "dwarrior.h"
//#include "dgameobj.h"

//------------------------------------------------------------------------------------------
#define MAX_FRAGMENTS	16
#define MAX_RADIOS		256

struct RadioData
{
	uint32_t	msgId;
	uint32_t	msgType;
	uint32_t	noiseId;
	int32_t			numFragments;
	puint8_t		data[MAX_FRAGMENTS];
	int32_t			dataSize[MAX_FRAGMENTS];
	puint8_t		noise[MAX_FRAGMENTS];
	int32_t			noiseSize[MAX_FRAGMENTS];
	UserHeapPtr		msgHeap;
	uint32_t	turnQueued;
	byte			priority;
	byte			movieCode;
	float			expirationDate;
	MechWarriorPtr	pilot;
};

struct RadioMessageInfo
{
	byte			priority;
	float			shelfLife;
	char			movieCode;
	byte			styleCount;
	byte			styleChance[3];
	uint32_t			messageMapping;
	bool			pilotIdentifiesSelf;
	bool			kludgeStyle;
};

//------------------------------------------------------------------------------------------
class Radio
{
	//Data Members
	//-------------
	protected:
	
		MechWarriorPtr	owner;
		bool			enabled;
		bool			ammoOutPlayed;
		int32_t			radioID;

	public:		
		static PacketFilePtr 	noiseFile;
		static RadioPtr			radioList[MAX_RADIOS];		//Warriors no longer delete their radios.  We do when the
															//SoundSystem Shutdown occurs.

		static PacketFilePtr	messagesFile[MAX_RADIOS];	//Do these when the shutdown occurs too to avoid leaks
														
		static bool				messageInfoLoaded;
		static bool				radioListGo;
		static int32_t				currentRadio;
		static UserHeapPtr		radioHeap;				//Only one Heap Per Game!!!
	
	//Member Functions
	//-----------------
	public:
	
		PVOID operator new (size_t mySize);
		void operator delete (PVOID us);

		void init (void)
		{
			enabled = TRUE;
			ammoOutPlayed = false;
			radioID = -1;
		}

		void destroy (void)
		{
		}

		Radio (void)
		{
			init();
		}
	
		~Radio (void)
		{
			destroy();
		}
	
		int32_t init (PSTR fileName, uint32_t heapSize, PSTR movie);

		void setOwner (MechWarriorPtr _owner)
		{
			owner = _owner;
		}
				
		int32_t playMessage (RadioMessageType msgId);

		void cancelMessage (RadioMessageType msgId);
		
		void turnOn (void) {
			enabled = TRUE;
		}

		void turnOff (void) {
			enabled = FALSE;
		}
		
		int32_t loadMessageInfo(void);

		void resetAmmoMessage (void)
		{
			ammoOutPlayed = false;
		}
};

//------------------------------------------------------------------------------------------
#endif