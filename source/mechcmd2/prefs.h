//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef PREFS_H
#define PREFS_H

// #include <mclib.h>

class CPrefs
{
public:
	CPrefs(void);
	int32_t load(PCSTR pFileName = "options");
	int32_t save(void);
	int32_t applyPrefs(bool bApplyResolution = 1);

	void setNewName(PCSTR pNewName);
	void setNewUnit(PCSTR pNewUnit);
	void setNewIP(PCSTR pNewIP);

public:
	int32_t DigitalMasterVolume;
	int32_t MusicVolume;
	int32_t sfxVolume;
	int32_t RadioVolume;
	int32_t BettyVolume;

	bool useShadows;
	bool useWaterInterestTexture;
	bool useHighObjectDetail;

	int32_t GameDifficulty;
	bool useUnlimitedAmmo;

	int32_t renderer;
	int32_t resolution;
	bool fullScreen;
	int32_t gammaLevel;
	bool useLeftRightMouseProfile; // if false, use old style commands
	int32_t baseColor;
	int32_t highlightColor;
	int32_t faction;
	char insigniaFile[256];
	char unitName[10][256];
	char playerName[10][256];
	char ipAddresses[10][24];

	bool pilotVideos;
	bool useNonWeaponEffects;
	bool useLocalShadows;
	bool asyncMouse;
	int32_t fogPos;
	char bitDepth; // 0 == 16, 1 == 32

	bool saveTranscripts;
	bool tutorials;

#if 0
	int32_t FilterState;
	int32_t TERRAIN_TXM_SIZE;
	int32_t ObjectTextureSize;
	bool	useRealLOS;
	float doubleClickThreshold;
	int32_t dragThreshold;
	uint32_t BaseVertexColor;		//This color is applied to all vertices in game as Brightness correction.
#endif
};

extern CPrefs prefs;
#endif /*PREFS_H*/
