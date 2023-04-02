//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLRPRIMITIVEBASE_HPP
#define MLRPRIMITIVEBASE_HPP

//#include "stuff/marray.h"
#include "stuff/vector2d.h"
#include "stuff/vector4d.h"
#include "stuff/color.h"
#include "mlr/mlrstate.h"
#include "mlr/mlrclippingstate.h"

namespace Stuff
{
class Line3D;
class Normal3D;
class ExtentBox;
} // namespace Stuff

//#include "mlr/gosvertexpool.h"

namespace MidLevelRenderer
{

class MLRState;
class MLRLight;
class GOSVertex;
class GOSVertex2UV;
class GOSVertexPool;

struct ClipPolygon2
{
	void Init(int32_t);
	void Destroy(void);

	std::vector<Stuff::Vector4D> coords; // [Max_Number_Vertices_Per_Polygon]
#if COLOR_AS_DWORD
	std::vector<uint32_t> colors; //[Max_Number_Vertices_Per_Polygon];
#else
	std::vector<Stuff::RGBAcolour> colors; //[Max_Number_Vertices_Per_Polygon];
#endif
	std::vector<Stuff::Vector2DScalar> texCoords; //[2*Max_Number_Vertices_Per_Polygon];
	std::vector<MLRClippingState> clipPerVertex; //[Max_Number_Vertices_Per_Polygon];
};

class MLRPrimitiveBase__ClassData;

//##########################################################################
//#################### MLRPrimitiveBase ##############################
//##########################################################################
// this is the abstract base class for all geometry. it has contains geometry
// with one and only one renderer state !!!

// typedef Stuff::Vector2DOf<float> Stuff::Vector2DScalar;

class _declspec(novtable) MLRPrimitiveBase // // : public Stuff::RegisteredClass
{
	friend class MLRShape;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialization
	//
public:
#if CONSIDERED_OBSOLETE
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);
	typedef MLRPrimitiveBase__ClassData ClassData;
	static ClassData* DefaultData;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	MLRPrimitiveBase(ClassData* class_data);

protected:
	MLRPrimitiveBase(ClassData* class_data, std::iostream& stream, uint32_t version);
#endif

protected:
	~MLRPrimitiveBase(void);

public:
	typedef MLRPrimitiveBase* (*Factory)(std::iostream stream, uint32_t version);

	static MLRPrimitiveBase* Make(std::iostream stream, uint32_t version);
	virtual void Save(std::iostream stream);

	// Subprimitives are units in which this geometry is split off, ie. nr of
	// polygons in a polygon mesh or number of tripstrips in a  tristriped mesh,
	// every of this subprimitives has another/ number which is type specific
	// ie. number of vertices in a polygon or number of vertices in a tristrip
	// the data for the coord/color/texcoord/normal or index ARE IN THIS ORDER
	virtual size_t GetNumPrimitives(void)
	{
		// Check_Object(this);
		return lengths.size();
	}

	virtual void SetSubprimitiveLengths(uint8_t* length_array, size_t subprimitive_count) = 0;

	// returns the number of subprimitives
	void GetSubprimitiveLengths(uint8_t** length_array, int32_t*);

	int32_t GetSubprimitiveLength(int32_t i) const;

	// ==============================================================

	virtual void SetReferenceState(const MLRState& _state, size_t pass = 0)
	{
		// Check_Object(this);
		(void)pass;
		referenceState = _state;
	};
	virtual const MLRState& GetReferenceState(size_t pass = 0) const
	{
		// Check_Object(this);
		(void)pass;
		return referenceState;
	};
	virtual const MLRState& GetCurrentState(size_t pass = 0) const
	{
		// Check_Object(this);
		(void)pass;
		return state;
	};

	virtual void CombineStates(const MLRState& master)
	{
		// Check_Object(this);
		state.Combine(master, referenceState);
	};

	size_t GetNumVertices(void)
	{
		// Check_Object(this);
		return coords.size();
	}

	virtual void SetCoordData(const Stuff::Point3D* array, size_t point_count);
	virtual void GetCoordData(Stuff::Point3D** array, size_t* point_count);
	virtual void SetTexCoordData(const Stuff::Vector2DScalar* array, size_t point_count, size_t pass = 0);
	virtual void GetTexCoordData(Stuff::Vector2DScalar** array, size_t* point_count, size_t pass = 0);

	// is to call befor clipping, parameter: camera point
	virtual int32_t FindBackFace(const Stuff::Point3D&) = 0;
	virtual void Lighting(MLRLight* const*, uint32_t nrLights) = 0;
	static void InitializeDraw(void);
	virtual void InitializeDrawPrimitive(uint8_t, int32_t = 0);

