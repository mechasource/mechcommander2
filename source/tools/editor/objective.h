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

class EditorObject;
class Unit;
class FitIniFile;

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
	virtual bool operator==(const CObjectiveCondition& rhs) const;
	int32_t Alignment()
	{
		return m_alignment;
	}
	void Alignment(int32_t alignment)
	{
		m_alignment = alignment;
	}
	bool DoCommonEditDialog() { }
	virtual condition_species_type Species(void) const = 0;
	virtual bool Init()
	{
		return true;
	}
	virtual bool Read(FitIniFile* missionFile)
	{
		return true;
	}
	virtual bool Save(FitIniFile* file)
	{
		return true;
	}
	virtual bool EditDialog()
	{
		return true;
	}
	virtual void WriteAbl() { }
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
	virtual bool RefersTo(const EditorObject* pObj)
	{
		return false;
	}
	virtual bool NoteThePositionsOfObjectsReferenced()
	{
		return true;
	}
	virtual bool RestoreObjectPointerReferencesFromNotedPositions()
	{
		return true;
	}
};

class CDestroyAllEnemyUnits : public CObjectiveCondition
{
public:
	CDestroyAllEnemyUnits(int32_t alignment)
		: CObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return DESTROY_ALL_ENEMY_UNITS;
	}
	std::wstring_view Description(void);
};

class CNumberOfEnemyUnitsObjectiveCondition : public CObjectiveCondition /*abstract class*/
{
protected:
	int32_t m_num;

public:
	CNumberOfEnemyUnitsObjectiveCondition(int32_t alignment)
		: CObjectiveCondition(alignment)
	{
		m_num = 0;
	}
	virtual bool operator==(const CObjectiveCondition& rhs) const;
	virtual bool Read(FitIniFile* missionFile);
	virtual bool Save(FitIniFile* file);
	virtual bool EditDialog(void);
	virtual std::wstring_view InstanceDescription(void);
	virtual void CastAndCopy(const CObjectiveCondition* pMaster)
	{
		(*this) = (*(dynamic_cast<const CNumberOfEnemyUnitsObjectiveCondition*>(pMaster)));
	}
};

class CDestroyNumberOfEnemyUnits : public CNumberOfEnemyUnitsObjectiveCondition
{
public:
	CDestroyNumberOfEnemyUnits(int32_t alignment)
		: CNumberOfEnemyUnitsObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return DESTROY_NUMBER_OF_ENEMY_UNITS;
	}
	std::wstring_view Description(void);
};

class CSpecificUnitObjectiveCondition : public CObjectiveCondition /*abstract class*/
{
protected:
	Unit* m_pUnit;
	float m_LastNotedPositionX;
	float m_LastNotedPositionY;

public:
	CSpecificUnitObjectiveCondition(int32_t alignment)
		: CObjectiveCondition(alignment)
	{
		m_pUnit = 0;
		m_LastNotedPositionX = m_LastNotedPositionY = 0.0;
	}
	virtual bool operator==(const CObjectiveCondition& rhs) const;
	virtual bool Read(FitIniFile* missionFile);
	virtual bool Save(FitIniFile* file);
	virtual bool EditDialog(void);
	virtual std::wstring_view InstanceDescription(void);
	virtual void CastAndCopy(const CObjectiveCondition* pMaster)
	{
		(*this) = (*(dynamic_cast<const CSpecificUnitObjectiveCondition*>(pMaster)));
	}
	virtual bool RefersTo(const EditorObject* pObj)
	{
		return (pObj == ((const EditorObject*)m_pUnit));
	}
	virtual bool NoteThePositionsOfObjectsReferenced(void);
	virtual bool RestoreObjectPointerReferencesFromNotedPositions(void);
};

class CSpecificEnemyUnitObjectiveCondition : public CSpecificUnitObjectiveCondition /*abstract class*/
{
public:
	CSpecificEnemyUnitObjectiveCondition(int32_t alignment)
		: CSpecificUnitObjectiveCondition(alignment)
	{
	}
	virtual bool EditDialog(void);
};

class CSpecificAlliedUnitObjectiveCondition : public CSpecificUnitObjectiveCondition /*abstract class*/
{
public:
	CSpecificAlliedUnitObjectiveCondition(int32_t alignment)
		: CSpecificUnitObjectiveCondition(alignment)
	{
	}
	// virtual bool EditDialog(void);
};

