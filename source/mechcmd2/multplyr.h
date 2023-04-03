//***************************************************************************
//
//	Multplyr.h -- Defs for Multiplayer routines
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MULTPLYR_H
#define MULTPLYR_H

//***************************************************************************

//#include "mclib.h"
//#include "dmultplyr.h"
//#include "dmover.h"
//#include "dwarrior.h"
//#include "dturret.h"
//#include "dtacordr.h"
//#include "dgameobj.h"
//#include "dbldng.h"
//#include <dunit.h>
//#include "unitdesg.h"
//#include "dgroup.h"
//#include "ficommonnetwork.h"
//#include "sessionmanager.h"
//#include <windows.h>
//#include <stdio.h>

//***************************************************************************
typedef PVOIDNETPLAYER;

enum NETMESSAGETYPE
{
	NMT_MESSAGE, // a network message
	NMT_ADDPLAYER, // a new player
	NMT_DELPLAYER, // a player has left
	NMT_TERMSESSION, // the session has been terminated
	NMT_HOSTMIGRATE, // the host has migrated
};

//========================================================================
// NETMESSAGE
//========================================================================
class NETMESSAGE
{
public:
	NETMESSAGETYPE m_Type; // 0==network message
	NETPLAYER m_pPlayer; // player message is to, or who it's from
	uint32_t m_dwFlags; // priority, guaranteed, etc.
	uint32_t m_dwInfo; // additional message info
	uint32_t m_dwTimeStamp;
	PVOID m_pData;
	int32_t m_size;
};

#define SERVER_HANDLES_ALL_WEAPONHITS TRUE

#define MAX_MC_PLAYERS 8
#define MAX_TEAMS 8
#define MAX_LOCAL_MOVERS MAX_MOVERGROUP_COUNT // 16
#define MAX_MULTIPLAYER_MOVERS 96
#define MAX_MULTIPLAYER_MECHS_IN_LOGISTICS 36

#define MAX_MULTIPLAYER_TURRETS 128
#define MAX_WEAPONHIT_CHUNKS 1024
#define MAX_WORLD_CHUNKS 1024
#define MAX_STORED_CHATS 16
#define MAX_CHAT_LENGTH 128

#define MCGROUP_SERVER 0xFFFFFFFF
#define MCGROUP_ALLPLAYERS 0xFFFFFFFE
#define MCGROUP_INNERSPHERE 0xFFFFFFFD
#define MCGROUP_CLAN 0xFFFFFFFC

#define MPLAYER_NO_ERR 0
#define MPLAYER_ERR_HOST_NOT_FOUND -1
#define MPLAYER_ERR_NO_CONNECTION -2
#define MPLAYER_ERR_SESSION_IN_PROGRESS -3
#define MPLAYER_ERR_SESSION_LOCKED -4
#define MPLAYER_ERR_SESSION_NOT_FOUND -5
#define MPLAYER_ERR_NOT_SCANNING -6
#define MPLAYER_ERR_BAD_VERSION -7
#define MPLAYER_ERR_SESSION_FULL -8

enum class
{
	BASECOLOR_PREFERENCE,
	BASECOLOR_SELF,
	BASECOLOR_TEAM
} BasecolourType;

enum class
{
	MULTIPLAYER_MODE_NONE,
	MULTIPLAYER_MODE_BROWSER,
	MULTIPLAYER_MODE_PARAMETERS,
	MULTIPLAYER_MODE_LOGISTICS,
	MULTIPLAYER_MODE_LOADING,
	MULTIPLAYER_MODE_MISSION,
	MULTIPLAYER_MODE_RESULTS
} MultiplayerMode;

enum class
{
	MISSION_TYPE_ELIMINATION,
	MISSION_TYPE_KING_OF_THE_HILL,
	MISSION_TYPE_CAPTURE_BASE,
	MISSION_TYPE_TERRITORIES,
	MISSION_TYPE_LAST_MAN_STANDING,
	MISSION_TYPE_LAST_MAN_ON_THE_HILL,
	MISSION_TYPE_OTHER,
	NUM_MISSION_TYPES
} MissionType;

enum class
{
	GO_SESSION_SCREEN = 1
};

enum class
{
	MCMSG_GENERIC,
	MCMSG_CHAT,
	MCMSG_NEW_SERVER,
	MCMSG_PLAYER_CID,
	MCMSG_PLAYER_UPDATE,
	MCMSG_MISSION_SETTINGS_UPDATE,
	MCMSG_PLAYER_INFO,
	MCMSG_PLAYER_CHECK_IN,
	MCMSG_PLAYER_SETUP,
	MCMSG_PLAYER_INSIGNIA,
	MCMSG_MISSION_SETUP,
	MCMSG_PLAYER_CHECK_IN_RECEIPT,
	MCMSG_MOVERS_SETUP, // not used?
	MCMSG_START_PLANNING, // not used?
	MCMSG_START_MISSION,
	MCMSG_END_MISSION,
	MCMSG_LEAVE_SESSION,
	MCMSG_PLAYER_ORDER,
	MCMSG_HOLD_POSITION,
	MCMSG_PLAYER_MOVERGROUP,
	MCMSG_PLAYER_ARTILLERY,
	MCMSG_MOVER_UPDATE,
	MCMSG_TURRET_UPDATE,
	MCMSG_MOVER_WEAPONFIRE_UPDATE,
	MCMSG_TURRET_WEAPONFIRE_UPDATE,
	MCMSG_MOVER_CRITICAL_UPDATE,
	MCMSG_WEAPONHIT_UPDATE,
	MCMSG_WORLD_UPDATE,
	MCMSG_DEPLOY_FORCE,
	MCMSG_REMOVE_FORCE,
	MCMSG_PREPARE_MISSION,
	MCMSG_READY_FOR_BATTLE,
	MCMSG_FILE_INQUIRY,
	MCMSG_FILE_REPORT,
	MCMSG_LOAD_MISSION,
	MCMSG_START,
	MCMSG_JOIN_TEAM,
	MCMSG_SWITCH_SCREEN,
	MCMSG_RP_UPDATE,
	MCMSG_TECHBASE_CHANGE,
	MCMSG_SESSIONCHECKIN,
	MCMSG_REINFORCEMENT
};

