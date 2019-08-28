//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRPOINTCLOUD_HPP
#define MLR_MLRPOINTCLOUD_HPP

#include "mlr/mlreffect.h"

namespace MidLevelRenderer
{

//##########################################################################
//#######################    MLRPointCloud    ##############################
//##########################################################################

class MLRPointCloud : public MLREffect
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialization
	//
public:
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors/Destructors
	//
public:
	MLRPointCloud(uint32_t, uint32_t = 0);
	~MLRPointCloud(void);

	void SetData(
		const size_t* count, const Stuff::Point3D* point_data, const Stuff::RGBAcolour* color_data);
	virtual uint32_t GetType(uint32_t) { return type; }
	void Draw(DrawEffectInformation*, GOSVertexPool*, MLRSorter*);

	void Transform(size_t, size_t);

	uint32_t Clip(MLRClippingState, GOSVertexPool*);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Class Data Support
	//
public:
	static ClassData* DefaultData;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
public:
	void TestInstance(void) const;

protected:
	uint32_t type;
	const size_t* usedNrOfVertices;
};
} // namespace MidLevelRenderer
#endif
