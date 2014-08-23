#pragma once
//===========================================================================//
// File:	 Sound Channel.cpp												 //
// Contents: Direct Sound manager											 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "EZVector.hpp"
//
// Maximum number of available channels;
//
#define MAX_SOUNDCHANNELS 32
class SoundChannel
{
	public:
		float m_fVolume, m_fPanning, m_fFrequency;
		uint32_t m_currentProperties;
		uint32_t m_oldProperties;
		SoundResource * m_currentSoundResource;
		bool m_true3D;
		bool m_emulatedByGameOS;
		bool m_changed;

		EZVector m_position3D;
		EZVector m_velocity3D;
		EZVector m_orientation3D;

		struct
		{
			float inner;
			float outer;
		} m_innerOuterAngles;

		float m_distanceMin, m_distanceMax;
		bool m_bLoopFlag;
	    bool m_fCued;                  // semaphore (stream cued)
		bool m_fPlaying;               // semaphore (stream playing)
		bool m_fPaused;               // semaphore (stream playing)
	    uint32_t m_cbBufOffset;            // last write position
	    uint32_t m_nBufLength;             // length of sound buffer in msec
	    uint32_t m_cbBufSize;              // size of sound buffer in bytes
	    uint32_t m_nDuration;              // duration of wave file
	    uint32_t m_nTimeStarted;           // time (in system time) playback started
	    uint32_t m_nTimeElapsed;           // elapsed time in msec since playback started
		uint32_t m_myEnum;

		gosAudio_ResourceType m_resourceType;
		char m_resourceName[128];

	public:
		SoundChannel();
		virtual ~SoundChannel();
		virtual void Reset(uint32_t chan);
		virtual void CreateAndLoadBuffer( SoundResource * ) = 0;
		virtual void CreateStreamBuffer( SoundResource * ) = 0;
		virtual void CreateBuffer() = 0;
		virtual void Play() = 0;
		virtual void Pause() = 0;
		virtual void Stop() = 0;
		virtual void ClearAndFree() = 0;
		virtual void PlayStream() = 0;
		virtual void PauseStream() = 0;
		virtual void StopStream() = 0;
		virtual void Cue() = 0;
		virtual void WriteWaveData(uint32_t size) = 0;
		virtual uint32_t GetMaxWriteSize() = 0;
		virtual BOOL ServiceBuffer() = 0;

		virtual void SetVolume(float f);
		virtual void SetPan(float f);
		virtual void SetFrequency(float f);
		virtual void SetPosition(float x, float y, float z);
		virtual void SetVelocity(float x, float y, float z);
		virtual void SetDistanceMinMax(float min, float max);
		virtual void SetConeAngles( float min, float max );
		virtual void SetConeOrientation( float x, float y, float z );
		virtual bool IsPlaying() = 0;
		virtual void Fake3D() = 0;
		virtual uint32_t GetBytesPlayed() = 0;
};

 
