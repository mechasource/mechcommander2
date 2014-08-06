//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// UnitSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "UnitSettingsDlg.h"
#include "EditorObjects.h"
#include "EditorObjectMgr.h"
#include "EditorInterface.h" // just for the undo manager


/////////////////////////////////////////////////////////////////////////////
// UnitSettingsDlg dialog

UnitSettingsDlg::UnitSettingsDlg( EList< Unit*, Unit* >& newList/*=NULL*/, ActionUndoMgr &undoMgr)
	: CDialog(UnitSettingsDlg::IDD), units( newList )
{
	//{{AFX_DATA_INIT(UnitSettingsDlg)
	m_Alignment = -1;
	m_SquadEdit = _T("");
	m_SelfRepairBehavior = -1;
	//}}AFX_DATA_INIT

	pUndoMgr = &undoMgr;
	pAction = NULL;
}


void UnitSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UnitSettingsDlg)
	DDX_Control(pDX, IDC_VARIANT, m_Variant);
	DDX_Control(pDX, IDC_PILOT, m_Pilot);
	DDX_Control(pDX, IDC_MECH, m_Mech);
	DDX_Control(pDX, IDC_GROUP, m_Group);
	DDX_Radio(pDX, IDC_ALIGN1, m_Alignment);
	DDX_Text(pDX, IDC_SQUAD_EDIT, m_SquadEdit);
	DDX_Radio(pDX, IDC_SELF_REPAIR1, m_SelfRepairBehavior);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UnitSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(UnitSettingsDlg)
	ON_CBN_SELCHANGE(IDC_GROUP, OnSelchangeGroup)
	ON_BN_CLICKED(IDC_HIGHILIGHT2EDIT, OnHighilight2edit)
	ON_EN_CHANGE(IDC_HIGHLIGHT1, OnChangeHighlight1)
	ON_BN_CLICKED(IDC_HIGHLIGHT1EDIT, OnHighlight1edit)
	ON_EN_CHANGE(IDC_HIGHLIGHT2, OnChangeHighlight2)
	ON_EN_CHANGE(IDC_BASE, OnChangeBase)
	ON_BN_CLICKED(IDC_BASEEDIT, OnBaseedit)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_MECH, OnSelchangeMech)
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE( BN_CLICKED, IDC_ALIGN1, IDC_ALIGN1 + 8, OnAlign1)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UnitSettingsDlg message handlers

void UnitSettingsDlg::OnSelchangeGroup() 
{
		m_Mech.ResetContent();

		int group = m_Group.GetCurSel();
		group = m_Group.GetItemData( group );

		const char* MechNames[256];
		int count = 256;
			
		EditorObjectMgr::instance()->getBuildingNamesInGroup( group, MechNames, count );

		for ( int i = 0; i < count; ++i )
		{
			m_Mech.AddString( MechNames[i] );
		}


		m_Mech.SetCurSel( 0 );	
}

void UnitSettingsDlg::OnHighilight2edit() 
{
	
	CWnd* pWnd = GetDlgItem( IDC_HIGHLIGHT2 );
	DoColorBox( pWnd );
}

void UnitSettingsDlg::OnChangeHighlight1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CString text;
	GetDlgItem( IDC_HIGHLIGHT1 )->GetWindowText( text );

	bool bChanged = false;
	int i = 0;
	
	if ( text.GetLength() > 1 && (text[0] == '0' && (text[1] == 'x' || text[i] == 'X' )) )
		i = 2;

	for ( ; i < text.GetLength(); ++i )
	{
		if ( !isxdigit( text[i] ) )
		{
			text.Remove( text[i] );
			bChanged = true;
		}
	}

	if ( bChanged )
		GetDlgItem( IDC_HIGHLIGHT1 )->SetWindowText( text );

	GetDlgItem( IDC_HIGHLIGHT1 )->RedrawWindow( );
	
}

