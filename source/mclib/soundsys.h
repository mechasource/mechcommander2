//---------------------------------------------------------------------------
// soundsys.h - This file is the sound system header
//
// MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef SOUNDSYS_H
#define SOUNDSYS_H
//---------------------------------------------------------------------------
// Include files
#ifndef DSTD_H
#include "dstd.h"
#endif

#ifndef HEAP_H
#include "heap.h"
#endif

#ifndef PACKET_H
#include "packet.h"
#endif

#ifndef CIDENT_H
#include "cident.h"
#endif

#ifndef PATHS_H
#include "paths.h"
#endif

#ifndef INIFILE_H
#include "inifile.h"
#endif

#ifndef TIMING_H
#include "timing.h"
#endif

#include <math.h>
#include <gameos.hpp>
#include <stuff/stuff.hpp>
//---------------------------------------------------------------------------
// Macro Defintiions
// Channels 0 through 10 are Main Sound Effect Channels
// Channels 11 through 12 are ambient sound channels
// Channel 13 is Support Officer
// Channel 14 is Betty
// Channel 15 is Fire Burning SFX
#define MAX_DIGITAL_SAMPLES			16

#define MAX_SAMPLE_VOLUME			127

#define RADIO_CHANNEL				0
#define FIRE_CHANNEL				(MAX_DIGITAL_SAMPLES-1)
#define BETTY_CHANNEL				(FIRE_CHANNEL-1)
#define SUPPORT_CHANNEL				(BETTY_CHANNEL-1)

#define AMBIENT_CHANNEL_START		11

#define STREAM1CHANNEL				16
#define STREAM2CHANNEL				17
#define STREAM3CHANNEL				18		//Used for in-Mission voiceovers

#define MAX_QUEUED_MESSAGES			8
//---------------------------------------------------------------------------
// Classes
#pragma pack(1)
typedef struct {     
	uint16_t  wFormatTag;     
	uint16_t  nChannels; 
    uint32_t nSamplesPerSec;     
	uint32_t nAvgBytesPerSec;     
	uint16_t  nBlockAlign; 
} MC2_WAVEFORMATEX;
#pragma pack()

//---------------------------------------------------------------------------
typedef struct _SoundBite
{
	uint32_t		priority;
	uint32_t		cacheStatus;
	uint32_t		soundId;
	uint32_t 		biteSize;
	puint8_t			biteData;
	float				volume;
	HGOSAUDIO			resourceHandle;
} SoundBite;

//---------------------------------------------------------------------------
class SoundSystem
{
	//Data Members
	//-------------
	protected:
		bool				channelInUse[MAX_DIGITAL_SAMPLES];
		int32_t				channelSampleId[MAX_DIGITAL_SAMPLES];
		bool				fadeDown[MAX_DIGITAL_SAMPLES];
		
		uint32_t		soundHeapSize;
		UserHeapPtr			soundHeap;
		
		uint32_t		numSoundBites;
		SoundBite			*sounds;

		uint32_t		numBettySamples;
		uint32_t		numSupportSamples;
		PacketFilePtr		soundDataFile;
		PacketFilePtr		bettyDataFile;
		PacketFilePtr		supportDataFile;
		
		int32_t				numDMS;
		float				streamFadeDownTime;
		uint32_t		digitalStreamBitDepth;
		uint32_t		digitalStreamChannels;
		int32_t				currentMusicId;
		
		HGOSAUDIO			stream1Handle;
		bool				stream1Active;
		float				stream1Time;
		int32_t				stream1Id;
		
		HGOSAUDIO			stream2Handle;
		bool				stream2Active;
		float				stream2Time;
		int32_t				stream2Id;
		
		HGOSAUDIO			stream3Handle;
		bool				stream3Active;

		char				**digitalMusicIds;
		bool				*digitalMusicLoopFlags;
		float				*digitalMusicVolume;
				
		float				maxSoundDistance;
		
		puint8_t			bettySoundBite;							//Used to store current bitching betty sample
		int32_t				lastBettyId;
		HGOSAUDIO			bettyHandle;

		puint8_t			supportSoundBite;						//USed to store current support officer sample
		int32_t				lastSupportId;
		HGOSAUDIO			supportHandle;
		
		uint32_t				isRaining;								//What sound FX should play as an ambient background loop.  Rain/Fire/Nothing, etc.
		uint32_t				oldRaining;								//Check if the old rain doesn't match new.  If so, play new instead!

