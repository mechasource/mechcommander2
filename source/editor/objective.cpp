#define OBJECTIVE_CPP
/*************************************************************************************************\
Objective.cpp			: Implementation of the Objective component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "terrain.h"

#include "Objective.h"
#include "ObjectiveDlg.h"
#include "ObjectivesDlg.h"
#include "ChooseUnitDlg.h"
#include "ChooseBuildingDlg.h"
#include "ResourceStringSelectionDlg.h"
#include "SingleValueDlg.h"
#include "BooleanFlagIsSetDialog.h"
#include "TargetAreaDlg.h"
#include "TextMessageDlg.h"
#include "MCLibResource.h"
#include "EditorInterface.h"
#include "../resource.h"/*this is mc2res.dll's header file*/

#include "EString.h"
#include "ECharString.h"

#ifndef INIFILE_H
#include "IniFile.h"
#endif

#include <assert.h>

static bool ESLoadString(int resourceID, EString &targetStr) {
	char szTmp[16384/*max string length*/];
	cLoadString( resourceID, szTmp, 16384/*max string length*/ );
	targetStr = szTmp;
	EString tmpStr;
	tmpStr.Format("mc2res.dll:%d Not defined", resourceID);
	if (0 == strcmp(tmpStr.Data(), szTmp)) {
		return (0);
	}
	return (!0);
}

static long sReadIdFloat(FitIniFile* missionFile, const char *varName, float &value) {
	long result = 0;
	float tmpFloat;
	result = missionFile->readIdFloat((char *)varName, tmpFloat);
	if (NO_ERR != result) {
		//assert(false);
	} else {
		value = tmpFloat;
	}
	return result;
}

static long sReadIdBoolean(FitIniFile* missionFile, const char *varName, bool &value) {
	long result = 0;
	bool tmpBool;
	result = missionFile->readIdBoolean((char *)varName, tmpBool);
	if (NO_ERR != result) {
		//assert(false);
	} else {
		value = tmpBool;
	}
	return result;
}

static long sReadIdWholeNum(FitIniFile* missionFile, const char *varName, int &value) {
	long result = 0;
	unsigned long tmpULong;
	result = missionFile->readIdULong((char *)varName, tmpULong);
	if (NO_ERR != result) {
		//assert(false);
	} else {
		value = tmpULong;
	}
	return result;
}

static long sReadIdString(FitIniFile* missionFile, const char *varName, ECharString &ECStr) {
	long result = 0;
	char buffer[2001/*buffer size*/]; buffer[0] = '\0';
	result = missionFile->readIdString((char *)varName, buffer, 2001/*buffer size*/ - 1);
	if ((NO_ERR != result) && (BUFFER_TOO_SMALL != result)) {
		//assert(false);
	} else {
		CString CStr = buffer;
		/*readIdString can't read in "\r\n"*/
		CStr.Replace("\n", "\r\n");
		ECStr = CStr.GetBuffer(0);
	}
	return result;
}

static long sWriteIdString(FitIniFile* missionFile, const char *varName, const char *szStr) {
	if (!szStr) { return !(NO_ERR); }
	long result = 0;
	CString CStr = szStr;
	/*readIdString can't read in "\r\n"*/
	CStr.Replace("\r\n", "\n");
	result = missionFile->writeIdString(varName, CStr.GetBuffer(0));
	return result;
}

static bool areCloseEnough(float f1, float f2)
{
	bool retval = false;
	float diff = fabs(f1 - f2);
	float bigger = fabs(f1);
	if (fabs(f2) > bigger) {
		bigger = fabs(f2);
	}
	float proportion_diff = 0.0;
	if (0.0 != bigger) {
		proportion_diff = diff / bigger;
	}
	if ((pow(10.0, -FLT_DIG) * 10.0/*arbitrary*/) > proportion_diff) {
		retval = true;
	}
	return retval;
}

bool CObjectiveCondition::operator==(const CObjectiveCondition &rhs) const {
	bool retval = false;
	if ((Species() == rhs.Species()) && (m_alignment == rhs.m_alignment)) {
		retval = true;
	}
	return retval;
}

EString CDestroyAllEnemyUnits::Description() {
	EString retval = "DestroyAllEnemyUnits"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_DestroyAllEnemyUnits, retval);
	return retval;
}

bool CNumberOfEnemyUnitsObjectiveCondition::operator==(const CObjectiveCondition &rhs) const {
	bool retval = false;
	const CNumberOfEnemyUnitsObjectiveCondition *pCastedRhs = dynamic_cast<const CNumberOfEnemyUnitsObjectiveCondition *>(&rhs);
	if ((pCastedRhs) && (CObjectiveCondition::operator==(rhs))) {
		if (m_num == pCastedRhs->m_num) {
			retval = true;
		}
	}
	return retval;
}

bool CNumberOfEnemyUnitsObjectiveCondition::Read( FitIniFile* missionFile )
{
	long result = 0;
	result = sReadIdWholeNum(missionFile, "Num", m_num);
	if (NO_ERR != result) { return false; }

	return true;
}

bool CNumberOfEnemyUnitsObjectiveCondition::Save( FitIniFile* file )
{
	file->writeIdULong( "Num", m_num );
	return true;
}

bool CNumberOfEnemyUnitsObjectiveCondition::EditDialog() {
	SingleValueDlg NumDialog( IDS_NUM_ENEMY_UNITS, IDS_BLANK_SPACE);
	NumDialog.SetVal(m_num);
	int ret = NumDialog.DoModal();
	if (IDOK == ret) {
		m_num = NumDialog.GetVal();
	}
	return (IDOK == ret);
}

EString CNumberOfEnemyUnitsObjectiveCondition::InstanceDescription() {
	EString tmpEStr;
	tmpEStr.Format("%d", m_num);
	return tmpEStr;
}

EString CDestroyNumberOfEnemyUnits::Description() {
	EString retval = "DestroyNumberOfEnemyUnits"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_DestroyNumberOfEnemyUnits, retval);
	return retval;
}

bool CSpecificUnitObjectiveCondition::operator==(const CObjectiveCondition &rhs) const {
	bool retval = false;
	const CSpecificUnitObjectiveCondition *pCastedRhs = dynamic_cast<const CSpecificUnitObjectiveCondition *>(&rhs);
	if ((pCastedRhs) && (CObjectiveCondition::operator==(rhs))) {
		if (m_pUnit == pCastedRhs->m_pUnit) {
			retval = true;
		}
	}
	return retval;
}

bool CSpecificUnitObjectiveCondition::Read( FitIniFile* missionFile )
{
	long result = 0;

	float positionX, positionY;
	result = sReadIdFloat(missionFile, "PositionX", positionX);
	if (NO_ERR != result) { return false; }
	result = sReadIdFloat(missionFile, "PositionY", positionY);
	if (NO_ERR != result) { return false; }
	EditorObjectMgr::UNIT_LIST units = EditorObjectMgr::instance()->getUnits();
	EditorObjectMgr::UNIT_LIST::EConstIterator it = units.Begin();
	while (!it.IsDone()) {
		if (areCloseEnough((*it)->getPosition().x, positionX)  && areCloseEnough((*it)->getPosition().y, positionY)) {
			break;
		}
		it++;
	}
	if (it.IsDone()) { assert(false); return false; }
	m_pUnit = (*it);
	if (m_pUnit) {
		m_LastNotedPositionX = m_pUnit->getPosition().x;
		m_LastNotedPositionY = m_pUnit->getPosition().y;
	}

	return true;
}

bool CSpecificUnitObjectiveCondition::Save( FitIniFile* file )
{
	assert(m_pUnit);	// todo: exception handling

	file->writeIdFloat( "PositionX", m_pUnit->getPosition().x  );
	file->writeIdFloat( "PositionY", m_pUnit->getPosition().y );

	long cellx, celly;
	m_pUnit->getCells(cellx, celly);
	file->writeIdULong( "CellX", cellx );
	file->writeIdULong( "CellY", celly );

	int alignment = m_pUnit->getAlignment();
	file->writeIdULong( "Commander", alignment );

	int newLance;
	int index;
	m_pUnit->getLanceInfo(newLance, index);
	file->writeIdULong( "Group", newLance );
	file->writeIdULong( "Mate", index );
	return true;
}

bool CSpecificUnitObjectiveCondition::EditDialog() {
	ChooseUnitDlg ChooseUnitDialog(m_pUnit);
	int ret = ChooseUnitDialog.DoModal();
	if (m_pUnit) {
		m_LastNotedPositionX = m_pUnit->getPosition().x;
		m_LastNotedPositionY = m_pUnit->getPosition().y;
	}
	return (IDOK == ret);
}

EString CSpecificUnitObjectiveCondition::InstanceDescription() {
	EString tmpEStr;
	Stuff::Vector3D pos = m_pUnit->getPosition();
	const char *szDisplayName = m_pUnit->getDisplayName(); // nb: localization
	assert(szDisplayName);
	tmpEStr.Format("(pos: %.3f, %.3f) %s", pos.x, pos.y, szDisplayName);
	return tmpEStr;
}

bool CSpecificUnitObjectiveCondition::NoteThePositionsOfObjectsReferenced() {
	bool retval = false;
	if (m_pUnit) {
		m_LastNotedPositionX = m_pUnit->getPosition().x;
		m_LastNotedPositionY = m_pUnit->getPosition().y;
		retval = true;
	}
	return retval;
}

