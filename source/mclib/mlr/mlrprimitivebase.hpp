//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved. //
//===========================================================================//

#pragma once

#ifndef MLRPRIMITIVEBASE_HPP
#define MLRPRIMITIVEBASE_HPP

#include <stuff/marray.hpp>
#include <stuff/vector2d.hpp>
#include <stuff/vector4d.hpp>
#include <stuff/color.hpp>
#include <mlr/mlrstate.hpp>
#include <mlr/mlrclippingstate.hpp>

namespace Stuff{
	class Line3D;
	class Normal3D;
	class ExtentBox;
}

//#include <mlr/gosvertexpool.hpp>

namespace MidLevelRenderer{

	class MLRState;
	class MLRLight;
	class GOSVertex;
	class GOSVertex2UV;
	class GOSVertexPool;

	struct ClipPolygon2
	{
		void Init(int32_t);
		void Destroy(void);

		Stuff::DynamicArrayOf<Stuff::Vector4D> coords; // [Max_Number_Vertices_Per_Polygon]
#if COLOR_AS_DWORD
		Stuff::DynamicArrayOf<uint32_t> colors; //[Max_Number_Vertices_Per_Polygon];
#else
		Stuff::DynamicArrayOf<Stuff::RGBAColor> colors; //[Max_Number_Vertices_Per_Polygon];
#endif
		Stuff::DynamicArrayOf<Stuff::Vector2DScalar> texCoords; //[2*Max_Number_Vertices_Per_Polygon];
		Stuff::DynamicArrayOf<MLRClippingState> clipPerVertex; //[Max_Number_Vertices_Per_Polygon];
	};


	class MLRPrimitiveBase__ClassData;

	//##########################################################################
	//#################### MLRPrimitiveBase ##############################
	//##########################################################################
	// this is the abstract base class for all geometry. it has contains geometry
	// with one and only one renderer state !!!

	//typedef Stuff::Vector2DOf<float> Stuff::Vector2DScalar;

	class _declspec(novtable) MLRPrimitiveBase:
		public Stuff::RegisteredClass
	{
		friend class MLRShape;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Initialization
		//
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);
		typedef MLRPrimitiveBase__ClassData ClassData;
		static ClassData* DefaultData;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors/Destructors
		//
	protected:
		MLRPrimitiveBase(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			uint32_t version
			);

		~MLRPrimitiveBase(void);

	public:
		MLRPrimitiveBase(ClassData *class_data);

		typedef MLRPrimitiveBase* (*Factory)(Stuff::MemoryStream *stream, uint32_t version);

		static MLRPrimitiveBase* Make(Stuff::MemoryStream *stream, uint32_t version);

		virtual void Save(Stuff::MemoryStream *stream);

		// Subprimitves are units in which this geometry is split off
		// ie. nr of polygons in a polygon mesh or number of tripstrips
		// in a tristriped mesh, every of this subprimitves has another
		// number which is type specific
		// ie. number of vertices in a polygon or number of vertices in
		// a tristrip
		// the data for the coord/color/texcoord/normal or index
		// ARE IN THIS ORDER
		virtual size_t GetNumPrimitives(void)
		{
			Check_Object(this); return lengths.GetLength();
		}

		virtual void SetSubprimitiveLengths(puint8_t length_array, size_t subprimitive_count) = 0;

		// returns the number of subprimitives
		void GetSubprimitiveLengths(puint8_t *length_array, pint32_t);

		int32_t GetSubprimitiveLength(int32_t i) const;

		// ==============================================================

		virtual void SetReferenceState(const MLRState& _state, size_t pass=0)
		{ Check_Object(this); (void)pass; referenceState = _state; };
		virtual const MLRState& GetReferenceState(size_t pass=0) const
		{ Check_Object(this);(void)pass; return referenceState; };
		virtual const MLRState& GetCurrentState(size_t pass=0) const
		{ Check_Object(this); (void)pass; return state; };

		virtual void CombineStates(const MLRState& master)
		{ Check_Object(this); state.Combine(master, referenceState); };

		size_t GetNumVertices(void)
		{ Check_Object(this); return coords.GetLength(); }

