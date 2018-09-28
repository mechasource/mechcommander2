//==========================================================================//
// File:	 gosFX_Effect.hpp												//
// Contents: Base Effect Component											//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#include <stuff/linearmatrix.hpp>
#include <stuff/registeredclass.hpp>
#include <gosfx/gosfx.hpp>
#include <gosfx/fcurve.hpp>

//#include <stuff/plug.hpp>
//#include <mlr/mlr.hpp>


namespace MidLevelRenderer
{
class MLRState;
class MLRClipper;
class MLRClippingState;
} // namespace MidLevelRenderer

namespace gosFX
{
class Effect__ClassData;

//############################################################################
//####################################  Event  ###############################
//############################################################################

class Event // : public Stuff::Plug
{
public:
	Event() /* : Plug(DefaultData) */  {}
	Event(const Event& event);

	static Event* Make(std::istream& stream, uint32_t gfx_version);

	void Save(std::ostream& stream);

	float m_time;
	uint32_t m_flags, m_effectID;
	Stuff::LinearMatrix4D m_localToParent;

protected:
	Event(std::istream& stream, uint32_t gfx_version);
};

//############################################################################
//########################  Effect__Specification  #############################
//############################################################################

class Effect__Specification
#if defined(_ARMOR)
	: public Stuff::Signature
#endif
{
	//----------------------------------------------------------------------
	// Constructors/Destructors
	//
protected:
	Effect__Specification(Stuff::RegisteredClass::ClassID class_id, std::iostream stream, uint32_t gfx_version);

public:
	Effect__Specification(Stuff::RegisteredClass::ClassID class_id = gosFX::EffectClassID);
	virtual ~Effect__Specification(void);

	static Effect__Specification* Create(std::iostream stream, uint32_t gfx_version);
	typedef Effect__Specification* (*Factory)(std::iostream stream, uint32_t gfx_version);
	static Effect__Specification* Make(std::iostream stream, uint32_t gfx_version);

	virtual void Save(std::iostream stream);
	virtual void BuildDefaults(void);
	virtual bool IsDataValid(bool fix_data = false);

	Stuff::RegisteredClass::ClassID GetClassID()
	{
		// Check_Object(this);
		return m_class;
	}

	virtual void Copy(Effect__Specification* spec);

protected:
	Stuff::RegisteredClass::ClassID m_class;

	uint32_t m_effectID;
	std::wstring m_name;

	//----------------------------------------------------------------------
	// Events
	//
public:
	void AdoptEvent(Event* event);

protected:
	std::list<Event*> m_events;		// Stuff::ChainOf<Event*> m_events;

	//-------------------------------------------------------------------------
	// FCurves
	//
public:

protected:
	ConstantCurve m_lifeSpan;
	SplineCurve m_maximumChildSeed;
	SplineCurve m_minimumChildSeed;

	//----------------------------------------------------------------------
	// States
	//
public:

protected:
	MidLevelRenderer::MLRState m_state;

	//----------------------------------------------------------------------
	// Testing
	//
public:
	void TestInstance(void) const {}
};

//############################################################################
//###############################  Effect  ###################################
//############################################################################

class Effect // : public Stuff::Node
{
	friend class EffectCloud;

	//----------------------------------------------------------------------------
	// Types
	//
public:
	struct ExecuteInfo
	{
		Stuff::Time m_time;
		float m_seed; // 0 <= m_seed <= 1
		float m_age;
		float m_ageRate;
		const Stuff::LinearMatrix4D* m_parentToWorld;
		Stuff::OBB* m_bounds;

		ExecuteInfo(Stuff::Time time, const Stuff::LinearMatrix4D* parent_to_world,
			Stuff::OBB* bounds, float seed = -1.0f)
		{
			m_time			= time;
			m_seed			= seed;
			m_parentToWorld = parent_to_world;
			m_bounds		= bounds;
			m_age			= -1.0f;
			m_ageRate		= -1.0f;
		}

		void TestInstance(void) const {}

	private:
		ExecuteInfo(float time, const Stuff::LinearMatrix4D* parent_to_world, Stuff::OBB* bounds,
			float seed = -1.0f);
	};

	struct DrawInfo
	{
		const Stuff::LinearMatrix4D* m_parentToWorld;
		MidLevelRenderer::MLRClippingState m_clippingFlags;
		MidLevelRenderer::MLRState m_state;
		MidLevelRenderer::MLRClipper* m_clipper;

