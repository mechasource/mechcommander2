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
class TerrainDlg: public CDialog
{
	public:

		TerrainDlg() : CDialog( IDD_TERRAINS ){ terrain = 0;}
		virtual ~TerrainDlg(){}
		virtual void Init();
		virtual BOOL OnInitDialog(){ Init(); return true; }
		void OnOK();


		int terrain;
};


//*************************************************************************************************
#endif  // end of file ( TerrainDlg.h )