bool CSpecificUnitObjectiveCondition::RestoreObjectPointerReferencesFromNotedPositions() {
	EditorObjectMgr::UNIT_LIST units = EditorObjectMgr::instance()->getUnits();
	EditorObjectMgr::UNIT_LIST::EConstIterator it = units.Begin();
	while (!it.IsDone()) {
		if (areCloseEnough((*it)->getPosition().x, m_LastNotedPositionX)  && areCloseEnough((*it)->getPosition().y, m_LastNotedPositionY)) {
			break;
		}
		it++;
	}
	if (it.IsDone()) { assert(false); return false; }
	m_pUnit = (*it);
	return true;
}

bool CSpecificEnemyUnitObjectiveCondition::EditDialog() {
	ChooseUnitDlg ChooseUnitDialog(m_pUnit, Alignment());
	int ret = ChooseUnitDialog.DoModal();
	if (m_pUnit) {
		m_LastNotedPositionX = m_pUnit->getPosition().x;
		m_LastNotedPositionY = m_pUnit->getPosition().y;
	}
	return (IDOK == ret);
}

EString CDestroySpecificEnemyUnit::Description() {
	EString retval = "DestroySpecificEnemyUnit"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_DestroySpecificEnemyUnit, retval);
	return retval;
}

bool CSpecificStructureObjectiveCondition::operator==(const CObjectiveCondition &rhs) const {
	bool retval = false;
	const CSpecificStructureObjectiveCondition *pCastedRhs = dynamic_cast<const CSpecificStructureObjectiveCondition *>(&rhs);
	if ((pCastedRhs) && (CObjectiveCondition::operator==(rhs))) {
		if (m_pBuilding == pCastedRhs->m_pBuilding) {
			retval = true;
		}
	}
	return retval;
}

bool CSpecificStructureObjectiveCondition::Read( FitIniFile* missionFile )
{
	long result = 0;

	float positionX, positionY;
	result = sReadIdFloat(missionFile, "PositionX", positionX);
	if (NO_ERR != result) { return false; }
	result = sReadIdFloat(missionFile, "PositionY", positionY);
	if (NO_ERR != result) { return false; }
	EditorObjectMgr::BUILDING_LIST buildings = EditorObjectMgr::instance()->getBuildings();
	EditorObjectMgr::BUILDING_LIST::EConstIterator it = buildings.Begin();
	while (!it.IsDone()) {
		if (areCloseEnough((*it)->getPosition().x, positionX)  && areCloseEnough((*it)->getPosition().y, positionY)) {
			break;
		}
		it++;
	}
	if (it.IsDone()) { assert(false); return false; }
	m_pBuilding = (*it);
	if (m_pBuilding) {
		m_LastNotedPositionX = m_pBuilding->getPosition().x;
		m_LastNotedPositionY = m_pBuilding->getPosition().y;
	}

	return true;
}

bool CSpecificStructureObjectiveCondition::Save( FitIniFile* file )
{
	assert(m_pBuilding);	// todo: exception handling

	file->writeIdFloat( "PositionX", m_pBuilding->getPosition().x  );
	file->writeIdFloat( "PositionY", m_pBuilding->getPosition().y );

	long cellx, celly;
	m_pBuilding->getCells(cellx, celly);
	file->writeIdULong( "CellX", cellx );
	file->writeIdULong( "CellY", celly );

	int alignment = m_pBuilding->getAlignment();
	file->writeIdULong( "Commander", alignment );
	return true;
}

bool CSpecificStructureObjectiveCondition::EditDialog() {
	ChooseBuildingDlg ChooseBuildingDlg(m_pBuilding);
	int ret = ChooseBuildingDlg.DoModal();
	if (m_pBuilding) {
		m_LastNotedPositionX = m_pBuilding->getPosition().x;
		m_LastNotedPositionY = m_pBuilding->getPosition().y;
	}
	return (IDOK == ret);
}

EString CSpecificStructureObjectiveCondition::InstanceDescription() {
	EString tmpEStr;
	Stuff::Vector3D pos = m_pBuilding->getPosition();
	const char *szDisplayName = m_pBuilding->getDisplayName(); // nb: localization
	assert(szDisplayName);
	tmpEStr.Format("(pos: %.3f, %.3f) %s", pos.x, pos.y, szDisplayName);
	return tmpEStr;
}

bool CSpecificStructureObjectiveCondition::NoteThePositionsOfObjectsReferenced() {
	bool retval = false;
	if (m_pBuilding) {
		m_LastNotedPositionX = m_pBuilding->getPosition().x;
		m_LastNotedPositionY = m_pBuilding->getPosition().y;
		retval = true;
	}
	return retval;
}

bool CSpecificStructureObjectiveCondition::RestoreObjectPointerReferencesFromNotedPositions() {
	EditorObjectMgr::BUILDING_LIST buildings = EditorObjectMgr::instance()->getBuildings();
	EditorObjectMgr::BUILDING_LIST::EConstIterator it = buildings.Begin();
	while (!it.IsDone()) {
		if (areCloseEnough((*it)->getPosition().x, m_LastNotedPositionX)  && areCloseEnough((*it)->getPosition().y, m_LastNotedPositionY)) {
			break;
		}
		it++;
	}
	if (it.IsDone()) { assert(false); return false; }
	m_pBuilding = (*it);
	return true;
}

EString CDestroySpecificStructure::Description() {
	EString retval = "DestroySpecificStructure"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_DestroySpecificStructure, retval);
	return retval;
}

EString CCaptureOrDestroyAllEnemyUnits::Description() {
	EString retval = "CaptureOrDestroyAllEnemyUnits"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_CaptureOrDestroyAllEnemyUnits, retval);
	return retval;
}

EString CCaptureOrDestroyNumberOfEnemyUnits::Description() {
	EString retval = "CaptureOrDestroyNumberOfEnemyUnits"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_CaptureOrDestroyNumberOfEnemyUnits, retval);
	return retval;
}

EString CCaptureOrDestroySpecificEnemyUnit::Description() {
	EString retval = "CaptureOrDestroySpecificEnemyUnit"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_CaptureOrDestroySpecificEnemyUnit, retval);
	return retval;
}

EString CCaptureOrDestroySpecificStructure::Description() {
	EString retval = "CaptureOrDestroySpecificStructure"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_CaptureOrDestroySpecificStructure, retval);
	return retval;
}

EString CDeadOrFledAllEnemyUnits::Description() {
	EString retval = "DeadOrFledAllEnemyUnits"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_DeadOrFledAllEnemyUnits, retval);
	return retval;
}

EString CDeadOrFledNumberOfEnemyUnits::Description() {
	EString retval = "DeadOrFledNumberOfEnemyUnits"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_DeadOrFledNumberOfEnemyUnits, retval);
	return retval;
}

EString CDeadOrFledSpecificEnemyUnit::Description() {
	EString retval = "DeadOrFledSpecificEnemyUnit"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_DeadOrFledSpecificEnemyUnit, retval);
	return retval;
}

EString CCaptureUnit::Description() {
	EString retval = "CaptureSpecificUnit"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_CaptureSpecificUnit, retval);
	return retval;
}

EString CCaptureStructure::Description() {
	EString retval = "CaptureSpecificStructure"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_CaptureSpecificStructure, retval);
	return retval;
}

EString CGuardSpecificUnit::Description() {
	EString retval = "GuardSpecificUnit"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_GuardSpecificUnit, retval);
	return retval;
}

EString CGuardSpecificStructure::Description() {
	EString retval = "GuardSpecificStructure"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_GuardSpecificStructure, retval);
	return retval;
}

bool CAreaObjectiveCondition::operator==(const CObjectiveCondition &rhs) const {
	bool retval = false;
	const CAreaObjectiveCondition *pCastedRhs = dynamic_cast<const CAreaObjectiveCondition *>(&rhs);
	if ((pCastedRhs) && (CObjectiveCondition::operator==(rhs))) {
		if ((m_targetCenterX == pCastedRhs->m_targetCenterX) &&
			(m_targetCenterY == pCastedRhs->m_targetCenterY) &&
			(m_targetRadius == pCastedRhs->m_targetRadius)) {
			retval = true;
		}
	}
	return retval;
}

bool CAreaObjectiveCondition::Read( FitIniFile* missionFile )
{
	long result = 0;
	result = sReadIdFloat(missionFile, "TargetCenterX", m_targetCenterX);
	if (NO_ERR != result) { return false; }
	result = sReadIdFloat(missionFile, "TargetCenterY", m_targetCenterY);
	if (NO_ERR != result) { return false; }
	result = sReadIdFloat(missionFile, "TargetRadius", m_targetRadius);
	if (NO_ERR != result) { return false; }
	return true;
}

bool CAreaObjectiveCondition::Save( FitIniFile* file )
{
	file->writeIdFloat( "TargetCenterX", m_targetCenterX );
	file->writeIdFloat( "TargetCenterY", m_targetCenterY );
	file->writeIdFloat( "TargetRadius", m_targetRadius );
	return true;
}

bool CAreaObjectiveCondition::EditDialog() {
	TargetAreaDlg TargetAreaDialog(m_targetCenterX, m_targetCenterY, m_targetRadius);
	int ret = TargetAreaDialog.DoModal();
	return (IDOK == ret);
}

EString CAreaObjectiveCondition::InstanceDescription() {
	EString tmpEStr;
	tmpEStr.Format("x: %.3f, y: %.3f, radius: %.3f", m_targetCenterX, m_targetCenterY, m_targetRadius);
	return tmpEStr;
}

EString CMoveAnyUnitToArea::Description() {
	EString retval = "MoveAnyUnitToArea"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_MoveAnyUnitToArea, retval);
	return retval;
}

EString CMoveAllUnitsToArea::Description() {
	EString retval = "MoveAllUnitsToArea"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_MoveAllUnitsToArea, retval);
	return retval;
}

