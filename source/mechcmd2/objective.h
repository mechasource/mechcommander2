/*************************************************************************************************\
Objective.h			: Interface for the Objective component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef OBJECTIVE_H
#define OBJECTIVE_H

//#include "elist.h"
//#include "estring.h"
//#include "tchar.h"
//#include "mover.h"
//#include "bldng.h"
//#include "gametacmap.h"

class FitIniFile;
class aFont;

/**************************************************************************************************
CLASS DESCRIPTION
CObjectiveCondition:
**************************************************************************************************/
enum condition_species_type
{
	DESTROY_ALL_ENEMY_UNITS,
	DESTROY_NUMBER_OF_ENEMY_UNITS,
	DESTROY_ENEMY_UNIT_GROUP,
	DESTROY_SPECIFIC_ENEMY_UNIT,
	DESTROY_SPECIFIC_STRUCTURE,

	CAPTURE_OR_DESTROY_ALL_ENEMY_UNITS,
	CAPTURE_OR_DESTROY_NUMBER_OF_ENEMY_UNITS,
	CAPTURE_OR_DESTROY_ENEMY_UNIT_GROUP,
	CAPTURE_OR_DESTROY_SPECIFIC_ENEMY_UNIT,
	CAPTURE_OR_DESTROY_SPECIFIC_STRUCTURE,

	DEAD_OR_FLED_ALL_ENEMY_UNITS,
	DEAD_OR_FLED_NUMBER_OF_ENEMY_UNITS,
	DEAD_OR_FLED_ENEMY_UNIT_GROUP,
	DEAD_OR_FLED_SPECIFIC_ENEMY_UNIT,

	CAPTURE_UNIT,
	CAPTURE_STRUCTURE,

	GUARD_SPECIFIC_UNIT,
	GUARD_SPECIFIC_STRUCTURE,

	MOVE_ANY_UNIT_TO_AREA,
	MOVE_ALL_UNITS_TO_AREA,
	MOVE_ALL_SURVIVING_UNITS_TO_AREA,
	MOVE_ALL_SURVIVING_MECHS_TO_AREA,

	BOOLEAN_FLAG_IS_SET,
	ELAPSED_MISSION_TIME,

	NUM_CONDITION_SPECIES
};

enum MARKER_TYPES
{
	NO_MARKER = 0,
	NAV = 1,
	NUMERIC = 2
};

static std::wstring_view g_conditionSpeciesStringArray[] = {
	"DestroyAllEnemyUnits",
	"DestroyNumberOfEnemyUnits",
	"DestroyEnemyUnitGroup",
	"DestroySpecificEnemyUnit",
	"DestroySpecificStructure",

	"CaptureOrDestroyAllEnemyUnits",
	"CaptureOrDestroyNumberOfEnemyUnits",
	"CaptureOrDestroyEnemyUnitGroup",
	"CaptureOrDestroySpecificEnemyUnit",
	"CaptureOrDestroySpecificStructure",

	"DeadOrFledAllEnemyUnits",
	"DeadOrFledNumberOfEnemyUnits",
	"DeadOrFledEnemyUnitGroup",
	"DeadOrFledSpecificEnemyUnit",

	"CaptureSpecificUnit",
	"CaptureSpecificStructure",

	"GuardSpecificUnit",
	"GuardSpecificStructure",

	"MoveAnyUnitToArea",
	"MoveAllUnitsToArea",
	"MoveAllSurvivingUnitsToArea",
	"MoveAllSurvivingMechsToArea",

	"BooleanFlagIsSet",
	"ElapsedMissionTime",
};

enum objective_status_type
{
	OS_UNDETERMINED = 0, // Make sure first one is zero just to be safe
	OS_SUCCESSFUL,
	OS_FAILED
};

class CEStringList : public EList<std::wstring_view, std::wstring_view>
{
};
class CBoolList : public EList<bool, bool>
{
};

class CBooleanArray
{
private:
	CEStringList m_FlagIDList;
	CBoolList m_valueList;

public:
	CBooleanArray() { }
	~CBooleanArray() { }
	void Clear()
	{
		m_FlagIDList.Clear(void);
		m_valueList.Clear(void);
	}
	int32_t elementPos(std::wstring_view element)
	{
		bool elementFound = false;
		int32_t pos = 0;
		CEStringList::EIterator flagIDListIter;
		for (flagIDListIter = m_FlagIDList.Begin(void); !flagIDListIter.IsDone(void);
			 flagIDListIter++)
		{
			if ((*flagIDListIter) == element)
			{
				elementFound = true;
				break;
			}
			pos += 1;
		}
		if (elementFound)
		{
			return pos;
		}
		else
		{
			return -1;
		}
	}
	void setElementValue(std::wstring_view element, bool value)
	{
		int32_t pos = elementPos(element);
		if (-1 != pos)
		{
			m_valueList.Replace(value, pos);
		}
		else
		{
			m_FlagIDList.Append(element);
			m_valueList.Append(value);
		}
	}
	bool getElementValue(std::wstring_view element)
	{
		int32_t pos = elementPos(element);
		if (-1 != pos)
		{
			return m_valueList[pos];
		}
		else
		{
			/*_ASSERT(false);*/
			return false;
		}
	}

