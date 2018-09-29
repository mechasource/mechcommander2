/*************************************************************************************************\
LogisticsVariant.cpp			: Implementation of the LogisticsVariant
component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#define LOGISTICSVARIANT_CPP

#include "stdinc.h"
#include "logisticsvariant.h"
#include <mclib.h>
#include "logisticsdata.h"
#include "..\resource.h"
#include "cmponent.h"
#include "logisticserrors.h"

int32_t LogisticsChassis::weightClasses[5] = {0, 35, 55, 75, 100};
/*[0- 34	Light
35-55		Medium
56-75		Heavy
76-100		Assualt
*/

LogisticsChassis::LogisticsChassis()
{
	appearance		   = 0;
	chassisNameID	  = 0;
	baseCost		   = 0;
	maxWeight		   = 0;
	refCount		   = 0;
	canHaveActiveProbe = canHaveAdvSensor = canHaveExtendedSensor = canHaveECM =
		canHaveGlobalSensor = canHaveOptics = canHaveJumpJets = 0;
}

int32_t LogisticsChassis::init(CSVFile* file, int32_t chassisID)
{
	ID = chassisID;
	char buffer[256];
	fileName = file->getFilename();
	fileName.MakeLower();
	int32_t result = file->readLong(11, 5, baseCost);
	gosASSERT(result == NO_ERROR);
	result = file->readLong(5, 2, chassisNameID);
	gosASSERT(result == NO_ERROR);
	result = file->readFloat(3, 5, maxWeight);
	gosASSERT(result == NO_ERROR);
	result = file->readLong(12, 5, maxArmor);
	gosASSERT(result == NO_ERROR);
	result = file->readLong(8, 2, helpID);
	gosASSERT(result == NO_ERROR);
	result = file->readLong(6, 2, encyclopediaID);
	gosASSERT(result == NO_ERROR);
	file->readLong(12, 2, iconPictureIndex);
	gosASSERT(result == NO_ERROR);
	houseID;
	result = file->readLong(4, 2, houseID);
	ID |= ((houseID << 8) & 0x0000ff00);
	gosASSERT(result == NO_ERROR);
	for (size_t row = 9; row < 12; ++row)
	{
		result = file->readString(row, 2, buffer, 256);
		gosASSERT(result == NO_ERROR);
		iconFileNames[row - 9] = buffer;
	}
	result = file->readBoolean(3, 8, canHaveAdvSensor);
	gosASSERT(result == NO_ERROR);
	result = file->readBoolean(4, 8, canHaveOptics);
	gosASSERT(result == NO_ERROR);
	result = file->readBoolean(5, 8, canHaveGlobalSensor);
	gosASSERT(result == NO_ERROR);
	result = file->readBoolean(6, 8, canHaveECM);
	gosASSERT(result == NO_ERROR);
	result = file->readBoolean(7, 8, canHaveActiveProbe);
	gosASSERT(result == NO_ERROR);
	result = file->readBoolean(8, 8, canHaveJumpJets);
	gosASSERT(result == NO_ERROR);
	result = file->readBoolean(9, 8, canHaveExtendedSensor);
	gosASSERT(result == NO_ERROR);
	result = file->readFloat(7, 5, speed);
	gosASSERT(result == NO_ERROR);
	result = file->readLong(4, 5, maxHeat);
	gosASSERT(result == NO_ERROR);
	result = file->readLong(6, 5, baseArmor);
	gosASSERT(result == NO_ERROR);
	result = file->readLong(13, 2, componentAreaWidth);
	gosASSERT(result == NO_ERROR);
	result = file->readLong(14, 2, componentAreaHeight);
	gosASSERT(result == NO_ERROR);
	int32_t ID = IDS_VERY_LIGHT;
	for (size_t i = 4; i > -1; --i)
	{
		if (maxWeight > weightClasses[i])
		{
			ID = IDS_VERY_LIGHT + i + 1;
			break;
		}
	}
	char tmpWeightClass[256];
	cLoadString(ID, tmpWeightClass, 256);
	mechClass = tmpWeightClass;
	return 0;
}

