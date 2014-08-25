//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved. //
//===========================================================================//

/* this header is not used */

#pragma once

#ifndef MLR_MLRPRIMITIVE_HPP
#define MLR_MLRPRIMITIVE_HPP

#include <stuff/vector2d.hpp>
#include <stuff/plug.hpp>
#include <stuff/marray.hpp>
#include <stuff/color.hpp>
#include <mlr/mlrstate.hpp>
#include <mlr/mlrclippingstate.hpp>

namespace Stuff{
	class Point3D;
	class RGBAColor;
	class Vector3D;
	class Line3D;
	class Normal3D;
}

namespace MidLevelRenderer{

	class MLRState;
	class MLRLight;
	class GOSVertex;
	class GOSVertexPool;

	struct Statistics{
		static uint32_t MLR_TransformedVertices;
		static uint32_t MLR_LitVertices;
		static uint32_t MLR_Primitives;
		static uint32_t MLR_NonClippedVertices;
		static uint32_t MLR_ClippedVertices;
		static uint32_t MLR_PrimitiveKB;
		static uint32_t MLR_PolysClippedButOutside;
		static uint32_t MLR_PolysClippedButInside;
		static uint32_t MLR_PolysClippedButOnePlane;
		static uint32_t MLR_PolysClippedButGOnePlane;
		static gos_CycleData MLR_ClipTime;
		static uint32_t MLR_NumAllIndices;
		static uint32_t MLR_NumAllVertices;
		static float MLR_Index_Over_Vertex_Ratio;;
	};

	class MLRPrimitive__ClassData;

	//##########################################################################
	//####################### MLRPrimitive ###############################
	//##########################################################################
	// this is the abstract base class for all geometry. it has contains geometry
	// with one and only one renderer state !!!

	//typedef Stuff::Vector2DOf<float> Vector2DScalar;

