//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

struct enumDevice
{
	GUID deviceGUID;
	uint32_t iter;
};

class DS3DSoundMixer
{
public:
	LPDIRECTSOUND3DLISTENER m_lpListener;
	LPDIRECTSOUNDBUFFER m_lpPrimaryBuffer;
	LPDIRECTSOUND3DBUFFER m_lpEffectsBuffer;
	LPDIRECTSOUND m_lpDirectSound;
	DSCAPS m_dscaps;
	DSBCAPS m_mixerCaps;
	DS3DLISTENER m_Attributes3D;

	WAVEFORMATEX m_waveFormatEx;

	float m_fVolume;
	float m_reverbFactor, m_decayFactor;
	uint32_t m_properties;
	bool m_changed;

public:
	DS3DSoundMixer(void);
	~DS3DSoundMixer(void);
	void Reset(void);
	void Flush(void);
	void SetVolume(float f);
	void SetPosition(float x, float y, float z);
	void SetVelocity(float x, float y, float z);
	void SetFrontOrientation(float x, float y, float z);
	void SetTopOrientation(float x, float y, float z);
	void SetDopplerFactor(float df);
	void SetRolloffFactor(float rf);
	void SetDistanceFactor(float df);
	void SetReverbFactor(float rf);
	void SetDecayFactor(float df);
	void GetCaps(void);
	void SetSpeakerConfig(uint32_t);
	void GetSpeakerConfig(uint32_t*);
	void ReturnFormattedDebugData(PSTR dst, uint32_t maxlen);
	void SetAllParameters(void);
};
