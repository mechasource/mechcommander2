#ifndef SINGLEVALDLG_H
#define SINGLEVALDLG_H
/*************************************************************************************************\
HeightDlg.h			: Interface for the HeightDlg component.
//-------------------------------------------------------------------------------------------------
Notes				: lets the user enter a minimum and maximum height for the map
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "stdafx.h"

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
HeightDlg:
**************************************************************************************************/
class SingleValueDlg: public CDialog
{
	public:

		SingleValueDlg( int captionID, int staticTextID, int val = 0);
		virtual ~SingleValueDlg();
		virtual void Init();
		virtual void OnOK();
//		virtual	void OnCommand(Window *wnd,int nCommand);

		int GetVal(){ return val; }

		void SetVal( int newVal ){ val = newVal; }
		void Update();

		virtual BOOL OnInitDialog( ){ Init(); return TRUE; }

	private:

		char  staticText[256];
		char  caption[256];

		int val;

};


//*************************************************************************************************
#endif  // end of file ( HeightDlg.h )
