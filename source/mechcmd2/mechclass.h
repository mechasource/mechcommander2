//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MECHCLASS_H
#define MECHCLASS_H

enum class
{
	MECH_CLASS_NONE,
	MECH_CLASS_LIGHT,
	MECH_CLASS_MEDIUM,
	MECH_CLASS_HEAVY,
	MECH_CLASS_ASSAULT,
	NUM_MECH_CLASSES
} MechClass;

enum class
{
	VEHICLE_CLASS_NONE,
	VEHICLE_CLASS_LIGHTMECH,
	VEHICLE_CLASS_MEDIUMMECH,
	VEHICLE_CLASS_HEAVYMECH,
	VEHICLE_CLASS_ASSAULTMECH,
	VEHICLE_CLASS_GROUND,
	VEHICLE_CLASS_ELEMENTAL,
	NUM_VEHICLE_CLASSES
} VehicleClass;

enum class
{
	PHYSICAL_ATTACK_RAM,
	PHYSICAL_ATTACK_DFA,
	NUM_PHYSICAL_ATTACKS
} PhysicalAttack;

#endif