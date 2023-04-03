#define EDITOROBJECTMGR_CPP
//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef BDACTOR_H
#include "bdactor.h"
#endif

#include "editorobjectmgr.h"

#ifndef FILE_H
#include "File.h"
#endif

#ifndef UTILITIES_H
#include "utilities.h"
#endif

#ifndef TERRAIN_H
#include "Terrain.h"
#endif

#ifndef ERR_H
#include "err.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef PATHS_H
#include "paths.h"
#endif

#ifndef CIDENT_H
#include "cident.h"
#endif

#ifndef PACKET_H
#include "packet.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef BDACTOR_H
#include "bdactor.h"
#endif

#ifndef MECH3D_H
#include "mech3d.h"
#endif

#ifndef MESSAGEBOX_H
#include "messagebox.h"
#endif

#ifndef APPRTYPE_H
#include "apprtype.h"
#endif

#ifndef TACMAP_H
#include "tacmap.h"
#endif

#ifndef EDITORDATA_H
#include "editordata.h"
#endif

#include "resource.h"

#include "McLibResource.h"

#include "buildinglink.h"
#include "utilities.h"

#include "malloc.h"

#include "_ASSERT.h"

// ARM
#include "../ARM/Microsoft.Xna.Arm.h"
//using namespace Microsoft::Xna::Arm;

EditorObjectMgr* EditorObjectMgr::s_instance = nullptr;
uint32_t gameResourceHandle = 0;

extern bool renderObjects;
extern bool renderTrees;

extern bool justResaveAllMaps;

#define TREE_GROUP 1

inline float
agsqrt(float _a, float _b)
{
	return sqrt(_a * _a + _b * _b);
}

EditorObjectMgr::EditorObjectMgr()
{
	gosASSERT(!s_instance);
	s_instance = this; // should only be one of these
}

EditorObjectMgr::~EditorObjectMgr()
{
	clear();
	if (appearanceTypeList)
		delete appearanceTypeList;
	appearanceTypeList = nullptr;
	for (GROUP_LIST::EIterator groupIter = groups.Begin(); !groupIter.IsDone(); groupIter++)
	{
		for (EList<Building, Building&>::EIterator buildIter = (*groupIter).buildings.Begin();
			 !buildIter.IsDone(); buildIter++)
		{
			Building teste = (*buildIter);
			if ((*buildIter).varNames)
			{
				for (size_t i = 0; i < 16; i++)
				{
					if ((*buildIter).varNames[i])
					{
						free((*buildIter).varNames[i]);
						(*buildIter).varNames[i] = nullptr;
					}
				}
				// Kinda need to delete this OUTSIDE the above loop.
				// Yum, Yum!
				free((*buildIter).varNames);
				(*buildIter).varNames = nullptr;
			}
		}
	}
	s_instance = 0;
}

