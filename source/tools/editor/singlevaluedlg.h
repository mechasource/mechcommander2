/*************************************************************************************************\
HeightDlg.h			: Interface for the HeightDlg component.
//-------------------------------------------------------------------------------------------------
Notes				: lets the user enter a minimum and maximum height for the map
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef SINGLEVALDLG_H
#define SINGLEVALDLG_H

//#include "stdafx.h"

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
HeightDlg:
**************************************************************************************************/
class SingleValueDlg: public CDialog
{
	public:

		SingleValueDlg( int32_t captionID, int32_t staticTextID, int32_t val = 0);
		virtual ~SingleValueDlg();
		virtual void Init();
		virtual void OnOK();
//		virtual	void OnCommand(Window *wnd,int32_t nCommand);

		int32_t GetVal(){ return val; }

		void SetVal( int32_t newVal ){ val = newVal; }
		void Update();

		virtual BOOL OnInitDialog( ){ Init(); return TRUE; }

	private:

		char  staticText[256];
		char  caption[256];

		int32_t val;

};


//*************************************************************************************************
#endif  // end of file ( HeightDlg.h )
