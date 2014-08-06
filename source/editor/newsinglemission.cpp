//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// newsinglemission.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "newsinglemission.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NewSingleMission dialog


NewSingleMission::NewSingleMission(CWnd* pParent /*=NULL*/)
	: CDialog(NewSingleMission::IDD, pParent)
{
	//{{AFX_DATA_INIT(NewSingleMission)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void NewSingleMission::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NewSingleMission)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NewSingleMission, CDialog)
	//{{AFX_MSG_MAP(NewSingleMission)
	ON_BN_CLICKED(ID_LOAD_MISSION, OnLoadMission)
	ON_BN_CLICKED(ID_NEWMISSION, OnNewmission)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NewSingleMission message handlers

void NewSingleMission::OnLoadMission() 
{
	EndDialog( ID_LOAD_MISSION );	
}

void NewSingleMission::OnNewmission() 
{
	EndDialog( ID_NEWMISSION );
	
}

void NewSingleMission::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
