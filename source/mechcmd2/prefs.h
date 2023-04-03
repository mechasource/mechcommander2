//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef PREFS_H
#define PREFS_H

// #include "mclib.h"

class CPrefs
{
public:
	CPrefs(void);
	int32_t load(std::wstring_view pFileName = "options");
	int32_t save(void);
	int32_t applyPrefs(bool bApplyResolution = 1);

	void setNewName(std::wstring_view pNewName);
	void setNewUnit(std::wstring_view pNewUnit);
	void setNewIP(std::wstring_view pNewIP);

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
	int32_t basecolour;
	int32_t highlightcolour;
	int32_t faction;
	wchar_t insigniaFile[256];
	wchar_t unitName[10][256];
	wchar_t playerName[10][256];
	wchar_t ipAddresses[10][24];

	bool pilotVideos;
	bool useNonWeaponEffects;
	bool useLocalShadows;
	bool asyncMouse;
	int32_t fogPos;
	wchar_t bitDepth; // 0 == 16, 1 == 32

	bool saveTranscripts;
	bool tutorials;

#if 0
	int32_t FilterState;
	int32_t TERRAIN_TXM_SIZE;
	int32_t ObjectTextureSize;
	bool	useRealLOS;
	float doubleClickThreshold;
	int32_t dragThreshold;
	uint32_t BaseVertexcolour;		//This color is applied to all vertices in game as Brightness correction.
#endif
};

extern CPrefs prefs;
#endif /*PREFS_H*/
