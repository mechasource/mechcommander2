//==========================================================================//
// File:	 gosFX_EffectCloud.hpp											//
// Contents: EffectCloud Component											//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//
#pragma once

#include <gosfx/gosfx.hpp>
#include <gosfx/spinningcloud.hpp>

namespace gosFX
{
	//############################################################################
	//########################  EffectCloud__Specification  #############################
	//############################################################################

	class EffectCloud__Specification:
		public SpinningCloud__Specification
	{
	//----------------------------------------------------------------------
	// Constructors/Destructors
	//
	protected:
		EffectCloud__Specification(
			Stuff::MemoryStream *stream,
			int32_t gfx_version
		);

	public:
		EffectCloud__Specification();

		static EffectCloud__Specification*
			Make(
				Stuff::MemoryStream *stream,
				int32_t gfx_version
			);

		void
			Copy(EffectCloud__Specification *spec);

		void
			Save(Stuff::MemoryStream *stream);

		uint32_t
			m_particleEffectID;
	};

	//############################################################################
	//########################  SpinningCloud__Particle  #############################
	//############################################################################

	class EffectCloud__Particle:
		public SpinningCloud__Particle
	{
	public:
		Effect
			*m_effect;
	};

	//############################################################################
	//#############################  EffectCloud  #################################
	//############################################################################

	class EffectCloud:
		public SpinningCloud
	{
	//----------------------------------------------------------------------------
	// Class Registration Support
	//
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);

		typedef EffectCloud__Specification Specification;
		typedef EffectCloud__Particle Particle;

		enum {
			ParticleSize = sizeof(Particle)
		};

	//----------------------------------------------------------------------------
	// Class Data Support
	//
	protected:
		EffectCloud(
			Specification *spec,
			uint32_t flags
		);

	public:
		~EffectCloud();

		static EffectCloud*
			Make(
				Specification *spec,
				uint32_t flags
			);

		Specification*
			GetSpecification()
				{
					Check_Object(this);
					return
						Cast_Object(Specification*, m_specification);
				}
		Particle*
			GetParticle(uint32_t index)
				{
					Check_Object(this); Check_Object(GetSpecification());
					return
						Cast_Pointer(
							Particle*,
							&m_data[index*GetSpecification()->m_particleClassSize]
						);
				}

		static ClassData* DefaultData;

	//----------------------------------------------------------------------------
	// Testing
	//
	public:
		void TestInstance(void) const;

	//----------------------------------------------------------------------------
	// API
	//
	protected:
		bool
			AnimateParticle(
				uint32_t index,
				const Stuff::LinearMatrix4D *world_to_new_local,
				Stuff::Time till
			);
		void
			CreateNewParticle(
				uint32_t index,
				Stuff::Point3D *translation
			);
		void
			DestroyParticle(uint32_t index);

	public:
		void
			Draw(DrawInfo *info);
	};
}
