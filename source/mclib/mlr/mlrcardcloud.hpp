//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRCardCloud_HPP
#define MLR_MLRCardCloud_HPP

#include <mlr/mlreffect.hpp>

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

	void SetData(pcsize_t pcount, const Stuff::Point3D* point_data,
		const Stuff::RGBAColor* color_data,
		const Stuff::Vector2DScalar* uv_data);

	void SetData(pcsize_t pcount, const Stuff::Point3D* point_data,
		const Stuff::RGBAColor* color_data);

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
	pcsize_t usedNrOfCards;

	const Stuff::Vector2DScalar* texCoords;

	static Stuff::DynamicArrayOf<MLRClippingState>*
		clipPerVertex; // , Max_Number_Vertices_Per_Mesh
	static Stuff::DynamicArrayOf<Stuff::Vector4D>*
		clipExtraCoords; // , Max_Number_Vertices_Per_Mesh
	static Stuff::DynamicArrayOf<Stuff::RGBAColor>*
		clipExtraColors; // , Max_Number_Vertices_Per_Mesh
	static Stuff::DynamicArrayOf<Stuff::Vector2DScalar>*
		clipExtraTexCoords; // , Max_Number_Vertices_Per_Mesh
	static Stuff::DynamicArrayOf<uint32_t>*
		clipExtraLength; // , Max_Number_Primitives_Per_Frame
};
}
#endif
