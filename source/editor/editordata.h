#ifndef EDITORDATA_H
#define EDITORDATA_H
//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "mclib.h"

#ifndef OBJECTIVE_H
#include "Objective.h"
#endif

#ifndef ELIST_H
#include "elist.h"
#endif

#ifndef ESTRING_H
#include "EString.h"
#endif

static const int GAME_MAX_PLAYERS = 8;

#define EDITOR_VISIBLE_VERTICES			60

// ARM
namespace Microsoft
{
	namespace Xna
	{
		namespace Arm
		{
			struct IProviderAsset;
		}
	}
}

extern Microsoft::Xna::Arm::IProviderAsset * mapAsset;
extern Microsoft::Xna::Arm::IProviderAsset * mechAsset;

class CTeam
{
public:
	CTeam(int alignment = 0) { Alignment(alignment); }
	bool operator==(const CTeam &rhs) const;
	void Clear() { m_objectives.Clear(); }
	int Alignment() { return m_alignment; }
	void Alignment(int alignment) { m_alignment = alignment; m_objectives.Alignment(m_alignment); }
	bool Read( FitIniFile* missionFile ) { return m_objectives.Read(missionFile); }
	bool Save( FitIniFile* missionFile ) { return m_objectives.Save(missionFile); }
	CObjectives &ObjectivesRef() { return m_objectives; }
	void handleObjectInvalidation(const EditorObject *pObj) { m_objectives.handleObjectInvalidation(pObj); }
	bool NoteThePositionsOfObjectsReferenced() { return m_objectives.NoteThePositionsOfObjectsReferenced(); }
	bool RestoreObjectPointerReferencesFromNotedPositions() { return m_objectives.RestoreObjectPointerReferencesFromNotedPositions(); }
	bool ThereAreObjectivesWithNoConditions() { return m_objectives.ThereAreObjectivesWithNoConditions(); }
private:
	int m_alignment;
	CObjectives m_objectives;
};

class CTeams
{
public:
	CTeams();
	CTeams &operator=(const CTeams &master);
	bool operator==(const CTeams &rhs) const;
	void Clear();
	bool Read( FitIniFile* missionFile );
	bool Save( FitIniFile* missionFile );
	CTeam &TeamRef(int i);
	void handleObjectInvalidation(const EditorObject *pObj);
	bool NoteThePositionsOfObjectsReferenced();
	bool RestoreObjectPointerReferencesFromNotedPositions();
	bool ThereAreObjectivesWithNoConditions();
private:
	CTeam m_teamArray[GAME_MAX_PLAYERS];
};

class CPlayer
{
public:
	CPlayer() { Clear(); }
	void Clear() { m_defaultTeam = 0; }
	int DefaultTeam() { return m_defaultTeam; }
	void DefaultTeam(int team);
	bool Read( FitIniFile* missionFile, int playerNum );
	bool Save( FitIniFile* missionFile, int playerNum );
private:
	int m_defaultTeam;
};

class CPlayers
{
public:
	void Clear();
	CPlayer &PlayerRef(int i) { return m_playerArray[i]; }
	bool Read( FitIniFile* missionFile );
	bool Save( FitIniFile* missionFile );
private:
	CPlayers &operator=(const CPlayers &master) { /*not valid, do not use*/ }
	CPlayer m_playerArray[GAME_MAX_PLAYERS];
};

class MissionSettings
{
public:
	
	int largeArtillery;
	int smallArtillery;
	int cameraDrones;
	int sensors;
	int time;

	void clear(){ largeArtillery = smallArtillery = cameraDrones = sensors = time = 0; }
	MissionSettings(){ clear(); }

	bool save( FitIniFile* file );
};

class EditorData
{
	public:

		static EditorData* instance;

		EditorData();
		~EditorData();

		// wipes all the data, the terrain, object etc.
		static bool clear(); 
		
		// makes a terrain from a height map.  
		static bool initTerrainFromTGA( int mapSize, int min = 0, int max = 512, int terrain = 0 );
		static bool initTerrainFromPCV( const char* fileName );
		static bool	reassignHeightsFromTGA( const char* fileName, int min, int max );

