
//==========================================================================//
// File:	 gosFX_Effect.cpp												//
// Contents: Base gosFX::Effect Component									//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//############################################################################
//########################  EffectSpecification  #############################
//############################################################################

#include "stdinc.h"

#include "effect.hpp"

//#include "gosfx/gosfxheaders.h"

//------------------------------------------------------------------------------
//
gosFX::Event::Event(const Event& event)
	: Plug(DefaultData)
{
	// Check_Pointer(this);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	m_time = event.m_time;
	m_flags = event.m_flags;
	m_effectID = event.m_effectID;
}

//------------------------------------------------------------------------------
//
gosFX::Event::Event(std::iostream stream, uint32_t gfx_version)
	: Plug(DefaultData)
{
	// Check_Pointer(this);
	Check_Object(stream);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	*stream >> m_time >> m_flags >> m_effectID >> m_localToParent;
}

//------------------------------------------------------------------------------
//
gosFX::Event*
gosFX::Event::Make(std::iostream stream, uint32_t gfx_version)
{
	Check_Object(stream);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	Event* event = new Event(stream, gfx_version);
	// gos_PopCurrentHeap();
	return event;
}

//------------------------------------------------------------------------------
//
void gosFX::Event::Save(std::iostream stream)
{
	// Check_Object(this);
	*stream << m_time << m_flags << m_effectID << m_localToParent;
}

//############################################################################
//########################  EffectSpecification  #############################
//############################################################################

//------------------------------------------------------------------------------
//
gosFX::Effect__Specification::Effect__Specification(
	Stuff::RegisteredClass::ClassID class_id, std::iostream stream, uint32_t gfx_version)
	: m_events(nullptr)
{
	// Check_Pointer(this);
	Check_Object(stream);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	if (gfx_version < 9)
	{
		STOP(("This version of gosFX is no longer supported"));
		return;
	}
	//
	//--------------
	// Read the name
	//--------------
	//
	m_class = class_id;
	*stream >> m_name;
	//
	//--------------------------------------------------------------------
	// Read the events.  If we are using an array, no events will be saved
	//--------------------------------------------------------------------
	//
	uint32_t event_count;
	*stream >> event_count;
	while (event_count-- > 0)
	{
		Event* event = Event::Make(stream, gfx_version);
		Register_Object(event);
		m_events.Add(event);
	}
	//
	//--------------------------------------------------------
	// Load the curves, variances, and MLRState if appropriate
	//--------------------------------------------------------
	//
	m_lifeSpan.Load(stream, gfx_version);
	m_minimumChildSeed.Load(stream, gfx_version);
	m_maximumChildSeed.Load(stream, gfx_version);
	m_state.Load(stream, MidLevelRenderer::ReadMLRVersion(stream));
	if (gfx_version < 14)
	{
		m_state.SetRenderPermissionMask(
			m_state.GetRenderPermissionMask() | MidLevelRenderer::MLRState::TextureMask);
	}
	if (m_state.GetAlphaMode() != MidLevelRenderer::MLRState::OneZeroMode && m_state.GetPriority() < MidLevelRenderer::MLRState::AlphaPriority)
		m_state.SetPriority(MidLevelRenderer::MLRState::AlphaPriority);
}

//------------------------------------------------------------------------------
//
gosFX::Effect__Specification::Effect__Specification(Stuff::RegisteredClass::ClassID class_id)
	: m_events(nullptr)
{
	// Check_Pointer(this);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	m_class = class_id;
}

//------------------------------------------------------------------------------
//
gosFX::Effect__Specification::~Effect__Specification()
{
	// Check_Pointer(this);
	// Stuff::ChainIteratorOf<Event*> events(&m_events);
	// events.DeletePlugs();
}

//------------------------------------------------------------------------------
//
gosFX::Effect__Specification*
gosFX::Effect__Specification::Make(
	std::iostream stream, uint32_t gfx_version)
{
	Check_Object(stream);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	Effect__Specification* spec =
		new gosFX::Effect__Specification(EffectClassID, stream, gfx_version);
	// gos_PopCurrentHeap();
	return spec;
}

//------------------------------------------------------------------------------
//

void gosFX::Effect__Specification::BuildDefaults()
{
	// Check_Object(this);
	m_lifeSpan.SetCurve(1.0f);
	m_minimumChildSeed.SetCurve(0.0f);
	m_maximumChildSeed.SetCurve(1.0f);
}

//------------------------------------------------------------------------------
//

