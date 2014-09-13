//===========================================================================//
// File:	MLRStuff.hpp                                                     //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#include <stuff/stuff.hpp>

namespace gosFX
{

//
//--------------
// gosFX classes
//--------------
//
	typedef enum __gosFX_const
	{
		EffectClassID = Stuff::FirstgosFXClassID,
		ParticleCloudClassID,
		PointCloudClassID,
		SpinningCloudClassID,
		ShardCloudClassID,
		PertCloudClassID,
		CardCloudClassID,
		ShapeCloudClassID,
		EffectCloudClassID,
		SingletonClassID,
		CardClassID,
		ShapeClassID,
		TubeClassID,
		DebrisCloudClassID,
		PointLightClassID,
		FirstFreegosFXClassID,
	};

	enum {CurrentGFXVersion = 17};

	int32_t __stdcall ReadGFXVersion(Stuff::MemoryStream* erf_stream);
	void 	__stdcall WriteGFXVersion(Stuff::MemoryStream* erf_stream);

	void __stdcall InitializeClasses(void);
	void __stdcall TerminateClasses(void);

	extern HGOSHEAP Heap;

	extern const Stuff::LinearMatrix4D& Effect_Into_Motion;
	extern const Stuff::LinearMatrix4D& Effect_Against_Motion;

	DECLARE_TIMER(extern, Animation_Time);
	DECLARE_TIMER(extern, Draw_Time);
	extern uint32_t Point_Count;
	extern uint32_t Shard_Count;
	extern uint32_t Pert_Count;
	extern uint32_t Card_Count;
	extern uint32_t Shape_Count;
	extern uint32_t Profile_Count;
}

#include <gosfx/fcurve.hpp>
