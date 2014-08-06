//===========================================================================//
// File:	munga.hpp                                                        //
// Contents: ???                                                             //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#pragma warning(disable: 4018)	// signed/unsigned mismatches
#pragma warning(disable: 4097)	// typedef synonyms
#pragma warning(disable: 4100)	// unreferenced parameters
#pragma warning(disable: 4102)	// unreferenced labels
#pragma warning(disable: 4127)	// constant comparisons
#pragma warning(disable: 4130)	// logical operation on string constants
#pragma warning(disable: 4201)	// nameless structs
#pragma warning(disable: 4245)	// casting enum to long
#pragma warning(disable: 4355)	// this used in base initializers
#pragma warning(disable: 4511)	// no copy constructor
#pragma warning(disable: 4512)	// no assignment operator
#pragma warning(disable: 4514)
#if defined(_ARMOR) && !defined(_DEBUG)
	#pragma warning(disable: 4700)	// variable used without initialization
	#pragma warning(disable: 4701)	// variable maybe not initialized
#endif
#pragma warning(disable: 4710)	// inline not expanded
#pragma warning(disable: 4711)	// auto-inline selection
#pragma warning(disable: 4291)	// exception handlings and new

#pragma warning(push,3)

#include <math.h>
#include <float.h>
#include <time.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#pragma warning(pop)

#pragma warning(push)
#include <gameOS.hpp>
#pragma warning(pop)

#define USE_ASSEMBLER_CODE	1

namespace Stuff {

	//-------------------------
	// ClassID reserved blocks
	//-------------------------
	enum
	{
		FirstStuffClassID					=	0,
		StuffClassIDCount					=	64,
		LastStuffClassID					=	FirstStuffClassID + StuffClassIDCount-1,

		FirstMLRClassID,
		MLRClassIDCount					=	64,
		LastMLRClassID						=	FirstMLRClassID + MLRClassIDCount-1,

		FirstElementRendererClassID,
		ElementRendererClassIDCount	=	64,
		LastElementRendererClassID		=	FirstElementRendererClassID + ElementRendererClassIDCount-1,

		FirstProxyClassID,
		ProxyClassIDCount					=	64,
		LastProxyClassID					=	FirstProxyClassID + ProxyClassIDCount-1,

		FirstElementProxyClassID,
		ElementProxyClassIDCount		=	64,
		LastElementProxyClassID			=	FirstElementProxyClassID + ElementProxyClassIDCount-1,

		FirstMGProxyClassID,
		MGProxyClassIDCount				=	64,
		LastMGProxyClassID				=	FirstMGProxyClassID + MGProxyClassIDCount-1,

		FirstSIProxyClassID,
		SIProxyClassIDCount				=	64,
		LastSIProxyClassID				=	FirstSIProxyClassID + SIProxyClassIDCount-1,

		FirstAdeptClassID,
		AdeptClassIDCount					=	256,
		LastAdeptClassID					=	FirstAdeptClassID + AdeptClassIDCount-1,

		FirstWindowsAdeptClassID,
		WindowsAdeptClassIDCount		=	32,
		LastWindowsAdeptClassID			=	FirstWindowsAdeptClassID + WindowsAdeptClassIDCount-1,

		FirstGOSAdeptClassID,
		GOSAdeptClassIDCount				=	16,
		LastGOSAdeptClassID				=	FirstGOSAdeptClassID + GOSAdeptClassIDCount-1,

		FirstSRClassID,
		SRClassIDCount						=	256,
		LastSRClassID						=	FirstSRClassID + SRClassIDCount-1,

		FirstSRGOSClassID,
		SRGOSClassIDCount					=	16,
		LastSRGOSClassID					=	FirstSRGOSClassID + SRGOSClassIDCount-1,

		FirstMWGOSClassID,
		MWGOSClassIDCount					=	16,
		LastMWGOSClassID					=	FirstMWGOSClassID + MWGOSClassIDCount-1,

		FirstMAXProxiesClassID,
		MAXProxiesClassIDCount			=	16,
		LastMAXProxiesClassID				=	FirstMAXProxiesClassID + MAXProxiesClassIDCount-1,

		FirstMW4ClassID,
		MW4ClassIDCount						= 256,
		LastMW4ClassID						= FirstMW4ClassID + MW4ClassIDCount - 1,

		FirstgosFXClassID,
		gosFXClassIDCount					= 64,
		LastgosFXClassID					= FirstgosFXClassID + gosFXClassIDCount -1,
		
		//
		//------------------------------------------
		// Please note that all new class id's
		// should be above this comment.  NO
		// class id's shoud be put between
		// the temporary and the class id count
		//------------------------------------------
		//
		FirstTemporaryClassID,
		TemporaryClassIDCount				=	16,
		LastTemporaryClassID				=	FirstTemporaryClassID + TemporaryClassIDCount-1,

		ClassIDCount
	};

	//
	//--------------
	// Stuff classes
	//--------------
	//
	enum
	{
		//
		//-------------------------
		// attribute IDs
		//-------------------------
		//
		NullClassID = ClassIDCount,
		MStringClassID = FirstStuffClassID,
		IntClassID,
		ScalarClassID,
		RadianClassID,
		Vector3DClassID,
		Point3DClassID,
		Normal3DClassID,
		UnitQuaternionClassID,
		Motion3DClassID,
		Origin3DClassID,
		LinearMatrix4DClassID,
		Vector2DOfIntClassID,
		Vector2DOfScalarClassID,
		EulerAnglesClassID,
		BoolClassID,

		//
		//---------------------
		// Registered class IDs
		//---------------------
		//
		RegisteredClassClassID,

		MemoryStreamClassID,
		FileStreamClassID,

		PlugClassID,
		NodeClassID,

		CharClassID,

		RGBAColorClassID,

		FirstFreeStuffClassID
	};

	void InitializeClasses();
	void TerminateClasses();
}

#define GROUP_STUFF "Stuff::"
#define GROUP_STUFF_TEST "Stuff::Test"
#define GROUP_STUFF_MEMORY "Stuff::Memory"
#define GROUP_STUFF_TRACE "Stuff::Trace"

#include "Style.hpp"
#include "MString.hpp"
#include "Slot.hpp"
#include "Chain.hpp"
#include "SafeChain.hpp"
#include "SortedChain.hpp"
#include "Table.hpp"
#include "Tree.hpp"
#include "Hash.hpp"
#include "Angle.hpp"
#include "Color.hpp"
#include "ExtentBox.hpp"
#include "FileStream.hpp"
#include "FileStreamManager.hpp"
#include "Line.hpp"
#include "MArray.hpp"
#include "MatrixStack.hpp"
#include "Auto_Ptr.hpp"
#include "Auto_Container.hpp"
#include "Noncopyable.hpp"
#include "Initialized_Ptr.hpp"
#include "Motion.hpp"
#include "NotationFile.hpp"
#include "Page.hpp"
#include "Note.hpp"
#include "OBB.hpp"
#include "Origin.hpp"
#include "Plane.hpp"
#include "Point3D.hpp"
#include "Polar.hpp"
#include "Random.hpp"
#include "Sphere.hpp"
#include "Vector2D.hpp"
#include "Vector4D.hpp"
#include "Trace.hpp"
#include "Average.hpp"
#include "Database.hpp"