	void save(int32_t alignment, FitIniFile* file);

	void load(int32_t alignment, FitIniFile* file);
};

class CObjectiveCondition
{
private:
	int32_t m_alignment;

public:
	CObjectiveCondition(int32_t alignment)
	{
		m_alignment = alignment;
	}
	virtual ~CObjectiveCondition() { }
	int32_t Alignment()
	{
		return m_alignment;
	}
	void Alignment(int32_t alignment)
	{
		m_alignment = alignment;
	}
	virtual condition_species_type Species() = 0;
	virtual bool Init()
	{
		return true;
	}
	virtual bool Read(FitIniFile* /*missionFile */)
	{
		return true;
	}
	virtual bool Save(FitIniFile* /*file*/)
	{
		return true;
	}
	virtual objective_status_type Status() = 0;
	virtual std::wstring_view Description() = 0;
	virtual std::wstring_view InstanceDescription()
	{
		std::wstring_view retval;
		return retval;
	}
	virtual void CastAndCopy(const CObjectiveCondition* pMaster)
	{
		(*this) = (*pMaster);
	}
	virtual Stuff::Vector3D GetObjectivePosition() // Used to draw on tacmap
	{
		return Stuff::Vector3D(-999999.0f, -999999.0f, -999999.0f);
	}
};

class CDestroyAllEnemyUnits : public CObjectiveCondition
{
public:
	CDestroyAllEnemyUnits(int32_t alignment)
		: CObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return DESTROY_ALL_ENEMY_UNITS;
	}
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "DestroyAllEnemyUnits"; /* needs to be put somewhere localizable */
		return retval;
	}
};

class CNumberOfUnitsObjectiveCondition : public CObjectiveCondition /*abstract class*/
{
protected:
	int32_t m_num;

public:
	CNumberOfUnitsObjectiveCondition(int32_t alignment)
		: CObjectiveCondition(alignment)
	{
		m_num = 0;
	}
	virtual bool Read(FitIniFile* missionFile);
	virtual bool Save(FitIniFile* file);
	virtual std::wstring_view InstanceDescription(void);
	virtual void CastAndCopy(const CObjectiveCondition* pMaster)
	{
		(*this) = (*(dynamic_cast<const CNumberOfUnitsObjectiveCondition*>(pMaster)));
	}
};

class CDestroyNumberOfEnemyUnits : public CNumberOfUnitsObjectiveCondition
{
public:
	CDestroyNumberOfEnemyUnits(int32_t alignment)
		: CNumberOfUnitsObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return DESTROY_NUMBER_OF_ENEMY_UNITS;
	}
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "DestroyNumberOfEnemyUnits"; /* needs to be put
														 somewhere localizable
													   */
		return retval;
	}
};

class CSpecificUnitObjectiveCondition : public CObjectiveCondition /*abstract class*/
{
protected:
	GameObjectWatchID m_pUnitWID;

public:
	CSpecificUnitObjectiveCondition(int32_t alignment)
		: CObjectiveCondition(alignment)
	{
		m_pUnitWID = 0;
	}
	virtual std::wstring_view InstanceDescription(void);
	virtual void CastAndCopy(const CObjectiveCondition* pMaster)
	{
		(*this) = (*(dynamic_cast<const CSpecificUnitObjectiveCondition*>(pMaster)));
	}
	virtual bool Save(FitIniFile* file);
};

class CSpecificEnemyUnitObjectiveCondition : public CSpecificUnitObjectiveCondition /*abstract class*/
{
public:
	CSpecificEnemyUnitObjectiveCondition(int32_t alignment)
		: CSpecificUnitObjectiveCondition(alignment)
	{
	}
	bool Read(FitIniFile* missionFile);
};

class CDestroySpecificEnemyUnit : public CSpecificEnemyUnitObjectiveCondition
{
public:
	CDestroySpecificEnemyUnit(int32_t alignment)
		: CSpecificEnemyUnitObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return DESTROY_SPECIFIC_ENEMY_UNIT;
	}
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "DestroySpecificEnemyUnit"; /* needs to be put
														somewhere localizable */
		return retval;
	}
};

