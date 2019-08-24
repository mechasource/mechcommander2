/*************************************************************************************************\
EditorObjects.h : Interface for the EditorObjects, buildings, mechs etc.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved. //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef EDITOROBJECTS_H
#define EDITOROBJECTS_H

//#include "heap.h"
//#include "elist.h"

#include <daprtype.h>
// #include "objectappearance.h"

namespace Stuff
{
class Vector3D;
}

struct _ScenarioMapCellInfo;
class FitIniFile;
class ObjectAppearance;

#define MAX_PILOT 64


class EditorObject
{
public:
	EditorObject(void);
	virtual ~EditorObject(void);
	EditorObject(const EditorObject&);
	EditorObject& operator=(const EditorObject&);
	virtual void CastAndCopy(const EditorObject& master) { (*this) = master; }
	virtual EditorObject* Clone(void) { return (new EditorObject(*this)); }

	PVOID operator new(size_t mySize);
	void operator delete(PVOID us);

	void select(bool bSelect);
	inline bool isSelected(void) const { return appearInfo->appearance->selected ? true : false; }

	void setAlignment(int32_t align);
	inline int32_t getAlignment(void) const { return appearInfo->appearance->teamId; }

	uint32_t getColor(void) const;
	int32_t getID(void) const { return id; }

	void getCells(int32_t& cellJ, int32_t& cellI) const;

	virtual bool save(FitIniFile* file, int32_t warriorNumber) { return false; }
	virtual bool load(FitIniFile* file, int32_t warriorNumber) { return false; }
	virtual int32_t getType(void) const { return BLDG_TYPE; }

	int32_t getSpecialType(void) const;
	int32_t getGroup(void) const;
	int32_t getIndexInGroup(void) const;

	void setDamage(bool bDamage);
	bool getDamage(void) const;

	const std::wstring_view& getDisplayName(void) const;

	const Stuff::Vector3D& getPosition(void) const { return appearance()->position; }

	void markTerrain(_ScenarioMapCellInfo* pInfo, int32_t type, int32_t counter)
	{
		appearInfo->appearance->markTerrain(pInfo, type, counter);
	}

	void setAppearance(int32_t Group, int32_t indexInGroup);

	ObjectAppearance* appearance(void) { return appearInfo->appearance; }
	const ObjectAppearance* appearance(void) const { return appearInfo->appearance; }

	int32_t getForestID(void) const { return forestId; }
	void setForestID(int32_t newID) { forestId = newID; }

	void setScale(int32_t newScale) { scale = newScale; }
	int32_t getScale() const { return scale; }

protected:
	struct AppearanceInfo
	{
		ObjectAppearance* appearance;
		int32_t refCount; // so we only delete once

		PVOID operator new(size_t mySize)
		{
			PVOID result = nullptr;
			result = systemHeap->Malloc(mySize);
			return (result);
		}

		void operator delete(PVOID us) { systemHeap->Free(us); }

		AppearanceInfo& operator=(const AppearanceInfo& src);
	};

	int32_t cellColumn;
	int32_t cellRow;
	int32_t id;
	AppearanceInfo* appearInfo;
	int32_t forestId;
	int32_t scale; // forest only

	friend class EditorObjectMgr; // the only thing that can move and change
		// these things
};

// THIS CLASS CLEARLY NEEDS FLESHING OUT!
class Pilot
{

public:
	Pilot(void) { info = 0; }

	static void initPilots(void);

	struct PilotInfo
	{
		const std::wstring_view& name;
		const std::wstring_view& fileName;
	};

	static PilotInfo s_GoodPilots[MAX_PILOT];
	static PilotInfo s_BadPilots[MAX_PILOT];
	static int32_t goodCount;
	static int32_t badCount;

	const std::wstring_view& getName(void) const { return info->name; }
	void setName(const std::wstring_view&);

	void save(FitIniFile* file, int32_t bGoodGuy);
	void load(FitIniFile* file, int32_t bGoodGuy);

	PilotInfo* info;
	/*note: The value of info should always be nullptr or a pointer to static
	data. So the default assignment/copy operator (shallow copy) is valid. */
};


