//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef EDITORDATA_H
#define EDITORDATA_H

//#include "mclib.h"
//#include "elist.h"
//#include "estring.h"
//#include "objective.h"

static const int32_t GAME_MAX_PLAYERS = 8;

#define EDITOR_VISIBLE_VERTICES 60

class CTeam
{
public:
	CTeam(int32_t alignment = 0) { Alignment(alignment); }
	bool operator==(const CTeam& rhs) const;
	void Clear() { m_objectives.Clear(void); }
	int32_t Alignment() { return m_alignment; }
	void Alignment(int32_t alignment)
	{
		m_alignment = alignment;
		m_objectives.Alignment(m_alignment);
	}
	bool Read(FitIniFile* missionFile) { return m_objectives.Read(missionFile); }
	bool Save(FitIniFile* missionFile) { return m_objectives.Save(missionFile); }
	CObjectives& ObjectivesRef() { return m_objectives; }
	void handleObjectInvalidation(const EditorObject* pObj)
	{
		m_objectives.handleObjectInvalidation(pObj);
	}
	bool NoteThePositionsOfObjectsReferenced()
	{
		return m_objectives.NoteThePositionsOfObjectsReferenced(void);
	}
	bool RestoreObjectPointerReferencesFromNotedPositions()
	{
		return m_objectives.RestoreObjectPointerReferencesFromNotedPositions(void);
	}
	bool ThereAreObjectivesWithNoConditions()
	{
		return m_objectives.ThereAreObjectivesWithNoConditions(void);
	}

private:
	int32_t m_alignment;
	CObjectives m_objectives;
};

class CTeams
{
public:
	CTeams(void);
	CTeams& operator=(const CTeams& master);
	bool operator==(const CTeams& rhs) const;
	void Clear(void);
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* missionFile);
	CTeam& TeamRef(int32_t i);
	void handleObjectInvalidation(const EditorObject* pObj);
	bool NoteThePositionsOfObjectsReferenced(void);
	bool RestoreObjectPointerReferencesFromNotedPositions(void);
	bool ThereAreObjectivesWithNoConditions(void);

private:
	CTeam m_teamArray[GAME_MAX_PLAYERS];
};

class CPlayer
{
public:
	CPlayer() { Clear(void); }
	void Clear() { m_defaultTeam = 0; }
	int32_t DefaultTeam() { return m_defaultTeam; }
	void DefaultTeam(int32_t team);
	bool Read(FitIniFile* missionFile, int32_t playerNum);
	bool Save(FitIniFile* missionFile, int32_t playerNum);

private:
	int32_t m_defaultTeam;
};

class CPlayers
{
public:
	void Clear(void);
	CPlayer& PlayerRef(int32_t i) { return m_playerArray[i]; }
	bool Read(FitIniFile* missionFile);
	bool Save(FitIniFile* missionFile);

private:
	CPlayers& operator=(const CPlayers& master)
	{ /*not valid, do not use*/
	}
	CPlayer m_playerArray[GAME_MAX_PLAYERS];
};

class MissionSettings
{
public:
	int32_t largeArtillery;
	int32_t smallArtillery;
	int32_t cameraDrones;
	int32_t sensors;
	int32_t time;

	void clear() { largeArtillery = smallArtillery = cameraDrones = sensors = time = 0; }
	MissionSettings() { clear(void); }

	bool save(FitIniFile* file);
};

class EditorData
{
public:
	static EditorData* instance;

	EditorData(void);
	~EditorData(void);

	// wipes all the data, the terrain, object etc.
	static bool clear(void);

	// makes a terrain from a height map.
	static bool initTerrainFromTGA(
		int32_t mapSize, int32_t min = 0, int32_t max = 512, int32_t terrain = 0);
	static bool initTerrainFromPCV(const std::wstring_view& fileName);
	static bool reassignheightsFromTGA(const std::wstring_view& fileName, int32_t min, int32_t max);

	bool save(const std::wstring_view& fileName, bool quickSave = false);
	bool quickSave(const std::wstring_view& fileName);
	static bool saveheightMap(File* file);
	bool saveMissionFitFileStuff(FitIniFile& fitFile);

