//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_C_DT_TMESH_HPP
#define MLR_MLR_I_C_DT_TMESH_HPP

#include "mlr/mlr_i_dt_tmesh.h"

namespace MidLevelRenderer
{

class MLR_I_C_DT_PMesh;

//##########################################################################
//###### MLRIndexedTriMesh with color no lighting two texture layer  #######
//##########################################################################

class MLR_I_C_DT_TMesh : public MLR_I_DT_TMesh
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
	MLR_I_C_DT_TMesh(ClassData* class_data, std::iostream stream, uint32_t version);
	~MLR_I_C_DT_TMesh(void);

public:
	MLR_I_C_DT_TMesh(ClassData* class_data = MLR_I_C_DT_TMesh::DefaultData);

	static MLR_I_C_DT_TMesh* Make(std::iostream stream, uint32_t version);

	void Save(std::iostream stream);

public:
#if COLOR_AS_DWORD
	virtual void SetcolourData(const uint32_t* array, size_t point_count);
	virtual void GetcolourData(uint32_t** array, size_t* point_count);
#else
	virtual void SetcolourData(const Stuff::RGBAcolour* array, size_t point_count);
	virtual void GetcolourData(Stuff::RGBAcolour** array, size_t* point_count);
#endif

	virtual void
#if COLOR_AS_DWORD
	PaintMe(const uint32_t* paintMe);
#else
	PaintMe(const Stuff::RGBAcolour* paintMe);
#endif

	virtual uint32_t TransformAndClip(
		Stuff::Matrix4D*, MLRClippingState, GOSVertexPool*, bool = false);

	virtual void TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*, bool = false);

	bool Copy(MLR_I_C_DT_PMesh*);

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
		size_t ret = MLR_I_DT_TMesh::GetSize();
		ret += colors.GetSize();
		return ret;
	}

protected:
#if COLOR_AS_DWORD
	std::vector<uint32_t> colors; // Base address of color list
	std::vector<uint32_t>* actualcolours;
#else
	std::vector<Stuff::RGBAcolour> colors; // Base address of color list
	std::vector<Stuff::RGBAcolour>* actualcolours;
#endif
};

MLR_I_C_DT_TMesh*
CreateIndexedTriCube_colour_NoLit_2Tex(float, MLRState*);
MLRShape*
CreateIndexedTriIcosahedron_colour_NoLit_2Tex(IcoInfo&, MLRState*, MLRState*);
} // namespace MidLevelRenderer
#endif
