//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef MAINMENU_H
#define MAINMENU_H

#include "wlib.h"
#include "resource.h"
#include "d3dfont.h"

class EditorInterface;

class MainMenu:public Window
{
  protected:
		bool bEndModal;
		EditorInterface *m_pEditorInterface;
	
	public:
		Menu *pMenu;

		MainMenu(EditorInterface *pEditorInterface);
		
		void DoModal();

		void OnCommand(Window *wnd, int nCommand);
};

extern MainMenu *pMainMenu;

#endif