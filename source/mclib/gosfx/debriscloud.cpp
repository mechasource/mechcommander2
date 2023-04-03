//==========================================================================//
// File:	 gosFX_DebrisCloud.cpp											//
// Contents: Base gosFX::DebrisCloud Component								//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//
//############################################################################
//########################  gosFX::DebrisCloud__Specification
//#############################
//############################################################################

#include "stdinc.h"
#include "gosfx/gosfxheaders.h"

//------------------------------------------------------------------------------
//
gosFX::DebrisCloud__Specification::DebrisCloud__Specification(
	std::iostream stream, uint32_t gfx_version)
	: Effect__Specification(gosFX::DebrisCloudClassID, stream, gfx_version)
{
	// Check_Pointer(this);
	_ASSERT(m_class == DebrisCloudClassID);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	m_minimumDeviation.Load(stream, gfx_version); //	ConstantCurve
	m_maximumDeviation.Load(stream, gfx_version); //	SplineCurve
	m_startingSpeed.Load(stream, gfx_version); //	SeededCurveOf<ComplexCurve,
		// ComplexCurve,Curve::e_ComplexComplexType>
	m_pLifeSpan.Load(stream, gfx_version); //	SeededCurveOf<ComplexCurve,
		// SplineCurve,Curve::e_ComplexSplineType>
	m_pEtherVelocityY.Load(stream, gfx_version); //	SeededCurveOf<ConstantCurve,
		// LinearCurve,Curve::e_ConstantLinearType>
	m_pAccelerationY.Load(stream, gfx_version); //	SeededCurveOf<SplineCurve,
		// LinearCurve,Curve::e_SplineLinearType>
	m_pDrag.Load(stream, gfx_version); //	SeededCurveOf<ComplexCurve,
		// ComplexCurve,Curve::e_ComplexComplexType>
	m_pAlpha.Load(stream, gfx_version); //	SeededCurveOf<ComplexCurve,
		// LinearCurve,Curve::e_ComplexLinearType>
	m_pSpin.Load(stream, gfx_version); //	SeededCurveOf<ConstantCurve,
		// LinearCurve,Curve::e_ConstantLinearType>
	*stream >> centerOfForce; //	Stuff::Point3D
	LoadGeometry(stream);
}

//------------------------------------------------------------------------------
//
gosFX::DebrisCloud__Specification::DebrisCloud__Specification()
	: Effect__Specification(gosFX::DebrisCloudClassID)
{
	// Check_Pointer(this);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	debrisPieces.SetLength(0);
	debrisPositions.SetLength(0);
	debrisSpheres.SetLength(0);
	debrisSeed.SetLength(0);
}

//------------------------------------------------------------------------------
//
gosFX::DebrisCloud__Specification::~DebrisCloud__Specification()
{
	// Check_Object(this);
	int32_t i, nrOfParticles = debrisPieces.GetLength();
	for (i = 0; i < nrOfParticles; i++)
	{
		Check_Object(debrisPieces[i]);
		debrisPieces[i]->DetachReference();
	}
	debrisPieces.SetLength(0);
	debrisPositions.SetLength(0);
	debrisSpheres.SetLength(0);
	debrisSeed.SetLength(0);
}

//------------------------------------------------------------------------------
//
gosFX::DebrisCloud__Specification*
gosFX::DebrisCloud__Specification::Make(
	std::iostream stream, uint32_t gfx_version)
{
	Check_Object(stream);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	DebrisCloud__Specification* spec = new gosFX::DebrisCloud__Specification(stream, gfx_version);
	// gos_PopCurrentHeap();
	return spec;
}

