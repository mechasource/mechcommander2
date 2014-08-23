#pragma once
//===========================================================================//
// File:	 ForceFeedback.hpp												 //
// Contents: ForceFeedback routines											 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#define GROUP_FORCEFEEDBACK "Group_ForceFeedback"

struct FFEffectData
{
	GUID diGUID;
	DIEFFECT diEffectData;
	LPDIRECTINPUTEFFECT diEffect;
	uint8_t diTypeSpecific[64];
	DIENVELOPE diEnvelope;
	uint32_t rgdwAxes[12];
	int32_t rglDirection[12];
	FFEffectData * pNextEffect;
};

struct gosForceEffect
{
public:
	FFEffectData * ffEffectData;
	gosForce m_eType;
	gosForceEffect * pNext;
	uint32_t dwStick;
	double longestDuration;
	double startTime;
	bool bStatus;
	bool fromFile;
public:
	gosForceEffect(gosJoystick_ForceEffect * fe, uint32_t joy);
	gosForceEffect(PCSTR  fe, uint32_t joy);
	void ReCreate();
	~gosForceEffect();
	void Play(uint32_t times);
	void Stop();
	void Update(gosJoystick_ForceEffect * fe);
};