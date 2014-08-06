#ifndef CHOOSEBUILDINGDLG_H
#define CHOOSEBUILDINGDLG_H
/*************************************************************************************************\
ChooseBuildingDlg.h		: Interface for the ChooseBuildingDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "resource.h"
#include <elist.h>
#include "Objective.h"
#include "EditorObjectMgr.h"
#include "stdafx.h"


class ChooseBuildingDlg: public CDialog
{
public:
	typedef EditorObject *building_ptr_type;

	ChooseBuildingDlg( building_ptr_type &buildingPtr );

	BOOL OnCommand(WPARAM wParam, LPARAM lParam); // called by child controls to inform of an event
	void OnUsePointer();
	void OnCancel();
	void OnOK();

	virtual ~ChooseBuildingDlg();

	BOOL OnInitDialog();

private:
	typedef CDialog inherited;

	// suppressing these
	inline ChooseBuildingDlg();
	ChooseBuildingDlg& operator=( const ChooseBuildingDlg& lgBuildingPtr );

	building_ptr_type *m_pModifiedBuildingPtr;
	building_ptr_type *m_pBuildingPtr;
	CComboBox *m_pComboBox;
	EditorObjectMgr::BUILDING_LIST m_buildingList;
	CButton *m_pUsingPointerButton;
	CButton *m_pCancelButton;
	CButton *m_pOKButton;
};


//*************************************************************************************************
#endif  // end of file ( ChooseBuildingDlg.h )