		virtual void SetCoordData(const Stuff::Point3D* array, size_t point_count);
		virtual void GetCoordData(Stuff::Point3D** array, psize_t point_count);
		virtual void SetTexCoordData(const Stuff::Vector2DScalar* array, size_t point_count, size_t pass=0);
		virtual void GetTexCoordData(Stuff::Vector2DScalar** array, psize_t point_count, size_t pass=0);

		// is to call befor clipping, parameter: camera point
		virtual int32_t FindBackFace(const Stuff::Point3D&) = 0;

		virtual void Lighting(MLRLight* const*, uint32_t nrLights) = 0;

		static void InitializeDraw(void);

		virtual void InitializeDrawPrimitive(uint8_t, int32_t=0);

		int32_t GetVisible ()
		{ Check_Object(this); return visible; }

		virtual GOSVertex* GetGOSVertices(uint32_t=0)
		{
			Check_Object(this); return gos_vertices;
		}

		uint32_t GetNumGOSVertices(void)
		{
			Check_Object(this); return numGOSVertices;
		}

		virtual GOSVertex2UV* GetGOSVertices2UV(uint32_t=0)
		{ Check_Object(this); return NULL; }

		int32_t
			GetSortDataMode()
		{ Check_Object(this); return drawMode; }

		virtual bool
			CastRay(
			Stuff::Line3D *line,
			Stuff::Normal3D *normal
			);

		virtual void
			PaintMe(const Stuff::RGBAColor *paintMe) = 0;

		virtual int32_t
			TransformAndClip(Stuff::Matrix4D*, MLRClippingState, GOSVertexPool*,bool=false) = 0;

		virtual void
			TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*,bool=false) = 0;

		virtual uint32_t GetNumPasses(void)
		{ Check_Object(this); return passes; }

		virtual void HurtMe(const Stuff::LinearMatrix4D&, float /*radius*/)
		{ Check_Object(this); }

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// This functions using the static buffers
		//
		void
			SetClipCoord(Stuff::Point3D &point, size_t index)
		{
			Check_Object(this); Verify(clipExtraCoords->GetLength() > index);
			(*clipExtraCoords)[index].x = point.x;
			(*clipExtraCoords)[index].y = point.y;
			(*clipExtraCoords)[index].z = point.z;
		}
		void
			FlashClipCoords(size_t num)
		{
			Check_Object(this); Verify(clipExtraCoords->GetLength() > num);
			coords.SetLength(num);
			for(size_t i=0;i<num;i++)
			{
				coords[i].x = (*clipExtraCoords)[i].x;
				coords[i].y = (*clipExtraCoords)[i].y;
				coords[i].z = (*clipExtraCoords)[i].z;
			}
		}
		void
			SetClipTexCoord(Stuff::Vector2DScalar &uvs, size_t index)
		{
			Check_Object(this); Verify(clipExtraTexCoords->GetLength() > index);
			Verify( MLRState::GetHasMaxUVs() ? (uvs[0]>=-100.0 && uvs[0]<=100.0) : 1);
			Verify( MLRState::GetHasMaxUVs() ? (uvs[1]>=-100.0 && uvs[1]<=100.0) : 1);

			(*clipExtraTexCoords)[index] = uvs;
		}
		void
			FlashClipTexCoords(size_t num)
		{
			Check_Object(this); Verify(clipExtraTexCoords->GetLength() > num);
			texCoords.SetLength(num);
			Mem_Copy(texCoords.GetData(), clipExtraTexCoords->GetData(), sizeof(Stuff::Vector2DScalar)*num, sizeof(Stuff::Vector2DScalar)*num);
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Reference counting
		//
	public:
		void
			AttachReference()
		{Check_Object(this); ++referenceCount;}
		void
			DetachReference()
		{
			Check_Object(this); Verify(referenceCount > 0);
			if ((--referenceCount) == 0)
			{
				Unregister_Object(this);
				delete this;
			}
		}

		size_t GetReferenceCount(void) {return referenceCount;}

	protected:
		size_t referenceCount;

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
			ret += texCoords.GetSize();
			ret += lengths.GetSize();

			return ret;
		}

		void GetExtend(Stuff::ExtentBox *box);

	protected:
		virtual void Transform(Stuff::Matrix4D*);

		static ClipPolygon2 *clipBuffer;

		uint8_t visible; // primitive visibilty per frame
		uint8_t passes;

		// int32_t numPrimitives; // Number of primitives, e.g. - num quads
		// Replaced by GetNumPrimitives

		// int32_t numVertices; // number of verts for stats and vert arrays
		// Replaced by GetNumVertices

		Stuff::DynamicArrayOf<Stuff::Point3D> coords; // Base address of coordinate list
		Stuff::DynamicArrayOf<Stuff::Vector2DScalar> texCoords; // Base address of texture coordinate list

		static Stuff::DynamicArrayOf<Stuff::Vector4D> *transformedCoords;

		Stuff::DynamicArrayOf<uint8_t> lengths; // List of strip lengths

