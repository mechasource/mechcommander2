//---------------------------------------------------------------------------
//
// Paths.cpp -- File contains the file path string definitions with defaults
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "paths.h"

//---------------------------------------------------------------------------
// static Globals

void
GetMcDataPath(const std::wstring_view& path, uint32_t id)
{
	switch (id)
	{
	case data_paths::terrainPath:
		path(L"data/terrain/");
	case data_paths::objectPath:
		path(L"data/objects/");
	case data_paths::missionPath:
		path(L"data/missions/");
	case data_paths::cameraPath:
		path(L"data/cameras/cameras.fit");
	case data_paths::cameracolorsPath:
		path(L"data/cameras/colors.fit");
	case data_paths::tilePath:
		path(L"data/tiles/");
	case data_paths::tile90Path:
		path(L"data/tiles/");
	case data_paths::moviePath:
		path(L"data/movies/");
	case data_paths::shapesPath:
		path(L"data/sprites/");
	case data_paths::saveTempPath:
		path(L"data/save/temp/");
	case data_paths::spritePath:
		path(L"data/sprites/");
	case data_paths::artPath:
		path(L"data/art/");
	case data_paths::soundPath:
		path(L"data/sound/");
	case data_paths::interfacePath:
		path(L"data/interface/");
	case data_paths::profilePath:
		path(L"data/missions/profiles/");
	case data_paths::warriorPath:
		path(L"data/missions/profiles/");
	case data_paths::fontPath:
		path(L"data/fonts/");
	case data_paths::savePath:
		path(L"data/savegame/");
	case data_paths::texturePath:
		path(L"data/textures/");
	case data_paths::tglPath:
		path(L"data/tgl/");
	case data_paths::effectsPath:
		path(L"data/effects/");
	case data_paths::campaignPath:
		path(L"data/campaign/");
	case data_paths::CDobjectPath:
		path(L"data/objects/");
	case data_paths::CDmissionPath:
		path(L"data/missions/");
	case data_paths::CDcameraPath:
		path(L"data/cameras/");
	case data_paths::CDtilePath:
		path(L"data/tiles/");
	case data_paths::CDmoviePath:
		path(L"data/movies/");
	case data_paths::CDspritePath:
		path(L"data/sprites/");
	case data_paths::CDsoundPath:
		path(L"data/sound/");
	case data_paths::transcriptsPath:
		path(L"data/multiplayer/transcripts");
	}
}