int32_t LogisticsChassis::getArmorClass(void) const
{
	if (baseArmor < 96)
		return IDS_ARMOR0;
	else if (baseArmor < 113)
		return IDS_ARMOR1;
	else if (baseArmor < 195)
		return IDS_ARMOR2;
	else if (baseArmor < 304)
		return IDS_ARMOR3;
	else
		return IDS_ARMOR4;
}

int32_t LogisticsChassis::getDisplaySpeed(void) const
{
	switch ((int32_t)speed)
	{
	case 5:
		return 17;
		break;
	case 10:
		return 34;
		break;
	case 15:
		return 54;
		break;
	case 19:
		return 65;
		break;
	case 23:
		return 86;
		break;
	case 27:
		return 97;
		break;
	case 31:
		return 119;
		break;
	case 35:
		return 130;
		break;
	}
	return -1;
}

int32_t LogisticsChassis::getSensorID(void) const
{
	if (canHaveAdvSensor)
		return 15;
	else if (canHaveExtendedSensor)
		return 14;
	else if (canHaveGlobalSensor)
		return 17;
	else
		return -1;
	return -1;
}

int32_t LogisticsChassis::getECM(void) const
{
	if (canHaveECM)
		return 38;
	return -1;
}

LogisticsVariant::LogisticsVariant()
{
	componentCount  = 0;
	ID				= -1;
	chassis			= 0;
	availableToUser = 0;
	memset(components, 0, sizeof(components));
	bDesignerMech = 0;
	bHidden		  = 0;
	fileID		  = 0;
}

LogisticsVariant::LogisticsVariant(const LogisticsChassis* pChassis, bool Designer)
{
	chassis			= const_cast<LogisticsChassis*>(pChassis);
	availableToUser = 1;
	componentCount  = 0;
	ID				= (pChassis->refCount << 16) | pChassis->ID;
	chassis->refCount++;
	bDesignerMech = Designer;
	bHidden		  = 0;
	fileID		  = 0;
}
LogisticsVariant::~LogisticsVariant()
{
	chassis->refCount--;
	if (chassis->refCount == 0)
		delete chassis;
	chassis = nullptr;
}

LogisticsVariant::LogisticsVariant(const LogisticsVariant& src)
{
	chassis			= src.chassis;
	availableToUser = src.availableToUser;
	componentCount  = src.componentCount;
	for (size_t i = 0; i < componentCount; ++i)
	{
		components[i] = src.components[i];
	}
	variantName = src.variantName;
	ID			= (chassis->refCount << 16) | chassis->ID;
	chassis->refCount++;
	bDesignerMech = 0;
	bHidden		  = 0;
	fileID		  = src.fileID;
}

LogisticsVariant& LogisticsVariant::operator=(const LogisticsVariant& src)
{
	if (&src != this)
	{
		chassis			= src.chassis;
		availableToUser = src.availableToUser;
		componentCount  = src.componentCount;
		for (size_t i = 0; i < componentCount; ++i)
		{
			components[i] = src.components[i];
		}
		variantName = src.variantName;
		ID			= (chassis->refCount << 16) | chassis->ID;
		chassis->refCount++;
		bDesignerMech = 0;
		fileID		  = src.fileID;
	}
	return *this;
}

bool LogisticsVariant::operator==(const LogisticsVariant& src)
{
	if (chassis->chassisNameID != src.chassis->chassisNameID)
		return 0;
	if (availableToUser != src.availableToUser)
		return 0;
	if (componentCount != src.componentCount)
		return 0;
	for (size_t i = 0; i < componentCount; ++i)
	{
		if (components[i].component->getID() != src.components[i].component->getID())
		{
			return 0;
		}
	}
	//		if ( variantName != src.variantName )
	//			return 0;
	return 1;
}