void EditorObjectMgr::clear()
{
	for (BUILDING_LIST::EIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
	{
		delete *iter;
	}
	for (UNIT_LIST::EIterator mIter = units.Begin(); !mIter.IsDone(); mIter++)
	{
		Unit* pMech = *mIter;
		delete pMech;
	}
	buildings.Clear();
	units.Clear();
	for (LINK_LIST::EIterator liter = links.Begin(); !liter.IsDone(); liter++)
		delete *liter;
	links.Clear();
	selectedObjects.Clear();
	selectedObjectsNeedsToBeSynched = false;
	for (DROP_LIST::EIterator dIter = dropZones.Begin(); !dIter.IsDone(); dIter++)
		delete *dIter;
	dropZones.Clear();
	resetAvailableSquadNums();
	for (GROUP_LIST::EIterator groupIter = groups.Begin(); !groupIter.IsDone(); groupIter++)
	{
		for (EList<Building, Building&>::EIterator buildIter = (*groupIter).buildings.Begin();
			 !buildIter.IsDone(); buildIter++)
		{
			(*buildIter).appearanceType = nullptr;
		}
	}
	for (FOREST_LIST::EIterator fIter = forests.Begin(); !fIter.IsDone(); fIter++)
	{
		delete (*fIter);
	}
	forests.Clear();
}

void EditorObjectMgr::init(std::wstring_view bldgListFileName, std::wstring_view objectFileName)
{
	File lstFile;
	lstFile.open(const_cast<std::wstring_view>(bldgListFileName));
	uint8_t buffer[512];
	lstFile.readLine(buffer, 512);
	PacketFile objectFile;
	int32_t result = objectFile.open(const_cast<std::wstring_view>(objectFileName));
	gosASSERT(result == NO_ERROR);
	while (lstFile.getLogicalPosition() < lstFile.getLength())
	{
		lstFile.readLine(buffer, 512);
		if (!strlen((std::wstring_view)buffer))
			break;
		Building bldg;
		bldg.appearanceType = nullptr;
		bldg.specialType = UNSPECIAL;
		bldg.varNames = 0;
		bldg.forestId = 0;
		wchar_t tmpBuffer[64];
		int32_t bufferLength = 64;
		uint8_t* pCur = buffer;
		wchar_t GroupName[64];
		// extract the file name
		if (ExtractNextString(pCur, tmpBuffer, bufferLength))
			strcpy(bldg.fileName, tmpBuffer);
		// extract the group
		if (ExtractNextString(pCur, tmpBuffer, bufferLength))
		{
			int32_t group = atoi(tmpBuffer);
			cLoadString(IDS_GROUPS + group, GroupName, 64, gameResourceHandle);
		}
		// extract the building name
		if (ExtractNextString(pCur, tmpBuffer, bufferLength))
		{
			int32_t strId = atoi(tmpBuffer);
			bldg.nameID = strId;
			cLoadString(strId, tmpBuffer, 64);
			strcpy(bldg.name, tmpBuffer);
		}
		// extract the type
		if (ExtractNextString(pCur, tmpBuffer, bufferLength))
		{
			if (_stricmp(tmpBuffer, "BUILDING") == 0)
				bldg.type = BLDG_TYPE;
			else if (_stricmp(tmpBuffer, "TREE") == 0)
				bldg.type = TREED_TYPE;
			else if (_stricmp(tmpBuffer, "VEHICLE") == 0)
				bldg.type = GV_TYPE;
			else
				bldg.type = MECH_TYPE;
		}
		if (ExtractNextString(pCur, tmpBuffer, bufferLength))
		{
			bldg.fitID = atoi(tmpBuffer);
			if (NO_ERROR != objectFile.seekPacket(bldg.fitID))
				gosASSERT(false);
			int32_t fileSize = objectFile.getPacketSize();
			FitIniFile bldgFile;
			result = bldgFile.open(&objectFile, fileSize);
			uint32_t lowTemplate, highTemplate;
			bldgFile.readIdULong("LowTemplate", lowTemplate);
			bldgFile.readIdULong("HighTemplate", highTemplate);
			bldg.impassability = (highTemplate << 32) | lowTemplate;
			bldgFile.readIdBoolean("BlocksLineOfFire", bldg.blocksLineOfFire);
			bldgFile.seekBlock("ObjectClass");
			bldgFile.readIdLong("ObjectTypeNum", bldg.objectTypeNum);
			bldgFile.seekBlock("General");
			bldg.isHoverCraft = false;
			bldgFile.readIdBoolean("HoverCraft", bldg.isHoverCraft);
			bldgFile.seekBlock("ObjectType");
			wchar_t tmpBuf[2000];
			strcpy(tmpBuf, "");
			bldgFile.readIdString("Name", tmpBuf, 1000);
			if (0 == strcmp(tmpBuf, "monsoon"))
			{
				int32_t i = 11;
			}
			bldgFile.close();
		}
		// extract the special type type
		if (ExtractNextString(pCur, tmpBuffer, bufferLength))
		{
			if (strstr(tmpBuffer, "TREE"))
			{
				bldg.forestId = atoi(tmpBuffer + 4);
			}
			else if (_stricmp(tmpBuffer, "GATE") == 0)
				bldg.specialType = EDITOR_GATE;
			else if (_stricmp(tmpBuffer, "GATE_CONTROL") == 0)
				bldg.specialType = GATE_CONTROL;
			else if (_stricmp(tmpBuffer, "POWER_GENERATOR") == 0)
				bldg.specialType = POWER_STATION;
			else if (_stricmp(tmpBuffer, "SENSOR_CONTROL") == 0)
				bldg.specialType = SENSOR_CONTROL;
			else if (_stricmp(tmpBuffer, "SENSOR") == 0)
				bldg.specialType = SENSOR_TOWER;
			else if (_stricmp(tmpBuffer, "SPOTLIGHT_CONTROL") == 0)
				bldg.specialType = SPOTLIGHT_CONTROL;
			else if (_stricmp(tmpBuffer, "SPOTLIGHT") == 0)
				bldg.specialType = SPOTLIGHT;
			else if (_stricmp(tmpBuffer, "LOOKOUT") == 0)
				bldg.specialType = LOOKOUT;
			else if (_stricmp(tmpBuffer, "TURRET_CONTROL") == 0)
				bldg.specialType = TURRET_CONTROL;
			else if (_stricmp(tmpBuffer, "TURRET_GENERATOR") == 0)
				bldg.specialType = TURRET_GENERATOR;
			else if (_stricmp(tmpBuffer, "BRIDGE_CONTROL") == 0)
				bldg.specialType = BRIDGE_CONTROL;
			else if (_stricmp(tmpBuffer, "BRIDGE") == 0)
				bldg.specialType = EDITOR_BRIDGE;
			else if (_stricmp(tmpBuffer, "DROPZONE") == 0)
				bldg.specialType = DROPZONE;
			else if (_stricmp(tmpBuffer, "TURRET") == 0)
				bldg.specialType = EDITOR_TURRET;
			else if (_stricmp(tmpBuffer, "NAVMARKER") == 0)
				bldg.specialType = NAV_MARKER;
			else if (_stricmp(tmpBuffer, "WALL") == 0)
				bldg.specialType = WALL;
			else if (_stricmp(tmpBuffer, "RPBUILDING") == 0)
				bldg.specialType = RESOURCE_BUILDING;
			else if (_stricmp(tmpBuffer, "HELICOPTER") == 0)
				bldg.specialType = HELICOPTER;
		}
		bldg.capturable = ExtractNextInt(pCur) ? true : false;
		bldg.alignable = ExtractNextInt(pCur) ? true : false;
		bldg.writeOnTacMap = ExtractNextInt(pCur);
		if (ExtractNextString(pCur, tmpBuffer, bufferLength))
			strcpy(bldg.tgaName, tmpBuffer);
		else
			strcpy(bldg.tgaName, "NONE");
		ExtractNextFloat(pCur);
		bldg.scale = ExtractNextFloat(pCur);
		// go out and find the variant file
		CSVFile csvFile;
		FullPathFileName csvFileName;
		csvFileName.init(objectPath, bldg.fileName, ".csv");
		wchar_t varName[256];
		if (NO_ERROR == csvFile.open(csvFileName))
		{
			bldg.varNames = (std::wstring_view*)malloc(sizeof(std::wstring_view) * 16);
			for (size_t i = 0; i < 16; ++i)
			{
				if (NO_ERROR == csvFile.readString(23 + 97 * i, 2, varName, 256) && strlen(varName))
				{
					bldg.varNames[i] = (std::wstring_view)malloc(sizeof(wchar_t) * (strlen(varName) + 1));
					strcpy(bldg.varNames[i], varName);
				}
				else
					bldg.varNames[i] = nullptr;
			}
		}
		if (bldg.specialType == DROPZONE)
		{
			strcpy(dropZoneGroup.name, GroupName);
			dropZoneGroup.buildings.Append(bldg);
		}
		else
		{
			bool bNeedGroup = true;
			// now need to add this building
			for (GROUP_LIST::EIterator iter = groups.Begin(); !iter.IsDone(); iter++)
			{
				if (strcmp((*iter).name, GroupName) == 0)
				{
					bNeedGroup = false;
					(*iter).buildings.Append(bldg);
				}
			}
			if (bNeedGroup)
			{
				group grp;
				strcpy(grp.name, GroupName);
				grp.buildings.Append(bldg);
				groups.Append(grp);
			}
		}
	}
}

int32_t
EditorObjectMgr::ExtractNextString(uint8_t*& pFileLine, std::wstring_view pBuffer, int32_t bufferLength)
{
	for (size_t i = 0; i < 512; ++i)
	{
		if (pFileLine[i] == '\n')
			break;
		else if (pFileLine[i] == ',')
			break;
		else if (pFileLine[i] == nullptr)
			break;
	}
	if (i == 512)
		return false;
	gosASSERT(i < bufferLength);
	memcpy(pBuffer, pFileLine, i);
	pBuffer[i] = nullptr;
	bufferLength = i + 1;
	pFileLine += i + 1;
	return i;
}

//---------------------------------------------------------------------------
int32_t
textToLong(std::wstring_view num)
{
	int32_t result = 0;
	//------------------------------------
	// Check if Hex Number
	std::wstring_view hexOffset = (std::wstring_view)strstr(num, "0x");
	if (hexOffset == nullptr)
	{
		result = atol(num);
	}
	else
	{
		hexOffset += 2;
		int32_t numDigits = strlen(hexOffset) - 1;
		for (size_t i = 0; i <= numDigits; i++)
		{
			if (!isalnum(hexOffset[i]) || (isalpha(hexOffset[i]) && toupper(hexOffset[i]) > 'F'))
			{
				hexOffset[i] = 0; // we've reach a "wrong" character. Either
					// start of a comment or something illegal.
					// Either way, stop evaluation here.
				break;
			}
		}
		numDigits = strlen(hexOffset) - 1;
		int32_t power = 0;
		for (size_t count = numDigits; count >= 0; count--, power++)
		{
			uint8_t currentDigit = toupper(hexOffset[count]);
			if (currentDigit >= 'A' && currentDigit <= 'F')
			{
				result += (currentDigit - 'A' + 10) << (4 * power);
			}
			else if (currentDigit >= '0' && currentDigit <= '9')
			{
				result += (currentDigit - '0') << (4 * power);
			}
			else
			{
				//---------------------------------------------------------
				// There is a digit in here I don't understand.  Return 0.
				result = 0;
				break;
			}
		}
	}
	return (result);
}

int32_t
EditorObjectMgr::ExtractNextInt(uint8_t*& pFileLine)
{
	wchar_t buffer[1024];
	int32_t count = ExtractNextString(pFileLine, buffer, 1024);
	if (count > 0)
	{
		return textToLong(buffer);
	}
	return -1;
}

float EditorObjectMgr::ExtractNextFloat(uint8_t*& pFileLine)
{
	wchar_t buffer[1024];
	int32_t count = ExtractNextString(pFileLine, buffer, 1024);
	if (count > 0)
	{
		return atof(buffer);
	}
	return -1;
}

//-------------------------------------------------------------------------------------------------
bool EditorObjectMgr::canAddBuilding(
	const Stuff::Vector3D& position, float rotation, uint32_t group, uint32_t indexWithinGroup)
{
	int32_t realCellI, realCellJ;
	land->worldToCell(position, realCellJ, realCellI);
	EditorObject* cellObject = getObjectAtCell(realCellJ, realCellI);
	if (cellObject)
	{
		if (cellObject->appearance()->rotation == rotation)
			return false;
	}
	// Check if we are ON the map!!
	return land->IsEditorSelectTerrainPosition(position);
}

bool EditorObjectMgr::canAddDropZone(const Stuff::Vector3D& position, int32_t alignment, bool bVTol)
{
	int32_t alignmentCount = 0;
	for (DROP_LIST::EIterator iter = dropZones.Begin(); !iter.IsDone(); iter++)
	{
		if (bVTol)
		{
			if ((*iter)->isVTol())
				return false;
		}
		else
		{
			if ((*iter)->getAlignment() == alignment && !(*iter)->isVTol())
				alignmentCount++;
		}
	}
	if (alignmentCount > 3)
		return false;
	return true;
}

//-------------------------------------------------------------------------------------------------
bool EditorObjectMgr::addBuilding(EditorObject* pObj)
{
	return addBuilding(pObj->appearance()->position, getGroup(pObj->id), getIndexInGroup(pObj->id),
			   pObj->appearance()->teamId, pObj->appearance()->rotation)
		? true
		: false;
}

EditorObject*
EditorObjectMgr::addDropZone(
	const Stuff::Vector3D& position, int32_t alignment, bool bVTol)
{
	gosASSERT((canAddDropZone(position, alignment, bVTol)));
	DropZone* pLast = new DropZone(position, alignment, bVTol);
	dropZones.Append(pLast);
	ObjectAppearance* pAppear = getAppearance(&dropZoneGroup.buildings.GetHead());
	pLast->appearInfo->appearance = pAppear;
	land->worldToCell(position, pLast->cellRow, pLast->cellColumn);
	pLast->id = DROPZONE << 24 | -1 << 16 | -1 << 8;
	Stuff::Vector3D realPos;
	land->getCellPos(pLast->cellRow, pLast->cellColumn, realPos);
	int32_t homeRelations[9] = {0, 0, 2, 1, 1, 1, 1, 1, 1};
	gosASSERT((8 > alignment) && (-1 <= alignment));
	pLast->appearance()->setObjectParameters(
		realPos, 0, false, alignment, homeRelations[alignment + 1]);
	eye->update();
	pLast->appearance()->update();
	pLast->appearance()->setInView(true);
	pLast->appearance()->rotation = 0;
	pLast->appearance()->teamId = alignment;
	return pLast;
}

//-------------------------------------------------------------------------------------------------
EditorObject*
EditorObjectMgr::getObjectAtPosition(const Stuff::Vector3D& position)
{
	Stuff::Vector4D screenPos;
	Stuff::Vector4D tmpPos;
	eye->projectZ(const_cast<Stuff::Vector3D&>(position), screenPos);
	for (BUILDING_LIST::EIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
	{
		ObjectAppearance* pAppearance = (*iter)->appearance();
		if (pAppearance->canBeSeen())
		{
			int32_t left, top, right, bottom;
			left = pAppearance->upperLeft.x;
			top = pAppearance->upperLeft.y;
			right = pAppearance->lowerRight.x;
			bottom = pAppearance->lowerRight.y;
			eye->projectZ(pAppearance->position, tmpPos);
			if (screenPos.x >= left && screenPos.x <= right && screenPos.y >= top && screenPos.y <= bottom && pAppearance->PerPolySelect(screenPos.x, screenPos.y))
			{
				return (*iter);
			}
		}
	}
	for (UNIT_LIST::EIterator mIter = units.Begin(); !mIter.IsDone(); mIter++)
	{
		ObjectAppearance* pAppearance = (*mIter)->appearance();
		if (pAppearance->canBeSeen())
		{
			int32_t left, top, right, bottom;
			left = pAppearance->upperLeft.x;
			top = pAppearance->upperLeft.y;
			right = pAppearance->lowerRight.x;
			bottom = pAppearance->lowerRight.y;
			eye->projectZ(pAppearance->position, tmpPos);
			if (screenPos.x >= left && screenPos.x <= right && screenPos.y >= top && screenPos.y <= bottom)
			{
				return (*mIter);
			}
		}
	}
	for (DROP_LIST::EIterator dIter = dropZones.Begin(); !dIter.IsDone(); dIter++)
	{
		// need to change this when we get drop zone art
		Stuff::Vector3D position = (*dIter)->getPosition();
		eye->projectZ(position, tmpPos);
		if (screenPos.x >= tmpPos.x - 3 && screenPos.x <= tmpPos.x + 3 && screenPos.y <= tmpPos.y + 3 && screenPos.y >= tmpPos.y - 3)
			return *dIter;
	}
	return nullptr;
}

//-------------------------------------------------------------------------------------------------
static bool
areCloseEnough(float f1, float f2)
{
	bool retval = false;
	float diff = fabs(f1 - f2);
	float bigger = fabs(f1);
	if (fabs(f2) > bigger)
	{
		bigger = fabs(f2);
	}
	float proportion_diff = 0.0;
	if (0.0 != bigger)
	{
		proportion_diff = diff / bigger;
	}
	if ((pow(10.0, -FLT_DIG) * 10.0 /*arbitrary*/) > proportion_diff)
	{
		retval = true;
	}
	return retval;
}

EditorObject*
EditorObjectMgr::getObjectAtLocation(float x, float y)
{
	for (BUILDING_LIST::EIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
	{
		if (areCloseEnough((*iter)->getPosition().x, x) && areCloseEnough((*iter)->getPosition().y, y))
		{
			return (*iter);
		}
	}
	for (UNIT_LIST::EIterator mIter = units.Begin(); !mIter.IsDone(); mIter++)
	{
		if (areCloseEnough((*mIter)->getPosition().x, x) && areCloseEnough((*mIter)->getPosition().y, y))
		{
			return (*mIter);
		}
	}
	for (DROP_LIST::EIterator dIter = dropZones.Begin(); !dIter.IsDone(); dIter++)
	{
		// need to change this when we get drop zone art
		if (areCloseEnough((*dIter)->getPosition().x, x) && areCloseEnough((*dIter)->getPosition().y, y))
		{
			return (*dIter);
		}
	}
	return nullptr;
}

//-------------------------------------------------------------------------------------------------
EditorObject*
EditorObjectMgr::addBuilding(const Stuff::Vector3D& position, uint32_t group,
	uint32_t indexWithinGroup, int32_t alignment, float rotation, float scale, bool bSnapToCell)
{
	if (group == 255)
		return addDropZone(position, alignment, 0);
	int32_t type = getType(group, indexWithinGroup);
	int32_t specialType = getSpecialType(getID(group, indexWithinGroup));
	EditorObject* info = nullptr;
	if (specialType == NAV_MARKER)
	{
		info = new NavMarker;
		buildings.Append(info);
		info->appearInfo->appearance = getAppearance(group, indexWithinGroup);
	}
	else if ((type == BLDG_TYPE) || (type == TREED_TYPE))
	{
		info = new EditorObject;
		buildings.Append(info);
		info->appearInfo->appearance = getAppearance(group, indexWithinGroup);
	}
	else if (type == GV_TYPE || type == MECH_TYPE)
	{
		Unit* tmp = new Unit(alignment);
		units.Append(tmp);
		info = tmp;
		info->appearInfo->appearance = getAppearance(group, indexWithinGroup);
		uint32_t c1, c2, c3;
		tmp->getcolours(c1, c2, c3);
		tmp->appearance()->resetPaintScheme(c1, c2, c3);
	}
	land->worldToCell(position, info->cellRow, info->cellColumn);
	info->id = type << 24 | group << 16 | indexWithinGroup << 8;
	Stuff::Vector3D realPos;
	land->getCellPos(info->cellRow, info->cellColumn, realPos);
	int32_t homeRelations[9] = {0, 0, 2, 1, 1, 1, 1, 1, 1};
	gosASSERT((8 > alignment) && (-1 <= alignment));
	Stuff::Vector3D pos = bSnapToCell ? realPos : position;
	info->appearance()->setObjectParameters(pos, 0, false, alignment, homeRelations[alignment + 1]);
	eye->update();
	info->appearance()->update();
	info->appearance()->setInView(true);
	info->appearance()->rotation = rotation;
	info->appearance()->teamId = isAlignable(info->id) ? alignment : EDITOR_TEAMNONE;
	if (scale != 1.0)
		info->appearance()->scale(scale);
	return info;
}

bool EditorObjectMgr::deleteBuilding(const EditorObject* pInfo)
{
	BuildingLink* pLink = getLinkWithParent(pInfo);
	if (pLink)
	{
		deleteLink(pLink);
	}
	{
		pLink = getLinkWithBuilding(pInfo);
		if (pLink)
		{
			pLink->RemoveObject(pInfo);
		}
	}
	if (buildings.Count() && pInfo)
	{
		for (BUILDING_LIST::EIterator iter = buildings.End(); !iter.IsDone(); iter--)
		{
			if (pInfo->cellColumn == (*iter)->cellColumn && pInfo->cellRow == (*iter)->cellRow && pInfo->appearance() == (*iter)->appearance())
			{
				delete (*iter);
				selectedObjects.RemoveIfThere(*iter);
				EditorData::instance->handleObjectInvalidation(*iter);
				buildings.Delete(iter);
				return true;
			}
		}
	}
	if (units.Count() && pInfo)
	{
		for (UNIT_LIST::EIterator mIter = units.End(); !mIter.IsDone(); mIter--)
		{
			if (pInfo->cellColumn == (*mIter)->cellColumn && pInfo->cellRow == (*mIter)->cellRow)
			{
				delete (*mIter);
				selectedObjects.RemoveIfThere(*mIter);
				EditorData::instance->handleObjectInvalidation(*mIter);
				units.Delete(mIter);
				return true;
			}
		}
	}
	for (DROP_LIST::EIterator dIter = dropZones.Begin(); !dIter.IsDone(); dIter++)
	{
		if (pInfo->cellColumn == (*dIter)->cellColumn && pInfo->cellRow == (*dIter)->cellRow)
		{
			delete (*dIter);
			selectedObjects.RemoveIfThere(*dIter);
			EditorData::instance->handleObjectInvalidation(*dIter);
			dropZones.Delete(dIter);
			return true;
		}
	}
	return false;
}

int32_t
EditorObjectMgr::getBuildingGroupCount(void) const
{
	return groups.Count();
}

int32_t
EditorObjectMgr::getNumberBuildingsInGroup(int32_t group) const
{
	return groups[group].buildings.Count();
}

void EditorObjectMgr::getBuildingGroupNames(std::wstring_view* names, int32_t& NumberOfNames) const
{
	if (NumberOfNames < groups.Count())
		NumberOfNames = groups.Count();
	int32_t i = 0;
	for (GROUP_LIST::EConstIterator iter = groups.Begin(); !iter.IsDone(); iter++, i++)
	{
		if (i >= NumberOfNames)
			return;
		names[i] = (*iter).name;
	}
}

void EditorObjectMgr::getBuildingNamesInGroup(
	int32_t group, std::wstring_view* names, int32_t& NumberOfNames) const
{
	group* pGroup = &groups[group];
	if (NumberOfNames < groups.Count())
		NumberOfNames = pGroup->buildings.Count();
	int32_t i = 0;
	for (EList<Building, Building&>::EIterator iter = pGroup->buildings.Begin(); !iter.IsDone();
		 iter++, i++)
	{
		if (i >= NumberOfNames)
			return;
		names[i] = (*iter).name;
	}
	NumberOfNames = i;
}

void EditorObjectMgr::update()
{
	static int32_t homeRelations[9] = {0, 0, 2, 1, 1, 1, 1, 1, 1};
	if (renderObjects)
	{
		for (BUILDING_LIST::EIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
		{
			currentFloatHelp = 0;
			if (renderTrees || (!renderTrees && ((*iter)->appearance()->getAppearanceClass() != TREE_APPR_TYPE)))
			{
				if ((*iter)->appearance()->recalcBounds())
				{
					Stuff::Vector3D pos = (*iter)->appearance()->position;
					int32_t cellR, cellC;
					land->worldToCell(pos, cellR, cellC);
					/*We're calling what appears to be a redundant
					moveBuilding() here to ensure that the objects are level
					with the terrain (incase the terrain got altered I guess).
					Note that moveBuilding() also updates any links that the
					building might be a part of.*/
					moveBuilding((*iter), cellR, cellC);
					pos = (*iter)->appearance()->position;
					gosASSERT((8 > (*iter)->appearance()->teamId) && (-1 <= (*iter)->appearance()->teamId));
					(*iter)->appearance()->setObjectParameters(pos, (*iter)->appearance()->rotation,
						(*iter)->appearance()->selected, (*iter)->appearance()->teamId,
						homeRelations[(*iter)->appearance()->teamId + 1]);
					(*iter)->appearance()->update();
					(*iter)->appearance()->setVisibility(true, true);
				}
			}
		}
	}
	// Always draw the Mechs/Vehicles
	for (UNIT_LIST::EIterator mIter = units.Begin(); !mIter.IsDone(); mIter++)
	{
		EditorObject* pObj = (*mIter);
		currentFloatHelp = 0;
		if (pObj->appearance()->recalcBounds())
		{
			// Vehicles and mech forced to ground in Object classes in game now.
			// NOT in the appearance anymore since they can fly when the die!
			// Must force here as well.
			Stuff::Vector3D pos = pObj->appearance()->position;
			pos.z = land->getTerrainElevation(pos);
			if (getIsHoverCraft(pObj->id))
			{
				float waterElevation = land->getWaterElevation();
				if (waterElevation > pos.z)
				{
					pos.z = waterElevation;
				}
			}
			gosASSERT((8 > pObj->appearance()->teamId) && (-1 <= pObj->appearance()->teamId));
			pObj->appearance()->setObjectParameters(pos, pObj->appearance()->rotation,
				pObj->appearance()->selected, pObj->appearance()->teamId,
				homeRelations[pObj->appearance()->teamId + 1]);
			if (pObj->getSpecialType() == HELICOPTER)
				pObj->appearance()->setMoverParameters(0.0f, 0.0f, 0.0f, true);
			pObj->appearance()->update();
			pObj->appearance()->setVisibility(true, true);
		}
	}
}

void EditorObjectMgr::render()
{
	//--------------------------------
	// Set States for Software Renderer
	if (Environment.Renderer == 3)
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_MonoEnable, 1);
		gos_SetRenderState(gos_State_Perspective, 0);
		gos_SetRenderState(gos_State_Clipping, 1);
		gos_SetRenderState(gos_State_Specular, 0);
		gos_SetRenderState(gos_State_Dither, 0);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureWrap);
		gos_SetRenderState(gos_State_ZCompare, 1);
		gos_SetRenderState(gos_State_ZWrite, 1);
	}
	//--------------------------------
	// Set States for Hardware Renderer
	else
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_MonoEnable, 0);
		gos_SetRenderState(gos_State_Perspective, 1);
		gos_SetRenderState(gos_State_Clipping, 1);
		gos_SetRenderState(gos_State_Specular, 1);
		gos_SetRenderState(gos_State_Dither, 1);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureWrap);
		gos_SetRenderState(gos_State_ZCompare, 1);
		gos_SetRenderState(gos_State_ZWrite, 1);
	}
	if (renderObjects)
	{
		for (BUILDING_LIST::EIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
		{
			currentFloatHelp = 0;
			if (renderTrees || (!renderTrees && ((*iter)->appearance()->getAppearanceClass() != TREE_APPR_TYPE)))
			{
				if ((*iter)->appearance()->recalcBounds())
				{
					if ((*iter)->getDamage())
						(*iter)->appearance()->drawBars();
					(*iter)->appearance()->render();
					if ((*iter)->getcolour() & 0xff000000)
						(*iter)->appearance()->drawSelectBrackets((*iter)->getcolour());
				}
			}
		}
	}
	// Always draw the Mechs/Vehicles
	for (UNIT_LIST::EIterator mIter = units.Begin(); !mIter.IsDone(); mIter++)
	{
		EditorObject* pObj = (*mIter);
		currentFloatHelp = 0;
		if (pObj->appearance()->recalcBounds())
		{
			pObj->appearance()->render();
			if ((*mIter)->getDamage())
				pObj->appearance()->drawBars();
			if ((*mIter)->getcolour() & 0xff000000)
				pObj->appearance()->drawSelectBrackets(pObj->getcolour());
		}
	}
	// draw the building links
	for (LINK_LIST::EIterator lIter = links.Begin(); !lIter.IsDone(); lIter++)
	{
		(*lIter)->render();
	}
	// draw the drop zones -- this will change as soon as we get art
	for (DROP_LIST::EIterator dIter = dropZones.Begin(); !dIter.IsDone(); dIter++)
	{
		Stuff::Vector3D pos = (*dIter)->getPosition();
		Stuff::Vector4D screen;
		eye->projectZ(pos, screen);
		RECT Rect;
		Rect.top = screen.y - 3;
		Rect.left = screen.x - 3;
		Rect.bottom = screen.y + 3;
		Rect.right = screen.x + 3;
		drawRect(Rect, (*dIter)->isVTol() ? 0xff0000ff : 0xffffff00);
	}
}