void UnitSettingsDlg::OnHighlight1edit() 
{
	
	CWnd* pWnd = GetDlgItem( IDC_HIGHLIGHT1 );
	DoColorBox( pWnd );
}

void UnitSettingsDlg::OnChangeHighlight2() 
{
	CString text;
	GetDlgItem( IDC_HIGHLIGHT2 )->GetWindowText( text );

	bool bChanged = false;
	int i = 0;
	
	if ( text.GetLength() > 1 && (text[0] == '0' && (text[1] == 'x' || text[i] == 'X' )) )
		i = 2;

	for ( ; i < text.GetLength(); ++i )
	{
		if ( !isxdigit( text[i] ) )
		{
			text.Remove( text[i] );
			bChanged = true;
		}
	}

	if ( bChanged )
		GetDlgItem( IDC_HIGHLIGHT2 )->SetWindowText( text );

	GetDlgItem( IDC_HIGHLIGHT2 )->RedrawWindow( );
	
}

void UnitSettingsDlg::OnChangeBase() 
{
	CString text;
	GetDlgItem( IDC_BASE )->GetWindowText( text );

	bool bChanged = false;
	int i = 0;
	
	if ( text.GetLength() > 1 && (text[0] == '0' && (text[1] == 'x' || text[i] == 'X' )) )
		i = 2;

	for ( ; i < text.GetLength(); ++i )
	{
		if ( !isxdigit( text[i] ) )
		{
			text.Remove( text[i] );
			bChanged = true;
		}
	}

	if ( bChanged )
		GetDlgItem( IDC_BASE )->SetWindowText( text );

	GetDlgItem( IDC_BASE )->RedrawWindow( );
		
}

void UnitSettingsDlg::OnBaseedit() 
{
	CWnd* pWnd = GetDlgItem( IDC_BASE );
	DoColorBox( pWnd );

}

void UnitSettingsDlg::DoColorBox( CWnd* pWnd )
{
	if ( pWnd )
	{
		CString tmpStr;
		pWnd->GetWindowText( tmpStr );

		tmpStr.Replace( "0x", "" );

		long base;
		sscanf( tmpStr, "%x", &base );
		base &= 0x00ffffff;


		CColorDialog dlg( reverseRGB(base), NULL, this );
		if (IDOK == dlg.DoModal() )
		{
			base = reverseRGB( dlg.GetColor() );

			tmpStr.Format( "0x%x", base );
			pWnd->SetWindowText( tmpStr );
		}
	}
}

