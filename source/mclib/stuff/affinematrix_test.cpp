//===========================================================================//
// File:	affnmtrx.cc                                                      //
// Contents: Implementation details for the Affine matrices                  //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"
// #include "stuffheaders.hpp"
#include <gameos.hpp>

// test compiling headers one by one to find out dependencies

//#include <stuff/affinematrix.hpp>
//#include <stuff/angle.hpp>
//#include <stuff/armorheap.hpp>
//#include <stuff/armoroff.hpp>
//#include <stuff/armoron.hpp>
//#include <stuff/auto_container.hpp>
//#include <stuff/auto_ptr.hpp>
//#include <stuff/average.hpp>
//#include <stuff/chain.hpp>
//#include <stuff/color.hpp>
//#include <stuff/database.hpp>
//#include <stuff/extentbox.hpp>
//#include <stuff/extentpoly.hpp>
//#include <stuff/filestream.hpp>
//#include <stuff/filestreammanager.hpp>
//#include <stuff/hash.hpp>
//#include <stuff/initialized_ptr.hpp>
//#include <stuff/iterator.hpp>
//#include <stuff/line.hpp>
//#include <stuff/linearmatrix.hpp>
//#include <stuff/link.hpp>
//#include <stuff/marray.hpp>
//#include <stuff/matrix.hpp>
//#include <stuff/matrixstack.hpp>
//#include <stuff/memoryblock.hpp>
//#include <stuff/memoryheap.hpp>
//#include <stuff/memorystream.hpp>
//#include <stuff/motion.hpp>
//#include <stuff/mstring.hpp>
//#include <stuff/namelist.hpp>
//#include <stuff/node.hpp>
//#include <stuff/noncopyable.hpp>
//#include <stuff/normal.hpp>
//#include <stuff/notationfile.hpp>
//#include <stuff/note.hpp>
//#include <stuff/obb.hpp>
//#include <stuff/origin.hpp>
//#include <stuff/page.hpp>
//#include <stuff/plane.hpp>
//#include <stuff/plug.hpp>
//#include <stuff/point3d.hpp>
//#include <stuff/polar.hpp>
//#include <stuff/random.hpp>
//#include <stuff/ray.hpp>
//#include <stuff/registeredclass.hpp>
//#include <stuff/rotation.hpp>
//#include <stuff/safechain.hpp>
//#include <stuff/safesocket.hpp>
//#include <stuff/scalar.hpp>
//#include <stuff/slot.hpp>
//#include <stuff/socket.hpp>
//#include <stuff/sortedchain.hpp>
//#include <stuff/sortedsocket.hpp>
//#include <stuff/sphere.hpp>
//#include <stuff/stuff.hpp>
//#include <stuff/stuffheaders.hpp>
//#include <stuff/style.hpp>
//#include <stuff/table.hpp>
//#include <stuff/trace.hpp>
//#include <stuff/tree.hpp>
//#include <stuff/unitvector.hpp>
//#include <stuff/vector2d.hpp>
//#include <stuff/vector3d.hpp>
//#include <stuff/vector4d.hpp>

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