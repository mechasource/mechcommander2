//===========================================================================//
// LogisticsMech.h			: Interface for the LogisticsMech component.     //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef LOGISTICSMECH_H
#define LOGISTICSMECH_H

//#include "logisticsvariant.h"

class LogisticsPilot;

class LogisticsMech
{
public:
	LogisticsMech(LogisticsVariant*, int32_t count);
	~LogisticsMech(void);

	inline int32_t getID()
	{
		return ID;
	}
	inline int32_t getInstanceID()
	{
		return ID >> 24;
	}
	void setPilot(LogisticsPilot* pPilot);
	inline LogisticsPilot* getPilot()
	{
		return pilot;
	}
	inline int32_t getForceGroup()
	{
		return forceGroup;
	}
	inline void setForceGroup(int32_t bInForceGroup)
	{
		forceGroup = bInForceGroup;
	}
	inline std::wstring_view getFileName()
	{
		return pVariant->chassis->fileName;
	}
	inline bool isAvailable(void) const
	{
		return pVariant->availableToUser;
	}
	uint32_t getID(void) const
	{
		return pVariant->ID;
	}
	float getMaxWeight(void) const
	{
		return pVariant->chassis->maxWeight;
	}
	int32_t getChassisName(void) const
	{
		return pVariant->chassis->chassisNameID;
	}
	std::wstring_view getName(void) const
	{
		return pVariant->variantName;
	}
	int32_t getEncyclopediaID(void) const
	{
		return pVariant->chassis->encyclopediaID;
	}
	int32_t getHelpID(void) const
	{
		return pVariant->chassis->helpid;
	}
	int32_t getBaseCost(void) const
	{
		return pVariant->chassis->baseCost;
	}
	int32_t getComponentCount(void) const
	{
		return pVariant->componentCount;
	}
	int32_t canAddComponent(LogisticsComponent* pComponent, int32_t x, int32_t y) const
	{
		return pVariant->canAddComponent(pComponent, x, y);
	}
	int32_t getCost(void) const
	{
		return pVariant->getCost(void);
	}
	int32_t getWeight(void) const
	{
		return pVariant->getWeight(void);
	}
	std::wstring_view getMechClass(void) const
	{
		return pVariant->getMechClass(void);
	}
	int32_t getChassisID(void) const
	{
		return pVariant->chassis->ID;
	}
	int32_t getArmor(void) const
	{
		return pVariant->getArmor(void);
	}
	int32_t getJumpRange(void) const
	{
		return pVariant->getJumpRange(void);
	}
	int32_t getSpeed(void) const
	{
		return pVariant->getSpeed(void);
	}
	int32_t getDisplaySpeed(void) const
	{
		return pVariant->getDisplaySpeed(void);
	}
	int32_t getVariantID(void) const
	{
		return pVariant->ID;
	}
	std::wstring_view getSmallIconFileName(void) const
	{
		return pVariant->chassis->iconFileNames[0];
	}
	std::wstring_view getMediumIconFileName(void) const
	{
		return pVariant->chassis->iconFileNames[1];
	}
	std::wstring_view getLargeIconFileName(void) const
	{
		return pVariant->chassis->iconFileNames[2];
	}
	int32_t getComponentsWithLocation(
		int32_t& count, int32_t* IDArray, int32_t* xLocationArray, int32_t* yLocationArray)
	{
		return pVariant->getComponentsWithLocation(count, IDArray, xLocationArray, yLocationArray);
	}

	int32_t getComponents(int32_t& count, int32_t* IDArray)
	{
		return pVariant->getComponents(count, IDArray);
	}

	void setVariant(LogisticsVariant* pVariant);

	int32_t getFitID(void) const
	{
		return pVariant->chassis->fitID;
	}
	int32_t save(FitIniFile& file, int32_t count);

	void setcolours(uint32_t base, uint32_t high1, uint32_t high2);
	void getcolours(uint32_t& base, uint32_t& high1, uint32_t& high2) const;
	int32_t getIconIndex(void) const
	{
		return pVariant->chassis->iconPictureIndex;
	}

	LogisticsVariant* getVariant()
	{
		return pVariant;
	}

private:
	LogisticsVariant* pVariant;

	uint32_t pilotID;
	int32_t forceGroup; // 0 if none, 1 if otherwise
	LogisticsPilot* pilot;
	int32_t ID;
	int32_t basecolour;
	int32_t highlightcolour1;
	int32_t highlightcolour2;

	// DATA

	// HELPER FUNCTIONS

	// suppress these puppies
	LogisticsMech(void);
	LogisticsMech& operator=(const LogisticsMech&);
};

#endif // end of file ( LogisticsMech.h )