void EditorObjectMgr::renderShadows()
{
	//-----------------------------------------------------
	// Set render states as few times as possible.
	//--------------------------------
	// Set States for Software Renderer
	if (Environment.Renderer == 3)
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeFlat);
		gos_SetRenderState(gos_State_MonoEnable, 1);
		gos_SetRenderState(gos_State_Perspective, 0);
		gos_SetRenderState(gos_State_Clipping, 1);
		gos_SetRenderState(gos_State_AlphaTest, 0);
		gos_SetRenderState(gos_State_Specular, 0);
		gos_SetRenderState(gos_State_Dither, 0);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendDecal);
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureWrap);
		gos_SetRenderState(gos_State_ZCompare, 0);
		gos_SetRenderState(gos_State_ZWrite, 0);
	}
	//--------------------------------
	// Set States for Hardware Renderer
	else
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeFlat);
		gos_SetRenderState(gos_State_MonoEnable, 1);
		gos_SetRenderState(gos_State_Perspective, 0);
		gos_SetRenderState(gos_State_Clipping, 1);
		gos_SetRenderState(gos_State_AlphaTest, 1);
		gos_SetRenderState(gos_State_Specular, 0);
		gos_SetRenderState(gos_State_Dither, 1);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureWrap);
		gos_SetRenderState(gos_State_ZCompare, 1);
		gos_SetRenderState(gos_State_ZWrite, 1);
	}
	if (renderObjects)
	{
		for (BUILDING_LIST::EIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
		{
			if (renderTrees || (!renderTrees && ((*iter)->appearance()->getAppearanceClass() != TREE_APPR_TYPE)))
			{
				if ((*iter)->appearance()->recalcBounds())
				{
					//(*iter)->appearance()->update();
					//(*iter)->appearance()->setVisibility(true,true);
					(*iter)->appearance()->renderShadows();
					//(*iter)->appearance()->render();
				}
			}
		}
	}
	// Always draw the Mechs/Vehicles
	for (UNIT_LIST::EIterator mIter = units.Begin(); !mIter.IsDone(); mIter++)
	{
		if ((*mIter)->appearance()->recalcBounds())
		{
			(*mIter)->appearance()->renderShadows();
		}
	}
}

