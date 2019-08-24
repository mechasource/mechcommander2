//---------------------------------------------------------------------------
// soundsys.cpp - This file is the sound system code
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//---------------------------------------------------------------------------
// Include files
#ifndef SOUNDSYS_H
#include "soundsys.h"
#endif

#ifndef SOUNDS_H
#include "sounds.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

//---------------------------------------------------------------------------
// static globals
bool useSound = TRUE;
bool useMusic = TRUE;

int32_t DigitalMasterVolume = 0;
int32_t MusicVolume = 0;
int32_t sfxVolume = 0;
int32_t RadioVolume = 0;
int32_t BettyVolume = 0;

#define MAX_SENSOR_UPDATE_TIME (5.0f) // Seconds

float SoundSystem::digitalMasterVolume = 0.99f;
float SoundSystem::SFXVolume = 0.6f;
float SoundSystem::musicVolume = 0.4f;
float SoundSystem::radioVolume = 0.6f;
float SoundSystem::bettyVolume = 0.7f;
int32_t SoundSystem::largestSensorContact = -1;

#define FALLOFF_DISTANCE (1500.0f)
//---------------------------------------------------------------------------
// class SoundSystem
//---------------------------------------------------------------------------
void
SoundSystem::destroy(void)
{
	if (soundHeap)
	{
		soundHeap->destroy();
		delete soundHeap;
		soundHeap = nullptr;
	}
}

//---------------------------------------------------------------------------
int32_t
SoundSystem::init(const std::wstring_view& soundFileName)
{
	if (useSound)
	{
		FullPathFileName soundName;
		soundName.init(soundPath, soundFileName, ".snd");
		FitIniFile soundFile;
		int32_t result = soundFile.open(soundName);
		gosASSERT(result == NO_ERROR);
		result = soundFile.seekBlock("SoundSetup");
		gosASSERT(result == NO_ERROR);
		result = soundFile.readIdULong("soundHeapSize", soundHeapSize);
		gosASSERT(result == NO_ERROR);
		result = soundFile.readIdFloat("MaxSoundDistance", maxSoundDistance);
		gosASSERT(result == NO_ERROR);
		soundHeap = new UserHeap;
		gosASSERT(soundHeap != nullptr);
		result = soundHeap->init(soundHeapSize, "SOUND");
		gosASSERT(result == NO_ERROR);
		//-----------------------------------------------------------------------
		// Startup the Sound packet File with the sound Blocks in it.
		// This works by sound ID.  The sound ID is the packet number.
		// When ordered to play a sample, the sound system check to see if that
		// sound ID is in the cache.  If not, it is loaded.  If there is no more
		// room, any idle sounds are flushed in order of priority.
		soundDataFile = new PacketFile;
		gosASSERT(soundDataFile != nullptr);
		FullPathFileName soundDataPath;
		soundDataPath.init(CDsoundPath, soundFileName, ".pak");
		result = soundDataFile->open(soundDataPath);
		gosASSERT(result == NO_ERROR);
		bettyDataFile = new PacketFile;
		gosASSERT(bettyDataFile != nullptr);
		FullPathFileName bettyDataPath;
		bettyDataPath.init(CDsoundPath, "Betty", ".pak");
		result = bettyDataFile->open(bettyDataPath);
		gosASSERT(result == NO_ERROR);
		numBettySamples = bettyDataFile->getNumPackets();
		supportDataFile = new PacketFile;
		gosASSERT(supportDataFile != nullptr);
		FullPathFileName supportDataPath;
		supportDataPath.init(CDsoundPath, "support", ".pak");
		result = supportDataFile->open(supportDataPath);
		gosASSERT(result == NO_ERROR);
		numSupportSamples = supportDataFile->getNumPackets();
		//-----------------------------------------------------------------------
		// Load all of the sound Bite data.  Do not load actual packet unless
		// preload field is TRUE.
		result = soundFile.seekBlock("SoundBites");
		gosASSERT(result == NO_ERROR);
		result = soundFile.readIdULong("numBites", numSoundBites);
		gosASSERT(result == NO_ERROR);
		//-----------------------------------------------------------------------
		// Preallocate SoundBites
		sounds = (SoundBite*)soundHeap->Malloc(sizeof(SoundBite) * numSoundBites);
		gosASSERT(sounds != nullptr);
		memset(sounds, 0, sizeof(SoundBite) * numSoundBites);
		int32_t i;
		for (i = 0; i < (int32_t)numSoundBites; i++)
		{
			char biteBlock[20];
			sprintf(biteBlock, "SoundBite%d", i);
			result = soundFile.seekBlock(biteBlock);
			gosASSERT(result == NO_ERROR);
			result = soundFile.readIdULong("priority", sounds[i].priority);
			gosASSERT(result == NO_ERROR);
			result = soundFile.readIdULong("cache", sounds[i].cacheStatus);
			gosASSERT(result == NO_ERROR);
			result = soundFile.readIdULong("soundId", sounds[i].soundId);
			gosASSERT(result == NO_ERROR);
			preloadSoundBite(i); // ALWAYS Preload!!!!!!!!!!!!!
			result = soundFile.readIdFloat("volume", sounds[i].volume);
			gosASSERT(result == NO_ERROR);
		}
		//---------------------------------------------------------------
		// Load the digital Music Data Strings
		result = soundFile.seekBlock("DigitalMusic");
		gosASSERT(result == NO_ERROR);
		result = soundFile.readIdLong("NumDMS", numDMS);
		gosASSERT(result == NO_ERROR);
		result = soundFile.readIdFloat("StreamFadeDownTime", streamFadeDownTime);
		gosASSERT(result == NO_ERROR);
		result = soundFile.readIdULong("StreamBitDepth", digitalStreamBitDepth);
		gosASSERT(result == NO_ERROR);
		result = soundFile.readIdULong("StreamChannels", digitalStreamChannels);
		gosASSERT(result == NO_ERROR);
		digitalMusicIds = (const std::wstring_view&*)soundHeap->Malloc(sizeof(const std::wstring_view&) * numDMS);
		gosASSERT(digitalMusicIds != nullptr);
		digitalMusicLoopFlags = (bool*)soundHeap->Malloc(sizeof(bool) * numDMS);
		gosASSERT(digitalMusicLoopFlags != nullptr);
		digitalMusicVolume = (float*)soundHeap->Malloc(sizeof(float) * numDMS);
		for (i = 0; i < numDMS; i++)
		{
			char digitalMSId[20];
			sprintf(digitalMSId, "DMS%d", i);
			char digitalMSBId[20];
			sprintf(digitalMSBId, "DMSLoop%d", i);
			char digitalMSVId[25];
			sprintf(digitalMSVId, "DMSVolume%d", i);
			digitalMusicIds[i] = (const std::wstring_view&)soundHeap->Malloc(30);
			result = soundFile.readIdString(digitalMSId, digitalMusicIds[i], 29);
			gosASSERT(result == NO_ERROR);
			result = soundFile.readIdBoolean(digitalMSBId, digitalMusicLoopFlags[i]);
			gosASSERT(result == NO_ERROR);
			result = soundFile.readIdFloat(digitalMSVId, digitalMusicVolume[i]);
			if (result != NO_ERROR)
				digitalMusicVolume[i] = 1.0f;
		}
		soundFile.close();
		for (i = 0; i < MAX_DIGITAL_SAMPLES + 4; i++)
		{
			gosAudio_AllocateChannelSliders(i, gosAudio_Volume | gosAudio_Panning);
		}
	}
	digitalMasterVolume = float(DigitalMasterVolume) / 256.0f;
	SFXVolume = float(sfxVolume) / 256.0f;
	musicVolume = float(MusicVolume) / 256.0f;
	radioVolume = float(RadioVolume) / 256.0f;
	bettyVolume = float(BettyVolume) / 256.0f;
	stream1Id = stream2Id = 0;
	return (NO_ERROR);
}