bool gosFX::Effect__Specification::IsDataValid(bool fix_data)
{
	// Check_Object(this);
	float minv, maxv;
	m_lifeSpan.ExpensiveComputeRange(&minv, &maxv);
	if (minv < 0.0f)
		if (fix_data)
		{
			m_lifeSpan.SetCurve(1.0f);
			// PAUSE(("Warning: Curve \"lifespan\" in Effect \"%s\" Is Out of
			// Range and has been Reset", (std::wstring_view)m_name));
		}
		else
			return false;
	return true;
}

//------------------------------------------------------------------------------
//
gosFX::Effect__Specification*
gosFX::Effect__Specification::Create(
	std::iostream stream, uint32_t gfx_version)
{
	Check_Object(stream);
	Stuff::RegisteredClass::ClassID class_id;
	*stream >> class_id;
	gosFX::Effect::ClassData* class_data =
		Cast_Pointer(gosFX::Effect::ClassData*, Stuff::RegisteredClass::FindClassData(class_id));
	Check_Object(class_data);
	Check_Pointer(class_data->specificationFactory);
	gosFX::Effect__Specification* spec = (*class_data->specificationFactory)(stream, gfx_version);
	Register_Object(spec);
	return spec;
}

//------------------------------------------------------------------------------
//
void gosFX::Effect__Specification::Save(std::iostream stream)
{
	// Check_Object(this);
	*stream << m_class << m_name;
	std::list<Event*> events(m_events); // Stuff::ChainIteratorOf<Event*> events(&m_events);
	size_t count = events.size();
	stream << count;
	Event* event;
	while ((event = events.ReadAndNext()) != nullptr)
	{
		Check_Object(event);
		event->Save(stream);
	}
	m_lifeSpan.Save(stream);
	m_minimumChildSeed.Save(stream);
	m_maximumChildSeed.Save(stream);
	MidLevelRenderer::WriteMLRVersion(stream);
	m_state.Save(stream);
}

//------------------------------------------------------------------------------
//
void gosFX::Effect__Specification::Copy(Effect__Specification* spec)
{
	// Check_Object(this);
	Check_Object(spec);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	_ASSERT(spec->m_class == m_class);
	m_name = spec->m_name;
	//
	//----------------------------------------------
	// Copy the events after delete our current ones
	//----------------------------------------------
	//
	std::list<Event*> old_events(m_events); // Stuff::ChainIteratorOf<Event*> old_events(&m_events);
	// old_events.DeletePlugs();
	std::list<Event*> new_events(
		spec->m_events); // Stuff::ChainIteratorOf<Event*> new_events(&spec->m_events);
	Event* event;
	while ((event = new_events.ReadAndNext()) != nullptr)
	{
		Check_Object(event);
		Event* new_event = new Event(*event);
		Register_Object(new_event);
		AdoptEvent(new_event);
	}
	//
	//---------------------------------------------------
	// Now copy the curves, variance modes, and MLR state
	//---------------------------------------------------
	//
	m_lifeSpan = spec->m_lifeSpan;
	m_minimumChildSeed = spec->m_minimumChildSeed;
	m_maximumChildSeed = spec->m_maximumChildSeed;
	m_state = spec->m_state;
	// gos_PopCurrentHeap();
}

//------------------------------------------------------------------------------
//
void gosFX::Effect__Specification::AdoptEvent(Event* event)
{
	// Check_Object(this);
	Check_Object(event);
	_ASSERT(event->m_time >= 0.0f && event->m_time <= 1.0f);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	//
	//-----------------------------------------------------------
	// The event must be inserted into the chain in order of time
	//-----------------------------------------------------------
	//
	std::list<Event*> events(m_events); // Stuff::ChainIteratorOf<Event*> events(&m_events);
	Event* insert = nullptr;
	while ((insert = events.GetCurrent()) != nullptr)
	{
		Check_Object(insert);
		if (insert->m_time > event->m_time)
		{
			events.Insert(event);
			return;
		}
		events.Next();
	}
	m_events.Add(event);
}

//############################################################################
//##############################   gosFX::Effect
//###################################
//############################################################################

gosFX::Effect::ClassData* gosFX::Effect::DefaultData = nullptr;

//------------------------------------------------------------------------------
//
void gosFX::Effect::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	DefaultData = new ClassData(
		EffectClassID, "gosFX::Effect", Node::DefaultData, &Make, &Specification::Make);
	Register_Object(DefaultData);
}

//------------------------------------------------------------------------------
//
void gosFX::Effect::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
gosFX::Effect::Effect(ClassData* class_data, Specification* spec, uint32_t flags)
	: Node(class_data)
	, m_children(nullptr)
	, m_event(&spec->m_events)
{
	// Check_Pointer(this);
	Check_Object(spec);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	m_specification = spec;
	m_age = 0.0f;
	m_flags = flags;
	m_lastRan = -1.0f;
	m_localToParent = Stuff::LinearMatrix4D::Identity;
}

