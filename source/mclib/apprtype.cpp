//---------------------------------------------------------------------------
//
// ApprType.cpp -- File contains the Basic Game Appearance Type code
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

//---------------------------------------------------------------------------
// Include Files

//#include "apprtype.h"
//#include "gvactor.h"
//#include "mech3d.h"
//#include "bdactor.h"
//#include "genactor.h"
//#include "cident.h"
//#include "inifile.h"
//#include "paths.h"
////#include "gameos.hpp"

//---------------------------------------------------------------------------
// static Globals
// UserHeapPtr AppearanceTypeList::appearanceHeap = nullptr;
// AppearanceTypeListPtr appearanceTypeList	   = nullptr;

//---------------------------------------------------------------------------
// Class AppearanceType
#if CONSIDERED_OBSOLETE
PVOID
AppearanceType::operator new(size_t memSize)
{
	PVOID result = nullptr;
	if (AppearanceTypeList::appearanceHeap && AppearanceTypeList::appearanceHeap->heapReady())
	{
		result = AppearanceTypeList::appearanceHeap->Malloc(memSize);
	}
	return (result);
}

//---------------------------------------------------------------------------
void AppearanceType::operator delete(PVOID treePtr)
{
	int32_t result;
	if (AppearanceTypeList::appearanceHeap && AppearanceTypeList::appearanceHeap->heapReady())
	{
		result = AppearanceTypeList::appearanceHeap->Free(treePtr);
	}
}
#endif

//---------------------------------------------------------------------------
void AppearanceType::init(std::wstring_view fileName)
{
	name = (std::wstring_view)AppearanceTypeList::appearanceHeap->Malloc(strlen(fileName) + 1);
	strcpy(name, fileName);
	// Dig out the Type Bounds here for selections
	FullPathFileName iniName;
	iniName.init(tglPath, fileName, ".ini");
	FitIniFile iniFile;
	int32_t result = iniFile.open(iniName);
	if (result != NO_ERROR)
		STOP(("Could not find appearance INI file %s", iniName));
	result = iniFile.seekBlock("3DBounds");
	if (result == NO_ERROR)
	{
		designerTypeBounds = true;
		int32_t tmpy;
		result = iniFile.readIdLong("UpperLeftX", tmpy);
		if (result != NO_ERROR)
			STOP(("Cant find upperleftX type bounds in %s", iniName));
		else
			typeUpperLeft.x = tmpy;
		result = iniFile.readIdLong("UpperLeftY", tmpy);
		if (result != NO_ERROR)
			STOP(("Cant find upperleftX type bounds in %s", iniName));
		else
			typeUpperLeft.y = tmpy;
		result = iniFile.readIdLong("UpperLeftZ", tmpy);
		if (result != NO_ERROR)
			STOP(("Cant find upperleftX type bounds in %s", iniName));
		else
			typeUpperLeft.z = tmpy;
		result = iniFile.readIdLong("LowerRightX", tmpy);
		if (result != NO_ERROR)
			STOP(("Cant find upperleftX type bounds in %s", iniName));
		else
			typeLowerRight.x = tmpy;
		result = iniFile.readIdLong("LowerRightY", tmpy);
		if (result != NO_ERROR)
			STOP(("Cant find upperleftX type bounds in %s", iniName));
		else
			typeLowerRight.y = tmpy;
		result = iniFile.readIdLong("LowerRightZ", tmpy);
		if (result != NO_ERROR)
			STOP(("Cant find upperleftX type bounds in %s", iniName));
		else
			typeLowerRight.z = tmpy;
	}
	iniFile.close();
}

