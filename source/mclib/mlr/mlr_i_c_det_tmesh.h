//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_C_DET_TMESH_HPP
#define MLR_MLR_I_C_DET_TMESH_HPP

#include <mlr/mlr_i_det_tmesh.hpp>

namespace MidLevelRenderer
{

class MLR_I_C_TMesh;
class MLR_I_C_DeT_PMesh;

//##########################################################################
//###### MLRIndexedTriMesh with color no lighting and detail texture #######
//##########################################################################

class MLR_I_C_DeT_TMesh : public MLR_I_DeT_TMesh
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
	MLR_I_C_DeT_TMesh(ClassData* class_data, std::iostream stream, uint32_t version);
	~MLR_I_C_DeT_TMesh(void);

public:
	MLR_I_C_DeT_TMesh(ClassData* class_data = MLR_I_C_DeT_TMesh::DefaultData);

	static MLR_I_C_DeT_TMesh* Make(std::iostream stream, uint32_t version);

	void Save(std::iostream stream);

public:
#if COLOR_AS_DWORD
	virtual void SetColorData(pcuint32_t array, size_t point_count);
	virtual void GetColorData(puint32_t* array, psize_t point_count);
#else
	virtual void SetColorData(const Stuff::RGBAColor* array, size_t point_count);
	virtual void GetColorData(Stuff::RGBAColor** array, psize_t point_count);
#endif

	virtual void
#if COLOR_AS_DWORD
	PaintMe(pcuint32_t paintMe);
#else
	PaintMe(const Stuff::RGBAColor* paintMe);
#endif

	virtual uint32_t TransformAndClip(
		Stuff::Matrix4D*, MLRClippingState, GOSVertexPool*, bool = false);

	virtual void TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*, bool = false);

	void Copy(MLR_I_C_TMesh*, MLRState detailState, float xOff, float yOff, float xFac, float yFac);

	bool Copy(MLR_I_C_DeT_PMesh*);

	virtual void HurtMe(const Stuff::LinearMatrix4D&, float radius);

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
		size_t ret = MLR_I_DeT_TMesh::GetSize();
		ret += colors.GetSize();
		return ret;
	}

protected:
#if COLOR_AS_DWORD
	std::vector<uint32_t> colors; // Base address of color list
	std::vector<uint32_t>* actualColors;
#else
	std::vector<Stuff::RGBAColor> colors; // Base address of color list
	std::vector<Stuff::RGBAColor>* actualColors;
#endif
};

MLR_I_C_DeT_TMesh* CreateIndexedTriCube_Color_NoLit_DetTex(float, MLRState*);
MLRShape* CreateIndexedTriIcosahedron_Color_NoLit_DetTex(IcoInfo&, MLRState*, MLRState*);
} // namespace MidLevelRenderer
#endif
