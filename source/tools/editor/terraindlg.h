
#pragma once

#ifndef TERRAINDLG_H
#define TERRAINDLG_H
/*************************************************************************************************\
TerrainDlg.h			: Interface for the TerrainDlg component.
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
class TerrainDlg : public CDialog
{
  public:
	TerrainDlg() : CDialog(IDD_TERRAINS) { terrain = 0; }
	virtual ~TerrainDlg() {}
	virtual void Init(void);
	virtual BOOL OnInitDialog()
	{
		Init(void);
		return true;
	}
	void OnOK(void);

	int32_t terrain;
};

//*************************************************************************************************
#endif // end of file ( TerrainDlg.h )