//---------------------------------------------------------------------------
void AppearanceType::reinit(void)
{
	// Dig out the Type Bounds here for selections
	FullPathFileName iniName;
	iniName.init(tglPath, name, ".ini");
	FitIniFile iniFile;
	int32_t result = iniFile.open(iniName);
	if (result != NO_ERROR)
		STOP(("Could not find appearance INI file %s", iniName));
	result = iniFile.seekBlock("3DBounds");
	if (result == NO_ERROR)
	{
		designerTypeBounds = true;
		int32_t tmpy;
		result = iniFile.readIdLong("UpperLeftX", tmpy);
		if (result != NO_ERROR)
			STOP(("Cant find upperleftX type bounds in %s", iniName));
		else
			typeUpperLeft.x = tmpy;
		result = iniFile.readIdLong("UpperLeftY", tmpy);
		if (result != NO_ERROR)
			STOP(("Cant find upperleftX type bounds in %s", iniName));
		else
			typeUpperLeft.y = tmpy;
		result = iniFile.readIdLong("UpperLeftZ", tmpy);
		if (result != NO_ERROR)
			STOP(("Cant find upperleftX type bounds in %s", iniName));
		else
			typeUpperLeft.z = tmpy;
		result = iniFile.readIdLong("LowerRightX", tmpy);
		if (result != NO_ERROR)
			STOP(("Cant find upperleftX type bounds in %s", iniName));
		else
			typeLowerRight.x = tmpy;
		result = iniFile.readIdLong("LowerRightY", tmpy);
		if (result != NO_ERROR)
			STOP(("Cant find upperleftX type bounds in %s", iniName));
		else
			typeLowerRight.y = tmpy;
		result = iniFile.readIdLong("LowerRightZ", tmpy);
		if (result != NO_ERROR)
			STOP(("Cant find upperleftX type bounds in %s", iniName));
		else
			typeLowerRight.z = tmpy;
	}
	iniFile.close();
}

//---------------------------------------------------------------------------
#if CONSIDERED_OBSOLETE
void AppearanceType::destroy(void)
{
	AppearanceTypeList::appearanceHeap->Free(name);
	name = nullptr;
}

//---------------------------------------------------------------------------
// class AppearanceTypeList
void AppearanceTypeList::init(uint32_t heapSize)
{
	appearanceHeap = new UserHeap;
	gosASSERT(appearanceHeap != nullptr);
	appearanceHeap->init(heapSize, "APPEAR");
	// No More sprite files.  Objects now have direct control over their
	// appearances.
}
#endif

//---------------------------------------------------------------------------
AppearanceTypePtr
AppearanceTypeList::getAppearance(uint32_t apprNum, std::wstring_view appearFile)
{
	//----------------------------------------------------------------
	// The type of appearance is stored in the upper 8 bits of the
	// apprNum.  To get the correct packet we mask off the top 8 bits
	// and store the number.  To get the appearance type, we right shift
	// by 24.
	int32_t appearanceClass = apprNum >> 24;
	AppearanceTypePtr appearanceType = nullptr;
	//----------------------------------------------------
	// If these top bits are wrong, return nullptr
	if (appearanceClass == 0)
		return (nullptr);
	//----------------------------------------------------
	// If string passed in is nullptr, return nullptr
	if (!appearFile)
	{
		return nullptr;
	}
	//-----------------------------------------------------------
	// Scan the list of appearances and see if we have this one.
	appearanceType = head;
	while (appearanceType && _stricmp(appearanceType->name, appearFile) != 0)
	{
		appearanceType = appearanceType->next;
	}
	if (appearanceType)
	{
		appearanceType->numUsers++;
	}
	else
	{
		//---------------------------------------------------------
		// This appearance is not on the list yet, so load/new it.
		switch (appearanceClass)
		{
		//----------------------------------------
		case MECH_TYPE:
		{
			appearanceType = new Mech3DAppearanceType;
			gosASSERT(appearanceType != nullptr);
			appearanceType->appearanceNum = apprNum;
			appearanceType->init(appearFile);
			//----------------------------------------
			// We have a new one, add it to the list.
			appearanceType->numUsers = 1;
			appearanceType->next = nullptr;
			if (head == nullptr)
			{
				head = appearanceType;
				last = head;
			}
			else
			{
				last->next = appearanceType;
				last = appearanceType;
			}
		}
		break;
		//----------------------------------------
		case GV_TYPE:
		{
			appearanceType = new GVAppearanceType;
			gosASSERT(appearanceType != nullptr);
			appearanceType->appearanceNum = apprNum;
			appearanceType->init(appearFile);
			//----------------------------------------
			// We have a new one, add it to the list.
			appearanceType->numUsers = 1;
			appearanceType->next = nullptr;
			if (head == nullptr)
			{
				head = appearanceType;
				last = head;
			}
			else
			{
				last->next = appearanceType;
				last = appearanceType;
			}
		}
		break;
		//----------------------------------------
		case TREED_TYPE:
		{
			appearanceType = new TreeAppearanceType;
			gosASSERT(appearanceType != nullptr);
			appearanceType->appearanceNum = apprNum;
			appearanceType->init(appearFile);
			//----------------------------------------
			// We have a new one, add it to the list.
			appearanceType->numUsers = 1;
			appearanceType->next = nullptr;
			if (head == nullptr)
			{
				head = appearanceType;
				last = head;
			}
			else
			{
				last->next = appearanceType;
				last = appearanceType;
			}
		}
		break;
		//----------------------------------------
		case GENERIC_APPR_TYPE:
		{
			appearanceType = new GenericAppearanceType;
			gosASSERT(appearanceType != nullptr);
			appearanceType->appearanceNum = apprNum;
			appearanceType->init(appearFile);
			//----------------------------------------
			// We have a new one, add it to the list.
			appearanceType->numUsers = 1;
			appearanceType->next = nullptr;
			if (head == nullptr)
			{
				head = appearanceType;
				last = head;
			}
			else
			{
				last->next = appearanceType;
				last = appearanceType;
			}
		}
		break;
		//----------------------------------------
		case BLDG_TYPE:
		{
			appearanceType = new BldgAppearanceType;
			gosASSERT(appearanceType != nullptr);
			appearanceType->appearanceNum = apprNum;
			appearanceType->init(appearFile);
			//----------------------------------------
			// We have a new one, add it to the list.
			appearanceType->numUsers = 1;
			appearanceType->next = nullptr;
			if (head == nullptr)
			{
				head = appearanceType;
				last = head;
			}
			else
			{
				last->next = appearanceType;
				last = appearanceType;
			}
		}
		break;
		default:
			// return(nullptr);
			NODEFAULT;
		}
	}
	return appearanceType;
}

