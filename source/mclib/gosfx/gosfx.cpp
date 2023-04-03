//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "gosfx/gosfx.h"
#include "stuff/linearmatrix.h"
#include "gosfx/effect.h"

// HGOSHEAP gosFX::Heap = nullptr;

DEFINE_TIMER(gosFX, Animation_Time);
DEFINE_TIMER(gosFX, Draw_Time);

uint32_t gosFX::Point_Count;
uint32_t gosFX::Shard_Count;
uint32_t gosFX::Pert_Count;
uint32_t gosFX::Card_Count;
uint32_t gosFX::Shape_Count;
uint32_t gosFX::Profile_Count;

static Stuff::LinearMatrix4D Effect_Into_Motion_data(true);
static Stuff::LinearMatrix4D Effect_Against_Motion_data(true);
const Stuff::LinearMatrix4D& gosFX::Effect_Into_Motion = Effect_Into_Motion_data;
const Stuff::LinearMatrix4D& gosFX::Effect_Against_Motion = Effect_Against_Motion_data;

//-----------------------------------------------------------------------------
//

void __stdcall gosFX::InitializeClasses(void)
{
	_ASSERT(FirstFreegosFXClassID <= Stuff::LastgosFXClassID);
	// _ASSERT(!Heap);
	// Heap = gos_CreateMemoryHeap("gosFX", 0, ParentClientHeap);
	// Check_Pointer(Heap);
	// gos_PushCurrentHeap(Heap);

	Effect::InitializeClass();
	ParticleCloud::InitializeClass();
	PointCloud::InitializeClass();
	SpinningCloud::InitializeClass();
	ShardCloud::InitializeClass();
	ShapeCloud::InitializeClass();
	PertCloud::InitializeClass();
	CardCloud::InitializeClass();
	EffectCloud::InitializeClass();
	Singleton::InitializeClass();
	Card::InitializeClass();
	Shape::InitializeClass();
	Tube::InitializeClass();
	DebrisCloud::InitializeClass();
	PointLight::InitializeClass();
	EffectLibrary::InitializeClass();
	Effect_Into_Motion_data(1, 1) = 0.0f;
	Effect_Into_Motion_data(1, 2) = 1.0f;
	Effect_Into_Motion_data(2, 1) = -1.0f;
	Effect_Into_Motion_data(2, 2) = 0.0f;
	Effect_Against_Motion_data(1, 1) = 0.0f;
	Effect_Against_Motion_data(1, 2) = -1.0f;
	Effect_Against_Motion_data(2, 1) = 1.0f;
	Effect_Against_Motion_data(2, 2) = 0.0f;
	// gos_PopCurrentHeap();
	StatisticFormat("");
	StatisticFormat("gosFX");
	StatisticFormat("=====");
	StatisticFormat("");
	Initialize_Timer(Animation_Time, "Animation Time");
	Initialize_Timer(Draw_Time, "Draw Time");
	AddStatistic("Points", "prims", gos_DWORD, &Point_Count, Stat_AutoReset);
	AddStatistic("Shards", "prims", gos_DWORD, &Shard_Count, Stat_AutoReset);
	AddStatistic("Perts", "prims", gos_DWORD, &Pert_Count, Stat_AutoReset);
	AddStatistic("Cards", "prims", gos_DWORD, &Card_Count, Stat_AutoReset);
	AddStatistic("Shapes", "prims", gos_DWORD, &Shape_Count, Stat_AutoReset);
	AddStatistic("Profiles", "prims", gos_DWORD, &Profile_Count, Stat_AutoReset);
}

//-----------------------------------------------------------------------------
//
void __stdcall gosFX::TerminateClasses(void)
{
	// make sure these were actually initialized
	if (!DebrisCloud::DefaultData)
		return;
	EffectLibrary::TerminateClass();
	PointLight::TerminateClass();
	DebrisCloud::TerminateClass();
	Tube::TerminateClass();
	Shape::TerminateClass();
	Card::TerminateClass();
	Singleton::TerminateClass();
	EffectCloud::TerminateClass();
	CardCloud::TerminateClass();
	PertCloud::TerminateClass();
	ShapeCloud::TerminateClass();
	ShardCloud::TerminateClass();
	SpinningCloud::TerminateClass();
	PointCloud::TerminateClass();
	ParticleCloud::TerminateClass();
	Effect::TerminateClass();
	// Check_Pointer(Heap);
	// gos_DestroyMemoryHeap(Heap, true);
	// Heap = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t
gosFX::ReadGFXVersion(std::iostream erf_stream)
{
	Check_Object(erf_stream);
	int32_t erf_signature;
	*erf_stream >> erf_signature;
	_ASSERT(erf_signature == 'GFX#');
	uint32_t version;
	*erf_stream >> version;
	if (version > CurrentGFXVersion)
		STOP(("Application must be rebuilt to use this version of GFX data"));
	return version;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::WriteGFXVersion(std::iostream erf_stream)
{
	Check_Object(erf_stream);
	*erf_stream << 'GFX#' << static_cast<int32_t>(CurrentGFXVersion);
}
