//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once
#define MLR_MLRPRIMITIVE_HPP

#if !defined(MLR_MLR_HPP)
	#include <mlr/mlr.hpp>
#endif

#if !defined(MLR_GOSVERTEXPOOL_HPP)
	#include <mlr/gosvertexpool.hpp>
#endif

namespace MidLevelRenderer {

	struct Statistics {
		static DWORD MLR_TransformedVertices;
		static DWORD MLR_LitVertices;
		static DWORD MLR_Primitives;
		static DWORD MLR_NonClippedVertices;
		static DWORD MLR_ClippedVertices;
		static DWORD MLR_PrimitiveKB;
		static DWORD MLR_PolysClippedButOutside;
		static DWORD MLR_PolysClippedButInside;
		static DWORD MLR_PolysClippedButOnePlane;
		static DWORD MLR_PolysClippedButGOnePlane;
		static gos_CycleData MLR_ClipTime;
		static DWORD MLR_NumAllIndices;
		static DWORD MLR_NumAllVertices;
		static float MLR_Index_Over_Vertex_Ratio;;
	};

	class MLRPrimitive__ClassData;

	//##########################################################################
	//#######################    MLRPrimitive    ###############################
	//##########################################################################
	// this is the abstract base class for all geometry. it has contains geometry
	// with one and only one renderer state !!!

	typedef Stuff::Vector2DOf<Stuff::Scalar> Vector2DScalar;

