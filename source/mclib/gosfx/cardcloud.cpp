//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
#include "gosfx/gosfxheaders.h"
#include "mlr/mlrcardcloud.h"

//------------------------------------------------------------------------------
//
gosFX::CardCloud__Specification::CardCloud__Specification(
	std::iostream stream, uint32_t gfx_version)
	: SpinningCloud__Specification(gosFX::CardCloudClassID, stream, gfx_version)
{
	// Check_Pointer(this);
	Check_Object(stream);
	_ASSERT(m_class == CardCloudClassID);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	m_halfheight.Load(stream, gfx_version);
	m_aspectRatio.Load(stream, gfx_version);
	//
	//-------------------------------------------------------------------
	// If we are reading an old version of the card cloud, ignore all the
	// animation on the UV channels
	//-------------------------------------------------------------------
	//
	if (gfx_version < 10)
	{
		m_pIndex.m_ageCurve.SetCurve(0.0f);
		m_pIndex.m_seedCurve.SetCurve(1.0f);
		m_pIndex.m_seeded = false;
		SeededCurveOf<ComplexCurve, LinearCurve, Curve::e_ComplexLinearType> temp;
		temp.Load(stream, gfx_version);
		float v = temp.ComputeValue(0.0f, 0.0f);
		m_UOffset.SetCurve(v);
		temp.Load(stream, gfx_version);
		v = temp.ComputeValue(0.0f, 0.0f);
		m_VOffset.SetCurve(v);
		m_USize.Load(stream, gfx_version);
		m_VSize.Load(stream, gfx_version);
		m_animated = false;
	}
	//
	//------------------------------
	// Otherwise, read in the curves
	//------------------------------
	//
	else
	{
		m_pIndex.Load(stream, gfx_version);
		m_UOffset.Load(stream, gfx_version);
		m_VOffset.Load(stream, gfx_version);
		m_USize.Load(stream, gfx_version);
		m_VSize.Load(stream, gfx_version);
		*stream >> m_animated;
	}
	Setwidth();
	m_totalParticleSize = gosFX::CardCloud::ParticleSize;
	m_particleClassSize = sizeof(gosFX::CardCloud::Particle);
}

//------------------------------------------------------------------------------
//
gosFX::CardCloud__Specification::CardCloud__Specification()
	: SpinningCloud__Specification(gosFX::CardCloudClassID)
{
	// Check_Pointer(this);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	m_animated = false;
	m_width = 1;
	m_totalParticleSize = gosFX::CardCloud::ParticleSize;
	m_particleClassSize = sizeof(gosFX::CardCloud::Particle);
}

//------------------------------------------------------------------------------
//
gosFX::CardCloud__Specification*
gosFX::CardCloud__Specification::Make(
	std::iostream stream, uint32_t gfx_version)
{
	Check_Object(stream);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	CardCloud__Specification* spec = new gosFX::CardCloud__Specification(stream, gfx_version);
	// gos_PopCurrentHeap();
	return spec;
}

//------------------------------------------------------------------------------
//
void gosFX::CardCloud__Specification::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	SpinningCloud__Specification::Save(stream);
	m_halfheight.Save(stream);
	m_aspectRatio.Save(stream);
	m_pIndex.Save(stream);
	m_UOffset.Save(stream);
	m_VOffset.Save(stream);
	m_USize.Save(stream);
	m_VSize.Save(stream);
	*stream << m_animated;
}
//------------------------------------------------------------------------------
//

