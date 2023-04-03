//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//***************************************************************************
//
//								WARRIOR.CPP
//
//***************************************************************************
#include "stdinc.h"

// #include "mclib.h"

#include "warrior.h"
#include "gameobj.h"
#include "objmgr.h"
#include "group.h"
#include "gamelog.h"
#include "team.h"
#include "comndr.h"
#include "mover.h"
#include "movemgr.h"
#include "mech.h"
#include "gvehicl.h"
#include "bldng.h"
#include "gate.h"
#include "turret.h"
#include "tacordr.h"
#include "radio.h"
#include "multplyr.h"
#include "goal.h"
#include "turret.h"
#include "bldng.h"
#include "elemntl.h"

#define TESTING_WITH_PLAYER 1

enum
{
	T_A = 0,
	T = T_A,
	T_B,
	U,
	V,
};

//***************************************************************************

#define MAXLEN_INPUTSTR 512
constexpr float CELL_DISTANCE = 42.6667f;

//***************************************************************************
enum class
{
	MECH_BODY_LOCATION_ANY = -1,
	MECH_BODY_LOCATION_HEAD,
	MECH_BODY_LOCATION_CTORSO,
	MECH_BODY_LOCATION_LTORSO,
	MECH_BODY_LOCATION_RTORSO,
	MECH_BODY_LOCATION_LARM,
	MECH_BODY_LOCATION_RARM,
	MECH_BODY_LOCATION_LLEG,
	MECH_BODY_LOCATION_RLEG,
	NUM_MECH_BODY_LOCATIONS
} MechBodyLocationType;

BldgAppearance* MechWarrior::wayPointMarkers[3] = {0, 0, 0};
GoalManager* MechWarrior::goalManager = nullptr;

extern float scenarioTime;
#ifdef USE_SCENARIO
extern turn;
#endif

float FireOddsTable[FireOddsType::count] = {20.0, 35.0, 50.0, 65.0, 80.0};

std::wstring_view pilotAlarmFunctionName[NUM_PILOT_ALARMS] = {"handletargetofweaponfire",
	"handlehitbyweaponfire", "handledamagetakenrate", "handledeathofmate",
	"handlecripplingoffriendlyvehicle", "handledestructionoffriendlyvehicle",
	"handleincapacitationofvehicle", "handledestructionofvehicle", "handlewithdraw",
	"handleattackorder", "handlecollision", "handleguardbreach", "handlekilledtarget",
	"handlematefiredweapon", "handleplayerorder", "handlenomovepath", "handlegateclosing",
	"handlefiredweapon", "handlenewmover"};

wchar_t ProfessionalismOffsetTable[NUM_OFFSET_RANGES][2] = {
	{10, 10}, {20, 5}, {30, 0}, {40, 5}, {100, -10}};

wchar_t DecorumOffsetTable[NUM_OFFSET_RANGES][2] = {{10, 10}, {20, 5}, {30, 0}, {40, 5}, {100, -10}};

wchar_t AmmoConservationModifiers[2][2] = {{50, -5}, {20, -10}};

std::wstring_view SkillsTable[Skill::numberofskills] = {"Piloting", "Sensors", "Gunnery"};

//-----------------------------------------------
// First column is ThreatRating, second column is
// for Inner Sphere, third column is Clan...
float ThreatRatingEffect[8][3] = {{0.5f, -1.0f, 0.0f}, {0.75f, -0.2f, 0.0f}, {1.0f, 0.0f, 0.0f},
	{1.25f, 0.0f, 0.0f}, {1.5f, 0.0f, 0.0f}, {2.0f, 0.1f, 0.0f}, {2.25f, 0.2f, -0.1f},
	{1000000.0f, 0.2f, -1.0f}};

float BrainUpdateFrequency = 2.25;
float MovementUpdateFrequency = 5.0;
float CombatUpdateFrequency = 0.25;
float CommandUpdateFrequency = 6.0;
float PilotCheckUpdateFrequency = 1.0;
float ThreatAnalysisFrequency = 5.0;

int32_t PilotCheckModifierTable[2] = {25, 25};

float SkillWeightings[Skill::numberofskills] = {1.0, 1.0, 1.0};

float WarriorRankScale[WarriorRank::numberofranks] = {50.0, 60.0, 70.0, 79.0, 999.0};

std::wstring_view SpecialtySkillsTable[NUM_SPECIALTY_SKILLS] = {
	"LightMechSpecialist", "LaserSpecialist", "LightACSpecialist", "MediumACSpecialist",
	"SRMSpecialist", "SmallArmsSpecialist", "SensorProfileSpecialist",
	"ToughnessSpecialist", // Thoughness Specialty

	"MediumMechSpecialist", "PulseLaserSpecialist", "ERLaserSpecialist", "LRMSpecialist",
	"Scout", // Scouting Specialty
	"LongJump", // Jump Jet Specialty

	"HevayMechSpecialist", // Heavy mech Specialty
	"PPCSpecialist", "HeavyACSpecialist", "ShortRangeSpecialist", "MediumRangeSpecialist",
	"LongRangeSpecialist",

	"AssaultMechSpecialist", "GaussSpecialist",
	"SharpShooter", // Sharpshooter specialty
};

SortListPtr MechWarrior::sortList = nullptr;
int32_t MechWarrior::numWarriors = 0;
int32_t MechWarrior::numWarriorsInCombat = 0;
bool MechWarrior::brainsEnabled[MAX_TEAMS] = {true, true, true, true, true, true, true, true};
float MechWarrior::minSkill = 1;
float MechWarrior::maxSkill = 100;
int32_t MechWarrior::increaseCap = 100;
float MechWarrior::maxVisualRadius = 100.0;
int32_t MechWarrior::curEventID = 0;
int32_t MechWarrior::curEventTrigger = 0;
MechWarrior* MechWarrior::warriorList[MAX_WARRIORS];

int32_t LastMoveCalcErr = 0;
int32_t GroupMoveTrailLen[2] = {0, 1};
float MoveTimeOut = 30.0;
float MoveYieldTime = 1.5;
float DefaultAttackRadius = 275.0;

QueuedTacOrder TacOrderQueue[MAX_QUEUED_TACORDERS];
int32_t TacOrderQueuePos = 0;

extern int32_t SimpleMovePathRange;
extern int32_t AttitudeEffectOnMovePath[AttitudeType::count][3];
extern wchar_t AttitudeEffect[AttitudeType::count][6];
#ifdef USE_TEAMS
extern TeamPtr homeTeam;
#endif
// extern GlobalMapPtr GlobalMoveMap[2];
extern MoverGroupPtr CurGroup;
extern GameObjectPtr CurObject;
extern int32_t CurObjectClass;
extern std::unique_ptr<MechWarrior> CurWarrior;
extern GameObjectPtr CurContact;
extern int32_t CurAlarm;
extern float MapCellDiagonal;
extern float WeaponRanges[NUM_WEAPON_RANGE_TYPES][2];

// extern float MoveMarginOfError[2];

extern const std::unique_ptr<Debugger>& debugger;

extern float MechClassWeights[NUM_MECH_CLASSES];

extern wchar_t OverlayIsBridge[NUM_OVERLAY_TYPES];

extern float SkillTry[4];
extern float SkillSuccess[4];
extern float KillSkill[6];
extern float WeaponHit;
extern float SensorSkill;

extern int32_t RamObjectWID;

extern UserHeapPtr missionHeap;

SpecialtySkillType MechWarrior::skillTypes[NUM_SPECIALTY_SKILLS] = {
	CHASSIS_SPECIALTY, //	"LightMechSpecialist",
	WEAPON_SPECIALTY, //	"LaserSpecialist",
	WEAPON_SPECIALTY, //	"LightACSpecialist",
	WEAPON_SPECIALTY, //	"MediumACSpecialist",
	WEAPON_SPECIALTY, //	"SRMSpecialist",
	WEAPON_SPECIALTY, //	"SmallArmsSpecialist",
	OTHER_SPECIALTY, //	"SensorProfileSpecialist",
	OTHER_SPECIALTY, //	"ToughnessSpecialist",

	CHASSIS_SPECIALTY, //	"MediumMechSpecialist",
	WEAPON_SPECIALTY, //	"PulseLaserSpecialist",
	WEAPON_SPECIALTY, //	"ERLaserSpecialist",
	WEAPON_SPECIALTY, //	"LRMSpecialist",
	OTHER_SPECIALTY, //	"Scout",
	OTHER_SPECIALTY, //	"LongJump",

	CHASSIS_SPECIALTY, //	"HevayMechSpecialist",
	WEAPON_SPECIALTY, //	"PPCSpecialist",
	WEAPON_SPECIALTY, //	"HeavyACSpecialist",
	RANGE_SPECIALTY, //	"ShortRangeSpecialist",
	RANGE_SPECIALTY, //	"MediumRangeSpecialist",
	RANGE_SPECIALTY, //	"LongRangeSpecialist",

	CHASSIS_SPECIALTY, //	"AssaultMechSpecialist",
	WEAPON_SPECIALTY, //	"GaussSpecialist",
	OTHER_SPECIALTY, //	"SharpShooter",
};
GameObjectPtr LastCoreAttackTarget = nullptr;

//***************************************************************************
// MISC routines
//***************************************************************************

void SeedRandomNumbers(void)
{
	srand((uint32_t)time(nullptr));
}

//---------------------------------------------------------------------------

int32_t
SignedRandomNumber(int32_t range)
{
	return (RandomNumber(range << 1) - range);
}

//---------------------------------------------------------------------------

int32_t
GetMissionTurn(void)
{
	return (turn);
}

//***************************************************************************
// TARGET PRIORITY routines
//***************************************************************************

PVOID
TargetPriorityList::operator new(size_t mySize)
{
	PVOID result = missionHeap->Malloc(mySize);
	return (result);
}

//---------------------------------------------------------------------------

void TargetPriorityList::operator delete(PVOID us)
{
	missionHeap->Free(us);
}

//---------------------------------------------------------------------------

void TargetPriorityList::init(void)
{
	size = 0;
}

//---------------------------------------------------------------------------

void TargetPriorityList::destroy(void)
{
	size = 0;
}

//---------------------------------------------------------------------------

int32_t
TargetPriorityList::insert(int32_t index, TargetPriorityPtr priority)
{
	if (size == MAX_TARGET_PRIORITIES)
		return (1);
	if (index == -1)
		index = size;
	if (index > size)
		return (2);
	if (index < size)
	{
		//------------------
		// shift 'em down...
		memmove(&list[index + 1], &list[index], (size - index) * sizeof(TargetPriority));
	}
	memcpy(&list[index], priority, sizeof(TargetPriority));
	size++;
	return (0);
}

//---------------------------------------------------------------------------

void TargetPriorityList::remove(int32_t index)
{
	if ((index > -1) && (index < size))
	{
		if (index < (size - 1))
		{
			//----------------
			// shift 'em up...
			memmove(&list[index], &list[index + 1], (size - index - 1) * sizeof(TargetPriority));
		}
		size--;
	}
};

//---------------------------------------------------------------------------

int32_t
TargetPriorityList::calcAction(std::unique_ptr<MechWarrior> pilot, GameObjectPtr target)
{
	if (target->isMover())
	{
		if (!pilot->getTeam() || pilot->getTeam()->isEnemy(target->getTeam()))
		{
			if (!target->isDisabled())
				return (TacticalOrderCode::attackobject);
			// else
			//	return(TacticalOrderCode::capture)
		}
	}
	else
	{
		switch (target->getObjectClass())
		{
		case BUILDING:
			if (target->getTeam() != pilot->getTeam())
			{
				if (target->getFlag(OBJECT_FLAG_CONTROLBUILDING))
					return (TacticalOrderCode::capture);
				return (TacticalOrderCode::attackobject);
			}
			break;
		case TURRET:
			break;
		}
	}
	return (TacticalOrderCode::none);
}

//---------------------------------------------------------------------------

int32_t
TargetPriorityList::calcTarget(
	std::unique_ptr<MechWarrior> pilot, Stuff::Vector3D location, int32_t contactCriteria, int32_t& action)
{
	if (!pilot->getTeam())
		return (0);
	for (size_t i = 0; i < size; i++)
	{
		switch (list[i].type)
		{
		case TARGET_PRIORITY_NONE:
			return (TARGET_PRIORITY_NONE);
			break;
		case TARGET_PRIORITY_GAMEOBJECT:
		{
			// param 0 = object WID
			// param 1 = engage radius
			// param 2 = contact criteria
			GameObjectPtr obj = nullptr;
			if (list[i].params[0])
				obj = (GameObjectPtr)ObjectManager->findByPartId(list[i].params[0]);
			//---------------------------------------------------
			// HACK! For now, we will not target unarmed movers...
			list[i].params[2] += CONTACT_CRITERIA_ARMED;
			/*
			if (!obj) {
				// select best gameobject that fits params
				if (list[i].params[2]) {
					// for now, just returns the closest contact...
					int32_t contactList[MAX_MOVERS];
					int32_t numContacts = CurObject->getContacts(contactList,
			list[i].params[2], CONTACT_SORT_DISTANCE); float smallestDistance =
			999999.0; for (size_t i = 0; i < numContacts; i++) { GameObjectPtr
			contact = ObjectManager->get(contactList[i]); float dist =
			pilot->getVehicle()->distanceFrom(contact->getPosition()); if (dist
			< smallestDistance) { obj = contact; smallestDistance = dist;
						}
					}
					}
				else {
					Fatal(0, " TargetPriorityList.calcTarget: bad criteria for
			mover target ");
				}
			}
			*/
			if (obj)
			{
				if (pilot->getVehicle()->distanceFrom(obj->getPosition()) < list[i].params[1])
				{
					if (list[i].params[2] && obj->isMover() && (!obj->isMarine() || (obj->getTeam() != Team::home)))
					{
						if (pilot->getTeam()->isContact(
								pilot->getVehicle(), (std::unique_ptr<Mover>)obj, list[i].params[2]))
						{
							action = calcAction(pilot, obj);
							return (obj->getWatchID());
						}
					}
					else
						return (obj->getWatchID());
				}
			}
		}
		break;
		case TARGET_PRIORITY_MOVER:
		{
			// param 0 = object WID
			// param 1 = engage radius
			// param 2 = contact criteria
			GameObjectPtr obj = nullptr;
			if (list[i].params[0])
				obj = (GameObjectPtr)ObjectManager->findByPartId(list[i].params[0]);
			//---------------------------------------------------
			// HACK! For now, we will not target unarmed movers...
			list[i].params[2] += CONTACT_CRITERIA_ARMED;
			if (!obj)
			{
				// select best gameobject that fits params
				if (list[i].params[2])
				{
					// for now, just returns the closest contact...
					int32_t contactList[MAX_MOVERS];
					int32_t numContacts = CurObject->getContacts(
						contactList, list[i].params[2], CONTACT_SORT_DISTANCE);
					float smallestDistance = 999999.0;
					for (size_t i = 0; i < numContacts; i++)
					{
						GameObjectPtr contact = ObjectManager->get(contactList[i]);
						if (!contact->isMarine() || (contact->getTeam() != Team::home))
						{
							float dist = pilot->getVehicle()->distanceFrom(contact->getPosition());
							if (dist < smallestDistance)
							{
								obj = contact;
								smallestDistance = dist;
							}
						}
					}
				}
				else
				{
					Fatal(0,
						" TargetPriorityList.calcTarget: bad criteria for "
						"mover target ");
				}
			}
			if (obj && obj->isMover())
			{
				if (pilot->getVehicle()->distanceFrom(obj->getPosition()) < list[i].params[1])
				{
					if (list[i].params[2])
					{
						if (pilot->getTeam()->isContact(
								pilot->getVehicle(), (std::unique_ptr<Mover>)obj, list[i].params[2]))
						{
							action = calcAction(pilot, obj);
							return (obj->getWatchID());
						}
					}
					else
						return (obj->getWatchID());
				}
			}
		}
		break;
		case TARGET_PRIORITY_BUILDING:
		{
			// param 0 = object WID
			// param 1 = engage radius
			// param 2 = if -1, looks for any enemy building. Else, specified
			// alignment.
			GameObjectPtr obj = nullptr;
			if (list[i].params[0])
				obj = (GameObjectPtr)ObjectManager->findByPartId(list[i].params[0]);
			if (!obj)
			{
				// select best gameobject that fits params
				if (list[i].params[2] == -1)
				{
					//----------------------------------------------
					// for now, return the closest enemy building...
					float smallestDistance = 999999.0;
					for (size_t j = 0; j < ObjectManager->getNumBuildings(); j++)
					{
						BuildingPtr building = ObjectManager->getBuilding(j);
						if (building->isEnemy(pilot->getTeam()))
							if (building->getObjectType()->getSubType() != BUILDING_SUBTYPE_WALL)
								if (!building->isDestroyed())
								{
									float dist =
										pilot->getVehicle()->distanceFrom(building->getPosition());
									if (dist < smallestDistance)
									{
										obj = building;
										smallestDistance = dist;
									}
								}
					}
				}
				else
				{
					Fatal(0,
						" TargetPriorityList.calcTarget: bad criteria for "
						"mover target ");
				}
			}
			if (obj)
			{
				if (pilot->getVehicle()->distanceFrom(obj->getPosition()) < list[i].params[1])
					return (obj->getWatchID());
			}
		}
		break;
		case TARGET_PRIORITY_CURTARGET:
		{
			// param 0 = disengage relative to point # (-1 for cur position)
			// param 1 = disengage radius
			// param 2 = contact criteria (0 for doesn't matter)
			GameObjectPtr target = nullptr;
			//------------------------------------------------------------------------
			// For now, the current order's target is considered the "current
			// target", even if the mover is moving while shooting. This can be
			// changed easily, if we want it to act differently...
			if (pilot->getCurTacOrder()->code != TacticalOrderCode::none)
			{
				target = ObjectManager->get(pilot->getCurTacOrder()->targetWID);
				if (target && target->isDisabled())
					if (!pilot->getCurTacOrder()->attackParams.obliterate)
						target = nullptr;
			}
			if (!target)
			{
				target = pilot->getLastTarget();
			}
			if (target)
			{
				Stuff::Vector3D disengageCenter;
				Stuff::Vector3D targetposition;
				if (list[i].params[0] == -1)
					disengageCenter = pilot->getVehicle()->getPosition();
				else
					disengageCenter =
						pilot->getVehicle()->getPosition(); // FOR NOW... should ref point list
				targetposition = target->getPosition();
				Stuff::Vector3D delta;
				delta.x = disengageCenter.x - targetposition.x;
				delta.y = disengageCenter.y - targetposition.y;
				delta.z = 0.0;
				float distanceToTarget = delta.GetLength() * metersPerWorldUnit;
				if (distanceToTarget < list[i].params[1])
				{
					if (list[i].params[2] && target->isMover())
					{
						if (pilot->getTeam()->isContact(
								pilot->getVehicle(), (std::unique_ptr<Mover>)target, list[i].params[2]))
						{
							action = calcAction(pilot, target);
							return (target->getWatchID());
						}
					}
					else
					{
						action = calcAction(pilot, target);
						return (target->getWatchID());
					}
				}
			}
		}
		break;
		case TARGET_PRIORITY_TURRET:
		{
			//------------------------------------------
			// For now, we'll pick the closest Turret...
			float shortestDistance = 999999.0;
			GameObjectPtr closestTurret = nullptr;
			for (size_t i = 0; i < ObjectManager->getNumTurrets(); i++)
			{
				GameObjectPtr turret = (GameObjectPtr)ObjectManager->getTurret(i);
				if (!turret->getTeam()->isFriendly(
						pilot->getTeam()) /* && turret->isCaptureable(pilot->getTeam()->getId())*/)
				{
					float distance = pilot->getVehicle()->distanceFrom(turret->getPosition());
					if (distance < shortestDistance)
					{
						shortestDistance = distance;
						closestTurret = turret;
					}
				}
			}
			if (closestTurret)
			{
				if (pilot->getVehicle()->distanceFrom(closestTurret->getPosition()) < list[i].params[1])
				{
					action = calcAction(pilot, closestTurret);
					return (closestTurret->getWatchID());
				}
			}
			return (0);
		}
		break;
		case TARGET_PRIORITY_TURRET_CONTROL:
		{
			//--------------------------------------------------
			// For now, we'll pick the closest Turret Control...
			// WE CANNOT ASSUME CAPTURE OR ATTACK HERE! FIX BELOW...
			float shortestDistance = 999999.0;
			GameObjectPtr closestControl = nullptr;
			for (size_t i = 0; i < ObjectManager->getNumTurretControls(); i++)
			{
				GameObjectPtr turretControl = (GameObjectPtr)ObjectManager->getTurretControl(i);
				if (!turretControl->getTeam()->isFriendly(pilot->getTeam()) && turretControl->isCaptureable(pilot->getTeam()->getId()))
				{
					float distance =
						pilot->getVehicle()->distanceFrom(turretControl->getPosition());
					if (distance < shortestDistance)
					{
						shortestDistance = distance;
						closestControl = turretControl;
					}
				}
			}
			if (closestControl)
			{
				action = calcAction(pilot, closestControl);
				return (closestControl->getWatchID());
			}
		}
		break;
		case TARGET_PRIORITY_GATE:
			break;
		case TARGET_PRIORITY_GATE_CONTROL:
		{
			//--------------------------------------------------
			// For now, we'll pick the closest Turret Control...
			// WE CANNOT ASSUME CAPTURE OR ATTACK HERE! FIX BELOW...
			float shortestDistance = 999999.0;
			GameObjectPtr closestControl = nullptr;
			for (size_t i = 0; i < ObjectManager->getNumGateControls(); i++)
			{
				GameObjectPtr gateControl = (GameObjectPtr)ObjectManager->getGateControl(i);
				if (!gateControl->getTeam()->isFriendly(pilot->getTeam()) && gateControl->isCaptureable(pilot->getTeam()->getId()))
				{
					float distance = pilot->getVehicle()->distanceFrom(gateControl->getPosition());
					if (distance < shortestDistance)
					{
						shortestDistance = distance;
						closestControl = gateControl;
					}
				}
			}
			if (closestControl)
			{
				action = calcAction(pilot, closestControl);
				return (closestControl->getWatchID());
			}
		}
		break;
		case TARGET_PRIORITY_SALVAGE:
			break;
		case TARGET_PRIORITY_MECHBAY:
			break;
		case TARGET_PRIORITY_LOCATION:
			break;
		case TARGET_PRIORITY_EVENT_TARGETED:
			// for (size_t i = 0; i < alarm[curEventID].numTriggers; i++)
			//	paramList[i] = alarm[curEventID].trigger[i];
			break;
		case TARGET_PRIORITY_EVENT_HIT:
			// param 0 = selection criteria
			// param 1 = engage radius
			// param 2 = contact criteria
			/*				GameObjectPtr obj = nullptr;
							if (list[i].params[0])
								obj =
			   (GameObjectPtr)ObjectManager->get(list[i].params[0]); if (!obj) {
								// select best gameobject that fits params
								if (list[i].params[2]) {
									// for now, just returns the closest
			   contact... int32_t contactList[MAX_MOVERS]; int32_t numContacts =
			   CurObject->getContacts(contactList, list[i].params[2],
			   CONTACT_SORT_DISTANCE); float smallestDistance = 999999.0; for
			   (size_t i = 0; i < numContacts; i++) { GameObjectPtr contact =
			   ObjectManager->get(contactList[i]); float dist =
			   pilot->getVehicle()->distanceFrom(contact->getPosition()); if
			   (dist < smallestDistance) { obj = contact; smallestDistance =
			   dist;
										}
									}
									}
								else {
									Fatal(0, " TargetPriorityList.calcTarget:
			   bad criteria for mover target ");
								}
							}
							if (obj) {
								if
			   (pilot->getVehicle()->distanceFrom(obj->getPosition()) <
			   list[i].params[1]) { if (list[i].params[2] && obj->isMover()) {
										if
			   (pilot->getTeam()->isContact(pilot->getVehicle(), (std::unique_ptr<Mover>)obj,
			   list[i].params[2])) { action = calcAction(pilot, obj);
											return(obj->getWatchID());
										}
										}
									else
										return(obj->getWatchID());
								}
							}
							}
							*/
			break;
		case TARGET_PRIORITY_EVENT_DAMAGED:
			break;
		case TARGET_PRIORITY_EVENT_MATE_DIED:
			break;
		case TARGET_PRIORITY_EVENT_FRIENDLY_DISABLED:
			break;
		case TARGET_PRIORITY_EVENT_FRIENDLY_DESTROYED:
			break;
		case TARGET_PRIORITY_EVENT_FRIENDLY_WITHDRAWS:
			break;
		case TARGET_PRIORITY_SKIP:
			break;
		}
	}
	return (0);
}

//***************************************************************************
// MECHWARRIOR class routines
//***************************************************************************

PVOID
MechWarrior::operator new(size_t mySize)
{
	PVOID result = missionHeap->Malloc(mySize);
	return (result);
}

//---------------------------------------------------------------------------

void MechWarrior::operator delete(PVOID us)
{
	missionHeap->Free(us);
}

//---------------------------------------------------------------------------

void MechWarrior::lobotomy(void)
{
	delete brain;
	brain = nullptr;
	//-----------------------------------------
	// Set up the pilot alarm callback table...
	for (size_t i = 0; i < NUM_PILOT_ALARMS; i++)
		brainAlarmCallback[i] = nullptr;
}

//---------------------------------------------------------------------------