EString CMoveAllSurvivingUnitsToArea::Description() {
	EString retval = "MoveAllSurvivingUnitsToArea"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_MoveAllSurvivingUnitsToArea, retval);
	return retval;
}

EString CMoveAllSurvivingMechsToArea::Description() {
	EString retval = "MoveAllSurvivingMechsToArea"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_MoveAllSurvivingMechsToArea, retval);
	return retval;
}

bool CBooleanFlagIsSet::operator==(const CObjectiveCondition &rhs) const {
	bool retval = false;
	const CBooleanFlagIsSet *pCastedRhs = dynamic_cast<const CBooleanFlagIsSet *>(&rhs);
	if ((pCastedRhs) && (CObjectiveCondition::operator==(rhs))) {
		if ((m_flagID == pCastedRhs->m_flagID) &&
			(m_value == pCastedRhs->m_value)) {
			retval = true;
		}
	}
	return retval;
}

bool CBooleanFlagIsSet::Read( FitIniFile* missionFile )
{
	long result = 0;
	ECharString tmpECStr;
	result = sReadIdString(missionFile, "FlagID", tmpECStr);
	if (NO_ERR != result) {
		/*for backward compatibility*/
		int flagIndex = 0;
		result = sReadIdWholeNum(missionFile, "FlagIndex", flagIndex);
		if (NO_ERR != result) { return false; }
		m_flagID.Format("%d", flagIndex);
	} else {
		m_flagID = tmpECStr.Data();
	}
	result = sReadIdBoolean(missionFile, "Value", m_value);
	return true;
}

bool CBooleanFlagIsSet::Save( FitIniFile* file )
{
	file->writeIdString("FlagID", m_flagID.Data());
	file->writeIdBoolean("Value", m_value);
	return true;
}

bool CBooleanFlagIsSet::EditDialog() {
	CBooleanFlagIsSetDialog BooleanFlagIsSetDlg;
	BooleanFlagIsSetDlg.m_FlagID = m_flagID.Data();
	BooleanFlagIsSetDlg.m_Value = 1;
	if (false == m_value) { BooleanFlagIsSetDlg.m_Value = 0; }
	if (IDOK == BooleanFlagIsSetDlg.DoModal()) {
		m_flagID = BooleanFlagIsSetDlg.m_FlagID.GetBuffer(0);
		m_value = true;
		if (0 == BooleanFlagIsSetDlg.m_Value) { m_value = false; }
		return true;
	} else {
		return false;
	}
}

EString CBooleanFlagIsSet::Description() {
	EString retval = "BooleanFlagIsSet"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_BooleanFlagIsSet, retval);
	return retval;
}

EString CBooleanFlagIsSet::InstanceDescription() {
	EString tmpEStr;
	tmpEStr.Format("flag name: %s, value: %d", m_flagID.Data(), m_value);
	return tmpEStr;
}

bool CElapsedMissionTime::operator==(const CObjectiveCondition &rhs) const {
	bool retval = false;
	const CElapsedMissionTime *pCastedRhs = dynamic_cast<const CElapsedMissionTime *>(&rhs);
	if ((pCastedRhs) && (CObjectiveCondition::operator==(rhs))) {
		if (m_time == pCastedRhs->m_time) {
			retval = true;
		}
	}
	return retval;
}

bool CElapsedMissionTime::Read( FitIniFile* missionFile )
{
	long result = 0;
	result = sReadIdFloat(missionFile, "Time", m_time);
	if (NO_ERR != result) { return false; }
	return true;
}

bool CElapsedMissionTime::Save( FitIniFile* file )
{
	file->writeIdFloat("Time", m_time);
	return true;
}

bool CElapsedMissionTime::EditDialog() {
	SingleValueDlg ElapsedMissionTimeDialog( IDS_ELAPSED_TIME, IDS_IN_SECONDS);
	ElapsedMissionTimeDialog.SetVal((int)m_time);
	if( ElapsedMissionTimeDialog.DoModal()==IDOK ) {
		m_time = (float)ElapsedMissionTimeDialog.GetVal();
		return true;
	} else {
		return false;
	}
}

EString CElapsedMissionTime::Description() {
	EString retval = "ElapsedMissionTime"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_ElapsedMissionTime, retval);
	return retval;
}

EString CElapsedMissionTime::InstanceDescription() {
	EString tmpEStr;
	tmpEStr.Format("%f seconds", m_time);
	return tmpEStr;
}

bool CObjectiveAction::operator==(const CObjectiveAction &rhs) const {
	bool retval = false;
	if ((Species() == rhs.Species()) && (m_alignment == rhs.m_alignment)) {
		retval = true;
	}
	return retval;
}

bool CPlayBIK::operator==(const CObjectiveAction &rhs) const {
	bool retval = false;
	const CPlayBIK *pCastedRhs = dynamic_cast<const CPlayBIK *>(&rhs);
	if ((pCastedRhs) && (CObjectiveAction::operator==(rhs))) {
		if (m_pathname == pCastedRhs->m_pathname) {
			retval = true;
		}
	}
	return retval;
}

bool CPlayBIK::Read( FitIniFile* missionFile )
{
	long result = 0;
	ECharString tmpECStr;
	result = sReadIdString(missionFile, "AVIFileName", tmpECStr);
	if (NO_ERR != result) { return false; }
	m_pathname = tmpECStr.Data();
	return true;
}

bool CPlayBIK::Save( FitIniFile* file )
{
	file->writeIdString( "AVIFileName", m_pathname.Data());
	return true;
}

bool CPlayBIK::EditDialog() {
	while (true) {
		CFileDialog selectAVIFileDialog(TRUE,_T("AVI"),_T("*.AVI"),
						 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
						 _T("Movie (*.AVI)|*.AVI|"));
		selectAVIFileDialog.m_ofn.lpstrInitialDir = moviePath;
		if( selectAVIFileDialog.DoModal()==IDOK ) {
			CString pathname = selectAVIFileDialog.GetPathName();
			int CurrentDirectoryBufferLength = GetCurrentDirectory(0, 0);
			assert(1 <= CurrentDirectoryBufferLength);
			TCHAR *CurrentDirectoryBuffer = new TCHAR[CurrentDirectoryBufferLength];
			int ret = GetCurrentDirectory(CurrentDirectoryBufferLength, CurrentDirectoryBuffer);
			assert(CurrentDirectoryBufferLength - 1 == ret);
			ret = -1;
			if (pathname.GetLength() > (CurrentDirectoryBufferLength - 1)) {
				ret = _tcsnicmp(CurrentDirectoryBuffer, pathname.GetBuffer(0), CurrentDirectoryBufferLength - 1);
			}
			delete [] CurrentDirectoryBuffer; CurrentDirectoryBuffer = 0;
			if (0 == ret) {
				m_pathname = (pathname.GetBuffer(0) + (CurrentDirectoryBufferLength - 1) + 1);
				return true;
			} else {
				AfxMessageBox(IDS_MUST_BE_IN_SUBDIRECTORY);
			}
		} else {
			return false;
		}
	}
}

EString CPlayBIK::Description() {
	EString retval = "PlayBIK"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_PlayBIK, retval);
	return retval;
}

EString CPlayBIK::InstanceDescription() {
	EString tmpEStr;
	tmpEStr.Format("%s", m_pathname.Data());
	return tmpEStr;
}

bool CPlayWAV::operator==(const CObjectiveAction &rhs) const {
	bool retval = false;
	const CPlayWAV *pCastedRhs = dynamic_cast<const CPlayWAV *>(&rhs);
	if ((pCastedRhs) && (CObjectiveAction::operator==(rhs))) {
		if (m_pathname == pCastedRhs->m_pathname) {
			retval = true;
		}
	}
	return retval;
}

bool CPlayWAV::Read( FitIniFile* missionFile )
{
	long result = 0;
	ECharString tmpECStr;
	result = sReadIdString(missionFile, "WAVFileName", tmpECStr);
	if (NO_ERR != result) { return false; }
	m_pathname = tmpECStr.Data();
	return true;
}

bool CPlayWAV::Save( FitIniFile* file )
{
	file->writeIdString( "WAVFileName", m_pathname.Data());
	return true;
}

bool CPlayWAV::EditDialog() {
	while (true) {
		CFileDialog selectWAVFileDialog(TRUE,_T("WAV"),_T("*.WAV"),
						 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
						 _T("Wavefile (*.WAV)|*.WAV|"));
		selectWAVFileDialog.m_ofn.lpstrInitialDir = soundPath;
		if( selectWAVFileDialog.DoModal()==IDOK ) {
			CString pathname = selectWAVFileDialog.GetPathName();
			int CurrentDirectoryBufferLength = GetCurrentDirectory(0, 0);
			assert(1 <= CurrentDirectoryBufferLength);
			TCHAR *CurrentDirectoryBuffer = new TCHAR[CurrentDirectoryBufferLength];
			int ret = GetCurrentDirectory(CurrentDirectoryBufferLength, CurrentDirectoryBuffer);
			assert(CurrentDirectoryBufferLength - 1 == ret);
			ret = -1;
			if (pathname.GetLength() > (CurrentDirectoryBufferLength - 1)) {
				ret = _tcsnicmp(CurrentDirectoryBuffer, pathname.GetBuffer(0), CurrentDirectoryBufferLength - 1);
			}
			delete CurrentDirectoryBuffer; CurrentDirectoryBuffer = 0;
			if (0 == ret) {
				m_pathname = (pathname.GetBuffer(0) + (CurrentDirectoryBufferLength - 1) + 1);
				return true;
			} else {
				AfxMessageBox(IDS_MUST_BE_IN_SUBDIRECTORY);
			}
		} else {
			return false;
		}
	}
}