#define MAXLEN_SESSION_NAME 25
#define MAXLEN_MAP_NAME 25
#define MAXLEN_MAP_DESC 32
#define MAX_SESSIONS 512
#define MAXLEN_PLAYER_NAME 25
#define MAXLEN_UNIT_NAME 25
#define MAXLEN_INSIGNIA_FILE 35
#define MAX_COLORS 32

typedef struct _MC2Session
{
	wchar_t name[MAXLEN_SESSION_NAME];
	wchar_t maxPlayers;
	wchar_t numPlayers;
	wchar_t map[MAXLEN_MAP_NAME];
	int32_t ping;
	bool locked;
	bool inProgress;
	bool cancelled;
	wchar_t versionStamp[15];
	GUID handle;
} MC2Session;

#pragma pack(1)
typedef struct _MC2SessionData
{
	wchar_t map[MAXLEN_MAP_NAME];
	bool locked;
	bool inProgress;
	wchar_t maxPlayers;
	wchar_t numPlayers;
	wchar_t versionStamp[15];
	bool cancelled;
} MC2SessionData;
#pragma pack()

typedef struct _TeamInfo
{
	uint32_t id;
	uint32_t team;
} TeamInfo;

typedef struct _MC2PlayerSlot
{
	NETPLAYER player;
	wchar_t commanderid;
	wchar_t name[MAXLEN_PLAYER_NAME];
} MC2PlayerSlot;

typedef struct _MC2Player
{
	bool checkedIn;
	NETPLAYER player;
	wchar_t commanderid;
	wchar_t dropZone;
	wchar_t name[MAXLEN_PLAYER_NAME];
	wchar_t ipAddress[16];
	wchar_t unitName[MAXLEN_UNIT_NAME];
	wchar_t insigniaFile[MAXLEN_INSIGNIA_FILE];
	wchar_t basecolour[3];
	wchar_t stripecolour;
	wchar_t teamSelected;
	wchar_t team;
	uint32_t teamSeniority;
	wchar_t faction;
	int32_t cBills;
	int32_t resourcePointsAtStart;
	int32_t resourcePointsGained;
	int32_t resourcePoints;
	int32_t rank;
	bool ready;
	int32_t score;
	int32_t kills;
	int32_t losses;
	bool winner;
	bool leftSession;
	bool booted;
} MC2Player;

typedef struct _MissionSettings
{
	wchar_t map[MAXLEN_MAP_NAME]; // file name
	wchar_t name[MAXLEN_MAP_DESC]; // displayNmae
	GUID mapGuid; // Insures maps are the same version!!
	wchar_t url[256]; /// GLENN, you probably want this somewhere else....
	int32_t defaultCBills;
	int32_t resourcePoints;
	float timeLimit;
	bool unlimitedAmmo;
	bool variants;
	bool allTech;
	bool airStrike;
	bool guardTower;
	bool repairVehicle;
	bool recoveryTeam;
	bool sensorProbe;
	bool scoutCopter;
	bool mineLayer;
	bool resourceBuilding;
	bool resourceForMechs;
	wchar_t missionType;
	bool quickStart;
	wchar_t maxTeams;
	wchar_t maxPlayers;
	wchar_t numPlayers;
	int32_t dropWeight;
	bool locked;
	bool inProgress;
} MissionSettings;

typedef struct _CompressedMech
{
	bool lastMech;
	int32_t objNumber;
	wchar_t commanderid;
	uint32_t basecolour;
	uint32_t highlightcolour1;
	uint32_t highlightcolour2;
	wchar_t pilotFile[50];
	wchar_t mechFile[50];
	wchar_t variantName[64];
	wchar_t variantNum;
	float pos[2];
	int32_t cBills;
	bool designerMech;
	int32_t numComponents;
	uint8_t components[50];
} CompressedMech;

#pragma pack(1)

//#pragma warning (disable : 4200)

//------------------------------------------------------------------------------------------

class WorldChunk
{

public:
	wchar_t type;
	int16_t tileRC[2];
	int32_t objectId;
	int32_t objectBlockOrTrainNumber;
	int32_t objectVertexOrCarNumber;
	wchar_t objectItemNumber;
	int32_t param1;
	int32_t param2;

	uint32_t data;

public:
	PVOID operator new(size_t mySize);

	void operator delete(PVOID us);

	void init(void)
	{
		type = -1;
		tileRC[0] = -1;
		tileRC[1] = -1;
		objectId = 0;
		objectBlockOrTrainNumber = 0;
		objectVertexOrCarNumber = 0;
		objectItemNumber = 0;
		param1 = 0;
		param2 = 0;
		data = 0;
	}

	void destroy(void) { }

	WorldChunk(void)
	{
		init(void);
	}

	~WorldChunk(void)
	{
		destroy(void);
	}

	void buildMissionScriptMessage(int32_t messageCode, int32_t messageParam);

	void buildArtillery(
		int32_t artilleryType, int32_t teamId, Stuff::Vector3D location, int32_t seconds);

	void buildMine(
		int32_t tileR, int32_t tileC, int32_t teamId, int32_t mineState, int32_t explosionType);

	void buildTerrainFire(GameObjectPtr object, int32_t seconds);

	void buildPilotKillStat(int32_t moverIndex, int32_t vehicleClass);

