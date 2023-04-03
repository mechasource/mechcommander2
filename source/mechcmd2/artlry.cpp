//***************************************************************************
//
//	artlry.cpp -- File contains the Artillery Strike Object code
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"

#include "artlry.h"
#include "team.h"
#include "objmgr.h"
#include "move.h"
#include "carnage.h"
#include "collsn.h"
#include "turret.h"
#include "terrain.h"
#include "comndr.h"
#include "mission.h"
#include "gate.h"
#include "gamesound.h"
#include "multplyr.h"

//---------------------------------------------------------------------------

// extern float worldUnitsPerMeter;

// extern ObjectMapPtr GameObjectMap;

// int32_t NumCameraDrones = 0;

#define ARTILLERYCHUNK_COMMANDERID_BITS 3
#define ARTILLERYCHUNK_STRIKETYPE_BITS 3
#define ARTILLERYCHUNK_CELLPOS_BITS 10
#define ARTILLERYCHUNK_SECONDS_BITS 6

#define ARTILLERYCHUNK_COMMANDERID_MASK 0x00000007
#define ARTILLERYCHUNK_STRIKETYPE_MASK 0x00000007
#define ARTILLERYCHUNK_CELLPOS_MASK 0x000003FF
#define ARTILLERYCHUNK_SECONDS_MASK 0x0000003F

extern MidLevelRenderer::MLRClipper* theClipper;
extern bool useShadows;

constexpr wchar_t AIRSTRIKE_NAME[] = L"airstrikemarker";
constexpr wchar_t SENSOR_NAME[] = L"sensormarker";

#define GV_LEFT_DUST_ID 0
#define GV_RIGHT_DUST_ID 1

extern bool MLRVertexLimitReached;
//***************************************************************************
// MISC
//***************************************************************************

void CallArtillery(int32_t commanderid, int32_t striketype, Stuff::Vector3D strikeloc,
	int32_t secondsToImpact, bool randomOff)
{
	//-------------------------------------------------------------------
	// If this commander already has 10 strikes going and I'm the server,
	// then no more! If I'm a client, trust the server...
	int32_t numArtillery = 0;
	for (size_t i = 0; i < ObjectManager->getNumArtillery(); i++)
	{
		ArtilleryPtr artillery = ObjectManager->getArtillery(i);
		if (artillery && artillery->getExists() && (artillery->getCommanderId() == commanderid))
			numArtillery++;
	}
	if (numArtillery >= 10)
	{
		if (MPlayer && MPlayer->isServer())
			return;
	}
	ArtilleryPtr artilleryStrike = ObjectManager->createArtillery(striketype, strikeloc);
	if (artilleryStrike)
	{
		artilleryStrike->iFacePosition = strikeloc;
		artilleryStrike->setPosition(strikeloc);
		//-----------------------------------------------------------
		// alignment only really matters for sensor & camera strikes.
		artilleryStrike->setTeamId(Commander::commanders[commanderid]->getTeam()->getId(), true);
		artilleryStrike->setCommanderId(commanderid);
		//-----------------------------------------------------------
		// Set Time to Impact here.  Only if timeToImpact is != -1.0
		if (secondsToImpact != -1)
			artilleryStrike->info.strike.timeToImpact = (float)secondsToImpact;
		// Of course, then our friends the designers set
		// secondsToImpact to 0.  Well Done.  No warning.  Looks like
		// a bug don't it?
		// MINIMUM time to impact is now four!!!!!
		if (secondsToImpact < 2)
			artilleryStrike->info.strike.timeToImpact = -1; // FULL Duration.
		//------------------------------------
		// We must be server if we got here...
		if (MPlayer)
		{
			if ((striketype == ARTILLERY_SMALL) || (striketype == ARTILLERY_LARGE))
				MPlayer->numAirStrikesUsed[commanderid]++;
			else
				MPlayer->numSensorProbesUsed[commanderid]++;
			if (MPlayer->isServer())
				MPlayer->addArtilleryChunk(commanderid, striketype, strikeloc, secondsToImpact);
		}
		artilleryStrike->update(); // call this so if in pause mode, there is
			// something to draw
	}
}

//---------------------------------------------------------------------------

void IfaceCallStrike(int32_t strikeID, Stuff::Vector3D* strikeloc, GameObjectPtr strikeTarget,
	bool playerStrike, bool clanStrike, float timeToImpact)
{
	if ((strikeID != ARTILLERY_LARGE) && (strikeID != ARTILLERY_SMALL) && (strikeID != ARTILLERY_SENSOR))
		return;
	if (!strikeloc && !strikeTarget)
		return;
	Stuff::Vector3D strikeLocation;
	if (strikeloc)
		strikeLocation = *strikeloc;
	else
		strikeLocation = strikeTarget->getPosition();
	bool bRandom = 0;
	int32_t commanderid = -1;
	if (playerStrike)
	{
		commanderid = Commander::home->getId();
		bRandom = Team::home->teamLineOfSight(*strikeloc, 0.0f) ? 0 : 1;
	}
	else if (clanStrike)
	{
		if (MPlayer)
			Fatal(0,
				" Iface.CallStrike: Need more info than clanStrike in "
				"MPlayer ");
		//------------------------------------------------------------
		// In campaign game, clans can strike into unrevealed terrain.
		bRandom = Team::teams[1]->teamLineOfSight(*strikeloc, 0.0f) ? 0 : 1;
		commanderid = 1;
	}
	switch (strikeID)
	{
	case ARTILLERY_LARGE:
	case ARTILLERY_SMALL:
		if (playerStrike && Team::home->teamLineOfSight(strikeLocation, 0.0f))
		{
			soundSystem->playDigitalSample(MAPBUTTONS_GUI);
		}
		break;
	}
	int32_t secondsToImpact = (int32_t)timeToImpact;
	if (strikeID == ARTILLERY_SMALL)
		strikeID = ARTILLERY_LARGE;
	if (strikeID == ARTILLERY_LARGE && bRandom)
		strikeID = ARTILLERY_SMALL;
	if (MPlayer)
	{
		if (MPlayer->isServer())
		{
			CallArtillery(commanderid, strikeID, strikeLocation, secondsToImpact, bRandom);
		}
		else
		{
			MPlayer->sendPlayerArtillery(strikeID, strikeLocation, secondsToImpact);
		}
	}
	else
	{
		CallArtillery(commanderid, strikeID, strikeLocation, secondsToImpact, bRandom);
	}
}

//***************************************************************************
// ARTILLERY CHUNK
//***************************************************************************

PVOID
ArtilleryChunk::operator new(size_t ourSize)
{
	PVOID result;
	result = systemHeap->Malloc(ourSize);
	return (result);
}

//---------------------------------------------------------------------------

void ArtilleryChunk::operator delete(PVOID us)
{
	systemHeap->Free(us);
}

//---------------------------------------------------------------------------

void ArtilleryChunk::build(
	int32_t _commanderId, int32_t _strikeType, Stuff::Vector3D location, int32_t _seconds)
{
	commanderId = _commanderId;
	striketype = _strikeType;
	land->worldToCell(location, cellRC[0], cellRC[1]);
	secondsToImpact = _seconds;
	data = 0;
}

