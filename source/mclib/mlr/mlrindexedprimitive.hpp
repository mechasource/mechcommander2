//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

/* this header is not used */

#pragma once

#ifndef MLR_MLRINDEXEDPRIMITIVE_HPP
#define MLR_MLRINDEXEDPRIMITIVE_HPP

#include <mlr/mlrprimitive.hpp>

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
			uint32_t version
			);
		~MLRIndexedPrimitive(void);

	public:
		MLRIndexedPrimitive(ClassData *class_data);

		static MLRIndexedPrimitive*
			Make(
			Stuff::MemoryStream *stream,
			uint32_t version
			);

		virtual void
			Save(Stuff::MemoryStream *stream);

		virtual	void	InitializeDrawPrimitive(int32_t, int32_t=0);

		virtual void	Lighting(MLRLight**, uint32_t nrLights);

		virtual void SetCoordData(const Stuff::Point3D *array, size_t point_count);
		virtual void SetIndexData(puint16_t index_array, size_t index_count);
		virtual void GetIndexData(puint16_t* index_array, psize_t index_count);

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

		virtual size_t GetSize(void)
		{ 
			Check_Object(this);
			size_t ret = MLRPrimitive::GetSize();
			ret += visibleIndexedVertices.GetSize();
			ret += index.GetSize();

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