void UnitSettingsDlg::applyChanges()
{
	// get the type info from the dlg box
	int index = m_Group.GetCurSel( );
	if ( index != -1 )
	{
		int group = m_Group.GetItemData( index );

		int indexInGroup = m_Mech.GetCurSel( );

		if ( indexInGroup != -1 )
		{
			for ( UNIT_LIST::EIterator iter = units.Begin(); !iter.IsDone(); iter++ )
			{
				(*iter)->setAppearance( group, indexInGroup );
			}

			int variant = m_Variant.GetCurSel();
			if ( variant != -1 )
			{
				for ( iter = units.Begin(); !iter.IsDone(); iter++ )
				{
					(*iter)->setVariant( variant );
				}
			}

		}
	}

	//set pilots 
	index = m_Pilot.GetCurSel();

	if ( index != -1 )
	{
		for ( UNIT_LIST::EIterator iter = units.Begin(); !iter.IsDone(); iter++ )
		{
			CString txt;
			m_Pilot.GetLBText( index, txt );
			(*iter)->getPilot()->setName( txt );
		}
	}

	// now set the alignment
	UpdateData( true );
	index = m_Alignment;

	if ( index != -1 )
	{
		for ( UNIT_LIST::EIterator iter = units.Begin(); !iter.IsDone(); iter++ )
		{
			(*iter)->setAlignment( index );
		}
	}

	if ( !(m_SquadEdit.IsEmpty()) )
	{
		index = _ttol(m_SquadEdit.GetBuffer(0));

		if (1 != units.Count())
		{
			for ( UNIT_LIST::EIterator iter = units.Begin(); !iter.IsDone(); iter++ )
			{
				(*iter)->setSquad( index );
			}
		}
		else
		{
			pFirstPossibility->setSquad( index );
		}
	}

	bool bSelfRepairBehavior = true;
	if (0 != m_SelfRepairBehavior)
	{
		bSelfRepairBehavior = false;
	}
	for ( UNIT_LIST::EIterator iter = units.Begin(); !iter.IsDone(); iter++ )
	{
		(*iter)->setSelfRepairBehaviorEnabled(bSelfRepairBehavior);
	}

	unsigned long base=0, color1=0, color2=0;
	bool bBase = false;
	bool bColor1 = false; 
	bool bColor2 = false;

	// now figure out the colors
	CWnd* pWnd = GetDlgItem( IDC_BASE );
	if ( pWnd )
	{
		CString tmpStr;
		pWnd->GetWindowText( tmpStr );

		if ( tmpStr.GetLength() )
		{
			bBase = true;
		

			tmpStr.Replace( "0x", "" );
			sscanf( tmpStr, "%x", &base );
			base |= 0xff000000;
		}
		
	}

	pWnd = GetDlgItem( IDC_HIGHLIGHT1 );
	if ( pWnd )
	{
		CString tmpStr;
		pWnd->GetWindowText( tmpStr );

		if ( tmpStr.GetLength() )
		{
			bColor1 = true;
		

			tmpStr.Replace( "0x", "" );
			sscanf( tmpStr, "%x", &color1 );
			color1 |= 0xff000000;
		}
		
	}
	pWnd = GetDlgItem( IDC_HIGHLIGHT2 );
	if ( pWnd )
	{
		CString tmpStr;
		pWnd->GetWindowText( tmpStr );

		if ( tmpStr.GetLength() )
		{
			bColor2 = true;
		

			tmpStr.Replace( "0x", "" );
			sscanf( tmpStr, "%x", &color2 );
			color2 |= 0xff000000;
		}
		
	}

	if ( bBase && bColor1 && bColor2 )
	{
		for ( UNIT_LIST::EIterator iter = units.Begin(); !iter.IsDone(); iter++ )
		{
			(*iter)->setColors( base, color1, color2 );
		}

	}
}

void UnitSettingsDlg::OnOK() 
{
	if (NULL != pUndoMgr)
	{
		pUndoMgr->AddAction(pAction);
	}
	else
	{
		delete pAction;
	}
	pAction = NULL;

	applyChanges();
	CDialog::OnOK();
}

BOOL UnitSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	pAction = new ModifyBuildingAction;

	UNIT_LIST::EIterator iter;

	for ( iter = units.Begin(); !iter.IsDone(); iter++ )
	{
		pAction->addBuildingInfo(*(*iter));
	}

	updateMemberVariables();

	if (1 != units.Count())
	{
		pFirstPossibility = NULL;
	}
	else
	{
		pFirstPossibility = (*(units.Begin()));
	}
	updatePossibiltyControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void UnitSettingsDlg::updatePossibiltyControls()
{
	updateMemberVariables();
}

