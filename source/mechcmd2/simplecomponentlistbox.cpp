#define SIMPLECOMPONENTLISTBOX_CPP
/*************************************************************************************************\
SimpleComponentListBox.cpp			: Implementation of the
SimpleComponentListBox component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdinc.h"

#include "simplecomponentlistbox.h"
#include "resource.h"
#include "LogisticsVariant.h"
#include "LogisticsComponent.h"
#include "LogisticsData.h"
#include "cmponent.h"

//-------------------------------------------------------------------------------------------------
ComponentListBox::ComponentListBox()
{
	clickSFX = -1;
	highlightSFX = -1;
	disabledSFX = -1;
	helpid = IDS_HELP_WEAPONS;
}

ComponentListBox::~ComponentListBox()
{
	aListBox::destroy();
}

void ComponentListBox::setMech(LogisticsVariant* pMech)
{
	removeAllItems(true);
	if (pMech)
	{
		LogisticsComponent* components[256];
		int32_t componentCount = 256;
		pMech->getComponents(componentCount, components);
		setComponents(componentCount, components);
		int32_t sensor = pMech->getSensorID();
		int32_t ECM = pMech->getECM();
		if (sensor > 0)
		{
			aTextListItem* textItem = new aTextListItem(IDS_SALVAGE_AREA_COMPONENTS);
			textItem->setText(32000 + sensor);
			textItem->setcolour(0xffc29b00);
			textItem->setHelpID(IDS_HELP_COMP0 + sensor);
			AddItem(textItem);
		}
		if (ECM > 0)
		{
			aTextListItem* textItem = new aTextListItem(IDS_SALVAGE_AREA_COMPONENTS);
			textItem->setText(32000 + ECM);
			textItem->setcolour(0xffc29b00);
			textItem->setHelpID(IDS_HELP_COMP0 + ECM);
			AddItem(textItem);
		}
	}
}

void ComponentListBox::setVehicle(LogisticsVehicle* pVeh)
{
	removeAllItems(true);
	if (pVeh)
	{
		LogisticsComponent* components[256];
		int32_t componentCount = 256;
		pVeh->getComponents(componentCount, components);
		setComponents(componentCount, components);
	}
}

void ComponentListBox::setComponents(int32_t componentCount, LogisticsComponent** components)
{
	LogisticsComponent* finalList[64];
	int32_t finalListCount[64];
	//		int32_t stringIDs[4] = { IDS_SHORT, IDS_MEDIUM, IDS_LONG,
	// IDS_COMPONENT};
	int32_t colors[4] = {0xff6E7C00, 0xff005392, 0xffA21600, 0xffc29b00};
	//		int32_t headercolours[4] = { 0xFFC8E100, 0xff0091FF, 0xFFFF0000,
	// 0xffFF8A00 };
	int32_t i, j, k;
	for (i = 0; i < 4; i++) // do int16_t, medium int32_t
	{
		memset(finalList, 0, sizeof(int32_t) * 64);
		memset(finalListCount, 0, sizeof(int32_t) * 64);
		for (j = 0; j < componentCount; j++)
		{
			if (components[j]->getRangeType() == (LogisticsComponent::WEAPON_RANGE)i) // int16_t, med, int32_t
			{
				bool bFound = 0;
				for (k = 0; k < 64; k++)
				{
					if (finalList[k] == components[j])
					{
						finalListCount[k]++;
						bFound = true;
						break;
					}
					else if (!finalList[k])
						break;
				}
				if (!bFound)
				{
					finalList[k] = components[j];
					finalListCount[k]++;
				}
			}
		}
		if (finalList[0])
		{
			std::wstring_view str;
			// add the header
			// no more headers, keeping code just in case
			//				aTextListItem* textItem = new aTextListItem(
			// IDS_SALVAGE_AREA_COMPONENTS ); 				textItem->setText(
			// stringIDs[i] ); 				textItem->setcolour( headercolours[i]
			//); 				AddItem( textItem );
			// add each componet
			for (j = 0; j < 64; j++)
			{
				if (!finalList[j])
					break;
				aTextListItem* textItem = new aTextListItem(IDS_SALVAGE_AREA_COMPONENTS);
				if (finalList[j]->getType() != COMPONENT_FORM_JUMPJET)
				{
					str.Format("%ld/%ld %s", finalListCount[j], finalListCount[j],
						finalList[j]->getName());
					textItem->setText(str);
				}
				else
					textItem->setText(finalList[j]->getName());
				textItem->setcolour(colors[i]);
				textItem->setHelpID(IDS_HELP_COMP0 + finalList[j]->getID());
				AddItem(textItem);
			}
		}
	}
}

// end of file ( SimpleComponentListBox.cpp )