	class _declspec(novtable) MLRPrimitive:
		public Stuff::Plug
	{
		friend class MLRShape;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Initialization
		//
	public:
		static void InitializeClass(void);
		static void TerminateClass(void);
		typedef MLRPrimitive__ClassData ClassData;
		static ClassData* DefaultData;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors/Destructors
		//
	protected:
		MLRPrimitive(
			ClassData* class_data, Stuff::MemoryStream *stream, uint32_t version);

		~MLRPrimitive(void);

	public:
		MLRPrimitive(ClassData *class_data);

		typedef MLRPrimitive* (*Factory)(Stuff::MemoryStream *stream, uint32_t version);
		static MLRPrimitive* Make(Stuff::MemoryStream *stream, uint32_t version);
		virtual void Save(Stuff::MemoryStream *stream);

		// Subprimitves are units in which this geometry is split off
		// ie. nr of polygons in a polygon mesh or number of tripstrips
		// in a tristriped mesh, every of this subprimitves has another
		// number which is type specific
		// ie. number of vertices in a polygon or number of vertices in
		// a tristrip
		// the data for the coord/color/texcoord/normal or index
		// ARE IN THIS ORDER
		size_t GetNumPrimitives(void)
		{
			Check_Object(this);
			return lengths.GetLength();
		}

		virtual void SetSubprimitiveLengths(puint8_t length_array, size_t subprimitive_count);

		// returns the number of subprimitives
		uint32_t GetSubprimitiveLengths(puint8_t* length_array);
		uint32_t GetSubprimitiveLength(uint32_t i) const;

		// ==============================================================

		void SetReferenceState(const MLRState& _state)
		{
			Check_Object(this); referenceState = _state;
		}
		const MLRState& GetReferenceState(void) const
		{ 
			Check_Object(this); return referenceState;
		}
		const MLRState& GetCurrentState(void) const
		{
			Check_Object(this); return state;
		}

		virtual void SetCoordData(const Stuff::Point3D* array, size_t point_count);
		virtual void GetCoordData(Stuff::Point3D** array, psize_t point_count);

#if COLOR_AS_DWORD
		virtual void SetColorData(pcuint32_t array, size_t point_count);
		virtual void GetColorData(uint32_t **array, psize_t point_count);
#else
		virtual void SetColorData(const Stuff::RGBAColor *array, size_t point_count);
		virtual void GetColorData(Stuff::RGBAColor **array, pint32_t point_count);
#endif

		virtual void SetNormalData(const Stuff::Vector3D* array, size_t point_count); 
		virtual void GetNormalData(Stuff::Vector3D** array, pint32_t point_count);

		virtual void SetTexCoordData(const Stuff::Vector2DScalar* array, size_t point_count, size_t pass=0);
		virtual void GetTexCoordData(Stuff::Vector2DScalar** array, psize_t point_count, size_t pass=0);

		// is to call before clipping, parameter: camera point
		virtual int32_t FindBackFace(const Stuff::Point3D&) = 0;
		virtual int32_t Clip(MLRClippingState, GOSVertexPool*) = 0;
		virtual void Lighting(MLRLight**, uint32_t nrLights);
		virtual void PaintMe(const Stuff::RGBAColor *paintMe);
		static void InitializeDraw(void);
		virtual void InitializeDrawPrimitive(int32_t, int32_t=0);

		int32_t GetVisible (void)
		{ Check_Object(this); return visible; }

		GOSVertex*
			GetGOSVertices(void)
		{ Check_Object(this); return gos_vertices; }

		int32_t
			GetNumGOSVertices(void)
		{ Check_Object(this); return numGOSVertices; }

		int32_t
			GetSortDataMode(void)
		{ Check_Object(this); return drawMode; }

		virtual bool
			CastRay(			Stuff::Line3D *line,
			Stuff::Normal3D *normal
			);

		virtual void
			Transform(Stuff::Matrix4D*);

		virtual void
			TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*) = 0;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Reference counting
		//
	public:
		void
			AttachReference(void)
		{Check_Object(this); ++referenceCount;}
		void
			DetachReference(void)
		{
			Check_Object(this); Verify(referenceCount > 0);
			if ((--referenceCount) == 0)
			{
				Unregister_Object(this);
				delete this;
			}
		}

		int32_t
			GetReferenceCount(void)
		{return referenceCount;}

	protected:
		int32_t
			referenceCount;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Testing
		//
	public:
		void TestInstance(void) const;

		virtual size_t GetSize(void)
		{
			Check_Object(this);
			size_t ret = 0;
			ret += coords.GetSize();
			ret += colors.GetSize();
			ret += normals.GetSize();
			ret += texCoords.GetSize();
			ret += litColors.GetSize();
			ret += transformedCoords.GetSize();
			ret += lengths.GetSize();

			return ret;
		}

	protected:
		int32_t visible; // primitive visibilty per frame

		// int32_t numPrimitives; // Number of primitives, e.g. - num quads
		// Replaced by GetNumPrimitives

		size_t numVertices; // number of verts for stats and vert arrays

		Stuff::DynamicArrayOf<Stuff::Point3D> coords;				// Base address of coordinate list
		Stuff::DynamicArrayOf<Stuff::Vector3D> normals;				// Base address of normal list
		Stuff::DynamicArrayOf<Stuff::Vector2DScalar> texCoords;		// Base address of texture coordinate list
		Stuff::DynamicArrayOf<Stuff::Vector4D> transformedCoords;

#if COLOR_AS_DWORD
		Stuff::DynamicArrayOf<uint32_t> colors; // Base address of color list
		Stuff::DynamicArrayOf<uint32_t> litColors;
		Stuff::DynamicArrayOf<uint32_t> *actualColors;

		static Stuff::StaticArrayOf<uint32_t, Max_Number_Vertices_Per_Mesh> clipExtraColors;
#else
		Stuff::DynamicArrayOf<Stuff::RGBAColor> colors; // Base address of color list
		Stuff::DynamicArrayOf<Stuff::RGBAColor> litColors;
		Stuff::DynamicArrayOf<Stuff::RGBAColor> *actualColors;

