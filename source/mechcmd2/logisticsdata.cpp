#define LOGISTICSDATA_CPP
/*************************************************************************************************\
LogisticsData.cpp			: Implementation of the LogisticsData component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdinc.h"

#include "LogisticsData.h"
#include "file.h"
#include "LogisticsErrors.h"
#include "cmponent.h"
#include "paths.h"
#include "warrior.h"
#include "resource.h"
#include "malloc.h"
#include "Team.h"
#include "Mech.h"
#include "LogisticsMissionInfo.h"
#include "packet.h"
#include "gamesound.h"
#include "prefs.h"
#include "comndr.h"
#include "missionresults.h"
#include "zlib.h"

#ifndef VIEWER
#include "multPlyr.h"
#include "chatwindow.h"
#else
bool MissionResults::FirstTimeResults = false;
#endif

extern CPrefs prefs;

//----------------------------------------------------------------------
// WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// PLEASE CHANGE THIS IF THE SAVEGAME FILE FORMAT CHANGES!!!!!!!!!!!!!
int32_t SaveGameVersionNumber = 10004;
//----------------------------------------------------------------------

LogisticsData* LogisticsData::instance = nullptr;

LogisticsData::LogisticsData()
{
	gosASSERT(!instance);
	instance = this;
	resourcePoints = 0;
	currentlyModifiedMech = 0;
	missionInfo = 0;
	rpJustAdded = false;
	bNewMechs = bNewWeapons = bNewPilots = 0;
}

LogisticsData::~LogisticsData()
{
	for (VARIANT_LIST::EIterator iter = variants.Begin(); !iter.IsDone(); iter++)
	{
		delete (*iter);
	}
	for (VEHICLE_LIST::EIterator vIter = vehicles.Begin(); !vIter.IsDone(); vIter++)
	{
		delete (*vIter);
	}
	for (MECH_LIST::EIterator mIter = inventory.Begin(); !mIter.IsDone(); mIter++)
	{
		delete (*mIter);
	}
	variants.Clear();
	vehicles.Clear();
	delete missionInfo;
	missionInfo = nullptr;
#ifndef VIEWER
	ChatWindow::destroy();
#endif
}

void LogisticsData::init()
{
	if (components.Count()) // already been initialized
	{
		return;
	}
	initComponents();
	initPilots();
	initVariants();
	missionInfo = new LogisticsMissionInfo;
	FitIniFile file;
	if (NO_ERROR != file.open("data\\campaign\\campaign.fit"))
	{
		Assert(0, 0, "coudln't find the campaign file\n");
	}
	missionInfo->init(file);
	// temporary, just so we can test
	int32_t count = 32;
	std::wstring_view missionNames[32];
	missionInfo->getAvailableMissions(missionNames, count);
	setCurrentMission(missionNames[0]);
}

void LogisticsData::initComponents()
{
	wchar_t componentPath[256];
	strcpy(componentPath, objectPath);
	strcat(componentPath, "compbas.csv");
	File componentFile;
#ifdef _DEBUG
	int32_t result =
#endif
		componentFile.open(componentPath);
	gosASSERT(result == NO_ERROR);
	uint8_t* data = new uint8_t[componentFile.getLength()];
	componentFile.read(data, componentFile.getLength());
	File dataFile;
	dataFile.open((std::wstring_view)data, componentFile.getLength());
	componentFile.close();
	uint8_t line[1024];
	std::wstring_view pLine = (std::wstring_view)line;
	// skip the first line
	dataFile.readLine(line, 1024);
	int32_t Ammo[512];
	memset(Ammo, 0, sizeof(int32_t) * 512);
	LogisticsComponent tmpComp;
	int32_t counter = 0;
	while (true)
	{
		int32_t result = dataFile.readLine(line, 1023);
		if (result < 2 || result == 0xBADF0008 || result > 1024)
			break;
		components.Append(tmpComp);
		LogisticsComponent& tmp = components.GetTail();
		if (-1 == tmp.init(pLine)) // failure
		{
			Ammo[counter] = (int32_t)tmp.getRecycleTime();
			components.DeleteTail();
		}
		counter++;
	}
	// fix up ammo
	for (COMPONENT_LIST::EIterator iter = components.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter).getAmmo())
		{
			(*iter).setAmmo(Ammo[(*iter).getAmmo()]);
		}
	}
	delete[] data;
	data = nullptr;
}

void LogisticsData::initPilots()
{
	pilots.Clear();
	wchar_t pilotPath[256];
	strcpy(pilotPath, objectPath);
	strcat(pilotPath, "pilots.csv");
	File pilotFile;
	pilotFile.open(pilotPath);
	uint8_t pilotFileName[256];
	int32_t id = 1;
	while (true)
	{
		int32_t bytesread = pilotFile.readLine(pilotFileName, 256);
		if (bytesread < 2)
			break;
		LogisticsPilot tmpPilot;
		pilots.Append(tmpPilot);
		LogisticsPilot& pilot = pilots.GetTail();
		pilot.id = id;
		if (-1 == pilot.init((std::wstring_view)pilotFileName))
			pilots.DeleteTail();
		id++;
	}
}

void LogisticsData::initVariants()
{
	wchar_t variantPath[256];
	strcpy(variantPath, artPath);
	strcat(variantPath, "buildings.csv");
	CSVFile variantFile;
	variantFile.open(variantPath);
	FullPathFileName pakPath;
	pakPath.init(objectPath, "Object2", ".pak");
	PacketFile pakFile;
	if (NO_ERROR != pakFile.open(pakPath))
	{
		wchar_t errorStr[256];
		sprintf(errorStr, "couldn't open file %s", (std::wstring_view)pakPath);
		Assert(0, 0, errorStr);
	}
	wchar_t variantFileName[256];
	wchar_t variantFullPath[1024];
	int32_t chassisID = 0;
	wchar_t tmpStr[256];
	int32_t i = 1;
	while (true)
	{
		int32_t fitID;
		int32_t retVal = variantFile.readString(i, 4, tmpStr, 256);
		if (retVal != 0)
			break;
		if (_stricmp(tmpStr, "VEHICLE") == 0)
		{
			float scale;
			variantFile.readFloat(i, 11, scale);
			if (scale)
			{
				variantFile.readLong(i, 5, fitID);
				addVehicle(fitID, pakFile, scale);
			}
			i++;
			continue;
		}
		if (_stricmp(tmpStr, "MECH") != 0)
		{
			float scale;
			if (NO_ERROR == variantFile.readFloat(i, 11, scale) && scale)
			{
				variantFile.readLong(i, 5, fitID);
				addBuilding(fitID, pakFile, scale);
			}
			i++;
			continue;
		}
		float scale;
		if (NO_ERROR != variantFile.readFloat(i, 11, scale))
			scale = 1.0;
		variantFile.readString(i, 1, variantFileName, 256);
		variantFile.readLong(i, 5, fitID);
		strcpy(variantFullPath, objectPath);
		strcat(variantFullPath, variantFileName);
		strcat(variantFullPath, ".csv");
		_strlwr(variantFullPath);
		CSVFile mechFile;
		if (NO_ERROR != mechFile.open(variantFullPath))
		{
			wchar_t error[256];
			sprintf(error, "couldn't open file %s", variantFullPath);
			Assert(0, 0, error);
			return;
		}
		LogisticsChassis* chassis = new LogisticsChassis();
		chassis->init(&mechFile, chassisID++);
		chassis->setFitID(fitID);
		chassis->setScale(scale);
		int32_t row = 23;
		wchar_t buffer[256];
		int32_t varCount = 0;
		while (NO_ERROR == mechFile.readString(row, 2, buffer, 256))
		{
			LogisticsVariant* pVariant = new LogisticsVariant;
			if (0 == pVariant->init(&mechFile, chassis, varCount++))
				variants.Append(pVariant);
			else
				delete pVariant;
			row += 97;
		}
		i++;
	}
}

void LogisticsData::addVehicle(int32_t fitID, PacketFile& objectFile, float scale)
{
	if (NO_ERROR != objectFile.seekPacket(fitID))
		return;
	int32_t fileSize = objectFile.getPacketSize();
	if (fileSize)
	{
		LogisticsVehicle* pVehicle = new LogisticsVehicle;
		FitIniFile file;
		file.open(&objectFile, fileSize);
		pVehicle->init(file);
		pVehicle->setScale(scale);
		vehicles.Append(pVehicle);
	}
}

void LogisticsData::RegisterFunctions()
{
}

void LogisticsData::UnRegisterFunctions()
{
}

int32_t
LogisticsData::getAvailableComponents(LogisticsComponent** pComps, int32_t& maxCount)
{
	int32_t retVal = 0;
	int32_t i = 0;
	for (COMPONENT_LIST::EIterator iter = components.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter).isAvailable())
		{
			if (i < maxCount)
				pComps[i] = &(*iter);
			else
				retVal = NEED_BIGGER_ARRAY;
			++i;
		}
	}
	maxCount = i;
	return retVal;
}
int32_t
LogisticsData::getAllComponents(LogisticsComponent** pComps, int32_t& maxCount)
{
	int32_t retVal = 0;
	int32_t i = 0;
	for (COMPONENT_LIST::EIterator iter = components.Begin(); !iter.IsDone(); iter++)
	{
		if (i < maxCount)
			pComps[i] = &(*iter);
		else
			retVal = NEED_BIGGER_ARRAY;
		++i;
	}
	maxCount = components.Count();
	return retVal;
}

int32_t
LogisticsData::getPurchasableMechs(LogisticsVariant** array, int32_t& count)
{
	int32_t retVal = 0;
	int32_t arraySize = count;
	count = 0;
	for (VARIANT_LIST::EIterator iter = instance->variants.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->isAvailable())
		{
			if (count >= arraySize)
			{
				retVal = NEED_BIGGER_ARRAY; // keep counting
			}
			else
			{
				array[count] = (*iter);
			}
			(count)++;
		}
	}
	return retVal;
}

int32_t
LogisticsData::purchaseMech(LogisticsVariant* pVariant)
{
	if (!pVariant)
		return -1;
	int32_t RP = pVariant->getCost();
	if (missionInfo->getCBills() - RP >= 0)
	{
		int32_t count = instance->createInstanceID(pVariant);
		LogisticsMech* pMech = new LogisticsMech(pVariant, count);
		instance->inventory.Append(pMech);
		missionInfo->decrementCBills(pVariant->getCost());
		return 0;
	}
	return NOT_ENOUGH_RESOURCE_POINTS;
}

int32_t
LogisticsData::canPurchaseMech(LogisticsVariant* pVar)
{
	if (!pVar)
		return INVALID_ID;
	int32_t RP = pVar->getCost();
	if (missionInfo->getCBills() - RP >= 0)
	{
		return 0;
	}
	return NOT_ENOUGH_RESOURCE_POINTS;
}

int32_t
LogisticsData::sellMech(LogisticsMech* pVar)
{
	if (!pVar)
		return -1;
	for (MECH_LIST::EIterator iter = instance->inventory.End(); !iter.IsDone(); iter--)
	{
		if ((*iter)->getForceGroup())
			continue;
		if ((*iter)->getVariant() == pVar->getVariant())
		{
			int32_t cost = ((*iter))->getCost();
			(*iter)->setPilot(nullptr);
			delete *iter;
			instance->inventory.Delete(iter);
			missionInfo->incrementCBills(cost);
			return 0;
		}
	}
	return -1;
}

int32_t
LogisticsData::removeVariant(std::wstring_view varName)
{
	if (!varName)
		return -1;
	LogisticsVariant* pVar = 0;
	VARIANT_LIST::EIterator vIter;
	if (currentlyModifiedMech->getName() == varName || oldVariant->getName() == varName)
		return -1;
	for (vIter = variants.Begin(); !vIter.IsDone(); vIter++)
	{
		if ((*vIter)->getName().Compare(varName, 0) == 0)
		{
			pVar = (*vIter);
			break;
		}
	}
	if (!pVar)
	{
		return INVALID_VARIANT;
	}
	for (MECH_LIST::EIterator iter = instance->inventory.End(); !iter.IsDone(); iter--)
	{
		if ((*iter)->getVariant() == pVar)
		{
			return VARIANT_IN_USE;
		}
	}
	delete pVar;
	variants.Delete(vIter);
	return 0;
}

int32_t
LogisticsData::createInstanceID(LogisticsVariant* pVar)
{
	int32_t count = -1;
	for (MECH_LIST::EIterator iter = inventory.Begin(); !iter.IsDone(); iter++)
	{
		if (pVar->getVariantID() == (*iter)->getVariantID())
		{
			int32_t tmp = (*iter)->getInstanceID();
			if (tmp > count)
				count = tmp;
		}
	}
	return count + 1;
}

LogisticsVariant*
LogisticsData::getVariant(int32_t ID)
{
	for (VARIANT_LIST::EIterator iter = variants.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getID() == (uint32_t)(ID & 0x00ffffff))
		{
			return *iter;
		}
	}
	if (instance->currentlyModifiedMech && ID == instance->currentlyModifiedMech->getID())
		return instance->currentlyModifiedMech->getVariant();
	return nullptr;
}

LogisticsMech*
LogisticsData::getMech(int32_t ID)
{
	for (MECH_LIST::EIterator iter = inventory.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getID() == ID)
			return (*iter);
	}
	return nullptr;
}

int32_t
LogisticsData::addMechToForceGroup(LogisticsMech* pMech, int32_t slot)
{
	if (!pMech)
		return -1;
	if (slot > 12)
		return -1;
	if (pMech && !pMech->getForceGroup())
	{
		pMech->setForceGroup(slot);
		return 0;
	}
	else // find another of the same variant
	{
		LogisticsMech* pNewMech = getMechWithoutForceGroup(pMech);
		if (pNewMech)
		{
			pNewMech->setForceGroup(slot);
			return 0;
		}
	}
	return -1;
}

int32_t
LogisticsData::removeMechFromForceGroup(int32_t slot)
{
	for (MECH_LIST::EIterator iter = inventory.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getForceGroup() == slot)
		{
			(*iter)->setForceGroup(0);
			(*iter)->setPilot(0);
			return 0;
		}
	}
	return -1;
}

LogisticsMech*
LogisticsData::getMechWithoutForceGroup(LogisticsMech* pMech)
{
	if (!pMech->getForceGroup())
		return pMech;
	for (MECH_LIST::EIterator iter = inventory.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getVariant() == pMech->getVariant() && !(*iter)->getForceGroup())
		{
			return (*iter);
		}
	}
	return nullptr;
}
int32_t
LogisticsData::removeMechFromForceGroup(LogisticsMech* pMech, bool bRemovePilot)
{
	if (!pMech)
		return -1;
	if (pMech && pMech->getForceGroup())
	{
		pMech->setForceGroup(0);
		// no mechs in inventory have pilots
		if (bRemovePilot)
			pMech->setPilot(0);
		return 0;
	}
	// find similar one
	for (MECH_LIST::EIterator iter = inventory.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getVariant() == pMech->getVariant() && (*iter)->getForceGroup())
		{
			(*iter)->setForceGroup(0);
			if (bRemovePilot)
				pMech->setPilot(0);
			return 0;
		}
	}
	return -1;
}

LogisticsPilot*
LogisticsData::getFirstAvailablePilot()
{
	for (PILOT_LIST::EIterator iter = pilots.Begin(); !iter.IsDone(); iter++)
	{
		bool bIsUsed = false;
		for (MECH_LIST::EIterator mIter = inventory.Begin(); !mIter.IsDone(); mIter++)
		{
			LogisticsPilot* pPilot = (*mIter)->getPilot();
			if (pPilot && pPilot->getID() == (*iter).getID())
			{
				bIsUsed = true;
				break;
			}
		}
		if (!bIsUsed)
		{
			return &(*iter);
		}
	}
	return nullptr;
}

// GetAvailableMissions( std::wstring_view* missionNames, int32_t& count )
int32_t
LogisticsData::getAvailableMissions(std::wstring_view* missionNames, int32_t& count)
{
	int32_t numberOfEm = 0;
	// first figure out how many there are
	missionInfo->getAvailableMissions(0, numberOfEm);
	// next make sure the array is big enough
	if (count < numberOfEm)
		return NEED_BIGGER_ARRAY;
	missionInfo->getAvailableMissions(missionNames, numberOfEm);
	count = numberOfEm;
	return 0;
}

int32_t
LogisticsData::getCurrentMissions(std::wstring_view* missionNames, int32_t& count)
{
	int32_t numberOfEm = 0;
	// first figure out how many there are
	missionInfo->getCurrentMissions(0, numberOfEm);
	// next make sure the array is big enough
	if (count < numberOfEm)
		return NEED_BIGGER_ARRAY;
	numberOfEm = count;
	missionInfo->getCurrentMissions(missionNames, numberOfEm);
	count = numberOfEm;
	return 0;
}

bool LogisticsData::getMissionAvailable(std::wstring_view missionName)
{
	return missionInfo->getMissionAvailable(missionName);
}

// SetCurrentMission( std::wstring_view missionName )
int32_t
LogisticsData::setCurrentMission(std::wstring_view missionName)
{
	int32_t result = missionInfo->setNextMission(missionName);
	if (result == NO_ERROR)
	{
		// if we made it this far
		updateAvailability();
		resourcePoints = missionInfo->getCurrentRP();
		removeDeadWeight();
	}
	return result;
}

void LogisticsData::removeDeadWeight()
{
	int32_t maxDropWeight = getMaxDropWeight();
	int32_t curDropWeight = getCurrentDropWeight();
	int32_t i = 12;
	while (curDropWeight > maxDropWeight)
	{
		LogisticsData::removeMechFromForceGroup(i);
		i--;
		curDropWeight = getCurrentDropWeight();
		if (i == 0)
			break;
	}
}

int32_t
LogisticsData::setCurrentMission(std::wstring_view missionName)
{
	return setCurrentMission((std::wstring_view)missionName);
}

void LogisticsData::getForceGroup(EList<LogisticsMech*, LogisticsMech*>& newList)
{
	int32_t count = 0;
	for (MECH_LIST::EIterator iter = inventory.Begin(); !iter.IsDone(); iter++)
	{
		if (count > 11)
			break;
		if ((*iter)->getForceGroup())
		{
			newList.Append((*iter));
			count++;
		}
	}
}

void LogisticsData::getInventory(EList<LogisticsMech*, LogisticsMech*>& newList)
{
	for (MECH_LIST::EIterator iter = inventory.Begin(); !iter.IsDone(); iter++)
	{
		newList.Append((*iter));
	}
}

void LogisticsData::addMechToInventory(
	LogisticsVariant* pVar, LogisticsPilot* pPilot, int32_t ForceGroup, bool bSubtractPts)
{
	if (pVar)
	{
		int32_t count = instance->createInstanceID(pVar);
		LogisticsMech* pMech = new LogisticsMech(pVar, count);
		inventory.Append(pMech);
		pMech->setForceGroup(ForceGroup);
		if (pPilot && !pPilot->isDead())
			pMech->setPilot(pPilot);
		if (ForceGroup > -1 && ForceGroup < 13)
			pMech->setForceGroup(ForceGroup);
		if (bSubtractPts)
			missionInfo->decrementCBills(pMech->getCost());
	}
}
void LogisticsData::addMechToInventory(LogisticsVariant* pVar, int32_t addToForceGroup,
	LogisticsPilot* pPilot, uint32_t basecolour, uint32_t highlight1, uint32_t highlight2)
{
	if (pVar)
	{
		int32_t count = instance->createInstanceID(pVar);
		LogisticsMech* pMech = new LogisticsMech(pVar, count);
		inventory.Append(pMech);
		if (addToForceGroup > -1 && addToForceGroup < 13)
			pMech->setForceGroup(addToForceGroup);
		if (pPilot && !pPilot->isDead())
			pMech->setPilot(pPilot);
		pMech->setcolours(basecolour, highlight1, highlight2);
		return;
	}
	else
	{
		gosASSERT(!"couldn't add the mech to the inventory");
	}
}

LogisticsVariant*
LogisticsData::getVariant(std::wstring_view pCSVFileName, int32_t VariantNum)
{
	std::wstring_view lowerCase = pCSVFileName;
	lowerCase.MakeLower();
	for (VARIANT_LIST::EIterator iter = variants.Begin(); !iter.IsDone(); iter++)
	{
		if (-1 != ((*iter)->getFileName().Find(lowerCase, -1)) && (((*iter)->getVariantID() >> 16) & 0xff) == VariantNum)
		{
			return *iter;
		}
	}
	return nullptr;
}

void LogisticsData::removeMechsInForceGroup()
{
	if (!inventory.Count())
		return;
	for (MECH_LIST::EIterator iter = inventory.End(); !iter.IsDone();)
	{
		if ((*iter)->getForceGroup())
		{
			MECH_LIST::EIterator tmpIter = iter;
			iter--;
			delete *tmpIter;
			inventory.Delete(tmpIter);
		}
		else
			iter--;
	}
}

std::wstring_view
LogisticsData::getBestPilot(int32_t mechWeight)
{
	if (!pilots.Count())
		initPilots();
	LogisticsPilot** pPilots = (LogisticsPilot**)_alloca(pilots.Count() * sizeof(LogisticsPilot*));
	memset(pPilots, 0, pilots.Count() * sizeof(LogisticsPilot*));
	int32_t counter = 0;
#ifndef VIEWER
	for (PILOT_LIST::EIterator iter = pilots.Begin(); !iter.IsDone(); iter++)
	{
		std::wstring_view nameCheck = (*iter).getName();
		if ((*iter).isAvailable() && (MPlayer || !MechWarrior::warriorInUse((std::wstring_view)nameCheck)))
			pPilots[counter++] = &(*iter);
	}
#endif
	int32_t count = counter;
	int32_t i;
	for (i = 1; i < count; ++i)
	{
		LogisticsPilot* cur = pPilots[i];
		for (size_t j = 0; j < i; ++j)
		{
			if (comparePilots(cur, pPilots[j], mechWeight) > 0 && j != i)
			{
				pPilots[i] = pPilots[j];
				pPilots[j] = cur;
				break;
			}
		}
	}
	for (i = 0; i < count; i++)
	{
		if (pPilots[i]->isAvailable() && !pPilots[i]->isUsed())
		{
			pPilots[i]->setUsed(1);
			return pPilots[i]->getFileName();
		}
	}
	gosASSERT(!"We're out of pilots, shouldn't be here");
	pPilots[0]->setUsed(true);
	return pPilots[0]->getFileName();
}

bool LogisticsData::gotPilotsLeft()
{
	if (!pilots.Count())
		initPilots();
	LogisticsPilot** pPilots = (LogisticsPilot**)_alloca(pilots.Count() * sizeof(LogisticsPilot*));
	memset(pPilots, 0, pilots.Count() * sizeof(LogisticsPilot*));
	int32_t counter = 0;
#ifndef VIEWER
	for (PILOT_LIST::EIterator iter = pilots.Begin(); !iter.IsDone(); iter++)
	{
		std::wstring_view nameCheck = (*iter).getName();
		if ((*iter).isAvailable() && (MPlayer || !MechWarrior::warriorInUse((std::wstring_view)nameCheck)))
			pPilots[counter++] = &(*iter);
	}
#endif
	int32_t count = counter;
	for (size_t i = 0; i < count; i++)
	{
		if (pPilots[i]->isAvailable() && !pPilots[i]->isUsed())
		{
			return 1;
		}
	}
	return 0;
}

int32_t
LogisticsData::comparePilots(LogisticsPilot* p1, LogisticsPilot* p2, int32_t weight)
{
	if (p1->isUsed())
		return -1;
	else if (p2->isUsed())
		return 1;
	for (MECH_LIST::EIterator mIter = instance->inventory.Begin(); !mIter.IsDone(); mIter++)
	{
		LogisticsPilot* pPilot = (*mIter)->getPilot();
		if (pPilot)
		{
			if (pPilot->getID() == p1->getID())
				return -1;
			else if (pPilot->getID() == p2->getID())
				return 1;
		}
	}
	if (p1->getRank() > p2->getRank())
		return 1;
	else if (p2->getRank() < p1->getRank())
		return -1;
	// need to check specialty skill text for weight, not really done yet
	else if (p2->getGunnery() > p1->getGunnery())
		return -1;
	else if (p1->getGunnery() > p2->getGunnery())
		return 1;
	else if (p1->getPiloting() > p2->getPiloting())
		return 1;
	else if (p2->getPiloting() > p1->getPiloting())
		return -1;
	return 0;
}

int32_t
LogisticsData::save(FitIniFile& file)
{
	int32_t variantCount = 0;
	// save the player created variants
	for (VARIANT_LIST::EIterator vIter = variants.Begin(); !vIter.IsDone(); vIter++)
	{
		if (!(*vIter)->isDesignerMech())
		{
			(*vIter)->save(file, variantCount);
			variantCount++;
		}
	}
	file.writeBlock("Version");
	file.writeIdLong("VersionNumber", SaveGameVersionNumber);
	file.writeBlock("General");
	file.writeIdLong("VariantCount", variantCount);
	file.writeIdLong("PilotCount", pilots.Count());
	file.writeIdLong("InventoryCount", inventory.Count());
	file.writeIdBoolean("FirstTimeResults", MissionResults::FirstTimeResults);
	// save the campaign info
	missionInfo->save(file);
	int32_t pilotCount = 0;
	// save the pilots
	for (PILOT_LIST::EIterator pIter = pilots.Begin(); !pIter.IsDone(); pIter++)
	{
		(*pIter).save(file, pilotCount++);
	}
	int32_t mechCount = 0;
	// save the inventory
	for (MECH_LIST::EIterator mIter = inventory.Begin(); !mIter.IsDone(); mIter++)
	{
		(*mIter)->save(file, mechCount++);
	}
	return 0;
}

void LogisticsData::clearVariants()
{
	for (VARIANT_LIST::EIterator iter = variants.End(); !iter.IsDone();)
	{
		if (!(*iter)->isDesignerMech())
		{
			VARIANT_LIST::EIterator tmpIter = iter;
			iter--;
			delete *tmpIter;
			variants.Delete(tmpIter);
		}
		else
			iter--;
	}
}

int32_t
LogisticsData::load(FitIniFile& file)
{
	clearInventory();
	resourcePoints = 0;
	pilots.Clear();
	initPilots();
	clearVariants();
	if (!missionInfo)
		missionInfo = new LogisticsMissionInfo;
	missionInfo->load(file);
	int32_t result = file.seekBlock("Version");
	if (result != NO_ERROR)
	{
		PAUSE(("SaveGame has no version number.  Not Loading"));
		return -1;
	}
	int32_t testVersionNum = 0;
	result = file.readIdLong("VersionNumber", testVersionNum);
	if (result != NO_ERROR)
	{
		PAUSE(("SaveGame has no version number.  Not Loading"));
		return -1;
	}
	if (testVersionNum != SaveGameVersionNumber)
	{
		PAUSE(("SaveGame is not Version %d.  It was Version %d which is not "
			   "valid!",
			SaveGameVersionNumber, testVersionNum));
		return -1;
	}
	file.seekBlock("General");
	int32_t variantCount, pilotCount, inventoryCount;
	variantCount = pilotCount = inventoryCount = 0;
	file.readIdLong("VariantCount", variantCount);
	file.readIdLong("PilotCount", pilotCount);
	file.readIdLong("InventoryCount", inventoryCount);
	file.readIdBoolean("FirstTimeResults", MissionResults::FirstTimeResults);
	wchar_t tmp[64];
	// load variants
	int32_t i;
	for (i = 0; i < variantCount; i++)
	{
		sprintf(tmp, "Variant%ld", i);
		file.seekBlock(tmp);
		result = loadVariant(file);
		if (result != NO_ERROR)
		{
			gosASSERT(0);
			return -1;
		}
	}
	// load pilots
	for (i = 0; i < pilotCount; i++)
	{
		sprintf(tmp, "Pilot%ld", i);
		if (NO_ERROR != file.seekBlock(tmp))
		{
			gosASSERT(0);
		}
		file.readIdString("filename", tmp, 255);
		// pilot should already exist
		for (PILOT_LIST::EIterator pIter = pilots.Begin(); !pIter.IsDone(); pIter++)
		{
			if ((*pIter).getFileName().Compare(tmp, 0) == 0)
			{
				(*pIter).load(file);
			}
			(*pIter).setAvailable(true);
		}
	}
	// load inventory
	int32_t count = 0;
	for (i = 0; i < inventoryCount; i++)
	{
		sprintf(tmp, "Inventory%ld", i);
		if (NO_ERROR != file.seekBlock(tmp))
		{
			gosASSERT(0);
		}
		loadMech(file, count);
	}
	updateAvailability();
	// Start finding the Leaks
	// systemHeap->dumpRecordLog();
	return 0;
}

int32_t
LogisticsData::loadVariant(FitIniFile& file)
{
	wchar_t tmp[256];
	file.readIdString("Chassis", tmp, 255);
	const LogisticsChassis* pChassis = nullptr;
	// go out and find that chassis
	for (VARIANT_LIST::EIterator vIter = variants.Begin(); !vIter.IsDone(); vIter++)
	{
		if ((*vIter)->getFileName().Compare(tmp, 0) == 0)
		{
			pChassis = (*vIter)->getChassis();
		}
	}
	if (!pChassis) // we can always try and make it ourseleves, but it should
		// have been loaded by now
	{
		gosASSERT(0);
		return INVALID_FILE_NAME;
	}
	// create the variant, add to the list
	file.readIdString("VariantName", tmp, 255);
	LogisticsVariant* pVariant = new LogisticsVariant(pChassis, 0);
	variants.Append(pVariant);
	pVariant->setName(tmp);
	int32_t componentCount = 0;
	int32_t x = 0;
	int32_t y = 0;
	int32_t location = 0;
	int32_t id = 0;
	wchar_t tmp2[256];
	// read number of components
	file.readIdLong("ComponentCount", componentCount);
	// add those components
	for (size_t i = 0; i < componentCount; i++)
	{
		sprintf(tmp, "Component%ld", i);
		file.readIdLong(tmp, id);
		strcpy(tmp2, tmp);
		strcat(tmp2, "x");
		file.readIdLong(tmp2, x);
		strcpy(tmp2, tmp);
		strcat(tmp2, "y");
		file.readIdLong(tmp2, y);
		strcpy(tmp2, tmp);
		strcat(tmp2, "Location");
		file.readIdLong(tmp2, location);
		pVariant->addComponent(id, x, y);
	}
	return 0;
}

int32_t
LogisticsData::loadMech(FitIniFile& file, int32_t& count)
{
	wchar_t tmp[256];
	file.readIdString("Variant", tmp, 255);
	for (VARIANT_LIST::EIterator mIter = variants.Begin(); !mIter.IsDone(); mIter++)
	{
		if ((*mIter)->getName().Compare(tmp, 0) == 0)
		{
			LogisticsMech* pMech = new LogisticsMech((*mIter), count);
			file.readIdString("Pilot", tmp, 255);
			inventory.Append(pMech);
			for (PILOT_LIST::EIterator pIter = pilots.Begin(); !pIter.IsDone(); pIter++)
			{
				if ((*pIter).getFileName().Compare(tmp, 0) == 0)
				{
					pMech->setPilot(&(*pIter));
					count++;
					if (count > -1 && count < 13)
						pMech->setForceGroup(count);
					break;
				}
			}
			// it could have had no pilot
			return 0;
		}
	}
	return -1; // failed in finding the variant
}

void LogisticsData::setMissionCompleted()
{
#ifndef VIEWER
	std::wstring_view pMissionName = missionInfo->getCurrentMission();
	missionInfo->setMissionComplete();
	rpJustAdded = 0;
	// first set all pilots as not just dead
	for (PILOT_LIST::EIterator iter = pilots.Begin(); !iter.IsDone(); iter++)
	{
		(*iter).setUsed(0);
	}
	for (EList<CObjective*, CObjective*>::EIterator oIter = Team::home->objectives.Begin();
		 !oIter.IsDone(); oIter++)
	{
		if ((*oIter)->Status(Team::home->objectives) == OS_SUCCESSFUL)
		{
			addCBills((*oIter)->ResourcePoints());
		}
	}
	// need to go find out which pilots died.
	Team* pTeam = Team::home;
	int32_t ForceGroupCount = 1;
	if (pTeam)
	{
		for (size_t i = pTeam->getRosterSize() - 1; i > -1; i--)
		{
			Mover* pMover = (Mover*)pTeam->getMover(i);
			// Must check if we ever linked up with the mech!!
			if (pMover->isOnGUI() && (pMover->getObjectType()->getObjectTypeClass() == BATTLEMECH_TYPE) && (pMover->getCommanderId() == Commander::home->getId()) && (pMover->getMoveType() != MOVETYPE_AIR))
			{
				LogisticsMech* pMech = getMech(pMover->getName(), pMover->getPilot()->getName());
				uint32_t base, highlight1, highlight2;
				((Mech3DAppearance*)pMover->getAppearance())
					->getPaintScheme(base, highlight1, highlight2);
				LogisticsPilot* pPilot = getPilot(pMover->getPilot()->getName());
				if (pMech)
				{
					if (pMover->isDestroyed() || pMover->isDisabled())
					{
						removeMechFromInventory(pMech->getName(), pMover->getPilot()->getName());
					}
					else
					{
						removeMechFromInventory(pMech->getName(), pMover->getPilot()->getName());
						LogisticsVariant* pVar = getVariant(((BattleMech*)pMover)->variantName);
						addMechToInventory(
							pVar, ForceGroupCount++, pPilot, base, highlight1, highlight2);
						if (pPilot)
							pPilot->setUsed(true);
					}
				}
				else // mech was recovered during the mission
				{
					if (!pMover->isDestroyed() && !pMover->isDisabled())
					{
						// find the variant with this mech's info
						LogisticsVariant* pVariant = getVariant(((BattleMech*)pMover)->variantName);
						if (!pVariant)
						{
							Assert(0, 0, "couldn't find the variant of a salvaged mech");
						}
						else
						{
							addMechToInventory(
								pVariant, ForceGroupCount++, pPilot, base, highlight1, highlight2);
							if (pPilot)
								pPilot->setUsed(true);
						}
					}
				}
				if (pPilot)
				{
					pPilot->update(pMover->getPilot());
					//					if ( pMover->isDestroyed() ||
					// pMover->isDisabled() )
					//						pPilot->setUsed( false );
				}
			}
		}
	}
#endif
}

LogisticsMech*
LogisticsData::getMech(std::wstring_view MechName, std::wstring_view pilotName)
{
	for (MECH_LIST::EIterator iter = inventory.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getName().Compare(MechName, 0) == 0)
		{
			if (!pilotName)
			{
				if (!(*iter)->getPilot())
					return (*iter);
			}
			else
			{
				if ((*iter)->getPilot() && (*iter)->getPilot()->getName().Compare(pilotName, 0) == 0)
					return (*iter);
			}
		}
	}
	return nullptr;
}

void LogisticsData::removeMechFromInventory(std::wstring_view mechName, std::wstring_view pilotName)
{
	LogisticsMech* pMech = getMech(mechName, pilotName);
	gosASSERT(pMech);
	if (pMech)
	{
		inventory.Delete(inventory.Find(pMech));
		delete pMech;
	}
}

LogisticsPilot*
LogisticsData::getPilot(std::wstring_view pilotName)
{
	// look for available ones first
	PILOT_LIST::EIterator iter;
	for (iter = pilots.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter).isAvailable())
		{
			if ((*iter).getName().Compare(pilotName, 0) == 0)
			{
				return &(*iter);
			}
		}
	}
	for (iter = pilots.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter).getName().Compare(pilotName, 0) == 0)
		{
			return &(*iter);
		}
	}
	return nullptr;
}

LogisticsVariant*
LogisticsData::getVariant(std::wstring_view mechName)
{
	for (VARIANT_LIST::EIterator iter = variants.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getName().Compare(mechName, 0) == 0)
			return (*iter);
	}
	return nullptr;
}

int32_t
LogisticsData::updateAvailability()
{
	bNewWeapons = 0;
	std::wstring_view purchaseFileName = missionInfo->getCurrentPurchaseFile();
	purchaseFileName.MakeLower();
	if (purchaseFileName.Length() < 1)
	{
		purchaseFileName = missionPath;
		purchaseFileName += "purchase.fit";
	}
	int32_t oldMechAvailableCount = 0;
	int32_t newMechAvailableCount = 0;
	int32_t oldPilotAvailableCount = 0;
	int32_t newPilotAvailableCount = 0;
	PILOT_LIST::EIterator pIter;
	for (pIter = pilots.Begin(); !pIter.IsDone(); pIter++)
	{
		if ((*pIter).bAvailable)
			oldPilotAvailableCount++;
		(*pIter).setAvailable(0);
	}
	// make sure its around and you can open it
	FitIniFile file;
	if (NO_ERROR != file.open((std::wstring_view)(std::wstring_view)purchaseFileName))
	{
		std::wstring_view error;
		error.Format("Couldn't open %s", (std::wstring_view)(std::wstring_view)purchaseFileName);
		PAUSE(((std::wstring_view)(std::wstring_view)error));
		return NO_PURCHASE_FILE;
	}
	// read in available components
	bool available[255];
	memset(available, 0, sizeof(bool) * 255);
	int32_t result = file.seekBlock("Components");
	if (result != NO_ERROR)
	{
		Assert(0, 0, "No components in the purchase file");
	}
	wchar_t tmp[32];
	int32_t component;
	bool bAll = 0;
	file.readIdBoolean("AllComponents", bAll);
	int32_t i;
	for (i = 0; i < 255; i++)
	{
		if (bAll)
			available[i] = 1;
		else
		{
			sprintf(tmp, "Component%ld", i);
			if (NO_ERROR != file.readIdLong(tmp, component))
				break;
			available[component] = 1;
		}
	}
	// go through comonent list, and set 'em
	COMPONENT_LIST::EIterator cIter;
	for (cIter = components.Begin(); !cIter.IsDone(); cIter++)
	{
		if (available[(*cIter).getID()] || bAll)
		{
			if (!(*cIter).isAvailable())
				bNewWeapons = true;
			(*cIter).setAvailable(1);
		}
	}
	std::wstring_view pFileNames[512];
	int32_t count = 512;
	missionInfo->getAdditionalPurachaseFiles(pFileNames, count);
	// reset all variants to unavailable
	VARIANT_LIST::EIterator vIter;
	for (vIter = variants.Begin(); !vIter.IsDone(); vIter++)
	{
		if ((*vIter)->isAvailable() && !((*vIter)->getID() >> 16))
			oldMechAvailableCount++;
		(*vIter)->setAvailable(0);
	}
	for (i = 0; i < count; i++)
	{
		appendAvailability(pFileNames[i], available);
	}
	// go through comonent list, and set 'em
	for (cIter = components.Begin(); !cIter.IsDone(); cIter++)
	{
		if (!available[(*cIter).getID()])
		{
			(*cIter).setAvailable(0);
		}
	}
	// go through each variant, and see if it's available
	wchar_t chassisFileName[255];
	int32_t componentArray[255];
	int32_t componentCount;
	file.seekBlock("Mechs");
	for (i = 0; i < 255; i++)
	{
		sprintf(tmp, "Mech%ld", i);
		if (NO_ERROR != file.readIdString(tmp, chassisFileName, 254))
			break;
		// go through each variant, if it has the same chassis, check and see if
		// all of its components are valid
		for (vIter = variants.Begin(); !vIter.IsDone(); vIter++)
		{
			std::wstring_view mechName = (*vIter)->getFileName();
			wchar_t realName[1024];
			_splitpath(mechName, nullptr, nullptr, realName, nullptr);
			if (_stricmp(realName, chassisFileName) == 0)
			{
				componentCount = 255;
				bool bRight = true;
				(*vIter)->getComponents(componentCount, componentArray);
				for (size_t i = 0; i < componentCount; i++)
				{
					if (!available[componentArray[i]]) // unavailable componets
					{
						// wchar_t errorStr[256];
						// sprintf( errorStr, "mech %s discarded because it
						// contains a %ld", 	chassisFileName, componentArray[i]
						//);  PAUSE(( errorStr ));
						bRight = false;
						break;
					}
				}
				if (bRight)
				{
					(*vIter)->setAvailable(true);
					if (!((*vIter)->getID() >> 16))
						newMechAvailableCount++;
				}
			}
		}
	}
	if (newMechAvailableCount != oldMechAvailableCount)
		bNewMechs = true;
	else
		bNewMechs = false;
	// add new pilots
	wchar_t pilotName[255];
	file.seekBlock("Pilots");
	for (i = 0; i < 255; i++)
	{
		sprintf(tmp, "Pilot%ld", i);
		if (NO_ERROR != file.readIdString(tmp, pilotName, 254))
			break;
		for (PILOT_LIST::EIterator pIter = pilots.Begin(); !pIter.IsDone(); pIter++)
		{
			if ((*pIter).getFileName().Compare(pilotName, 0) == 0)
			{
				(*pIter).setAvailable(true);
			}
		}
	}
	for (pIter = pilots.Begin(); !pIter.IsDone(); pIter++)
	{
		if ((*pIter).bAvailable)
			newPilotAvailableCount++;
	}
	if (oldPilotAvailableCount != newPilotAvailableCount && newPilotAvailableCount > oldPilotAvailableCount)
		bNewPilots = true;
	else
		bNewPilots = 0;
	return 0;
}

void LogisticsData::appendAvailability(std::wstring_view pFileName, bool* availableArray)
{
	FitIniFile file;
	if (NO_ERROR != file.open(pFileName))
	{
		return;
	}
	int32_t result = file.seekBlock("Components");
	if (result == NO_ERROR)
	{
		wchar_t tmp[32];
		int32_t component;
		bool bAll = 0;
		file.readIdBoolean("AllComponents", bAll);
		for (size_t i = 0; i < 255; i++)
		{
			{
				sprintf(tmp, "Component%ld", i);
				if (NO_ERROR != file.readIdLong(tmp, component))
					break;
				availableArray[component] = true;
				LogisticsComponent* pComp = getComponent(component);
				if (!pComp->isAvailable())
					bNewWeapons = true;
				pComp->setAvailable(true);
			}
		}
	}
	// add new pilots
	wchar_t pilotName[255];
	wchar_t tmp[256];
	file.seekBlock("Pilots");
	int32_t i;
	for (i = 0; i < 255; i++)
	{
		sprintf(tmp, "Pilot%ld", i);
		if (NO_ERROR != file.readIdString(tmp, pilotName, 254))
			break;
		for (PILOT_LIST::EIterator pIter = pilots.Begin(); !pIter.IsDone(); pIter++)
		{
			if ((*pIter).getFileName().Compare(pilotName, 0) == 0)
			{
				(*pIter).setAvailable(true);
				bNewPilots = true;
			}
		}
	}
	file.seekBlock("Mechs");
	int32_t newAvailableCount = 0;
	wchar_t chassisFileName[256];
	for (i = 0; i < 255; i++)
	{
		sprintf(tmp, "Mech%ld", i);
		if (NO_ERROR != file.readIdString(tmp, chassisFileName, 255))
			break;
		// go through each variant, if it has the same chassis, check and see if
		// all of its components are valid
		for (VARIANT_LIST::EIterator vIter = variants.Begin(); !vIter.IsDone(); vIter++)
		{
			std::wstring_view mechName = (*vIter)->getFileName();
			wchar_t realName[255];
			_splitpath(mechName, nullptr, nullptr, realName, nullptr);
			if (_stricmp(realName, chassisFileName) == 0)
			{
				int32_t componentCount = 255;
				int32_t componentArray[256];
				bool bRight = true;
				(*vIter)->getComponents(componentCount, componentArray);
				for (size_t i = 0; i < componentCount; i++)
				{
					LogisticsComponent* pComp = getComponent(componentArray[i]);
					if (!pComp->isAvailable()) // unavailable componets
					{
						// wchar_t errorStr[256];
						// sprintf( errorStr, "mech %s discarded because it
						// contains a %ld", 	chassisFileName, componentArray[i]
						//);  PAUSE(( errorStr ));
						bRight = false;
						break;
					}
				}
				if (bRight)
				{
					(*vIter)->setAvailable(true);
					newAvailableCount++;
				}
				break;
			}
		}
	}
}

std::wstring_view
LogisticsData::getCurrentMission(void) const
{
	return missionInfo->getCurrentMission();
}

std::wstring_view
LogisticsData::getLastMission(void) const
{
	return missionInfo->getLastMission();
}

std::wstring_view
LogisticsData::getCurrentABLScript(void) const
{
	return missionInfo->getCurrentABLScriptName();
}

int32_t
LogisticsData::getCurrentMissionTune()
{
	return missionInfo->getCurrentLogisticsTuneId();
}

int32_t
LogisticsData::getCurrentMissionId()
{
	return missionInfo->getCurrentMissionId();
}

void LogisticsData::clearInventory()
{
	for (MECH_LIST::EIterator iter = inventory.Begin(); !iter.IsDone(); iter++)
	{
		(*iter)->setPilot(nullptr);
		delete *iter;
	}
	inventory.Clear();
}

int32_t
LogisticsData::getPilotCount()
{
	return pilots.Count();
}
int32_t
LogisticsData::getPilots(LogisticsPilot** pArray, int32_t& count)
{
	if (count < pilots.Count())
	{
		return NEED_BIGGER_ARRAY;
	}
	count = 0;
	for (PILOT_LIST::EIterator iter = instance->pilots.Begin(); !iter.IsDone(); iter++)
	{
		pArray[count++] = &(*iter);
	}
	return 0;
}

int32_t
LogisticsData::getMaxDropWeight(void) const
{
	return missionInfo->getCurrentDropWeight();
}

int32_t
LogisticsData::getCurrentDropWeight(void) const
{
	int32_t retVal = 0;
	for (MECH_LIST::EIterator iter = instance->inventory.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getForceGroup())
		{
			retVal += (*iter)->getMaxWeight();
		}
	}
	return retVal;
}

bool LogisticsData::canAddMechToForceGroup(LogisticsMech* pMech)
{
	if (!pMech)
		return 0;
	int32_t maxUnits = 12;
#ifndef VIEWER
	if (MPlayer)
	{
		int32_t playerCount;
		MPlayer->getPlayers(playerCount);
		maxUnits = MAX_MULTIPLAYER_MECHS_IN_LOGISTICS / playerCount;
		if (maxUnits > 12)
			maxUnits = 12;
	}
#endif
	int32_t fgCount = 0;
	for (MECH_LIST::EIterator iter = instance->inventory.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getForceGroup())
		{
			fgCount++;
		}
	}
	if (fgCount >= maxUnits)
		return 0;
	return (pMech->getMaxWeight() + getCurrentDropWeight() <= getMaxDropWeight()) ? 1 : 0;
}

int32_t
LogisticsData::getVariantsInInventory(LogisticsVariant* pVar, bool bIncludeForceGroup)
{
	int32_t retVal = 0;
	for (MECH_LIST::EIterator iter = instance->inventory.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getVariant() == pVar)
		{
			if (!(*iter)->getForceGroup() || bIncludeForceGroup)
			{
				retVal++;
			}
		}
	}
	return retVal;
}

int32_t
LogisticsData::getChassisVariants(
	const LogisticsChassis* pChassis, LogisticsVariant** pVar, int32_t& maxCount)
{
	int32_t retVal = 0;
	int32_t i = 0;
	for (VARIANT_LIST::EIterator iter = variants.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getChassis() == pChassis)
		{
			if (i < maxCount)
				pVar[i] = (*iter);
			else
				retVal = NEED_BIGGER_ARRAY;
			++i;
		}
	}
	maxCount = i;
	return retVal;
}

int32_t
LogisticsData::setMechToModify(LogisticsMech* pMech)
{
	if (!pMech)
		return -1;
	currentlyModifiedMech = pMech;
	oldVariant = pMech->getVariant();
	LogisticsVariant* pVar = new LogisticsVariant(*oldVariant);
	pMech->setVariant(pVar);
	return 0;
}

void encryptFile(std::wstring_view inputFile, std::wstring_view outputFile)
{
	// Now we encrypt this by zlib Compressing the file passed in.
	// Then LZ Compressing the resulting zlib data.
	// Since our LZ compression is pretty much non-standard, that should be
	// enough.
	uint8_t* rawData = nullptr;
	uint8_t* zlibData = nullptr;
	uint8_t* LZData = nullptr;
	File dataFile;
	dataFile.open(inputFile);
	uint32_t fileSize = dataFile.fileSize();
	rawData = (uint8_t*)malloc(fileSize);
	zlibData = (uint8_t*)malloc(fileSize * 2);
	LZData = (uint8_t*)malloc(fileSize * 2);
	dataFile.read(rawData, fileSize);
	uint32_t zlibSize = fileSize * 2;
	compress2(zlibData, &zlibSize, rawData, fileSize, 0);
	uint32_t lzSize = LZCompress(LZData, zlibData, zlibSize);
	dataFile.close();
	File binFile;
	binFile.create(outputFile);
	binFile.writeLong(lzSize);
	binFile.writeLong(zlibSize);
	binFile.writeLong(fileSize);
	binFile.write(LZData, lzSize);
	binFile.close();
	free(rawData);
	free(zlibData);
	free(LZData);
}

void decryptFile(std::wstring_view inputFile, std::wstring_view outputFile)
{
	// Now we decrypt this by lz deCompressing the zlib file created.
	// Then zlib deCompressing the resulting zlib data into the raw File again.
	// Since our LZ compression is pretty much non-standard, that should be
	// enough.
	uint8_t* rawData = nullptr;
	uint8_t* zlibData = nullptr;
	uint8_t* LZData = nullptr;
	File dataFile;
	int32_t result = dataFile.open(inputFile);
	if (result == NO_ERROR)
	{
		uint32_t lzSize = dataFile.readLong();
		uint32_t zlibSize = dataFile.readLong();
		uint32_t fileSize = dataFile.readLong();
		rawData = (uint8_t*)malloc(fileSize);
		zlibData = (uint8_t*)malloc(zlibSize);
		LZData = (uint8_t*)malloc(lzSize);
		dataFile.read(LZData, lzSize);
		uint32_t testSize = fileSize;
		uint32_t test2Size = LZDecomp(zlibData, LZData, lzSize);
		if (test2Size != zlibSize)
			STOP(("Didn't Decompress to same size as started with!!"));
		uncompress((uint8_t*)rawData, &testSize, zlibData, zlibSize);
		if (testSize != fileSize)
			STOP(("Didn't Decompress to correct format"));
		dataFile.close();
		File binFile;
		binFile.create(outputFile);
		binFile.write(rawData, fileSize);
		binFile.close();
		free(rawData);
		free(zlibData);
		free(LZData);
	}
}

int32_t
LogisticsData::acceptMechModifications(std::wstring_view name)
{
	if (!currentlyModifiedMech)
		return -1;
	bool bFound = 0;
	if (oldVariant)
	{
		missionInfo->incrementCBills(oldVariant->getCost());
	}
	if (oldVariant && oldVariant->isDesignerMech())
	{
		bFound = 1;
	}
	else
	{
		for (MECH_LIST::EIterator iter = inventory.Begin(); !iter.IsDone(); iter++)
		{
			if ((*iter)->getVariant() == oldVariant && (*iter) != currentlyModifiedMech)
			{
				bFound = 1;
			}
		}
	}
	if (!bFound)
	{
		VARIANT_LIST::EIterator vIter = variants.Find(oldVariant);
		if (vIter != VARIANT_LIST::INVALID_ITERATOR && oldVariant->getName().Compare(name) == 0)
		{
			variants.Delete(vIter);
			delete oldVariant;
		}
	}
	// now need to get rid of variants with this name....
	// Code added by Frank on  May 3, 2001 @ 9:54pm.
	// If you run this with the compare set to vIter.IsDone, it crashes in
	// Profile based on Sean Bug number 4359.  We traverse past end of list and
	// crash. Doing it by count does not crash and has the added advantage of
	// being easy to debug! I suspect ESI going south again.  Probably a
	// compiler option... 05/04 HKG, actually, if you increment vIter after
	// deleting it, it still won't work Good Point.  As you can see, it was
	// pretty late when I "fixed" this!
	// int32_t numVariants = variants.Count();
	// int32_t i=0;
	for (VARIANT_LIST::EIterator vIter = variants.Begin(); !vIter.IsDone();)
	{
		if ((*vIter)->getName().Compare(name, 0) == 0)
		{
			VARIANT_LIST::EIterator tmpIter = vIter;
			vIter++;
			delete (*tmpIter);
			variants.Delete(tmpIter);
		}
		else
			vIter++;
	}
	currentlyModifiedMech->getVariant()->setName(name);
	variants.Append(currentlyModifiedMech->getVariant());
	missionInfo->decrementCBills(currentlyModifiedMech->getVariant()->getCost());
	currentlyModifiedMech = 0;
	oldVariant = 0;
	// temporary, looking for dangling pointers
	for (MECH_LIST::EIterator iter = inventory.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getVariant()->getCost())
		{
			bFound = 1;
		}
	}
#ifndef VIEWER
	if (MPlayer)
	{
		// save the player created variants
		for (VARIANT_LIST::EIterator vIter = variants.Begin(); !vIter.IsDone(); vIter++)
		{
			if (!(*vIter)->isDesignerMech())
			{
				FullPathFileName mechFile;
				mechFile.init("data\\multiplayer\\", (*vIter)->getName(), ".var");
				FitIniFile file;
				file.create(mechFile);
				(*vIter)->save(file, 0);
				file.close();
				encryptFile(mechFile, mechFile);
			}
		}
	}
#endif
	return 0;
}
int32_t
LogisticsData::acceptMechModificationsUseOldVariant(std::wstring_view name)
{
	if (!currentlyModifiedMech)
		return -1;
	if (oldVariant)
	{
		missionInfo->incrementCBills(oldVariant->getCost());
	}
	LogisticsVariant* pVar = getVariant(name);
	if (!pVar)
		Assert(0, 0, "couldn't find the old variant\n");
	LogisticsVariant* pOldVar = currentlyModifiedMech->getVariant();
	delete pOldVar;
	currentlyModifiedMech->setVariant(pVar);
	missionInfo->decrementCBills(pVar->getCost());
	currentlyModifiedMech = 0;
	oldVariant = 0;
	return 0;
}

bool LogisticsData::canReplaceVariant(std::wstring_view name)
{
	int32_t nameCount = 0;
	for (MECH_LIST::EIterator iter = inventory.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getName().Compare(name, 0) == 0)
		{
			nameCount++;
			if ((*iter)->getVariant() != oldVariant && (*iter) != currentlyModifiedMech)
			{
				return 0;
			}
		}
	}
	if (nameCount > 1)
		return 0;
	for (VARIANT_LIST::EIterator vIter = variants.Begin(); !vIter.IsDone(); vIter++)
	{
		if ((*vIter)->isDesignerMech() && (*vIter)->getName().Compare(name, 0) == 0)
			return 0;
	}
	return true;
}

bool LogisticsData::canDeleteVariant(std::wstring_view name)
{
	LogisticsVariant* pVariant = getVariant(name);
	if (!pVariant)
		return 0;
	if (!canReplaceVariant(name))
		return 0;
	if (currentlyModifiedMech->getName() == name || oldVariant->getName() == name)
		return 0;
	return 1;
}

int32_t
LogisticsData::cancelMechModfications()
{
	if (!currentlyModifiedMech)
		return -1;
	LogisticsVariant* pCancel = currentlyModifiedMech->getVariant();
	delete pCancel;
	currentlyModifiedMech->setVariant(oldVariant);
	oldVariant = 0;
	currentlyModifiedMech = 0;
	return 0;
}

std::wstring_view
LogisticsData::getCurrentOperationFileName()
{
	return missionInfo->getCurrentOperationFile();
}
std::wstring_view
LogisticsData::getCurrentVideoFileName()
{
	return missionInfo->getCurrentVideo();
}

std::wstring_view
LogisticsData::getCurrentMissionDescription()
{
	return missionInfo->getCurrentMissionDescription();
}

std::wstring_view
LogisticsData::getCurrentMissionFriendlyName()
{
	return missionInfo->getCurrentMissionFriendlyName();
}

std::wstring_view
LogisticsData::getMissionFriendlyName(std::wstring_view missionName)
{
	return missionInfo->getMissionFriendlyName(missionName);
}

/*int32_t				LogisticsData::getMaxTeams(void) const
{
/	return missionInfo->getMaxTeams( );
}*/