void gosFX::CardCloud__Specification::BuildDefaults()
{
	// Check_Object(this);
	SpinningCloud__Specification::BuildDefaults();
	m_halfheight.m_ageCurve.SetCurve(1.0f);
	m_halfheight.m_seeded = false;
	m_halfheight.m_seedCurve.SetCurve(1.0f);
	m_aspectRatio.m_ageCurve.SetCurve(1.0f);
	m_aspectRatio.m_seeded = false;
	m_aspectRatio.m_seedCurve.SetCurve(1.0f);
	m_pIndex.m_ageCurve.SetCurve(0.0f);
	m_pIndex.m_seeded = false;
	m_pIndex.m_seedCurve.SetCurve(1.0f);
	m_UOffset.SetCurve(0.0f);
	m_VOffset.SetCurve(0.0f);
	m_USize.SetCurve(1.0f);
	m_VSize.SetCurve(1.0f);
	m_animated = false;
	m_width = 1;
}

//------------------------------------------------------------------------------
//

bool gosFX::CardCloud__Specification::IsDataValid(bool fix_data)
{
	// Check_Object(this);
	float max_offset, min_offset;
	float max_scale, min_scale;
	m_USize.ExpensiveComputeRange(&min_scale, &max_scale);
	float lower = min_scale;
	if (lower > 0.0f)
		lower = 0.0f;
	float upper = max_scale;
	//
	//------------------------------------
	// Calculate the worst case UV offsets
	//------------------------------------
	//
	m_VOffset.ExpensiveComputeRange(&min_offset, &max_offset);
	lower += min_offset;
	upper += max_offset;
	if (upper > 99.0f || lower < -99.0f)
	{
		if (fix_data)
		{
			m_VOffset.SetCurve(0.0f);
			// PAUSE(("Warning: Curve \"VOffset\" in Effect \"%s\" Is Out of
			// Range and has been Reset", (std::wstring_view)m_name));
		}
		else
			return false;
	}
	m_VSize.ExpensiveComputeRange(&min_scale, &max_scale);
	lower = min_scale;
	if (lower > 0.0f)
		lower = 0.0f;
	upper = max_scale;
	//
	//------------------------------------
	// Calculate the worst case UV offsets
	//------------------------------------
	//
	max_offset, min_offset;
	m_UOffset.ExpensiveComputeRange(&min_offset, &max_offset);
	lower += min_offset;
	upper += max_offset;
	if (upper > 99.0f || lower < -99.0f)
	{
		if (fix_data)
		{
			m_UOffset.SetCurve(0.0f);
			// PAUSE(("Warning: Curve \"UOffset\" in Effect \"%s\" Is Out of
			// Range and has been Reset", (std::wstring_view)m_name));
		}
		else
			return false;
	}
	return SpinningCloud__Specification::IsDataValid(fix_data);
}

//------------------------------------------------------------------------------
//
void gosFX::CardCloud__Specification::Copy(CardCloud__Specification* spec)
{
	// Check_Object(this);
	Check_Object(spec);
	SpinningCloud__Specification::Copy(spec);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	m_halfheight = spec->m_halfheight;
	m_aspectRatio = spec->m_aspectRatio;
	m_pIndex = spec->m_pIndex;
	m_UOffset = spec->m_UOffset;
	m_VOffset = spec->m_VOffset;
	m_USize = spec->m_USize;
	m_VSize = spec->m_VSize;
	m_animated = spec->m_animated;
	m_width = spec->m_width;
	// gos_PopCurrentHeap();
}

//------------------------------------------------------------------------------
//
void gosFX::CardCloud__Specification::Setwidth()
{
	m_width = static_cast<uint8_t>(1.0f / m_USize.ComputeValue(0.0f, 0.0f));
}

//############################################################################
//##############################  gosFX::CardCloud
//################################
//############################################################################

gosFX::CardCloud::ClassData* gosFX::CardCloud::DefaultData = nullptr;

//------------------------------------------------------------------------------
//
void gosFX::CardCloud::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	DefaultData = new ClassData(CardCloudClassID, "gosFX::CardCloud", SpinningCloud::DefaultData,
		(Effect::Factory)&Make, (Specification::Factory)&Specification::Make);
	Register_Object(DefaultData);
}

