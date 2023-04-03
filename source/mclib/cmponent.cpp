//******************************************************************************************
//
//	cmponent.cp - This file contains the Component Class
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

//******************************************************************************************
#ifndef CMPONENT_H
#include "cmponent.h"
#endif

#ifndef HEAP_H
#include "heap.h"
#endif

#include "utilities.h"
//******************************************************************************************

std::wstring_view ComponentFormString[] = {"Simple", "Cockpit", "Sensors", "Actuator", "Engine", "HeatSink",
	"Weapon", "EnergyWeapon", "BallisticWeapon", "MissileWeapon", "Ammo", "JumpJet", "Case",
	"LifeSupport", "Gyroscope", "PowerAmplifier", "ECM", "Probe", "Jammer", "Bulk", nullptr};

std::wstring_view WeaponRangeString[] = {"int16_t", "medium", "int32_t"};

MasterComponentPtr MasterComponent::masterList = nullptr;
int32_t MasterComponent::numComponents = 0;
int32_t MasterComponent::armActuatorID = 4;
int32_t MasterComponent::legActuatorID = 5;
int32_t MasterComponent::clanAntiMissileSystemID = 115;
int32_t MasterComponent::innerSphereAntiMissileSystemID = 106;

#define COMPONENT_NAME_START 32000
#define COMPONENT_ABBR_START 33000
//******************************************************************************************
// MASTER COMPONENT routines
//******************************************************************************************

PVOID
MasterComponent::operator new(size_t mySize)
{
	PVOID result = systemHeap->Malloc(mySize);
	return (result);
}

//******************************************************************************************

void MasterComponent::operator delete(PVOID us)
{
	systemHeap->Free(us);
}

//******************************************************************************************

void MasterComponent::destroy(void)
{
}

