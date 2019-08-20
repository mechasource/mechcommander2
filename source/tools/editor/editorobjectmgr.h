/*************************************************************************************************\
EditorObjectMgr.h	: Interface for the EditorObjectMgr component.  This thing
holds lists of all the objects on the map.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef EDITOROBJECTMGR_H
#define EDITOROBJECTMGR_H

//#include <elist.h>
//#include "editorobjects.h"
//#include "forest.h"
//#include <mclib.h>

class ObjectAppearance;
class AppearanceType;
class EditorObject;
class Mech;
class BuildingLink;

class EditorObjectMgr
{
private:
	struct Building;

public:
	static EditorObjectMgr* instance() { return s_instance; }

	enum BuildingType
	{
		UNSPECIAL = -1,
		NORMAL_BUILDING = 0,
		DROP_ZONE = 1,
		TURRET_CONTROL,
		GATE_CONTROL,
		POWER_STATION,
		TURRET_GENERATOR,
		SENSOR_CONTROL,
		EDITOR_GATE,
		EDITOR_TURRET,
		SENSOR_TOWER,
		EDITOR_BRIDGE,
		BRIDGE_CONTROL,
		SPOTLIGHT,
		SPOTLIGHT_CONTROL,
		DROPZONE,
		NAV_MARKER,
		WALL,
		LOOKOUT,
		RESOURCE_BUILDING,
		HELICOPTER
	};

	class EDITOR_OBJECT_LIST : public EList<EditorObject*, EditorObject*>
	{
	public:
		void AddUnique(EditorObject* item)
		{
			if ((0 < Count()) && (INVALID_ITERATOR != Find(item)))
			{
				return;
			}
			Append(item);
		}
		void RemoveIfThere(EditorObject* item)
		{
			if (0 < Count())
			{
				EIterator it = Find(item);
				if (INVALID_ITERATOR != it)
				{
					Delete(it);
				}
			}
		}
	};

	EditorObjectMgr(void); // should only be one of these
	~EditorObjectMgr(void);

	void render(void);

	void update(void);

	void renderShadows(void);

	void init(PCSTR bldgListFileName, PCSTR objectFileName);

	EditorObject* addBuilding(const Stuff::Vector3D& position, uint32_t group,
		uint32_t indexWithinGroup, int32_t alignment, float rotation = 0.0, float height = 1.0,
		bool bSnapToCell = true); // returns nullptr if failure

	bool addBuilding(EditorObject* pObjectThatWeWillCopy);

	EditorObject* addDropZone(const Stuff::Vector3D& position, int32_t alignment,
		bool bVTol); // returns nullptr if failure

	bool deleteBuilding(const EditorObject* pInfo); // should only be one building in a spot
	bool deleteObject(const EditorObject* pInfo)
	{
		return deleteBuilding(pInfo); // only object FOR NOW
	}

	/* This function (getObjectAtPosition(...)) is for use in determining
	objects selected by the mouse, not really for general use. It appears to
	return an object that a) is visible on screen, and b) has the property that
	the screen projection of the given position is contained in (intersect with)
	the screen projection of the object. */
	EditorObject* getObjectAtPosition(const Stuff::Vector3D& position);
	/*This function (getObjectAtCell(...)) returns an object that occupies the
	given cell (i.e. it doesn't have to be centered at the given cell).*/
	EditorObject* getObjectAtCell(int32_t cellJ, int32_t cellI);

	/*This function (getObjectAtLocation(...)) returns an object whose x and y
	positions are the same as (or very close to) those given. Screen visibility,
	object geometry, etc., are not considerred.*/
	EditorObject* getObjectAtLocation(float x, float y);

	EditorObject* getBuilding(const EditorObject& building);

	bool canAddBuilding(
		const Stuff::Vector3D& position, float rotation, uint32_t group, uint32_t indexWithinGroup);

	bool canAddDropZone(const Stuff::Vector3D& position, int32_t alignment, bool bVTol);

	int32_t getBuildingGroupCount(void) const; // mechs count too!
	int32_t getNumberBuildingsInGroup(int32_t Group) const;
	void getBuildingGroupNames(PCSTR* names, int32_t& numberOfNames) const;
	void getNamesOfObjectsInGroup(PCSTR groupName, PCSTR* names, int32_t& numberOfNames) const;
	void getBuildingNamesInGroup(int32_t Group, PCSTR* names, int32_t& numberOfNames) const;
	int32_t getNumberOfVariants(int32_t group, int32_t indexInGroup) const;
	void getVariantNames(
		int32_t group, int32_t indexInGroup, PCSTR* names, int32_t& numberOfNames) const;
	PCSTR getGroupName(int32_t group) const;
	PCSTR getObjectName(int32_t ID) const;

	int32_t getUnitGroupCount(void) const;
	void getUnitGroupNames(PCSTR* names, pint32_t IDs, int32_t& numberOfNames) const;

	bool save(PacketFile& file, int32_t whichPacket);
	bool load(PacketFile& file, int32_t whichPacket);

	bool saveMechs(FitIniFile& file);
	bool loadMechs(FitIniFile& file); // need to implement this

	bool saveDropZones(FitIniFile& file);
	bool loadDropZones(FitIniFile& file);

	bool saveForests(FitIniFile& file);
	bool loadForests(FitIniFile& file);

	int32_t getFitID(int32_t id) const;

	bool moveBuilding(EditorObject* pInfo, int32_t cellI, int32_t cellJ);
	bool getBlocksLineOfFire(int32_t id) const;
	bool getIsHoverCraft(int32_t id) const;

	inline int32_t getID(int32_t group, int32_t index)
	{
		return (getType(group, index) << 24) | (group << 16) | (index << 8);
	}

	void clear(void); // delete all objects

	int64_t getImpassability(int32_t id);

	inline static uint32_t getGroup(int32_t id);
	inline static uint32_t getIndexInGroup(int32_t id);
	inline int32_t getAppearanceType(int32_t ID);

	bool getBuildingFromID(int32_t fitID, uint32_t& group, uint32_t& index, bool canBeMech);

	void unselectAll(void);
	void select(const Stuff::Vector4D& pos1, const Stuff::Vector4D& pos2);
	void select(EditorObject& object, bool bSelect = true);
	void objectSelectionChanged(void) /* use this to notify the EditorObjectMgr
										 when selection states of objects are
										 changed externally */
	{
		selectedObjectsNeedsToBeSynched = true;
	}
	bool hasSelection(void);
	int32_t getSelectionCount(void); // returns the number of selected items
	EDITOR_OBJECT_LIST getSelectedObjectList(void);

	void deleteSelectedObjects(void);

	void adjustObjectsToNewTerrainHeights(void);

	ObjectAppearance* getAppearance(uint32_t group, uint32_t indexWithinGroup);
	ObjectAppearance* getAppearance(Building* pBuilding);

	// LINKS!
	BuildingLink* getLinkWithParent(const EditorObject* pObj);
	BuildingLink* getLinkWithChild(const EditorObject* pObj);
	BuildingLink* getLinkWithBuilding(const EditorObject* pObj); // parent or child
	void addLink(BuildingLink*);
	void deleteLink(BuildingLink*);

	inline BuildingType getSpecialType(int32_t ID);
	inline bool isCapturable(int32_t ID);
	inline float getScale(int32_t ID);
	inline bool isAlignable(int32_t ID);
	inline int32_t getObjectTypeNum(int32_t ID);
	inline PCSTR getFileName(int32_t ID) const;
	inline PCSTR getTGAFileName(int32_t ID) const;
	inline uint32_t getTacMapColor(int32_t ID) const;

	typedef EList<EditorObject*, EditorObject*> BUILDING_LIST; // buildings on the map
	typedef EList<Unit*, Unit*> UNIT_LIST;

	void getSelectedUnits(UNIT_LIST&);
	UNIT_LIST getUnits() { return units; }
	BUILDING_LIST getBuildings() { return buildings; }

	uint32_t getNextAvailableSquadNum() { return nextAvailableSquadNum; }
	void registerSquadNum(uint32_t squadNum);
	void unregisterSquadNum(uint32_t squadNum) {}
	void resetAvailableSquadNums() { nextAvailableSquadNum = 1; }

	int32_t getNextAvailableForestID(void);
	int32_t createForest(const Forest& settings);
	void editForest(int32_t& ID, const Forest& newSettings);
	void removeForest(const Forest& settings);
	const Forest* getForest(int32_t ID);
	int32_t getForests(Forest** pForests, int32_t& count);
	void selectForest(int32_t ID);