	void buildScore(int32_t commanderid, int32_t score);

	void buildKillLoss(int32_t killerCID, int32_t loserCID);

	void buildEndMission(void);

	void buildCaptureBuilding(BuildingPtr building, int32_t newCommanderID);

	void pack(void);

	void unpack(void);

	bool equalTo(WorldChunkPtr chunk);
};

// The following messages are for multiplayer games and are sent when
// the user deploys a unit or drags it back to MechBay.
//------------------------------------------------------------------------------------------

/*
class MCMSG_DeployForce: public FIGuaranteedMessageHeader
{
	private:

	// Keep the constructor private because we don't want
	// anyone to call it when there is an undefined size
	// for the class.
	MCMSG_DeployForce(void);

	uint8_t dataFlags;

	enum
	{
		PartTypeBit	= 0,
		TeamBit,
		VariantBits1,
		VariantBits2,
		LanceBits1,
		LanceBits2,
		SlotBits1,
		SlotBits2
	};

	enum
	{
		PartTypeFlag = 1 << PartTypeBit,
		TeamFlag = 1 << TeamBit,
		VariantMask = 1 << VariantBits1 | 1 << VariantBits2,
		LanceMask = 1 << LanceBits1 | 1 << LanceBits2,
		SlotMask = 1 << SlotBits1 | 1 << SlotBits2
	};

	public:
		// data
		uint8_t partNameIndex;
		uint8_t pilotNameIndex[2];
		uint8_t inventoryCount; // Number of components
		uint16_t  componentIDs[];

		// member functions
		void Init()
		{
			FIGuaranteedMessageHeader::Init(void);
			SetType(MCMSG_DEPLOY_FORCE);
			dataFlags = 0;
			partNameIndex = 0;
			pilotNameIndex[0] = 255;
			pilotNameIndex[1] = 255;
			inventoryCount = 0;
		}

		// Teams:  InnerSphere is 0, Clan is 1
		void SetInnerSphere()
		{
			dataFlags &= ~TeamFlag;
		}

		void SetClan()
		{
			dataFlags |= TeamFlag;
		}

		void SetVehicle()
		{
			dataFlags &= ~PartTypeFlag;
		}

		void SetMech()
		{
			dataFlags |= PartTypeFlag;
		}


		void SetVariant(int32_t variant)
		{
			if (variant > 3)
				return;
			dataFlags &= ~VariantMask; // erase old variant
			dataFlags |= (variant << VariantBits1); // set new
		}

		void SetLance(int32_t lance)
		{
			if (lance > (LanceMask >> LanceBits1))
				return;

			dataFlags &= ~LanceMask;
			dataFlags |= (lance << LanceBits1);
		}

		void SetSlot(int32_t slot)
		{
			if (slot > (SlotMask >> SlotBits1))
				return;

			dataFlags &= ~SlotMask;
			dataFlags |= (slot << SlotBits1);
		}


		int32_t GetVariant()
		{
			return (dataFlags & VariantMask) >> VariantBits1;
		}

		int32_t GetLance()
		{
			return (dataFlags & LanceMask) >> LanceBits1;
		}

		int32_t GetSlot()
		{
			return (dataFlags & SlotMask) >> SlotBits1;
		}

		BOOL IsClan()
		{
			return (dataFlags & TeamFlag);
		}

		BOOL IsMech()
		{
			return (dataFlags & PartTypeFlag);
		}


};
*/

//---------------------------------------------------------------------------

/*
class MCMSG_JoinTeam:public FIGuaranteedMessageHeader
{
	public:
		void Init()
		{
			FIGuaranteedMessageHeader::Init(void);
			SetType(MCMSG_JOIN_TEAM);
			player = -1;
			team = -99;
			slot = -99;
		}

		DPID player;
		wchar_t team;
		wchar_t slot;
};
*/

//---------------------------------------------------------------------------

/*
class MCMSG_RemoveForce:public FIGuaranteedMessageHeader
{
	public:
		void Init()
		{
			FIGuaranteedMessageHeader::Init(void);
			SetType(MCMSG_REMOVE_FORCE);
			slotNumber = 0;
			lanceNumber = 0;
		}

		uint8_t slotNumber;
		uint8_t lanceNumber;
};
*/

//---------------------------------------------------------------------------

class MCMSG_Generic
{

public:
	uint8_t type;

public:
	MCMSG_Generic(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_GENERIC;
	}
};

//---------------------------------------------------------------------------

class MCMSG_PlayerCID
{

public:
	uint8_t type;
	wchar_t commanderid;
	uint8_t subType;

public:
	MCMSG_PlayerCID(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_PLAYER_CID;
		commanderid = -1;
		subType = 0;
	}
};

//---------------------------------------------------------------------------

class MCMSG_PlayerUpdate
{

public:
	uint8_t type;
	wchar_t stage;
	float senderTime;
	wchar_t sessionIPAddress[16];
	wchar_t versionStamp[15];
	MC2Player info;

public:
	MCMSG_PlayerUpdate(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_PLAYER_UPDATE;
		senderTime = -1.0;
		stage = 0;
	}
};

//---------------------------------------------------------------------------

class MCMSG_MissionSettingsUpdate
{

public:
	uint8_t type;
	MissionSettings missionSettings;

public:
	MCMSG_MissionSettingsUpdate(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_MISSION_SETTINGS_UPDATE;
	}
};

//---------------------------------------------------------------------------

class MCMSG_MissionSetup
{

public:
	uint8_t type;
	int32_t randomSeed;
	wchar_t subType;
	wchar_t commandersToLoad[MAX_MC_PLAYERS][3];
	CompressedMech mechData;

public:
	MCMSG_MissionSetup(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_MISSION_SETUP;
		subType = 0;
	}
};

