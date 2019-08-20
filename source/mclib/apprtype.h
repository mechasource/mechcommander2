//---------------------------------------------------------------------------
//
// ApprType.h -- File contains the Basic Game Appearance Type Declaration
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef APPRTYPE_H
#define APPRTYPE_H
//---------------------------------------------------------------------------
// Include Files
//#include <daprtype.h>
//#include "dappear.h"
//#include "dstd.h"
//#include "heap.h"
//#include <stuff\stuffheaders.hpp>

enum appearancetype_const : uint32_t
{
	BASE_APPEARANCE = 0x00,
	SPRITE_TREE = 0x01,
	VFX_APPEAR = 0x02,
	FSY_APPEAR = 0x03,
	LINE_APPEAR = 0x04,
	GV_TYPE = 0x05,
	ARM_APPEAR = 0x06,
	BUILD_APPEAR = 0x07,
	ELM_TREE = 0x08,
	PU_TYPE = 0x09,
	SMOKE_TYPE = 0x0a,
	POLY_APPEARANCE = 0x0b,
	MLR_APPEARANCE = 0x0c,
	MECH_TYPE = 0x0d,
	BLDG_TYPE = 0x0e,
	TREED_TYPE = 0x0f,
	BUILDING_APPR_TYPE = 0x10,
	TREE_APPR_TYPE = 0x11,
	VEHICLE_APPR_TYPE = 0x12,
	MECH_APPR_TYPE = 0x13,
	GENERIC_APPR_TYPE = 0x14,
};

//---------------------------------------------------------------------------
// Macro definitions

constexpr const uint32_t MAX_LODS = 3;

//---------------------------------------------------------------------------
// Class definitions
class AppearanceType
{
	// Data Members
	//-------------
public:
	size_t numUsers; // Number of users using this appearanceType.
	size_t appearanceNum; // What kind am I.
	AppearanceType* next; // Pointer to next type in list.

	char* name; // Appearance Base FileName.

	Stuff::Vector3D typeUpperLeft; // For Designer defined extents of objects
	Stuff::Vector3D typeLowerRight; // For Designer defined extents of objects

	float boundsUpperLeftX;
	float boundsUpperLeftY;

	float boundsLowerRightX;
	float boundsLowerRightY;

	bool designerTypeBounds; // So I know not to change them if the designer
		// typed them in.

	// Member Functions
	//-----------------
public:
	AppearanceType(void) { init(); }
	virtual ~AppearanceType(void) { destroy(); }

	PVOID operator new(size_t memSize);
	void operator delete(PVOID treePtr);

	void init(void)
	{
		numUsers = 0;
		next = nullptr;
		appearanceNum = 0xffffffff;
		name = nullptr;
		typeUpperLeft.Zero();
		typeLowerRight.Zero();
		designerTypeBounds = false;
	}

	virtual void init(PSTR fileName);

	virtual void destroy(void);

	void reinit(void);

	bool typeBoundExists(void)
	{
		return true; // Always exists now
	}

	bool getDesignerTypeBounds(void) { return designerTypeBounds; }

	size_t getAppearanceClass(void) { return (appearanceNum >> 24); }
};

//---------------------------------------------------------------------------
class AppearanceTypeList
{
	// Data Members
	//-------------
protected:
	AppearanceTypePtr head;
	AppearanceTypePtr last;

public:
	static UserHeapPtr appearanceHeap;

	// Member Functions
	//----------------
public:
	AppearanceTypeList(void)
	{
		head = last = nullptr;
		appearanceHeap = nullptr;
	}

	void init(size_t heapSize);

	AppearanceTypePtr getAppearance(size_t apprNum, PSTR apprFile);

	int32_t removeAppearance(AppearanceTypePtr which);

	void destroy(void);

	~AppearanceTypeList(void) { destroy(void); }

	bool pointerCanBeDeleted(PVOIDptr);
};

extern AppearanceTypeListPtr appearanceTypeList;
//---------------------------------------------------------------------------
#endif
