
#pragma once

#ifndef MECHLABSCREEN_H
#define MECHLABSCREEN_H
/*************************************************************************************************\
MechLabScreen.h			: Interface for the MechLabScreen component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef LOGISTICSSCREEN_H
#include <mechgui/logisticsscreen.h>
#endif

#ifndef ATTRIBUTEMETER_H
#include "attributemeter.h"
#endif

#ifndef COMPONENTLISTBOX_H
#include "componentlistbox.h"
#endif

#ifndef SIMPLECAMERA_H
#include "simplecamera.h"
#endif

#define MECH_LAB_ATTRIBUTE_METER_COUNT 9
class FitIniFile;
class LogisticsVariant;
class LogisticsComponent;
class LogisticsVariantDialog;
class LogisticsAcceptVariantDialog;
//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
MechLabScreen:
**************************************************************************************************/
class MechLabScreen : public LogisticsScreen
{
public:
	MechLabScreen(void);
	virtual ~MechLabScreen(void);

	int32_t init(FitIniFile&);

	virtual void begin(void);
	virtual void end(void);
	virtual void update(void);
	virtual void render(int32_t xOffset, int32_t yOffset);

	virtual int32_t handleMessage(uint32_t, uint32_t);

	void setComponent(LogisticsComponent* pComponent, bool bMessageFromLB = 0);
	int32_t addComponent(LogisticsComponent* pComponent, int32_t& x, int32_t& y);
	void beginDrag(LogisticsComponent* pComponent);
	void endDrag();

	int32_t canAddComponent(LogisticsComponent* pComponent);

	static MechLabScreen* instance() { return s_instance; }

	bool canRemoveComponent(LogisticsComponent* pComp);

private:
	MechLabScreen(const MechLabScreen& src);
	MechLabScreen& operator=(const MechLabScreen& echLabScreen);
	static MechLabScreen* s_instance;

	static LogisticsVariantDialog* saveDlg;
	static LogisticsAcceptVariantDialog* acceptDlg;

	LogisticsVariantDialog* pCurDialog;

	LogisticsVariant* pVariant;
	LogisticsComponent* pSelectedComponent;
	int32_t selI;
	int32_t selJ;

	ComponentIconListBox componentListBox;
	aComboBox variantList;

	AttributeMeter attributeMeters[MECH_LAB_ATTRIBUTE_METER_COUNT];

	aObject componentIcons[128];
	int32_t componentCount;

	aObject payloadIcon;

	aObject dragIcon;

	LogisticsComponent* pDragComponent;
	LogisticsComponent* pCurComponent;

	void showJumpJetItems(bool bShow);

	void updateDiagram(void);

	float originalCost;

	aObject selRects[2][5];
	aObject selJumpJetRect;
	aObject* selRect;

	bool bSaveDlg;
	bool bDragLeft;

	SimpleCamera camera;

	std::wstring varName;

	static RECT sensorRects[4];
	static int32_t sensorHelpIDs[4];

	float countDownTime;
	float curCount;
	float previousAmount;
	float oldCBillsAmount;

	float oldHeat;
	float heatTime;
	bool flashHeatRange;

	float oldArmor;
	float newArmor;
	float armorTime;

	bool bErrorDlg;

	// HELPER FUNCTIONS
	void getMouseDiagramCoords(int32_t& x, int32_t& y);
	void getMouseDiagramCoords(int32_t screenX, int32_t screenY, int32_t& x, int32_t& y);

	void diagramToScreen(int32_t i, int32_t j, int32_t& x, int32_t& y);
	int32_t selectFirstDiagramComponent(void);
	int32_t selectFirstLBComponent(void);
	void updateDiagramInput(void);
	void swapVariant(void);
	void updateHeatMeter(void);
	void updateArmorMeter(void);
	void removeComponent(int32_t i, int32_t j);
};

//*************************************************************************************************
#endif // end of file ( MechLabScreen.h )