class CSpecificStructureObjectiveCondition : public CObjectiveCondition /*abstract class*/
{
protected:
	int32_t m_pBuildingWID;

public:
	CSpecificStructureObjectiveCondition(int32_t alignment)
		: CObjectiveCondition(alignment)
	{
		m_pBuildingWID = 0;
	}
	virtual bool Read(FitIniFile* missionFile);
	virtual bool Save(FitIniFile* file);
	virtual std::wstring_view InstanceDescription(void);
	virtual void CastAndCopy(const CObjectiveCondition* pMaster)
	{
		(*this) = (*(dynamic_cast<const CSpecificStructureObjectiveCondition*>(pMaster)));
	}

	virtual Stuff::Vector3D GetObjectivePosition() // Used to draw on tacmap
	{
		Building* m_pBuilding = (Building*)ObjectManager->getByWatchID(m_pBuildingWID);
		if (m_pBuilding)
			return m_pBuilding->getPosition(void);
		return Stuff::Vector3D(-999999.0f, -999999.0f, -999999.0f);
	}
};

class CDestroySpecificStructure : public CSpecificStructureObjectiveCondition
{
public:
	CDestroySpecificStructure(int32_t alignment)
		: CSpecificStructureObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return DESTROY_SPECIFIC_STRUCTURE;
	}
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "DestroySpecificStructure"; /* needs to be put
														somewhere localizable */
		return retval;
	}
};

class CCaptureOrDestroyAllEnemyUnits : public CObjectiveCondition
{
public:
	CCaptureOrDestroyAllEnemyUnits(int32_t alignment)
		: CObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return CAPTURE_OR_DESTROY_ALL_ENEMY_UNITS;
	}
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "CaptureOrDestroyAllEnemyUnits"; /* needs to be put
															 somewhere
															 localizable */
		return retval;
	}
};

class CCaptureOrDestroyNumberOfEnemyUnits : public CNumberOfUnitsObjectiveCondition
{
public:
	CCaptureOrDestroyNumberOfEnemyUnits(int32_t alignment)
		: CNumberOfUnitsObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return CAPTURE_OR_DESTROY_NUMBER_OF_ENEMY_UNITS;
	}
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "CaptureOrDestroyNumberOfEnemyUnits"; /* needs to be put somewhere
																	   localizable */
		return retval;
	}
};

class CCaptureOrDestroySpecificEnemyUnit : public CSpecificEnemyUnitObjectiveCondition
{
public:
	CCaptureOrDestroySpecificEnemyUnit(int32_t alignment)
		: CSpecificEnemyUnitObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return CAPTURE_OR_DESTROY_SPECIFIC_ENEMY_UNIT;
	}
	bool Read(FitIniFile* missionFile);
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "CaptureOrDestroySpecificEnemyUnit"; /* needs to be put somewhere
																	  localizable */
		return retval;
	}
};

class CCaptureOrDestroySpecificStructure : public CSpecificStructureObjectiveCondition
{
public:
	CCaptureOrDestroySpecificStructure(int32_t alignment)
		: CSpecificStructureObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return CAPTURE_OR_DESTROY_SPECIFIC_STRUCTURE;
	}
	bool Read(FitIniFile* missionFile);
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "CaptureOrDestroySpecificStructure"; /* needs to be put somewhere
																	  localizable */
		return retval;
	}
};

class CDeadOrFledAllEnemyUnits : public CObjectiveCondition
{
public:
	CDeadOrFledAllEnemyUnits(int32_t alignment)
		: CObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return DEAD_OR_FLED_ALL_ENEMY_UNITS;
	}
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "DeadOrFledAllEnemyUnits"; /* needs to be put somewhere
													   localizable */
		return retval;
	}
};

class CDeadOrFledNumberOfEnemyUnits : public CNumberOfUnitsObjectiveCondition
{
public:
	CDeadOrFledNumberOfEnemyUnits(int32_t alignment)
		: CNumberOfUnitsObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return DEAD_OR_FLED_NUMBER_OF_ENEMY_UNITS;
	}
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "DeadOrFledNumberOfEnemyUnits"; /* needs to be put
															somewhere
															localizable */
		return retval;
	}
};

class CDeadOrFledSpecificEnemyUnit : public CSpecificEnemyUnitObjectiveCondition
{
public:
	CDeadOrFledSpecificEnemyUnit(int32_t alignment)
		: CSpecificEnemyUnitObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return DEAD_OR_FLED_SPECIFIC_ENEMY_UNIT;
	}
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "DeadOrFledSpecificEnemyUnit"; /* needs to be put somewhere
																localizable */
		return retval;
	}
};

