//==========================================================================//
// File:	 gosFX_Shape.cpp											    //
// Contents: Base gosFX::Shape Component									//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//
//############################################################################
//########################  gosFX::Shape__Specification
//#############################
//############################################################################

#include "stdinc.h"
#include "gosfx/gosfxheaders.h"

//------------------------------------------------------------------------------
//
gosFX::Shape__Specification::Shape__Specification(std::iostream stream, uint32_t gfx_version)
	: Singleton__Specification(gosFX::ShapeClassID, stream, gfx_version)
{
	// Check_Pointer(this);
	_ASSERT(m_class == ShapeClassID);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	//
	//---------------
	// Load the shape
	//---------------
	//
	m_shape = MidLevelRenderer::MLRShape::Make(stream, MidLevelRenderer::ReadMLRVersion(stream));
	Register_Object(m_shape);
	*stream >> m_radius;
}

//------------------------------------------------------------------------------
//
gosFX::Shape__Specification::Shape__Specification(MidLevelRenderer::MLRShape* shape)
	: Singleton__Specification(gosFX::ShapeClassID)
{
	// Check_Pointer(this);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	m_shape = nullptr;
	SetShape(shape);
}

//------------------------------------------------------------------------------
//
gosFX::Shape__Specification::~Shape__Specification()
{
	// Check_Object(this);
	if (m_shape)
	{
		Check_Object(m_shape);
		m_shape->DetachReference();
	}
}

//------------------------------------------------------------------------------
//
gosFX::Shape__Specification*
gosFX::Shape__Specification::Make(
	std::iostream stream, uint32_t gfx_version)
{
	Check_Object(stream);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	Shape__Specification* spec = new gosFX::Shape__Specification(stream, gfx_version);
	// gos_PopCurrentHeap();
	return spec;
}

//------------------------------------------------------------------------------
//
void gosFX::Shape__Specification::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	Singleton__Specification::Save(stream);
	MidLevelRenderer::WriteMLRVersion(stream);
	m_shape->Save(stream);
	*stream << m_radius;
}

//------------------------------------------------------------------------------
//
void gosFX::Shape__Specification::Copy(Shape__Specification* spec)
{
	// Check_Object(this);
	Check_Object(spec);
	Singleton__Specification::Copy(spec);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	m_radius = spec->m_radius;
	m_shape = spec->m_shape;
	// gos_PopCurrentHeap();
	Check_Object(m_shape);
	m_shape->AttachReference();
}

//------------------------------------------------------------------------------
//
void gosFX::Shape__Specification::SetShape(MidLevelRenderer::MLRShape* shape)
{
	// Check_Object(this);
	//
	//------------------------------------
	// Detach the old shape if it is there
	//------------------------------------
	//
	if (m_shape)
	{
		Check_Object(m_shape);
		m_shape->DetachReference();
	}
	//
	//------------------------------------
	// Attach the new shape if it is there
	//------------------------------------
	//
	if (shape)
	{
		Check_Object(shape);
		m_shape = shape;
		m_shape->AttachReference();
		//
		//-----------------------------------------------------------------
		// Get the radius of the bounding sphere.  This will be the largest
		// distance any point is from the origin
		//-----------------------------------------------------------------
		//
		m_radius = 0.0f;
		int32_t count = m_shape->GetNum();
		for (size_t i = 0; i < count; ++i)
		{
			MidLevelRenderer::MLRPrimitiveBase* primitive = m_shape->Find(i);
			Check_Object(primitive);
			Stuff::Point3D* points;
			int32_t vertex_count;
			primitive->GetCoordData(&points, &vertex_count);
			for (size_t v = 0; v < vertex_count; ++v)
			{
				float len = points[v].GetLengthSquared();
				if (len > m_radius)
					m_radius = len;
			}
		}
		m_radius = Stuff::Sqrt(m_radius);
	}
}

//############################################################################
//##############################  gosFX::Shape  ################################
//############################################################################