//---------------------------------------------------------------------------

void ArtilleryChunk::pack(void)
{
	data = 0;
	data |= (secondsToImpact + 1);
	data <<= ARTILLERYCHUNK_CELLPOS_BITS;
	data |= cellRC[0];
	data <<= ARTILLERYCHUNK_CELLPOS_BITS;
	data |= cellRC[1];
	data <<= ARTILLERYCHUNK_STRIKETYPE_BITS;
	data |= striketype;
	data <<= ARTILLERYCHUNK_COMMANDERID_BITS;
	data |= commanderId;
}

//---------------------------------------------------------------------------

void ArtilleryChunk::unpack(void)
{
	uint32_t tempData = data;
	commanderId = (tempData & ARTILLERYCHUNK_COMMANDERID_MASK);
	tempData >>= ARTILLERYCHUNK_COMMANDERID_BITS;
	striketype = (tempData & ARTILLERYCHUNK_STRIKETYPE_MASK);
	tempData >>= ARTILLERYCHUNK_STRIKETYPE_BITS;
	cellRC[1] = (tempData & ARTILLERYCHUNK_CELLPOS_MASK);
	tempData >>= ARTILLERYCHUNK_CELLPOS_BITS;
	cellRC[0] = (tempData & ARTILLERYCHUNK_CELLPOS_MASK);
	tempData >>= ARTILLERYCHUNK_CELLPOS_BITS;
	secondsToImpact = (tempData & ARTILLERYCHUNK_SECONDS_MASK) - 1;
}

//---------------------------------------------------------------------------

bool ArtilleryChunk::equalTo(ArtilleryChunkPtr chunk)
{
	if (commanderId != chunk->commanderId)
		return (false);
	if (striketype != chunk->striketype)
		return (false);
	if (cellRC[0] != chunk->cellRC[0])
		return (false);
	if (cellRC[1] != chunk->cellRC[1])
		return (false);
	if (secondsToImpact != chunk->secondsToImpact)
		return (false);
	return (true);
}

//***************************************************************************
// class ArtilleryType
//***************************************************************************

GameObjectPtr
ArtilleryType::createInstance(void)
{
	ArtilleryPtr newArtillery = new Artillery;
	if (!newArtillery)
		return (nullptr);
	newArtillery->init(true, this);
	return (newArtillery);
}

//---------------------------------------------------------------------------

void ArtilleryType::init(void)
{
	ObjectType::init();
	objectTypeClass = ARTILLERY_TYPE;
	objectClass = ARTILLERY;
}

//---------------------------------------------------------------------------

void ArtilleryType::destroy(void)
{
	if (explosionOffsetX)
	{
		systemHeap->Free(explosionOffsetX);
		explosionOffsetX = nullptr;
	}
	if (explosionOffsetY)
	{
		systemHeap->Free(explosionOffsetY);
		explosionOffsetY = nullptr;
	}
	if (explosionDelay)
	{
		systemHeap->Free(explosionDelay);
		explosionDelay = nullptr;
	}
	ObjectType::destroy();
}

//---------------------------------------------------------------------------

int32_t
ArtilleryType::init(std::unique_ptr<File> objFile, uint32_t fileSize)
{
	int32_t result = 0;
	FitIniFile miFile;
	result = miFile.open(objFile, fileSize);
	if (result != NO_ERROR)
		return (result);
	//---------------------------------------------------------------
	// Load up the artillery data.
	result = miFile.seekBlock("Artillery");
	if (result != NO_ERROR)
		return (result);
	wchar_t artillerySpriteName[80];
	result = miFile.readIdString("ArtillerySpriteName", artillerySpriteName, 79);
	if (result != NO_ERROR)
		return (result);
	result = miFile.readIdULong("FrameCount", frameCount);
	if (result != NO_ERROR)
		return (result);
	result = miFile.readIdULong("StartFrame", startFrame);
	if (result != NO_ERROR)
		return (result);
	result = miFile.readIdFloat("FrameRate", frameRate);
	if (result != NO_ERROR)
		return (result);
	result = miFile.readIdFloat("NominalTimeToImpact", nominalTimeToImpact);
	if (result != NO_ERROR)
		return (result);
	result = miFile.readIdFloat("NominalTimeToLaunch", nominalTimeToLaunch);
	if (result != NO_ERROR)
		nominalTimeToLaunch = nominalTimeToImpact - 10;
	result = miFile.readIdFloat("NominalDamage", nominalDamage);
	if (result != NO_ERROR)
		return (result);
	result = miFile.readIdFloat("NominalMajorRange", nominalMajorRange);
	if (result != NO_ERROR)
		return (result);
	result = miFile.readIdFloat("NominalMajorHits", nominalMajorHits);
	if (result != NO_ERROR)
		return (result);
	result = miFile.readIdFloat("NominalMinorRange", nominalMinorRange);
	if (result != NO_ERROR)
		return (result);
	result = miFile.readIdFloat("NominalMinorHits", nominalMinorHits);
	if (result != NO_ERROR)
		return (result);
	result = miFile.readIdFloat("NominalSensorTime", nominalSensorTime);
	if (result != NO_ERROR)
		return (result);
	result = miFile.readIdFloat("NominalSensorRange", nominalSensorRange);
	if (result != NO_ERROR)
		return (result);
	result = miFile.readIdFloat("fontScale", fontScale);
	if (result != NO_ERROR)
		return (result);
	result = miFile.readIdFloat("fontXOffset", fontXOffset);
	if (result != NO_ERROR)
		return (result);
	result = miFile.readIdFloat("fontYOffset", fontYOffset);
	if (result != NO_ERROR)
		return (result);
	result = miFile.readIdULong("fontcolour", fontcolour);
	if (result != NO_ERROR)
		return (result);
	if (nominalDamage)
	{
		result = miFile.readIdLong("NumExplosions", numExplosions);
		if (result != NO_ERROR)
			return (result);
		explosionOffsetX = (float*)systemHeap->Malloc(sizeof(float) * numExplosions);
		gosASSERT(explosionOffsetX != nullptr);
		explosionOffsetY = (float*)systemHeap->Malloc(sizeof(float) * numExplosions);
		gosASSERT(explosionOffsetY != nullptr);
		explosionDelay = (float*)systemHeap->Malloc(sizeof(float) * numExplosions);
		gosASSERT(explosionDelay != nullptr);
		for (size_t i = 0; i < numExplosions; i++)
		{
			wchar_t explosionId[50];
			sprintf(explosionId, "ExplosionDelay%d", i);
			result = miFile.readIdFloat(explosionId, explosionDelay[i]);
			if (result != NO_ERROR)
				return (result);
			sprintf(explosionId, "ExplosionOffsetX%d", i);
			result = miFile.readIdFloat(explosionId, explosionOffsetX[i]);
			if (result != NO_ERROR)
				return (result);
			sprintf(explosionId, "ExplosionOffsetY%d", i);
			result = miFile.readIdFloat(explosionId, explosionOffsetY[i]);
			if (result != NO_ERROR)
				return (result);
		}
		result = miFile.readIdLong("ExplosionsPerExplosion", numExplosionsPerExplosion);
		if (result != NO_ERROR)
			return (result);
		result = miFile.readIdLong("ExplosionRandomOffsetX", explosionRandomX);
		if (result != NO_ERROR)
			return (result);
		result = miFile.readIdLong("ExplosionRandomOffsetY", explosionRandomY);
		if (result != NO_ERROR)
			return (result);
		result = miFile.readIdLong("MinArtilleryHeadRange", minArtilleryHeadRange);
		if (result != NO_ERROR)
			minArtilleryHeadRange = 5.0;
	}
	else
	{
		explosionOffsetX = explosionOffsetY = explosionDelay = nullptr;
	}
	//-------------------------------------------------------
	// Initialize the base object Type from the current file.
	result = ObjectType::init(&miFile);
	return (result);
}