//******************************************************************************************
int32_t
MasterComponent::initEXCEL(std::wstring_view dataLine, float baseSensorRange)
{
	//----------------------------------------------------------
	// Component data was read in, so parse it. First, parse the
	// fields common to all components...
	std::wstring_view next_token = nullptr;
	std::wstring_view field = strtok_s(dataLine, ",", &next_token);
	int32_t ammoAmount = 1;
	health = 1;
	masterID = atoi(field);
	cLoadString(COMPONENT_NAME_START + masterID, name, MAXLEN_COMPONENT_NAME);
	cLoadString(COMPONENT_ABBR_START + masterID, abbreviation, MAXLEN_COMPONENT_ABBREV);
	field = strtok_s(nullptr, ",", &next_token);
	int32_t formIndex;
	for (formIndex = 0; ComponentFormString[formIndex] != nullptr; formIndex++)
		if (strcmp(field, ComponentFormString[formIndex]) == 0)
			break;
	if (ComponentFormString[formIndex] == nullptr)
		return (-1);
	if (strcmp(field, "undefined") == 0)
	{
		masterID = -1;
		return (NO_ERROR);
	}
	form = (ComponentFormType)formIndex;
	field = strtok_s(nullptr, ",", &next_token); // name, ignore
	field = strtok_s(nullptr, ",", &next_token);
	size = atof(field);
	field = strtok_s(nullptr, ",", &next_token);
	float recycleTime = atof(field);
	field = strtok_s(nullptr, ",", &next_token);
	float heat = (uint32_t)atof(field);
	field = strtok_s(nullptr, ",", &next_token);
	tonnage = atof(field);
	field = strtok_s(nullptr, ",", &next_token);
	float damage = atof(field);
	field = strtok_s(nullptr, ",", &next_token);
	CV = atof(field);
	field = strtok_s(nullptr, ",", &next_token);
	resourcePoints = atoi(field);
	field = strtok_s(nullptr, ",", &next_token);
	uint8_t rangeType = 255;
	_Check_return_wat_ _CRTIMP errno_t __cdecl _strlwr_s(
		_Inout_updates_z_(_Size) std::wstring_view _Str, _In_ size_t _Size);
	_strlwr(field);
	if (strcmp(field, "0") != 0)
	{
		if (strcmp(field, WeaponRangeString[WEAPON_RANGE_SHORT]) == 0)
			rangeType = WEAPON_RANGE_SHORT;
		else if (strcmp(field, WeaponRangeString[WEAPON_RANGE_MEDIUM]) == 0)
			rangeType = WEAPON_RANGE_MEDIUM;
		else if (strcmp(field, WeaponRangeString[WEAPON_RANGE_LONG]) == 0)
			rangeType = WEAPON_RANGE_LONG;
		else
			Fatal(0,
				" MasterComponent.initEXCEL: bad weapon range type in "
				"compbase ");
	}
	field = strtok_s(nullptr, ",", &next_token);
	for (size_t location = 0; location < NUM_BODY_LOCATIONS; location++)
	{
		if (field)
		{
			if (strcmp(field, "No") == 0)
				criticalSpacesReq[location] = -1;
			else if (strcmp(field, "Yes") == 0)
				criticalSpacesReq[location] = 0;
			else
				criticalSpacesReq[location] = atoi(field);
		}
		field = strtok_s(nullptr, ",", &next_token);
	}
	int32_t ammoType = WEAPON_AMMO_NONE;
	if (field)
	{
		if (strcmp(field, "1") == 0)
			ammoType = WEAPON_AMMO_LIMITED;
		else if (strcmp(field, "SRM") == 0)
			ammoType = WEAPON_AMMO_SRM;
		else if (strcmp(field, "LRM") == 0)
			ammoType = WEAPON_AMMO_LRM;
		else if (strcmp(field, "ST") == 0)
			ammoType = WEAPON_AMMO_ST;
	}
	field = strtok(nullptr, ",");
	int32_t flags = 0;
	if (field)
		flags = atoi(field);
	field = strtok_s(nullptr, ",", &next_token);
	int32_t specialEffect = 0;
	if (field)
		specialEffect = (wchar_t)atoi(field);
	field = strtok_s(nullptr, ",", &next_token);
	int32_t ammoMasterId = 0;
	if (field)
		ammoMasterId = (wchar_t)atoi(field);
	switch (form)
	{
	case COMPONENT_FORM_SIMPLE:
		//----------------------
		// No additional data...
		break;
	case COMPONENT_FORM_COCKPIT:
		break;
	case COMPONENT_FORM_SENSOR:
		stats.sensor.range = damage * baseSensorRange;
		break;
	case COMPONENT_FORM_ECM:
		stats.ecm.effect = damage;
		field = strtok_s(nullptr, ",", &next_token);
		stats.ecm.range = recycleTime;
		break;
	case COMPONENT_FORM_JAMMER:
		stats.jammer.effect = damage;
		break;
	case COMPONENT_FORM_PROBE:
		stats.probe.effect = damage;
		break;
	case COMPONENT_FORM_ACTUATOR:
		break;
	case COMPONENT_FORM_ENGINE:
		stats.engine.rating = 0;
		break;
	case COMPONENT_FORM_HEATSINK:
		stats.heatsink.dissipation = damage;
		break;
	case COMPONENT_FORM_WEAPON_ENERGY:
	case COMPONENT_FORM_WEAPON_MISSILE:
	case COMPONENT_FORM_WEAPON_BALLISTIC:
		stats.weapon.damage = damage;
		stats.weapon.recycleTime = recycleTime;
		stats.weapon.heat = heat;
		stats.weapon.ammoAmount = 1;
		stats.weapon.ammoType = ammoType;
		stats.weapon.range = rangeType;
		stats.weapon.specialEffect = specialEffect;
		stats.weapon.ammoMasterId = ammoMasterId;
		stats.weapon.flags = ammoType == WEAPON_AMMO_ST ? WEAPON_FLAG_STREAK : 0;
		stats.weapon.ammoAmount = ammoAmount;
		break;
	case COMPONENT_FORM_AMMO:
		stats.ammo.ammoPerTon = recycleTime;
		stats.ammo.explosiveDamage = damage;
		break;
	case COMPONENT_FORM_JUMPJET:
		stats.jumpjet.rangeMod = damage;
		break;
	case COMPONENT_FORM_CASE:
		break;
	case COMPONENT_FORM_LIFESUPPORT:
		break;
	case COMPONENT_FORM_GYROSCOPE:
		break;
	case COMPONENT_FORM_POWER_AMPLIFIER:
		break;
	case COMPONENT_FORM_BULK:
		break;
	default:
		// return(-2);
		NODEFAULT;
	}
	return (NO_ERROR);
}