		static Stuff::DynamicArrayOf<Stuff::RGBAColor> clipExtraColors; // , Max_Number_Vertices_Per_Mesh
#endif

		Stuff::DynamicArrayOf<uint8_t> lengths; // List of strip lengths

		static Stuff::DynamicArrayOf<MLRClippingState> clipPerVertex; // , Max_Number_Vertices_Per_Mesh

		static Stuff::DynamicArrayOf<Stuff::Vector4D> clipExtraCoords; // , Max_Number_Vertices_Per_Mesh
		static Stuff::DynamicArrayOf<Stuff::Vector2DScalar> clipExtraTexCoords; // , Max_Number_Vertices_Per_Mesh

		static Stuff::DynamicArrayOf<int32_t> clipExtraIndex; // , Max_Number_Vertices_Per_Mesh

		static Stuff::DynamicArrayOf<uint16_t> clipExtraLength; // , Max_Number_Primitives_Per_Frame

		MLRState state, referenceState;

		int32_t drawMode;

		GOSVertex *gos_vertices;
		uint16_t numGOSVertices;
	};

	inline float
		GetBC(int32_t nr, const Stuff::Vector4D& v4d)
	{
		switch(nr)
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

	inline void GetDoubleBC(
		int32_t nr, float& result1, float& result2, const Stuff::Vector4D& v4d1,
		const Stuff::Vector4D& v4d2)
	{
		switch(nr)
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

	inline float GetLerpFactor(
		int32_t nr, const Stuff::Vector4D& v4d1, const Stuff::Vector4D& v4d2)
	{
		float result1, result2;

		switch(nr)
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
			result1 = 0.0f;
			result2 = 0.0f;
			STOP(("Invalid plane number used !"));
			break;
		}
		if(result1 == 0.0f)
		{
			return 0.0f;
		}
		Verify(!Stuff::Close_Enough(result1, result2, Stuff::SMALL*0.1f));
		return result1 / (result1 - result2);
	}


	//##########################################################################
	//##################### MLRPrimitive__ClassData ######################
	//##########################################################################

	class MLRPrimitive__ClassData:
		public Stuff::Plug::ClassData
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//
	public:
		MLRPrimitive__ClassData(Stuff::RegisteredClass::ClassID class_id, 
			PCSTR class_name, Stuff::Plug::ClassData *parent_class, 
			MLRPrimitive::Factory primitive_factory)
			: RegisteredClass__ClassData(class_id, class_name, parent_class), 
			primitiveFactory(primitive_factory)
		{
		}

		MLRPrimitive::Factory primitiveFactory;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//
	public:
		void TestInstance(void);
	};

	struct ClipPolygon
	{
		ClipPolygon(void);

		Stuff::DynamicArrayOf<Stuff::Vector4D> coords; // [Max_Number_Vertices_Per_Polygon]
#if COLOR_AS_DWORD
		Stuff::DynamicArrayOf<COLORREF> colors; // [Max_Number_Vertices_Per_Polygon]
#else
		Stuff::DynamicArrayOf<Stuff::RGBAColor> colors; // [Max_Number_Vertices_Per_Polygon]
#endif
		Stuff::DynamicArrayOf<Stuff::Vector2DScalar> texCoords; // [Max_Number_Vertices_Per_Polygon]
		Stuff::DynamicArrayOf<MLRClippingState> clipPerVertex; // [Max_Number_Vertices_Per_Polygon]
	};

	struct ClipData
	{
		Stuff::Vector4D* coords;
#if COLOR_AS_DWORD
		puint32_t colors;
#else
		Stuff::RGBAColor* colors;
#endif
		Stuff::Vector2DScalar* texCoords;
		MLRClippingState* clipPerVertex;

		uint32_t flags;
		uint32_t length;
	};

}
#endif
