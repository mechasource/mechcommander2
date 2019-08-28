//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRTRIANGLECLOUD_HPP
#define MLR_MLRTRIANGLECLOUD_HPP

#include "mlr/mlreffect.h"

namespace MidLevelRenderer
{

//##########################################################################
//#####################    MLRTriangleCloud    #############################
//##########################################################################

class MLRTriangleCloud : public MLREffect
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
	MLRTriangleCloud(uint32_t);
	~MLRTriangleCloud(void);

	void SetData(
		const size_t* count, const Stuff::Point3D* point_data, const Stuff::RGBAcolour* color_data);

	// added due to warning C4263 in MLRIndexedTriangleCloud
	virtual void SetData(const size_t* tri_count, const size_t* point_count, const uint16_t* index_data,
		const Stuff::Point3D* point_data, const Stuff::RGBAcolour* color_data,
		const Stuff::Vector2DScalar* uv_data);

	void Draw(DrawEffectInformation*, GOSVertexPool*, MLRSorter*);

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
	const size_t* usedNrOfTriangles;

	static std::vector<Stuff::RGBAcolour>*
		clipExtracolours; // , Max_Number_Vertices_Per_Mesh

	static std::vector<MLRClippingState>* clipPerVertex; // , Max_Number_Vertices_Per_Mesh

	static std::vector<Stuff::Vector4D>*
		clipExtraCoords; // , Max_Number_Vertices_Per_Mesh

	static std::vector<int32_t>* clipExtraLength; // , Max_Number_Primitives_Per_Frame
};
} // namespace MidLevelRenderer
#endif