int32_t LogisticsVariant::init(CSVFile* file, LogisticsChassis* pChassis, int32_t Variant)
{
	bDesignerMech  = true;
	int32_t offset = 97 * Variant;
	fileID		   = Variant;
	gosASSERT(pChassis);
	chassis = pChassis;
	ID		= (pChassis->refCount << 16) | pChassis->ID;

	pChassis->refCount++;
	file->readBoolean(21 + offset, 4, bHidden);
	char buffer[256];
	if (NO_ERROR == file->readString(23 + offset, 2, buffer, 256))
	{
		variantName = buffer;
	}
	LogisticsComponent* pComps[128];
	int32_t xLocs[128];
	int32_t yLocs[128];
	memset(pComps, 0, sizeof(LogisticsComponent*) * 128);
	int32_t componentCount = 0;
	int32_t i, j, k;
	for (i = 26; i < 97; i++)
	{
		int32_t componentID;
		if (NO_ERROR == file->readLong(offset + i, 5, componentID) && componentID != 0xff)
		{
			LogisticsComponent* pComp = LogisticsData::instance->getComponent(componentID);
			if (pComp)
			{
				file->readLong(offset + i, 4, yLocs[componentCount]);
				file->readLong(offset + i, 3, xLocs[componentCount]);
				pComps[componentCount++] = pComp;
			}
		}
	}
	for (j = 4; j > 0; j--)
	{
		for (k = 3; k > 0; k--)
		{
			for (i = 0; i < componentCount; i++)
			{
				LogisticsComponent* pComponent = pComps[i];
				if (pComponent && pComponent->getComponentWidth() == k &&
					pComponent->getComponentHeight() == j)
				{
					if (!addComponent(pComps[i]->getID(), xLocs[i], yLocs[i]))
					{
						char errorString[256];
						sprintf(
							errorString, "Couldn't add component with id %ld", pComponent->getID());
					}
				}
			}
		}
	}
	return 0;
}

bool LogisticsVariant::addComponent(int32_t idFromFitFile, int32_t& x, int32_t& y)
{
	LogisticsComponent* pComponent = LogisticsData::instance->getComponent(idFromFitFile);
	if (!pComponent)
	{
		//	Assert( 0, idFromFitFile, "couldn't find the component in the fit
		// file\n" );
		delete pComponent;
		return false;
	}
	if (pComponent->getType() == COMPONENT_FORM_JUMPJET)
	{
		components[componentCount].location = LEGS;
		x = y = -2;
	}
	else if (pComponent->getType() == COMPONENT_FORM_ECM ||
		pComponent->getType() == COMPONENT_FORM_SENSOR)
	{
		components[componentCount].location = HEAD;
		x = y = -3;
	}
	else
	{
		// need to see if this thing will fit
		int32_t componentWidth  = pComponent->getComponentWidth();
		int32_t componentHeight = pComponent->getComponentHeight();
		if (x == -1 && y == -1)
		{
			for (size_t j = 0; j < chassis->componentAreaHeight && x == -1; j++)
			{
				for (size_t i = 0; i < chassis->componentAreaWidth && x == -1; i++)
				{
					bool bAdd = true;
					for (size_t l = 0; l < componentHeight; ++l)
					{
						for (size_t k = 0; k < componentWidth; ++k)
						{
							if (getComponentAtLocation(i + k, j + l) ||
								(i + k >= chassis->componentAreaWidth) ||
								(j + l >= chassis->componentAreaHeight))
							{
								bAdd = false;
								break;
								break;
							}
						}
					}
					if (bAdd)
					{
						x = i;
						y = j;
					}
				}
			}
		}
		if (x == -1 && y == -1)
			return 0;
		if (x > -1)
		{
			for (size_t i = 0; i < componentWidth; ++i)
			{
				for (size_t j = 0; j < componentHeight; ++j)
				{
					if (getComponentAtLocation(x + i, y + j))
					{
						char errorString[1024];
						sprintf(errorString,
							"couldn't add component %s to variant %s because "
							"another object was in the specified location",
							pComponent->getName(), this->getName());
						//				Assert( 0, 0, errorString );
						return false;
					}
				}
			}
		}
		components[componentCount].location = CHEST;
	}
	components[componentCount].component = pComponent;
	components[componentCount].xCoord	= x;
	components[componentCount].yCoord	= y;
	componentCount++;
	return true;
}