		void TestInstance(void) const {}
	};

	typedef Effect__Specification Specification;
	typedef Effect__ClassData ClassData;

	//----------------------------------------------------------------------------
	// Initialization
	//
public:
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);
	static ClassData* DefaultData;

	//----------------------------------------------------------------------------
	// Constructors/Destructors
	//
protected:
	Effect(ClassData* class_data, Specification* spec, uint32_t flags);

public:
	~Effect(void);

	typedef Effect* (*Factory)(Specification* spec, uint32_t flags);

	static Effect* Make(Specification* spec, uint32_t flags);

	Specification* GetSpecification()
	{
		// Check_Object(this);
		return m_specification;
	}

protected:
	Specification* m_specification;

	//----------------------------------------------------------------------------
	// Events
	//
protected:
	std::list<Effect*> m_children;	// Stuff::ChainOf<Effect*> m_children;
	std::unique_ptr<Event> m_event; // Stuff::ChainIteratorOf<Event*> m_event;

	//----------------------------------------------------------------------------
	// Testing
	//
public:
	void TestInstance(void) const;

	//----------------------------------------------------------------------------
	// API
	//
public:
	virtual void Start(ExecuteInfo* info);
	void Stop(void);
	virtual bool Execute(ExecuteInfo* info);
	virtual void Kill(void);
	virtual void Draw(DrawInfo* info);
	virtual bool HasFinished(void);

	enum : uint32_t
	{
		ExecuteFlag						= 1,
		LoopFlag						= 2,
		LocalSpaceSimulationMode		= 0,
		DynamicWorldSpaceSimulationMode = 4,
		StaticWorldSpaceSimulationMode  = 8,
		ParentSimulationMode			= 12,
		SimulationModeMask				= 12
	};

	static Stuff::Vector3D s_ether;
	static Stuff::Vector3D s_gravity;

public:
	void SetExecuteOn()
	{
		// Check_Object(this);
		m_flags |= ExecuteFlag;
	}
	void SetExecuteOff()
	{
		// Check_Object(this);
		m_flags &= ~ExecuteFlag;
	}
	bool IsExecuted()
	{
		// Check_Object(this);
		return (m_flags & ExecuteFlag) != 0;
	}

	void SetLoopOn()
	{
		// Check_Object(this);
		m_flags |= LoopFlag;
	}
	void SetLoopOff()
	{
		// Check_Object(this);
		m_flags &= ~LoopFlag;
	}
	bool IsLooped()
	{
		// Check_Object(this);
		return (m_flags & LoopFlag) != 0;
	}

	void UseLocalSpaceSimulation()
	{
		// Check_Object(this);
		m_flags &= ~SimulationModeMask;
	}
	void UseStaticWorldSpaceSimulation()
	{
		// Check_Object(this);
		m_flags &= ~SimulationModeMask;
		m_flags |= StaticWorldSpaceSimulationMode;
	}
	void UseDynamicWorldSpaceSimulation()
	{
		// Check_Object(this);
		m_flags &= ~SimulationModeMask;
		m_flags |= DynamicWorldSpaceSimulationMode;
	}
	int32_t GetSimulationMode()
	{
		// Check_Object(this);
		return m_flags & SimulationModeMask;
	}

	int32_t GetSimulationFlags()
	{
		// Check_Object(this);
		return m_flags;
	}

	Stuff::LinearMatrix4D m_localToWorld;

protected:
	Stuff::LinearMatrix4D m_localToParent;
	time_t m_lastRan;
	float m_age, m_ageRate, m_seed;
	uint32_t m_flags;
};

//##########################################################################
//########################    Effect__ClassData    #########################
//##########################################################################

class Effect__ClassData //: public Stuff::Plug::ClassData
{
	//----------------------------------------------------------------------------
	//
public:
	Effect__ClassData(Stuff::RegisteredClass::ClassID class_id, PCSTR class_name,
		Stuff::Plug::ClassData* parent_class, Effect::Factory effect_factory,
		Effect::Specification::Factory spec_factory)
		: RegisteredClass__ClassData(class_id, class_name, parent_class),
		  effectFactory(effect_factory), specificationFactory(spec_factory)
	{
	}

	Effect::Factory effectFactory;
	Effect::Specification::Factory specificationFactory;

	//----------------------------------------------------------------------------
	//
public:
	void TestInstance(void);
};
} // namespace gosFX