	uint8_t GetVisible(void)
	{
		// Check_Object(this);
		return visible;
	}

	virtual GOSVertex* GetGOSVertices(uint32_t = 0)
	{
		// Check_Object(this);
		return gos_vertices;
	}

	uint32_t GetNumGOSVertices(void)
	{
		// Check_Object(this);
		return numGOSVertices;
	}

	virtual GOSVertex2UV* GetGOSVertices2UV(uint32_t = 0)
	{
		// Check_Object(this);
		return nullptr;
	}

	uint32_t GetSortDataMode(void)
	{
		// Check_Object(this);
		return drawMode;
	}

	virtual bool CastRay(Stuff::Line3D* line, Stuff::Normal3D* normal);
	virtual void PaintMe(const Stuff::RGBAcolour* paintMe) = 0;
	virtual uint32_t TransformAndClip(
		Stuff::Matrix4D*, MLRClippingState, GOSVertexPool*, bool = false) = 0;
	virtual void TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*, bool = false) = 0;

	virtual uint32_t GetNumPasses(void)
	{
		// Check_Object(this);
		return passes;
	}

	virtual void HurtMe(const Stuff::LinearMatrix4D&, float /*radius*/)
	{
		// Check_Object(this);
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// This functions using the static buffers
	//
	void SetClipCoord(Stuff::Point3D& point, size_t index)
	{
		// Check_Object(this);
		_ASSERT(clipExtraCoords.size() > index);
		(clipExtraCoords)[index].x = point.x;
		(clipExtraCoords)[index].y = point.y;
		(clipExtraCoords)[index].z = point.z;
	}
	void FlashClipCoords(size_t num)
	{
		// Check_Object(this);
		_ASSERT(clipExtraCoords.size() > num);
		coords.resize(num);
		for (size_t i = 0u; i < num; i++)
		{
			coords[i].x = (clipExtraCoords)[i].x;
			coords[i].y = (clipExtraCoords)[i].y;
			coords[i].z = (clipExtraCoords)[i].z;
		}
	}
	void SetClipTexCoord(Stuff::Vector2DScalar& uvs, size_t index)
	{
		// Check_Object(this);
		_ASSERT(clipExtraTexCoords.size() > index);
		_ASSERT(MLRState::GetHasMaxUVs() ? (uvs[0] >= -100.0 && uvs[0] <= 100.0) : 1);
		_ASSERT(MLRState::GetHasMaxUVs() ? (uvs[1] >= -100.0 && uvs[1] <= 100.0) : 1);
		(clipExtraTexCoords)[index] = uvs;
	}
	void FlashClipTexCoords(size_t num)
	{
		// Check_Object(this);
		// _ASSERT(clipExtraTexCoords.size() > num);	// ?

		// texCoords.resize(num);
		// TODO: solve V501 warning
		// Mem_Copy(texCoords.GetData(), clipExtraTexCoords->GetData(),
		//	sizeof(Stuff::Vector2DScalar) * num, sizeof(Stuff::Vector2DScalar) * num);

		for (auto i = 0u; i < num; i++)
			texCoords.push_back(clipExtraTexCoords[i]);
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Reference counting
	//
public:
	void AttachReference(void)
	{
		// Check_Object(this);
		++referenceCount;
	}
	void DetachReference(void)
	{
		// Check_Object(this);
		_ASSERT(referenceCount > 0);
		if ((--referenceCount) == 0)
		{
			Unregister_Object(this);
			delete this;
		}
	}
	size_t GetReferenceCount(void) { return referenceCount; }

protected:
	size_t referenceCount;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
public:
	void TestInstance(void) const;

	virtual size_t GetSize(void)
	{
		// Check_Object(this);
		size_t ret = 0;
		ret += coords.size();
		ret += texCoords.size();
		ret += lengths.size();
		return ret;
	}

	void GetExtend(Stuff::ExtentBox* box);

protected:
	virtual void Transform(Stuff::Matrix4D*);

	static ClipPolygon2* clipBuffer;

	uint8_t visible; // primitive visibilty per frame
	uint8_t passes;

	// int32_t numPrimitives; // Number of primitives, e.g. - num quads
	// Replaced by GetNumPrimitives

	// int32_t numvertices; // number of verts for stats and vert arrays
	// Replaced by GetNumVertices

	std::vector<Stuff::Point3D> coords; // Base address of coordinate list
	std::vector<Stuff::Vector2DScalar> texCoords; // Base address of texture coordinate list

	static std::vector<Stuff::Vector4D>* transformedCoords;

	std::vector<uint8_t> lengths; // List of strip lengths

#if COLOR_AS_DWORD // clipExtracolours for the future generations !!!
	static std::vector<uint32_t> clipExtracolours; // , Max_Number_Vertices_Per_Mesh
#else
	static std::vector<Stuff::RGBAcolour> clipExtracolours; // , Max_Number_Vertices_Per_Mesh
#endif

	static std::vector<MLRClippingState> clipPerVertex; // , Max_Number_Vertices_Per_Mesh
	static std::vector<Stuff::Vector4D> clipExtraCoords; // , Max_Number_Vertices_Per_Mesh
	static std::vector<Stuff::Vector2DScalar> clipExtraTexCoords; // , Max_Number_Vertices_Per_Mesh
	static std::vector<uint16_t> clipExtraLength; // , Max_Number_Primitives_Per_Frame

	MLRState state;
	MLRState referenceState;

	uint32_t drawMode;

	GOSVertex* gos_vertices;
	uint16_t numGOSVertices;
};

struct IcoInfo
{
	uint32_t type;
	uint32_t depth;
	bool indexed;
	float radius;
	float all;
	bool onOff;
	const std::wstring_view& GetTypeName(void);
};

MLRShape*
CreateIndexedIcosahedron(IcoInfo&, std::vector<MLRState>*);

//##########################################################################
//################### MLRPrimitiveBase__ClassData ####################
//##########################################################################

class MLRPrimitiveBase__ClassData // : public Stuff::RegisteredClass::ClassData
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
public:
	MLRPrimitiveBase__ClassData(Stuff::RegisteredClass::ClassID class_id, const std::wstring_view& class_name,
		Stuff::RegisteredClass::ClassData* parent_class,
		MLRPrimitiveBase::Factory primitive_factory) :
		RegisteredClass__ClassData(class_id, class_name, parent_class),
		primitiveFactory(primitive_factory)
	{
	}

	MLRPrimitiveBase::Factory primitiveFactory;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
public:
	void TestInstance(void);
};

