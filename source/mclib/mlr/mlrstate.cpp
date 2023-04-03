//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/mlrstate.h"

namespace MidLevelRenderer
{

//#############################################################################
//###############################    MLRState ##################################
//#############################################################################

int32_t MLRState::systemFlags;
float MLRState::maxUV;

#ifndef OLDFOG
uint32_t MLRState::fogcolour;
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRState::MLRState(std::iostream stream, uint32_t version)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	// Check_Pointer(this);
	Check_Object(stream);
	Load(stream, version);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRState::MLRState()
{
	renderState = renderDeltaMask = 0;
	renderPermissionMask = 0xFFFFFFFF;
	processState = processDeltaMask = 0;
	processPermissionMask = 0xFFFFFFFF;
#ifdef OLDFOG
	fogcolour = 0xffffffff;
	fogDensity = 0.0f;
	nearFog = 1.0f;
	farFog = 100.0f;
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRState::MLRState(const MLRState& mState)
{
	Check_Object(&mState);
	renderState = mState.renderState;
	renderDeltaMask = mState.renderDeltaMask;
	renderPermissionMask = mState.renderPermissionMask;
	processState = mState.processState;
	processDeltaMask = mState.processDeltaMask;
	processPermissionMask = mState.processPermissionMask;
#ifdef OLDFOG
	nearFog = mState.nearFog;
	farFog = mState.farFog;
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRState*
MLRState::Make(std::iostream stream, uint32_t version)
{
	Check_Object(stream);
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLRState* state = new MLRState(stream, version);
	gos_PopCurrentHeap();
	return state;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRState::Save(std::ostream& stream)
{
	// Check_Object(this);
	// Check_Object(stream);
	//
	//-----------------------------
	// Save the renderState to the stream
	//-----------------------------
	//
	stream << renderState << renderDeltaMask << renderPermissionMask;
	stream << processState << processDeltaMask << processPermissionMask;
#if OLDFOG
	stream << nearFog << farFog << fogDensity << fogcolour;
#endif
	if (renderState & TextureMask)
	{
		MLRTexture* texture = (*MLRTexturePool::Instance)[this];
		Check_Object(texture);
		std::wstring_view name = texture->GetTextureName();
		int32_t hint = texture->GetHint();
		hint <<= 4;
		hint |= texture->GetTextureInstance();
		stream << name << hint;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRState::Load(std::istream& stream, uint32_t version)
{
	// Check_Object(this);
	// Check_Object(stream);
	//
	//-------------------------------------
	// Load the renderState from the stream
	//-------------------------------------
	//
	stream >> renderState >> renderDeltaMask >> renderPermissionMask;
	stream >> processState >> processDeltaMask >> processPermissionMask;
#ifdef OLDFOG
	stream >> nearFog >> farFog;
#else
	if (version < 6)
	{
		float dummy;
		stream >> dummy >> dummy;
	}
#endif
	if (version > 3)
	{
#ifdef OLDFOG
		stream >> fogDensity >> fogcolour;
#else
		if (version < 6)
		{
			int32_t dummy1;
			float dummy2;
			stream >> dummy2 >> dummy1;
		}
#endif
	}
	else
	{
#ifdef OLDFOG
		fogDensity = 0.0f;
		fogcolour = 0;
#endif
		if (renderDeltaMask & FogMask)
			renderDeltaMask |= FogMask;
		renderState &= ~(TextureWrapMask | SpecularMask);
		renderDeltaMask &= ~(TextureWrapMask | SpecularMask);
		renderPermissionMask |= TextureWrapMask | SpecularMask;
	}
	if (renderState & TextureMask)
	{
		std::wstring_view name;
		int32_t instance;
		stream >> name;
		stream >> instance;
		Check_Object(MLRTexturePool::Instance);
		MLRTexture* texture = (*MLRTexturePool::Instance)(name, instance);
		if (!texture)
		{
			texture = MLRTexturePool::Instance->Add(name, instance);
		}
		Check_Object(texture);
		texture->SetHint(instance >> 4);
		instance &= 0xf;
		SetTextureHandle(texture->GetTextureHandle());
		_ASSERT((GetTextureHandle() & 7) == 1);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRState&
MLRState::Combine(const MLRState& master, const MLRState& slave)
{
	// Check_Pointer(this);
	Check_Object(&master);
	Check_Object(&slave);
	renderPermissionMask = master.renderPermissionMask & slave.renderPermissionMask;
	renderDeltaMask = slave.renderDeltaMask & master.renderPermissionMask;
	renderState = (master.renderState & ~renderDeltaMask) | (slave.renderState & renderDeltaMask);
	processPermissionMask = master.processPermissionMask & slave.processPermissionMask;
	processDeltaMask = slave.processDeltaMask & master.processPermissionMask;
	processState =
		(master.processState & ~processDeltaMask) | (slave.processState & processDeltaMask);
#ifdef OLDFOG
	uint32_t fog_mode = renderDeltaMask & FogMask;
	if (fog_mode)
	{
		fogDensity = slave.fogDensity;
		nearFog = slave.nearFog;
		farFog = slave.farFog;
		if ((renderState & fog_mode) == OverrideFogAndcolourMode)
		{
			fogcolour = slave.fogcolour;
		}
		else
		{
			nearFog = master.nearFog;
			farFog = master.farFog;
			fogcolour = master.fogcolour;
		}
	}
	else
	{
		fogcolour = master.fogcolour;
		fogDensity = master.fogDensity;
		nearFog = master.nearFog;
		farFog = master.farFog;
	}
#endif
	return *this;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
void
	MLRState::SetFogData (
		uint32_t fog_color,
		float fog_density,
		float near_fog,
		float far_fog
	)
{
	fogcolour = fog_color;
	fogDensity = fog_density;
	nearFog = near_fog;
	farFog = far_fog;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRState::SetFogData (
		RGBAcolour fog_color,
		float fog_density,
		float near_fog,
		float far_fog
	)
{
	SetFogData(GOSCopycolour(&fog_color), fog_density, near_fog, far_fog);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRState::GetFogData (
		RGBAcolour *fog_color,
		float *fog_density,
		float *near_fog,
		float *far_fog
	) const
{
	*fog_color =
		RGBAcolour(
			((fogcolour>>16)&0xFF)/255.0f,
			((fogcolour>>8)&0xFF)/255.0f,
			(fogcolour&0xFF)/255.0f,
			((fogcolour>>24)&0xFF)/255.0f
		);
	*fog_density = fogDensity;
	*near_fog = nearFog;
	*far_fog = farFog;
}
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRState::SetRendererState(MLRTexturePool* texturePool)
{
	// Check_Object(this);
	Check_Object(texturePool);
	if (renderState & MLRState::TextureMask)
		gos_SetRenderState(gos_State_Texture,
			(*texturePool)[renderState & MLRState::TextureMask]->GetImage(nullptr)->GetHandle());
	else
		gos_SetRenderState(gos_State_Texture, 0);
	switch (renderState & MLRState::AlphaMask)
	{
	case MLRState::OneZeroMode:
		gos_SetRenderState(gos_State_AlphaTest, 0);
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
		break;
	case MLRState::OneOneMode:
		gos_SetRenderState(gos_State_AlphaTest, 0);
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneOne);
		break;
	case MLRState::AlphaOneMode:
		SPEW(("micgaert", "MLRState::AlphaOneMode not available anymore"));
		break;
	case MLRState::OneAlphaMode:
		SPEW(("micgaert", "MLRState::OneAlphaMode not available anymore"));
		break;
	case MLRState::AlphaInvAlphaMode:
		gos_SetRenderState(gos_State_AlphaTest, 1);
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
		break;
	case MLRState::OneInvAlphaMode:
		gos_SetRenderState(gos_State_AlphaTest, 0);
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneInvAlpha);
		break;
	default:
		STOP(("Unknown alpha state"));
	}
	switch (renderState & MLRState::FilterMask)
	{
	case MLRState::NoFilterMode:
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
		break;
	case MLRState::BiLinearFilterMode:
		gos_SetRenderState(gos_State_Filter, gos_FilterBiLinear);
		break;
	case MLRState::TriLinearFilterMode:
		gos_SetRenderState(gos_State_Filter, gos_FilterTriLinear);
		break;
	default:
		STOP(("Unknown filter state"));
	}
	gos_SetRenderState(gos_State_Dither, renderState & MLRState::DitherOnMode);
	if (renderState & MLRState::WireFrameOnlyMode)
	{
		uint32_t wfcolour = 0xffffff;
		gos_SetRenderState(gos_State_WireframeMode, (uint32_t)&wfcolour);
	}
	else
	{
		gos_SetRenderState(gos_State_WireframeMode, 0);
	}
	gos_SetRenderState(gos_State_Perspective, renderState & MLRState::TextureCorrectionOnMode);
	if (renderState & MLRState::FogMask)
	{
		gos_SetRenderState(gos_State_Fog, (int32_t)&fogcolour);
	}
	else
	{
		gos_SetRenderState(gos_State_Fog, 0);
	}
	gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
	gos_SetRenderState(gos_State_ZWrite, (renderState & MLRState::ZBufferWriteMask) ? 1 : 0);
	gos_SetRenderState(gos_State_Specular, renderState & MLRState::SpecularOnMode);
	gos_SetRenderState(gos_State_TextureAddress,
		(renderState & MLRState::TextureClamp) ? gos_TextureClamp : gos_TextureWrap);
	gos_SetRenderState(gos_State_ZCompare, (renderState & MLRState::ZBufferCompareMask) ? 1 : 0);
	gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulateAlpha);
}

Stuff::IteratorPosition
GetHashFunctions::GetHashValue(const MLRState& value)
{
	_ASSERT(sizeof(Stuff::IteratorPosition) == sizeof(uint32_t));
	return (((value.processState & MidLevelRenderer::MLRState::UsedProcessMask)
				<< MidLevelRenderer::MLRState::UsedRenderBits)
			   | (value.renderState & MidLevelRenderer::MLRState::UsedRenderMask))
		& 0x7FFFFFFF;
}

} // namespace MidLevelRenderer