class CCaptureUnit : public CSpecificEnemyUnitObjectiveCondition
{
public:
	CCaptureUnit(int32_t alignment)
		: CSpecificEnemyUnitObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return CAPTURE_UNIT;
	}
	bool Read(FitIniFile* missionFile);
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "CaptureSpecificUnit"; /* needs to be put somewhere localizable */
		return retval;
	}
};

class CCaptureStructure : public CSpecificStructureObjectiveCondition
{
public:
	CCaptureStructure(int32_t alignment)
		: CSpecificStructureObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return CAPTURE_STRUCTURE;
	}
	bool Read(FitIniFile* missionFile);
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "CaptureStructure"; /* needs to be put somewhere localizable */
		return retval;
	}
};

class CGuardSpecificUnit : public CSpecificUnitObjectiveCondition
{
public:
	CGuardSpecificUnit(int32_t alignment)
		: CSpecificUnitObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return GUARD_SPECIFIC_UNIT;
	}
	bool Read(FitIniFile* missionFile);
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "GuardSpecificUnit"; /* needs to be put somewhere localizable */
		return retval;
	}
};

class CGuardSpecificStructure : public CSpecificStructureObjectiveCondition
{
public:
	CGuardSpecificStructure(int32_t alignment)
		: CSpecificStructureObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return GUARD_SPECIFIC_STRUCTURE;
	}
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "GuardSpecificStructure"; /* needs to be put somewhere localizable
														 */
		return retval;
	}
};

class CAreaObjectiveCondition : public CObjectiveCondition /*abstract class*/
{
protected:
	float m_targetCenterX;
	float m_targetCenterY;
	float m_targetRadius;

public:
	CAreaObjectiveCondition(int32_t alignment)
		: CObjectiveCondition(alignment)
	{
		m_targetCenterX = 0.0;
		m_targetCenterY = 0.0;
		m_targetRadius = 0.0;
	}
	virtual bool SetParams(float targetCenterX, float targetCenterY, float targetRadius)
	{
		m_targetCenterX = targetCenterX;
		m_targetCenterY = targetCenterY;
		m_targetRadius = targetRadius;
		return true;
	}
	virtual bool Read(FitIniFile* missionFile);
	virtual bool Save(FitIniFile* file);
	virtual std::wstring_view InstanceDescription(void);
	virtual void CastAndCopy(const CObjectiveCondition* pMaster)
	{
		(*this) = (*(dynamic_cast<const CAreaObjectiveCondition*>(pMaster)));
	}

	virtual Stuff::Vector3D GetObjectivePosition() // Used to draw on tacmap
	{
		return Stuff::Vector3D(m_targetCenterX, m_targetCenterY, 0.0f);
	}
};

class CMoveAnyUnitToArea : public CAreaObjectiveCondition
{
public:
	CMoveAnyUnitToArea(int32_t alignment)
		: CAreaObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return MOVE_ANY_UNIT_TO_AREA;
	}
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "MoveAnyUnitToArea"; /* needs to be put somewhere localizable */
		return retval;
	}
};

class CMoveAllUnitsToArea : public CAreaObjectiveCondition
{
public:
	CMoveAllUnitsToArea(int32_t alignment)
		: CAreaObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return MOVE_ALL_UNITS_TO_AREA;
	}
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "MoveAllUnitsToArea"; /* needs to be put somewhere localizable */
		return retval;
	}
};

class CMoveAllSurvivingUnitsToArea : public CAreaObjectiveCondition
{
public:
	CMoveAllSurvivingUnitsToArea(int32_t alignment)
		: CAreaObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return MOVE_ALL_SURVIVING_UNITS_TO_AREA;
	}
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "MoveAllSurvivingUnitsToArea"; /* needs to be put somewhere
																localizable */
		return retval;
	}
};

class CMoveAllSurvivingMechsToArea : public CAreaObjectiveCondition
{
public:
	CMoveAllSurvivingMechsToArea(int32_t alignment)
		: CAreaObjectiveCondition(alignment)
	{
	}
	condition_species_type Species()
	{
		return MOVE_ALL_SURVIVING_MECHS_TO_AREA;
	}
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "MoveAllSurvivingMechsToArea"; /* needs to be put somewhere
																localizable */
		return retval;
	}
};