//---------------------------------------------------------------------------

class MCMSG_PlayerInfo
{

public:
	uint8_t type;
	wchar_t commanderid;

public:
	MCMSG_PlayerInfo(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_PLAYER_INFO;
		commanderid = -1;
	}
};

//---------------------------------------------------------------------------

class MCMSG_Chat
{

	// GUARANTEED

public:
	uint8_t type;
	bool allPlayers;
	bool isDeadChat;
	bool hideName;
	wchar_t string[];

public:
	MCMSG_Chat(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_CHAT;
		allPlayers = false;
		isDeadChat = false;
		hideName = false;
		string[0] = nullptr;
	}
};

//---------------------------------------------------------------------------

class MCMSG_PlayerCheckIn
{

	// GUARANTEED

public:
	uint8_t type;
	wchar_t commanderid;

public:
	MCMSG_PlayerCheckIn(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_PLAYER_CHECK_IN;
		commanderid = -1;
	}
};

//---------------------------------------------------------------------------

class MCMSG_PlayerSetup
{

	// GUARANTEED

public:
	uint8_t type;
	uint8_t moverData[MAX_MULTIPLAYER_MOVERS];

public:
	MCMSG_PlayerSetup(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_PLAYER_SETUP;
		for (size_t i = 0; i < MAX_MULTIPLAYER_MOVERS; i++)
			moverData[i] = 0;
	}
};

//---------------------------------------------------------------------------

class MCMSG_PlayerInsignia
{

public:
	uint8_t type;
	wchar_t commanderid;
	wchar_t fileName[64];
	int32_t dataSize;
	uint8_t data[];

public:
	MCMSG_PlayerInsignia(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_PLAYER_INSIGNIA;
	}
};

//---------------------------------------------------------------------------

class MCMSG_StartMission
{

	// GUARANTEED

public:
	uint8_t type;
	int32_t huh;

public:
	MCMSG_StartMission(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_START_MISSION;
		huh = 0;
	}
};

//---------------------------------------------------------------------------

class MCMSG_EndMission
{

	// GUARANTEED

public:
	uint8_t type;
	int32_t teamScore[MAX_MC_PLAYERS];
	int32_t playerScore[MAX_MC_PLAYERS];
	int32_t result;

public:
	MCMSG_EndMission(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_END_MISSION;
		result = -1;
	}
};

//---------------------------------------------------------------------------

class MCMSG_LeaveSession
{

public:
	uint8_t type;
	uint8_t subType;
	wchar_t commanderid;

public:
	MCMSG_LeaveSession(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_LEAVE_SESSION;
		subType = 0;
		commanderid = -1;
	}
};

//---------------------------------------------------------------------------

class MCMSG_PlayerOrder
{

	// GUARANTEED

public:
	uint8_t type;
	wchar_t commanderid;
	uint8_t flags;
	float location[2];
	uint32_t tacOrderChunk[2];

public:
	MCMSG_PlayerOrder(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_PLAYER_ORDER;
		commanderid = -1;
		flags = 0;
		location[0] = 0.0;
		location[1] = 0.0;
		tacOrderChunk[0] = 0;
		tacOrderChunk[1] = 0;
	}
};

//---------------------------------------------------------------------------

class MCMSG_HoldPosition
{

public:
	uint8_t type;
	wchar_t commanderid;
	uint16_t flags;

public:
	MCMSG_HoldPosition(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_HOLD_POSITION;
		commanderid = -1;
		flags = 0;
	}
};

//---------------------------------------------------------------------------

#define MOVERGROUP_ACTION_DEFAULT 0
#define MOVERGROUP_ACTION_HOLDPOSITION_OFF 1
#define MOVERGROUP_ACTION_HOLDPOSITION_ON 2

class MCMSG_PlayerMoverGroup
{

	// GUARANTEED

public:
	uint8_t type;
	uint8_t action;
	wchar_t commanderid;
	wchar_t groupId;
	uint16_t moverGroupInfo;

public:
	MCMSG_PlayerMoverGroup(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_PLAYER_MOVERGROUP;
		commanderid = -1;
		groupId = -1;
		moverGroupInfo = 0;
	}
};

//---------------------------------------------------------------------------

class MCMSG_PlayerArtillery
{

	// GUARANTEED

public:
	uint8_t type;
	float location[2];
	uint32_t chunk;

	MCMSG_PlayerArtillery(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_PLAYER_ARTILLERY;
		location[0] = 0.0;
		location[1] = 0.0;
		chunk = 0;
	}
};

//---------------------------------------------------------------------------

class MCMSG_MoverUpdate
{

public:
	uint8_t type;
	int32_t teamScore[MAX_MC_PLAYERS];
	int32_t playerScore[MAX_MC_PLAYERS];
	// int32_t				resourcePoints[MAX_MC_PLAYERS];
	uint16_t updateId;
	uint8_t numRLEs;
	uint8_t moveData[];

public:
	MCMSG_MoverUpdate(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_MOVER_UPDATE;
		updateId = 0xFFFF;
		numRLEs = 0;
	}
};

//---------------------------------------------------------------------------

class MCMSG_TurretUpdate
{

public:
	uint8_t type;
	uint16_t updateId;
	wchar_t targetList[];

public:
	MCMSG_TurretUpdate(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_TURRET_UPDATE;
		updateId = 0xFFFF;
		for (size_t i = 0; i < MAX_MULTIPLAYER_TURRETS; i++)
			targetList[i] = 0;
	}
};

//---------------------------------------------------------------------------

class MCMSG_MoverWeaponFireUpdate
{

	// GUARANTEED

public:
	uint8_t type;
	uint8_t numRLEs;
	uint8_t weaponFireData[];

public:
	MCMSG_MoverWeaponFireUpdate(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_MOVER_WEAPONFIRE_UPDATE;
		numRLEs = 0;
	}
};

