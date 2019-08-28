//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRCardCloud_HPP
#define MLR_MLRCardCloud_HPP

#include "mlr/mlreffect.h"

namespace MidLevelRenderer
{

//##########################################################################
//#######################    MLRCardCloud    ###############################
//##########################################################################

class MLRCardCloud : public MLREffect
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
	MLRCardCloud(uint32_t);
	~MLRCardCloud(void);

	void SetData(const size_t* pcount, const Stuff::Point3D* point_data,
		const Stuff::RGBAcolour* color_data, const Stuff::Vector2DScalar* uv_data);

	void SetData(
		const size_t* pcount, const Stuff::Point3D* point_data, const Stuff::RGBAcolour* color_data);

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
	const size_t* usedNrOfCards;

	const Stuff::Vector2DScalar* texCoords;

	static std::vector<MLRClippingState>* clipPerVertex; // , Max_Number_Vertices_Per_Mesh
	static std::vector<Stuff::Vector4D>*
		clipExtraCoords; // , Max_Number_Vertices_Per_Mesh
	static std::vector<Stuff::RGBAcolour>*
		clipExtracolours; // , Max_Number_Vertices_Per_Mesh
	static std::vector<Stuff::Vector2DScalar>*
		clipExtraTexCoords; // , Max_Number_Vertices_Per_Mesh
	static std::vector<uint32_t>* clipExtraLength; // , Max_Number_Primitives_Per_Frame
};
} // namespace MidLevelRenderer
#endif
