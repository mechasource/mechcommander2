//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once
#define MLR_MLRINDEXEDPRIMITIVE_HPP

#if !defined(MLR_MLR_HPP)
	#include <mlr/mlr.hpp>
#endif

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
		static void
			InitializeClass(void);
		static void
			TerminateClass(void);
		static ClassData
			*DefaultData;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors/Destructors
	//
	protected:
		MLRIndexedPrimitive(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			int version
		);
		~MLRIndexedPrimitive(void);

	public:
		MLRIndexedPrimitive(ClassData *class_data);

		static MLRIndexedPrimitive*
			Make(
				Stuff::MemoryStream *stream,
				int version
			);

		virtual void
			Save(Stuff::MemoryStream *stream);

		virtual	void	InitializeDrawPrimitive(int, int=0);

		virtual void	Lighting(MLRLight**, int nrLights);

		virtual void
			SetCoordData(
				const Stuff::Point3D *array,
				int point_count
			);

		virtual void
			SetIndexData(
				USHORT *index_array,
				int index_count
			);

		virtual void
			GetIndexData(
				USHORT **index_array,
				int *index_count
			);

		USHORT*
			GetGOSIndices(void)
				{ Check_Object(this); return gos_indices; }

		int
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
		void
			TestInstance(void) const;

	virtual int
			GetSize(void)
		{ 
			Check_Object(this);
			int ret = MLRPrimitive::GetSize(void);
			ret += visibleIndexedVertices.GetSize(void);
			ret += index.GetSize(void);

			return ret;
		}

	protected:
		bool visibleIndexedVerticesKey;

		Stuff::DynamicArrayOf<USHORT>	visibleIndexedVertices;

		Stuff::DynamicArrayOf<USHORT>	index;	// List of color indexes 

		USHORT *gos_indices;
		USHORT	numGOSIndices;
	};

}
