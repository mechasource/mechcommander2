//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "gameos.hpp"
#include "stuff/polar.h"
#include "mlr/mlrlight.h"
#include "mlr/mlrlightmap.h"
#include "mlr/mlreffect.h"
#include "mlr/mlrprimitivebase.h"
#include "mlr/mlr_i_c_pmesh.h"
#include "mlr/mlrclipper.h"

namespace MidLevelRenderer {

//#############################################################################

extern uint32_t gShowBirdView, gEnableDetailTexture, gEnableMultiTexture, gEnableLightMaps;

DrawShapeInformation::DrawShapeInformation()
{
	shape = nullptr;
	state = 0;
	shapeToWorld = nullptr;
	worldToShape = nullptr;
	activeLights = nullptr;
	nrOfActiveLights = 0;
	clippingFlags.SetClippingState(0);
};

DrawScalableShapeInformation::DrawScalableShapeInformation() :
	DrawShapeInformation()
{
	scaling = nullptr;
	paintMe = nullptr;
}

DrawEffectInformation::DrawEffectInformation()
{
	effect = nullptr;
	state = 0;
	effectToWorld = nullptr;
#if 0
	activeLights = nullptr;
	nrOfActiveLights = 0;
#endif
	clippingFlags.SetClippingState(0);
};

DrawScreenQuadsInformation::DrawScreenQuadsInformation()
{
	coords = nullptr;
	colors = nullptr;
	texCoords = nullptr;
	onOrOff = nullptr;
	nrOfQuads = 0;
};

//#############################################################################
//########################### MLRClipper ################################
//#############################################################################

MLRClipper::ClassData* MLRClipper::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRClipper::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(
		MLRClipperClassID, "MidLevelRenderer::MLRClipper", RegisteredClass::DefaultData);
	Register_Object(DefaultData);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRClipper::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRClipper::MLRClipper(AndyDisplay* ad, MLRSorter* s) :
	RegisteredClass(DefaultData), display(ad)
{
	////_ASSERT(gos_GetCurrentHeap() == Heap);
	frameRate = 0;
	usedTime = 0.0f;
	nowTime = 0.0f;
	sorter = s;
	// camMatrix;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRClipper::~MLRClipper()
{
	if (sorter)
	{
		Unregister_Object(sorter);
		delete sorter;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRClipper::StartDraw(const Stuff::LinearMatrix4D& camera_to_world,
	const Stuff::Matrix4D& clip_matrix,
	const Stuff::RGBAColor& fog_color, // NOT USED ANYMORE
	const Stuff::RGBAColor* background_color, const MLRState& default_state, const float* z_value)
{
	// Check_Object(this);
	(void)fog_color;
	//
	// No detail under software rasterizer
	//
	if (Environment.Renderer == 3)
	{
		gEnableDetailTexture = 0;
	}
	MLRState::SetAGPAvailable(TRUE == gos_GetMachineInformation(gos_Info_HasAGPAvailable));
	MLRState::SetMaxUV(
		static_cast<float>(gos_GetMachineInformation(gos_Info_GetMaximumUVSize, 256)));
	if (MLRState::GetHasMaxUVs() && MLRState::GetMaxUV() < 128.0f)
	{
		gEnableLightMaps = 0;
	}
	if (gEnableMultiTexture != 0)
	{
		MLRState::SetMultitextureLightMap(
			TRUE == gos_GetMachineInformation(gos_Info_CanMultitextureLightMap));
		MLRState::SetMultitextureSpecularMap(
			TRUE == gos_GetMachineInformation(gos_Info_CanMultitextureSpecularMap));
	}
	else
	{
		MLRState::SetMultitextureLightMap(false);
		MLRState::SetMultitextureSpecularMap(false);
	}
//
// Make viewport the whole screen
//
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	float z = 1.0f;
	uint32_t back_color = 0;
	bool fill = false;
	bool clear = false;
	if (z_value)
	{
		Check_Pointer(z_value);
		z = *z_value;
		fill = true;
	}
	if (background_color)
	{
		Check_Pointer(background_color);
		back_color = GOSCopyColor(background_color);
		clear = true;
	}
	MLRState::fogColor = back_color;
	/* Already done in MC2
	gos_SetupViewport(
	fill,
	z,
	clear,
	back_color,
	1.0,
	1.0,
	0.0,
	0.0
	);
	*/
	gos_SetRenderState(gos_State_Texture, 0);
	gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
	gos_SetRenderState(gos_State_Clipping, 0);
	gos_SetRenderState(gos_State_ZWrite, 1);
	gos_SetRenderState(gos_State_Filter, gos_FilterTriLinear);
	gos_SetRenderState(gos_State_ZCompare, 1);
	gos_SetRenderState(gos_State_WireframeMode, 0);
	gos_SetRenderState(gos_State_Specular, 0);
	gos_SetRenderState(gos_State_Dither, 0);
	gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
	gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulateAlpha);
	gos_SetRenderState(gos_State_AlphaTest, 0);
	gos_SetRenderState(gos_State_Fog, 0);
	sorter->SetFarClipReciprocal((1.0f - clip_matrix(2, 2)) / clip_matrix(3, 2));
	sorter->StartDraw(default_state);
	cameraToWorldMatrix = camera_to_world;
	worldToCameraMatrix.Invert(cameraToWorldMatrix);
#ifdef LAB_ONLY
	if (gShowBirdView)
	{
		DISABLE_WARNING_PUSH(4640) // construction of local static object is not thread-safe
		static Stuff::YawPitchRange Camera_Direction(-Stuff::Pi_Over_6, 0.0f, 15.0f);
		static Stuff::Vector2DOf<float> Camera_Shift(0.0f, 0.0f);
		static Stuff::LinearMatrix4D birdsEye = Stuff::LinearMatrix4D::Identity;
		DISABLE_WARNING_POP
		//
		//---------------
		// Read the mouse
		//---------------
		//
		int32_t x_delta, y_delta;
		uint32_t buttons;
		gos_GetMouseInfo(nullptr, nullptr, &x_delta, &y_delta, nullptr, &buttons);
		float x_speed = x_delta * 0.01f, y_speed = y_delta * 0.01f;
		//
		//------------------------------------------------------
		// If the left Ctrl button is held down, move the camera
		//------------------------------------------------------
		//
		gosEnum_KeyStatus status = gos_GetKeyStatus(KEY_LCONTROL);
		if (status != KEY_FREE && status != KEY_RELEASED)
		{
			Camera_Direction.pitch += y_speed;
			Camera_Direction.yaw -= x_speed;
		}
		//
		//------------------------------------------------------
		// If the left Shift button is held down, pan the camera
		//------------------------------------------------------
		//
		status = gos_GetKeyStatus(KEY_LSHIFT);
		if (status != KEY_FREE && status != KEY_RELEASED)
		{
			Camera_Shift.x += x_speed;
			Camera_Shift.y += y_speed;
		}
		//
		//----------------------------------------------------------------
		// If the left Alt button is held down, zoom the camera in and out
		//----------------------------------------------------------------
		//
		status = gos_GetKeyStatus(KEY_LMENU);
		if (status != KEY_FREE && status != KEY_RELEASED)
		{
			Camera_Direction.range *= 1.0f + 3.0f * y_speed;
			Camera_Direction.range = Camera_Direction.range > 0.0f ? Camera_Direction.range : 0.0f;
		}
		//
		//----------------------
		// Set the camera matrix
		//----------------------
		//
		birdsEye.BuildRotation(
			Stuff::EulerAngles(Camera_Direction.pitch, Camera_Direction.yaw, 0.0f));
		Stuff::UnitVector3D world_left;
		Stuff::UnitVector3D world_up;
		birdsEye.GetLocalLeftInWorld(&world_left);
		birdsEye.GetLocalUpInWorld(&world_up);
		Stuff::Point3D translation(Camera_Direction);
		translation.AddScaled(translation, world_left, Camera_Shift.x);
		translation.AddScaled(translation, world_up, Camera_Shift.y);
		birdsEye.BuildTranslation(translation);
		Stuff::LinearMatrix4D worldToBird = Stuff::LinearMatrix4D::Identity;
		worldToBird.Multiply(worldToCameraMatrix, birdsEye);
		worldToCameraMatrix = worldToBird;
		// push the far clip out
		float near_clip, far_clip, left_clip, right_clip, top_clip, bottom_clip;
		Stuff::Matrix4D birdToClip;
		clip_matrix.GetPerspective(
			&near_clip, &far_clip, &left_clip, &right_clip, &top_clip, &bottom_clip);
		birdToClip.SetPerspective(near_clip, far_clip + 2 * Camera_Direction.range, left_clip,
			right_clip, top_clip, bottom_clip);
		worldToClipMatrix.Multiply(worldToCameraMatrix, birdToClip);
	}
	else
#endif
		worldToClipMatrix.Multiply(worldToCameraMatrix, clip_matrix);
	cameraPosition = cameraToWorldMatrix;
	// Tell_Value(cameraPosition);
	sorter->Reset();
	allVerticesToDraw.Reset();
#ifdef CalDraw
	ToBeDrawnPrimitive::allVerticesToDraw = &allVerticesToDraw;
#endif
	MLRPrimitiveBase::InitializeDraw();
#ifdef LAB_ONLY
	if (gShowBirdView)
	{
		DrawShapeInformation drawShapeInfo;
		float near_clip, far_clip, left_clip, right_clip, top_clip, bottom_clip;
		Stuff::RGBAColor fruCol(0.0f, 0.5f, 0.0f, 0.5f);
		MLRState fruState;
		clip_matrix.GetPerspective(
			&near_clip, &far_clip, &left_clip, &right_clip, &top_clip, &bottom_clip);
		drawShapeInfo.shape = new MLRShape(1);
		Register_Object(drawShapeInfo.shape);
		fruState.SetTextureHandle(0);
		fruState.SetRenderDeltaMask(MLRState::TextureMask);
#ifdef OLDFOG
		fruState.SetFogMode(MLRState::DisableFogMode);
#else
		fruState.SetFogMode(0);
#endif
		fruState.SetZBufferCompareOn();
		fruState.SetZBufferWriteOn();
		fruState.SetBackFaceOff();
		fruState.SetWireFrameMode(MLRState::WireFrameAddMode);
		fruState.SetFilterMode(MLRState::BiLinearFilterMode);
		fruState.SetAlphaMode(MLRState::AlphaInvAlphaMode);
		fruState.SetPriority(MLRState::PriorityCount - 1);
		MLRPrimitiveBase* primitive = CreateIndexedViewFrustrum_Color_NoLit(
			near_clip, far_clip, left_clip, right_clip, top_clip, bottom_clip, fruCol, &fruState);
		drawShapeInfo.shape->Add(primitive);
		drawShapeInfo.clippingFlags.SetClippingState(0x3f);
		drawShapeInfo.shapeToWorld = &cameraToWorldMatrix;
		drawShapeInfo.worldToShape = &worldToCameraMatrix;
		DrawShape(&drawShapeInfo);
		drawShapeInfo.shape->DetachReference();
		gos_SetRenderState(gos_State_Fog, 0);
	}
#endif
	gos_PopCurrentHeap();
	//
	// End timing function
	//
}

static Stuff::AffineMatrix4D scaledShapeToWorld;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRClipper::DrawShape(DrawShapeInformation* dInfo)
{
// Check_Object(this);
//
// Statistic timing function
//
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLRShape* shape = dInfo->shape;
	MLRPrimitiveBase* primitive = nullptr;
	if (dInfo->nrOfActiveLights > Limits::Max_Number_Of_Lights_Per_Primitive)
	{
		dInfo->nrOfActiveLights = Limits::Max_Number_Of_Lights_Per_Primitive;
	}
	shape->shapeToClipMatrix.Multiply(*dInfo->shapeToWorld, worldToClipMatrix);
	shape->worldToShape = dInfo->worldToShape;
	shape->InitializePrimitives(true, dInfo->state);
	size_t i;
	uint32_t j;
	Stuff::Point3D sp;
	int32_t nrOfLightMaps = 0;
	sp.Multiply(cameraPosition, *shape->worldToShape);
	for (i = 0; i < dInfo->nrOfActiveLights; i++)
	{
		dInfo->activeLights[i]->SetLightToShapeMatrix(*shape->worldToShape);
		nrOfLightMaps += (dInfo->activeLights[i]->GetLightMap()) ? 1 : 0;
	}
	if (!gEnableLightMaps)
	{
		nrOfLightMaps = 0;
	}
	gos_GetViewport(&ViewportScalars::MulX, &ViewportScalars::MulY, &ViewportScalars::AddX,
		&ViewportScalars::AddY);
#ifdef LAB_ONLY
	if (gShowBirdView)
	{
		dInfo->clippingFlags = 0x3f;
#ifdef OLDFOG
		dInfo->state.SetFogMode(MLRState::DisableFogMode);
#else
		dInfo->state.SetFogMode(0);
#endif
		dInfo->state.SetRenderPermissionMask(
			dInfo->state.GetRenderPermissionMask() & ~MLRState::FogMask);
	}
#endif
	for (i = 0; i < shape->numPrimitives; i++)
	{
		primitive = shape->allPrimitives[i];
		Check_Object(primitive);
		if (primitive->GetCurrentState().GetDrawNowMode() == MLRState::DrawNowOffMode)
		{
			ToBeDrawnPrimitive* tbdp = sorter->GetCurrentTBDP();
			Check_Pointer(tbdp);
			tbdp->primitive = primitive;
			tbdp->state = primitive->GetCurrentState();
			tbdp->cameraPosition = sp;
			tbdp->clippingFlags = dInfo->clippingFlags;
			Check_Object(&tbdp->shapeToClipMatrix);
			tbdp->shapeToClipMatrix = shape->shapeToClipMatrix;
			tbdp->worldToShape = *shape->worldToShape;
			_ASSERT(dInfo->nrOfActiveLights <= Limits::Max_Number_Of_Lights_Per_Primitive);
			tbdp->nrOfActiveLights = dInfo->nrOfActiveLights;
			for (j = 0; j < tbdp->nrOfActiveLights; j++)
			{
				Check_Object(dInfo->activeLights[j]);
				tbdp->activeLights[j] = dInfo->activeLights[j];
			}
			sorter->IncreaseTBDPCounter();
		}
		else
		{
			if (nrOfLightMaps)
			{
				MLRLightMap::SetDrawData(&allVerticesToDraw, &shape->shapeToClipMatrix,
					dInfo->clippingFlags, dInfo->state);
			}
			if (primitive->FindBackFace(sp))
			{
				primitive->Lighting(dInfo->activeLights, dInfo->nrOfActiveLights);
				if (dInfo->clippingFlags.GetClippingState() != 0)
				{
					if (primitive->TransformAndClip(
							&shape->shapeToClipMatrix, dInfo->clippingFlags, &allVerticesToDraw))
					{
						if (primitive->GetVisible())
						{
							for (j = 0; j < primitive->GetNumPasses(); j++)
							{
								sorter->DrawPrimitive(primitive, j);
							}
						}
					}
				}
				else
				{
					primitive->TransformNoClip(&shape->shapeToClipMatrix, &allVerticesToDraw);
					for (j = 0; j < primitive->GetNumPasses(); j++)
					{
						sorter->DrawPrimitive(primitive, j);
					}
				}
#ifdef LAB_ONLY
				Set_Statistic(Number_Of_Primitives, Number_Of_Primitives + 1);
				if (primitive->IsDerivedFrom(MLRIndexedPrimitiveBase::DefaultData))
				{
					Stuff::Point3D* coords;
					puint16_t indices;
					size_t nr;
					(Cast_Pointer(MLRIndexedPrimitiveBase*, primitive))
						->GetIndexData(&indices, &nr);
					NumAllIndices += (uint32_t)nr;
					primitive->GetCoordData(&coords, &nr);
					NumAllVertices += (uint32_t)nr;
					Set_Statistic(
						Index_Over_Vertex_Ratio, (float)NumAllIndices / (float)NumAllVertices);
				}
#endif
				if (nrOfLightMaps)
				{
					MLRLightMap::DrawLightMaps(sorter);
				}
			}
		}
	}
	//
	// End timing function
	//
	gos_PopCurrentHeap();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRClipper::DrawScalableShape(DrawScalableShapeInformation* dInfo)
{
	// Check_Object(this);
	//
	// Statistic timing function
	//
	MLRShape* shape = dInfo->shape;
	MLRPrimitiveBase* primitive = nullptr;
	size_t i;
	shape->shapeToClipMatrix.Multiply(*dInfo->shapeToWorld, worldToClipMatrix);
	shape->worldToShape = dInfo->worldToShape;
	shape->InitializePrimitives(true, dInfo->state, 1);
	if (dInfo->scaling != nullptr)
	{
		Stuff::LinearMatrix4D scale = Stuff::LinearMatrix4D::Identity;
		scale(0, 0) = dInfo->scaling->x;
		scale(1, 1) = dInfo->scaling->y;
		scale(2, 2) = dInfo->scaling->z;
		scaledShapeToWorld.Multiply(scale, *dInfo->shapeToWorld);
		shape->shapeToClipMatrix.Multiply(scaledShapeToWorld, worldToClipMatrix);
	}
	else
	{
		shape->shapeToClipMatrix.Multiply(*dInfo->shapeToWorld, worldToClipMatrix);
	}
	shape->worldToShape = nullptr;
	gos_GetViewport(&ViewportScalars::MulX, &ViewportScalars::MulY, &ViewportScalars::AddX,
		&ViewportScalars::AddY);
#ifdef LAB_ONLY
	if (gShowBirdView)
	{
		dInfo->clippingFlags = 0x3f;
	}
#endif
	for (i = 0; i < shape->numPrimitives; i++)
	{
		primitive = shape->allPrimitives[i];
		Check_Object(primitive);
		if (dInfo->paintMe)
		{
			primitive->PaintMe(dInfo->paintMe);
		}
		if (dInfo->clippingFlags.GetClippingState() != 0)
		{
			if (primitive->TransformAndClip(
					&shape->shapeToClipMatrix, dInfo->clippingFlags, &allVerticesToDraw))
			{
				if (primitive->GetVisible())
				{
					if (primitive->GetCurrentState().GetDrawNowMode() == MLRState::DrawNowOnMode)
					{
						sorter->DrawPrimitive(primitive);
					}
					else
					{
						sorter->AddPrimitive(primitive);
					}
#ifdef LAB_ONLY
					Set_Statistic(Number_Of_Primitives, Number_Of_Primitives + 1);
#endif
				}
			}
		}
		else
		{
			primitive->TransformNoClip(&shape->shapeToClipMatrix, &allVerticesToDraw);
			if (primitive->GetCurrentState().GetDrawNowMode() == MLRState::DrawNowOnMode)
			{
				sorter->DrawPrimitive(primitive);
			}
			else
			{
				sorter->AddPrimitive(primitive);
			}
#ifdef LAB_ONLY
			Set_Statistic(Number_Of_Primitives, Number_Of_Primitives + 1);
#endif
		}
	}
	//
	// End timing function
	//
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRClipper::DrawEffect(DrawEffectInformation* dInfo)
{
	//
	// Statistic timing function
	//
	// Check_Object(this);
	Check_Object(dInfo);
	Check_Object(dInfo->effect);
#ifdef LAB_ONLY
	if (gShowBirdView)
	{
		dInfo->clippingFlags = 0x3f;
	}
#endif
	dInfo->effect->SetEffectToClipMatrix(dInfo->effectToWorld, &worldToClipMatrix);
	gos_GetViewport(&ViewportScalars::MulX, &ViewportScalars::MulY, &ViewportScalars::AddX,
		&ViewportScalars::AddY);
	dInfo->effect->Draw(dInfo, &allVerticesToDraw, sorter);
	//
	// End timing function
	//
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRClipper::DrawScreenQuads(DrawScreenQuadsInformation* dInfo)
{
	//
	// Statistic timing function
	//
	// Check_Object(this);
	Check_Object(dInfo);
	gos_GetViewport(&ViewportScalars::MulX, &ViewportScalars::MulY, &ViewportScalars::AddX,
		&ViewportScalars::AddY);
	GOSVertex* vertices = allVerticesToDraw.GetActualVertexPool();
	size_t i, j;
	dInfo->currentNrOfQuads = 0;
	for (i = 0, j = 0; i < dInfo->nrOfQuads; i++)
	{
		if (dInfo->onOrOff[i] == true)
		{
			dInfo->currentNrOfQuads += 4;
			for (; j < dInfo->currentNrOfQuads; j++)
			{
				size_t offset = (i << 2) + (j & 3);
				_ASSERT(dInfo->coords[offset].x >= 0.0f && dInfo->coords[offset].x <= dInfo->coords[offset].w);
				_ASSERT(dInfo->coords[offset].y >= 0.0f && dInfo->coords[offset].y <= dInfo->coords[offset].w);
				_ASSERT(dInfo->coords[offset].z >= 0.0f && dInfo->coords[offset].z <= dInfo->coords[offset].w);
				vertices[j].x = (1.0f - dInfo->coords[offset].x) * ViewportScalars::MulX + ViewportScalars::AddX;
				vertices[j].y = (1.0f - dInfo->coords[offset].y) * ViewportScalars::MulY + ViewportScalars::AddY;
				vertices[j].z = dInfo->coords[offset].z;
				vertices[j].rhw = dInfo->coords[offset].w;
				vertices[j].argb = GOSCopyColor(dInfo->colors + offset);
				vertices[j].u = dInfo->texCoords[offset][0];
				vertices[j].v = dInfo->texCoords[offset][1];
			}
		}
	}
	if (j > 0)
	{
		allVerticesToDraw.Increase((uint32_t)j);
		sorter->AddScreenQuads(vertices, dInfo);
	}
	//
	// End timing function
	//
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRClipper::Clear(uint32_t /*flags*/)
{
	// Check_Object(this);
}

} // namespace MidLevelRenderer
