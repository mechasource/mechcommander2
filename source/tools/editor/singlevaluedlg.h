/*************************************************************************************************\
heightDlg.h			: Interface for the heightDlg component.
//-------------------------------------------------------------------------------------------------
Notes				: lets the user enter a minimum and maximum height for the
map
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef SINGLEVALDLG_H
#define SINGLEVALDLG_H

/**************************************************************************************************
CLASS DESCRIPTION
heightDlg:
**************************************************************************************************/
class SingleValueDlg : public CDialog
{
public:
	SingleValueDlg(int32_t captionID, int32_t staticTextID, int32_t val = 0);
	virtual ~SingleValueDlg(void);
	virtual void Init(void);
	virtual void OnOK(void);
	//		virtual	void OnCommand(Window *wnd,int32_t nCommand);

	int32_t GetVal()
	{
		return val;
	}

	void SetVal(int32_t newVal)
	{
		val = newVal;
	}
	void Update(void);

	virtual BOOL OnInitDialog()
	{
		Init(void);
		return TRUE;
	}

private:
	wchar_t staticText[256];
	wchar_t caption[256];

	int32_t val;
};

#endif // end of file ( heightDlg.h )
