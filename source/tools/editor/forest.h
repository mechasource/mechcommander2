/*************************************************************************************************\
forest.h			: Interface for the forest component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef FOREST_H
#define FOREST_H

//#include "stdafx.h"

class FitIniFile;

#define FOREST_TYPES 15

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
forest:
**************************************************************************************************/
class Forest
{

public:

	Forest( int32_t newID );
	Forest( const Forest& src );

	Forest& operator=( const Forest& src );

	void init( );
	void init( FitIniFile& file );
	void save( );
	void save( FitIniFile& file );


	PCSTR getFileName() const { return fileName; }
	void		setFileName( PCSTR newName ){ fileName = newName; }

	PCSTR getName() const { return name;}
	void		setName( PCSTR newName ){ name = newName; }

	int32_t		getID() const { return ID; }
	

private:



		int32_t	ID;
		CString fileName;
		CString	name;
		
		bool	bRandom;

		float	maxHeight;
		float	minHeight;
		float	maxDensity;
		float	minDensity;

		float	percentages[FOREST_TYPES];

		float	centerX;
		float	centerY;
		float	radius;
	

		friend class EditorObjectMgr;
		friend class ForestDlg;

};


//*************************************************************************************************
#endif  // end of file ( forest.h )
