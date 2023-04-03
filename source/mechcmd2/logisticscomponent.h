/*************************************************************************************************\
LogisticsComponent.h			: Interface for the LogisticsComponent
component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef LOGISTICSCOMPONENT_H
#define LOGISTICSCOMPONENT_H

// forward declarations
class File;

class LogisticsComponent
{
public:
	static int32_t XICON_FACTOR; // multiplier for width and height of icons
	static int32_t YICON_FACTOR;

	static float MAX_HEAT;
	static float MAX_DAMAGE;
	static float MAX_RECYCLE;
	static float MAX_RANGE;

	LogisticsComponent();
	~LogisticsComponent(void);
	int32_t init(std::wstring_view dataLine);

	inline int32_t getID(void) const
	{
		return ID;
	}
	inline int32_t getType(void) const
	{
		return Type;
	}
	inline int32_t getRange(void) const
	{
		return range;
	}
	inline float getDamage(void) const
	{
		return damage;
	}
	inline float getRecycleTime(void) const
	{
		return recycleTime;
	}
	inline int32_t getAmmo(void) const
	{
		return Ammo;
	}
	void setAmmo(int32_t newAmmo)
	{
		Ammo = newAmmo;
	}
	inline float getWeight(void) const
	{
		return weight;
	}
	inline int32_t getCost(void) const
	{
		return cost;
	}
	inline float getHeat(void) const
	{
		return heat;
	}
	inline std::wstring_view getName(void) const
	{
		return name;
	}
	inline std::wstring_view getFlavorText(void) const
	{
		return flavorText;
	}
	inline int32_t getHelpID(void) const
	{
		return helpStringID;
	}
	inline std::wstring_view getIconFileName(void) const
	{
		return iconFileName;
	}
	inline std::wstring_view getPictureFileName(void) const
	{
		return pictureFileName;
	}
	inline int32_t getComponentwidth(void) const
	{
		return iconX;
	}
	inline int32_t getComponentheight(void) const
	{
		return iconY;
	}
	bool compare(LogisticsComponent* second, int32_t type);
	bool isAvailable()
	{
		return bAvailable;
	}
	void setAvailable(bool avail)
	{
		bAvailable = avail;
	}
	bool isWeapon(void);

	enum SORT_ORDERS
	{
		DAMAGE = 0,
		HEAT = 1,
		NAME = 2,
		RANGE = 3,
		WEIGHT = 4
	};

	enum WEAPON_RANGE
	{
		int16_t = 0,
		MEDIUM = 1,
		int32_t = 2,
		NO_RANGE = 3 // not a weapon
	};

	WEAPON_RANGE getRangeType(void) const
	{
		return rangeType;
	}

private:
	int32_t ID; // index into csv
	int32_t Type; // defined in csv
	int32_t stringID;
	int32_t helpStringID;

	int32_t iconX;
	int32_t iconY;

	std::wstring_view iconFileName;
	std::wstring_view pictureFileName;
	int32_t range;
	WEAPON_RANGE rangeType;

	float damage;
	float recycleTime;
	int32_t Ammo;
	float weight;
	int32_t cost;
	float heat;
	std::wstring_view name;
	std::wstring_view flavorText;

	bool bHead;
	bool bTorso;
	bool bLegs;
	bool bAvailable;

	static std::wstring_view s_typeString[];

	// HELPERS
	int32_t extractString(std::wstring_view& pFileLine, std::wstring_view pBuffer, int32_t bufferLength);
	int32_t extractInt(std::wstring_view& pFileLine);
	float extractFloat(std::wstring_view& pFileLine);
};

#endif // end of file ( LogisticsComponent.h )