gosFX::Shape::ClassData* gosFX::Shape::DefaultData = nullptr;

//------------------------------------------------------------------------------
//
void gosFX::Shape::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	DefaultData = new ClassData(ShapeClassID, "gosFX::Shape", Singleton::DefaultData,
		(Effect::Factory)&Make, (Specification::Factory)&Specification::Make);
	Register_Object(DefaultData);
}

//------------------------------------------------------------------------------
//
void gosFX::Shape::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//------------------------------------------------------------------------------
//
gosFX::Shape::Shape(Specification* spec, uint32_t flags)
	: Singleton(DefaultData, spec, flags)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	m_radius = spec->m_radius;
}

//------------------------------------------------------------------------------
//
gosFX::Shape*
gosFX::Shape::Make(Specification* spec, uint32_t flags)
{
	Check_Object(spec);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	Shape* cloud = new gosFX::Shape(spec, flags);
	// gos_PopCurrentHeap();
	return cloud;
}

//------------------------------------------------------------------------------
//
void gosFX::Shape::Draw(DrawInfo* info)
{
	// Check_Object(this);
	Check_Object(info);
	Check_Object(info->m_parentToWorld);
	//
	//----------------------------
	// Set up the common draw info
	//----------------------------
	//
	MidLevelRenderer::DrawScalableShapeInformation dinfo;
	MidLevelRenderer::MLRShape* shape = GetSpecification()->m_shape;
	dinfo.clippingFlags.SetClippingState(0x3f);
	dinfo.worldToShape = nullptr;
	Specification* spec = GetSpecification();
	Check_Object(spec);
	dinfo.state.Combine(info->m_state, spec->m_state);
	dinfo.activeLights = nullptr;
	dinfo.nrOfActiveLights = 0;
	dinfo.shape = shape;
	Stuff::Vector3D scale(m_scale, m_scale, m_scale);
	dinfo.scaling = &scale;
	dinfo.paintMe = &m_color;
	Stuff::LinearMatrix4D local_to_world;
	local_to_world.Multiply(m_localToParent, *info->m_parentToWorld);
	dinfo.shapeToWorld = &local_to_world;
	//
	//--------------------------------------------------------------
	// Check the orientation mode.  The first case is XY orientation
	//--------------------------------------------------------------
	//
	if (spec->m_alignZUsingX)
	{
		Stuff::Point3D camera_in_world(info->m_clipper->GetCameraToWorldMatrix());
		Stuff::Point3D card_in_world(local_to_world);
		Stuff::Vector3D look_at;
		look_at.Subtract(camera_in_world, card_in_world);
		if (spec->m_alignZUsingY)
			local_to_world.AlignLocalAxisToWorldVector(
				look_at, Stuff::Z_Axis, Stuff::Y_Axis, Stuff::X_Axis);
		else
			local_to_world.AlignLocalAxisToWorldVector(look_at, Stuff::Z_Axis, Stuff::X_Axis, -1);
	}
	//
	//-------------------------------------------------------
	// Each matrix needs to be aligned to the camera around Y
	//-------------------------------------------------------
	//
	else if (spec->m_alignZUsingY)
	{
		Stuff::Point3D camera_in_world(info->m_clipper->GetCameraToWorldMatrix());
		Stuff::Point3D card_in_world(local_to_world);
		Stuff::Vector3D look_at;
		look_at.Subtract(camera_in_world, card_in_world);
		local_to_world.AlignLocalAxisToWorldVector(look_at, Stuff::Z_Axis, Stuff::Y_Axis, -1);
	}
	//
	//----------------------------
	// Let our parent do its thing
	//----------------------------
	//
	info->m_clipper->DrawScalableShape(&dinfo);
	Singleton::Draw(info);
}

//------------------------------------------------------------------------------
//
void gosFX::Shape::TestInstance(void) const
{
	_ASSERT(IsDerivedFrom(DefaultData));
}
