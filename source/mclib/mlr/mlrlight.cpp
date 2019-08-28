//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/mlrlight.h"

namespace MidLevelRenderer {

//#############################################################################
//###########################    MLRLight    ##################################
//#############################################################################

MLRLight::ClassData* MLRLight::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRLight::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData =
		new ClassData(MLRLightClassID, "MidLevelRenderer::MLRLight", RegisteredClass::DefaultData);
	Register_Object(DefaultData);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRLight::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRLight::MLRLight(ClassData* class_data) :
	RegisteredClass(class_data)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	intensity = 1.0f;
	lightToWorld = LinearMatrix4D::Identity;
	lightToShape = LinearMatrix4D::Identity;
	color = RGBcolour(0.0f, 0.0f, 0.0f);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRLight::MLRLight(ClassData* class_data, std::iostream stream, uint32_t version) :
	RegisteredClass(class_data)
{
	Check_Object(stream);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	LinearMatrix4D matrix;
	*stream >> intensity >> color >> matrix;
	if (version >= 9)
	{
		*stream >> lightName;
	}
	else
	{
		lightName = "Light";
	}
	SetLightToWorldMatrix(matrix);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRLight::MLRLight(ClassData* class_data, Stuff::Page* page) :
	RegisteredClass(class_data)
{
	Check_Object(page);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	lightName = page->GetName();
	intensity = 1.0f;
	page->GetEntry("Intensity", &intensity);
	color = RGBcolour(0.0f, 0.0f, 0.0f);
	page->GetEntry("colour", &color);
	LinearMatrix4D matrix(true);
	Point3D position = Point3D::Identity;
	page->GetEntry("Position", &position);
	matrix.BuildTranslation(position);
	Vector3D direction(0.0f, 0.0f, 1.0f);
	page->GetEntry("Direction", &direction);
	matrix.AlignLocalAxisToWorldVector(direction, Z_Axis, Y_Axis, X_Axis);
	SetLightToWorldMatrix(matrix);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRLight::~MLRLight() {}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRLight*
MLRLight::Make(std::iostream stream, uint32_t version)
{
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	int32_t type;
	MLRLight* light = nullptr;
	*stream >> type;
	switch (type)
	{
	case AmbientLight:
		light = new MLRAmbientLight(stream, version);
		break;
	case InfiniteLight:
		light = new MLRInfiniteLight(MLRInfiniteLight::DefaultData, stream, version);
		break;
	case PointLight:
		light = new MLRPointLight(stream, version);
		break;
	case SpotLight:
		light = new MLRSpotLight(stream, version);
		break;
	case LookUpLight:
		light = new MLRLookUpLight(stream, version);
		break;
	default:
		STOP(("Bad light type"));
	}
	gos_PopCurrentHeap();
	return light;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRLight*
MLRLight::Make(Stuff::Page* page)
{
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	const std::wstring_view& type;
	page->GetEntry("LightType", &type, true);
	MLRLight* light = nullptr;
	if (!_stricmp(type, "Ambient"))
		light = new MLRAmbientLight(page);
	else if (!_stricmp(type, "Infinite"))
		light = new MLRInfiniteLight(MLRInfiniteLight::DefaultData, page);
	else if (!_stricmp(type, "Point"))
		light = new MLRPointLight(page);
	else if (!_stricmp(type, "Spot"))
		light = new MLRSpotLight(page);
	else if (!_stricmp(type, "LookUp"))
		light = new MLRLookUpLight(page);
	else
		STOP(("Bad light type"));
	gos_PopCurrentHeap();
	return light;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRLight::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	*stream << static_cast<int32_t>(GetLightType());
	*stream << intensity << color << lightToWorld;
	*stream << lightName;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRLight::Write(Stuff::Page* page)
{
	// Check_Object(this);
	Check_Object(page);
	switch (GetLightType())
	{
	case AmbientLight:
		page->SetEntry("LightType", "Ambient");
		break;
	case InfiniteLight:
		page->SetEntry("LightType", "Infinite");
		break;
	case PointLight:
		page->SetEntry("LightType", "Point");
		break;
	case SpotLight:
		page->SetEntry("LightType", "Spot");
		break;
	case LookUpLight:
		page->SetEntry("LightType", "LookUp");
		break;
	}
	page->SetEntry("Intensity", intensity);
	page->SetEntry("colour", color);
	Point3D position(lightToWorld);
	page->SetEntry("Position", position);
	UnitVector3D direction;
	lightToWorld.GetLocalForwardInWorld(&direction);
	page->SetEntry("Direction", direction);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRLight::TestInstance()
{
	_ASSERT(IsDerivedFrom(DefaultData));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRLight::SetLightToShapeMatrix(const LinearMatrix4D& worldToShape)
{
	// Check_Object(this);
	lightToShape.Multiply(lightToWorld, worldToShape);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRLight::SetLightToWorldMatrix(const LinearMatrix4D& matrix)
{
	// Check_Object(this);
	lightToWorld = matrix;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRLight::Setcolour(float r, float g, float b)
{
	Setcolour(RGBcolour(r, b, b));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRLight::Getcolour(float& r, float& g, float& b)
{
	// Check_Object(this);
	r = color.red;
	g = color.green;
	b = color.blue;
}

} // namespace MidLevelRenderer
