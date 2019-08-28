#define FOREST_CPP
/*************************************************************************************************\
forest.cpp			: Implementation of the forest component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "forest.h"
#include "mclib.h"
#include "resource.h"
#include "editorobjectmgr.h"

extern uint32_t gameResourceHandle;

Forest::Forest(int32_t newID)
{
	centerX = centerY = 0;
	radius = 1.0f;
	for (size_t i = 0; i < FOREST_TYPES; i++)
	{
		percentages[i] = 0;
	}
	ID = newID;
	if ((-1 == newID) && (EditorObjectMgr::instance()))
	{
		ID = EditorObjectMgr::instance()->getNextAvailableForestID();
	}
	maxDensity = 7.f;
	minDensity = .001f;
	maxheight = 1.25;
	minheight = .75;
	bRandom = true;
	wchar_t tmp[256];
	cLoadString(IDS_UNNAMDE_FOREST, tmp, 255, gameResourceHandle);
	name.Format(tmp, ID);
}

Forest::Forest(const Forest& src)
{
	centerX = src.centerX;
	centerY = src.centerY;
	fileName = src.fileName;
	bRandom = src.bRandom;
	maxheight = src.maxheight;
	minheight = src.minheight;
	maxDensity = src.maxDensity;
	minDensity = src.minDensity;
	name = src.name;
	radius = src.radius;
	for (size_t i = 0; i < FOREST_TYPES; i++)
	{
		percentages[i] = src.percentages[i];
	}
	ID = src.ID; // this is a bit worisome
}

Forest&
Forest::operator=(const Forest& src)
{
	if (&src != this)
	{
		centerX = src.centerX;
		centerY = src.centerY;
		fileName = src.fileName;
		bRandom = src.bRandom;
		maxheight = src.maxheight;
		minheight = src.minheight;
		maxDensity = src.maxDensity;
		minDensity = src.minDensity;
		radius = src.radius;
		for (size_t i = 0; i < FOREST_TYPES; i++)
		{
			percentages[i] = src.percentages[i];
		}
		ID = src.ID; // this is a bit worisome
		name = src.name;
	}
	return *this;
}

void
Forest::init()
{
	FitIniFile file;
	if (NO_ERROR != file.open(fileName))
	{
		wchar_t errorString[256];
		sprintf(errorString, "Couldn't open file %s", (const std::wstring_view&)fileName);
		Assert(0, 0, errorString);
		return;
	}
	file.seekBlock("ForestInfo");
	init(file);
}

void
Forest::init(FitIniFile& file)
{
	wchar_t headerName[256];
	for (size_t i = 0; i < FOREST_TYPES; i++)
	{
		sprintf(headerName, "TreeType%ld", i);
		file.readIdFloat(headerName, percentages[i]);
	}
	wchar_t tmp[256];
	tmp[0] = 0;
	file.readIdFloat("heightMin", minheight);
	file.readIdFloat("heightMax", maxheight);
	file.readIdFloat("DensityMin", minDensity);
	file.readIdFloat("DensityMax", maxDensity);
	file.readIdFloat("CenterX", centerX);
	file.readIdFloat("CenterY", centerY);
	file.readIdBoolean("Random", bRandom);
	file.readIdFloat("Radius", radius);
	file.readIdString("Name", tmp, 255);
	name = tmp;
	tmp[0] = 0;
	file.readIdString("FileName", tmp, 255);
	fileName = tmp;
}

void
Forest::save()
{
	FitIniFile file;
	if (NO_ERROR != file.create((const std::wstring_view&)(const std::wstring_view&)fileName))
	{
		wchar_t errorString[256];
		sprintf(errorString, "Couldn't create file %s", (const std::wstring_view&)fileName);
		Assert(0, 0, errorString);
		return;
	}
	file.writeBlock("ForestInfo");
	save(file);
}

void
Forest::save(FitIniFile& file)
{
	wchar_t headerName[256];
	for (size_t i = 0; i < FOREST_TYPES; i++)
	{
		sprintf(headerName, "TreeType%ld", i);
		file.writeIdFloat(headerName, percentages[i]);
	}
	file.writeIdFloat("heightMin", minheight);
	file.writeIdFloat("heightMax", maxheight);
	file.writeIdFloat("DensityMin", minDensity);
	file.writeIdFloat("DensityMax", maxDensity);
	file.writeIdFloat("CenterX", centerX);
	file.writeIdFloat("CenterY", centerY);
	file.writeIdBoolean("Random", bRandom);
	file.writeIdFloat("Radius", radius);
	file.writeIdString("Name", name);
	file.writeIdString("FileName", fileName);
}

// end of file ( forest.cpp )