//------------------------------------------------------------------------------
//
void gosFX::DebrisCloud__Specification::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	Effect__Specification::Save(stream);
	m_minimumDeviation.Save(stream); //	ConstantCurve
	m_maximumDeviation.Save(stream); //	SplineCurve
	m_startingSpeed.Save(stream); //	SeededCurveOf<ComplexCurve,
		// ComplexCurve,Curve::e_ComplexComplexType>
	m_pLifeSpan.Save(stream); //	SeededCurveOf<ComplexCurve,
		// SplineCurve,Curve::e_ComplexSplineType>
	m_pEtherVelocityY.Save(stream); //	SeededCurveOf<ConstantCurve,
		// LinearCurve,Curve::e_ConstantLinearType>
	m_pAccelerationY.Save(stream); //	SeededCurveOf<SplineCurve,
		// LinearCurve,Curve::e_SplineLinearType>
	m_pDrag.Save(stream); //	SeededCurveOf<ComplexCurve,
		// ComplexCurve,Curve::e_ComplexComplexType>
	m_pAlpha.Save(stream); //	SeededCurveOf<ComplexCurve,
		// LinearCurve,Curve::e_ComplexLinearType>
	m_pSpin.Save(stream); //	SeededCurveOf<ConstantCurve,
		// LinearCurve,Curve::e_ConstantLinearType>
	*stream << centerOfForce; //	Stuff::Point3D
	MidLevelRenderer::WriteMLRVersion(stream);
	int32_t i, nrOfParticles = debrisPieces.GetLength();
	*stream << nrOfParticles;
	for (i = 0; i < nrOfParticles; i++)
	{
		*stream << debrisPositions[i];
		*stream << debrisSpheres[i].center;
		*stream << debrisSpheres[i].radius;
		Check_Object(debrisPieces[i]);
		debrisPieces[i]->Save(stream);
	}
}

//------------------------------------------------------------------------------
//
void gosFX::DebrisCloud__Specification::Copy(DebrisCloud__Specification* spec)
{
	// Check_Object(this);
	Check_Object(spec);
	Effect__Specification::Copy(spec);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	m_minimumDeviation = spec->m_minimumDeviation; //	ConstantCurve
	m_maximumDeviation = spec->m_maximumDeviation; //	SplineCurve
	m_startingSpeed = spec->m_startingSpeed; //	SeededCurveOf<ComplexCurve,
		// ComplexCurve,Curve::e_ComplexComplexType>
	m_pLifeSpan = spec->m_pLifeSpan; //	SeededCurveOf<ComplexCurve,
		// SplineCurve,Curve::e_ComplexSplineType>
	m_pEtherVelocityY = spec->m_pEtherVelocityY; //	SeededCurveOf<ConstantCurve,
		// LinearCurve,Curve::e_ConstantLinearType>
	m_pAccelerationY = spec->m_pAccelerationY; //	SeededCurveOf<SplineCurve,
		// LinearCurve,Curve::e_SplineLinearType>
	m_pDrag = spec->m_pDrag; //	SeededCurveOf<ComplexCurve,
		// ComplexCurve,Curve::e_ComplexComplexType>
	m_pAlpha = spec->m_pAlpha; //	SeededCurveOf<ComplexCurve,
		// LinearCurve,Curve::e_ComplexLinearType>
	m_pSpin = spec->m_pSpin; //	SeededCurveOf<ConstantCurve,
		// LinearCurve,Curve::e_ConstantLinearType>
	centerOfForce = spec->centerOfForce; //	Stuff::Point3D
	int32_t i, nrOfParticles = spec->debrisPieces.GetLength();
	debrisPieces.SetLength(nrOfParticles);
	debrisPositions.SetLength(nrOfParticles);
	debrisSpheres.SetLength(nrOfParticles);
	debrisSeed.SetLength(nrOfParticles);
	for (i = 0; i < nrOfParticles; i++)
	{
		debrisPositions[i] = spec->debrisPositions[i];
		debrisSpheres[i] = spec->debrisSpheres[i];
		debrisPieces[i] = spec->debrisPieces[i];
		Check_Object(debrisPieces[i]);
		debrisPieces[i]->AttachReference();
		debrisSeed[i] = spec->debrisSeed[i];
	}
	// gos_PopCurrentHeap();
}

