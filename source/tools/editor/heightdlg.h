/*************************************************************************************************\
HeightDlg.h			: Interface for the HeightDlg component.
//-------------------------------------------------------------------------------------------------
Notes				: lets the user enter a minimum and maximum height for the
map
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//*************************************************************************************************/

#pragma once

#ifndef HEIGHTDLG_H
#define HEIGHTDLG_H

/**************************************************************************************************
CLASS DESCRIPTION
HeightDlg:
**************************************************************************************************/
class HeightDlg : public CDialog
{
public:
	HeightDlg(int32_t min = 0, int32_t max = 0);
	void Init(void);
	void OnOK(void);
	// virtual	void OnCommand(Window *wnd,int32_t nCommand);

	int32_t GetMin() { return min; }
	int32_t GetMax() { return max; }

	void SetMin(int32_t newMin) { min = newMin; }
	void SetMax(int32_t newMax) { max = newMax; }
	virtual BOOL OnInitDialog()
	{
		Init(void);
		return TRUE;
	}

private:
	int32_t min;
	int32_t max;
};

//*************************************************************************************************
#endif // end of file ( HeightDlg.h )
