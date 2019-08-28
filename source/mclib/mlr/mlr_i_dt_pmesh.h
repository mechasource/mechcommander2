//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_DT_PMESH_HPP
#define MLR_MLR_I_DT_PMESH_HPP

#include "mlr/gosvertex.h"
#include "mlr/mlr_i_pmesh.h"

namespace MidLevelRenderer
{

//##########################################################################
//#### MLRIndexedPolyMesh with no color no lighting two texture layer  #####
//##########################################################################

class MLR_I_DT_PMesh : public MLR_I_PMesh
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
	MLR_I_DT_PMesh(ClassData* class_data, std::iostream stream, uint32_t version);
	~MLR_I_DT_PMesh(void);

public:
	MLR_I_DT_PMesh(ClassData* class_data = MLR_I_DT_PMesh::DefaultData);

	static MLR_I_DT_PMesh* Make(std::iostream stream, uint32_t version);

	void Save(std::iostream stream);

public:
	//		void Copy(MLRIndexedPolyMesh*);

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
		virtual uint16_t*
		GetGOSIndices(size_t pass = 0)
		{
			// Check_Object(this);
			if(pass == 0)
				return gos_indices;
			else
				return gos_indices2;
		}
#endif
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
	MLRState state2, referenceState2;

	static std::vector<Stuff::Vector2DScalar>* texCoords2; // Max_Number_Vertices_Per_Mesh
	static std::vector<Stuff::Vector2DScalar>*
		clipExtraTexCoords2; // Max_Number_Vertices_Per_Mesh
};

MLR_I_DT_PMesh*
CreateIndexedCube_Nocolour_NoLit_2Tex(float, MLRState*, MLRState*);
MLRShape*
CreateIndexedIcosahedron_Nocolour_NoLit_2Tex(IcoInfo&, MLRState*, MLRState*);
} // namespace MidLevelRenderer
#endif
