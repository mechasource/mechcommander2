//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRINDEXEDPRIMITIVE_HPP
#define MLR_MLRINDEXEDPRIMITIVE_HPP

// #include <mlr/mlr.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//###################    MLRIndexedPrimitive    ############################
	//##########################################################################

	class MLRIndexedPrimitive:
		public MLRPrimitive
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
		MLRIndexedPrimitive(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			int32_t version
			);
		~MLRIndexedPrimitive(void);

	public:
		MLRIndexedPrimitive(ClassData *class_data);

		static MLRIndexedPrimitive*
			Make(
			Stuff::MemoryStream *stream,
			int32_t version
			);

		virtual void
			Save(Stuff::MemoryStream *stream);

		virtual	void	InitializeDrawPrimitive(int32_t, int32_t=0);

		virtual void	Lighting(MLRLight**, int32_t nrLights);

		virtual void
			SetCoordData(
			const Stuff::Point3D *array,
			int32_t point_count
			);

		virtual void
			SetIndexData(
			puint16_t index_array,
			int32_t index_count
			);

		virtual void
			GetIndexData(
			puint16_t* index_array,
			pint32_t index_count
			);

		puint16_t GetGOSIndices(void)
		{ Check_Object(this); return gos_indices; }

		int32_t
			GetNumGOSIndices(void)
		{ Check_Object(this); return numGOSIndices; }

		virtual void
			Transform(Stuff::Matrix4D*);

		virtual void
			TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*) = 0;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Testing
		//
	public:
		void TestInstance(void) const;

		virtual int32_t GetSize(void)
		{ 
			Check_Object(this);
			int32_t ret = MLRPrimitive::GetSize(void);
			ret += visibleIndexedVertices.GetSize(void);
			ret += index.GetSize(void);

			return ret;
		}

	protected:
		bool visibleIndexedVerticesKey;
		Stuff::DynamicArrayOf<uint16_t>	visibleIndexedVertices;
		Stuff::DynamicArrayOf<uint16_t>	index;	// List of color indexes 
		puint16_t	gos_indices;
		uint16_t	numGOSIndices;
	};

}
#endif