//------------------------------------------------------------------------------
//
gosFX::Effect::~Effect()
{
	// Check_Object(this);
	Stuff::ChainIteratorOf<gosFX::Effect*> children(&m_children);
	children.DeletePlugs();
}

//------------------------------------------------------------------------------
//
gosFX::Effect*
gosFX::Effect::Make(Specification* spec, uint32_t flags)
{
	Check_Object(spec);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	Effect* effect = new gosFX::Effect(DefaultData, spec, flags);
	// gos_PopCurrentHeap();
	return effect;
}

//------------------------------------------------------------------------------
//
void gosFX::Effect::TestInstance(void) const
{
	_ASSERT(IsDerivedFrom(DefaultData));
}

//------------------------------------------------------------------------------
//
void gosFX::Effect::Start(ExecuteInfo* info)
{
	// Check_Object(this);
	Check_Pointer(info);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	//
	//---------------------------------------------------------------------
	// Don't override m_lastran if we are issuing a Start command while the
	// effect is already running
	//---------------------------------------------------------------------
	//
	if (!IsExecuted() || m_lastRan == -1.0)
		m_lastRan = info->m_time;
	SetExecuteOn();
	//
	//-------------------------------------------
	// If no seed was provided, pick one randomly
	//-------------------------------------------
	//
	m_seed = (info->m_seed == -1.0f) ? Stuff::Random::GetFraction() : info->m_seed;
	_ASSERT(m_seed >= 0.0f && m_seed <= 1.0f);
	//
	//--------------------------------------------------------------------
	// Figure out how int32_t the emitter will live and its initial age based
	// upon the effect seed
	//--------------------------------------------------------------------
	//
	Check_Object(m_specification);
	if (info->m_age == -1.0f)
	{
		float lifetime = m_specification->m_lifeSpan.ComputeValue(m_seed, 0.0f);
		Min_Clamp(lifetime, 0.033333f);
		m_ageRate = 1.0f / lifetime;
		m_age = 0;
	}
	else
	{
		m_age = info->m_age;
		m_ageRate = info->m_ageRate;
		_ASSERT(m_age >= 0.0f && m_age <= 1.0f);
	}
	//
	//--------------------
	// Set up the matrices
	//--------------------
	//
	Check_Object(info->m_parentToWorld);
	m_localToWorld.Multiply(m_localToParent, *info->m_parentToWorld);
	//
	//-------------------------
	// Set up the event pointer
	//-------------------------
	//
	m_event.First();
	// gos_PopCurrentHeap();
}

//------------------------------------------------------------------------------
//
bool gosFX::Effect::Execute(ExecuteInfo* info)
{
	// Check_Object(this);
	Check_Pointer(info);
	_ASSERT(IsExecuted());
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	//
	//-----------------------------------------------------
	// If a new seed is provided, override the current seed
	//-----------------------------------------------------
	//
	if (info->m_seed != -1.0f)
	{
		_ASSERT(info->m_seed >= 0.0f && info->m_seed < 1.0f);
		m_seed = info->m_seed;
	}
	//
	//--------------------------------------------
	// Figure out the new age and clear the bounds
	//--------------------------------------------
	//
	float age = m_age + static_cast<float>(info->m_time - m_lastRan) * m_ageRate;
	_ASSERT(age >= 0.0f && age >= m_age);
	*info->m_bounds = Stuff::OBB::Identity;
	//
	//--------------------------------
	// Update the effectToWorld matrix
	//--------------------------------
	//
	Check_Object(info->m_parentToWorld);
	m_localToWorld.Multiply(m_localToParent, *info->m_parentToWorld);
	//
	//--------------------------------------------------
	// Check to see if the top event needs to be handled
	//--------------------------------------------------
	//
	Check_Object(m_specification);
	Event* event;
	while ((event = m_event.GetCurrent()) != nullptr)
	{
		Check_Object(event);
		if (event->m_time > m_age)
			break;
		//
		//-------------------------------------------------------------
		// This event needs to go, so spawn and bump the effect pointer
		//-------------------------------------------------------------
		//
		uint32_t flags = ExecuteFlag;
		if ((event->m_flags & SimulationModeMask) == ParentSimulationMode)
		{
			_ASSERT((m_flags & SimulationModeMask) != ParentSimulationMode);
			flags |= m_flags & SimulationModeMask;
		}
		else
			flags |= event->m_flags & SimulationModeMask;
		Effect* effect = EffectLibrary::Instance->MakeEffect(event->m_effectID, flags);
		Register_Object(effect);
		m_children.Add(effect);
		m_event.Next();
		//
		//---------------------------------------------
		// Now set the info for starting the new effect
		//---------------------------------------------
		//
		effect->m_localToParent = event->m_localToParent;
		float min_seed = m_specification->m_minimumChildSeed.ComputeValue(m_age, m_seed);
		float seed_range =
			m_specification->m_maximumChildSeed.ComputeValue(m_age, m_seed) - min_seed;
		float seed = Stuff::Random::GetFraction() * seed_range + min_seed;
		Clamp(seed, 0.0f, 1.0f);
		ExecuteInfo local_info(info->m_time, &m_localToWorld, nullptr, seed);
		effect->Start(&local_info);
	}
	//
	//------------------------------------------------------------
	// Execute all the children.  If any of them finish, kill them
	//------------------------------------------------------------
	//
	Stuff::ChainIteratorOf<gosFX::Effect*> children(&m_children);
	gosFX::Effect* child;
	Stuff::OBB child_obb = Stuff::OBB::Identity;
	ExecuteInfo child_info(info->m_time, &m_localToWorld, &child_obb);
	child_info.m_bounds = &child_obb;
	while ((child = children.ReadAndNext()) != nullptr)
	{
		Check_Object(child);
		if (!child->Execute(&child_info))
		{
			Unregister_Object(child);
			delete child;
		}
		//
		//--------------------------------------------------------------
		// Merge the bounding sphere of the child into the bounds of the
		// parent
		//--------------------------------------------------------------
		//
		Stuff::OBB parent_bounds;
		parent_bounds.Multiply(child_obb, m_localToParent);
		info->m_bounds->Union(*info->m_bounds, parent_bounds);
	}
	Check_Object(info->m_bounds);
	//
	//----------------------------------------------------------------------
	// Set the new time, then if we have run the course of the effect, start
	// over if we loop, otherwise wait for our children to finish before
	// killing ourselves
	//----------------------------------------------------------------------
	//
	m_lastRan = info->m_time;
	m_age = age;
	if (m_age >= 1.0f)
	{
		if (IsLooped())
			Start(info);
		else if (HasFinished())
			Kill();
	}
	//
	//----------------------------------
	// Tell our parent if we need to die
	//----------------------------------
	//
	// gos_PopCurrentHeap();
	return IsExecuted();
}

