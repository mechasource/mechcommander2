#ifndef MAPSIZEDLG_H
#define MAPSIZEDLG_H
/*************************************************************************************************\
mapsizedlg.h			: Interface for the MapSizeDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "stdafx.h"
#include "resource.h"

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
TerrainDlg:
**************************************************************************************************/
class MapSizeDlg: public CDialog
{
	public:

		MapSizeDlg() : CDialog( IDD_MAPSIZE ){ mapSize = 0;}
		virtual ~MapSizeDlg(){}
		virtual void Init();
		virtual BOOL OnInitDialog(){ Init(); return true; }
		void OnOK();

		int mapSize;
};


//*************************************************************************************************
#endif  // end of file ( TerrainDlg.h )