EString CPlayWAV::Description() {
	EString retval = "PlayWAV"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_PlayWAV, retval);
	return retval;
}

EString CPlayWAV::InstanceDescription() {
	EString tmpEStr;
	tmpEStr.Format("%s", m_pathname.Data());
	return tmpEStr;
}

bool CDisplayTextMessage::operator==(const CObjectiveAction &rhs) const {
	bool retval = false;
	const CDisplayTextMessage *pCastedRhs = dynamic_cast<const CDisplayTextMessage *>(&rhs);
	if ((pCastedRhs) && (CObjectiveAction::operator==(rhs))) {
		if (m_message == pCastedRhs->m_message) {
			retval = true;
		}
	}
	return retval;
}

bool CDisplayTextMessage::Read( FitIniFile* missionFile )
{
	long result = 0;
	ECharString tmpECStr;
	result = sReadIdString(missionFile, "Message", tmpECStr);
	if (NO_ERR != result) { return false; }
	m_message = tmpECStr.Data();
	return true;
}

bool CDisplayTextMessage::Save( FitIniFile* file )
{
	file->writeIdString( "Message", m_message.Data());
	return true;
}

bool CDisplayTextMessage::EditDialog() {
	TextMessageDlg textMessageDlg;
	textMessageDlg.m_TextMessage = m_message.Data();
	int result = textMessageDlg.DoModal();
	if (IDOK == result) {
		m_message = textMessageDlg.m_TextMessage.GetBuffer(0);
		return true;
	}
	return false;
}

EString CDisplayTextMessage::Description() {
	EString retval = "DisplayTextMessage"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_DisplayTextMessage, retval);
	return retval;
}

EString CDisplayTextMessage::InstanceDescription() {
	EString tmpEStr;
	tmpEStr.Format("%s", m_message.Data());
	return tmpEStr;
}

bool CDisplayResourceTextMessage::operator==(const CObjectiveAction &rhs) const {
	bool retval = false;
	const CDisplayResourceTextMessage *pCastedRhs = dynamic_cast<const CDisplayResourceTextMessage *>(&rhs);
	if ((pCastedRhs) && (CObjectiveAction::operator==(rhs))) {
		if (m_resourceStringID == pCastedRhs->m_resourceStringID) {
			retval = true;
		}
	}
	return retval;
}

bool CDisplayResourceTextMessage::Read( FitIniFile* missionFile )
{
	long result = 0;
	result = sReadIdWholeNum(missionFile, "ResourceStringID", m_resourceStringID);
	if (NO_ERR != result) { return false; }
	return true;
}

bool CDisplayResourceTextMessage::Save( FitIniFile* file )
{
	file->writeIdULong( "ResourceStringID", m_resourceStringID);
	return true;
}

bool CDisplayResourceTextMessage::EditDialog() {
	ResourceStringSelectionDlg resourceStringSelectionDlg;
	//resourceStringSelectionDlg.m_BottomOfIDRange = ;
	//resourceStringSelectionDlg.m_TopOfIDRange = ;
	resourceStringSelectionDlg.m_SelectedResourceStringID = m_resourceStringID;
	int result = resourceStringSelectionDlg.DoModal();
	if (IDOK == result) {
		m_resourceStringID = resourceStringSelectionDlg.m_SelectedResourceStringID;
		return true;
	}
	return false;
}

EString CDisplayResourceTextMessage::Description() {
	EString retval = "DisplayResourceTextMessage"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_DisplayResourceTextMessage, retval);
	return retval;
}

EString CDisplayResourceTextMessage::InstanceDescription() {
	EString resourceText;
	ESLoadString(m_resourceStringID, resourceText);
	EString tmpEStr;
	tmpEStr.Format("%d: %s", m_resourceStringID, resourceText.Data());
	return tmpEStr;
}

bool CSetBooleanFlag::operator==(const CObjectiveAction &rhs) const {
	bool retval = false;
	const CSetBooleanFlag *pCastedRhs = dynamic_cast<const CSetBooleanFlag *>(&rhs);
	if ((pCastedRhs) && (CObjectiveAction::operator==(rhs))) {
		if ((m_flagID == pCastedRhs->m_flagID) &&
			(m_value == pCastedRhs->m_value)) {
			retval = true;
		}
	}
	return retval;
}

bool CSetBooleanFlag::Read( FitIniFile* missionFile )
{
	long result = 0;
	ECharString tmpECStr;
	result = sReadIdString(missionFile, "FlagID", tmpECStr);
	if (NO_ERR != result) {
		/*for backward compatibility*/
		int flagIndex = 0;
		result = sReadIdWholeNum(missionFile, "FlagIndex", flagIndex);
		if (NO_ERR != result) { return false; }
		m_flagID.Format("%d", flagIndex);
	} else {
		m_flagID = tmpECStr.Data();
	}
	result = sReadIdBoolean(missionFile, "Value", m_value);
	return true;
}

bool CSetBooleanFlag::Save( FitIniFile* file )
{
	file->writeIdString("FlagID", m_flagID.Data());
	file->writeIdBoolean("Value", m_value);
	return true;
}

bool CSetBooleanFlag::EditDialog() {
	CBooleanFlagIsSetDialog BooleanFlagIsSetDlg;
	BooleanFlagIsSetDlg.m_FlagID = m_flagID.Data();
	BooleanFlagIsSetDlg.m_Value = 1;
	if (false == m_value) { BooleanFlagIsSetDlg.m_Value = 0; }
	if (IDOK == BooleanFlagIsSetDlg.DoModal()) {
		m_flagID = BooleanFlagIsSetDlg.m_FlagID.GetBuffer(0);
		m_value = true;
		if (0 == BooleanFlagIsSetDlg.m_Value) { m_value = false; }
		return true;
	} else {
		return false;
	}
}

EString CSetBooleanFlag::Description() {
	EString retval = "SetBooleanFlag"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_SetBooleanFlag, retval);
	return retval;
}

EString CSetBooleanFlag::InstanceDescription() {
	EString tmpEStr;
	tmpEStr.Format("flag name: %s, value: %d", m_flagID.Data(), m_value);
	return tmpEStr;
}

bool CMakeNewTechnologyAvailable::operator==(const CObjectiveAction &rhs) const {
	bool retval = false;
	const CMakeNewTechnologyAvailable *pCastedRhs = dynamic_cast<const CMakeNewTechnologyAvailable *>(&rhs);
	if ((pCastedRhs) && (CObjectiveAction::operator==(rhs))) {
		if (m_purchaseFilePathname == pCastedRhs->m_purchaseFilePathname) {
			retval = true;
		}
	}
	return retval;
}

bool CMakeNewTechnologyAvailable::Read( FitIniFile* missionFile )
{
	long result = 0;
	ECharString tmpECStr;
	result = sReadIdString(missionFile, "PurchaseFileName", tmpECStr);
	if (NO_ERR != result) { return false; }
	m_purchaseFilePathname = tmpECStr.Data();
	return true;
}

bool CMakeNewTechnologyAvailable::Save( FitIniFile* file )
{
	file->writeIdString( "PurchaseFileName", m_purchaseFilePathname.Data());
	return true;
}

bool CMakeNewTechnologyAvailable::EditDialog() {
	while (true) {
		CFileDialog selectPurchaseFileDialog(TRUE,_T("FIT"),_T("*.FIT"),
						 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
						 _T("Purchase File (*.FIT)|*.FIT|"));
		selectPurchaseFileDialog.m_ofn.lpstrInitialDir = missionPath;
		if( selectPurchaseFileDialog.DoModal()==IDOK ) {
			CString pathname = selectPurchaseFileDialog.GetPathName();
			int CurrentDirectoryBufferLength = GetCurrentDirectory(0, 0);
			assert(1 <= CurrentDirectoryBufferLength);
			TCHAR *CurrentDirectoryBuffer = new TCHAR[CurrentDirectoryBufferLength];
			int ret = GetCurrentDirectory(CurrentDirectoryBufferLength, CurrentDirectoryBuffer);
			assert(CurrentDirectoryBufferLength - 1 == ret);
			ret = -1;
			if (pathname.GetLength() > (CurrentDirectoryBufferLength - 1)) {
				ret = _tcsnicmp(CurrentDirectoryBuffer, pathname.GetBuffer(0), CurrentDirectoryBufferLength - 1);
			}
			delete CurrentDirectoryBuffer; CurrentDirectoryBuffer = 0;
			if (0 == ret) {
				m_purchaseFilePathname = (pathname.GetBuffer(0) + (CurrentDirectoryBufferLength - 1) + 1);
				return true;
			} else {
				/* Put a dialog box saying that the wav must be in a subdirectory of the runtime 
				directory. */
			}
		} else {
			return false;
		}
	}
}

EString CMakeNewTechnologyAvailable::Description() {
	EString retval = "MakeNewTechnologyAvailable"; /* needs to be put somewhere localizable */
	ESLoadString(IDS_MakeNewTechnologyAvailable, retval);
	return retval;
}

EString CMakeNewTechnologyAvailable::InstanceDescription() {
	EString tmpEStr;
	tmpEStr.Format("%s", m_purchaseFilePathname.Data());
	return tmpEStr;
}

CObjectiveConditionList &CObjectiveConditionList::operator=(const CObjectiveConditionList &master) {
	Clear();
	CObjectiveConditionList::EConstIterator it = master.Begin();
	while (!it.IsDone()) {
		condition_species_type conditionSpecies = (*it)->Species();
		CObjectiveCondition *pTmpCondition = CObjective::new_CObjectiveCondition(conditionSpecies, (*it)->Alignment());
		(*pTmpCondition).CastAndCopy(*it);
		Append(pTmpCondition);
		it++;
	}
	return (*this);
}