//---------------------------------------------------------------------------

class MCMSG_TurretWeaponFireUpdate
{

	// GUARANTEED

public:
	uint8_t type;
	wchar_t numTurrets;
	uint16_t info[];

public:
	MCMSG_TurretWeaponFireUpdate(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_TURRET_WEAPONFIRE_UPDATE;
		numTurrets = 0;
	}
};

//---------------------------------------------------------------------------

class MCMSG_MoverCriticalUpdate
{

	// GUARANTEED

public:
	uint8_t type;
	uint8_t numCritHitChunks[MAX_MULTIPLAYER_MOVERS];
	uint8_t numRadioChunks[MAX_MULTIPLAYER_MOVERS];
	uint8_t chunk[];

public:
	MCMSG_MoverCriticalUpdate(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_MOVER_CRITICAL_UPDATE;
		for (size_t i = 0; i < MAX_MULTIPLAYER_MOVERS; i++)
		{
			numCritHitChunks[i] = 0;
			numRadioChunks[i] = 0;
		}
	}
};

//---------------------------------------------------------------------------

class MCMSG_WeaponHitUpdate
{

	// GUARANTEED

public:
	uint8_t type;
	uint8_t numWeaponHits;
	uint32_t weaponHitChunk[];

public:
	MCMSG_WeaponHitUpdate(void)
	{
		init(void);
	}

	void init()
	{
		type = MCMSG_WEAPONHIT_UPDATE;
		numWeaponHits = 0;
	}
};

//---------------------------------------------------------------------------

class MCMSG_WorldUpdate
{

	// GUARANTEED

public:
	uint8_t type;
	uint8_t numWorldChanges;
	uint8_t numArtilleryStrikes;
	uint32_t worldChunk[];

public:
	MCMSG_WorldUpdate(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_WORLD_UPDATE;
		numWorldChanges = 0;
		numArtilleryStrikes = 0;
	}
};

//---------------------------------------------------------------------------

class MCMSG_Reinforcement
{

public:
	uint8_t type;
	uint8_t stage;
	uint8_t rosterIndex;
	int32_t vehicleID;
	wchar_t pilotName[24];
	wchar_t commanderid;
	float location[2];

public:
	MCMSG_Reinforcement(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_REINFORCEMENT;
		stage = 0;
		rosterIndex = 255;
		vehicleID = 0;
		pilotName[0] = nullptr;
		commanderid = 0;
		location[0] = 0.0;
		location[1] = 0.0;
	}
};

//---------------------------------------------------------------------------

class MCMSG_NewServer
{

public:
	uint8_t type;
	wchar_t ipAddress[16];

public:
	MCMSG_NewServer(void)
	{
		init(void);
	}

	void init(void)
	{
		type = MCMSG_NEW_SERVER;
		ipAddress[0] = nullptr;
	}
};

//---------------------------------------------------------------------------

class MCMSG_FileReport
{

	// GUARANTEED

public:
	uint8_t type;
	uint32_t checkSum;
	GUID fileGuid;
	wchar_t fileName[];

	MCMSG_FileReport(void)
	{
		init(void);
	}

	void init()
	{
		type = MCMSG_FILE_REPORT;
		checkSum = 0;
		fileName[0] = nullptr;
		memset(&fileGuid, 0, sizeof(GUID));
	}
};

//---------------------------------------------------------------------------

/*
class MCMSG_PassValue {

	public:

		int32_t			value[2];

	public:

		MCMSG_PassValue (void) {
			init(void);
		}

		void init (void) {
			value[0] = -1;
			value[1] = -1;
		}
};
*/

//#pragma warning (default : 4200)

#pragma pack()

//***************************************************************************

enum class
{
	VERSION_STATUS_UNKNOWN,
	VERSION_STATUS_GOOD,
	VERSION_STATUS_BAD,
	NUM_VERSION_STATUS_TYPES
} VersionStatus;

class MultiPlayer
{

public:
	bool onLAN;
	bool hostLeft;
	VersionStatus versionStatus;
	int32_t numSessions;
	bool availableCIDs[MAX_MC_PLAYERS];
	MC2Session sessionList[MAX_SESSIONS];
	uint32_t teamSeniority[MAX_TEAMS];
	static int32_t colors[MAX_COLORS];
	wchar_t colorsCID[MAX_COLORS];
	wchar_t sessionIPAddress[16];
	bool sessionScanning;
	bool sessionScanningError;
	bool sessionScanningPersistent;
	bool fitStart;
	int32_t numFitPlayers;
	MissionSettings missionSettings;
	wchar_t commandersToLoad[MAX_MC_PLAYERS][3];
	CompressedMech mechData[MAX_MC_PLAYERS][12];
	bool locked;
	bool inProgress;
	bool cancelled;
	int32_t randomSeed;
	bool startLogistics;
	bool startLoading;
	bool readyToLoad[MAX_MC_PLAYERS];
	bool mechDataReceived[MAX_MC_PLAYERS];
	bool missionDataLoaded[MAX_MC_PLAYERS];
	bool missionFullySetup[MAX_MC_PLAYERS];
	bool leaveSessionConfirmed[MAX_MC_PLAYERS];

	int32_t numAirStrikesUsed[MAX_MC_PLAYERS];
	int32_t numMinelayersUsed[MAX_MC_PLAYERS];
	int32_t numScoutCoptersUsed[MAX_MC_PLAYERS];
	int32_t numArtilleryPiecesUsed[MAX_MC_PLAYERS];
	int32_t numSensorProbesUsed[MAX_MC_PLAYERS];
	int32_t numRepairVehiclesUsed[MAX_MC_PLAYERS];
	int32_t numSalvageCraftUsed[MAX_MC_PLAYERS];