	static void setMapName(const std::wstring_view& name);
	static const std::wstring_view& getMapName() { return strlen(mapName) ? mapName : 0; }
	static void updateTitleBar(void);

	const std::wstring_view& MissionName() { return m_missionName; }
	void MissionName(const std::wstring_view& missionName) { m_missionName = missionName; }

	bool MissionNameUseResourceString() { return m_missionNameUseResourceString; }
	void MissionNameUseResourceString(bool missionNameUseResourceString)
	{
		m_missionNameUseResourceString = missionNameUseResourceString;
	}

	int32_t MissionNameResourceStringID() { return m_missionNameResourceStringID; }
	void MissionNameResourceStringID(int32_t missionNameResourceStringID)
	{
		m_missionNameResourceStringID = missionNameResourceStringID;
	}

	const std::wstring_view& Author() { return m_author; }
	void Author(const std::wstring_view& author) { m_author = author; }

	const std::wstring_view& Blurb() { return m_blurb; }
	void Blurb(const std::wstring_view& blurb) { m_blurb = blurb; }

	bool BlurbUseResourceString() { return m_blurbUseResourceString; }
	void BlurbUseResourceString(bool blurbUseResourceString)
	{
		m_blurbUseResourceString = blurbUseResourceString;
	}

	int32_t BlurbResourceStringID() { return m_blurbResourceStringID; }
	void BlurbResourceStringID(int32_t blurbResourceStringID)
	{
		m_blurbResourceStringID = blurbResourceStringID;
	}

	const std::wstring_view& Blurb2() { return m_blurb2; }
	void Blurb2(const std::wstring_view& blurb2) { m_blurb2 = blurb2; }

	bool Blurb2UseResourceString() { return m_blurb2UseResourceString; }
	void Blurb2UseResourceString(bool blurb2UseResourceString)
	{
		m_blurb2UseResourceString = blurb2UseResourceString;
	}

	int32_t Blurb2ResourceStringID() { return m_blurb2ResourceStringID; }
	void Blurb2ResourceStringID(int32_t blurb2ResourceStringID)
	{
		m_blurb2ResourceStringID = blurb2ResourceStringID;
	}

	float TimeLimit() { return m_timeLimit; }
	void TimeLimit(float timeLimit) { m_timeLimit = timeLimit; }

	float DropWeightLimit() { return m_dropWeightLimit; }
	void DropWeightLimit(float dropWeightLimit) { m_dropWeightLimit = dropWeightLimit; }

	int32_t InitialResourcePoints() { return m_initialResourcePoints; }
	void InitialResourcePoints(int32_t initialResourcePoints)
	{
		m_initialResourcePoints = initialResourcePoints;
	}

	int32_t CBills() { return m_CBills; }
	void CBills(int32_t newCBills) { m_CBills = newCBills; }

	bool IsSinglePlayer() { return m_isSinglePlayer; }
	void IsSinglePlayer(bool isSinglePlayer) { m_isSinglePlayer = isSinglePlayer; }

	int32_t MaxTeams() { return m_maxTeams; }
	void MaxTeams(int32_t maxTeams)
	{
		m_maxTeams = maxTeams; /* Not finished. Need to check 2 <= param <= 8,
								  disable menu items, etc.,. */
	}

	int32_t MaxPlayers() { return m_maxPlayers; }
	void MaxPlayers(int32_t maxPlayers);
	//{ m_maxPlayers = maxPlayers; /* Not finished. Need to check 2 <= param <=
	// 8, disable menu items, etc.,. */}

	CTeams& TeamsRef() { return m_teams; }
	CPlayers& PlayersRef() { return m_players; }

	int32_t ScenarioTune() { return m_scenarioTune; }
	void ScenarioTune(int32_t scenarioTune) { m_scenarioTune = scenarioTune; }

	const std::wstring_view& VideoFilename() { return m_videoFilename; }
	void VideoFilename(const std::wstring_view& videoFilename) { m_videoFilename = videoFilename; }

