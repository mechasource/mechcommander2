//==========================================================================//
// File:	 gosFX_PointCloud.hpp											//
// Contents: PointCloud Component											//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//
#pragma once

#include <gosfx/gosfx.hpp>
#include <gosfx/particlecloud.hpp>
#include <mlr/mlr.hpp>

namespace MidLevelRenderer {class MLRPointCloud;}

namespace gosFX
{
	//############################################################################
	//########################  PointCloud__Specification  #############################
	//############################################################################

	class PointCloud__Specification:
		public ParticleCloud__Specification
	{
	//----------------------------------------------------------------------
	// Constructors/Destructors
	//
	protected:
		PointCloud__Specification(
			Stuff::MemoryStream *stream,
			int gfx_version
		);

	public:
		PointCloud__Specification();

		static PointCloud__Specification*
			Make(
				Stuff::MemoryStream *stream,
				int gfx_version
			);
	};

	//############################################################################
	//########################  ParticleCloud__Particle  #############################
	//############################################################################

	class PointCloud__Particle:
		public ParticleCloud__Particle
	{
	public:
		Stuff::Point3D
			m_worldTranslation;
	};

//############################################################################
//#############################  PointCloud  #################################
//############################################################################

	class PointCloud : public ParticleCloud
	{
	//----------------------------------------------------------------------------
	// Class Registration Support
	//
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);

		static ClassData* DefaultData;

		typedef PointCloud__Specification Specification;
		typedef PointCloud__Particle Particle;

		enum {
			ParticleSize = sizeof(Particle) + sizeof(Stuff::Point3D) + sizeof(Stuff::RGBAColor)
		};

	protected:
		MidLevelRenderer::MLRPointCloud
			*m_cloudImplementation;
		Stuff::Point3D
			*m_P_localTranslation;
		Stuff::RGBAColor
			*m_P_color;

	//----------------------------------------------------------------------------
	// Constructor/Destructor
	//
	protected:
		PointCloud(
			Specification *spec,
			uint32_t flags
		);

	public:
		~PointCloud();

		static PointCloud*
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
		bool
			Execute(ExecuteInfo *info);
		void
			Draw(DrawInfo *info);
	};
}
