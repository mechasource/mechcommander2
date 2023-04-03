/*************************************************************************************************\
OptionsScreenWrapper.cpp			: Implementation of the OptionsScreenWrapper
component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#define OPTIONSSCREENWRAPPER_CPP

#include "stdinc.h"
#include "optionsscreenwrapper.h"
#include "mclib.h"
#include "optionsarea.h"

OptionsScreenWrapper::OptionsScreenWrapper()
{
	pOptionsScreen = 0;
	isActive = false;
}

OptionsScreenWrapper::~OptionsScreenWrapper()
{
	if (pOptionsScreen && isActive)
	{
		end();
	}
}

void OptionsScreenWrapper::begin()
{
	isActive = true;
	if (!pOptionsScreen)
	{
		pOptionsScreen = new OptionsXScreen();
		wchar_t path[256];
		strcpy(path, artPath);
		strcat(path, "mcl_options.fit");
		FitIniFile file;
		if (NO_ERROR != file.open(path))
		{
			wchar_t error[256];
			sprintf(error, "couldn't open file %s", path);
			Assert(0, 0, error);
			return;
		}
		pOptionsScreen->init(&file);
	}
}

void OptionsScreenWrapper::init()
{
}

void OptionsScreenWrapper::destroy()
{
}

void OptionsScreenWrapper::end()
{
	delete pOptionsScreen;
	pOptionsScreen = nullptr;
	isActive = false;
}

OptionsScreenWrapper::status_type
OptionsScreenWrapper::update()
{
	if (pOptionsScreen && isActive)
	{
		userInput->setMouseCursor(mState_NORMAL);
		pOptionsScreen->update();
		if (pOptionsScreen->isDone())
		{
			pOptionsScreen->updateOptions();
			isActive = 0;
			return opt_DONE;
		}
		else
		{
			return opt_ACTIVE;
		}
	}
	else
	{
		return opt_INACTIVE;
	}
}

void OptionsScreenWrapper::render()
{
	if (pOptionsScreen && isActive)
		pOptionsScreen->render();
}