	int32_t NumRandomRPbuildings() { return m_numRandomRPbuildings; }
	void NumRandomRPbuildings(int32_t numRandomRPbuildings)
	{
		m_numRandomRPbuildings = numRandomRPbuildings;
	}

	const std::wstring_view& DownloadURL() { return m_downloadURL; }
	void DownloadURL(const std::wstring_view& downloadURL) { m_downloadURL = downloadURL; }

	int32_t MissionType() { return m_missionType; }
	void MissionType(int32_t missionType) { m_missionType = missionType; }

	bool AirStrikesEnabledDefault() { return m_airStrikesEnabledDefault; }
	void AirStrikesEnabledDefault(bool airStrikesEnabledDefault)
	{
		m_airStrikesEnabledDefault = airStrikesEnabledDefault;
	}

	bool MineLayersEnabledDefault() { return m_mineLayersEnabledDefault; }
	void MineLayersEnabledDefault(bool mineLayersEnabledDefault)
	{
		m_mineLayersEnabledDefault = mineLayersEnabledDefault;
	}

	bool ScoutCoptersEnabledDefault() { return m_scoutCoptersEnabledDefault; }
	void ScoutCoptersEnabledDefault(bool scoutCoptersEnabledDefault)
	{
		m_scoutCoptersEnabledDefault = scoutCoptersEnabledDefault;
	}

	bool SensorProbesEnabledDefault() { return m_sensorProbesEnabledDefault; }
	void SensorProbesEnabledDefault(bool sensorProbesEnabledDefault)
	{
		m_sensorProbesEnabledDefault = sensorProbesEnabledDefault;
	}

	bool UnlimitedAmmoEnabledDefault() { return m_unlimitedAmmoEnabledDefault; }
	void UnlimitedAmmoEnabledDefault(bool unlimitedAmmoEnabledDefault)
	{
		m_unlimitedAmmoEnabledDefault = unlimitedAmmoEnabledDefault;
	}

	bool AllTechEnabledDefault() { return m_allTechEnabledDefault; }
	void AllTechEnabledDefault(bool allTechEnabledDefault)
	{
		m_allTechEnabledDefault = allTechEnabledDefault;
	}

	bool RepairVehicleEnabledDefault() { return m_repairVehicleEnabledDefault; }
	void RepairVehicleEnabledDefault(bool repairVehicleEnabledDefault)
	{
		m_repairVehicleEnabledDefault = repairVehicleEnabledDefault;
	}

	bool SalvageCraftEnabledDefault() { return m_salvageCraftEnabledDefault; }
	void SalvageCraftEnabledDefault(bool salvageCraftEnabledDefault)
	{
		m_salvageCraftEnabledDefault = salvageCraftEnabledDefault;
	}

	bool ResourceBuildingsEnabledDefault() { return m_resourceBuildingsEnabledDefault; }
	void ResourceBuildingsEnabledDefault(bool resourceBuildingsEnabledDefault)
	{
		m_resourceBuildingsEnabledDefault = resourceBuildingsEnabledDefault;
	}

	bool NoVariantsEnabledDefault() { return m_noVariantsEnabledDefault; }
	void NoVariantsEnabledDefault(bool noVariantsEnabledDefault)
	{
		m_noVariantsEnabledDefault = noVariantsEnabledDefault;
	}

	bool ArtilleryPieceEnabledDefault() { return m_artilleryPieceEnabledDefault; }
	void ArtilleryPieceEnabledDefault(bool artilleryPieceEnabledDefault)
	{
		m_artilleryPieceEnabledDefault = artilleryPieceEnabledDefault;
	}

	bool RPsForMechsEnabledDefault() { return m_rPsForMechsEnabledDefault; }
	void RPsForMechsEnabledDefault(bool rPsForMechsEnabledDefault)
	{
		m_rPsForMechsEnabledDefault = rPsForMechsEnabledDefault;
	}

	// weather
	int32_t MaxRaindrops() { return m_maxRaindrops; }
	void MaxRaindrops(int32_t maxRaindrops) { m_maxRaindrops = maxRaindrops; }

	float StartingRainLevel() { return m_startingRainLevel; }
	void StartingRainLevel(float startingRainLevel) { m_startingRainLevel = startingRainLevel; }

