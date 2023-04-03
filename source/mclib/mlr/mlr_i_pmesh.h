//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_PMESH_HPP
#define MLR_MLR_I_PMESH_HPP

#include "stuff/plane.h"
#include "mlr/mlrindexedprimitivebase.h"

namespace Stuff
{
class Plane;
}

namespace MidLevelRenderer
{

//##########################################################################
//#### MLRIndexedPolyMesh with no color no lighting one texture layer #####
//##########################################################################

class MLR_I_PMesh : public MLRIndexedPrimitiveBase
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
	MLR_I_PMesh(ClassData* class_data, std::iostream& stream, uint32_t version);
	~MLR_I_PMesh(void);

public:
	MLR_I_PMesh(ClassData* class_data = MLR_I_PMesh::DefaultData);

	static MLR_I_PMesh* Make(std::iostream stream, uint32_t version);
	void Save(std::ostream& stream)
	{
		// Check_Object(this);
		//Check_Object(stream);
		MLRIndexedPrimitiveBase::Save(stream);
	}

public:
	// void Copy(MLRIndexedPolyMesh*);

	virtual void InitializeDrawPrimitive(uint8_t, int32_t = 0);

	virtual void SetSubprimitiveLengths(uint8_t*, size_t);
	virtual void GetSubprimitiveLengths(uint8_t**, size_t*);

	void FindFacePlanes(void);

	virtual int32_t FindBackFace(const Stuff::Point3D&);

	const Stuff::Plane* GetPolygonPlane(size_t ix)
	{
		// Check_Object(this);
		_ASSERT(ix < facePlanes.size());
		return &facePlanes[ix];
	}

	virtual void Lighting(MLRLight* const*, uint32_t nrLights);
	virtual void LightMapLighting(MLRLight*);

#if COLOR_AS_DWORD
	virtual void PaintMe(const uint32_t* paintMe)
	{
		(void)paintMe;
	};
#else
	virtual void PaintMe(const Stuff::RGBAcolour* paintMe)
	{
		(void)paintMe;
	};
#endif

	virtual uint32_t TransformAndClip(
		Stuff::Matrix4D*, MLRClippingState, GOSVertexPool*, bool = false);
	bool CastRay(Stuff::Line3D* line, Stuff::Normal3D* normal);
	virtual void TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*, bool = false);

	// Initializes the visibility test list
	void ResetTestList(void);

	// find which vertices are visible which not - returns nr of visible
	// vertices the result is stored in the visibleIndexedVertices array
	uint32_t FindVisibleVertices(void);

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
		size_t ret = MLRIndexedPrimitiveBase::GetSize();
		ret += testList.size();
		ret += facePlanes.size();
		return ret;
	}

protected:
	void Transform(Stuff::Matrix4D*);
	std::vector<uint8_t> testList;
	std::vector<Stuff::Plane> facePlanes;
};

#define ICO_X 0.525731112119133606f
#define ICO_Z 0.850650808352039932f

extern float vdata[12][3];
extern uint32_t tindices[20][3];

extern uint32_t triDrawn;

void subdivide(Stuff::Point3D* coords, Stuff::Point3D& v1, Stuff::Point3D& v2, Stuff::Point3D& v3,
	uint32_t depth, uint32_t tri2draw, float rad = 1.0f);

MLR_I_PMesh*
CreateIndexedCube_Nocolour_NoLit(float, MLRState*);
MLRShape*
CreateIndexedIcosahedron_Nocolour_NoLit(IcoInfo&, MLRState*);
} // namespace MidLevelRenderer
#endif
