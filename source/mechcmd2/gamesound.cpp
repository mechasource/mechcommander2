//---------------------------------------------------------------------------
// GameSound.cpp - This file is the sound system code for the GAME
//
// MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"

//---------------------------------------------------------------------------
// Include files
#include "gamesound.h"
#ifndef WARRIOR_H
#include "warrior.h"
#endif

#ifndef VIEWER
#ifndef CONTROLGUI_H
#include "controlgui.h"
#endif
#endif

//---------------------------------------------------------------------------
GameSoundSystem* soundSystem = nullptr;

extern bool wave_ParseWaveMemory(puint8_t lpChunkOfMemory, MC2_WAVEFORMATEX** lplpWaveHeader,
	puint8_t* lplpWaveSamples, uint32_t* lpcbWaveSize);

bool friendlyDestroyed = false;
bool enemyDestroyed	= false;
extern bool GeneralAlarm;

#define ALARM_TIME 5.0f

//---------------------------------------------------------------------------
void GameSoundSystem::purgeSoundSystem(void)
{
	wholeMsgDone	= true;
	currentMessage  = nullptr;
	messagesInQueue = 0;
	currentFragment = 0;
	playingNoise	= false;
	radioHandle		= nullptr;
	//------------------------------------------------------------
	// dump the Radio Message Queue.
	messagesInQueue = 0;
	wholeMsgDone	= true;
	int32_t i;
	for (i = 0; i < MAX_QUEUED_MESSAGES; i++)
		queue[i] = nullptr;
	generalAlarmTimer = 0.0f;
	//---------------------------------------------------------
	// Toss everybody's radio.  This MUST happen AFTER
	// all of the currently queued radio messages are dumped.
	// Radios all come from the Radio Heap now.  This dumps 'em.
	for (i = 0; i < MAX_RADIOS; i++)
	{
		Radio::radioList[i] = nullptr;
		delete Radio::messagesFile[i];
		Radio::messagesFile[i] = nullptr;
	}
	delete Radio::radioHeap;
	Radio::radioHeap = nullptr;
	delete Radio::noiseFile;
	Radio::noiseFile		 = nullptr;
	Radio::currentRadio		 = 0;
	Radio::radioListGo		 = FALSE;
	Radio::messageInfoLoaded = FALSE;
	isRaining = oldRaining = false;
	largestSensorContact   = -1;
	//-----------------------------------------------
	// Check all samples to see if one should end.
	for (i = 0; i < MAX_DIGITAL_SAMPLES; i++)
	{
		if (i != BETTY_CHANNEL)
		{
			gosAudio_SetChannelPlayMode(i, gosAudio_Stop);
			fadeDown[i]		   = FALSE;
			channelSampleId[i] = -1;
			channelInUse[i]	= FALSE;
		}
	}
}

//---------------------------------------------------------------------------
void GameSoundSystem::removeQueuedMessage(int32_t msgNumber)
{
	int32_t i;
	if (msgNumber < 0 || msgNumber >= MAX_QUEUED_MESSAGES)
		return;
	if (queue[msgNumber])
	{
		if (queue[msgNumber]->pilot)
			queue[msgNumber]->pilot->clearMessagePlaying();
		UserHeapPtr msgHeap = queue[msgNumber]->msgHeap;
		for (i = 0; i < MAX_FRAGMENTS; i++)
		{
			msgHeap->Free(queue[msgNumber]->data[i]);
			queue[msgNumber]->data[i] = nullptr;
			msgHeap->Free(queue[msgNumber]->noise[i]);
			queue[msgNumber]->noise[i] = nullptr;
		}
		msgHeap->Free(queue[msgNumber]);
		if (messagesInQueue > 0)
			messagesInQueue--;
#ifdef _DEBUG
		for (size_t test = 0; test < (int32_t)messagesInQueue; test++)
		{
			if (queue[test])
				continue;
			else
				Fatal(-1, " Bad Message in Queue -- RmoveQMsg");
		}
#endif
		for (i = msgNumber; i < MAX_QUEUED_MESSAGES - 1; i++)
			queue[i] = queue[i + 1];
		queue[MAX_QUEUED_MESSAGES - 1] = nullptr;
#ifdef _DEBUG
		for (test = 0; test < (int32_t)messagesInQueue; test++)
		{
			if (queue[test])
				continue;
			else
				Fatal(-1, " Bad Message in Queue -- RmoveQMsg");
		}
#endif
	}
}