//------------------------------------------------------------------------------
//
void gosFX::CardCloud::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//------------------------------------------------------------------------------
//
gosFX::CardCloud::CardCloud(Specification* spec, uint32_t flags)
	: SpinningCloud(DefaultData, spec, flags)
{
	Check_Object(spec);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	// gos_PushCurrentHeap(MidLevelRenderer::Heap);
	m_cloudImplementation = new MidLevelRenderer::MLRCardCloud(spec->m_maxParticleCount);
	Register_Object(m_cloudImplementation);
	// gos_PopCurrentHeap();
	uint32_t index = spec->m_maxParticleCount * sizeof(Particle);
	m_P_vertices = Cast_Pointer(Stuff::Point3D*, &m_data[index]);
	index += 4 * spec->m_maxParticleCount * sizeof(Stuff::Point3D);
	m_P_color = Cast_Pointer(Stuff::RGBAcolour*, &m_data[index]);
	index += spec->m_maxParticleCount * sizeof(Stuff::RGBAcolour);
	m_P_uvs = Cast_Pointer(Stuff::Vector2DOf<float>*, &m_data[index]);
	m_cloudImplementation->SetData(
		Cast_Pointer(const size_t*, &m_activeParticleCount), m_P_vertices, m_P_color, m_P_uvs);
}

//------------------------------------------------------------------------------
//
gosFX::CardCloud::~CardCloud()
{
	Unregister_Object(m_cloudImplementation);
	delete m_cloudImplementation;
}

//------------------------------------------------------------------------------
//
gosFX::CardCloud*
gosFX::CardCloud::Make(Specification* spec, uint32_t flags)
{
	Check_Object(spec);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	CardCloud* cloud = new gosFX::CardCloud(spec, flags);
	// gos_PopCurrentHeap();
	return cloud;
}

//------------------------------------------------------------------------------
//
void gosFX::CardCloud::CreateNewParticle(uint32_t index, Stuff::Point3D* translation)
{
	// Check_Object(this);
	//
	//-------------------------------------------------------------------
	// Let our parent do creation, then turn on the particle in the cloud
	//-------------------------------------------------------------------
	//
	SpinningCloud::CreateNewParticle(index, translation);
	m_cloudImplementation->TurnOn(index);
	_ASSERT(m_cloudImplementation->IsOn(index));
	//
	//-----------------------------
	// Figure out the particle size
	//-----------------------------
	//
	Specification* spec = GetSpecification();
	Check_Object(spec);
	Particle* particle = GetParticle(index);
	Check_Object(particle);
	particle->m_halfY = spec->m_halfheight.ComputeValue(m_age, particle->m_seed);
	particle->m_halfX =
		particle->m_halfY * spec->m_aspectRatio.ComputeValue(m_age, particle->m_seed);
	particle->m_radius =
		Stuff::Sqrt(particle->m_halfX * particle->m_halfX + particle->m_halfY * particle->m_halfY);
}