//////////////////////////////////////////////////////////////////
//
// wave_ParseWaveMemory
//   Parses a chunk of memory into the header and samples.
//   This is done by looking for the "fmt " and "data"
//   fields in the memory.
//
//////////////////////////////////////////////////////////////////
bool
wave_ParseWaveMemory(puint8_t lpChunkOfMemory, MC2_WAVEFORMATEX** lplpWaveHeader,
	puint8_t* lplpWaveSamples, uint32_t* lpcbWaveSize)
{
	uint32_t* pdw;
	uint32_t* pdwEnd;
	uint32_t dwRiff;
	uint32_t dwType;
	uint32_t dwLength;
	// Set defaults to nullptr or zero
	if (*lplpWaveHeader)
		*lplpWaveHeader = nullptr;
	if (*lplpWaveSamples)
		*lplpWaveSamples = nullptr;
	if (lpcbWaveSize)
		*lpcbWaveSize = 0;
	// Set up uint32_t pointers to the start of the chunk
	// of memory.
	pdw = (uint32_t*)lpChunkOfMemory;
	// Get the type and length of the chunk of memory
	dwRiff = *pdw++;
	dwLength = *pdw++;
	dwType = *pdw++;
	// Using the mmioFOURCC macro (part of Windows SDK), ensure
	// that this is a RIFF WAVE chunk of memory
	if (dwRiff != 0x46464952)
		return FALSE; // not even RIFF
	if (dwType != 0x45564157)
		return FALSE; // not a WAV
	// Find the pointer to the end of the chunk of memory
	pdwEnd = (uint32_t*)((puint8_t)pdw + dwLength - 4);
	// Run through the bytes looking for the tags
	while (pdw < pdwEnd)
	{
		dwType = *pdw++;
		dwLength = *pdw++;
		switch (dwType)
		{
		// Found the format part
		case 0x20746d66:
			if (*lplpWaveHeader == nullptr)
			{
				if (dwLength < sizeof(MC2_WAVEFORMATEX))
					return FALSE; // something's wrong! Not a WAV
				// Set the lplpWaveHeader to point to this part of
				// the memory chunk
				*lplpWaveHeader = (MC2_WAVEFORMATEX*)pdw;
				// Check to see if the other two items have been
				// filled out yet (the bits and the size of the
				// bits). If so, then this chunk of memory has
				// been parsed out and we can exit
				if ((!lplpWaveSamples || *lplpWaveSamples) && (!lpcbWaveSize || *lpcbWaveSize))
				{
					return TRUE;
				}
			}
			break;
		// Found the samples
		case 0x61746164:
			if ((lplpWaveSamples && !*lplpWaveSamples) || (lpcbWaveSize && !*lpcbWaveSize))
			{
				// Point the samples pointer to this part of the
				// chunk of memory.
				if (*lplpWaveSamples == nullptr)
					*lplpWaveSamples = (puint8_t)pdw;
				// Set the size of the wave
				if (lpcbWaveSize)
					*lpcbWaveSize = dwLength;
				// Make sure we have our header pointer set up.
				// If we do, we can exit
				if (lplpWaveHeader)
					return TRUE;
			}
			break;
		} // End case
		// Move the pointer through the chunk of memory
		pdw = (uint32_t*)((puint8_t)pdw + ((dwLength + 1) & ~1));
	}
	// Failed! If we made it here, we did not get all the peices
	// of the wave
	return FALSE;
}

