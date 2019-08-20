//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_C_TMESH_HPP
#define MLR_MLR_I_C_TMESH_HPP

#include <mlr/mlr_i_tmesh.hpp>

namespace MidLevelRenderer
{

class MLR_I_C_PMesh;

//##########################################################################
//#### MLRIndexedTriMesh with no color no lighting one texture layer  #####
//##########################################################################

class MLR_I_C_TMesh : public MLR_I_TMesh
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
	MLR_I_C_TMesh(ClassData* class_data, std::iostream stream, uint32_t version);
	~MLR_I_C_TMesh(void);

public:
	MLR_I_C_TMesh(ClassData* class_data = MLR_I_C_TMesh::DefaultData);

	static MLR_I_C_TMesh* Make(std::iostream stream, uint32_t version);

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

	bool Copy(MLR_I_C_PMesh*);

	void HurtMe(const Stuff::LinearMatrix4D& pain, float radius);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// This functions using the static buffers
	//
	void
#if COLOR_AS_DWORD
	SetClipColor(uint32_t& color, size_t index)
#else
	SetClipColor(Stuff::RGBAColor& color, size_t index)
#endif
	{
		// Check_Object(this);
		_ASSERT(clipExtraColors.size() > index);
		(*clipExtraColors)[index] = color;
	}

	void FlashClipColors(size_t num)
	{
		// Check_Object(this);
		_ASSERT(clipExtraTexCoords.size() > num);
		colors.SetLength(num);
		visibleIndexedVertices.SetLength(num);
#if COLOR_AS_DWORD
		Mem_Copy(colors.GetData(), clipExtraColors->GetData(), sizeof(uint32_t) * num,
			sizeof(uint32_t) * num);
#else
		Mem_Copy(colors.GetData(), clipExtraColors->GetData(), sizeof(Stuff::RGBAColor) * num,
			sizeof(Stuff::RGBAColor) * num);
#endif
	}
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
		size_t ret = MLR_I_TMesh::GetSize();
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

MLR_I_C_TMesh* CreateIndexedTriCube_Color_NoLit(float, MLRState*);
MLRShape* CreateIndexedTriIcosahedron_Color_NoLit(IcoInfo&, MLRState*);
} // namespace MidLevelRenderer

#endif // MLR_MLR_I_C_TMESH_HPP