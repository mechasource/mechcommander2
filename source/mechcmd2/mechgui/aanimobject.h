/*************************************************************************************************\
aAnimObject.h			: Interface for the aAnimObject component of the GUI library.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef AANIMOBJECT_H
#define AANIMOBJECT_H

//#include <mechgui/asystem.h>
//#include <mechgui/aanim.h>

//*************************************************************************************************

namespace mechgui
{

	/**************************************************************************************************
	CLASS DESCRIPTION
	aAnimObject:
	**************************************************************************************************/
	class aAnimObject: public aObject
	{
	public:

		aAnimObject(void);
		virtual ~aAnimObject(void);
		aAnimObject& operator=(const aAnimObject& AnimObject);


		int32_t init(FitIniFile* file, PCSTR blockName, uint32_t neverFlush = 0);

		virtual void update(void);
		virtual void render(void);

		void begin()
		{
			animInfo.begin(void);
		}
		void end(void);
		void reverseBegin()
		{
			animInfo.reverseBegin(void);
		}

		bool isDone()
		{
			return animInfo.isDone(void);
		}

		aAnimation		animInfo;


	private:

		aAnimObject(const aAnimObject& src);

	};


//*************************************************************************************************
#endif  // end of file ( aAnimObject.h )
