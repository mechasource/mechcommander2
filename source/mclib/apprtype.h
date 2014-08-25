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
#ifndef DAPRTYPE_H
#include <daprtype.h>
#endif

#ifndef DAPPEAR_H
#include "dappear.h"
#endif

#ifndef DSTD_h
#include "dstd.h"
#endif

#ifndef HEAP_H
#include "heap.h"
#endif

#include <stuff\stuffheaders.hpp>

//---------------------------------------------------------------------------
// Macro definitions
#ifndef NO_ERROR
#define NO_ERROR					0
#endif

#define MAX_LODS				3
//---------------------------------------------------------------------------
// Class definitions
class AppearanceType
{
	//Data Members
	//-------------
	public:
	
		size_t 		numUsers;			//Number of users using this appearanceType.
		size_t		appearanceNum;		//What kind am I.
		AppearanceTypePtr	next;				//Pointer to next type in list.

		char 				*name;				//Appearance Base FileName.
	
		Stuff::Vector3D		typeUpperLeft;		//For Designer defined extents of objects
		Stuff::Vector3D		typeLowerRight;		//For Designer defined extents of objects

		float				boundsUpperLeftX;
		float				boundsUpperLeftY;
		
		float				boundsLowerRightX;
		float				boundsLowerRightY;
		
		bool				designerTypeBounds;	//So I know not to change them if the designer typed them in.
		
	//Member Functions
	//-----------------
	public:
	
		PVOID operator new (size_t memSize);
		void operator delete (PVOID treePtr);

		void init (void)
		{
			numUsers = 0;
			next = NULL;
			appearanceNum = 0xffffffff;

			name = NULL;

			typeUpperLeft.Zero(void);
			typeLowerRight.Zero(void);
			designerTypeBounds = false;
 		}

		AppearanceType (void)
		{
			init(void);
		}

		virtual void init (PSTR fileName);
		
		virtual void destroy (void);

		void reinit (void);

		bool typeBoundExists (void)
		{
			return true;		//Always exists now
		}

		bool getDesignerTypeBounds (void)
		{
			return designerTypeBounds;
		}

		size_t getAppearanceClass (void)
		{
			return(appearanceNum >> 24);
		}
		
		virtual ~AppearanceType (void)
		{
			destroy(void);
		}
};

//---------------------------------------------------------------------------
class AppearanceTypeList
{
	//Data Members
	//-------------
	protected:
	
		AppearanceTypePtr	head;
		AppearanceTypePtr	last;
		
	public:

		static UserHeapPtr	appearanceHeap;
		
	//Member Functions
	//----------------
	public:
	
		AppearanceTypeList (void)
		{
			head = last = NULL;
			appearanceHeap = NULL;
		}

		void init (size_t heapSize);

		AppearanceTypePtr getAppearance (size_t apprNum, PSTR  apprFile);
		
		int32_t removeAppearance (AppearanceTypePtr which);
		
		void destroy (void);
		
		~AppearanceTypeList (void)
		{
			destroy(void);
		}

		bool pointerCanBeDeleted (PVOIDptr);
};

extern AppearanceTypeListPtr appearanceTypeList;
//---------------------------------------------------------------------------
#endif