	class _declspec(novtable) MLRPrimitive:
		public Stuff::Plug
	{
		friend class MLRShape;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialization
	//
	public:
		static void
			InitializeClass(void);
		static void
			TerminateClass(void);
		typedef MLRPrimitive__ClassData ClassData;
		static ClassData
			*DefaultData;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors/Destructors
	//
	protected:
		MLRPrimitive(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			int version
		);

		~MLRPrimitive(void);

	public:
		MLRPrimitive(ClassData *class_data);

		typedef MLRPrimitive*
			(*Factory)(
				Stuff::MemoryStream *stream,
				int version
			);

		static MLRPrimitive*
			Make(
				Stuff::MemoryStream *stream,
				int version
			);

		virtual void
			Save(Stuff::MemoryStream *stream);

	// Subprimitves are units in which this geometry is split off
	// ie. nr of polygons in a polygon mesh or number of tripstrips
	// in a tristriped mesh, every of this subprimitves has another
	// number which is type specific
	// ie. number of vertices in a polygon or number of vertices in
	// a tristrip
	// the data for the coord/color/texcoord/normal or index
	// ARE IN THIS ORDER
		int
			GetNumPrimitives(void)
				{ Check_Object(this); return lengths.GetLength(void); }

		virtual void
			SetSubprimitiveLengths(
				unsigned char *length_array,
				int subprimitive_count
			);

	// returns the number of subprimitives
		int
			GetSubprimitiveLengths(unsigned char **length_array);

		int
			GetSubprimitiveLength(int i) const;

	// ==============================================================

		void	SetReferenceState(const MLRState& _state)
			{ Check_Object(this); referenceState = _state; };
		const MLRState&
			GetReferenceState(void) const
				{ Check_Object(this); return referenceState; }; 
		const MLRState&
			GetCurrentState(void) const
				{ Check_Object(this); return state; }; 

		virtual void
			SetCoordData(
				const Stuff::Point3D *array,
				int point_count
			);
		virtual void
			GetCoordData(
				Stuff::Point3D **array,
				int *point_count
			);

#if COLOR_AS_DWORD
		virtual void
			SetColorData(
				const DWORD *array,
				int point_count
			);
		virtual void
			GetColorData(
				DWORD **array,
				int *point_count
			);
#else
		virtual void
			SetColorData(
				const Stuff::RGBAColor *array,
				int point_count
			);
		virtual void
			GetColorData(
				Stuff::RGBAColor **array,
				int *point_count
			);
#endif

		virtual void
			SetNormalData(
				const Stuff::Vector3D *array,
				int point_count
			);
		virtual void
			GetNormalData(
				Stuff::Vector3D **array,
				int *point_count
			);

		virtual void
			SetTexCoordData(
				const Vector2DScalar *array,
				int point_count
			);
		virtual void
			GetTexCoordData(
				Vector2DScalar **array,
				int *point_count
			);

	//	is to call befor clipping, parameter: camera point
		virtual int	FindBackFace(const Stuff::Point3D&) = 0;

		virtual int	Clip(MLRClippingState, GOSVertexPool*) = 0;

		virtual void	Lighting(MLRLight**, int nrLights);

		virtual void	PaintMe(const Stuff::RGBAColor *paintMe);

		static	void	InitializeDraw(void);

		virtual	void	InitializeDrawPrimitive(int, int=0);

		int	GetVisible (void) 
			{ Check_Object(this); return visible; }

		GOSVertex*
			GetGOSVertices(void)
				{ Check_Object(this); return gos_vertices; }

		int
			GetNumGOSVertices(void)
				{ Check_Object(this); return numGOSVertices; }

		int
			GetSortDataMode(void)
				{ Check_Object(this); return drawMode; }

		virtual bool
			CastRay(
				Stuff::Line3D *line,
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

		int
			GetReferenceCount(void)
				{return referenceCount;}

	protected:
		int
			referenceCount;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
	public:
		void
			TestInstance(void) const;

		virtual int
			GetSize(void)
		{ 
			Check_Object(this);
			int ret = 0;
			ret += coords.GetSize(void);
			ret += colors.GetSize(void);
			ret += normals.GetSize(void);
			ret += texCoords.GetSize(void);
			ret += litColors.GetSize(void);
			ret += transformedCoords.GetSize(void);
			ret += lengths.GetSize(void);

			return ret;
		}

	protected:
		int		visible;	//	primitive visibilty per frame

//		int		numPrimitives;	// Number of primitives, e.g. - num quads 
//		Replaced by GetNumPrimitives

		int		numVertices;	// number of verts for stats and vert arrays

		Stuff::DynamicArrayOf<Stuff::Point3D> coords;	// Base address of coordinate list 
		Stuff::DynamicArrayOf<Stuff::Vector3D> normals;		// Base address of normal list 
		Stuff::DynamicArrayOf<Vector2DScalar> texCoords;	// Base address of texture coordinate list 

		Stuff::DynamicArrayOf<Stuff::Vector4D> transformedCoords;

#if COLOR_AS_DWORD
		Stuff::DynamicArrayOf<DWORD> colors;	// Base address of color list 
		Stuff::DynamicArrayOf<DWORD> litColors;
		Stuff::DynamicArrayOf<DWORD> *actualColors;

		static Stuff::StaticArrayOf<DWORD, Max_Number_Vertices_Per_Mesh> clipExtraColors;
#else
		Stuff::DynamicArrayOf<Stuff::RGBAColor> colors;	// Base address of color list 
		Stuff::DynamicArrayOf<Stuff::RGBAColor> litColors;
		Stuff::DynamicArrayOf<Stuff::RGBAColor> *actualColors;

		static Stuff::DynamicArrayOf<Stuff::RGBAColor> clipExtraColors;  // , Max_Number_Vertices_Per_Mesh
#endif

		Stuff::DynamicArrayOf<unsigned char>	lengths;	// List of strip lengths 

		static Stuff::DynamicArrayOf<MLRClippingState> clipPerVertex; // , Max_Number_Vertices_Per_Mesh

		static Stuff::DynamicArrayOf<Stuff::Vector4D> clipExtraCoords; // , Max_Number_Vertices_Per_Mesh
		static Stuff::DynamicArrayOf<Vector2DScalar> clipExtraTexCoords; // , Max_Number_Vertices_Per_Mesh

		static Stuff::DynamicArrayOf<int> clipExtraIndex; // , Max_Number_Vertices_Per_Mesh

		static Stuff::DynamicArrayOf<USHORT> clipExtraLength; // , Max_Number_Primitives_Per_Frame

		MLRState	state, referenceState;

		int drawMode;

		GOSVertex *gos_vertices;
		USHORT	numGOSVertices;
	};

	inline Stuff::Scalar
		GetBC(int nr, const Stuff::Vector4D& v4d) 
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
				int nr, 
				Stuff::Scalar& result1,
				Stuff::Scalar& result2,
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

	inline Stuff::Scalar
		GetLerpFactor
			(
				int nr, 
				const Stuff::Vector4D& v4d1, 
				const Stuff::Vector4D& v4d2
			)
	{
		Stuff::Scalar result1, result2;

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
	//#####################    MLRPrimitive__ClassData    ######################
	//##########################################################################

	class MLRPrimitive__ClassData:
		public Stuff::Plug::ClassData
	{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	public:
		MLRPrimitive__ClassData(
			Stuff::RegisteredClass::ClassID class_id,
			const char* class_name,
			Stuff::Plug::ClassData *parent_class,
			MLRPrimitive::Factory primitive_factory
		):
			RegisteredClass__ClassData(class_id, class_name, parent_class),
			primitiveFactory(primitive_factory)
				{}

		MLRPrimitive::Factory 
			primitiveFactory;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	public:
		void
			TestInstance(void);
	};

	struct ClipPolygon
	{
		ClipPolygon(void);

		Stuff::DynamicArrayOf<Stuff::Vector4D> coords; // [Max_Number_Vertices_Per_Polygon]
#if COLOR_AS_DWORD
		Stuff::DynamicArrayOf<DWORD> colors; // [Max_Number_Vertices_Per_Polygon]
#else
		Stuff::DynamicArrayOf<Stuff::RGBAColor> colors; // [Max_Number_Vertices_Per_Polygon]
#endif
		Stuff::DynamicArrayOf<Vector2DScalar> texCoords; // [Max_Number_Vertices_Per_Polygon]
		Stuff::DynamicArrayOf<MLRClippingState> clipPerVertex; // [Max_Number_Vertices_Per_Polygon]
	};

	struct ClipData
	{
		Stuff::Vector4D *coords;
#if COLOR_AS_DWORD
		DWORD *colors;
#else
		Stuff::RGBAColor *colors;
#endif
		Vector2DScalar *texCoords;
		MLRClippingState *clipPerVertex;

		int flags;
		USHORT length;
	};

}