bool CObjectiveConditionList::operator==(const CObjectiveConditionList &rhs) const {
	bool retval = true;
	if (Count() != rhs.Count()) {
		retval = false;
	} else {
		CObjectiveConditionList::EConstIterator it = Begin();
		CObjectiveConditionList::EConstIterator it2 = rhs.Begin();
		while (!it.IsDone()) {
			assert(!it2.IsDone()); assert(*it); assert(*it2);
			if (!((*(*it)) == (*(*it2)))) {
				retval = false;
				break;
			}
			it++;
			it2++;
		}
	}
	return retval;
}

void CObjectiveConditionList::Clear() {
	EIterator it = Begin();
	while (!it.IsDone()) {
		delete (*it);
		it++;
	}
	EList <CObjectiveCondition *, CObjectiveCondition *>::Clear();
}

CObjectiveActionList &CObjectiveActionList::operator=(const CObjectiveActionList &master) {
	CObjectiveActionList::EConstIterator it = master.Begin();
	while (!it.IsDone()) {
		action_species_type actionSpecies = (*it)->Species();
		CObjectiveAction *pTmpAction = CObjective::new_CObjectiveAction(actionSpecies, (*it)->Alignment());
		(*pTmpAction).CastAndCopy(*it);
		Append(pTmpAction);
		it++;
	}
	return (*this);
}

bool CObjectiveActionList::operator==(const CObjectiveActionList &rhs) const {
	bool retval = true;
	if (Count() != rhs.Count()) {
		retval = false;
	} else {
		CObjectiveActionList::EConstIterator it = Begin();
		CObjectiveActionList::EConstIterator it2 = rhs.Begin();
		while (!it.IsDone()) {
			assert(!it2.IsDone()); assert(*it); assert(*it2);
			if (!((*(*it)) == (*(*it2)))) {
				retval = false;
				break;
			}
			it++;
			it2++;
		}
	}
	return retval;
}

void CObjectiveActionList::Clear() {
	EIterator it = Begin();
	while (!it.IsDone()) {
		delete (*it);
		it++;
	}
	EList <CObjectiveAction *, CObjectiveAction *>::Clear();
}

CObjectiveCondition *CObjective::new_CObjectiveCondition(condition_species_type conditionSpecies, int alignment) {
	CObjectiveCondition *retval = 0;
	switch (conditionSpecies) {
	case DESTROY_ALL_ENEMY_UNITS: retval = new CDestroyAllEnemyUnits(alignment); break;
	case DESTROY_NUMBER_OF_ENEMY_UNITS: retval = new CDestroyNumberOfEnemyUnits(alignment); break;
	case DESTROY_SPECIFIC_ENEMY_UNIT: retval = new CDestroySpecificEnemyUnit(alignment); break;
	case DESTROY_SPECIFIC_STRUCTURE: retval = new CDestroySpecificStructure(alignment); break;
	case CAPTURE_OR_DESTROY_ALL_ENEMY_UNITS: retval = new CCaptureOrDestroyAllEnemyUnits(alignment); break;
	case CAPTURE_OR_DESTROY_NUMBER_OF_ENEMY_UNITS: retval = new CCaptureOrDestroyNumberOfEnemyUnits(alignment); break;
	case CAPTURE_OR_DESTROY_SPECIFIC_ENEMY_UNIT: retval = new CCaptureOrDestroySpecificEnemyUnit(alignment); break;
	case CAPTURE_OR_DESTROY_SPECIFIC_STRUCTURE: retval = new CCaptureOrDestroySpecificStructure(alignment); break;
	case DEAD_OR_FLED_ALL_ENEMY_UNITS: retval = new CDeadOrFledAllEnemyUnits(alignment); break;
	case DEAD_OR_FLED_NUMBER_OF_ENEMY_UNITS: retval = new CDeadOrFledNumberOfEnemyUnits(alignment); break;
	case DEAD_OR_FLED_SPECIFIC_ENEMY_UNIT: retval = new CDeadOrFledSpecificEnemyUnit(alignment); break;
	case CAPTURE_UNIT: retval = new CCaptureUnit(alignment); break;
	case CAPTURE_STRUCTURE: retval = new CCaptureStructure(alignment); break;
	case GUARD_SPECIFIC_UNIT: retval = new CGuardSpecificUnit(alignment); break;
	case GUARD_SPECIFIC_STRUCTURE: retval = new CGuardSpecificStructure(alignment); break;
	case MOVE_ANY_UNIT_TO_AREA: retval = new CMoveAnyUnitToArea(alignment); break;
	case MOVE_ALL_UNITS_TO_AREA: retval = new CMoveAllUnitsToArea(alignment); break;
	case MOVE_ALL_SURVIVING_UNITS_TO_AREA: retval = new CMoveAllSurvivingUnitsToArea(alignment); break;
	case MOVE_ALL_SURVIVING_MECHS_TO_AREA: retval = new CMoveAllSurvivingMechsToArea(alignment); break;
	case BOOLEAN_FLAG_IS_SET: retval = new CBooleanFlagIsSet(alignment); break;
	case ELAPSED_MISSION_TIME: retval = new CElapsedMissionTime(alignment); break;
	default:
		assert(false);
		retval = 0;
		break;
	}
	return retval;
}

EString CObjective::DescriptionOfConditionSpecies(condition_species_type conditionSpecies) {
	CString CStr;
	CStr.LoadString(IDS_UNIMPLEMENTED_CONDITION);
	EString retval;
	retval = CStr.GetBuffer(0);
	CObjectiveCondition *tmpCondition = new_CObjectiveCondition(conditionSpecies, 0/*dummy alignment*/);
	if (0 != tmpCondition) {
		retval = tmpCondition->Description();
	}
	delete tmpCondition;
	return retval;
}

CObjectiveAction *CObjective::new_CObjectiveAction(action_species_type actionSpecies, int alignment) {
	CObjectiveAction *retval = 0;
	switch (actionSpecies) {
	case PLAY_BIK: retval = new CPlayBIK(alignment); break;
	case PLAY_WAV: retval = new CPlayWAV(alignment); break;
	case DISPLAY_TEXT_MESSAGE: retval = new CDisplayTextMessage(alignment); break;
	case DISPLAY_RESOURCE_TEXT_MESSAGE: retval = new CDisplayResourceTextMessage(alignment); break;
	case SET_BOOLEAN_FLAG: retval = new CSetBooleanFlag(alignment); break;
	case MAKE_NEW_TECHNOLOGY_AVAILABLE: retval = new CMakeNewTechnologyAvailable(alignment); break;
	default:
		assert(false);
		retval = 0;
		break;
	}
	return retval;
}

EString CObjective::DescriptionOfActionSpecies(action_species_type actionSpecies) {
	CString CStr;
	CStr.LoadString(IDS_UNIMPLEMENTED_ACTION);
	EString retval;
	retval = CStr.GetBuffer(0);
	CObjectiveAction *tmpAction = new_CObjectiveAction(actionSpecies, 0/*dummy alignment*/);
	if (0 != tmpAction) {
		retval = tmpAction->Description();
	}
	delete tmpAction;
	return retval;
}

void CObjective::Construct(int alignment) {
	m_alignment = alignment;
	Title(_TEXT(""));
	m_titleUseResourceString = false;
	m_titleResourceStringID = 0;
	Description(_TEXT(""));
	m_descriptionUseResourceString = false;
	m_descriptionResourceStringID = 0;
	m_priority = 1;
	m_resourcePoints = 0;
	m_previousPrimaryObjectiveMustBeComplete = false;
	m_allPreviousPrimaryObjectivesMustBeComplete = false;
	m_displayMarker = false;
	m_markerX = 0.0;
	m_markerY = 0.0;
	m_isHiddenTrigger = false;
	m_activateOnFlag = false;
	ActivateFlagID(_TEXT(""));
	m_resetStatusOnFlag = false;
	ResetStatusFlagID(_TEXT(""));
	m_baseColor = -1;
	m_highlightColor = -1;
	m_highlightColor2 = -1;
	m_modelID = -1;
}

CObjective &CObjective::operator=(const CObjective &master) {
	Clear();
	m_alignment = master.m_alignment;
	Title(master.Title());
	m_titleUseResourceString = master.m_titleUseResourceString;
	m_titleResourceStringID = master.m_titleResourceStringID;
	Description(master.Description());
	m_descriptionUseResourceString = master.m_descriptionUseResourceString;
	m_descriptionResourceStringID = master.m_descriptionResourceStringID;
	m_priority = master.m_priority;
	m_resourcePoints = master.m_resourcePoints;
	m_previousPrimaryObjectiveMustBeComplete = master.m_previousPrimaryObjectiveMustBeComplete;
	m_allPreviousPrimaryObjectivesMustBeComplete = master.m_allPreviousPrimaryObjectivesMustBeComplete;
	m_displayMarker = master.m_displayMarker;
	m_markerX = master.m_markerX;
	m_markerY = master.m_markerY;
	m_isHiddenTrigger = master.m_isHiddenTrigger;
	m_activateOnFlag = master.m_activateOnFlag;
	m_activateFlagID = master.m_activateFlagID;
	m_resetStatusOnFlag = master.m_resetStatusOnFlag;
	m_resetStatusFlagID = master.m_resetStatusFlagID;
	m_modelID = master.m_modelID;
	m_highlightColor = master.m_highlightColor;
	m_highlightColor2 = master.m_highlightColor2;
	m_baseColor = master.m_baseColor;

	CObjectiveConditionList::operator=(master);
	m_actionList = master.m_actionList;
	m_failureConditionList = master.m_failureConditionList;
	m_failureActionList = master.m_failureActionList;

	return (*this);
}