int32_t LogisticsVariant::getCost() const
{
	int32_t cost = chassis->baseCost;
	for (size_t i = 0; i < componentCount; ++i)
	{
		cost += components[i].component->getCost();
	}
	return cost;
}

int32_t LogisticsVariant::getWeight(void) const
{
	// Yup.  20% of base weight Plus 7 tons is used by armor and misc components
	// which we do not explicitly track.
	float baseWeight = (chassis->maxWeight * .20) + 7.0;
	for (size_t i = 0; i < componentCount; ++i)
	{
		baseWeight += components[i].component->getWeight();
	}
	return baseWeight;
}

const std::wstring& LogisticsVariant::getMechClass(void) const { return chassis->mechClass; }

int32_t LogisticsVariant::getMaxJumpRange(void) const { return 5; }

// BOGUS, this is entirely made up.  There is no real spec for jumpranges yet
int32_t LogisticsVariant::getJumpRange(void) const
{
	int32_t jumpJetCount = 0;
	for (size_t i = 0; i < componentCount; ++i)
	{
		if (components[i].component->getType() == COMPONENT_FORM_JUMPJET)
			jumpJetCount++;
	}
	if (jumpJetCount)
		return 5;
	return 0;
}

int32_t LogisticsVariant::getHeat(void) const
{
	int32_t retVal = 0;
	for (size_t i = 0; i < componentCount; ++i)
	{
		retVal += components[i].component->getHeat();
	}
	return retVal;
}

// BOGUS -- range isn't really the right thing to add...
int32_t LogisticsVariant::getArmor() const
{
	int32_t retArmor = chassis->baseArmor;
	for (size_t i = 0; i < componentCount; ++i)
	{
		if (components[i].component->getType() == COMPONENT_FORM_BULK)
		{
			retArmor += 32;
		}
	}
	return retArmor;
}

int32_t LogisticsVariant::getSpeed(void) const { return chassis->speed; }

int32_t LogisticsVariant::getDisplaySpeed(void) const { return chassis->getDisplaySpeed(); }
int32_t LogisticsVariant::getMaxHeat(void) const
{
	int32_t heat = 0;
	for (size_t i = 0; i < componentCount; i++)
	{
		if (components[i].component->getType() == COMPONENT_FORM_HEATSINK)
		{
			heat += components[i].component->getDamage();
		}
	}
	return heat + chassis->maxHeat;
}
// if you pass in -1's for x and y, we'll figure out where it can go, and return
// where it went
int32_t LogisticsVariant::canAddComponent(
	LogisticsComponent* pComponent, int32_t& x, int32_t& y) const
{
	/*  weight no longer matters
	int32_t weight = getWeight();
	if ( weight + pComponent->getWeight() > chassis->maxWeight )
		return COMPONENT_TOO_HEAVY;*/
	if (getHeat() + pComponent->getHeat() > getMaxHeat())
		return COMPONENT_TOO_HOT;
	if (pComponent->getType() == COMPONENT_FORM_JUMPJET)
	{
		if (!chassis->canHaveJumpJets)
			return JUMPJETS_NOT_ALLOWED;
		else if (hasJumpJets())
			return ONLY_ONE_JUMPJET_ALLOWED;
		else if ((x != -1 && x != -2) || (y != -1 && y != -2))
			return COMPONENT_SLOT_FULL;
		return 0;
	}
	else if (x == -2 && y == -2) // trying to put something illegal in jump jet slot
		return COMPONENT_SLOT_FULL;
	if (x != -1 && y != -1)
	{
		for (size_t i = 0; i < pComponent->getComponentWidth(); i++)
		{
			for (size_t j = 0; j < pComponent->getComponentHeight(); j++)
			{
				if (getComponentAtLocation(x + i, y + j) || x + i >= chassis->componentAreaWidth ||
					j + y >= chassis->componentAreaHeight)
					return COMPONENT_SLOT_FULL;
			}
		}
	}
	else
	{
		for (size_t j = 0; j < chassis->componentAreaHeight && x == -1; j++)
		{
			for (size_t i = 0; i < chassis->componentAreaWidth && x == -1; i++)
			{
				if (!getComponentAtLocation(i, j))
				{
					bool bAdd = true;
					for (size_t l = 0; l < pComponent->getComponentHeight(); ++l)
					{
						for (size_t k = 0; k < pComponent->getComponentWidth(); ++k)
						{
							if (getComponentAtLocation(i + k, j + l) ||
								(i + k >= chassis->componentAreaWidth) ||
								(j + l >= chassis->componentAreaHeight))
							{
								bAdd = false;
								break;
								break;
							}
						}
					}
					if (bAdd)
					{
						x = i;
						y = j;
					}
				}
			}
		}
		if (x == -1 || y == -1)
		{
			return ADD_COMPONENT_FAILED;
		}
	}
	if (pComponent->getType() == COMPONENT_FORM_BULK)
	{
		if (getArmor() + 32 > getMaxArmor())
			return NO_MORE_ARMOR;
	}
	if (pComponent->getType() == COMPONENT_FORM_SENSOR)
	{
		if (!chassis->canHaveAdvSensor)
			return SENSORS_NOT_ALLOWED;
		else if (hasSensor())
			return ONLY_ONE_SENSOR_ALLOWED;
	}
	if (pComponent->getType() == COMPONENT_FORM_ECM)
	{
		if (!chassis->canHaveECM)
			return ECM_NOT_ALLOWED;
		else if (hasECM())
			return ONLY_ONE_ECM_ALLOWED;
	}
	return 0;
}

