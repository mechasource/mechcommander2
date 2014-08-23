/*************************************************************************************************\
aAnimObject.h			: Interface for the aAnimObject component of the GUI library.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef AANIMOBJECT_H
#define AANIMOBJECT_H

#include <mechgui/asystem.h>
#include <mechgui/aanim.h>

//*************************************************************************************************

namespace mechgui {

/**************************************************************************************************
CLASS DESCRIPTION
aAnimObject:
**************************************************************************************************/
class aAnimObject: public aObject
{
	public:

		aAnimObject();
		virtual ~aAnimObject();
		aAnimObject& operator=( const aAnimObject& AnimObject );


		int init( FitIniFile* file, PCSTR blockName, ULONG neverFlush = 0);

		virtual void update();
		virtual void render();

		void begin() { animInfo.begin(); }
		void end();
		void reverseBegin() { animInfo.reverseBegin(); }

		bool isDone() { return animInfo.isDone(); }

		aAnimation		animInfo;


	private:
	
		aAnimObject( const aAnimObject& src );	

};


//*************************************************************************************************
#endif  // end of file ( aAnimObject.h )