//------------------------------------------------------------------------------
//
void gosFX::Effect::Stop()
{
	// Check_Object(this);
	//
	//------------------------------------
	// Stop the children then stop ourself
	//------------------------------------
	//
	m_age = 1.0f;
	m_event.Last();
	m_event.Next();
}

//------------------------------------------------------------------------------
//
void gosFX::Effect::Kill()
{
	// Check_Object(this);
	//
	//------------------------------------
	// Kill the children then kill ourself
	//------------------------------------
	//
	Stuff::ChainIteratorOf<gosFX::Effect*> children(&m_children);
	gosFX::Effect* child;
	while ((child = children.ReadAndNext()) != nullptr)
	{
		Check_Object(child);
		child->Kill();
	}
	SetExecuteOff();
}

//------------------------------------------------------------------------------
//
void gosFX::Effect::Draw(DrawInfo* info)
{
	// Check_Object(this);
	Check_Pointer(info);
	//
	//-------------------------------------
	// Make sure all the children get drawn
	//-------------------------------------
	//
	DrawInfo new_info;
	Check_Object(m_specification);
	new_info.m_state.Combine(info->m_state, m_specification->m_state);
	Stuff::LinearMatrix4D local_to_world;
	local_to_world.Multiply(m_localToParent, *info->m_parentToWorld);
	new_info.m_parentToWorld = &local_to_world;
	new_info.m_clipper = info->m_clipper;
	new_info.m_clippingFlags = info->m_clippingFlags;
	Stuff::ChainIteratorOf<gosFX::Effect*> children(&m_children);
	gosFX::Effect* child;
	while ((child = children.ReadAndNext()) != nullptr)
	{
		Check_Object(child);
		child->Draw(&new_info);
	}
}

//------------------------------------------------------------------------------
//
bool gosFX::Effect::HasFinished()
{
	// Check_Object(this);
	//
	//-------------------------------------------------------------------------
	// An effect is not finished if it is executing and its life hasn't expired
	//-------------------------------------------------------------------------
	//
	if (IsExecuted() && m_age < 1.0f)
		return false;
	//
	//-----------------------------------------------
	// It is also not finished if it has any children
	//-----------------------------------------------
	//
	Stuff::ChainIteratorOf<gosFX::Effect*> children(&m_children);
	return children.GetCurrent() == nullptr;
}

//#############################################################################
//##########################    gosFX::Effect__ClassData
//##########################
//#############################################################################

void gosFX::Effect__ClassData::TestInstance()
{
	_ASSERT(IsDerivedFrom(gosFX::Effect::DefaultData));
}