//------------------------------------------------------------------------------
//
void gosFX::DebrisCloud__Specification::LoadGeometry(std::iostream stream)
{
	int32_t i, mlrVersion, nrOfParticles;
	mlrVersion = MidLevelRenderer::ReadMLRVersion(stream);
	*stream >> nrOfParticles;
	debrisPieces.SetLength(nrOfParticles);
	debrisPositions.SetLength(nrOfParticles);
	debrisSpheres.SetLength(nrOfParticles);
	debrisSeed.SetLength(nrOfParticles);
	float minRadius = 100000000000.0f, maxRadius = 0.0f;
	for (i = 0; i < nrOfParticles; i++)
	{
		*stream >> debrisPositions[i];
		*stream >> debrisSpheres[i].center;
		*stream >> debrisSpheres[i].radius;
		if (debrisSpheres[i].radius < minRadius)
		{
			minRadius = debrisSpheres[i].radius;
		}
		if (debrisSpheres[i].radius > maxRadius)
		{
			maxRadius = debrisSpheres[i].radius;
		}
		//
		//---------------
		// Load the shape
		//---------------
		//
		debrisPieces[i] = MidLevelRenderer::MLRShape::Make(stream, mlrVersion);
		Register_Object(debrisPieces[i]);
	}
	_ASSERT(maxRadius > minRadius);
	for (i = 0; i < nrOfParticles; i++)
	{
		debrisSeed[i] = (debrisSpheres[i].radius - minRadius) / (maxRadius - minRadius);
	}
}

//------------------------------------------------------------------------------
//
void gosFX::DebrisCloud__Specification::BuildDefaults()
{
	// Check_Object(this);
	Effect__Specification::BuildDefaults();
	centerOfForce = Stuff::Point3D(0.0f, 00.0f, 0.0f);
	m_lifeSpan.SetCurve(20.0f);
	m_minimumDeviation.SetCurve(0.0f);
	m_maximumDeviation.SetCurve(Stuff::Pi_Over_6);
	m_startingSpeed.m_ageCurve.SetCurve(1.0f);
	m_startingSpeed.m_seeded = true;
	m_startingSpeed.m_seedCurve.SetCurve(1.0f, 3.0f);
	m_pEtherVelocityY.m_ageCurve.SetCurve(0.0f);
	m_pEtherVelocityY.m_seeded = false;
	m_pEtherVelocityY.m_seedCurve.SetCurve(1.0f);
	m_pAccelerationY.m_ageCurve.SetCurve(-9.8f);
	m_pAccelerationY.m_seeded = false;
	m_pAccelerationY.m_seedCurve.SetCurve(1.0f);
	m_pDrag.m_ageCurve.SetCurve(0.3f);
	m_pDrag.m_seeded = false;
	m_pDrag.m_seedCurve.SetCurve(1.0f);
	m_pAlpha.m_ageCurve.SetCurve(1.0f);
	m_pAlpha.m_seeded = false;
	m_pAlpha.m_seedCurve.SetCurve(1.0f);
	m_pSpin.m_ageCurve.SetCurve(Stuff::Pi_Over_2);
	m_pSpin.m_seeded = true;
	m_pSpin.m_seedCurve.SetCurve(-1.0f, 1.0f);
	m_pLifeSpan.m_ageCurve.SetCurve(20.0f);
	m_pLifeSpan.m_seeded = false;
	m_pLifeSpan.m_seedCurve.SetCurve(1.0f);
}

//------------------------------------------------------------------------------
//
bool gosFX::DebrisCloud__Specification::IsDataValid(bool fix_data)
{
	// Check_Object(this);
	return Effect__Specification::IsDataValid(fix_data);
	/*
		centerOfForce = Stuff::Point3D(0.0f, 00.0f, 0.0f);

		m_lifeSpan.SetCurve(20.0f);

		m_minimumDeviation.SetCurve(0.0f);
		m_maximumDeviation.SetCurve(Stuff::Pi_Over_6);

		m_startingSpeed.m_ageCurve.SetCurve(1.0f);
		m_startingSpeed.m_seeded = true;
		m_startingSpeed.m_seedCurve.SetCurve(1.0f, 3.0f);

		m_pEtherVelocityY.m_ageCurve.SetCurve(0.0f);
		m_pEtherVelocityY.m_seeded = false;
		m_pEtherVelocityY.m_seedCurve.SetCurve(1.0f);

		m_pAccelerationY.m_ageCurve.SetCurve(-9.8f);
		m_pAccelerationY.m_seeded = false;
		m_pAccelerationY.m_seedCurve.SetCurve(1.0f);

		m_pDrag.m_ageCurve.SetCurve(0.3f);
		m_pDrag.m_seeded = false;
		m_pDrag.m_seedCurve.SetCurve(1.0f);

		m_pAlpha.m_ageCurve.SetCurve(1.0f);
		m_pAlpha.m_seeded = false;
		m_pAlpha.m_seedCurve.SetCurve(1.0f);

		m_pSpin.m_ageCurve.SetCurve(Stuff::Pi_Over_2);
		m_pSpin.m_seeded = true;
		m_pSpin.m_seedCurve.SetCurve(-1.0f, 1.0f);

		m_pLifeSpan.m_ageCurve.SetCurve(20.0f);
		m_pLifeSpan.m_seeded = false;
		m_pLifeSpan.m_seedCurve.SetCurve(1.0f);
		*/
}

