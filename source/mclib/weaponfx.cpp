//---------------------------------------------------------------------
// Weapon FX equivalency Table for Mechs/Vehicles/Whatever
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"

//---------------------------------------------------------------------------------
// Include Files
// #include "mclib.h"

#include "weaponfx.h"
//---------------------------------------------------------------------------------
// weapon FX Equivalancy.  Stores names now for GOS FX
WeaponEffects* weaponEffects = nullptr;
wchar_t mc2_word_none[5] = "NONE";

//---------------------------------------------------------------------------------
void
WeaponEffects::destroy(void)
{
	systemHeap->Free(effects);
	effects = nullptr;
	numEffects = 0;
}

//---------------------------------------------------------------------------------
void
WeaponEffects::init(const std::wstring_view& effectCSVFileName)
{
	FullPathFileName effectsName;
	effectsName.init(objectPath, effectCSVFileName, ".csv");
	CSVFile effectFile;
	int32_t result = effectFile.open(effectsName);
	if (result != NO_ERROR)
		STOP(("Unable to open Effects File %s", effectsName));
	numEffects = effectFile.getNumLines() - 1; // Always subtract one for the column headers
	effects = (EffectData*)systemHeap->Malloc(sizeof(EffectData) * numEffects);
	gosASSERT(effects != nullptr);
	for (size_t i = 0; i < numEffects; i++)
	{
		effectFile.readString(i + 2, 2, effects[i].effectName, 49);
		effectFile.readString(i + 2, 3, effects[i].muzzleFlashName, 49);
		effectFile.readString(i + 2, 4, effects[i].hitEffectName, 49);
		effectFile.readString(i + 2, 6, effects[i].missEffectName, 49);
		effectFile.readLong(i + 2, 5, effects[i].effectObjNum);
	}
}

//---------------------------------------------------------------------------------
