#ifndef FOREST_H
#define FOREST_H
/*************************************************************************************************\
forest.h			: Interface for the forest component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "stdafx.h"

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

	Forest( int newID );
	Forest( const Forest& src );

	Forest& operator=( const Forest& src );

	void init( );
	void init( FitIniFile& file );
	void save( );
	void save( FitIniFile& file );


	const char* getFileName() const { return fileName; }
	void		setFileName( const char* newName ){ fileName = newName; }

	const char* getName() const { return name;}
	void		setName( const char* newName ){ name = newName; }

	long		getID() const { return ID; }
	

private:



		long	ID;
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