	int32_t numTeams;
	int32_t teamScore[MAX_TEAMS];
	int32_t winningTeam;
	BuildingPtr mainHqBuildings[MAX_TEAMS];
	int32_t numHqBuildings;
	int32_t* hqCIDs;
	BuildingPtr* hqBuildings;
	int32_t numResourceBuildings;
	BuildingPtr* resourceBuildings;
	uint32_t numRandomResourceBuildings;

	int32_t mode;
	int32_t sessionEntry;
	uint8_t* msgBuffer;
	int32_t numLocalMovers;
	int32_t numMovers;
	int32_t numTurrets;
	uint8_t moverRosterRLE[MAX_MULTIPLAYER_MOVERS];
	int32_t numMoverRosterRLE;
	std::unique_ptr<Mover> localMovers[MAX_LOCAL_MOVERS];
	std::unique_ptr<Mover> moverRoster[MAX_MULTIPLAYER_MOVERS];
	std::unique_ptr<Mover> playerMoverRoster[MAX_MC_PLAYERS][MAX_LOCAL_MOVERS];
	TurretPtr turretRoster[MAX_MULTIPLAYER_TURRETS];

	bool iAmHost;
	// int32_t				serverCID;
	NETPLAYER myPlayer;
	NETPLAYER serverPlayer;

	bool inSession;
	bool hostDroppedOut;
	int32_t commanderid; // same as commanderId
	wchar_t sessionName[80];
	wchar_t playerName[80];

	// MC2Player			playerList[MAX_MC_PLAYERS];		// list of
	// players--order doesn't matter
	MC2PlayerSlot playerList[MAX_MC_PLAYERS];
	bool insigniaList[MAX_MC_PLAYERS]; // list of insignia data
	int32_t insigniaSizeList[MAX_MC_PLAYERS];
	// int32_t				maxPlayers;						// max number of players
	// allowed in session
	MC2Player playerInfo[MAX_MC_PLAYERS]; // list of players--indexed by commanderid

	bool playerReady[MAX_MC_PLAYERS];
	bool inSessionScreen[MAX_MC_PLAYERS];
	bool startPlanning;
	bool setupMission;
	bool startMission;
	bool endMission;
	bool waitingToStartMission;
	bool preparingMission;
	float startTime;
	bool allUnitsDestroyed[MAX_MC_PLAYERS];
	int32_t missionStatus;

	// TeamInfo			teamInfo[MAX_MC_PLAYERS];

	// ChatMessageCallback is called when a chat message arrives.
	// LPMESSAGECALLBACK ChatMessageCallback;

	// Server Data
	float pingUpdateTime;
	float pingUpdateFrequency;
	float moverUpdateTime;
	float moverUpdateFrequency;
	uint16_t moverUpdateId;
	float turretUpdateTime;
	float turretUpdateFrequency;
	uint16_t turretUpdateId;
	float worldUpdateTime;
	float worldUpdateFrequency;
	float lastServerUpdateTime;
	int32_t numWeaponHitChunks;
	uint32_t weaponHitChunks[MAX_WEAPONHIT_CHUNKS];
	int32_t numWorldChunks;
	uint32_t worldChunks[MAX_WORLD_CHUNKS];
	uint32_t serverOrder[MAX_MC_PLAYERS];
	int32_t reinforcements[MAX_MC_PLAYERS][2]; // index 0 = current
		// reinforcement, index 1 =
		// current recoverery
	wchar_t reinforcementPilot[MAX_MC_PLAYERS][32];

	bool isMPlayerGame;
	int32_t badSessionCounter;
	float warpFactor;

	int32_t totalLoad;
	int32_t moverWeaponFireLoad;
	int32_t turretWeaponFireLoad;
	int32_t moverCriticalLoad;
	int32_t weaponHitLoad;
	int32_t worldStateLoad;
	int32_t maxTotalLoad;
	int32_t maxMoverWeaponFireLoad;
	int32_t maxTurretWeaponFireLoad;
	int32_t maxMoverCriticalLoad;
	int32_t maxWeaponHitLoad;
	int32_t maxWorldLoad;
	int32_t maxReceiveLoad;
	int32_t maxReceiveSize;
	int32_t worldChunkTally[NUM_WORLDCHUNK_TYPES];

	wchar_t currentChatMessages[MAX_STORED_CHATS][MAX_CHAT_LENGTH];
	int32_t currentChatMessagePlayerIDs[MAX_STORED_CHATS];
	int32_t chatCount;

	static bool launchedFromLobby;
	static bool registerZone;
	static int32_t presetDropZones[MAX_MC_PLAYERS];

public:
	PVOID operator new(size_t mySize);

	void operator delete(PVOID us);

	void init(void);

	int32_t setup(void);

	// initUpdateFrequencies initializes the update frequencies from the
	// prefs.cfg file if the entries exist.  Otherwise, it uses defaults.
	void initUpdateFrequencies(void);

	// initStartupParameters is called by both init() and
	// leaveSession()
	void initStartupParameters(bool fresh);

	void resetForNewGame(void);

	MultiPlayer(void)
	{
		init(void);
	}

	void destroy(void);

	~MultiPlayer(void)
	{
		destroy(void);
	}

	bool isServer(void)
	{
		return (false);
	}

	void setServer(NETPLAYER player, wchar_t playerIPAddress[16]);

	bool isHost(void)
	{
		return (isServer());
	}

	void setMode(int32_t newMode)
	{
		mode = newMode;
	}

	VersionStatus getVersionStatus(void)
	{
		return (VERSION_STATUS_GOOD);
	}

	int32_t update(void);

	int32_t beginSessionScan(std::wstring_view ipAddress, bool persistent = true);

