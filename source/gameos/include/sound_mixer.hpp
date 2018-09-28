//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#include "EZVector.hpp"

class SoundMixer
{
public:
	WAVEFORMATEX m_waveFormatEx;

	float m_dopplerFactor, m_rolloffFactor, m_distanceFactor;
	float m_fVolume;
	float m_reverbFactor, m_decayFactor;
	uint32_t m_properties;
	bool m_isCertified;
	uint32_t m_freeHW3DChannels;
	uint32_t m_totalHW3DChannels;
	uint32_t m_freeHWMemory;
	uint32_t m_totalHWMemory;

	EZVector m_position3D;
	EZVector m_velocity3D;
	EZVector m_orientation3DTop;
	EZVector m_orientation3DFront;

	bool m_changed;

public:
	SoundMixer(void);
	virtual ~SoundMixer(void);
	virtual void GetHardwareData(void);
	virtual void Pause(void);
	virtual void Continue(void);
	virtual void Clear(void);
	virtual void Flush(void);
	virtual void Reset(void);
	virtual void SetVolume(float f);
	virtual void SetPosition(float x, float y, float z);
	virtual void SetVelocity(float x, float y, float z);
	virtual void SetFrontOrientation(float x, float y, float z);
	virtual void SetTopOrientation(float x, float y, float z);
	virtual void SetDopplerFactor(float df);
	virtual void SetRolloffFactor(float rf);
	virtual void SetDistanceFactor(float df);
	virtual void SetReverbFactor(float rf);
	virtual void SetDecayFactor(float df);
	virtual void SetSpeakerConfig(uint32_t)  = 0;
	virtual void GetSpeakerConfig(uint32_t*) = 0;
};