void LogisticsData::startNewCampaign(std::wstring_view fileName)
{
#ifndef VIEWER
	if (MPlayer)
	{
		delete MPlayer;
		MPlayer = nullptr;
	}
#endif
	inventory.Clear();
	resourcePoints = 0;
	pilots.Clear();
	initPilots();
	FitIniFile file;
	FullPathFileName path;
	path.init(campaignPath, fileName, ".fit");
	if (NO_ERROR != file.open(path))
	{
		STOP(("COuld not find file %s to load campaign", path));
	}
	missionInfo->init(file);
	// temporary, just so we can test
	int32_t count = 32;
	std::wstring_view missionNames[32];
	missionInfo->getAvailableMissions(missionNames, count);
	setCurrentMission(missionNames[0]);
	soundSystem->setMusicVolume(prefs.MusicVolume);
	soundSystem->playDigitalMusic(missionInfo->getCurrentLogisticsTuneId());
}

void LogisticsData::startMultiPlayer()
{
	inventory.Clear();
	resourcePoints = 0;
	pilots.Clear();
	initPilots();
	// kill all old designer mechs
	clearVariants();
	// need to initialize multiplayer variants here...
	wchar_t findString[512];
	sprintf(findString, "data\\multiplayer\\*.var");
	WIN32_FIND_DATA findResult;
	HANDLE searchHandle = FindFirstFile(findString, &findResult);
	if (searchHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((findResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				FullPathFileName path;
				path.init("data\\multiplayer\\", findResult.cFileName, "");
				decryptFile(path, "tmp.fit");
				FitIniFile file;
				file.open("tmp.fit");
				file.seekBlock("Variant0");
				loadVariant(file);
				file.close();
				DeleteFile("tmp.fit");
			}
		} while (FindNextFile(searchHandle, &findResult) != 0);
		FindClose(searchHandle);
	}
	missionInfo->setMultiplayer();
#ifndef VIEWER
	if (!MPlayer)
	{
		MPlayer = new MultiPlayer;
		MPlayer->setup();
		if (!strlen(&prefs.playerName[0][0]))
		{
			cLoadString(IDS_UNNAMED_PLAYER, &prefs.playerName[0][0], 255);
		}
		ChatWindow::init();
	}
#endif
}
void LogisticsData::setPurchaseFile(std::wstring_view fileName)
{
	missionInfo->setPurchaseFile(fileName);
	if (MPlayer)
		clearInventory();
	updateAvailability();
}

