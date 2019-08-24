/*************************************************************************************************\
MessageBox.h			: quick helper function to call up a message box
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

//#include "utilities.h"
//#include "editorinterface.h"


/**************************************************************************************************
CLASS DESCRIPTION
MessageBox:
**************************************************************************************************/
extern uint32_t gameResourceHandle;

inline int32_t
EMessageBox(int32_t MessageID, int32_t CaptionID, uint32_t dwS)
{
	char buffer[512];
	char bufferCaption[512];
	cLoadString(MessageID, buffer, 512, gameResourceHandle);
	cLoadString(CaptionID, bufferCaption, 512, gameResourceHandle);
	if (EditorInterface::instance() && EditorInterface::instance()->ThisIsInitialized())
	{
		return EditorInterface::instance()->MessageBoxA(buffer, bufferCaption, dwS);
	}
	else
	{
		/*note: this messagebox will not be modal wrt the application*/
		return ::MessageBoxA(nullptr, buffer, bufferCaption, dwS);
	}
}

#endif // end of file ( MessageBox.h )