	float ChanceOfRain() { return m_chanceOfRain; }
	void ChanceOfRain(float chanceOfRain) { m_chanceOfRain = chanceOfRain; }

	float BaseLightningChance() { return m_baseLightningChance; }
	void BaseLightningChance(float baseLightningChance)
	{
		m_baseLightningChance = baseLightningChance;
	}

	int32_t TheSkyNumber() { return m_theSkyNumber; }

	void TheSkyNumber(int32_t theSkyNumber) { m_theSkyNumber = theSkyNumber; }

	bool MissionNeedsSaving() { return m_missionNeedsSaving; }
	void MissionNeedsSaving(bool missionNeedsSaving) { m_missionNeedsSaving = missionNeedsSaving; }

	bool DetailTextureNeedsSaving() { return m_detailTextureNeedsSaving; }
	void DetailTextureNeedsSaving(bool detailTextureNeedsSaving)
	{
		m_detailTextureNeedsSaving = detailTextureNeedsSaving;
	}

	bool WaterTextureNeedsSaving() { return m_waterTextureNeedsSaving; }
	void WaterTextureNeedsSaving(bool waterTextureNeedsSaving)
	{
		m_waterTextureNeedsSaving = waterTextureNeedsSaving;
	}

	bool WaterDetailTextureNeedsSaving() { return m_waterDetailTextureNeedsSaving; }
	void WaterDetailTextureNeedsSaving(bool waterDetailTextureNeedsSaving)
	{
		m_waterDetailTextureNeedsSaving = waterDetailTextureNeedsSaving;
	}

	bool saveObjectives(FitIniFile* file);
	bool DoTeamDialog(int32_t team)
	{
		return TeamsRef().TeamRef(team).ObjectivesRef().EditDialog(void);
	}
	void handleObjectInvalidation(const EditorObject* pObj)
	{
		TeamsRef().handleObjectInvalidation(pObj);
	}

	void makeTacMap(uint8_t*& pOutput, int32_t& dataSize,
		int32_t tacMapSize); // this allocates memory, below just draws
	void drawTacMap(uint8_t* pDest, size_t dataSize, int32_t tacMapSize);
	void loadTacMap(PacketFile* file, uint8_t*& pDest, size_t dataSize,
		int32_t tacMapSize); // this allocates memory.  Saves millions of CPU
		// cycles on load

private:
	EditorData& operator=(const EditorData& editorData);
	EditorData(const EditorData& editorData);
	bool saveTacMap(PacketFile* file, int32_t whichPacket);

	const std::wstring_view& m_missionName;
	bool m_missionNameUseResourceString;
	int32_t m_missionNameResourceStringID;
	const std::wstring_view& m_author;
	const std::wstring_view& m_blurb;
	bool m_blurbUseResourceString;
	int32_t m_blurbResourceStringID;
	const std::wstring_view& m_blurb2;
	bool m_blurb2UseResourceString;
	int32_t m_blurb2ResourceStringID;
	float m_timeLimit;
	float m_dropWeightLimit;
	int32_t m_initialResourcePoints;
	int32_t m_CBills;
	bool m_isSinglePlayer;
	int32_t m_maxTeams;
	int32_t m_maxPlayers;
	CTeams m_teams;
	CPlayers m_players;
	int32_t m_scenarioTune;
	const std::wstring_view& m_videoFilename;
	int32_t m_numRandomRPbuildings;
	const std::wstring_view& m_downloadURL;
	int32_t m_missionType;

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

	static wchar_t mapName[256];

	static uint32_t* tacMapBmp; // tac map data, keep around so we can update

	MissionSettings missionSettings;

	// weather info
	int32_t m_maxRaindrops;
	float m_startingRainLevel;
	float m_chanceOfRain;
	float m_baseLightningChance;

	int32_t m_theSkyNumber;

	bool m_missionNeedsSaving;
	bool m_detailTextureNeedsSaving;
	bool m_waterTextureNeedsSaving;
	bool m_waterDetailTextureNeedsSaving;
};

#endif // end of file ( EditorData.h )