//******************************************************************************************
int32_t
MasterComponent::saveEXCEL(std::unique_ptr<File> componentFile, uint8_t masterId, float baseSensorRange)
{
	wchar_t dataLine[512];
	wchar_t piece[512];
	wchar_t comma[2] = ",";
	if (masterID == -1)
	{
		//---------------------------------------
		// Blank undefined line in compbas here.
		sprintf_s(dataLine, _countof(dataline),
			"0,undefined,undefined,0,0,0,0,0,0,0,0,0,0,0,0,No,No,0,No,0,0,0,0,"
			"na,na,na,na,na,na,na,na,na,,,,,");
		componentFile->writeLine(dataLine);
		return (0);
	}
	//----------------------------------------------------------
	// Build the dataline piece by piece
	sprintf_s(piece, _countof(piece), "%d", masterId);
	strcpy(dataLine, piece);
	strcat(dataLine, comma);
	cLoadString(COMPONENT_NAME_START + masterID, name, MAXLEN_COMPONENT_NAME);
	strcat(dataLine, name);
	strcat(dataLine, comma);
	cLoadString(COMPONENT_ABBR_START + masterID, abbreviation, MAXLEN_COMPONENT_ABBREV);
	strcat(dataLine, abbreviation);
	strcat(dataLine, comma);
	strcat(dataLine, ComponentFormString[form]);
	strcat(dataLine, comma);
	sprintf_s(piece, _countof(piece), "%d", size);
	strcat(dataLine, piece);
	strcat(dataLine, comma);
	sprintf_s(piece, _countof(piece), "1");
	strcat(dataLine, piece);
	strcat(dataLine, comma);
	sprintf_s(piece, _countof(piece), "%3.1f", tonnage);
	strcat(dataLine, piece);
	strcat(dataLine, comma);
	sprintf_s(piece, _countof(piece), "%d", resourcePoints);
	strcat(dataLine, piece);
	strcat(dataLine, comma);
	for (size_t location = 0; location < NUM_BODY_LOCATIONS; location++)
	{
		if (criticalSpacesReq[location] == -1)
			sprintf_s(piece, _countof(piece), "No");
		else if (criticalSpacesReq[location] == 0)
			sprintf_s(piece, _countof(piece), "Yes");
		else
			Fatal(criticalSpacesReq[location], "Bad Data");
		strcat(dataLine, piece);
		strcat(dataLine, comma);
	}
	if (getCanVehicleUse())
		sprintf_s(piece, _countof(piece), "Yes");
	else
		sprintf_s(piece, _countof(piece), "No");
	strcat(dataLine, piece);
	strcat(dataLine, comma);
	if (getCanMechUse())
		sprintf_s(piece, _countof(piece), "Yes");
	else
		sprintf_s(piece, _countof(piece), "No");
	strcat(dataLine, piece);
	strcat(dataLine, comma);
	if (getClanTechBase() && getISTechBase())
	{
		sprintf_s(piece, _countof(piece), "Both");
	}
	else if (getClanTechBase())
	{
		sprintf_s(piece, _countof(piece), "Clan");
	}
	else if (getISTechBase())
	{
		sprintf_s(piece, _countof(piece), "IS");
	}
	else
	{
		sprintf_s(piece, _countof(piece), "0");
	}
	strcat(dataLine, piece);
	strcat(dataLine, comma);
	if (getCanISUse())
		sprintf_s(piece, _countof(piece), "Yes");
	else
		sprintf_s(piece, _countof(piece), "No");
	strcat(dataLine, piece);
	strcat(dataLine, comma);
	sprintf_s(piece, _countof(piece), "0");
	strcat(dataLine, piece);
	strcat(dataLine, comma);
	sprintf_s(piece, _countof(piece), "1");
	strcat(dataLine, piece);
	strcat(dataLine, comma);
	sprintf_s(piece, _countof(piece), "%8.1f", CV);
	strcat(dataLine, piece);
	strcat(dataLine, comma);
	switch (form)
	{
	case COMPONENT_FORM_COCKPIT:
	case COMPONENT_FORM_ACTUATOR:
	case COMPONENT_FORM_SIMPLE:
	case COMPONENT_FORM_CASE:
	case COMPONENT_FORM_POWER_AMPLIFIER:
	case COMPONENT_FORM_GYROSCOPE:
	case COMPONENT_FORM_LIFESUPPORT:
	case COMPONENT_FORM_BULK:
	default:
		//----------------------
		// No additional data...
		sprintf_s(piece, _countof(piece), "na,na,na,na,na,na,na,na,na,,,,,");
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		break;
	case COMPONENT_FORM_SENSOR:
		sprintf_s(piece, _countof(piece), "%4.1f", float(stats.sensor.range / baseSensorRange));
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "na,na,na,na,na,na,na,na,,,,,");
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		break;
	case COMPONENT_FORM_ECM:
		sprintf_s(piece, _countof(piece), "%4.1f", stats.ecm.effect);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%4.1f", stats.ecm.range);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "na,na,na,na,na,na,na,,,,,");
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		break;
	case COMPONENT_FORM_JAMMER:
		sprintf_s(piece, _countof(piece), "%4.1f", stats.jammer.effect);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "na,na,na,na,na,na,na,na,,,,,");
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		break;
	case COMPONENT_FORM_PROBE:
		sprintf_s(piece, _countof(piece), "%4.1f", stats.probe.effect);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "na,na,na,na,na,na,na,na,,,,,");
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		break;
	case COMPONENT_FORM_ENGINE:
		sprintf_s(piece, _countof(piece), "%d", 0);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "na,na,na,na,na,na,na,na,,,,,");
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		break;
	case COMPONENT_FORM_HEATSINK:
		sprintf_s(piece, _countof(piece), "%4.1f", stats.heatsink.dissipation);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "na,na,na,na,na,na,na,na,,,,,");
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		break;
	case COMPONENT_FORM_WEAPON_ENERGY:
		sprintf_s(piece, _countof(piece), "%4.1f", stats.weapon.damage);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%4.2f", stats.weapon.recycleTime);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%4.1f", stats.weapon.heat);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%d", 0);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%d", WEAPON_AMMO_NONE);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%s", WeaponRangeString[stats.weapon.range]);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		// sprintf_s(piece,_countof(piece),"%d",stats.weapon.type);
		// strcat(dataLine,piece);
		// strcat(dataLine,comma);
		sprintf_s(piece, _countof(piece), "%d", stats.weapon.specialEffect);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%d", 0);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%d", stats.weapon.flags);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		break;
	case COMPONENT_FORM_WEAPON_BALLISTIC:
		sprintf_s(piece, _countof(piece), "%4.1f", stats.weapon.damage);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%4.2f", stats.weapon.recycleTime);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%4.1f", stats.weapon.heat);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%d", stats.weapon.ammoAmount);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%d", stats.weapon.ammoType);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%s", WeaponRangeString[stats.weapon.range]);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		//			sprintf_s(piece,_countof(piece),"%d",stats.weapon.type);
		//			strcat(dataLine,piece);
		//			strcat(dataLine,comma);
		sprintf_s(piece, _countof(piece), "%d", stats.weapon.specialEffect);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%d", stats.weapon.flags);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		break;
	case COMPONENT_FORM_WEAPON_MISSILE:
		sprintf_s(piece, _countof(piece), "%4.1f", stats.weapon.damage);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%4.2f", stats.weapon.recycleTime);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%4.1f", stats.weapon.heat);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%d", stats.weapon.ammoAmount);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%d", stats.weapon.ammoType);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%s", WeaponRangeString[stats.weapon.range]);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		//			sprintf_s(piece,_countof(piece),"%d",stats.weapon.type);
		//			strcat(dataLine,piece);
		//			strcat(dataLine,comma);
		sprintf_s(piece, _countof(piece), "%d", stats.weapon.specialEffect);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%d", stats.weapon.flags);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		break;
	case COMPONENT_FORM_AMMO:
		sprintf_s(piece, _countof(piece), "%d", stats.ammo.ammoPerTon);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "%6.1f", stats.ammo.explosiveDamage);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "na,na,na,na,na,na,na,,,,,");
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		break;
	case COMPONENT_FORM_JUMPJET:
		sprintf_s(piece, _countof(piece), "%6.2f", stats.jumpjet.rangeMod);
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		sprintf_s(piece, _countof(piece), "na,na,na,na,na,na,na,na,,,,,");
		strcat(dataLine, piece);
		strcat(dataLine, comma);
		break;
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

