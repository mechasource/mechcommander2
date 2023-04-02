#pragma once
//===========================================================================//
// File:	 Sound DS3DChannel.cpp 											 //
// Contents: Direct Sound Channel											 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//
// Maximum number of available channels;
//
class DS3DSoundChannel
{
public:
	LPDIRECTSOUNDBUFFER m_Buffer;
	LPDIRECTSOUND3DBUFFER m_Buffer3D;
	DSBCAPS m_dsbCaps;
	DS3DBUFFER m_Attributes3D;

	float m_fVolume, m_fPanning, m_fFrequency;
	uint32_t m_currentProperties;
	uint32_t m_oldProperties;
	SoundResource* m_currentSoundResource;
	BOOLEAN m_true3D;
	BOOLEAN m_changed;
	BOOLEAN m_sysPause;

	uint32_t m_previousPlayPosition;
	uint32_t m_playPosition;

	BOOLEAN m_bLoopFlag;
	BOOLEAN m_fCued; // semaphore (stream cued)
	BOOLEAN m_fPlaying; // semaphore (stream playing)
	BOOLEAN m_fPaused; // semaphore (stream playing)
	uint32_t m_cbBufOffset; // last write position
	uint32_t m_nBufLength; // length of sound buffer in msec
	uint32_t m_cbBufSize; // size of sound buffer in bytes
	float m_fDuration; // duration of wave file
	uint32_t m_myEnum;
	BOOLEAN playPending;

	BOOLEAN bIsDuplicate;
	BOOLEAN bOnly2D;

	gosAudio_ResourceType m_resourceType;
	char m_resourceName[128];

public:
	DS3DSoundChannel(void);
	~DS3DSoundChannel(void);
	void Reset(uint32_t chan);
	void CreateAndLoadBuffer(SoundResource*);
	void CreateStreamBuffer(SoundResource*);
	void CreateBuffer(void);
	void Play(void);
	void Pause(void);
	void Stop(void);
	void ClearAndFree(void);
	void PlayStream(void);
	void PauseStream(void);
	void StopStream(void);
	void Cue(void);
	void WriteWaveData(uint32_t size);
	uint32_t GetMaxWriteSize(void);
	BOOL ServicePlayList(void);
	void SetVolume(float f);
	void SetPan(float f);
	void SetFrequency(float f);
	void SetPosition(float x, float y, float z);
	void SetVelocity(float x, float y, float z);
	void SetDistanceMinMax(float min, float max);
	BOOLEAN IsPlaying(void);
	uint32_t GetBytesPlayed(void);
	void SpewCaps(void);
	void ReturnFormattedDebugData(PSTR dst, uint32_t maxlen);
	void SetConeAngles(uint32_t min, uint32_t max);
	void SetConeOrientation(float x, float y, float z);
	void SetAllParameters(void);
	void ReleaseCurrent(void);
};
