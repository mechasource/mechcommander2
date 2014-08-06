//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "StdAfx.h"
#include "CampaignData.h"

#include "EString.h"
#include "ECharString.h"

#include "assert.h"

static long sReadIdBoolean(FitIniFile &missionFile, const char *varName, bool &value) {
	long result = 0;
	bool tmpBool;
	result = missionFile.readIdBoolean((char *)varName, tmpBool);
	if (NO_ERR != result) {
		//assert(false);
	} else {
		value = tmpBool;
	}
	return result;
}

static long sReadIdInteger(FitIniFile &missionFile, const char *varName, int &value) {
	long result = 0;
	long tmpLong;
	result = missionFile.readIdLong((char *)varName, tmpLong);
	if (NO_ERR != result) {
		//assert(false);
	} else {
		value = tmpLong;
	}
	return result;
}

static long sReadIdString(FitIniFile &missionFile, const char *varName, CString &CStr) {
	long result = 0;
	char buffer[2001/*buffer size*/]; buffer[0] = '\0';
	result = missionFile.readIdString((char *)varName, buffer, 2001/*buffer size*/ - 1);
	if (NO_ERR != result) {
		//assert(false);
	} else {
		CStr = buffer;
	}
	return result;
}


CMissionData::CMissionData() {
	m_PurchaseFile = "purchase_All";
	m_LogisticsEnabled = true;
	m_IsMandatory = true;
	m_PilotPromotionEnabled = true;
	m_PurchasingEnabled = false;
	m_SalvageEnabled = true;
	m_MissionSelectionEnabled = true;
}

bool CMissionData::operator==(const CMissionData &rhs) const {
	if ((m_MissionFile == rhs.m_MissionFile)
		&& (m_PurchaseFile == rhs.m_PurchaseFile)
		&& (m_LogisticsEnabled == rhs.m_LogisticsEnabled)
		&& (m_IsMandatory == rhs.m_IsMandatory)
		&& (m_PilotPromotionEnabled == rhs.m_PilotPromotionEnabled)
		&& (m_PurchasingEnabled == rhs.m_PurchasingEnabled)
		&& (m_MissionSelectionEnabled == rhs.m_MissionSelectionEnabled)
		&& (m_SalvageEnabled == rhs.m_SalvageEnabled)) {
		return true;
	} else {
		return false;
	}
}

bool CMissionData::Save(FitIniFile &fitFile) {
	fitFile.writeIdString( "FileName", m_MissionFile.GetBuffer(0));
	fitFile.writeIdBoolean("Mandatory", m_IsMandatory);
	fitFile.writeIdString( "PurchaseFile", m_PurchaseFile.GetBuffer(0));
	fitFile.writeIdBoolean("PlayLogistics", m_LogisticsEnabled);
	fitFile.writeIdBoolean("PlaySalvage", m_SalvageEnabled);
	fitFile.writeIdBoolean("PlayPilotPromotion", m_PilotPromotionEnabled);
	fitFile.writeIdBoolean("PlayPurchasing", m_PurchasingEnabled);
	fitFile.writeIdBoolean( "PlaySelection", m_MissionSelectionEnabled );

	return true;
}

bool CMissionData::Read(FitIniFile &fitFile) {
	int result;
	result = sReadIdString(fitFile, "FileName", m_MissionFile);
	result = sReadIdBoolean(fitFile, "Mandatory", m_IsMandatory);
	result = sReadIdString(fitFile, "PurchaseFile", m_PurchaseFile);
	result = sReadIdBoolean(fitFile, "PlayLogistics", m_LogisticsEnabled);
	result = sReadIdBoolean(fitFile, "PlaySalvage", m_SalvageEnabled);
	result = sReadIdBoolean(fitFile, "PlayPilotPromotion", m_PilotPromotionEnabled);
	result = sReadIdBoolean(fitFile, "PlayPurchasing", m_PurchasingEnabled);
	result = sReadIdBoolean( fitFile, "PlaySelection", m_MissionSelectionEnabled );

	return true;
}

CGroupData::CGroupData() {
	m_OperationFile = "mcl_cm_op_sample";
	m_NumMissionsToComplete = 1;
	m_TuneNumber = 0;
}

bool CGroupData::operator==(const CGroupData &rhs) const {
	if ((m_MissionList == rhs.m_MissionList)
		&& (m_OperationFile == rhs.m_OperationFile)
		&& (m_PreVideoFile == rhs.m_PreVideoFile)
		&& (m_VideoFile == rhs.m_VideoFile)
		&& (m_Label == rhs.m_Label)
		&& (m_NumMissionsToComplete == rhs.m_NumMissionsToComplete)
		&& (m_TuneNumber == rhs.m_TuneNumber)
		&& (m_ABLScript == rhs.m_ABLScript)) {
		return true;
	} else {
		return false;
	}
}