class CDestroySpecificEnemyUnit : public CSpecificEnemyUnitObjectiveCondition
{
public:
	CDestroySpecificEnemyUnit(int32_t alignment)
		: CSpecificEnemyUnitObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return DESTROY_SPECIFIC_ENEMY_UNIT;
	}
	std::wstring_view Description(void);
};

class CSpecificStructureObjectiveCondition : public CObjectiveCondition /*abstract class*/
{
protected:
	EditorObject* m_pBuilding;
	float m_LastNotedPositionX;
	float m_LastNotedPositionY;

public:
	CSpecificStructureObjectiveCondition(int32_t alignment)
		: CObjectiveCondition(alignment)
	{
		m_pBuilding = 0;
		m_LastNotedPositionX = m_LastNotedPositionY = 0.0;
	}
	virtual bool operator==(const CObjectiveCondition& rhs) const;
	virtual bool Read(FitIniFile* missionFile);
	virtual bool Save(FitIniFile* file);
	virtual bool EditDialog(void);
	virtual std::wstring_view InstanceDescription(void);
	virtual void CastAndCopy(const CObjectiveCondition* pMaster)
	{
		(*this) = (*(dynamic_cast<const CSpecificStructureObjectiveCondition*>(pMaster)));
	}
	virtual bool RefersTo(const EditorObject* pObj)
	{
		return (pObj == m_pBuilding);
	}
	virtual bool NoteThePositionsOfObjectsReferenced(void);
	virtual bool RestoreObjectPointerReferencesFromNotedPositions(void);
};

class CSpecificEnemyStructureObjectiveCondition : public CSpecificStructureObjectiveCondition /*abstract class*/
{
public:
	CSpecificEnemyStructureObjectiveCondition(int32_t alignment)
		: CSpecificStructureObjectiveCondition(alignment)
	{
	}
	// virtual bool EditDialog(void);
};

class CSpecificAlliedStructureObjectiveCondition : public CSpecificStructureObjectiveCondition /*abstract class*/
{
public:
	CSpecificAlliedStructureObjectiveCondition(int32_t alignment)
		: CSpecificStructureObjectiveCondition(alignment)
	{
	}
	// virtual bool EditDialog(void);
};

class CDestroySpecificStructure : public CSpecificEnemyStructureObjectiveCondition
{
public:
	CDestroySpecificStructure(int32_t alignment)
		: CSpecificEnemyStructureObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return DESTROY_SPECIFIC_STRUCTURE;
	}
	std::wstring_view Description(void);
};

class CCaptureOrDestroyAllEnemyUnits : public CObjectiveCondition
{
	/* The CaptureOrDestroyAllEnemyUnits condition should not set the units to
	be capturable. */
public:
	CCaptureOrDestroyAllEnemyUnits(int32_t alignment)
		: CObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return CAPTURE_OR_DESTROY_ALL_ENEMY_UNITS;
	}
	std::wstring_view Description(void);
};

class CCaptureOrDestroyNumberOfEnemyUnits : public CNumberOfEnemyUnitsObjectiveCondition
{
	/* The CaptureOrDestroyAllEnemyUnits condition should not set the units to
	be capturable. */
public:
	CCaptureOrDestroyNumberOfEnemyUnits(int32_t alignment)
		: CNumberOfEnemyUnitsObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return CAPTURE_OR_DESTROY_NUMBER_OF_ENEMY_UNITS;
	}
	std::wstring_view Description(void);
};

class CCaptureOrDestroySpecificEnemyUnit : public CSpecificEnemyUnitObjectiveCondition
{
public:
	CCaptureOrDestroySpecificEnemyUnit(int32_t alignment)
		: CSpecificEnemyUnitObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return CAPTURE_OR_DESTROY_SPECIFIC_ENEMY_UNIT;
	}
	std::wstring_view Description(void);
};

class CCaptureOrDestroySpecificStructure : public CSpecificEnemyStructureObjectiveCondition
{
public:
	CCaptureOrDestroySpecificStructure(int32_t alignment)
		: CSpecificEnemyStructureObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return CAPTURE_OR_DESTROY_SPECIFIC_STRUCTURE;
	}
	std::wstring_view Description(void);
};

