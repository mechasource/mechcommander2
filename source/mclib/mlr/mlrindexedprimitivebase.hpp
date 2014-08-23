//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRINDEXEDPRIMITIVEBASE_HPP
#define MLR_MLRINDEXEDPRIMITIVEBASE_HPP

//#include <mlr/mlr.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//###################    MLRIndexedPrimitiveBase    ########################
	//##########################################################################

	class MLRIndexedPrimitiveBase:
		public MLRPrimitiveBase
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Initialization
		//
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);
		static ClassData* DefaultData;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors/Destructors
		//
	protected:
		MLRIndexedPrimitiveBase(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			int version
			);
		~MLRIndexedPrimitiveBase();

	public:
		MLRIndexedPrimitiveBase(ClassData *class_data);

		static MLRIndexedPrimitiveBase*
			Make(
			Stuff::MemoryStream *stream,
			int version
			);

		virtual void
			Save(Stuff::MemoryStream *stream);

		virtual	void	InitializeDrawPrimitive(uint8_t, int=0);

		virtual void	Lighting(MLRLight* const*, int nrLights) = 0;

		virtual void
			SetCoordData(
			const Stuff::Point3D *array,
			int point_count
			);

		virtual void
			SetIndexData(
			puint16_t index_array,
			int index_count
			);

		virtual void
			GetIndexData(
			puint16_t *index_array,
			int *index_count
			);

		virtual puint16_t
			GetGOSIndices(int=0)
		{ Check_Object(this); return gos_indices; }

		int
			GetNumGOSIndices()
		{ Check_Object(this); return numGOSIndices; }

		virtual void
			Transform(Stuff::Matrix4D*);

		virtual void
			TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*,bool=false) = 0;

		void
			TheIndexer(int num)
		{
			Check_Object(this);
			index.SetLength(num);
			for(uint16_t i=0;i<num;i++)
			{
				index[i] = i;
			}
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Testing
		//
	public:
		void TestInstance(void) const;

		virtual int
			GetSize()
		{ 
			Check_Object(this);
			int ret = MLRPrimitiveBase::GetSize();
			ret += visibleIndexedVertices.GetSize();
			ret += index.GetSize();

			return ret;
		}

		bool
			CheckIndicies();

	protected:
		bool visibleIndexedVerticesKey;

		Stuff::DynamicArrayOf<uint8_t>	visibleIndexedVertices;

		Stuff::DynamicArrayOf<uint16_t>	index;	// List of color indexes 

		static Stuff::DynamicArrayOf<uint16_t> *clipExtraIndex; // , Max_Number_Vertices_Per_Mesh

		puint16_t gos_indices;
		uint16_t	numGOSIndices;
	};

}
#endif