//############################################################################
//##########################  gosFX::DebrisCloud  ############################
//############################################################################

gosFX::DebrisCloud::ClassData* gosFX::DebrisCloud::DefaultData = nullptr;

//------------------------------------------------------------------------------
//
void gosFX::DebrisCloud::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	DefaultData = new ClassData(DebrisCloudClassID, "gosFX::DebrisCloud", EffectCloud::DefaultData,
		(Effect::Factory)&Make, (Specification::Factory)&Specification::Make);
	Register_Object(DefaultData);
}

//------------------------------------------------------------------------------
//
void gosFX::DebrisCloud::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//------------------------------------------------------------------------------
//
gosFX::DebrisCloud::DebrisCloud(Specification* spec, uint32_t flags)
	: Effect(DefaultData, spec, flags)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	debrisPieces.SetLength(spec->debrisPieces.GetLength());
}

//------------------------------------------------------------------------------
//
gosFX::DebrisCloud*
gosFX::DebrisCloud::Make(Specification* spec, uint32_t flags)
{
	Check_Object(spec);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	DebrisCloud* cloud = new gosFX::DebrisCloud(spec, flags);
	// gos_PopCurrentHeap();
	return cloud;
}

//------------------------------------------------------------------------------
//
void gosFX::DebrisCloud::Start(struct gosFX::Effect::ExecuteInfo* info)
{
	// Check_Object(this);
	Check_Pointer(info);
	//
	//--------------------------------------------------------------------------
	// Let effect initialize, then figure out how many particles we want to make
	//--------------------------------------------------------------------------
	//
	Effect::Start(info);
	Specification* spec = GetSpecification();
	Check_Object(spec);
	_ASSERT(debrisPieces.GetLength() == spec->debrisPieces.GetLength());
	int32_t i, numOfParticles = debrisPieces.GetLength();
	for (i = 0; i < numOfParticles; i++)
	{
		Particle* particle = &debrisPieces[i];
		particle->m_localToParent = Stuff::LinearMatrix4D::Identity;
		particle->m_localToParent.BuildTranslation(spec->debrisPositions[i]);
		particle->m_age = 0.0f;
		particle->m_seed = spec->debrisSeed[i];
		float lifetime = spec->m_pLifeSpan.ComputeValue(0.0f, particle->m_seed);
		Min_Clamp(lifetime, 0.0333333f);
		particle->m_ageRate = 1.0f / lifetime;
		Stuff::Vector3D v;
		v.Subtract(spec->debrisPositions[i], spec->centerOfForce);
		//		v.y = 0.0f;
		float lerpFactor =
			Stuff::Lerp(spec->m_minimumDeviation.ComputeValue(0.0f, particle->m_seed),
				spec->m_maximumDeviation.ComputeValue(0.0f, particle->m_seed),
				Stuff::Random::GetFraction());
		particle->m_linearVelocity.Lerp(Stuff::Vector3D::Up, v, lerpFactor);
		particle->m_linearVelocity.Normalize(particle->m_linearVelocity);
		particle->m_linearVelocity *= spec->m_startingSpeed.ComputeValue(0.0f, particle->m_seed);
		float temp = spec->m_pSpin.ComputeValue(0.0f, particle->m_seed);
		particle->m_angularVelocity.x = temp * (2 * Stuff::Random::GetFraction() - 1.0f);
		particle->m_angularVelocity.y = temp * (2 * Stuff::Random::GetFraction() - 1.0f);
		particle->m_angularVelocity.z = temp * (2 * Stuff::Random::GetFraction() - 1.0f);
	}
}