		float				sensorBeepUpdateTime;
		
		bool				gamePaused;								//Flag to tell me if game is paused

		static float		digitalMasterVolume;
		static float		SFXVolume;
		static float		musicVolume;
		static float		radioVolume;
		static float		bettyVolume;

	public:

		static int32_t			largestSensorContact;
		
	//Member Functions
	//----------------
	public:
	
		SoundSystem (void)
		{
			init(void);
		}
		
		~SoundSystem (void)
		{
			destroy(void);
		}
		
		void init (void)
		{
			memset(channelInUse,0,sizeof(bool)*MAX_DIGITAL_SAMPLES);
			memset(channelSampleId,-1,sizeof(int32_t)*MAX_DIGITAL_SAMPLES);
			memset(fadeDown,0,sizeof(bool)*MAX_DIGITAL_SAMPLES);
			
			soundHeapSize = 0;
			soundHeap = NULL;

			numSoundBites = 0;
			sounds = NULL;
			
			soundDataFile = bettyDataFile = NULL;
			
			numDMS = 0;
			streamFadeDownTime = 0.0;
			digitalStreamBitDepth = 0;
			digitalStreamChannels = 0;
			
			digitalMusicIds = NULL;
			digitalMusicLoopFlags = NULL;
			
			maxSoundDistance = 0.0;
			
			bettySoundBite = NULL;
			lastBettyId = -1;
			
			currentMusicId = -1;
			
			stream1Handle = 0;
			stream1Active = FALSE;
			stream1Time = 0.0;
			
			stream2Handle = 0;
			stream2Active = FALSE;
			stream2Time = 0.0;
			
			stream3Handle = 0;
			stream3Active = false;
			
			numBettySamples = 0;
			bettyHandle = NULL;

			supportSoundBite = NULL;
			lastSupportId = -1;
			
			numSupportSamples = 0;
			supportHandle = NULL;
			
			isRaining = oldRaining = 0;
			sensorBeepUpdateTime = 0.0f;
			gamePaused = false;
		}
		
		int32_t init (PSTR soundFileName);
		virtual void destroy (void);
		
		void preloadSoundBite (int32_t sampleId);
		int32_t findOpenChannel (int32_t start, int32_t end);

		int32_t playDigitalSample (uint32_t sampleId, Stuff::Vector3D pos = Stuff::Vector3D(-9999.0f,-9999.0,-9999.0f), bool allowDupes = false);
		
		int32_t playDigitalMusic (int32_t musicId);
		
		int32_t playDigitalStream (PCSTR streamName);
											 
		bool isDigitalMusicPlaying (void);
		bool isPlayingSample(int32_t sampleId);
		bool isChannelPlaying (int32_t channelId);
		bool isPlayingVoiceOver (void);

		int32_t playBettySample (uint32_t bettySampleId);
		void stopBettySample (void);

		
		int32_t playSupportSample (uint32_t supportSampleId, PSTR fileName = NULL);
		
 		void stopDigitalSample (uint32_t sampleHandleNumber);
		void stopDigitalMusic (void);
		void stopSupportSample (void);
	
		void setDigitalMasterVolume (byte volume);
		int32_t getDigitalMasterVolume (void);
		
		uint8_t getSFXVolume(void);
		uint8_t getRadioVolume(void);
		uint8_t getMusicVolume(void);
		uint8_t getBettyVolume(void);

		void setSFXVolume(uint8_t volume);
		void setRadioVolume(uint8_t volume);
		void setMusicVolume(uint8_t volume);
		void setBettyVolume(uint8_t volume);

		virtual void update (void);
		
		void playABLDigitalMusic (int32_t musicId);
		void stopABLMusic (void);
		int32_t getCurrentMusicId (void)
		{
			return currentMusicId;
		}
		
		void playABLSFX (int32_t sfxId);
		void playABLVideo (int32_t videoId);
		void playABLSpeech (int32_t pilotId, int32_t speechId);
		
		int32_t playPilotSpeech (PSTR pilotSpeechFilename, int32_t msgId);
		
		void setIsRaining (uint32_t rainLevel)
		{
			isRaining = rainLevel;
		}
		
		void setIsPaused (void)
		{
			gamePaused = true;
		}
		
		void clearIsPaused (void)
		{
			gamePaused = false;
		}
};

//---------------------------------------------------------------------------

extern bool useSound;
extern bool useMusic;

//---------------------------------------------------------------------------
#endif
