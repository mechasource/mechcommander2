//===========================================================================//
// File:	affnmtrx.cc                                                      //
// Contents: Implementation details for the Affine matrices                  //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
// #include "stuffheaders.hpp"
#include "gameos.hpp"

// test compiling headers one by one to find out dependencies

//#include "stuff/armoroff.h"
//#include "stuff/armoron.h"

//#include "stuff/affinematrix.h"
//#include "stuff/angle.h"
//#include "stuff/auto_container.h"
//#include "stuff/auto_ptr.h"
//#include "stuff/average.h"
//#include "stuff/chain.h"
//#include "stuff/color.h"
//#include "stuff/database.h"
//#include "stuff/extentbox.h"
//#include "stuff/filestream.h"
//#include "stuff/filestreammanager.h"
//#include "stuff/hash.h"
//#include "stuff/initialized_ptr.h"
//#include "stuff/iterator.h"
//#include "stuff/line.h"
//#include "stuff/linearmatrix.h"
//#include "stuff/link.h"
//#include "stuff/marray.h"
//#include "stuff/matrix.h"
//#include "stuff/matrixstack.h"
//#include "stuff/memoryblock.h"
//// #include "stuff/memorystream.h"
//#include "stuff/motion.h"
//#include "stuff/mstring.h"
//#include "stuff/namelist.h"
//#include "stuff/node.h"
//#include "stuff/noncopyable.h"
//#include "stuff/normal.h"
//#include "stuff/notationfile.h"
//#include "stuff/note.h"
//#include "stuff/obb.h"
//#include "stuff/origin.h"
//#include "stuff/page.h"
//#include "stuff/plane.h"
//#include "stuff/plug.h"
//#include "stuff/point3d.h"
//#include "stuff/polar.h"
//#include "stuff/random.h"
//#include "stuff/ray.h"
//#include "stuff/registeredclass.h"
//#include "stuff/rotation.h"
//#include "stuff/safechain.h"
//#include "stuff/safesocket.h"
//#include "stuff/scalar.h"
//#include "stuff/slot.h"
//#include "stuff/socket.h"
//#include "stuff/sortedchain.h"
//#include "stuff/sortedsocket.h"
//#include "stuff/sphere.h"
//#include "stuff/stuff.h"
//#include "stuff/stuffheaders.h"
//#include "stuff/style.h"
//#include "stuff/table.h"
//#include "stuff/trace.h"
//#include "stuff/tree.h"
//#include "stuff/unitvector.h"
//#include "stuff/vector2d.h"
//#include "stuff/vector3d.h"
//#include "stuff/vector4d.h"

//#include "stuff/armoron.h"
//#include "stuff/extentpoly.h"
//#include "stuff/memoryheap.h"

// using namespace Stuff;

#if 0
//
//###########################################################################
//###########################################################################
//
bool AffineMatrix4D::TestClass()
{
	SPEW((GROUP_STUFF_TEST, "Starting AffineMatrix4D Test..."));
	SPEW((GROUP_STUFF_TEST, "  AffineMatrix4D::TestClass() stubbed out!"));
	return false;
}
#endif