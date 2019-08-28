//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_L_DeT_PMESH_HPP
#define MLR_MLR_I_L_DeT_PMESH_HPP

#include "mlr/mlr_i_c_det_pmesh.h"

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
	MLR_I_L_DeT_PMesh(ClassData* class_data, std::iostream stream, uint32_t version);
	~MLR_I_L_DeT_PMesh(void);

public:
	MLR_I_L_DeT_PMesh(ClassData* class_data = MLR_I_L_DeT_PMesh::DefaultData);

	static MLR_I_L_DeT_PMesh* Make(std::iostream stream, uint32_t version);

	void Save(std::iostream stream);

public:
	virtual void SetNormalData(const Stuff::Vector3D* array, size_t point_count);
	virtual void GetNormalData(Stuff::Vector3D** array, size_t* point_count);

#if COLOR_AS_DWORD
	virtual void SetcolourData(const uint32_t* array, size_t point_count);
#else
	virtual void SetcolourData(const Stuff::RGBAcolour* array, size_t point_count);
#endif

	virtual void Lighting(MLRLight* const*, uint32_t nrLights);

	virtual void
#if COLOR_AS_DWORD
	PaintMe(const uint32_t* paintMe);
#else
	PaintMe(const Stuff::RGBAcolour* paintMe);
#endif

	virtual uint32_t TransformAndClip(
		Stuff::Matrix4D*, MLRClippingState, GOSVertexPool*, bool = false);

	virtual void TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*, bool = false);

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
		ret += litcolours.GetSize();
		return ret;
	}

protected:
	std::vector<Stuff::Vector3D> normals; // Base address of normal list

#if COLOR_AS_DWORD
	std::vector<uint32_t> litcolours;
	std::vector<uint32_t>* actualcolours;
#else
	std::vector<Stuff::RGBAcolour> litcolours;
	std::vector<Stuff::RGBAcolour>* actualcolours;
#endif
};

// MLR_I_L_DeT_PMesh*
// CreateIndexedCube(float, Stuff::RGBAcolour*, Stuff::Vector3D*, MLRState*);
MLRShape*
CreateIndexedIcosahedron_colour_Lit_DetTex(IcoInfo&, MLRState*, MLRState*);
} // namespace MidLevelRenderer
#endif
