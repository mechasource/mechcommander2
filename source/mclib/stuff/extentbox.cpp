//===========================================================================//
// File:	extntbox.cc                                                      //
// Contents: Implementation details of bounding box class                    //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
//#include "stuff/stuffheaders.h"

////#include "gameos.hpp"
#include "stuff/scalar.h"
#include "stuff/obb.h"
// #include "stuff/mstring.h"
#include "stuff/extentbox.h"

// using namespace Stuff;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
ExtentBox::ExtentBox(const Vector3D& min, const Vector3D& max)
{
	// Check_Pointer(this);
	Check_Object(&min);
	Check_Object(&max);
	if (min.x <= max.x)
	{
		minX = min.x;
		maxX = max.x;
	}
	else
	{
		minX = max.x;
		maxX = min.x;
	}
	if (min.y <= max.y)
	{
		minY = min.y;
		maxY = max.y;
	}
	else
	{
		minY = max.y;
		maxY = min.y;
	}
	if (min.z <= max.z)
	{
		minZ = min.z;
		maxZ = max.z;
	}
	else
	{
		minZ = max.z;
		maxZ = min.z;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
ExtentBox::ExtentBox(const ExtentBox& box)
{
	// Check_Pointer(this);
	Check_Object(&box);
	minX = box.minX;
	maxX = box.maxX;
	minY = box.minY;
	maxY = box.maxY;
	minZ = box.minZ;
	maxZ = box.maxZ;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
ExtentBox::ExtentBox(const OBB& obb)
{
	// Check_Pointer(this);
	Check_Object(&obb);
	Point3D center(obb.localToParent);
	minX = center.x - obb.sphereRadius;
	maxX = center.x + obb.sphereRadius;
	minY = center.y - obb.sphereRadius;
	maxY = center.y + obb.sphereRadius;
	minZ = center.z - obb.sphereRadius;
	maxZ = center.z + obb.sphereRadius;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
ExtentBox&
ExtentBox::Intersect(const ExtentBox& box_1, const ExtentBox& box_2)
{
	// Check_Pointer(this);
	Check_Object(&box_1);
	Check_Object(&box_2);
	_ASSERT(box_1.minX <= box_2.maxX);
	_ASSERT(box_1.maxX >= box_2.minX);
	_ASSERT(box_1.minY <= box_2.maxY);
	_ASSERT(box_1.maxY >= box_2.minY);
	_ASSERT(box_1.minZ <= box_2.maxZ);
	_ASSERT(box_1.maxZ >= box_2.minZ);
	minX = Max(box_1.minX, box_2.minX);
	maxX = Min(box_1.maxX, box_2.maxX);
	minY = Max(box_1.minY, box_2.minY);
	maxY = Min(box_1.maxY, box_2.maxY);
	minZ = Max(box_1.minZ, box_2.minZ);
	maxZ = Min(box_1.maxZ, box_2.maxZ);
	return *this;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
ExtentBox&
ExtentBox::Union(const ExtentBox& box_1, const ExtentBox& box_2)
{
	// Check_Pointer(this);
	Check_Object(&box_1);
	Check_Object(&box_2);
	minX = Min(box_1.minX, box_2.minX);
	maxX = Max(box_1.maxX, box_2.maxX);
	minY = Min(box_1.minY, box_2.minY);
	maxY = Max(box_1.maxY, box_2.maxY);
	minZ = Min(box_1.minZ, box_2.minZ);
	maxZ = Max(box_1.maxZ, box_2.maxZ);
	// Check_Object(this);
	return *this;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
ExtentBox&
ExtentBox::Union(const ExtentBox& box, const Vector3D& point)
{
	// Check_Pointer(this);
	Check_Object(&box);
	Check_Object(&point);
	minX = Min(box.minX, point.x);
	maxX = Max(box.maxX, point.x);
	minY = Min(box.minY, point.y);
	maxY = Max(box.maxY, point.y);
	minZ = Min(box.minZ, point.z);
	maxZ = Max(box.maxZ, point.z);
	return *this;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Vector3D*
ExtentBox::Constrain(Vector3D* point) const
{
	// Check_Object(this);
	Check_Object(point);
	Clamp(point->x, minX, maxX);
	Clamp(point->y, minY, maxY);
	Clamp(point->z, minZ, maxZ);
	return point;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool ExtentBox::Contains(const Vector3D& point) const
{
	// Check_Object(this);
	Check_Object(&point);
	return minX <= point.x && maxX >= point.x && minY <= point.y && maxY >= point.y && minZ <= point.z && maxZ >= point.z;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool ExtentBox::Contains(const ExtentBox& box) const
{
	// Check_Object(this);
	Check_Object(&box);
	return minX <= box.minX && maxX >= box.maxX && minY <= box.minY && maxY >= box.maxY && minZ <= box.minZ && maxZ >= box.maxZ;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool ExtentBox::Intersects(const ExtentBox& box) const
{
	// Check_Object(this);
	Check_Object(&box);
	return minX <= box.maxX && maxX >= box.minX && minY <= box.maxY && maxY >= box.minY && minZ <= box.maxZ && maxZ >= box.minZ;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void ExtentBox::GetCenterpoint(Point3D* point) const
{
	// Check_Object(this);
	Check_Pointer(point);
	point->x = 0.5f * (minX + maxX);
	point->y = 0.5f * (minY + maxY);
	point->z = 0.5f * (minZ + maxZ);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void ExtentBox::TestInstance(void) const
{
	_ASSERT(minX <= maxX && minY <= maxY && minZ <= maxZ);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
#if !defined(Spew)
void Spew(std::wstring_view group, const ExtentBox& box)
{
	Check_Object(&box);
	SPEW((group, "[%f..%f,%f..%f,%f..%f]+", box.minX, box.maxX, box.minY, box.maxY, box.minZ,
		box.maxZ));
}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~ ExtentBox functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::Convert_From_Ascii(std::wstring_view str, ExtentBox* extent_box)
{
	Check_Pointer(str);
	Check_Object(extent_box);
	std::wstring_view parse_string(str);
	Check_Object(&parse_string);
	std::wstring_view token = parse_string.GetNthToken(0);
	extent_box->minX = AtoF(token);
	token = parse_string.GetNthToken(1);
	extent_box->minY = AtoF(token);
	token = parse_string.GetNthToken(2);
	extent_box->minZ = AtoF(token);
	token = parse_string.GetNthToken(3);
	extent_box->maxX = AtoF(token);
	token = parse_string.GetNthToken(4);
	extent_box->maxY = AtoF(token);
	token = parse_string.GetNthToken(5);
	extent_box->maxZ = AtoF(token);
	Check_Object(extent_box);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::Use_Scalar_In_Sorted_Array(std::vector<float>* values, float value, uint32_t* max_index,
	uint32_t block_size, float threshold)
{
	Check_Object(values);
	Check_Pointer(max_index);
	//
	//-------------------------------------------------------------
	// First, look to see if the table contains the specified value
	//-------------------------------------------------------------
	//
	uint32_t bottom = 0;
	uint32_t top = *max_index;
	while (top > bottom)
	{
		uint32_t middle = (top + bottom - 1) >> 1;
		if (Close_Enough((*values)[middle], value, threshold))
		{
			return;
		}
		else if ((*values)[middle] > value)
		{
			top = middle;
		}
		else
		{
			bottom = middle + 1;
		}
	}
	_ASSERT(top == bottom);
	//
	//-----------------------------------------------------------------------
	// Since we got here, a new value must be added to the table.  First make
	// room in the table
	//-----------------------------------------------------------------------
	//
	if (*max_index == values.size())
	{
		values->SetLength(*max_index + block_size);
	}
	uint32_t to_move = *max_index - bottom;
	_ASSERT(to_move <= *max_index);
	if (to_move > 0)
	{
		memmove(&(*values)[bottom + 1], &(*values)[bottom], to_move * sizeof(value));
	}
	++(*max_index);
	(*values)[bottom] = value;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Stuff::Find_Planes_Of_Boxes(std::vector<Plane>* planes, const std::vector<ExtentBox>& boxes)
{
	Check_Object(planes);
	Check_Object(&boxes);
	//
	//------------------------------------------------------------
	// Figure out all the unique axis components used by the boxes
	//------------------------------------------------------------
	//
	std::vector<float> xs;
	std::vector<float> ys;
	std::vector<float> zs;
	uint32_t max_x = 0;
	uint32_t max_y = 0;
	uint32_t max_z = 0;
	size_t count = boxes.GetLength();
	uint32_t i;
	_ASSERT(count > 0);
	for (i = 0; i < count; ++i)
	{
		Use_Scalar_In_Sorted_Array(&xs, boxes[i].minX, &max_x, 10);
		Use_Scalar_In_Sorted_Array(&xs, boxes[i].maxX, &max_x, 10);
		Use_Scalar_In_Sorted_Array(&ys, boxes[i].minY, &max_y, 10);
		Use_Scalar_In_Sorted_Array(&ys, boxes[i].maxY, &max_y, 10);
		Use_Scalar_In_Sorted_Array(&zs, boxes[i].minZ, &max_z, 10);
		Use_Scalar_In_Sorted_Array(&zs, boxes[i].maxZ, &max_z, 10);
	}
	//
	//------------------------------------------------
	// There will be a plane for each unique component
	//------------------------------------------------
	//
	count = max_x + max_y + max_z;
	planes->SetLength(count);
	//
	//------------------------
	// Generate the Y-Z planes
	//------------------------
	//
	Plane* plane = &(*planes)[0];
	for (i = 0; i < max_x; ++i)
	{
		_ASSERT(static_cast<uint32_t>(plane - &(*planes)[0]) < count);
		plane->normal.x = 1.0f;
		plane->normal.y = 0.0f;
		plane->normal.z = 0.0f;
		plane->offset = xs[i];
		++plane;
	}
	//
	//------------------------
	// Generate the X-Z planes
	//------------------------
	//
	for (i = 0; i < max_y; ++i)
	{
		_ASSERT(static_cast<uint32_t>(plane - &(*planes)[0]) < count);
		plane->normal.x = 0.0f;
		plane->normal.y = 1.0f;
		plane->normal.z = 0.0f;
		plane->offset = ys[i];
		++plane;
	}
	//
	//------------------------
	// Generate the X-Y planes
	//------------------------
	//
	for (i = 0; i < max_z; ++i)
	{
		_ASSERT(static_cast<uint32_t>(plane - &(*planes)[0]) < count);
		plane->normal.x = 0.0f;
		plane->normal.y = 0.0f;
		plane->normal.z = 1.0f;
		plane->offset = zs[i];
		++plane;
	}
}
