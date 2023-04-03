//***************************************************************************
//
//	cmponent.h -- File contains the Component definition
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef CMPONENT_H
#define CMPONENT_H

//---------------------------------------------------------------------------
#ifndef INIFILE_H
#include "inifile.h"
#endif

#ifndef ERR_H
#include "err.h"
#endif

#include <string.h>

#ifndef _MBCS
//#include "gameos.hpp"
#else
#include <_ASSERT.h>
#define gosASSERT _ASSERT
#define gos_Malloc malloc
#define gos_Free free
#endif

//***************************************************************************

#define MAXLEN_COMPONENT_NAME 256
#define MAXLEN_COMPONENT_ABBREV 256

#define NUM_BODY_LOCATIONS 8

#define TECH_BASE_CLAN 1
#define TECH_BASE_INNERSPHERE 2

#define COMPONENT_USE_MECH 1
#define COMPONENT_USE_VEHICLE 2
#define COMPONENT_USE_CLAN 16
#define COMPONENT_USE_INNERSPHERE 32

#define WEAPON_AMMO_OFFSET 65 // Master Component Index Offset from weapon

enum class
{
	COMPONENT_FORM_SIMPLE = 0,
	COMPONENT_FORM_COCKPIT,
	COMPONENT_FORM_SENSOR,
	COMPONENT_FORM_ACTUATOR,
	COMPONENT_FORM_ENGINE,
	COMPONENT_FORM_HEATSINK,
	COMPONENT_FORM_WEAPON,
	COMPONENT_FORM_WEAPON_ENERGY,
	COMPONENT_FORM_WEAPON_BALLISTIC,
	COMPONENT_FORM_WEAPON_MISSILE,
	COMPONENT_FORM_AMMO,
	COMPONENT_FORM_JUMPJET,
	COMPONENT_FORM_CASE,
	COMPONENT_FORM_LIFESUPPORT,
	COMPONENT_FORM_GYROSCOPE,
	COMPONENT_FORM_POWER_AMPLIFIER,
	COMPONENT_FORM_ECM,
	COMPONENT_FORM_PROBE,
	COMPONENT_FORM_JAMMER,
	COMPONENT_FORM_BULK,
	NUM_COMPONENT_FORMS
} ComponentFormType;

enum class
{
	WEAPON_AMMO_NONE, // unlimited ammo (energy weapons)
	WEAPON_AMMO_SRM,
	WEAPON_AMMO_LRM,
	WEAPON_AMMO_ST,
	WEAPON_AMMO_LIMITED, // not SRM, LRM or ST, but still finite ammo amount
	NUM_WEAPON_AMMO_TYPES
} WeaponAmmoType;

enum class
{
	WEAPON_STATE_READY,
	WEAPON_STATE_RECYCLING,
	WEAPON_STATE_DAMAGED,
	WEAPON_STATE_DESTROYED
} WeaponStateType;

enum class
{
	WEAPON_RANGE_SHORT,
	WEAPON_RANGE_MEDIUM,
	WEAPON_RANGE_LONG,
	WEAPON_RANGE_SHORT_MEDIUM,
	WEAPON_RANGE_MEDIUM_LONG,
	NUM_WEAPON_RANGE_TYPES
} WeaponRangeType;

//******************************************************************************************

#define WEAPON_FLAG_STREAK 1
#define WEAPON_FLAG_INFERNO 2
#define WEAPON_FLAG_LBX 4
#define WEAPON_FLAG_ARTILLERY 8

typedef union
{
	struct
	{
		float range; // in meters
	} sensor;
	struct
	{
		float range;
		float effect;
	} ecm;
	struct
	{
		float effect;
	} jammer;
	struct
	{
		float effect;
	} probe;
	struct
	{
		int16_t rating;
	} engine;
	struct
	{
		int16_t dissipation;
	} heatsink;
	struct
	{
		float heat; // how much heat does it generate?
		float damage; // amount of damage
		float recycleTime; // in seconds
		int32_t ammoAmount; // amount of ammo per shot
		uint8_t ammoType; // 0 = unlimited, 1 = finite amount (e.g. bullets)
		uint8_t ammoMasterId; // ammo used by this weapon
		uint8_t range; // int16_t, med or int32_t
		uint8_t flags;
		int16_t type; // which weapon type is this
		wchar_t specialEffect; // used to cue whatever visual/sound effects this
			// needs
	} weapon;
	struct
	{
		int32_t ammoPerTon; // ammo per ton
		float explosiveDamage; // damage done (per missile) if it explodes
	} ammo;
	struct
	{
		int32_t rangeMod; // range modifier
	} jumpjet;
} ComponentStats;

//******************************************************************************************

class MasterComponent;
typedef MasterComponent* MasterComponentPtr;