bool EditorObjectMgr::save(PacketFile& PakFile, int32_t whichPacket)
{
	File file;
	if (!buildings.Count())
	{
		PakFile.writePacket(whichPacket, nullptr);
		PakFile.writePacket(whichPacket + 1, nullptr);
	}
	int32_t bufferSize =
		buildings.Count() * (4 * sizeof(float) + 6 * sizeof(int32_t)) + 2 * sizeof(int32_t);
	std::wstring_view pBuffer = (std::wstring_view)malloc(bufferSize);
	int32_t* pTacMapPoints = (int32_t*)malloc(sizeof(int32_t) * (buildings.Count() * 2 + 1));
	int32_t* pPoints = pTacMapPoints + 1;
	int32_t pointCounter = 0;
	file.open(pBuffer, bufferSize);
	file.writeLong(buildings.Count());
	gos_VERTEX point;
	for (BUILDING_LIST::EIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
	{
		int32_t id = (*iter)->id;
		Building& bldg = groups[(id >> 16) & 0x000000ff].buildings[(id >> 8) & 0x000000ff];
		if (bldg.writeOnTacMap)
		{
			TacMap::worldToTacMap((*iter)->appearance()->position, 0, 0, 115, 115, point);
			*pPoints++ = point.x;
			*pPoints++ = point.y;
			pointCounter++;
		}
		file.writeLong(bldg.fitID);
		file.writeFloat((*iter)->appearance()->position.x);
		file.writeFloat((*iter)->appearance()->position.y);
		file.writeFloat((*iter)->appearance()->position.z);
		file.writeFloat((*iter)->appearance()->rotation);
		file.writeLong((*iter)->appearance()->damage);
		file.writeLong((*iter)->appearance()->teamId);
		BuildingLink* pLink = getLinkWithChild(*iter);
		if (pLink)
		{
			Stuff::Vector3D pos = pLink->GetParentPosition();
			// convert to cells
			int32_t cellI, cellJ;
			land->worldToCell(pos, cellJ, cellI);
			int32_t val = (cellJ << 16) | cellI;
			// How about BEFORE we write this, we make sure there's really a
			// building there!
			EditorObject* pObject = getObjectAtCell(cellJ, cellI);
			if (!pObject)
			{
				// Dissolve the link here.  Maybe we should warn?
				val = 0xffffffff;
				PAUSE(("Turret/Gate linked to bldg @ R %d, C %d  X:%f Y:%f No "
					   "Bldg there!  Link Deleted",
					cellJ, cellI, pos.x, pos.y));
			}
			file.writeLong(val);
		}
		else
			file.writeLong(0xffffffff);
		// forest stuff
		file.writeLong((*iter)->getForestID() + 1);
		if ((*iter)->getForestID() != -1)
			file.writeLong((*iter)->getScale());
		else
			file.writeLong(0);
	}
	int32_t pos = file.getLogicalPosition();
	file.seek(0);
	if (!PakFile.writePacket(whichPacket, (uint8_t*)pBuffer, pos))
	{
		gosASSERT(false);
	}
	*pTacMapPoints = pointCounter;
	PakFile.writePacket(
		whichPacket + 1, (uint8_t*)pTacMapPoints, (pointCounter * 2 + 1) * sizeof(int32_t));
	free(pTacMapPoints);
	free(pBuffer);
	if (!justResaveAllMaps)
	{
		int32_t numTurrets = 0;
		for (BUILDING_LIST::EIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
		{
			switch ((*iter)->getSpecialType())
			{
			case EDITOR_TURRET:
			{
				numTurrets += 1;
			}
			break;
			}
		}
		if (64 /*max turrets*/ < numTurrets)
		{
			CString CStr;
			CStr.Format(IDS_TOO_MANY_TURRETS, numTurrets);
			AfxMessageBox(CStr);
		}
	}
	return true;
}

bool EditorObjectMgr::load(PacketFile& PakFile, int32_t whichPacket)
{
	PakFile.seekPacket(whichPacket);
	int32_t size = PakFile.getPacketSize();
	std::wstring_view pBuffer = (std::wstring_view)malloc(size);
	PakFile.readPacket(whichPacket, (uint8_t*)pBuffer);
	File file;
	file.open(pBuffer, size);
	int32_t count = file.readLong();
	for (size_t i = 0; i < count; ++i)
	{
		int32_t id = file.readLong();
		uint32_t group, index;
		if (!getBuildingFromID(id, group, index, false))
		{
			// Until buildings stabilize, just toss 'em when you can't find 'em
			// -fs
			// Assert( false, 0, "Couldn't find the fit id in the building
			// list\n" );
			continue;
		}
		Stuff::Vector3D vector;
		vector.x = file.readFloat();
		vector.y = file.readFloat();
		vector.z = file.readFloat();
		float rotation = file.readFloat();
		EditorObject* pInfo =
			(EditorObject*)addBuilding(vector, group, index, EDITOR_TEAMNONE, rotation);
		if (pInfo)
			((EditorObject*)pInfo)->appearance()->rotation = rotation;
		int32_t damage = file.readLong();
		if (damage)
			((EditorObject*)pInfo)->appearance()->setDamage(1);
		pInfo->appearance()->teamId = file.readLong();
		// Most everything can have an alignment now
		// -fs  6/14/2000
		//		if ( !isAlignable( pInfo->id ) )
		//			pInfo->appearance()->teamId = -1;
		int32_t cells = file.readLong();
		if (cells && (cells != 0xffffffff)) // this building is linked
		{
			int32_t cellJ = cells >> 16;
			int32_t cellI = cells & 0x0000ffff;
			Stuff::Vector3D pos;
			land->cellToWorld(cellJ, cellI, pos);
			pos.z = land->getTerrainElevation(pos);
			bool bFound = false;
			for (LINK_LIST::EIterator iter = links.Begin(); !iter.IsDone(); iter++)
			{
				if ((*iter)->GetParentPosition() == pos)
				{
					// we've got the link
					(*iter)->AddChild(pInfo);
					bFound = true;
					break;
				}
			}
			if (!bFound)
			{
				BuildingLink* pLink = new BuildingLink(pos);
				pLink->AddChild(pInfo);
				links.Append(pLink);
			}
		}
		// forest stuff
		pInfo->setForestID(file.readLong() - 1);
		int32_t scale = file.readLong();
		if (pInfo->getForestID() != -1)
			pInfo->setScale(scale);
	}
	LINK_LIST linksToDelete;
	// gotta go through all the links and make sure they have id's
	for (LINK_LIST::EIterator iter = links.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->parent.m_ID == -1)
		{
			// This absolutely positively could never have worked!
			// Why?  Because getObjectAtPosition depends upon the object BEING
			// ON SCREEN!!!!!!! At this point, we're still loading!!! We should,
			// instead, be checking getObjectAtCell which ALWAYS works! -fs
			int32_t cellR, cellC;
			land->worldToCell((*iter)->parent.pos, cellR, cellC);
			EditorObject* pObject = getObjectAtCell(cellR, cellC);
			gosASSERT(pObject);
			if (pObject)
				(*iter)->parent.m_ID = pObject->getID();
			else
				linksToDelete.Append(*iter);
		}
	}
	for (iter = linksToDelete.Begin(); !iter.IsDone(); iter++)
	{
		deleteLink(*iter);
	}
	free(pBuffer);
	return true;
}

// this will NOT RETURN MECHS AND VEHICLES
bool EditorObjectMgr::getBuildingFromID(
	int32_t fitID, uint32_t& group, uint32_t& index, bool canBeMech)
{
	group = 0;
	index = 0;
	for (GROUP_LIST::EIterator groupIter = groups.Begin(); !groupIter.IsDone(); groupIter++)
	{
		index = 0;
		for (EList<Building, Building&>::EIterator buildIter = (*groupIter).buildings.Begin();
			 !buildIter.IsDone(); buildIter++)
		{
			if ((*buildIter).fitID == fitID && (canBeMech || (*buildIter).type != GV_TYPE && (*buildIter).type != MECH_TYPE))
			{
				return true;
			}
			index++;
		}
		group++;
	}
	return false;
}

int32_t
EditorObjectMgr::getFitID(int32_t id) const
{
	return groups[getGroup(id)].buildings[getIndexInGroup(id)].fitID;
}

bool EditorObjectMgr::getBlocksLineOfFire(int32_t id) const
{
	return groups[getGroup(id)].buildings[getIndexInGroup(id)].blocksLineOfFire;
}
bool EditorObjectMgr::getIsHoverCraft(int32_t id) const
{
	return groups[getGroup(id)].buildings[getIndexInGroup(id)].isHoverCraft;
}
int64_t
EditorObjectMgr::getImpassability(int32_t id)
{
	return groups[getGroup(id)].buildings[getIndexInGroup(id)].impassability;
}

void EditorObjectMgr::select(const Stuff::Vector4D& pos1, const Stuff::Vector4D& pos2)
{
	int32_t xMin, xMax;
	int32_t yMin, yMax;
	if (pos1.x < pos2.x)
	{
		xMin = pos1.x;
		xMax = pos2.x;
	}
	else
	{
		xMin = pos2.x;
		xMax = pos1.x;
	}
	if (pos1.y < pos2.y)
	{
		yMin = pos1.y;
		yMax = pos2.y;
	}
	else
	{
		yMin = pos2.y;
		yMax = pos1.y;
	}
	Stuff::Vector4D screenPos;
	for (BUILDING_LIST::EIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
	{
		eye->projectZ((*iter)->appearance()->position, screenPos);
		if (screenPos.x >= xMin && screenPos.x <= xMax && screenPos.y >= yMin && screenPos.y <= yMax)
		{
			(*iter)->appearance()->selected = true;
			selectedObjects.AddUnique(*iter);
		}
	}
	for (UNIT_LIST::EIterator mIter = units.Begin(); !mIter.IsDone(); mIter++)
	{
		eye->projectZ((*mIter)->appearance()->position, screenPos);
		if (screenPos.x >= xMin && screenPos.x <= xMax && screenPos.y >= yMin && screenPos.y <= yMax)
		{
			(*mIter)->appearance()->selected = true;
			selectedObjects.AddUnique(*mIter);
		}
	}
	for (DROP_LIST::EIterator dIter = dropZones.Begin(); !dIter.IsDone(); dIter++)
	{
		Stuff::Vector3D pos = (*dIter)->getPosition();
		eye->projectZ(pos, screenPos);
		if (screenPos.x >= xMin && screenPos.x <= xMax && screenPos.y >= yMin && screenPos.y <= yMax)
		{
			(*dIter)->appearance()->selected = true;
			selectedObjects.AddUnique(*dIter);
		}
	}
}

void EditorObjectMgr::select(EditorObject& object, bool bSelect)
{
	// we'll just assume it's valid
	object.appearance()->selected =
		bSelect ? DRAW_BARS | DRAW_BRACKETS | DRAW_TEXT : 0; // hack, 6 = draw bars, text,
	if (true == bSelect)
	{
		selectedObjects.AddUnique(&object);
	}
	else
	{
		selectedObjects.RemoveIfThere(&object);
	}
}

void EditorObjectMgr::unselectAll()
{
	// mh: at somepoint we can change the code so that it iterates over the
	// selectedObjects list instead of the lists of all the buildings and units
	for (BUILDING_LIST::EIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
	{
		(*iter)->appearance()->selected = false;
	}
	for (UNIT_LIST::EIterator mIter = units.Begin(); !mIter.IsDone(); mIter++)
	{
		(*mIter)->appearance()->selected = false;
	}
	for (DROP_LIST::EIterator dIter = dropZones.Begin(); !dIter.IsDone(); dIter++)
		(*dIter)->appearance()->selected = false;
	selectedObjects.Clear();
}

bool EditorObjectMgr::hasSelection()
{
#if 1
	syncSelectedObjectPointerList();
	if (0 < selectedObjects.Count())
	{
		return true;
	}
#else
	for (BUILDING_LIST::EConstIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->appearance()->selected)
			return true;
	}
	for (UNIT_LIST::EConstIterator mIter = units.Begin(); !mIter.IsDone(); mIter++)
	{
		if ((*mIter)->appearance()->selected)
			return true;
	}
#endif
	return false;
}

int32_t
EditorObjectMgr::getSelectionCount()
{
#if 1
	syncSelectedObjectPointerList();
	return selectedObjects.Count();
#else
	int32_t counter = 0;
	for (BUILDING_LIST::EConstIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->appearance()->selected)
			counter++;
	}
	for (UNIT_LIST::EConstIterator mIter = units.Begin(); !mIter.IsDone(); mIter++)
	{
		if ((*mIter)->appearance()->selected)
			counter++;
	}
	return counter;
#endif
}

EditorObjectMgr::EDITOR_OBJECT_LIST
EditorObjectMgr::getSelectedObjectList()
{
	syncSelectedObjectPointerList();
	return selectedObjects;
}

void EditorObjectMgr::syncSelectedObjectPointerList()
{
	if (!selectedObjectsNeedsToBeSynched)
	{
		return;
	}
	selectedObjects.Clear();
	for (BUILDING_LIST::EConstIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->appearance()->selected)
			selectedObjects.Append(*iter);
	}
	for (UNIT_LIST::EConstIterator mIter = units.Begin(); !mIter.IsDone(); mIter++)
	{
		if ((*mIter)->appearance()->selected)
			selectedObjects.Append(*mIter);
	}
	for (DROP_LIST::EIterator dIter = dropZones.Begin(); !dIter.IsDone(); dIter++)
	{
		if ((*dIter)->appearance()->selected)
			selectedObjects.Append(*dIter);
	}
	selectedObjectsNeedsToBeSynched = false;
}