bool CObjective::operator==(const CObjective &rhs) const {
	bool retval = false;
	if ((m_alignment == rhs.m_alignment) &&
		(Title() == rhs.Title()) &&
		(m_titleUseResourceString == rhs.m_titleUseResourceString) &&
		(m_titleResourceStringID == rhs.m_titleResourceStringID) &&
		(Description() == rhs.Description()) &&
		(m_descriptionUseResourceString == rhs.m_descriptionUseResourceString) &&
		(m_descriptionResourceStringID == rhs.m_descriptionResourceStringID) &&
		(m_priority == rhs.m_priority) &&
		(m_resourcePoints == rhs.m_resourcePoints) &&
		(m_previousPrimaryObjectiveMustBeComplete == rhs.m_previousPrimaryObjectiveMustBeComplete) &&
		(m_allPreviousPrimaryObjectivesMustBeComplete == rhs.m_allPreviousPrimaryObjectivesMustBeComplete) &&
		(m_displayMarker == rhs.m_displayMarker) &&
		(m_markerX == rhs.m_markerX) &&
		(m_markerY == rhs.m_markerY) &&
		(m_isHiddenTrigger == rhs.m_isHiddenTrigger) &&
		(m_activateOnFlag == rhs.m_activateOnFlag) &&
		(m_activateFlagID == rhs.m_activateFlagID) &&
		(m_resetStatusOnFlag == rhs.m_resetStatusOnFlag) &&
		(m_resetStatusFlagID == rhs.m_resetStatusFlagID) &&
		(m_modelID == rhs.m_modelID) &&
		(m_highlightColor == rhs.m_highlightColor) &&
		(m_highlightColor2 == rhs.m_highlightColor2) &&
		(m_baseColor == rhs.m_baseColor)) {
		if ((inherited::operator==(rhs)) &&
			(m_actionList == rhs.m_actionList) &&
			(m_failureConditionList == rhs.m_failureConditionList) &&
			(m_failureActionList == rhs.m_failureActionList)) {
			retval = true;
		}
	}
	return retval;
}

void CObjective::Clear() {
	inherited::Clear();
	m_actionList.Clear();
	m_failureConditionList.Clear();
	m_failureActionList.Clear();
}

void CObjective::Alignment(int alignment) {
	m_alignment = alignment;
	{
		EIterator it = Begin();
		while (!it.IsDone()) {
			(*it)->Alignment(m_alignment);
			it++;
		}
	}

	{
		action_list_type::EIterator it = m_actionList.Begin();
		while (!it.IsDone()) {
			(*it)->Alignment(m_alignment);
			it++;
		}
	}

	{
		condition_list_type::EIterator it = m_failureConditionList.Begin();
		while (!it.IsDone()) {
			(*it)->Alignment(m_alignment);
			it++;
		}
	}

	{
		action_list_type::EIterator it = m_failureActionList.Begin();
		while (!it.IsDone()) {
			(*it)->Alignment(m_alignment);
			it++;
		}
	}
}

static condition_species_type ConditionSpeciesMap(const char *speciesString) {
	condition_species_type retval = DESTROY_ALL_ENEMY_UNITS;
	int i;
	for (i = 0; i < (int)NUM_CONDITION_SPECIES; i += 1) {
		if (0 == strcmp(speciesString, g_conditionSpeciesStringArray[i])) {
			retval = (condition_species_type)i;
			break;
		}
	}
	assert(i < (int)NUM_CONDITION_SPECIES);
	return retval;
}

static action_species_type ActionSpeciesMap(const char *speciesString) {
	action_species_type retval = PLAY_BIK;
	int i;
	for (i = 0; i < (int)NUM_ACTION_SPECIES; i += 1) {
		if (0 == strcmp(speciesString, g_actionSpeciesStringArray[i])) {
			retval = (action_species_type)i;
			break;
		}
	}
	assert(i < (int)NUM_ACTION_SPECIES);
	return retval;
}

bool CObjective::Read( FitIniFile* missionFile, int objectiveNum, int version )
{
	long result = 0;
	ECharString tmpECStr;
	result = sReadIdString(missionFile, "Title", tmpECStr);
	Title(_T(tmpECStr.Data()));
	result = sReadIdBoolean(missionFile, "TitleUseResourceString", m_titleUseResourceString);
	result = sReadIdWholeNum(missionFile, "TitleResourceStringID", m_titleResourceStringID);
	result = sReadIdString(missionFile, "Description", tmpECStr);
	Description(_T(tmpECStr.Data()));
	result = sReadIdBoolean(missionFile, "DescriptionUseResourceString", m_descriptionUseResourceString);
	result = sReadIdWholeNum(missionFile, "DescriptionResourceStringID", m_descriptionResourceStringID);

	result = sReadIdWholeNum(missionFile, "Priority", m_priority);
	result = sReadIdWholeNum(missionFile, "ResourcePoints", m_resourcePoints);
	result = sReadIdBoolean(missionFile, "PreviousPrimaryObjectiveMustBeComplete", m_previousPrimaryObjectiveMustBeComplete);
	if (NO_ERR != result) {
		result = sReadIdBoolean(missionFile, "PreviousObjectiveMustBeComplete", m_previousPrimaryObjectiveMustBeComplete);
	}
	result = sReadIdBoolean(missionFile, "AllPreviousPrimaryObjectivesMustBeComplete", m_allPreviousPrimaryObjectivesMustBeComplete);
	result = sReadIdBoolean(missionFile, "DisplayMarker", m_displayMarker);
	result = sReadIdFloat(missionFile, "MarkerX", m_markerX);
	result = sReadIdFloat(missionFile, "MarkerY", m_markerY);
	result = sReadIdBoolean(missionFile, "HiddenTrigger", m_isHiddenTrigger);
	result = sReadIdBoolean(missionFile, "ActivateOnFlag", m_activateOnFlag);
	result = sReadIdString(missionFile, "ActivateFlagID", tmpECStr);
	if (NO_ERR != result) {
		/*for backward compatibility*/
		int flagIndex = 0;
		result = sReadIdWholeNum(missionFile, "ActivateFlagIndex", flagIndex);
		if (NO_ERR == result) {
			m_activateFlagID.Format("%d", flagIndex);
		}
	} else {
		m_activateFlagID = tmpECStr.Data();
	}
	result = sReadIdBoolean(missionFile, "ResetStatusOnFlag", m_resetStatusOnFlag);
	result = sReadIdString(missionFile, "ResetStatusFlagID", tmpECStr);
	if (NO_ERR == result) {
		m_resetStatusFlagID = tmpECStr.Data();
	}

	if ( NO_ERR != missionFile->readIdLong( "BaseColor", m_baseColor ) )
		m_baseColor = -1;

	if ( NO_ERR != missionFile->readIdLong( "HighlightColor", m_highlightColor ) )
		m_highlightColor = -1;

	if ( missionFile->readIdLong( "HighlightColor2", m_highlightColor2 ) )
		m_highlightColor2 = -1;

	int modelIDVersion = 0;
	result = sReadIdWholeNum(missionFile, "ModelID Version", modelIDVersion);
	if ((NO_ERR != result) || (1 >= modelIDVersion) )
	{
		m_modelID = -1;
	} else {
		if ( NO_ERR != missionFile->readIdLong( "ModelID", m_modelID ) )
			m_modelID = -1;
	}

	


	int numConditions = 0;
	result = sReadIdWholeNum(missionFile, "NumConditions", numConditions);
	if (NO_ERR != result) { return false; }
	int numActions = 0;
	result = sReadIdWholeNum(missionFile, "NumActions", numActions);
	int numFailureConditions = 0;
	result = sReadIdWholeNum(missionFile, "NumFailureConditions", numFailureConditions);
	int numFailureActions = 0;
	result = sReadIdWholeNum(missionFile, "NumFailureActions", numFailureActions);

	{
		int i;
		for (i = 0; i < numConditions; i += 1) {
			ECharString tmpStr;
			if (2 == version) {
				tmpStr.Format("Objective%dCondition%d", objectiveNum, i);
			} else {
				tmpStr.Format("Team%dObjective%dCondition%d", Alignment(), objectiveNum, i);
			}
			result = missionFile->seekBlock(tmpStr.Data());
			if (NO_ERR != result) { assert(false); continue; }
			condition_species_type species;
			result = sReadIdString(missionFile, "ConditionSpeciesString", tmpECStr);
			if (NO_ERR != result) {
				/* this is just for backward compatibility, this entry is depricated */
				int ispecies = 0;
				result = sReadIdWholeNum(missionFile, "ConditionSpecies", ispecies);
				if (NO_ERR != result) { continue; }
				species = (condition_species_type)ispecies;
			} else {
				species = ConditionSpeciesMap(tmpECStr.Data());
			}

			CObjectiveCondition *pNewObjectiveCondition = new_CObjectiveCondition(species, m_alignment);
			if (!pNewObjectiveCondition) { assert(false); delete pNewObjectiveCondition; continue; }
			bool bresult = pNewObjectiveCondition->Read(missionFile);
			if (true != bresult) { assert(false); delete pNewObjectiveCondition; continue; }
			Append(pNewObjectiveCondition);
		}
	}

	{
		int i;
		for (i = 0; i < numActions; i += 1) {
			ECharString tmpStr;
			tmpStr.Format("Team%dObjective%dAction%d", Alignment(), objectiveNum, i);
			result = missionFile->seekBlock(tmpStr.Data());
			if (NO_ERR != result) { assert(false); continue; }
			action_species_type species;
			result = sReadIdString(missionFile, "ActionSpeciesString", tmpECStr);
			if (NO_ERR != result) { assert(false); continue; }
			species = ActionSpeciesMap(tmpECStr.Data());

			CObjectiveAction *pNewObjectiveAction = new_CObjectiveAction(species, m_alignment);
			if (!pNewObjectiveAction) { assert(false); delete pNewObjectiveAction; continue; }
			bool bresult = pNewObjectiveAction->Read(missionFile);
			if (true != bresult) { assert(false); delete pNewObjectiveAction; continue; }
			m_actionList.Append(pNewObjectiveAction);
		}
	}

	{
		int i;
		for (i = 0; i < numFailureConditions; i += 1) {
			ECharString tmpStr;
			tmpStr.Format("Team%dObjective%dFailureCondition%d", Alignment(), objectiveNum, i);
			result = missionFile->seekBlock(tmpStr.Data());
			if (NO_ERR != result) { assert(false); continue; }
			condition_species_type species;
			result = sReadIdString(missionFile, "FailureConditionSpeciesString", tmpECStr);
			if (NO_ERR != result) { assert(false); continue; }
			species = ConditionSpeciesMap(tmpECStr.Data());

			CObjectiveCondition *pNewObjectiveFailureCondition = new_CObjectiveCondition(species, m_alignment);
			if (!pNewObjectiveFailureCondition) { assert(false); delete pNewObjectiveFailureCondition; continue; }
			bool bresult = pNewObjectiveFailureCondition->Read(missionFile);
			if (true != bresult) { assert(false); delete pNewObjectiveFailureCondition; continue; }
			m_failureConditionList.Append(pNewObjectiveFailureCondition);
		}
	}

	{
		int i;
		for (i = 0; i < numFailureActions; i += 1) {
			ECharString tmpStr;
			tmpStr.Format("Team%dObjective%dFailureAction%d", Alignment(), objectiveNum, i);
			result = missionFile->seekBlock(tmpStr.Data());
			if (NO_ERR != result) { assert(false); continue; }
			action_species_type species;
			result = sReadIdString(missionFile, "FailureActionSpeciesString", tmpECStr);
			if (NO_ERR != result) { assert(false); continue; }
			species = ActionSpeciesMap(tmpECStr.Data());

			CObjectiveAction *pNewObjectiveFailureAction = new_CObjectiveAction(species, m_alignment);
			if (!pNewObjectiveFailureAction) { assert(false); delete pNewObjectiveFailureAction; continue; }
			bool bresult = pNewObjectiveFailureAction->Read(missionFile);
			if (true != bresult) { assert(false); delete pNewObjectiveFailureAction; continue; }
			m_failureActionList.Append(pNewObjectiveFailureAction);
		}
	}
	return true;
}

