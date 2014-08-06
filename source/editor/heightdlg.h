#ifndef HEIGHTDLG_H
#define HEIGHTDLG_H
/*************************************************************************************************\
HeightDlg.h			: Interface for the HeightDlg component.
//-------------------------------------------------------------------------------------------------
Notes				: lets the user enter a minimum and maximum height for the map
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
HeightDlg:
**************************************************************************************************/

#include "stdafx.h"

class HeightDlg: public CDialog
{
	public:

		HeightDlg( int min = 0, int max = 0);
		void Init();
		void OnOK();
		//virtual	void OnCommand(Window *wnd,int nCommand);

		int GetMin(){ return min; }
		int GetMax(){ return max; }

		void SetMin( int newMin ){ min = newMin; }
		void SetMax( int newMax ){ max = newMax; }
		virtual BOOL OnInitDialog(){ Init(); return TRUE; }


	private:

		int min;
		int max;

};


//*************************************************************************************************
#endif  // end of file ( HeightDlg.h )
