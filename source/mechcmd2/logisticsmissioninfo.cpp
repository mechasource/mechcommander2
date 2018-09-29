#define LOGISTICSMISSIONINFO_CPP
/*************************************************************************************************\
LogisticsMissionInfo.cpp			: Implementation of the LogisticsMissionInfo
component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "stdinc.h"
#include <mclib.h>
#include "logisticsmissioninfo.h"

#ifndef FITINIFILE_H
#include "inifile.h"
#endif

#include "logisticserrors.h"
#include "multplyr.h"

extern char missionName[1024];

LogisticsMissionInfo::LogisticsMissionInfo()
{
	currentMission	 = -1;
	currentStage	   = 0;
	groups			   = 0;
	currentMissionName = missionName;
	groupCount		   = 0;
	CBills			   = 0;
	if (currentMissionName.Length() < 2)
		currentMissionName = "m0101";
	bMultiplayer = 0;
	//	maxTeams = maxPlayers = 2;
}

LogisticsMissionInfo::~LogisticsMissionInfo() { clear(); }

void LogisticsMissionInfo::clear()
{
	if (groups)
	{
		for (size_t i = 0; i < groupCount; i++)
		{
			for (MISSION_LIST::EIterator iter = groups[i].infos.Begin(); !iter.IsDone(); iter++)
			{
				delete (*iter);
			}
		}
		delete[] groups;
	}
	for (FILE_LIST::EIterator iter = additionalPurchaseFiles.Begin(); !iter.IsDone(); iter++)
	{
		delete *iter;
	}
	additionalPurchaseFiles.Clear();
	currentMission	 = -1;
	currentStage	   = 0;
	groups			   = 0;
	currentMissionName = missionName;
	groupCount		   = 0;
	CBills			   = 0;
}

int32_t LogisticsMissionInfo::init(FitIniFile& file)
{
	clear();
	bMultiplayer = 0;
	campaignName = file.getFilename();
	// read the number of mission groups
	file.seekBlock("Campaign");
	char resultName[256];
	resultName[0]  = 0;
	int32_t lName  = 0;
	int32_t result = file.readIdLong("NameID", lName);
	if (result == NO_ERROR)
	{
		cLoadString(lName, resultName, 255);
	}
	else
	{
		result = file.readIdString("CampaignName", resultName, 255);
		Assert(result == NO_ERROR, 0, "couldn't find the missionName");
	}
	campaignDisplayName = resultName;
	result				= file.readIdLong("GroupCount", groupCount);
	if (result != NO_ERROR)
	{
		Assert(0, 0, "No group count in campaign file");
		return -1;
	}
	result = file.readIdLong("CBills", CBills);
	char tmp[256];
	if (NO_ERROR == file.readIdString("FinalVideo", tmp, 255))
	{
		finalVideoName = tmp;
	}
	// create storage for 'em
	groups		 = new MissionGroup[groupCount];
	currentStage = 0;
	char blockName[32];
	for (size_t i = 0; i < groupCount; i++)
	{
		groups[i].numberToBeCompleted = -1; // initialize
		sprintf(blockName, "Group%ld", i);
		if (NO_ERROR != file.seekBlock(blockName))
		{
			Assert(0, i, "couldn't find this group in the campaign file");
			continue;
		}
		file.readIdLong("NumberToComplete", groups[i].numberToBeCompleted);
		file.readIdLong("Tune", groups[i].logisticsTuneId);
		if (NO_ERROR != file.readIdString("ABLScript", tmp, 255))
		{
			// Perfectly valid to have no ABL brain for this group!
			groups[i].ablBrainName = "NONE";
		}
		else
		{
			groups[i].ablBrainName = tmp;
		}
		if (NO_ERROR != file.readIdString("Video", tmp, 255))
		{
			char errorStr[256];
			sprintf(errorStr, "couldn't find the video for operation %ld", i);
			Assert(0, 0, errorStr);
		}
		groups[i].videoFileName = tmp;
		groups[i].videoShown	= 0;
		if (NO_ERROR == file.readIdString("PreVideo", tmp, 255))
		{
			groups[i].bigVideoName  = tmp;
			groups[i].bigVideoShown = 0;
		}
		if (NO_ERROR != file.readIdString("OperationFile", tmp, 255) || !strlen(tmp))
		{
			strcpy(tmp, "mcl_cm_opplayer.fit");
		}
		groups[i].operationFileName = tmp;
		int32_t missionCount;
		// read the number of missions in the group
		file.readIdLong("MissionCount", missionCount);
		for (size_t j = 0; j < missionCount; j++)
		{
			MissionInfo* pInfo = new MissionInfo;
			sprintf(blockName, "Group%ldMission%ld", i, j);
			if (NO_ERROR == file.seekBlock(blockName))
			{
				char fileName[1024];
				if (NO_ERROR != file.readIdString("FileName", fileName, 1023))
				{
					Assert(0, j,
						"couldn't find a mission file name in the campaign "
						"file");
					continue;
				}
				groups[i].infos.Append(pInfo);
				std::wstring path = missionPath;
				path += fileName;
				path += ".fit";
				pInfo->fileName  = fileName;
				pInfo->completed = 0;
				FitIniFile missionFile;
				if (NO_ERROR != missionFile.open((PSTR)(PCSTR)path))
				{
					char errorStr[256];
					sprintf(errorStr, "couldn't open file %s", fileName);
					Assert(0, 0, errorStr);
					continue;
				}
				readMissionInfo(missionFile, pInfo);
				char videoFileName[255];
				if (NO_ERROR == file.readIdString("VideoOverride", videoFileName, 255))
					pInfo->videoName = videoFileName;
				file.readIdBoolean("Mandatory", pInfo->mandatory);
				file.readIdBoolean("CompletePrevious", pInfo->completePrevious);
				int32_t result = file.readIdString("PurchaseFile", fileName, 1023);
				if ((NO_ERROR != result) || (0 == strcmp("", fileName)))
				{
					strcpy(fileName, "purchase_All");
				}
				file.readIdBoolean("Hidden", pInfo->hidden);
				if (NO_ERROR != file.readIdBoolean("PlayLogistics", pInfo->playLogistics))
					pInfo->playLogistics = true;
				if (NO_ERROR != file.readIdBoolean("PlaySalvage", pInfo->playSalvage))
					pInfo->playSalvage = true;
				if (NO_ERROR != file.readIdBoolean("PlayPilotPromotion", pInfo->playPilotPromotion))
					pInfo->playPilotPromotion = true;
				if (NO_ERROR != file.readIdBoolean("PlayPurchasing", pInfo->playPurchasing))
					pInfo->playPurchasing = true;
				if (NO_ERROR != file.readIdBoolean("PlaySelection", pInfo->playMissionSelection))
				{
					pInfo->playMissionSelection = false;
				}
				pInfo->purchaseFileName = missionPath;
				pInfo->purchaseFileName += fileName;
				pInfo->purchaseFileName += ".fit";
			}
		}
	}
	return 0;
}

void LogisticsMissionInfo::readMissionInfo(
	FitIniFile& file, LogisticsMissionInfo::MissionInfo* pInfo)
{
	int32_t result = file.seekBlock("MissionSettings");
	Assert(result == NO_ERROR, 0, "Coudln't find the mission settings block in the mission file");
	char missionName[256];
	missionName[0] = 0;
	bool bRes	  = 0;
	result		   = file.readIdBoolean("MissionNameUseResourceString", bRes);
	// Assert( result == NO_ERROR, 0, "couldn't find the
	// MissionNameUseResourceString" );
	if (bRes)
	{
		uint32_t lRes;
		result = file.readIdULong("MissionNameResourceStringID", lRes);
		Assert(result == NO_ERROR, 0, "couldn't find the MissionNameResourceStringID");
		cLoadString(lRes, missionName, 255);
	}
	else
	{
		result = file.readIdString("MissionName", missionName, 255);
		Assert(result == NO_ERROR, 0, "couldn't find the missionName");
	}
	pInfo->missionDescriptiveName = missionName;
	char blurb[4096];
	result = file.readIdString("Blurb2", blurb, 4095);
	Assert(result == NO_ERROR, 0, "couldn't find the mission blurb");
	bool tmpBool = false;
	result		 = file.readIdBoolean("Blurb2UseResourceString", tmpBool);
	if (NO_ERROR == result && tmpBool)
	{
		uint32_t tmpInt = 0;
		result			= file.readIdULong("Blurb2ResourceStringID", tmpInt);
		if (NO_ERROR == result)
		{
			cLoadString(tmpInt, blurb, 2047);
		}
	}
	pInfo->description = blurb;
	result			   = file.readIdLong("ResourcePoints", pInfo->resourcePoints);
	result			   = file.readIdLong("AdditionalCBills", pInfo->additionalCBills);
	float fTmp;
	result = file.readIdFloat("DropWeightLimit", fTmp);
	if (result != NO_ERROR)
		pInfo->dropWeight = 300;
	else
		pInfo->dropWeight = fTmp;
	if (NO_ERROR != file.readIdBoolean("AirStrikesEnabledDefault", pInfo->enableAirStrike))
		pInfo->enableAirStrike = 1;
	if (NO_ERROR != file.readIdBoolean("MineLayersEnabledDefault", pInfo->enableMineLayer))
		pInfo->enableMineLayer = 1;
	if (NO_ERROR != file.readIdBoolean("ScoutCoptersEnabledDefault", pInfo->enableScoutCopter))
		pInfo->enableScoutCopter = 1;
	if (NO_ERROR != file.readIdBoolean("SalvageCraftEnabledDefault", pInfo->enableSalavageCraft))
		pInfo->enableSalavageCraft = 1;
	if (NO_ERROR != file.readIdBoolean("SensorProbesEnabledDefault", pInfo->enableSensorStrike))
		pInfo->enableSensorStrike = true;
	if (NO_ERROR != file.readIdBoolean("RepairVehicleEnabledDefault", pInfo->enableRepairTruck))
		pInfo->enableRepairTruck = true;
	if (NO_ERROR != file.readIdBoolean("ArtilleryPieceEnabledDefault", pInfo->enableArtilleryPiece))
		pInfo->enableArtilleryPiece = 0;
}

int32_t LogisticsMissionInfo::load(FitIniFile& file)
{
	// read the campaign file name
	char path[1024];
	char fileName[64];
	int32_t result = file.seekBlock("General");
	Assert(result == NO_ERROR, 0, "couldn't find general block in campaign file");
	result = file.readIdString("CampaignFile", fileName, 63);
	FitIniFile campaignFile;
	if (NO_ERROR != campaignFile.open(fileName))
	{
		char errorStr[256];
		sprintf(errorStr, "couldn't find file %s", path);
		return -1;
	}
	campaignName = fileName;
	// initialize this with campaign info
	if (NO_ERROR != init(campaignFile))
	{
		Assert(0, 0, "couldn't initialize the campaign");
		return -1;
	}
	currentMission = 0;
	// reset rp
	file.readIdLong("CBills", CBills);
	// set the current mission group
	file.readIdLong("Group", currentStage);
	if (currentStage >= groupCount)
		currentStage = groupCount - 1;
	// set completed missions
	int32_t count;
	file.readIdLong("CompletedMissions", count);
	char header[32];
	MissionGroup* pGroup = &groups[currentStage];
	file.readIdBoolean("BigMoviePlayed", pGroup->bigVideoShown);
	int32_t numberCompleted = 0;
	int32_t i;
	for (i = 0; i < count; i++)
	{
		int32_t cnt = 0;
		sprintf(header, "Mission%ld", i);
		file.readIdLong(header, cnt);
		MissionInfo* pInfo = pGroup->infos[(uint32_t)cnt];
		pInfo->completed   = 1;
		numberCompleted++;
	}
	if (numberCompleted >= pGroup->numberToBeCompleted) // unhide necessary missions
	{
		MISSION_LIST::EIterator iter = pGroup->infos.Begin();
		while (!iter.IsDone())
		{
			if ((*iter)->hidden) // use the hidden video file name for this
			{
				if ((*iter)->videoName.Length())
				{
					pGroup->videoFileName = (*iter)->videoName;
					pGroup->videoShown	= 0;
				}
			}
			(*iter)->hidden = 0;
			iter++;
		}
	}
	if (count < pGroup->infos.Count())
		setNextMission(pGroup->infos[count]->fileName);
	char tmpPlayerName[64];
	file.readIdString("PlayerName", tmpPlayerName, 63);
	playerName = tmpPlayerName;
	if (NO_ERROR == file.seekBlock("AdditionalPurchaseFiles"))
	{
		i = 0;
		while (true)
		{
			sprintf(header, "File%ld", i);
			char tmp[256];
			if (NO_ERROR == file.readIdString(header, tmp, 255))
			{
				PSTR pfName = new char[strlen(tmp) + 1];
				strcpy(pfName, tmp);
				additionalPurchaseFiles.Append(pfName);
				i++;
			}
			else
				break;
		}
	}
	return 0;
}
void LogisticsMissionInfo::save(FitIniFile& file)
{
	// save the campaign file name
	file.writeIdString("CampaignFile", campaignName);
	file.writeIdString("CampaignName", campaignDisplayName);
	// save the player name
	if (playerName.Length())
		file.writeIdString("PlayerName", playerName);
	else
		file.writeIdString("PlayerName", "");
	// save the rp
	file.writeIdLong("CBills", CBills);
	// save the current mission group
	file.writeIdLong("Group", currentStage);
	int32_t count = 0;
	char header[32];
	if (currentStage < groupCount)
	{
		// save the missions completed thus far in the mission group
		MissionGroup* pGroup = &groups[currentStage];
		file.writeIdBoolean("BigMoviePlayed", pGroup->bigVideoShown);
		// write out the name of the mission, for ease of filling dialog
		MissionInfo* pInfo = pGroup->infos[currentMission];
		file.writeIdString("MissionName", pInfo->missionDescriptiveName);
		file.writeIdString("MissionFileName", pInfo->fileName);
		int32_t curMission = 0;
		for (MISSION_LIST::EIterator iter = pGroup->infos.Begin(); !iter.IsDone();
			 iter++, curMission++)
		{
			if ((*iter)->completed)
			{
				sprintf(header, "Mission%ld", count);
				file.writeIdLong(header, curMission);
				count++;
			}
		}
	}
	file.writeIdLong("CompletedMissions", count);
	file.writeBlock("AdditionalPurchaseFiles");
	int32_t i = 0;
	for (FILE_LIST::EIterator fIter = additionalPurchaseFiles.Begin(); !fIter.IsDone(); fIter++)
	{
		sprintf(header, "File%ld", i);
		file.writeIdString(header, (*fIter));
		i++;
	}
}

int32_t LogisticsMissionInfo::getAvailableMissions(PCSTR* missions, int32_t& numberOfEm)
{
	MissionGroup* pGroup		 = &groups[currentStage];
	int32_t count				 = 0;
	int32_t maxOut				 = numberOfEm;
	MISSION_LIST::EIterator iter = pGroup->infos.Begin();
	while (!iter.IsDone())
	{
		if (!(*iter)->completed)
		{
			if (count < maxOut)
			{
				missions[count] = (*iter)->fileName;
			}
			count++;
		}
		iter++;
	}
	numberOfEm = count;
	if (count > maxOut)
	{
		return NEED_BIGGER_ARRAY;
	}
	return 0;
}

int32_t LogisticsMissionInfo::getCurrentMissions(PCSTR* missions, int32_t& numberOfEm)
{
	MissionGroup* pGroup		 = &groups[currentStage];
	int32_t count				 = 0;
	int32_t maxOut				 = numberOfEm;
	MISSION_LIST::EIterator iter = pGroup->infos.Begin();
	while (!iter.IsDone())
	{
		if (!(*iter)->hidden)
		{
			if (count < maxOut)
			{
				missions[count] = (*iter)->fileName;
			}
			count++;
		}
		iter++;
	}
	numberOfEm = count;
	if (count > maxOut)
	{
		return NEED_BIGGER_ARRAY;
	}
	return 0;
}
int32_t LogisticsMissionInfo::setNextMission(PCSTR missionName)
{
	if (!missionName || !strlen(missionName))
		return -1;
	if (bMultiplayer)
	{
		std::wstring path = missionPath;
		path += missionName;
		path += ".fit";
		currentMission		  = 0;
		currentStage		  = 0;
		MissionInfo* pInfo	= groups[0].infos[0];
		pInfo->fileName		  = missionName;
		pInfo->completed	  = 0;
		pInfo->playPurchasing = true;
		currentMissionName	= missionName;
		FitIniFile missionFile;
		if (NO_ERROR != missionFile.open((PSTR)(PCSTR)path))
		{
			char errorStr[256];
			sprintf(errorStr, "couldn't open file %s", missionName);
			Assert(0, 0, errorStr);
			return -1;
		}
		readMissionInfo(missionFile, pInfo);
		//			result = missionFile.readIdLong( "MaximumNumberOfTeams",
		// maxTeams ); 			result = missionFile.readIdLong(
		//"MaximumNumberOfPlayers", maxPlayers );
		MC2Player* pPlayerInfo = MPlayer->getPlayerInfo(MPlayer->commanderID);
		;
		if (pPlayerInfo)
		{
			pInfo->resourcePoints   = MPlayer->missionSettings.resourcePoints;
			pInfo->additionalCBills = pPlayerInfo->cBills;
			// need to put dropweight in here as sooon as we have it
		}
		pInfo->dropWeight = MPlayer->missionSettings.dropWeight;
		missionFile.close();
		CBills						= pInfo->additionalCBills;
		pInfo->enableAirStrike		= MPlayer->missionSettings.airStrike;
		pInfo->enableArtilleryPiece = MPlayer->missionSettings.guardTower;
		pInfo->enableRepairTruck	= MPlayer->missionSettings.repairVehicle;
		pInfo->enableSensorStrike   = MPlayer->missionSettings.sensorProbe;
		pInfo->enableScoutCopter	= MPlayer->missionSettings.scoutCopter;
		pInfo->enableMineLayer		= MPlayer->missionSettings.mineLayer;
		pInfo->enableSalavageCraft  = MPlayer->missionSettings.recoveryTeam;
	}
	else
	{
		MissionGroup* pGroup		 = &groups[currentStage];
		int32_t count				 = 0;
		bool bFound					 = 0;
		MISSION_LIST::EIterator iter = pGroup->infos.Begin();
		while (!iter.IsDone())
		{
			if (_stricmp((*iter)->fileName, missionName) == 0)
			{
				bFound = 1;
				if ((*iter)->completed)
				{
					return MISSION_ALREADY_DONE;
				}
				else
				{
					if (currentMission != -1)
					{
						MissionInfo* pInfo = pGroup->infos[currentMission];
						if (!(pInfo)->completed)
							CBills -= pInfo->additionalCBills;
					}
					currentMission	 = count;
					currentMissionName = (*iter)->fileName;
					currentMissionName.Remove(std::wstring(".fit"));
					CBills += (*iter)->additionalCBills;
				}
				break;
			}
			count++;
			iter++;
		}
		if (!bFound)
			return INVALID_MISSION;
	}
	return 0;
}

void LogisticsMissionInfo::setSingleMission(PCSTR missionFileName)
{
	clear();
	bMultiplayer	   = true;
	groups			   = new MissionGroup[1];
	currentStage	   = 0;
	MissionInfo* pInfo = new MissionInfo;
	groups[0].infos.Append(pInfo);
	groups[0].operationFileName   = "MCL_CM_Op1_1.fit";
	groups[0].ablBrainName		  = "";
	groups[0].bigVideoName		  = "";
	groups[0].bigVideoShown		  = true;
	groups[0].logisticsTuneId	 = 0;
	groups[0].numberToBeCompleted = 1;
	groups[0].videoFileName		  = "";
	groupCount					  = 1;
	std::wstring path			  = missionPath;
	path += missionFileName;
	path += ".fit";
	currentMission				= 0;
	currentStage				= 0;
	pInfo->fileName				= missionName;
	pInfo->completed			= 0;
	pInfo->playMissionSelection = 0;
	pInfo->playPurchasing		= true;
	pInfo->playSalvage			= 0;
	pInfo->playPilotPromotion   = 0;
	currentMissionName			= missionFileName;
	FitIniFile missionFile;
	if (NO_ERROR != missionFile.open((PSTR)(PCSTR)path))
	{
		char errorStr[256];
		sprintf(errorStr, "couldn't open file %s", missionName);
		Assert(0, 0, errorStr);
		return;
	}
	readMissionInfo(missionFile, pInfo);
	float fTmp;
	int32_t result	= missionFile.readIdFloat("DropWeightLimit", fTmp);
	pInfo->dropWeight = fTmp;
	result			  = missionFile.readIdLong("AdditionalCBills", pInfo->additionalCBills);
	if (result != NO_ERROR || !pInfo->additionalCBills)
		pInfo->additionalCBills = 1200 * fTmp + 100000;
	CBills = pInfo->additionalCBills;
}

int32_t LogisticsMissionInfo::getCurrentMissionId()
{
	int32_t missionId = 0;
	for (size_t i = 0; i <= lastStage; i++)
	{
		MissionGroup* pGroup = &groups[i];
		if (i == lastStage)
		{
			missionId += lastMission;
		}
		else
		{
			int32_t count				 = 0;
			MISSION_LIST::EIterator iter = pGroup->infos.Begin();
			while (!iter.IsDone())
			{
				count++;
				iter++;
			}
			missionId += count;
		}
	}
	return missionId;
}

void LogisticsMissionInfo::setMissionComplete()
{
	if (currentStage >= groupCount)
	{
		return;
	}
	// Needed to tell pilots which mission they just completed.
	lastStage			 = currentStage;
	lastMission			 = currentMission;
	lastMissionName		 = currentMissionName;
	MissionGroup* pGroup = &groups[currentStage];
	if (pGroup && currentMission > -1)
	{
		CBills += pGroup->infos[currentMission]->additionalCBills;
	}
	int32_t count				 = 0;
	int32_t numberCompleted		 = 0;
	bool bAllMandatoryCompleted  = 1;
	int32_t nextAvailableMission = -1;
	// figure out whether to go to the next stage orn not
	MISSION_LIST::EIterator iter = pGroup->infos.Begin();
	while (!iter.IsDone())
	{
		if (currentMission == count)
		{
			(*iter)->completed = 1;
		}
		if ((*iter)->completed)
		{
			numberCompleted++;
		}
		else
		{
			if (nextAvailableMission == -1)
				nextAvailableMission = count;
			if ((*iter)->mandatory)
				bAllMandatoryCompleted = 0;
		}
		count++;
		iter++;
	}
	if (numberCompleted >= pGroup->numberToBeCompleted)
	{
		if (bAllMandatoryCompleted)
		{
			currentStage++;
			currentMission = 0;
		}
		else
		{
			MISSION_LIST::EIterator iter = pGroup->infos.Begin();
			while (!iter.IsDone())
			{
				if ((*iter)->hidden) // use the hidden video file name for this
				{
					if ((*iter)->videoName.Length())
					{
						pGroup->videoFileName = (*iter)->videoName;
						pGroup->videoShown	= 0;
					}
				}
				(*iter)->hidden = 0;
				iter++;
			}
			currentMission = nextAvailableMission;
		}
	}
	else
	{
		gosASSERT(nextAvailableMission != -1);
		currentMission = nextAvailableMission;
	}
	if (currentStage < groupCount)
	{
		currentMissionName = groups[currentStage].infos[currentMission]->fileName;
	}
}

LogisticsMissionInfo::MissionInfo::~MissionInfo() {}

const std::wstring& LogisticsMissionInfo::getCurrentPurchaseFile(void) const
{
	MissionGroup* pGroup = &groups[currentStage];
	return pGroup->infos[currentMission]->purchaseFileName;
}

int32_t LogisticsMissionInfo::getCurrentDropWeight(void) const
{
	MissionGroup* pGroup = &groups[currentStage];
	if (currentMission != -1)
		return pGroup->infos[currentMission]->dropWeight;
	else
		return 10000;
}

PCSTR LogisticsMissionInfo::getCurrentOperationFile(void) const
{
	MissionGroup* pGroup = &groups[currentStage];
	if (pGroup)
		return pGroup->operationFileName;
	return nullptr;
}

PCSTR LogisticsMissionInfo::getCurrentVideo(void) const
{
	MissionGroup* pGroup = &groups[currentStage];
	if (pGroup)
	{
		MissionInfo* pInfo = pGroup->infos[currentMission];
		if (pInfo)
		{
			if (pInfo->videoName.Length())
				return pInfo->videoName;
		}
		return pGroup->videoFileName;
	}
	return nullptr;
}

int32_t LogisticsMissionInfo::getCurrentLogisticsTuneId()
{
	MissionGroup* pGroup = &groups[currentStage];
	if (pGroup)
		return pGroup->logisticsTuneId;
	return -1;
}

PCSTR LogisticsMissionInfo::getCurrentMissionDescription(void) const
{
	MissionGroup* pGroup = &groups[currentStage];
	if (pGroup)
		return pGroup->infos[currentMission]->description;
	return nullptr;
}

bool LogisticsMissionInfo::getMissionAvailable(PCSTR missionName)
{
	MissionGroup* pGroup = &groups[currentStage];
	for (size_t i = 0; i < pGroup->infos.Count(); i++)
	{
		if (pGroup->infos[i]->fileName.Compare(missionName) == 0)
		{
			return !pGroup->infos[i]->completed;
		}
	}
	return nullptr;
}

PCSTR LogisticsMissionInfo::getCurrentMissionFriendlyName(void) const
{
	if (currentStage >= groupCount)
		return nullptr;
	MissionGroup* pGroup = &groups[currentStage];
	if (pGroup)
		return pGroup->infos[currentMission]->missionDescriptiveName;
	return nullptr;
}

PCSTR LogisticsMissionInfo::getCurrentABLScriptName(void) const
{
	if (currentStage >= groupCount)
		return nullptr;
	MissionGroup* pGroup = &groups[currentStage];
	if (pGroup)
		return pGroup->ablBrainName;
	return nullptr;
}

PCSTR LogisticsMissionInfo::getMissionFriendlyName(PCSTR missionName) const
{
	if (currentStage >= groupCount)
		return nullptr;
	MissionGroup* pGroup = &groups[currentStage];
	for (size_t i = 0; i < pGroup->infos.Count(); i++)
	{
		if (pGroup->infos[i]->fileName.Compare(missionName) == 0)
		{
			return pGroup->infos[i]->missionDescriptiveName;
		}
	}
	return nullptr;
}

int32_t LogisticsMissionInfo::getCurrentRP(void) const
{
	if (currentMission == -1)
		return 0;
	MissionGroup* pGroup = &groups[currentStage];
	return pGroup->infos[currentMission]->resourcePoints;
}

PCSTR LogisticsMissionInfo::getCurrentBigVideo(void) const
{
	if (currentStage == -1)
		return nullptr;
	MissionGroup* pGroup = &groups[currentStage];
	if (pGroup->bigVideoShown)
		return nullptr;
	pGroup->bigVideoShown = true;
	return pGroup->bigVideoName;
}
PCSTR LogisticsMissionInfo::getFinalVideo(void) const { return finalVideoName; }

bool LogisticsMissionInfo::campaignOver(void) const { return currentStage == groupCount; }

void LogisticsMissionInfo::setMultiplayer()
{
	clear();
	bMultiplayer	   = true;
	groups			   = new MissionGroup[1];
	currentStage	   = 0;
	MissionInfo* pInfo = new MissionInfo;
	groups[0].infos.Append(pInfo);
	currentMission = 0;
	groupCount	 = 1;
}
void LogisticsMissionInfo::setPurchaseFile(PCSTR fileName)
{
	gosASSERT(bMultiplayer);
	MissionInfo* pInfo		= groups[0].infos[0];
	pInfo->purchaseFileName = missionPath;
	pInfo->purchaseFileName += fileName;
	pInfo->purchaseFileName += ".fit";
}

int32_t LogisticsMissionInfo::getAdditionalPurachaseFiles(PCSTR* list, int32_t& maxCount)
{
	if (maxCount >= additionalPurchaseFiles.Count())
	{
		int32_t i = 0;
		for (FILE_LIST::EIterator iter = additionalPurchaseFiles.Begin(); !iter.IsDone(); iter++)
		{
			list[i++] = *iter;
		}
	}
	maxCount = additionalPurchaseFiles.Count();
	return additionalPurchaseFiles.Count();
}
void LogisticsMissionInfo::addBonusPurchaseFile(PCSTR fileName)
{
	if (!fileName)
		return;
	PSTR pNewFile = new char[strlen(fileName) + 1];
	strcpy(pNewFile, fileName);
	additionalPurchaseFiles.Append(pNewFile);
}

bool LogisticsMissionInfo::skipLogistics()
{
	if (MPlayer)
		return 0;
	if (currentStage >= groupCount)
		return false;
	MissionGroup* pGroup = &groups[currentStage];
	return !pGroup->infos[currentMission]->playLogistics;
}
bool LogisticsMissionInfo::skipPilotReview()
{
	// need to subtract a mission, since we already set this one complete if we
	// got here
	MissionInfo* pInfo = getPreviousMission();
	if (!pInfo)
		return 0;
	return !pInfo->playPilotPromotion;
}
bool LogisticsMissionInfo::skipSalvageScreen()
{
	// need to subtract a mission, since we already set this one complete if we
	// got here
	MissionInfo* pInfo = getPreviousMission();
	if (!pInfo)
		return 0;
	return !pInfo->playSalvage;
}
bool LogisticsMissionInfo::skipPurchasing()
{
	if (MPlayer)
		return 0;
	MissionGroup* pGroup = &groups[currentStage];
	return !pGroup->infos[currentMission]->playPurchasing;
}

LogisticsMissionInfo::MissionInfo* LogisticsMissionInfo::getPreviousMission()
{
	if (currentMission > 0)
	{
		MissionGroup* pGroup = &groups[currentStage];
		return pGroup->infos[currentMission - 1];
	}
	else if (currentStage > 0)
	{
		MissionGroup* pGroup = &groups[currentStage] - 1;
		int32_t Count		 = pGroup->infos.Count();
		if (Count)
			return pGroup->infos[Count - 1];
		else
			return nullptr;
	}
	else if (currentStage == 0) // There may be only one stage.  I.e. Tutorials!
	{
		MissionGroup* pGroup = &groups[currentStage];
		int32_t Count		 = pGroup->infos.Count();
		if (Count)
			return pGroup->infos[Count - 1];
		else
			return nullptr;
	}
	return nullptr;
}

bool LogisticsMissionInfo::isSingleMission(void) const
{
	if (groupCount == 1)
	{
		if (groups[0].infos.Count() == 1)
			return 1;
	}
	return 0;
}

bool LogisticsMissionInfo::showChooseMission()
{
	if (MPlayer)
		return 0;
	MissionGroup* pGroup = &groups[currentStage];
	return pGroup->infos[currentMission]->playMissionSelection;
}

bool LogisticsMissionInfo::canHaveSalavageCraft()
{
	MissionGroup* pGroup = &groups[currentStage];
	return pGroup->infos[currentMission]->enableSalavageCraft;
}

bool LogisticsMissionInfo::canHaveRepairTruck()
{
	MissionGroup* pGroup = &groups[currentStage];
	return pGroup->infos[currentMission]->enableRepairTruck;
}
bool LogisticsMissionInfo::canHaveScoutCopter()
{
	MissionGroup* pGroup = &groups[currentStage];
	return pGroup->infos[currentMission]->enableScoutCopter;
}
bool LogisticsMissionInfo::canHaveArtilleryPiece()
{
	MissionGroup* pGroup = &groups[currentStage];
	return pGroup->infos[currentMission]->enableArtilleryPiece;
}
bool LogisticsMissionInfo::canHaveAirStrike()
{
	MissionGroup* pGroup = &groups[currentStage];
	return pGroup->infos[currentMission]->enableAirStrike;
}
bool LogisticsMissionInfo::canHaveSensorStrike()
{
	MissionGroup* pGroup = &groups[currentStage];
	return pGroup->infos[currentMission]->enableSensorStrike;
}
bool LogisticsMissionInfo::canHaveMineLayer()
{
	MissionGroup* pGroup = &groups[currentStage];
	return pGroup->infos[currentMission]->enableMineLayer;
}

bool LogisticsMissionInfo::getVideoShown()
{
	MissionGroup* pGroup = &groups[currentStage];
	return pGroup->videoShown;
}

void LogisticsMissionInfo::setVideoShown()
{
	MissionGroup* pGroup = &groups[currentStage];
	pGroup->videoShown   = true;
}

//*************************************************************************************************
// end of file ( LogisticsMissionInfo.cpp )