#if COLOR_AS_DWORD// clipExtraColors for the future generations !!!
		static Stuff::DynamicArrayOf<uint32_t> *clipExtraColors; // , Max_Number_Vertices_Per_Mesh
#else
		static Stuff::DynamicArrayOf<Stuff::RGBAColor> *clipExtraColors; // , Max_Number_Vertices_Per_Mesh
#endif

		static Stuff::DynamicArrayOf<MLRClippingState> *clipPerVertex; // , Max_Number_Vertices_Per_Mesh

		static Stuff::DynamicArrayOf<Stuff::Vector4D> *clipExtraCoords; // , Max_Number_Vertices_Per_Mesh
		static Stuff::DynamicArrayOf<Stuff::Vector2DScalar> *clipExtraTexCoords; // , Max_Number_Vertices_Per_Mesh

		static Stuff::DynamicArrayOf<uint16_t> *clipExtraLength; // , Max_Number_Primitives_Per_Frame

		MLRState state, referenceState;

		int32_t drawMode;

		GOSVertex* gos_vertices;
		uint16_t numGOSVertices;
	};

	struct IcoInfo{
		int32_t type;
		int32_t depth;
		bool indexed;
		float radius;
		float all;
		bool onOff;
		PCSTR GetTypeName(void);
	};

	MLRShape*
		CreateIndexedIcosahedron(
		IcoInfo&,
		Stuff::DynamicArrayOf<MLRState>*
		);

	//##########################################################################
	//################### MLRPrimitiveBase__ClassData ####################
	//##########################################################################

	class MLRPrimitiveBase__ClassData:
		public Stuff::RegisteredClass::ClassData
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//
	public:
		MLRPrimitiveBase__ClassData(
			Stuff::RegisteredClass::ClassID class_id,
			PCSTR class_name,
			Stuff::RegisteredClass::ClassData *parent_class,
			MLRPrimitiveBase::Factory primitive_factory) 
			: RegisteredClass__ClassData(class_id, class_name, parent_class), 
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
		Stuff::Vector4D *coords;
#if COLOR_AS_DWORD
		uint32_t *colors;
#else
		Stuff::RGBAColor *colors;
#endif
		Stuff::Vector2DScalar *texCoords;
		MLRClippingState *clipPerVertex;

		uint16_t length;
	};

#if 0// still defined in "MLRPrimitive.hpp"
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

	inline void
		GetDoubleBC
		(
		int32_t nr,
		float& result1,
		float& result2,
		const Stuff::Vector4D& v4d1,
		const Stuff::Vector4D& v4d2
		)
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

	inline float
		GetLerpFactor
		(
		int32_t nr,
		const Stuff::Vector4D& v4d1,
		const Stuff::Vector4D& v4d2
		)
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
			Abort_Program("Invalid plane number used !");
			break;
		}
		Verify(!Stuff::Close_Enough(result1, result2));
		return result1 / (result1 - result2);
	}

#endif

	inline float GetBC(uint32_t nr, const Stuff::Vector4D& v4d)
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
		uint32_t nr, float& result1, float& result2, const Stuff::Vector4D& v4d1, 
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
		uint32_t nr, const Stuff::Vector4D& v4d1, const Stuff::Vector4D& v4d2)
	{
		float result1;
		float result2;

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
}
#endif