//---------------------------------------------------------------------------
void
SoundSystem::preloadSoundBite(int32_t soundId)
{
	int32_t result = soundDataFile->seekPacket(soundId);
	if (result != NO_ERROR)
		return;
	//-------------------------------------------------------------
	// If there is a packet in this spot, load it.
	// First, check if there is enough room in the sound heap
	// and if not, free up any samples whose cache status says dump.
	uint32_t packetSize = soundDataFile->getPacketSize();
	if (packetSize > 0)
	{
		SoundBite* thisSoundBite = (SoundBite*)(&(sounds[soundId]));
		thisSoundBite->resourceHandle = 0;
		if (thisSoundBite->biteSize == 0 || thisSoundBite->biteData == nullptr)
		{
			thisSoundBite->biteSize = packetSize;
			thisSoundBite->biteData = (puint8_t)soundHeap->Malloc(packetSize);
			if (!thisSoundBite->biteData)
				return;
		}
		soundDataFile->readPacket(soundId, thisSoundBite->biteData);
		//--------------------------------------------------------------------
		// Hand GOS sound the data it needs to create the resource Handle
		gosAudio_Format soundFormat;
		soundFormat.wFormatTag = 1; // PCM
		MC2_WAVEFORMATEX* waveFormat = nullptr;
		puint8_t dataOffset = nullptr;
		uint32_t length = 0;
		uint32_t bitsPerSec = 0;
		wave_ParseWaveMemory(thisSoundBite->biteData, &waveFormat, &dataOffset, &length);
		if (waveFormat && dataOffset)
		{
			bitsPerSec = waveFormat->nBlockAlign / waveFormat->nChannels * 8;
			soundFormat.nChannels = waveFormat->nChannels;
			soundFormat.nSamplesPerSec = waveFormat->nSamplesPerSec;
			soundFormat.nAvgBytesPerSec = waveFormat->nAvgBytesPerSec;
			soundFormat.nBlockAlign = waveFormat->nBlockAlign;
			soundFormat.wBitsPerSample = bitsPerSec;
			soundFormat.cbSize = 0;
			gosAudio_CreateResource(&thisSoundBite->resourceHandle, gosAudio_UserMemory, nullptr,
				&soundFormat, dataOffset, length);
		}
		else
		{
			thisSoundBite->resourceHandle = 0;
		}
	}
}

