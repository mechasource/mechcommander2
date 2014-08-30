//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"
#include <eh.h>
//#include "stuffheaders.hpp"

#include <gameos.hpp>
#include <toolos.hpp>
#include <stuff/plug.hpp>
#include <stuff/node.hpp>
#include <stuff/slot.hpp>
#include <stuff/chain.hpp>
#include <stuff/safechain.hpp>
#include <stuff/rotation.hpp>
#include <stuff/random.hpp>
#include <stuff/filestream.hpp>
#include <stuff/filestreammanager.hpp>
#include <stuff/trace.hpp>
#include <stuff/stuff.hpp>

using namespace Stuff;


//#include "trace.cpp"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void __stdcall Stuff::Flood_Memory_With_NAN(PVOID where, size_t how_much)
{
	Verify(!(reinterpret_cast<intptr_t>(where)&3));
	Check_Pointer(where);
	pint32_t filler = Cast_Pointer(pint32_t, where);
	for (size_t i = how_much >> 2; i; --i)
	{
		*filler++ = SNAN_NEGATIVE_LONG;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void __cdecl Terminate_Handler(void)
{
	STOP(("Unhandled exception"));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
static uint8_t __stdcall Check_0(void) {return uint8_t((ArmorLevel == 0) ? 1 : 0);}
static uint8_t __stdcall Check_1(void) {return uint8_t((ArmorLevel == 1) ? 1 : 0);}
static uint8_t __stdcall Check_2(void) {return uint8_t((ArmorLevel == 2) ? 1 : 0);}
static uint8_t __stdcall Check_3(void) {return uint8_t((ArmorLevel == 3) ? 1 : 0);}
static uint8_t __stdcall Check_4(void) {return uint8_t((ArmorLevel == 4) ? 1 : 0);}

static void __stdcall Activate_0(void) {ArmorLevel = 0;}
static void __stdcall Activate_1(void) {ArmorLevel = 1;}
static void __stdcall Activate_2(void) {ArmorLevel = 2;}
static void __stdcall Activate_3(void) {ArmorLevel = 3;}
static void __stdcall Activate_4(void) {ArmorLevel = 4;}

static uint8_t __stdcall Greyed(void)
{
	#if defined(_ARMOR)
		return uint8_t(0);
	#else
		return uint8_t(1);
	#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
static uint8_t __stdcall Check_4hz() {return uint8_t((Environment.MaxTimeDelta == 0.25f) ? 1 : 0);}
static uint8_t __stdcall Check_40hz() {return uint8_t((Environment.MaxTimeDelta == 0.025f) ? 1 : 0);}
static uint8_t __stdcall Check_400hz() {return uint8_t((Environment.MaxTimeDelta == 0.0025f) ? 1 : 0);}

static void __stdcall Activate_4hz() {Environment.MaxTimeDelta = Environment.MinimumTimeDelta = 0.25f;}
static void __stdcall Activate_40hz() {Environment.MaxTimeDelta = Environment.MinimumTimeDelta = 0.025f;}
static void __stdcall Activate_400hz() {Environment.MaxTimeDelta = Environment.MinimumTimeDelta = 0.0025f;}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void __stdcall Stuff::InitializeClasses(void)
{
	set_terminate(Terminate_Handler);

	//
	//-------------------------------------------------
	// First, Initialize all the non registered classes
	//-------------------------------------------------
	//
	Verify(FirstFreeStuffClassID <= LastStuffClassID);
	Verify(!FileStreamManager::Instance);
	FileStreamManager::Instance = new FileStreamManager;
	RegisteredClass::InitializeClass();
	Plug::InitializeClass();
	Node::InitializeClass();

	SlotLink::InitializeClass();
	ChainLink::InitializeClass();
	SafeChainLink::InitializeClass();

	Random::InitializeClass();

	//
	//----------------------------------------------
	// Now, initialize all of the registered classes
	//----------------------------------------------
	//
	MemoryStream::InitializeClass();
	FileStream::InitializeClass();
	#if defined(TRACE_ENABLED)
		TraceManager::InitializeClass();
	#endif


	//
	//------------------------------------
	// Build the quatenion fast lerp table
	//------------------------------------
	//
	

	UnitQuaternion::InitializeClass();

	//
	//-------------------
	// Add the armor menu
	//-------------------
	//
	AddDebuggerMenuItem("Libraries\\Stuff\\Armor Level 0", Check_0, Activate_0, Greyed );
	AddDebuggerMenuItem("Libraries\\Stuff\\Armor Level 1", Check_1, Activate_1, Greyed );
	AddDebuggerMenuItem("Libraries\\Stuff\\Armor Level 2", Check_2, Activate_2, Greyed );
	AddDebuggerMenuItem("Libraries\\Stuff\\Armor Level 3", Check_3, Activate_3, Greyed );
	AddDebuggerMenuItem("Libraries\\Stuff\\Armor Level 4", Check_4, Activate_4, Greyed );

	AddDebuggerMenuItem("Libraries\\Stuff\\4hz min", Check_4hz, Activate_4hz, nullptr );
	AddDebuggerMenuItem("Libraries\\Stuff\\40hz min", Check_40hz, Activate_40hz, nullptr );
	AddDebuggerMenuItem("Libraries\\Stuff\\400hz min", Check_400hz, Activate_400hz, nullptr );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void __stdcall Stuff::TerminateClasses(void)
{
	//
	//-----------------------------------------------
	// First, terminate all of the registered classes
	//-----------------------------------------------
	//
	if ( !FileStream::DefaultData ) // yet again, nobody every checks for nullptr pointers
		return;

	UnitQuaternion::TerminateClass();

	#if defined(TRACE_ENABLED)
		TraceManager::TerminateClass();
	#endif
	FileStream::TerminateClass();
	MemoryStream::TerminateClass();

	//
	//-----------------------------------------------
	// Then, terminate all the non registered classes
	//-----------------------------------------------
	//
	Random::TerminateClass();

	SafeChainLink::TerminateClass();
	ChainLink::TerminateClass();
	SlotLink::TerminateClass();

	Node::TerminateClass();
	Plug::TerminateClass();
	RegisteredClass::TerminateClass();
	Check_Pointer(FileStreamManager::Instance);
	delete FileStreamManager::Instance;
	FileStreamManager::Instance = nullptr;
}