//---------------------------------------------------------------------------
bool ArtilleryType::handleCollision(GameObjectPtr collidee, GameObjectPtr collider)
{
	if (MPlayer && !MPlayer->isServer())
		return (false);
	//----------------------------------------------------------
	// Artillery counts as damage IF AND ONLY IF the artillery
	// strike arrives during the collision time.  At all other
	// times, the artillery doesn't matter.
	ArtilleryPtr artillery = (ArtilleryPtr)collidee;
	ArtilleryTypePtr artilleryType = (ArtilleryTypePtr)artillery->getObjectType();
	if (artillery->getFlag(OBJECT_FLAG_BOOM))
	{
		//---------------------
		// Get Range to target.
		Stuff::Vector3D distance = collider->getPosition();
		distance -= collidee->getPosition();
		distance.z = 0.0; // Do NOT use elevation!!!  Will make it look like its
			// not WORKING!!!!
		float range = distance.GetLength() * metersPerWorldUnit;
		if ((collider->getObjectClass() == GATE) || (collider->getObjectClass() == TURRET))
		{
			switch (collider->getObjectClass())
			{
			case GATE:
			{
				//---------------------------------------------------
				// An explosion may NOT damage a gate or turret unless
				// is is within the "little" extent radius.
				float littleExtent = ((GatePtr)collider)->getLittleExtent() * metersPerWorldUnit;
				if (littleExtent < range)
				{
					float realRange = range - littleExtent;
					if (realRange > ((ArtilleryTypePtr)artillery->getObjectType())->nominalMajorRange)
						return FALSE;
				}
			}
			break;
			case TURRET:
			{
				//---------------------------------------------------
				// An explosion may NOT damage a gate or turret unless
				// is is within the "little" extent radius.
				float littleExtent = ((TurretPtr)collider)->getLittleExtent() * metersPerWorldUnit;
				if (littleExtent < range)
				{
					float realRange = range - littleExtent;
					if (realRange > artilleryType->nominalMajorRange)
						return (false);
				}
			}
			break;
			}
		}
		else if ((collider->getObjectClass() == BATTLEMECH) && (((std::unique_ptr<Mover>)collider)->getMoveType() == MOVETYPE_AIR) && (collider->getStatus() != OBJECT_STATUS_SHUTDOWN))
		{
			// DO Nothing.  Helicopters are immune.
			return false;
		}
		bool isMajorHit = (range <= artilleryType->nominalMajorRange);
		int32_t numHits = artilleryType->nominalMajorHits;
		if (!isMajorHit)
			numHits = artilleryType->nominalMinorHits;
		for (size_t i = 0; i < numHits; i++)
		{
			WeaponShotInfo shot;
			shot.init(collidee->getWatchID(), -3, artilleryType->nominalDamage, 0, 0);
			if (collider->isMover())
			{
				if (range <= minArtilleryHeadRange)
					shot.hitLocation = collider->calcHitLocation(collidee, -1, ATTACKSOURCE_DFA, 0);
				else
					shot.hitLocation =
						collider->calcHitLocation(collidee, -1, ATTACKSOURCE_ARTILLERY, 0);
				shot.setEntryAngle(collider->relFacingTo(collidee->getPosition()));
			}
			collider->handleWeaponHit(&shot, (MPlayer != nullptr));
		}
	}
	return (false);
}

//---------------------------------------------------------------------------
bool ArtilleryType::handleDestruction(GameObjectPtr collidee, GameObjectPtr collider)
{
	//-------------------------------------------------------
	// Artillery goes away when it hits.  Nothing happens as
	// a result of a collision.
	return (false);
}

//***************************************************************************
// class Artillery
//***************************************************************************
void Artillery::init(bool create)
{
	artilleryType = ARTILLERY_SMALL;
	initFlags();
	setFlag(OBJECT_FLAG_JUSTCREATED, true);
	info.strike.timeToImpact = -1.0;
	info.strike.timeToLaunch = -1.0;
	info.strike.sensorRange = 0.0;
	info.strike.contactUpdate = scenarioTime;
	info.strike.sensorSystemIndex = -1;
	info.strike.timeToBlind = 0.0;
	hitEffect = nullptr;
	leftContrail = nullptr;
	rightContrail = nullptr;
	bomber = nullptr;
	setFlag(OBJECT_FLAG_RANDOM_OFFSET, 0);
}

//---------------------------------------------------------------------------
void Artillery::init(bool create, int32_t _artilleryType)
{
	init(create);
	artilleryType = _artilleryType;
	if (isStrike())
	{
	}
	else if (isSensor())
	{
	}
}

//---------------------------------------------------------------------------
void Artillery::handleStaticCollision(void)
{
	if (getFlag(OBJECT_FLAG_TANGIBLE))
	{
		//-----------------------------------------------------
		// What is our block and vertex number?
		int32_t blockNumber = 0;
		int32_t vertexNumber = 0;
		getBlockAndVertexNumber(blockNumber, vertexNumber);
		int32_t CellRow, CellCol;
		land->worldToCell(getPosition(), CellRow, CellCol);
		//-------------------------------------------------------
		// MUST figure out radius to set off mines in CELLS now.
		// -fs
		int32_t startCellRow = CellRow - 4;
		int32_t startCellCol = CellCol - 4;
		int32_t i = 0;
		for (i = startCellRow; i < startCellRow + 9; i++)
		{
			for (size_t j = startCellCol; j < startCellCol + 9; j++)
			{
				if (GameMap->inBounds(i, j))
				{
					int32_t mineResult = 0;
					mineResult = GameMap->getMine(i, j);
					if (mineResult == 1)
					{
						Stuff::Vector3D minePosition;
						land->cellToWorld(i, j, minePosition);
						GameMap->setMine(i, j, 2);
						if (MPlayer)
						{
							MPlayer->addMineChunk(i, j, 1, 2, 2);
						}
						ObjectManager->createExplosion(MINE_EXPLOSION_ID, nullptr, minePosition,
							MineSplashDamage, MineSplashRange * worldUnitsPerMeter);
					}
				}
			}
		}
		//-------------------------------------------------------------------------
		// We must now move out into other tiles for the artillery strike to
		// work. Remember, Its pretty big! Just grab the nine vertices around
		// this one.  Problems arise when on Block border.  handle it.
		blockNumber = 0;
		vertexNumber = 0;
		getBlockAndVertexNumber(blockNumber, vertexNumber);
		//-------------------------------------------------------------------------
		// We must now move out into other tiles for the artillery strike to
		// work. Remember, Its pretty big! Just grab the nine vertices around
		// this one.  Problems arise when on Block border.  handle it.
		int32_t topLeftBlockNumber = blockNumber - Terrain::blocksMapSide - 1;
		int32_t currentBlockNumber = topLeftBlockNumber;
		int32_t totalBlocks = Terrain::blocksMapSide * Terrain::blocksMapSide;
		for (i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				if ((currentBlockNumber >= 0) && (currentBlockNumber < totalBlocks))
				{
					int32_t numObjectsInBlock =
						ObjectManager->getObjBlockNumObjects(currentBlockNumber);
					for (size_t objIndex = 0; objIndex < numObjectsInBlock; objIndex++)
					{
						GameObjectPtr obj =
							ObjectManager->getObjBlockObject(currentBlockNumber, objIndex);
						if (!obj)
							STOP(("Object Number %d in terrain Block %d was "
								  "nullptr!",
								objIndex, currentBlockNumber));
						if (obj->getExists())
							ObjectManager->detectStaticCollision(this, obj);
					}
				}
				currentBlockNumber++;
			}
			currentBlockNumber = topLeftBlockNumber + (Terrain::blocksMapSide * (i + 1));
		}
	}
}