int32_t
LogisticsData::getCBills()
{
	return missionInfo->getCBills();
}
void LogisticsData::addCBills(int32_t amount)
{
	missionInfo->incrementCBills(amount);
}
void LogisticsData::decrementCBills(int32_t amount)
{
	missionInfo->decrementCBills(amount);
}

int32_t
LogisticsData::getPlayerVariantNames(std::wstring_view* array, int32_t& count)
{
	int32_t maxCount = count;
	count = 0;
	int32_t retVal = 0;
	for (VARIANT_LIST::EIterator iter = variants.Begin(); !iter.IsDone(); iter++)
	{
		if (!(*iter)->isDesignerMech())
		{
			if (count < maxCount)
			{
				array[count] = (*iter)->getName();
			}
			else
			{
				retVal = NEED_BIGGER_ARRAY;
			}
			count++;
		}
	}
	return retVal;
}

int32_t
LogisticsData::getEncyclopediaMechs(const LogisticsVariant** pChassis, int32_t& count)
{
	int32_t retVal = 0;
	int32_t maxCount = count;
	count = 0;
	for (VARIANT_LIST::EIterator vIter = variants.Begin(); !vIter.IsDone(); vIter++)
	{
		if ((*vIter)->getName().Find("Prime") != -1 && (*vIter)->isDesignerMech())
		{
			if (count < maxCount)
				pChassis[count] = (*vIter);
			else
				retVal = NEED_BIGGER_ARRAY;
			count++;
		}
	}
	return retVal;
}

