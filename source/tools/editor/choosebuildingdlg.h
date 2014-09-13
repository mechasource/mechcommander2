/*************************************************************************************************\
ChooseBuildingDlg.h		: Interface for the ChooseBuildingDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef CHOOSEBUILDINGDLG_H
#define CHOOSEBUILDINGDLG_H

//#include <elist.h>
//#include "objective.h"
//#include "editorobjectmgr.h"

class ChooseBuildingDlg: public CDialog
{
public:
	typedef EditorObject* building_ptr_type;

	ChooseBuildingDlg(building_ptr_type& buildingPtr);

	BOOL OnCommand(WPARAM wParam, LPARAM lParam); // called by child controls to inform of an event
	void OnUsePointer(void);
	void OnCancel(void);
	void OnOK(void);

	virtual ~ChooseBuildingDlg(void);

	BOOL OnInitDialog(void);

private:
	typedef CDialog inherited;

	// suppressing these
	inline ChooseBuildingDlg(void);
	ChooseBuildingDlg& operator=(const ChooseBuildingDlg& lgBuildingPtr);

	building_ptr_type* m_pModifiedBuildingPtr;
	building_ptr_type* m_pBuildingPtr;
	CComboBox* m_pComboBox;
	EditorObjectMgr::BUILDING_LIST m_buildingList;
	CButton* m_pUsingPointerButton;
	CButton* m_pCancelButton;
	CButton* m_pOKButton;
};


//*************************************************************************************************
#endif  // end of file ( ChooseBuildingDlg.h )