void EditorObjectMgr::deleteSelectedObjects()
{
	for (BUILDING_LIST::EIterator iter = buildings.End(); !iter.IsDone();)
	{
		if ((*iter)->appearance()->selected)
		{
			BUILDING_LIST::EIterator tmpIter = iter;
			iter--;
			buildings.Delete(tmpIter);
		}
		else
			iter--;
	}
	for (UNIT_LIST::EIterator uIter = units.End(); !uIter.IsDone();)
	{
		if ((*uIter)->appearance()->selected)
		{
			UNIT_LIST::EIterator tmpIter = uIter;
			uIter--;
			units.Delete(tmpIter);
		}
		else
			uIter--;
	}
	for (DROP_LIST::EIterator dIter = dropZones.End(); !dIter.IsDone();)
	{
		if ((*dIter)->appearance()->selected)
		{
			DROP_LIST::EIterator tmpIter = dIter;
			dIter--;
			dropZones.Delete(tmpIter);
		}
		else
			dIter--;
	}
	selectedObjects.Clear();
}

void EditorObjectMgr::adjustObjectsToNewTerrainheights()
{
	for (BUILDING_LIST::EIterator iter = buildings.End(); !iter.IsDone(); iter++)
	{
		float Z = land->getTerrainElevation((*iter)->appearance()->position);
		(*iter)->appearance()->position.z = Z;
		(*iter)->appearance()->update();
	}
	for (LINK_LIST::EIterator lIter = links.Begin(); !lIter.IsDone(); lIter++)
	{
		(*lIter)->Fixheights();
	}
	for (UNIT_LIST::EIterator uIter = units.Begin(); !uIter.IsDone(); uIter++)
	{
		float Z = land->getTerrainElevation((*uIter)->appearance()->position);
		(*uIter)->appearance()->position.z = Z;
		(*uIter)->appearance()->update();
	}
}