bool CObjective::Save( FitIniFile* file, int objectiveNum )
{
	sWriteIdString( file, "Title", Title().Data());
	file->writeIdBoolean( "TitleUseResourceString", m_titleUseResourceString );
	file->writeIdULong( "TitleResourceStringID", m_titleResourceStringID );
	sWriteIdString( file, "Description", Description().Data());
	file->writeIdBoolean( "DescriptionUseResourceString", m_descriptionUseResourceString );
	file->writeIdULong( "DescriptionResourceStringID", m_descriptionResourceStringID );
	file->writeIdULong( "Priority", m_priority );
	file->writeIdULong( "ResourcePoints", m_resourcePoints );
	file->writeIdBoolean( "PreviousPrimaryObjectiveMustBeComplete", m_previousPrimaryObjectiveMustBeComplete );
	file->writeIdBoolean( "AllPreviousPrimaryObjectivesMustBeComplete", m_allPreviousPrimaryObjectivesMustBeComplete );
	file->writeIdBoolean( "DisplayMarker", m_displayMarker );
	file->writeIdFloat( "MarkerX", m_markerX );
	file->writeIdFloat( "MarkerY", m_markerY );
	file->writeIdBoolean( "HiddenTrigger", m_isHiddenTrigger );
	file->writeIdBoolean( "ActivateOnFlag", m_activateOnFlag );
	file->writeIdString( "ActivateFlagID", m_activateFlagID );
	file->writeIdBoolean( "ResetStatusOnFlag", m_resetStatusOnFlag );
	file->writeIdString( "ResetStatusFlagID", m_resetStatusFlagID.Data() );

	file->writeIdULong( "NumConditions", Count() );
	file->writeIdULong( "NumActions", m_actionList.Count() );
	file->writeIdULong( "NumFailureConditions", m_failureConditionList.Count() );
	file->writeIdULong( "NumFailureActions", m_failureActionList.Count() );

	const char* pName = "";
	int type = 0;
	float scale = 1.0;
	if ( m_modelID != -1 )
	{
		unsigned long ulGroup, ulIndex;
		EditorObjectMgr::instance()->getBuildingFromID( m_modelID, ulGroup, ulIndex, true);
		int ID = EditorObjectMgr::instance()->getID( ulGroup, ulIndex );
		pName = EditorObjectMgr::instance()->getFileName( ID );
		type = (ID >> 24) & 0xff;
		scale = EditorObjectMgr::instance()->getScale( ID );

	}
	file->writeIdString( "ObjectiveModel", pName );
	file->writeIdLong( "ModelType", type );
	file->writeIdLong( "BaseColor", m_baseColor );
	file->writeIdLong( "HighlightColor", m_highlightColor );
	file->writeIdLong( "HighlightColor2", m_highlightColor2 );
	file->writeIdULong( "ModelID Version", 2 );
	file->writeIdLong( "ModelID", m_modelID );
	file->writeIdFloat( "ModelScale", scale );

	

	{
		int i = 0;
		EIterator it = Begin();
		while (!it.IsDone()) {
			ECharString tmpStr;
			tmpStr.Format("Team%dObjective%dCondition%d", Alignment(), objectiveNum, i);
			file->writeBlock( tmpStr.Data() );
			file->writeIdULong( "ConditionSpecies", (unsigned long)(*it)->Species());
			file->writeIdString( "ConditionSpeciesString", g_conditionSpeciesStringArray[(int)(*it)->Species()]);
			(*it)->Save(file);
			i += 1;
			it++;
		}
	}

	{
		int i = 0;
		action_list_type::EIterator it = m_actionList.Begin();
		while (!it.IsDone()) {
			ECharString tmpStr;
			tmpStr.Format("Team%dObjective%dAction%d", Alignment(), objectiveNum, i);
			file->writeBlock( tmpStr.Data() );
			file->writeIdULong( "ActionSpecies", (unsigned long)(*it)->Species());
			file->writeIdString( "ActionSpeciesString", g_actionSpeciesStringArray[(int)(*it)->Species()]);
			(*it)->Save(file);
			i += 1;
			it++;
		}
	}

	{
		int i = 0;
		condition_list_type::EIterator it = m_failureConditionList.Begin();
		while (!it.IsDone()) {
			ECharString tmpStr;
			tmpStr.Format("Team%dObjective%dFailureCondition%d", Alignment(), objectiveNum, i);
			file->writeBlock( tmpStr.Data() );
			file->writeIdULong( "FailureConditionSpecies", (unsigned long)(*it)->Species());
			file->writeIdString( "FailureConditionSpeciesString", g_conditionSpeciesStringArray[(int)(*it)->Species()]);
			(*it)->Save(file);
			i += 1;
			it++;
		}
	}

	{
		int i = 0;
		action_list_type::EIterator it = m_failureActionList.Begin();
		while (!it.IsDone()) {
			ECharString tmpStr;
			tmpStr.Format("Team%dObjective%dFailureAction%d", Alignment(), objectiveNum, i);
			file->writeBlock( tmpStr.Data() );
			file->writeIdULong( "FailureActionSpecies", (unsigned long)(*it)->Species());
			file->writeIdString( "FailureActionSpeciesString", g_actionSpeciesStringArray[(int)(*it)->Species()]);
			(*it)->Save(file);
			i += 1;
			it++;
		}
	}

	return true;
}

bool CObjective::EditDialog() {
	ObjectiveDlg ObjectiveDialog;

	if (EditorInterface::instance()->ObjectSelectOnlyMode()) {
		ObjectiveDialog.m_ModifiedObjective = EditorInterface::instance()->objectivesEditState.ModifiedObjective;
		ObjectiveDialog.nConditionSpeciesSelectionIndex = EditorInterface::instance()->objectivesEditState.nConditionSpeciesSelectionIndex;
		ObjectiveDialog.nActionSpeciesSelectionIndex = EditorInterface::instance()->objectivesEditState.nActionSpeciesSelectionIndex;
		ObjectiveDialog.nFailureConditionSpeciesSelectionIndex = EditorInterface::instance()->objectivesEditState.nFailureConditionSpeciesSelectionIndex;
		ObjectiveDialog.nFailureActionSpeciesSelectionIndex = EditorInterface::instance()->objectivesEditState.nFailureActionSpeciesSelectionIndex;
	} else {
		ObjectiveDialog.m_ModifiedObjective = (*this);
		ObjectiveDialog.nConditionSpeciesSelectionIndex = -1;
		ObjectiveDialog.nActionSpeciesSelectionIndex = -1;
		ObjectiveDialog.nFailureConditionSpeciesSelectionIndex = -1;
		ObjectiveDialog.nFailureActionSpeciesSelectionIndex = -1;
	}

	int ret = ObjectiveDialog.DoModal();

	if (!EditorInterface::instance()->ObjectSelectOnlyMode()) {
		if (IDOK == ret) {
			(*this) = ObjectiveDialog.m_ModifiedObjective;
		}
	}
	return (IDOK == ret);
}

