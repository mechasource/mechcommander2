#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H
/*************************************************************************************************\
MessageBox.h			: quick helper function to call up a message box
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#ifndef UTILITIES_H
#include "Utilities.h"
#endif

#include "EditorInterface.h"

#include "stdafx.h"

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
MessageBox:
**************************************************************************************************/
extern unsigned long gameResourceHandle;

inline int EMessageBox(int MessageID, int CaptionID,DWORD dwS )
{
	char buffer[512];
	char bufferCaption[512];

	cLoadString( MessageID, buffer, 512, gameResourceHandle );
	
	cLoadString( CaptionID, bufferCaption, 512, gameResourceHandle );
	
	if (EditorInterface::instance() && EditorInterface::instance()->ThisIsInitialized())
	{
		return EditorInterface::instance()->MessageBoxA( buffer, bufferCaption, dwS );
	}
	else
	{
		/*note: this messagebox will not be modal wrt the application*/
		return ::MessageBoxA( NULL, buffer, bufferCaption, dwS );
	}

}





//*************************************************************************************************
#endif  // end of file ( MessageBox.h )