		bool save( const char* fileName, bool quickSave = false );
		bool quickSave( const char* fileName );
		static bool saveHeightMap( File* file );
		bool saveMissionFitFileStuff( FitIniFile &fitFile );

		static void setMapName( const char* name );
		static const char* getMapName(){ return strlen( mapName ) ? mapName : 0; }
		static void updateTitleBar();

		EString MissionName() { return m_missionName; }
		void MissionName(EString missionName) { m_missionName = missionName; }

		bool MissionNameUseResourceString() { return m_missionNameUseResourceString; }
		void MissionNameUseResourceString(bool missionNameUseResourceString) { m_missionNameUseResourceString = missionNameUseResourceString; }

		int MissionNameResourceStringID() { return m_missionNameResourceStringID; }
		void MissionNameResourceStringID(int missionNameResourceStringID) { m_missionNameResourceStringID = missionNameResourceStringID; }

		EString Author() { return m_author; }
		void Author(EString author) { m_author = author; }

		EString Blurb() { return m_blurb; }
		void Blurb(EString blurb) { m_blurb = blurb; }

		bool BlurbUseResourceString() { return m_blurbUseResourceString; }
		void BlurbUseResourceString(bool blurbUseResourceString) { m_blurbUseResourceString = blurbUseResourceString; }

		int BlurbResourceStringID() { return m_blurbResourceStringID; }
		void BlurbResourceStringID(int blurbResourceStringID) { m_blurbResourceStringID = blurbResourceStringID; }

		EString Blurb2() { return m_blurb2; }
		void Blurb2(EString blurb2) { m_blurb2 = blurb2; }

		bool Blurb2UseResourceString() { return m_blurb2UseResourceString; }
		void Blurb2UseResourceString(bool blurb2UseResourceString) { m_blurb2UseResourceString = blurb2UseResourceString; }

		int Blurb2ResourceStringID() { return m_blurb2ResourceStringID; }
		void Blurb2ResourceStringID(int blurb2ResourceStringID) { m_blurb2ResourceStringID = blurb2ResourceStringID; }

		float TimeLimit() { return m_timeLimit; }
		void TimeLimit(float timeLimit) { m_timeLimit = timeLimit; }

		float DropWeightLimit() { return m_dropWeightLimit; }
		void DropWeightLimit(float dropWeightLimit) { m_dropWeightLimit = dropWeightLimit; }

		int InitialResourcePoints() { return m_initialResourcePoints; }
		void InitialResourcePoints(int initialResourcePoints) { m_initialResourcePoints = initialResourcePoints; }

		int CBills() { return m_CBills; }
		void CBills( int newCBills ) { m_CBills = newCBills; }

		bool IsSinglePlayer() { return m_isSinglePlayer; }
		void IsSinglePlayer(bool isSinglePlayer) { m_isSinglePlayer = isSinglePlayer; }

		int MaxTeams() { return m_maxTeams; }
		void MaxTeams(int maxTeams) { m_maxTeams = maxTeams; /* Not finished. Need to check 2 <= param <= 8, disable menu items, etc.,. */}

		int MaxPlayers() { return m_maxPlayers; }
		void MaxPlayers(int maxPlayers);
		//{ m_maxPlayers = maxPlayers; /* Not finished. Need to check 2 <= param <= 8, disable menu items, etc.,. */}

		CTeams &TeamsRef() { return m_teams; }
		CPlayers &PlayersRef() { return m_players; }

		int ScenarioTune() { return m_scenarioTune; }
		void ScenarioTune(int scenarioTune) { m_scenarioTune = scenarioTune; }

		EString VideoFilename() { return m_videoFilename; }
		void VideoFilename(EString videoFilename) { m_videoFilename = videoFilename; }

		int NumRandomRPbuildings() { return m_numRandomRPbuildings; }
		void NumRandomRPbuildings(int numRandomRPbuildings) { m_numRandomRPbuildings = numRandomRPbuildings; }