//---------------------------------------------------------------------------
void Artillery::setJustCreated(void)
{
	if (getFlag(OBJECT_FLAG_JUSTCREATED))
	{
		ArtilleryTypePtr type = (ArtilleryTypePtr)getObjectType();
		setFlag(OBJECT_FLAG_JUSTCREATED, false);
		if (info.strike.timeToImpact == -1.0)
			info.strike.timeToImpact = type->nominalTimeToImpact;
		info.strike.timeToLaunch = type->nominalTimeToLaunch;
		info.strike.sensorRange = type->nominalSensorRange;
		info.strike.timeToBlind = 0.0;
		setFlag(OBJECT_FLAG_TANGIBLE, false);
		setFlag(OBJECT_FLAG_SENSORS_GOING, false);
		if (info.strike.sensorRange)
		{
			int32_t cellR, cellC;
			land->worldToCell(position, cellR, cellC);
			if (GameMap->getDeepWater(cellR, cellC) || GameMap->getShallowWater(cellR, cellC))
				effectId--;
		}
		// Create the GOSFX here.
		if (strcmp(weaponEffects->GetEffectName(effectId), "NONE") != 0)
		{
			//--------------------------------------------
			// Yes, load it on up.
			uint32_t flags = gosFX::Effect::ExecuteFlag;
			Check_Object(gosFX::EffectLibrary::Instance);
			gosFX::Effect::Specification* gosEffectSpec =
				gosFX::EffectLibrary::Instance->Find(weaponEffects->GetEffectName(effectId));
			if (gosEffectSpec)
			{
				hitEffect =
					gosFX::EffectLibrary::Instance->MakeEffect(gosEffectSpec->m_effectID, flags);
				gosASSERT(hitEffect != nullptr);
				MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
			}
		}
		if (strcmp(weaponEffects->GetEffectName(JET_CONTRAIL_ID), "NONE") != 0)
		{
			//--------------------------------------------
			// Yes, load it on up.
			uint32_t flags = gosFX::Effect::ExecuteFlag;
			Check_Object(gosFX::EffectLibrary::Instance);
			gosFX::Effect::Specification* gosEffectSpec =
				gosFX::EffectLibrary::Instance->Find(weaponEffects->GetEffectName(JET_CONTRAIL_ID));
			if (gosEffectSpec)
			{
				// leftContrail =
				// gosFX::EffectLibrary::Instance->MakeEffect(gosEffectSpec->m_effectID,
				// flags);  gosASSERT(leftContrail != nullptr);
				rightContrail =
					gosFX::EffectLibrary::Instance->MakeEffect(gosEffectSpec->m_effectID, flags);
				gosASSERT(rightContrail != nullptr);
				MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
			}
		}
		if (info.strike.sensorRange)
		{
			SensorSystemPtr sensor = SensorManager->newSensor();
			info.strike.sensorSystemIndex = sensor->id;
			setSensorData(getTeam());
			if ((info.strike.timeToBlind == 0.0f) && !getFlag(OBJECT_FLAG_SENSORS_GOING))
			{
				if (hitEffect)
				{
					Stuff::LinearMatrix4D shapeOrigin;
					Stuff::Point3D actualPosition;
					actualPosition.x = -position.x;
					actualPosition.y = position.z;
					actualPosition.z = position.y;
					shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
					shapeOrigin.BuildTranslation(actualPosition);
					gosFX::Effect::ExecuteInfo info(
						(Stuff::Time)scenarioTime, &shapeOrigin, nullptr);
					hitEffect->Start(&info);
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
int32_t
Artillery::update(void)
{
	if (getFlag(OBJECT_FLAG_JUSTCREATED))
	{
		setJustCreated();
	}
	else
	{
		info.strike.timeToImpact -= frameLength;
		info.strike.timeToLaunch -= frameLength;
	}
	ArtilleryTypePtr type = (ArtilleryTypePtr)getObjectType();
	recalcBounds(eye); // Are we even visible?
	if (inView)
	{
		windowsVisible = turn;
	}
	//-------------------------------------------------------------------------------
	// Special Cases here.  If we are an explosive round, let everyone know we
	// went.
	if (getFlag(OBJECT_FLAG_BOOM) && (type->nominalDamage > 0.0))
	{
		setFlag(OBJECT_FLAG_TANGIBLE, false);
		//------------------------------------------------
		// Update GOSFX
		if (hitEffect && hitEffect->IsExecuted())
		{
			Stuff::Point3D actualPosition;
			actualPosition.x = -position.x;
			actualPosition.y = position.z;
			actualPosition.z = position.y;
			Stuff::LinearMatrix4D shapeOrigin;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(actualPosition);
			Stuff::OBB boundingBox;
			gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, &boundingBox);
			bool result = hitEffect->Execute(&info);
			if (!result)
			{
				hitEffect->Kill(); // Effect is over.  Otherwise, wait until
					// hit!
				delete hitEffect;
				hitEffect = nullptr;
				if (bomber)
				{
					delete bomber;
					bomber = nullptr;
				}
				if (leftContrail)
				{
					leftContrail->Kill();
					delete leftContrail;
					leftContrail = nullptr;
				}
				if (rightContrail)
				{
					rightContrail->Kill();
					delete rightContrail;
					rightContrail = nullptr;
				}
				return false; // Strike is also over
			}
		}
	}
	if ((info.strike.timeToImpact > 0.0f) || (type->nominalDamage == 0.0))
	{
		bool oldShadows = useShadows;
		useShadows = false;
		appearance->setObjectParameters(iFacePosition, ((ObjectAppearance*)appearance)->rotation,
			false, getTeamId(), Team::getRelation(getTeamId(), Team::home->getId()));
		appearance->recalcBounds();
		appearance->update();
		useShadows = oldShadows;
	}
	if (bomber && (info.strike.timeToImpact <= 2.5f) && !bombRunStarted)
	{
		bomber->setObjectParameters(position, 0.0f, false, getTeamId(),
			Team::getRelation(getTeamId(), Team::home->getId()));
		bomber->setMoverParameters(0.0f);
		bomber->setGesture(0);
		bomber->setObjStatus(OBJECT_STATUS_DESTROYED);
		bomber->recalcBounds();
		bomber->setVisibility(true, true);
		bomber->update(); // Force it to try and draw or stuff will not work!
		bombRunStarted = true;
		if (rightContrail && rightContrail->IsExecuted())
		{
			Stuff::LinearMatrix4D shapeOrigin;
			Stuff::Vector3D lPosition = bomber->getNodeNamePosition("activity_node");
			Stuff::Point3D actualPosition;
			actualPosition.x = -lPosition.x;
			actualPosition.y = lPosition.z;
			actualPosition.z = lPosition.y;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(actualPosition);
			gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, nullptr);
			rightContrail->Start(&info);
		}
		if (leftContrail && leftContrail->IsExecuted())
		{
			Stuff::LinearMatrix4D shapeOrigin;
			Stuff::Vector3D lPosition = bomber->getDustNodePosition(GV_LEFT_DUST_ID);
			Stuff::Point3D actualPosition;
			actualPosition.x = -lPosition.x;
			actualPosition.y = lPosition.z;
			actualPosition.z = lPosition.y;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(actualPosition);
			gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, nullptr);
			leftContrail->Start(&info);
		}
	}
	if (bomber && bombRunStarted)
	{
		bomber->setObjectParameters(position, 0.0f, false, getTeamId(),
			Team::getRelation(getTeamId(), Team::home->getId()));
		bomber->recalcBounds();
		bomber->setVisibility(true, true);
		bomber->update();
		if (bomber && leftContrail && leftContrail->IsExecuted())
		{
			Stuff::Vector3D lPosition = bomber->getDustNodePosition(GV_LEFT_DUST_ID);
			Stuff::Point3D actualPosition;
			actualPosition.x = -lPosition.x;
			actualPosition.y = lPosition.z;
			actualPosition.z = lPosition.y;
			Stuff::LinearMatrix4D shapeOrigin;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(actualPosition);
			Stuff::OBB boundingBox;
			gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, &boundingBox);
			leftContrail->Execute(&info);
		}
		if (bomber && rightContrail && rightContrail->IsExecuted())
		{
			Stuff::Vector3D lPosition = bomber->getNodeNamePosition("activity_node");
			Stuff::Point3D actualPosition;
			actualPosition.x = -lPosition.x;
			actualPosition.y = lPosition.z;
			actualPosition.z = lPosition.y;
			Stuff::LinearMatrix4D shapeOrigin;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(actualPosition);
			Stuff::OBB boundingBox;
			gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, &boundingBox);
			rightContrail->Execute(&info);
		}
	}
	if ((info.strike.timeToImpact <= 5.5) && !getFlag(OBJECT_FLAG_WHOOSH))
	{
		if (soundSystem && (type->nominalDamage > 0.0))
		{
			setFlag(OBJECT_FLAG_WHOOSH, true);
			soundSystem->playDigitalSample(INCOMING_AIRSTRIKE, position, true);
		}
	}
	if (!getFlag(OBJECT_FLAG_BOOM) && (info.strike.timeToImpact <= 0.0) && (type->nominalDamage > 0.0))
	{
		Stuff::Vector3D actualPosition = position;
		//-----------------------------------
		// Create the giant artillery blast.
		// GOS Fx now.
		if (hitEffect && hitEffect->IsExecuted())
		{
			Stuff::LinearMatrix4D shapeOrigin;
			Stuff::Point3D actualPosition;
			actualPosition.x = -position.x;
			actualPosition.y = position.z;
			actualPosition.z = position.y;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(actualPosition);
			gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, nullptr);
			hitEffect->Start(&info);
		}
		//------------------------------------------------
		// Update GOSFX -- One free update here.
		if (hitEffect && hitEffect->IsExecuted())
		{
			Stuff::Point3D actualPosition;
			actualPosition.x = -position.x;
			actualPosition.y = position.z;
			actualPosition.z = position.y;
			Stuff::LinearMatrix4D shapeOrigin;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(actualPosition);
			Stuff::OBB boundingBox;
			gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, &boundingBox);
			bool result = hitEffect->Execute(&info);
			if (!result)
			{
				hitEffect->Kill(); // Effect is over.  Otherwise, wait until
					// hit!
				delete hitEffect;
				hitEffect = nullptr;
			}
		}
		setFlag(OBJECT_FLAG_BOOM, true);
		setFlag(OBJECT_FLAG_TANGIBLE, true);
	}
	//----------------------------------------------------------------------------
	// Sensor round is ticking now.  Update Everything.
	if ((info.strike.timeToBlind > 0.0) && getFlag(OBJECT_FLAG_SENSORS_GOING))
	{
		info.strike.timeToBlind -= frameLength;
		info.strike.sensorRange = info.strike.timeToBlind / type->nominalSensorTime;
		info.strike.sensorRange *= type->nominalSensorRange;
		info.strike.sensorRange *= worldUnitsPerMeter;
		SensorSystemPtr sensor = SensorManager->getSensor(info.strike.sensorSystemIndex);
		Assert(sensor != nullptr, 0, " Artillery.update: nullptr sensor ");
		sensor->setRange(info.strike.sensorRange * metersPerWorldUnit);
		//--------------------------------------
		// Actual scan is done in team update...
	}
	else if ((info.strike.timeToBlind <= 0.0) && getFlag(OBJECT_FLAG_SENSORS_GOING))
	{
		//---------------------------------------------------
		// All done, return FALSE;
		SensorSystemPtr sensor = SensorManager->getSensor(info.strike.sensorSystemIndex);
		SensorManager->removeTeamSensor(getTeam()->getId(), sensor);
		SensorManager->freeSensor(sensor);
		return (0);
	}
	//-------------------------------------------------------------------------------------------
	// If we are a sensor round, start the sensor countdown.  ALWAYS on, even
	// before effect hits. time to blind is being reset everytime without this
	// else
	else if (type->nominalSensorTime > 0)
	{
		info.strike.timeToBlind = type->nominalSensorTime;
		setFlag(OBJECT_FLAG_SENSORS_GOING, true);
	}
	if (type->nominalDamage == 0)
	{
		//-------------------------------------
		// Create the Sensor Probe Landing now
		// GOS Fx now.
		if (hitEffect)
		{
			Stuff::Point3D actualPosition;
			actualPosition.x = -position.x;
			actualPosition.y = position.z;
			actualPosition.z = position.y;
			Stuff::LinearMatrix4D shapeOrigin;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(actualPosition);
			Stuff::OBB boundingBox;
			gosFX::Effect::ExecuteInfo xinfo((Stuff::Time)scenarioTime, &shapeOrigin, &boundingBox);
			bool result = hitEffect->Execute(&xinfo);
			if (!result)
			{
				hitEffect->Kill(); // Effect is over.  Otherwise, wait until
					// hit!
				delete hitEffect;
				hitEffect = nullptr;
			}
		}
	}
	return (1);
}

