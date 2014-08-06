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

#include "EditorObjectMgr.h"

extern unsigned long gameResourceHandle;

Forest::Forest( int newID )
{ 
	centerX = centerY = 0;
	radius = 1.0f;
	for ( int i = 0; i < FOREST_TYPES; i++ )
	{
		percentages[i] = 0;
	}

	ID = newID;
	if ((-1 == newID) && (EditorObjectMgr::instance())) {
		ID = EditorObjectMgr::instance()->getNextAvailableForestID();
	}

	maxDensity = 7.f;
	minDensity = .001f;
	maxHeight = 1.25;
	minHeight = .75;
	bRandom = true;

	
	char tmp[256];
	cLoadString( IDS_UNNAMDE_FOREST, tmp, 255, gameResourceHandle );
	name.Format( tmp, ID );


}

Forest::Forest( const Forest& src )
{
	centerX =  src.centerX;
	centerY = src.centerY;
	fileName = src.fileName;
	
	bRandom = src.bRandom;

	maxHeight = src.maxHeight;
	minHeight = src.minHeight;
	maxDensity = src.maxDensity;
	minDensity = src.minDensity;

	name = src.name;


	radius = src.radius;
	for ( int i = 0; i < FOREST_TYPES; i++ )
	{
		percentages[i] = src.percentages[i];
	}
	
	ID = src.ID; // this is a bit worisome
}

Forest& Forest::operator=( const Forest& src )
{
	if ( &src != this )
	{
		centerX =  src.centerX;
		centerY = src.centerY;
		fileName = src.fileName;
		
		bRandom = src.bRandom;

		maxHeight = src.maxHeight;
		minHeight = src.minHeight;
		maxDensity = src.maxDensity;
		minDensity = src.minDensity;


		radius = src.radius;
		for ( int i = 0; i < FOREST_TYPES; i++ )
		{
			percentages[i] = src.percentages[i];
		}
		
		ID = src.ID; // this is a bit worisome
		name = src.name;
	}

	return *this;
}

void Forest::init()
{
	FitIniFile file;

	if ( NO_ERR != file.open( fileName ) )
	{
		char errorString[256];
		sprintf( errorString, "Couldn't open file %s", (const char*)fileName);
		Assert( 0, 0, errorString );
		return;
	}

	file.seekBlock( "ForestInfo" );

	init( file );



}

void Forest::init( FitIniFile& file )
{
	char headerName[256];
	for ( int i = 0; i < FOREST_TYPES; i++ )
	{
		sprintf( headerName, "TreeType%ld", i );
		file.readIdFloat( headerName, percentages[i] );	
	}


	char tmp[256];
	tmp[0] = 0;

	file.readIdFloat( "HeightMin", minHeight );
	file.readIdFloat( "HeightMax", maxHeight );
	file.readIdFloat( "DensityMin", minDensity );
	file.readIdFloat( "DensityMax", maxDensity );
	file.readIdFloat( "CenterX", centerX );
	file.readIdFloat( "CenterY", centerY );
	file.readIdBoolean( "Random", bRandom );
	file.readIdFloat( "Radius", radius );
	file.readIdString( "Name", tmp, 255);
	name = tmp;
	tmp[0] = 0;
	file.readIdString( "FileName", tmp, 255 );
	fileName = tmp;
}


void Forest::save()
{
	FitIniFile file;
	if ( NO_ERR != file.create( (char*)(const char*)fileName ) )
	{
		char errorString[256];
		sprintf( errorString, "Couldn't create file %s", (const char*)fileName);
		Assert( 0, 0, errorString );
		return;
	}

	file.writeBlock( "ForestInfo" );

	save( file );

	


}

void Forest::save( FitIniFile& file )
{

	char headerName[256];
	for ( int i = 0; i < FOREST_TYPES; i++ )
	{
		sprintf( headerName, "TreeType%ld", i );
		file.writeIdFloat( headerName, percentages[i] );	
	}

	file.writeIdFloat( "HeightMin", minHeight );
	file.writeIdFloat( "HeightMax", maxHeight );
	file.writeIdFloat( "DensityMin", minDensity );
	file.writeIdFloat( "DensityMax", maxDensity );
	file.writeIdFloat( "CenterX", centerX );
	file.writeIdFloat( "CenterY", centerY );
	file.writeIdBoolean( "Random", bRandom );
	file.writeIdFloat( "Radius", radius );
	file.writeIdString( "Name", name );
	file.writeIdString( "FileName", fileName );

}



//*************************************************************************************************
// end of file ( forest.cpp )