//---------------------------------------------------------------------------
bool GameSoundSystem::checkMessage(MechWarriorPtr pilot, byte priority, uint32_t messageType)
{
	for (size_t i = 0; i < MAX_QUEUED_MESSAGES; i++)
	{
		if (queue[i])
			if ((queue[i]->pilot == pilot && priority > queue[i]->priority) || // I'm already saying
																			   // something more
																			   // important, or
				(queue[i]->priority > 1 &&
					queue[i]->msgType == messageType)) // someone else is saying this
				return FALSE;
	}
	return TRUE;
}

//---------------------------------------------------------------------------
void GameSoundSystem::moveFromQueueToPlaying(void)
{
	removeCurrentMessage();
	while (queue[0] && queue[0]->pilot && !(queue[0]->pilot->active()) &&
		(queue[0]->msgType != RADIO_DEATH) && (queue[0]->msgType != RADIO_EJECTING))
	{
		removeQueuedMessage(0);
	}
	currentMessage = queue[0];
	for (size_t i = 0; i < MAX_QUEUED_MESSAGES - 1; i++)
		queue[i] = queue[i + 1];
	queue[MAX_QUEUED_MESSAGES - 1] = nullptr;
	if (messagesInQueue > 0)
		messagesInQueue--;
}

//---------------------------------------------------------------------------
void GameSoundSystem::removeCurrentMessage(void)
{
	if (currentMessage)
	{
		if (currentMessage->pilot)
			currentMessage->pilot->clearMessagePlaying();
		UserHeapPtr msgHeap = currentMessage->msgHeap;
		for (size_t j = 0; j < MAX_FRAGMENTS; j++)
		{
			msgHeap->Free(currentMessage->data[j]);
			currentMessage->data[j] = nullptr;
			msgHeap->Free(currentMessage->noise[j]);
			currentMessage->noise[j] = nullptr;
		}
		msgHeap->Free(currentMessage);
		currentMessage = nullptr;
	}
	gosAudio_SetChannelPlayMode(RADIO_CHANNEL, gosAudio_Stop);
	wholeMsgDone = TRUE;
}

//---------------------------------------------------------------------------
int32_t GameSoundSystem::queueRadioMessage(RadioData* msgData)
{
	int32_t i;
	//-------------------------------------------------
	// First, search the Queue and see if this message
	// was already sent this turn.
	if (msgData->msgId >= MSG_TOTAL_MSGS) // is this a real message? (why are we asking this???)
	{
		for (i = MAX_QUEUED_MESSAGES - 1; i >= 0; i--)
		{
			if (queue[i])
			{
				if ((msgData->turnQueued == queue[i]->turnQueued) &&
					(msgData->msgId == queue[i]->msgId))
				{
					removeQueuedMessage(i);
				}
			}
		}
	}
	if (msgData->priority == 0) // top cancels playing message! top also means pilot's gone, so
	{
		// remove other messages from that pilot.
		removeCurrentMessage();
		for (i = MAX_QUEUED_MESSAGES - 1; i >= 0; i--)
		{
			if (queue[i] && queue[i]->pilot == msgData->pilot)
			{
				removeQueuedMessage(i);
			}
		}
	}
	for (i = 0; i < MAX_QUEUED_MESSAGES; i++)
	{
		if (!queue[i])
			break;
		if (queue[i]->priority > msgData->priority) // if this messages priority
													// higher (a lower number: 1
													// is top priority)
		{
			// push things down to make room.
			for (size_t j = MAX_QUEUED_MESSAGES - 1; j > i; j--)
			{
				queue[j] = queue[j - 1];
			}
			break;
		}
	}
	if (i == MAX_QUEUED_MESSAGES)
		return (-1);
	if (queue[MAX_QUEUED_MESSAGES - 1])
		removeQueuedMessage(MAX_QUEUED_MESSAGES - 1);
	queue[i] = msgData;
#ifdef _DEBUG
	for (size_t test = 0; test < (int32_t)messagesInQueue; test++)
	{
		if (queue[test])
			continue;
		else
			Fatal(-1, " Bad Message in Queue -- RmoveQMsg");
	}
#endif
	messagesInQueue++;
#ifdef _DEBUG
	for (test = 0; test < (int32_t)messagesInQueue; test++)
	{
		if (queue[test])
			continue;
		else
			Fatal(-1, " Bad Message in Queue -- RmoveQMsg");
	}
#endif
	return NO_ERROR;
}

