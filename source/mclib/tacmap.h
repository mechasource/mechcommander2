#ifndef TACMAP_H
#define TACMAP_H
/*************************************************************************************************\
TacMap.h			: Interface for the TacMap component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

//*************************************************************************************************
#include <mclib.h>

#define BUILDING_COLOR 0xffff9f00
/**************************************************************************************************
CLASS DESCRIPTION
TacMap:
**************************************************************************************************/
class TacMap
{
	public:

	TacMap(void);

	static void worldToTacMap( Stuff::Vector3D& world, int32_t xOffset, int32_t yOffset, int32_t xSize, int32_t ySize, gos_VERTEX& tac );
	static void tacMapToWorld( const Stuff::Vector2DOf<int32_t>& screen, int32_t xSize, int32_t ySize,  Stuff::Vector3D& world );

};


//*************************************************************************************************
#endif  // end of file ( TacMap.h )