const LogisticsChassis::ComponentInfo* LogisticsVariant::getComponentAtLocation(
	int32_t x, int32_t y) const
{
	if (x == -1 && y == -1)
		return nullptr;
	for (size_t i = 0; i < componentCount; ++i)
	{
		LogisticsComponent* pComponent = components[i].component;
		for (size_t j = 0; j < pComponent->getComponentHeight(); ++j)
		{
			for (size_t k = 0; k < pComponent->getComponentWidth(); ++k)
			{
				if (components[i].xCoord + k == x && components[i].yCoord + j == y)
				{
					return &components[i];
				}
			}
		}
	}
	return nullptr;
}

bool LogisticsVariant::hasJumpJets(void) const
{
	for (size_t i = 0; i < componentCount; i++)
	{
		if (components[i].component->getType() == COMPONENT_FORM_JUMPJET)
			return true;
	}
	return false;
}
bool LogisticsVariant::hasECM(void) const
{
	for (size_t i = 0; i < componentCount; i++)
	{
		if (components[i].component->getType() == COMPONENT_FORM_ECM)
			return true;
	}
	return false;
}
bool LogisticsVariant::hasSensor(void) const
{
	for (size_t i = 0; i < componentCount; i++)
	{
		if (components[i].component->getType() == COMPONENT_FORM_SENSOR)
			return true;
	}
	return false;
}

int32_t LogisticsVariant::getComponentsWithLocation(
	int32_t& count, int32_t* IDArray, int32_t* xLocationArray, int32_t* yLocationArray)
{
	if (count < componentCount)
		return NEED_BIGGER_ARRAY;
	for (size_t i = 0; i < componentCount; ++i)
	{
		IDArray[i]		  = components[i].component->getID();
		xLocationArray[i] = components[i].xCoord;
		yLocationArray[i] = components[i].yCoord;
	}
	count = componentCount;
	return 0;
}

int32_t LogisticsVariant::getComponents(int32_t& count, int32_t* array)
{
	for (size_t i = 0; i < componentCount; ++i)
	{
		array[i] = components[i].component->getID();
	}
	count = componentCount;
	return count;
}