ObjectAppearance*
EditorObjectMgr::getAppearance(EditorObjectMgr::Building* pBuilding)
{
	ObjectAppearance* appearance = nullptr;
	if (!appearanceTypeList)
	{
		appearanceTypeList = new AppearanceTypeList;
		gosASSERT(appearanceTypeList != nullptr);
		appearanceTypeList->init(2048000);
	}
	// MUST ALWAYS CALL GET, EVEN IF WE HAVE AN APPEARANCE TYPE OR REFERENCE
	// COUNT DOES NOT INCREASE!
	pBuilding->appearanceType =
		appearanceTypeList->getAppearance(pBuilding->type << 24, pBuilding->fileName);
	if (pBuilding->type == BLDG_TYPE)
		appearance = new BldgAppearance;
	else if (pBuilding->type == MECH_TYPE)
		appearance = new Mech3DAppearance;
	else if (pBuilding->type == GV_TYPE)
		appearance = new GVAppearance;
	else
		appearance = new TreeAppearance;
	appearance->init(pBuilding->appearanceType);
	appearance->setObjectNameId(pBuilding->nameID);
	return appearance;
}

ObjectAppearance*
EditorObjectMgr::getAppearance(uint32_t group, uint32_t indexWithinGroup)
{
	gosASSERT(group >= 0 && group < groups.Count());
	group* pGroup = &groups[group];
	gosASSERT(pGroup->buildings.Count() > indexWithinGroup);
	Building* pBuilding = &pGroup->buildings[indexWithinGroup];
	return getAppearance(pBuilding);
}

bool EditorObjectMgr::loadMechs(FitIniFile& file)
{
	// disable animation loading to decrease loading time
	Mech3DAppearanceType::disableAnimationLoading();
	uint32_t count;
	file.seekBlock("Parts");
	file.readIdULong("NumParts", count);
	int32_t alternativeInstancesCounter = count + 1;
	Stuff::Vector3D position;
	position.x = position.y = position.z = 0;
	wchar_t buffer[256];
	for (size_t i = 1; i < count + 1; ++i)
	{
		sprintf(buffer, "Part%ld", i);
		file.seekBlock(buffer);
		uint32_t fitID;
		file.readIdULong("ObjectNumber", fitID);
		uint32_t group, index;
		getBuildingFromID(fitID, group, index, true);
		EditorObject* pObject = (EditorObject*)(addBuilding(position, group, index, 0, 0));
		gosASSERT(dynamic_cast<Unit*>(pObject));
		pObject->load(&file, i);
		alternativeInstancesCounter += 1;
		int32_t tmpI = ((Unit*)(pObject))->tmpAlternativeStartIndex;
		if (0 < tmpI)
		{
			alternativeInstancesCounter = tmpI;
		}
		int32_t j;
		for (j = 0; j < ((Unit*)(pObject))->tmpNumAlternativeInstances; j += 1)
		{
			sprintf(buffer, "Part%ld", alternativeInstancesCounter);
			file.seekBlock(buffer);
			uint32_t fitID;
			file.readIdULong("ObjectNumber", fitID);
			uint32_t group, index;
			getBuildingFromID(fitID, group, index, true);
			EditorObject* pAltObject = (EditorObject*)(addBuilding(position, group, index, 0, 0));
			gosASSERT(dynamic_cast<Unit*>(pAltObject));
			pAltObject->load(&file, alternativeInstancesCounter);
			((Unit*)(pObject))->pAlternativeInstances->Append(*((Unit*)(pAltObject)));
			deleteBuilding(pAltObject);
			alternativeInstancesCounter += 1;
		}
	}
	return true;
}

bool EditorObjectMgr::saveMechs(FitIniFile& file)
{
	file.writeBlock("Warriors");
	UNIT_LIST unitsByPlayer[8 /*max players*/];
	{
		for (UNIT_LIST::EIterator iter = units.Begin(); !iter.IsDone(); iter++)
		{
			int32_t alignment = (*iter)->getAlignment();
			if (8 /*max players*/ > alignment)
			{
				unitsByPlayer[alignment].Append((*iter));
			}
			else
			{
				_ASSERT(false);
			}
		}
	}
	UNIT_LIST reorderedUnits;
	UNIT_LIST reorderedUnitsByPlayer[8 /*max players*/];
	for (size_t playerNum = 0; 8 /*max players*/ > playerNum; playerNum += 1)
	{
		/*O(n-squared) sort. Who cares.*/
		int32_t numUnitsOfThisPlayer = unitsByPlayer[playerNum].Count();
		for (size_t i = 0; i < numUnitsOfThisPlayer; i += 1)
		{
			int32_t lowestSquadNum = INT_MAX;
			UNIT_LIST::EIterator iter;
			for (iter = unitsByPlayer[playerNum].Begin(); !iter.IsDone(); iter++)
			{
				if ((*iter)->getSquad() < lowestSquadNum)
				{
					lowestSquadNum = (*iter)->getSquad();
				}
			}
			for (iter = unitsByPlayer[playerNum].Begin(); !iter.IsDone(); iter++)
			{
				if (lowestSquadNum == (*iter)->getSquad())
				{
					break;
				}
			}
			if (!iter.IsDone())
			{
				reorderedUnitsByPlayer[playerNum].Append(*iter);
				/*we delete the unit from unitsByPlayer[playerNum] because more
				than one unit may have the same squad number*/
				unitsByPlayer[playerNum].Delete(iter);
			}
			else
			{
				_ASSERT(false);
			}
		}
		UNIT_LIST::EIterator iter;
		for (iter = reorderedUnitsByPlayer[playerNum].Begin(); !iter.IsDone(); iter++)
		{
			reorderedUnits.Append(*iter);
		}
	}
	_ASSERT(reorderedUnits.Count() == units.Count());
	{
		/*When saving, each player's units need to be grouped into "lances" of
		12 units for historical reasons.*/
		for (size_t playerNum = 0; 8 /*max players*/ > playerNum; playerNum += 1)
		{
			int32_t lanceGroup = 0;
			int32_t mateIndex = 0;
			UNIT_LIST::EIterator iter;
			for (iter = reorderedUnitsByPlayer[playerNum].Begin(); !iter.IsDone(); iter++)
			{
				(*iter)->setLanceInfo(lanceGroup, mateIndex);
				mateIndex += 1;
				if (12 /*just twelve*/ >= mateIndex)
				{
					mateIndex = 0;
					lanceGroup += 1;
				}
			}
		}
	}
	/*
	if ( 1 > reorderedUnitsByPlayer[EDITOR_TEAM1].Count() )
	{
		EMessageBox( IDS_MUST_HAVE_PLAYER, IDS_CANT_SAVE, MB_OK );
		return false;
	}
	*/
	if (!justResaveAllMaps)
	{
		if (EditorData::instance->IsSinglePlayer())
		{
			bool enemyUnitFound = false;
			for (size_t playerNum = 1 /*first non-user player*/; 8 /*max players*/ > playerNum;
				 playerNum += 1)
			{
				if (0 /*player1's (user's) team*/ != EditorData::instance->PlayersRef().PlayerRef(playerNum).DefaultTeam())
				{
					if (0 < reorderedUnitsByPlayer[playerNum].Count())
					{
						enemyUnitFound = true;
						break;
					}
				}
			}
			if (!enemyUnitFound)
			{
				EMessageBox(IDS_NO_ENEMY_UNITS, IDS_WARNING, MB_OK);
			}
			if (0 >= reorderedUnitsByPlayer[0 /*user player*/].Count())
			{
				EMessageBox(IDS_NO_PLAYER_UNITS, IDS_WARNING, MB_OK);
			}
		}
		else
		{
			for (size_t playerNum = 0; EditorData::instance->MaxPlayers() > playerNum;
				 playerNum += 1)
			{
				if (1 > reorderedUnitsByPlayer[playerNum].Count())
				{
					EMessageBox(IDS_PLAYERS_WITH_NO_UNITS, IDS_WARNING, MB_OK);
					break;
				}
			}
		}
	}
	file.writeIdULong("NumWarriors", units.Count());
	if (!justResaveAllMaps && (units.Count() > 104))
	{
		EMessageBox(IDS_TOO_MANY_PILOTS, IDS_WARNING, MB_OK);
	}
	file.writeBlock("Parts");
	file.writeIdULong("NumParts", units.Count());
	// file.writeIdBoolean( "AlliedTeam", alignmentCount[NONE] ? true : false );
	wchar_t buffer[256];
	int32_t counter = 1; // for some unknown reason we index from 1
	int32_t alternativeInstancesCounter = reorderedUnits.Count() + 1;
	UNIT_LIST::EIterator iter;
	for (iter = reorderedUnits.Begin(); !iter.IsDone(); counter++, iter++)
	{
		sprintf(buffer, "Warrior%ld", counter);
		file.writeBlock(buffer);
		// ARM
		// Make a new name of the form "level23_warrior8"
		wchar_t armName[512] = {0};
		strcpy(armName, file.getFilename());
		_strlwr(armName);
		std::wstring_view fitExt = strstr(armName, ".fit");
		*fitExt = '_';
		sprintf(fitExt + 1, "warrior%02d", counter);
		mapAsset->AddRelationship("warrior", armName);
		IProviderAssetPtr mechAssetPtr =
			armProvider->OpenAsset(armName, AssetType_Virtual, ProviderType_Primary);
		mechAsset = (IProviderAsset*)mechAssetPtr;
		(*iter)->save(&file, counter);
		mechAssetPtr->Close();
		mechAssetPtr = nullptr;
		mechAsset = nullptr;
		file.writeIdULong("AlternativeStartIndex", alternativeInstancesCounter + 1);
		int32_t IndicesOfAlternatives[15 /*max alternatives*/] = {
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
		int32_t numAlternatives = (*iter)->pAlternativeInstances->Count();
		if (15 /*max alternatives*/ < numAlternatives)
		{
			gosASSERT(false);
			numAlternatives = 15 /*max alternatives*/;
		}
		int32_t j;
		for (j = 0; j < numAlternatives; j++)
		{
			IndicesOfAlternatives[j] = (alternativeInstancesCounter + 1) + j;
		}
		file.writeIdLongArray("IndicesOfAlternatives", IndicesOfAlternatives, 15);
		alternativeInstancesCounter += 1;
		int32_t i;
		for (i = 0; i < (*iter)->pAlternativeInstances->Count(); i++)
		{
			sprintf(buffer, "Warrior%ld", alternativeInstancesCounter);
			file.writeBlock(buffer);
			(*((*iter)->pAlternativeInstances))[i].save(&file, alternativeInstancesCounter);
			alternativeInstancesCounter += 1;
		}
	}
	// you should be able to get the force groups frm the units themselves
	file.writeBlock("Teams");
	// bool alliedFlag = alignmentCount[2] || alignmentCount[3] ||
	// alignmentCount[4] || alignmentCount[5] || alignmentCount[6] ||
	// alignmentCount[7];  file.writeIdBoolean( "AlliedTeam", alliedFlag ? true :
	// false );
	{
		/*When saving, each player's units need to be grouped into "lances" of
		12 units for historical reasons.*/
		int32_t reorderedUnitIndex = 1; /*1-based indexing by convention*/
		for (size_t playerNum = 0; 8 /*max players*/ > playerNum; playerNum += 1)
		{
			int32_t reorderedUnitByPlayerIndex = 0;
			int32_t lanceGroup = 0;
			for (lanceGroup = 0;
				 lanceGroup * (12 /*just twelve*/) < reorderedUnitsByPlayer[playerNum].Count();
				 lanceGroup += 1)
			{
				int32_t mates[12 /*just twelve*/];
				int32_t mateIndex = 0;
				for (mateIndex = 0; 12 /*just twelve*/ > mateIndex; mateIndex += 1)
				{
					if (reorderedUnitByPlayerIndex < reorderedUnitsByPlayer[playerNum].Count())
					{
						mates[mateIndex] = reorderedUnitIndex;
						reorderedUnitIndex += 1;
						reorderedUnitByPlayerIndex += 1;
					}
					else
					{
						mates[mateIndex] = 0;
					}
				}
				std::wstring_view commanderBlock;
				commanderBlock.Format("Commander%ldGroup:%ld", playerNum, lanceGroup);
				file.writeBlock(commanderBlock.Data());
				file.writeIdLongArray("Mates", &(mates[0]), 12);
			}
		}
	}
	return true;
}

bool EditorObjectMgr::saveForests(FitIniFile& file)
{
	int32_t counter = 0;
	wchar_t header[256];
	for (FOREST_LIST::EIterator iter = forests.Begin(); !iter.IsDone(); iter++, counter++)
	{
		sprintf(header, "Forest%ld", counter);
		file.writeBlock(header);
		(*iter)->save(file);
	}
	return true;
}

bool EditorObjectMgr::loadForests(FitIniFile& file)
{
	int32_t counter = 0;
	wchar_t header[256];
	int32_t tmp;
	while (true)
	{
		sprintf(header, "Forest%ld", counter);
		if (NO_ERROR != file.seekBlock(header))
			break;
		file.readIdString("filename", header, 255);
		file.readIdLong("ID", tmp);
		Forest* pForest = new Forest(tmp);
		pForest->setFileName(header);
		pForest->init(file);
		pForest->ID = counter;
		forests.Append(pForest);
		counter++;
	}
	return true;
}

int32_t
EditorObjectMgr::getNextAvailableForestID()
{
	int32_t max = -1;
	for (FOREST_LIST::EIterator iter = forests.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getID() > max)
		{
			max = (*iter)->getID();
		}
	}
	int32_t ID = max + 1;
	return ID;
}