int32_t
LogisticsData::getHelicopters(const LogisticsVariant** pChassis, int32_t& count)
{
	int32_t retVal = 0;
	int32_t maxCount = count;
	count = 0;
	for (VARIANT_LIST::EIterator vIter = variants.Begin(); !vIter.IsDone(); vIter++)
	{
		if ((((*vIter)->getVariantID() >> 16 & 0xff) == 0) && (*vIter)->getName().Find("Prime") == -1 && (*vIter)->isDesignerMech())
		{
			if (count < maxCount)
				pChassis[count] = (*vIter);
			else
				retVal = NEED_BIGGER_ARRAY;
			count++;
		}
	}
	return retVal;
}

int32_t
LogisticsData::getVehicles(const LogisticsVehicle** pChassis, int32_t& count)
{
	int32_t retVal = 0;
	int32_t maxCount = count;
	count = 0;
	for (VEHICLE_LIST::EIterator vIter = vehicles.Begin(); !vIter.IsDone(); vIter++)
	{
		if (count < maxCount)
			pChassis[count] = (*vIter);
		else
			retVal = NEED_BIGGER_ARRAY;
		count++;
	}
	return retVal;
}

LogisticsVehicle*
LogisticsData::getVehicle(std::wstring_view pName)
{
	wchar_t tmpStr[256];
	for (VEHICLE_LIST::EIterator vIter = vehicles.Begin(); !vIter.IsDone(); vIter++)
	{
		cLoadString((*vIter)->getNameID(), tmpStr, 255);
		if (_stricmp(tmpStr, pName) == 0)
		{
			return *vIter;
		}
	}
	return nullptr;
}