//---------------------------------------------------------------------------
bool Artillery::recalcBounds(CameraPtr myEye)
{
	if (myEye)
	{
		//--------------------------------------------------
		// First, if we are using perspective, figure out
		// if object too far from camera.  Far Clip Plane.
		if (eye->usePerspective)
		{
			Stuff::Point3D Distance;
			Stuff::Point3D objPosition(position);
			Stuff::Point3D eyePosition(eye->getPosition());
			Distance.Subtract(objPosition, eyePosition);
			float eyeDistance = Distance.GetApproximateLength();
			if (eyeDistance > Camera::MaxClipDistance)
			{
				inView = false;
			}
			else
			{
				inView = true;
			}
			//-----------------------------------------------------------------
			// If inside farClip plane, check if behind camera.
			// Find angle between lookVector of Camera and vector from camPos
			// to Target.  If angle is less then halfFOV, object is visible.
			if (inView)
			{
				Stuff::Vector3D Distance;
				Stuff::Point3D objPosition;
				Stuff::Point3D eyePosition(eye->getCameraOrigin());
				objPosition.x = -position.x;
				objPosition.y = position.z;
				objPosition.z = position.y;
				Distance.Subtract(objPosition, eyePosition);
				Distance.Normalize(Distance);
				float cosine = Distance * eye->getLookVector();
				if (cosine > eye->cosHalfFOV)
					inView = true;
				else
					inView = false;
			}
		}
		else
		{
			inView = true;
		}
		if (inView)
		{
			eye->projectZ(position, screenPos);
			Stuff::Vector4D iFaceScreen;
			eye->projectZ(iFacePosition, iFaceScreen);
			if ((screenPos.x >= 0) && (screenPos.y >= 0) && (screenPos.x <= eye->getScreenResX()) && (screenPos.y <= eye->getScreenResY()) || ((iFaceScreen.x >= 0) && (iFaceScreen.y >= 0) && (iFaceScreen.x <= eye->getScreenResX()) && (iFaceScreen.y <= eye->getScreenResY())))
			{
				inView = true;
			}
			else
			{
				inView = false;
			}
		}
	}
	return (inView);
}