//------------------------------------------------------------------------------
//
bool gosFX::DebrisCloud::Execute(struct gosFX::Effect::ExecuteInfo* info)
{
	// Check_Object(this);
	Check_Object(info);
	//
	//----------------------------------------
	// If we aren't supposed to execute, don't
	//----------------------------------------
	//
	if (!IsExecuted())
		return false;
	//
	//--------------------------------------------------------
	// Figure out the birth rate and request the new particles
	//--------------------------------------------------------
	//
	Specification* spec = GetSpecification();
	Check_Object(spec);
	float dT = static_cast<float>(info->m_time - m_lastRan);
	_ASSERT(dT >= 0.0f);
	float prev_age = m_age;
	m_age += dT * m_ageRate;
	//
	//-----------------------------------
	// Deal with all the active particles
	//-----------------------------------
	//
	int32_t i;
	Stuff::LinearMatrix4D local_to_world;
	local_to_world.Multiply(m_localToParent, *info->m_parentToWorld);
	for (i = 0; i < debrisPieces.GetLength(); i++)
	{
		//
		//--------------------------------------------------------------------
		// If the particle is active, age it and if it is not yet time to die,
		// go to the next particle, otherwise kill it
		//--------------------------------------------------------------------
		//
		Particle* particle = GetParticle(i);
		Check_Object(particle);
		if (particle->m_age < 1.0f)
		{
			particle->m_age += dT * particle->m_ageRate;
			if (AnimateParticle(i, &local_to_world, info->m_time))
			{
				continue;
			}
			DestroyParticle(i);
		}
	}
	//
	//----------------------------
	// Now let effect do its thing
	//----------------------------
	//
	m_age = prev_age;
	if (!Effect::Execute(info))
		return false;
	//
	//-------------------------------------------------------------------
	// If there is no bounds yet, we need to create our extent box around
	// the first legal point we find
	//-------------------------------------------------------------------
	//
	Stuff::ExtentBox box(Stuff::Point3D::Identity, Stuff::Point3D::Identity);
	for (i = 0; i < debrisPieces.GetLength(); i++)
	{
		Particle* particle = GetParticle(i);
		Check_Object(particle);
		//
		//-----------------------------------------------------------
		// We have found our first particle, so put the box around it
		//-----------------------------------------------------------
		//
		if (particle->m_age < 1.0f)
		{
			Stuff::Point3D point;
			float radius = spec->debrisSpheres[i].radius;
			point.Multiply(spec->debrisSpheres[i].center, particle->m_localToParent);
			box.maxX = point.x + radius;
			box.minX = point.x - radius;
			box.maxY = point.y + radius;
			box.minY = point.y - radius;
			box.maxZ = point.z + radius;
			box.minZ = point.z - radius;
			break;
		}
		i++;
	}
	//
	//-----------------------------
	// Look for the other particles
	//-----------------------------
	//
	while (i < debrisPieces.GetLength())
	{
		Particle* particle = GetParticle(i);
		Check_Object(particle);
		if (particle->m_age < 1.0f)
		{
			Stuff::ExtentBox local_box;
			Stuff::Point3D point;
			float radius = spec->debrisSpheres[i].radius;
			point.Multiply(spec->debrisSpheres[i].center, particle->m_localToParent);
			local_box.maxX = point.x + radius;
			local_box.minX = point.x - radius;
			local_box.maxY = point.y + radius;
			local_box.minY = point.y - radius;
			local_box.maxZ = point.z + radius;
			local_box.minZ = point.z - radius;
			box.Union(box, local_box);
		}
		i++;
	}
	//
	//------------------------------------
	// Now, build a info->m_bounds around this box
	//------------------------------------
	//
	_ASSERT(box.maxX >= box.minX);
	_ASSERT(box.maxY >= box.minY);
	_ASSERT(box.maxZ >= box.minZ);
	Stuff::OBB local_bounds = Stuff::OBB::Identity;
	local_bounds.axisExtents.x = 0.5f * (box.maxX - box.minX);
	local_bounds.axisExtents.y = 0.5f * (box.maxY - box.minY);
	local_bounds.axisExtents.z = 0.5f * (box.maxZ - box.minZ);
	local_bounds.localToParent(3, 0) = box.minX + local_bounds.axisExtents.x;
	local_bounds.localToParent(3, 1) = box.minY + local_bounds.axisExtents.y;
	local_bounds.localToParent(3, 2) = box.minZ + local_bounds.axisExtents.z;
	local_bounds.sphereRadius = local_bounds.axisExtents.GetLength();
	if (local_bounds.sphereRadius < Stuff::SMALL)
		local_bounds.sphereRadius = 0.01f;
	Stuff::OBB parent_bounds;
	parent_bounds.Multiply(local_bounds, m_localToParent);
	info->m_bounds->Union(*info->m_bounds, parent_bounds);
	//
	//----------------------------------------------
	// Tell our caller that we get to keep executing
	//----------------------------------------------
	//
	return true;
}

