//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_DT_TMESH_HPP
#define MLR_MLR_I_DT_TMESH_HPP

#include <mlr/gosvertex.hpp>
#include <mlr/mlr_i_tmesh.hpp>

namespace MidLevelRenderer
{

class MLR_I_DT_PMesh;

//##########################################################################
//## MLRIndexedTriangleMesh with no color no lighting two texture layer  ###
//##########################################################################

class MLR_I_DT_TMesh : public MLR_I_TMesh
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
	MLR_I_DT_TMesh(ClassData* class_data, std::iostream stream, uint32_t version);
	~MLR_I_DT_TMesh(void);

public:
	MLR_I_DT_TMesh(ClassData* class_data = MLR_I_DT_TMesh::DefaultData);

	static MLR_I_DT_TMesh* Make(std::iostream stream, uint32_t version);

	void Save(std::iostream stream);

public:
	bool Copy(MLR_I_DT_PMesh* pmesh);

	virtual uint32_t TransformAndClip(
		Stuff::Matrix4D*, MLRClippingState, GOSVertexPool*, bool = false);

	virtual void TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*, bool = false);
	void SetTexCoordData(const Stuff::Vector2DScalar* array, size_t point_count, size_t pass = 0);
	virtual void SetReferenceState(const MLRState& _state, size_t pass = 0)
	{
		// Check_Object(this);
		_ASSERT(pass < 2);
		if (pass == 0)
		{
			referenceState = _state;
		}
		else
		{
			passes = 2;
			referenceState2 = _state;
		}
	}
	virtual const MLRState& GetReferenceState(size_t pass = 0) const
	{
		// Check_Object(this);
		if (pass == 0)
			return referenceState;
		else
			return referenceState2;
	}
	virtual const MLRState& GetCurrentState(size_t pass = 0) const
	{
		// Check_Object(this);
		if (pass == 0)
			return state;
		else
			return state2;
	}

	virtual void CombineStates(const MLRState& master)
	{
		// Check_Object(this);
		state.Combine(master, referenceState);
		state2.Combine(master, referenceState2);
	};

	virtual GOSVertex* GetGOSVertices(uint32_t pass = 0)
	{
		// Check_Object(this);
		if (pass == 0)
			return gos_vertices;
		else
			return gos_vertices + numGOSVertices;
	}
#if 0
		virtual puint16_t
		GetGOSIndices(size_t pass = 0)
		{
			// Check_Object(this);
			if(pass == 0)
				return gos_indices;
			else
				return gos_indices2;
		}
#endif
	GOSVertex2UV* GetGOSVertices2UV(uint32_t = 0)
	{
		// Check_Object(this);
		return gos_vertices2uv;
	}

	//		int32_t
	//			GetNumGOSVertices2UV()
	//				{ // Check_Object(this); return numGOSVertices2uv; }

	virtual uint32_t GetNumPasses(void);

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
		return ret;
	}

protected:
	MLRState state2, referenceState2;

	static std::vector<Stuff::Vector2DScalar>* texCoords2; // Max_Number_Vertices_Per_Mesh
	static std::vector<Stuff::Vector2DScalar>*
		clipExtraTexCoords2; // Max_Number_Vertices_Per_Mesh

	GOSVertex2UV* gos_vertices2uv;
	//		uint16_t	numGOSVertices2uv;
};

MLR_I_DT_TMesh*
CreateIndexedTriCube_NoColor_NoLit_2Tex(float, MLRState*, MLRState*);

MLRShape*
CreateIndexedTriIcosahedron_NoColor_NoLit_2Tex(IcoInfo&, MLRState*, MLRState*);
} // namespace MidLevelRenderer
#endif
