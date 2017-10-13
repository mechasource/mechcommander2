//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_L_DeT_PMESH_HPP
#define MLR_MLR_I_L_DeT_PMESH_HPP

#include <mlr/mlr_i_c_det_pmesh.hpp>

namespace MidLevelRenderer
{

//##########################################################################
//###### MLRIndexedPolyMesh with color and lighting two texture layer #####
//##########################################################################

class MLR_I_L_DeT_PMesh : public MLR_I_C_DeT_PMesh
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
  protected:
	MLR_I_L_DeT_PMesh(
		ClassData* class_data, Stuff::MemoryStream* stream, uint32_t version);
	~MLR_I_L_DeT_PMesh(void);

  public:
	MLR_I_L_DeT_PMesh(ClassData* class_data = MLR_I_L_DeT_PMesh::DefaultData);

	static MLR_I_L_DeT_PMesh* Make(
		Stuff::MemoryStream* stream, uint32_t version);

	void Save(Stuff::MemoryStream* stream);

  public:
	virtual void SetNormalData(
		const Stuff::Vector3D* array, size_t point_count);
	virtual void GetNormalData(Stuff::Vector3D** array, psize_t point_count);

#if COLOR_AS_DWORD
	virtual void SetColorData(pcuint32_t array, size_t point_count);
#else
	virtual void SetColorData(
		const Stuff::RGBAColor* array, size_t point_count);
#endif

	virtual void Lighting(MLRLight* const*, uint32_t nrLights);

	virtual void
#if COLOR_AS_DWORD
	PaintMe(pcuint32_t paintMe);
#else
	PaintMe(const Stuff::RGBAColor* paintMe);
#endif

	virtual uint32_t TransformAndClip(
		Stuff::Matrix4D*, MLRClippingState, GOSVertexPool*, bool = false);

	virtual void TransformNoClip(
		Stuff::Matrix4D*, GOSVertexPool*, bool = false);

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

	virtual size_t GetSize(void)
	{
		// Check_Object(this);
		size_t ret = MLR_I_C_DeT_PMesh::GetSize();
		ret += normals.GetSize();
		ret += litColors.GetSize();
		return ret;
	}

  protected:
	Stuff::DynamicArrayOf<Stuff::Vector3D>
		normals; // Base address of normal list

#if COLOR_AS_DWORD
	Stuff::DynamicArrayOf<uint32_t> litColors;
	Stuff::DynamicArrayOf<uint32_t>* actualColors;
#else
	Stuff::DynamicArrayOf<Stuff::RGBAColor> litColors;
	Stuff::DynamicArrayOf<Stuff::RGBAColor>* actualColors;
#endif
};

// MLR_I_L_DeT_PMesh*
// CreateIndexedCube(float, Stuff::RGBAColor*, Stuff::Vector3D*, MLRState*);
MLRShape* CreateIndexedIcosahedron_Color_Lit_DetTex(
	IcoInfo&, MLRState*, MLRState*);
}
#endif
