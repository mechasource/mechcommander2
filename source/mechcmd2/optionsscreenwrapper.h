/*************************************************************************************************\
OptionsScreenWrapper.h			: Interface for the OptionsScreenWrapper component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

//*************************************************************************************************

#pragma once

#ifndef OPTIONSSCREENWRAPPER_H
#define OPTIONSSCREENWRAPPER_H

//#include <mechgui/asystem.h>
//#include <mechgui/alistbox.h>


/**************************************************************************************************
CLASS DESCRIPTION
OptionsScreenWrapper:
**************************************************************************************************/

class OptionsXScreen;

class OptionsScreenWrapper
{
public:
	enum status_type
	{
		opt_INACTIVE,
		opt_ACTIVE,
		opt_DONE
	};

	OptionsScreenWrapper(void);
	~OptionsScreenWrapper(void);

	void init(void);
	void destroy(void);

	status_type update(void);
	void render(void);

	void begin(void);
	void end(void);
	bool isDone()
	{
		return (!isActive);
	}

private:

	OptionsXScreen*		pOptionsScreen;
	bool		isActive;

};





//*************************************************************************************************
#endif  // end of file ( OptionsScreenWrapper.h )