	int32_t endSessionScan(void);

	bool getSessionScanFailed(void)
	{
		return (false);
	}

	MC2Session* getSessions(int32_t& sessionCount);

	// for read purposes
	const MC2Player* getPlayers(int32_t& playerCount);

	// for write purposes....
	MC2Player* getPlayerInfo(int32_t commanderid)
	{
		return nullptr;
	}

	bool hostSession(std::wstring_view sessionName, std::wstring_view playerName, int32_t mxPlayers);

	int32_t joinSession(MC2Session* session, std::wstring_view playerName);

	bool getOnLAN(void)
	{
		return (onLAN);
	}

	bool isServerMissing(void);

	void updateSessionData(MC2SessionData* sessionData);

	void setLocked(bool set);

	void setCancelled(bool set);

	void setInProgress(bool set);

	bool getLocked(void)
	{
		return (locked);
	}

	bool getLaunchedFromLobby(void)
	{
		return (launchedFromLobby);
	}

	bool getInProgress(void)
	{
		return (inProgress);
	}

	void addTeamScore(int32_t teamID, int32_t score);

	int32_t closeSession(void);

	void logMessage(NETMESSAGE* message, bool sent);

	int32_t bootPlayer(NETPLAYER bootedPlayer);

	void sendMessage(
		NETPLAYER player, PVOID data, size_t dataSize, bool guaranteed, bool toSelf = true);

	bool hostGame(std::wstring_view sessionName, std::wstring_view playerName, int32_t nPlayers);

	int32_t joinGame(std::wstring_view ipAddress, std::wstring_view sessionName, std::wstring_view playerName);

	bool waitTillStartLoading(void);

	bool waitTillMechDataReceived(void);

	bool waitTillMissionLoaded(void);

	bool waitTillMissionSetup(void);

	bool waitForSessionEntry(void);

	bool playersReadyToLoad(void);

	void handlePlayerLeftSession(NETPLAYER leftPlayer);

	void handleTerminateSession(void);

	bool launchBrowser(std::wstring_view link);

	void initParametersScreen(void);

	void setDefaultPlayerInfo(void);

	void initSpecialBuildings(wchar_t commandersToLoad[8][3]);

	std::wstring_view getPlayerName(void)
	{
		return (nullptr);
	}

	void addToLocalMovers(std::unique_ptr<Mover> mover);

	void removeFromLocalMovers(std::unique_ptr<Mover> mover);

	void addToMoverRoster(std::unique_ptr<Mover> mover);

	bool canAddToMoverRoster(void)
	{
		return false;
	}

	void removeFromMoverRoster(std::unique_ptr<Mover> mover);

	void addToTurretRoster(TurretPtr turret);

	void addToPlayerMoverRoster(int32_t playerCommanderID, std::unique_ptr<Mover> mover);

	void removeFromPlayerMoverRoster(std::unique_ptr<Mover> mover);

	int32_t getNumWorldChunks(void)
	{
		return (numWorldChunks);
	}

	void clearWorldChunks(void)
	{
		numWorldChunks = 0;
	}

	int32_t setClosestcolour(int32_t colorIndex, int32_t commanderid);

	int32_t setNextFreecolour(int32_t commanderid);

	void setcolour(int32_t colorIndex, int32_t commanderid);

	void setPlayerBasecolour(int32_t commanderid, int32_t colorIndex);

	void setPlayerTeam(int32_t commanderid, int32_t teamID);

	void calcPlayerRanks(void);

	int32_t addWorldChunk(WorldChunkPtr chunk);

	int32_t addMissionScriptMessageChunk(int32_t code, int32_t param1);

	//		int32_t addTerrainAlignmentChunk (GameObjectPtr object, int32_t
	// alignment);

	int32_t addArtilleryChunk(
		int32_t artilleryType, int32_t teamId, Stuff::Vector3D location, int32_t seconds);

	int32_t addMineChunk(
		int32_t tileR, int32_t tileC, int32_t teamId, int32_t mineState, int32_t explosionType);

	int32_t addLightOnFireChunk(GameObjectPtr object, int32_t seconds);

	int32_t addPilotKillStat(std::unique_ptr<Mover> mover, int32_t vehicleClass);

	int32_t addScoreChunk(int32_t commanderid, int32_t score);

	int32_t addKillLossChunk(int32_t killerCID, int32_t loserCID);

	int32_t addCaptureBuildingChunk(
		BuildingPtr building, int32_t prevCommanderID, int32_t newCommanderID);

	int32_t addEndMissionChunk(void);

	int32_t grabWorldChunks(uint32_t* packedChunkBuffer);

	int32_t updateWorldChunks(void);

	int32_t getNumWeaponHitChunks(void)
	{
		return (numWeaponHitChunks);
	}

	void clearWeaponHitChunks(void)
	{
		numWeaponHitChunks = 0;
	}

	int32_t addWeaponHitChunk(WeaponHitChunkPtr chunk);

	int32_t addWeaponHitChunk(
		GameObjectPtr target, WeaponShotInfoPtr shotInfo, bool isRefit = false);

	void grabWeaponHitChunks(uint32_t* packedChunkBuffer, int32_t numChunks);

	int32_t updateWeaponHitChunks(void);

	int32_t getHomeTeamPlayers(uint32_t* playerIdList);

	int32_t getEnemyTeamPlayers(uint32_t* playerIdList);

	bool processGameMessage(NETMESSAGE* msg);

	void processMessages(void);

	int32_t findPlayer(NETPLAYER player)
	{
		return (-1);
	}

	bool calcMissionStatus(void);

	void handleChat(NETPLAYER sender, MCMSG_Chat* msg);

	void handlePlayerCID(NETPLAYER sender, MCMSG_PlayerCID* msg);

