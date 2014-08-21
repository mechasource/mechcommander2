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

//#include "asystem.h"
//#include "alistbox.h"


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

		OptionsScreenWrapper();
		~OptionsScreenWrapper();

		void init();
		void destroy();

		status_type update();
		void render();

		void begin();
		void end();
		bool isDone() { return (!isActive); }

	private:

		OptionsXScreen*		pOptionsScreen;
		bool		isActive;

};





//*************************************************************************************************
#endif  // end of file ( OptionsScreenWrapper.h )