//---------------------------------------------------------------------------
int32_t
AppearanceTypeList::removeAppearance(AppearanceTypePtr which)
{
	AppearanceTypePtr appearanceType = head;
	AppearanceTypePtr previous = nullptr;
	while (appearanceType && (appearanceType != which))
	{
		previous = appearanceType;
		appearanceType = appearanceType->next;
	}
	if (appearanceType)
		appearanceType->numUsers--;
	else
		return (-1);
	//----------------------------------------------------------
	// NEVER remove the types anymore.  Save cache time!
	//#if 0
	if (appearanceType && (appearanceType->numUsers == 0))
	{
		//------------------------------------------------------
		// Check if there is no previous appearanceType in list
		if (previous == nullptr)
		{
			//----------------------------------------------------
			// if there was no previous, head is the next in list
			head = appearanceType->next;
		}
		else
		{
			previous->next = appearanceType->next;
		}
		//-------------------------------------------------------------
		// Make sure that we don't gratuitously free the last pointer
		if (appearanceType == last)
			last = previous;
		delete appearanceType;
		appearanceType = nullptr;
	}
	//#endif
	return NO_ERROR;
}

//---------------------------------------------------------------------------
void AppearanceTypeList::destroy(void)
{
	//---------------------------------------------------------------------
	// Run through the list and force a destroy call for each list element.
	// This will free any RAM used by each individual appearance.
	// DO NOT really need to do this.  Just toss the heap!
	AppearanceTypePtr currentAppr = head;
	while (currentAppr)
	{
		AppearanceTypePtr nextPtr = currentAppr->next;
		currentAppr->destroy();
		currentAppr = nextPtr;
	}
	head = last = nullptr;
	delete appearanceHeap;
	appearanceHeap = nullptr;
	if (GVAppearanceType::SensorCircleShape)
	{
		delete GVAppearanceType::SensorCircleShape;
		GVAppearanceType::SensorCircleShape = nullptr;
	}
	if (GVAppearanceType::SensorTriangleShape)
	{
		delete GVAppearanceType::SensorTriangleShape;
		GVAppearanceType::SensorTriangleShape = nullptr;
	}
	if (Mech3DAppearanceType::SensorSquareShape)
	{
		delete Mech3DAppearanceType::SensorSquareShape;
		Mech3DAppearanceType::SensorSquareShape = nullptr;
	}
}

//---------------------------------------------------------------------------
bool AppearanceTypeList::pointerCanBeDeleted(PVOIDptr)
{
	if (appearanceHeap && appearanceHeap->pointerOnHeap(ptr))
		return true;
	return false;
}

//---------------------------------------------------------------------------