int32_t LogisticsVariant::removeComponent(int32_t xCoord, int32_t yCoord)
{
	LogisticsChassis::ComponentInfo* info =
		const_cast<LogisticsChassis::ComponentInfo*>(getComponentAtLocation(xCoord, yCoord));
	if (!info)
		return -1;
	if (info->component && info->component->getType() == COMPONENT_FORM_HEATSINK)
	{
		if (getMaxHeat() - info->component->getDamage() < getHeat())
			return INSUFFICIENT_HEAT;
	}
	if (!info)
		return INVALID_LOCATION;
	for (size_t i = 0; i < componentCount; i++)
	{
		if (&components[i] == info)
		{
			memmove(&components[i], &components[i + 1],
				(componentCount - (i + 1)) * sizeof(LogisticsChassis::ComponentInfo));
			break;
		}
	}
	componentCount--;
	return 0;
}

// if you pass in -1's for x and y, we'll figure out where it can go, and return
// where it went
int32_t LogisticsVariant::addComponent(
	LogisticsComponent* pComponent, int32_t& xCoord, int32_t& yCoord)
{
	int32_t retVal = canAddComponent(pComponent, xCoord, yCoord);
	if (retVal == 0)
	{
		if (addComponent(pComponent->getID(), xCoord, yCoord))
			return 0;
		else
			return ADD_COMPONENT_FAILED;
	}
	return retVal;
}

int32_t LogisticsVariant::save(FitIniFile& file, int32_t counter)
{
	char tmp[256];
	char tmp2[256];
	sprintf(tmp, "Variant%ld", counter);
	file.writeBlock(tmp);
	file.writeIdString("Chassis", chassis->fileName);
	file.writeIdString("VariantName", variantName);
	file.writeIdLong("ComponentCount", componentCount);
	for (size_t i = 0; i < componentCount; i++)
	{
		sprintf(tmp, "Component%ld", i);
		file.writeIdLong(tmp, components[i].component->getID());
		strcpy(tmp2, tmp);
		strcat(tmp2, "x");
		file.writeIdLong(tmp2, components[i].xCoord);
		strcpy(tmp2, tmp);
		strcat(tmp2, "y");
		file.writeIdLong(tmp2, components[i].yCoord);
		strcpy(tmp2, tmp);
		strcat(tmp2, "Location");
		file.writeIdLong(tmp2, components[i].location);
	}
	return 0;
}

void LogisticsVariant::setName(PCSTR pName) { variantName = pName; }

int32_t LogisticsVariant::getComponents(int32_t& count, LogisticsComponent** array)
{
	if (count < componentCount)
		return NEED_BIGGER_ARRAY;
	for (size_t i = 0; i < componentCount; ++i)
	{
		array[i] = components[i].component;
	}
	count = componentCount;
	return 0;
}

LogisticsComponent* LogisticsVariant::getCompAtLocation(
	int32_t i, int32_t j, int32_t& realI, int32_t& realJ)
{
	realI = realJ								 = -1;
	const LogisticsChassis::ComponentInfo* pInfo = getComponentAtLocation(i, j);
	if (pInfo)
	{
		realI = pInfo->xCoord;
		realJ = pInfo->yCoord;
		return pInfo->component;
	}
	return 0;
}

int32_t LogisticsVariant::getComponentLocation(LogisticsComponent* pComp, int32_t& x, int32_t& y)
{
	const LogisticsChassis::ComponentInfo* pInfo = getComponentAtLocation(x, y);
	x = y = -1;
	if (pInfo)
	{
		x = pInfo->xCoord;
		y = pInfo->yCoord;
	}
	else
	{
		for (size_t i = 0; i < componentCount; i++)
		{
			if ((components[i].component) == pComp)
			{
				x = components[i].xCoord;
				y = components[i].yCoord;
				return 0;
			}
		}
		return COMPONENT_NOT_FOUND;
	}
	return 0;
}

