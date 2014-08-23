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

		HeightDlg( int32_t min = 0, int32_t max = 0);
		void Init();
		void OnOK();
		//virtual	void OnCommand(Window *wnd,int32_t nCommand);

		int32_t GetMin(){ return min; }
		int32_t GetMax(){ return max; }

		void SetMin( int32_t newMin ){ min = newMin; }
		void SetMax( int32_t newMax ){ max = newMax; }
		virtual BOOL OnInitDialog(){ Init(); return TRUE; }


	private:

		int32_t min;
		int32_t max;

};


//*************************************************************************************************
#endif  // end of file ( HeightDlg.h )