//---------------------------------------------------------------------------
void
SoundSystem::update(void)
{
	if (useSound && (isRaining != oldRaining))
	{
		// Play the isRaining SoundEffect at the soundEffect level in a loop.
		if (isRaining && (isRaining < numSoundBites))
		{
			int32_t ourChannel = FIRE_CHANNEL;
			gosAudio_SetChannelSlider(ourChannel, gosAudio_Panning, 0.0);
			float vol = sounds[isRaining].volume;
			gosAudio_SetChannelSlider(
				ourChannel, gosAudio_Volume, (digitalMasterVolume * vol * SFXVolume));
			channelSampleId[ourChannel] = isRaining;
			channelInUse[ourChannel] = TRUE;
			if (sounds[isRaining].biteData)
			{
				gosAudio_AssignResourceToChannel(ourChannel, sounds[isRaining].resourceHandle);
				gosAudio_SetChannelPlayMode(ourChannel, gosAudio_Loop);
			}
			oldRaining = isRaining;
		}
		else
		{
			// Stop the sound Effect by fading it to zero!
			oldRaining = isRaining;
			gosAudio_PlayMode sampleStatus = gosAudio_GetChannelPlayMode(FIRE_CHANNEL);
			if (sampleStatus == gosAudio_PlayOnce || sampleStatus == gosAudio_Loop)
			{
				fadeDown[FIRE_CHANNEL] = TRUE;
			}
		}
	}
	if (useSound && bettySoundBite)
	{
		//---------------------------------------------------
		// Check if betty is done.  If so, whack the memory
		gosAudio_PlayMode sampleStatus = gosAudio_GetChannelPlayMode(BETTY_CHANNEL);
		if (sampleStatus != gosAudio_PlayOnce)
		{
			gosAudio_DestroyResource(&bettyHandle);
			soundHeap->Free(bettySoundBite);
			bettySoundBite = nullptr;
		}
	}
	if (useSound && supportSoundBite)
	{
		//---------------------------------------------------
		// Check if betty is done.  If so, whack the memory
		gosAudio_PlayMode sampleStatus = gosAudio_GetChannelPlayMode(SUPPORT_CHANNEL);
		if (sampleStatus != gosAudio_PlayOnce)
		{
			gosAudio_DestroyResource(&supportHandle);
			soundHeap->Free(supportSoundBite);
			supportSoundBite = nullptr;
		}
	}
	if (useSound && stream3Handle)
	{
		gosAudio_PlayMode sampleStatus = gosAudio_GetChannelPlayMode(STREAM3CHANNEL);
		if (!stream3Active && (sampleStatus != gosAudio_PlayOnce))
		{
			stream3Active = true;
			gosAudio_SetChannelPlayMode(STREAM3CHANNEL, gosAudio_PlayOnce);
		}
		sampleStatus = gosAudio_GetChannelPlayMode(STREAM3CHANNEL);
		if (stream3Active && sampleStatus != gosAudio_PlayOnce)
		{
			gosAudio_DestroyResource(&stream3Handle);
			stream3Handle = 0;
			stream3Active = false;
		}
	}
	if ((largestSensorContact > -1) && (sensorBeepUpdateTime > MAX_SENSOR_UPDATE_TIME))
	{
		playDigitalSample(SENSOR2);
		sensorBeepUpdateTime = 0.0f;
	}
	// Start the music change we requested playing!!
	// This avoids the stupid sound bug of starting more then one stream per
	// frame!!
	if (stream1Handle && !stream1Active)
	{
		stream1Active = true;
		gosAudio_SetChannelPlayMode(STREAM1CHANNEL,
			digitalMusicLoopFlags[currentMusicId] ? gosAudio_Loop : gosAudio_PlayOnce);
	}
	if (stream2Handle && !stream2Active)
	{
		stream2Active = true;
		gosAudio_SetChannelPlayMode(STREAM2CHANNEL,
			digitalMusicLoopFlags[currentMusicId] ? gosAudio_Loop : gosAudio_PlayOnce);
	}
	if (useMusic && (stream1Active || stream2Active))
	{
		if (stream1Active && (stream1Time != 0.0))
		{
			if (stream1Time < 0.0)
			{
				//------------------------------------------
				// We are fading Down.
				stream1Time += frameLength;
				if (stream1Time >= 0.0)
				{
					stream1Time = 0.0;
					if (stream1Handle)
						gosAudio_DestroyResource(&stream1Handle);
					stream1Handle = nullptr;
					stream1Active = FALSE;
				}
				else
				{
					float volLevel = fabs(stream1Time) / streamFadeDownTime;
					if (volLevel < 0.0)
						volLevel = 0.0;
					if (volLevel > 1.0)
						volLevel = 1.0;
					gosAudio_SetChannelSlider(STREAM1CHANNEL, gosAudio_Volume,
						digitalMasterVolume * volLevel * musicVolume * digitalMusicVolume[currentMusicId]);
				}
			}
			else if (stream1Time > 0.0)
			{
				//------------------------------------------
				// We are fading Up.
				stream1Time -= frameLength;
				if (stream1Time <= 0.0)
				{
					stream1Time = 0.0;
					gosAudio_SetChannelSlider(STREAM1CHANNEL, gosAudio_Volume,
						digitalMasterVolume * musicVolume * digitalMusicVolume[currentMusicId]);
				}
				else
				{
					float volLevel = (streamFadeDownTime - fabs(stream1Time)) / streamFadeDownTime;
					if (volLevel < 0.0)
						volLevel = 0.0;
					if (volLevel > 1.0)
						volLevel = 1.0;
					gosAudio_SetChannelSlider(STREAM1CHANNEL, gosAudio_Volume,
						digitalMasterVolume * volLevel * musicVolume * digitalMusicVolume[currentMusicId]);
				}
			}
		}
		if (stream2Active && (stream2Time != 0.0))
		{
			if (stream2Time < 0.0)
			{
				//------------------------------------------
				// We are fading Down.
				stream2Time += frameLength;
				if (stream2Time >= 0.0)
				{
					stream2Time = 0.0;
					if (stream2Handle)
						gosAudio_DestroyResource(&stream2Handle);
					stream2Handle = nullptr;
					stream2Active = FALSE;
				}
				else
				{
					float volLevel = fabs(stream2Time) / streamFadeDownTime;
					if (volLevel < 0.0)
						volLevel = 0.0;
					if (volLevel > 1.0)
						volLevel = 1.0;
					gosAudio_SetChannelSlider(STREAM2CHANNEL, gosAudio_Volume,
						digitalMasterVolume * volLevel * musicVolume * digitalMusicVolume[currentMusicId]);
				}
			}
			else if (stream2Time > 0.0)
			{
				//------------------------------------------
				// We are fading Up.
				stream2Time -= frameLength;
				if (stream2Time <= 0.0)
				{
					stream2Time = 0.0;
					gosAudio_SetChannelSlider(STREAM2CHANNEL, gosAudio_Volume,
						digitalMasterVolume * musicVolume * digitalMusicVolume[currentMusicId]);
				}
				else
				{
					float volLevel = (streamFadeDownTime - fabs(stream2Time)) / streamFadeDownTime;
					if (volLevel < 0.0)
						volLevel = 0.0;
					if (volLevel > 1.0)
						volLevel = 1.0;
					gosAudio_SetChannelSlider(STREAM2CHANNEL, gosAudio_Volume,
						digitalMasterVolume * volLevel * musicVolume * digitalMusicVolume[currentMusicId]);
				}
			}
		}
		if (stream1Active && stream1Handle && (gosAudio_GetChannelPlayMode(STREAM1CHANNEL) == gosAudio_Stop))
		{
			if (stream1Handle)
				gosAudio_DestroyResource(&stream1Handle);
			stream1Handle = nullptr;
			stream1Time = 0.0;
			stream1Active = FALSE;
		}
		if (stream2Active && stream2Handle && (gosAudio_GetChannelPlayMode(STREAM2CHANNEL) == gosAudio_Stop))
		{
			if (stream2Handle)
				gosAudio_DestroyResource(&stream2Handle);
			stream2Handle = nullptr;
			stream2Time = 0.0;
			stream2Active = FALSE;
		}
		if (!stream1Active && !stream2Active)
			currentMusicId = -1;
	}
	if (useSound)
	{
		//-----------------------------------------------
		// Check all samples to see if one should end.
		for (size_t i = 0; i < MAX_DIGITAL_SAMPLES; i++)
		{
			gosAudio_PlayMode sampleStatus = gosAudio_GetChannelPlayMode(i);
			if (sampleStatus == gosAudio_PlayOnce)
			{
				if (fadeDown[i])
				{
					float vol;
					gosAudio_GetChannelSlider(i, gosAudio_Volume, &vol);
					if (vol <= 0.1f)
						vol = 0.1f;
					gosAudio_SetChannelSlider(i, gosAudio_Volume, vol - 0.1f);
					gosAudio_GetChannelSlider(i, gosAudio_Volume, &vol);
					if (vol == 0.0f)
					{
						fadeDown[i] = FALSE;
						gosAudio_SetChannelPlayMode(i, gosAudio_Stop);
					}
				}
			}
			else
			{
				fadeDown[i] = FALSE;
				channelSampleId[i] = -1;
				channelInUse[i] = FALSE;
			}
		}
	}
	if (!gamePaused)
		sensorBeepUpdateTime += frameLength;
}

