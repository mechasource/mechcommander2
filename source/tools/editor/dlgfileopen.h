/*************************************************************************************************\
DlgFileOpen.h		: Interface for the DlgFileOpen component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef DLGFILEOPEN_H
#define DLGFILEOPEN_H

//#include "editormessages.h"		// only for the definition of ids_ok and ids_cancel

/**************************************************************************************************
CLASS DESCRIPTION  this is a fileOpenDlg ...
DlgFileOpen:
**************************************************************************************************/

class DlgFileOpen: public CDialog
{
	public:

		DlgFileOpen( PCSTR directoryName, PCSTR extension,  bool bSave );
		DlgFileOpen( const DlgFileOpen& lgFileOpen );

		void OnCancel(void);
		void OnOK(void);
		//virtual void handleMessage (GObject *from, int32_t messageId, int32_t specificMsg);
		
		virtual ~DlgFileOpen(void);

		void Init(void);

		PCSTR getFileName(void) const { return fileName; }
		PCSTR getExtension(void) const { return extension; }

	private:

		// HELPER FUNCTIONS
		void init(void); // I think I'll hard wire the file name here


		//{{AFX_DATA(DlgFileOpen)
		enum { IDD = IDD_FILEOPEN };
		//}}AFX_DATA

		//{{AFX_VIRTUAL(DlgFileOpen)
		//}}AFX_VIRTUAL

	protected:
		//{{AFX_MSG(DlgFileOpen)
		virtual BOOL OnInitDialog(void);
	afx_msg void OnSelchangeFileopenFilelist(void);
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()


		// suppressing these
		inline DlgFileOpen(void);
		DlgFileOpen& operator=( const DlgFileOpen& lgFileOpen );

		char fileName[MAX_PATH];
		char extension[8];

		char m_directory[MAX_PATH];
		bool m_bSave;

};


//*************************************************************************************************
#endif  // end of file ( DlgFileOpen.h )