//------------------------------------------------------------------------------
//
bool gosFX::DebrisCloud::HasFinished(void)
{
	// Check_Object(this);
	return Effect::HasFinished();
}

//------------------------------------------------------------------------------
//
void gosFX::DebrisCloud::Kill(void)
{
	// Check_Object(this);
	//
	//-------------------------------------------------------------
	// Destroy all the particles and set up an empty particle cloud
	//-------------------------------------------------------------
	//
	for (size_t i = 0; i < debrisPieces.GetLength(); i++)
	{
		DestroyParticle(i);
	}
	//
	//----------------------------------------
	// Now let the base effect handle stopping
	//----------------------------------------
	//
	Effect::Kill();
}

//------------------------------------------------------------------------------
//
bool gosFX::DebrisCloud::AnimateParticle(
	uint32_t index, const Stuff::LinearMatrix4D* world_to_new_local, Stuff::Time till)
{
	// Check_Object(this);
	//
	//-----------------------------------------
	// Animate the parent then get our pointers
	//-----------------------------------------
	//
	Specification* spec = GetSpecification();
	Check_Object(spec);
	Particle* particle = GetParticle(index);
	Check_Object(particle);
	float seed = particle->m_seed;
	float age = particle->m_age;
	if (age >= 1.0f)
		return false;
	//
	//------------------
	// Animate the color
	//------------------
	//
	Set_Statistic(Shape_Count, Shape_Count + 1);
	particle->m_alpha = spec->m_pAlpha.ComputeValue(age, seed);
	//
	//-----------------------------------------------------------------------
	// If this cloud is unparented, we need to transform the point from local
	// space into world space and set the internal position/velocity pointers
	// to these temporary values
	//-----------------------------------------------------------------------
	//
	Stuff::Point3D translation;
	translation = particle->m_localToParent;
	Stuff::UnitQuaternion rotation;
	rotation = particle->m_localToParent;
	//
	//------------------------------------------------------------------
	// First, calculate the drag on the particle.  Drag can never assist
	// velocity
	//------------------------------------------------------------------
	//
	float drag = -spec->m_pDrag.ComputeValue(age, seed);
	Max_Clamp(drag, 0.0f);
	Stuff::Vector3D ether;
	ether.x = 0.0f;
	ether.y = spec->m_pEtherVelocityY.ComputeValue(age, seed);
	ether.z = 0.0f;
	Stuff::Vector3D accel(Stuff::Vector3D::Identity);
	//
	//-------------------------------------------------------------------
	// Deal with pseudo-world simulation.  In this mode, we interpret the
	// forces as if they are already in worldspace, and we transform them
	// back to local space
	//-------------------------------------------------------------------
	//
	Stuff::LinearMatrix4D world_to_effect;
	world_to_effect.Invert(m_localToWorld);
	Stuff::Vector3D local_ether;
	local_ether.MultiplyByInverse(ether, world_to_effect);
	Stuff::Vector3D rel_vel;
	rel_vel.Subtract(particle->m_linearVelocity, local_ether);
	accel.Multiply(rel_vel, drag);
	//
	//-----------------------------------------
	// Now, add in acceleration of the particle
	//-----------------------------------------
	//
	Stuff::Vector3D world_accel;
	world_accel.x = 0.0f;
	world_accel.y = spec->m_pAccelerationY.ComputeValue(age, seed);
	world_accel.z = 0.0f;
	Stuff::Vector3D local_accel;
	local_accel.Multiply(world_accel, world_to_effect);
	accel += local_accel;
	//
	//-------------------------------------------------
	// Compute the particle's new velocity and position
	//-------------------------------------------------
	//
	float time_slice = static_cast<float>(till - m_lastRan);
	particle->m_linearVelocity.AddScaled(particle->m_linearVelocity, accel, time_slice);
	translation.AddScaled(translation, particle->m_linearVelocity, time_slice);
	//
	//-----------------------
	// Deal with the rotation
	//-----------------------
	//
	Stuff::Vector3D omega(particle->m_angularVelocity);
	omega *= time_slice;
	Stuff::UnitQuaternion omega_q;
	omega_q = omega;
	rotation.Multiply(omega_q, Stuff::UnitQuaternion(rotation));
	rotation.Normalize();
	particle->m_localToParent.BuildRotation(rotation);
	particle->m_localToParent.BuildTranslation(translation);
	Check_Object(&particle->m_localToParent);
	return true;
}

