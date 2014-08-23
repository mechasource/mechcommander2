//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRLOOKUPLIGHT_HPP
#define MLR_MLRLOOKUPLIGHT_HPP

//#include <mlr/mlr.hpp>
//#include <mlr/mlrinfinitelight.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//########################    MLRLookUpLight    ############################
	//##########################################################################

	class MLRLookUpLight:
		public MLRInfiniteLight
	{
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);

		MLRLookUpLight();
		MLRLookUpLight(
			Stuff::MemoryStream *stream,
			int32_t version
			);
		MLRLookUpLight(Stuff::Page *page);
		~MLRLookUpLight();

		void
			Save(Stuff::MemoryStream *stream);
		void
			Write(Stuff::Page *page);

		virtual LightType
			GetLightType() 
		{ Check_Object(this); return LookUpLight; }


		virtual void
			LightVertex(const MLRVertexData&);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// LookUp Light Specific
		//
		void
			SetMapOrigin(Scalar x, Scalar y, Scalar z)
		{ Check_Object(this); mapOrigin.x = x; mapOrigin.y = y; mapOrigin.z = z; }

		Point3D
			GetMapOrigin()
		{ Check_Object(this); return mapOrigin; }

		void
			SetMapSizeAndName(int32_t x, int32_t z, PCSTR name);

		int32_t
			GetMapZoneCountX()
		{ Check_Object(this); return mapZoneCountX; }
		int32_t
			GetMapZoneCountZ()
		{ Check_Object(this); return mapZoneCountZ; }

		PCSTR
			GetMapName()
		{ Check_Object(this); return mapName; }

		void
			SetMapZoneSizeX(Scalar x)
		{ Check_Object(this); zoneSizeX = x; Verify(x>SMALL); one_Over_zoneSizeX = 1.0f/x; }
		void
			SetMapZoneSizeZ(Scalar z)
		{ Check_Object(this); zoneSizeZ = z; Verify(z>SMALL); one_Over_zoneSizeZ = 1.0f/z;  }

		Scalar
			GetMapZoneSizeX()
		{ Check_Object(this); return zoneSizeX; }
		Scalar
			GetMapZoneSizeZ()
		{ Check_Object(this); return zoneSizeZ; }

		void SetLightToShapeMatrix(const Stuff::LinearMatrix4D&);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Class Data Support
		//
	public:
		static ClassData* DefaultData;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Testing
		//
	public:
		void
			TestInstance();

	protected:
		bool
			LoadMap();

		Point3D mapOrigin;
		Scalar zoneSizeX, zoneSizeZ;
		Scalar one_Over_zoneSizeX, one_Over_zoneSizeZ;

		int32_t mapZoneCountX, mapZoneCountZ;
		MString mapName;

		puint8_t *maps;

		Stuff::LinearMatrix4D
			shapeToWorld;
	};

}
#endif