private:
	struct Building
	{
		char name[64];
		PSTR* varNames;
		int32_t nameID;
		char fileName[64];
		AppearanceType* appearanceType;
		int32_t type;
		uint32_t fitID;
		int64_t impassability;
		bool blocksLineOfFire;
		BuildingType specialType;
		bool capturable;
		bool alignable;
		uint32_t writeOnTacMap;
		char tgaName[64];
		int32_t objectTypeNum;
		char forestId;
		float scale;
		bool isHoverCraft;
		~Building(void);
	};

	struct Group
	{
		char name[64];
		EList<Building, Building&> buildings;
	};

	typedef EList<EditorObject*, EditorObject*> BUILDING_LIST; // buildings on the map
	typedef EList<Unit*, Unit*> UNIT_LIST;
	typedef EList<Group, Group&> GROUP_LIST;
	typedef EList<BuildingLink*, BuildingLink*> LINK_LIST;
	typedef EList<DropZone*, DropZone*> DROP_LIST;
	typedef EList<Forest*, Forest*> FOREST_LIST;

	GROUP_LIST groups; // list of possible buildings
	Group dropZoneGroup;
	BUILDING_LIST buildings;
	UNIT_LIST units;
	LINK_LIST links;
	DROP_LIST dropZones;
	FOREST_LIST forests;

	EDITOR_OBJECT_LIST selectedObjects;
	bool selectedObjectsNeedsToBeSynched;
	void syncSelectedObjectPointerList(void); // makes sure that the
		// "selectedObjects" list
		// corresponds to the objects
		// marked as selected

	// HELPERS
	int32_t ExtractNextString(puint8_t& pFileLine, PSTR pBuffer, int32_t bufferLength);
	int32_t ExtractNextInt(puint8_t& pFileLine);
	float ExtractNextFloat(puint8_t& pFileLine);

	int32_t getType(uint32_t group, uint32_t indexWithinGroup);
	void getRandomTreeFromGroup(int32_t treeGroup, int32_t& group, int32_t& index);

	void doForest(const Forest& forest);

	uint32_t nextAvailableSquadNum;

	static EditorObjectMgr* s_instance; // the one and only instance of this object
};

