/*************************************************************************************************\
mapsizedlg.h			: Interface for the MapSizeDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#pragma once

#ifndef MAPSIZEDLG_H
#define MAPSIZEDLG_H

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
TerrainDlg:
**************************************************************************************************/
class MapSizeDlg : public CDialog
{
public:
	MapSizeDlg() : CDialog(IDD_MAPSIZE) { mapSize = 0; }
	virtual ~MapSizeDlg() {}
	virtual void Init(void);
	virtual BOOL OnInitDialog()
	{
		Init(void);
		return true;
	}
	void OnOK(void);

	int32_t mapSize;
};

//*************************************************************************************************
#endif // end of file ( TerrainDlg.h )
