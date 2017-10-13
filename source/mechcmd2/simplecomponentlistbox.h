/*************************************************************************************************\
SimpleComponentListBox.h			: Interface for the SimpleComponentListBox
component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef SIMPLECOMPONENTLISTBOX_H
#define SIMPLECOMPONENTLISTBOX_H

//#include <mechgui/alistbox.h>

class LogisticsVariant;
class LogisticsVehicle;
class LogisticsComponent;
//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
SimpleComponentListBox:
**************************************************************************************************/
class ComponentListBox : public aListBox
{
  public:
	ComponentListBox(void);
	~ComponentListBox(void);
	void setMech(LogisticsVariant* pMech);
	void setVehicle(LogisticsVehicle* pVehicle);
	void setComponents(int32_t componentCount, LogisticsComponent** components);

  private:
	ComponentListBox(const ComponentListBox& src);
	ComponentListBox& operator=(const ComponentListBox& omponentListBox);
	// HELPER FUNCTIONS
};

//*************************************************************************************************
#endif // end of file ( SimpleComponentListBox.h )