		EString DownloadURL() { return m_downloadURL; }
		void DownloadURL(EString downloadURL) { m_downloadURL = downloadURL; }

		int MissionType() { return m_missionType; }
		void MissionType(int missionType) { m_missionType = missionType; }

		bool AirStrikesEnabledDefault() { return m_airStrikesEnabledDefault; }
		void AirStrikesEnabledDefault(bool airStrikesEnabledDefault) { m_airStrikesEnabledDefault = airStrikesEnabledDefault; }

		bool MineLayersEnabledDefault() { return m_mineLayersEnabledDefault; }
		void MineLayersEnabledDefault(bool mineLayersEnabledDefault) { m_mineLayersEnabledDefault = mineLayersEnabledDefault; }

		bool ScoutCoptersEnabledDefault() { return m_scoutCoptersEnabledDefault; }
		void ScoutCoptersEnabledDefault(bool scoutCoptersEnabledDefault) { m_scoutCoptersEnabledDefault = scoutCoptersEnabledDefault; }

		bool SensorProbesEnabledDefault() { return m_sensorProbesEnabledDefault; }
		void SensorProbesEnabledDefault(bool sensorProbesEnabledDefault) { m_sensorProbesEnabledDefault = sensorProbesEnabledDefault; }

		bool UnlimitedAmmoEnabledDefault() { return m_unlimitedAmmoEnabledDefault; }
		void UnlimitedAmmoEnabledDefault(bool unlimitedAmmoEnabledDefault) { m_unlimitedAmmoEnabledDefault = unlimitedAmmoEnabledDefault; }

		bool AllTechEnabledDefault() { return m_allTechEnabledDefault; }
		void AllTechEnabledDefault(bool allTechEnabledDefault) { m_allTechEnabledDefault = allTechEnabledDefault; }

		bool RepairVehicleEnabledDefault() { return m_repairVehicleEnabledDefault; }
		void RepairVehicleEnabledDefault(bool repairVehicleEnabledDefault) { m_repairVehicleEnabledDefault = repairVehicleEnabledDefault; }

		bool SalvageCraftEnabledDefault() { return m_salvageCraftEnabledDefault; }
		void SalvageCraftEnabledDefault(bool salvageCraftEnabledDefault) { m_salvageCraftEnabledDefault = salvageCraftEnabledDefault; }

		bool ResourceBuildingsEnabledDefault() { return m_resourceBuildingsEnabledDefault; }
		void ResourceBuildingsEnabledDefault(bool resourceBuildingsEnabledDefault) { m_resourceBuildingsEnabledDefault = resourceBuildingsEnabledDefault; }

		bool NoVariantsEnabledDefault() { return m_noVariantsEnabledDefault; }
		void NoVariantsEnabledDefault(bool noVariantsEnabledDefault) { m_noVariantsEnabledDefault = noVariantsEnabledDefault; }

		bool ArtilleryPieceEnabledDefault() { return m_artilleryPieceEnabledDefault; }
		void ArtilleryPieceEnabledDefault(bool artilleryPieceEnabledDefault) { m_artilleryPieceEnabledDefault = artilleryPieceEnabledDefault; }

		bool RPsForMechsEnabledDefault() { return m_rPsForMechsEnabledDefault; }
		void RPsForMechsEnabledDefault(bool rPsForMechsEnabledDefault) { m_rPsForMechsEnabledDefault = rPsForMechsEnabledDefault; }

		//weather
		int MaxRaindrops() { return m_maxRaindrops; }
		void MaxRaindrops(int maxRaindrops) { m_maxRaindrops = maxRaindrops; }

		float StartingRainLevel() { return m_startingRainLevel; }
		void StartingRainLevel(float startingRainLevel) { m_startingRainLevel = startingRainLevel; }

		float ChanceOfRain() { return m_chanceOfRain; }
		void ChanceOfRain(float chanceOfRain) { m_chanceOfRain = chanceOfRain; }

		float BaseLightningChance() { return m_baseLightningChance; }
		void BaseLightningChance(float baseLightningChance) { m_baseLightningChance = baseLightningChance; }
		