	void handlePlayerUpdate(NETPLAYER sender, MCMSG_PlayerUpdate* msg);

	void handleMissionSettingsUpdate(NETPLAYER sender, MCMSG_MissionSettingsUpdate* msg);

	void handlePlayerInfo(NETPLAYER sender, MCMSG_PlayerInfo* msg);

	void handlePlayerCheckIn(NETPLAYER sender, MCMSG_PlayerCheckIn* msg);

	void handlePlayerSetup(NETPLAYER sender, MCMSG_PlayerSetup* msg);

	void handlePlayerInsignia(NETPLAYER sender, MCMSG_PlayerInsignia* msg);

	void handleMissionSetup(NETPLAYER sender, MCMSG_MissionSetup* msg);

	void handleStartPlanning(NETPLAYER sender);

	void handleReadyForBattle(NETPLAYER sender, MCMSG_PlayerCheckIn* msg);

	void handleStartMission(NETPLAYER sender);

	void handleEndMission(NETPLAYER sender, MCMSG_EndMission* msg);

	void handleReinforcement(NETPLAYER sender, MCMSG_Reinforcement* msg);

	void handleNewServer(NETPLAYER sender, MCMSG_NewServer* msg);

	void handleLeaveSession(NETPLAYER sender, MCMSG_LeaveSession* msg);

	void handleHoldPosition(NETPLAYER sender, MCMSG_HoldPosition* msg);

	void handlePlayerOrder(NETPLAYER sender, MCMSG_PlayerOrder* msg);

	void handlePlayerMoverGroup(NETPLAYER sender, MCMSG_PlayerMoverGroup* msg);

	void handlePlayerArtillery(NETPLAYER sender, MCMSG_PlayerArtillery* msg);

	void handleMoverUpdate(NETPLAYER sender, MCMSG_MoverUpdate* msg);

	void handleTurretUpdate(NETPLAYER sender, MCMSG_TurretUpdate* msg);

	void handleMoverWeaponFireUpdate(NETPLAYER sender, MCMSG_MoverWeaponFireUpdate* msg);

	void handleTurretWeaponFireUpdate(NETPLAYER sender, MCMSG_TurretWeaponFireUpdate* msg);

	void handleMoverCriticalUpdate(NETPLAYER sender, MCMSG_MoverCriticalUpdate* msg);

	void handleWeaponHitUpdate(NETPLAYER sender, MCMSG_WeaponHitUpdate* msg);

	void handleWorldUpdate(NETPLAYER sender, MCMSG_WorldUpdate* msg);

	void sendChat(NETPLAYER receiver, wchar_t team, std::wstring_view chatString);
	void sendPlayerActionChat(NETPLAYER receiver, std::wstring_view playerName, uint32_t resID);

	void sendPlayerCID(NETPLAYER receiver, uint8_t subType, wchar_t CID);

	void sendPlayerUpdate(NETPLAYER receiver, int32_t stage, int32_t commanderid);

	void sendMissionSettingsUpdate(NETPLAYER receiver);

	void sendPlayerInfo(NETPLAYER receiver);

	void sendPlayerCheckIn(void);

	void sendPlayerSetup(void);

	void sendPlayerInsignia(std::wstring_view insigniaFileName, uint8_t* insigniaData, int32_t insigniaDataSize);

	void sendMissionSetup(NETPLAYER receiver, int32_t stage, CompressedMech* mechData);

	void sendStartPlanning(void);

	void sendStartMission(void);

	void sendEndMission(int32_t result);

	void sendReinforcement(int32_t vehicleID, int32_t rosterIndex, wchar_t pilotName[16],
		int32_t commanderid, Stuff::Vector3D pos, uint8_t stage);

	void sendNewServer(void);

	void sendLeaveSession(wchar_t subType, wchar_t commanderid);

	void sendPlayerOrder(TacticalOrderPtr tacOrder, bool needsSelection, int32_t numMovers,
		std::unique_ptr<Mover>* moverList, int32_t numGroups = 0, MoverGroupPtr* groupList = nullptr,
		bool queuedOrder = false);

	void sendHoldPosition(void);

	void sendPlayerMoverGroup(
		int32_t groupId, int32_t numMovers, std::unique_ptr<Mover>* moverList, int32_t point);

	void sendPlayerArtillery(int32_t striketype, Stuff::Vector3D location, int32_t seconds);

	int32_t sendPing(void);

	void sendMoverUpdate(void);

	void sendTurretUpdate(void);

	void sendMoverWeaponFireUpdate(void);

	void sendTurretWeaponFireUpdate(void);

	void sendMoverCriticalUpdate(void);

	void sendWeaponHitUpdate(void);

	void sendWorldUpdate(void);

	void sendFile(std::wstring_view path, std::wstring_view fileName);

	void sendReadyForBattle(void);

	void sendPrepareMission(void);

	void sendFileInquiry(std::wstring_view filename);

	int32_t updateServer(void);

	void buildMoverRosterRLE(void);

	int32_t updateClients(bool forceIt = false);

	bool isMyTeammate(uint32_t playerId);

	bool allPlayersCheckedIn(void);

	bool allPlayersReady(void);

	void switchServers(void);

	void calcDropZones(wchar_t dropZonesCID[8], wchar_t hqs[MAX_TEAMS]);

	int32_t saveTranscript(std::wstring_view fileName, bool debugging = false);

	void playerLeftGame(uint32_t playerId);

	// call this to exit the current session.
	void leaveSession(void);

	void getChatMessages(std::wstring_view* buffer, int32_t* playerIDs, int32_t& count);

	void redistributeRP(void);
};

bool StartupNetworking(void);
void ResetNetworking(void);
void ShutdownNetworking(void);

extern MultiPlayer* MPlayer;

//***************************************************************************

#endif