//------------------------------------------------------------------------------
//
bool gosFX::CardCloud::AnimateParticle(
	uint32_t index, const Stuff::LinearMatrix4D* world_to_new_local, Stuff::Time till)
{
	// Check_Object(this);
	//
	//-----------------------------------------
	// Animate the parent then get our pointers
	//-----------------------------------------
	//
	if (!SpinningCloud::AnimateParticle(index, world_to_new_local, till))
		return false;
	Set_Statistic(Card_Count, Card_Count + 1);
	Specification* spec = GetSpecification();
	Check_Object(spec);
	Particle* particle = GetParticle(index);
	Check_Object(particle);
	float seed = particle->m_seed;
	float age = particle->m_age;
	//
	//------------------
	// Animate the color
	//------------------
	//
	Check_Pointer(m_P_color);
	m_P_color[index].red = spec->m_pRed.ComputeValue(age, seed);
	m_P_color[index].green = spec->m_pGreen.ComputeValue(age, seed);
	m_P_color[index].blue = spec->m_pBlue.ComputeValue(age, seed);
	m_P_color[index].alpha = spec->m_pAlpha.ComputeValue(age, seed);
	//
	//----------------
	// Animate the uvs
	//----------------
	//
	float u = spec->m_UOffset.ComputeValue(age, seed);
	float v = spec->m_VOffset.ComputeValue(age, seed);
	float u2 = spec->m_USize.ComputeValue(age, seed);
	float v2 = spec->m_VSize.ComputeValue(age, seed);
	//
	//--------------------------------------------------------------
	// If we are animated, figure out the row/column to be displayed
	//--------------------------------------------------------------
	//
	if (spec->m_animated)
	{
		uint8_t columns = Stuff::Truncate_Float_To_Byte(spec->m_pIndex.ComputeValue(age, seed));
		uint8_t rows = static_cast<uint8_t>(columns / spec->m_width);
		columns = static_cast<uint8_t>(columns - rows * spec->m_width);
		//
		//---------------------------
		// Now compute the end points
		//---------------------------
		//
		u += u2 * columns;
		v += v2 * rows;
	}
	u2 += u;
	v2 += v;
	index *= 4;
	m_P_uvs[index].x = u;
	m_P_uvs[index].y = v2;
	m_P_uvs[++index].x = u2;
	m_P_uvs[index].y = v2;
	m_P_uvs[++index].x = u2;
	m_P_uvs[index].y = v;
	m_P_uvs[++index].x = u;
	m_P_uvs[index].y = v;
	return true;
}

//------------------------------------------------------------------------------
//
void gosFX::CardCloud::DestroyParticle(uint32_t index)
{
	// Check_Object(this);
	m_cloudImplementation->TurnOff(index);
	_ASSERT(!m_cloudImplementation->IsOn(index));
	SpinningCloud::DestroyParticle(index);
}