void MechWarrior::init(bool create)
{
	used = false;
	name[0] = nullptr;
	callsign[0] = nullptr;
	videoStr[0] = nullptr;
	audioStr[0] = nullptr;
	brainStr[0] = nullptr;
	photoIndex = 0;
	rank = WarriorRank::green;
	for (size_t i = 0; i < Skill::numberofskills; i++)
	{
		for (size_t j = 0; j < NUM_COMBAT_STATS; j++)
		{
			numSkillUses[i][j] = 0;
			numSkillSuccesses[i][j] = 0;
		}
		skillRank[i] = 0.0;
		skillPoints[i] = 0.0;
		originalSkills[Skill::numberofskills] = 0;
		startingSkills[Skill::numberofskills] = 0;
	}
	professionalism = 40;
	professionalismModifier = 0;
	decorum = 40;
	decorumModifier = 0;
	aggressiveness = 40;
	courage = 40;
	baseCourage = courage;
	wounds = 0.0;
	health = 0.0;
	status = WARRIOR_STATUS_NORMAL;
	escapesThruEjection = false;
	radioLog.lastMessage = -1;
	radioLog.lastUnderFire = -1000.0;
	radioLog.weaponsIneffective = false;
	radioLog.weaponsOut = false;
	radioLog.lastContact = -1000.0;
	notMineYet = false;
	teamId = 0;
	vehicleWID = 0;
	numKilled = 0;
	memset(killed, 0, sizeof(GameObject*) * MAX_MOVERS / 3);
	for (i = 0; i < NUM_VEHICLE_CLASSES; i++)
		for (size_t j = 0; j < NUM_COMBAT_STATS; j++)
			numMechKills[i][j] = 0;
	for (i = 0; i < NUM_PHYSICAL_ATTACKS; i++)
		for (size_t j = 0; j < NUM_COMBAT_STATS; j++)
			numPhysicalAttacks[i][j] = 0;
	for (i = 0; i < NUM_SPECIALTY_SKILLS; i++)
		specialtySkills[i] = false;
	descID = 0;
	nameIndex = 0;
	keepMoving = false;
	timeOfLastOrders = -1.0;
	numAttackers = 0;
	attackRadius = DefaultAttackRadius;
	//---------------------
	// ABL Brain and Memory
	for (i = 0; i < NUM_MEMORY_CELLS; i++)
		memory[i].integer = 0;
	brain = nullptr;
	for (i = 0; i < NUM_PILOT_ALARMS; i++)
		brainAlarmCallback[i] = nullptr;
	for (i = 0; i < NUM_PILOT_DEBUG_STRINGS; i++)
		debugStrings[i][0] = nullptr;
	brainUpdate = (float)(numWarriors % 30) * 0.2;
	combatUpdate = (float)(numWarriors % 15) * 0.1;
	movementUpdate = (float)(numWarriors % 15) * 0.2;
	for (size_t w = 0; w < MAX_WEAPONS_PER_MOVER; w++)
		weaponsStatus[w] = 0;
	weaponsStatusResult = WEAPONS_STATUS_NO_TARGET;
	useGoalPlan = false;
	mainGoalAction = GOAL_ACTION_NONE;
	mainGoalObjectWID = 0;
	mainGoalLocation.Zero();
	mainGoalControlRadius = -1.0;
	lastGoalPathSize = 0;
	newTacOrderReceived[ORDERSTATE_GENERAL] = false;
	newTacOrderReceived[ORDERSTATE_PLAYER] = false;
	newTacOrderReceived[ORDERSTATE_ALARM] = false;
	tacOrder[ORDERSTATE_GENERAL].init();
	tacOrder[ORDERSTATE_PLAYER].init();
	tacOrder[ORDERSTATE_ALARM].init();
	lastTacOrder.init();
	curTacOrder.init();
	clearAlarms();
	clearAlarmsHistory();
	alarmPriority = 0;
	tacOrderQueue = nullptr;
	enableTacOrderQueue();
	curPlayerOrderFromQueue = false;
	tacOrderQueueLocked = false;
	tacOrderQueueExecuting = false;
	tacOrderQueueLooping = false;
	numTacOrdersQueued = 0;
	tacOrderQueueIndex = 0;
	nextTacOrderId = 0;
	lastTacOrderId = -1;
	coreScanTargetWID = 0;
	coreAttackTargetWID = 0;
	coreMoveTargetWID = 0;
	targetPriorityList.init();
	brainState = 0;
	willHelp = true;
	if (create)
		moveOrders.init();
	attackOrders.init();
	situationOrders.init();
	lastTargetWID = 0;
	lastTargetTime = -1.0;
	lastTargetObliterate = false;
	lastTargetFriendly = false;
	orderState = ORDERSTATE_GENERAL;
	movePathRequest = nullptr;
	if (create)
	{
		for (i = 0; i < 2; i++)
		{
			moveOrders.path[i] = new MovePath;
			if (!moveOrders.path[i])
				Fatal(0, " No RAM for warrior path ");
		}
	}
	debugFlags = WARRIOR_DEBUG_FLAG_NONE;
	radio = nullptr;
	isPlayingMsg = false;
	oldPilot = 0;
	//------------
	// Static vars
	if (!sortList)
	{
		sortList = new SortList;
		if (!sortList)
			Fatal(0, " Unable to create Warrior::sortList ");
		sortList->init(100);
	}
	numWarriors++;
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::init(FitIniFile* warriorFile)
{
	//----------------------------------------------------------------------
	// If we want to, we could make the following tables part of an ini file
	// that defines which skills, etc. are being used in the game system.
	// This way, non-programming designers could modify which skills are
	// being used (and so on), without having to change the underlying
	// system code. The global defines, Skill::numberofskills, etc. would also have to
	// a part of the ini file...
	//------------------
	// Main Info section
	int32_t result = warriorFile->seekBlock("General");
	if (result != NO_ERROR)
		return (result);
	result = warriorFile->readIdLong("DescIndex", descID);
	if (result != NO_ERROR)
		descID = -1;
	cLoadString(descID, name, MAXLEN_PILOT_NAME - 1);
	result = warriorFile->readIdLong("NameIndex", nameIndex);
	if (result != NO_ERROR)
		nameIndex = -1;
	result = warriorFile->readIdBoolean("NotMineYet", notMineYet);
	if (result != NO_ERROR)
		notMineYet = false;
	result = warriorFile->readIdLong("PictureIndex", photoIndex);
	if (result != NO_ERROR)
		photoIndex = 0;
	strcpy(callsign, name);
	for (size_t i = 0; i < strlen(callsign); i++)
		CharUpper(callsign);
	result = warriorFile->readIdUChar("OldPilot", oldPilot);
	if (result != NO_ERROR)
		oldPilot = 0;
	result = warriorFile->readIdString("pilotAudio", audioStr, MAXLEN_PILOT_AUDIO - 1);
	if (result != NO_ERROR)
	{
		radio = nullptr; // No pilot Audio, so no radio.
	}
	else
	{
		result = warriorFile->readIdString("pilotVideo", videoStr, MAXLEN_PILOT_VIDEO - 1);
		if (result != NO_ERROR)
		{
			videoStr[0] = '\0';
		}
		radio = new Radio;
		result = radio->init(audioStr, 102400,
			videoStr); // Size of radio heap should be data driven?
		if (result != NO_ERROR)
		{
			delete radio; // Radio couldn't start, so no radio.
			radio = nullptr;
		}
	}
	result = warriorFile->readIdLong("PaintScheme", paintScheme);
	if (result != NO_ERROR)
		paintScheme = -1; // This means mech is its normal color.
	result = warriorFile->seekBlock("PersonalityTraits");
	if (result != NO_ERROR)
		return (result);
	result = warriorFile->readIdChar("Professionalism", professionalism);
	if (result != NO_ERROR)
		return (result);
	result = warriorFile->readIdChar("Decorum", decorum);
	if (result != NO_ERROR)
		return (result);
	result = warriorFile->readIdChar("Aggressiveness", aggressiveness);
	if (result != NO_ERROR)
		return (result);
	result = warriorFile->readIdChar("Courage", courage);
	if (result != NO_ERROR)
		return (result);
	baseCourage = courage;
	result = warriorFile->seekBlock("Skills");
	if (result != NO_ERROR)
		return (result);
	i = 0;
	for (i = 0; i < Skill::numberofskills; i++)
	{
		result = warriorFile->readIdChar(SkillsTable[i], skills[i]);
		if (result != NO_ERROR)
			return (result);
		skillRank[i] = skills[i];
	}
	result = warriorFile->seekBlock("SpecialtySkills");
	if (result == NO_ERROR)
	{
		for (i = 0; i < NUM_SPECIALTY_SKILLS; i++)
		{
			wchar_t tmpChar;
			result = warriorFile->readIdChar(SpecialtySkillsTable[i], tmpChar);
			if (result == NO_ERROR)
				specialtySkills[i] = (tmpChar == 1);
		}
	}
	result = warriorFile->seekBlock("OriginalSkills");
	if (result == NO_ERROR)
	{
		for (i = 0; i < NUM_SPECIALTY_SKILLS; i++)
		{
			result = warriorFile->readIdChar(SkillsTable[i], originalSkills[i]);
			if (result != NO_ERROR)
				originalSkills[i] = skills[i];
		}
	}
	else
	{
		for (i = 0; i < Skill::numberofskills; i++)
		{
			originalSkills[i] = skills[i];
		}
	}
	result = warriorFile->seekBlock("LatestSkills");
	if (result == NO_ERROR)
	{
		for (i = 0; i < Skill::numberofskills; i++)
		{
			result = warriorFile->readIdChar(SkillsTable[i], startingSkills[i]);
			if (result != NO_ERROR)
				startingSkills[i] = skills[i];
		}
	}
	else
	{
		for (i = 0; i < Skill::numberofskills; i++)
		{
			startingSkills[i] = skills[i];
		}
	}
	result = warriorFile->seekBlock("SkillPoints");
	if (result != NO_ERROR)
	{
		for (size_t i = 0; i < Skill::numberofskills; i++)
		{
			skillPoints[i] = 0.0;
		}
	}
	else
	{
		for (size_t i = 0; i < Skill::numberofskills; i++)
		{
			result = warriorFile->readIdFloat(SkillsTable[i], skillPoints[i]);
			if (result != NO_ERROR)
				skillPoints[i] = 0.0;
		}
	}
	//-----------------------
	// Calc warrior's rank...
	calcRank();
	result = warriorFile->seekBlock("Status");
	if (result != NO_ERROR)
		return (result);
	wchar_t wnds;
	result = warriorFile->readIdChar("Wounds", wnds);
	if (result != NO_ERROR)
		return (result);
	wounds = (float)wnds;
	int32_t roll = RandomNumber(100);
	escapesThruEjection = ((roll <= (getSkill(Skill::piloting) + 30)) && (roll < 95));
	if (MPlayer || isToughnessSpecialist())
		escapesThruEjection = true;
	enableTacOrderQueue();
	return NO_ERROR;
}

#define OLD_ACKNOWLEDGE_MSG 57
//---------------------------------------------------------------------------

void MoveOrders::init(void)
{
	time = scenarioTime;
	origin = OrderOriginType::commander;
	speedType = MoveSpeedType::moderate;
	speedVelocity = 0.0;
	speedState = MECH_STATE_WALKING;
	speedThrottle = 100;
	/*
		moveOrders[which].zone.angle[0] = 45.0;
		moveOrders[which].zone.angle[1] = 90.0;
		moveOrders[which].zone.range[0] = 60.0;
		moveOrders[which].zone.range[1] = 90.0;
		moveOrders[which].pattern = MovePatternType::direct;
	*/
	goalType = MOVEGOAL_NONE;
	goalObjectWID = 0;
	goalObjectPosition.Zero();
	//------------------------------------------------
	// Since world coord should never reach -999999...
	goalLocation.x = -999999.0;
	goalLocation.y = -999999.0;
	goalLocation.z = -999999.0;
	nextUpdate = 0.0;
	newGoal = false;
	numWayPts = 0;
	curWayPt = 0;
	curWayDir = 0;
	pathType = MOVEPATH_UNDEFINED;
	originalGlobalGoal[0].x = -999999.0;
	originalGlobalGoal[0].y = -999999.0;
	originalGlobalGoal[0].z = -999999.0;
	originalGlobalGoal[1].x = -999999.0;
	originalGlobalGoal[1].y = -999999.0;
	originalGlobalGoal[1].z = -999999.0;
	globalGoalLocation.x = -666666.0;
	globalGoalLocation.y = -666666.0;
	globalGoalLocation.z = -666666.0;
	numGlobalSteps = 0;
	curGlobalStep = 0;
	path[0] = nullptr;
	path[1] = nullptr;
	timeOfLastStep = -1.0;
	moveState = MOVESTATE_FORWARD;
	moveStateGoal = MOVESTATE_FORWARD;
	twisting = false;
	yieldTime = -1.0;
	yieldState = 0;
	waitForPointTime = -1.0;
	run = false;
}

//---------------------------------------------------------------------------

void AttackOrders::init(void)
{
	time = scenarioTime;
	origin = OrderOriginType::commander;
	type = AttackType::none;
	targetWID = 0;
	targetpoint.Zero();
	aimLocation = -1;
	pursue = false;
	targetTime = 0.0;
}

//---------------------------------------------------------------------------

void SituationOrders::init(void)
{
	time = 0.0;
	mode = SituationType::normal;
	defFormation = 10.0;
	curFormation = 10.0;
	openFire = false;
	uponFireOnly = false;
	fireRange = -1.0;
	fireOdds = -1.0;
	guardObjectWID = 0;
}

//---------------------------------------------------------------------------

#define DEBUG_RADIO_MESSAGES 0

void MechWarrior::radioMessage(int32_t message, bool propogateIfMultiplayer)
{
	bool limitFrequency = true;
	if (message >= RADIO_MESSAGE_COUNT)
		return;
	if (radio && active() && (message != -1) && GetMissionTurn() > 0)
	{
		if (underHomeCommand())
		{
			switch (message)
			{
			case RADIO_WEAPONS_OUT:
				if (radioLog.weaponsOut)
					return;
				radioLog.weaponsOut = true;
				break;
			case RADIO_SENSOR_CONTACT:
				if (radioLog.lastContact > (scenarioTime - 15.0))
					return;
				radioLog.lastContact = scenarioTime;
				break;
			case RADIO_UNDER_ATTACK:
				if (radioLog.lastUnderFire > (scenarioTime - 20.0))
					return;
				radioLog.lastUnderFire = scenarioTime;
				break;
			}
			switch (message)
			{
			case RADIO_TARGET_ACK:
			case RADIO_MOVE_BLOCKED:
			case RADIO_ILLEGAL_ORDER:
				limitFrequency = false;
			}
			if (limitFrequency != 0 && radioLog.lastMessageType == message && radioLog.lastMessageTime > (scenarioTime - 10.0))
				return;
			radioLog.lastMessageTime = scenarioTime;
			radioLog.lastMessage = radio->playMessage((RadioMessageType)message);
			radioLog.lastMessageType = (RadioMessageType)message;
		}
		// According to Derek this never happens.  ONLY your pilots speak!!
		/*
		else if (MPlayer && MPlayer->isServer() && propogateIfMultiplayer)
		{
			getVehicle()->addRadioChunk(CHUNK_SEND, (uint8_t)message);
		}
		*/
	}
}

//---------------------------------------------------------------------------

void MechWarrior::clear(void)
{
	//----------------------------------------------------------------------
	// Before we do anything, make sure the warrior's orders are cleared and
	// pulled from any combat...
	clearCurTacOrder();
	//------------------------
	// Now, let's clear 'em...
	used = false;
	name[0] = nullptr;
	callsign[0] = nullptr;
	videoStr[0] = nullptr;
	audioStr[0] = nullptr;
	brainStr[0] = nullptr;
	rank = WarriorRank::green;
	for (size_t i = 0; i < Skill::numberofskills; i++)
	{
		for (size_t j = 0; j < NUM_COMBAT_STATS; j++)
		{
			numSkillUses[i][j] = 0;
			numSkillSuccesses[i][j] = 0;
		}
		skillRank[i] = 0.0;
		skillPoints[i] = 0.0;
		originalSkills[Skill::numberofskills] = 0;
		startingSkills[Skill::numberofskills] = 0;
	}
	professionalism = 40;
	professionalismModifier = 0;
	decorum = 40;
	decorumModifier = 0;
	aggressiveness = 40;
	courage = 40;
	baseCourage = courage;
	wounds = 0.0;
	health = 0.0;
	status = WARRIOR_STATUS_NORMAL;
	escapesThruEjection = false;
	radioLog.lastMessage = -1;
	radioLog.lastUnderFire = -1000.0;
	radioLog.weaponsIneffective = false;
	radioLog.weaponsOut = false;
	radioLog.lastContact = -1000.0;
	notMineYet = false;
	teamId = 0;
	vehicleWID = 0;
	numKilled = 0;
	memset(killed, 0, sizeof(GameObject*) * MAX_MOVERS / 3);
	for (i = 0; i < NUM_VEHICLE_CLASSES; i++)
		for (size_t j = 0; j < NUM_COMBAT_STATS; j++)
			numMechKills[i][j] = 0;
	for (i = 0; i < NUM_PHYSICAL_ATTACKS; i++)
		for (size_t j = 0; j < NUM_COMBAT_STATS; j++)
			numPhysicalAttacks[i][j] = 0;
	for (i = 0; i < NUM_SPECIALTY_SKILLS; i++)
		specialtySkills[i] = false;
	descID = 0;
	nameIndex = 0;
	timeOfLastOrders = -1.0;
	numAttackers = 0;
	attackRadius = DefaultAttackRadius;
	//---------------------
	// ABL Brain and Memory
	for (i = 0; i < NUM_MEMORY_CELLS; i++)
		memory[i].integer = 0;
	// OK if brain is nullptr. Like for Infantry!!
	if (brain)
		brain->init(-1);
	for (i = 0; i < NUM_PILOT_ALARMS; i++)
		brainAlarmCallback[i] = nullptr;
	for (i = 0; i < NUM_PILOT_DEBUG_STRINGS; i++)
		debugStrings[i][0] = nullptr;
	// brainUpdate = (float)(numWarriors % 30) * 0.2;
	// combatUpdate = (float)(numWarriors % 15) * 0.1;
	// movementUpdate = (float)(numWarriors % 15) * 0.2;
	for (size_t w = 0; w < MAX_WEAPONS_PER_MOVER; w++)
		weaponsStatus[w] = 0;
	weaponsStatusResult = WEAPONS_STATUS_NO_TARGET;
	newTacOrderReceived[ORDERSTATE_GENERAL] = false;
	newTacOrderReceived[ORDERSTATE_PLAYER] = false;
	newTacOrderReceived[ORDERSTATE_ALARM] = false;
	tacOrder[ORDERSTATE_GENERAL].init();
	tacOrder[ORDERSTATE_PLAYER].init();
	tacOrder[ORDERSTATE_ALARM].init();
	lastTacOrder.init();
	curTacOrder.init();
	for (size_t curAlarm = 0; curAlarm < NUM_PILOT_ALARMS; curAlarm++)
		clearAlarm(curAlarm);
	alarmPriority = 0;
	// tacOrderQueue = nullptr;
	enableTacOrderQueue();
	curPlayerOrderFromQueue = false;
	tacOrderQueueLocked = false;
	tacOrderQueueExecuting = false;
	tacOrderQueueLooping = false;
	numTacOrdersQueued = 0;
	tacOrderQueueIndex = 0;
	nextTacOrderId = 0;
	lastTacOrderId = -1;
	coreScanTargetWID = 0;
	coreAttackTargetWID = 0;
	coreMoveTargetWID = 0;
	targetPriorityList.init();
	brainState = 0;
	MovePathPtr path1 = moveOrders.path[0];
	path1->numSteps = path1->numStepsWhenNotPaused = 0;
	MovePathPtr path2 = moveOrders.path[1];
	path2->numSteps = path2->numStepsWhenNotPaused = 0;
	moveOrders.init();
	moveOrders.path[0] = path1;
	moveOrders.path[1] = path2;
	attackOrders.init();
	situationOrders.init();
	lastTargetWID = 0;
	lastTargetTime = -1.0;
	lastTargetObliterate = false;
	lastTargetFriendly = false;
	orderState = ORDERSTATE_GENERAL;
	movePathRequest = nullptr;
	clearMoveOrders();
	// for (i = 0; i < 2; i++) {
	//	moveOrders.path[i] = new MovePath;
	//	if (!moveOrders.path[i])
	//		Fatal(0, " No RAM for warrior path ");
	//}
	debugFlags = WARRIOR_DEBUG_FLAG_NONE;
	// radio = nullptr;
	oldPilot = 0;
	//------------
	// Static vars
	// if (!sortList) {
	//	sortList = new SortList;
	//	if (!sortList)
	//		Fatal(0, " Unable to create Warrior::sortList ");
	//	sortList->init(100);
	//}
}

//---------------------------------------------------------------------------

void MechWarrior::destroy(void)
{
	if (brain)
	{
		delete brain;
		brain = nullptr;
	}
	for (size_t i = 0; i < 2; i++)
		if (moveOrders.path[i])
		{
			delete moveOrders.path[i];
			moveOrders.path[i] = nullptr;
		}
	numWarriors--;
	if (numWarriors == 0)
	{
		delete sortList;
		sortList = nullptr;
	}
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::getAggressiveness(bool current)
{
	if (current && curTacOrder.isCombatOrder())
		return (aggressiveness + (100 - aggressiveness) / 2);
	else
		return (aggressiveness);
}

//---------------------------------------------------------------------------
// QUEUED PLAYER ORDERS routines
//---------------------------------------------------------------------------

bool MechWarrior::enableTacOrderQueue(void)
{
	if ((MAX_QUEUED_TACORDERS - TacOrderQueuePos) < MAX_QUEUED_TACORDERS_PER_WARRIOR)
		return (false);
	tacOrderQueue = &TacOrderQueue[TacOrderQueuePos];
	TacOrderQueuePos += MAX_QUEUED_TACORDERS_PER_WARRIOR;
	numTacOrdersQueued = 0;
	return (true);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::addQueuedTacOrder(TacticalOrder tacOrder)
{
	if (!tacOrderQueue)
	{
		gosASSERT(!"No TacOrder Queue");
	}
	if (numTacOrdersQueued == MAX_QUEUED_TACORDERS_PER_WARRIOR)
		return (2);
	tacOrderQueue[numTacOrdersQueued].moveMode = tacOrder.moveparams.wayPath.mode[0];
	if (tacOrder.moveparams.wayPath.mode[0] == TravelModeType::slow)
		tacOrderQueue[numTacOrdersQueued].marker =
			getWayPointMarker(tacOrder.getWayPoint(0), "WalkWayPoint");
	else if (tacOrder.moveparams.wayPath.mode[0] == TravelModeType::fast)
		tacOrderQueue[numTacOrdersQueued].marker =
			getWayPointMarker(tacOrder.getWayPoint(0), "RunWayPoint");
	else if (tacOrder.moveparams.wayPath.mode[0] == TravelModeType::jump)
		tacOrderQueue[numTacOrdersQueued].marker =
			getWayPointMarker(tacOrder.getWayPoint(0), "JumpWayPoint");
	tacOrderQueue[numTacOrdersQueued].point = tacOrder.getWayPoint(0);
	tacOrderQueue[numTacOrdersQueued].tactic = tacOrder.attackParams.tactic;
	tacOrderQueue[numTacOrdersQueued].packedData[0] = tacOrder.data[0];
	tacOrderQueue[numTacOrdersQueued].packedData[1] = tacOrder.data[1];
	numTacOrdersQueued++;
	if (numTacOrdersQueued == 1)
	{
		if (!newTacOrderReceived[ORDERSTATE_PLAYER])
		{
			if (!curPlayerOrderFromQueue || (curTacOrder.origin != OrderOriginType::player))
			{
				//--------------------------------------------------------------------
				// First queued order just given, so begin executing it
				// immediately... NO, don't do
				if (tacOrderQueueExecuting)
					executeTacOrderQueue();
			}
		}
	}
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::removeQueuedTacOrder(TacticalOrderPtr tacOrder)
{
	if (!tacOrderQueue)
		return (1);
	if (numTacOrdersQueued == 0)
		return (2);
	tacOrderQueueIndex = 0;
	tacOrder->data[0] = tacOrderQueue[tacOrderQueueIndex].packedData[0];
	tacOrder->data[1] = tacOrderQueue[tacOrderQueueIndex].packedData[1];
	Stuff::Vector3D point = tacOrderQueue[tacOrderQueueIndex].point;
	TacticType tactic = (TacticType)tacOrderQueue[tacOrderQueueIndex].tactic;
	if (tacOrderQueue[tacOrderQueueIndex].marker)
		delete tacOrderQueue[tacOrderQueueIndex].marker;
	tacOrderQueue[tacOrderQueueIndex].marker = 0;
	for (size_t i = 0; i < numTacOrdersQueued; i++)
		tacOrderQueue[i] = tacOrderQueue[i + 1];
	numTacOrdersQueued--;
	tacOrder->unpack();
	tacOrder->attackParams.tactic = tactic;
	tacOrder->setWayPoint(0, point);
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::getNextQueuedTacOrder(TacticalOrderPtr tacOrder)
{
	if (!tacOrderQueue)
		return (1);
	if (numTacOrdersQueued == 0)
		return (2);
	if (!tacOrderQueueLooping)
		return (removeQueuedTacOrder(tacOrder));
	tacOrder->data[0] = tacOrderQueue[tacOrderQueueIndex].packedData[0];
	tacOrder->data[1] = tacOrderQueue[tacOrderQueueIndex].packedData[1];
	tacOrder->unpack();
	tacOrder->setWayPoint(0, tacOrderQueue[tacOrderQueueIndex].point);
	tacOrder->attackParams.tactic = (TacticType)tacOrderQueue[tacOrderQueueIndex].tactic;
	tacOrderQueueIndex++;
	if (tacOrderQueueIndex >= numTacOrdersQueued)
		tacOrderQueueIndex = 0;
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::peekQueuedTacOrder(int32_t index, TacticalOrderPtr tacOrder)
{
	if (!tacOrderQueue)
		return (1);
	if (numTacOrdersQueued == 0)
		return (2);
	if (index == -1)
		index = numTacOrdersQueued - 1;
	tacOrder->data[0] = tacOrderQueue[index].packedData[0];
	tacOrder->data[1] = tacOrderQueue[index].packedData[1];
	Stuff::Vector3D point = tacOrderQueue[index].point;
	TacticType tactic = (TacticType)tacOrderQueue[index].tactic;
	int32_t tacOrderId = tacOrderQueue[index].id;
	tacOrder->unpack();
	tacOrder->setWayPoint(0, point);
	tacOrder->attackParams.tactic = tactic;
	tacOrder->setId(tacOrderId);
	return (0);
}

//---------------------------------------------------------------------------

void MechWarrior::clearTacOrderQueue(void)
{
	for (size_t i = 0; i < numTacOrdersQueued; i++)
		if (tacOrderQueue[i].marker)
		{
			delete tacOrderQueue[i].marker;
			tacOrderQueue[i].marker = 0;
		}
	numTacOrdersQueued = 0;
	tacOrderQueueIndex = 0;
	tacOrderQueueExecuting = false;
	tacOrderQueueLooping = false;
}

//---------------------------------------------------------------------------

void MechWarrior::executeTacOrderQueue(void)
{
	if (numTacOrdersQueued > 0)
	{
		tacOrderQueueExecuting = true;
		TacticalOrder tacOrder;
		if (getNextQueuedTacOrder(&tacOrder) == 0)
		{
			setPlayerTacOrder(tacOrder, true);
		}
	}
	else
		tacOrderQueueExecuting = false;
}

//---------------------------------------------------------------------------

void MechWarrior::lockTacOrderQueue(void)
{
	tacOrderQueueLocked = true;
}

//---------------------------------------------------------------------------

void MechWarrior::unlockTacOrderQueue(void)
{
	tacOrderQueueLocked = false;
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::getTacOrderQueue(QueuedTacOrderPtr list)
{
	int32_t numQueuedOrders = 0;
	if (curPlayerOrderFromQueue)
	{
		//--------------------------------------------------------------------
		// First queued order just given, so begin executing it immediately...
		list[0].point = tacOrder[ORDERSTATE_PLAYER].getWayPoint(0);
		list[0].packedData[0] = tacOrder[ORDERSTATE_PLAYER].data[0];
		list[0].packedData[1] = tacOrder[ORDERSTATE_PLAYER].data[1];
		numQueuedOrders = 1;
	}
	if (numTacOrdersQueued > 0)
	{
		for (size_t i = 0; i < numTacOrdersQueued; i++)
		{
			list[i + 1].point = tacOrderQueue[i].point;
			list[i + 1].tactic = tacOrderQueue[i].tactic;
			list[i + 1].packedData[0] = tacOrderQueue[i].packedData[0];
			list[i + 1].packedData[1] = tacOrderQueue[i].packedData[1];
			numQueuedOrders += numTacOrdersQueued;
		}
	}
	return (numQueuedOrders);
}

//---------------------------------------------------------------------------

int32_t
compareTacOrderId(int32_t id1, int32_t id2)
{
	if (id1 > 240)
	{
		if (id2 < 16)
			return (id1 - (id2 + 255));
		else
			return (id1 - id2);
	}
	else if (id1 < 16)
	{
		if (id2 > 240)
			return ((id1 + 255) - id2);
		else
			return (id1 - id2);
	}
	return (id1 - id2);
}

//---------------------------------------------------------------------------

void MechWarrior::updateClientOrderQueue(int32_t curTacOrderId)
{
	TacticalOrder nextTacOrder;
	int32_t peekResult = peekQueuedTacOrder(0, &nextTacOrder);
	//---------------------------------------------------------------
	// Basically, used in MPlayer games to make sure the ordersQueue
	// is current with what the server says the pilot has queued (for
	// UI purposes--e.g. displaying waypts on the client).
	if (curTacOrderId == 0)
	{
		if ((peekResult == 0) && (nextTacOrder.getId() == lastTacOrderId))
			removeQueuedTacOrder(&nextTacOrder);
		return;
	}
	lastTacOrderId = curTacOrderId;
	while (peekResult == 0)
	{
		int32_t compareResult = compareTacOrderId(nextTacOrder.getId(), curTacOrderId);
		if (compareResult < 0)
		{
			removeQueuedTacOrder(&nextTacOrder);
			/*
			wchar_t s[256];
			sprintf(s, "%s - removed tacorder, %d remaining\n", getName(),
			numTacOrdersQueued); OutputDebugString(s);
			*/
		}
		else
			return;
		peekResult = peekQueuedTacOrder(0, &nextTacOrder);
	}
}

//---------------------------------------------------------------------------
// QUEUED PLAYER ORDERS end
//---------------------------------------------------------------------------

void MechWarrior::setMainGoal(
	int32_t action, GameObjectPtr obj, Stuff::Vector3D* location, float range)
{
	mainGoalAction = action;
	if (obj)
		mainGoalObjectWID = obj->getWatchID();
	else
		mainGoalObjectWID = 0;
	if (location)
		mainGoalLocation = *location;
	else
		mainGoalLocation.Zero();
	mainGoalControlRadius = range;
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::getMainGoal(GameObjectPtr& obj, Stuff::Vector3D& location, float& radius)
{
	obj = nullptr;
	if (mainGoalObjectWID)
		obj = ObjectManager->getByWatchID(mainGoalObjectWID);
	location = mainGoalLocation;
	radius = mainGoalControlRadius;
	return (mainGoalAction);
}

//---------------------------------------------------------------------------

MoverGroupPtr
MechWarrior::getGroup(void)
{
#ifdef USE_GROUPS
	if (getVehicle())
		return (getVehicle()->getGroup());
#endif
	return (nullptr);
}

//---------------------------------------------------------------------------

std::unique_ptr<Mover>
MechWarrior::getPoint(void)
{
#ifdef USE_GROUPS
	if (getGroup())
		return (getGroup()->getPoint());
#endif
	return (nullptr);
}

//---------------------------------------------------------------------------

bool MechWarrior::onHomeTeam(void)
{
	return (getTeam() == Team::home);
}

//---------------------------------------------------------------------------

bool MechWarrior::underHomeCommand(void)
{
	std::unique_ptr<Mover> myVehicle = getVehicle();
	if (myVehicle)
		return (myVehicle->getCommanderId() == Commander::home->getId());
	return (false);
}

//---------------------------------------------------------------------------

TeamPtr
MechWarrior::getTeam(void)
{
	if (teamId > -1)
		return (Team::teams[teamId]);
	return (nullptr);
}

//---------------------------------------------------------------------------

CommanderPtr
MechWarrior::getCommander(void)
{
	std::unique_ptr<Mover> myVehicle = getVehicle();
	if (myVehicle && (myVehicle->commanderId > -1))
		return (Commander::commanders[myVehicle->commanderId]);
	return (nullptr);
}

//---------------------------------------------------------------------------

std::unique_ptr<Mover>
MechWarrior::getVehicle(void)
{
	//----------------------------------------------------
	// This assumes we have a Mover. Do we want to verify?
	// I say no, for speed's sake...
	return (std::unique_ptr<Mover>(ObjectManager->getByWatchID(vehicleWID)));
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::checkSkill(int32_t skillId, float factor)
{
	//------------------------------------------------------------------
	// Makes a skill check, and returns the success\fail magnitude.
	// If returned value is negative, the check failed. If non-negative,
	// the check succeeded.
	numSkillUses[skillId][COMBAT_STAT_MISSION]++;
	skillPoints[skillId] += SkillTry[skillId];
	int32_t successMargin = (int32_t)(getSkill(skillId) * factor) - RandomNumber(100) - 1;
	if (successMargin >= 0 && skillId != Skill::sensors)
	{
		numSkillSuccesses[skillId][COMBAT_STAT_MISSION]++;
		skillPoints[skillId] += SkillSuccess[skillId];
	}
	return (successMargin);
}

//---------------------------------------------------------------------------

bool MechWarrior::injure(float numWounds, bool checkEject)
{
	if (status != WARRIOR_STATUS_NORMAL)
		return (false);
	wounds += numWounds;
	if (wounds >= 6.0)
	{
		std::unique_ptr<Mover> myVehicle = getVehicle();
		Assert(myVehicle != nullptr, 0, " Pilot has no vehicle ");
		if (checkEject || escapesThruEjection)
		{
			numSkillUses[Skill::piloting][COMBAT_STAT_MISSION]++;
			skillPoints[Skill::piloting] += SkillTry[Skill::piloting];
			if (escapesThruEjection)
			{
				wounds = 3.0f;
				numSkillSuccesses[Skill::piloting][COMBAT_STAT_MISSION]++;
				skillPoints[Skill::piloting] += SkillSuccess[Skill::piloting];
				//---------------------------------
				// If we can't eject, we're dead...
				// We always succeed according to this function...
				// How about, we comment it out then!!
				// -fs
				if (!myVehicle->handleEjection())
					wounds = 6.0;
			}
		}
		//------------------------------------------
		// Warrior's vehicle immediately disabled...
		if ((wounds >= 6.0) && !escapesThruEjection)
		{
			radioMessage(RADIO_DEATH);
			status = WARRIOR_STATUS_DEAD;
			if (myVehicle)
				myVehicle->disable(PILOT_DEATH);
			if (getGroup())
				getGroup()->handleMateDestroyed(myVehicle->getWatchID());
			//---------------------------------------
			// Kill our radio. Dead Men Don't Talk...
			if (radio)
				radio->turnOff();
			return (true);
		}
	}
	else // DON'T play this if they die!
	{
		if (numWounds > 0)
			radioMessage(RADIO_PILOT_HURT);
	}
	return (false);
}

//---------------------------------------------------------------------------

void MechWarrior::eject(void)
{
	if ((status != WARRIOR_STATUS_NORMAL) && (status != WARRIOR_STATUS_WITHDRAWING))
		return;
	if (wounds < 6)
		wounds++; // Pilots take one point on ejection.  Only if won't kill
			// them?
	if (wounds >= 6) // Nope, eject 'em with one point and they die!
	{
		radioMessage(RADIO_DEATH);
		status = WARRIOR_STATUS_DEAD;
	}
	else
	{
		radioMessage(RADIO_EJECTING);
		status = WARRIOR_STATUS_EJECTED;
	}
	//------------------------------------------
	// Warrior's vehicle immediately disabled...
	std::unique_ptr<Mover> myVehicle = getVehicle();
	if (myVehicle)
	{
		myVehicle->disable(EJECTION_DEATH);
		if (getGroup())
			getGroup()->handleMateEjected(myVehicle->getWatchID());
	}
	//-------------------------------------------------
	// Kill our radio. Dead (missing) Men Don't Talk...
	if (radio)
		radio->turnOff();
}

//---------------------------------------------------------------------------

void MechWarrior::setTeam(TeamPtr team)
{
	if (team)
		teamId = team->getId();
	else
		teamId = -1;
	if (!MPlayer && (team != Team::home))
	{
		setUseGoalPlan(true);
		keepMoving = true;
	}
	else
	{
		setUseGoalPlan(false);
		keepMoving = false;
	}
}

//---------------------------------------------------------------------------

void MechWarrior::setVehicle(GameObjectPtr vehicle)
{
	if (vehicle)
	{
		if (!vehicle->isMover())
			Fatal(0, " bad vehicle type ");
		vehicleWID = vehicle->getWatchID();
	}
	else
		vehicleWID = 0;
	if (radio)
		radio->setOwner(this);
}

//---------------------------------------------------------------------------
void MechWarrior::setBrainName(std::wstring_view brainName)
{
	strncpy(brainStr, brainName, MAXLEN_PILOT_BRAIN - 1);
	brainStr[MAXLEN_PILOT_BRAIN - 1] = nullptr;
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::setBrain(int32_t brainHandle)
{
	int32_t brainErr = NO_ERROR;
	if (brain)
	{
		delete brain;
		brain = nullptr;
		//----------------------------------------
		// Clear the pilot alarm callback table...
		for (size_t i = 0; i < NUM_PILOT_ALARMS; i++)
			brainAlarmCallback[i] = nullptr;
	}
	if (brainHandle > -1)
	{
		warriorBrainHandle = brainHandle;
		brain = new ABLModule;
		brainErr = brain->init(brainHandle);
		if (brainErr == NO_ERROR)
		{
			wchar_t brainName[500];
			sprintf(brainName, "Pilot %s", name);
			brain->setName(brainName);
			//-----------------------------------------
			// Set up the pilot alarm callback table...
			for (size_t i = 0; i < NUM_PILOT_ALARMS; i++)
				brainAlarmCallback[i] = brain->findFunction(pilotAlarmFunctionName[i], true);
		}
	}
	return (brainErr);
}

//---------------------------------------------------------------------------
extern int64_t MCTimeRunBrainUpdate;

int32_t
MechWarrior::runBrain(void)
{
	//	if (teamId  > -1)
	//		if (teamId != Team::home->getId())
	//		return(0);
	if (!brain)
		return (0);
	//----------------------------------
	// Param 1 is the ID of this mech...
	// ABLi_setIntegerParam(brainParams, 0, ((BattleMechPtr)owner)->ID);
	//-------------------------------------
	// Param 2 is the current game clock...
	// ABLi_setRealParam(brainParams, 1, scenarioTime);
	//--------------------------------------
	// Param 3 is the type of AI event (0 is
	// standard sequential update)...
	// ABLi_setIntegerParam(brainParams, 2, 0);
	//-----------------------
	// Now, call the brain...
	CurGroup = getGroup();
	CurObject = (GameObjectPtr)getVehicle();
	CurObjectClass = getVehicle()->getObjectClass();
	CurWarrior = this;
	CurContact = nullptr;
	curEventID = 0;
	curEventTrigger = 0;
	ModuleInfo moduleInfo;
	brain->getInfo(&moduleInfo);
	brain->execute();
#ifdef LAB_ONLY
	int64_t startTime = GetCycles();
#endif
	//--------------------------------------------------------------
	// Well, we'll just set it every frame so it doesn't screw up :)
	setUseGoalPlan(!MPlayer && (getCommander() != Commander::home));
	if (useGoalPlan)
	{
		TacticalOrder tacOrder;
		int32_t result = calcTacOrder(mainGoalAction, mainGoalObjectWID, mainGoalLocation,
			mainGoalControlRadius, 0, 1, 150.0, 300, tacOrder);
		if (result == 0)
		{
			if (tacOrder.code != TacticalOrderCode::none)
			{
				if (getCommander() == Commander::home)
					setPlayerTacOrder(tacOrder);
				else
					setGeneralTacOrder(tacOrder);
			}
		}
		else if (result > 0)
		{
			setMainGoal(GOAL_ACTION_NONE, nullptr, nullptr, -1.0);
			clearCurTacOrder();
		}
	}
#ifdef LAB_ONLY
	MCTimeRunBrainUpdate += (GetCycles() - startTime);
#endif
	CurGroup = nullptr;
	CurObject = nullptr;
	CurObjectClass = 0;
	CurWarrior = nullptr;
	CurContact = nullptr;
	//-------------------------------------------------
	// All brain modules should return an error code...
	int32_t brainErr = brain->getInteger();
	switch (brainErr)
	{
	case 0:
		//------------
		// No error...
		break;
	case 1:
		//-----------------------------------------------
		// General purpose error, for now, so bomb out...
		break;
	}
	clearAlarmsHistory();
	return (brainErr);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::getVehicleStatus(void)
{
	int32_t myVehicleStatus = -1;
	GameObjectPtr myVehicle = getVehicle();
	if (myVehicle)
		myVehicleStatus = myVehicle->getStatus();
	return (myVehicleStatus);
}

//---------------------------------------------------------------------------

void MechWarrior::updateAttackerStatus(uint32_t attackerWID, float time)
{
	int32_t curAttacker;
	for (curAttacker = 0; curAttacker < numAttackers; curAttacker++)
		if (attackers[curAttacker].WID == attackerWID)
			break;
	if (curAttacker == numAttackers)
	{
		if (numAttackers == MAX_ATTACKERS)
			return;
		attackers[numAttackers++].WID = attackerWID;
	}
	attackers[curAttacker].lastTime = time;
}

//---------------------------------------------------------------------------

GameObjectPtr
MechWarrior::calcTurretThreats(float threatRange, int32_t minThreat)
{
	if (minThreat < 1)
		minThreat = 1;
	int32_t turretThreat[256];
	int32_t turretControlThreat[256];
	for (size_t i = 0; i < ObjectManager->getNumTurretControls(); i++)
		turretControlThreat[i] = 0;
	for (i = 0; i < ObjectManager->getNumTurrets(); i++)
		turretThreat[i] = 0;
	uint32_t vehicleWID = getVehicle()->getWatchID();
	for (i = 0; i < ObjectManager->getNumTurrets(); i++)
	{
		Turret* turret = ObjectManager->getTurret(i);
		if (!turret->isDisabled() && (turret->targetWID == vehicleWID))
		{
			turretThreat[i] += turret->getThreatRating();
			if (turret->parent) // Pop-up turrets do NOT have parents!!
				turretControlThreat[((BuildingPtr)ObjectManager->getByWatchID(turret->parent))
										->listID] += turret->getThreatRating();
		}
	}
	int32_t biggestThreat = -1;
	for (i = 0; i < ObjectManager->getNumTurretControls(); i++)
	{
		BuildingPtr controlBuilding = ObjectManager->getTurretControl(i);
		float distance = getVehicle()->distanceFrom(controlBuilding->getPosition());
		if (distance > threatRange)
			turretControlThreat[i] = 0;
		if (controlBuilding->getTeam() == getTeam())
			turretControlThreat[i] = 0;
		if (controlBuilding->isDisabled())
			turretControlThreat[i] = 0;
		if (turretControlThreat[i] < minThreat)
			turretControlThreat[i] = 0;
		else
		{
			//---------------------------------------------
			// It's a threat, and big enough to consider...
			if (biggestThreat == -1)
				biggestThreat = i;
			else if (turretControlThreat[i] > turretControlThreat[biggestThreat])
				biggestThreat = i;
		}
	}
	if (biggestThreat > -1)
		return (ObjectManager->getTurretControl(biggestThreat));
	return (nullptr);
}

//---------------------------------------------------------------------------

AttackerRecPtr
MechWarrior::getAttackerInfo(uint32_t attackerWID)
{
	for (size_t i = 0; i < numAttackers; i++)
		if (attackers[i].WID == attackerWID)
			return (&attackers[i]);
	return (nullptr);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::getAttackers(uint32_t* attackerList, float seconds)
{
	int32_t count = 0;
	float earliestTime = scenarioTime - seconds;
	for (size_t i = 0; i < numAttackers; i++)
		if (attackers[i].lastTime >= earliestTime)
			attackerList[count++] = attackers[i].WID;
	return (count);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::setAttackTarget(GameObjectPtr object)
{
	if (object)
		attackOrders.targetWID = object->getWatchID();
	else
		attackOrders.targetWID = 0;
	attackOrders.targetTime = scenarioTime;
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

GameObjectPtr
MechWarrior::getLastTarget(void)
{
	GameObjectPtr target = ObjectManager->getByWatchID(lastTargetWID);
	if (target)
	{
		bool clearTarget = target->isDestroyed() || (target->isDisabled() && !lastTargetObliterate) || (target->isFriendly(getVehicle()) && !lastTargetFriendly);
		if (clearTarget)
		{
			setLastTarget(nullptr);
			lastTargetTime = -1.0;
			lastTargetObliterate = false;
			lastTargetFriendly = false;
			if (curTacOrder.isCombatOrder())
				clearCurTacOrder();
			return (nullptr);
		}
	}
	if (lastTargetConserveAmmo)
		curTacOrder.attackParams.type = AttackType::conservingammo;
	return (target);
}

//---------------------------------------------------------------------------

void MechWarrior::setLastTarget(GameObjectPtr target, bool obliterate, bool conserveAmmo)
{
	Assert(getVehicle() != nullptr, vehicleWID, " MechWarrior.setLastTarget: nullptr vehicle ");
	if (getCommander() == Commander::home)
	{
		if (lastTargetWID)
		{
			GameObjectPtr lastTarget = ObjectManager->getByWatchID(lastTargetWID);
			if (lastTarget)
				lastTarget->decrementAttackers();
		}
		if (target)
			target->incrementAttackers();
	}
	if (target)
	{
		lastTargetWID = target->getWatchID();
		lastTargetFriendly = target->isFriendly(getVehicle());
	}
	else
	{
		lastTargetWID = 0;
		lastTargetFriendly = false;
	}
	lastTargetTime = scenarioTime;
	lastTargetObliterate = obliterate;
	lastTargetConserveAmmo = conserveAmmo;
}

//---------------------------------------------------------------------------

GameObjectPtr
MechWarrior::getAttackTarget(void)
{
	return (ObjectManager->getByWatchID(attackOrders.targetWID));
}

//---------------------------------------------------------------------------

void MechWarrior::setCurrentTarget(GameObjectPtr target)
{
	setLastTarget(target);
}

//---------------------------------------------------------------------------

GameObjectPtr
MechWarrior::getAttackTargetPosition(Stuff::Vector3D& pos)
{
	GameObjectPtr object = ObjectManager->getByWatchID(attackOrders.targetWID);
	if (!object)
	{
		clearAttackOrders();
		return (nullptr);
	}
	pos = object->getPosition();
	return (object);
}

//---------------------------------------------------------------------------

void MechWarrior::clearAttackOrders(void)
{
	attackOrders.origin = OrderOriginType::commander;
	attackOrders.type = AttackType::none;
	attackOrders.targetWID = 0;
	attackOrders.aimLocation = -1;
	attackOrders.pursue = false;
	attackOrders.targetTime = -1.0;
}

//---------------------------------------------------------------------------

void MechWarrior::clearMoveOrders(void)
{
	setMoveGoal(MOVEGOAL_NONE, nullptr);
	setMoveWayPath(0);
	for (size_t i = 0; i < 2; i++)
		clearMovePath(i);
	setMoveState(MOVESTATE_FORWARD);
	setMoveStateGoal(MOVESTATE_FORWARD);
	setMoveYieldTime(-1.0);
	setMoveYieldState(0);
	setMoveWaitForPointTime(-1.0);
	setMoveTimeOfLastStep(-1.0);
	setMoveTwisting(false);
	setMoveGlobalPath(nullptr, 0);
	PathManager->remove(this);
}

//---------------------------------------------------------------------------

GameObjectPtr
MechWarrior::getSituationGuardObject(void)
{
	return (ObjectManager->getByWatchID(situationOrders.guardObjectWID));
}

//---------------------------------------------------------------------------

void MechWarrior::setSituationGuardObject(GameObjectWatchID objWID)
{
	situationOrders.guardObjectWID = objWID;
}

//---------------------------------------------------------------------------

uint32_t
MechWarrior::getSituationGuardObjectPartId(void)
{
	GameObjectPtr object = getSituationGuardObject();
	if (!object)
		return (0);
	return (object->getPartId());
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::setMoveGoal(uint32_t type, Stuff::Vector3D* location, GameObjectPtr obj)
{
	moveOrders.goalType = type;
	switch (type)
	{
	case MOVEGOAL_NONE:
		clearMoveGoal();
		break;
	case MOVEGOAL_LOCATION:
		if (location->z < -10.0)
			location->z = land->getTerrainElevation(*location);
		moveOrders.goalLocation = *location;
		moveOrders.goalObjectWID = 0;
		break;
	default:
		if (location->z < -10.0)
			location->z = land->getTerrainElevation(*location);
		moveOrders.goalLocation = *location;
		// if (!obj)
		//	obj = ObjectManager->get(type);
		moveOrders.goalObjectWID = type;
		break;
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

uint32_t
MechWarrior::getMoveGoal(Stuff::Vector3D* location, GameObjectPtr* obj)
{
	if (obj)
		*obj = ObjectManager->getByWatchID(moveOrders.goalObjectWID);
	if (location)
		*location = moveOrders.goalLocation;
	return (moveOrders.goalType);
}

//---------------------------------------------------------------------------

void MechWarrior::pausePath(void)
{
	if (moveOrders.path[0])
		moveOrders.path[0]->numSteps = 0;
}

//---------------------------------------------------------------------------

void MechWarrior::resumePath(void)
{
	if (moveOrders.path[0])
		moveOrders.path[0]->numSteps = moveOrders.path[0]->numStepsWhenNotPaused;
}

//---------------------------------------------------------------------------

void MechWarrior::rethinkPath(uint32_t strategy)
{
	Stuff::Vector3D nextWayPoint;
	bool haveWayPoint = getNextWayPoint(nextWayPoint, false);
	if (moveOrders.pathType == MOVEPATH_SIMPLE)
	{
		//------------------------------------------------
		// Since we're close to the end, just stop here...
		if (!haveWayPoint)
		{
			clearMoveOrders();
			if (curTacOrder.isMoveOrder() || curTacOrder.isWayPathOrder())
				clearCurTacOrder();
			triggerAlarm(PILOT_ALARM_NO_MOVEPATH, MOVEPATH_ERR_EARLY_STOP);
		}
		else
		{
			moveOrders.path[0]->numSteps = 0;
			//-----------------------------------------------
			// If you got here, you care about pathlocking...
			uint32_t params = MOVEPARAM_RECALC | MOVEPARAM_AVOID_PATHLOCKS | MOVEPARAM_FACE_TARGET;
			if (curTacOrder.moveparams.jump)
				params |= MOVEPARAM_JUMP;
			requestMovePath(curTacOrder.selectionindex, params, 2);
		}
	}
	else if (moveOrders.pathType == MOVEPATH_COMPLEX)
	{
		if (moveOrders.path[0]->globalStep == (moveOrders.numGlobalSteps - 1))
		{
			if (!haveWayPoint)
			{
				clearMoveOrders();
				if (curTacOrder.isMoveOrder() || curTacOrder.isWayPathOrder())
					clearCurTacOrder();
				triggerAlarm(PILOT_ALARM_NO_MOVEPATH, MOVEPATH_ERR_EARLY_STOP);
			}
		}
		else
		{
			//----------------------------------------------------------------
			// Not even on our last local path, so we may want to try again...
			/*			if (moveOrders[OrderType::current].yieldState == 5) {
							// We've tried enough. Just kill it...
							clearMoveOrders(OrderType::current);
							if (curTacOrder.isMoveOrder() ||
			   curTacOrder.isWayPathOrder()) clearCurTacOrder();
							triggerAlarm(PILOT_ALARM_NO_MOVEPATH,
			   MOVEPATH_ERR_EARLY_STOP);
							}
						else {
							moveOrders[OrderType::current].yieldState++;
			*/
			// pausePath();
			// setMoveYieldTime(scenarioTime + 1.5);
			moveOrders.path[0]->numSteps = 0;
			//-----------------------------------------------
			// If you got here, you care about pathlocking...
			uint32_t params = MOVEPARAM_RECALC | MOVEPARAM_AVOID_PATHLOCKS | MOVEPARAM_FACE_TARGET;
			if (curTacOrder.moveparams.jump)
				params |= MOVEPARAM_JUMP;
			requestMovePath(curTacOrder.selectionindex, params, 3);
			//			}
		}
	}
}

//---------------------------------------------------------------------------

float MechWarrior::getMoveDistanceLeft(void)
{
	float distanceLeft = 0.0;
	MovePathPtr curPath = moveOrders.path[0]; // getMovePath();
	if (curPath && (curPath->numStepsWhenNotPaused > 0))
	{
		distanceLeft = curPath->getDistanceLeft(getVehicle()->getPosition());
		if (moveOrders.pathType == MOVEPATH_COMPLEX)
		{
			//-------------------------------------------------------------
			// Ultimately, we should have a pre-calced distanceToGoal field
			// for each door-to-door link which we can use as an estimate.
			// For now, we can just assume 100.0 meters per area...
			distanceLeft += (float)(moveOrders.globalPath[curPath->globalStep]
										.costToGoal); // ((moveOrders.numGlobalSteps -
			// curPath->globalStep) * 50.0);
		}
	}
	return (distanceLeft);
}

//---------------------------------------------------------------------------

bool MechWarrior::isJumping(Stuff::Vector3D* jumpGoal)
{
	std::unique_ptr<Mover> myVehicle = getVehicle();
	if (myVehicle)
		return (myVehicle->isJumping(jumpGoal));
	return (false);
}

//---------------------------------------------------------------------------

MovePathPtr
MechWarrior::getMovePath(void)
{
	Assert((moveOrders.path[0] != nullptr) && (moveOrders.path[1] != nullptr), 0,
		" nullptr move paths ");
	if (moveOrders.path[0]->numStepsWhenNotPaused == 0)
	{
		clearMovePath(0);
		MovePathPtr temp = moveOrders.path[0];
		moveOrders.path[0] = moveOrders.path[1];
		moveOrders.path[1] = temp;
		if (moveOrders.path[0]->numStepsWhenNotPaused > 0)
		{
			/*			if (moveOrders.path[0]->numStepsWhenNotPaused > 1) {
							std::unique_ptr<Mover> myVehicle = getVehicle();
							int32_t lookAhead = moveOrders.path[0]->numStepsWhenNotPaused - 1;
							if (lookAhead > 3)
								lookAhead = 3;
							for (size_t i = lookAhead; i > 0; i--) {
								float distanceFromPt =
			   myVehicle->distanceFrom(moveOrders.path[0]->stepList[i].destination); if
			   (distanceFromPt <= MapCellDiagonal) break;
							}
							moveOrders.path[0]->curStep = i;
						}
			*/
			//-------------------------------------------------------------
			// We have a new move path, so set move orders appropriately...
			Stuff::Vector3D goal;
			GameObjectPtr goalObject;
			uint32_t goalType = getMoveGoal(&goal, &goalObject);
			if (goalType == MOVEGOAL_NONE)
			{
				// No more goal, so no more path:)
				// clearMovePath(OrderType::current, 0);
				// moveOrders[which].path[0]->numSteps = 0;
				moveOrders.path[0]->numStepsWhenNotPaused = 0;
				return (moveOrders.path[0]);
			}
			GameObjectPtr target = nullptr;
			if (goalType > 0)
			{
				//-----------------------
				// Target is an object...
				if (goalObject)
					target = goalObject;
				else
					target = ObjectManager->get(goalType);
				if (target)
					moveOrders.path[0]->target = target->getPosition();
				else
				{
					//--------------------------------------------------
					// If the object no longer exists, we'll stay put...
					// clearMovePath(OrderType::current, 0);
					moveOrders.path[0]->numStepsWhenNotPaused = 0;
					return (moveOrders.path[0]);
				}
			}
			if (isYielding())
			{
				moveOrders.yieldTime = scenarioTime + 1.5;
				// moveOrders.yieldState = 0;
				moveOrders.path[0]->numSteps = 0;
			}
			else if (isWaitingForPoint())
			{
				moveOrders.path[0]->numSteps = 0;
			}
			else
			{
				moveOrders.yieldTime = -1.0;
				moveOrders.yieldState = 0;
			}
			setMoveGoal(
				(target ? target->getWatchID() : MOVEGOAL_LOCATION), &(moveOrders.path[0]->goal));
			if (moveOrders.path[0]->globalStep == (moveOrders.numGlobalSteps - 1))
			{
				// if (moveOrders[which].path[0]->selectionindex > 0) {
				moveOrders.globalGoalLocation =
					moveOrders.path[0]
						->stepList[moveOrders.path[0]->numStepsWhenNotPaused - 1]
						.destination;
				curTacOrder.setWayPoint(0, moveOrders.globalGoalLocation);
				//}
			}
		}
	}
	return (moveOrders.path[0]);
}

//---------------------------------------------------------------------------
bool InitWayPath = true;

void MechWarrior::setMoveWayPath(WayPathPtr wayPath, bool patrol)
{
	if (wayPath)
	{
		for (size_t curPt = 0; curPt < wayPath->numPoints; curPt++)
		{
			moveOrders.wayPath[curPt].x = wayPath->points[curPt * 3];
			moveOrders.wayPath[curPt].y = wayPath->points[curPt * 3 + 1];
			moveOrders.wayPath[curPt].z = wayPath->points[curPt * 3 + 2];
		}
		moveOrders.numWayPts = wayPath->numPoints;
	}
	else
		moveOrders.numWayPts = 0;
	if (InitWayPath)
	{
		moveOrders.curWayPt = 0; // really points to next way point
		if (patrol)
			moveOrders.curWayDir = 1;
		else
			moveOrders.curWayDir = 0;
	}
}

//---------------------------------------------------------------------------

void MechWarrior::addMoveWayPoint(Stuff::Vector3D wayPt, bool patrol)
{
	moveOrders.wayPath[moveOrders.numWayPts] = wayPt;
	moveOrders.numWayPts++;
	if (moveOrders.numWayPts == 1)
	{
		if (patrol)
			moveOrders.curWayDir = 1;
		else
			moveOrders.curWayDir = 0;
	}
}

//---------------------------------------------------------------------------

void MechWarrior::setMoveGlobalPath(GlobalPathStepPtr path, int32_t numSteps)
{
	if (numSteps > MAX_GLOBAL_PATH)
		Fatal(0, " Global Path Too Long ");
	memcpy(moveOrders.globalPath, path, sizeof(GlobalPathStep) * numSteps);
	moveOrders.numGlobalSteps = numSteps;
	moveOrders.curGlobalStep = 0;
}

//---------------------------------------------------------------------------

void MechWarrior::requestMovePath(int32_t selectionindex, uint32_t moveparams, int32_t source)
{
	PathManager->request(this, selectionindex, moveparams, source);
}

//---------------------------------------------------------------------------
#ifdef LAB_ONLY
extern int64_t MCTimePath1Update;
extern int64_t MCTimePath2Update;
extern int64_t MCTimePath3Update;
extern int64_t MCTimePath4Update;
extern int64_t MCTimePath5Update;
#endif
int32_t
MechWarrior::calcMovePath(int32_t selectionindex, uint32_t moveparams)
{
	std::unique_ptr<Mover> myVehicle = getVehicle();
	bool flying = (myVehicle->getMoveLevel() > 0);
	Stuff::Vector3D jumpGoal;
	bool jumping = isJumping(&jumpGoal);
	Stuff::Vector3D start;
	if (jumping)
		start = jumpGoal;
	else
		start = myVehicle->getPosition();
	//--------------------------------------------------------------
	// If we're starting on a blocked cell, let's use our last valid
	// position as our start...
	//***NOTE***:
	// Does this hold up if we're on a cell that was just changed to impassable
	//(e.g. on a bridge tile just blown?)?!
	//**********
	if (!flying && !jumping)
	{
		int32_t cellRow, cellCol;
		myVehicle->getCellPosition(cellRow, cellCol);
		if (!GameMap->getPassable(cellRow, cellCol))
			start = myVehicle->getLastValidPosition();
	}
	int32_t posCellR, posCellC;
	land->worldToCell(start, posCellR, posCellC);
	int32_t startArea = GlobalMoveMap[myVehicle->getMoveLevel()]->calcArea(posCellR, posCellC);
	bool escapeTile = ((moveparams & MOVEPARAM_ESCAPE_TILE) != 0);
	Stuff::Vector3D goal;
	GameObjectPtr goalObject;
	uint32_t goalType = getMoveGoal(&goal, &goalObject);
	if (goalType == MOVEGOAL_NONE)
	{
		LastMoveCalcErr = MOVEPATH_ERR_NO_GOAL;
		triggerAlarm(PILOT_ALARM_NO_MOVEPATH, LastMoveCalcErr);
		return (LastMoveCalcErr);
	}
	GameObjectPtr target = nullptr;
	if (goalType > 0)
	{
		//-----------------------
		// Target is an object...
		if (goalObject)
			target = goalObject;
		else
			target = ObjectManager->get(goalType);
		if (!target)
		{
			//--------------------------------------------------
			// If the object no longer exists, we'll stay put...
			LastMoveCalcErr = MOVEPATH_ERR_GOALOBJ_DEAD;
			triggerAlarm(PILOT_ALARM_NO_MOVEPATH, LastMoveCalcErr);
			return (LastMoveCalcErr);
		}
	}
	int32_t pathNum = -1;
	if (moveparams & MOVEPARAM_INIT)
	{
		moveOrders.pathType = MOVEPATH_UNDEFINED;
		moveOrders.numGlobalSteps = 0;
		moveOrders.originalGlobalGoal[0] = goal;
		if ((moveOrders.moveStateGoal == MOVESTATE_PIVOT_FORWARD) || (moveOrders.moveStateGoal == MOVESTATE_PIVOT_REVERSE) || (moveOrders.moveStateGoal == MOVESTATE_PIVOT_TARGET))
		{
			moveOrders.moveState = MOVESTATE_FORWARD;
			moveOrders.moveStateGoal = MOVESTATE_FORWARD;
		}
		pathNum = 0;
	}
	bool yielding = isYielding();
	if (moveparams & MOVEPARAM_RECALC)
	{
		if (target)
		{
			Stuff::Vector3D pos = target->getPosition();
			setMoveGoal(target->getWatchID(), &pos, target);
		}
		else
			setMoveGoal(MOVEGOAL_LOCATION, &moveOrders.originalGlobalGoal[0]);
		goalType = getMoveGoal(&goal, &goalObject);
		// Assert(goal.x > -999000.0, 0, " Warrior:calcMovePath--Bad Original
		// Global Goal ");
		for (size_t i = 0; i < 2; i++)
			clearMovePath(i);
		moveOrders.pathType = MOVEPATH_UNDEFINED;
		moveOrders.numGlobalSteps = 0;
		moveOrders.moveState = MOVESTATE_FORWARD;
		moveOrders.moveStateGoal = MOVESTATE_FORWARD;
		pathNum = 0;
	}
	// Assert(goal.x > -999000.0, 0, " Warrior:calcMovePath--Bad Original Global
	// Goal 2");
	if (goal.x < -666000.0)
		return (LastMoveCalcErr = NO_ERROR);
	//-----------------------------------------
	// Are we calcing the current or next path?
	if (pathNum == -1)
	{
		if (moveOrders.path[0]->numStepsWhenNotPaused == 0)
			pathNum = 0;
		else
			pathNum = 1;
	}
	int64_t startTime = 0;
	//----------------------------------------------------------------------
	// Before we do anything else, check if we already have a global path...
	if (moveOrders.pathType == MOVEPATH_UNDEFINED /*numGlobalSteps == 0*/)
	{
		//---------------------------------------------------------
		// Must be a new move order, so let's first calc the actual
		// move goal...
		if (!escapeTile)
		{
			int32_t result = NO_ERROR;
			if ((myVehicle->getCommander() == Commander::home) && (curTacOrder.code != TacticalOrderCode::none) && (curTacOrder.origin == OrderOriginType::player))
				moveparams |= MOVEPARAM_PLAYER;
			startTime = GetCycles();
			if (myVehicle->moveRadius > 0.0)
				result =
					myVehicle->calcMoveGoal(target, myVehicle->moveCenter, myVehicle->moveRadius,
						goal, selectionindex, goal, lastGoalPathSize, lastGoalPath, moveparams);
			else
				result = myVehicle->calcMoveGoal(target, goal, -1.0, goal, selectionindex, goal,
					lastGoalPathSize, lastGoalPath, moveparams);
#ifdef LAB_ONLY
			MCTimePath1Update += (GetCycles() - startTime);
#endif
			if (result != NO_ERROR)
			{
				LastMoveCalcErr = MOVEPATH_ERR_NO_VALID_GOAL;
				triggerAlarm(PILOT_ALARM_NO_MOVEPATH, LastMoveCalcErr);
				return (LastMoveCalcErr);
			}
			//-------------------------------
			// Save the revised final goal...
		}
		moveOrders.originalGlobalGoal[1] = goal;
		if (escapeTile)
		{
			//------------------------------
			// Do stuff here, then return...
			Assert(pathNum == 0, pathNum, " Warrior.calcMovePath: escapePath should be pathNum 0 ");
			moveOrders.pathType = MOVEPATH_SIMPLE;
			myVehicle->updatePathLock(false);
			if ((curTacOrder.code == TacticalOrderCode::attackobject) && (curTacOrder.attackParams.method == AttackMethod::ramming))
				RamObjectWID = curTacOrder.targetWID;
			else
				RamObjectWID = 0;
			GameObjectPtr ramObject = ObjectManager->getByWatchID(RamObjectWID);
			if (ramObject && ramObject->isMover())
				((std::unique_ptr<Mover>)ramObject)->updatePathLock(false);
			if (myVehicle->getObjectClass() != ELEMENTAL)
				moveparams |= MOVEPARAM_AVOID_PATHLOCKS;
			Stuff::Vector3D escapeGoal;
			int32_t numSteps = myVehicle->calcEscapePath(moveOrders.path[pathNum], start, goal,
				nullptr, moveparams | MOVEPARAM_STATIONARY_MOVERS, escapeGoal);
			if (ramObject && ramObject->isMover())
				((std::unique_ptr<Mover>)ramObject)->updatePathLock(true);
			myVehicle->updatePathLock(true);
			RamObjectWID = 0;
			if (numSteps > 0)
			{
				//----------------------------------------
				// Found escape path from this BAD tile...
				moveOrders.path[pathNum]->numSteps = numSteps;
				moveOrders.path[pathNum]->numStepsWhenNotPaused = numSteps;
				moveOrders.globalGoalLocation =
					moveOrders.path[pathNum]->stepList[numSteps - 1].destination;
				curTacOrder.setWayPoint(0, moveOrders.globalGoalLocation);
				if (target)
					moveOrders.path[pathNum]->target = target->getPosition();
				setMoveGoal((target ? target->getWatchID() : MOVEGOAL_LOCATION), &goal);
				moveOrders.nextUpdate = scenarioTime + MovementUpdateFrequency;
				if (pathNum == 0)
				{
					if (yielding)
					{
						moveOrders.yieldTime = scenarioTime + 1.5;
						// moveOrders[OrderType::current].yieldState = 0;
						moveOrders.path[pathNum]->numSteps = 0;
					}
					else if (isWaitingForPoint())
					{
						moveOrders.path[pathNum]->numSteps = 0;
					}
					else
					{
						moveOrders.yieldTime = -1.0;
						moveOrders.yieldState = 0;
					}
				}
				LastMoveCalcErr = NO_ERROR;
			}
			else
			{
				// clearMoveOrders(OrderType::current);
				LastMoveCalcErr = MOVEPATH_ERR_LR_NOT_ENABLED;
				triggerAlarm(PILOT_ALARM_NO_MOVEPATH, LastMoveCalcErr);
			}
			return (LastMoveCalcErr);
		}
		bool faceTarget = (moveparams & MOVEPARAM_FACE_TARGET) != 0;
		if ((myVehicle->distanceFrom(goal) < Mover::marginOfError[1]) && !faceTarget)
		{
			clearMoveGoal();
			LastMoveCalcErr = MOVEPATH_ERR_ALREADY_THERE;
			triggerAlarm(PILOT_ALARM_NO_MOVEPATH, LastMoveCalcErr);
			return (LastMoveCalcErr);
		}
		//----------------------------------------------------------------------------
		// If the distance between the start and goal is within our "quickmove"
		// range, don't do any global pathfinding--do it all on one movemap. In
		// other words, make it a "simple" path. Also, if we're starting on a
		// blocked tile (burnt forest tile, for example), we'll want to do a
		// quickmove, as well...
		int32_t goalCellR, goalCellC;
		land->worldToCell(goal, goalCellR, goalCellC);
		bool doQuickMove = false;
		int32_t rowDiff = goalCellR - posCellR;
		if (rowDiff < 0)
			rowDiff *= -1;
		if (rowDiff <= SimpleMovePathRange)
		{
			int32_t colDiff = goalCellC - posCellC;
			if (colDiff < 0)
				colDiff *= -1;
			if (colDiff <= SimpleMovePathRange)
				doQuickMove = true;
		}
		bool startAreaOpen = (startArea >= 0) && (GlobalMoveMap[myVehicle->getMoveLevel()]->areas[startArea].open || GlobalMoveMap[myVehicle->getMoveLevel()]->areas[startArea].offMap);
		if (doQuickMove)
		{
			moveOrders.pathType = MOVEPATH_SIMPLE;
			myVehicle->updatePathLock(false);
			if ((curTacOrder.code == TacticalOrderCode::attackobject) && (curTacOrder.attackParams.method == AttackMethod::ramming))
				RamObjectWID = curTacOrder.targetWID;
			else
				RamObjectWID = 0;
			GameObjectPtr ramObject = ObjectManager->getByWatchID(RamObjectWID);
			if (ramObject && ramObject->isMover())
				((std::unique_ptr<Mover>)ramObject)->updatePathLock(false);
			if (myVehicle->getObjectClass() != ELEMENTAL)
				moveparams |= MOVEPARAM_AVOID_PATHLOCKS;
			startTime = GetCycles();
			int32_t numSteps = myVehicle->calcMovePath(moveOrders.path[pathNum], MOVEPATH_SIMPLE,
				start, goal, nullptr, moveparams | MOVEPARAM_STATIONARY_MOVERS);
#ifdef LAB_ONLY
			MCTimePath2Update += (GetCycles() - startTime);
#endif
			if (ramObject && ramObject->isMover())
				((std::unique_ptr<Mover>)ramObject)->updatePathLock(true);
			myVehicle->updatePathLock(true);
			RamObjectWID = 0;
			bool foundPath = (numSteps > 0);
			if ((numSteps > 0) && (selectionindex > 0))
			{
				//---------------------------------------------
				// We're supposed to stop early on this path...
				numSteps -= (selectionindex / GroupMoveTrailLen[1] * GroupMoveTrailLen[0]);
				if (numSteps <= 0)
				{
					//----------------------------------------------------
					// Found a path, but it's not far enough to even move!
					clearMoveOrders();
					LastMoveCalcErr = MOVEPATH_ERR_ALREADY_THERE;
					triggerAlarm(PILOT_ALARM_NO_MOVEPATH, LastMoveCalcErr);
					return (LastMoveCalcErr);
				}
			}
			if (numSteps > 0)
			{
				moveOrders.path[pathNum]->numSteps = numSteps;
				moveOrders.path[pathNum]->numStepsWhenNotPaused = numSteps;
				moveOrders.globalGoalLocation =
					moveOrders.path[pathNum]->stepList[numSteps - 1].destination;
				curTacOrder.setWayPoint(0, moveOrders.globalGoalLocation);
				if (target)
					moveOrders.path[pathNum]->target = target->getPosition();
				setMoveGoal((target ? target->getWatchID() : MOVEGOAL_LOCATION), &goal);
				moveOrders.nextUpdate = scenarioTime + MovementUpdateFrequency;
			}
			else if (!foundPath && (startArea == -1))
			{
				Assert(pathNum == 0, pathNum, " Warrior.calcMovePath: pathNum should be 0 ");
				//----------------------------------------------------------------------
				// Starting on a non-area (i.e. bad:) tile, so try to find a SR
				// escape route to a cell that has a LR route to our desired
				// goal (or is within the same area as the goal's area)...
				TacticalOrder alarmTacOrder;
				alarmTacOrder.init(OrderOriginType::self, TacticalOrderCode::movetopoint, false);
				alarmTacOrder.setWayPoint(0, goal);
				alarmTacOrder.moveparams.escapeTile = true;
				alarmTacOrder.moveparams.wait = false;
				alarmTacOrder.moveparams.wayPath.mode[0] =
					(getMoveRun() ? TravelModeType::fast : TravelModeType::slow);
				setAlarmTacOrder(alarmTacOrder, 255);
				// clearMoveOrders(OrderType::current);
				LastMoveCalcErr = MOVEPATH_ERR_ESCAPING_TILE;
				triggerAlarm(PILOT_ALARM_NO_MOVEPATH, LastMoveCalcErr);
				return (LastMoveCalcErr);
			}
			else if (!foundPath /* && longRangeEnabled*/)
				doQuickMove = false;
			else
			{
				// clearMoveOrders(OrderType::current);
				LastMoveCalcErr = MOVEPATH_ERR_LR_NOT_ENABLED;
				triggerAlarm(PILOT_ALARM_NO_MOVEPATH, LastMoveCalcErr);
				return (LastMoveCalcErr);
			}
		}
		if (!doQuickMove)
		{
			//---------------------------------------------------------------
			// The path must be a multi-part, or "complex", path that travels
			// across globalmap areas/doors...
			moveOrders.globalGoalLocation = goal;
			curTacOrder.setWayPoint(0, moveOrders.globalGoalLocation);
			int32_t goalArea =
				GlobalMoveMap[myVehicle->getMoveLevel()]->calcArea(goalCellR, goalCellC);
			int32_t numSteps = -1;
			if (startAreaOpen)
			{
				// if ((myVehicle->control.type == CONTROL_AI) && (getTeam() !=
				// Team::home))
				if (useGoalPlan)
					if (myVehicle->numFunctionalWeapons > 0)
						GlobalMoveMap[myVehicle->getMoveLevel()]->useClosedAreas = true;
				GlobalMoveMap[myVehicle->getMoveLevel()]->moverTeamID = myVehicle->getTeamId();
				startTime = GetCycles();
				if (GlobalMap::logEnabled)
				{
					static wchar_t s[256];
					sprintf(s, "[%.2f] calcPath: [%05d]%s", scenarioTime, myVehicle->getPartId(),
						myVehicle->getName());
					GlobalMap::writeLog(s);
				}
				numSteps = GlobalMoveMap[myVehicle->getMoveLevel()]->calcPath(startArea, goalArea,
					moveOrders.globalPath, posCellR, posCellC, goalCellR, goalCellC);
#ifdef LAB_ONLY
				MCTimePath3Update += (GetCycles() - startTime);
#endif
				GlobalMoveMap[myVehicle->getMoveLevel()]->useClosedAreas = false;
			}
			if (numSteps == -1)
			{
				Assert(pathNum == 0, pathNum, " Warrior.calcMovePath: pathNum should be 0 ");
				//----------------------------------------------------------------------
				// Starting on a non-area (i.e. bad:) tile, so try to find a SR
				// escape route to a cell that has a LR route to our desired
				// goal (or is within the same area as the goal's area)...
				TacticalOrder alarmTacOrder;
				alarmTacOrder.init(OrderOriginType::self, TacticalOrderCode::movetopoint, false);
				alarmTacOrder.setWayPoint(0, goal);
				alarmTacOrder.moveparams.escapeTile = true;
				alarmTacOrder.moveparams.wait = false;
				alarmTacOrder.moveparams.wayPath.mode[0] =
					(getMoveRun() ? TravelModeType::fast : TravelModeType::slow);
				setAlarmTacOrder(alarmTacOrder, 255);
				// clearMoveOrders(OrderType::current);
				LastMoveCalcErr = MOVEPATH_ERR_ESCAPING_TILE;
				triggerAlarm(PILOT_ALARM_NO_MOVEPATH, LastMoveCalcErr);
				return (LastMoveCalcErr);
				/*				//-------------------------------------------------------------------
								// Starting on a blocked tile. For now, we
				   simply go int16_t-range move
								// straight toward the goal...
								clearMoveOrders();
								LastMoveCalcErr = MOVEPATH_ERR_LR_START_BLOCKED;
								triggerAlarm(PILOT_ALARM_NO_MOVEPATH,
				   LastMoveCalcErr); return(LastMoveCalcErr);
				*/
			}
			else if (numSteps == 0)
			{
				clearMoveOrders();
				LastMoveCalcErr = MOVEPATH_ERR_LR_NO_PATH;
				triggerAlarm(PILOT_ALARM_NO_MOVEPATH, LastMoveCalcErr);
				if (moveparams & MOVEPARAM_RADIO_RESULT)
				{
					clearCurTacOrder();
					radioMessage(RADIO_MOVE_BLOCKED, true);
				}
				return (LastMoveCalcErr);
			}
			moveOrders.pathType = MOVEPATH_COMPLEX;
			moveOrders.numGlobalSteps = numSteps;
			moveOrders.curGlobalStep = 0;
		}
	}
	else if (moveOrders.pathType == MOVEPATH_COMPLEX)
	{
		moveOrders.curGlobalStep++;
	}
	if (moveOrders.pathType == MOVEPATH_COMPLEX)
	{
		int32_t curGlobalStep = moveOrders.curGlobalStep;
		int32_t numGlobalSteps = moveOrders.numGlobalSteps;
		if (curGlobalStep == numGlobalSteps)
		{
			//--------------------------------
			// We've finished the global path.
			// triggerAlarm(PILOT_ALARM_NO_MOVEPATH, LastMoveCalcErr);
			return (LastMoveCalcErr = NO_ERROR);
		}
		//--------------------------------------------------------------------------------
		// NEW: Now, our goal will be determined by our current global step in
		// our global path. Our moveOrders goal will contain our final goal of
		// the entire global path. Thus, our local (current) goal may not nec.
		// be the same as our moveOrders (final) goal...
		int32_t prevThruArea = -1;
		int32_t prevGoalDoor = -1;
		if ((curGlobalStep > 0) && (curGlobalStep < (numGlobalSteps - 1)))
		{
			GlobalPathStep prevGlobalStep = moveOrders.globalPath[curGlobalStep - 1];
			prevThruArea = moveOrders.globalPath[curGlobalStep - 1].thruArea;
			prevGoalDoor = moveOrders.globalPath[curGlobalStep - 1].goalDoor;
			land->cellToWorld(prevGlobalStep.goalCell[0], prevGlobalStep.goalCell[1], start);
		}
		GlobalPathStepPtr globalStep = &moveOrders.globalPath[curGlobalStep];
		GlobalMapDoorPtr globalDoor =
			&GlobalMoveMap[myVehicle->getMoveLevel()]->doors[globalStep->goalDoor];
		if (curGlobalStep < (numGlobalSteps - 1))
			if (!globalDoor->open)
			{
				LastMoveCalcErr = MOVEPATH_ERR_LR_DOOR_CLOSED;
				setMoveWayPath(nullptr, 0);
				setMoveTimeOfLastStep(scenarioTime);
				setMoveGlobalPath(nullptr, 0);
				PathManager->request(
					this, selectionindex, MOVEPARAM_RECALC + MOVEPARAM_FACE_TARGET, 19);
				triggerAlarm(PILOT_ALARM_NO_MOVEPATH, LastMoveCalcErr);
				return (LastMoveCalcErr);
			}
		int32_t numSteps = 0;
		if (curGlobalStep < (numGlobalSteps - 2))
		{
			clearMovePath(pathNum);
			myVehicle->updatePathLock(false);
			if ((curTacOrder.code == TacticalOrderCode::attackobject) && (curTacOrder.attackParams.method == AttackMethod::ramming))
				RamObjectWID = curTacOrder.targetWID;
			else
				RamObjectWID = 0;
			GameObjectPtr ramObject = ObjectManager->getByWatchID(RamObjectWID);
			if (ramObject && ramObject->isMover())
				((std::unique_ptr<Mover>)ramObject)->updatePathLock(false);
			if (myVehicle->getObjectClass() != ELEMENTAL)
				moveparams |= MOVEPARAM_AVOID_PATHLOCKS;
			startTime = GetCycles();
			int32_t thruArea[2] = {-1, -1};
			int32_t goalDoor = -1;
			if (curGlobalStep == (numGlobalSteps - 2))
			{
				// Second from last, so the actual goal cell instead of the door
				thruArea[0] = globalStep->thruArea;
				goalDoor = globalStep->goalDoor;
			}
			else
			{
				thruArea[0] = globalStep->thruArea;
				thruArea[1] = moveOrders.globalPath[curGlobalStep + 1].thruArea;
				goalDoor = moveOrders.globalPath[curGlobalStep + 1].goalDoor;
			}
			numSteps = myVehicle->calcMovePath(moveOrders.path[pathNum], start, thruArea, goalDoor,
				moveOrders.globalGoalLocation, &goal, globalStep->goalCell,
				moveparams | MOVEPARAM_STATIONARY_MOVERS);
#ifdef LAB_ONLY
			MCTimePath4Update += (GetCycles() - startTime);
#endif
			if (ramObject && ramObject->isMover())
				((std::unique_ptr<Mover>)ramObject)->updatePathLock(true);
			myVehicle->updatePathLock(true);
			RamObjectWID = 0;
		}
		else if (curGlobalStep == (numGlobalSteps - 2))
		{
			clearMovePath(pathNum);
			goal = moveOrders.originalGlobalGoal[1];
			myVehicle->updatePathLock(false);
			if ((curTacOrder.code == TacticalOrderCode::attackobject) && (curTacOrder.attackParams.method == AttackMethod::ramming))
				RamObjectWID = curTacOrder.targetWID;
			else
				RamObjectWID = 0;
			GameObjectPtr ramObject = ObjectManager->getByWatchID(RamObjectWID);
			if (ramObject && ramObject->isMover())
				((std::unique_ptr<Mover>)ramObject)->updatePathLock(false);
			if (myVehicle->getObjectClass() != ELEMENTAL)
				moveparams |= MOVEPARAM_AVOID_PATHLOCKS;
			startTime = GetCycles();
			numSteps = myVehicle->calcMovePath(moveOrders.path[pathNum], MOVEPATH_COMPLEX, start,
				goal, globalStep->goalCell, moveparams | MOVEPARAM_STATIONARY_MOVERS);
#ifdef LAB_ONLY
			MCTimePath5Update += (GetCycles() - startTime);
#endif
			if (ramObject && ramObject->isMover())
				((std::unique_ptr<Mover>)ramObject)->updatePathLock(true);
			myVehicle->updatePathLock(true);
			RamObjectWID = 0;
			if (numSteps > 0)
			{
				if (selectionindex > 0)
				{
					//---------------------------------------------
					// We're supposed to stop early on this path...
					numSteps -= (selectionindex / GroupMoveTrailLen[1] * GroupMoveTrailLen[0]);
					if (numSteps <= 0)
					{
						//----------------------------------------------------
						// Found a path, but it's not far enough to even move!
						clearMoveOrders();
						LastMoveCalcErr = MOVEPATH_ERR_ALREADY_THERE;
						triggerAlarm(PILOT_ALARM_NO_MOVEPATH, LastMoveCalcErr);
						return (LastMoveCalcErr);
					}
					if (pathNum == 0)
					{
						moveOrders.globalGoalLocation =
							moveOrders.path[pathNum]->stepList[numSteps - 1].destination;
						curTacOrder.setWayPoint(0, moveOrders.globalGoalLocation);
					}
				}
				curGlobalStep++;
			}
		}
		else if (curGlobalStep != (numGlobalSteps - 1))
			STOP((" Mechwarrior.calcMovePath: bad global step (%d/%d)", curGlobalStep,
				numGlobalSteps));
		if (numSteps > 0)
		{
			//			Assert(!isWaitingForPoint() || (pathNum != 0), 0, " fudge
			//");
			moveOrders.path[pathNum]->numSteps = numSteps;
			moveOrders.path[pathNum]->numStepsWhenNotPaused = numSteps;
			moveOrders.path[pathNum]->globalStep = curGlobalStep;
			if (pathNum == 0)
			{
				if (target)
					moveOrders.path[pathNum]->target = target->getPosition();
				setMoveGoal((target ? target->getWatchID() : MOVEGOAL_LOCATION), &goal);
			}
			// moveOrders[OrderType::current].nextUpdate  = scenarioTime +
			// MovementUpdateFrequency;
		}
		else if (curGlobalStep != (numGlobalSteps - 1))
		{
			// clearMoveOrders(OrderType::current);
			moveOrders.curGlobalStep--;
			if (numSteps == -999)
			{
				//---------------------------------------------------------------------------------------
				// Moving to a blocked door (all possible goal cells for the
				// door are currently blocked).
				LastMoveCalcErr = MOVEPATH_ERR_LR_DOOR_BLOCKED;
			}
			else
				LastMoveCalcErr = MOVEPATH_ERR_LR_NO_SR_PATH;
			triggerAlarm(PILOT_ALARM_NO_MOVEPATH, LastMoveCalcErr);
			return (LastMoveCalcErr);
		}
	}
	else if ((moveOrders.pathType != MOVEPATH_SIMPLE) && (moveOrders.pathType != MOVEPATH_UNDEFINED))
		Fatal(0, " Bad Move Path Type ");
	if (pathNum == 0)
	{
		if (yielding)
		{
			moveOrders.yieldTime = scenarioTime + 1.5;
			// moveOrders[OrderType::current].yieldState = 0;
			moveOrders.path[pathNum]->numSteps = 0;
		}
		else if (isWaitingForPoint())
		{
			moveOrders.path[pathNum]->numSteps = 0;
		}
		else
		{
			moveOrders.yieldTime = -1.0;
			moveOrders.yieldState = 0;
		}
	}
	return (LastMoveCalcErr = NO_ERROR);
}

//---------------------------------------------------------------------------

bool MechWarrior::getNextWayPoint(Stuff::Vector3D& nextPoint, bool incWayPoint)
{
	if (!curTacOrder.isWayPathOrder())
		return (false);
	// if curWayDir == 0, then the path is one-time only (loop through it once).
	// if curWayDir == 1, then loop through it forward. Once end is hit, loop
	// back... if curWayDir == -1, then loop through it backward. Once start is
	// hit, loop forward...
	if (moveOrders.curWayDir == 0)
	{
		//-------------------------------------------------
		// This is not a patrol path, so stop at the end...
		int32_t nextWayPt = moveOrders.curWayPt + 1;
		if (nextWayPt < moveOrders.numWayPts)
			nextPoint = moveOrders.wayPath[nextWayPt];
		if (incWayPoint)
		{
			// curTacOrder.moveparams.wayPath.points[0] = nextPoint.x;
			// curTacOrder.moveparams.wayPath.points[1] = nextPoint.y;
			// curTacOrder.moveparams.wayPath.points[2] = nextPoint.z;
			moveOrders.curWayPt = nextWayPt;
			if (curTacOrder.moveparams.wayPath.mode[nextWayPt] == TravelModeType::fast)
				setMoveRun(true);
			else
				setMoveRun(false);
			curTacOrder.moveparams.wayPath.curPoint = nextWayPt;
			if (nextWayPt == moveOrders.numWayPts)
				curTacOrder.setStage(2);
		}
		if (nextWayPt == moveOrders.numWayPts)
			return (false);
	}
	else
	{
		int32_t nextWayPt = 0;
		int32_t nextWayDir = moveOrders.curWayDir;
		if (moveOrders.curWayDir == 1)
		{
			if (moveOrders.curWayPt == (moveOrders.numWayPts - 1))
			{
				//------------------------------------------------------
				// Reached end, so turn around and start backtracking...
				nextWayDir = -1;
				nextWayPt = moveOrders.numWayPts - 2;
			}
			else
				//----------------------------------
				// Going forward to next waypoint...
				nextWayPt = moveOrders.curWayPt + 1;
		}
		else
		{
			if (moveOrders.curWayPt == 0)
			{
				nextWayDir = 1;
				nextWayPt = 1;
			}
			else
				nextWayPt = moveOrders.curWayPt - 1;
		}
		nextPoint = moveOrders.wayPath[nextWayPt];
		if (incWayPoint)
		{
			// curTacOrder.moveparams.wayPath.points[0] = nextPoint.x;
			// curTacOrder.moveparams.wayPath.points[1] = nextPoint.y;
			// curTacOrder.moveparams.wayPath.points[2] = nextPoint.z;
			moveOrders.curWayDir = nextWayDir;
			moveOrders.curWayPt = nextWayPt;
			if (curTacOrder.moveparams.wayPath.mode[nextWayPt] == TravelModeType::fast)
				setMoveRun(true);
			else
				setMoveRun(false);
			curTacOrder.moveparams.wayPath.curPoint = nextWayPt;
		}
	}
	return (true);
}

//---------------------------------------------------------------------------
// COMBAT DECISION TREE
//---------------------------------------------------------------------------

int32_t MechWarrior::calcWeaponsStatus(
	GameObjectPtr target,
	int32_t* weaponList,
	_In_ DirectX::XMFLOAT3& targetpoint)
{
	std::unique_ptr<Mover> myVehicle = getVehicle();
	if (!myVehicle->canFireWeapons())
		return (WEAPONS_STATUS_CANNOT_FIRE);
	Stuff::Vector3D targetposition;
	if (target)
		targetposition = target->getPosition();
	else if (targetpoint)
		targetposition = *targetpoint;
	else
		return (WEAPONS_STATUS_NO_TARGET);
	//------------------------
	// Am I even within range?
	float distanceToTarget = myVehicle->distanceFrom(targetposition);
	if ((distanceToTarget <= myVehicle->getMinFireRange()) || (distanceToTarget >= myVehicle->getMaxFireRange()))
		return (WEAPONS_STATUS_OUT_OF_RANGE);
	int32_t numWeaponsWithShot = 0;
	for (size_t curWeapon = 0; curWeapon < myVehicle->numWeapons; curWeapon++)
	{
		int32_t weaponIndex = myVehicle->numOther + curWeapon;
		if (!myVehicle->isWeaponReady(weaponIndex))
			weaponList[curWeapon] = WEAPON_STATUS_NOT_READY;
		else
		{
			int32_t curShots = myVehicle->getWeaponShots(weaponIndex);
			if (curShots < 1)
				weaponList[curWeapon] = WEAPON_STATUS_OUT_OF_AMMO;
			else
			{
				if (!myVehicle->weaponInRange(weaponIndex, distanceToTarget, MapCellDiagonal))
					weaponList[curWeapon] = WEAPON_STATUS_OUT_OF_RANGE;
				else
				{
					float relAngle = myVehicle->weaponLocked(weaponIndex, targetposition);
					float fireArc = myVehicle->getFireArc();
					bool lineOfFire = false;
					// ALWAYS do the SIMPLE calculation first.  If they have
					// indirect fire weapons, direct LOS is Still VALID!
					if (target)
						lineOfFire = myVehicle->lineOfSight(target);
					else
						lineOfFire = myVehicle->lineOfSight(targetposition);
					// Indirect fire weapons can fire if ANYONE can see target!
					if (!lineOfFire && myVehicle->getWeaponIndirectFire(weaponIndex))
					{
						if (!myVehicle->getTeam())
							lineOfFire = false;
						else if (target)
							lineOfFire = myVehicle->getTeam()->teamLineOfSight(
								target->getLOSPosition(), target->getAppearRadius());
						else
							lineOfFire = myVehicle->getTeam()->teamLineOfSight(
								targetposition, CELL_DISTANCE);
					}
					if (!lineOfFire || (relAngle < -fireArc) || (relAngle > fireArc))
						weaponList[curWeapon] = WEAPON_STATUS_NOT_LOCKED;
					else
					{
						int32_t aimLocation = -1;
						if (curTacOrder.isCombatOrder())
							aimLocation = curTacOrder.attackParams.aimLocation;
						float odds = myVehicle->calcAttackChance(target, aimLocation, scenarioTime,
							weaponIndex, 0.0, nullptr, targetpoint);
						if (odds < 1.0)
							weaponList[curWeapon] = WEAPON_STATUS_NO_CHANCE;
						else
						{
							weaponList[curWeapon] = odds;
							numWeaponsWithShot++;
						}
					}
				}
			}
		}
	}
	return (numWeaponsWithShot);
}

//---------------------------------------------------------------------------

void MechWarrior::printWeaponsStatus(std::wstring_view s)
{
	if (weaponsStatusResult == WEAPONS_STATUS_OUT_OF_RANGE)
		sprintf(s, "Out of Range");
	else if (getCurrentTarget() && !getVehicle()->lineOfSight(getCurrentTarget()))
		sprintf(s, "No Line-of-sight");
	else if (weaponsStatusResult == WEAPONS_STATUS_CANNOT_FIRE)
		sprintf(s, "Cannot Fire");
	else if (weaponsStatusResult == WEAPONS_STATUS_NO_TARGET)
		sprintf(s, "No Target");
	else
	{
		int32_t tally[6] = {0, 0, 0, 0, 0, 0};
		int32_t numWithChance = 0;
		for (size_t curWeapon = 0; curWeapon < getVehicle()->numWeapons; curWeapon++)
		{
			if (weaponsStatus[curWeapon] < 0)
				tally[weaponsStatus[curWeapon] + 6]++;
			else if (weaponsStatus[curWeapon] > 0)
				numWithChance++;
		}
		sprintf(s, "#=%02d(%02d),RD=%02d,RN=%02d,LK=%02d,CH=%02d,AM=%02d", getVehicle()->numWeapons,
			numWithChance, tally[5], tally[3], tally[2], tally[1], tally[4]);
	}
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::combatDecisionTree(void)
{
	int32_t result = -1;
	combatUpdate = scenarioTime + CombatUpdateFrequency;
	Stuff::Vector3D tPoint;
	std::unique_ptr<Mover> myVehicle = getVehicle();
	Assert(myVehicle != nullptr, 0, " Pilot has no vehicle! ");
	//----------------------------------
	// Let's see if we have ANY ammo...
	bool outOfAmmo = true;
	for (size_t i = 0; i < myVehicle->getNumAmmoTypes(); i++)
		if (myVehicle->getAmmoTypeTotal(i) > 0)
		{
			outOfAmmo = false;
			break;
		}
	GameObjectPtr target = getLastTarget(); // getAttackTarget(OrderType::current);
	//------------------------------------------------------------------------
	// Our target is our last target, if we had one (and we weren't explicitly
	// told to hold fire since then)...
	Stuff::Vector3D* targetpoint = nullptr;
	int32_t attackType = AttackType::destroy;
	int32_t aimLocation = -1;
	if (curTacOrder.isCombatOrder())
	{
		attackType = curTacOrder.attackParams.type;
		aimLocation = curTacOrder.attackParams.aimLocation;
		if (curTacOrder.code == TacticalOrderCode::attackpoint)
		{
			tPoint = getAttackTargetPoint();
			targetpoint = &tPoint;
			// MUST set the suppressionFire flag if we are host and this is from
			// client
			// AND we are an artilleryPiece WITHOUT suppressionFire set AND
			// OUR attackParams are method == AttackMethod::ranged, range =
			// FireRangeType::current AND pursue = false.  Complex but keeps from
			// having to figure out how to pass the friggering flag around.
			if (MPlayer && MPlayer->isServer())
			{
				if (myVehicle->isGuardTower() && !(myVehicle->suppressionFire) && (curTacOrder.attackParams.method == AttackMethod::ranged) && (curTacOrder.attackParams.range == FireRangeType::current) && (curTacOrder.attackParams.pursue == false))
				{
					myVehicle->suppressionFire = true;
				}
			}
		}
	}
	else
	{
		if (lastTargetConserveAmmo)
			attackType = AttackType::conservingammo;
	}
	if (target)
	{
		if (!curTacOrder.isCombatOrder())
		{
			Stuff::Vector3D pos = target->getPosition();
			if (myVehicle->distanceFrom(pos) > attackRadius)
			{
				setLastTarget(nullptr);
				target = nullptr;
			}
		}
	}
	if (!target && (curTacOrder.code != TacticalOrderCode::attackpoint))
	{
#if WARRIOR_DEBUGGING_ENABLED
		if (debugFlags & WARRIOR_DEBUG_FLAG_COMBAT)
		{
			wchar_t s[128];
			sprintf(s, "%s (%.2f) has no attack target.\n", callsign, getSituationFireRange());
			debugPrint(s, true);
		}
#endif
		return (result);
	}
	if (target)
	{
		if (target->isDestroyed())
		{
#if WARRIOR_DEBUGGING_ENABLED
			if (debugFlags & WARRIOR_DEBUG_FLAG_COMBAT)
			{
				wchar_t s[128];
				sprintf(
					s, "%s (%.2f) has a destroyed target.\n", callsign, getSituationFireRange());
				debugPrint(s, true);
			}
#endif
			return (result);
		}
		if (target->isDisabled() && !lastTargetObliterate)
		{
#if WARRIOR_DEBUGGING_ENABLED
			if (debugFlags & WARRIOR_DEBUG_FLAG_COMBAT)
			{
				wchar_t s[128];
				sprintf(s, "%s (%.2f) has a disabled target.\n", callsign, getSituationFireRange());
				debugPrint(s, true);
			}
#endif
			return (result);
		}
	}
#if WARRIOR_DEBUGGING_ENABLED
	if (debugFlags & WARRIOR_DEBUG_FLAG_COMBAT)
	{
		wchar_t s[512];
		if (!myVehicle->canFireWeapons())
		{
			sprintf(s, "%s's (%.2f) vehicle cannot fire now.\n", callsign, getSituationFireRange());
			debugPrint(s, true);
		}
		else if (weaponsStatusResult < 1)
		{
			switch (weaponsStatusResult)
			{
			case WEAPONS_STATUS_CANNOT_FIRE:
				sprintf(
					s, "%s's (%.2f) vehicle cannot fire now.", callsign, getSituationFireRange());
				break;
			case WEAPONS_STATUS_NO_TARGET:
				sprintf(s, "%s (%.2f) has no target.\n", callsign, getSituationFireRange());
				break;
			case WEAPONS_STATUS_OUT_OF_RANGE:
				sprintf(s, "%s (%.2f) out of range.\n", callsign, getSituationFireRange());
				break;
			case 0:
			{
				int32_t numWeaponsNotReady = 0;
				int32_t numWeaponsOutOfRange = 0;
				int32_t numWeaponsNotLocked = 0;
				int32_t numWeaponsOutOfAmmo = 0;
				int32_t numWeaponsNoChance = 0;
				int32_t numWeaponsTooHot = 0;
				for (size_t i = 0; i < myVehicle->numWeapons; i++)
				{
					if (weaponsStatus[i] == WEAPON_STATUS_NOT_READY)
						numWeaponsNotReady++;
					if (weaponsStatus[i] == WEAPON_STATUS_OUT_OF_AMMO)
						numWeaponsOutOfAmmo++;
					if (weaponsStatus[i] == WEAPON_STATUS_OUT_OF_RANGE)
						numWeaponsOutOfRange++;
					if (weaponsStatus[i] == WEAPON_STATUS_NOT_LOCKED)
						numWeaponsNotLocked++;
					if (weaponsStatus[i] == WEAPON_STATUS_NO_CHANCE)
						numWeaponsNoChance++;
					if (weaponsStatus[i] == WEAPON_STATUS_TOO_HOT)
						numWeaponsTooHot++;
				}
				sprintf(s,
					"%s (%.2f) has no shot: %d !ready, %d !ammo, %d !inrange, "
					"%d !locked, %d !chance, %d hot\n",
					callsign, getSituationFireRange(), numWeaponsNotReady, numWeaponsOutOfAmmo,
					numWeaponsOutOfRange, numWeaponsNotLocked, numWeaponsNoChance,
					numWeaponsTooHot);
			}
			break;
			default:
				// sprintf(s, "%s (%.2f)  cannot fire for unknown reason.\n",
				// callsign,  getSituationFireRange());
				NODEFAULT;
			}
			debugPrint(s, true);
		}
	}
#endif
	//----------------------------------------------------------------------
	// So we don't muck up the brains at this late date, simply report we're
	// out of ammo...
	if (attackType != AttackType::conservingammo && outOfAmmo)
	{
		radioMessage(RADIO_AMMO_OUT, TRUE);
	}
	BOOL cantConserve = FALSE; // report if we're asked to use ammo conservation
		// and that's impossible
	if (attackType == AttackType::conservingammo)
		cantConserve = TRUE;
	bool firedWeapon = false;
	if (myVehicle->canFireWeapons() && (weaponsStatusResult > 0))
	{
		//------------------
		// First, a taunt...
		// Only Taunt Moving things!!
		if (target && target->isMover() && RollDice(1))
			radioMessage(RADIO_TAUNT, true);
		//-------------
		// Let loose...
		float targetTime = getLastTargetTime();
		// Track how much damage I've done to the target when weapons finally
		// hit. If I've done enough to blow the target, STOP firing!!
		float damageDoneToTarget = 0.0f;
		for (size_t curWeapon = 0; curWeapon < myVehicle->numWeapons; curWeapon++)
		{
			int32_t weaponIndex = myVehicle->numOther + curWeapon;
			if (weaponsStatus[curWeapon] > 0)
			{
				if (!target || (target && (damageDoneToTarget < target->getDestructLevel())))
				{
					if (attackType != AttackType::conservingammo || myVehicle->getWeaponIsEnergy(weaponIndex))
					{
						float dmgDone = 0.0f;
						if (NO_ERROR == myVehicle->fireWeapon(target, targetTime, myVehicle->numOther + curWeapon, attackType, aimLocation, targetpoint, dmgDone))
							cantConserve = FALSE;
						firedWeapon = true;
						damageDoneToTarget += dmgDone;
					}
				}
			}
			result = NO_ERROR;
		}
	}
	if (curTacOrder.attackParams.tactic == TacticType::stopandfire)
	{
		if (firedWeapon)
			clearMoveOrders();
	}
	if (cantConserve) // probably a lie
	{
		for (size_t curWeapon = myVehicle->numOther;
			 curWeapon < myVehicle->numOther + myVehicle->numWeapons; curWeapon++)
		{
			if (myVehicle->getWeaponShots(curWeapon) == UNLIMITED_SHOTS && myVehicle->isWeaponWorking(curWeapon))
			{
				cantConserve = FALSE; // it is a lie! we have a working energy
					// weapon, it's just recycling or
					// something...
				break;
			}
		}
	}
	if (cantConserve)
	{
		radioMessage(RADIO_ILLEGAL_ORDER);
		setLastTarget(nullptr);
		clearCurTacOrder(); // so we don't keep trying (and announcing that it
			// doesn't work!!)
	}
	return (result);
}

//---------------------------------------------------------------------------
// MOVEMENT DECISION TREE
//---------------------------------------------------------------------------

#if 0

Stuff::Vector3D vectorOffset(Stuff::Vector3D start, Stuff::Vector3D goal, int32_t how)
{
	Stuff::Vector2DOf<float> start2d(start.x, start.y);
	Stuff::Vector2DOf<float> goal2d(goal.x, goal.y);
	Stuff::Vector2DOf<float> deltaVector;
	//start2d.init(start.x, start.y);
	//goal2d.init(goal.x, goal.y);
	if(how == 0)
	{
		// check from start to goal
		deltaVector.x = goal2d.x - start2d.x;
		deltaVector.y = goal2d.y - start2d.y;
	}
	else
	{
		// check from goal to start
		deltaVector.x = start2d.x - goal2d.x;
		deltaVector.y = start2d.y - goal2d.y;
	}
	//-------------------------------------------------------------
	// First, we need to calc the delta vector--how much we extend
	// the ray everytime we check the map cell for clear placement.
	deltaVector.Normalize(deltaVector);
	float cellLength = Terrain::MetersPerCell;
	cellLength *= 0.5;
	deltaVector *= cellLength;
	if(deltaVector.GetLength() == 0.0)
		return(start);
	//-------------------------------------------------
	// Determine the max length the ray must be cast...
	float maxLength = distance_from(start, goal);
	//------------------------------------------------------------
	// We'll start at the target, and if it's blocked, we'll move
	// toward our start location, looking for the first valid/open
	// cell...
	vector_2d curPoint;
	vector_2d startPoint;
	if(how == 0)
		curPoint.init(start2d.x, start2d.y);
	else
		curPoint.init(goal2d.x, goal2d.y);
	startPoint = curPoint;
	vector_2d curRay;
	curRay.zero();
	float rayLength = 0.0;
	int32_t tileR, tileC, cellR, cellC;
	Stuff::Vector3D curPoint3d;
	curPoint3d.init(curPoint.x, curPoint.y, 0.0);
	GameMap->worldToMapPos(curPoint3d, tileR, tileC, cellR, cellC);
	bool cellClear = GameMap->cellPassable(tileR, tileC, cellR, cellC);
	while(!cellClear && (rayLength < maxLength))
	{
		curPoint3d.init(curPoint.x, curPoint.y, 0.0);
		GameMap->worldToMapPos(curPoint3d, tileR, tileC, cellR, cellC);
		cellClear = GameMap->cellPassable(tileR, tileC, cellR, cellC);
		curPoint += deltaVector;
		curRay = curPoint - startPoint;
		rayLength = curRay.magnitude();
	}
	curPoint3d.init(curPoint.x, curPoint.y, 0.0);
	curPoint3d.z = GameMap->getTerrainElevation(curPoint3d);
	return(curPoint3d);
}

#endif

//---------------------------------------------------------------------------

Stuff::Vector3D
MechWarrior::calcWithdrawGoal(float withdrawRange)
{
	Stuff::Vector3D escapeVector;
#ifdef USE_TEAMS
	//------------------------------------------------------------------------------
	// Should also check against allied team, if the clan team is
	// withdrawing (but will fix after we know this first works with just one
	// team:)
	if ((teamId == INNER_SPHERE_TEAM_ID) || (teamId == ALLIED_TEAM_ID))
		escapeVector =
			GetClanTeam->calcEscapeVector((std::unique_ptr<Mover>(BaseObjectPtr(vehicle))), withdrawRange);
	else
		escapeVector =
			GetInnerSphereTeam->calcEscapeVector((std::unique_ptr<Mover>(BaseObjectPtr(vehicle))), withdrawRange);
	std::unique_ptr<Mover> myVehicle = (std::unique_ptr<Mover>(BaseObjectPtr(vehicle)));
	Assert(myVehicle != nullptr, 0, " Warrior has nullptr Vehicle ");
	if (escapeVector.magnitude() == 0.0)
		return (myVehicle->getPosition());
	//-------------------------------------------------------------------------
	// Let's just go as far as we can in this direction (if not far, then we're
	// screwed, for now. Perhaps we should go berserk, and just attack the
	// closest enemy if they're near. Otherwise, ask player for help? Or, we
	// could scan a few other directions for possible escape)...
	//-------------------------------------------------------------
	// First, we need to calc the delta vector--how much we extend
	// the ray everytime we check the map cell for clear placement.
	float cellLength = Terrain::MetersPerCell;
	cellLength *= 0.5;
	escapeVector *= cellLength;
	Stuff::Vector3D curPoint = myVehicle->getPosition();
	Stuff::Vector3D curRay = curPoint - myVehicle->getPosition();
	float rayLength = curRay.magnitude() * metersPerWorldUnit;
	int32_t lastTileRow, lastTileCol;
	GameMap->worldToMapTilePos(curPoint, lastTileRow, lastTileCol);
	while (rayLength < withdrawRange)
	{
		int32_t tileRow, tileCol;
		GameMap->worldToMapTilePos(curPoint, tileRow, tileCol);
		if ((tileRow != lastTileRow) || (tileCol != lastTileCol))
		{
			if (!GameMap->inBounds(tileRow, tileCol))
				break;
			MapTile tile = GameMap->getTile(tileRow, tileCol);
			if (tile.isFullyBlocked())
				break;
			//----------------------------------------------------
			// We've entered a new tile, so record where we are...
			lastTileRow = tileRow;
			lastTileCol = tileCol;
		}
		curPoint += escapeVector;
		curRay = curPoint - myVehicle->getPosition();
		rayLength = curRay.magnitude() * metersPerWorldUnit;
	}
	return (curPoint);
#else
	return (getVehicle()->getPosition());
#endif
}

//---------------------------------------------------------------------------

bool MechWarrior::movingOverBlownBridge(void)
{
	if (getMovePath())
	{
#ifdef USE_BRIDGES
		//-------------------------------------
		// If on a blown bridge, return true...
		std::unique_ptr<Mover> myVehicle = getVehicle();
		int32_t tileRow = 0, tileCol = 0, cellRow = 0, cellCol = 0;
		myVehicle->getTileCellPosition(tileRow, tileCol, cellRow, cellCol);
		int32_t overlay = GameMap->getOverlayType(tileRow, tileCol);
		if (OverlayIsBridge[overlay])
		{
			int32_t bridgeArea = GlobalMoveMap->calcArea(tileRow, tileCol);
			if (GlobalMoveMap->isClosedArea(bridgeArea))
				return (true);
		}
		//-----------------------------------------------------
		// If SR path will cross a blown bridge, return true...
		int32_t bridgeArea = getMovePath()->crossesBridge(-1, 3);
		if (bridgeArea > -1)
			if (GlobalMoveMap->isClosedArea(bridgeArea))
				return (true);
		//-----------------------------------------------------
		// If LR path will cross a blown bridge, return true...
		if (moveOrders.pathType == MOVEPATH_COMPLEX)
		{
			int32_t curGlobalStep = moveOrders.curGlobalStep;
			int32_t numGlobalSteps = moveOrders.numGlobalSteps;
			for (size_t i = curGlobalStep; i < numGlobalSteps; i++)
			{
				int32_t thruArea = moveOrders.globalPath[i].thruArea;
				if (GlobalMoveMap->isBridgeArea(thruArea))
				{
					if (GlobalMoveMap->isClosedArea(thruArea))
						return (true);
				}
				else
					return (false);
			}
		}
#endif
	}
	return (false);
}

//---------------------------------------------------------------------------

bool MechWarrior::movementDecisionTree(void)
{
	if (moveOrders.timeOfLastStep > -1.0)
	{
		if (moveOrders.timeOfLastStep < (scenarioTime - MoveTimeOut))
		{
			//-------------------------------------------------------
			// Abort move order, 'cause we are unable to take a step!
			clearMoveOrders();
			if (curTacOrder.isMoveOrder() || curTacOrder.isWayPathOrder())
			{
				if (curTacOrder.time < (scenarioTime - MoveTimeOut))
				{
					radioMessage(RADIO_MOVE_BLOCKED, true);
					clearCurTacOrder();
				}
			}
			triggerAlarm(PILOT_ALARM_NO_MOVEPATH, MOVEPATH_ERR_TIME_OUT);
		}
	}
	std::unique_ptr<Mover> myVehicle = getVehicle();
	if (isYielding())
	{
		if (getMoveYieldTime() < scenarioTime)
		{
			setMoveYieldTime(scenarioTime + MoveYieldTime);
			//-----------------------------------------------------------------------
			// For now, we'll use random chance to decide if we should recalc or
			// not. DEFINITELY use a smarter priority scheme before this thing
			// ships :)
			bool onBlownBridge = false;
			std::unique_ptr<Mover> myVehicle = getVehicle();
			int32_t cellRow = 0, cellCol = 0;
			myVehicle->getCellPosition(cellRow, cellCol);
#ifdef USE_OVERLAYS
			int32_t overlay = GameMap->getOverlay(cellRow, cellCol);
			if (OverlayIsBridge[overlay])
			{
				int32_t bridgeArea = GlobalMoveMap->calcArea(tileRow, tileCol);
				if (GlobalMoveMap->isClosedArea(bridgeArea))
					onBlownBridge = true;
			}
#endif
			bool reroute = onBlownBridge || (RandomNumber(100) < 75);
			if (getMovePath())
			{
				int32_t bridgeArea = getMovePath()->crossesBridge(-1, 3);
				if (bridgeArea > -1)
					reroute = reroute || movingOverBlownBridge();
			}
			if (reroute)
			{
				uint32_t params = MOVEPARAM_RECALC | MOVEPARAM_FACE_TARGET;
				if (curTacOrder.moveparams.jump)
					params |= MOVEPARAM_JUMP;
				requestMovePath(curTacOrder.selectionindex, params, 4);
			}
		}
	}
	if (getMovePathType() == MOVEPATH_COMPLEX)
	{
		//-------------------------------------------------------------------
		// Currently following a complex (LR) path--make sure we queue up the
		// next path segment if we haven't already...
		if (getMoveCurGlobalStep() < (getMoveNumGlobalSteps() - 1))
		{
			if (moveOrders.path[1]->numStepsWhenNotPaused == 0)
			{
				//------------------------------------------------
				// Currently, we don't have any paths queued up...
				if (!movePathRequest)
				{
					//----------------------------------------------------------------
					// And, we don't already have a request to fill the queue
					// for this complex path, so let's do it...
					TacticalOrderPtr curTacOrder = getCurTacOrder();
					uint32_t params = MOVEPARAM_FACE_TARGET;
					if (curTacOrder->moveparams.jump)
						params |= MOVEPARAM_JUMP;
					requestMovePath(curTacOrder->selectionindex, params, 5);
				}
			}
		}
	}
	//-----------------------------------------------------------------
	// Everything up to this point is done EVERY frame update. Now, see
	// if it's time to do the ugly stuff...
	if (movementUpdate > scenarioTime)
		return (true);
	movementUpdate = scenarioTime + MovementUpdateFrequency;
	//	if (myVehicle->isSelected())
	//		OutputDebugString("DebugMe\n");
	if ((curTacOrder.code == TacticalOrderCode::none) || (curTacOrder.code == TacticalOrderCode::stop))
	{
		GameObjectPtr target = getLastTarget();
		if (target)
			setMoveStateGoal(MOVESTATE_PIVOT_TARGET);
		return (true);
	}
	if (getVehicle()->attackRange == FireRangeType::current)
		if (!curTacOrder.isMoveOrder() && (curTacOrder.code != TacticalOrderCode::capture))
		{
			GameObjectPtr target = getLastTarget();
			if (target)
				setMoveStateGoal(MOVESTATE_PIVOT_TARGET);
			return (true);
		}
	GameObjectPtr goalObject = nullptr;
	uint32_t goalType = getMoveGoal(nullptr, &goalObject);
	if (goalType == MOVEGOAL_NONE)
	{
		//--------------------------------------------
		// Are we currently pursuing an attack target?
		// Is this what we really want?
		if (curTacOrder.code == TacticalOrderCode::withdraw)
		{
			//--------------------------------------------------------
			// We are on the run, so let's choose a new escape goal...
			Stuff::Vector3D escapeGoal = calcWithdrawGoal();
			setMoveGoal(MOVEGOAL_LOCATION, &escapeGoal);
			uint32_t params = MOVEPARAM_INIT | MOVEPARAM_FACE_TARGET;
			if (curTacOrder.moveparams.jump)
				params |= MOVEPARAM_JUMP;
			requestMovePath(curTacOrder.selectionindex, params, 6);
		}
		else if (curTacOrder.code == TacticalOrderCode::movetopoint)
		{
			int32_t msg;
			curTacOrder.execute(this, msg);
		}
		else if ((curTacOrder.code == TacticalOrderCode::movetoobject) || (curTacOrder.code == TacticalOrderCode::capture))
		{
			GameObjectPtr moveTarget = curTacOrder.getTarget();
			//-------------------------------------------------
			// Pick the best move goal based upon the object...
			Stuff::Vector3D targetPos = moveTarget->getPosition();
			if (moveTarget->isMover())
				targetPos = moveTarget->getPosition();
			else
			{
				//------------------------------------------------
				// If not a mover, then pick an open cell adjacent
				// to it. If it happens to be adjacent to blocked
				// cells, this could be off...
				if (moveTarget->isBuilding())
				{
					BuildingPtr building = (BuildingPtr)moveTarget;
					int32_t goalRow = 0, goalCol = 0;
					bool foundGoal = building->calcAdjacentAreaCell(
						getVehicle()->moveLevel, -1, goalRow, goalCol);
					if (foundGoal)
						land->cellToWorld(goalRow, goalCol, targetPos);
					else
					{
						Stuff::Vector3D objectPos = moveTarget->getPosition();
						targetPos = getVehicle()->calcOffsetMoveGoal(objectPos);
					}
				}
				else
				{
					Stuff::Vector3D objectPos = moveTarget->getPosition();
					targetPos = getVehicle()->calcOffsetMoveGoal(objectPos);
				}
			}
			setMoveGoal(moveTarget->getWatchID(), &targetPos, moveTarget);
			uint32_t params = MOVEPARAM_INIT | MOVEPARAM_FACE_TARGET;
			if (curTacOrder.moveparams.jump)
				params |= MOVEPARAM_JUMP;
			if (!moveTarget->isMover())
				requestMovePath(
					curTacOrder.selectionindex, params | MOVEPARAM_STEP_ADJACENT_TARGET, 7);
			else
				requestMovePath(curTacOrder.selectionindex, params, 8);
		}
		else if (curTacOrder.isCombatOrder())
		{
			Stuff::Vector3D targetposition;
			GameObjectPtr target = getCurrentTarget();
			if (target)
				targetposition = target->getPosition();
			else if (curTacOrder.code == TacticalOrderCode::attackpoint)
				targetposition = getAttackTargetPoint();
			else
				return (true);
			if (curTacOrder.attackParams.method == AttackMethod::ramming)
			{
				Stuff::Vector3D pos = target->getPosition();
				setMoveGoal(target->getWatchID(), &pos, target);
				uint32_t params = MOVEPARAM_INIT | MOVEPARAM_FACE_TARGET;
				if (curTacOrder.moveparams.jump)
					params |= MOVEPARAM_JUMP;
				requestMovePath(curTacOrder.selectionindex, params, 9);
			}
			else if (/*!getAttackPursuit() && */ !curTacOrder.attackParams.pursue)
				setMoveStateGoal(MOVESTATE_PIVOT_TARGET);
			else
			{
				bool recalcPath = false;
				uint32_t moveparams = MOVEPARAM_NONE;
				//---------------------------------------------------------------
				// We will want to re-evaluate optimal range if our optimal
				// range has changed...
				if (target && target->isMover())
					if (((std::unique_ptr<Mover>)target)->getLastWeaponEffectivenessCalc() > myVehicle->getLastOptimalRangeCalc())
						if (myVehicle->calcFireRanges())
						{
							recalcPath = true;
							moveparams = MOVEPARAM_SOMEWHERE_ELSE;
						}
				float distanceToTarget = myVehicle->distanceFrom(targetposition);
				float desiredAttackRange = myVehicle->getOrderedFireRange();
				float attackRangeDelta = distanceToTarget - desiredAttackRange;
				float recalcDelta = Terrain::worldUnitsPerVertex * metersPerWorldUnit;
				if ((curTacOrder.attackParams.tactic != TacticType::stopandfire) && (curTacOrder.attackParams.tactic != TacticType::turret) && ((attackRangeDelta > recalcDelta) || (attackRangeDelta < -recalcDelta)))
				{
					//----------------------------------------
					// Try to maintain attack range...
					recalcPath = true;
					moveparams = MOVEPARAM_SOMEWHERE_ELSE;
				}
				else if ((curTacOrder.attackParams.tactic != TacticType::turret) && (weaponsStatusResult == WEAPONS_STATUS_OUT_OF_RANGE))
				{
					recalcPath = true;
					moveparams = MOVEPARAM_SOMEWHERE_ELSE;
				}
				else if (weaponsStatusResult >= 0)
				{
					int32_t numWeaponsNotReady = 0;
					int32_t numWeaponsOutOfAmmo = 0;
					int32_t numWeaponsOutOfRange = 0;
					int32_t numWeaponsNotLocked = 0;
					int32_t numWeaponsNoChance = 0;
					int32_t numWeaponsTooHot = 0;
					for (size_t i = 0; i < myVehicle->numWeapons; i++)
					{
						if (weaponsStatus[i] == WEAPON_STATUS_NOT_READY)
							numWeaponsNotReady++;
						if (weaponsStatus[i] == WEAPON_STATUS_OUT_OF_AMMO)
							numWeaponsOutOfAmmo++;
						if (weaponsStatus[i] == WEAPON_STATUS_OUT_OF_RANGE)
							numWeaponsOutOfRange++;
						if (weaponsStatus[i] == WEAPON_STATUS_NOT_LOCKED)
							numWeaponsNotLocked++;
						if (weaponsStatus[i] == WEAPON_STATUS_NO_CHANCE)
							numWeaponsNoChance++;
						if (weaponsStatus[i] == WEAPON_STATUS_TOO_HOT)
							numWeaponsTooHot++;
					}
					switch (myVehicle->getObjectClass())
					{
					case BATTLEMECH:
						if (weaponsStatusResult == 0)
						{
							if ((target && !myVehicle->lineOfSight(target)) || (!target && !myVehicle->lineOfSight(targetposition)))
							{
								recalcPath = true;
								moveparams = MOVEPARAM_SOMEWHERE_ELSE;
							}
							else if ((numWeaponsNotReady > 0) || (numWeaponsNotLocked > 0) || (numWeaponsTooHot > 0))
							{
								if ((getMovePath()->numSteps == 0) && !getMoveTwisting())
									setMoveStateGoal(MOVESTATE_PIVOT_TARGET);
							}
							else
							{
								recalcPath = true;
								moveparams = MOVEPARAM_SOMEWHERE_ELSE;
							}
						}
						break;
					case GROUNDVEHICLE:
						if (weaponsStatusResult == 0)
							if ((target && !myVehicle->lineOfSight(target)) || (!target && !myVehicle->lineOfSight(target)))
							{
								recalcPath = true;
								moveparams = MOVEPARAM_SOMEWHERE_ELSE;
							}
							else if (numWeaponsNotReady == 0)
							{
								if ((numWeaponsNotLocked > 0) || (numWeaponsTooHot > 0))
								{
									if ((getMovePath()->numSteps == 0) && !getMoveTwisting())
										setMoveStateGoal(MOVESTATE_PIVOT_TARGET);
								}
								else
								{
									recalcPath = true;
									moveparams = MOVEPARAM_SOMEWHERE_ELSE;
								}
							}
						break;
					case ELEMENTAL:
						if (weaponsStatusResult == 0)
							if (numWeaponsNotReady == 0)
							{
								if ((numWeaponsNotLocked > 0) || (numWeaponsTooHot > 0))
								{
									if ((getMovePath()->numSteps == 0) && !getMoveTwisting())
										setMoveStateGoal(MOVESTATE_PIVOT_TARGET);
								}
								else
								{
									recalcPath = true;
									moveparams = MOVEPARAM_SOMEWHERE_ELSE;
								}
							}
						break;
					}
				}
				if (!recalcPath && (curTacOrder.attackParams.tactic != TacticType::stopandfire))
					if (keepMoving && curTacOrder.moveparams.keepMoving)
						if (RandomNumber(100) < 60)
						{
							recalcPath = true;
							moveparams = MOVEPARAM_SOMEWHERE_ELSE + MOVEPARAM_RANDOM_OPTIMAL;
						}
				if (recalcPath && (curTacOrder.attackParams.tactic != TacticType::turret))
				{
					//					setMoveStateGoal(MOVESTATE_FORWARD);
					if (target)
					{
						setMoveGoal(target->getWatchID(), &targetposition, target);
						if (curTacOrder.moveparams.jump)
							moveparams |= MOVEPARAM_JUMP;
						requestMovePath(curTacOrder.selectionindex,
							moveparams | MOVEPARAM_INIT | MOVEPARAM_FACE_TARGET, 10);
					}
					else
					{
						setMoveGoal(MOVEGOAL_LOCATION, &targetposition);
						uint32_t params = MOVEPARAM_INIT | MOVEPARAM_FACE_TARGET;
						if (curTacOrder.moveparams.jump)
							params |= MOVEPARAM_JUMP;
						requestMovePath(curTacOrder.selectionindex, params, 11);
					}
				}
			}
		}
		else
		{
			GameObjectPtr target = getLastTarget();
			if (target)
				setMoveStateGoal(MOVESTATE_PIVOT_TARGET);
		}
	}
	else if (goalType == MOVEGOAL_LOCATION)
	{
		if (!isYielding() && !isJumping() && !getMoveTwisting() && !isWaitingForPoint() && (getMovePath()->numSteps == 0))
		{
			//---------------------------------------------
			// We may be so close that we don't need a move
			// path...
			if (!getMovePathRequest())
			{
				uint32_t params = MOVEPARAM_INIT | MOVEPARAM_FACE_TARGET;
				if (curTacOrder.moveparams.jump)
					params |= MOVEPARAM_JUMP;
				requestMovePath(curTacOrder.selectionindex, params, 12);
			}
		}
	}
	else
	{
		//-------------------------------------------------------
		// Check if our target is still around. If not, clear our
		// current move goal...
		if (goalObject)
		{
			//---------------------------------------------------------------
			// We may want to call a modified orderMoveToObject routine (but,
			// we don't want to play with the tacOrders)...
			Stuff::Vector3D targetPos = goalObject->getPosition();
			if (!curTacOrder.isCombatOrder())
			{
				if (distance_from(targetPos, moveOrders.goalObjectPosition) > 50.0)
				{
					Stuff::Vector3D pos = goalObject->getPosition();
					setMoveGoal(goalObject->getWatchID(), &pos, goalObject);
					uint32_t params = MOVEPARAM_INIT | MOVEPARAM_FACE_TARGET;
					if (curTacOrder.moveparams.jump)
						params |= MOVEPARAM_JUMP;
					requestMovePath(curTacOrder.selectionindex, params, 13);
				}
				else if (getMovePath()->numStepsWhenNotPaused == 0)
				{
					Stuff::Vector3D pos = goalObject->getPosition();
					setMoveGoal(goalObject->getWatchID(), &pos, goalObject);
					uint32_t params = MOVEPARAM_INIT | MOVEPARAM_FACE_TARGET;
					if (curTacOrder.moveparams.jump)
						params |= MOVEPARAM_JUMP;
					requestMovePath(curTacOrder.selectionindex, params, 13);
				}
			}
			else
			{
				GameObjectPtr target = getCurrentTarget();
				// float relFacing = myVehicle->relViewFacingTo(targetPos);
				if (target && (target == goalObject) && getAttackPursuit())
				{
					bool recalcPath = false;
					uint32_t moveparams = MOVEPARAM_NONE;
					//---------------------------------------------------------------
					// We will want to re-evaluate optimal range if our optimal
					// range has changed...
					if (target->isMover())
					{
						if (((std::unique_ptr<Mover>)target)->getLastWeaponEffectivenessCalc() > myVehicle->getLastOptimalRangeCalc())
							if (myVehicle->calcFireRanges())
							{
								recalcPath = true;
								moveparams = MOVEPARAM_SOMEWHERE_ELSE;
							}
					}
					else
					{
						if (getMovePath()->numSteps > 0)
							return (true);
					}
					float distanceToTarget = myVehicle->distanceFrom(target->getPosition());
					float desiredAttackRange = myVehicle->getOrderedFireRange();
					float attackRangeDelta = distanceToTarget - desiredAttackRange;
					float recalcDelta = Terrain::worldUnitsPerVertex * metersPerWorldUnit;
					if ((curTacOrder.attackParams.tactic != TacticType::stopandfire) && (curTacOrder.attackParams.tactic != TacticType::turret) && ((attackRangeDelta > recalcDelta) || (attackRangeDelta < -recalcDelta)))
					{
						//----------------------------------------
						// Try to maintain attack range...
						recalcPath = true;
						moveparams = MOVEPARAM_SOMEWHERE_ELSE;
					}
					else if ((curTacOrder.attackParams.tactic != TacticType::turret) && (weaponsStatusResult == WEAPONS_STATUS_OUT_OF_RANGE))
					{
						recalcPath = true;
						moveparams = MOVEPARAM_SOMEWHERE_ELSE;
					}
					else if (weaponsStatusResult >= 0)
					{
						int32_t numWeaponsNotReady = 0;
						int32_t numWeaponsOutOfRange = 0;
						int32_t numWeaponsOutOfAmmo = 0;
						int32_t numWeaponsNotLocked = 0;
						int32_t numWeaponsNoChance = 0;
						int32_t numWeaponsTooHot = 0;
						for (size_t i = 0; i < myVehicle->numWeapons; i++)
						{
							if (weaponsStatus[i] == WEAPON_STATUS_NOT_READY)
								numWeaponsNotReady++;
							if (weaponsStatus[i] == WEAPON_STATUS_OUT_OF_AMMO)
								numWeaponsOutOfAmmo++;
							if (weaponsStatus[i] == WEAPON_STATUS_OUT_OF_RANGE)
								numWeaponsOutOfRange++;
							if (weaponsStatus[i] == WEAPON_STATUS_NOT_LOCKED)
								numWeaponsNotLocked++;
							if (weaponsStatus[i] == WEAPON_STATUS_NO_CHANCE)
								numWeaponsNoChance++;
							if (weaponsStatus[i] == WEAPON_STATUS_TOO_HOT)
								numWeaponsTooHot++;
						}
						switch (myVehicle->getObjectClass())
						{
						case BATTLEMECH:
							if (weaponsStatusResult == 0)
							{
								if (!myVehicle->lineOfSight(target))
								{
									recalcPath = true;
									moveparams = MOVEPARAM_SOMEWHERE_ELSE;
								}
								else if ((numWeaponsNotReady > 0) || (numWeaponsTooHot > 0) || (numWeaponsNotLocked > 0))
								{
									//-----------------------------------------------------------
									// If we already have a path, don't pivot,
									// finish the move...
									if ((getMovePath()->numSteps == 0) && !getMoveTwisting())
										setMoveStateGoal(MOVESTATE_PIVOT_TARGET);
								}
								else
								{
									recalcPath = true;
									moveparams = MOVEPARAM_SOMEWHERE_ELSE;
								}
							}
							break;
						case GROUNDVEHICLE:
							if (weaponsStatusResult == 0)
								if (!myVehicle->lineOfSight(target))
								{
									recalcPath = true;
									moveparams = MOVEPARAM_SOMEWHERE_ELSE;
								}
								else if (numWeaponsNotReady == 0)
								{
									if ((numWeaponsNotLocked > 0) || (numWeaponsTooHot > 0))
									{
										if ((getMovePath()->numSteps == 0) && !getMoveTwisting())
											setMoveStateGoal(MOVESTATE_PIVOT_TARGET);
									}
									else
									{
										recalcPath = true;
										moveparams = MOVEPARAM_STEP_TOWARD_TARGET;
									}
								}
							break;
						case ELEMENTAL:
							if (weaponsStatusResult == 0)
								if (numWeaponsNotReady == 0)
								{
									if ((numWeaponsNotLocked > 0) || (numWeaponsTooHot > 0))
									{
										if ((getMovePath()->numSteps == 0) && !getMoveTwisting())
											setMoveStateGoal(MOVESTATE_PIVOT_TARGET);
									}
									else
									{
										recalcPath = true;
										moveparams = MOVEPARAM_SOMEWHERE_ELSE;
									}
								}
							break;
						}
					}
					//--added 6/3/01 by GD
					if (!recalcPath && (curTacOrder.attackParams.tactic != TacticType::stopandfire))
						if (keepMoving && curTacOrder.moveparams.keepMoving)
							if (RandomNumber(100) < 60)
							{
								recalcPath = true;
								moveparams = MOVEPARAM_SOMEWHERE_ELSE + MOVEPARAM_RANDOM_OPTIMAL;
							}
					//--
					if (recalcPath && (curTacOrder.attackParams.tactic != TacticType::turret))
					{
						//						setMoveStateGoal(MOVESTATE_FORWARD);
						Stuff::Vector3D pos = target->getPosition();
						setMoveGoal(target->getWatchID(), &pos, target);
						if (curTacOrder.moveparams.jump)
							moveparams |= MOVEPARAM_JUMP;
						requestMovePath(curTacOrder.selectionindex,
							moveparams | MOVEPARAM_INIT | MOVEPARAM_FACE_TARGET, 14);
					}
				}
			}
		}
	}
	return (true);
}

//---------------------------------------------------------------------------
// MAIN DECISION TREE
//---------------------------------------------------------------------------

void MechWarrior::clearCurTacOrder(bool updateTacOrder)
{
	if (curTacOrder.isCombatOrder())
	{
		numWarriorsInCombat--;
	}
	gosASSERT(numWarriorsInCombat >= 0);
	if (getVehicle())
		getVehicle()->suppressionFire = 0;
	curTacOrder.init();
	clearMoveOrders();
	triggerAlarm(PILOT_ALARM_NO_MOVEPATH, MOVEPATH_ERR_TACORDER_CLEARED);
	clearAttackOrders();
	lastTacOrder.lastTime = scenarioTime;
	if (updateTacOrder)
	{
		TacticalOrder newTacOrder;
		switch (orderState)
		{
		case ORDERSTATE_GENERAL:
			if (!newTacOrderReceived[ORDERSTATE_GENERAL])
				tacOrder[ORDERSTATE_GENERAL].init();
			//--------------------------------------------
			// If we reach this, then we have no orders...
			break;
		case ORDERSTATE_PLAYER:
			if (!newTacOrderReceived[ORDERSTATE_PLAYER])
			{
				tacOrder[ORDERSTATE_PLAYER].init();
				curPlayerOrderFromQueue = false;
			}
			newTacOrder = tacOrder[ORDERSTATE_GENERAL];
			orderState = ORDERSTATE_GENERAL;
			break;
		case ORDERSTATE_ALARM:
			if (!newTacOrderReceived[ORDERSTATE_ALARM])
				tacOrder[ORDERSTATE_ALARM].init();
			alarmPriority = 0;
			if (tacOrder[ORDERSTATE_PLAYER].code != TacticalOrderCode::none)
			{
				newTacOrder = tacOrder[ORDERSTATE_PLAYER];
				orderState = ORDERSTATE_PLAYER;
			}
			else
			{
				newTacOrder = tacOrder[ORDERSTATE_GENERAL];
				orderState = ORDERSTATE_GENERAL;
			}
			break;
		}
		if (newTacOrder.code != TacticalOrderCode::none)
		{
			Assert((moveOrders.path[0] != nullptr) && (moveOrders.path[1] != nullptr), 0,
				" bad warrior path ");
			MovePathPtr tempPath[2];
			for (size_t i = 0; i < 2; i++)
			{
				tempPath[i] = moveOrders.path[i];
				tempPath[i]->numSteps = 0;
			}
			int32_t tempMoveState = moveOrders.moveState;
			int32_t tempMoveStateGoal = moveOrders.moveStateGoal;
			moveOrders.init();
			PathManager->remove(this);
			moveOrders.moveState = tempMoveState;
			moveOrders.moveStateGoal = tempMoveStateGoal;
			moveOrders.path[0] = tempPath[0];
			moveOrders.path[1] = tempPath[1];
			attackOrders.init();
			int32_t message = -1;
			newTacOrder.execute(this, message);
			if (orderState == ORDERSTATE_PLAYER)
				radioMessage(message, true);
			setCurTacOrder(newTacOrder);
			if (useGoalPlan)
			{
				Stuff::Vector3D movePoint = newTacOrder.getWayPoint(0);
				if (!newTacOrder.subOrder)
					switch (newTacOrder.code)
					{
					case TacticalOrderCode::movetopoint:
						setMainGoal(GOAL_ACTION_MOVE, nullptr, &movePoint, -1.0);
						break;
					case TacticalOrderCode::attackobject:
						setMainGoal(GOAL_ACTION_ATTACK, newTacOrder.getTarget(), nullptr, -1.0);
						break;
					case TacticalOrderCode::capture:
						setMainGoal(GOAL_ACTION_CAPTURE, newTacOrder.getTarget(), nullptr, 400.0);
						break;
					default:
						// setMainGoal(GOAL_ACTION_NONE, nullptr, nullptr,
						// -1.0);
						NODEFAULT;
					}
			}
			timeOfLastOrders = -1.0;
		}
		//-------------------------------------
		// These must be cleared every frame...
		newTacOrderReceived[ORDERSTATE_GENERAL] = false;
		newTacOrderReceived[ORDERSTATE_PLAYER] = false;
		newTacOrderReceived[ORDERSTATE_ALARM] = false;
	}
}

//---------------------------------------------------------------------------
bool MechWarrior::anyPlayerInCombat(void)
{
	for (size_t i = 0; i < MAX_WARRIORS; i++)
	{
		if (warriorList[i] && (warriorList[i]->getVehicle() && (warriorList[i]->getVehicle()->getCommanderId() == Commander::home->getId())) && (warriorList[i]->getCurrentTarget()))
		{
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------------

void MechWarrior::setCurTacOrder(TacticalOrder newTacOrder)
{
	curTacOrder = newTacOrder;
	lastTacOrder = newTacOrder;
	if (curTacOrder.isCombatOrder())
	{
		numWarriorsInCombat++;
	}
}

//---------------------------------------------------------------------------

void MechWarrior::setGeneralTacOrder(TacticalOrder newTacOrder)
{
	tacOrder[ORDERSTATE_GENERAL] = newTacOrder;
	newTacOrderReceived[ORDERSTATE_GENERAL] = true;
}

//---------------------------------------------------------------------------

void MechWarrior::setPlayerTacOrder(TacticalOrder newTacOrder, bool fromQueue)
{
	tacOrder[ORDERSTATE_PLAYER] = newTacOrder;
	newTacOrderReceived[ORDERSTATE_PLAYER] = true;
	curPlayerOrderFromQueue = fromQueue;
	if (!fromQueue)
		clearTacOrderQueue();
}

//---------------------------------------------------------------------------

void MechWarrior::setAlarmTacOrder(TacticalOrder newTacOrder, int32_t priority)
{
	if (priority >= alarmPriority)
	{
		tacOrder[ORDERSTATE_ALARM] = newTacOrder;
		newTacOrderReceived[ORDERSTATE_ALARM] = true;
		alarmPriority = priority;
	}
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::triggerAlarm(int32_t alarmCode, uint32_t triggerId)
{
	if (alarm[alarmCode].numTriggers == MAX_ALARM_TRIGGERS)
		return (-1);
	if (alarmHistory[alarmCode].numTriggers == MAX_ALARM_TRIGGERS)
		return (-1);
	alarm[alarmCode].trigger[alarm[alarmCode].numTriggers++] = triggerId;
	alarmHistory[alarmCode].trigger[alarmHistory[alarmCode].numTriggers++] = triggerId;
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::getEventHistory(int32_t alarmCode, int32_t* paramList)
{
	int32_t numValidTriggers = 0;
	switch (alarmCode)
	{
	case PILOT_ALARM_ATTACK_ORDER:
	case PILOT_ALARM_DEATH_OF_MATE:
	case PILOT_ALARM_KILLED_TARGET:
	case PILOT_ALARM_FRIENDLY_VEHICLE_CRIPPLED:
	case PILOT_ALARM_VEHICLE_DESTROYED:
	case PILOT_ALARM_MATE_FIRED_WEAPON:
	case PILOT_ALARM_COLLISION:
	case PILOT_ALARM_TARGET_OF_WEAPONFIRE:
	case PILOT_ALARM_FIRED_WEAPON:
	{
		for (size_t i = 0; i < alarmHistory[alarmCode].numTriggers; i++)
		{
			GameObjectPtr obj = ObjectManager->getByWatchID(alarmHistory[alarmCode].trigger[i]);
			if (obj)
				paramList[numValidTriggers++] = obj->getPartId();
		}
	}
	break;
	case PILOT_ALARM_HIT_BY_WEAPONFIRE:
	{
		for (size_t i = 0; i < alarmHistory[alarmCode].numTriggers; i++)
			if (alarmHistory[alarmCode].trigger[i] < 0)
				paramList[numValidTriggers++] = alarmHistory[alarmCode].trigger[i];
			else
			{
				GameObjectPtr obj = ObjectManager->getByWatchID(alarmHistory[alarmCode].trigger[i]);
				if (obj)
					paramList[numValidTriggers++] = obj->getPartId();
			}
	}
	break;
	default:
		for (size_t i = 0; i < alarmHistory[alarmCode].numTriggers; i++)
			paramList[i] = alarmHistory[alarmCode].trigger[i];
		numValidTriggers = alarmHistory[alarmCode].numTriggers;
		break;
	}
	return (numValidTriggers);
}

//-----------------------------------------------------------------------------

int32_t
MechWarrior::getNextEventHistory(int32_t* paramList)
{
	if (curEventID < NUM_PILOT_ALARMS)
	{
		while ((curEventID < NUM_PILOT_ALARMS) && (alarmHistory[curEventID].numTriggers == 0))
			curEventID++;
		if (curEventID < NUM_PILOT_ALARMS)
		{
			getEventHistory(curEventID, paramList);
			curEventID++;
			return (curEventID - 1);
		}
	}
	return (-1);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleAlarm(int32_t alarmCode, uint32_t triggerId)
{
	std::unique_ptr<Mover> myVehicle = getVehicle();
	Assert(myVehicle != nullptr, 0, " bad vehicle for pilot ");
	if (!myVehicle->getAwake())
		return (NO_ERROR);
	//------------------------------------------------------------------
	// handle Alarm Events. Only three alarms may be called outside the
	// normal pilot alarm procedure: basically, the three that cause the
	// mover to be wiped from the game before the normal pilot alarm
	// update would be called.
	switch (alarmCode)
	{
	case PILOT_ALARM_VEHICLE_INCAPACITATED:
		//----------------------------------------------------------
		// NOTE: This will never happen, since a disabled mech won't
		// get into this routine. The alarm is handled IMMEDIATELY
		// when it happens...
		handleOwnVehicleIncapacitation(triggerId);
		break;
	case PILOT_ALARM_VEHICLE_DESTROYED:
		//-----------------------------------------------------------
		// NOTE: This will never happen, since a destroyed mech won't
		// get into this routine. The alarm is handled IMMEDIATELY
		// when it happens...
		handleOwnVehicleDestruction(triggerId);
		break;
	case PILOT_ALARM_VEHICLE_WITHDRAWN:
		//-----------------------------------------------------------
		// NOTE: This will never happen, since a withdrawn mech won't
		// get into this routine. The alarm is handled IMMEDIATELY
		// when it happens...
		handleOwnVehicleWithdrawn();
		break;
	}
	//-----------------------------------------------------
	// After applying game-system effects for the alarm,
	// check for ABL-handler in the pilot's brain module...
	if (MPlayer && !MPlayer->isServer())
		return (NO_ERROR);
	if (brainAlarmCallback[alarmCode])
	{
		CurGroup = getGroup();
		CurObject = (GameObjectPtr)getVehicle();
		CurObjectClass = getVehicle()->getObjectClass();
		CurWarrior = this;
		CurContact = nullptr;
		CurAlarm = alarmCode;
		brain->execute(nullptr, brainAlarmCallback[alarmCode]);
		CurGroup = nullptr;
		CurObject = nullptr;
		CurObjectClass = 0;
		CurWarrior = nullptr;
		CurContact = nullptr;
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::getAlarmTriggers(int32_t alarmCode, uint32_t* triggerList)
{
	memcpy(triggerList, alarm[alarmCode].trigger, sizeof(uint32_t) * alarm[alarmCode].numTriggers);
	return (alarm[alarmCode].numTriggers);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::getAlarmTriggersHistory(int32_t alarmCode, uint32_t* triggerList)
{
	memcpy(triggerList, alarmHistory[alarmCode].trigger,
		sizeof(uint32_t) * alarmHistory[alarmCode].numTriggers);
	return (alarmHistory[alarmCode].numTriggers);
}

//---------------------------------------------------------------------------

void MechWarrior::clearAlarm(int32_t alarmCode)
{
	alarm[alarmCode].numTriggers = 0;
}

//---------------------------------------------------------------------------

void MechWarrior::clearAlarms(void)
{
	for (size_t code = 0; code < NUM_PILOT_ALARMS; code++)
		clearAlarm(code);
}

//---------------------------------------------------------------------------

void MechWarrior::clearAlarmHistory(int32_t alarmCode)
{
	alarmHistory[alarmCode].numTriggers = 0;
}

//---------------------------------------------------------------------------

void MechWarrior::clearAlarmsHistory(void)
{
	for (size_t code = 0; code < NUM_PILOT_ALARMS; code++)
		clearAlarmHistory(code);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::checkAlarms(void)
{
	//-------------------------------------------
	// In case we call ABL-handlers, set these...
	if (brain)
	{
		CurGroup = getGroup();
		CurObject = (GameObjectPtr)getVehicle();
		CurObjectClass = getVehicle()->getObjectClass();
		CurWarrior = this;
		CurContact = nullptr;
	}
	//-----------------------
	// handle Alarm Events...
	for (size_t code = 0; code < NUM_PILOT_ALARMS; code++)
		if (alarm[code].numTriggers > 0)
		{
			switch (code)
			{
			case PILOT_ALARM_TARGET_OF_WEAPONFIRE:
				handleTargetOfWeaponFire();
				break;
			case PILOT_ALARM_HIT_BY_WEAPONFIRE:
				handleHitByWeaponFire();
				break;
#ifdef USEHEAT
			case PILOT_ALARM_OVERHEAT:
				handleOverHeat();
				break;
#endif
			case PILOT_ALARM_DAMAGE_TAKEN_RATE:
				handleDamageTakenRate();
				break;
			case PILOT_ALARM_DEATH_OF_MATE:
				handleUnitMateDeath();
				break;
			case PILOT_ALARM_FRIENDLY_VEHICLE_CRIPPLED:
				handleFriendlyVehicleCrippled();
				break;
			case PILOT_ALARM_FRIENDLY_VEHICLE_DESTROYED:
				handleFriendlyVehicleDestruction();
				break;
			case PILOT_ALARM_VEHICLE_INCAPACITATED:
				//----------------------------------------------------------
				// NOTE: This will never happen, since a disabled mech won't
				// get into this routine. Thei alarm is handled IMMEDIATELY
				// when it happens...
				handleOwnVehicleIncapacitation(0);
				break;
			case PILOT_ALARM_VEHICLE_DESTROYED:
				//----------------------------------------------------------
				// NOTE: This will never happen, since a disabled mech won't
				// get into this routine. Thei alarm is handled IMMEDIATELY
				// when it happens...
				handleOwnVehicleDestruction(0);
				break;
			case PILOT_ALARM_VEHICLE_WITHDRAWN:
				//----------------------------------------------------------
				// NOTE: This will never happen, since a disabled mech won't
				// get into this routine. Thei alarm is handled IMMEDIATELY
				// when it happens...
				handleOwnVehicleWithdrawn();
				break;
			case PILOT_ALARM_ATTACK_ORDER:
				handleAttackOrder();
				break;
			case PILOT_ALARM_COLLISION:
				handleCollision();
				break;
			case PILOT_ALARM_KILLED_TARGET:
				handleKilledTarget();
				break;
			case PILOT_ALARM_MATE_FIRED_WEAPON:
				handleUnitMateFiredWeapon();
				break;
			case PILOT_ALARM_PLAYER_ORDER:
				handlePlayerOrder();
				break;
			case PILOT_ALARM_NO_MOVEPATH:
				handleNoMovePath();
				break;
			case PILOT_ALARM_GATE_CLOSING:
				handleGateClosing();
				break;
			case PILOT_ALARM_FIRED_WEAPON:
				handleFiredWeapon();
				break;
			case PILOT_ALARM_NEW_MOVER:
				handleNewMover();
				break;
			}
			//------------------------------------------------------
			// After applying game-system effects for the alarm,
			// check for ABL-handlers in the pilot's brain module...
			if (MPlayer)
			{
				if (MPlayer->isServer())
					if (brain && brainAlarmCallback[code])
					{
						CurAlarm = code;
						brain->execute(nullptr, brainAlarmCallback[code]);
					}
			}
			else
			{
				if (brain && brainAlarmCallback[code])
				{
					CurAlarm = code;
					brain->execute(nullptr, brainAlarmCallback[code]);
				}
			}
			//----------------------------------
			// Clear these alarms every frame...
			alarm[code].numTriggers = 0;
		}
	if (brain)
	{
		CurGroup = nullptr;
		CurObject = nullptr;
		CurObjectClass = 0;
		CurWarrior = nullptr;
		CurContact = nullptr;
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

void MechWarrior::updateActions(void)
{
	if ((teamId == -1) || !brainsEnabled[teamId])
	{
		clearCurTacOrder(false);
		clearMoveOrders();
		clearAttackOrders();
		tacOrder[ORDERSTATE_GENERAL].init();
		tacOrder[ORDERSTATE_PLAYER].init();
		tacOrder[ORDERSTATE_ALARM].init();
		setMainGoal(GOAL_ACTION_NONE, nullptr, nullptr, -1.0);
		return;
	}
	std::unique_ptr<Mover> myVehicle = getVehicle();
	if (myVehicle->isCaptured())
	{
		clearCurTacOrder();
		setLastTarget(nullptr);
		return;
	}
	if (combatUpdate <= scenarioTime)
		combatDecisionTree();
	movementDecisionTree();
}

// extern L_INTEGER startCk;
// extern L_INTEGER endCk;
// extern int32_t		 srWAblUpd;
//---------------------------------------------------------------------------

int32_t
MechWarrior::mainDecisionTree(void)
{
	Assert(moveOrders.path != nullptr, 0, " bad warrior path ");
	if ((teamId == -1) || !brainsEnabled[teamId])
	{
		clearCurTacOrder(false);
		tacOrder[ORDERSTATE_GENERAL].init();
		tacOrder[ORDERSTATE_PLAYER].init();
		tacOrder[ORDERSTATE_ALARM].init();
		setMainGoal(GOAL_ACTION_NONE, nullptr, nullptr, -1.0);
	}
	//---------------------------------------------------------------------------
	// If we currently have no order, check if we have any player orders
	// queued...
	if (curTacOrder.code == TacticalOrderCode::none)
	{
		if (tacOrderQueueExecuting && (numTacOrdersQueued > 0))
		{
			if (!newTacOrderReceived[ORDERSTATE_PLAYER])
			{
				//--------------------------------------------------------------------
				// First queued order just given, so begin executing it
				// immediately...
				executeTacOrderQueue();
			}
		}
	}
	//---------------------------------------
	// Is the current tac order accomplished?
	if ((curTacOrder.code != TacticalOrderCode::none) && (curTacOrder.status(this) == TACORDER_SUCCESS))
	{
		bool playerOrderQueued = false;
		if ((orderState == ORDERSTATE_PLAYER) && tacOrderQueueExecuting)
		{
			executeTacOrderQueue();
			playerOrderQueued = newTacOrderReceived[ORDERSTATE_PLAYER];
		}
		clearCurTacOrder();
		newTacOrderReceived[ORDERSTATE_PLAYER] = playerOrderQueued;
	}
	//------------------------------------------------------
	// If we're without orders now, let's record the time...
	if (underHomeCommand() && (curTacOrder.code == TacticalOrderCode::none) && (timeOfLastOrders < 0.0))
		timeOfLastOrders = scenarioTime;
	//-----------------------------------------------
	// Update Weapons Status, if needed this frame...
	if ((brainUpdate <= scenarioTime) || (combatUpdate <= scenarioTime) || (movementUpdate <= scenarioTime))
	{
		GameObjectPtr target = getCurrentTarget(); // getAttackTarget(OrderType::current);
		if (target)
			weaponsStatusResult = calcWeaponsStatus(target, weaponsStatus, nullptr);
		else if (curTacOrder.code == TacticalOrderCode::attackpoint)
		{
			Stuff::Vector3D pos = getAttackTargetPoint();
			weaponsStatusResult = calcWeaponsStatus(target, weaponsStatus, &pos);
		}
	}
	//-------------------------------------------------------------------------------------
	// Make sure, if I have a target, it's still legit.
	// Now, when we go into runBrain, we know the current target is up-to-date
	// with current game conditions...
	GameObjectPtr tacOrderTarget = nullptr;
	if (curTacOrder.code != TacticalOrderCode::none)
		tacOrderTarget = ObjectManager->getByWatchID(curTacOrder.targetWID);
	GameObjectPtr target = getLastTarget();
	if (target)
	{
		bool clearTarget = false;
		if (target->isDestroyed())
			clearTarget = true;
		else if (target->isDisabled() && !lastTargetObliterate)
			clearTarget = true;
		if (clearTarget)
		{
			setLastTarget(nullptr);
			if ((curTacOrder.code == TacticalOrderCode::attackobject) && (target == tacOrderTarget))
			{
				clearCurTacOrder();
				target = nullptr;
				tacOrderTarget = nullptr;
			}
		}
	}
	if ((curTacOrder.code == TacticalOrderCode::attackobject) && (target != tacOrderTarget))
		setLastTarget(tacOrderTarget);
	//----------------------
	// Update pilot brain...
	if ((brainUpdate <= scenarioTime) && ((teamId == -1) || brainsEnabled[teamId]))
	{
		runBrain();
		brainUpdate += BrainUpdateFrequency;
	}
	//------------------------------------------------------------------
	// In case the brain set a new target, let's recalc weaponsStatus...
	target = getLastTarget(); // getAttackTarget(OrderType::current);
	if (target && (lastTargetTime /*getLastTargetTime()*/ == scenarioTime))
		weaponsStatusResult = calcWeaponsStatus(target, weaponsStatus, nullptr);
	//------------
	// Any Alarms?
	checkAlarms();
	//-------------------------------------------------
	// Now, check if we have a new current tac order...
	TacticalOrder newTacOrder;
	switch (orderState)
	{
	case ORDERSTATE_GENERAL:
		if (newTacOrderReceived[ORDERSTATE_ALARM])
		{
			clearCurTacOrder(false);
			newTacOrder = tacOrder[ORDERSTATE_ALARM];
			orderState = ORDERSTATE_ALARM;
		}
		else if (newTacOrderReceived[ORDERSTATE_PLAYER])
		{
			//--------------------------------------------------------
			// If we're currently executing a general (brain) order,
			// purge it permanently (NOW, we DON'T want them to
			// fall back to the general order--the brain must re-order
			// it)...
			tacOrder[ORDERSTATE_GENERAL].init();
			//-----------------------------
			// Now grab the player order...
			newTacOrder = tacOrder[ORDERSTATE_PLAYER];
			orderState = ORDERSTATE_PLAYER;
		}
		else if (newTacOrderReceived[ORDERSTATE_GENERAL])
			newTacOrder = tacOrder[ORDERSTATE_GENERAL];
		break;
	case ORDERSTATE_PLAYER:
		if (newTacOrderReceived[ORDERSTATE_ALARM])
		{
			newTacOrder = tacOrder[ORDERSTATE_ALARM];
			orderState = ORDERSTATE_ALARM;
		}
		else if (newTacOrderReceived[ORDERSTATE_PLAYER])
		{
			//--------------------------------------------------------
			// Make sure we have no general order we fall back to when
			// we finish this, per Tim's request...
			tacOrder[ORDERSTATE_GENERAL].init();
			newTacOrder = tacOrder[ORDERSTATE_PLAYER];
		}
		break;
	case ORDERSTATE_ALARM:
		//--------------------------------------------------------------
		// If the player issues a new order, let it override any current
		// alarm (self-issued) order...
		if (newTacOrderReceived[ORDERSTATE_PLAYER])
		{
			alarmPriority = 0;
			tacOrder[ORDERSTATE_ALARM].init();
			newTacOrder = tacOrder[ORDERSTATE_PLAYER];
		}
		else if (newTacOrderReceived[ORDERSTATE_ALARM])
			newTacOrder = tacOrder[ORDERSTATE_ALARM];
		break;
	}
	if (newTacOrder.code != TacticalOrderCode::none)
	{
		//------------------------------------------------------
		// Note how we need to copy the move path over, since we
		// allocate it only once now...
		MovePathPtr tempPath[2];
		if ((newTacOrder.code == TacticalOrderCode::movetopoint) || (newTacOrder.code == TacticalOrderCode::movetoobject))
		{
			for (size_t i = 0; i < 2; i++)
			{
				tempPath[i] = moveOrders.path[i];
				if (i > 0)
					tempPath[i]->numSteps = 0;
			}
		}
		else
		{
			for (size_t i = 0; i < 2; i++)
			{
				tempPath[i] = moveOrders.path[i];
				tempPath[i]->numSteps = 0;
			}
		}
		int32_t tempMoveState = moveOrders.moveState;
		int32_t tempMoveStateGoal = moveOrders.moveStateGoal;
		bool running = moveOrders.run;
		moveOrders.init();
		moveOrders.run = running;
		PathManager->remove(this);
		moveOrders.moveState = tempMoveState;
		moveOrders.moveStateGoal = tempMoveStateGoal;
		moveOrders.path[0] = tempPath[0];
		moveOrders.path[1] = tempPath[1];
		attackOrders.init();
		int32_t message = -1;
		newTacOrder.execute(this, message);
		if (orderState == ORDERSTATE_PLAYER)
			radioMessage(message, true);
		setCurTacOrder(newTacOrder);
		if (useGoalPlan)
		{
			Stuff::Vector3D movePoint = newTacOrder.getWayPoint(0);
			if (!newTacOrder.subOrder)
				switch (newTacOrder.code)
				{
				case TacticalOrderCode::movetopoint:
					setMainGoal(GOAL_ACTION_MOVE, nullptr, &movePoint, -1.0);
					break;
				case TacticalOrderCode::attackobject:
					setMainGoal(GOAL_ACTION_ATTACK, newTacOrder.getTarget(), nullptr, -1.0);
					break;
				case TacticalOrderCode::capture:
					setMainGoal(GOAL_ACTION_CAPTURE, newTacOrder.getTarget(), nullptr, 400.0);
					break;
				default:
					// setMainGoal(GOAL_ACTION_NONE, nullptr, nullptr, -1.0);
					NODEFAULT;
				}
		}
		timeOfLastOrders = -1.0;
	}
	//-------------------------------------
	// These must be cleared every frame...
	newTacOrderReceived[ORDERSTATE_GENERAL] = false;
	newTacOrderReceived[ORDERSTATE_PLAYER] = false;
	newTacOrderReceived[ORDERSTATE_ALARM] = false;
	//--------------------------------------------
	// Now that we know what we're doing, do it...
	updateActions();
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

void MechWarrior::setDebugFlag(uint32_t flag, bool on)
{
	if (on)
		debugFlags |= flag;
	else
		debugFlags &= (flag ^ 0xFFFFFFFF);
}

//---------------------------------------------------------------------------

bool MechWarrior::getDebugFlag(uint32_t flag)
{
	return ((debugFlags & flag) != 0);
}

//---------------------------------------------------------------------------

void MechWarrior::debugPrint(std::wstring_view s, bool debugMode)
{
	if (debugger)
	{
		debugger->print(s);
		if (debugMode)
			debugger->debugMode();
	}
}

//---------------------------------------------------------------------------

void MechWarrior::debugOrders(void)
{
	wchar_t s[256];
	switch (curTacOrder.code)
	{
	case TacticalOrderCode::none:
		sprintf(s, "CURRENT ORDERS: None");
		break;
	case TacticalOrderCode::wait:
		sprintf(s, "CURRENT ORDERS: Wait");
		break;
	case TacticalOrderCode::movetopoint:
		sprintf(s, "CURRENT ORDERS: Move to (%.2f, %.2f, %.2f)", curTacOrder.getWayPoint(0).x,
			curTacOrder.getWayPoint(0).y, curTacOrder.getWayPoint(0).z);
		break;
	case TacticalOrderCode::movetoobject:
	{
		GameObjectPtr obj = ObjectManager->getByWatchID(curTacOrder.targetWID);
		sprintf(s, "CURRENT ORDERS: Move To Object %d", obj ? obj->getPartId() : 0);
	}
	break;
	case TacticalOrderCode::jumptopoint:
		sprintf(s, "CURRENT ORDERS: Jump to (%.2f, %.2f, %.2f)", curTacOrder.getWayPoint(0).x,
			curTacOrder.getWayPoint(0).y, curTacOrder.getWayPoint(0).z);
		break;
	case TacticalOrderCode::jumptoobject:
	{
		GameObjectPtr obj = ObjectManager->getByWatchID(curTacOrder.targetWID);
		sprintf(s, "CURRENT ORDERS: Jump To Object %d", obj ? obj->getPartId() : 0);
	}
	break;
	case TacticalOrderCode::traversepath:
		sprintf(s, "CURRENT ORDERS: Traverse Path");
		break;
	case TacticalOrderCode::patrolpath:
		sprintf(s, "CURRENT ORDERS: Patrol Path");
		break;
	case TacticalOrderCode::escort:
		sprintf(s, "CURRENT ORDERS: Escort");
		break;
	case TacticalOrderCode::follow:
		sprintf(s, "CURRENT ORDERS: Follow");
		break;
	case TacticalOrderCode::guard:
		sprintf(s, "CURRENT ORDERS: Guard");
		break;
	case TacticalOrderCode::stop:
		sprintf(s, "CURRENT ORDERS: Stop");
		break;
	case TacticalOrderCode::powerup:
		sprintf(s, "CURRENT ORDERS: Power Up");
		break;
	case TacticalOrderCode::powerdown:
		sprintf(s, "CURRENT ORDERS: Power Down");
		break;
	case TacticalOrderCode::waypointsdone:
		sprintf(s, "CURRENT ORDERS: Waypoints Done");
		break;
	case TacticalOrderCode::eject:
		sprintf(s, "CURRENT ORDERS: Eject");
		break;
	case TacticalOrderCode::attackobject:
	{
		GameObjectPtr obj = ObjectManager->getByWatchID(curTacOrder.targetWID);
		sprintf(s, "CURRENT ORDERS: Attack Object %d", obj ? obj->getPartId() : 0);
	}
	break;
	case TacticalOrderCode::holdfire:
		sprintf(s, "CURRENT ORDERS: Hold Fire");
		break;
	case TacticalOrderCode::withdraw:
		sprintf(s, "CURRENT ORDERS: Withdraw");
		break;
	default:
		// sprintf(s, "CURRENT ORDERS: Unknown Tac Order Type");
		NODEFAULT;
	}
	debugPrint(s);
	GameObjectPtr target = getCurrentTarget();
	sprintf(s, "     CURRENT TARGET: Object %d", target ? target->getPartId() : 0);
	debugPrint(s);
}

//---------------------------------------------------------------------------

void MechWarrior::setMoveSpeedType(int32_t type)
{
	moveOrders.speedType = type;
}

//---------------------------------------------------------------------------

void MechWarrior::setMoveSpeedVelocity(float speed)
{
	moveOrders.speedVelocity = speed;
	std::unique_ptr<Mover> myVehicle = getVehicle();
	int32_t state, throttle;
	myVehicle->calcSpriteSpeed(speed, 0, state, throttle);
	moveOrders.speedState = state;
	moveOrders.speedThrottle = throttle;
}

//----------------------------------------------------------------------------

void MechWarrior::initTargetPriorities(void)
{
	targetPriorityList.init();
}

//----------------------------------------------------------------------------

int32_t
MechWarrior::setTargetPriority(
	int32_t index, int32_t type, int32_t param1, int32_t param2, int32_t param3)
{
	targetPriorityList.list[index].type = type;
	targetPriorityList.list[index].params[0] = param1;
	targetPriorityList.list[index].params[1] = param2;
	targetPriorityList.list[index].params[2] = param3;
	if (index >= targetPriorityList.size)
		targetPriorityList.size = index + 1;
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::insertTargetPriority(
	int32_t index, int32_t type, int32_t param1, int32_t param2, int32_t param3)
{
	TargetPriority priority;
	priority.type = type;
	priority.params[0] = param1;
	priority.params[1] = param2;
	priority.params[2] = param3;
	return (targetPriorityList.insert(index, &priority));
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::setBrainState(int32_t newState)
{
	int32_t oldBrainState = brainState;
	brainState = newState;
	return (oldBrainState);
}

//----------------------------------------------------------------------------

int32_t
MechWarrior::getBrainState(void)
{
	return (brainState);
}

//---------------------------------------------------------------------------

#if 0
GameObjectPtr MechWarrior::selectTarget(bool moversOnly, int32_t criteria)
{
}
#endif

//---------------------------------------------------------------------------
// CORE COMMANDS (MC2 BEHAVIORS)
//---------------------------------------------------------------------------

int32_t
MechWarrior::coreMoveTo(Stuff::Vector3D location, uint32_t params)
{
	bool run = ((params & TACORDER_PARAM_RUN) != 0);
	bool jump = ((params & TACORDER_PARAM_JUMP) != 0);
	bool wait = ((params & TACORDER_PARAM_WAIT) != 0);
	bool layMines = ((params & TACORDER_PARAM_LAY_MINES) != 0);
	bool escape = ((params & TACORDER_PARAM_ESCAPE_TILE) != 0);
	bool setOrder = ((params & TACORDER_PARAM_DONT_SET_ORDER) == 0);
	bool keepMoving = ((params & TACORDER_PARAM_DONT_KEEP_MOVING) == 0);
	TacticalOrder tacOrder;
	tacOrder.init(OrderOriginType::commander, TacticalOrderCode::movetopoint, false);
	tacOrder.moveparams.wayPath.points[0] = location.x;
	tacOrder.moveparams.wayPath.points[1] = location.y;
	tacOrder.moveparams.wayPath.points[2] = location.z;
	tacOrder.selectionindex = -1;
	tacOrder.moveparams.wayPath.mode[0] = (run ? TravelModeType::fast : TravelModeType::slow);
	tacOrder.moveparams.wait = wait;
	tacOrder.moveparams.mode = (layMines ? SpecialMoveMode::minelaying : SpecialMoveMode::normal);
	tacOrder.moveparams.escapeTile = escape;
	tacOrder.moveparams.jump = jump;
	tacOrder.moveparams.keepMoving = keepMoving;
	int32_t result = tacOrder.status(this);
	if (result == TACORDER_SUCCESS)
		return (1);
	if (curTacOrder.equals(&tacOrder))
		return (0);
	if (setOrder)
		clearCurTacOrder();
	setMoveWayPath(nullptr, 0);
	setMoveGoal(MOVEGOAL_LOCATION, &location);
	moveOrders.timeOfLastStep = scenarioTime;
	setMoveRun(run);
	if (setOrder)
		clearAttackOrders();
	uint32_t moveparams = MOVEPARAM_INIT | MOVEPARAM_FACE_TARGET;
	if (jump)
		moveparams |= MOVEPARAM_JUMP;
	PathManager->request(this, -1, moveparams, 20);
	if (setOrder)
	{
		setGeneralTacOrder(tacOrder);
		if (useGoalPlan)
			setMainGoal(GOAL_ACTION_MOVE, nullptr, &location, -1.0);
		else
			setMainGoal(GOAL_ACTION_NONE, nullptr, nullptr, -1.0);
	}
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::coreMoveToObject(GameObjectPtr object, uint32_t params)
{
	bool run = ((params & TACORDER_PARAM_RUN) != 0);
	bool jump = ((params & TACORDER_PARAM_JUMP) != 0);
	bool faceObject = ((params & TACORDER_PARAM_FACE_OBJECT) != 0);
	bool layMines = ((params & TACORDER_PARAM_LAY_MINES) != 0);
	bool setOrder = ((params & TACORDER_PARAM_DONT_SET_ORDER) == 0);
	bool keepMoving = ((params & TACORDER_PARAM_DONT_KEEP_MOVING) == 0);
	if (!object)
	{
		if (!coreScanTargetWID)
			coreScan(nullptr, params);
		object = ObjectManager->getByWatchID(coreScanTargetWID);
	}
	if (!object)
		return (1);
	//-------------------------------------------------
	// Pick the best move goal based upon the object...
	Stuff::Vector3D goal;
	if (object->isMover())
		goal = object->getPosition();
	else
	{
		//------------------------------------------------
		// If not a mover, then pick an open cell adjacent
		// to it. If it happens to be adjacent to blocked
		// cells, this could be off...
		Stuff::Vector3D objectPos = object->getPosition();
		goal = getVehicle()->calcOffsetMoveGoal(objectPos);
	}
	TacticalOrder tacOrder;
	tacOrder.init(OrderOriginType::commander, TacticalOrderCode::movetoobject, false);
	tacOrder.targetWID = object->getWatchID();
	tacOrder.selectionindex = -1;
	tacOrder.moveparams.fromArea = -1;
	tacOrder.moveparams.wayPath.points[0] = goal.x;
	tacOrder.moveparams.wayPath.points[1] = goal.y;
	tacOrder.moveparams.wayPath.points[2] = goal.z;
	tacOrder.moveparams.wayPath.mode[0] = (run ? TravelModeType::fast : TravelModeType::slow);
	tacOrder.moveparams.faceObject = faceObject;
	tacOrder.moveparams.wait = false;
	tacOrder.moveparams.mode = (layMines ? SpecialMoveMode::minelaying : SpecialMoveMode::normal);
	tacOrder.moveparams.jump = jump;
	tacOrder.moveparams.keepMoving = keepMoving;
	int32_t result = tacOrder.status(this);
	if (result == TACORDER_SUCCESS)
		return (1);
	if (curTacOrder.equals(&tacOrder))
		return (0);
	if (setOrder)
		clearCurTacOrder();
	//-----------------------------------
	// Is this already our current order?
	// if ((curTacOrder.code == TacticalOrderCode::movetoobject) &&
	//	(curTacOrder.params.move.targetId == target->getIdNumber()) &&
	//	(curTacOrder.params.move.selectionindex == selectionindex))
	//	return(TACORDER_FAILURE);
	tacOrder.targetWID = object->getWatchID();
	Stuff::Vector3D pos = object->getPosition();
	setMoveGoal(object->getWatchID(), &pos, object);
	setMoveRun(run);
	if (setOrder)
		clearAttackOrders();
	uint32_t moveparams = MOVEPARAM_INIT;
	if (jump)
		moveparams |= MOVEPARAM_JUMP;
	if (faceObject)
		moveparams |= MOVEPARAM_FACE_TARGET;
	if (setOrder && !object->isMover())
		moveparams |= MOVEPARAM_STEP_ADJACENT_TARGET;
	requestMovePath(-1, moveparams, 15);
	moveOrders.goalObjectPosition = object->getPosition();
	if (setOrder)
	{
		setGeneralTacOrder(tacOrder);
		if (useGoalPlan)
			setMainGoal(GOAL_ACTION_MOVE, object, nullptr, -1.0);
		else
			setMainGoal(GOAL_ACTION_NONE, nullptr, nullptr, -1.0);
	}
	return (0);
}

//---------------------------------------------------------------------------

#if 0

int32_t MechWarrior::coreJumpTo(Stuff::Vector3D location, uint32_t params)
{
	bool setOrder = ((params & TACORDER_PARAM_DONT_SET_ORDER) == 0);
	std::unique_ptr<Mover> myVehicle = getVehicle();
	if(myVehicle->getJumpRange() < myVehicle->distanceFrom(location))
		return(1);
	if(myVehicle->getObjectClass() == BATTLEMECH)
	{
		int32_t cellR, cellC;
		land->worldToCell(location, cellR, cellC);
		if(!GameMap->getPassable(cellR, cellC))
			return(1);
	}
	TacticalOrder tacOrder;
	tacOrder.init(OrderOriginType::commander, TacticalOrderCode::jumptopoint, false);
	tacOrder.selectionindex = -1;
	tacOrder.moveparams.fromArea = -1;
	tacOrder.moveparams.wayPath.points[0] = location.x;
	tacOrder.moveparams.wayPath.points[1] = location.y;
	tacOrder.moveparams.wayPath.points[2] = location.z;
	int32_t result = tacOrder.status(this);
	if(result == TACORDER_SUCCESS)
		return(1);
	if(curTacOrder.equals(&tacOrder))
		return(0);
	if(setOrder)
	{
		clearMoveOrders();
		clearAttackOrders();
		clearCurTacOrder();
	}
	if(setOrder)
	{
		setGeneralTacOrder(tacOrder);
		if(useGoalPlan)
			setMainGoal(GOAL_ACTION_MOVE, object, nullptr, -1.0);
		else
			setMainGoal(GOAL_ACTION_NONE, nullptr, nullptr, -1.0);
	}
	return(TACORDER_SUCCESS);
}

#endif

//---------------------------------------------------------------------------

int32_t
MechWarrior::coreEject(void)
{
	std::unique_ptr<Mover> myVehicle = getVehicle();
	if (myVehicle->getObjectClass() != BATTLEMECH)
		return (1);
	if (myVehicle->isDisabled())
		return (1);
	TacticalOrder tacOrder;
	tacOrder.init(OrderOriginType::commander, TacticalOrderCode::eject, false);
	// POINTLESS TO CHECK STATUS...
	// int32_t result = tacOrder.status(this);
	// if (result == TACORDER_SUCCESS)
	//	return(1);
	if (curTacOrder.equals(&tacOrder))
		return (0);
	//--------------------------
	// Order has no move goal...
	clearMoveOrders();
	//------------------------------
	// Order has no attack target...
	clearAttackOrders();
	myVehicle->handleEjection();
	setGeneralTacOrder(tacOrder);
	return (1);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::corePower(bool powerUp)
{
	std::unique_ptr<Mover> myVehicle = getVehicle();
	if (myVehicle->getObjectClass() != BATTLEMECH)
		return (1);
	if (myVehicle->isDisabled())
		return (1);
	if (powerUp)
	{
		if (myVehicle->getStatus() == OBJECT_STATUS_NORMAL)
			return (1);
		// if (myVehicle->getStatus() == OBJECT_STATUS_STARTING_UP)
		//	return(0);
	}
	else
	{
		// if (myVehicle->getStatus() == OBJECT_STATUS_SHUTTING_DOWN)
		//	return(0);
		if (myVehicle->getStatus() == OBJECT_STATUS_SHUTDOWN)
			return (1);
	}
	TacticalOrder tacOrder;
	tacOrder.init(
		OrderOriginType::commander, powerUp ? TacticalOrderCode::powerup : TacticalOrderCode::powerdown, false);
	int32_t result = tacOrder.status(this);
	if (result == TACORDER_SUCCESS)
		return (1);
	if (curTacOrder.equals(&tacOrder))
		return (0);
	//--------------------------
	// Order has no move goal...
	clearMoveOrders();
	//------------------------------
	// Order has no attack target...
	clearAttackOrders();
	//-------------------------------------------
	// Check if we've cooled enough to restart...
	if (myVehicle)
	{
		if (powerUp)
		{
			if (myVehicle->canPowerUp())
				myVehicle->startUp();
		}
		else
			myVehicle->shutDown();
	}
	setGeneralTacOrder(tacOrder);
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::coreAttack(GameObjectPtr target, uint32_t params)
{
	bool pursue = ((params & TACORDER_PARAM_PURSUE) != 0);
	bool obliterate = ((params & TACORDER_PARAM_OBLITERATE) != 0);
	// bool run = ((params & TACORDER_PARAM_RUN) != 0);
	bool jump = ((params & TACORDER_PARAM_JUMP) != 0);
	// bool scan = ((params & TACORDER_PARAM_SCAN) != 0);
	bool keepMoving = ((params & TACORDER_PARAM_DONT_KEEP_MOVING) == 0);
	int32_t attackMethod = AttackMethod::ranged;
	switch (params & TACORDER_ATTACK_MASK)
	{
	case TACORDER_PARAM_ATTACK_DFA:
		attackMethod = AttackMethod::dfa;
		break;
	case TACORDER_PARAM_ATTACK_RAMMING:
		attackMethod = AttackMethod::ramming;
		break;
	}
	int32_t attackRange = FireRangeType::optimal;
	switch (params & TACORDER_RANGE_MASK)
	{
	case TACORDER_PARAM_RANGE_RAMMING:
		attackRange = FireRangeType::ramming;
		break;
	case TACORDER_PARAM_RANGE_LONGEST:
		attackRange = FireRangeType::longest;
		break;
	case TACORDER_PARAM_RANGE_OPTIMAL:
		attackRange = FireRangeType::optimal;
		break;
	case TACORDER_PARAM_RANGE_SHORT:
		attackRange = FireRangeType::shortest;
		break;
	case TACORDER_PARAM_RANGE_MEDIUM:
		attackRange = FireRangeType::medium;
		break;
	case TACORDER_PARAM_RANGE_LONG:
		attackRange = FireRangeType::extended;
		break;
	}
	int32_t aimLocation = -1;
	switch (params & TACORDER_AIM_MASK)
	{
	case TACORDER_PARAM_AIM_HEAD:
		aimLocation = MECH_BODY_LOCATION_HEAD;
		break;
	case TACORDER_PARAM_AIM_ARM:
		aimLocation = MECH_BODY_LOCATION_RARM;
		break;
	case TACORDER_PARAM_AIM_LEG:
		aimLocation = MECH_BODY_LOCATION_RLEG;
		break;
	}
	int32_t tactic = TacticType::none;
	switch (params & TACORDER_TACTIC_MASK)
	{
	case TACORDER_PARAM_TACTIC_FLANK_RIGHT:
		tactic = TacticType::flankright;
		break;
	case TACORDER_PARAM_TACTIC_FLANK_LEFT:
		tactic = TacticType::flankleft;
		break;
	case TACORDER_PARAM_TACTIC_FLANK_REAR:
		tactic = TacticType::flankrear;
		break;
	case TACORDER_PARAM_TACTIC_STOP_AND_FIRE:
		tactic = TacticType::stopandfire;
		pursue = true;
		break;
	case TACORDER_PARAM_TACTIC_TURRET:
		tactic = TacticType::turret;
		pursue = false;
		jump = false;
		keepMoving = false;
		break;
	case TACORDER_PARAM_TACTIC_JOUST:
		tactic = TacticType::joust;
		break;
	}
	if ((getVehicle()->numWeapons == 0) && (attackMethod != AttackMethod::ramming))
	{
		clearAttackOrders();
		return (1);
	}
	if (!target)
	{
		if (!coreScanTargetWID)
			coreScan(nullptr, params);
		target = ObjectManager->getByWatchID(coreScanTargetWID);
	}
	LastCoreAttackTarget = target;
	if (!target)
		return (1);
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)OrderOriginType::commander, TacticalOrderCode::attackobject, false);
	tacOrder.targetWID = target->getWatchID();
	tacOrder.attackParams.method = (AttackMethod)attackMethod;
	tacOrder.attackParams.aimLocation = aimLocation;
	if (attackMethod == AttackMethod::ramming)
		attackRange = FireRangeType::ramming;
	tacOrder.attackParams.range = (FireRangeType)attackRange;
	tacOrder.attackParams.pursue = pursue;
	tacOrder.attackParams.obliterate = obliterate;
	tacOrder.attackParams.tactic = (TacticType)tactic;
	tacOrder.moveparams.keepMoving = keepMoving;
	tacOrder.moveparams.jump = jump;
	int32_t result = tacOrder.status(this);
	if (result == TACORDER_SUCCESS)
		return (1);
	if (curTacOrder.equals(&tacOrder))
		return (0);
	clearCurTacOrder();
	setSituationFireRange(getVehicle()->getOrderedFireRange(&attackRange));
	//------------------------------------------------------------
	// If we also want to pursue it, set up the move order here...
	if (pursue)
	{
		//------------------------------------------------------
		// Order moveToObject, making sure we face the object...
		params |= TACORDER_PARAM_FACE_OBJECT;
		// coreMoveToObject(target, params + TACORDER_PARAM_DONT_SET_ORDER);
	}
	else
		clearMoveOrders();
	//------------------------------------------------------------
	// Now, set up the tactical order, and pass it to the pilot...
	setAttackType(AttackType::destroy);
	setAttackTarget(target);
	setAttackAimLocation(aimLocation);
	setAttackPursuit(pursue);
	setSituationOpenFire(true);
	//-----------------------------------
	// Remember the most recent target...
	setLastTarget(target, obliterate);
	setGeneralTacOrder(tacOrder);
	if (useGoalPlan)
		setMainGoal(GOAL_ACTION_ATTACK, target, nullptr, -1.0);
	else
		setMainGoal(GOAL_ACTION_NONE, nullptr, nullptr, -1.0);
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::coreCapture(GameObjectPtr object, uint32_t params)
{
	if (!object)
	{
		if (!coreScanTargetWID)
			coreScan(nullptr, params);
		object = ObjectManager->getByWatchID(coreScanTargetWID);
	}
	if (!getTeam())
		return (1);
	if (!object || !object->isCaptureable(getTeam()->getId()) || object->isFriendly(getVehicle()))
		return (1);
	//-------------------------------------------------------------------------
	// Once Goal Planning is in, this should deal with the blockers rather than
	// abort the order...
	//	GameObjectPtr blockerList[MAX_MOVERS];
	//	int32_t numBlockers = object->getCaptureBlocker(getVehicle(),
	// blockerList); 	if (numBlockers) 		return(1);
	bool run = ((params & TACORDER_PARAM_RUN) != 0);
	bool jump = ((params & TACORDER_PARAM_JUMP) != 0);
	bool faceObject = ((params & TACORDER_PARAM_FACE_OBJECT) != 0);
	TacticalOrder tacOrder;
	tacOrder.init(OrderOriginType::commander, TacticalOrderCode::capture, false);
	tacOrder.targetWID = object->getWatchID();
	tacOrder.selectionindex = -1;
	tacOrder.moveparams.fromArea = -1;
	tacOrder.moveparams.wayPath.mode[0] = (run ? TravelModeType::fast : TravelModeType::slow);
	tacOrder.moveparams.faceObject = faceObject;
	tacOrder.moveparams.wait = false;
	tacOrder.moveparams.keepMoving = false;
	tacOrder.moveparams.jump = jump;
	int32_t result = tacOrder.status(this);
	if (result == TACORDER_SUCCESS)
		return (1);
	if (curTacOrder.equals(&tacOrder))
		return (0);
	setGeneralTacOrder(tacOrder);
	if (useGoalPlan)
		setMainGoal(GOAL_ACTION_CAPTURE, object, nullptr, 250.0);
	else
		setMainGoal(GOAL_ACTION_NONE, nullptr, nullptr, -1.0);
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::coreScan(GameObjectPtr object, uint32_t params)
{
	if (object && object->getTeam())
	{
		//--------------------------------------------------------------
		// If this object is currently a contact, set myTarget to it and
		// return true. Otherwise, return false.
		if (object->isMover())
		{
			if (getTeam()->isContact(getVehicle(), (std::unique_ptr<Mover>)object, (int32_t)params))
			{
				coreScanTargetWID = object->getWatchID();
				return (coreScanTargetWID);
			}
		}
		else
		{
			if (/*CurWarrior->*/ getVehicle()->lineOfSight(object))
			{
				coreScanTargetWID = object->getWatchID();
				return (coreScanTargetWID);
			}
		}
	}
	else
	{
		//-------------------------------------------
		// Scan for anything matching the criteria...
		Stuff::Vector3D myPosition = getVehicle()->getPosition();
		int32_t action = -1;
		coreScanTargetWID = targetPriorityList.calcTarget(this, myPosition, params, action);
		return (coreScanTargetWID);
	}
	return (coreScanTargetWID = 0);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::coreControl(GameObjectPtr object, uint32_t params)
{
	if (!object)
	{
		if (!coreScanTargetWID)
			coreScan(nullptr, params);
		object = ObjectManager->getByWatchID(coreScanTargetWID);
	}
	if (!object)
		return (1);
	int32_t action = targetPriorityList.calcAction(this, object);
	if (action == TacticalOrderCode::attackobject)
		return (coreAttack(object, params));
	else if (action == TacticalOrderCode::capture)
		return (coreCapture(object, params));
	return (1);
}

//---------------------------------------------------------------------------

#if 0

int32_t MechWarrior::coreWithdraw(Stuff::Vector3D location, uint32_t params)
{
	bool run = ((params & TACORDER_PARAM_RUN) != 0);
	TacticalOrder tacOrder;
	tacOrder.init(OrderOriginType::commander, TacticalOrderCode::withdraw, false);
	tacOrder.moveparams.wayPath.points[0] = location.x;
	tacOrder.moveparams.wayPath.points[1] = location.y;
	tacOrder.moveparams.wayPath.points[2] = location.z;
	int32_t result = tacOrder.status(this);
	if(result == TACORDER_SUCCESS)
		return(1);
	if(curTacOrder.equals(&tacOrder))
		return(0);
	clearCurTacOrder();
	//location = calcWithdrawGoal();
	//result = orderMoveToPoint(false, false/*true*/, OrderOriginType::commander, location, -1, run ? TACORDER_PARAM_RUN : TACORDER_PARAM_NONE);
	std::unique_ptr<Mover> myVehicle = getVehicle();
	Assert(myVehicle != nullptr, 0, " orderWithdraw:Warrior has no Vehicle ");
	myVehicle->withdrawing = true;
	setGeneralTacOrder(tacOrder);
	if(useGoalPlan)
		setMainGoal(GOAL_ACTION_MOVE, nullptr, &location, -1.0);
	else
		setMainGoal(GOAL_ACTION_NONE, nullptr, nullptr, -1.0);
	curTacOrder.code = TacticalOrderCode::withdraw;
	return(0);
}

#endif

//---------------------------------------------------------------------------

int32_t
MechWarrior::coreSetState(int32_t stateID, bool thinkAgain)
{
	return (0);
}

//---------------------------------------------------------------------------
// CORE TACTICAL ORDERS
//---------------------------------------------------------------------------

int32_t
MechWarrior::orderWait(
	bool unitOrder, int32_t origin, int32_t seconds, bool clearLastTarget)
{
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::wait, unitOrder);
	tacOrder.delayedTime = scenarioTime + seconds;
	//--------------------------
	// Order has no move goal...
	clearMoveOrders();
	//------------------------------
	// Order has no attack target...
	clearAttackOrders();
	if (clearLastTarget)
		setLastTarget(nullptr);
	if (origin == OrderOriginType::commander)
		setGeneralTacOrder(tacOrder);
	return (tacOrder.status(this));
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderStop(bool unitOrder, bool setTacOrder)
{
	TacticalOrder tacOrder;
	tacOrder.init(OrderOriginType::player, TacticalOrderCode::stop, unitOrder);
	clearTacOrderQueue();
	//--------------------------
	// Order has no move goal...
	clearMoveOrders();
	//------------------------------
	// Order has no attack target...
	clearAttackOrders();
	//	if (clearLastTarget)
	setLastTarget(nullptr);
	//	if (origin == OrderOriginType::commander)
	//		setGeneralTacOrder(tacOrder);
	getVehicle()->suppressionFire = 0;
	return (tacOrder.status(this));
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderMoveToPoint(bool unitOrder, bool setTacOrder, int32_t origin,
	Stuff::Vector3D location, int32_t selectionindex, uint32_t params)
{
	bool jump = ((params & TACORDER_PARAM_JUMP) != 0);
	bool run = ((params & TACORDER_PARAM_RUN) != 0);
	bool wait = ((params & TACORDER_PARAM_WAIT) != 0);
	bool layMines = ((params & TACORDER_PARAM_LAY_MINES) != 0);
	bool escape = ((params & TACORDER_PARAM_ESCAPE_TILE) != 0);
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::movetopoint, unitOrder);
	tacOrder.moveparams.wayPath.points[0] = location.x;
	tacOrder.moveparams.wayPath.points[1] = location.y;
	tacOrder.moveparams.wayPath.points[2] = location.z;
	tacOrder.selectionindex = selectionindex;
	tacOrder.moveparams.wayPath.mode[0] = (run ? TravelModeType::fast : TravelModeType::slow);
	tacOrder.moveparams.wait = wait;
	tacOrder.moveparams.mode = (layMines ? SpecialMoveMode::minelaying : SpecialMoveMode::normal);
	tacOrder.moveparams.escapeTile = escape;
	tacOrder.moveparams.jump = jump;
	int32_t result = tacOrder.status(this);
	if (result == TACORDER_SUCCESS)
		return (TACORDER_SUCCESS);
	// clearMoveOrders(which);
	setMoveGoal(MOVEGOAL_NONE, nullptr);
	setMoveWayPath(nullptr, 0);
	// for (size_t i = 0; i < 2; i++)
	//	clearMovePath(which, i);
	// setMoveState(which, MOVESTATE_FORWARD);
	// setMoveStateGoal(which, MOVESTATE_FORWARD);
	// setMoveYieldTime(which, -1.0);
	// setMoveYieldState(which, 0);
	// setMoveGlobalPath(which, nullptr, 0);
	setMoveGoal(MOVEGOAL_LOCATION, &location);
	moveOrders.timeOfLastStep = scenarioTime;
	setMoveRun(run);
	//-----------------------------------
	// Move order has no attack target...
	if (setTacOrder)
		clearAttackOrders();
	uint32_t moveparams = MOVEPARAM_INIT | MOVEPARAM_FACE_TARGET;
	if (escape)
		moveparams |= MOVEPARAM_ESCAPE_TILE;
	if (jump)
		moveparams |= MOVEPARAM_JUMP;
	if (setTacOrder && (origin == OrderOriginType::player))
		if (!unitOrder || (getPoint() == getVehicle()))
			moveparams |= MOVEPARAM_RADIO_RESULT;
	PathManager->request(this, selectionindex, moveparams, setTacOrder ? 21 : 23);
	if (setTacOrder && (result == TACORDER_FAILURE) && (origin == OrderOriginType::commander))
		setGeneralTacOrder(tacOrder);
	return (result);
}

//---------------------------------------------------------------------------

void MechWarrior::clearMovePath(int32_t pathNum)
{
	if (moveOrders.path[pathNum])
	{
		if (moveOrders.path[pathNum]->numStepsWhenNotPaused > 0)
			int32_t i = 0;
		moveOrders.path[pathNum]->clear();
	}
}

int32_t
MechWarrior::orderMoveToObject(bool unitOrder, bool setTacOrder, int32_t origin,
	GameObjectPtr target, int32_t fromArea, int32_t selectionindex, uint32_t params)
{
	bool jump = ((params & TACORDER_PARAM_JUMP) != 0);
	bool run = ((params & TACORDER_PARAM_RUN) != 0);
	bool faceObject = ((params & TACORDER_PARAM_FACE_OBJECT) != 0);
	bool layMines = ((params & TACORDER_PARAM_LAY_MINES) != 0);
	if (!target)
		return (TACORDER_SUCCESS);
	//-------------------------------------------------
	// Pick the best move goal based upon the object...
	Stuff::Vector3D goal;
	if (target->isMover())
		goal = target->getPosition();
	else
	{
		//------------------------------------------------
		// If not a mover, then pick an open cell adjacent
		// to it. If it happens to be adjacent to blocked
		// cells, this could be off...
		if (target->isBuilding())
		{
			BuildingPtr building = (BuildingPtr)target;
			int32_t goalRow = 0, goalCol = 0;
			bool foundGoal =
				building->calcAdjacentAreaCell(getVehicle()->moveLevel, fromArea, goalRow, goalCol);
			if (foundGoal)
				land->cellToWorld(goalRow, goalCol, goal);
			else
			{
				Stuff::Vector3D objectPos = target->getPosition();
				goal = getVehicle()->calcOffsetMoveGoal(objectPos);
			}
		}
		else
		{
			Stuff::Vector3D objectPos = target->getPosition();
			goal = getVehicle()->calcOffsetMoveGoal(objectPos);
		}
	}
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::movetoobject, unitOrder);
	tacOrder.targetWID = target->getWatchID();
	tacOrder.selectionindex = selectionindex;
	tacOrder.moveparams.fromArea = fromArea;
	tacOrder.moveparams.wayPath.points[0] = goal.x;
	tacOrder.moveparams.wayPath.points[1] = goal.y;
	tacOrder.moveparams.wayPath.points[2] = goal.z;
	tacOrder.moveparams.wayPath.mode[0] = (run ? TravelModeType::fast : TravelModeType::slow);
	tacOrder.moveparams.faceObject = faceObject;
	tacOrder.moveparams.wait = false;
	tacOrder.moveparams.mode = (layMines ? SpecialMoveMode::minelaying : SpecialMoveMode::normal);
	tacOrder.moveparams.jump = jump;
	int32_t result = tacOrder.status(this);
	if (result == TACORDER_SUCCESS)
		return (TACORDER_SUCCESS);
	//-----------------------------------
	// Is this already our current order?
	// if ((curTacOrder.code == TacticalOrderCode::movetoobject) &&
	//	(curTacOrder.params.move.targetId == target->getIdNumber()) &&
	//	(curTacOrder.params.move.selectionindex == selectionindex))
	//	return(TACORDER_FAILURE);
	tacOrder.targetWID = target->getWatchID();
	Stuff::Vector3D pos = target->getPosition();
	setMoveGoal(target->getWatchID(), &pos, target);
	setMoveRun(run);
	//-----------------------------------
	// Move order has no attack target...
	if (setTacOrder)
		clearAttackOrders();
	uint32_t moveparams = MOVEPARAM_INIT;
	if (jump)
		moveparams |= MOVEPARAM_JUMP;
	if (faceObject)
		moveparams |= MOVEPARAM_FACE_TARGET;
	if (setTacOrder && !target->isMover())
		moveparams |= MOVEPARAM_STEP_ADJACENT_TARGET;
	if (setTacOrder && (origin == OrderOriginType::player))
		if (!unitOrder || (getPoint() == getVehicle()))
			moveparams |= MOVEPARAM_RADIO_RESULT;
	requestMovePath(selectionindex, moveparams, 16);
	moveOrders.goalObjectPosition = target->getPosition();
	if (setTacOrder && (result == TACORDER_FAILURE) && (origin == OrderOriginType::commander))
		setGeneralTacOrder(tacOrder);
	return (result);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderJumpToPoint(bool unitOrder, bool setTacOrder, int32_t origin,
	Stuff::Vector3D location, int32_t selectionindex)
{
	std::unique_ptr<Mover> myVehicle = getVehicle();
	if (myVehicle->getJumpRange() < myVehicle->distanceFrom(location))
		return (TACORDER_SUCCESS);
	if (myVehicle->getObjectClass() == BATTLEMECH)
	{
		int32_t cellR, cellC;
		land->worldToCell(location, cellR, cellC);
		if (!GameMap->getPassable(cellR, cellC))
			return (TACORDER_SUCCESS);
	}
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::jumptopoint, unitOrder);
	tacOrder.selectionindex = selectionindex;
	tacOrder.moveparams.wayPath.points[0] = location.x;
	tacOrder.moveparams.wayPath.points[1] = location.y;
	tacOrder.moveparams.wayPath.points[2] = location.z;
	int32_t result = tacOrder.status(this);
	if (result == TACORDER_SUCCESS)
		return (TACORDER_SUCCESS);
	//-----------------------------------
	// Move order has no attack target...
	if (setTacOrder)
	{
		clearMoveOrders();
		clearAttackOrders();
		// setLastTarget(nullptr);
	}
	if (setTacOrder && (result == TACORDER_FAILURE) && (origin == OrderOriginType::commander))
		setGeneralTacOrder(tacOrder);
	return (TACORDER_SUCCESS);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderJumpToObject(
	bool unitOrder, bool setTacOrder, int32_t origin, GameObjectPtr target, int32_t selectionindex)
{
	std::unique_ptr<Mover> myVehicle = getVehicle();
	Stuff::Vector3D location = target->getPosition();
	if (target->isMover())
	{
		if (target->getTeam() == myVehicle->getTeam())
			return (TACORDER_SUCCESS);
	}
	if (myVehicle->getJumpRange() < myVehicle->distanceFrom(location))
		return (TACORDER_SUCCESS);
	if (myVehicle->getObjectClass() == BATTLEMECH)
	{
		int32_t cellR, cellC;
		land->worldToCell(location, cellR, cellC);
		if (!GameMap->getPassable(cellR, cellC))
			return (TACORDER_SUCCESS);
	}
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::jumptopoint, unitOrder);
	tacOrder.selectionindex = selectionindex;
	tacOrder.moveparams.wayPath.points[0] = location.x;
	tacOrder.moveparams.wayPath.points[1] = location.y;
	tacOrder.moveparams.wayPath.points[2] = location.z;
	tacOrder.targetWID = target->getWatchID();
	int32_t result = tacOrder.status(this);
	if (result == TACORDER_SUCCESS)
		return (TACORDER_SUCCESS);
	//-----------------------------------
	// Move order has no attack target...
	if (setTacOrder)
	{
		clearMoveOrders();
		clearAttackOrders();
	}
	if (setTacOrder && (result == TACORDER_FAILURE) && (origin == OrderOriginType::commander))
		setGeneralTacOrder(tacOrder);
	return (TACORDER_SUCCESS);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderTraversePath(
	bool unitOrder, bool setTacOrder, int32_t origin, WayPathPtr wayPath, uint32_t params)
{
	bool layMines = ((params & TACORDER_PARAM_LAY_MINES) != 0);
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::traversepath, unitOrder);
	memcpy(&tacOrder.moveparams.wayPath, wayPath, sizeof(WayPath));
	tacOrder.moveparams.mode = (layMines ? SpecialMoveMode::minelaying : SpecialMoveMode::normal);
	int32_t result = tacOrder.status(this);
	if (result == TACORDER_SUCCESS)
		return (TACORDER_SUCCESS);
	Stuff::Vector3D location;
	location.x = tacOrder.moveparams.wayPath.points[0];
	location.y = tacOrder.moveparams.wayPath.points[1];
	location.z = tacOrder.moveparams.wayPath.points[2];
	setMoveGoal(MOVEGOAL_LOCATION, &location);
	setMoveWayPath(wayPath, false);
	//-----------------------------------
	// Move order has no attack target...
	if (setTacOrder)
		clearAttackOrders();
	requestMovePath(-1, MOVEPARAM_INIT | MOVEPARAM_FACE_TARGET, 17);
	if (setTacOrder && (result == TACORDER_FAILURE) && (origin == OrderOriginType::commander))
		setGeneralTacOrder(tacOrder);
	return (result);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderPatrolPath(
	bool unitOrder, bool setTacOrder, int32_t origin, WayPathPtr wayPath)
{
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::patrolpath, unitOrder);
	memcpy(&tacOrder.moveparams.wayPath, wayPath, sizeof(WayPath));
	int32_t result = tacOrder.status(this);
	if (result == TACORDER_SUCCESS)
		return (TACORDER_SUCCESS);
	Stuff::Vector3D location;
	location.x = tacOrder.moveparams.wayPath.points[0];
	location.y = tacOrder.moveparams.wayPath.points[1];
	location.z = tacOrder.moveparams.wayPath.points[2];
	setMoveGoal(MOVEGOAL_LOCATION, &location);
	setMoveWayPath(wayPath, true);
	//-----------------------------------
	// Move order has no attack target...
	if (setTacOrder)
		clearAttackOrders();
	requestMovePath(-1, MOVEPARAM_INIT | MOVEPARAM_FACE_TARGET, 18);
	if (setTacOrder && (result == TACORDER_FAILURE) && (origin == OrderOriginType::commander))
		setGeneralTacOrder(tacOrder);
	return (result);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderPowerUp(bool unitOrder, int32_t origin)
{
	std::unique_ptr<Mover> myVehicle = getVehicle();
	if ((myVehicle->getStatus() != OBJECT_STATUS_SHUTDOWN))
		return TACORDER_SUCCESS;
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::powerup, unitOrder);
	int32_t result = tacOrder.status(this);
	if (result == TACORDER_SUCCESS)
		return (TACORDER_SUCCESS);
	//--------------------------
	// Order has no move goal...
	clearMoveOrders();
	//------------------------------
	// Order has no attack target...
	clearAttackOrders();
	//-------------------------------------------
	// Check if we've cooled enough to restart...
	if (myVehicle)
		if (myVehicle->canPowerUp())
			myVehicle->startUp();
	if (origin == OrderOriginType::commander)
		setGeneralTacOrder(tacOrder);
	else if (origin == OrderOriginType::self)
		setAlarmTacOrder(tacOrder, 255);
	return (result);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderPowerDown(bool unitOrder, int32_t origin)
{
	std::unique_ptr<Mover> myVehicle = getVehicle();
	if ((myVehicle->getStatus() == OBJECT_STATUS_SHUTDOWN) || (myVehicle->getStatus() == OBJECT_STATUS_SHUTTING_DOWN))
		return (TACORDER_SUCCESS);
	if (myVehicle->isDisabled())
		return (TACORDER_SUCCESS);
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::powerdown, unitOrder);
	int32_t result = tacOrder.status(this);
	if (result == TACORDER_SUCCESS)
		return (TACORDER_SUCCESS);
	//--------------------------
	// Order has no move goal...
	clearMoveOrders();
	//------------------------------
	// Order has no attack target...
	clearAttackOrders();
	if (myVehicle)
		myVehicle->shutDown();
	if (origin == OrderOriginType::commander)
		setGeneralTacOrder(tacOrder);
	return (result);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderUseSpeed(float speed)
{
	//-----------------------------------------------------------
	// The speed we're given is in meters/sec. We need to convert
	// this to our proper "sprite speed", using gesture state and
	// (if appropriate) throttle setting...
	moveOrders.speedVelocity = speed;
	std::unique_ptr<Mover> myVehicle = getVehicle();
	int32_t state, throttle;
	myVehicle->calcSpriteSpeed(speed, 0, state, throttle);
	moveOrders.speedState = state;
	moveOrders.speedThrottle = throttle;
	return (TACORDER_SUCCESS);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderOrbitPoint(Stuff::Vector3D location)
{
	return (1);
}

//---------------------------------------------------------------------------

int32_t
orderOrbitObject(GameObjectPtr target)
{
	return (1);
}

//---------------------------------------------------------------------------

int32_t
orderUseOrbitRange(int32_t type, float range)
{
	return (1);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderAttackObject(bool unitOrder, int32_t origin, GameObjectPtr target,
	int32_t type, int32_t method, int32_t range, int32_t aimLocation, int32_t fromArea,
	uint32_t params)
{
	bool run = ((params & TACORDER_PARAM_RUN) != 0);
	bool jump = ((params & TACORDER_PARAM_JUMP) != 0);
	bool pursue = ((params & TACORDER_PARAM_PURSUE) != 0);
	bool obliterate = ((params & TACORDER_PARAM_OBLITERATE) != 0);
	bool conserve = (type == AttackType::conservingammo);
	bool keepMoving = ((params & TACORDER_PARAM_DONT_KEEP_MOVING) == 0);
	int32_t tactic = TacticType::none;
	switch (params & TACORDER_TACTIC_MASK)
	{
	case TACORDER_PARAM_TACTIC_FLANK_RIGHT:
		tactic = TacticType::flankright;
		break;
	case TACORDER_PARAM_TACTIC_FLANK_LEFT:
		tactic = TacticType::flankleft;
		break;
	case TACORDER_PARAM_TACTIC_FLANK_REAR:
		tactic = TacticType::flankrear;
		break;
	case TACORDER_PARAM_TACTIC_STOP_AND_FIRE:
		tactic = TacticType::stopandfire;
		pursue = true;
		break;
	case TACORDER_PARAM_TACTIC_TURRET:
		tactic = TacticType::turret;
		jump = false;
		pursue = false;
		keepMoving = false;
		break;
	case TACORDER_PARAM_TACTIC_JOUST:
		tactic = TacticType::joust;
		break;
	}
	if (getVehicle()->attackRange == FireRangeType::current)
	{
		range = FireRangeType::current;
		pursue = false;
		jump = false;
	}
	if (!target)
	{
		//-----------------------
		// Clear Attack orders...
		clearAttackOrders();
		return (TACORDER_SUCCESS);
	}
	if ((getVehicle()->numWeapons == 0) && (method != AttackMethod::ramming))
	{
		clearAttackOrders();
		return (TACORDER_SUCCESS);
	}
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::attackobject, unitOrder);
	tacOrder.targetWID = target->getWatchID();
	tacOrder.attackParams.type = (AttackType)type;
	tacOrder.attackParams.method = (AttackMethod)method;
	tacOrder.attackParams.aimLocation = aimLocation;
	tacOrder.attackParams.tactic = (TacticType)tactic;
	if (method == AttackMethod::ramming)
		range = FireRangeType::ramming;
	tacOrder.attackParams.range = (FireRangeType)range;
	tacOrder.attackParams.pursue = pursue;
	tacOrder.attackParams.obliterate = obliterate;
	tacOrder.moveparams.fromArea = fromArea;
	tacOrder.moveparams.jump = jump;
	tacOrder.moveparams.keepMoving = keepMoving;
	int32_t result = tacOrder.status(this);
	if (result == TACORDER_SUCCESS)
		return (TACORDER_SUCCESS);
	if (curTacOrder.equals(&tacOrder))
		return (0);
	setSituationFireRange(getVehicle()->getOrderedFireRange(&range));
	//------------------------------------------------------------
	// If we also want to pursue it, set up the move order here...
	if (pursue)
	{
		//------------------------------------------------------
		// Order moveToObject, making sure we face the object...
		setMoveRun(run);
		params |= TACORDER_PARAM_FACE_OBJECT;
		// orderMoveToObject(unitOrder, false, origin, target, fromArea,
		// -1/*unitMateId*/, params);
	}
	else
		clearMoveOrders();
	//------------------------------------------------------------
	// Now, set up the tactical order, and pass it to the pilot...
	setAttackType(type);
	setAttackTarget(target);
	setAttackAimLocation(aimLocation);
	setAttackPursuit(pursue);
	setSituationOpenFire(true);
	//-----------------------------------
	// Remember the most recent target...
	setLastTarget(target, obliterate, conserve);
	if (origin == OrderOriginType::commander)
		setGeneralTacOrder(tacOrder);
	/*
	if (GameSystemWindow) {
		wchar_t debugStr[200];
		GameSystemWindow->print("");
		GameSystemWindow->print("-----------------------------------");
		sprintf(debugStr, "%s:", name);
		GameSystemWindow->print(debugStr);
		sprintf(debugStr, "Longest Range Weapon = %s (%.4f)",
			MasterComponentList[(std::unique_ptr<Mover>(BaseObjectPtr(vehicle)))->inventory[(std::unique_ptr<Mover>(BaseObjectPtr(vehicle)))->longestRangeWeapon].masterID].getName(),
			MasterComponentList[(std::unique_ptr<Mover>(BaseObjectPtr(vehicle)))->inventory[(std::unique_ptr<Mover>(BaseObjectPtr(vehicle)))->longestRangeWeapon].masterID].getWeaponLongRange());
		GameSystemWindow->print(debugStr);
		sprintf(debugStr, "Optimal Range = %.4f",
	(std::unique_ptr<Mover>(BaseObjectPtr(vehicle)))->optimalRange);
		GameSystemWindow->print(debugStr);
		GameSystemWindow->print("-----------------------------------");
	}
	*/
	return (TACORDER_FAILURE);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderAttackPoint(bool unitOrder, int32_t origin, Stuff::Vector3D location,
	int32_t type, int32_t method, int32_t range, uint32_t params)
{
	bool pursue = ((params & TACORDER_PARAM_PURSUE) != 0);
	bool jump = ((params & TACORDER_PARAM_JUMP) != 0);
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::attackpoint, unitOrder);
	tacOrder.attackParams.type = (AttackType)type;
	tacOrder.attackParams.method = (AttackMethod)method;
	tacOrder.attackParams.range = (FireRangeType)range;
	tacOrder.attackParams.pursue = pursue;
	tacOrder.attackParams.targetpoint = location;
	// tacOrder.moveparams.fromArea = fromArea;
	tacOrder.moveparams.jump = jump;
	int32_t result = tacOrder.status(this);
	if (result == TACORDER_SUCCESS)
		return (TACORDER_SUCCESS);
	setSituationFireRange(getVehicle()->getOrderedFireRange(&range));
	//------------------------------------------------------------
	// If we also want to pursue it, set up the move order here...
	if (pursue)
		orderMoveToPoint(unitOrder, false, origin, location, -1 /*unitMateId*/, params);
	else
		clearMoveOrders();
	//------------------------------------------------------------
	// Now, set up the tactical order, and pass it to the pilot...
	setAttackType(type);
	setAttackTarget(nullptr);
	setAttackTargetPoint(location);
	setAttackAimLocation(-1);
	setAttackPursuit(pursue);
	setSituationOpenFire(true);
	//-----------------------------------
	// Remember the most recent target...
	setLastTarget(nullptr);
	if (origin == OrderOriginType::commander)
		setGeneralTacOrder(tacOrder);
	/*
	if (GameSystemWindow) {
		wchar_t debugStr[200];
		GameSystemWindow->print("");
		GameSystemWindow->print("-----------------------------------");
		sprintf(debugStr, "%s:", name);
		GameSystemWindow->print(debugStr);
		sprintf(debugStr, "Longest Range Weapon = %s (%.4f)",
			MasterComponentList[(std::unique_ptr<Mover>(BaseObjectPtr(vehicle)))->inventory[(std::unique_ptr<Mover>(BaseObjectPtr(vehicle)))->longestRangeWeapon].masterID].getName(),
			MasterComponentList[(std::unique_ptr<Mover>(BaseObjectPtr(vehicle)))->inventory[(std::unique_ptr<Mover>(BaseObjectPtr(vehicle)))->longestRangeWeapon].masterID].getWeaponLongRange());
		GameSystemWindow->print(debugStr);
		sprintf(debugStr, "Optimal Range = %.4f",
	(std::unique_ptr<Mover>(BaseObjectPtr(vehicle)))->optimalRange);
		GameSystemWindow->print(debugStr);
		GameSystemWindow->print("-----------------------------------");
	}
	*/
	return (TACORDER_FAILURE);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderWithdraw(bool unitOrder, int32_t origin, Stuff::Vector3D location)
{
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::withdraw, unitOrder);
	tacOrder.moveparams.wayPath.points[0] = location.x;
	tacOrder.moveparams.wayPath.points[1] = location.y;
	tacOrder.moveparams.wayPath.points[2] = location.z;
	location = calcWithdrawGoal();
	int32_t result = orderMoveToPoint(unitOrder, true, origin, location, -1, TACORDER_PARAM_RUN);
	std::unique_ptr<Mover> myVehicle = getVehicle();
	Assert(myVehicle != nullptr, 0, " orderWithdraw:Warrior has no Vehicle ");
	myVehicle->withdrawing = true;
	if (origin == OrderOriginType::commander)
		setGeneralTacOrder(tacOrder);
	curTacOrder.code = TacticalOrderCode::withdraw;
	return (result);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderEject(bool unitOrder, bool setTacOrder, int32_t origin)
{
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::eject, unitOrder);
	std::unique_ptr<Mover> myVehicle = getVehicle();
	Assert(myVehicle != nullptr, 0, " orderWithdraw:Warrior has no Vehicle ");
	myVehicle->handleEjection();
	if (origin == OrderOriginType::commander)
		setGeneralTacOrder(tacOrder);
	return (TACORDER_SUCCESS);
}

//---------------------------------------------------------------------------

#if 0

int32_t MechWarrior::orderUseFireOdds(int32_t odds)
{
	setSituationFireOdds(FireOddsTable[odds]);
	return(TACORDER_SUCCESS);
}

#endif

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderRefit(int32_t origin, GameObjectPtr target, uint32_t params)
{
	if (!target || target->getObjectClass() != BATTLEMECH)
		return (TACORDER_SUCCESS);
	bool run = ((params & TACORDER_PARAM_RUN) != 0);
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::refit, false);
	tacOrder.targetWID = target->getWatchID();
	tacOrder.selectionindex = -1;
	tacOrder.moveparams.wayPath.mode[0] = (run ? TravelModeType::fast : TravelModeType::slow);
	tacOrder.moveparams.faceObject = true;
	tacOrder.moveparams.wait = false;
	if (origin == OrderOriginType::commander)
		setGeneralTacOrder(tacOrder);
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderRecover(int32_t origin, GameObjectPtr target, uint32_t params)
{
	if (!target || target->getObjectClass() != BATTLEMECH)
		return (TACORDER_SUCCESS);
	bool run = ((params & TACORDER_PARAM_RUN) != 0);
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::recover, false);
	tacOrder.targetWID = target->getWatchID();
	tacOrder.selectionindex = -1;
	tacOrder.moveparams.wayPath.mode[0] = (run ? TravelModeType::fast : TravelModeType::slow);
	tacOrder.moveparams.faceObject = true;
	tacOrder.moveparams.wait = false;
	if (origin == OrderOriginType::commander)
		setGeneralTacOrder(tacOrder);
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderGetFixed(int32_t origin, GameObjectPtr target, uint32_t params)
{
	if (!target || target->getObjectClass() != TREEBUILDING && target->getRefitPoints() <= 0.0)
		return (TACORDER_SUCCESS);
#ifdef USE_BUILDINGS
	if ((getVehicle()->getObjectClass() == BATTLEMECH && ((TreeBuildingPtr)target)->mechBay == false) || (getVehicle()->getObjectClass() == GROUNDVEHICLE && ((TreeBuildingPtr)target)->mechBay == true))
		return (TACORDER_SUCCESS);
#endif
	bool run = ((params & TACORDER_PARAM_RUN) != 0);
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::getfixed, false);
	tacOrder.targetWID = target->getWatchID();
	tacOrder.selectionindex = -1;
	tacOrder.moveparams.wayPath.mode[0] = (run ? TravelModeType::fast : TravelModeType::slow);
	tacOrder.moveparams.faceObject = true;
	tacOrder.moveparams.wait = false;
	if (origin == OrderOriginType::commander)
		setGeneralTacOrder(tacOrder);
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderLoadIntoCarrier(int32_t origin, GameObjectPtr target, uint32_t params)
{
#ifdef USE_ELEMENTALS
	if ((getVehicle()->getObjectClass() != ELEMENTAL) || !target || (target->getObjectClass() != GROUNDVEHICLE) || (((GroundVehiclePtr)target)->elementalCarrier == false))
		return (TACORDER_SUCCESS);
#endif
	bool run = ((params & TACORDER_PARAM_RUN) != 0);
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::loadintocarrier, false);
	tacOrder.targetWID = target->getWatchID();
	tacOrder.selectionindex = -1;
	tacOrder.moveparams.wayPath.mode[0] = (run ? TravelModeType::fast : TravelModeType::slow);
	tacOrder.moveparams.faceObject = true;
	tacOrder.moveparams.wait = false;
	if (origin == OrderOriginType::commander)
		setGeneralTacOrder(tacOrder);
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderDeployElementals(int32_t origin, uint32_t params)
{
#ifdef USE_ELEMENTALS
	if ((getVehicle()->getObjectClass() != GROUNDVEHICLE) || (((GroundVehiclePtr)getVehicle())->elementalCarrier == false))
		return (TACORDER_SUCCESS);
#endif
	bool run = ((params & TACORDER_PARAM_RUN) != 0);
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::deployelementals, false);
	tacOrder.moveparams.wayPath.mode[0] = (run ? TravelModeType::fast : TravelModeType::slow);
	tacOrder.moveparams.wait = false;
	if (origin == OrderOriginType::commander)
		setGeneralTacOrder(tacOrder);
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::orderCapture(
	int32_t origin, GameObjectPtr target, int32_t fromArea, uint32_t params)
{
	if (!target || /*!target->isCaptureable(getTeam()->getId()) ||*/
		target->isFriendly(getVehicle()) || target->getCaptureBlocker(getVehicle()) != nullptr)
		return (1);
	bool run = ((params & TACORDER_PARAM_RUN) != 0);
	bool jump = ((params & TACORDER_PARAM_JUMP) != 0);
	TacticalOrder tacOrder;
	tacOrder.init((OrderOriginType)origin, TacticalOrderCode::capture, false);
	tacOrder.targetWID = target->getWatchID();
	tacOrder.selectionindex = -1;
	tacOrder.moveparams.wayPath.mode[0] = (run ? TravelModeType::fast : TravelModeType::slow);
	tacOrder.moveparams.faceObject = true;
	tacOrder.moveparams.wait = false;
	tacOrder.moveparams.fromArea = fromArea;
	tacOrder.moveparams.jump = jump;
	if (origin == OrderOriginType::commander)
		setGeneralTacOrder(tacOrder);
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
// ALARM EVENTS
//---------------------------------------------------------------------------

int32_t
MechWarrior::handleTargetOfWeaponFire(void)
{
	// uint32_t attackerId = alarm[PILOT_ALARM_TARGET_OF_WEAPONFIRE].trigger[0];
	// GameObjectPtr myVehicle = getVehicle();
#ifdef USE_IFACE
	if (myVehicle)
		theInterface->ObjectAttacked(myVehicle->getPartId());
#endif
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleHitByWeaponFire(void)
{
	int32_t attackerWID = alarm[PILOT_ALARM_HIT_BY_WEAPONFIRE].trigger[0];
	if (attackerWID == 0)
	{
		//----------------
		// Ammo Explosion?
	}
	else if (attackerWID == -3)
		radioMessage(RADIO_UNDER_AIRSTRIKE, true);
	else if (attackerWID == -2)
		radioMessage(RADIO_HITTING_MINES, true);
	else
	{
		int32_t targetWID = 0;
		if (getCurTacOrder()->code != TacticalOrderCode::none)
			targetWID = getCurTacOrder()->targetWID;
		if (!targetWID)
			radioMessage(RADIO_UNDER_ATTACK, true);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleCollision(void)
{
	//--------------------------------------------
	// Colliding with trees should not get here...
	// int32_t colliderHandle = alarm[PILOT_ALARM_COLLISION].trigger[0];
	// GameObjectPtr collider = ObjectManager->get(colliderHandle);
#if 0
	//----------------------------------------------------------
	// If we're Clan, we'll fire back. IS ignore this for now...
	if(alignment == INNER_SPHERE)
		return(NO_ERROR);
	//---------------------------------------------------------
	// Fire back on the attacker ONLY if I'm Clan and currently
	// have no target...
	if(colliderObjectClass != BUILDING)
	{
		if(getAttackTargetId(OrderType::current))
			return(NO_ERROR);
		if(colliderId != getAttackTargetId(OrderType::current))
		{
			TacticalOrder tacOrder;
			tacOrder.init(OrderOriginType::self, TacticalOrderCode::attackobject);
			tacOrder.params.attack.targetId = colliderId;
			tacOrder.params.attack.range = FireRangeType::shortest;
			tacOrder.params.attack.pursue = true;
			tacOrder.params.attack.type = AttackType::destroy;
			setAlarmTacOrder(tacOrder, 10);
		}
	}
#endif
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleDamageTakenRate(void)
{
	// int32_t damageRate = alarm[PILOT_ALARM_DAMAGE_TAKEN_RATE].trigger[0];
#if 0
	TacticalOrder tacOrder;
	tacOrder.init(OrderOriginType::self, TacticalOrderCode::withdraw);
	Stuff::Vector3D withdrawLocation = ((LancePtr)unit)->getWithdrawThresholdGate();
	tacOrder.params.move.location[0] = withdrawLocation.x;
	tacOrder.params.move.location[1] = withdrawLocation.y;
	tacOrder.params.move.location[2] = withdrawLocation.z;
	tacOrder.params.move.selectionindex = -1;
	setAlarmTacOrder(tacOrder, 15);
#endif
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleUnitMateDeath(void)
{
	uint32_t mateWID = alarm[PILOT_ALARM_DEATH_OF_MATE].trigger[0];
	//--------------------------------------
	// First, check if the death was seen...
	std::unique_ptr<Mover> myVehicle = getVehicle();
	if ((uint32_t)myVehicle->getWatchID() == mateWID)
		return (NO_ERROR);
	std::unique_ptr<Mover> mateVehicle = (std::unique_ptr<Mover>)ObjectManager->getByWatchID(mateWID);
	if (!mateVehicle)
		return (-1);
#if 0
	float rangeToMate = myVehicle->distanceFrom((GameObjectPtr(BaseObjectPtr(mateVehicle)))->getPosition());
	float factor = 1.5;
	if(/*(rangeToMate <= scenario->maxVisualRange) &&*/ myVehicle->lineOfSight((GameObjectPtr(BaseObjectPtr(mateVehicle)))))
		factor = 1.0;
	//--------------------------------
	// Check whether we snap or not...
	changeMorale(-5);
	if(checkMorale() < 0)
	{
#if 0
		int32_t checkDelta = checkRelationship(matePilot->getID());
		if(checkDelta >= 0)
		{
			//----------------------------------------------------
			// Since this is an interrupt, we don't care about the
			// updateTime...
			float time = 0.0;
			commandDecisionTree(1, time, checkDelta);
		}
#endif
	}
#endif
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleFriendlyVehicleCrippled(void)
{
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleFriendlyVehicleDestruction(void)
{
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleOwnVehicleIncapacitation(uint32_t cause)
{
	std::unique_ptr<Mover> myVehicle = getVehicle();
	Assert(myVehicle != nullptr, 0, " pilot has no vehicle ");
	switch (cause)
	{
	case 0:
	case 1:
	case 66:
		//---------------------------------------------------------------
		// Center Torso Internal Structure gone. Automatically ejects, if
		// can...
		myVehicle->handleEjection();
		break;
	}
	//----------------
	// Clear orders...
	clearCurTacOrder(false);
	setOrderState(ORDERSTATE_GENERAL);
	MovePathPtr tempPath[2];
	for (size_t i = 0; i < 2; i++)
	{
		tempPath[i] = moveOrders.path[i];
		tempPath[i]->numSteps = 0;
	}
	moveOrders.init();
	PathManager->remove(this);
	moveOrders.path[0] = tempPath[0];
	moveOrders.path[1] = tempPath[1];
	attackOrders.init();
	setLastTarget(nullptr);
	//--------------------------------------------------------
	// Let's let the unit know we're dying if we're a point...
	// No, Don't!!!
	//	if (getPoint() == vehicle) {
	//		GameObjectPtr newPointVehicle = getGroup()->selectPoint(true);
	//--------------------------------------------------------
	// If there is no new point, all units must be blown away.
	// How do we want to handle this?
	//	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleOwnVehicleDestruction(uint32_t cause)
{
	// uint32_t attackerId = cause;
	// std::unique_ptr<Mover> myVehicle = getVehicle();
	// Assert(myVehicle != nullptr, 0, "handleOwnVehicleDestruction:pilot has no
	// vehicle ");
	//--------------------------------------------------------
	// Let's let the unit know we're dying if we're a point...
	// No, Don't!!!
	//	if (getPoint() == myVehicle) {
	//		GameObjectPtr newPointVehicle = getGroup()->selectPoint(true);
	//--------------------------------------------------------
	// If there is no new point, all units must be blown away.
	// How do we want to handle this?
	//	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleOwnVehicleWithdrawn(void)
{
	// std::unique_ptr<Mover> myVehicle = getVehicle();
	// Assert(myVehicle != nullptr, 0, "handleOwnVehicleWithdrawn:pilot has no
	// vehicle ");
	//--------------------------------------------------------
	// Let's let the unit know we're dying if we're a point...
	// No, Don't!!!
	//	if (getPoint() == myVehicle) {
	//		GameObjectPtr newPointVehicle = getGroup()->selectPoint(true);
	//--------------------------------------------------------
	// If there is no new point, all units must be blown away.
	// How do we want to handle this?
	//	}
	setStatus(WARRIOR_STATUS_WITHDRAWN);
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleAttackOrder(void)
{
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleFiredWeapon(void)
{
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleNewMover(void)
{
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleCollisionAlert(void)
{
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
int32_t
MechWarrior::handleKilledTarget(void)
{
	uint32_t targetWID = alarm[PILOT_ALARM_KILLED_TARGET].trigger[0];
	GameObjectPtr target = ObjectManager->getByWatchID(targetWID);
	if (target)
	{
		int32_t message = -1;
		float skillIncrement = 0;
		int32_t vehicleClass = -1;
		switch (target->getObjectClass())
		{
		case BATTLEMECH:
			if (((std::unique_ptr<Mover>)target)->getMoveType() != MOVETYPE_AIR) // Killing Helicopters does not count as a mech.
			{
				vehicleClass = target->getMechClass();
				skillIncrement = KillSkill[vehicleClass];
				numMechKills[vehicleClass][COMBAT_STAT_MISSION]++;
				message = RADIO_MECH_DEAD;
				if (numKilled < MAX_MOVERS / 3)
					killed[numKilled++] = target->getWatchID();
			}
			else
			{
				vehicleClass = VEHICLE_CLASS_GROUND;
				skillIncrement = KillSkill[4];
				numMechKills[VEHICLE_CLASS_GROUND][COMBAT_STAT_MISSION]++;
				message = RADIO_VEHICLE_DEAD;
				if (numKilled < MAX_MOVERS / 3)
					killed[numKilled++] = target->getWatchID();
			}
			break;
		case GROUNDVEHICLE:
		{
			std::unique_ptr<Mover> gv = (std::unique_ptr<Mover>)target;
			if (gv->pathLocks) // Killing Infantry/Powered Armor is meaningless!
			{
				vehicleClass = VEHICLE_CLASS_GROUND;
				skillIncrement = KillSkill[4];
				numMechKills[VEHICLE_CLASS_GROUND][COMBAT_STAT_MISSION]++;
				message = RADIO_VEHICLE_DEAD;
				if (numKilled < MAX_MOVERS / 3)
					killed[numKilled++] = target->getWatchID();
			}
		}
		break;
		case TURRET:
		case GATE:
			message = RADIO_OBJECT_DEAD;
			break;
		case ELEMENTAL:
			vehicleClass = VEHICLE_CLASS_ELEMENTAL;
			skillIncrement = KillSkill[5];
			numMechKills[VEHICLE_CLASS_ELEMENTAL][COMBAT_STAT_MISSION]++;
		case BUILDING:
			message = RADIO_BUILDING_DEAD;
			break;
		default:
			message = RADIO_OBJECT_DEAD;
		}
		radioMessage(message);
		skillPoints[Skill::gunnery] += skillIncrement;
		if (MPlayer && MPlayer->isServer() && (vehicleClass != -1))
			MPlayer->addPilotKillStat(getVehicle(), vehicleClass);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleUnitMateFiredWeapon(void)
{
	// uint32_t mateVehicleHandle =
	// alarm[PILOT_ALARM_MATE_FIRED_WEAPON].trigger[0];
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handlePlayerOrder(void)
{
	if ((getVehicleStatus() == OBJECT_STATUS_SHUTDOWN) && (curTacOrder.code != TacticalOrderCode::powerdown))
		orderPowerUp(false, OrderOriginType::self);
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleNoMovePath(void)
{
	if (getCurTacOrder()->code == TacticalOrderCode::getfixed)
	{
		clearCurTacOrder();
		radioMessage(RADIO_MOVE_BLOCKED);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::handleGateClosing(void)
{
	/*
		//------------------------------------------------------------
		// Do this here for now... but, we may want to put it into the
		// player brain instead and allow kamikazee pilots :)
		for (size_t i = 0; i < alarm[PILOT_ALARM_GATE_CLOSING].numTriggers; i++)
	   { int32_t gateTileRow = alarm[PILOT_ALARM_GATE_CLOSING].trigger[i] /
	   MAX_MAPWIDTH; int32_t gateTileCol =
	   alarm[PILOT_ALARM_GATE_CLOSING].trigger[i] % MAX_MAPWIDTH; int32_t
	   overlayType = GameMap->getOverlayType(gateTileRow, gateTileCol); bool
	   check = true; if (alignment == CLANS) check = ((overlayType ==
	   OVERLAY_GATE_IS_NS_CLOSED) || (overlayType ==
	   OVERLAY_GATE_IS_EW_CLOSED)); else check = ((overlayType ==
	   OVERLAY_GATE_CLAN_NS_CLOSED) || (overlayType ==
	   OVERLAY_GATE_IS_EW_CLOSED)); if (check && getMovePath()) { int32_t
	   firstStep = getMovePath()->crossesTile(-1, 100, gateTileRow,
	   gateTileCol); if (firstStep > 1) {
					//setMoveGoal(MOVEGOAL_NONE, nullptr);
					setMoveWayPath(0);
					for (size_t i = 0; i < 2; i++)
						clearMovePath(i);
					setMoveState(MOVESTATE_FORWARD);
					setMoveStateGoal(MOVESTATE_FORWARD);
					setMoveYieldTime(-1.0);
					setMoveYieldState(0);
					setMoveWaitForPointTime(-1.0);
					setMoveTimeOfLastStep(-1.0);
					setMoveTwisting(false);
					setMoveGlobalPath(nullptr, 0);
					PathManager->remove(this);
					break;
				}
			}
		}
	*/
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::missionLog(std::unique_ptr<File> file, int32_t unitLevel)
{
#if 0
	for(size_t i = 0; i < (unitLevel * 2); i++)
		file->writeString(" ");
	wchar_t s[80];
	sprintf(s, "MechWarrior: %s\n", name);
	file->writeString(s);
	for(size_t skill = 0; skill < Skill::numberofskills; skill)
	{
		for(i = 0; i < ((unitLevel + 1) * 2); i++)
		{
			file->writeString(" ");
		}
		sprintf(s, "%s: %04d/04%d\n",
				SkillsTable[skill],
				numSkillSuccesses[skill][COMBAT_STAT_MISSION],
				numSkillUses[skill][COMBAT_STAT_MISSION]);
	}
#endif
	return (0);
}

//---------------------------------------------------------------------------
void MechWarrior::calcRank(void)
{
	float skillAverage = skillRank[Skill::gunnery] + skillRank[Skill::piloting];
	skillAverage /= 2.0f;
	float avg = skillAverage;
	if (avg > 79.f)
		rank = WarriorRank::ace;
	else if (avg > 70.f)
		rank = WarriorRank::elite;
	else if (avg > 60.f)
		rank = WarriorRank::veteran;
	else if (avg > 50.f)
		rank = WarriorRank::regular;
	else
		rank = WarriorRank::green;
}

//---------------------------------------------------------------------------
void MechWarrior::updateMissionSkills()
{
	// DO NOT CALL THIS IF THE MISSION FAILED!
	float MaxPilotSkill = 80.f;
	// increment skills and see if pilot has been promoted
	skillRank[Skill::gunnery] += (float)skillPoints[Skill::gunnery] * 0.01f;
	if (skillRank[Skill::gunnery] > MaxPilotSkill)
		skillRank[Skill::gunnery] = MaxPilotSkill;
	skillRank[Skill::piloting]++; // One free point just for coming along!
	skillRank[Skill::piloting] += skillPoints[Skill::piloting];
	if (skillRank[Skill::piloting] > MaxPilotSkill)
		skillRank[Skill::piloting] = MaxPilotSkill;
}

//---------------------------------------------------------------------------
int32_t
MechWarrior::loadBrainParameters(FitIniFile* brainFile, int32_t warriorId)
{
	if (!brain)
		Fatal(0, " Warrior.loadBrainParameters: nullptr brain ");
	//------------------
	// Main Info section
	wchar_t warriorName[32];
	sprintf(warriorName, "Warrior%d", warriorId);
	int32_t result = brainFile->seekBlock(warriorName);
	if (result != NO_ERROR)
		return (result);
	int32_t numCells = 0;
	result = brainFile->readIdLong("NumCells", numCells);
	if (result != NO_ERROR)
		return (result);
	int32_t numStaticVars = 0;
	result = brainFile->readIdLong("NumStaticVars", numStaticVars);
	if (result != NO_ERROR)
		return (result);
	for (size_t i = 0; i < numCells; i++)
	{
		wchar_t blockname[64];
		sprintf(blockname, "%sCell%d", warriorName, i);
		int32_t result = brainFile->seekBlock(blockname);
		if (result != NO_ERROR)
			return (result);
		int32_t cell = 0;
		result = brainFile->readIdLong("Cell", cell);
		if (result != NO_ERROR)
			return (result);
		int32_t memType = 0;
		result = brainFile->readIdLong("MemType", memType);
		if (result != NO_ERROR)
			return (result);
		switch (memType)
		{
		case 0:
		{
			// Integer Memory
			int32_t value = 0;
			result = brainFile->readIdLong("value", value);
			if (result != NO_ERROR)
				return (result);
			setIntegerMemory(cell, value);
		}
		break;
		case 1:
		{
			// Real Memory
			float value = 0.0;
			result = brainFile->readIdFloat("value", value);
			if (result != NO_ERROR)
				return (result);
			setRealMemory(cell, value);
		}
		break;
		default:
			// result = 666;
			// return(result);
			NODEFAULT;
		}
	}
	for (i = 0; i < numStaticVars; i++)
	{
		wchar_t blockname[64];
		sprintf(blockname, "%sStatic%d", warriorName, i);
		int32_t result = brainFile->seekBlock(blockname);
		if (result != NO_ERROR)
			return (result);
		int32_t type = 0;
		result = brainFile->readIdLong("type", type);
		if (result != NO_ERROR)
			return (result);
		wchar_t varName[256];
		result = brainFile->readIdString("Name", varName, 255);
		if (result != NO_ERROR)
			return (result);
		switch (type)
		{
		case 0:
		{
			// Integer
			int32_t value = 0;
			result = brainFile->readIdLong("value", value);
			if (result != NO_ERROR)
				return (result);
			brain->setStaticInteger(varName, value);
		}
		break;
		case 1:
		{
			// Real
			float value = 0;
			result = brainFile->readIdFloat("value", value);
			if (result != NO_ERROR)
				return (result);
			brain->setStaticReal(varName, value);
		}
		break;
		case 2:
		{
			// Integer Array
			int32_t numValues = 0;
			result = brainFile->readIdLong("NumValues", numValues);
			if (result != NO_ERROR)
				return (result);
			int32_t values[1024];
			result = brainFile->readIdLongArray("Values", values, numValues);
			if (result != NO_ERROR)
				return (result);
			brain->setStaticIntegerArray(varName, numValues, values);
		}
		break;
		case 3:
		{
			// Real Array
			int32_t numValues = 0;
			result = brainFile->readIdLong("NumValues", numValues);
			if (result != NO_ERROR)
				return (result);
			float values[1024];
			result = brainFile->readIdFloatArray("Values", values, numValues);
			if (result != NO_ERROR)
				return (result);
			brain->setStaticRealArray(varName, numValues, values);
		}
		break;
		default:
			// result = 667;
			// return(result);
			NODEFAULT;
		}
	}
	return (NO_ERROR);
}

void MechWarrior::drawWaypointPath()
{
	if (tacOrderQueue)
	{
		Stuff::Vector3D tmpPos = this->getVehicle()->getPosition();
		{
			Stuff::Vector4D screenPos1;
			Stuff::Vector4D screenPos2;
			Stuff::Vector3D pos;
			pos.x = getCurTacOrder()->moveparams.wayPath.points[0];
			pos.y = getCurTacOrder()->moveparams.wayPath.points[1];
			pos.z = getCurTacOrder()->moveparams.wayPath.points[2];
			if (tacOrderQueueLooping)
			{
				tmpPos = tacOrderQueue[numTacOrdersQueued - 1].point;
			}
			else if (pos.x != 0 && pos.y != 0)
			{
				pos.z = land->getTerrainElevation(pos);
				eye->projectZ(tmpPos, screenPos1);
				eye->projectZ(pos, screenPos2);
				Stuff::Vector4D vertices[2];
				vertices[0].x = screenPos1.x;
				vertices[0].y = screenPos1.y;
				vertices[1].x = screenPos2.x;
				vertices[1].y = screenPos2.y;
				vertices[0].z = vertices[1].z = 0.1f;
				vertices[0].w = vertices[1].w = 0.9999f;
				LineElement line(vertices[0], vertices[1], 0xffff0000, 0, -1);
				line.draw();
				tmpPos = pos;
			}
		}
		for (size_t i = 0; i < numTacOrdersQueued; i++)
		{
			tacOrderQueue[i].marker->render();
			Stuff::Vector4D screenPos1;
			Stuff::Vector4D screenPos2;
			eye->projectZ(tacOrderQueue[i].point, screenPos1);
			eye->projectZ(tmpPos, screenPos2);
			Stuff::Vector4D vertices[2];
			vertices[0].x = screenPos1.x;
			vertices[0].y = screenPos1.y;
			vertices[1].x = screenPos2.x;
			vertices[1].y = screenPos2.y;
			vertices[0].z = vertices[1].z = 0.1f;
			vertices[0].w = vertices[1].w = 0.9999f;
			LineElement line(vertices[0], vertices[1], 0xffff0000, 0, -1);
			line.draw();
			tmpPos = tacOrderQueue[i].point;
		}
	}
}

void MechWarrior::updateDrawWaypointPath()
{
	if (tacOrderQueue)
	{
		for (size_t i = 0; i < numTacOrdersQueued; i++)
		{
			tacOrderQueue[i].marker->setObjectParameters(tacOrderQueue[i].point,
				((ObjectAppearance*)tacOrderQueue[i].marker)->rotation, false, 0, 0);
			tacOrderQueue[i].marker->recalcBounds();
			//------------------------------------------------
			tacOrderQueue[i].marker->update();
			float zPos = land->getTerrainElevation(tacOrderQueue[i].point);
			tacOrderQueue[i].point.z = zPos;
		}
	}
}

bool MechWarrior::isCloseToFirstTacOrder(Stuff::Vector3D& pos)
{
	if (tacOrderQueue)
	{
		Stuff::Vector3D tmp;
		tmp.Subtract(tacOrderQueue[0].point, pos);
		if (tmp.GetLength() < 5) // random amount
			return true;
	}
	return false;
}

//---------------------------------------------------------------------------

void MechWarrior::setup(void)
{
	TacOrderQueuePos = 0;
	for (size_t i = 0; i < MAX_WARRIORS; i++)
	{
		warriorList[i] = new MechWarrior;
		warriorList[i]->index = i;
		warriorList[i]->used = false;
		if (warriorList[i] == nullptr)
			Fatal(i, " MechWarrior.setup: nullptr warrior ");
	}
}

//---------------------------------------------------------------------------

MechWarrior*
MechWarrior::newWarrior(void)
{
	for (size_t i = 1; i < MAX_WARRIORS; i++)
		if (!warriorList[i]->used)
		{
			warriorList[i]->used = true;
			return (warriorList[i]);
		}
	return (nullptr);
}

//---------------------------------------------------------------------------

void MechWarrior::freeWarrior(MechWarrior* warrior)
{
	warrior->clear();
}

void MechWarrior::changeAttackRange(int32_t newRange)
{
	if (curTacOrder.isCombatOrder())
	{
		TacticalOrder order;
		// hope this assignment operator works
		order = curTacOrder;
		order.attackParams.range = (FireRangeType)newRange;
		order.pack(0, 0);
		getVehicle()->handleTacticalOrder(order);
	}
}

//---------------------------------------------------------------------------

void MechWarrior::shutdown(void)
{
	for (size_t i = 0; i < MAX_WARRIORS; i++)
	{
		if (warriorList[i])
		{
			delete warriorList[i];
			warriorList[i] = nullptr;
		}
	}
}

BldgAppearance*
MechWarrior::getWayPointMarker(const Stuff::Vector3D& pos, std::wstring_view name)
{
	int32_t appearanceType = (BLDG_TYPE << 24);
	AppearanceTypePtr buildingAppearanceType =
		appearanceTypeList->getAppearance(appearanceType, name);
	BldgAppearance* appearance = new BldgAppearance;
	gosASSERT(appearance != nullptr);
	//--------------------------------------------------------------
	// The only appearance type for buildings is MLR_APPEARANCE.
	gosASSERT(buildingAppearanceType->getAppearanceClass() == BLDG_TYPE);
	appearance->init((BldgAppearanceType*)buildingAppearanceType, (GameObjectPtr)this);
	appearance->setObjectParameters((const_cast<Stuff::Vector3D&>(pos)), 0, 1, 0, 0);
	appearance->recalcBounds();
	appearance->update();
	return appearance;
}

//---------------------------------------------------------------------------

int32_t
MechWarrior::calcTacOrder(int32_t goalAction, int32_t goalWID, Stuff::Vector3D goalLocation,
	float controlRadius, int32_t aggressiveness, int32_t searchDepth, float turretRange,
	int32_t turretThreat, TacticalOrder& newTacOrder)
{
	if (getCommander() == Commander::home)
		STOP(("Mechwarrior.runBrain: player pilot using goalplanning"));
	bool canCapture = ((getVehicle()->getObjectClass() != GROUNDVEHICLE) && (getVehicle()->moveType != MOVETYPE_AIR));
	//--------------------------------------------------------------------
	// If we have no goal, we may want to at least assess turret threat...
	bool alreadyAssessedTurrets = false;
	if (goalWID == 0)
	{
		if (turretRange > 0.0)
		{
			// if (getLastTarget())
			//	if (getLastTarget()->getThreatRating() > turretThreat)
			//		turretThreat = getLastTarget()->getThreatRating();
			GameObjectPtr turretTarget = calcTurretThreats(turretRange, turretThreat);
			if (turretTarget)
			{
				if (turretTarget->getObjectClass() == TURRET)
					goalAction = GOAL_ACTION_ATTACK;
				else if (turretTarget->getCaptureBlocker(getVehicle()))
					goalAction = GOAL_ACTION_ATTACK;
				else if (canCapture)
					goalAction = GOAL_ACTION_CAPTURE;
				else
					goalAction = GOAL_ACTION_ATTACK;
				goalWID = turretTarget->getWatchID();
				goalLocation.Zero();
			}
			alreadyAssessedTurrets = true;
		}
	}
	//------------------------------------------------------
	// At this point, if we don't have any goal, we're done.
	GameObjectPtr mainTarget = nullptr;
	if (goalWID)
	{
		mainTarget = ObjectManager->getByWatchID(goalWID);
		if (!mainTarget)
		{
			goalAction = GOAL_ACTION_NONE;
			goalWID = 0;
			goalLocation.Zero();
			lastGoalPathSize = 0;
			return (-1);
		}
	}
	//-----------------------------------------------------------
	// Preserve our original goal, as it may change int16_t-term...
	int32_t currentAction = goalAction;
	int32_t currentWID = goalWID;
	Stuff::Vector3D currentLocation = goalLocation;
	GameObjectPtr currentTarget = mainTarget;
	OrderOriginType orderOrigin = OrderOriginType::commander;
	if (getCommander() == Commander::home)
		orderOrigin = OrderOriginType::player;
	if (currentAction == GOAL_ACTION_UNDECIDED)
	{
		//-------------------
		// Pick one for me...
		if (currentTarget)
			if (currentTarget->isMover())
				if (getTeam()->isEnemy(currentTarget->getTeam()))
				{
					if (!currentTarget->isDisabled())
						currentAction = GOAL_ACTION_ATTACK;
				}
				else
				{
					if (!currentTarget->isDisabled())
						currentAction = GOAL_ACTION_GUARD;
				}
			else
				switch (currentTarget->getObjectClass())
				{
				case BUILDING:
					if (currentTarget->getFlag(OBJECT_FLAG_CONTROLBUILDING))
						currentAction = GOAL_ACTION_ATTACK; // CONTROL_ACTION_CAPTURE;
					break;
				case TURRET:
					currentAction = GOAL_ACTION_ATTACK;
					break;
				}
		else
		{
		}
	}
	if (!alreadyAssessedTurrets && (turretRange > 0.0))
	{
		if (currentAction == GOAL_ACTION_ATTACK)
			if (getLastTarget())
				if (getLastTarget()->getThreatRating() > turretThreat)
					turretThreat = getLastTarget()->getThreatRating();
		GameObjectPtr turretTarget = calcTurretThreats(turretRange, turretThreat);
		if (turretTarget)
		{
			if (turretTarget->getObjectClass() == TURRET)
				currentAction = GOAL_ACTION_ATTACK;
			else if (turretTarget->getCaptureBlocker(getVehicle()))
				goalAction = GOAL_ACTION_ATTACK;
			else
				currentAction = canCapture ? GOAL_ACTION_CAPTURE : GOAL_ACTION_ATTACK;
			currentWID = turretTarget->getWatchID();
			currentLocation.Zero();
			currentTarget = turretTarget;
		}
	}
	TacticalOrder newestTacOrder;
	newestTacOrder.init();
	if (newTacOrderReceived[ORDERSTATE_GENERAL])
		newestTacOrder = tacOrder[ORDERSTATE_GENERAL];
	else if (newTacOrderReceived[ORDERSTATE_PLAYER])
		newestTacOrder = tacOrder[ORDERSTATE_PLAYER];
	else if (curTacOrder.code != TacticalOrderCode::none)
		newestTacOrder = curTacOrder;
	if (!canCapture)
		if (currentAction == GOAL_ACTION_CAPTURE)
			STOP(("Mechwarrior.calcTacOrder: vehicle cannot capture")); // currentAction
				// =
				// GOAL_ACTION_ATTACK;
	newTacOrder.init(OrderOriginType::commander, TacticalOrderCode::none);
	if (currentAction == GOAL_ACTION_MOVE)
	{
		GlobalPathStep globalPath[50];
		int32_t numSteps = 0;
		if (currentTarget)
			numSteps = getVehicle()->calcGlobalPath(globalPath, currentTarget, nullptr, true);
		else
			numSteps = getVehicle()->calcGlobalPath(globalPath, nullptr, &currentLocation, true);
		if (numSteps > 0)
		{
			lastGoalPathSize = numSteps;
			for (size_t i = 0; i < numSteps; i++)
				lastGoalPath[i] = globalPath[i].thruArea;
		}
		//-----------------------------------------------------------
		// What's the next subgoal to overcome before we can move on?
		for (size_t i = 0; i < numSteps; i++)
		{
			GlobalMapAreaPtr area =
				&GlobalMoveMap[getVehicle()->moveLevel]->areas[globalPath[i].thruArea];
			if (area->type == AREA_TYPE_GATE)
			{
				GatePtr gate = (GatePtr)ObjectManager->getByWatchID(area->ownerWID);
				if (gate && !gate->isDestroyed() && (gate->getTeam() != getTeam()))
				{
					GameObjectPtr controlBuilding = ObjectManager->getByWatchID(gate->parent);
					if (canCapture && controlBuilding && (searchDepth > 0))
					{
						Stuff::Vector3D tempLocation;
						tempLocation.Zero();
						int32_t result = calcTacOrder(GOAL_ACTION_CAPTURE,
							controlBuilding->getWatchID(), tempLocation, -1.0, aggressiveness,
							searchDepth - 1, -1.0, 0, newTacOrder);
						return (result);
					}
					else
					{
						newTacOrder.init(orderOrigin, TacticalOrderCode::attackobject);
						newTacOrder.subOrder = true;
						newTacOrder.targetWID = gate->getWatchID();
						newTacOrder.attackParams.type = AttackType::destroy;
						newTacOrder.attackParams.method = AttackMethod::ranged;
						newTacOrder.attackParams.range = FireRangeType::optimal;
						newTacOrder.attackParams.tactic = newestTacOrder.attackParams.tactic;
						newTacOrder.attackParams.pursue = true;
						newTacOrder.moveparams.wayPath.mode[0] =
							newestTacOrder.moveparams.wayPath.mode[0];
						newTacOrder.moveparams.jump = newestTacOrder.moveparams.jump;
						newTacOrder.moveparams.keepMoving = newestTacOrder.moveparams.keepMoving;
						return (0);
					}
				}
			}
			else if (area->type == AREA_TYPE_WALL)
			{
				BuildingPtr wall = (BuildingPtr)ObjectManager->getByWatchID(area->ownerWID);
				if (wall && (!wall->isDestroyed()))
				{
					newTacOrder.init(orderOrigin, TacticalOrderCode::attackobject);
					newTacOrder.subOrder = true;
					newTacOrder.targetWID = wall->getWatchID();
					newTacOrder.attackParams.type = AttackType::destroy;
					newTacOrder.attackParams.method = AttackMethod::ranged;
					newTacOrder.attackParams.range = FireRangeType::optimal;
					newTacOrder.attackParams.tactic = newestTacOrder.attackParams.tactic;
					newTacOrder.attackParams.pursue = true;
					newTacOrder.moveparams.wayPath.mode[0] =
						newestTacOrder.moveparams.wayPath.mode[0];
					newTacOrder.moveparams.jump = newestTacOrder.moveparams.jump;
					newTacOrder.moveparams.keepMoving = newestTacOrder.moveparams.keepMoving;
					return (0);
				}
			}
		}
		//------------------------------------------------------------------------------------------
		// If we made it here, then there are no steps in the global path
		// blocked. So, let's just go straight for the main goal location...
		if (currentTarget)
		{
			int32_t fromArea = -1; // change this?
			//-------------------------------------------------
			// Pick the best move goal based upon the object...
			Stuff::Vector3D goal;
			if (currentTarget->isMover())
				goal = currentTarget->getPosition();
			else
			{
				//------------------------------------------------
				// If not a mover, then pick an open cell adjacent
				// to it. If it happens to be adjacent to blocked
				// cells, this could be off...
				if (currentTarget->isBuilding())
				{
					BuildingPtr building = (BuildingPtr)currentTarget;
					int32_t goalRow = 0, goalCol = 0;
					bool foundGoal = building->calcAdjacentAreaCell(
						getVehicle()->moveLevel, fromArea, goalRow, goalCol);
					if (foundGoal)
						land->cellToWorld(goalRow, goalCol, goal);
					else
					{
						Stuff::Vector3D objectPos = currentTarget->getPosition();
						goal = getVehicle()->calcOffsetMoveGoal(objectPos);
					}
				}
				else
				{
					Stuff::Vector3D objectPos = currentTarget->getPosition();
					goal = getVehicle()->calcOffsetMoveGoal(objectPos);
				}
			}
			newTacOrder.init(orderOrigin, TacticalOrderCode::movetoobject);
			newTacOrder.subOrder = true;
			newTacOrder.targetWID = currentTarget->getWatchID();
			newTacOrder.selectionindex = -1;
			newTacOrder.moveparams.fromArea = fromArea;
			newTacOrder.moveparams.wayPath.points[0] = goal.x;
			newTacOrder.moveparams.wayPath.points[1] = goal.y;
			newTacOrder.moveparams.wayPath.points[2] = goal.z;
			newTacOrder.moveparams.wayPath.mode[0] = newestTacOrder.moveparams.wayPath.mode[0];
			newTacOrder.moveparams.faceObject = true;
			newTacOrder.moveparams.wait = false;
			newTacOrder.moveparams.mode = SpecialMoveMode::normal;
			newTacOrder.moveparams.jump = newestTacOrder.moveparams.jump;
			newTacOrder.moveparams.keepMoving = newestTacOrder.moveparams.keepMoving;
		}
		else
		{
			newTacOrder.init(orderOrigin, TacticalOrderCode::movetopoint);
			newTacOrder.subOrder = true;
			newTacOrder.setWayPoint(0, currentLocation);
			newTacOrder.moveparams.wayPath.mode[0] = newestTacOrder.moveparams.wayPath.mode[0];
			newTacOrder.moveparams.wait = false;
			newTacOrder.moveparams.mode = SpecialMoveMode::normal;
			newTacOrder.moveparams.jump = newestTacOrder.moveparams.jump;
			newTacOrder.moveparams.keepMoving = newestTacOrder.moveparams.keepMoving;
		}
	}
	else if (currentAction == GOAL_ACTION_ATTACK)
	{
		GlobalPathStep globalPath[50];
		int32_t numSteps = getVehicle()->calcGlobalPath(globalPath, currentTarget, nullptr, true);
		if (numSteps > 0)
		{
			lastGoalPathSize = numSteps;
			for (size_t i = 0; i < numSteps; i++)
				lastGoalPath[i] = globalPath[i].thruArea;
		}
		//-----------------------------------------------------------
		// What's the next subgoal to overcome before we can move on?
		for (size_t i = 0; i < numSteps; i++)
		{
			GlobalMapAreaPtr area =
				&GlobalMoveMap[getVehicle()->moveLevel]->areas[globalPath[i].thruArea];
			if (area->type == AREA_TYPE_GATE)
			{
				GatePtr gate = (GatePtr)ObjectManager->getByWatchID(area->ownerWID);
				if (gate && !gate->isDestroyed() && (gate->getTeam() != getTeam()))
				{
					GameObjectPtr controlBuilding = ObjectManager->getByWatchID(gate->parent);
					if (canCapture && controlBuilding && (searchDepth > 0))
					{
						Stuff::Vector3D tempLocation;
						tempLocation.Zero();
						int32_t result = calcTacOrder(GOAL_ACTION_CAPTURE,
							controlBuilding->getWatchID(), tempLocation, -1.0, aggressiveness,
							searchDepth - 1, -1.0, 0, newTacOrder);
						return (result);
					}
					else
					{
						newTacOrder.init(orderOrigin, TacticalOrderCode::attackobject);
						newTacOrder.subOrder = true;
						newTacOrder.targetWID = gate->getWatchID();
						newTacOrder.attackParams.type = AttackType::destroy;
						newTacOrder.attackParams.method = AttackMethod::ranged;
						newTacOrder.attackParams.range = FireRangeType::optimal;
						newTacOrder.attackParams.tactic = newestTacOrder.attackParams.tactic;
						newTacOrder.attackParams.pursue = true;
						newTacOrder.moveparams.wayPath.mode[0] =
							newestTacOrder.moveparams.wayPath.mode[0];
						newTacOrder.moveparams.jump = newestTacOrder.moveparams.jump;
						newTacOrder.moveparams.keepMoving = newestTacOrder.moveparams.keepMoving;
						return (0);
					}
				}
			}
			else if (area->type == AREA_TYPE_WALL)
			{
				BuildingPtr wall = (BuildingPtr)ObjectManager->getByWatchID(area->ownerWID);
				if (wall && (!wall->isDestroyed()))
				{
					newTacOrder.init(orderOrigin, TacticalOrderCode::attackobject);
					newTacOrder.subOrder = true;
					newTacOrder.targetWID = wall->getWatchID();
					newTacOrder.attackParams.type = AttackType::destroy;
					newTacOrder.attackParams.method = AttackMethod::ranged;
					newTacOrder.attackParams.range = FireRangeType::optimal;
					newTacOrder.attackParams.tactic = newestTacOrder.attackParams.tactic;
					newTacOrder.attackParams.pursue = true;
					newTacOrder.moveparams.wayPath.mode[0] =
						newestTacOrder.moveparams.wayPath.mode[0];
					newTacOrder.moveparams.jump = newestTacOrder.moveparams.jump;
					newTacOrder.moveparams.keepMoving = newestTacOrder.moveparams.keepMoving;
					return (0);
				}
			}
		}
		//------------------------------------------------------------------------------------------
		// If we made it here, then there are no steps in the global path
		// blocked. So, let's just go straight for the main target...
		if (currentTarget->isDisabled())
			return (1);
		newTacOrder.init(orderOrigin, TacticalOrderCode::attackobject);
		newTacOrder.subOrder = true;
		newTacOrder.targetWID = currentTarget->getWatchID();
		newTacOrder.attackParams.type = AttackType::destroy;
		newTacOrder.attackParams.method = AttackMethod::ranged;
		newTacOrder.attackParams.range = FireRangeType::optimal;
		newTacOrder.attackParams.tactic = newestTacOrder.attackParams.tactic;
		newTacOrder.attackParams.pursue = true;
		newTacOrder.moveparams.wayPath.mode[0] = newestTacOrder.moveparams.wayPath.mode[0];
		newTacOrder.moveparams.jump = newestTacOrder.moveparams.jump;
		newTacOrder.moveparams.keepMoving = newestTacOrder.moveparams.keepMoving;
	}
	else if (currentAction == GOAL_ACTION_CAPTURE)
	{
		bool mainGoalControlled = (currentTarget->getTeam() == getTeam());
		if (!mainGoalControlled)
		{
			//---------------------------------------------
			// Are there any enemies blocking this capture?
			// float distanceToGoal =
			// getVehicle()->distanceFrom(currentTarget->getPosition());
			GameObjectPtr blockerList[MAX_MOVERS];
			int32_t numBlockers = currentTarget->getCaptureBlocker(getVehicle(), blockerList);
			if (numBlockers > 0)
			{
				//-----------------------------------------------------
				// Why is it blocked, and can we do something about it?
				/*
				if ((origin == OrderOriginType::self) && (numBlockers > 3)) {
					TacticalOrder alarmTacOrder;
					alarmTacOrder.init(orderOrigin,
				TacticalOrderCode::attackobject); alarmTacOrder.playerSubOrder =
				true; alarmTacOrder.targetWID = target->getWatchID();
					alarmTacOrder.attackParams.type = AttackType::destroy;
					alarmTacOrder.attackParams.method = AttackMethod::ranged;
					alarmTacOrder.attackParams.aimLocation = -1;
					alarmTacOrder.attackParams.range = FireRangeType::optimal;
					alarmTacOrder.attackParams.pursue = true;
					alarmTacOrder.attackParams.obliterate = false;
					alarmTacOrder.moveparams.wayPath.mode[0] =
				newestTacOrder.moveparams.wayPath.mode[0];
					warrior->setAlarmTacOrder(alarmTacOrder, 255);
					}
				else {
				*/
				GameObjectPtr curTarget = getLastTarget();
				int32_t blockerToAttack = -1;
				for (size_t i = 0; i < numBlockers; i++)
					if (blockerList[i] == curTarget)
					{
						blockerToAttack = i;
						break;
					}
				if (blockerToAttack == -1)
					blockerToAttack = 0;
				currentAction = GOAL_ACTION_ATTACK;
				currentWID = blockerList[blockerToAttack]->getWatchID();
				currentLocation.Zero();
				currentTarget = blockerList[blockerToAttack];
				//}
			}
			GlobalPathStep globalPath[50];
			int32_t numSteps =
				getVehicle()->calcGlobalPath(globalPath, currentTarget, nullptr, true);
			if (numSteps > 0)
			{
				lastGoalPathSize = numSteps;
				for (size_t i = 0; i < numSteps; i++)
					lastGoalPath[i] = globalPath[i].thruArea;
			}
			//-----------------------------------------------------------
			// What's the next subgoal to overcome before we can move on?
			for (size_t i = 0; i < numSteps; i++)
			{
				GlobalMapAreaPtr area =
					&GlobalMoveMap[getVehicle()->moveLevel]->areas[globalPath[i].thruArea];
				if (area->type == AREA_TYPE_GATE)
				{
					GatePtr gate = (GatePtr)ObjectManager->getByWatchID(area->ownerWID);
					if (gate && !gate->isDestroyed() && (gate->getTeam() != getTeam()))
					{
						GameObjectPtr controlBuilding = ObjectManager->getByWatchID(gate->parent);
						if (controlBuilding && (searchDepth > 0))
						{
							Stuff::Vector3D tempLocation;
							tempLocation.Zero();
							calcTacOrder(GOAL_ACTION_CAPTURE, controlBuilding->getWatchID(),
								tempLocation, -1.0, aggressiveness, searchDepth - 1, -1.0, 0,
								newTacOrder);
							return (0);
						}
						else
						{
							newTacOrder.init(orderOrigin, TacticalOrderCode::attackobject);
							newTacOrder.subOrder = true;
							newTacOrder.targetWID = gate->getWatchID();
							newTacOrder.attackParams.type = AttackType::destroy;
							newTacOrder.attackParams.method = AttackMethod::ranged;
							newTacOrder.attackParams.range = FireRangeType::optimal;
							newTacOrder.attackParams.tactic = newestTacOrder.attackParams.tactic;
							newTacOrder.attackParams.pursue = true;
							newTacOrder.moveparams.wayPath.mode[0] =
								newestTacOrder.moveparams.wayPath.mode[0];
							newTacOrder.moveparams.jump = newestTacOrder.moveparams.jump;
							newTacOrder.moveparams.keepMoving =
								newestTacOrder.moveparams.keepMoving;
							return (0);
						}
					}
				}
				else if (area->type == AREA_TYPE_WALL)
				{
					BuildingPtr wall = (BuildingPtr)ObjectManager->getByWatchID(area->ownerWID);
					if (wall && (!wall->isDestroyed()))
					{
						newTacOrder.init(orderOrigin, TacticalOrderCode::attackobject);
						newTacOrder.subOrder = true;
						newTacOrder.targetWID = wall->getWatchID();
						newTacOrder.attackParams.type = AttackType::destroy;
						newTacOrder.attackParams.method = AttackMethod::ranged;
						newTacOrder.attackParams.range = FireRangeType::optimal;
						newTacOrder.attackParams.tactic = newestTacOrder.attackParams.tactic;
						newTacOrder.attackParams.pursue = true;
						newTacOrder.moveparams.wayPath.mode[0] =
							newestTacOrder.moveparams.wayPath.mode[0];
						newTacOrder.moveparams.jump = newestTacOrder.moveparams.jump;
						newTacOrder.moveparams.keepMoving = newestTacOrder.moveparams.keepMoving;
						return (0);
					}
				}
			}
			newTacOrder.init(orderOrigin, TacticalOrderCode::capture);
			newTacOrder.subOrder = true;
			newTacOrder.targetWID = currentTarget->getWatchID();
			newTacOrder.attackParams.type = AttackType::none;
			newTacOrder.attackParams.method = AttackMethod::ramming;
			newTacOrder.attackParams.pursue = true;
			newTacOrder.moveparams.fromArea = globalPath[numSteps - 2].thruArea;
			newTacOrder.moveparams.wayPath.mode[0] = newestTacOrder.moveparams.wayPath.mode[0];
			newTacOrder.moveparams.jump = newestTacOrder.moveparams.jump;
			newTacOrder.moveparams.keepMoving = newestTacOrder.moveparams.keepMoving;
		}
		else if (controlRadius > 0.0)
		{
			//-------------------------------------------------------------------------------------------
			// Main Goal has been captured, so focus on clearing out the area.
			// First, look for turrets...
			bool noTargetSelected = true;
			GameObjectPtr nextTarget = getLastTarget();
			//-----------------------------------------------------------------------
			// If our current target is a turret threat, then just keep
			// attacking it.
			if (nextTarget)
				if (nextTarget->getObjectClass() == TURRET)
					if (getVehicle()->isEnemy(nextTarget->getTeam()))
						if (mainTarget /*getVehicle()*/->distanceFrom(nextTarget->getPosition()) <= ((Turret*)nextTarget)->maxRange)
							noTargetSelected = false;
			if (noTargetSelected)
			{
				//---------------------------------------
				// Look for any turret threats around us.
				int32_t numTurrets = ObjectManager->getNumTurrets();
				for (size_t i = 0; i < numTurrets; i++)
				{
					Turret* turret = ObjectManager->getTurret(i);
					if (!turret->isDisabled())
						if (getVehicle()->isEnemy(turret->getTeam()))
							if (mainTarget /*getVehicle()*/->distanceFrom(turret->getPosition()) <= turret->maxRange)
							{
								nextTarget = turret;
								noTargetSelected = false;
								break;
							}
				}
			}
			if (noTargetSelected)
			{
				//----------------------------------------------------------------------------
				// Are we currently targeting an enemy mover that is within our
				// control range?
				if (nextTarget)
					if (nextTarget->isMover())
						if (mainTarget /*getVehicle()*/->distanceFrom(nextTarget->getPosition()) <= controlRadius)
							noTargetSelected = false;
			}
			if (noTargetSelected)
			{
				//---------------------------------------------------------
				// Let's look at any enemy movers within our control range.
				int32_t numMovers = ObjectManager->getNumMovers();
				std::unique_ptr<Mover> biggestMoverThreat = nullptr;
				int32_t biggestThreatRating = 0;
				for (size_t i = 0; i < numMovers; i++)
				{
					std::unique_ptr<Mover> mover = ObjectManager->getMover(i);
					if (getVehicle()->isEnemy(mover->getTeam()))
						if (!mover->isDisabled())
							if (mainTarget /*getVehicle()*/->distanceFrom(mover->getPosition()) <= controlRadius)
								if (mover->getThreatRating() > biggestThreatRating)
								{
									biggestThreatRating = mover->getThreatRating();
									biggestMoverThreat = mover;
								}
				}
				nextTarget = biggestMoverThreat;
			}
			if (nextTarget)
			{
				if (nextTarget->getObjectClass() == TURRET)
				{
					GameObjectPtr turretControl =
						ObjectManager->getByWatchID(((TurretPtr)nextTarget)->parent);
					if (turretControl && (mainTarget /*getVehicle()*/->distanceFrom(turretControl->getPosition()) < controlRadius))
						if (!turretControl->isDisabled())
						{
							Stuff::Vector3D tempLocation;
							tempLocation.Zero();
							calcTacOrder(GOAL_ACTION_CAPTURE, turretControl->getWatchID(),
								tempLocation, -1.0, aggressiveness, searchDepth - 1, -1.0, 0,
								newTacOrder);
							return (0);
						}
				}
				Stuff::Vector3D tempLocation;
				tempLocation.Zero();
				calcTacOrder(GOAL_ACTION_ATTACK, nextTarget->getWatchID(), tempLocation, -1.0,
					aggressiveness, searchDepth, -1.0, 0, newTacOrder);
				return (0);
			}
		}
	}
	else if (currentAction == GOAL_ACTION_GUARD)
	{
	}
	//	int32_t numBlockers = controlObject->getCaptureBlocker (GameObjectPtr
	// capturingMover, GameObjectPtr* blockerList) {
	return (0);
}

//---------------------------------------------------------------------------

void MechWarrior::initGoalManager(int32_t poolSize)
{
	goalManager = new GoalManager;
	gosASSERT(goalManager != nullptr);
	goalManager->setup(poolSize);
	goalManager->build();
	//--------------------------------------
	// For now, process this map manually...
}

//---------------------------------------------------------------------------

void MechWarrior::logPilots(GameLogPtr log)
{
	for (size_t i = 0; i < ObjectManager->getNumMovers(); i++)
	{
		std::unique_ptr<Mover> mover = ObjectManager->getMover(i);
		if (mover)
		{
			wchar_t s[256];
			sprintf(s, "pilot = (%d)%s", i, mover->getPilot()->getName());
			log->write(s);
			sprintf(s, "     vehicle = (%d)%s", mover->getPartId(), mover->getName());
			log->write(s);
		}
	}
}

void MechWarrior::copyToData(MechWarriorData& data)
{
	data.used = used;
	strcpy(data.name, name);
	strcpy(data.callsign, callsign);
	strcpy(data.videoStr, videoStr);
	strcpy(data.audioStr, audioStr);
	strcpy(data.brainStr, brainStr);
	data.index = index;
	data.paintScheme = paintScheme;
	data.photoIndex = photoIndex;
	data.rank = rank;
	memcpy(data.skills, skills, sizeof(wchar_t) * Skill::numberofskills);
	data.professionalism = professionalism;
	data.professionalismModifier = professionalismModifier;
	data.decorum = decorum;
	data.decorumModifier = decorumModifier;
	data.aggressiveness = aggressiveness;
	data.courage = courage;
	data.baseCourage = baseCourage;
	data.wounds = wounds;
	data.health = health;
	data.status = status;
	data.escapesThruEjection = escapesThruEjection;
	data.radioLog = radioLog;
	data.notMineYet = notMineYet;
	data.teamId = teamId;
	data.vehicleWID = vehicleWID;
	memcpy(data.numSkillUses, numSkillUses, sizeof(int32_t) * Skill::numberofskills * NUM_COMBAT_STATS);
	memcpy(
		data.numSkillSuccesses, numSkillSuccesses, sizeof(int32_t) * Skill::numberofskills * NUM_COMBAT_STATS);
	memcpy(
		data.numMechKills, numMechKills, sizeof(int32_t) * NUM_VEHICLE_CLASSES * NUM_COMBAT_STATS);
	memcpy(data.numPhysicalAttacks, numPhysicalAttacks,
		sizeof(int32_t) * NUM_PHYSICAL_ATTACKS * NUM_COMBAT_STATS);
	memcpy(data.skillRank, skillRank, sizeof(float) * Skill::numberofskills);
	memcpy(data.skillPoints, skillPoints, sizeof(float) * Skill::numberofskills);
	memcpy(data.originalSkills, originalSkills, sizeof(wchar_t) * Skill::numberofskills);
	memcpy(data.startingSkills, startingSkills, sizeof(wchar_t) * Skill::numberofskills);
	memcpy(data.specialtySkills, specialtySkills, sizeof(bool) * NUM_SPECIALTY_SKILLS);
	memcpy(data.killed, killed, sizeof(GameObjectWatchID) * (MAX_MOVERS / 3));
	data.numKilled = numKilled;
	data.descID = descID;
	data.nameIndex = nameIndex;
	data.timeOfLastOrders = timeOfLastOrders;
	memcpy(data.attackers, attackers, sizeof(AttackerRec) * MAX_ATTACKERS);
	data.numAttackers = numAttackers;
	data.attackRadius = attackRadius;
	memcpy(data.memory, memory, sizeof(MemoryCell) * NUM_MEMORY_CELLS);
	memcpy(data.debugStrings, debugStrings,
		sizeof(wchar_t) * NUM_PILOT_DEBUG_STRINGS * MAXLEN_PILOT_DEBUG_STRING);
	data.brainUpdate = brainUpdate;
	data.combatUpdate = combatUpdate;
	data.movementUpdate = movementUpdate;
	memcpy(data.weaponsStatus, weaponsStatus, sizeof(int32_t) * MAX_WEAPONS_PER_MOVER);
	data.weaponsStatusResult = weaponsStatusResult;
	data.useGoalPlan = useGoalPlan;
	data.mainGoalAction = mainGoalAction;
	data.mainGoalObjectWID = mainGoalObjectWID;
	data.mainGoalLocation = mainGoalLocation;
	data.mainGoalControlRadius = mainGoalControlRadius;
	data.lastGoalPathSize = lastGoalPathSize;
	memcpy(data.lastGoalPath, lastGoalPath, sizeof(int16_t) * MAX_GLOBAL_PATH);
	memcpy(data.newTacOrderReceived, newTacOrderReceived, sizeof(bool) * NUM_ORDERSTATES);
	memcpy(data.tacOrder, tacOrder, sizeof(TacticalOrder) * NUM_ORDERSTATES);
	data.lastTacOrder = lastTacOrder;
	data.curTacOrder = curTacOrder;
	memcpy(data.alarm, alarm, sizeof(PilotAlarm) * NUM_PILOT_ALARMS);
	memcpy(data.alarmHistory, alarmHistory,
		sizeof(PilotAlarm) * NUM_PILOT_ALARMS); // used by brain update in ABL
	data.alarmPriority = alarmPriority;
	data.curPlayerOrderFromQueue = curPlayerOrderFromQueue;
	data.tacOrderQueueLocked = tacOrderQueueLocked;
	data.tacOrderQueueExecuting = tacOrderQueueExecuting;
	data.tacOrderQueueLooping = tacOrderQueueLooping;
	data.numTacOrdersQueued = numTacOrdersQueued;
	// Pilots which are not used may not have this assigned.
	if (tacOrderQueue)
		memcpy(data.tacOrderQueue, tacOrderQueue,
			sizeof(QueuedTacOrder) * MAX_QUEUED_TACORDERS_PER_WARRIOR);
	data.tacOrderQueueIndex = tacOrderQueueIndex;
	data.nextTacOrderId = nextTacOrderId;
	data.lastTacOrderId = lastTacOrderId;
	data.coreScanTargetWID = coreScanTargetWID;
	data.coreAttackTargetWID = coreAttackTargetWID;
	data.coreMoveTargetWID = coreMoveTargetWID;
	data.targetPriorityList = targetPriorityList;
	data.brainState = brainState;
	data.willHelp = willHelp;
	data.moveOrders.copy(moveOrders);
	data.attackOrders = attackOrders;
	data.situationOrders = situationOrders;
	data.lastTargetWID = lastTargetWID;
	data.lastTargetTime = lastTargetTime;
	data.lastTargetObliterate = lastTargetObliterate;
	data.lastTargetFriendly = lastTargetFriendly;
	data.lastTargetConserveAmmo = lastTargetConserveAmmo;
	data.keepMoving = keepMoving;
	data.orderState = orderState;
	data.debugFlags = debugFlags;
	data.oldPilot = oldPilot;
	if (brain)
		data.warriorBrainHandle = brain->getRealId();
	else
		data.warriorBrainHandle = -1;
}

//---------------------------------------------------------------------------
void MechWarrior::copyFromData(MechWarriorData& data)
{
	used = data.used;
	strcpy(name, data.name);
	strcpy(callsign, data.callsign);
	strcpy(videoStr, data.videoStr);
	strcpy(audioStr, data.audioStr);
	strcpy(brainStr, data.brainStr);
	index = data.index;
	paintScheme = data.paintScheme;
	photoIndex = data.photoIndex;
	rank = data.rank;
	memcpy(skills, data.skills, sizeof(wchar_t) * Skill::numberofskills);
	professionalism = data.professionalism;
	professionalismModifier = data.professionalismModifier;
	decorum = data.decorum;
	decorumModifier = data.decorumModifier;
	aggressiveness = data.aggressiveness;
	courage = data.courage;
	baseCourage = data.baseCourage;
	wounds = data.wounds;
	health = data.health;
	status = data.status;
	escapesThruEjection = data.escapesThruEjection;
	radioLog = data.radioLog;
	notMineYet = data.notMineYet;
	teamId = data.teamId;
	vehicleWID = data.vehicleWID;
	memcpy(numSkillUses, data.numSkillUses, sizeof(int32_t) * Skill::numberofskills * NUM_COMBAT_STATS);
	memcpy(
		numSkillSuccesses, data.numSkillSuccesses, sizeof(int32_t) * Skill::numberofskills * NUM_COMBAT_STATS);
	memcpy(
		numMechKills, data.numMechKills, sizeof(int32_t) * NUM_VEHICLE_CLASSES * NUM_COMBAT_STATS);
	memcpy(numPhysicalAttacks, data.numPhysicalAttacks,
		sizeof(int32_t) * NUM_PHYSICAL_ATTACKS * NUM_COMBAT_STATS);
	memcpy(skillRank, data.skillRank, sizeof(float) * Skill::numberofskills);
	memcpy(skillPoints, data.skillPoints, sizeof(float) * Skill::numberofskills);
	memcpy(originalSkills, data.originalSkills, sizeof(wchar_t) * Skill::numberofskills);
	memcpy(startingSkills, data.startingSkills, sizeof(wchar_t) * Skill::numberofskills);
	memcpy(specialtySkills, data.specialtySkills, sizeof(bool) * NUM_SPECIALTY_SKILLS);
	memcpy(killed, data.killed, sizeof(GameObjectWatchID) * (MAX_MOVERS / 3));
	numKilled = data.numKilled;
	descID = data.descID;
	nameIndex = data.nameIndex;
	timeOfLastOrders = data.timeOfLastOrders;
	memcpy(attackers, data.attackers, sizeof(AttackerRec) * MAX_ATTACKERS);
	numAttackers = data.numAttackers;
	attackRadius = data.attackRadius;
	memcpy(memory, data.memory, sizeof(MemoryCell) * NUM_MEMORY_CELLS);
	memcpy(debugStrings, data.debugStrings,
		sizeof(wchar_t) * NUM_PILOT_DEBUG_STRINGS * MAXLEN_PILOT_DEBUG_STRING);
	brainUpdate = data.brainUpdate;
	combatUpdate = data.combatUpdate;
	movementUpdate = data.movementUpdate;
	memcpy(weaponsStatus, data.weaponsStatus, sizeof(int32_t) * MAX_WEAPONS_PER_MOVER);
	weaponsStatusResult = data.weaponsStatusResult;
	useGoalPlan = data.useGoalPlan;
	mainGoalAction = data.mainGoalAction;
	mainGoalObjectWID = data.mainGoalObjectWID;
	mainGoalLocation = data.mainGoalLocation;
	mainGoalControlRadius = data.mainGoalControlRadius;
	lastGoalPathSize = data.lastGoalPathSize;
	memcpy(lastGoalPath, data.lastGoalPath, sizeof(int16_t) * MAX_GLOBAL_PATH);
	memcpy(newTacOrderReceived, data.newTacOrderReceived, sizeof(bool) * NUM_ORDERSTATES);
	memcpy(tacOrder, data.tacOrder, sizeof(TacticalOrder) * NUM_ORDERSTATES);
	lastTacOrder = data.lastTacOrder;
	curTacOrder = data.curTacOrder;
	memcpy(alarm, data.alarm, sizeof(PilotAlarm) * NUM_PILOT_ALARMS);
	memcpy(alarmHistory, data.alarmHistory,
		sizeof(PilotAlarm) * NUM_PILOT_ALARMS); // used by brain update in ABL
	alarmPriority = data.alarmPriority;
	curPlayerOrderFromQueue = data.curPlayerOrderFromQueue;
	tacOrderQueueLocked = data.tacOrderQueueLocked;
	tacOrderQueueExecuting = data.tacOrderQueueExecuting;
	tacOrderQueueLooping = data.tacOrderQueueLooping;
	numTacOrdersQueued = data.numTacOrdersQueued;
	memcpy(tacOrderQueue, data.tacOrderQueue,
		sizeof(QueuedTacOrder) * MAX_QUEUED_TACORDERS_PER_WARRIOR);
	for (size_t i = 0; i < MAX_QUEUED_TACORDERS_PER_WARRIOR; i++)
	{
		// Set the Marker pointers to nullptr and then recreate them from the
		// movemode data.
		tacOrderQueue[i].marker = nullptr;
		if (tacOrderQueue[i].moveMode == TravelModeType::slow)
			tacOrderQueue[i].marker = getWayPointMarker(tacOrderQueue[i].point, "WalkWayPoint");
		else if (tacOrderQueue[i].moveMode == TravelModeType::fast)
			tacOrderQueue[i].marker = getWayPointMarker(tacOrderQueue[i].point, "RunWayPoint");
		else if (tacOrderQueue[i].moveMode == TravelModeType::jump)
			tacOrderQueue[i].marker = getWayPointMarker(tacOrderQueue[i].point, "JumpWayPoint");
	}
	tacOrderQueueIndex = data.tacOrderQueueIndex;
	nextTacOrderId = data.nextTacOrderId;
	lastTacOrderId = data.lastTacOrderId;
	coreScanTargetWID = data.coreScanTargetWID;
	coreAttackTargetWID = data.coreAttackTargetWID;
	coreMoveTargetWID = data.coreMoveTargetWID;
	targetPriorityList = data.targetPriorityList;
	brainState = data.brainState;
	willHelp = data.willHelp;
	data.moveOrders.copyTo(moveOrders);
	attackOrders = data.attackOrders;
	situationOrders = data.situationOrders;
	lastTargetWID = data.lastTargetWID;
	lastTargetTime = data.lastTargetTime;
	lastTargetObliterate = data.lastTargetObliterate;
	lastTargetFriendly = data.lastTargetFriendly;
	lastTargetConserveAmmo = data.lastTargetConserveAmmo;
	keepMoving = data.keepMoving;
	orderState = data.orderState;
	debugFlags = data.debugFlags;
	oldPilot = data.oldPilot;
	warriorBrainHandle = data.warriorBrainHandle;
}

#define USE_ABL_LOAD
//---------------------------------------------------------------------------
void MechWarrior::init(MechWarriorData data)
{
	init(false);
	// First, copy all of the data to the class.
	copyFromData(data);
#ifdef USE_ABL_LOAD
	// Set the brain Pointer from the handle.
	// NOT IMPLEMENTED BY GLENNDOR YET!!!!!!!!!
	if (warriorBrainHandle > -1)
	{
		brain = ABLi_getModule(warriorBrainHandle);
		//-----------------------------------------
		// Set up the pilot alarm callback table...
		for (size_t i = 0; i < NUM_PILOT_ALARMS; i++)
			brainAlarmCallback[i] = brain->findFunction(pilotAlarmFunctionName[i], true);
	}
	else
	{
		brain = nullptr;
	}
#else
	int32_t numErrors, numLinesProcessed;
	FullPathFileName brainFileName;
	brainFileName.init(warriorPath, brainStr, ".abl");
	if (brainStr[0])
	{
		int32_t moduleHandle = ABLi_preProcess(brainFileName, &numErrors, &numLinesProcessed);
		gosASSERT(moduleHandle >= 0);
		int32_t error = setBrain(moduleHandle);
		gosASSERT(error == 0);
	}
#endif
	// Set anything else.
	movePathRequest = nullptr;
	if (audioStr[0] != 0)
	{
		radio = new Radio;
		int32_t result = radio->init(audioStr, 102400,
			videoStr); // Size of radio heap should be data driven?
		if (result != NO_ERROR)
		{
			delete radio; // Radio couldn't start, so no radio.
			radio = nullptr;
		}
	}
	isPlayingMsg = false; // Never playing when we reload an in-mission save!
	if (radio)
		radio->setOwner(this);
}

//---------------------------------------------------------------------------
void MechWarrior::save(PacketFilePtr file, int32_t packetNum)
{
	MechWarriorData warriorData;
	copyToData(warriorData);
	file->writePacket(packetNum, (uint8_t*)&warriorData, sizeof(warriorData), STORAGE_TYPE_ZLIB);
}

//---------------------------------------------------------------------------
int32_t
MechWarrior::Save(PacketFilePtr file, int32_t packetNum)
{
	StaticMechWarriorData staticData;
	staticData.numWarriors = numWarriors;
	staticData.numWarriorsInCombat = numWarriorsInCombat;
	memcpy(staticData.brainsEnabled, brainsEnabled, sizeof(bool) * MAX_TEAMS);
	staticData.minSkill = minSkill;
	staticData.maxSkill = maxSkill;
	staticData.increaseCap = increaseCap;
	staticData.maxVisualRadius = maxVisualRadius;
	staticData.curEventID = curEventID;
	staticData.curEventTrigger = curEventTrigger;
	file->writePacket(
		packetNum, (uint8_t*)&staticData, sizeof(StaticMechWarriorData), STORAGE_TYPE_RAW);
	packetNum++;
	for (size_t i = 0; i < numWarriors; i++)
	{
		MechWarrior::warriorList[i]->save(file, packetNum);
		packetNum++;
	}
	return packetNum;
}

//---------------------------------------------------------------------------
void MechWarrior::load(PacketFilePtr file, int32_t packetNum)
{
	MechWarriorData warriorData;
	file->readPacket(packetNum, (uint8_t*)&warriorData);
	init(warriorData);
}

//---------------------------------------------------------------------------
int32_t
MechWarrior::Load(PacketFilePtr file, int32_t packetNum)
{
	StaticMechWarriorData staticData;
	file->readPacket(packetNum, (uint8_t*)&staticData);
	packetNum++;
	numWarriors = staticData.numWarriors;
	numWarriorsInCombat = staticData.numWarriorsInCombat;
	memcpy(brainsEnabled, staticData.brainsEnabled, sizeof(bool) * MAX_TEAMS);
	minSkill = staticData.minSkill;
	maxSkill = staticData.maxSkill;
	increaseCap = staticData.increaseCap;
	maxVisualRadius = staticData.maxVisualRadius;
	curEventID = staticData.curEventID;
	curEventTrigger = staticData.curEventTrigger;
	TacOrderQueuePos = 0;
	for (size_t i = 0; i < numWarriors; i++)
	{
		MechWarrior::warriorList[i]->load(file, packetNum);
		packetNum++;
		// Kind of a hack but Glenn increments numWarriors with each init.
		// Take it back to the real number here!!
		numWarriors--;
	}
	// initGoalManager(200);
	//------------
	if (!sortList)
	{
		sortList = new SortList;
		if (!sortList)
			Fatal(0, " Unable to create Warrior::sortList ");
		sortList->init(100);
	}
	return packetNum;
}

bool MechWarrior::warriorInUse(std::wstring_view warriorName)
{
	for (size_t i = 0; i < numWarriors; i++)
	{
		if (_stricmp(MechWarrior::warriorList[i]->name, warriorName) == 0)
			return true;
	}
	return false;
}
//---------------------------------------------------------------------------