HBRUSH UnitSettingsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if ( GetDlgItem( IDC_BASE )->m_hWnd == pWnd->m_hWnd )
	{
		CString tmpStr;
		pWnd->GetWindowText( tmpStr );

		tmpStr.Replace( "0x", "" );

		long base;
		sscanf( tmpStr, "%x", &base );
		base &= 0x00ffffff;
		base = reverseRGB( base );
		
		if ( baseBrush.m_hObject )
			baseBrush.DeleteObject();

		baseBrush.CreateSolidBrush( base );
		pDC->SetBkColor( base );

		
		if ( ((base & 0xff) + ( (base & 0xff00)>>8 ) + ( (base & 0xff0000)>>16 ))/3 < 85 )
			pDC->SetTextColor( 0x00ffffff );

		return (HBRUSH)baseBrush.m_hObject;
	}

	if ( GetDlgItem( IDC_HIGHLIGHT1 )->m_hWnd == pWnd->m_hWnd )
	{
		CString tmpStr;
		pWnd->GetWindowText( tmpStr );

		tmpStr.Replace( "0x", "" );

		long base;
		sscanf( tmpStr, "%x", &base );
		base &= 0x00ffffff;
		base = reverseRGB( base );

		if ( brush1.m_hObject )
			brush1.DeleteObject();

		brush1.CreateSolidBrush( base );
		pDC->SetBkColor( base );

		if ( ((base & 0xff) + ( (base & 0xff00)>>8 ) + ( (base & 0xff0000)>>16 ))/3 < 85 )
			pDC->SetTextColor( 0x00ffffff );
		return (HBRUSH)brush1.m_hObject;
	}

	if ( GetDlgItem( IDC_HIGHLIGHT2 )->m_hWnd == pWnd->m_hWnd )
	{
		CString tmpStr;
		pWnd->GetWindowText( tmpStr );
		
		tmpStr.Replace( "0x", "" );

		long base;
		sscanf( tmpStr, "%x", &base );
		base &= 0x00ffffff;
		base = reverseRGB( base );

		
		if ( brush2.m_hObject )
			brush2.DeleteObject();

		brush2.CreateSolidBrush( base );
		pDC->SetBkColor( base );
		
		if ( ((base & 0xff) + ( (base & 0xff00)>>8 ) + ( (base & 0xff0000)>>16 ))/3 < 85 )
			pDC->SetTextColor( 0x00ffffff );

		return (HBRUSH)brush2.m_hObject;
	}

		
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void UnitSettingsDlg::OnSelchangeMech() 
{
	m_Variant.ResetContent();
	
	int group = m_Group.GetCurSel();
	group = m_Group.GetItemData( group );

	int indexInGroup = m_Mech.GetCurSel();

	int varCount =  EditorObjectMgr::instance()->getNumberOfVariants( group, indexInGroup );
	const char** VariantNames = 0;
	if (0 < varCount)
	{
		VariantNames = new const char*[varCount];
		EditorObjectMgr::instance()->getVariantNames( group, indexInGroup, VariantNames, varCount );

		for ( int v = 0; v < varCount; ++v )
		{
			m_Variant.AddString( VariantNames[v] );
		}

		delete VariantNames;
		VariantNames = 0;

		m_Variant.SetCurSel( 0 );
	}
}

int UnitSettingsDlg::getPossibilityIndex()
{
	if (!pFirstPossibility) { gosASSERT(false); return -1; }

	const Unit * pUnit = (*(units.Begin()));
	if (pUnit == pFirstPossibility)
	{
		return 0;
	}
	int i;
	for (i = 0; i < (int)pFirstPossibility->pAlternativeInstances->Count(); i++)
	{
		if (pUnit == &(*(pFirstPossibility->pAlternativeInstances->Iterator(i))))
		{
			return i + 1;
		}
	}
	return -1;
}