class CBooleanFlagIsSet : public CObjectiveCondition
{
protected:
	std::wstring_view m_flagID;
	bool m_value;

public:
	CBooleanFlagIsSet(int32_t alignment)
		: CObjectiveCondition(alignment)
	{
		m_flagID = _TEXT("flag0");
		m_value = true;
	}
	condition_species_type Species()
	{
		return BOOLEAN_FLAG_IS_SET;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "BooleanFlagIsSet"; /* needs to be put somewhere localizable */
		return retval;
	}
	std::wstring_view InstanceDescription(void);
	void CastAndCopy(const CObjectiveCondition* pMaster)
	{
		(*this) = (*(dynamic_cast<const CBooleanFlagIsSet*>(pMaster)));
	}
};

class CElapsedMissionTime : public CObjectiveCondition
{
protected:
	float m_time;

public:
	CElapsedMissionTime(int32_t alignment)
		: CObjectiveCondition(alignment)
	{
		m_time = 0.0;
	}
	condition_species_type Species()
	{
		return ELAPSED_MISSION_TIME;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	objective_status_type Status(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "ElapsedMissionTime"; /* needs to be put somewhere localizable */
		return retval;
	}
	std::wstring_view InstanceDescription(void);
	void CastAndCopy(const CObjectiveCondition* pMaster)
	{
		(*this) = (*(dynamic_cast<const CElapsedMissionTime*>(pMaster)));
	}
};

enum action_species_type
{
	PLAY_BIK,
	PLAY_WAV,
	DISPLAY_TEXT_MESSAGE,
	DISPLAY_RESOURCE_TEXT_MESSAGE,
	SET_BOOLEAN_FLAG,
	MAKE_NEW_TECHNOLOGY_AVAILABLE,
	_REMOVE_STRUCTURE,

