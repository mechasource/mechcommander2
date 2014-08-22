//===========================================================================//
// File:	munga.hpp                                                        //
// Contents: ???                                                             //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _STUFF_HPP_
#define _STUFF_HPP_

//#pragma warning(disable: 4018)	// signed/uint32_t mismatches
//#pragma warning(disable: 4097)	// typedef synonyms
//#pragma warning(disable: 4100)	// unreferenced parameters
//#pragma warning(disable: 4102)	// unreferenced labels
//#pragma warning(disable: 4127)	// constant comparisons
//#pragma warning(disable: 4130)	// logical operation on string constants
//#pragma warning(disable: 4201)	// nameless structs
//#pragma warning(disable: 4245)	// casting enum to int32_t
//#pragma warning(disable: 4355)	// this used in base initializers
//#pragma warning(disable: 4511)	// no copy constructor
//#pragma warning(disable: 4512)	// no assignment operator
//#pragma warning(disable: 4514)
//#if defined(_ARMOR) && !defined(_DEBUG)
//	#pragma warning(disable: 4700)	// variable used without initialization
//	#pragma warning(disable: 4701)	// variable maybe not initialized
//#endif
//#pragma warning(disable: 4710)	// inline not expanded
//#pragma warning(disable: 4711)	// auto-inline selection
//#pragma warning(disable: 4291)	// exception handlings and new
//
//#pragma warning(push,3)

//#include <math.h>
//#include <float.h>
//#include <time.h>
//#include <string.h>
//#include <memory.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <limits.h>
//#pragma warning(pop)

//#pragma warning(push)
//#pragma warning(pop)

namespace Stuff {

	//-------------------------
	// ClassID reserved blocks
	//-------------------------
	typedef enum __classid_constants {
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
	typedef enum __classid_attribute {
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

#if _CONSIDERED_OBSOLETE
#include <gameos.hpp>
#if defined(_ARMOR)
#include <stuff/armoron.hpp>
#else
#include <stuff/armoroff.hpp>
#endif
#include <stuff/style.hpp>

#include <stuff/registeredclass.hpp>
#include <stuff/memorystream.hpp>
#include <stuff/memoryblock.hpp>
#include <stuff/iterator.hpp>
#include <stuff/plug.hpp>
#include <stuff/link.hpp>
#include <stuff/mstring.hpp>
#include <stuff/socket.hpp>
#include <stuff/safesocket.hpp>
#include <stuff/sortedsocket.hpp>
#include <stuff/slot.hpp>
#include <stuff/chain.hpp>
#include <stuff/safechain.hpp>
#include <stuff/sortedchain.hpp>
#include <stuff/table.hpp>
#include <stuff/tree.hpp>
#include <stuff/hash.hpp>
#include <stuff/marray.hpp>

#include <stuff/scalar.hpp>
#include <stuff/angle.hpp>
#include <stuff/color.hpp>
#include <stuff/vector3d.hpp>
#include <stuff/point3d.hpp>
#include <stuff/unitvector.hpp>
#include <stuff/normal.hpp>
#include <stuff/plane.hpp>
#include <stuff/extentbox.hpp>
#include <stuff/filestream.hpp>
#include <stuff/filestreammanager.hpp>
#include <stuff/ray.hpp>
#include <stuff/rotation.hpp>
#include <stuff/line.hpp>
#include <stuff/node.hpp>
#include <stuff/affinematrix.hpp>
#include <stuff/matrix.hpp>
#include <stuff/linearmatrix.hpp>

//#include <stuff/matrixstack.hpp>
//#include <stuff/auto_ptr.hpp>
//#include <stuff/auto_container.hpp>
//#include <stuff/noncopyable.hpp>
//#include <stuff/initialized_ptr.hpp>
#include <stuff/motion.hpp>
#include <stuff/sphere.hpp>

#include <stuff/notationfile.hpp>
#include <stuff/page.hpp>
#include <stuff/note.hpp>
#include <stuff/obb.hpp>
#include <stuff/origin.hpp>
#include <stuff/plane.hpp>
#include <stuff/point3d.hpp>
#include <stuff/polar.hpp>
#include <stuff/random.hpp>
#include <stuff/sphere.hpp>
#include <stuff/vector2d.hpp>
#include <stuff/vector4d.hpp>
#include <stuff/trace.hpp>
#include <stuff/average.hpp>
#include <stuff/database.hpp>
#endif

#endif