struct ClipData2
{
	Stuff::Vector4D* coords;
#if COLOR_AS_DWORD
	uint32_t* colors;
#else
	Stuff::RGBAcolour* colors;
#endif
	Stuff::Vector2DScalar* texCoords;
	MLRClippingState* clipPerVertex;

	uint16_t length;
};

inline float
GetBC(uint32_t nr, const Stuff::Vector4D& v4d)
{
	switch (nr)
	{
	case 0:
		return (v4d.w - v4d.y);
	case 1:
		return v4d.y;
	case 2:
		return (v4d.w - v4d.x);
	case 3:
		return v4d.x;
	case 4:
		return v4d.z;
	case 5:
		return (v4d.w - v4d.z);
	}
	return 0.0f;
}

inline void
GetDoubleBC(uint32_t nr, float& result1, float& result2, const Stuff::Vector4D& v4d1,
	const Stuff::Vector4D& v4d2)
{
	switch (nr)
	{
	case 0:
		result1 = (v4d1.w - v4d1.y);
		result2 = (v4d2.w - v4d2.y);
		break;
	case 1:
		result1 = v4d1.y;
		result2 = v4d2.y;
		break;
	case 2:
		result1 = (v4d1.w - v4d1.x);
		result2 = (v4d2.w - v4d2.x);
		break;
	case 3:
		result1 = v4d1.x;
		result2 = v4d2.x;
		break;
	case 4:
		result1 = v4d1.z;
		result2 = v4d2.z;
		break;
	case 5:
		result1 = (v4d1.w - v4d1.z);
		result2 = (v4d2.w - v4d2.z);
		break;
	}
}

inline float
GetLerpFactor(uint32_t nr, const Stuff::Vector4D& v4d1, const Stuff::Vector4D& v4d2)
{
	float result1 = 0.0f;
	float result2 = 0.0f;
	switch (nr)
	{
	case 0:
		result1 = (v4d1.w - v4d1.y);
		result2 = (v4d2.w - v4d2.y);
		break;
	case 1:
		result1 = v4d1.y;
		result2 = v4d2.y;
		break;
	case 2:
		result1 = (v4d1.w - v4d1.x);
		result2 = (v4d2.w - v4d2.x);
		break;
	case 3:
		result1 = v4d1.x;
		result2 = v4d2.x;
		break;
	case 4:
		result1 = v4d1.z;
		result2 = v4d2.z;
		break;
	case 5:
		result1 = (v4d1.w - v4d1.z);
		result2 = (v4d2.w - v4d2.z);
		break;
	default:
		NODEFAULT;
		break;
	}
	if (result1 == 0.0f)
	{
		return 0.0f;
	}
	_ASSERT(!Stuff::Close_Enough(result1, result2, Stuff::SMALL * 0.1f));
	return result1 / (result1 - result2);
}
} // namespace MidLevelRenderer
#endif
