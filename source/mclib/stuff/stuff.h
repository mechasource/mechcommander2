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

namespace Stuff
{
void __stdcall InitializeClasses(void);
void __stdcall TerminateClasses(void);
} // namespace Stuff

#define GROUP_STUFF "Stuff::"
#define GROUP_STUFF_TEST "Stuff::Test"
#define GROUP_STUFF_MEMORY "Stuff::Memory"
#define GROUP_STUFF_TRACE "Stuff::Trace"

#if CONSIDERED_OBSOLETE
#include <gameos.hpp"
#if defined(_ARMOR)
#include "stuff/armoron.h"
#else
#include "stuff/armoroff.h"
#endif
#include "stuff/style.h"

#include "stuff/registeredclass.h"
// #include "stuff/memorystream.h"
#include "stuff/memoryblock.h"
#include "stuff/iterator.h"
#include "stuff/plug.h"
#include "stuff/link.h"
//#include "stuff/mstring.h"
#include "stuff/socket.h"
#include "stuff/safesocket.h"
#include "stuff/sortedsocket.h"
#include "stuff/slot.h"
// #include "stuff/chain.h"
#include "stuff/safechain.h"
#include "stuff/sortedchain.h"
#include "stuff/table.h"
#include "stuff/tree.h"
#include "stuff/hash.h"
//#include "stuff/marray.h"

#include "stuff/scalar.h"
#include "stuff/angle.h"
#include "stuff/color.h"
#include "stuff/vector3d.h"
#include "stuff/point3d.h"
#include "stuff/unitvector.h"
#include "stuff/normal.h"
#include "stuff/plane.h"
#include "stuff/extentbox.h"
#include "stuff/filestream.h"
#include "stuff/filestreammanager.h"
#include "stuff/ray.h"
#include "stuff/rotation.h"
#include "stuff/line.h"
#include "stuff/node.h"
#include "stuff/affinematrix.h"
#include "stuff/matrix.h"
#include "stuff/linearmatrix.h"

//#include "stuff/matrixstack.h"
//#include "stuff/auto_ptr.h"
//#include "stuff/auto_container.h"
//#include "stuff/noncopyable.h"
//#include "stuff/initialized_ptr.h"
#include "stuff/motion.h"
#include "stuff/sphere.h"

#include "stuff/notationfile.h"
#include "stuff/page.h"
#include "stuff/note.h"
#include "stuff/obb.h"
#include "stuff/origin.h"
#include "stuff/plane.h"
#include "stuff/point3d.h"
#include "stuff/polar.h"
#include "stuff/random.h"
#include "stuff/sphere.h"
#include "stuff/vector2d.h"
#include "stuff/vector4d.h"
#include "stuff/trace.h"
#include "stuff/average.h"
#include "stuff/database.h"
#endif

#endif
