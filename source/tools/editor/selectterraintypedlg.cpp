//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// SelectTerrainTypeDlg.cpp : implementation file
//

#include "stdinc.h"
#include "resource.h"
#include "SelectTerrainTypeDlg.h"

#include "utilities.h"
#include "TerrTxm.h"
#include "Terrain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static wchar_t THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SelectTerrainTypeDlg dialog

SelectTerrainTypeDlg::SelectTerrainTypeDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(SelectTerrainTypeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SelectTerrainTypeDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void SelectTerrainTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelectTerrainTypeDlg)
	DDX_Control(pDX, IDC_SELECT_TERRAIN_TYPE_LIST, m_terrainTypeList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SelectTerrainTypeDlg, CDialog)
//{{AFX_MSG_MAP(SelectTerrainTypeDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelectTerrainTypeDlg message handlers

BOOL SelectTerrainTypeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_terrainTypeList.ResetContent();
	int32_t numTerrains = 0;
	numTerrains = land->terrainTextures->getNumTypes();
	int32_t i;
	for (i = 0; i < numTerrains; i++)
	{
		if (land->terrainTextures->getTextureNameID(i)) // if we start to repeat, quit
		{
			wchar_t buffer[256];
			cLoadString(land->terrainTextures->getTextureNameID(i), buffer, 256);
			CString newStr(buffer);
			int32_t count = m_terrainTypeList.GetCount();
			bool bPlaced = 0;
			for (size_t j = 0; j < count; ++j)
			{
				CString tmp;
				m_terrainTypeList.GetText(j, tmp);
				if (tmp > newStr)
				{
					m_terrainTypeList.InsertString(j, newStr);
					m_terrainTypeList.SetItemData(j, ID_TERRAINS_BLUEWATER + i);
					bPlaced = 1;
					break;
				}
			}
			if (!bPlaced)
			{
				int32_t j = m_terrainTypeList.GetCount();
				m_terrainTypeList.InsertString(j, newStr);
				m_terrainTypeList.SetItemData(j, ID_TERRAINS_BLUEWATER + i);
			}
		}
	}
	if (ID_TERRAINS_BLUEWATER <= SelectedTerrainType())
	{
		int32_t k;
		for (k = 0; k < m_terrainTypeList.GetCount(); k++)
		{
			if (SelectedTerrainType() == m_terrainTypeList.GetItemData(k))
			{
				m_terrainTypeList.SetCurSel(k);
				break;
			}
		}
	}
	SelectedTerrainType(-1);
	// TODO: Add extra initialization here
	return TRUE; // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
}

void SelectTerrainTypeDlg::OnOK()
{
	int32_t curSelIndex = m_terrainTypeList.GetCurSel();
	if (LB_ERR != curSelIndex)
	{
		int32_t terrainTypeID = m_terrainTypeList.GetItemData(curSelIndex);
		SelectedTerrainType(terrainTypeID);
	}
	else
	{
		SelectedTerrainType(-1);
	}
	// TODO: Add extra validation here
	CDialog::OnOK();
}