int32_t LogisticsVariant::getOptimalRangeString(int32_t& color) const
{
	float rangeDamage[3];
	int32_t rangeColors[3] = {0xff6e7c00, 0xff005392, 0xffa21600};
	memset(rangeDamage, 0, sizeof(float) * 3);
	float maxDamage = -1.f;
	int32_t i;
	for (i = 0; i < componentCount; i++)
	{
		if (components[i].component->isWeapon())
		{
			float damageTimeRating = components[i].component->getDamage();
			if (components[i].component->getRecycleTime())
				damageTimeRating = components[i].component->getDamage() /
					components[i].component->getRecycleTime();
			rangeDamage[components[i].component->getRangeType()] += damageTimeRating;
			if (rangeDamage[components[i].component->getRangeType()] > maxDamage)
				maxDamage = rangeDamage[components[i].component->getRangeType()];
		}
	}
	for (i = 0; i < 3; i++)
	{
		if (rangeDamage[i] == maxDamage)
		{
			color = rangeColors[i];
			return IDS_HOTKEY1 + i;
		}
	}
	return IDS_HOTKEY1;
}

bool LogisticsVariant::allComponentsAvailable(void) const
{
	// 06/05/01 hack, we wan't to hide this atlas for the whole game.
	if (_stricmp("Renard's Atlas", variantName) == 0)
		return 0;
	for (size_t i = 0; i < componentCount; i++)
	{
		if (!components[i].component->isAvailable())
			return false;
	}
	return true;
}

void LogisticsVariant::setAvailable(bool available)
{
	// 06/05/01 hack, we wan't to hide this atlas for the whole game.
	if (_stricmp("Renard's Atlas", variantName) == 0)
		availableToUser = 0;
	else
		availableToUser = available;
}

void LogisticsVehicle::init(FitIniFile& file)
{
	componentCount = 0;
	file.seekBlock("ObjectType");
	char tmp[256];
	file.readIdString("AppearanceName", tmp, 255);
	fileName = tmp;
	file.seekBlock("General");
	file.readIdLong("DescIndex", chassisNameID);
	file.readIdFloat("CurTonnage", maxWeight);
	char tmpWeightClass[256];
	cLoadString(IDS_VEHICLE_CLASS, tmpWeightClass, 256);
	mechClass = tmpWeightClass;
	if (NO_ERROR != file.readIdLong("HouseID", houseID))
	{
		houseID = -1;
	}
	if (NO_ERROR != file.readIdLong("EncyclopediaID", encyclopediaID))
	{
		encyclopediaID = IDS_VEHICLE_DESCRIPTION_0;
	}
	file.seekBlock("VehicleDynamics");
	file.readIdFloat("MaxVelocity", speed);
	PSTR parts[5] = {"Front", "Left", "Right", "Rear", "Turret"};
	baseArmor	 = 0;
	uint8_t pts;
	int32_t i;
	for (i = 0; i < 5; i++)
	{
		file.seekBlock(parts[i]);
		file.readIdUChar("MaxArmorPoints", pts);
		baseArmor += pts;
		file.readIdUChar("CurInternalStructure", pts);
		baseArmor += pts;
	}
	file.seekBlock("InventoryInfo");
	file.readIdUChar("NumWeapons", pts);
	char blockName[256];
	for (i = 4; i < 4 + pts; i++)
	{
		sprintf(blockName, "Item:%ld", i);
		if (NO_ERROR == file.seekBlock(blockName))
		{
			uint8_t fitID;
			file.readIdUChar("MasterID", fitID);
			LogisticsComponent* pComponent = LogisticsData::instance->getComponent(fitID);
			if (pComponent)
			{
				components[componentCount].component = pComponent;
				components[componentCount].xCoord	= 0;
				components[componentCount].yCoord	= 0;
				componentCount++;
			}
		}
	}
}

int32_t LogisticsVehicle::getComponents(int32_t& count, LogisticsComponent** array)
{
	gosASSERT(componentCount <= count);
	for (size_t i = 0; i < componentCount; ++i)
	{
		array[i] = components[i].component;
	}
	count = componentCount;
	return count;
}

//*************************************************************************************************
// end of file ( LogisticsVariant.cpp )
