#define MECHLABSCREEN_CPP
/*************************************************************************************************\
MechLabScreen.cpp			: Implementation of the MechLabScreen component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdinc.h"

#include "MechLabScreen.h"
#include "IniFile.h"
#include "LogisticsData.h"
#include "MechBayScreen.h"
#include "cmponent.H"
#include "windows.h"
#include "resource.h"
#include "logisticsdialog.h"
#include <malloc.h>
#include "gamesound.h"
#include "chatwindow.h"
#include "Multplyr.h"
extern bool useUnlimitedAmmo;
#include "Prefs.h"

MechLabScreen* MechLabScreen::s_instance = 0;
LogisticsVariantDialog* MechLabScreen::saveDlg = 0;
LogisticsAcceptVariantDialog* MechLabScreen::acceptDlg = 0;

RECT MechLabScreen::sensorRects[4] = {35, 126, 35 + 64, 126 + 64, 35, 126, 35 + 79, 126 + 64, 35,
	126, 35 + 94, 126 + 64, 249, 126, 249 + 79, 126 + 64};

int32_t MechLabScreen::sensorHelpIDs[4] = {
	IDS_HELP_COMP14, IDS_HELP_COMP15, IDS_HELP_COMP17, IDS_HELP_COMP38};

MechLabScreen::MechLabScreen()
{
	status = RUNNING;
	pVariant = nullptr;
	helpTextArrayID = 14;
	pCurComponent = 0;
	gosASSERT(!s_instance);
	s_instance = this;
	pDragComponent = nullptr;
	pSelectedComponent = nullptr;
	selI = -1;
	selJ = -1;
	countDownTime = .5;
	curCount = 0.0;
	previousAmount = 0;
	oldCBillsAmount = 0;
	oldHeat = 0;
	heatTime = 0;
	oldArmor = 0;
	newArmor = 0;
	armorTime = 0;
	bDragLeft = 0;
	selRect = nullptr;
}

//-------------------------------------------------------------------------------------------------

MechLabScreen::~MechLabScreen()
{
	s_instance = nullptr;
	if (saveDlg)
	{
		delete saveDlg;
		saveDlg = nullptr;
	}
	if (acceptDlg)
	{
		delete acceptDlg;
		acceptDlg = nullptr;
	}
	variantList.destroy();
	componentListBox.destroy();
}

int32_t
MechLabScreen::init(FitIniFile& file)
{
	if (saveDlg) // already initialized
		return -1;
	ComponentListItem::init(file);
	LogisticsScreen::init(file, "Static", "Text", "Rect", "Button");
	componentListBox.init(rects[2].left(), rects[2].top(), rects[2].width(), rects[2].height());
	componentListBox.setHighlightFX(-1); // want to do this by hand.
	saveDlg = new LogisticsVariantDialog;
	acceptDlg = new LogisticsAcceptVariantDialog;
	FitIniFile saveFile;
	FullPathFileName path;
	path.init(artPath, "MCL_SV", ".fit");
	if (NO_ERROR != saveFile.open(path))
	{
		Assert(0, 0, "couldn't open MCL_SV.fit");
		return -1;
	}
	saveDlg->init(saveFile);
	saveFile.close();
	path.init(artPath, "mcl_sv_acceptchanges", ".fit");
	if (NO_ERROR != saveFile.open(path))
	{
		Assert(0, 0, "couldn't open MCL_SV.fit");
		return -1;
	}
	acceptDlg->init(saveFile);
	payloadIcon.init(&file, "PayloadUnit");
	wchar_t blockname[64];
	for (size_t i = 0; i < MECH_LAB_ATTRIBUTE_METER_COUNT; i++)
	{
		sprintf(blockname, "AttributeMeter%ld", i);
		attributeMeters[i].init(&file, blockname);
	}
	for (i = 0; i < buttonCount; i++)
	{
		switch (buttons[i].getID())
		{
		case COMPONENT_FORM_WEAPON_ENERGY:
		case COMPONENT_FORM_WEAPON_BALLISTIC:
		case COMPONENT_FORM_WEAPON_MISSILE:
		case COMPONENT_FORM_JUMPJET:
			break;
		default:
		{
			buttons[i].setMessageOnRelease();
			buttons[i].setPressFX(LOG_CLICKONBUTTON);
		}
		}
	}
	file.seekBlock("ComboBox0");
	int32_t xLoc, yLoc;
	file.readIdLong("XLocation", xLoc);
	file.readIdLong("YLocation", yLoc);
	wchar_t fileName[256];
	file.readIdString("filename", fileName, 255);
	path.init(artPath, fileName, ".fit");
	FitIniFile comboFile;
	if (NO_ERROR != comboFile.open(path))
	{
		wchar_t errorStr[255];
		sprintf(errorStr, "couldn't open file %s", (std::wstring_view)path);
		Assert(0, 0, errorStr);
	}
	variantList.init(&comboFile, "VariantComboBox");
	variantList.setReadOnly(true);
	variantList.ListBox().setOrange(true);
	variantList.setParent(this);
	variantList.setHelpID(IDS_HELP_MC_VARIANT_DROP_DOWN_LIST_BOX);
	camera.init(rects[4].left(), rects[4].top(), rects[4].right(), rects[4].bottom());
	getButton(COMPONENT_FORM_WEAPON_ENERGY)->setPressFX(LOG_VIDEOBUTTONS);
	getButton(COMPONENT_FORM_WEAPON_BALLISTIC)->setPressFX(LOG_VIDEOBUTTONS);
	getButton(COMPONENT_FORM_WEAPON_MISSILE)->setPressFX(LOG_VIDEOBUTTONS);
	getButton(COMPONENT_FORM_JUMPJET)->setPressFX(LOG_VIDEOBUTTONS);
	getButton(COMPONENT_FORM_WEAPON_ENERGY)->setHighlightFX(LOG_DIGITALHIGHLIGHT);
	getButton(COMPONENT_FORM_WEAPON_BALLISTIC)->setHighlightFX(LOG_DIGITALHIGHLIGHT);
	getButton(COMPONENT_FORM_WEAPON_MISSILE)->setHighlightFX(LOG_DIGITALHIGHLIGHT);
	getButton(COMPONENT_FORM_JUMPJET)->setHighlightFX(LOG_DIGITALHIGHLIGHT);
	// initialize the selection rectangles
	for (i = 0; i < 5; i++)
	{
		for (size_t j = 0; j < 2; j++)
		{
			sprintf(fileName, "mcl_mc_%ldby%ld_selection.tga", j + 1, i + 1);
			path.init(artPath, fileName, ".tga");
			if (fileExists(path))
			{
				selRects[j][i].setTexture(path);
				selRects[j][i].resize((j + 1) * LogisticsComponent::XICON_FACTOR,
					(i + 1) * LogisticsComponent::YICON_FACTOR);
				// now need to set the UV's
				selRects[j][i].setUVs(0, 0, selRects[j][i].width(), selRects[j][i].height());
			}
		}
	}
	path.init(artPath, "mcl_mc_jumpjets_selection", ".tga");
	selJumpJetRect.setTexture(path);
	selJumpJetRect.resize(
		2 * LogisticsComponent::XICON_FACTOR, 2 * LogisticsComponent::YICON_FACTOR);
	// now need to set the UV's
	selJumpJetRect.setUVs(0, 0, selJumpJetRect.width(), selJumpJetRect.height());
	return true;
}

void MechLabScreen::begin()
{
	componentCount = 0;
	bSaveDlg = 0;
	status = RUNNING;
	bDragLeft = 0;
	bErrorDlg = 0;
	if (!pVariant) // if we are coming directly from the main menu
	{
		pVariant = LogisticsData::instance->getMechToModify()->getVariant();
		variantList.ListBox().removeAllItems(true);
		int32_t maxCount = 0;
		LogisticsVariant** pVar = nullptr;
		int32_t addedCount = 0;
		LogisticsData::instance->getChassisVariants(pVariant->getChassis(), pVar, maxCount);
		if (maxCount)
		{
			maxCount++;
			pVar = (LogisticsVariant**)_alloca(maxCount * sizeof(LogisticsVariant*));
			LogisticsData::instance->getChassisVariants(pVariant->getChassis(), pVar, maxCount);
			for (size_t i = 0; i < maxCount; i++)
			{
				if (pVar[i]->allComponentsAvailable())
				{
					variantList.AddItem(pVar[i]->getName(), rects[1].getcolour());
					if (pVar[i]->getName().Compare(pVariant->getName()) == 0)
					{
						variantList.SelectItem(i);
						variantList.EditBox().setEntry(pVar[i]->getName());
					}
					addedCount++;
				}
			}
		}
		if (!addedCount) // this is a mech we don't have access to yet
		{
			variantList.AddItem(pVariant->getName(), rects[1].getcolour());
			variantList.SelectItem(0);
			variantList.EditBox().setEntry(pVariant->getName());
		}
		// no changes, set cost to 0
		textObjects[5].setText("0");
	}
	if (pVariant)
	{
		textObjects[3].setText(pVariant->getChassisName());
		originalCost = pVariant->getCost();
		wchar_t path[256];
		strcpy(path, artPath);
		strcat(path, "MCL_MC_");
		wchar_t mechName[64];
		std::wstring_view fileName = pVariant->getFileName();
		_splitpath(fileName, nullptr, nullptr, mechName, nullptr);
		strcat(path, mechName);
		strcat(path, "_B.tga");
		CharLower(path);
		for (size_t i = 51; i < 54; i++)
			statics[i].setTexture(path);
		strcpy(path, artPath);
		strcat(path, "MCL_MC_");
		strcat(path, mechName);
		strcat(path, "_A.tga");
		CharLower(path);
		statics[50].setTexture(path);
		for (i = COMPONENT_FORM_WEAPON_ENERGY; i < COMPONENT_FORM_JUMPJET + 1; i++)
		{
			aButton* pButton = getButton(i);
			if (pButton)
			{
				pButton->press(0);
			}
		}
		componentListBox.removeAllItems(0);
		componentListBox.setType(COMPONENT_FORM_WEAPON_BALLISTIC, -1, -1);
		getButton(COMPONENT_FORM_WEAPON_BALLISTIC)->press(true);
		updateDiagram();
		if (-1 == selectFirstDiagramComponent())
			selectFirstLBComponent();
		oldCBillsAmount = 0;
	}
	// init CBills
	wchar_t text[32];
	sprintf(text, "%ld ", LogisticsData::instance->getCBills());
	textObjects[1].setText(text);
	variantList.EditBox().getEntry(varName);
	if (pVariant->getChassis()->jumpJetsAllowed())
	{
		statics[54].showGUIWindow(true);
	}
	else
		statics[54].showGUIWindow(false);
}
void MechLabScreen::end()
{
	camera.setMech(nullptr);
	pVariant = nullptr;
}
void MechLabScreen::update()
{
	if (bSaveDlg)
	{
		pCurDialog->update();
		if (pCurDialog->isDone())
		{
			if (pCurDialog->getStatus() == YES)
			{
				LogisticsData::instance->acceptMechModifications(pCurDialog->getFileName());
				pCurDialog->end();
				status = UP;
				bSaveDlg = 0;
			}
			else if (pCurDialog->getStatus() == NO)
			{
				bSaveDlg = 0;
				pCurDialog->end();
			}
		}
		return;
	}
	else if (bErrorDlg)
	{
		LogisticsOneButtonDialog::instance()->update();
		if (LogisticsOneButtonDialog::instance()->isDone())
			bErrorDlg = 0;
		return;
	}
	if (!pVariant || status != RUNNING)
		return;
	camera.update();
	int32_t oldSel = componentListBox.GetSelectedItem(); // need to check this
		// before we update the
		// list box
	for (size_t i = 0; i < MECH_LAB_ATTRIBUTE_METER_COUNT; i++)
		attributeMeters[i].update();
	if (!MPlayer || !ChatWindow::instance()->pointInside(userInput->getMouseX(), userInput->getMouseY()))
		LogisticsScreen::update();
	updateDiagramInput();
	variantList.update();
	bool vals[4];
	if (pVariant && status == RUNNING) // sensor help texts
	{
		int32_t sensorID = pVariant->getSensorID();
		for (size_t i = 0; i < 3; i++)
		{
			if (sensorID == sensorHelpIDs[i] - IDS_HELP_COMP0)
				vals[i] = true;
			else
				vals[i] = false;
		}
		vals[3] = pVariant->getECM() > 0;
		for (i = 0; i < 4; i++)
		{
			if (vals[i])
			{
				if (sensorRects[i].left <= userInput->getMouseX() && sensorRects[i].right >= userInput->getMouseX() && sensorRects[i].top <= userInput->getMouseY() && sensorRects[i].bottom >= userInput->getMouseY() && !helpTextID)
				{
					helpTextID = sensorHelpIDs[i];
					break;
				}
			}
		}
	}
	// hack, in case the variant list sets the helpid
	if (::helpTextID)
	{
		textObjects[helpTextArrayID].setText(helpTextID);
	}
	// see if the variant changed
	std::wstring_view tmp;
	variantList.EditBox().getEntry(tmp);
	if (varName.Compare(tmp) != 0)
	{
		swapVariant();
	}
	if (status != RUNNING) // can change after button updates
		return;
	// update the list box
	componentListBox.update();
	int32_t newSel = componentListBox.GetSelectedItem();
	if (oldSel != newSel)
	{
		if (newSel != -1)
		{
			pSelectedComponent = nullptr;
			selI = selJ = -1;
		}
		else
		{
			bool bFound = 0;
			for (size_t i = 0; i < componentListBox.GetItemCount(); i++)
			{
				if (componentListBox.GetItem(i)->getState() != aListItem::DISABLED && !canAddComponent(((ComponentListItem*)componentListBox.GetItem(i))->getComponent()))
				{
					bFound = 1;
					break;
				}
			}
			if (!bFound)
				selectFirstDiagramComponent();
		}
	}
	bool bNoDelete = !canRemoveComponent(pSelectedComponent);
	if (pSelectedComponent)
	{
		setComponent(pSelectedComponent);
		if (bNoDelete || (selI == -1 && selJ == -1))
			getButton(MB_MSG_REMOVE)->disable(1);
		else
			getButton(MB_MSG_REMOVE)->disable(0);
		getButton(MB_MSG_ADD)->disable(1);
	}
	else
	{
		LogisticsComponent* pComp = componentListBox.getComponent();
		if (pComp)
		{
			setComponent(pComp);
			getButton(MB_MSG_ADD)->disable(0);
			getButton(MB_MSG_REMOVE)->disable(1);
		}
		else
			getButton(MB_MSG_REMOVE)->disable(1);
	}
	int32_t color = 0xffffffff;
	float newCost = pVariant->getCost();
	// figure out change
	float costChange = newCost - originalCost;
	if (curCount && curCount + frameLength < countDownTime)
	{
		curCount += frameLength;
		float curAmount = previousAmount - (curCount / countDownTime * previousAmount);
		costChange += curAmount;
		color = 0xffa21600;
		if (curAmount > 0)
			color = 0xffc8e100;
	}
	updateHeatMeter();
	updateArmorMeter();
	wchar_t text[64];
	sprintf(text, "%.0lf ", costChange);
	textObjects[5].setText(text);
	textObjects[5].setcolour(color);
	if (MPlayer && ChatWindow::instance())
		ChatWindow::instance()->update();
}

void MechLabScreen::updateDiagramInput()
{
	int32_t mouseX = userInput->getMouseX();
	int32_t mouseY = userInput->getMouseY();
	if (rects[13].pointInside(mouseX, mouseY))
	{
		int32_t x, y, x2, y2;
		getMouseDiagramCoords(x, y);
		if (x != -2)
		{
			// now offset by size of the component
			if (pDragComponent)
			{
				int32_t screenX, screenY;
				diagramToScreen(x, y, screenX, screenY);
				x -= pDragComponent->getComponentwidth() / 2;
				y -= pDragComponent->getComponentheight() / 2;
				if (mouseX - screenX > LogisticsComponent::XICON_FACTOR / 2 && pDragComponent->getComponentwidth() / 2)
				{
					x += 1;
				}
			}
			if (x < 0)
				x = 0;
			if (x >= pVariant->getComponentAreawidth() - 1)
				x = pVariant->getComponentAreawidth() - 1;
			if (y < 0)
				y = 0;
			if (y >= pVariant->getComponentAreaheight() - 1)
				y = pVariant->getComponentAreaheight() - 1;
			if (pDragComponent)
			{
				x2 = x + pDragComponent->getComponentwidth();
				y2 = y + pDragComponent->getComponentheight();
				if (x2 > pVariant->getComponentAreawidth())
				{
					x = pVariant->getComponentAreawidth() - pDragComponent->getComponentwidth();
					x2 = pVariant->getComponentAreawidth();
				}
				if (y2 > pVariant->getComponentAreaheight())
				{
					y = pVariant->getComponentAreaheight() - pDragComponent->getComponentheight();
					y2 = pVariant->getComponentAreaheight();
				}
			}
			else
			{
				x2 = x + 1;
				y2 = y + 1;
			}
			int32_t tmpX, tmpY;
			diagramToScreen(x, y, tmpX, tmpY);
			// update outline rect
			if (selRect)
				selRect->moveTo(tmpX, tmpY);
			// highlight text if appropriate
			LogisticsComponent* pComp = pVariant->getCompAtLocation(x, y, tmpX, tmpY);
			if (pComp)
			{
				int32_t compX, compY, compX2, compY2;
				diagramToScreen(tmpX, tmpY, compX, compY);
				diagramToScreen(tmpX + pComp->getComponentwidth(),
					tmpY + pComp->getComponentheight(), compX2, compY2);
				if ((compX <= userInput->getMouseX() && compX2 >= userInput->getMouseX() && compY <= userInput->getMouseY() && compY2 >= userInput->getMouseY()) || tmpX == -2)
				{
					::helpTextID = IDS_HELP_COMP0 + pComp->getID();
				}
			}
		}
		else if (selRect)
		{
			selRect->moveTo(rects[6].left() + rects[6].width() / 2 - selRect->width() / 2,
				rects[6].top() + rects[6].height() / 2 - selRect->height() / 2);
		}
		// check for jump jet hot text
		if (x == -2 && y == -2)
		{
			int32_t tmpX, tmpY;
			LogisticsComponent* pComp = pVariant->getCompAtLocation(x, y, tmpX, tmpY);
			if (pComp)
			{
				int32_t compX, compY, compX2, compY2;
				diagramToScreen(tmpX, tmpY, compX, compY);
				diagramToScreen(tmpX + pComp->getComponentwidth(),
					tmpY + pComp->getComponentheight(), compX2, compY2);
				if ((compX <= userInput->getMouseX() && compX2 >= userInput->getMouseX() && compY <= userInput->getMouseY() && compY2 >= userInput->getMouseY()) || tmpX == -2)
				{
					::helpTextID = IDS_HELP_COMP0 + pComp->getID();
				}
			}
		}
		if (pDragComponent)
		{
			if (NO_ERROR == pVariant->canAddComponent(pDragComponent, x, y) && x != -1 && y != -1)
			{
				selRect->setcolour(0xffffffff);
			}
			else
				selRect->setcolour(0xffff0000);
		}
		else if (userInput->isLeftDrag())
		{
			int32_t i, j;
			getMouseDiagramCoords(i, j);
			if (i != -1 && j != -1)
			{
				LogisticsComponent* pComp = pVariant->getCompAtLocation(i, j, selI, selJ);
				if (pComp && pComp == pSelectedComponent)
				{
					if (canRemoveComponent(pComp))
					{
						componentListBox.SelectItem(-1);
						beginDrag(pComp);
						bDragLeft = true;
						removeComponent(i, j);
						setComponent(pComp);
						updateDiagram();
					}
					else
						soundSystem->playDigitalSample(LOG_WRONGBUTTON);
				}
				else
					pSelectedComponent = 0;
			}
		}
		else if (userInput->isLeftDoubleClick())
		{
			int32_t tmpI, tmpJ;
			getMouseDiagramCoords(tmpI, tmpJ);
			if (tmpI != -1 && tmpJ != -1)
			{
				selI = tmpI;
				selJ = tmpJ;
				LogisticsComponent* pComp = pVariant->getCompAtLocation(tmpI, tmpJ, selI, selJ);
				if (pComp)
				{
					if (canRemoveComponent(pComp))
					{
						removeComponent(selI, selJ);
						componentListBox.SelectItem(-1);
						if (-1 == selectFirstDiagramComponent())
							selectFirstLBComponent();
					}
					else
						soundSystem->playDigitalSample(LOG_WRONGBUTTON);
				}
			}
		}
		else if (userInput->isLeftClick())
		{
			int32_t tmpI, tmpJ;
			getMouseDiagramCoords(tmpI, tmpJ);
			if (tmpI != -1 && tmpJ != -1)
			{
				selI = tmpI;
				selJ = tmpJ;
				LogisticsComponent* pComp = pVariant->getCompAtLocation(tmpI, tmpJ, selI, selJ);
				if (pComp)
				{
					pSelectedComponent = pComp;
					setComponent(pComp);
					soundSystem->playDigitalSample(LOG_SELECT);
					componentListBox.SelectItem(-1);
				}
			}
		}
	}
	if (userInput->leftMouseReleased() && pDragComponent)
		endDrag();
}
void MechLabScreen::render(int32_t xOffset, int32_t yOffset)
{
	if (!xOffset && !yOffset)
	{
		if (!MPlayer && !LogisticsData::instance->isSingleMission() && LogisticsData::instance->newWeaponsAvailable())
		{
			soundSystem->playBettySample(BETTY_NEW_WEAPONS);
			LogisticsData::instance->setNewWeaponsAcknowledged();
		}
	}
	componentListBox.move(xOffset, yOffset);
	componentListBox.render();
	componentListBox.move(-xOffset, -yOffset);
	for (size_t i = 0; i < MECH_LAB_ATTRIBUTE_METER_COUNT; i++)
		attributeMeters[i].render(xOffset, yOffset);
	if (!xOffset && !yOffset)
		camera.render();
	LogisticsScreen::render(xOffset, yOffset);
	if (MPlayer && ChatWindow::instance())
		ChatWindow::instance()->render(xOffset, yOffset);
	// figure where components go
	if (pVariant)
	{
		int32_t width = pVariant->getComponentAreawidth();
		int32_t minX = rects[7 + 6 - width].left();
		int32_t maxY = rects[7].bottom();
		int32_t minY = maxY - pVariant->getComponentAreaheight() * LogisticsComponent::YICON_FACTOR;
		// draw little component slots
		int32_t x = minX;
		int32_t y = minY;
		for (size_t j = 0; j < pVariant->getComponentAreaheight(); j++)
		{
			x = minX;
			for (size_t i = 0; i < pVariant->getComponentAreawidth(); i++)
			{
				payloadIcon.render(x + xOffset, y + yOffset);
				x += payloadIcon.width();
			}
			y += payloadIcon.height();
		}
	}
	for (i = 0; i < componentCount; i++)
		componentIcons[i].render(xOffset, yOffset);
	if (pSelectedComponent && pVariant)
	{
		int32_t i = selI;
		int32_t j = selJ;
		pVariant->getComponentLocation(pSelectedComponent, i, j);
		if (i != -1 && j != -1)
		{
			aObject* tmpRect = nullptr;
			if (pSelectedComponent->getType() == COMPONENT_FORM_JUMPJET)
			{
				tmpRect = &selJumpJetRect;
				selJumpJetRect.moveTo(rects[6].left() + xOffset, rects[6].top() + yOffset);
			}
			else
			{
				int32_t left, right;
				diagramToScreen(i, j, left, right);
				tmpRect = &selRects[pSelectedComponent->getComponentwidth() - 1]
								   [pSelectedComponent->getComponentheight() - 1];
				tmpRect->moveTo(left + xOffset, right + yOffset);
			}
			if (!pDragComponent && tmpRect)
			{
				tmpRect->setcolour(0xffffffff);
				tmpRect->render();
			}
		}
		else
			pSelectedComponent = 0;
	}
	variantList.move(xOffset, yOffset);
	variantList.render();
	variantList.move(-xOffset, -yOffset);
	if (pDragComponent)
	{
		dragIcon.moveTo(userInput->getMouseX() - dragIcon.width() / 2,
			userInput->getMouseY() - dragIcon.height() / 2);
		dragIcon.render();
		selRect->render();
	}
	if (bSaveDlg)
	{
		pCurDialog->render();
		return;
	}
	else if (bErrorDlg)
	{
		LogisticsOneButtonDialog::instance()->render();
	}
}

void MechLabScreen::updateHeatMeter()
{
	int32_t maxHeat = pVariant->getMaxHeat();
	int32_t curHeat = pVariant->getHeat();
	float fCurHeat = curHeat;
	bool bShowDenom = true;
	if (heatTime)
	{
		if (heatTime < .6)
		{
			if (oldHeat >= curHeat)
			{
				curHeat = oldHeat - (oldHeat - fCurHeat) * heatTime / .6;
				fCurHeat = curHeat;
			}
			else
			{
				curHeat = (float)oldHeat * heatTime / .6;
				fCurHeat = curHeat;
			}
			if (flashHeatRange)
				curHeat = pVariant->getHeat();
			heatTime += frameLength;
		}
		else if (heatTime > 1.6 || oldHeat >= curHeat)
		{
			heatTime = 0; // done animating;
		}
		else
		{
			float flashTime = heatTime - .6;
			if (flashTime < .25 || (flashTime > .5 && flashTime < .75))
			{
			}
			else
			{
				if (flashHeatRange)
					bShowDenom = false;
				else
					fCurHeat = oldHeat;
			}
			heatTime += frameLength;
		}
	}
	wchar_t text[64];
	if (bShowDenom)
		sprintf(text, "%ld/%ld", curHeat, maxHeat);
	else
		sprintf(text, "%ld/", curHeat);
	textObjects[12].setText(text);
	attributeMeters[MECH_LAB_ATTRIBUTE_METER_COUNT - 4].setValue(fCurHeat / (float)maxHeat);
}

void MechLabScreen::updateArmorMeter()
{
	int32_t maxarmor = pVariant->getMaxArmor();
	int32_t curarmor = pVariant->getArmor();
	float fCurarmor = curarmor;
	if (armorTime)
	{
		if (armorTime < .6)
		{
			if (oldArmor >= curarmor)
			{
				curarmor = oldArmor - (oldArmor - fCurarmor) * armorTime / .6;
				fCurarmor = curarmor;
			}
			else
			{
				curarmor = (float)oldArmor * armorTime / .6;
				fCurarmor = curarmor;
			}
			armorTime += frameLength;
		}
		else if (armorTime > 1.6 || oldArmor >= curarmor)
		{
			armorTime = 0; // done animating;
		}
		else
		{
			float flashTime = armorTime - .6;
			if (flashTime < .25 || (flashTime > .5 && flashTime < .75))
			{
			}
			else
			{
				fCurarmor = oldArmor;
			}
			armorTime += frameLength;
		}
	}
	wchar_t text[64];
	sprintf(text, "%ld/%ld", curarmor, maxarmor);
	textObjects[11].setText(text);
	attributeMeters[MECH_LAB_ATTRIBUTE_METER_COUNT - 3].setValue(fCurarmor / (float)maxarmor);
}

void MechLabScreen::swapVariant()
{
	// variant changed
	variantList.EditBox().getEntry(varName);
	LogisticsVariant* pNewVariant = LogisticsData::instance->getVariant(varName);
	if (pVariant && pNewVariant)
	{
		std::wstring_view oldName = pVariant->getName();
		*pVariant = *pNewVariant;
		pVariant->setName(oldName);
	}
	updateDiagram();
	selectFirstDiagramComponent();
}

int32_t
MechLabScreen::handleMessage(uint32_t msg, uint32_t who)
{
	int32_t i;
	if (msg == aMSG_SELCHANGED)
	{
		swapVariant();
		return 1;
	}
	switch (who)
	{
	case MB_MSG_NEXT: // actuall accept
	{
		// make sure we have the money...
		float newCost = pVariant->getCost();
		// figure out change
		float costChange = newCost - originalCost;
		if (costChange > LogisticsData::instance->getCBills())
		{
			LogisticsOneButtonDialog::instance()->setText(
				IDS_MC_INSUFFICIENT_CBILLS, IDS_DIALOG_OK, IDS_DIALOG_OK);
			LogisticsOneButtonDialog::instance()->begin();
			bErrorDlg = true;
		}
		else
		{
			// see if it's the same as the current variant...
			LogisticsVariant* pCurVar = LogisticsData::instance->getVariant(varName);
			if (pCurVar && *pCurVar == *pVariant)
			{
				LogisticsData::instance->acceptMechModificationsUseOldVariant(pCurVar->getName());
				status = UP;
			}
			else
			{
				if (pCurVar && !pCurVar->allComponentsAvailable())
					pCurDialog = acceptDlg;
				else if (pVariant && !pVariant->allComponentsAvailable())
					pCurDialog = acceptDlg;
				else
					pCurDialog = saveDlg;
				pCurDialog->begin();
				bSaveDlg = true;
			}
		}
	}
	break;
	case MB_MSG_PREV: // actually cancel
		LogisticsData::instance->cancelMechModfications();
		pVariant = nullptr;
		status = UP;
		break;
	case COMPONENT_FORM_WEAPON_ENERGY:
	case COMPONENT_FORM_WEAPON_BALLISTIC:
	case COMPONENT_FORM_WEAPON_MISSILE:
		for (i = COMPONENT_FORM_WEAPON_ENERGY; i < COMPONENT_FORM_JUMPJET + 1; i++)
		{
			aButton* pButton = getButton(i);
			if (pButton)
			{
				pButton->press(0);
			}
		}
		getButton(who)->press(1);
		componentListBox.setType(who, -1, -1);
		break;
	case COMPONENT_FORM_JUMPJET:
		for (i = COMPONENT_FORM_WEAPON_ENERGY; i < COMPONENT_FORM_JUMPJET + 1; i++)
		{
			aButton* pButton = getButton(i);
			if (pButton)
			{
				pButton->press(0);
			}
		}
		getButton(who)->press(1);
		componentListBox.setType(
			COMPONENT_FORM_JUMPJET, COMPONENT_FORM_BULK, COMPONENT_FORM_HEATSINK);
		break;
	case MB_MSG_ADD:
	{
		int32_t x = -1;
		int32_t y = -1;
		addComponent(componentListBox.getComponent(), x, y);
	}
	break;
	case MB_MSG_REMOVE:
		if (pSelectedComponent && (selI != -1 && selJ != -1))
		{
			removeComponent(selI, selJ);
			pSelectedComponent = 0;
			if (-1 == selectFirstDiagramComponent())
				selectFirstLBComponent();
		}
		updateDiagram();
		break;
	case MB_MSG_MAINMENU:
		status = MAINMENU;
		break;
	}
	return 0;
}

void MechLabScreen::setComponent(LogisticsComponent* pComponent, bool bMessageFromLB)
{
	if (bMessageFromLB && canAddComponent(pComponent)) // if they clicked on a disabled item
	{
		componentListBox.SelectItem(-1);
		selI = selJ = -1;
		pSelectedComponent = nullptr;
		getButton(MB_MSG_ADD)->disable(true);
	}
	if (pComponent && pComponent == pCurComponent)
		return;
	pCurComponent = pComponent;
	// ammo doesn't deserve a meter
	attributeMeters[3].showGUIWindow(0);
	wchar_t text[64];
	if (pCurComponent)
	{
		// set the little spinning figure
		// uncomment this when the art is in, if ever...
		camera.setComponent(pCurComponent->getPictureFileName());
		textObjects[6].setText(pComponent->getName());
		// first figure out if this is a jump jet or not
		if (pCurComponent->getType() == COMPONENT_FORM_JUMPJET)
		{
			showJumpJetItems(1);
			// set up jump range
			attributeMeters[4].setValue(
				((float)pVariant->getMaxJumpRange() * 25.0f) / MAX_JUMP_RANGE);
			sprintf(text, "%ld", pVariant->getMaxJumpRange() * 25);
			textObjects[7].setText(text);
			textObjects[7].setcolour(0xff005392);
			rects[16].setHelpID(IDS_HELP_PM_JUMP);
			// set up heat
			attributeMeters[7].setValue(pCurComponent->getHeat() / LogisticsComponent::MAX_HEAT);
			attributeMeters[7].showGUIWindow(true);
			sprintf(text, "%.1lf", pCurComponent->getHeat());
			textObjects[15].setText(text);
			textObjects[8].setText("");
			textObjects[9].setText("");
			textObjects[10].setText("");
			textObjects[13].setText("");
			rects[17].setHelpID(IDS_HELP_MC_HEAT_GENERATED);
			attributeMeters[0].showGUIWindow(false);
			rects[18].setHelpID(0);
			rects[19].setHelpID(0);
			rects[20].setHelpID(0);
		}
		else if (pCurComponent->getType() == COMPONENT_FORM_HEATSINK)
		{
			showJumpJetItems(1);
			// hide jump range stuff
			attributeMeters[4].showGUIWindow(0);
			textObjects[7].showGUIWindow(0);
			statics[47].showGUIWindow(0);
			textObjects[15].setText("");
			// jump range
			statics[48].showGUIWindow(0);
			// armor
			statics[49].showGUIWindow(0);
			// damage
			statics[30].showGUIWindow(0);
			// heat
			statics[55].showGUIWindow(1);
			attributeMeters[7].showGUIWindow(0);
			// set up heat, negative heat is stored in the damage area of heat
			// sinks, god knows why
			attributeMeters[8].setValue(pCurComponent->getDamage() / LogisticsComponent::MAX_HEAT);
			attributeMeters[8].showGUIWindow(true);
			sprintf(text, "+%.1lf", pCurComponent->getDamage());
			textObjects[16].setText(text);
			textObjects[16].showGUIWindow(true);
			textObjects[8].setText("");
			textObjects[9].setText("");
			textObjects[10].setText("");
			textObjects[13].setText("");
			attributeMeters[0].showGUIWindow(false);
			rects[16].setHelpID(IDS_HELP_MC_HEAT_GENERATED);
			rects[17].setHelpID(0);
			rects[18].setHelpID(0);
			rects[19].setHelpID(0);
			rects[20].setHelpID(0);
		}
		else if (pCurComponent->getType() == COMPONENT_FORM_BULK)
		{
			showJumpJetItems(1);
			// jump range
			statics[48].showGUIWindow(0);
			// armor
			statics[49].showGUIWindow(1);
			// damage
			statics[30].showGUIWindow(0);
			sprintf(text, "%.1lf", pCurComponent->getDamage());
			textObjects[7].setText(text);
			textObjects[7].setcolour(0xff005392);
			textObjects[8].setText("");
			textObjects[9].setText("");
			textObjects[10].setText("");
			textObjects[15].setText("");
			textObjects[13].setText("");
			attributeMeters[7].showGUIWindow(false);
			for (size_t i = 0; i < 4; i++)
			{
				attributeMeters[i].showGUIWindow(0);
			}
			rects[16].setHelpID(IDS_HELP_MC_ARMOR);
			rects[17].setHelpID(0);
			rects[18].setHelpID(0);
			rects[19].setHelpID(0);
			rects[20].setHelpID(0);
		}
		else
		{
			int32_t rangecolours[3] = {0xff6e7c00, 0xff005392, 0xffa21600};
			showJumpJetItems(0);
			textObjects[7].setText(pCurComponent->getRangeType() + IDS_SHORT_RANGE);
			textObjects[7].setcolour(rangecolours[pCurComponent->getRangeType()]);
			attributeMeters[0].showGUIWindow(true);
			attributeMeters[0].setValue(
				pCurComponent->getDamage() / LogisticsComponent::MAX_DAMAGE);
			sprintf(text, "%.1lf", pCurComponent->getDamage());
			textObjects[8].setText(text);
			if (pCurComponent->getRecycleTime())
			{
				attributeMeters[1].setValue(
					(10.f / pCurComponent->getRecycleTime()) / LogisticsComponent::MAX_RECYCLE);
				sprintf(text, "%.1lf", 10.f / pCurComponent->getRecycleTime());
				textObjects[9].setText(text);
			}
			else
			{
				attributeMeters[1].setValue(0.f);
				sprintf(text, "%.1lf", 0.f);
				textObjects[9].setText(text);
			}
			attributeMeters[2].setValue(pCurComponent->getHeat() / LogisticsComponent::MAX_HEAT);
			sprintf(text, "%.1lf", pCurComponent->getHeat());
			textObjects[10].setText(text);
			attributeMeters[7].showGUIWindow(false);
			if (!useUnlimitedAmmo)
			{
				sprintf(text, "%ld", pCurComponent->getAmmo());
				textObjects[13].setText(text);
				attributeMeters[3].setValue(((float)pCurComponent->getAmmo()) / 164.f);
			}
			else
				textObjects[13].setText("");
			textObjects[15].setText("");
			rects[16].setHelpID(IDS_HELP_MC_RANGE);
			rects[17].setHelpID(IDS_HELP_MC_DAMAGE);
			rects[18].setHelpID(IDS_HELP_MC_FIRE_RATE);
			rects[19].setHelpID(IDS_HELP_MC_HEAT_GENERATED);
			if (!prefs.useUnlimitedAmmo)
				rects[20].setHelpID(IDS_HELP_MC_AMMO);
			else
				rects[20].setHelpID(0);
		}
	}
	else
	{
		for (size_t i = 7; i < 11; i++)
		{
			textObjects[i].setText("");
		}
		textObjects[13].setText("");
		for (i = 0; i < 5; i++)
		{
			attributeMeters[i].setValue(0);
			attributeMeters[i].showGUIWindow(0);
		}
	}
}

void MechLabScreen::showJumpJetItems(bool doshow)
{
	// show jump jet stuff
	statics[47].showGUIWindow(doshow);
	statics[48].showGUIWindow(doshow);
	statics[49].showGUIWindow(0);
	attributeMeters[4].showGUIWindow(doshow);
	attributeMeters[0].showGUIWindow(1);
	statics[30].showGUIWindow(!doshow); // damage
	textObjects[7].showGUIWindow(1);
	// hide all heat sink stuff, regardless of who
	attributeMeters[8].showGUIWindow(0);
	textObjects[16].showGUIWindow(0);
	statics[55].showGUIWindow(0);
	// turn off weapon stuff
	for (size_t i = 31; i < 35; i++)
	{
		statics[i].showGUIWindow(!doshow);
	}
	// but not the heat one!
	statics[33].showGUIWindow(true);
	// turn off ammo thing if appropriate
	if (!useUnlimitedAmmo && !doshow)
	{
		statics[34].showGUIWindow(1);
		attributeMeters[3].showGUIWindow(1);
	}
	else
	{
		statics[34].showGUIWindow(0);
		attributeMeters[3].showGUIWindow(0);
	}
	// weapon stuff
	for (i = 1; i < 3; i++)
	{
		attributeMeters[i].showGUIWindow(!doshow);
	}
}

int32_t
MechLabScreen::canAddComponent(LogisticsComponent* pComponent)
{
	if (!pVariant)
		return 0;
	int32_t i = -1;
	int32_t j = -1;
	int32_t retVal = pVariant->canAddComponent(pComponent, i, j);
	return retVal;
}

int32_t
MechLabScreen::addComponent(LogisticsComponent* pComponent, int32_t& x, int32_t& y)
{
	if (!pComponent)
		return -1;
	int32_t retVal = -1;
	if (pComponent->getHeat() || pComponent->getType() == COMPONENT_FORM_HEATSINK)
	{
		oldHeat = pVariant->getHeat();
		if (pComponent->getType() == COMPONENT_FORM_HEATSINK)
		{
			oldHeat -= 1;
			flashHeatRange = true;
		}
		else
			flashHeatRange = false;
		heatTime = .0001f;
	}
	else if (pComponent->getType() == COMPONENT_FORM_BULK)
	{
		oldArmor = pVariant->getArmor();
		armorTime = .0001f;
	}
	if (pComponent)
		retVal = pVariant->addComponent(pComponent, x, y);
	updateDiagram();
	return retVal;
}

void MechLabScreen::beginDrag(LogisticsComponent* pComponent)
{
	if (!pDragComponent)
	{
		pDragComponent = pComponent;
		if (pDragComponent)
		{
			// initialize the drag object
			int32_t sizeX = pComponent->getComponentwidth();
			int32_t sizeY = pComponent->getComponentheight();
			std::wstring_view pFile = pComponent->getIconFileName();
			FullPathFileName path;
			path.init(artPath, pFile, "tga");
			dragIcon.setTexture(path);
			dragIcon.resize(
				sizeX * LogisticsComponent::XICON_FACTOR, sizeY * LogisticsComponent::YICON_FACTOR);
			dragIcon.setUVs(0.f, 0.f, sizeX * 48.f, sizeY * 32.f);
			dragIcon.setcolour(0xffffffff);
			selRect = &selRects[sizeX - 1][sizeY - 1];
			if (pComponent->getType() == COMPONENT_FORM_JUMPJET)
			{
				selRect = &selJumpJetRect;
			}
		}
	}
}

void MechLabScreen::endDrag()
{
	float mouseX = userInput->getMouseX();
	float mouseY = userInput->getMouseY();
	if (pDragComponent && rects[13].pointInside(mouseX, mouseY))
	{
		int32_t x, y;
		// use selRect, it already has proper coords
		getMouseDiagramCoords(selRect->left(), selRect->top(), x, y);
		if (x == -2 && pDragComponent->getType() == COMPONENT_FORM_JUMPJET)
		{
			x = -2;
			y = -2;
			addComponent(pDragComponent, x, y);
		}
		else
		{
			if (NO_ERROR == addComponent(pDragComponent, x, y))
			{
				componentListBox.SelectItem(-1);
				selI = x;
				selJ = y;
				pSelectedComponent = pDragComponent;
			}
			if (bDragLeft) // put it bag, if we didn't drag far enough away
			{
				if (NO_ERROR == addComponent(pDragComponent, selI, selJ))
				{
					componentListBox.SelectItem(-1);
					pSelectedComponent = pDragComponent;
				}
			}
		}
		updateDiagram();
	}
	else
	{
		if (-1 == componentListBox.GetSelectedItem())
		{
			if (-1 == selI)
				selectFirstLBComponent();
		}
	}
	pDragComponent = nullptr;
	bDragLeft = 0;
}

void MechLabScreen::getMouseDiagramCoords(int32_t& x, int32_t& y)
{
	int32_t mouseX = userInput->getMouseX();
	int32_t mouseY = userInput->getMouseY();
	getMouseDiagramCoords(mouseX, mouseY, x, y);
}

void MechLabScreen::getMouseDiagramCoords(int32_t screenX, int32_t screenY, int32_t& x, int32_t& y)
{
	x = -1;
	y = -1;
	// get position under cursor
	int32_t width = pVariant->getComponentAreawidth();
	int32_t height = pVariant->getComponentAreaheight();
	int32_t minX = rects[7 + 6 - width].left();
	int32_t maxY = rects[7].bottom();
	int32_t minY = maxY - height * LogisticsComponent::YICON_FACTOR;
	int32_t maxX = minX + width * LogisticsComponent::XICON_FACTOR;
	int32_t mouseX = screenX;
	int32_t mouseY = screenY;
	// are we in the right area?
	if (mouseX >= rects[6].left() && mouseX <= rects[6].right() && mouseY >= rects[6].top() && mouseY <= rects[6].bottom() && pVariant->getChassis()->jumpJetsAllowed())
	{
		x = -2;
		y = -2;
	}
	else
	//		if ( mouseX >= minX && mouseX <= maxX
	//		&& mouseY >= minY && mouseY <= maxY )
	{
		// specificly, which square are we in
		x = (mouseX - minX) / LogisticsComponent::XICON_FACTOR;
		y = (mouseY - minY) / LogisticsComponent::YICON_FACTOR;
	}
}

void MechLabScreen::diagramToScreen(int32_t i, int32_t j, int32_t& x, int32_t& y)
{
	if (i == -2 && j == -2)
	{
		x = rects[6].left();
		y = rects[6].top();
		return;
	}
	if (!pVariant)
	{
		x = 0;
		y = 0;
		return;
	}
	int32_t height = pVariant->getComponentAreaheight();
	int32_t width = pVariant->getComponentAreawidth();
	int32_t minX = rects[7 + 6 - width].left();
	int32_t maxY = rects[7].bottom();
	int32_t minY = maxY - height * LogisticsComponent::YICON_FACTOR;
	x = minX + i * LogisticsComponent::XICON_FACTOR;
	y = minY + j * LogisticsComponent::YICON_FACTOR;
}

void MechLabScreen::updateDiagram()
{
	if (!pVariant || status != RUNNING)
		return;
	int32_t width = pVariant->getComponentAreawidth();
	int32_t minX = rects[7 + 6 - width].left();
	int32_t maxY = rects[7].bottom();
	int32_t minY = maxY - pVariant->getComponentAreaheight() * LogisticsComponent::YICON_FACTOR;
	int32_t IDArray[128];
	int32_t xLocs[128];
	int32_t yLocs[128];
	componentCount = 128;
	pVariant->getComponentsWithLocation(componentCount, IDArray, xLocs, yLocs);
	for (size_t i = 0; i < componentCount; i++)
	{
		LogisticsComponent* pComponent = LogisticsData::instance->getComponent(IDArray[i]);
		int32_t xLoc = minX + xLocs[i] * LogisticsComponent::XICON_FACTOR;
		int32_t yLoc = minY + yLocs[i] * LogisticsComponent::YICON_FACTOR;
		if (pComponent->getType() == COMPONENT_FORM_JUMPJET)
		{
			xLoc = rects[6].left();
			yLoc = rects[6].top();
		}
		int32_t sizeX = pComponent->getComponentwidth();
		int32_t sizeY = pComponent->getComponentheight();
		std::wstring_view pFile = pComponent->getIconFileName();
		FullPathFileName path;
		path.init(artPath, pFile, "tga");
		componentIcons[i].setTexture(path);
		componentIcons[i].resize(
			sizeX * LogisticsComponent::XICON_FACTOR, sizeY * LogisticsComponent::YICON_FACTOR);
		componentIcons[i].setUVs(0.f, 0.f, sizeX * 48.f, sizeY * 32.f);
		componentIcons[i].moveTo(xLoc, yLoc);
		componentIcons[i].setcolour(0xffffffff);
	}
	// update heat
	updateHeatMeter();
	updateArmorMeter();
	// update cost
	float newCost = pVariant->getCost();
	// figure out change
	float costChange = newCost - originalCost;
	if (costChange != oldCBillsAmount)
	{
		previousAmount = oldCBillsAmount - costChange;
		curCount = .00001f;
		oldCBillsAmount = costChange;
		if (previousAmount < 0)
			soundSystem->playDigitalSample(WINDOW_OPEN);
		else
			soundSystem->playDigitalSample(WINDOW_CLOSE);
	}
}

int32_t
MechLabScreen::selectFirstDiagramComponent()
{
	if (!pVariant)
		return -1;
	int32_t count = 0;
	bool bFound = 0;
	componentListBox.SelectItem(-1);
	for (size_t j = 0; j < pVariant->getComponentAreaheight(); j++)
	{
		for (size_t i = 0; i < pVariant->getComponentAreawidth(); i++)
		{
			LogisticsComponent* pComp = pVariant->getCompAtLocation(i, j, selI, selJ);
			if (pComp)
			{
				count++;
				bFound = true;
				pSelectedComponent = pComp;
				camera.setComponent(pComp->getPictureFileName());
				break;
			}
		}
		if (bFound)
			break;
	}
	if (!bFound)
	{
		selI = -1;
		selJ = -1;
		return -1;
	}
	return count;
}
int32_t
MechLabScreen::selectFirstLBComponent()
{
	selI = selJ = -1;
	pSelectedComponent = nullptr;
	int32_t index = componentListBox.selectFirstAvailableComponent();
	if (index != -1)
	{
		LogisticsComponent* pComp = componentListBox.getComponent();
		camera.setComponent(pComp->getPictureFileName());
	}
	return index;
}
void MechLabScreen::removeComponent(int32_t i, int32_t j)
{
	int32_t tmpX, tmpY;
	LogisticsComponent* pComp = pVariant->getCompAtLocation(i, j, tmpX, tmpY);
	if (pComp)
	{
		if (!pComp->isAvailable())
		{
			LogisticsOneButtonDialog::instance()->setText(
				IDS_MC_UNAVAILABLE_TECH, IDS_DIALOG_OK, IDS_DIALOG_OK);
			LogisticsOneButtonDialog::instance()->begin();
			bErrorDlg = true;
		}
	}
	int32_t tmpHeat = pVariant->getHeat();
	int32_t tmpArmor = pVariant->getArmor();
	pVariant->removeComponent(i, j);
	if (tmpHeat != pVariant->getHeat())
	{
		oldHeat = tmpHeat;
		heatTime = .0001f;
	}
	else if (tmpArmor != pVariant->getArmor())
	{
		oldArmor = tmpArmor;
		armorTime = .0001f;
	}
	updateDiagram();
}
bool MechLabScreen::canRemoveComponent(LogisticsComponent* pComp)
{
	if (pComp && pComp->getType() == COMPONENT_FORM_HEATSINK)
	{
		if (pVariant->getMaxHeat() - pComp->getDamage() < pVariant->getHeat())
			return 0;
	}
	return 1;
}

// end of file ( MechLabScreen.cpp )
