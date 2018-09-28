//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_MT_PMESH_HPP
#define MLR_MLR_I_MT_PMESH_HPP

#include <mlr/gosvertex.hpp>
#include <mlr/mlr_i_pmesh.hpp>

namespace MidLevelRenderer
{

//##########################################################################
//### MLRIndexedPolyMesh with no color no lighting multi texture layer  ####
//##########################################################################

class MLR_I_MT_PMesh : public MLR_I_PMesh
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
	MLR_I_MT_PMesh(ClassData* class_data, std::iostream stream, uint32_t version);
	~MLR_I_MT_PMesh(void);

public:
	MLR_I_MT_PMesh(ClassData* class_data = MLR_I_MT_PMesh::DefaultData);

	static MLR_I_MT_PMesh* Make(std::iostream stream, uint32_t version);

	void Save(std::iostream stream);

public:
	void Copy(MLR_I_PMesh*);

	virtual uint32_t TransformAndClip(
		Stuff::Matrix4D*, MLRClippingState, GOSVertexPool*, bool = false);

	virtual void TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*, bool = false);

	void SetTexCoordData(const Stuff::Vector2DScalar* array, size_t point_count, size_t pass = 0);
	void GetTexCoordData(Stuff::Vector2DScalar** data, psize_t dataSize, size_t pass = 0);

	virtual void SetReferenceState(const MLRState& _state, size_t pass = 0)
	{
		// Check_Object(this);
		_ASSERT((intptr_t(pass) >= 0) && (pass < Limits::Max_Number_Of_Multitextures));
		if (pass == 0)
		{
			referenceState = _state;
		}
		multiReferenceState[pass] = _state;
	}

	virtual const MLRState& GetReferenceState(size_t pass = 0) const
	{
		// Check_Object(this);
		_ASSERT((intptr_t(pass) >= 0) && pass < Limits::Max_Number_Of_Multitextures);
		return multiReferenceState[pass];
	}
	virtual const MLRState& GetCurrentState(size_t pass = 0) const
	{
		// Check_Object(this);
		_ASSERT((intptr_t(pass) >= 0) && pass < Limits::Max_Number_Of_Multitextures);
		return multiState[pass];
	}

	virtual void CombineStates(const MLRState& master)
	{
		// Check_Object(this);
		state.Combine(master, referenceState);
		for (size_t i = 0; i < currentNrOfPasses; i++)
		{
			multiState[i].Combine(master, multiReferenceState[i]);
		}
	};

	virtual GOSVertex* GetGOSVertices(uint32_t pass = 0)
	{
		// Check_Object(this);
		_ASSERT((intptr_t(pass) >= 0) && pass < Limits::Max_Number_Of_Multitextures);
		return gos_vertices + pass * numGOSVertices;
	}

	virtual void InitializeDrawPrimitive(uint8_t, int32_t = 0);

	uint32_t GetNumPasses(void)
	{
		// Check_Object(this);
		return currentNrOfPasses;
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
		size_t ret = MLR_I_PMesh::GetSize();
		return ret;
	}

protected:
	void SetTexCoordDataPointer(const Stuff::Vector2DScalar*);

	uint8_t currentNrOfPasses;

	std::vector<MLRState> multiState, multiReferenceState;

	std::vector<std::vector<Stuff::Vector2DScalar>*>
		multiTexCoords; // Max_Number_Vertices_Per_Mesh

	std::vector<const Stuff::Vector2DScalar*> multiTexCoordsPointers;

	static std::vector<std::vector<Stuff::Vector2DScalar>>*
		clipExtraMultiTexCoords; // Max_Number_Vertices_Per_Mesh
	static std::vector<std::vector<Stuff::Vector2DScalar>>*
		extraMultiTexCoords; // Max_Number_Vertices_Per_Mesh
};

MLRShape* CreateIndexedIcosahedron_NoColor_NoLit_MultiTexture(
	IcoInfo&, std::vector<MLRState>*);
} // namespace MidLevelRenderer
#endif