class CDeadOrFledAllEnemyUnits : public CObjectiveCondition
{
public:
	CDeadOrFledAllEnemyUnits(int32_t alignment)
		: CObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return DEAD_OR_FLED_ALL_ENEMY_UNITS;
	}
	std::wstring_view Description(void);
};

class CDeadOrFledNumberOfEnemyUnits : public CNumberOfEnemyUnitsObjectiveCondition
{
public:
	CDeadOrFledNumberOfEnemyUnits(int32_t alignment)
		: CNumberOfEnemyUnitsObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return DEAD_OR_FLED_NUMBER_OF_ENEMY_UNITS;
	}
	std::wstring_view Description(void);
};

class CDeadOrFledSpecificEnemyUnit : public CSpecificEnemyUnitObjectiveCondition
{
public:
	CDeadOrFledSpecificEnemyUnit(int32_t alignment)
		: CSpecificEnemyUnitObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return DEAD_OR_FLED_SPECIFIC_ENEMY_UNIT;
	}
	std::wstring_view Description(void);
};

class CCaptureUnit : public CSpecificEnemyUnitObjectiveCondition
{
public:
	CCaptureUnit(int32_t alignment)
		: CSpecificEnemyUnitObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return CAPTURE_UNIT;
	}
	std::wstring_view Description(void);
};

class CCaptureStructure : public CSpecificEnemyStructureObjectiveCondition
{
public:
	CCaptureStructure(int32_t alignment)
		: CSpecificEnemyStructureObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return CAPTURE_STRUCTURE;
	}
	std::wstring_view Description(void);
};

class CGuardSpecificUnit : public CSpecificAlliedUnitObjectiveCondition
{
public:
	CGuardSpecificUnit(int32_t alignment)
		: CSpecificAlliedUnitObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return GUARD_SPECIFIC_UNIT;
	}
	std::wstring_view Description(void);
};

class CGuardSpecificStructure : public CSpecificAlliedStructureObjectiveCondition
{
public:
	CGuardSpecificStructure(int32_t alignment)
		: CSpecificAlliedStructureObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return GUARD_SPECIFIC_STRUCTURE;
	}
	std::wstring_view Description(void);
};

class CAreaObjectiveCondition : public CObjectiveCondition /*abstract class*/
{
private:
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
	virtual bool operator==(const CObjectiveCondition& rhs) const;
	virtual bool Read(FitIniFile* missionFile);
	virtual bool Save(FitIniFile* file);
	virtual bool EditDialog(void);
	virtual std::wstring_view InstanceDescription(void);
	virtual void CastAndCopy(const CObjectiveCondition* pMaster)
	{
		(*this) = (*(dynamic_cast<const CAreaObjectiveCondition*>(pMaster)));
	}
};

class CMoveAnyUnitToArea : public CAreaObjectiveCondition
{
public:
	CMoveAnyUnitToArea(int32_t alignment)
		: CAreaObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return MOVE_ANY_UNIT_TO_AREA;
	}
	std::wstring_view Description(void);
};

class CMoveAllUnitsToArea : public CAreaObjectiveCondition
{
public:
	CMoveAllUnitsToArea(int32_t alignment)
		: CAreaObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return MOVE_ALL_UNITS_TO_AREA;
	}
	std::wstring_view Description(void);
};

class CMoveAllSurvivingUnitsToArea : public CAreaObjectiveCondition
{
public:
	CMoveAllSurvivingUnitsToArea(int32_t alignment)
		: CAreaObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return MOVE_ALL_SURVIVING_UNITS_TO_AREA;
	}
	std::wstring_view Description(void);
};

