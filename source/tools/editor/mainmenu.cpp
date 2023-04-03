//===========================================================================//
// File:	 mainmenu.cpp 												     //
// Contents: Main Menu for Links                                             //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "pch.h"
#include "wlib.h"
#include "mainmenu.h"
//#include "resource.h"
#include <_ASSERT.h>

#include "editorinterface.h"

MainMenu* pMainMenu = nullptr;
MainMenu::MainMenu(EditorInterface* pEditorInterface)
	: Window(L"MainMenu", 0, 0, Environment.screenwidth, Environment.screenheight,
		pEditorInterface /*pgWinMan->Root()*/, 0, 0)
{
	pMenu = nullptr;
	bEndModal = false;
	pMenu = new Menu(this, 0, 0, 0, 0, IDR_EDITOR_MENU, pgRes);
	WindowRect(pMenu->WindowRect());
	Show();
	pMainMenu = this;
	m_pEditorInterface = pEditorInterface;
	_ASSERT(m_pEditorInterface);
	/*
	uint32_t dwCheck =
	gos_NewTextureFromFile(gos_Texture_Alpha,"data/wlib/check.tga",gosHint_VideoMemory|gosHint_DisableMipmap);
	CHECKMARK_APPEARANCE ca;
	ca.dwTexture = dwCheck;
	ca.fSize = 1.0;
	ca.iPixelSize = 16;
	ca.ptCheck.x = .0;
	ca.ptCheck.y = .0;

	pgWinMan->SetCheckMarkAppearance( ca );
	*/
}

extern bool
gos_RunMainLoop(void (*DoGameLogic)());
extern uint32_t TerminateGame; // BUGBUG exported function handles this with return value

void MainMenu::DoModal()
{
	while (!TerminateGame && !bEndModal)
	{
		gos_RunMainLoop(nullptr);
		pgWinMan->Dispatch();
	}
}

void MainMenu::OnCommand(Window* wnd, int32_t nCommand)
{
	if (MC_CLICKED == nCommand)
	{
		/* Set focus to parent window (EditorInterface). This is not technically
		correct. Technically, focus should be set to the window that previously
		had it. But at the moment we know that EditorInterface previously had
		it. Also, I think perhaps the Menu class should automatically restore
		focus appropriately. I could be wrong. */
		if ((pMenu == pgWinMan->InFocus()) || (this == pgWinMan->InFocus()))
		{
			if (Parent() == 0)
				pgWinMan->SetFocus(0);
			else
			{
				if (!Parent()->SetFocus())
					pgWinMan->SetFocus(0);
			}
		}
		m_pEditorInterface->handleNewMenuMessage(wnd->GetID());
		bEndModal = true;
	}
}