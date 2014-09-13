//===========================================================================//
// File:	extntbox.hh                                                      //
// Contents: Interface specification of bounding box class                   //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _EXTENTBOX_HPP_
#define _EXTENTBOX_HPP_

#include <stuff/plane.hpp>

namespace Stuff
{
	class ExtentBox;
}

#if !defined(Spew)
void
Spew(
	PCSTR group,
	const Stuff::ExtentBox& box
);
#endif

namespace Stuff
{

	class Vector3D;
	class Point3D;
	class NotationFile;
	class OBB;

//~~~~~~~~~~~~~~~~~~~~~~~~~ ExtentBox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class ExtentBox
	{
	public:
		float
		minX,
		maxX,
		minY,
		maxY,
		minZ,
		maxZ;

		ExtentBox() {}
		ExtentBox(
			const Vector3D& min,
			const Vector3D& max
		);
		ExtentBox(const ExtentBox& box);
		explicit ExtentBox(const OBB& obb);

		const float&
		operator[](int32_t index) const
		{
			Check_Object(this);
			return (&minX)[index];
		}
		float&
		operator[](int32_t index)
		{
			Check_Object(this);
			return (&minX)[index];
		}

		ExtentBox&
		Intersect(
			const ExtentBox& box_1,
			const ExtentBox& box_2
		);
		ExtentBox&
		Union(
			const ExtentBox& box_1,
			const ExtentBox& box_2
		);
		ExtentBox&
		Union(
			const ExtentBox& box_1,
			const Vector3D& point
		);
		Vector3D*
		Constrain(Vector3D* point) const;

		bool
		Contains(const Vector3D& point) const;
		bool
		Contains(const ExtentBox& box) const;
		bool
		Intersects(const ExtentBox& box) const;

		void
		GetCenterpoint(Point3D* point) const;

		void TestInstance(void) const;
		static bool
		TestClass(void);
#if !defined(Spew)
		friend void
		::Spew(
			PCSTR group,
			const ExtentBox& box
		);
#endif
	};

//~~~~~~~~~~~~~~~~~~~~~~~~~ ExtentBox functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	void
	Convert_From_Ascii(
		PCSTR str,
		ExtentBox* extent_box
	);

	void
	Use_Scalar_In_Sorted_Array(
		DynamicArrayOf<float>* values,
		float value,
		puint32_t max_index,
		uint32_t block_size,
		float threshold = SMALL
	);

	void
	Find_Planes_Of_Boxes(
		DynamicArrayOf<Plane>* planes,
		const DynamicArrayOf<ExtentBox>& boxes
	);

}

namespace MemoryStreamIO
{

	inline Stuff::MemoryStream&
	Read(
		Stuff::MemoryStream* stream,
		Stuff::ExtentBox* output
	)
	{
		return stream->ReadBytes(output, sizeof(*output));
	}
	inline Stuff::MemoryStream&
	Write(
		Stuff::MemoryStream* stream,
		const Stuff::ExtentBox* input
	)
	{
		return stream->WriteBytes(input, sizeof(*input));
	}

}
#endif