class CMoveAllSurvivingMechsToArea : public CAreaObjectiveCondition
{
public:
	CMoveAllSurvivingMechsToArea(int32_t alignment)
		: CAreaObjectiveCondition(alignment)
	{
	}
	condition_species_type Species(void) const
	{
		return MOVE_ALL_SURVIVING_MECHS_TO_AREA;
	}
	std::wstring_view Description(void);
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
	bool operator==(const CObjectiveCondition& rhs) const;
	condition_species_type Species(void) const
	{
		return BOOLEAN_FLAG_IS_SET;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	bool EditDialog(void);
	std::wstring_view Description(void);
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
	bool operator==(const CObjectiveCondition& rhs) const;
	condition_species_type Species(void) const
	{
		return ELAPSED_MISSION_TIME;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	bool EditDialog(void);
	std::wstring_view Description(void);
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

	NUM_ACTION_SPECIES
};

static std::wstring_view g_actionSpeciesStringArray[] = {
	"PlayBIK",
	"PlayWAV",
	"DisplayTextMessage",
	"DisplayResourceTextMessage",
	"SetBooleanFlag",
	"MakeNewTechnologyAvailable",
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
	virtual bool operator==(const CObjectiveAction& rhs) const;
	int32_t Alignment()
	{
		return m_alignment;
	}
	void Alignment(int32_t alignment)
	{
		m_alignment = alignment;
	}
	bool DoCommonEditDialog() { }
	virtual action_species_type Species(void) const = 0;
	virtual bool Init() = 0;
	virtual bool Read(FitIniFile* missionFile) = 0;
	virtual bool Save(FitIniFile* file) = 0;
	virtual bool EditDialog() = 0;
	virtual void WriteAbl() = 0;
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
	bool operator==(const CObjectiveAction& rhs) const;
	action_species_type Species(void) const
	{
		return PLAY_BIK;
	}
	bool Init()
	{
		return true;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	bool EditDialog(void);
	void WriteAbl() {};
	std::wstring_view Description(void);
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
	bool operator==(const CObjectiveAction& rhs) const;
	action_species_type Species(void) const
	{
		return PLAY_WAV;
	}
	bool Init()
	{
		return true;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	bool EditDialog(void);
	void WriteAbl() {};
	std::wstring_view Description(void);
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
	bool operator==(const CObjectiveAction& rhs) const;
	action_species_type Species(void) const
	{
		return DISPLAY_TEXT_MESSAGE;
	}
	bool Init()
	{
		return true;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	bool EditDialog(void);
	void WriteAbl() {};
	std::wstring_view Description(void);
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
	bool operator==(const CObjectiveAction& rhs) const;
	action_species_type Species(void) const
	{
		return DISPLAY_RESOURCE_TEXT_MESSAGE;
	}
	bool Init()
	{
		return true;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	bool EditDialog(void);
	void WriteAbl() {};
	std::wstring_view Description(void);
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
	bool operator==(const CObjectiveAction& rhs) const;
	action_species_type Species(void) const
	{
		return SET_BOOLEAN_FLAG;
	}
	bool Init()
	{
		return true;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	bool EditDialog(void);
	void WriteAbl() {};
	std::wstring_view Description(void);
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
	bool operator==(const CObjectiveAction& rhs) const;
	action_species_type Species(void) const
	{
		return MAKE_NEW_TECHNOLOGY_AVAILABLE;
	}
	bool Init()
	{
		return true;
	}
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	bool EditDialog(void);
	void WriteAbl() {};
	std::wstring_view Description(void);
	std::wstring_view InstanceDescription(void);
	void CastAndCopy(const CObjectiveAction* pMaster)
	{
		(*this) = (*(dynamic_cast<const CMakeNewTechnologyAvailable*>(pMaster)));
	}
};

class CObjectiveConditionList : public EList<CObjectiveCondition*, CObjectiveCondition*>
{
public:
	virtual ~CObjectiveConditionList()
	{
		Clear(void);
	}
	virtual CObjectiveConditionList& operator=(const CObjectiveConditionList& master);
	virtual bool operator==(const CObjectiveConditionList& rhs) const;
	virtual void Clear(void);
};

class CObjectiveActionList : public EList<CObjectiveAction*, CObjectiveAction*>
{
public:
	virtual ~CObjectiveActionList()
	{
		Clear(void);
	}
	virtual CObjectiveActionList& operator=(const CObjectiveActionList& master);
	virtual bool operator==(const CObjectiveActionList& rhs) const;
	virtual void Clear(void);
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
	bool m_displayMarker;
	float m_markerX;
	float m_markerY;
	bool m_isHiddenTrigger;
	bool m_activateOnFlag;
	std::wstring_view m_activateFlagID;
	bool m_resetStatusOnFlag;
	std::wstring_view m_resetStatusFlagID;
	int32_t m_modelID;
	int32_t m_basecolour;
	int32_t m_highlightcolour;
	int32_t m_highlightcolour2;

public: /* we could make this protected if only the editdialog is to acces
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

	void Construct(int32_t alignment);
	CObjective()
	{
		Construct(0);
	}
	CObjective(int32_t alignment)
	{
		Construct(alignment);
	}
	CObjective(const CObjective& master)
	{
		(*this) = master;
	}
	~CObjective()
	{
		Clear(void);
	}
	CObjective& operator=(const CObjective& master);
	bool operator==(const CObjective& rhs) const;
	void Init() { }
	void Clear(void);
	int32_t Alignment()
	{
		return m_alignment;
	}
	void Alignment(int32_t alignment);
	bool Read(FitIniFile* missionFile, int32_t objectiveNum, uint32_t version = 0);
	bool Save(FitIniFile* file, int32_t objectiveNum);
	bool EditDialog(void);
	bool NoteThePositionsOfObjectsReferenced(void);
	bool RestoreObjectPointerReferencesFromNotedPositions(void);
	bool ThisObjectiveHasNoConditions()
	{
		return ((0 == Count()) && (0 == m_failureConditionList.Count()));
	}
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
	bool DisplayMarker()
	{
		return m_displayMarker;
	}
	void DisplayMarker(bool displayMarker)
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
	int32_t Basecolour(void) const
	{
		return m_basecolour;
	}
	void Basecolour(int32_t newcolour)
	{
		m_basecolour = newcolour;
	}
	int32_t Highlightcolour(void) const
	{
		return m_highlightcolour;
	}
	void Highlightcolour(int32_t newcolour)
	{
		m_highlightcolour = newcolour;
	}
	int32_t Highlightcolour2(void) const
	{
		return m_highlightcolour2;
	}
	void Highlightcolour2(int32_t newcolour)
	{
		m_highlightcolour2 = newcolour;
	}
	int32_t ModelID(void) const
	{
		return m_modelID;
	}
	void ModelID(int32_t newID)
	{
		m_modelID = newID;
	}
};

class CObjectives : public /*maybe protected*/ EList<CObjective*, CObjective*>
{
private:
	typedef EList<CObjective*, CObjective*> inherited;
	int32_t m_alignment;

public:
	CObjectives()
	{
		m_alignment = 0;
	}
	CObjectives(int32_t alignment)
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
	bool operator==(const CObjectives& rhs) const;
	void Init(void);
	void Clear(void);
	int32_t Alignment()
	{
		return m_alignment;
	}
	void Alignment(int32_t alignment);
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* file);
	bool EditDialog(void);
	bool WriteMissionScript(std::wstring_view Name, std::wstring_view OutputPath);
	void handleObjectInvalidation(const EditorObject* pObj);
	bool NoteThePositionsOfObjectsReferenced(void);
	bool RestoreObjectPointerReferencesFromNotedPositions(void);
	bool ThereAreObjectivesWithNoConditions(void);
};

class CObjectivesEditState
{
public:
	int32_t alignment;
	CObjectives ModifiedObjectives;
	enum objective_function_id_type
	{
		ADD,
		EDIT
	};
	objective_function_id_type objectiveFunction;
	int32_t nSelectionIndex;

	CObjective ModifiedObjective;
	enum list_id_type
	{
		SUCCESS_CONDITION,
		FAILURE_CONDITION
	};
	list_id_type listID;
	int32_t nConditionSpeciesSelectionIndex;
	int32_t nActionSpeciesSelectionIndex;
	int32_t nFailureConditionSpeciesSelectionIndex;
	int32_t nFailureActionSpeciesSelectionIndex;

	PVOIDpModifiedUnitPtr;
	PVOIDpModifiedBuildingPtr;

	CObjectivesEditState()
	{
		Clear(void);
	}

	void Clear()
	{
		alignment = 0;
		ModifiedObjectives.Clear(void);
		objectiveFunction = ADD;
		nSelectionIndex = -1;
		ModifiedObjective.Clear(void);
		listID = SUCCESS_CONDITION;
		nConditionSpeciesSelectionIndex = -1;
		nActionSpeciesSelectionIndex = -1;
		nFailureConditionSpeciesSelectionIndex = -1;
		nFailureActionSpeciesSelectionIndex = -1;
		pModifiedUnitPtr = 0;
		pModifiedBuildingPtr = 0;
	}
};
#endif // end of file ( Objective.h )