class MasterComponent
{

private:
	int32_t masterID; // unique ID for component
	int32_t resourcePoints; // resource cost of object
	ComponentFormType form; // form of component
	wchar_t name[MAXLEN_COMPONENT_NAME]; // name string/description
	wchar_t abbreviation[MAXLEN_COMPONENT_ABBREV]; // abbreviated name
	float tonnage; // in tons
	wchar_t size; // # of total spaces used
	wchar_t health; // # of hits before destroyed
	wchar_t criticalSpacesReq[NUM_BODY_LOCATIONS]; // # of critical spaces required
		// in specific location
	wchar_t disableLevel; // # of critical spaces to disable
	uint8_t uses;
	uint8_t techBase;
	float CV; // CV for this component
	ComponentStats stats;
	uint32_t art;

public:
	static MasterComponentPtr masterList;
	static int32_t numComponents;
	static int32_t armActuatorID;
	static int32_t legActuatorID;
	static int32_t clanAntiMissileSystemID;
	static int32_t innerSphereAntiMissileSystemID;

public:
	PVOID operator new(size_t mySize);

	void operator delete(PVOID us);

	void destroy(void);

	~MasterComponent(void)
	{
		destroy(void);
	}

	void init(void)
	{
		masterID = -1;
		name[0] = nullptr;
		abbreviation[0] = nullptr;
	}

	MasterComponent(void)
	{
		init(void);
	}

	int32_t init(FitIniFile* componentFile);

	int32_t initEXCEL(std::wstring_view dataLine, float baseSensorRange);

	int32_t saveEXCEL(std::unique_ptr<File> file, uint8_t masterId, float baseSensorRange);

	std::wstring_view getName(void)
	{
		return (&name[0]);
	}

	int32_t getMasterID(void)
	{
		return (masterID);
	}

	int32_t getResourcePoints(void)
	{
		return (resourcePoints);
	}

	ComponentFormType getForm(void)
	{
		return (form);
	}

	int32_t getSize(void)
	{
		return (size);
	}

	void setSize(int32_t sz)
	{
		size = sz;
	}

	int32_t getHealth(void)
	{
		return (health);
	}

	wchar_t getCriticalSpacesReq(int32_t location)
	{
		if ((location < 0) && (location > NUM_BODY_LOCATIONS))
			return -1;
		return (criticalSpacesReq[location]);
	}

	void setCriticalSpacesReq(int32_t location, wchar_t value)
	{
		if ((location < 0) && (location > NUM_BODY_LOCATIONS))
			return;
		criticalSpacesReq[location] = value;
	}

	wchar_t getDisableLevel(void)
	{
		return (disableLevel);
	}

	float getCV(void)
	{
		return (CV);
	}

	void setCV(float cv)
	{
		CV = cv;
	}

	float getTonnage(void)
	{
		return (tonnage);
	}

	void setTonnage(float tons)
	{
		tonnage = tons;
	}

	float getHeatDissipation(void)
	{
		return (stats.heatsink.dissipation);
	}

	void setHeatDissipation(float heatD)
	{
		stats.heatsink.dissipation = heatD;
	}

	float getWeaponHeat(void)
	{
		return (stats.weapon.heat);
	}

	void setWeaponHeat(float weaponHeat)
	{
		stats.weapon.heat = weaponHeat;
	}

	float getWeaponDamage(void)
	{
		return (stats.weapon.damage);
	}

	void setWeaponDamage(float weapDmg)
	{
		stats.weapon.damage = weapDmg;
	}

	float getWeaponRecycleTime(void)
	{
		return (stats.weapon.recycleTime);
	}

	void setWeaponRecycleTime(float recycleTime)
	{
		stats.weapon.recycleTime = recycleTime;
	}

	int32_t getWeaponAmmoAmount(void)
	{
		return (stats.weapon.ammoAmount);
	}

	void setWeaponAmmoAmount(int32_t weaponAmmo)
	{
		stats.weapon.ammoAmount = weaponAmmo;
	}

	uint32_t getWeaponAmmoType(void)
	{
		return (stats.weapon.ammoType);
	}

	void setWeaponAmmoType(int32_t ammoType)
	{
		stats.weapon.ammoType = ammoType;
	}

	uint32_t getWeaponAmmoMasterId(void)
	{
		return (stats.weapon.ammoMasterId);
	}

	void setWeaponAmmoMasterId(int32_t ammoId)
	{
		stats.weapon.ammoMasterId = ammoId;
	}

	int32_t getWeaponRange(void)
	{
		return (stats.weapon.range);
	}

	void setWeaponRange(int32_t weaponRange)
	{
		stats.weapon.range = weaponRange;
	}

	void clearWeaponFlags(void)
	{
		stats.weapon.flags = 0;
	}

	bool getWeaponInferno(void)
	{
		return ((stats.weapon.flags & WEAPON_FLAG_INFERNO) != 0);
	}

	void setWeaponInferno(void)
	{
		stats.weapon.flags |= WEAPON_FLAG_INFERNO;
	}