//---------------------------------------------------------------------------
TeamPtr
Artillery::getTeam(void)
{
	if (teamId == -1)
		return (nullptr);
	return (Team::teams[teamId]);
}

//---------------------------------------------------------------------------
void Artillery::setCommanderId(int32_t _commanderId)
{
	commanderId = _commanderId;
}

//---------------------------------------------------------------------------
int32_t
Artillery::setTeamId(int32_t _teamId, bool setup)
{
	teamId = _teamId;
	Assert(teamId > -1, teamId, " Mover.setTeamId: bad teamId ");
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

bool Artillery::isFriendly(TeamPtr team)
{
	if (teamId > -1)
		return (Team::relations[teamId][team->getId()] == RELATION_FRIENDLY);
	return (false);
}

//---------------------------------------------------------------------------

bool Artillery::isEnemy(TeamPtr team)
{
	if (teamId > -1)
		return (Team::relations[teamId][team->getId()] == RELATION_ENEMY);
	return (false);
}

//---------------------------------------------------------------------------

bool Artillery::isNeutral(TeamPtr team)
{
	if (teamId > -1)
		return (Team::relations[teamId][team->getId()] == RELATION_NEUTRAL);
	return (true);
}

//---------------------------------------------------------------------------
void Artillery::setSensorRange(float range)
{
	SensorSystemPtr sensor = SensorManager->getSensor(info.strike.sensorSystemIndex);
	if (sensor)
		sensor->setRange(range);
}

//---------------------------------------------------------------------------
void Artillery::setSensorData(TeamPtr team, float sensorTime, float range)
{
	if (sensorTime != -1.0)
		info.strike.timeToBlind = sensorTime;
	if (range != -1.0)
		info.strike.sensorRange = range;
	SensorSystemPtr sensor = SensorManager->getSensor(info.strike.sensorSystemIndex);
	Assert(sensor != nullptr, info.strike.sensorSystemIndex,
		" Artillery.setSensorData: nullptr sensor ");
	sensor->setOwner(this);
	SensorManager->addTeamSensor(team->getId(), sensor);
	sensor->setRange(range);
	sensor->setLOSCapability(false);
}

//---------------------------------------------------------------------------
void Artillery::drawSelectBox(uint8_t color)
{
}

//---------------------------------------------------------------------------
void Artillery::render(void)
{
	if (inView)
	{
		if (hitEffect)
		{
			gosFX::Effect::DrawInfo drawInfo;
			drawInfo.m_clipper = theClipper;
			MidLevelRenderer::MLRState mlrState;
			// mlrState.SetBackFaceOn();
			mlrState.SetDitherOn();
			mlrState.SetTextureCorrectionOn();
			mlrState.SetZBufferCompareOn();
			mlrState.SetZBufferWriteOn();
			// mlrState.SetFilterMode(MidLevelRenderer::MLRState::BiLinearFilterMode);
			// mlrState.SetAlphaMode(MidLevelRenderer::MLRState::AlphaInvAlphaMode);
			drawInfo.m_state = mlrState;
			drawInfo.m_clippingFlags = 0x0;
			Stuff::Point3D actualPosition;
			actualPosition.x = -position.x;
			actualPosition.y = position.z;
			actualPosition.z = position.y;
			Stuff::LinearMatrix4D shapeOrigin;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(actualPosition);
			drawInfo.m_parentToWorld = &shapeOrigin;
			if (!MLRVertexLimitReached)
				hitEffect->Draw(&drawInfo);
		}
		if ((!MPlayer && (info.strike.timeToImpact > 0.f)) || (MPlayer && info.strike.timeToImpact > 0.f && ((teamId == Team::home->getId()) || info.strike.timeToImpact < 3.0f)))
		{
			// We are drawing the timer.
			// Draw it.
			wchar_t text[256];
			switch (artilleryType)
			{
			case ARTILLERY_LARGE:
				sprintf(text, "%02.2f", info.strike.timeToImpact);
				break;
			case ARTILLERY_SMALL:
				sprintf(text, "%02.2f", info.strike.timeToImpact);
				break;
			case ARTILLERY_SENSOR:
				text[0] = 0; // players are confused by sensor count down
				break;
			}
			uint32_t width, height;
			Stuff::Vector4D moveHere;
			eye->projectZ(iFacePosition, moveHere);
			gos_TextSetAttributes(gosFontHandle, 0, gosFontScale, false, false, false, false);
			gos_TextStringLength(&width, &height, text);
			moveHere.y += 10.0f;
			moveHere.x -= width / 2;
			moveHere.z = width;
			moveHere.w = height;
			globalFloatHelp->setFloatHelp(
				text, moveHere, SD_GREEN, 0x0, gosFontScale, false, false, false, false);
			if (appearance->canBeSeen())
			{
				windowsVisible = turn;
				appearance->setVisibility(true, true);
				appearance->render(-1);
			}
		}
	}
	if (bomber && bombRunStarted)
	{
		bomber->setVisibility(true, true);
		bomber->render();
		bomber->renderShadows();
		gosFX::Effect::DrawInfo drawInfo;
		drawInfo.m_clipper = theClipper;
		MidLevelRenderer::MLRState mlrState;
		// mlrState.SetBackFaceOn();
		mlrState.SetDitherOn();
		mlrState.SetTextureCorrectionOn();
		mlrState.SetZBufferCompareOn();
		mlrState.SetZBufferWriteOn();
		// mlrState.SetFilterMode(MidLevelRenderer::MLRState::BiLinearFilterMode);
		// mlrState.SetAlphaMode(MidLevelRenderer::MLRState::AlphaInvAlphaMode);
		drawInfo.m_state = mlrState;
		drawInfo.m_clippingFlags = 0x0;
		if (leftContrail)
		{
			Stuff::Vector3D lPosition = bomber->getDustNodePosition(GV_LEFT_DUST_ID);
			Stuff::Point3D actualPosition;
			actualPosition.x = -lPosition.x;
			actualPosition.y = lPosition.z;
			actualPosition.z = lPosition.y;
			Stuff::LinearMatrix4D shapeOrigin;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(actualPosition);
			drawInfo.m_parentToWorld = &shapeOrigin;
			if (!MLRVertexLimitReached)
				leftContrail->Draw(&drawInfo);
		}
		if (rightContrail)
		{
			Stuff::Vector3D lPosition = bomber->getNodeNamePosition("activity_node");
			Stuff::Point3D actualPosition;
			actualPosition.x = -lPosition.x;
			actualPosition.y = lPosition.z;
			actualPosition.z = lPosition.y;
			Stuff::LinearMatrix4D shapeOrigin;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(actualPosition);
			drawInfo.m_parentToWorld = &shapeOrigin;
			if (!MLRVertexLimitReached)
				rightContrail->Draw(&drawInfo);
		}
	}
}

//---------------------------------------------------------------------------
int32_t
Artillery::handleWeaponHit(WeaponShotInfoPtr shotInfo, bool addMultiplayChunk)
{
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
void Artillery::destroy(void)
{
	// Must delete here too in case effect was NOT over when mission ended!
	if (hitEffect)
	{
		hitEffect->Kill();
		delete hitEffect;
		hitEffect = nullptr;
	}
	if (leftContrail)
	{
		leftContrail->Kill();
		delete leftContrail;
		leftContrail = nullptr;
	}
	if (rightContrail)
	{
		rightContrail->Kill();
		delete rightContrail;
		rightContrail = nullptr;
	}
	if (bomber)
	{
		delete bomber;
		bomber = nullptr;
	}
}

//---------------------------------------------------------------------------
void Artillery::init(bool create, ObjectTypePtr _type)
{
	GameObject::init(create, _type);
	initFlags();
	switch (_type->getObjectTypeClass())
	{
	case ARTILLERY_TYPE:
		objectClass = ARTILLERY;
		setFlag(OBJECT_FLAG_EXISTS, true);
		setFlag(OBJECT_FLAG_JUSTCREATED, true);
		setFlag(OBJECT_FLAG_BOOM, false);
		info.strike.timeToImpact = -1.0;
		info.strike.explosionOffFlags = 0;
		break;
	}
	effectId = -1;
	switch (_type->whatAmI())
	{
	case BIG_ARTLRY:
	{
		effectId = LARGE_AIRSTRIKE_ID;
		init(ARTILLERY_LARGE);
		//--------------------------------------------
		// Load up the strike appearance before hit.
		wchar_t appearName[1024];
		strcpy(appearName, AIRSTRIKE_NAME);
		//--------------------------------------------------------------
		// New code!!!
		// We need to append the sprite type to the appearance num now.
		// The MechEdit tool does not assume a sprite type, nor should it.
		// MechCmdr2 features much simpler objects which only use 1 type of
		// sprite!
		int32_t appearanceType = (BLDG_TYPE << 24);
		AppearanceTypePtr buildingAppearanceType = nullptr;
		if (!appearName)
		{
			//------------------------------------------------------
			// LOAD a dummy appearance until real ones are available
			// for this building!
			appearanceType = (BLDG_TYPE << 24);
			buildingAppearanceType = appearanceTypeList->getAppearance(appearanceType, "TESTBLDG");
		}
		else
		{
			buildingAppearanceType = appearanceTypeList->getAppearance(appearanceType, appearName);
			if (!buildingAppearanceType)
			{
				wchar_t msg[1024];
				sprintf(msg, "No Building Appearance Named %s", appearName);
				Fatal(0, msg);
			}
		}
		appearance = new BldgAppearance;
		gosASSERT(appearance != nullptr);
		//--------------------------------------------------------------
		// The only appearance type for buildings is MLR_APPEARANCE.
		gosASSERT(buildingAppearanceType->getAppearanceClass() == BLDG_TYPE);
		appearance->init((BldgAppearanceType*)buildingAppearanceType, (GameObjectPtr)this);
		//--------------------------------------------------------------
		appearanceType = (GV_TYPE << 24);
		AppearanceTypePtr bomberAppearanceType =
			appearanceTypeList->getAppearance(appearanceType, "Shilone");
		if (!bomberAppearanceType)
		{
			STOP(("Unable to create Bomber for air strike Shilone"));
		}
		if ((appearanceType >> 24) == GV_TYPE)
		{
			bomber = new GVAppearance;
			if (!bomber)
				STOP((" Artillery.init: unable to create appearance for "
					  "Shilone"));
			//-----------------------------------------------------------------
			bomber->init((GVAppearanceType*)bomberAppearanceType, (GameObjectPtr)this);
		}
		bombRunStarted = false;
	}
	break;
	case SMALL_ARTLRY:
	{
		effectId = SMALL_AIRSTRIKE_ID;
		init(ARTILLERY_SMALL);
		//--------------------------------------------
		// Load up the strike appearance before hit.
		wchar_t appearName[1024];
		strcpy(appearName, AIRSTRIKE_NAME);
		//--------------------------------------------------------------
		// New code!!!
		// We need to append the sprite type to the appearance num now.
		// The MechEdit tool does not assume a sprite type, nor should it.
		// MechCmdr2 features much simpler objects which only use 1 type of
		// sprite!
		int32_t appearanceType = (BLDG_TYPE << 24);
		AppearanceTypePtr buildingAppearanceType = nullptr;
		if (!appearName)
		{
			//------------------------------------------------------
			// LOAD a dummy appearance until real ones are available
			// for this building!
			appearanceType = (BLDG_TYPE << 24);
			buildingAppearanceType = appearanceTypeList->getAppearance(appearanceType, "TESTBLDG");
		}
		else
		{
			buildingAppearanceType = appearanceTypeList->getAppearance(appearanceType, appearName);
			if (!buildingAppearanceType)
			{
				wchar_t msg[1024];
				sprintf(msg, "No Building Appearance Named %s", appearName);
				Fatal(0, msg);
			}
		}
		appearance = new BldgAppearance;
		gosASSERT(appearance != nullptr);
		//--------------------------------------------------------------
		// The only appearance type for buildings is MLR_APPEARANCE.
		gosASSERT(buildingAppearanceType->getAppearanceClass() == BLDG_TYPE);
		appearance->init((BldgAppearanceType*)buildingAppearanceType, (GameObjectPtr)this);
		//--------------------------------------------------------------
		appearanceType = (GV_TYPE << 24);
		AppearanceTypePtr bomberAppearanceType =
			appearanceTypeList->getAppearance(appearanceType, "shilone");
		if (!bomberAppearanceType)
		{
			STOP(("Unable to create Bomber for air strike Shilone"));
		}
		if ((appearanceType >> 24) == GV_TYPE)
		{
			bomber = new GVAppearance;
			if (!bomber)
				STOP((" Artillery.init: unable to create appearance for "
					  "Shilone"));
			//-----------------------------------------------------------------
			bomber->init((GVAppearanceType*)bomberAppearanceType, (GameObjectPtr)this);
		}
		bombRunStarted = false;
	}
	break;
	case SENSOR_ARTLRY:
	{
		effectId = SENSOR_AIRSTRIKE_ID;
		init(create, ARTILLERY_SENSOR);
		//--------------------------------------------
		// Load up the strike appearance before hit.
		wchar_t appearName[1024];
		strcpy(appearName, SENSOR_NAME);
		//--------------------------------------------------------------
		// New code!!!
		// We need to append the sprite type to the appearance num now.
		// The MechEdit tool does not assume a sprite type, nor should it.
		// MechCmdr2 features much simpler objects which only use 1 type of
		// sprite!
		int32_t appearanceType = (BLDG_TYPE << 24);
		AppearanceTypePtr buildingAppearanceType = nullptr;
		if (!appearName)
		{
			//------------------------------------------------------
			// LOAD a dummy appearance until real ones are available
			// for this building!
			appearanceType = (BLDG_TYPE << 24);
			buildingAppearanceType = appearanceTypeList->getAppearance(appearanceType, "TESTBLDG");
		}
		else
		{
			buildingAppearanceType = appearanceTypeList->getAppearance(appearanceType, appearName);
			if (!buildingAppearanceType)
			{
				wchar_t msg[1024];
				sprintf(msg, "No Building Appearance Named %s", appearName);
				Fatal(0, msg);
			}
		}
		appearance = new BldgAppearance;
		gosASSERT(appearance != nullptr);
		//--------------------------------------------------------------
		// The only appearance type for buildings is MLR_APPEARANCE.
		gosASSERT(buildingAppearanceType->getAppearanceClass() == BLDG_TYPE);
		appearance->init((BldgAppearanceType*)buildingAppearanceType, (GameObjectPtr)this);
	}
	break;
	}
}

//---------------------------------------------------------------------------
void Artillery::Save(PacketFilePtr file, int32_t packetNum)
{
	ArtilleryData data;
	CopyTo(&data);
	// PacketNum incremented in ObjectManager!!
	file->writePacket(packetNum, (uint8_t*)&data, sizeof(ArtilleryData), STORAGE_TYPE_ZLIB);
}

//---------------------------------------------------------------------------
void Artillery::CopyTo(ArtilleryData* data)
{
	data->artilleryType = artilleryType;
	data->teamId = teamId;
	data->commanderId = commanderId;
	data->info = info;
	data->effectId = effectId;
	data->bombRunStarted = bombRunStarted;
	data->inView = inView;
	data->iFacePosition = iFacePosition;
	GameObject::CopyTo(dynamic_cast<GameObjectData*>(data));
}

//---------------------------------------------------------------------------
void Artillery::Load(ArtilleryData* data)
{
	GameObject::Load(dynamic_cast<GameObjectData*>(data));
	artilleryType = data->artilleryType;
	commanderId = data->commanderId;
	teamId = data->teamId;
	info = data->info;
	effectId = data->effectId;
	bombRunStarted = data->bombRunStarted;
	inView = data->inView;
	iFacePosition = data->iFacePosition;
	// if we're not a true artillery round, we're a sensor probe.
	// This checks to see if we've hit yet.
	// If not, create the impact effect and move on.
	if ((data->info.strike.sensorRange != 0.0f) && (data->info.strike.timeToImpact <= 0.0) && !getFlag(OBJECT_FLAG_SENSORS_GOING))
	{
		int32_t cellR, cellC;
		land->worldToCell(position, cellR, cellC);
		if (GameMap->getDeepWater(cellR, cellC) || GameMap->getShallowWater(cellR, cellC))
			effectId--;
		// Create the GOSFX here.
		if (strcmp(weaponEffects->GetEffectName(effectId), "NONE") != 0)
		{
			//--------------------------------------------
			// Yes, load it on up.
			uint32_t flags = gosFX::Effect::ExecuteFlag;
			Check_Object(gosFX::EffectLibrary::Instance);
			gosFX::Effect::Specification* gosEffectSpec =
				gosFX::EffectLibrary::Instance->Find(weaponEffects->GetEffectName(effectId));
			if (gosEffectSpec)
			{
				hitEffect =
					gosFX::EffectLibrary::Instance->MakeEffect(gosEffectSpec->m_effectID, flags);
				gosASSERT(hitEffect != nullptr);
				MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
			}
		}
		if (hitEffect)
		{
			Stuff::LinearMatrix4D shapeOrigin;
			Stuff::Point3D actualPosition;
			actualPosition.x = -position.x;
			actualPosition.y = position.z;
			actualPosition.z = position.y;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(actualPosition);
			gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, nullptr);
			hitEffect->Start(&info);
		}
	}
	if (!getFlag(OBJECT_FLAG_BOOM) && (data->info.strike.timeToImpact > 0.0) && (data->info.strike.sensorRange == 0.0f))
	{
		// Create the GOSFX here.
		if (strcmp(weaponEffects->GetEffectName(effectId), "NONE") != 0)
		{
			//--------------------------------------------
			// Yes, load it on up.
			uint32_t flags = gosFX::Effect::ExecuteFlag;
			Check_Object(gosFX::EffectLibrary::Instance);
			gosFX::Effect::Specification* gosEffectSpec =
				gosFX::EffectLibrary::Instance->Find(weaponEffects->GetEffectName(effectId));
			if (gosEffectSpec)
			{
				hitEffect =
					gosFX::EffectLibrary::Instance->MakeEffect(gosEffectSpec->m_effectID, flags);
				gosASSERT(hitEffect != nullptr);
				MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
			}
		}
	}
	if (data->info.strike.sensorSystemIndex != -1)
	{
		SensorSystemPtr sensor = SensorManager->newSensor();
		info.strike.sensorSystemIndex = sensor->id;
		setSensorData(getTeam());
	}
}

//---------------------------------------------------------------------------