	NUM_ACTION_SPECIES
};

static std::wstring_view g_actionSpeciesStringArray[] = {
	"PlayBIK",
	"PlayWAV",
	"DisplayTextMessage",
	"DisplayResourceTextMessage",
	"SetBooleanFlag",
	"MakeNewTechnologyAvailable",
	"_RemoveStructure",
};

class CObjectiveAction
{
private:
	int32_t m_alignment;

public:
	CObjectiveAction(int32_t alignment)
	{
		m_alignment = alignment;
	}
	virtual ~CObjectiveAction() { }
	int32_t Alignment()
	{
		return m_alignment;
	}
	void Alignment(int32_t alignment)
	{
		m_alignment = alignment;
	}
	bool DoCommonEditDialog() { }
	virtual action_species_type Species() = 0;
	virtual bool Init() = 0;
	virtual bool Read(FitIniFile* missionFile) = 0;
	virtual bool Save(FitIniFile* file) = 0;
	virtual int32_t Execute() = 0;
	virtual std::wstring_view Description() = 0;
	virtual std::wstring_view InstanceDescription()
	{
		std::wstring_view retval;
		return retval;
	}
	virtual void CastAndCopy(const CObjectiveAction* pMaster)
	{
		(*this) = (*pMaster);
	}
};

class CPlayBIK : public CObjectiveAction
{
private:
	std::wstring_view m_pathname;

public:
	CPlayBIK(int32_t alignment)
		: CObjectiveAction(alignment)
	{
	}
	action_species_type Species()
	{
		return PLAY_BIK;
	}
	bool Init()
	{
		return true;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	int32_t Execute(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "PlayBIK"; /* needs to be put somewhere localizable */
		return retval;
	}
	std::wstring_view InstanceDescription(void);
	void CastAndCopy(const CObjectiveAction* pMaster)
	{
		(*this) = (*(dynamic_cast<const CPlayBIK*>(pMaster)));
	}
};

class CPlayWAV : public CObjectiveAction
{
private:
	std::wstring_view m_pathname;

public:
	CPlayWAV(int32_t alignment)
		: CObjectiveAction(alignment)
	{
	}
	action_species_type Species()
	{
		return PLAY_WAV;
	}
	bool Init()
	{
		return true;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	int32_t Execute(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "PlayWAV"; /* needs to be put somewhere localizable */
		return retval;
	}
	std::wstring_view InstanceDescription(void);
	void CastAndCopy(const CObjectiveAction* pMaster)
	{
		(*this) = (*(dynamic_cast<const CPlayWAV*>(pMaster)));
	}
};

class CDisplayTextMessage : public CObjectiveAction
{
private:
	std::wstring_view m_message;

public:
	CDisplayTextMessage(int32_t alignment)
		: CObjectiveAction(alignment)
	{
	}
	action_species_type Species()
	{
		return DISPLAY_TEXT_MESSAGE;
	}
	bool Init()
	{
		return true;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	int32_t Execute(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "DisplayTextMessage"; /* needs to be put somewhere localizable */
		return retval;
	}
	std::wstring_view InstanceDescription(void);
	void CastAndCopy(const CObjectiveAction* pMaster)
	{
		(*this) = (*(dynamic_cast<const CDisplayTextMessage*>(pMaster)));
	}
};

class CDisplayResourceTextMessage : public CObjectiveAction
{
private:
	int32_t m_resourceStringID;

public:
	CDisplayResourceTextMessage(int32_t alignment)
		: CObjectiveAction(alignment)
	{
	}
	action_species_type Species()
	{
		return DISPLAY_RESOURCE_TEXT_MESSAGE;
	}
	bool Init()
	{
		return true;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	int32_t Execute(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "DisplayResourceTextMessage"; /* needs to be put somewhere
															   localizable */
		return retval;
	}
	std::wstring_view InstanceDescription(void);
	void CastAndCopy(const CObjectiveAction* pMaster)
	{
		(*this) = (*(dynamic_cast<const CDisplayResourceTextMessage*>(pMaster)));
	}
};

class CSetBooleanFlag : public CObjectiveAction
{
private:
	std::wstring_view m_flagID;
	bool m_value;

public:
	CSetBooleanFlag(int32_t alignment)
		: CObjectiveAction(alignment)
	{
		m_flagID = _TEXT("flag0");
		m_value = true;
	}
	action_species_type Species()
	{
		return SET_BOOLEAN_FLAG;
	}
	bool Init()
	{
		return true;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	int32_t Execute(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "SetBooleanFlag"; /* needs to be put somewhere localizable */
		return retval;
	}
	std::wstring_view InstanceDescription(void);
	void CastAndCopy(const CObjectiveAction* pMaster)
	{
		(*this) = (*(dynamic_cast<const CSetBooleanFlag*>(pMaster)));
	}
};

class CMakeNewTechnologyAvailable : public CObjectiveAction
{
private:
	std::wstring_view m_purchaseFilePathname;

public:
	CMakeNewTechnologyAvailable(int32_t alignment)
		: CObjectiveAction(alignment)
	{
	}
	action_species_type Species()
	{
		return MAKE_NEW_TECHNOLOGY_AVAILABLE;
	}
	bool Init()
	{
		return true;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	int32_t Execute(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "MakeNewTechnologyAvailable"; /* needs to be put somewhere
															   localizable */
		return retval;
	}
	std::wstring_view InstanceDescription(void);
	void CastAndCopy(const CObjectiveAction* pMaster)
	{
		(*this) = (*(dynamic_cast<const CMakeNewTechnologyAvailable*>(pMaster)));
	}
};

class C_RemoveStructure : public CObjectiveAction
{
private:
	int32_t m_pBuildingWID;

public:
	C_RemoveStructure(int32_t alignment)
		: CObjectiveAction(alignment)
	{
	}
	bool SetParams(float positionX, float positionY);
	action_species_type Species()
	{
		return _REMOVE_STRUCTURE;
	}
	bool Init()
	{
		return true;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	int32_t Execute(void);
	std::wstring_view Description()
	{
		std::wstring_view retval = "_RemoveStructure"; /* needs to be put somewhere localizable */
		return retval;
	}
	std::wstring_view InstanceDescription(void);
	void CastAndCopy(const CObjectiveAction* pMaster)
	{
		(*this) = (*(dynamic_cast<const C_RemoveStructure*>(pMaster)));
	}
};

class CObjectives;
class CObjectiveConditionList : public EList<CObjectiveCondition*, CObjectiveCondition*>
{
};
class CObjectiveActionList : public EList<CObjectiveAction*, CObjectiveAction*>
{
};

class CObjective : public /*maybe protected*/ CObjectiveConditionList
{
private:
	typedef CObjectiveConditionList inherited;
	int32_t m_alignment;
	std::wstring_view m_title;
	bool m_titleUseResourceString;
	int32_t m_titleResourceStringID;
	std::wstring_view m_description;
	bool m_descriptionUseResourceString;
	int32_t m_descriptionResourceStringID;
	int32_t m_priority;
	int32_t m_resourcePoints;
	bool m_previousPrimaryObjectiveMustBeComplete;
	bool m_allPreviousPrimaryObjectivesMustBeComplete;
	int32_t m_displayMarker;
	float m_markerX;
	float m_markerY;
	wchar_t m_markerText[4];
	bool m_isHiddenTrigger;
	bool m_isActive;
	bool m_activateOnFlag;
	std::wstring_view m_activateFlagID;
	bool m_resetStatusOnFlag;
	std::wstring_view m_resetStatusFlagID;
	double m_activationTime;
	bool m_resolved;
	bool m_changedStatus;
	objective_status_type m_resolvedStatus;
	std::wstring_view m_modelName;
	int32_t m_modelType;
	int32_t m_modelBasecolour;
	int32_t m_modelHighlightcolour;
	int32_t m_modelHighlightcolour2;
	float m_modelScale;
	static float s_blinkLength;
	static float s_lastBlinkTime;
	static uint32_t s_blinkcolour;
	static aFont* s_markerFont;

public: /* we could make this protected if only the editdialog is to access
		   these functions */
	static CObjectiveCondition* new_CObjectiveCondition(
		condition_species_type conditionSpecies, int32_t alignment);
	static std::wstring_view DescriptionOfConditionSpecies(condition_species_type conditionSpecies);
	static CObjectiveAction* new_CObjectiveAction(
		action_species_type actionSpecies, int32_t alignment);
	static std::wstring_view DescriptionOfActionSpecies(action_species_type actionSpecies);

public:
	typedef CObjectiveConditionList condition_list_type;
	typedef CObjectiveActionList action_list_type;
	action_list_type m_actionList;
	condition_list_type m_failureConditionList;
	action_list_type m_failureActionList;

