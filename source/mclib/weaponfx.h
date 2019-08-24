//---------------------------------------------------------------------
// Weapon FX equivalency Table for Mechs/Vehicles/Whatever
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef WEAPONFX_H
#define WEAPONFX_H
//---------------------------------------------------------------------------------
// weapon FX Equivalancy.  Now reads in data from CSV for GOS Effects
#define BUILDING_EXPLOSION_ID 45
#define VEHICLE_EXPLOSION_ID 40
#define SPLASH_VEHICLE_EXPLOSION_ID 46
#define DFA_BOOM_EFFECT 47
#define MECH_EXPLOSION_ID 39
#define MECH_CRITICAL_HIT_ID 35
#define MECH_SMOKE_ID 48
#define TURRET_EXPLOSION_ID 44
#define MECH_DISABLE_EXPLOSION_ID 49
#define LARGE_AIRSTRIKE_ID 50
#define SMALL_AIRSTRIKE_ID 51
#define SENSOR_AIRSTRIKE_ID 72
#define MINE_EXPLOSION_ID 53
#define EMPTY_EXPLOSION_ID 54
#define JET_CONTRAIL_ID 58

#define ARM_FLYING_ID 61
#define SHOULDER_POP_ID 62
#define CRITICAL_SMOKE_ID 65
#define MECH_WATER_WAKE 34
#define VEHICLE_WATER_WAKE 66
#define VEHICLE_DUST_CLOUD 70
#define VTOL_DUST_CLOUD 56
#define KARNOV_DUST_CLOUD 73
#define KARNOV_RECOVERY_BEAM 69
#define MINELAYER_EFFECT_ID 68
#define REPAIR_VEHICLE_EFFECT_ID 57
#define HELICOPTER_DUST_CLOUD 67
#define TURRET_POWER_DOWN_EFFECT 75
#define WATER_MISS_FX 76
#define INFANTRY_BOOM_EFFECT 55
#define INFANTRY_BLOOD_EFFECT 82

extern char mc2_word_none[];

typedef struct _EffectData
{
	char effectName[50];
	char muzzleFlashName[50];
	char hitEffectName[50];
	char missEffectName[50];
	int32_t effectObjNum;
} EffectData;

class WeaponEffects
{
	// Data Members
	//-------------
protected:
	EffectData* effects;
	int32_t numEffects;

	// Member Functions
	//----------------
public:
	WeaponEffects(void) { init(void); }

	~WeaponEffects(void) { destroy(void); }

	void init(void)
	{
		effects = nullptr;
		numEffects = 0;
	}

	void destroy(void);

	void init(const std::wstring_view& effectCSVFileName);

	const std::wstring_view& GetEffectName(int32_t effectId)
	{
		if ((effectId >= 0) && (effectId < numEffects))
		{
			return effects[effectId].effectName;
		}
		return mc2_word_none;
	}

	const std::wstring_view& GetEffectMuzzleFlashName(int32_t effectId)
	{
		if ((effectId >= 0) && (effectId < numEffects))
		{
			return effects[effectId].muzzleFlashName;
		}
		return mc2_word_none;
	}

	const std::wstring_view& GetEffectHitName(int32_t effectId)
	{
		if ((effectId >= 0) && (effectId < numEffects))
		{
			return effects[effectId].hitEffectName;
		}
		return mc2_word_none;
	}

	const std::wstring_view& GetEffectMissName(int32_t effectId)
	{
		if ((effectId >= 0) && (effectId < numEffects))
		{
			return effects[effectId].missEffectName;
		}
		return mc2_word_none;
	}

	int32_t GetEffectObjNum(int32_t effectId)
	{
		if ((effectId >= 0) && (effectId < numEffects))
		{
			return effects[effectId].effectObjNum;
		}
		return -1;
	}
};

extern WeaponEffects* weaponEffects;
//---------------------------------------------------------------------------------
#endif