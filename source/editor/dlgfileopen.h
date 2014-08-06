#ifndef DLGFILEOPEN_H
#define DLGFILEOPEN_H
/*************************************************************************************************\
DlgFileOpen.h		: Interface for the DlgFileOpen component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "resource.h"
#include "EditorMessages.h"		// only for the definition of IDS_OK and IDS_CANCEL
#include "stdafx.h"

/**************************************************************************************************
CLASS DESCRIPTION  this is a fileOpenDlg ...
DlgFileOpen:
**************************************************************************************************/

class DlgFileOpen: public CDialog
{
	public:

		DlgFileOpen( const char* directoryName, const char* extension,  bool bSave );
		DlgFileOpen( const DlgFileOpen& lgFileOpen );

		void OnCancel();
		void OnOK();
		//virtual void handleMessage (GObject *from, long messageId, long specificMsg);
		
		virtual ~DlgFileOpen();

		void Init(void);

		const char* getFileName() const { return fileName; }
		const char* getExtension() const { return extension; }

	private:

		// HELPER FUNCTIONS
		void init(); // I think I'll hard wire the file name here


		//{{AFX_DATA(DlgFileOpen)
		enum { IDD = IDD_FILEOPEN };
		//}}AFX_DATA

		//{{AFX_VIRTUAL(DlgFileOpen)
		//}}AFX_VIRTUAL

	protected:
		//{{AFX_MSG(DlgFileOpen)
		virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFileopenFilelist();
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()


		// suppressing these
		inline DlgFileOpen();
		DlgFileOpen& operator=( const DlgFileOpen& lgFileOpen );

		char fileName[MAX_PATH];
		char extension[8];

		char m_directory[MAX_PATH];
		bool m_bSave;

};


//*************************************************************************************************
#endif  // end of file ( DlgFileOpen.h )