int32_t
EditorObjectMgr::createForest(const Forest& forest)
{
	int32_t ID = getNextAvailableForestID();
	Forest* pForest = new Forest(ID);
	CString oldForestName = pForest->getFileName();
	*pForest = forest;
	pForest->ID = ID;
	if (!strstr(pForest->getFileName(), ".fit"))
		pForest->setFileName(oldForestName);
	forests.Append(pForest);
	// here I need to go about adding the trees -- will do in a sec
	doForest(*pForest);
	return ID;
}

void EditorObjectMgr::editForest(int32_t& oldID, const Forest& forest)
{
	land->unselectAll();
	for (BUILDING_LIST::EIterator bIter = buildings.End(); !bIter.IsDone();)
	{
		if ((*bIter)->getForestID() == oldID)
		{
			int32_t cellI, cellJ;
			(*bIter)->getCells(cellJ, cellI);
			int32_t tileI = cellI / 3;
			int32_t tileJ = cellJ / 3;
			land->selectVertex(tileJ, tileI, true);
			delete *bIter;
			BUILDING_LIST::EIterator tmpIter = bIter;
			bIter--;
			buildings.Delete(tmpIter);
		}
		else
			bIter--;
	}
	// update the forest object
	for (FOREST_LIST::EIterator iter = forests.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getID() == oldID)
		{
			*(*iter) = forest;
			break;
		}
	}
	doForest(*(*iter));
	land->unselectAll();
}

void EditorObjectMgr::removeForest(const Forest& forest)
{
	unselectAll();
	for (BUILDING_LIST::EIterator bIter = buildings.End(); !bIter.IsDone();)
	{
		if ((*bIter)->getForestID() == forest.getID())
		{
			delete *bIter;
			BUILDING_LIST::EIterator tmpIter = bIter;
			bIter--;
			buildings.Delete(tmpIter);
		}
		else
		{
			bIter--;
		}
	}
	// now remove the forest from the list
	for (FOREST_LIST::EIterator iter = forests.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getID() == forest.getID())
		{
			delete (*iter);
			forests.Delete(iter);
			return;
		}
	}
}

void EditorObjectMgr::selectForest(int32_t ID)
{
	unselectAll();
	for (BUILDING_LIST::EIterator bIter = buildings.Begin(); !bIter.IsDone(); bIter++)
	{
		if ((*bIter)->getForestID() == ID)
		{
			select(*(*bIter), true);
		}
	}
}

