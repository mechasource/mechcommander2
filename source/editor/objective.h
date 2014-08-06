#ifndef OBJECTIVE_H
#define OBJECTIVE_H
/*************************************************************************************************\
Objective.h			: Interface for the Objective component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "elist.h"
#include "EString.h"
#include "tchar.h"

class EditorObject;
class Unit;
class FitIniFile;

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
CObjectiveCondition:
**************************************************************************************************/
enum condition_species_type {
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

static const char *g_conditionSpeciesStringArray[] = {
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

class CObjectiveCondition {
private:
	int m_alignment;
public:
	CObjectiveCondition(int alignment) { m_alignment = alignment; }
	virtual ~CObjectiveCondition() {}
	virtual bool operator==(const CObjectiveCondition &rhs) const;
	int Alignment() { return m_alignment; }
	void Alignment(int alignment) { m_alignment = alignment; }
	bool DoCommonEditDialog() {}
	virtual condition_species_type Species() const = 0;
	virtual bool Init() { return true; }
	virtual bool Read( FitIniFile* missionFile ) { return true; }
	virtual bool Save( FitIniFile* file ) { return true; }
	virtual bool EditDialog() { return true; }
	virtual void WriteAbl() {}
	virtual EString Description() = 0;
	virtual EString InstanceDescription() { EString retval; return retval; }
	virtual void CastAndCopy(const CObjectiveCondition *pMaster) { (*this) = (*pMaster); }
	virtual bool RefersTo(const EditorObject *pObj) { return false; }
	virtual bool NoteThePositionsOfObjectsReferenced() { return true; }
	virtual bool RestoreObjectPointerReferencesFromNotedPositions() { return true; }
};

class CDestroyAllEnemyUnits: public CObjectiveCondition {
public:
	CDestroyAllEnemyUnits(int alignment) : CObjectiveCondition(alignment) {}
	condition_species_type Species() const { return DESTROY_ALL_ENEMY_UNITS; }
	EString Description();
};

class CNumberOfEnemyUnitsObjectiveCondition: public CObjectiveCondition { /*abstract class*/
protected:
	int m_num;
public:
	CNumberOfEnemyUnitsObjectiveCondition(int alignment) : CObjectiveCondition(alignment) { m_num = 0; }
	virtual bool operator==(const CObjectiveCondition &rhs) const;
	virtual bool Read( FitIniFile* missionFile );
	virtual bool Save( FitIniFile* file );
	virtual bool EditDialog();
	virtual EString InstanceDescription();
	virtual void CastAndCopy(const CObjectiveCondition *pMaster) { (*this) = (*(dynamic_cast<const CNumberOfEnemyUnitsObjectiveCondition *>(pMaster))); }
};

class CDestroyNumberOfEnemyUnits: public CNumberOfEnemyUnitsObjectiveCondition {
public:
	CDestroyNumberOfEnemyUnits(int alignment) : CNumberOfEnemyUnitsObjectiveCondition(alignment) {}
	condition_species_type Species() const { return DESTROY_NUMBER_OF_ENEMY_UNITS; }
	EString Description();
};

class CSpecificUnitObjectiveCondition: public CObjectiveCondition { /*abstract class*/
protected:
	Unit *m_pUnit;
	float m_LastNotedPositionX;
	float m_LastNotedPositionY;
public:
	CSpecificUnitObjectiveCondition(int alignment) : CObjectiveCondition(alignment) { m_pUnit = 0; m_LastNotedPositionX = m_LastNotedPositionY = 0.0; }
	virtual bool operator==(const CObjectiveCondition &rhs) const;
	virtual bool Read( FitIniFile* missionFile );
	virtual bool Save( FitIniFile* file );
	virtual bool EditDialog();
	virtual EString InstanceDescription();
	virtual void CastAndCopy(const CObjectiveCondition *pMaster) { (*this) = (*(dynamic_cast<const CSpecificUnitObjectiveCondition *>(pMaster))); }
	virtual bool RefersTo(const EditorObject *pObj) { return (pObj == ((const EditorObject *)m_pUnit)); }
	virtual bool NoteThePositionsOfObjectsReferenced();
	virtual bool RestoreObjectPointerReferencesFromNotedPositions();
};

class CSpecificEnemyUnitObjectiveCondition: public CSpecificUnitObjectiveCondition { /*abstract class*/
public:
	CSpecificEnemyUnitObjectiveCondition(int alignment) : CSpecificUnitObjectiveCondition(alignment) {}
	virtual bool EditDialog();
};

class CSpecificAlliedUnitObjectiveCondition: public CSpecificUnitObjectiveCondition { /*abstract class*/
public:
	CSpecificAlliedUnitObjectiveCondition(int alignment) : CSpecificUnitObjectiveCondition(alignment) {}
	//virtual bool EditDialog();
};

class CDestroySpecificEnemyUnit: public CSpecificEnemyUnitObjectiveCondition {
public:
	CDestroySpecificEnemyUnit(int alignment) : CSpecificEnemyUnitObjectiveCondition(alignment) {}
	condition_species_type Species() const { return DESTROY_SPECIFIC_ENEMY_UNIT; }
	EString Description();
};

class CSpecificStructureObjectiveCondition: public CObjectiveCondition { /*abstract class*/
protected:
	EditorObject *m_pBuilding;
	float m_LastNotedPositionX;
	float m_LastNotedPositionY;
public:
	CSpecificStructureObjectiveCondition(int alignment) : CObjectiveCondition(alignment) { m_pBuilding = 0; m_LastNotedPositionX = m_LastNotedPositionY = 0.0; }
	virtual bool operator==(const CObjectiveCondition &rhs) const;
	virtual bool Read( FitIniFile* missionFile );
	virtual bool Save( FitIniFile* file );
	virtual bool EditDialog();
	virtual EString InstanceDescription();
	virtual void CastAndCopy(const CObjectiveCondition *pMaster) { (*this) = (*(dynamic_cast<const CSpecificStructureObjectiveCondition *>(pMaster))); }
	virtual bool RefersTo(const EditorObject *pObj) { return (pObj == m_pBuilding); }
	virtual bool NoteThePositionsOfObjectsReferenced();
	virtual bool RestoreObjectPointerReferencesFromNotedPositions();
};

class CSpecificEnemyStructureObjectiveCondition: public CSpecificStructureObjectiveCondition { /*abstract class*/
public:
	CSpecificEnemyStructureObjectiveCondition(int alignment) : CSpecificStructureObjectiveCondition(alignment) {}
	//virtual bool EditDialog();
};

class CSpecificAlliedStructureObjectiveCondition: public CSpecificStructureObjectiveCondition { /*abstract class*/
public:
	CSpecificAlliedStructureObjectiveCondition(int alignment) : CSpecificStructureObjectiveCondition(alignment) {}
	//virtual bool EditDialog();
};

class CDestroySpecificStructure: public CSpecificEnemyStructureObjectiveCondition {
public:
	CDestroySpecificStructure(int alignment) : CSpecificEnemyStructureObjectiveCondition(alignment) {}
	condition_species_type Species() const { return DESTROY_SPECIFIC_STRUCTURE; }
	EString Description();
};

class CCaptureOrDestroyAllEnemyUnits: public CObjectiveCondition {
	/* The CaptureOrDestroyAllEnemyUnits condition should not set the units to be
	capturable. */
public:
	CCaptureOrDestroyAllEnemyUnits(int alignment) : CObjectiveCondition(alignment) {}
	condition_species_type Species() const { return CAPTURE_OR_DESTROY_ALL_ENEMY_UNITS; }
	EString Description();
};

class CCaptureOrDestroyNumberOfEnemyUnits: public CNumberOfEnemyUnitsObjectiveCondition {
	/* The CaptureOrDestroyAllEnemyUnits condition should not set the units to be
	capturable. */
public:
	CCaptureOrDestroyNumberOfEnemyUnits(int alignment) : CNumberOfEnemyUnitsObjectiveCondition(alignment) {}
	condition_species_type Species() const { return CAPTURE_OR_DESTROY_NUMBER_OF_ENEMY_UNITS; }
	EString Description();
};

class CCaptureOrDestroySpecificEnemyUnit: public CSpecificEnemyUnitObjectiveCondition {
public:
	CCaptureOrDestroySpecificEnemyUnit(int alignment) : CSpecificEnemyUnitObjectiveCondition(alignment) {}
	condition_species_type Species() const { return CAPTURE_OR_DESTROY_SPECIFIC_ENEMY_UNIT; }
	EString Description();
};

class CCaptureOrDestroySpecificStructure: public CSpecificEnemyStructureObjectiveCondition {
public:
	CCaptureOrDestroySpecificStructure(int alignment) : CSpecificEnemyStructureObjectiveCondition(alignment) {}
	condition_species_type Species() const { return CAPTURE_OR_DESTROY_SPECIFIC_STRUCTURE; }
	EString Description();
};

class CDeadOrFledAllEnemyUnits: public CObjectiveCondition {
public:
	CDeadOrFledAllEnemyUnits(int alignment) : CObjectiveCondition(alignment) {}
	condition_species_type Species() const { return DEAD_OR_FLED_ALL_ENEMY_UNITS; }
	EString Description();
};

class CDeadOrFledNumberOfEnemyUnits: public CNumberOfEnemyUnitsObjectiveCondition {
public:
	CDeadOrFledNumberOfEnemyUnits(int alignment) : CNumberOfEnemyUnitsObjectiveCondition(alignment) {}
	condition_species_type Species() const { return DEAD_OR_FLED_NUMBER_OF_ENEMY_UNITS; }
	EString Description();
};

class CDeadOrFledSpecificEnemyUnit: public CSpecificEnemyUnitObjectiveCondition {
public:
	CDeadOrFledSpecificEnemyUnit(int alignment) : CSpecificEnemyUnitObjectiveCondition(alignment) {}
	condition_species_type Species() const { return DEAD_OR_FLED_SPECIFIC_ENEMY_UNIT; }
	EString Description();
};

class CCaptureUnit: public CSpecificEnemyUnitObjectiveCondition {
public:
	CCaptureUnit(int alignment) : CSpecificEnemyUnitObjectiveCondition(alignment) {}
	condition_species_type Species() const { return CAPTURE_UNIT; }
	EString Description();
};

class CCaptureStructure: public CSpecificEnemyStructureObjectiveCondition {
public:
	CCaptureStructure(int alignment) : CSpecificEnemyStructureObjectiveCondition(alignment) {}
	condition_species_type Species() const { return CAPTURE_STRUCTURE; }
	EString Description();
};

class CGuardSpecificUnit: public CSpecificAlliedUnitObjectiveCondition {
public:
	CGuardSpecificUnit(int alignment) : CSpecificAlliedUnitObjectiveCondition(alignment) {}
	condition_species_type Species() const { return GUARD_SPECIFIC_UNIT; }
	EString Description();
};

class CGuardSpecificStructure: public CSpecificAlliedStructureObjectiveCondition {
public:
	CGuardSpecificStructure(int alignment) : CSpecificAlliedStructureObjectiveCondition(alignment) {}
	condition_species_type Species() const { return GUARD_SPECIFIC_STRUCTURE; }
	EString Description();
};

class CAreaObjectiveCondition: public CObjectiveCondition { /*abstract class*/
private:
	float m_targetCenterX;
	float m_targetCenterY;
	float m_targetRadius;
public:
	CAreaObjectiveCondition(int alignment) : CObjectiveCondition(alignment) { m_targetCenterX = 0.0; m_targetCenterY = 0.0; m_targetRadius = 0.0; }
	virtual bool operator==(const CObjectiveCondition &rhs) const;
	virtual bool Read( FitIniFile* missionFile );
	virtual bool Save( FitIniFile* file );
	virtual bool EditDialog();
	virtual EString InstanceDescription();
	virtual void CastAndCopy(const CObjectiveCondition *pMaster) { (*this) = (*(dynamic_cast<const CAreaObjectiveCondition *>(pMaster))); }
};

class CMoveAnyUnitToArea: public CAreaObjectiveCondition {
public:
	CMoveAnyUnitToArea(int alignment) : CAreaObjectiveCondition(alignment) {}
	condition_species_type Species() const { return MOVE_ANY_UNIT_TO_AREA; }
	EString Description();
};

class CMoveAllUnitsToArea: public CAreaObjectiveCondition {
public:
	CMoveAllUnitsToArea(int alignment) : CAreaObjectiveCondition(alignment) {}
	condition_species_type Species() const { return MOVE_ALL_UNITS_TO_AREA; }
	EString Description();
};

class CMoveAllSurvivingUnitsToArea: public CAreaObjectiveCondition {
public:
	CMoveAllSurvivingUnitsToArea(int alignment) : CAreaObjectiveCondition(alignment) {}
	condition_species_type Species() const { return MOVE_ALL_SURVIVING_UNITS_TO_AREA; }
	EString Description();
};

class CMoveAllSurvivingMechsToArea: public CAreaObjectiveCondition {
public:
	CMoveAllSurvivingMechsToArea(int alignment) : CAreaObjectiveCondition(alignment) {}
	condition_species_type Species() const { return MOVE_ALL_SURVIVING_MECHS_TO_AREA; }
	EString Description();
};

class CBooleanFlagIsSet: public CObjectiveCondition {
protected:
	EString m_flagID;
	bool m_value;
public:
	CBooleanFlagIsSet(int alignment) : CObjectiveCondition(alignment) { m_flagID = _TEXT("flag0"); m_value = true; }
	bool operator==(const CObjectiveCondition &rhs) const;
	condition_species_type Species() const { return BOOLEAN_FLAG_IS_SET; }
	bool Read( FitIniFile* missionFile );
	bool Save( FitIniFile* file );
	bool EditDialog();
	EString Description();
	EString InstanceDescription();
	void CastAndCopy(const CObjectiveCondition *pMaster) { (*this) = (*(dynamic_cast<const CBooleanFlagIsSet *>(pMaster))); }
};

class CElapsedMissionTime: public CObjectiveCondition {
protected:
	float m_time;
public:
	CElapsedMissionTime(int alignment) : CObjectiveCondition(alignment) { m_time = 0.0; }
	bool operator==(const CObjectiveCondition &rhs) const;
	condition_species_type Species() const { return ELAPSED_MISSION_TIME; }
	bool Read( FitIniFile* missionFile );
	bool Save( FitIniFile* file );
	bool EditDialog();
	EString Description();
	EString InstanceDescription();
	void CastAndCopy(const CObjectiveCondition *pMaster) { (*this) = (*(dynamic_cast<const CElapsedMissionTime *>(pMaster))); }
};

enum action_species_type {
	PLAY_BIK,
	PLAY_WAV,
	DISPLAY_TEXT_MESSAGE,
	DISPLAY_RESOURCE_TEXT_MESSAGE,
	SET_BOOLEAN_FLAG,
	MAKE_NEW_TECHNOLOGY_AVAILABLE,

	NUM_ACTION_SPECIES
};

static const char *g_actionSpeciesStringArray[] = {
	"PlayBIK",
	"PlayWAV",
	"DisplayTextMessage",
	"DisplayResourceTextMessage",
	"SetBooleanFlag",
	"MakeNewTechnologyAvailable",
};

class CObjectiveAction {
private:
	int m_alignment;
public:
	CObjectiveAction(int alignment) { m_alignment = alignment; }
	virtual ~CObjectiveAction() {}
	virtual bool operator==(const CObjectiveAction &rhs) const;
	int Alignment() { return m_alignment; }
	void Alignment(int alignment) { m_alignment = alignment; }
	bool DoCommonEditDialog() {}
	virtual action_species_type Species() const = 0;
	virtual bool Init() = 0;
	virtual bool Read( FitIniFile* missionFile ) = 0;
	virtual bool Save( FitIniFile* file ) = 0;
	virtual bool EditDialog() = 0;
	virtual void WriteAbl() = 0;
	virtual EString Description() = 0;
	virtual EString InstanceDescription() { EString retval; return retval; }
	virtual void CastAndCopy(const CObjectiveAction *pMaster) { (*this) = (*pMaster); }
};

class CPlayBIK: public CObjectiveAction {
private:
	EString m_pathname;
public:
	CPlayBIK(int alignment) : CObjectiveAction(alignment) {}
	bool operator==(const CObjectiveAction &rhs) const;
	action_species_type Species() const { return PLAY_BIK; }
	bool Init() { return true; }
	bool Read( FitIniFile* missionFile );
	bool Save( FitIniFile* file );
	bool EditDialog();
	void WriteAbl() {};
	EString Description();
	EString InstanceDescription();
	void CastAndCopy(const CObjectiveAction *pMaster) { (*this) = (*(dynamic_cast<const CPlayBIK *>(pMaster))); }
};

class CPlayWAV: public CObjectiveAction {
private:
	EString m_pathname;
public:
	CPlayWAV(int alignment) : CObjectiveAction(alignment) {}
	bool operator==(const CObjectiveAction &rhs) const;
	action_species_type Species() const { return PLAY_WAV; }
	bool Init() { return true; }
	bool Read( FitIniFile* missionFile );
	bool Save( FitIniFile* file );
	bool EditDialog();
	void WriteAbl() {};
	EString Description();
	EString InstanceDescription();
	void CastAndCopy(const CObjectiveAction *pMaster) { (*this) = (*(dynamic_cast<const CPlayWAV *>(pMaster))); }
};

class CDisplayTextMessage: public CObjectiveAction {
private:
	EString m_message;
public:
	CDisplayTextMessage(int alignment) : CObjectiveAction(alignment) {}
	bool operator==(const CObjectiveAction &rhs) const;
	action_species_type Species() const { return DISPLAY_TEXT_MESSAGE; }
	bool Init() { return true; }
	bool Read( FitIniFile* missionFile );
	bool Save( FitIniFile* file );
	bool EditDialog();
	void WriteAbl() {};
	EString Description();
	EString InstanceDescription();
	void CastAndCopy(const CObjectiveAction *pMaster) { (*this) = (*(dynamic_cast<const CDisplayTextMessage *>(pMaster))); }
};

class CDisplayResourceTextMessage: public CObjectiveAction {
private:
	int m_resourceStringID;
public:
	CDisplayResourceTextMessage(int alignment) : CObjectiveAction(alignment) {}
	bool operator==(const CObjectiveAction &rhs) const;
	action_species_type Species() const { return DISPLAY_RESOURCE_TEXT_MESSAGE; }
	bool Init() { return true; }
	bool Read( FitIniFile* missionFile );
	bool Save( FitIniFile* file );
	bool EditDialog();
	void WriteAbl() {};
	EString Description();
	EString InstanceDescription();
	void CastAndCopy(const CObjectiveAction *pMaster) { (*this) = (*(dynamic_cast<const CDisplayResourceTextMessage *>(pMaster))); }
};

class CSetBooleanFlag: public CObjectiveAction {
private:
	EString m_flagID;
	bool m_value;
public:
	CSetBooleanFlag(int alignment) : CObjectiveAction(alignment) { m_flagID = _TEXT("flag0"); m_value = true; }
	bool operator==(const CObjectiveAction &rhs) const;
	action_species_type Species() const { return SET_BOOLEAN_FLAG; }
	bool Init() { return true; }
	bool Read( FitIniFile* missionFile );
	bool Save( FitIniFile* file );
	bool EditDialog();
	void WriteAbl() {};
	EString Description();
	EString InstanceDescription();
	void CastAndCopy(const CObjectiveAction *pMaster) { (*this) = (*(dynamic_cast<const CSetBooleanFlag *>(pMaster))); }
};

class CMakeNewTechnologyAvailable: public CObjectiveAction {
private:
	EString m_purchaseFilePathname;
public:
	CMakeNewTechnologyAvailable(int alignment) : CObjectiveAction(alignment) {}
	bool operator==(const CObjectiveAction &rhs) const;
	action_species_type Species() const { return MAKE_NEW_TECHNOLOGY_AVAILABLE; }
	bool Init() { return true; }
	bool Read( FitIniFile* missionFile );
	bool Save( FitIniFile* file );
	bool EditDialog();
	void WriteAbl() {};
	EString Description();
	EString InstanceDescription();
	void CastAndCopy(const CObjectiveAction *pMaster) { (*this) = (*(dynamic_cast<const CMakeNewTechnologyAvailable *>(pMaster))); }
};

class  CObjectiveConditionList : public EList <CObjectiveCondition *, CObjectiveCondition *> {
public:
	virtual ~CObjectiveConditionList() { Clear(); }
	virtual CObjectiveConditionList &operator=(const CObjectiveConditionList &master);
	virtual bool operator==(const CObjectiveConditionList &rhs) const;
	virtual void Clear();
};

class  CObjectiveActionList : public EList <CObjectiveAction *, CObjectiveAction *> {
public:
	virtual ~CObjectiveActionList() { Clear(); }
	virtual CObjectiveActionList &operator=(const CObjectiveActionList &master);
	virtual bool operator==(const CObjectiveActionList &rhs) const;
	virtual void Clear();
};

class CObjective : public/*maybe protected*/ CObjectiveConditionList {
private:
	typedef CObjectiveConditionList inherited;
	int m_alignment;
	EString m_title;
	bool m_titleUseResourceString;
	int m_titleResourceStringID;
	EString m_description;
	bool m_descriptionUseResourceString;
	int m_descriptionResourceStringID;
	int m_priority;
	int m_resourcePoints;
	bool m_previousPrimaryObjectiveMustBeComplete;
	bool m_allPreviousPrimaryObjectivesMustBeComplete;
	bool m_displayMarker;
	float m_markerX;
	float m_markerY;
	bool m_isHiddenTrigger;
	bool m_activateOnFlag;
	EString m_activateFlagID;
	bool m_resetStatusOnFlag;
	EString m_resetStatusFlagID;
	long	m_modelID;
	long m_baseColor;
	long m_highlightColor;
	long m_highlightColor2;

public: /* we could make this protected if only the editdialog is to acces these functions */
	static CObjectiveCondition *new_CObjectiveCondition(condition_species_type conditionSpecies, int alignment);
	static EString DescriptionOfConditionSpecies(condition_species_type conditionSpecies);
	static CObjectiveAction *new_CObjectiveAction(action_species_type actionSpecies, int alignment);
	static EString DescriptionOfActionSpecies(action_species_type actionSpecies);
public:
	typedef CObjectiveConditionList condition_list_type;
	typedef CObjectiveActionList action_list_type;
	action_list_type m_actionList;
	condition_list_type m_failureConditionList;
	action_list_type m_failureActionList;

	void Construct(int alignment);
	CObjective() { Construct(0); }
	CObjective(int alignment) { Construct(alignment); }
	CObjective(const CObjective &master) { (*this) = master; }
	~CObjective() { Clear(); }
	CObjective &operator=(const CObjective &master);
	bool operator==(const CObjective &rhs) const;
	void Init() {}
	void Clear();
	int Alignment() { return m_alignment; }
	void Alignment(int alignment);
	bool Read( FitIniFile* missionFile, int objectiveNum, int version = 0);
	bool Save( FitIniFile* file, int objectiveNum);
	bool EditDialog();
	bool NoteThePositionsOfObjectsReferenced();
	bool RestoreObjectPointerReferencesFromNotedPositions();
	bool ThisObjectiveHasNoConditions() { return ((0 == Count()) && (0 == m_failureConditionList.Count())); }
	EString Title() const { return m_title; }
	void Title(EString title) { m_title = title; }
	bool TitleUseResourceString() const { return m_titleUseResourceString; }
	void TitleUseResourceString(bool titleUseResourceString) { m_titleUseResourceString = titleUseResourceString; }
	int TitleResourceStringID() const { return m_titleResourceStringID; }
	void TitleResourceStringID(int titleResourceStringID) { m_titleResourceStringID = titleResourceStringID; }
	EString LocalizedTitle() const;
	EString Description() const { return m_description; }
	void Description(EString description) { m_description = description; }
	bool DescriptionUseResourceString() const { return m_descriptionUseResourceString; }
	void DescriptionUseResourceString(bool descriptionUseResourceString) { m_descriptionUseResourceString = descriptionUseResourceString; }
	int DescriptionResourceStringID() const { return m_descriptionResourceStringID; }
	void DescriptionResourceStringID(int descriptionResourceStringID) { m_descriptionResourceStringID = descriptionResourceStringID; }
	EString LocalizedDescription() const;
	int Priority() { return m_priority; }
	void Priority(int priority) { m_priority = priority; }
	int ResourcePoints() { return m_resourcePoints; }
	void ResourcePoints(int resourcePoints) { m_resourcePoints = resourcePoints; }
	bool PreviousPrimaryObjectiveMustBeComplete() { return m_previousPrimaryObjectiveMustBeComplete; }
	void PreviousPrimaryObjectiveMustBeComplete(bool previousPrimaryObjectiveMustBeComplete) { m_previousPrimaryObjectiveMustBeComplete = previousPrimaryObjectiveMustBeComplete; }
	bool AllPreviousPrimaryObjectivesMustBeComplete() { return m_allPreviousPrimaryObjectivesMustBeComplete; }
	void AllPreviousPrimaryObjectivesMustBeComplete(bool allPreviousPrimaryObjectivesMustBeComplete) { m_allPreviousPrimaryObjectivesMustBeComplete = allPreviousPrimaryObjectivesMustBeComplete; }
	bool DisplayMarker() { return m_displayMarker; }
	void DisplayMarker(bool displayMarker) { m_displayMarker = displayMarker; }
	float MarkerX() { return m_markerX; }
	void MarkerX(float markerX) { m_markerX = markerX; }
	float MarkerY() { return m_markerY; }
	void MarkerY(float markerY) { m_markerY = markerY; }
	void IsHiddenTrigger(bool isHiddenTrigger) { m_isHiddenTrigger = isHiddenTrigger; }
	bool IsHiddenTrigger() { return m_isHiddenTrigger; }
	void ActivateOnFlag(bool activateOnFlag) { m_activateOnFlag = activateOnFlag; }
	bool ActivateOnFlag() { return m_activateOnFlag; }
	void ActivateFlagID(EString activateFlagId) { m_activateFlagID = activateFlagId; }
	EString ActivateFlagID() { return m_activateFlagID; }
	void ResetStatusOnFlag(bool resetStatusOnFlag) { m_resetStatusOnFlag = resetStatusOnFlag; }
	bool ResetStatusOnFlag() { return m_resetStatusOnFlag; }
	void ResetStatusFlagID(EString resetStatusFlagID) { m_resetStatusFlagID = resetStatusFlagID; }
	EString ResetStatusFlagID() { return m_resetStatusFlagID; }
	int BaseColor( ) const { return m_baseColor; }
	void BaseColor( int newColor ) { m_baseColor = newColor; }
	int HighlightColor( ) const { return m_highlightColor; }
	void HighlightColor( int newColor ) { m_highlightColor = newColor; }
	int HighlightColor2( ) const { return m_highlightColor2; }
	void HighlightColor2( int newColor ) { m_highlightColor2 = newColor; }
	long ModelID() const { return m_modelID; }
	void ModelID( int newID ) { m_modelID = newID; }
};

class CObjectives : public/*maybe protected*/ EList <CObjective *, CObjective *> {
private:
	typedef EList <CObjective *, CObjective *> inherited;
	int m_alignment;
public:
	CObjectives() { m_alignment = 0; }
	CObjectives(int alignment) { m_alignment = alignment; }
	CObjectives(const CObjectives &master) { (*this) = master; }
	~CObjectives() { Clear(); }
	CObjectives &operator=(const CObjectives &master);
	bool operator==(const CObjectives &rhs) const;
	void Init();
	void Clear();
	int Alignment() { return m_alignment; }
	void Alignment(int alignment);
	bool Read( FitIniFile* missionFile );
	bool Save( FitIniFile* file );
	bool EditDialog();
	bool WriteMissionScript(const char *Name, const char *OutputPath);
	void handleObjectInvalidation(const EditorObject *pObj);
	bool NoteThePositionsOfObjectsReferenced();
	bool RestoreObjectPointerReferencesFromNotedPositions();
	bool ThereAreObjectivesWithNoConditions();
};

class CObjectivesEditState {
public:
	int alignment;
	CObjectives ModifiedObjectives;
	enum objective_function_id_type {
		ADD,
		EDIT
	};
	objective_function_id_type objectiveFunction;
	int nSelectionIndex;

	CObjective ModifiedObjective;
	enum list_id_type {
		SUCCESS_CONDITION,
		FAILURE_CONDITION
	};
	list_id_type listID;
	int nConditionSpeciesSelectionIndex;
	int nActionSpeciesSelectionIndex;
	int nFailureConditionSpeciesSelectionIndex;
	int nFailureActionSpeciesSelectionIndex;

	void *pModifiedUnitPtr;
	void *pModifiedBuildingPtr;

	CObjectivesEditState() {
		Clear();
	}

	void Clear() {
		alignment = 0;
		ModifiedObjectives.Clear();
		objectiveFunction = ADD;
		nSelectionIndex = -1;
		ModifiedObjective.Clear();
		listID = SUCCESS_CONDITION;
		nConditionSpeciesSelectionIndex = -1;
		nActionSpeciesSelectionIndex = -1;
		nFailureConditionSpeciesSelectionIndex = -1;
		nFailureActionSpeciesSelectionIndex = -1;
		pModifiedUnitPtr = 0;
		pModifiedBuildingPtr = 0;
	}
};
//*************************************************************************************************
#endif  // end of file ( Objective.h )