void UnitSettingsDlg::updateMemberVariables()
{
	// now need to check all of the colors
	unsigned long tmpBase, tmpHighlight1, tmpHighlight2;
	unsigned long base, highlight1, highlight2;
	bool	bBase = true;
	bool	bHighlight = true;
	bool	bHighlight2 = true;


	Unit* pUnit = units.GetHead();

	m_Alignment = pUnit->getAlignment();
	for ( UNIT_LIST::EIterator iter = units.Begin(); !iter.IsDone(); iter++ )
	{
		if ( (*iter)->getAlignment() != m_Alignment )
		{
			m_Alignment = -1;
			break;
		}
	}

	m_SelfRepairBehavior = 0;
	if (true != pUnit->getSelfRepairBehaviorEnabled())
	{
		m_SelfRepairBehavior = 1;
	}
	for ( iter = units.Begin(); !iter.IsDone(); iter++ )
	{
		if ( (*iter)->getSelfRepairBehaviorEnabled() != pUnit->getSelfRepairBehaviorEnabled() )
		{
			m_SelfRepairBehavior = -1;
			break;
		}
	}

	unsigned long tmpSquadNum = pUnit->getSquad();
	m_SquadEdit.Format("%lu", tmpSquadNum);

	for ( iter = units.Begin(); !iter.IsDone(); iter++ )
	{
		if ( (*iter)->getSquad() != tmpSquadNum )
		{
			m_SquadEdit.Empty();
			break;
		}
	}

	UpdateData( false );	
	
	pUnit->getColors( base, highlight1, highlight2 );
		char	pBase[256];
	char	pH1[256];
	char	pH2[256];

	for ( iter = units.Begin(); !iter.IsDone(); iter++ )
	{
		(*iter)->getColors( tmpBase, tmpHighlight1, tmpHighlight2 );

		if ( tmpBase != base )
			bBase = false;
	
		if ( tmpHighlight1 != highlight1 )
			bHighlight = false;
	
		if ( tmpHighlight2 != highlight2 )
			bHighlight2 = false;
	}

	base &= 0x00ffffff;
	highlight1 &= 0x00ffffff;
	highlight2 &= 0x00ffffff;

	sprintf( pBase, "0x%6x", base );
	sprintf( pH1, "0x%6x", highlight1 );
	sprintf( pH2, "0x%6x", highlight2 );	

	if ( bBase )
	{
		GetDlgItem( IDC_BASE )->SetWindowText( pBase );
	}

	if ( bHighlight )
		GetDlgItem( IDC_HIGHLIGHT1 )->SetWindowText( pH1 );
	
	if ( bHighlight2 )
		GetDlgItem( IDC_HIGHLIGHT2 )->SetWindowText( pH2 );

	EditorObjectMgr* pMgr = EditorObjectMgr::instance();

	int groupCount = pMgr->getUnitGroupCount();

	const char** pGroups = new const char*[groupCount];
	int*		 groupIDs = new int[groupCount];

	
	m_Group.ResetContent();
	
	pMgr->getUnitGroupNames(pGroups, groupIDs, groupCount);

	for ( int i = 0; i < groupCount; ++i )
	{
		m_Group.AddString( pGroups[i] );
		m_Group.SetItemData( i, groupIDs[i] );
	}

	delete [] pGroups;
	delete [] groupIDs;

	// make sure all the units we are editing are in the same group
	int group = units.GetHead()->getGroup();	
	for ( iter = units.Begin(); !iter.IsDone(); iter++ )
	{
		if ( (*iter)->getGroup() != group )
		{
			group = -1;
			break;
		}
	}

	if ( group != -1 ) // we found a valid group
	{
		const char* pGroupName = pMgr->getGroupName( group );
		
		int index = m_Group.FindString( -1, pGroupName );
		m_Group.SetCurSel( index );

		// OK, now fill in the index....
		const char* MechNames[256];
		int count = 256;

		m_Mech.ResetContent();
		pMgr->getBuildingNamesInGroup( group, MechNames, count );

		for ( int i = 0; i < count; ++i )
		{
			m_Mech.AddString( MechNames[i] );
		}

		// ok, now determine if all of the mechs are the same.
		int indexInGroup = units.GetHead()->getIndexInGroup();

		for ( iter = units.Begin(); !iter.IsDone(); iter++ )
		{
			if ( (*iter)->getIndexInGroup() != indexInGroup )
			{
				indexInGroup = -1;
				break;
			}
		}

		if ( indexInGroup != -1 )
		{
			const char* pName = units.GetHead()->getDisplayName();
			index = m_Mech.FindString( -1, pName );

			if ( index != -1 )
			{
				m_Mech.SetCurSel( index );

				// now we need to find the variant

				m_Variant.ResetContent();
				
				int varCount =  EditorObjectMgr::instance()->getNumberOfVariants( group, indexInGroup );
				const char** VariantNames = 0;
				if (0 < varCount)
				{
					VariantNames = new const char*[varCount];
					EditorObjectMgr::instance()->getVariantNames( group, indexInGroup, VariantNames, varCount );

					for ( int v = 0; v < varCount; ++v )
					{
						m_Variant.AddString( VariantNames[v] );
					}

					delete VariantNames;
					VariantNames = 0;

					// OK, now see if they all have the same variant name
					int variant = units.GetHead()->getVariant();	
					for ( iter = units.Begin(); !iter.IsDone(); iter++ )
					{
						if ( (*iter)->getVariant() != variant )
						{
							variant = -1;
							break;
						}
					}

					if ( variant != -1 )
						m_Variant.SetCurSel( variant );
				}
			}
		}
	}


	Pilot::PilotInfo* pInfo = Pilot::s_BadPilots;
	long* count = &Pilot::badCount;
	if ( (m_Alignment == 0) || (!EditorData::instance->IsSinglePlayer()) )
	{
		pInfo = Pilot::s_GoodPilots;
		count = &Pilot::goodCount;
	}
	else if ( m_Alignment == -1 )
		pInfo = 0;

	if ( pInfo )
	{
		m_Pilot.ResetContent();
		for ( int i = 0; i < *count; i++ )
		{
			m_Pilot.AddString( pInfo[i].name );
			m_Pilot.SetItemDataPtr(i, (void *)pInfo[i].fileName);
		}
	}


	Pilot* pPilot = pUnit->getPilot();

	const char* defaultPilot = pPilot->info->fileName;

	for ( iter = units.Begin(); !iter.IsDone(); iter++ )
	{
		pPilot = (*iter)->getPilot();

		const char* tmpName = pPilot->info->fileName;

		if ( stricmp( tmpName, defaultPilot ) != 0 )
		{
			defaultPilot = 0;
			break;
		}
	}

	
	if ( defaultPilot )
	{
		int index;
		for (index = 0; index < m_Pilot.GetCount(); index++)
		{
			char *fileName = (char *)m_Pilot.GetItemDataPtr(index);
			if (0 == strcmp(fileName, defaultPilot))
			{
				break;
			}
		}
		if (!(index < m_Pilot.GetCount()))
		{
			index = -1;
		}
		m_Pilot.SetCurSel( index );
	}
}