class Brain
{
	int32_t numCells;
	int32_t* cellNum;
	int32_t* cellType;
	float* cellData;

	int32_t numStaticVars;

	char brainName[256];

public:
	Brain(void)
	{
		numStaticVars = numCells = 0;
		cellNum = cellType = nullptr;
		cellData = nullptr;
		brainName[0] = 0;
	}

	~Brain(void)
	{
		free(cellNum);
		free(cellType);
		free(cellData);
		numStaticVars = numCells = 0;
		cellNum = cellType = nullptr;
		cellData = nullptr;
		brainName[0] = 0;
	}

	Brain(const Brain&);
	Brain& operator=(const Brain&);

	bool save(FitIniFile* file, int32_t warriorNumber, bool usePBrain = false);
	bool load(FitIniFile* file, int32_t warriorNumber);
};


class CUnitList;

class Unit : public EditorObject
{
public:
	Unit(int32_t alignment);
	Unit(const Unit& src);
	Unit& operator=(const Unit& src);
	virtual ~Unit(void);
	virtual void CastAndCopy(const EditorObject& master);
	virtual EditorObject* Clone(void) { return (new Unit(*this)); }

	void setLanceInfo(int32_t newLance, int32_t index)
	{
		lance = newLance;
		lanceIndex = index;
	}
	void getLanceInfo(int32_t& newLance, int32_t& index)
	{
		newLance = lance;
		index = lanceIndex;
	}
	virtual int32_t getType(void) const { return GV_TYPE; }

	uint32_t getSquad(void) const { return squad; }
	void setSquad(uint32_t newSquad);

	virtual bool save(FitIniFile* file, int32_t warriorNumber);
	virtual bool load(FitIniFile* file, int32_t warriorNumber);

	void getColors(uint32_t& base, uint32_t& color1, uint32_t& color2) const;
	void setColors(uint32_t base, uint32_t color1, uint32_t color2);

	bool getSelfRepairBehaviorEnabled(void) const { return selfRepairBehaviorEnabled; }
	void setSelfRepairBehaviorEnabled(bool val) { selfRepairBehaviorEnabled = val; }

	inline Pilot* getPilot(void) { return &pilot; }

	void setVariant(uint32_t newVar) { variant = newVar; }
	inline int32_t getVariant(void) const { return variant; }

	CUnitList* pAlternativeInstances;
	uint32_t tmpNumAlternativeInstances;
	uint32_t tmpAlternativeStartIndex;

protected:
	bool save(FitIniFile* file, int32_t WarriorNumber, int32_t controlDataType, const std::wstring_view& objectProfile);

	Brain brain;
	bool selfRepairBehaviorEnabled;
	int32_t lance; // which lance
	int32_t lanceIndex; // number within lance 1 to 12
	uint32_t squad;
	Pilot pilot;

	uint32_t baseColor;
	uint32_t highlightColor;
	uint32_t highlightColor2;

	uint32_t variant;
};

// class CUnitList: public EList<Unit, Unit&> {};

class DropZone : public EditorObject
{
public:
	DropZone(const Stuff::Vector3D& position, int32_t alignment, bool bVTol);
	DropZone& operator=(const DropZone& src)
	{
		bVTol = src.bVTol;
		EditorObject::operator=(src);
		return *this;
	}
	virtual void CastAndCopy(const EditorObject& master);
	virtual EditorObject* Clone(void) { return (new DropZone(*this)); }
	virtual bool save(FitIniFile* file, int32_t number);

	bool isVTol(void) { return bVTol; }

private:
	bool bVTol;
};

class NavMarker : public EditorObject
{
public:
	NavMarker(void);
	virtual EditorObject* Clone(void) { return (new NavMarker(*this)); }
	virtual bool save(FitIniFile* file, int32_t number);
};


#endif // end of file ( EditorObjects.h )
