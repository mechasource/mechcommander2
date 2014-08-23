/*************************************************************************************************\
LogisticsMechDisplay.h			: Interface for the LogisticsMechDisplay component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef LOGISTICSMECHDISPLAY_H
#define LOGISTICSMECHDISPLAY_H

//#include <mechgui/logisticsscreen.h>
//#include "simplecamera.h"
//#include "attributemeter.h"
//#include "simplecomponentlistbox.h"

class LogisticsMech;

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
LogisticsMechDisplay:
**************************************************************************************************/
class LogisticsMechDisplay : public LogisticsScreen
{
	public:

		LogisticsMechDisplay();

		virtual ~LogisticsMechDisplay();

		virtual void		update();
		virtual void		render(int xOffset, int yOffset);
		int					init( );
		void				setMech( LogisticsMech* pMech, bool bFromLB = 0 );

private:
		LogisticsMech*		pCurMech;
		AttributeMeter		attributeMeters[3];
		SimpleCamera		mechCamera;
		ComponentListBox	componentListBox;


};


//*************************************************************************************************
#endif  // end of file ( LogisticsMechDisplay.h )