void UnitSettingsDlg::OnCancel() 
{
	pAction->undo();
	delete pAction;
	pAction = NULL;
	
	CDialog::OnCancel();
}

void UnitSettingsDlg::OnAlign1( UINT whichID ) 
{
	UpdateData();

	Pilot::PilotInfo* pInfo = Pilot::s_BadPilots;
	long* count = &Pilot::badCount;
	if ( m_Alignment == 0 )
	{
		pInfo = Pilot::s_GoodPilots;
		count = &Pilot::goodCount;
	}
	else if ( m_Alignment == -1 )
		pInfo = 0;

	if ( pInfo )
	{
		m_Pilot.ResetContent();
		for ( int i = 0; i < *count; i++ )
		{
			m_Pilot.AddString( pInfo[i].name );
		}
	}

	Unit * pUnit = (*(units.Begin()));
	Pilot* pPilot = pUnit->getPilot();

	const char* defaultPilot = pPilot->getName();

	for ( UNIT_LIST::EIterator iter = units.Begin(); !iter.IsDone(); iter++ )
	{
		pPilot = (*iter)->getPilot();

		const char* tmpName = pPilot->getName();

		if ( stricmp( tmpName, defaultPilot ) != 0 )
		{
			defaultPilot = 0;
			break;
		}
	}

	
	if ( defaultPilot )
	{
		int  index = m_Pilot.FindString( -1, defaultPilot );
		m_Pilot.SetCurSel( index );
	}

	
}
