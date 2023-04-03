//---------------------------------------------------------------------------
// GameSound.h - This file is the sound system header for the GAME
//
// MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef GAMESOUND_H
#define GAMESOUND_H

//#include "mclib.h"
//#include "dwarrior.h"
//#include "radio.h"

//---------------------------------------------------------------------------
// Macro Defintiions

//---------------------------------------------------------------------------
class GameSoundSystem : public SoundSystem
{
	// Data Members
	//-------------
protected:
	RadioData* currentMessage; // Radio message playing.
	uint32_t messagesInQueue; // Radio messages waiting to play.
	RadioData* queue[MAX_QUEUED_MESSAGES]; // Radio message queue.
	uint32_t currentFragment; // Which piece are we playing.
	uint32_t playingNoise; // are we playing noise right now?
	bool wholeMsgDone; // Are all fragments played?
	HGOSAUDIO radioHandle;

	float generalAlarmTimer; // How int32_t do we play the alarm buzzer?

	// Member Functions
	//----------------
public:
	GameSoundSystem(void)
	{
		init(void);
	}

	~GameSoundSystem(void)
	{
		destroy(void);
	}

	void init(void)
	{
		SoundSystem::init(void);
		wholeMsgDone = true;
		currentMessage = nullptr;
		messagesInQueue = 0;
		currentFragment = 0;
		playingNoise = false;
		radioHandle = nullptr;
		//------------------------------------------------------------
		// Startup the Radio message Queue.
		messagesInQueue = 0;
		wholeMsgDone = true;
		for (size_t i = 0; i < MAX_QUEUED_MESSAGES; i++)
			queue[i] = nullptr;
		generalAlarmTimer = 0.0f;
	}

	virtual void update(void);

	void purgeSoundSystem(void); // This will shutdown all active sound.
	void removeQueuedMessage(int32_t msgNumber);
	bool checkMessage(std::unique_ptr<MechWarrior> pilot, byte priority, uint32_t messageType);
	int32_t queueRadioMessage(RadioData* msgData);
	void moveFromQueueToPlaying(void);
	void removeCurrentMessage(void);
};

//---------------------------------------------------------------------------
extern GameSoundSystem* soundSystem;

extern bool useSound;
extern bool useMusic;

//---------------------------------------------------------------------------
#endif
