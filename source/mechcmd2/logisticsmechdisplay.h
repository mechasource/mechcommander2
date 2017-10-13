/*************************************************************************************************\
LogisticsMechDisplay.h			: Interface for the LogisticsMechDisplay
component.
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
	LogisticsMechDisplay(void);

	virtual ~LogisticsMechDisplay(void);

	virtual void update(void);
	virtual void render(int32_t xOffset, int32_t yOffset);
	int32_t init();
	void setMech(LogisticsMech* pMech, bool bFromLB = 0);

  private:
	LogisticsMech* pCurMech;
	AttributeMeter attributeMeters[3];
	SimpleCamera mechCamera;
	ComponentListBox componentListBox;
};

//*************************************************************************************************
#endif // end of file ( LogisticsMechDisplay.h )