bool CGroupData::Save(FitIniFile &fitFile, const char *groupName) {
	fitFile.writeIdString( "Label", m_Label.GetBuffer(0));
	fitFile.writeIdLong("NumberToComplete", m_NumMissionsToComplete);
	fitFile.writeIdString( "OperationFile", m_OperationFile.GetBuffer(0));
	fitFile.writeIdString( "Video", m_VideoFile.GetBuffer(0));
	fitFile.writeIdString( "PreVideo", m_PreVideoFile.GetBuffer(0));
	fitFile.writeIdLong("Tune", m_TuneNumber);
	fitFile.writeIdLong("MissionCount", m_MissionList.Count());
	if (!m_ABLScript.IsEmpty()) {
		fitFile.writeIdString( "ABLScript", m_ABLScript.GetBuffer(0));
	}

	CMissionList::EIterator it;
	int index;
	for (it = m_MissionList.Begin(), index = 0; !it.IsDone(); it++, index+=1) {
		ECharString blockName;
		blockName.Format("%sMission%d", groupName, index);
		fitFile.writeBlock(blockName.Data());
		(*it).Save(fitFile);
	}
	return true;
}

bool CGroupData::Read(FitIniFile &fitFile, const char *groupName) {
	int result;
	result = sReadIdString(fitFile, "Label", m_Label);
	result = sReadIdInteger(fitFile, "NumberToComplete", m_NumMissionsToComplete);
	result = sReadIdString(fitFile, "OperationFile", m_OperationFile);
	result = sReadIdString(fitFile, "Video", m_VideoFile);
	result = sReadIdString(fitFile, "PreVideo", m_PreVideoFile);
	result = sReadIdInteger(fitFile, "Tune", m_TuneNumber);
	result = sReadIdString(fitFile, "ABLScript", m_ABLScript);
	int missionCount = 0;
	result = sReadIdInteger(fitFile, "MissionCount", missionCount);
	if (NO_ERR != result) { return false; }

	int index;
	for (index = 0; missionCount > index; index+=1) {
		ECharString blockName;
		blockName.Format("%sMission%d", groupName, index);
		result = fitFile.seekBlock(blockName.Data());
		if (NO_ERR != result) { assert(false); continue; }
		CMissionData missionData;
		bool bresult = missionData.Read(fitFile);
		if (true != bresult) { assert(false); continue; }
		m_MissionList.Append(missionData);
	}

	return true;
}

CCampaignData::CCampaignData() {
	m_NameUseResourceString = false;
	m_NameResourceStringID = 0;
	m_CBills = 0;
}

bool CCampaignData::operator==(const CCampaignData &rhs) const {
	if ((m_GroupList == rhs.m_GroupList)
		&& (m_Name == rhs.m_Name)
		&& (m_NameUseResourceString == rhs.m_NameUseResourceString)
		&& (m_NameResourceStringID == rhs.m_NameResourceStringID)
		&& (m_CBills == rhs.m_CBills)
		&& (m_FinalVideo == rhs.m_FinalVideo)) {
		return true;
	} else {
		return false;
	}
}

bool CCampaignData::Save(CString pathName) {
	FitIniFile fitFile;
	int result = fitFile.create(pathName.GetBuffer(0));
	if (result != NO_ERR)
	{
		return false;
	}

	fitFile.writeBlock("Campaign");
	fitFile.writeIdBoolean("NameUseResourceString", m_NameUseResourceString);
	if (!m_NameUseResourceString) {
		fitFile.writeIdString("CampaignName", m_Name.GetBuffer(0));
	} else {
		fitFile.writeIdLong("NameID", m_NameResourceStringID);
	}
	fitFile.writeIdLong("CBills", m_CBills);
	fitFile.writeIdString( "FinalVideo", m_FinalVideo.GetBuffer(0));
	fitFile.writeIdLong("GroupCount", m_GroupList.Count());

	CGroupList::EIterator it;
	int index;
	for (it = m_GroupList.Begin(), index = 0; !it.IsDone(); it++, index+=1) {
		ECharString blockName;
		blockName.Format("Group%d", index);
		fitFile.writeBlock(blockName.Data());
		(*it).Save(fitFile, blockName.Data());
	}

	fitFile.close();
	return true;
}

bool CCampaignData::Read(CString pathName) {
	FitIniFile fitFile;
	int result = fitFile.open(pathName.GetBuffer(0));
	if (NO_ERR != result) { assert(false); return false; }

	result = fitFile.seekBlock("Campaign");
	if (NO_ERR != result) { assert(false); }
	result = sReadIdString(fitFile, "CampaignName", m_Name);
	if (NO_ERR == result) { m_NameUseResourceString = false; }
	result = sReadIdInteger(fitFile, "NameID", m_NameResourceStringID);
	if (NO_ERR == result) { m_NameUseResourceString = true; }
	result = sReadIdBoolean(fitFile, "NameUseResourceString", m_NameUseResourceString);
	result = sReadIdInteger(fitFile, "CBills", m_CBills);
	result = sReadIdString(fitFile, "FinalVideo", m_FinalVideo);
	int groupCount = 0;
	result = sReadIdInteger(fitFile, "GroupCount", groupCount);
	if (NO_ERR != result) { return false; }

	int index;
	for (index = 0; groupCount > index; index+=1) {
		ECharString blockName;
		blockName.Format("Group%d", index);
		result = fitFile.seekBlock(blockName.Data());
		if (NO_ERR != result) { assert(false); continue; }
		CGroupData groupData;
		bool bresult = groupData.Read(fitFile, blockName.Data());
		if (true != bresult) { assert(false); continue; }
		m_GroupList.Append(groupData);
	}

	fitFile.close();
	return true;
}
