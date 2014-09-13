//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
/******************************************************************************/

#pragma once

#ifndef CAMPAIGNDATA_H
#define CAMPAIGNDATA_H

//#include <elist.h>
//#include "inifile.h"

class CMissionData
{
public:
	CMissionData(void);
	bool operator==(const CMissionData& rhs) const;
	bool operator!=(const CMissionData& rhs) const
	{
		return (!((*this) == rhs));
	}
	bool Save(FitIniFile& fitFile);
	bool Read(FitIniFile& fitFile);

	CString	m_MissionFile;
	CString	m_PurchaseFile;
	bool	m_LogisticsEnabled;
	bool	m_IsMandatory;
	bool	m_PilotPromotionEnabled;
	bool	m_PurchasingEnabled;
	bool	m_MissionSelectionEnabled;
	bool	m_SalvageEnabled;
};

class CMissionList : public EList<CMissionData, CMissionData> {};

class CGroupData
{
public:
	CGroupData(void);
	bool operator==(const CGroupData& rhs) const;
	bool operator!=(const CGroupData& rhs) const
	{
		return (!((*this) == rhs));
	}
	bool Save(FitIniFile& fitFile, PCSTR groupName);
	bool Read(FitIniFile& fitFile, PCSTR groupName);

	CMissionList	m_MissionList;
	CString	m_OperationFile;
	CString	m_PreVideoFile;
	CString	m_VideoFile;
	CString	m_Label;
	int32_t		m_NumMissionsToComplete;
	int32_t		m_TuneNumber;
	CString	m_ABLScript;
};

class CGroupList : public EList<CGroupData, CGroupData> {};

class CCampaignData
{
public:
	CCampaignData(void);
	bool operator==(const CCampaignData& rhs) const;
	bool operator!=(const CCampaignData& rhs) const
	{
		return (!((*this) == rhs));
	}
	bool Save(CString pathName);
	bool Read(CString pathName);
	void Clear()
	{
		CCampaignData a;
		*this = a;
	}

	CString m_PathName;
	CGroupList	m_GroupList;
	CString	m_Name;
	bool m_NameUseResourceString;
	int32_t m_NameResourceStringID;
	int32_t		m_CBills;
	CString	m_FinalVideo;
};

#endif  /*CAMPAIGNDATA_H*/