int32_t
LogisticsData::addBuilding(int32_t fitID, PacketFile& objectFile, float scale)
{
	if (NO_ERROR != objectFile.seekPacket(fitID))
		return -1;
	int32_t fileSize = objectFile.getPacketSize();
	if (fileSize)
	{
		Building bldg;
		FitIniFile file;
		file.open(&objectFile, fileSize);
		if (NO_ERROR != file.seekBlock("ObjectType"))
			gosASSERT(0);
		file.readIdString("AppearanceName", bldg.fileName, 63);
		file.readIdLong("EncyclopediaID", bldg.encycloID);
		bool bIsTurret = 0;
		if (NO_ERROR != file.seekBlock("BuildingData"))
		{
			if (NO_ERROR != file.seekBlock("GateData"))
			{
				if (NO_ERROR != file.seekBlock("TurretData"))
				{
					if (NO_ERROR != file.seekBlock("General")) // hack for artillery piece
					{
						wchar_t errorStr[256];
						sprintf(
							errorStr, "coudn't find appropriate block in file %s", bldg.fileName);
						Assert(0, 0, errorStr);
					}
				}
				else
					bIsTurret = true;
			}
		}
		uint32_t tmp;
		file.readIdLong("BuildingName", bldg.nameID);
		file.readIdULong("DmgLevel", tmp);
		bldg.weight = tmp;
		if (bIsTurret)
		{
			wchar_t weaponNameStr[64];
			strcpy(weaponNameStr, "WeaponType");
			for (size_t i = 0; i < 4; i++)
			{
				file.readIdLong(weaponNameStr, bldg.componentIDs[i]);
				sprintf(weaponNameStr, "WeaponType%ld", i + 1);
			}
		}
		else
		{
			for (size_t i = 0; i < 4; i++)
			{
				bldg.componentIDs[i] = 0;
			}
		}
		bldg.scale = scale;
		buildings.Append(bldg);
	}
	return 0;
}