inline uint32_t
EditorObjectMgr::getGroup(int32_t id)
{
	return ((id >> 16) & 0x00ff);
}
inline uint32_t
EditorObjectMgr::getIndexInGroup(int32_t id)
{
	return ((id >> 8) & 0x00ff);
}
inline EditorObjectMgr::BuildingType
EditorObjectMgr::getSpecialType(int32_t ID)
{
	return groups[getGroup(ID)].buildings[getIndexInGroup(ID)].specialType;
}

inline bool
EditorObjectMgr::isAlignable(int32_t ID)
{
	return groups[getGroup(ID)].buildings[getIndexInGroup(ID)].alignable;
}

inline int32_t
EditorObjectMgr::getAppearanceType(int32_t ID)
{
	return groups[getGroup(ID)].buildings[getIndexInGroup(ID)].appearanceType->getAppearanceClass(
		void);
}

inline int32_t
EditorObjectMgr::getObjectTypeNum(int32_t ID)
{
	return groups[getGroup(ID)].buildings[getIndexInGroup(ID)].objectTypeNum;
}

inline PCSTR
EditorObjectMgr::getFileName(int32_t ID) const
{
	return groups[getGroup(ID)].buildings[getIndexInGroup(ID)].fileName;
}

inline PCSTR
EditorObjectMgr::getTGAFileName(int32_t ID) const
{
	return groups[getGroup(ID)].buildings[getIndexInGroup(ID)].tgaName;
}

inline uint32_t
EditorObjectMgr::getTacMapColor(int32_t ID) const
{
	return groups[getGroup(ID)].buildings[getIndexInGroup(ID)].writeOnTacMap;
}

inline float
EditorObjectMgr::getScale(int32_t ID)
{
	return groups[getGroup(ID)].buildings[getIndexInGroup(ID)].scale;
}

//*************************************************************************************************
#endif // end of file ( EditorObjectMgr.h )