	CObjective(int32_t alignment);
	CObjective(const CObjective& master)
	{
		(*this) = master;
	}
	~CObjective()
	{
		Clear(void);
	}
	CObjective& operator=(const CObjective& master);
	void Init() { }
	void Clear(void);
	int32_t Alignment()
	{
		return m_alignment;
	}
	void Alignment(int32_t alignment);
	bool Read(FitIniFile* missionFile, int32_t objectiveNum, uint32_t version, int32_t markerNum,
		wchar_t secondaryMarkerNum);
	bool Save(FitIniFile* file, int32_t objectiveNum);
	/* The following function evaluates the status of the objective irrespective
	of the other objectives (i.e. it disregards qualifiers like
	"PreviousPrimaryObjectiveMustbeComplete"). */
	objective_status_type Status(void);
	/* The following function evaluates the status of the objective in the
	context of the given objectives. */
	objective_status_type Status(CObjectives& objectives);
	void Status(objective_status_type newStatus);
	/* The following function evaluates the status of the objective in the
	context of the given objectives. It returns a bool telling me if the
	objective has failed or been completed since I last checked!*/
	bool StatusChangedSuccess(void);
	bool StatusChangedFailed(void);
	std::wstring_view Title(void) const
	{
		return m_title;
	}
	void Title(std::wstring_view title)
	{
		m_title = title;
	}
	bool TitleUseResourceString(void) const
	{
		return m_titleUseResourceString;
	}
	void TitleUseResourceString(bool titleUseResourceString)
	{
		m_titleUseResourceString = titleUseResourceString;
	}
	int32_t TitleResourceStringID(void) const
	{
		return m_titleResourceStringID;
	}
	void TitleResourceStringID(int32_t titleResourceStringID)
	{
		m_titleResourceStringID = titleResourceStringID;
	}
	std::wstring_view LocalizedTitle(void) const;
	std::wstring_view Description(void) const
	{
		return m_description;
	}
	void Description(std::wstring_view description)
	{
		m_description = description;
	}
	bool DescriptionUseResourceString(void) const
	{
		return m_descriptionUseResourceString;
	}
	void DescriptionUseResourceString(bool descriptionUseResourceString)
	{
		m_descriptionUseResourceString = descriptionUseResourceString;
	}
	int32_t DescriptionResourceStringID(void) const
	{
		return m_descriptionResourceStringID;
	}
	void DescriptionResourceStringID(int32_t descriptionResourceStringID)
	{
		m_descriptionResourceStringID = descriptionResourceStringID;
	}
	std::wstring_view LocalizedDescription(void) const;
	int32_t Priority()
	{
		return m_priority;
	}
	void Priority(int32_t priority)
	{
		m_priority = priority;
	}
	int32_t ResourcePoints()
	{
		return m_resourcePoints;
	}
	void ResourcePoints(int32_t resourcePoints)
	{
		m_resourcePoints = resourcePoints;
	}
	bool PreviousPrimaryObjectiveMustBeComplete()
	{
		return m_previousPrimaryObjectiveMustBeComplete;
	}
	void PreviousPrimaryObjectiveMustBeComplete(bool previousPrimaryObjectiveMustBeComplete)
	{
		m_previousPrimaryObjectiveMustBeComplete = previousPrimaryObjectiveMustBeComplete;
	}
	bool AllPreviousPrimaryObjectivesMustBeComplete()
	{
		return m_allPreviousPrimaryObjectivesMustBeComplete;
	}
	void AllPreviousPrimaryObjectivesMustBeComplete(bool allPreviousPrimaryObjectivesMustBeComplete)
	{
		m_allPreviousPrimaryObjectivesMustBeComplete = allPreviousPrimaryObjectivesMustBeComplete;
	}
	int32_t DisplayMarker()
	{
		return m_displayMarker;
	}
	void DisplayMarker(int32_t displayMarker)
	{
		m_displayMarker = displayMarker;
	}
	float MarkerX()
	{
		return m_markerX;
	}
	void MarkerX(float markerX)
	{
		m_markerX = markerX;
	}
	float MarkerY()
	{
		return m_markerY;
	}
	void MarkerY(float markerY)
	{
		m_markerY = markerY;
	}
	void IsHiddenTrigger(bool isHiddenTrigger)
	{
		m_isHiddenTrigger = isHiddenTrigger;
	}
	bool IsHiddenTrigger()
	{
		return m_isHiddenTrigger;
	}
	void IsActive(bool isActive)
	{
		m_isActive = isActive;
	}
	bool IsActive()
	{
		return m_isActive;
	}
	void ActivateOnFlag(bool activateOnFlag)
	{
		m_activateOnFlag = activateOnFlag;
	}
	bool ActivateOnFlag()
	{
		return m_activateOnFlag;
	}
	void ActivateFlagID(std::wstring_view activateFlagId)
	{
		m_activateFlagID = activateFlagId;
	}
	std::wstring_view ActivateFlagID()
	{
		return m_activateFlagID;
	}
	void ResetStatusOnFlag(bool resetStatusOnFlag)
	{
		m_resetStatusOnFlag = resetStatusOnFlag;
	}
	bool ResetStatusOnFlag()
	{
		return m_resetStatusOnFlag;
	}
	void ResetStatusFlagID(std::wstring_view resetStatusFlagID)
	{
		m_resetStatusFlagID = resetStatusFlagID;
	}
	std::wstring_view ResetStatusFlagID()
	{
		return m_resetStatusFlagID;
	}
	std::wstring_view ModelName()
	{
		return m_modelName;
	}
	void ModelName(std::wstring_view modelName)
	{
		m_modelName = modelName;
	}
	int32_t ModelType()
	{
		return m_modelType;
	}
	void ModelType(int32_t modelType)
	{
		m_modelType = modelType;
	}
	int32_t ModelBasecolour()
	{
		return m_modelBasecolour;
	}
	void ModelBasecolour(int32_t modelBasecolour)
	{
		m_modelBasecolour = modelBasecolour;
	}
	int32_t ModelHighlightcolour()
	{
		return m_modelHighlightcolour;
	}
	void ModelHighlightcolour(int32_t modelHighlightcolour)
	{
		m_modelHighlightcolour = modelHighlightcolour;
	}
	int32_t ModelHighlightcolour2()
	{
		return m_modelHighlightcolour2;
	}
	void ModelHighlightcolour2(int32_t modelHighlightcolour2)
	{
		m_modelHighlightcolour2 = modelHighlightcolour2;
	}
	float ModelScale()
	{
		return m_modelScale;
	}
	void ModelScale(float modelScale)
	{
		m_modelScale = modelScale;
	}
	void ActivationTime(double activationTime)
	{
		m_activationTime = activationTime;
	}
	double ActivationTime()
	{
		return m_activationTime;
	}
	void Render(uint32_t xPos, uint32_t yPos, HGOSFONT3D);
	bool RenderMarkers(GameTacMap* tacMap,
		bool blink); // TacMap calls this to draw objective markers on tacMap.

	friend class Mission;
};

class CObjectives : public /*maybe protected*/ EList<CObjective*, CObjective*>
{
public:
	CObjectives(int32_t alignment = 0)
	{
		m_alignment = alignment;
	}
	CObjectives(const CObjectives& master)
	{
		(*this) = master;
	}
	~CObjectives()
	{
		Clear(void);
	}
	CObjectives& operator=(const CObjectives& master);
	void Init(void);
	void Clear(void);
	int32_t Alignment()
	{
		return m_alignment;
	}
	void Alignment(int32_t alignment);
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	objective_status_type Status(void);
	CBooleanArray boolFlags;

private:
	typedef EList<CObjective*, CObjective*> inherited;
	int32_t m_alignment;
};

/* Reads Nav Marker info and adds appropriate "hidden trigger" objectives. */
bool ReadNavMarkers(FitIniFile* missionFile, CObjectives& objectives);

#endif // end of file ( Objective.h )
