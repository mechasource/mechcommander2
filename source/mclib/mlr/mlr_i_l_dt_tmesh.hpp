//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_L_DT_TMESH_HPP
#define MLR_MLR_I_L_DT_TMESH_HPP

#include <mlr/mlr_i_c_dt_tmesh.hpp>

namespace MidLevelRenderer
{

class MLR_I_L_DT_PMesh;

//##########################################################################
//#### MLRIndexedTriMesh with no color no lighting one texture layer  #####
//##########################################################################

class MLR_I_L_DT_TMesh : public MLR_I_C_DT_TMesh
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
	MLR_I_L_DT_TMesh(
		ClassData* class_data, Stuff::MemoryStream* stream, uint32_t version);
	~MLR_I_L_DT_TMesh(void);

  public:
	MLR_I_L_DT_TMesh(ClassData* class_data = MLR_I_L_DT_TMesh::DefaultData);

	static MLR_I_L_DT_TMesh* Make(
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

	bool Copy(MLR_I_L_DT_PMesh*);

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
		size_t ret = MLR_I_C_DT_TMesh::GetSize();
		ret += normals.GetSize();
		ret += litColors.GetSize();
		return ret;
	}

  protected:
	Stuff::DynamicArrayOf<Stuff::Vector3D>
		normals; // Base address of normal list

#if COLOR_AS_DWORD
	Stuff::DynamicArrayOf<uint32_t> litColors;
#else
	Stuff::DynamicArrayOf<Stuff::RGBAColor> litColors;
#endif
};

//	MLR_I_L_DT_TMesh*
//		CreateIndexedTriCube_NoColor_NoLit(float, MLRState*);
MLRShape* CreateIndexedTriIcosahedron_Color_Lit_2Tex(
	IcoInfo&, MLRState*, MLRState*);
}
#endif
