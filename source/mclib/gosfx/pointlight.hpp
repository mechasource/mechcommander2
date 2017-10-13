//==========================================================================//
// File:	 gosFX_PointLight.hpp									    	//
// Contents: Base PointLight Particle									    //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//
#pragma once

//#include <gosfx/gosfx.hpp>
//#include <gosfx/effect.hpp>

namespace gosFX
{
//############################################################################
//############################  LightManager  ################################
//############################################################################

	class Light;

	class LightManager
#if defined(_ARMOR)
		: public Stuff::Signature
#endif
	{
	public:
		static LightManager* Instance;

		virtual Light* MakePointLight(PCSTR light_map = nullptr);
		struct Info
		{
			Stuff::RGBColor m_color;
			Stuff::LinearMatrix4D m_origin;
			float m_intensity,
			float m_inner;
			float m_outer;
			Stuff::Radian m_spread;
		};

		virtual void ChangeLight(Light* light, Info* info);
		virtual void DeleteLight(Light* light);

		void TestInstance(void) const {}
	};

//############################################################################
//####################  PointLight__Specification  #########################
//############################################################################

	class PointLight__Specification:
		public Effect__Specification
	{
		//----------------------------------------------------------------------
		// Constructors/Destructors
		//
	protected:
		PointLight__Specification(
			Stuff::MemoryStream* stream,
			uint32_t gfx_version
		);

	public:
		PointLight__Specification(void);

		void Copy(PointLight__Specification* spec);
		void Save(Stuff::MemoryStream* stream);
		void BuildDefaults(void);
		bool IsDataValid(bool fix_data = false);

		static PointLight__Specification* Make(Stuff::MemoryStream* stream, uint32_t gfx_version);

		//-------------------------------------------------------------------------
		// FCurves
		//
	public:
		ComplexCurve m_red;
		ComplexCurve m_green;
		ComplexCurve m_blue;
		ComplexCurve m_intensity;
		SplineCurve  m_innerRadius;
		SplineCurve  m_outerRadius;

		bool m_twoSided;
		std::wstring m_lightMap;	// Stuff::MString	m_lightMap;
	};

//############################################################################
//##############################  PointLight  #############################
//############################################################################

	class PointLight:
		public Effect
	{
	public:
		static void __stdcall InitializeClass(void);
		static void TerminateClass(void);

		static ClassData* DefaultData;

		typedef PointLight__Specification Specification;

		static PointLight* Make(Specification* spec, uint32_t flags);
		~PointLight(void);

	protected:
		PointLight(Specification* spec, uint32_t flags);
		Light* m_light;

		//----------------------------------------------------------------------------
		// Class Data Support
		//
	public:
		Specification* GetSpecification(void)
		{
			// Check_Object(this);
			return Cast_Object(Specification*, m_specification);
		}

		//----------------------------------------------------------------------------
		// API
		//
	public:
		void Start(ExecuteInfo* info);
		bool Execute(ExecuteInfo* info);
		void Kill(void);

		//----------------------------------------------------------------------------
		// Testing
		//
	public:
		void TestInstance(void) const;
	};
}