//---------------------------------------------------------------------------
int32_t
SoundSystem::playDigitalMusic(int32_t musicId)
{
	//-------------------------------------------------------------------
	// Make sure we have a real music filename.
	if (useMusic)
	{
		if ((musicId >= 0) && (musicId < numDMS) && (strncmp(digitalMusicIds[musicId], "NONE", 4) != 0))
		{
			if (musicId == currentMusicId)
				return (-1);
			if (useSound)
			{
				//---------------------------------------------------------------------------------------------
				// Just start tune.  No fade necessary.  Set fadeTime to 0.0 to
				// tell update to leave it alone. Use Stream1 by default.
				FullPathFileName digitalStream;
				digitalStream.init(soundPath, digitalMusicIds[musicId], ".wav");
				//---------------------------------------------
				// Gotta check if music file is there
				// new sound system can't handle it if its not.
				if (fileExists(digitalStream))
				{
					if (!stream1Active && !stream2Active)
					{
						if (stream1Handle)
						{
							gosAudio_DestroyResource(&stream1Handle);
							stream1Handle = 0;
						}
						gosAudio_CreateResource(
							&stream1Handle, gosAudio_StreamedFile, (const std::wstring_view&)digitalStream);
						stream1Time = streamFadeDownTime;
						gosAudio_SetChannelSlider(STREAM1CHANNEL, gosAudio_Volume, 0.0f);
						gosAudio_SetChannelSlider(STREAM1CHANNEL, gosAudio_Panning, 0.0f);
						gosAudio_AssignResourceToChannel(STREAM1CHANNEL, stream1Handle);
						currentMusicId = musicId;
						stream1Id = musicId;
					}
					else if (!stream2Active)
					{
						if (stream2Handle)
						{
							gosAudio_DestroyResource(&stream2Handle);
							stream2Handle = 0;
						}
						gosAudio_CreateResource(
							&stream2Handle, gosAudio_StreamedFile, (const std::wstring_view&)digitalStream);
						// Need to check if stream1 ever got all the way faded
						// up!
						if (stream1Time > 0.0f)
							stream1Time = -stream1Time;
						else
							stream1Time = -streamFadeDownTime;
						stream2Time = streamFadeDownTime;
						gosAudio_SetChannelSlider(STREAM2CHANNEL, gosAudio_Volume, 0.0f);
						gosAudio_SetChannelSlider(STREAM2CHANNEL, gosAudio_Panning, 0.0f);
						gosAudio_AssignResourceToChannel(STREAM2CHANNEL, stream2Handle);
						currentMusicId = musicId;
						stream2Id = musicId;
					}
					else if (!stream1Active)
					{
						if (stream1Handle)
						{
							gosAudio_DestroyResource(&stream1Handle);
							stream1Handle = 0;
						}
						gosAudio_CreateResource(
							&stream1Handle, gosAudio_StreamedFile, (const std::wstring_view&)digitalStream);
						stream1Time = streamFadeDownTime;
						// Need to check if stream2 ever got all the way faded
						// up!
						if (stream2Time > 0.0f)
							stream2Time = -stream2Time;
						else
							stream2Time = -streamFadeDownTime;
						gosAudio_SetChannelSlider(STREAM1CHANNEL, gosAudio_Volume, 0.0f);
						gosAudio_SetChannelSlider(STREAM1CHANNEL, gosAudio_Panning, 0.0f);
						gosAudio_AssignResourceToChannel(STREAM1CHANNEL, stream1Handle);
						currentMusicId = musicId;
						stream1Id = musicId;
					}
					else
					{
						return (-1);
					}
				}
			}
		}
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
SoundSystem::playDigitalStream(const std::wstring_view& streamName)
{
	//-------------------------------------------------------------------
	// Make sure we have a real music filename.
	if (useSound)
	{
		char actualName[1024];
		_splitpath(streamName, nullptr, nullptr, actualName, nullptr);
		//---------------------------------------------------------------------------------------------
		// Just start tune.  No fade necessary.  Set fadeTime to 0.0 to tell
		// update to leave it alone. Use Stream1 by default.
		FullPathFileName digitalStream;
		digitalStream.init(soundPath, actualName, ".wav");
		//---------------------------------------------
		// Gotta check if music file is there
		// new sound system can't handle it if its not.
		if (fileExists(digitalStream))
		{
			if (!stream3Active)
			{
				if (stream3Handle)
				{
					gosAudio_DestroyResource(&stream3Handle);
					stream3Handle = 0;
				}
				gosAudio_CreateResource(
					&stream3Handle, gosAudio_StreamedFile, (const std::wstring_view&)digitalStream);
				stream3Active = false;
				gosAudio_AssignResourceToChannel(STREAM3CHANNEL, stream3Handle);
				gosAudio_SetChannelSlider(
					STREAM3CHANNEL, gosAudio_Volume, digitalMasterVolume * bettyVolume);
				gosAudio_SetChannelSlider(STREAM3CHANNEL, gosAudio_Panning, 0.0f);
			}
			else
			{
				return (-1);
			}
		}
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
SoundSystem::playBettySample(uint32_t bettySampleId)
{
	if (useSound && (bettySoundBite == nullptr)) // Playing Betty takes precedence
	{
		if (bettySampleId >= numBettySamples)
			return (-1);
		int32_t ourChannel = BETTY_CHANNEL;
		gosAudio_SetChannelSlider(ourChannel, gosAudio_Panning, 0.0f);
		float vol = 1.0;
		gosAudio_SetChannelSlider(
			ourChannel, gosAudio_Volume, digitalMasterVolume * vol * bettyVolume);
		gosAudio_SetChannelSlider(
			RADIO_CHANNEL, gosAudio_Volume, digitalMasterVolume * vol * radioVolume * 0.5f);
		lastBettyId = bettySampleId;
		channelInUse[ourChannel] = TRUE;
		int32_t result = bettyDataFile->seekPacket(bettySampleId);
		if (result != NO_ERROR)
			return (-1);
		int32_t bettySize = bettyDataFile->getPacketSize();
		bettySoundBite = (puint8_t)soundHeap->Malloc(bettySize);
		gosASSERT(bettySoundBite != nullptr);
		bettyDataFile->readPacket(bettySampleId, bettySoundBite);
		//--------------------------------------------------------------------
		// Hand GOS sound the data it needs to create the resource Handle
		gosAudio_Format soundFormat;
		soundFormat.wFormatTag = 1; // PCM
		MC2_WAVEFORMATEX* waveFormat = nullptr;
		puint8_t dataOffset = nullptr;
		uint32_t length = 0;
		uint32_t bitsPerSec = 0;
		wave_ParseWaveMemory(bettySoundBite, &waveFormat, &dataOffset, &length);
		bitsPerSec = waveFormat->nBlockAlign / waveFormat->nChannels * 8;
		soundFormat.nChannels = waveFormat->nChannels;
		soundFormat.nSamplesPerSec = waveFormat->nSamplesPerSec;
		soundFormat.nAvgBytesPerSec = waveFormat->nAvgBytesPerSec;
		soundFormat.nBlockAlign = waveFormat->nBlockAlign;
		soundFormat.wBitsPerSample = bitsPerSec;
		soundFormat.cbSize = 0;
		gosAudio_CreateResource(
			&bettyHandle, gosAudio_UserMemory, nullptr, &soundFormat, dataOffset, length);
		gosAudio_AssignResourceToChannel(ourChannel, bettyHandle);
		gosAudio_SetChannelPlayMode(ourChannel, gosAudio_PlayOnce);
		return (BETTY_CHANNEL);
	}
	return (-1);
}

//---------------------------------------------------------------------------
int32_t
SoundSystem::playSupportSample(uint32_t supportSampleId, const std::wstring_view& fileName)
{
	if (useSound && (supportSoundBite == nullptr)) // Playing Support takes precedence
	{
		if (!fileName)
		{
			if (supportSampleId >= numSupportSamples)
				return (-1);
		}
		int32_t ourChannel = SUPPORT_CHANNEL;
		if (fileName)
		{
			playDigitalStream(fileName);
			return (STREAM3CHANNEL);
		}
		else
		{
			gosAudio_SetChannelSlider(ourChannel, gosAudio_Panning, 0.0f);
			float vol = 1.0;
			gosAudio_SetChannelSlider(
				ourChannel, gosAudio_Volume, digitalMasterVolume * vol * bettyVolume);
			lastSupportId = supportSampleId;
			channelInUse[ourChannel] = TRUE;
			int32_t result = supportDataFile->seekPacket(supportSampleId);
			if (result != NO_ERROR)
				return (-1);
			int32_t supportSize = supportDataFile->getPacketSize();
			if (supportSize > 0)
			{
				supportSoundBite = (puint8_t)soundHeap->Malloc(supportSize);
				gosASSERT(supportSoundBite != nullptr);
			}
			else
			{
				return -1; // Sound Bite does NOT exist!!
			}
			supportDataFile->readPacket(supportSampleId, supportSoundBite);
		}
		//--------------------------------------------------------------------
		// Hand GOS sound the data it needs to create the resource Handle
		gosAudio_Format soundFormat;
		soundFormat.wFormatTag = 1; // PCM
		MC2_WAVEFORMATEX* waveFormat = nullptr;
		puint8_t dataOffset = nullptr;
		uint32_t length = 0;
		uint32_t bitsPerSec = 0;
		wave_ParseWaveMemory(supportSoundBite, &waveFormat, &dataOffset, &length);
		bitsPerSec = waveFormat->nBlockAlign / waveFormat->nChannels * 8;
		soundFormat.nChannels = waveFormat->nChannels;
		soundFormat.nSamplesPerSec = waveFormat->nSamplesPerSec;
		soundFormat.nAvgBytesPerSec = waveFormat->nAvgBytesPerSec;
		soundFormat.nBlockAlign = waveFormat->nBlockAlign;
		soundFormat.wBitsPerSample = bitsPerSec;
		soundFormat.cbSize = 0;
		gosAudio_CreateResource(
			&supportHandle, gosAudio_UserMemory, nullptr, &soundFormat, dataOffset, length);
		gosAudio_AssignResourceToChannel(ourChannel, supportHandle);
		gosAudio_SetChannelPlayMode(ourChannel, gosAudio_PlayOnce);
		return (SUPPORT_CHANNEL);
	}
	return (-1);
}

//---------------------------------------------------------------------------
bool
SoundSystem::isPlayingSample(int32_t sampleId)
{
	for (size_t i = 0; i < MAX_DIGITAL_SAMPLES; i++)
	{
		if ((sampleId == channelSampleId[i]))
			return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------------------------
bool
SoundSystem::isChannelPlaying(int32_t channelId)
{
	if ((channelId < 0) || (channelId > MAX_DIGITAL_SAMPLES))
		return (FALSE);
	gosAudio_PlayMode sampleStatus = gosAudio_GetChannelPlayMode(channelId);
	if (sampleStatus == gosAudio_PlayOnce || sampleStatus == gosAudio_Loop)
		return (TRUE);
	return FALSE;
}

//---------------------------------------------------------------------------
bool
SoundSystem::isPlayingVoiceOver(void)
{
	if (stream3Handle)
		return true;
	return false;
}

//---------------------------------------------------------------------------
bool
SoundSystem::isDigitalMusicPlaying(void)
{
	if (stream2Handle || stream1Handle)
		return true;
	return false;
}

//---------------------------------------------------------------------------
int32_t
SoundSystem::findOpenChannel(int32_t start, int32_t end)
{
	int32_t channel = start;
	while (channelInUse[channel] && (channel < end))
		channel++;
	if (channel == end)
		return (-1);
	return (channel);
}

//---------------------------------------------------------------------------
int32_t
SoundSystem::playDigitalSample(uint32_t sampleId, Stuff::Vector3D pos, bool allowDupes)
{
	if (useSound && allowDupes || (!isPlayingSample(sampleId) && !allowDupes))
	{
		if (sampleId >= numSoundBites)
			return (-1);
		int32_t ourChannel = findOpenChannel(1, SUPPORT_CHANNEL);
		if (ourChannel != -1)
		{
			float distanceVolume = 1.0f;
			float panVolume = 0.0f;
			if (eye && (pos.z != -9999.0f))
			{
				Stuff::Vector3D distance;
				distance.Subtract(eye->getPosition(), pos);
				float dist = distance.GetApproximateLength();
				if (dist < FALLOFF_DISTANCE)
					distanceVolume = (FALLOFF_DISTANCE - dist) / FALLOFF_DISTANCE;
				else
					return -1; // Do not play sound.  TOO far away!!
				// Figure out where in stereo field to play.
				OppRotate(distance, eye->getRotation().y);
				panVolume = distance.x / (FALLOFF_DISTANCE * 0.5f);
				if (panVolume > 1.0f)
					panVolume = 1.0f;
				else if (panVolume < -1.0f)
					panVolume = -1.0f;
			}
			float vol = sounds[sampleId].volume * distanceVolume;
			if (vol > 1.0f)
				vol = 1.0f;
			else if (vol <= 0.0f) // No VOlume.  DON't PLAY!
				return -1;
			gosAudio_SetChannelSlider(ourChannel, gosAudio_Panning, panVolume);
			gosAudio_SetChannelSlider(
				ourChannel, gosAudio_Volume, (digitalMasterVolume * vol * SFXVolume));
			channelSampleId[ourChannel] = sampleId;
			channelInUse[ourChannel] = TRUE;
			if (sounds[sampleId].biteData && sounds[sampleId].resourceHandle)
			{
				gosAudio_AssignResourceToChannel(ourChannel, sounds[sampleId].resourceHandle);
				gosAudio_SetChannelPlayMode(ourChannel, gosAudio_PlayOnce);
			}
			return (ourChannel);
		}
	}
	return (-1);
}

//---------------------------------------------------------------------------
void
SoundSystem::stopDigitalSample(uint32_t sampleHandleNumber)
{
	if (useSound)
	{
		gosAudio_PlayMode sampleStatus = gosAudio_GetChannelPlayMode(sampleHandleNumber);
		if (sampleStatus == gosAudio_PlayOnce || sampleStatus == gosAudio_Loop)
		{
			fadeDown[sampleHandleNumber] = TRUE;
		}
	}
}

//---------------------------------------------------------------------------
void
SoundSystem::stopDigitalMusic(void)
{
	if (useSound && useMusic)
	{
		if (stream1Active && stream1Handle)
		{
			if (stream1Handle)
				gosAudio_DestroyResource(&stream1Handle);
			stream1Handle = nullptr;
			stream1Time = 0.0;
			stream1Active = FALSE;
		}
		if (stream2Active && stream2Handle)
		{
			if (stream2Handle)
				gosAudio_DestroyResource(&stream2Handle);
			stream2Handle = nullptr;
			stream2Time = 0.0;
			stream2Active = FALSE;
		}
		currentMusicId = -1;
	}
}

//---------------------------------------------------------------------------
void
SoundSystem::stopSupportSample(void)
{
	if (stream3Handle)
	{
		gosAudio_DestroyResource(&stream3Handle);
		stream3Handle = 0;
		stream3Active = false;
	}
}

//---------------------------------------------------------------------------
void
SoundSystem::stopBettySample(void)
{
	if (bettyHandle)
	{
		gosAudio_SetChannelPlayMode(BETTY_CHANNEL, gosAudio_Stop);
		gosAudio_DestroyResource(&bettyHandle);
		soundHeap->Free(bettySoundBite);
		bettySoundBite = nullptr;
		bettyHandle = nullptr;
	}
}

//---------------------------------------------------------------------------
void
SoundSystem::setDigitalMasterVolume(byte volume)
{
	if (useSound)
	{
		DigitalMasterVolume = volume;
		digitalMasterVolume = float(DigitalMasterVolume) / 256.0f;
	}
}

//---------------------------------------------------------------------------
int32_t
SoundSystem::getDigitalMasterVolume(void)
{
	if (useSound)
	{
		return DigitalMasterVolume;
	}
	return (0);
}

//---------------------------------------------------------------------------
void
SoundSystem::setSFXVolume(uint8_t volume)
{
	if (useSound)
	{
		sfxVolume = volume;
		SFXVolume = float(sfxVolume) / 256.0f;
	}
}

//---------------------------------------------------------------------------
uint8_t
SoundSystem::getSFXVolume(void)
{
	if (useSound)
	{
		return sfxVolume;
	}
	return (0);
}

//---------------------------------------------------------------------------
void
SoundSystem::setRadioVolume(uint8_t volume)
{
	if (useSound)
	{
		RadioVolume = volume;
		radioVolume = float(RadioVolume) / 256.0f;
	}
}

//---------------------------------------------------------------------------
uint8_t
SoundSystem::getRadioVolume(void)
{
	if (useSound)
	{
		return RadioVolume;
	}
	return (0);
}

//---------------------------------------------------------------------------
void
SoundSystem::setMusicVolume(uint8_t volume)
{
	if (useSound)
	{
		MusicVolume = volume;
		musicVolume = float(MusicVolume) / 256.0f;
		if (useMusic)
		{
			if (stream1Active && stream1Handle)
			{
				gosAudio_SetChannelSlider(
					STREAM1CHANNEL, gosAudio_Volume, (digitalMasterVolume * musicVolume));
			}
			if (stream2Active && stream2Handle)
			{
				gosAudio_SetChannelSlider(
					STREAM2CHANNEL, gosAudio_Volume, (digitalMasterVolume * musicVolume));
			}
		}
	}
}

//---------------------------------------------------------------------------
uint8_t
SoundSystem::getMusicVolume(void)
{
	if (useSound)
	{
		return MusicVolume;
	}
	return (0);
}

//---------------------------------------------------------------------------
void
SoundSystem::setBettyVolume(uint8_t volume)
{
	if (useSound)
	{
		BettyVolume = volume;
		bettyVolume = float(BettyVolume) / 256.0f;
	}
}

//---------------------------------------------------------------------------
uint8_t
SoundSystem::getBettyVolume(void)
{
	if (useSound)
	{
		return BettyVolume;
	}
	return (0);
}

//---------------------------------------------------------------------------
void
SoundSystem::stopABLMusic(void)
{
	stopDigitalMusic();
}

//---------------------------------------------------------------------------
void
SoundSystem::playABLSFX(int32_t sfxId)
{
	playDigitalSample(sfxId);
}

//---------------------------------------------------------------------------
void
SoundSystem::playABLDigitalMusic(int32_t musicId)
{
	//	PAUSE(("Switching to Tune %d.  Playing Tune
	//%d.",musicId,currentMusicId));
	playDigitalMusic(musicId);
}

//---------------------------------------------------------------------------
