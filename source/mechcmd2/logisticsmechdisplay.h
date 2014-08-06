#ifndef LOGISTICSMECHDISPLAY_H
#define LOGISTICSMECHDISPLAY_H
/*************************************************************************************************\
LogisticsMechDisplay.h			: Interface for the LogisticsMechDisplay component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef LOGISTICSSCREEN_H
#include "LogisticsScreen.h"
#endif

#ifndef SIMPLECAMERA_H
#include "SimpleCamera.h"
#endif

#ifndef ATTRIBUTEMETER_H
#include "AttributeMeter.h"
#endif

#ifndef SIMPLECOMPONENTLISTBOX_H
#include "SimpleComponentListBox.h"
#endif

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