LogisticsComponent*
LogisticsData::getComponent(int32_t componentID)
{
	for (COMPONENT_LIST::EIterator iter = components.Begin(); !iter.IsDone(); iter++)
	{
		if (((*iter).getID() & 0x000000ff) == (componentID & 0x000000ff))
			return &(*iter);
	}
	return nullptr;
}

LogisticsData::Building*
LogisticsData::getBuilding(int32_t nameID)
{
	for (BUILDING_LIST::EIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter).nameID == nameID)
			return &(*iter);
	}
	return nullptr;
}

int32_t
LogisticsData::getBuildings(Building** bdgs, int32_t& count)
{
	int32_t maxCount = count;
	count = 0;
	int32_t retVal = 0;
	for (BUILDING_LIST::EIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
	{
		if (count < maxCount)
		{
			bdgs[count] = &(*iter);
		}
		else
			retVal = NEED_BIGGER_ARRAY;
		count++;
	}
	return retVal;
}

std::wstring_view
LogisticsData::getCampaignName(void) const
{
	return missionInfo->getCampaignName();
}

bool LogisticsData::campaignOver()
{
	return missionInfo->campaignOver();
}
std::wstring_view
LogisticsData::getCurrentBigVideo(void) const
{
	return missionInfo->getCurrentBigVideo();
}
std::wstring_view
LogisticsData::getFinalVideo(void) const
{
	return missionInfo->getFinalVideo();
}

void LogisticsData::addNewBonusPurchaseFile(std::wstring_view pFileName)
{
	missionInfo->addBonusPurchaseFile(pFileName);
}

bool LogisticsData::skipLogistics()
{
	return missionInfo->skipLogistics();
}
bool LogisticsData::skipPilotReview()
{
	return missionInfo->skipPilotReview();
}
bool LogisticsData::skipSalvageScreen()
{
	return missionInfo->skipSalvageScreen();
}
bool LogisticsData::skipPurchasing()
{
	return missionInfo->skipPurchasing();
}

bool LogisticsData::showChooseMission()
{
	return missionInfo->showChooseMission();
}

void LogisticsData::setSingleMission(std::wstring_view pName)
{
	missionInfo->setSingleMission(pName);
	clearVariants();
	initPilots(); // reset pilotsb
	clearInventory();
	updateAvailability();
}

bool LogisticsData::isSingleMission()
{
	if (missionInfo)
	{
		return missionInfo->isSingleMission();
	}
	return 0;
}

bool LogisticsData::canHaveSalavageCraft()
{
	if (!missionInfo)
		return true;
	return missionInfo->canHaveSalavageCraft();
}
bool LogisticsData::canHaveRepairTruck()
{
	if (!missionInfo)
		return true;
	return missionInfo->canHaveRepairTruck();
}
bool LogisticsData::canHaveScoutCopter()
{
	if (!missionInfo)
		return true;
	return missionInfo->canHaveScoutCopter();
}
bool LogisticsData::canHaveArtilleryPiece()
{
	if (!missionInfo)
		return true;
	return missionInfo->canHaveArtilleryPiece();
}
bool LogisticsData::canHaveAirStrike()
{
	if (!missionInfo)
		return true;
	return missionInfo->canHaveAirStrike();
}
bool LogisticsData::canHaveSensorStrike()
{
	if (!missionInfo)
		return true;
	return missionInfo->canHaveSensorStrike();
}
bool LogisticsData::canHaveMineLayer()
{
	if (!missionInfo)
		return true;
	return missionInfo->canHaveMineLayer();
}

bool LogisticsData::getVideoShown()
{
	if (!missionInfo)
		return true;
	return missionInfo->getVideoShown();
}
void LogisticsData::setVideoShown()
{
	if (missionInfo)
		missionInfo->setVideoShown();
}
void LogisticsData::setPilotUnused(std::wstring_view pName)
{
	for (PILOT_LIST::EIterator iter = pilots.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter).getFileName().Compare(pName, 0) == 0)
		{
			(*iter).setUsed(0);
			break;
		}
	}
}

void LogisticsData::setCurrentMissionNum(int32_t cMission)
{
	missionInfo->setCurrentMissionNumber(cMission);
}

int32_t
LogisticsData::getCurrentMissionNum(void)
{
	return missionInfo->getCurrentMissionNumber();
}

// end of file ( LogisticsData.cpp )
