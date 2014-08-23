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
	ULONG rgdwAxes[12];
	LONG rglDirection[12];
	FFEffectData * pNextEffect;
};

struct gosForceEffect
{
public:
	FFEffectData * ffEffectData;
	gosForce m_eType;
	gosForceEffect * pNext;
	ULONG dwStick;
	double longestDuration;
	double startTime;
	bool bStatus;
	bool fromFile;
public:
	gosForceEffect(gosJoystick_ForceEffect * fe, ULONG joy);
	gosForceEffect(PCSTR  fe, ULONG joy);
	void ReCreate();
	~gosForceEffect();
	void Play(ULONG times);
	void Stop();
	void Update(gosJoystick_ForceEffect * fe);
};