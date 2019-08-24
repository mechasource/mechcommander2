//---------------------------------------------------------------------------
//
// Paths.h -- File contains the file path string declarations
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef PATHS_H
#define PATHS_H

//---------------------------------------------------------------------------
// static Globals

enum class data_paths : uint32_t
{
	terrainPath,
	objectPath,
	missionPath,
	cameraPath,
	tilePath,
	tile90Path,
	moviePath,
	shapesPath,
	saveTempPath,
	spritePath,
	artPath,
	soundPath,
	interfacePath,
	profilePath,
	warriorPath,
	fontPath,
	savePath,
	texturePath,
	tglPath,
	effectsPath,
	campaignPath,
	CDobjectPath,
	CDmissionPath,
	CDcameraPath,
	CDtilePath,
	CDmoviePath,
	CDspritePath,
	CDsoundPath,
	transcriptsPath,
};

void
GetMcDataPath(const std::wstring_view& path, uint32_t id);