bool CObjective::NoteThePositionsOfObjectsReferenced() {
	bool retval = true;
	{
		EIterator it = Begin();
		while (!it.IsDone()) {
			bool res = (*it)->NoteThePositionsOfObjectsReferenced();
			if (false == res) {
				retval = false;
			}
			it++;
		}
	}
	{
		condition_list_type::EIterator it = m_failureConditionList.Begin();
		while (!it.IsDone()) {
			bool res = (*it)->NoteThePositionsOfObjectsReferenced();
			if (false == res) {
				retval = false;
			}
			it++;
		}
	}
	return retval;
}

bool CObjective::RestoreObjectPointerReferencesFromNotedPositions() {
	bool retval = true;
	{
		EIterator it = Begin();
		while (!it.IsDone()) {
			bool res = (*it)->RestoreObjectPointerReferencesFromNotedPositions();
			if (false == res) {
				retval = false;
			}
			it++;
		}
	}
	{
		condition_list_type::EIterator it = m_failureConditionList.Begin();
		while (!it.IsDone()) {
			bool res = (*it)->RestoreObjectPointerReferencesFromNotedPositions();
			if (false == res) {
				retval = false;
			}
			it++;
		}
	}
	return retval;
}

static BOOL CSLoadString(int resourceID, CString &targetStr) {
	char szTmp[16384/*max string length*/];
	cLoadString( resourceID, szTmp, 16384/*max string length*/ );
	targetStr = szTmp;
	CString tmpStr;
	tmpStr.Format("mc2res.dll:%d Not defined", resourceID);
	if (0 == strcmp(tmpStr.GetBuffer(0), szTmp)) {
		return (0);
	}
	return (!0);
}

EString CObjective::LocalizedTitle() const {
	EString retval;
	if (TitleUseResourceString()) {
		CString CStr;
		CSLoadString(TitleResourceStringID(), CStr);
		retval = CStr.GetBuffer(0);
	} else {
		retval = Title();
	}
	return retval;
}

EString CObjective::LocalizedDescription() const {
	EString retval;
	if (DescriptionUseResourceString()) {
		CString CStr;
		CSLoadString(DescriptionResourceStringID(), CStr);
		retval = CStr.GetBuffer(0);
	} else {
		retval = Description();
	}
	return retval;
}

CObjectives &CObjectives::operator=(const CObjectives &master) {
	m_alignment = master.m_alignment;

	Clear();
	CObjectives::EConstIterator it = master.Begin();
	while (!it.IsDone()) {
		//CObjective *tmpObjective = new CObjective(*(*it));
		CObjective *tmpObjective = new CObjective(m_alignment);
		(*tmpObjective) = (*(*it));
		Append(tmpObjective);
		it++;
	}
	return (*this);
}

bool CObjectives::operator==(const CObjectives &rhs) const {
	bool retval = true;
	if (m_alignment != rhs.m_alignment) {
		retval = false;
	} else if (Count() != rhs.Count()) {
		retval = false;
	} else {
		EConstIterator it = Begin();
		EConstIterator it2 = rhs.Begin();
		while (!it.IsDone()) {
			if (!((*(*it)) == (*(*it2)))) {
				retval = false;
				break;
			}
			it2++;
			it++;
		}
	}
	return retval;
}

void CObjectives::Clear() {
	EIterator it = Begin();
	while (!it.IsDone()) {
		delete (*it);
		it++;
	}
	inherited::Clear();
}

void CObjectives::Alignment(int alignment) {
	m_alignment = alignment;
	EIterator it = Begin();
	while (!it.IsDone()) {
		(*it)->Alignment(m_alignment);
		it++;
	}
}

bool CObjectives::Read( FitIniFile* missionFile )
{
	long result = 0;
	result = missionFile->seekBlock("Objectives Version");
	if (NO_ERR != result) { assert(false); }
	int objectivesVersion = 0;
	result = sReadIdWholeNum(missionFile, "Version", objectivesVersion);
	if (result != NO_ERR) {
		assert(false); return false;
	} else {
		if (2 == objectivesVersion) {
			if (0 != Alignment()) {
				// assign version 2 objectives to team1 only
				return true;
			}
			result = missionFile->seekBlock("Objectives");
		} else {
			ECharString tmpStr;
			tmpStr.Format("Team%dObjectives", Alignment());
			result = missionFile->seekBlock(tmpStr.Data());
		}
		if (NO_ERR != result) { assert(false); }
		int numObjectives = 0;
		result = sReadIdWholeNum(missionFile, "NumObjectives", numObjectives);
		if (NO_ERR != result) { return false; }
		int i;
		for (i = 0; i < numObjectives; i += 1) {
			ECharString tmpStr;
			if (2 == objectivesVersion) {
				tmpStr.Format("Objective%d", i);
			} else {
				tmpStr.Format("Team%dObjective%d", Alignment(), i);
			}
			result = missionFile->seekBlock(tmpStr.Data());
			if (NO_ERR != result) { assert(false); continue; }
			CObjective *pNewObjective = new CObjective(m_alignment);
			bool bresult = pNewObjective->Read(missionFile, i, objectivesVersion);
			if (true != bresult) { assert(false); delete pNewObjective; continue; }
			Append(pNewObjective);
		}
	}
	return true;
}

bool CObjectives::Save( FitIniFile* file )
{
	ECharString tmpStr;
	tmpStr.Format("Team%dObjectives", Alignment());
	file->writeBlock( tmpStr.Data() );
	file->writeIdULong( "NumObjectives", Count() );
	int i = 0;
	EIterator it = Begin();
	while (!it.IsDone()) {
		ECharString tmpStr;
		tmpStr.Format("Team%dObjective%d", Alignment(), i);
		file->writeBlock( tmpStr.Data() );
		(*it)->Save(file, i);
		i += 1;
		it++;
	}

	return true;
}

bool CObjectives::EditDialog() {
	ObjectivesDlg ObjectivesDialog;

	if (EditorInterface::instance()->ObjectSelectOnlyMode()) {
		ObjectivesDialog.m_ModifiedObjectives = EditorInterface::instance()->objectivesEditState.ModifiedObjectives;
		ObjectivesDialog.nSelectionIndex = EditorInterface::instance()->objectivesEditState.nSelectionIndex;
	} else {
		ObjectivesDialog.m_ModifiedObjectives = (*this);
		ObjectivesDialog.nSelectionIndex = -1;
	}

	int ret = ObjectivesDialog.DoModal();

	if (!EditorInterface::instance()->ObjectSelectOnlyMode()) {
		if (IDOK == ret) {
			(*this) = ObjectivesDialog.m_ModifiedObjectives;
		}
	}
	return (IDOK == ret);
}

bool CObjectives::WriteMissionScript(const char *Name, const char *OutputPath)
{
	return true;
}

void CObjectives::handleObjectInvalidation(const EditorObject *pObj) {
	/* if we cared about performance, we could maintain a mapping */
	if (Count() <= 0) {
		return;
	}
	EIterator it = Begin();
	while (!it.IsDone()) {
		CObjective *pObjective = *it;
		if (pObjective->Count() > 0) {
			/*do success conditions*/
			CObjective::EIterator it2 = pObjective->End();
			while (!it2.IsDone()) {
				if (true == (*it2)->RefersTo(pObj)) {
					CObjective::EIterator it3 = it2;
					it2--;
					bool weAreDeletingTheLastCondition = false;
					if (it2.IsDone()) {
						weAreDeletingTheLastCondition = true;
					}
					delete (*it3);
					pObjective->Delete(it3);
					if (weAreDeletingTheLastCondition) {
						/* after deleting the last element in the EList, it seems that any iterators
						of that EList (eg. it2) are no longer valid, so we have to break out of the
						loop here so that we don't try refer to it2 again (i.e. evaluating it2.IsDone()). */
						break;
					}
				} else {
					it2--;
				}
			}
		}
		if (pObjective->m_failureConditionList.Count() > 0) {
			/*do failure conditions*/
			CObjective::EIterator it2 = pObjective->m_failureConditionList.End();
			while (!it2.IsDone()) {
				if (true == (*it2)->RefersTo(pObj)) {
					CObjective::EIterator it3 = it2;
					it2--;
					bool weAreDeletingTheLastCondition = false;
					if (it2.IsDone()) {
						weAreDeletingTheLastCondition = true;
					}
					delete (*it3);
					pObjective->m_failureConditionList.Delete(it3);
					if (weAreDeletingTheLastCondition) {
						/* after deleting the last element in the EList, it seems that any iterators
						of that EList (eg. it2) are no longer valid, so we have to break out of the
						loop here so that we don't try refer to it2 again (i.e. evaluating it2.IsDone()). */
						break;
					}
				} else {
					it2--;
				}
			}
		}
		it++;
	}
}

bool CObjectives::NoteThePositionsOfObjectsReferenced() {
	bool retval = true;
	EIterator it = Begin();
	while (!it.IsDone()) {
		bool res = (*it)->NoteThePositionsOfObjectsReferenced();
		if (false == res) {
			retval = false;
		}
		it++;
	}
	return retval;
}

bool CObjectives::RestoreObjectPointerReferencesFromNotedPositions() {
	bool retval = true;
	EIterator it = Begin();
	while (!it.IsDone()) {
		bool res = (*it)->RestoreObjectPointerReferencesFromNotedPositions();
		if (false == res) {
			retval = false;
		}
		it++;
	}
	return retval;
}

bool CObjectives::ThereAreObjectivesWithNoConditions() {
	bool retval = false;
	EIterator it = Begin();
	while (!it.IsDone()) {
		bool res = (*it)->ThisObjectiveHasNoConditions();
		if (true == res) {
			retval = true;
			break;
		}
		it++;
	}
	return retval;
}
//*************************************************************************************************
// end of file ( Objective.cpp )