bool MasterComponent::isOffensiveWeapon(void)
{
	//----------------------------------------------------------------------
	// For now, we have just one defensive weapon--the anti-missile system.
	// So, we can pretty much special-case this routine. If we add others,
	// we will want to specify DEFENSIVE/OFFENSIVE for each weapon, and then
	// check against this field...
	return ((masterID != clanAntiMissileSystemID) && (masterID != innerSphereAntiMissileSystemID));
}

//---------------------------------------------------------------------------

bool MasterComponent::isDefensiveWeapon(void)
{
	//----------------------------------------------------------------------
	// For now, we have just one defensive weapon--the anti-missile system.
	// So, we can pretty much special-case this routine. If we add others,
	// we will want to specify DEFENSIVE/OFFENSIVE for each weapon, and then
	// check against this field...
	return ((masterID == clanAntiMissileSystemID) || (masterID == innerSphereAntiMissileSystemID));
}

//---------------------------------------------------------------------------

int32_t
MasterComponent::loadMasterList(std::wstring_view fileName, int32_t listSize, float baseSensorRange)
{
	if (masterList)
	{
		systemHeap->Free(masterList);
		masterList = nullptr;
	}
	numComponents = listSize;
	masterList = (MasterComponentPtr)systemHeap->Malloc(sizeof(MasterComponent) * numComponents);
	for (size_t curComponent = 0; curComponent < numComponents; curComponent++)
		masterList[curComponent].init();
	armActuatorID = -1;
	legActuatorID = -1;
	clanAntiMissileSystemID = -1;
	innerSphereAntiMissileSystemID = -1;
	//-----------------------------------------------------------------
	// All components are in one data file. Open it up, and read in the
	// comma-delimited data...
	MechFile componentFile;
	int32_t result = componentFile.open(fileName);
	if (result != NO_ERROR)
		return (result);
	//-----------------------
	// Special Component Data
	wchar_t dataLine[512];
	int32_t lineLength;
	lineLength = componentFile.readLine((uint8_t*)dataLine, 511);
	for (size_t componentNum = 0; componentNum < numComponents; componentNum++)
	{
		//----------------------------------------
		// Read in the line from the table file...
		lineLength = componentFile.readLine((uint8_t*)dataLine, 511);
		if (!lineLength)
			return (-1);
		masterList[componentNum].initEXCEL(dataLine, baseSensorRange);
	}
	componentFile.close();
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
MasterComponent::saveMasterList(std::wstring_view fileName, int32_t listSize, float baseSensorRange)
{
	//-----------------------------------------------------------------
	// All components are in one data file. Save it in CSV format!
	MechFile componentFile;
	int32_t result = componentFile.create(fileName);
	if (result != NO_ERROR)
		return (result);
	//----------------------------------------------------
	// Comment header so we know who screwed the file up!
	wchar_t dataLine[512];
	sprintf(dataLine,
		"// <Built by Editor v3.0> Special Component "
		"Data,,,,,******REVISED FOR NEW MECH "
		"STATS,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
	componentFile.writeLine(dataLine);
	//-----------------------
	// Special Component Data
	sprintf(
		dataLine, "MasterArmActuatorID = %d,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", armActuatorID);
	componentFile.writeLine(dataLine);
	sprintf(
		dataLine, "MasterLegActuatorID = %d,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", legActuatorID);
	componentFile.writeLine(dataLine);
	sprintf(dataLine,
		"MasterClanAntiMissileSystemID = "
		"115,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
		clanAntiMissileSystemID);
	componentFile.writeLine(dataLine);
	sprintf(dataLine,
		"MasterInnerSphereAntiMissileSystemID = "
		"106,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
		innerSphereAntiMissileSystemID);
	componentFile.writeLine(dataLine);
	sprintf(dataLine,
		"//"
		",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,Weapon,Effect,Ammo,Flag,"
		"1 = streak 2 = inferno");
	componentFile.writeLine(dataLine);
	sprintf(dataLine,
		"// Component "
		"Table,,abbr,type,crits,?,tons,RP,head,CT,LT,RT,LA,RA,LL,"
		"RL,Vehicle?,Fit both?,Side,Fit "
		"IS?,art,disable,BR,Damage,Recycle,heat,#miss,miss "
		"type,min,int16_t,med,int32_t,Type,Field,Master "
		"ID,Fields,4 = LBX 8 = artillery");
	componentFile.writeLine(dataLine);
	for (size_t componentNum = 0; componentNum < listSize; componentNum++)
	{
		masterList[componentNum].saveEXCEL(&componentFile, componentNum, baseSensorRange);
	}
	componentFile.close();
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MasterComponent::freeMasterList(void)
{
	if (masterList)
	{
		systemHeap->Free(masterList);
		masterList = nullptr;
		numComponents = 0;
	}
	return (NO_ERROR);
}

//***************************************************************************