//---------------------------------------------------------------------------
void GameSoundSystem::update(void)
{
	//---------------------------------------------
	// Dynamic Music Code goes here!!!
	// New System(tm)
	//
	SoundSystem::update();
	if (GeneralAlarm && (generalAlarmTimer < ALARM_TIME))
	{
		playDigitalSample(BUZZER1);
		generalAlarmTimer += frameLength;
	}
	if (useSound && currentMessage &&
		(gosAudio_GetChannelPlayMode(RADIO_CHANNEL) != gosAudio_PlayOnce))
	{
		if (radioHandle)
		{
			gosAudio_DestroyResource(&radioHandle); // Toss the current radio
													// data which just completed!
			radioHandle = nullptr;
		}
		if (!wholeMsgDone)
		{
			//---------------------------------------------------------------
			// We are finished with a fragment, move on.
			if (playingNoise)
			{
				playingNoise = FALSE;
				if (currentMessage->data[currentFragment])
				{
					//--------------------------------------------------------------------
					// Hand GOS sound the data it needs to create the resource
					// Handle
					gosAudio_Format soundFormat;
					soundFormat.wFormatTag		 = 1; // PCM
					MC2_WAVEFORMATEX* waveFormat = nullptr;
					puint8_t dataOffset			 = nullptr;
					uint32_t length				 = 0;
					uint32_t bitsPerSec			 = 0;
					wave_ParseWaveMemory(
						currentMessage->data[currentFragment], &waveFormat, &dataOffset, &length);
					bitsPerSec			  = waveFormat->nBlockAlign / waveFormat->nChannels * 8;
					soundFormat.nChannels = waveFormat->nChannels;
					soundFormat.nSamplesPerSec  = waveFormat->nSamplesPerSec;
					soundFormat.nAvgBytesPerSec = waveFormat->nAvgBytesPerSec;
					soundFormat.nBlockAlign		= waveFormat->nBlockAlign;
					soundFormat.wBitsPerSample  = bitsPerSec;
					soundFormat.cbSize			= 0;
					gosAudio_CreateResource(&radioHandle, gosAudio_UserMemory, nullptr,
						&soundFormat, dataOffset, length);
					if (isChannelPlaying(BETTY_CHANNEL))
						gosAudio_SetChannelSlider(RADIO_CHANNEL, gosAudio_Volume,
							SoundSystem::digitalMasterVolume * radioVolume * 0.5f);
					else
						gosAudio_SetChannelSlider(RADIO_CHANNEL, gosAudio_Volume,
							SoundSystem::digitalMasterVolume * radioVolume);
					gosAudio_AssignResourceToChannel(RADIO_CHANNEL, radioHandle);
					gosAudio_SetChannelPlayMode(RADIO_CHANNEL, gosAudio_PlayOnce);
					if (currentMessage->pilot)
						currentMessage->pilot->setMessagePlaying();
				}
				else
					wholeMsgDone = TRUE;
				currentFragment++;
			}
			else
			{
				if (currentMessage->noise[currentFragment])
				{
					//--------------------------------------------------------------------
					// Hand GOS sound the data it needs to create the resource
					// Handle
					gosAudio_Format soundFormat;
					soundFormat.wFormatTag		 = 1; // PCM
					MC2_WAVEFORMATEX* waveFormat = nullptr;
					puint8_t dataOffset			 = nullptr;
					uint32_t length				 = 0;
					uint32_t bitsPerSec			 = 0;
					wave_ParseWaveMemory(
						currentMessage->noise[currentFragment], &waveFormat, &dataOffset, &length);
					bitsPerSec			  = waveFormat->nBlockAlign / waveFormat->nChannels * 8;
					soundFormat.nChannels = waveFormat->nChannels;
					soundFormat.nSamplesPerSec  = waveFormat->nSamplesPerSec;
					soundFormat.nAvgBytesPerSec = waveFormat->nAvgBytesPerSec;
					soundFormat.nBlockAlign		= waveFormat->nBlockAlign;
					soundFormat.wBitsPerSample  = bitsPerSec;
					soundFormat.cbSize			= 0;
					gosAudio_CreateResource(&radioHandle, gosAudio_UserMemory, nullptr,
						&soundFormat, dataOffset, length);
					if (isChannelPlaying(BETTY_CHANNEL))
						gosAudio_SetChannelSlider(RADIO_CHANNEL, gosAudio_Volume,
							SoundSystem::digitalMasterVolume * radioVolume * 0.5f);
					else
						gosAudio_SetChannelSlider(RADIO_CHANNEL, gosAudio_Volume,
							SoundSystem::digitalMasterVolume * radioVolume);
					gosAudio_AssignResourceToChannel(RADIO_CHANNEL, radioHandle);
					gosAudio_SetChannelPlayMode(RADIO_CHANNEL, gosAudio_PlayOnce);
					playingNoise = TRUE;
					if (currentMessage->pilot)
						currentMessage->pilot->setMessagePlaying();
				}
				else
				{
					playingNoise = FALSE;
					if (currentMessage->data[currentFragment])
					{
						//--------------------------------------------------------------------
						// Hand GOS sound the data it needs to create the
						// resource Handle
						gosAudio_Format soundFormat;
						soundFormat.wFormatTag		 = 1; // PCM
						MC2_WAVEFORMATEX* waveFormat = nullptr;
						puint8_t dataOffset			 = nullptr;
						uint32_t length				 = 0;
						uint32_t bitsPerSec			 = 0;
						wave_ParseWaveMemory(currentMessage->data[currentFragment], &waveFormat,
							&dataOffset, &length);
						bitsPerSec			  = waveFormat->nBlockAlign / waveFormat->nChannels * 8;
						soundFormat.nChannels = waveFormat->nChannels;
						soundFormat.nSamplesPerSec  = waveFormat->nSamplesPerSec;
						soundFormat.nAvgBytesPerSec = waveFormat->nAvgBytesPerSec;
						soundFormat.nBlockAlign		= waveFormat->nBlockAlign;
						soundFormat.wBitsPerSample  = bitsPerSec;
						soundFormat.cbSize			= 0;
						gosAudio_CreateResource(&radioHandle, gosAudio_UserMemory, nullptr,
							&soundFormat, dataOffset, length);
						if (isChannelPlaying(BETTY_CHANNEL))
							gosAudio_SetChannelSlider(RADIO_CHANNEL, gosAudio_Volume,
								SoundSystem::digitalMasterVolume * radioVolume * 0.5f);
						else
							gosAudio_SetChannelSlider(RADIO_CHANNEL, gosAudio_Volume,
								SoundSystem::digitalMasterVolume * radioVolume);
						gosAudio_AssignResourceToChannel(RADIO_CHANNEL, radioHandle);
						gosAudio_SetChannelPlayMode(RADIO_CHANNEL, gosAudio_PlayOnce);
						if (currentMessage->pilot)
							currentMessage->pilot->setMessagePlaying();
					}
					else
						wholeMsgDone = TRUE;
					currentFragment++;
				}
			}
		}
		else // wholeMessageDone
		{
			if (currentMessage)
			{
				if (currentMessage->pilot)
				{
					currentMessage->pilot->clearMessagePlaying();
#ifndef VIEWER
					ControlGui::instance->endPilotVideo();
#endif
				}
				removeCurrentMessage();
			}
		}
	}
	if (useSound && messagesInQueue && wholeMsgDone)
	{
		if (radioHandle)
		{
			gosAudio_DestroyResource(&radioHandle); // Toss the current radio
													// data which just completed!
			radioHandle = nullptr;
		}
		//-----------------------------------------------------
		// We are done with the current sample, start the next
		currentFragment = 0;
		moveFromQueueToPlaying();
		if (!currentMessage && !messagesInQueue) // It is now possible for ALL
												 // remaining messages to go away
												 // because the pilot/mech died!
			return;
#ifndef VIEWER
		if (currentMessage->movieCode)
		{
			ControlGui::instance->playPilotVideo(currentMessage->pilot, currentMessage->movieCode);
		}
#endif
		if (currentMessage->noise[currentFragment])
		{
			//--------------------------------------------------------------------
			// Hand GOS sound the data it needs to create the resource Handle
			gosAudio_Format soundFormat;
			soundFormat.wFormatTag		 = 1; // PCM
			MC2_WAVEFORMATEX* waveFormat = nullptr;
			puint8_t dataOffset			 = nullptr;
			uint32_t length				 = 0;
			uint32_t bitsPerSec			 = 0;
			wave_ParseWaveMemory(
				currentMessage->noise[currentFragment], &waveFormat, &dataOffset, &length);
			bitsPerSec					= waveFormat->nBlockAlign / waveFormat->nChannels * 8;
			soundFormat.nChannels		= waveFormat->nChannels;
			soundFormat.nSamplesPerSec  = waveFormat->nSamplesPerSec;
			soundFormat.nAvgBytesPerSec = waveFormat->nAvgBytesPerSec;
			soundFormat.nBlockAlign		= waveFormat->nBlockAlign;
			soundFormat.wBitsPerSample  = bitsPerSec;
			soundFormat.cbSize			= 0;
			gosAudio_CreateResource(
				&radioHandle, gosAudio_UserMemory, nullptr, &soundFormat, dataOffset, length);
			if (isChannelPlaying(BETTY_CHANNEL))
				gosAudio_SetChannelSlider(RADIO_CHANNEL, gosAudio_Volume,
					SoundSystem::digitalMasterVolume * radioVolume * 0.5f);
			else
				gosAudio_SetChannelSlider(
					RADIO_CHANNEL, gosAudio_Volume, SoundSystem::digitalMasterVolume * radioVolume);
			gosAudio_AssignResourceToChannel(RADIO_CHANNEL, radioHandle);
			gosAudio_SetChannelPlayMode(RADIO_CHANNEL, gosAudio_PlayOnce);
			playingNoise = TRUE;
			if (currentMessage->pilot)
				currentMessage->pilot->setMessagePlaying();
		}
		else
		{
			playingNoise = FALSE;
			if (currentMessage->data[currentFragment])
			{
				//--------------------------------------------------------------------
				// Hand GOS sound the data it needs to create the resource
				// Handle
				gosAudio_Format soundFormat;
				soundFormat.wFormatTag		 = 1; // PCM
				MC2_WAVEFORMATEX* waveFormat = nullptr;
				puint8_t dataOffset			 = nullptr;
				uint32_t length				 = 0;
				uint32_t bitsPerSec			 = 0;
				wave_ParseWaveMemory(
					currentMessage->data[currentFragment], &waveFormat, &dataOffset, &length);
				bitsPerSec					= waveFormat->nBlockAlign / waveFormat->nChannels * 8;
				soundFormat.nChannels		= waveFormat->nChannels;
				soundFormat.nSamplesPerSec  = waveFormat->nSamplesPerSec;
				soundFormat.nAvgBytesPerSec = waveFormat->nAvgBytesPerSec;
				soundFormat.nBlockAlign		= waveFormat->nBlockAlign;
				soundFormat.wBitsPerSample  = bitsPerSec;
				soundFormat.cbSize			= 0;
				gosAudio_CreateResource(
					&radioHandle, gosAudio_UserMemory, nullptr, &soundFormat, dataOffset, length);
				if (isChannelPlaying(BETTY_CHANNEL))
					gosAudio_SetChannelSlider(RADIO_CHANNEL, gosAudio_Volume,
						SoundSystem::digitalMasterVolume * radioVolume * 0.5f);
				else
					gosAudio_SetChannelSlider(RADIO_CHANNEL, gosAudio_Volume,
						SoundSystem::digitalMasterVolume * radioVolume);
				gosAudio_AssignResourceToChannel(RADIO_CHANNEL, radioHandle);
				gosAudio_SetChannelPlayMode(RADIO_CHANNEL, gosAudio_PlayOnce);
				if (currentMessage->pilot)
					currentMessage->pilot->setMessagePlaying();
			}
		}
		wholeMsgDone = FALSE;
	}
}