//------------------------------------------------------------------------------
//
void gosFX::CardCloud::Draw(DrawInfo* info)
{
	// Check_Object(this);
	Check_Object(info);
	//
	//---------------------------------------------------------
	// If we have active particles, set up the draw information
	//---------------------------------------------------------
	//
	if (m_activeParticleCount)
	{
		MidLevelRenderer::DrawEffectInformation dInfo;
		dInfo.effect = m_cloudImplementation;
		Specification* spec = GetSpecification();
		Check_Object(spec);
		dInfo.state.Combine(info->m_state, spec->m_state);
		dInfo.clippingFlags = info->m_clippingFlags;
		Stuff::LinearMatrix4D local_to_world;
		local_to_world.Multiply(m_localToParent, *info->m_parentToWorld);
		dInfo.effectToWorld = &local_to_world;
		//
		//--------------------------------------------------------------
		// Check the orientation mode.  The first case is XY orientation
		//--------------------------------------------------------------
		//
		uint32_t i;
		uint32_t vert = 0;
		if (spec->m_alignZUsingX)
		{
			if (spec->m_alignZUsingY)
			{
				//
				//-----------------------------------------
				// Get the camera location into local space
				//-----------------------------------------
				//
				Stuff::Point3D camera_in_world(info->m_clipper->GetCameraToWorldMatrix());
				Stuff::Point3D camera_in_cloud;
				camera_in_cloud.MultiplyByInverse(camera_in_world, local_to_world);
				//
				//--------------------------------------
				// Spin through all the active particles
				//--------------------------------------
				//
				for (i = 0; i < m_activeParticleCount; i++)
				{
					Particle* particle = GetParticle(i);
					Check_Object(particle);
					if (particle->m_age < 1.0f)
					{
						//
						//--------------------------------
						// Build the local to cloud matrix
						//--------------------------------
						//
						Stuff::Vector3D direction_in_cloud;
						direction_in_cloud.Subtract(camera_in_cloud, particle->m_localTranslation);
						Stuff::LinearMatrix4D card_to_cloud;
						card_to_cloud.BuildRotation(particle->m_localRotation);
						card_to_cloud.AlignLocalAxisToWorldVector(
							direction_in_cloud, Stuff::Z_Axis, Stuff::Y_Axis, Stuff::X_Axis);
						card_to_cloud.BuildTranslation(particle->m_localTranslation);
						//
						//-------------------------------------------------
						// Figure out the scale, then build the four points
						//-------------------------------------------------
						//
						float scale = particle->m_scale;
						m_P_vertices[vert++].Multiply(Stuff::Point3D(scale * particle->m_halfX,
														  -scale * particle->m_halfY, 0.0f),
							card_to_cloud);
						m_P_vertices[vert++].Multiply(Stuff::Point3D(-scale * particle->m_halfX,
														  -scale * particle->m_halfY, 0.0f),
							card_to_cloud);
						m_P_vertices[vert++].Multiply(Stuff::Point3D(-scale * particle->m_halfX,
														  scale * particle->m_halfY, 0.0f),
							card_to_cloud);
						m_P_vertices[vert++].Multiply(Stuff::Point3D(scale * particle->m_halfX,
														  scale * particle->m_halfY, 0.0f),
							card_to_cloud);
					}
					else
						vert += 4;
				}
			}
			//
			//-----------------------
			// handle X-only rotation
			//-----------------------
			//
			else
			{
				//
				//-----------------------------------------
				// Get the camera location into local space
				//-----------------------------------------
				//
				Stuff::Point3D camera_in_world(info->m_clipper->GetCameraToWorldMatrix());
				Stuff::Point3D camera_in_cloud;
				camera_in_cloud.MultiplyByInverse(camera_in_world, local_to_world);
				//
				//--------------------------------------
				// Spin through all the active particles
				//--------------------------------------
				//
				for (i = 0; i < m_activeParticleCount; i++)
				{
					Particle* particle = GetParticle(i);
					Check_Object(particle);
					if (particle->m_age < 1.0f)
					{
						//
						//--------------------------------
						// Build the local to cloud matrix
						//--------------------------------
						//
						Stuff::Vector3D direction_in_cloud;
						direction_in_cloud.Subtract(camera_in_cloud, particle->m_localTranslation);
						Stuff::LinearMatrix4D card_to_cloud;
						card_to_cloud.BuildRotation(particle->m_localRotation);
						card_to_cloud.AlignLocalAxisToWorldVector(
							direction_in_cloud, Stuff::Z_Axis, Stuff::X_Axis, -1);
						card_to_cloud.BuildTranslation(particle->m_localTranslation);
						//
						//-------------------------------------------------
						// Figure out the scale, then build the four points
						//-------------------------------------------------
						//
						float scale = particle->m_scale;
						m_P_vertices[vert++].Multiply(Stuff::Point3D(scale * particle->m_halfX,
														  -scale * particle->m_halfY, 0.0f),
							card_to_cloud);
						m_P_vertices[vert++].Multiply(Stuff::Point3D(-scale * particle->m_halfX,
														  -scale * particle->m_halfY, 0.0f),
							card_to_cloud);
						m_P_vertices[vert++].Multiply(Stuff::Point3D(-scale * particle->m_halfX,
														  scale * particle->m_halfY, 0.0f),
							card_to_cloud);
						m_P_vertices[vert++].Multiply(Stuff::Point3D(scale * particle->m_halfX,
														  scale * particle->m_halfY, 0.0f),
							card_to_cloud);
					}
					else
						vert += 4;
				}
			}
		}
		//
		//-------------------------------------------------------
		// Each matrix needs to be aligned to the camera around Y
		//-------------------------------------------------------
		//
		else if (spec->m_alignZUsingY)
		{
			//
			//-----------------------------------------
			// Get the camera location into local space
			//-----------------------------------------
			//
			Stuff::Point3D camera_in_world(info->m_clipper->GetCameraToWorldMatrix());
			Stuff::Point3D camera_in_cloud;
			camera_in_cloud.MultiplyByInverse(camera_in_world, local_to_world);
			//
			//--------------------------------------
			// Spin through all the active particles
			//--------------------------------------
			//
			for (i = 0; i < m_activeParticleCount; i++)
			{
				Particle* particle = GetParticle(i);
				Check_Object(particle);
				if (particle->m_age < 1.0f)
				{
					//
					//--------------------------------
					// Build the local to cloud matrix
					//--------------------------------
					//
					Stuff::Vector3D direction_in_cloud;
					direction_in_cloud.Subtract(camera_in_cloud, particle->m_localTranslation);
					Stuff::LinearMatrix4D card_to_cloud;
					card_to_cloud.BuildRotation(particle->m_localRotation);
					card_to_cloud.AlignLocalAxisToWorldVector(
						direction_in_cloud, Stuff::Z_Axis, Stuff::Y_Axis, -1);
					card_to_cloud.BuildTranslation(particle->m_localTranslation);
					//
					//-------------------------------------------------
					// Figure out the scale, then build the four points
					//-------------------------------------------------
					//
					float scale = particle->m_scale;
					m_P_vertices[vert++].Multiply(
						Stuff::Point3D(scale * particle->m_halfX, -scale * particle->m_halfY, 0.0f),
						card_to_cloud);
					m_P_vertices[vert++].Multiply(Stuff::Point3D(-scale * particle->m_halfX,
													  -scale * particle->m_halfY, 0.0f),
						card_to_cloud);
					m_P_vertices[vert++].Multiply(
						Stuff::Point3D(-scale * particle->m_halfX, scale * particle->m_halfY, 0.0f),
						card_to_cloud);
					m_P_vertices[vert++].Multiply(
						Stuff::Point3D(scale * particle->m_halfX, scale * particle->m_halfY, 0.0f),
						card_to_cloud);
				}
				else
					vert += 4;
			}
		}
		//
		//---------------------------------------------------------------
		// No alignment is necessary, so just multiply out all the active
		// particles
		//---------------------------------------------------------------
		//
		else
		{
			for (i = 0; i < m_activeParticleCount; i++)
			{
				Particle* particle = GetParticle(i);
				Check_Object(particle);
				if (particle->m_age < 1.0f)
				{
					//
					//--------------------------------
					// Build the local to cloud matrix
					//--------------------------------
					//
					Stuff::LinearMatrix4D card_to_cloud;
					card_to_cloud.BuildRotation(particle->m_localRotation);
					card_to_cloud.BuildTranslation(particle->m_localTranslation);
					//
					//-------------------------------------------------
					// Figure out the scale, then build the four points
					//-------------------------------------------------
					//
					float scale = particle->m_scale;
					m_P_vertices[vert++].Multiply(
						Stuff::Point3D(scale * particle->m_halfX, -scale * particle->m_halfY, 0.0f),
						card_to_cloud);
					m_P_vertices[vert++].Multiply(Stuff::Point3D(-scale * particle->m_halfX,
													  -scale * particle->m_halfY, 0.0f),
						card_to_cloud);
					m_P_vertices[vert++].Multiply(
						Stuff::Point3D(-scale * particle->m_halfX, scale * particle->m_halfY, 0.0f),
						card_to_cloud);
					m_P_vertices[vert++].Multiply(
						Stuff::Point3D(scale * particle->m_halfX, scale * particle->m_halfY, 0.0f),
						card_to_cloud);
				}
				else
					vert += 4;
			}
		}
		//
		//---------------------
		// Now just do the draw
		//---------------------
		//
		info->m_clipper->DrawEffect(&dInfo);
	}
	SpinningCloud::Draw(info);
}

//------------------------------------------------------------------------------
//
void gosFX::CardCloud::TestInstance(void) const
{
	_ASSERT(IsDerivedFrom(DefaultData));
}