void EditorObjectMgr::doForest(const Forest& forest)
{
	float centerX = forest.centerX;
	float centerY = forest.centerY;
	float maxDensity = forest.maxDensity / 9;
	float minDensity = forest.minDensity / 9;
	int32_t probabilities[FOREST_TYPES];
	int32_t probabilityTotal = 0;
	for (size_t i = 0; i < FOREST_TYPES; i++)
	{
		probabilityTotal += forest.percentages[i];
	}
	for (i = 0; i < FOREST_TYPES; i++)
	{
		probabilities[i] = forest.percentages[i] / probabilityTotal * 100;
		if (i > 0)
		{
			probabilities[i] = probabilities[i - 1] + probabilities[i];
		}
	}
	float r2 = forest.radius; // temporary... calc real radius at some point
	r2 *= r2; // square it
	for (size_t j = 0; j < land->realVerticesMapSide; j++)
	{
		for (size_t i = 0; i < land->realVerticesMapSide; i++)
		{
			if (land->isVertexSelected(j, i))
			{
				int32_t cellI = i * 3;
				int32_t cellJ = j * 3;
				for (size_t l = -1; l < 2; l++)
				{
					for (size_t m = -1; m < 2; m++)
					{
						Stuff::Vector3D pos;
						if (cellJ + l < 0 || cellJ + l > land->realVerticesMapSide * 3)
							continue;
						if (cellI + m < 0 || cellI + m > land->realVerticesMapSide * 3)
							continue;
						EditorObject* pObject = getObjectAtCell(cellJ + l, cellI + m);
						if (pObject && pObject->getForestID() == -1) // only forests allow multiples in a cell
						{
							continue;
						}
						// figure out how far from center we are
						land->cellToWorld(cellJ + l, cellI + m, pos);
						pos.z = land->getTerrainElevation(pos);
						// distance
						float deltaX = centerX - pos.x;
						float deltaY = centerY - pos.y;
						float tmpR = deltaX * deltaX + deltaY * deltaY;
						tmpR = sqrt(tmpR);
						float fm = (minDensity - maxDensity) / forest.radius;
						float density = maxDensity + fm * (tmpR);
						int32_t lDensity = (int32_t)density;
						float remainder = density - (float)lDensity;
						int32_t count =
							(int32_t)density + (RandomNumber(100) < remainder * 100 ? 1 : 0);
						// OK, now we have the density randomly (if possible)
						// populate the tile with these...
						// might want to break down by cell
						for (size_t t = 0; t < count; t++)
						{
							// oh god, now we need to pick a tree
							int32_t random = RandomNumber(100);
							int32_t group = -1;
							int32_t index = -1;
							for (size_t k = 0; k < FOREST_TYPES; k++)
							{
								if (random < probabilities[k])
								{
									getRandomTreeFromGroup(k + 1, group, index);
									break;
								}
							}
							if (group == -1 || index == -1)
							{
								continue;
							}
							Stuff::Vector3D treePos = pos;
							// now place equally randomly.
							treePos.x += (float)RandomNumber(Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
							treePos.y += (float)RandomNumber(Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
							int32_t rotation = RandomNumber(32);
							float scale = RandomNumber(100 * (forest.maxheight - forest.minheight));
							scale = forest.minheight + (scale) / 100.f;
							if ((AppearanceTypeList::appearanceHeap->totalCoreLeft() < 1000 /*arbitrary*/) || (AppearanceTypeList::appearanceHeap->totalCoreLeft() < 0.01 /*arbitrary*/ * AppearanceTypeList::appearanceHeap->size()))
							{
								AfxMessageBox(IDS_APPEARANCE_HEAP_EXHAUSTED);
								/*exit all loops*/
								t = count;
								m = 2;
								l = 2;
								i = land->realVerticesMapSide;
								j = land->realVerticesMapSide;
							}
							else
							{
								EditorObject* pObject = (EditorObject*)addBuilding(treePos, group,
									index, -1, rotation * 11.25, scale, !forest.bRandom);
								pObject->setForestID(forest.getID());
							}
						}
					}
				}
			}
		}
	}
}

const Forest*
EditorObjectMgr::getForest(int32_t ID)
{
	for (FOREST_LIST::EIterator iter = forests.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getID() == ID)
			return (*iter);
	}
	gosASSERT(0); // you shouldn't get here
	return nullptr;
}

int32_t
EditorObjectMgr::getForests(Forest** pForests, int32_t& count)
{
	if (forests.Count() > count)
	{
		count = forests.Count();
		return -1;
	}
	count = 0;
	for (FOREST_LIST::EIterator iter = forests.Begin(); !iter.IsDone(); iter++)
	{
		pForests[count++] = (*iter);
	}
	return count;
}

void EditorObjectMgr::getRandomTreeFromGroup(int32_t treeGroup, int32_t& group, int32_t& index)
{
	group = TREE_GROUP;
	index = 0;
	int32_t treeCount = 0;
	// figure out how many trees are in the group
	group* pGroup = &groups[TREE_GROUP];
	if (pGroup)
	{
		for (EList<Building, Building&>::EIterator buildIter = groups[group].buildings.Begin();
			 !buildIter.IsDone(); buildIter++)
		{
			if ((*buildIter).forestId == treeGroup)
				treeCount++;
		}
		int32_t whichOne = RandomNumber(treeCount);
		treeCount = 0;
		int32_t counter = 0;
		for (buildIter = groups[group].buildings.Begin(); !buildIter.IsDone();
			 buildIter++, counter++)
		{
			if ((*buildIter).forestId == treeGroup)
			{
				if (treeCount == whichOne)
				{
					index = counter;
				}
				treeCount++;
			}
		}
	}
}

bool EditorObjectMgr::saveDropZones(FitIniFile& file)
{
	int32_t counter = 0;
	wchar_t Header[256];
	for (DROP_LIST::EIterator iter = dropZones.Begin(); !iter.IsDone(); iter++, counter++)
	{
		sprintf(Header, "DropZone%ld", counter);
		file.writeBlock(Header);
		(*iter)->save(&file, counter);
	}
	// going to do nav markers here
	// need to sort this list first.  ick.
	EditorObject** pObjects = (EditorObject**)_alloca(sizeof(EditorObject*) * buildings.Count());
	int32_t i = 0;
	for (BUILDING_LIST::EIterator bIter = buildings.Begin(); !bIter.IsDone(); bIter++)
	{
		pObjects[i++] = *bIter;
	}
	int32_t count = i;
	for (i = 1; i < count; ++i)
	{
		EditorObject* cur = pObjects[i];
		for (size_t j = 0; j < i; ++j)
		{
			if ((getFitID(cur->getID()) < getFitID(pObjects[j]->getID())) && j != i)
			{
				pObjects[i] = pObjects[j];
				pObjects[j] = cur;
				break;
			}
		}
	}
	counter = 0;
	for (i = 0; i < count; i++)
	{
		if (pObjects[i]->save(&file, counter))
			counter++;
	}
	// so  it writes out afterward....
	file.writeBlock("NavMarkers");
	file.writeIdULong("NumNavMarkers", counter);
	return true;
}

bool EditorObjectMgr::loadDropZones(FitIniFile& file)
{
	int32_t counter = 0;
	wchar_t Header[256];
	while (true)
	{
		sprintf(Header, "DropZone%ld", counter);
		if (NO_ERROR != file.seekBlock(Header))
			return counter > 0 ? true : false;
		bool bVTol;
		file.readIdBoolean("IsVTOL", bVTol);
		Stuff::Vector3D pos;
		file.readIdFloat("PositionX", pos.x);
		file.readIdFloat("PositionY", pos.y);
		pos.z = land->getTerrainElevation(pos);
		// assuming single player for now
		addDropZone(pos, 0, bVTol);
		counter++;
	}
	// no need to read in nav markers, they are saved in the building file as
	// well
	return true;
}

int32_t
EditorObjectMgr::getType(uint32_t group, uint32_t indexWithinGroup)
{
	group* pGroup = &groups[group];
	gosASSERT(pGroup->buildings.Count() > indexWithinGroup);
	Building* pBuilding = &pGroup->buildings[indexWithinGroup];
	return pBuilding->type;
}

EditorObjectMgr::Building::~Building() { }

bool EditorObjectMgr::moveBuilding(EditorObject* pInfo, int32_t cellJ, int32_t cellI)
{
	if (getObjectAtCell(cellJ, cellI) && (getObjectAtCell(cellJ, cellI) != pInfo))
	{
		// a building is already there
		return false;
	}
	BuildingLink* pLink = getLinkWithParent(pInfo);
	if (pLink)
	{
		Stuff::Vector3D worldPos;
		land->cellToWorld(cellJ, cellI, worldPos);
		worldPos.z = land->getTerrainElevation(worldPos);
		pLink->SetParentPosition(worldPos);
	}
	pLink = getLinkWithChild(pInfo);
	if (pLink)
	{
		pLink->RemoveObject(pInfo);
	}
	Stuff::Vector3D position;
	land->cellToWorld(cellJ, cellI, position);
	position.z = land->getTerrainElevation(position);
	pInfo->appearance()->position = position;
	pInfo->cellRow = cellJ;
	pInfo->cellColumn = cellI;
	if (pLink)
		pLink->AddChild(pInfo);
	return true;
}

EditorObject*
EditorObjectMgr::getObjectAtCell(int32_t realCellJ, int32_t realCellI)
{
	for (BUILDING_LIST::EIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
	{
		if (labs(realCellI - ((*iter)->cellColumn - 4)) <= 8 && labs(realCellJ - ((*iter)->cellRow - 4)) <= 8)
		{
			int64_t FootPrint = getImpassability((*iter)->id);
			int32_t i = realCellI - ((*iter)->cellColumn - 4);
			int32_t j = realCellJ - ((*iter)->cellRow - 4);
			if ((FootPrint >> (j * 8 + i)) & 0x00000001 || (i == 4 && j == 4))
				return (*iter);
		}
	}
	for (UNIT_LIST::EIterator uIter = units.Begin(); !uIter.IsDone(); uIter++)
	{
		if (labs(realCellI - ((*uIter)->cellColumn - 4)) <= 8 && labs(realCellJ - ((*uIter)->cellRow - 4)) <= 8)
		{
			int64_t FootPrint = getImpassability((*uIter)->id);
			int32_t i = realCellI - ((*uIter)->cellColumn - 4);
			int32_t j = realCellJ - ((*uIter)->cellRow - 4);
			if ((FootPrint >> (j * 8 + i)) & 0x00000001 || (i == 4 && j == 4))
				return (*uIter);
		}
	}
	return nullptr;
}

EditorObject*
EditorObjectMgr::getBuilding(const EditorObject& building)
{
	for (BUILDING_LIST::EIterator iter = buildings.Begin(); !iter.IsDone(); iter++)
	{
		if (building.cellColumn == (*iter)->cellColumn && building.cellRow == (*iter)->cellRow)
		{
			return (*iter);
		}
	}
	return nullptr;
}

BuildingLink*
EditorObjectMgr::getLinkWithBuilding(const EditorObject* pObj)
{
	for (LINK_LIST::EIterator iter = links.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->HasParent(pObj) || (*iter)->HasChild(pObj))
			return *iter;
	}
	return nullptr;
}

BuildingLink*
EditorObjectMgr::getLinkWithParent(const EditorObject* pObj)
{
	for (LINK_LIST::EIterator iter = links.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->HasParent(pObj))
			return *iter;
	}
	return nullptr;
}

BuildingLink*
EditorObjectMgr::getLinkWithChild(const EditorObject* pObj)
{
	for (LINK_LIST::EIterator iter = links.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->HasChild(pObj))
			return *iter;
	}
	return nullptr;
}

void EditorObjectMgr::addLink(BuildingLink* pLink)
{
	for (LINK_LIST::EIterator iter = links.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter) == pLink)
			return;
	}
	links.Append(pLink);
}

void EditorObjectMgr::deleteLink(BuildingLink* pLink)
{
	for (LINK_LIST::EIterator iter = links.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter) == pLink)
		{
			delete (*iter);
			links.Delete(iter);
			return;
		}
	}
}

std::wstring_view
EditorObjectMgr::getObjectName(int32_t ID) const
{
	return groups[getGroup(ID)].buildings[getIndexInGroup(ID)].name;
}

std::wstring_view
EditorObjectMgr::getGroupName(int32_t ID) const
{
	return groups[ID].name;
}

void EditorObjectMgr::getUnitGroupNames(std::wstring_view* names, int32_t* ids, int32_t& numberOfEm) const
{
	int32_t counter = 0;
	int32_t index = 0;
	for (GROUP_LIST::EConstIterator iter = groups.Begin(); !iter.IsDone(); index++, iter++)
	{
		if ((*iter).buildings.Count() && ((*iter).buildings.GetHead().type == MECH_TYPE || (*iter).buildings.GetHead().type == GV_TYPE))
		{
			if (counter < numberOfEm)
			{
				names[counter] = (*iter).name;
				ids[counter] = index;
			}
			counter++;
		}
	}
	numberOfEm = counter;
}

int32_t
EditorObjectMgr::getUnitGroupCount(void) const
{
	int32_t counter = 0;
	for (GROUP_LIST::EConstIterator iter = groups.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter).buildings.Count() && ((*iter).buildings.GetHead().type == MECH_TYPE || (*iter).buildings.GetHead().type == GV_TYPE))
		{
			counter++;
		}
	}
	return counter;
}

void EditorObjectMgr::getSelectedUnits(UNIT_LIST& selUnits)
{
	for (UNIT_LIST::EIterator iter = units.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->isSelected())
			selUnits.Append((*iter));
	}
}

int32_t
EditorObjectMgr::getNumberOfVariants(int32_t group, int32_t indexInGroup) const
{
	Building bldg = groups[group].buildings[indexInGroup];
	if (!bldg.varNames)
		return 0;
	for (size_t i = 0; i < 16; ++i)
	{
		if (!bldg.varNames[i])
			break;
	}
	return i + 1;
}

void EditorObjectMgr::getVariantNames(
	int32_t group, int32_t indexInGroup, std::wstring_view* names, int32_t& numberOfNames) const
{
	Building bldg = groups[group].buildings[indexInGroup];
	for (size_t i = 0; i < 16 && i < numberOfNames; ++i)
	{
		if (!bldg.varNames[i])
			break;
		names[i] = bldg.varNames[i];
	}
	numberOfNames = i;
}

void EditorObjectMgr::registerSquadNum(uint32_t squadNum)
{
	if (squadNum >= nextAvailableSquadNum)
	{
		nextAvailableSquadNum = squadNum + 1;
	}
}

// end of file ( EditorObjectMgr.cpp )
