//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_TERRAIN_HPP
#define MLR_MLR_TERRAIN_HPP

#include "mlr/mlr_i_det_tmesh.h"

namespace MidLevelRenderer
{

//##########################################################################
//#### MLRIndexedPolyMesh with no color no lighting w/ detail texture  #####
//##########################################################################

class MLR_Terrain : public MLR_I_DeT_TMesh
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
	MLR_Terrain(ClassData* class_data, std::iostream stream, uint32_t version);
	~MLR_Terrain(void);

public:
	MLR_Terrain(ClassData* class_data = MLR_Terrain::DefaultData);

	static MLR_Terrain* Make(std::iostream stream, uint32_t version);

	void Save(std::iostream stream);

public:
	virtual uint32_t TransformAndClip(
		Stuff::Matrix4D*, MLRClippingState, GOSVertexPool*, bool = false);

	virtual void TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*, bool = false);

	void SetUVData(float, float, float, float, float);

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
		return ret;
	}

protected:
	float borderPixelFun;
	float xUVFac, zUVFac, minX, minZ;

	static std::vector<Stuff::Vector2DScalar>*
		clipTexCoords; // , Max_Number_Vertices_Per_Mesh
};

MLRShape*
CreateIndexedTriIcosahedron_TerrainTest(IcoInfo&, MLRState*, MLRState*);
} // namespace MidLevelRenderer
#endif