//------------------------------------------------------------------------------
//
void gosFX::DebrisCloud::DestroyParticle(uint32_t index)
{
	Particle* particle = GetParticle(index);
	Check_Object(particle);
	particle->m_age = 1.0f;
}

//------------------------------------------------------------------------------
//
void gosFX::DebrisCloud::Draw(DrawInfo* info)
{
	// Check_Object(this);
	Check_Object(info);
	Check_Object(info->m_parentToWorld);
	//
	//----------------------------
	// Set up the common draw info
	//----------------------------
	//
	Specification* spec = GetSpecification();
	Check_Object(spec);
	int32_t i, nrOfParticle = debrisPieces.GetLength();
	for (i = 0; i < nrOfParticle; i++)
	{
		//
		//-----------------------------------------------------------------
		// If the particle is still alive, concatenate into world space and
		// issue the draw command
		//-----------------------------------------------------------------
		//
		Particle* particle = GetParticle(i);
		Check_Object(particle);
		if (particle->m_age < 1.0f)
		{
			MidLevelRenderer::DrawScalableShapeInformation dinfo;
			MidLevelRenderer::MLRShape* shape = spec->debrisPieces[i];
			dinfo.clippingFlags.SetClippingState(0x3f);
			dinfo.worldToShape = nullptr;
			dinfo.state.Combine(info->m_state, spec->m_state);
			dinfo.activeLights = nullptr;
			dinfo.nrOfActiveLights = 0;
			dinfo.shape = shape;
			Stuff::LinearMatrix4D local_to_world;
			local_to_world.Multiply(m_localToParent, *info->m_parentToWorld);
			//
			//---------------------------------------------------------------
			// No alignment is necessary, so just multiply out all the active
			// particles
			//---------------------------------------------------------------
			//
			Stuff::LinearMatrix4D shape_to_world;
			shape_to_world.Multiply(particle->m_localToParent, local_to_world);
			dinfo.shapeToWorld = &shape_to_world;
			dinfo.scaling = nullptr;
			Stuff::RGBAcolour color(1.0f, 1.0f, 1.0f, particle->m_alpha);
			dinfo.paintMe = &color;
			info->m_clipper->DrawScalableShape(&dinfo);
		}
	}
	//
	//----------------------------
	// Let our parent do its thing
	//----------------------------
	//
	Effect::Draw(info);
}

//------------------------------------------------------------------------------
//
void gosFX::DebrisCloud::TestInstance(void) const
{
	_ASSERT(IsDerivedFrom(DefaultData));
}
