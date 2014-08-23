//===========================================================================//
// File:	ExtentPoly.hpp                                                   //
// Contents:																 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef STUFF_EXTENTPOLY_HPP
#define STUFF_EXTENTPOLY_HPP

#include <stuff/memorystream.hpp>
#include <stuff/filestream.hpp>

//#include <stuff/stuff.hpp>
//#include <stuff/scalar.hpp>
//#include <stuff/marray.hpp>
//#include <stuff/plane.hpp>


namespace Stuff {

	class Vector3D;
	class Point3D;
	class NotationFile;

	//~~~~~~~~~~~~~~~~~~~~~~~~~ ExtentPoly ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class ExtentPoly
	{
	public:
		float
			minY,
			maxY;
		int numberOfVertices;
		Vector3D *vertex;

		ExtentPoly() {}
		ExtentPoly(MemoryStream *stream);
		ExtentPoly(FileStream *file_stream,	PSTR page_name);
		~ExtentPoly(void);
		void
			Save(MemoryStream *stream);

		ExtentPoly&
			Intersect(
			const ExtentPoly &poly_1,
			const ExtentPoly &poly_2
			);
		ExtentPoly&
			Union(
			const ExtentPoly &poly_1,
			const ExtentPoly &poly_2
			);
		ExtentPoly&
			Union(
			const ExtentPoly &poly_1,
			const Vector3D &point
			);

		Vector3D*
			ClosestPointOnLine(
			Vector3D *point,
			const Vector3D &p1,
			const Vector3D &p2
			);

		float
			DistanceBetweenLineAndPoint(
			const Vector3D &point,
			const Vector3D &p1,
			const Vector3D &p2
			);
		Vector3D*
			Constrain(Vector3D *point);
		int
			InfiniteLineTestWithXAxis(
			const Vector3D &point,
			const Vector3D &p1,
			const Vector3D &p2
			);
		bool
			Contains(const Vector3D &point);
		void TestInstance(void) const;
		static bool
			TestClass();
	};

}

#endif