		long TheSkyNumber() 
		{
			return m_theSkyNumber;
		}
		
		void TheSkyNumber (long theSkyNumber)
		{
			m_theSkyNumber = theSkyNumber;
		}

		bool MissionNeedsSaving() { return m_missionNeedsSaving; }
		void MissionNeedsSaving(bool missionNeedsSaving) { m_missionNeedsSaving = missionNeedsSaving; }

		bool DetailTextureNeedsSaving() { return m_detailTextureNeedsSaving; }
		void DetailTextureNeedsSaving(bool detailTextureNeedsSaving) { m_detailTextureNeedsSaving = detailTextureNeedsSaving; }

		bool WaterTextureNeedsSaving() { return m_waterTextureNeedsSaving; }
		void WaterTextureNeedsSaving(bool waterTextureNeedsSaving) { m_waterTextureNeedsSaving = waterTextureNeedsSaving; }

		bool WaterDetailTextureNeedsSaving() { return m_waterDetailTextureNeedsSaving; }
		void WaterDetailTextureNeedsSaving(bool waterDetailTextureNeedsSaving) { m_waterDetailTextureNeedsSaving = waterDetailTextureNeedsSaving; }

		bool saveObjectives( FitIniFile* file );
		bool DoTeamDialog(int team) { return TeamsRef().TeamRef(team).ObjectivesRef().EditDialog(); }
		void handleObjectInvalidation(const EditorObject *pObj) { TeamsRef().handleObjectInvalidation(pObj); }

		void makeTacMap(BYTE*& pOutput, long& dataSize, int tacMapSize ); // this allocates memory, below just draws
		void drawTacMap( BYTE* pDest, long dataSize, int tacMapSize );
		void loadTacMap(PacketFile *file, BYTE*& pDest, long dataSize, int tacMapSize ); // this allocates memory.  Saves millions of CPU cycles on load 

	private:

		EditorData& operator=( const EditorData& editorData );
		EditorData( const EditorData& editorData );
		bool saveTacMap (PacketFile* file, long whichPacket );

		EString m_missionName;
		bool m_missionNameUseResourceString;
		int m_missionNameResourceStringID;
		EString m_author;
		EString m_blurb;
		bool m_blurbUseResourceString;
		int m_blurbResourceStringID;
		EString m_blurb2;
		bool m_blurb2UseResourceString;
		int m_blurb2ResourceStringID;
		float m_timeLimit;
		float m_dropWeightLimit;
		int m_initialResourcePoints;
		int	m_CBills;
		bool m_isSinglePlayer;
		int m_maxTeams;
		int m_maxPlayers;
		CTeams m_teams;
		CPlayers m_players;
		int m_scenarioTune;
		EString m_videoFilename;
		int m_numRandomRPbuildings;
		EString m_downloadURL;
		int m_missionType;

		/*defaults for multiplayer options*/
		bool m_airStrikesEnabledDefault;
		bool m_mineLayersEnabledDefault;
		bool m_scoutCoptersEnabledDefault;
		bool m_sensorProbesEnabledDefault;
		bool m_unlimitedAmmoEnabledDefault;
		bool m_allTechEnabledDefault;
		bool m_repairVehicleEnabledDefault;
		bool m_salvageCraftEnabledDefault;
		bool m_resourceBuildingsEnabledDefault;
		bool m_noVariantsEnabledDefault;
		bool m_artilleryPieceEnabledDefault;
		bool m_rPsForMechsEnabledDefault;

		static char mapName[256];

		static unsigned long*	tacMapBmp; // tac map data, keep around so we can update

		MissionSettings	missionSettings;

		//weather info
		int m_maxRaindrops;
		float m_startingRainLevel;
		float m_chanceOfRain;
		float m_baseLightningChance;
		
		long m_theSkyNumber;

		bool m_missionNeedsSaving;
		bool m_detailTextureNeedsSaving;
		bool m_waterTextureNeedsSaving;
		bool m_waterDetailTextureNeedsSaving;
};




//*************************************************************************************************
#endif  // end of file ( EditorData.h )