	bool getWeaponStreak(void)
	{
		return ((stats.weapon.flags & WEAPON_FLAG_STREAK) != 0);
	}

	void setWeaponStreak(void)
	{
		stats.weapon.flags |= WEAPON_FLAG_STREAK;
	}

	bool getWeaponLBX(void)
	{
		return ((stats.weapon.flags & WEAPON_FLAG_LBX) != 0);
	}

	void setWeaponLBX(void)
	{
		stats.weapon.flags |= WEAPON_FLAG_LBX;
	}

	bool getWeaponArtillery(void)
	{
		return ((stats.weapon.flags & WEAPON_FLAG_ARTILLERY) != 0);
	}

	void setWeaponArtillery(void)
	{
		stats.weapon.flags |= WEAPON_FLAG_ARTILLERY;
	}

	bool getCanClanUse(void)
	{
		return ((uses & COMPONENT_USE_CLAN) != 0);
	}

	bool getCanISUse(void)
	{
		return ((uses & COMPONENT_USE_INNERSPHERE) != 0);
	}

	bool getCanMechUse(void)
	{
		return ((uses & COMPONENT_USE_MECH) != 0);
	}

	bool getCanVehicleUse(void)
	{
		return ((uses & COMPONENT_USE_VEHICLE) != 0);
	}

	void clearUseFlags(void)
	{
		uses = 0;
	}

	void setCanClanUse(void)
	{
		uses |= COMPONENT_USE_CLAN;
	}

	void setCanISUse(void)
	{
		uses |= COMPONENT_USE_INNERSPHERE;
	}

	void setCanVehicleUse(void)
	{
		uses |= COMPONENT_USE_VEHICLE;
	}

	void setCanMechUse(void)
	{
		uses |= COMPONENT_USE_MECH;
	}

	bool getClanTechBase(void)
	{
		return ((techBase & TECH_BASE_CLAN) != 0);
	}

	bool getISTechBase(void)
	{
		return ((techBase & TECH_BASE_INNERSPHERE) != 0);
	}

	void setClanTechBase(void)
	{
		techBase = TECH_BASE_CLAN;
	}

	void setISTechBase(void)
	{
		techBase = TECH_BASE_INNERSPHERE;
	}

	void setBothTechBase(void)
	{
		techBase = TECH_BASE_INNERSPHERE + TECH_BASE_CLAN;
	}

	//		int32_t getWeaponType (void) {
	//			return(stats.weapon.type);
	//		}

	int32_t getWeaponSpecialEffect(void)
	{
		return (stats.weapon.specialEffect);
	}

	int32_t getAmmoPerTon(void)
	{
		return (stats.ammo.ammoPerTon);
	}

	void setAmmoPerTon(int32_t ammoPerTon)
	{
		stats.ammo.ammoPerTon = ammoPerTon;
	}

	float getJumpJetRangeMod(void)
	{
		return (stats.jumpjet.rangeMod);
	}

	void setJumpJetRangeMod(float rangeMod)
	{
		stats.jumpjet.rangeMod = rangeMod;
	}

	float getAmmoExplosiveDamage(void)
	{
		return (stats.ammo.explosiveDamage);
	}

	void setAmmoExplosiveDamage(float ammoDamage)
	{
		stats.ammo.explosiveDamage = ammoDamage;
	}

	float getSensorRange(void)
	{
		return (stats.sensor.range);
	}

	uint8_t getTechBase(void)
	{
		return (techBase);
	}

	void setResourcePoints(int32_t points)
	{
		resourcePoints = points;
	}

	void setSensorRange(float range)
	{
		stats.sensor.range = (int16_t)range;
	}

	float getEcmRange(void)
	{
		return (stats.ecm.range);
	}

	void setEcmRange(float range)
	{
		stats.ecm.range = range;
	}

	float getEcmEffect(void)
	{
		return (stats.ecm.effect);
	}

	void setEcmEffect(float effect)
	{
		stats.ecm.effect = effect;
	}

	float getJammerEffect(void)
	{
		return (stats.jammer.effect);
	}

	void setJammerEffect(float effect)
	{
		stats.jammer.effect = effect;
	}

	float getProbeEffect(void)
	{
		return (stats.probe.effect);
	}

	void setProbeEffect(float effect)
	{
		stats.probe.effect = effect;
	}

	std::wstring_view getAbbreviation(void)
	{
		return (&abbreviation[0]);
	}

	bool isOffensiveWeapon(void);

	bool isDefensiveWeapon(void);

	void multiplyWeaponRanges(float factor);

	static int32_t loadMasterList(std::wstring_view fileName, int32_t numComponents, float baseSensorRange);
	static int32_t saveMasterList(std::wstring_view fileName, int32_t numComponents, float baseSensorRange);

	static int32_t freeMasterList(void);

	static int32_t multiplyMasterListWeaponRanges(float factor);
};

#endif

//******************************************************************************************
