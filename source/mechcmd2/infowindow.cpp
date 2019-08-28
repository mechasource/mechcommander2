/*************************************************************************************************\
InfoWindow.cpp			: Implementation of the InfoWindow component.
//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#define INFOWINDOW_CPP

#include "stdinc.h"
#include "infowindow.h"
#include "mclib.h"
#include "resource.h"
#include "mover.h"
#include "mechicon.h"
#include "team.h"
#include "txmmgr.h"
#include "estring.h"

int32_t InfoWindow::SCROLLLEFT = 0;
int32_t InfoWindow::SCROLLRIGHT = 0;
int32_t InfoWindow::SCROLLTOP = 0;
int32_t InfoWindow::SCROLLBOTTOM = 0;
int32_t InfoWindow::SECTIONSKIP = 0;
int32_t InfoWindow::NAMELEFT = 0;
int32_t InfoWindow::NAMERIGHT = 0;
int32_t InfoWindow::NAMETOP = 0;
int32_t InfoWindow::NAMEBOTTOM = 0;
int32_t InfoWindow::HEALTHLEFT = 0;
int32_t InfoWindow::HEALTHRIGHT = 0;
int32_t InfoWindow::HEALTHTOP = 0;
int32_t InfoWindow::HEALTHBOTTOM = 0;
int32_t InfoWindow::DIVIDERCOLOR = 0;
int32_t InfoWindow::DIVIDERLEFT = 0;
int32_t InfoWindow::DIVIDERRIGHT = 0;
int32_t InfoWindow::PILOTLEFT = 0;
int32_t InfoWindow::PILOTRIGHT = 0;
int32_t InfoWindow::PILOTHEIGHT = 0;
int32_t InfoWindow::MECHLEFT = 0;
int32_t InfoWindow::MECHRIGHT = 0;
int32_t InfoWindow::MECHHEIGHT = 0;
int32_t InfoWindow::MECHBACKLEFT = 0;
int32_t InfoWindow::MECHBACKRIGHT = 0;
int32_t InfoWindow::MECHBACKHEIGHT = 0;
int32_t InfoWindow::SCROLLBUTTONV = 0;
StaticInfo* InfoWindow::skillInfos = 0;
int32_t InfoWindow::SCROLLBUTTONU = 0;
int32_t InfoWindow::SCROLLBUTTONHEIGHT = 0;
int32_t InfoWindow::SCROLLBUTTONWIDTH = 0;
int32_t InfoWindow::SCROLLBUTTONX = 0;
int32_t InfoWindow::SCROLLMAX = 0;
int32_t InfoWindow::SCROLLMIN = 0;
int32_t InfoWindow::NUMBERSKILLBARS = 0;
int32_t InfoWindow::SKILLUNITWIDTH = 0;
int32_t InfoWindow::SKILLSKIP = 0;
int32_t InfoWindow::SKILLHEIGHT = 0;
int32_t InfoWindow::SKILLRIGHT = 0;
int32_t InfoWindow::SKILLLEFT = 0;
int32_t InfoWindow::SCROLLCOLOR = -1;
int32_t InfoWindow::INFOHEIGHT = 0;
int32_t InfoWindow::INFOWIDTH = 0;
int32_t InfoWindow::INFOTOP = 0;
int32_t InfoWindow::INFOLEFT = 0;
int32_t InfoWindow::COMPONENTLEFT = 0;

int32_t InfoWindow::SCROLLBOXLEFT = 0;
int32_t InfoWindow::SCROLLBOXRIGHT = 0;
int32_t InfoWindow::SCROLLBOXTOP = 0;
int32_t InfoWindow::SCROLLBOXBOTTOM = 0;
int32_t InfoWindow::PILOTNAMELEFT = 0;

RECT InfoWindow::NameRect = {0};

ControlButton InfoWindow::buttons[2] = {0};

InfoWindow* InfoWindow::s_instance = nullptr;

#define SCROLLAMOUNT 3.f * Environment.screenwidth / 640.f
extern float WeaponRanges[NUM_WEAPON_RANGE_TYPES][2];

// initialize statics
ButtonData InfoWindow::buttonData[2];
;

extern bool useUnlimitedAmmo;

InfoWindow::InfoWindow()
{
	pUnit = nullptr;
	scrollLength = 0;
	scrollPos = 0;
	backgroundTexture = 0;
	icon = nullptr;
	infoLength = 0;
	skillInfos = new StaticInfo[7];
	memset(skillInfos, 0, sizeof(StaticInfo) * 7);
	memset(buttonData, 0, sizeof(buttonData));
	lastYClick = -1.f;
	s_instance = this;
}

void
InfoWindow::init(FitIniFile& file)
{
	if (NO_ERROR != file.seekBlock("Fonts"))
		Assert(0, 0, "couldn't find the font block");
	for (size_t i = 0; i < 7; i++)
	{
		if (skillInfos[i].textureHandle)
		{
			int32_t gosID = mcTextureManager->get_gosTextureHandle(skillInfos[i].textureHandle);
			mcTextureManager->removeTexture(gosID);
		}
	}
	memset(skillInfos, 0, sizeof(skillInfos));
	memset(buttonData, 0, sizeof(buttonData));
	for (i = 0; i < 2; i++)
	{
		if (buttonData[i].textureHandle)
		{
			int32_t gosID = mcTextureManager->get_gosTextureHandle(buttonData[i].textureHandle);
			mcTextureManager->removeTexture(gosID);
		}
	}
	int32_t fontID;
	if (NO_ERROR != file.readIdLong("InfoWndFont", fontID))
	{
		Assert(0, 0, "couldn't find infoWndFont in button layout file");
	}
	s_instance->nameFont.init(fontID);
	if (NO_ERROR != file.readIdLong("ComponentFont", fontID))
	{
		Assert(0, 0, "couldn't find componentFont in button layout file");
	}
	s_instance->componentFont.init(fontID);
	if (NO_ERROR != file.seekBlock("InfoWindow"))
	{
		Assert(0, 0, "couldn't find Info block in button layout file");
	}
	file.readIdLong("XLocation", InfoWindow::INFOLEFT);
	file.readIdLong("YLocation", InfoWindow::INFOTOP);
	file.readIdLong("width", InfoWindow::INFOWIDTH);
	file.readIdLong("height", InfoWindow::INFOHEIGHT);
	InfoWindow::INFOLEFT += ControlGui::hiResOffsetX;
	InfoWindow::INFOTOP += ControlGui::hiResOffsetY;
	file.readIdLong("SkipBetweenSections", InfoWindow::SECTIONSKIP);
	file.readIdLong("InfoHeaderBoxLeft", NameRect.left);
	file.readIdLong("InfoHeaderBoxRight", NameRect.right);
	file.readIdLong("InfoHeaderBoxTop", NameRect.top);
	file.readIdLong("InfoHeaderBoxBottom", NameRect.bottom);
	NameRect.left += ControlGui::hiResOffsetX;
	NameRect.right += ControlGui::hiResOffsetX;
	NameRect.top += ControlGui::hiResOffsetY;
	NameRect.bottom += ControlGui::hiResOffsetY;
	file.readIdLong("ScrollBarToP", InfoWindow::SCROLLTOP);
	file.readIdLong("ScrollBarBottom", InfoWindow::SCROLLBOTTOM);
	file.readIdLong("ScrollBarLeft", InfoWindow::SCROLLLEFT);
	file.readIdLong("ScrollBarRight", InfoWindow::SCROLLRIGHT);
	file.readIdLong("ScrollBarcolour", InfoWindow::SCROLLCOLOR);
	InfoWindow::SCROLLTOP += ControlGui::hiResOffsetY;
	InfoWindow::SCROLLBOTTOM += ControlGui::hiResOffsetY;
	InfoWindow::SCROLLLEFT += ControlGui::hiResOffsetX;
	InfoWindow::SCROLLRIGHT += ControlGui::hiResOffsetX;
	file.readIdLong("ScrollContentBoxLeft", InfoWindow::SCROLLBOXLEFT);
	file.readIdLong("ScrollContentBoxRight", InfoWindow::SCROLLBOXRIGHT);
	file.readIdLong("ScrollContentBoxBarTop", InfoWindow::SCROLLBOXTOP);
	file.readIdLong("ScrollContentBoxBarBottom", InfoWindow::SCROLLBOXBOTTOM);
	InfoWindow::SCROLLBOXLEFT += ControlGui::hiResOffsetX;
	InfoWindow::SCROLLBOXRIGHT += ControlGui::hiResOffsetX;
	InfoWindow::SCROLLBOXTOP += ControlGui::hiResOffsetY;
	InfoWindow::SCROLLBOXBOTTOM += ControlGui::hiResOffsetY;
	file.readIdLong("NameLocationLeft", InfoWindow::NAMELEFT);
	file.readIdLong("NameLocationRight", InfoWindow::NAMERIGHT);
	file.readIdLong("NameLocationTop", InfoWindow::NAMETOP);
	file.readIdLong("NameLocationBottom", InfoWindow::NAMEBOTTOM);
	InfoWindow::NAMELEFT += ControlGui::hiResOffsetX;
	InfoWindow::NAMERIGHT += ControlGui::hiResOffsetX;
	InfoWindow::NAMETOP += ControlGui::hiResOffsetY;
	InfoWindow::NAMEBOTTOM += ControlGui::hiResOffsetY;
	file.readIdLong("HealthBarLeft", InfoWindow::HEALTHLEFT);
	file.readIdLong("HealthBarRight", InfoWindow::HEALTHRIGHT);
	file.readIdLong("HealthBarTop", InfoWindow::HEALTHTOP);
	file.readIdLong("HealthBarBottom", InfoWindow::HEALTHBOTTOM);
	InfoWindow::HEALTHLEFT += ControlGui::hiResOffsetX;
	InfoWindow::HEALTHRIGHT += ControlGui::hiResOffsetX;
	InfoWindow::HEALTHTOP += ControlGui::hiResOffsetY;
	InfoWindow::HEALTHBOTTOM += ControlGui::hiResOffsetY;
	file.readIdLong("DividerLeft", InfoWindow::DIVIDERLEFT);
	file.readIdLong("DividerRight", InfoWindow::DIVIDERRIGHT);
	file.readIdLong("Dividercolour", InfoWindow::DIVIDERCOLOR);
	InfoWindow::DIVIDERLEFT += ControlGui::hiResOffsetX;
	InfoWindow::DIVIDERRIGHT += ControlGui::hiResOffsetX;
	file.readIdLong("PilotLeft", InfoWindow::PILOTLEFT);
	file.readIdLong("PilotRight", InfoWindow::PILOTRIGHT);
	InfoWindow::PILOTLEFT += ControlGui::hiResOffsetX;
	InfoWindow::PILOTRIGHT += ControlGui::hiResOffsetX;
	file.readIdLong("Pilotheight", InfoWindow::PILOTHEIGHT);
	file.readIdLong("PilotNameLeft", PILOTNAMELEFT);
	PILOTNAMELEFT += ControlGui::hiResOffsetX;
	file.readIdLong("MechIconLeft", InfoWindow::MECHLEFT);
	file.readIdLong("MechIconRight", InfoWindow::MECHRIGHT);
	file.readIdLong("MechIconheight", InfoWindow::MECHHEIGHT);
	file.readIdLong("MechIconBackLeft", InfoWindow::MECHBACKLEFT);
	file.readIdLong("MechIconBackRight", InfoWindow::MECHBACKRIGHT);
	file.readIdLong("MechIconBackheight", InfoWindow::MECHBACKHEIGHT);
	InfoWindow::MECHLEFT += ControlGui::hiResOffsetX;
	InfoWindow::MECHRIGHT += ControlGui::hiResOffsetX;
	InfoWindow::MECHBACKLEFT += ControlGui::hiResOffsetX;
	InfoWindow::MECHBACKRIGHT += ControlGui::hiResOffsetX;
	file.seekBlock("SkillMeter");
	file.readIdLong("Left", InfoWindow::SKILLLEFT);
	file.readIdLong("Right", InfoWindow::SKILLRIGHT);
	InfoWindow::SKILLLEFT += ControlGui::hiResOffsetX;
	InfoWindow::SKILLRIGHT += ControlGui::hiResOffsetX;
	file.readIdLong("height", InfoWindow::SKILLHEIGHT);
	file.readIdLong("Skip", InfoWindow::SKILLSKIP);
	file.readIdLong("Unitwidth", InfoWindow::SKILLUNITWIDTH);
	file.readIdLong("NumberUnits", InfoWindow::NUMBERSKILLBARS);
	file.seekBlock("ScrollButton");
	file.readIdLong("Min", SCROLLMIN);
	file.readIdLong("Max", SCROLLMAX);
	file.readIdLong("XLocation", SCROLLBUTTONX);
	SCROLLMIN += ControlGui::hiResOffsetY;
	SCROLLMAX += ControlGui::hiResOffsetY;
	SCROLLBUTTONX += ControlGui::hiResOffsetX;
	file.readIdLong("width", SCROLLBUTTONWIDTH);
	file.readIdLong("height", SCROLLBUTTONHEIGHT);
	file.readIdLong("UNormal", SCROLLBUTTONU);
	file.readIdLong("VNormal", SCROLLBUTTONV);
	ControlButton::initButtons(file, 2, buttons, buttonData, "InfoButton");
	wchar_t SkillText[32];
	for (i = 0; i < 7; i++)
	{
		sprintf(SkillText, "Skill%ld", i);
		skillInfos[i].init(file, SkillText, ControlGui::hiResOffsetX, ControlGui::hiResOffsetY);
	}
	InfoWindow::COMPONENTLEFT = InfoWindow::PILOTLEFT;
}

InfoWindow::~InfoWindow()
{
	delete[] skillInfos;
}

void
InfoWindow::setUnit(Mover* pNewMover)
{
	if (pNewMover && (pNewMover->getTeamId() != Team::home->getId() && (CONTACT_VISUAL != pNewMover->getContactStatus(Team::home->getId(), true)) && !pNewMover->isDisabled()))
	{
		return;
	}
	if (pUnit != pNewMover)
	{
		pUnit = pNewMover;
		bUnitChanged = true;
		if (icon)
		{
			delete icon;
			icon = nullptr;
		}
		icon = nullptr;
		scrollPos = 0;
		infoLength = 0;
		if (pNewMover)
		{
			if (pNewMover->getObjectType()->getObjectTypeClass() == BATTLEMECH_TYPE)
			{
				MechIcon* pIcon = new MechIcon;
				pIcon->setLocationIndex(16);
				pIcon->init(pNewMover);
				pIcon->setDrawBack(true);
				icon = pIcon;
			}
			else
			{
				VehicleIcon* pIcon = new VehicleIcon;
				pIcon->setLocationIndex(16);
				pIcon->setDrawBack(1);
				pIcon->init(pNewMover);
				icon = pIcon;
			}
		}
	}
}

void
InfoWindow::drawName(const std::wstring_view& name)
{
	RECT rect = {NAMELEFT, NAMETOP, NAMERIGHT, SCROLLTOP};
	drawRect(rect, 0xff000000);
	drawEmptyRect(NameRect, 0xff002f55, 0xff002f55);
	nameFont.render(
		name, NAMELEFT, NAMETOP, NAMERIGHT - NAMELEFT, NAMEBOTTOM - NAMETOP, 0xff5c96c2, 0, 3);
}

void
InfoWindow::render()
{
	drawScrollingStuff();
	RECT tmpRect = {SCROLLBOXLEFT, SCROLLBOXBOTTOM, SCROLLBOXRIGHT, INFOTOP + INFOHEIGHT};
	drawRect(tmpRect, 0xff000000);
	for (size_t i = 0; i < 2; i++)
	{
		if (buttons[i].isEnabled())
			buttons[i].render();
	}
	int32_t scrollBarLength =
		buttons[1].location[0].y - buttons[0].location[2].y - 4 - SCROLLBUTTONHEIGHT;
	gos_VERTEX v[4];
	for (i = 0; i < 4; i++)
	{
		v[i].argb = 0xff5c96c2;
		v[i].frgb = 0;
		v[i].rhw = .5;
		v[i].x = SCROLLBUTTONX;
		v[i].y = SCROLLMIN + scrollPos;
		v[i].z = 0.f;
		v[i].u = v[i].v = 0.f;
		//		v[i].u = ((float)SCROLLBUTTONU)/256.f + .1/256.f;
		//		v[i].v = ((float)SCROLLBUTTONV)/256.f + .1/256.f ;
	}
	if (infoLength < scrollBarLength || !pUnit)
	{
		buttons[0].disable(1);
		buttons[1].disable(1);
		SCROLLMAX = 0;
	}
	else
	{
		buttons[0].disable(0);
		buttons[1].disable(0);
		float physicalRange = buttons[1].location[0].y - buttons[0].location[2].y;
		float buttonheight = SCROLLBUTTONHEIGHT;
		float RealRange = infoLength;
		buttonheight = physicalRange * physicalRange / (physicalRange + RealRange);
		if (buttonheight < SCROLLBUTTONHEIGHT)
			buttonheight = SCROLLBUTTONHEIGHT;
		SCROLLMAX = buttons[1].location[0].y - buttonheight - 2;
		v[2].x = v[3].x = SCROLLBUTTONX + SCROLLBUTTONWIDTH;
		v[1].y = v[2].y = SCROLLBOTTOM;
		v[1].y = v[2].y = v[0].y + buttonheight;
		//	v[2].u = v[3].u = .1/256.f + (float)(SCROLLBUTTONU +
		//((float)SCROLLBUTTONWIDTH))/256.f; 	v[1].v = v[2].v = .1/256.f +
		//(float)(SCROLLBUTTONV + ((float)SCROLLBUTTONHEIGHT))/256.f;
		gos_SetRenderState(gos_State_Texture, 0);
		gos_DrawQuads(v, 4);
	}
	// draw the name of the unit
	if (pUnit)
	{
		drawName(pUnit->getIfaceName());
		gos_SetRenderState(gos_State_Texture, 0);
		// draw the health bar
		uint32_t color;
		float barStatus = pUnit->getAppearance()->barStatus;
		if (barStatus > 1.0f)
			barStatus = 1.0f;
		if (barStatus >= 0.5)
			color = SB_GREEN;
		else if (barStatus > 0.2)
			color = SB_YELLOW;
		else
			color = SB_RED;
		v[0].x = v[1].x = HEALTHLEFT;
		v[2].x = v[3].x = HEALTHRIGHT;
		v[0].y = v[3].y = HEALTHTOP;
		v[1].y = v[2].y = HEALTHBOTTOM;
		uint32_t dimcolour = (color & 0xff7f7f7f);
		for (size_t i = 0; i < 4; i++)
			v[i].argb = dimcolour;
		gos_DrawQuads(v, 4);
		v[2].x = v[3].x = HEALTHLEFT + (HEALTHRIGHT - HEALTHLEFT) * barStatus;
		for (i = 0; i < 4; i++)
			v[i].argb = color | 0xff000000;
		gos_DrawQuads(v, 4);
	}
	else
	{
		wchar_t noUnit[256];
		cLoadString(IDS_NOUNIT, noUnit, 255);
		drawName(noUnit);
	}
	RECT border = {SCROLLBOXLEFT, SCROLLBOXTOP, SCROLLBOXRIGHT, SCROLLBOXBOTTOM};
	drawEmptyRect(border, SCROLLCOLOR, SCROLLCOLOR);
	RECT rect = {SCROLLLEFT, SCROLLTOP, SCROLLRIGHT, SCROLLBOTTOM};
	drawEmptyRect(rect, SCROLLCOLOR, SCROLLCOLOR);
}

void
InfoWindow::update()
{
	if (pUnit && (pUnit->getTeamId() != Team::home->getId() && (CONTACT_VISUAL != (pUnit->getContactStatus(Team::home->getId(), true)) && !pUnit->isDisabled())))
	{
		setUnit(0);
		return;
	}
	int32_t mouseX = userInput->getMouseX();
	int32_t mouseY = userInput->getMouseY();
	if (icon)
		icon->update();
	for (size_t i = 0; i < 2; i++)
	{
		if (buttons[i].location[0].x <= mouseX && mouseX <= buttons[i].location[2].x && mouseY >= buttons[i].location[0].y && mouseY <= buttons[i].location[1].y)
		{
			if (userInput->getMouseLeftButtonState() == MC2_MOUSE_DOWN && buttons[i].isEnabled())
			{
				{
					handleClick(buttons[i].ID);
					return;
				}
			}
		}
	}
	if (userInput->isLeftDrag())
	{
		if (lastYClick != -1.f)
		{
			int32_t tmpLastY = mouseY;
			tmpLastY -= userInput->getMouseDragY();
			setScrollPos(lastYClick + tmpLastY);
			return;
		}
	}
	// see if its in the scroll bar area
	else if (mouseX > SCROLLLEFT && mouseX < SCROLLRIGHT && mouseY > SCROLLTOP && mouseY < SCROLLBOTTOM && buttons[0].isEnabled() && userInput->getMouseDragX() > SCROLLLEFT && userInput->getMouseDragX() < SCROLLRIGHT && userInput->getMouseDragY() > SCROLLTOP && userInput->getMouseDragY() < SCROLLBOTTOM)
	{
		// if its in the thumbdrag thingie, save the y
		float physicalRange = buttons[1].location[0].y - buttons[0].location[2].y;
		float buttonheight = SCROLLBUTTONHEIGHT;
		float RealRange = infoLength;
		buttonheight = physicalRange * physicalRange / (physicalRange + RealRange);
		if (buttonheight < SCROLLBUTTONHEIGHT)
			buttonheight = SCROLLBUTTONHEIGHT;
		if (mouseY > scrollPos + SCROLLMIN && mouseY < scrollPos + SCROLLMIN + buttonheight)
		{
			lastYClick = scrollPos;
		}
		else if ((userInput->leftMouseReleased() || userInput->getMouseLeftHeld() > .5))
		{
			lastYClick = -1;
			if (mouseY > buttons[0].location[2].y && mouseY < buttons[1].location[0].y)
			{
				float newScrollPos = scrollPos;
				// if above the thumb, page up, otherwise page down
				if (mouseY < SCROLLMIN + scrollPos)
				{
					newScrollPos = scrollPos - buttonheight;
				}
				else if (mouseY > SCROLLMIN + scrollPos + buttonheight)
				{
					newScrollPos = scrollPos + buttonheight;
				}
				if (newScrollPos < 0)
					newScrollPos = 0;
				if (newScrollPos > infoLength)
					newScrollPos = infoLength;
				setScrollPos(newScrollPos);
			}
		}
	}
	if (userInput->leftMouseReleased())
		lastYClick = -1;
}

void
InfoWindow::drawScrollingStuff()
{
	if (!pUnit)
		return;
	float offset = 0.f;
	if (infoLength)
	{
		float increment = infoLength / (SCROLLMAX - SCROLLMIN);
		offset = -increment * scrollPos;
	}
	float curY = SCROLLBOXTOP + SECTIONSKIP + offset;
	if (icon)
	{
		if (SCROLLTOP + offset > NAMETOP) // draw icons if visible
		{
			icon->renderUnitIcon(MECHLEFT, curY, MECHRIGHT, curY + MECHHEIGHT);
			icon->renderUnitIconBack(MECHBACKLEFT, curY, MECHBACKRIGHT, curY + MECHHEIGHT);
		}
		curY += MECHHEIGHT + SECTIONSKIP;
		if (curY > NAMEBOTTOM) // draw divider if visible
			drawDivider(curY);
	}
	int32_t textcolours[4] = {0xff6E7C00, 0xff005392, 0xffA21600};
	wchar_t disabledCount[60][2];
	int32_t ammo[60];
	wchar_t ranges[60];
	int32_t names[60];
	memset(disabledCount, 0, sizeof(wchar_t) * 60 * 2);
	memset(names, 0, sizeof(const std::wstring_view&) * 60);
	memset(ranges, 0, sizeof(wchar_t) * 60);
	memset(ammo, 0, sizeof(int32_t) * 60);
	bool bDraw[4];
	memset(bDraw, 0, sizeof(bool) * 4);
	int32_t curComponentCount = 60;
	int32_t i = 0;
	for (size_t curWeapon = pUnit->numOther; curWeapon < (pUnit->numOther + pUnit->numWeapons);
		 curWeapon++)
	{
		int32_t nName = pUnit->inventory[curWeapon].masterID;
		bool bFound = 0;
		for (size_t j = 0; j < i; j++)
		{
			if (nName == names[j])
			{
				disabledCount[j][1]++;
				if (!pUnit->inventory[curWeapon].disabled && (pUnit->getWeaponShots(curWeapon) > 0) && pUnit->inventory[curWeapon].health > 0)
				{
					disabledCount[j][0]++;
				}
				bFound = true;
			}
		}
		if (bFound)
			continue;
		names[i] = nName;
		// ONly need to add in the AMMO once!!
		// Glenn has taken the liberty of combining all shots of this type
		// into one master ammo list!!
		if (MasterComponent::masterList[pUnit->inventory[curWeapon].masterID].getWeaponAmmoType() == WEAPON_AMMO_NONE || useUnlimitedAmmo)
			ammo[i] = -1;
		else
			ammo[i] += pUnit->getWeaponShots(curWeapon);
		int32_t range =
			MasterComponent::masterList[pUnit->inventory[curWeapon].masterID].getWeaponRange();
		ranges[i] = range;
		bDraw[range] = true;
		if (!pUnit->inventory[curWeapon].disabled && (pUnit->getWeaponShots(curWeapon) > 0) && pUnit->inventory[curWeapon].health > 0)
		{
			disabledCount[i][0]++;
		}
		disabledCount[i][1]++;
		i++;
	}
	curY += SECTIONSKIP;
	uint32_t height = componentFont.height();
	// removing headers for now
	//	int32_t stringIDs[4] = { IDS_SHORT, IDS_MEDIUM, IDS_LONG,
	// IDS_COMPONENT}; 	int32_t headercolours[4] = { 0xFFC8E100, 0xff0091FF,
	// 0xFFFF0000, 0xffFF8A00 };
	const std::wstring_view& capHeader;
	for (size_t j = 0; j < 3; j++)
	{
		if (!bDraw[j]) // make sure we have one
			continue;
		//		wchar_t header[64];
		if (curY > NAMETOP)
		{
			//			cLoadString( stringIDs[j], header, 63 );
			//			capHeader = header;
			// capHeader.MakeUpper();
			//			componentFont.render( capHeader, COMPONENTLEFT, curY,
			// SCROLLLEFT - COMPONENTLEFT, height, headercolours[j], 0, 0 );
		}
		//		curY += height;
		for (i = 0; i < curComponentCount; i++)
		{
			if (!names[i])
				break;
			wchar_t tmpName[256];
			if (ranges[i] == j)
			{
				if (curY > NAMETOP)
				{
					cLoadString(IDS_COMP_ABBR0 + names[i], tmpName, 255);
					capHeader.Format(
						"%ld/%ld  %s", disabledCount[i][0], disabledCount[i][1], tmpName);
					componentFont.render(capHeader, COMPONENTLEFT, curY, SCROLLLEFT - COMPONENTLEFT,
						height, textcolours[j], 0, 0);
				}
				curY += height;
				if (ammo[i] != -1)
				{
					if (curY > NAMETOP)
					{
						// make the ammo number
						wchar_t tmpNumber[64];
						wchar_t tmpNumber2[64];
						cLoadString(IDS_MISSION_SHOTSLEFT, tmpNumber, 63);
						sprintf(tmpNumber2, tmpNumber, ammo[i]);
						componentFont.render(tmpNumber2, COMPONENTLEFT, curY,
							SCROLLLEFT - COMPONENTLEFT, height, textcolours[j], 0, 0);
					}
					curY += height;
				}
			}
		}
		curY += SECTIONSKIP;
		if (curY > NAMEBOTTOM)
			drawDivider(curY);
		curY += SECTIONSKIP;
	}
	memset(names, 0, sizeof(const std::wstring_view&) * 60);
	int32_t count[4];
	count[0] = pUnit->ecm;
	count[1] = pUnit->probe;
	count[2] = pUnit->jumpJets;
	count[3] = pUnit->isMech() ? pUnit->sensor : 255;
	if ((count[0] || count[1] || count[2] || count[3]))
	{
		if (curY > NAMETOP)
		{
			//			cLoadString( stringIDs[j], header, 63 );
			//			componentFont.render( header, COMPONENTLEFT, curY,
			// SCROLLLEFT - COMPONENTLEFT, height, headercolours[j], 0, 0 );
		}
		//		curY += height;
	}
	for (curWeapon = 0; curWeapon < 4; curWeapon++)
	{
		if (count[curWeapon] != 255)
		{
			int32_t color = 0xffc29b00;
			// Neither the ecm, probe, sensors or JumpJets can ever REALLY be
			// disabled. No matter what the setting is!!
			//			if (pUnit->inventory[count[curWeapon]].disabled)
			//				color = 0xff7f7f7f;
			wchar_t tmpName[256];
			cLoadString(IDS_COMP_ABBR0 + pUnit->inventory[count[curWeapon]].masterID, tmpName, 255);
			if (curY > NAMETOP)
			{
				componentFont.render(
					tmpName, COMPONENTLEFT, curY, SCROLLLEFT - COMPONENTLEFT, height, color, 0, 0);
			}
			curY += height;
			curY += InfoWindow::SECTIONSKIP;
		}
	}
	if (curY > NAMEBOTTOM)
		drawDivider(curY);
	curY += SECTIONSKIP;
	// DON'T DO PILOT INFO if ENEMY OR mech is destroyed or disabled.
	if (pUnit->getTeam() && !Team::home->isEnemy(pUnit->getTeam()) && pUnit->isMech() && !pUnit->isDisabled() && !pUnit->isDestroyed())
	{
		MechWarrior* pWarrior = pUnit->getPilot();
		if (icon)
		{
			if (curY > NAMETOP)
			{
				if (pWarrior->active())
					icon->renderPilotIcon(PILOTLEFT, curY, PILOTRIGHT, curY + PILOTHEIGHT);
				RECT tmpRect = {PILOTLEFT, curY, PILOTRIGHT + 1, curY + PILOTHEIGHT + 1};
				drawEmptyRect(tmpRect, SCROLLCOLOR, SCROLLCOLOR);
				float right = SCROLLLEFT;
				float top = curY + PILOTHEIGHT / 2 - height / 2;
				float bottom = top + height;
				// draw the name of the pilot
				wchar_t deadPilotName[256];
				cLoadString(IDS_NOPILOT, deadPilotName, 255);
				capHeader = pWarrior->active() ? pWarrior->getName() : deadPilotName;
				componentFont.render(capHeader, PILOTNAMELEFT, top, right - PILOTNAMELEFT,
					bottom - top, 0xff005392, 0, 0);
			}
			curY += PILOTHEIGHT;
			curY += SECTIONSKIP;
		}
		curY += SECTIONSKIP;
		int32_t rank = pWarrior->getRank();
		int32_t skills[2] = {Skill::gunnery, Skill::piloting};
		wchar_t buffer[256];
		// ACE not continguous with other ranks.  Added too late!
		if (rank != 4)
			cLoadString(IDS_GREEN + rank, buffer, 256);
		else
			cLoadString(IDS_ACE, buffer, 256);
		if (curY > NAMETOP)
		{
			componentFont.render(
				buffer, SKILLLEFT, curY, SCROLLLEFT - SKILLLEFT, height, 0xff005392, 0, 0);
		}
		int32_t currentSkill = rank;
		for (j = 0; j < 3; j++)
		{
			gos_VERTEX v[4];
			float height =
				skillInfos[currentSkill].location[1].y - skillInfos[currentSkill].location[0].y;
			for (i = 0; i < 4; i++)
			{
				v[i] = skillInfos[currentSkill].location[i];
				v[i].y = curY;
				v[i].rhw = .5;
			}
			v[1].y = v[2].y = curY + height;
			if (curY > NAMETOP)
			{
				RECT tmpRect = {v[0].x - .5, v[0].y - .5, v[2].x + 1.5, v[2].y + 1.5};
				drawEmptyRect(tmpRect, 0xff002f55, 0xff002f55);
				uint32_t gosID =
					mcTextureManager->get_gosTextureHandle(skillInfos[currentSkill].textureHandle);
				gos_SetRenderState(gos_State_Texture, gosID);
				gos_DrawQuads(v, 4);
				if (j != 0)
				{
					int32_t skill = pWarrior->getSkill(skills[j - 1]);
					drawSkillBar(skill, curY, height);
				}
			}
			curY += height;
			curY += SECTIONSKIP;
			currentSkill = j + 5;
		}
		for (i = 0; i < NUM_SPECIALTY_SKILLS; i++)
		{
			if (pWarrior->specialtySkills[i])
			{
				if (curY > NAMETOP)
				{
					cLoadString(IDS_SPECIALTY + i, buffer, 256);
					componentFont.render(
						buffer, NAMELEFT, curY, NAMERIGHT - NAMELEFT, height, 0xff005392, 0, 0);
				}
				curY += height;
			}
		}
	}
	else
		curY += 10 * SECTIONSKIP;
	if (infoLength == 0)
		infoLength = curY - SCROLLTOP - (SCROLLBOTTOM - SCROLLTOP);
}

void
InfoWindow::handleClick(int32_t ID)
{
	switch (ID)
	{
	case SCROLLUP:
		setScrollPos(scrollPos - SCROLLAMOUNT);
		break;
	case SCROLLDOWN:
		setScrollPos(scrollPos + SCROLLAMOUNT);
		break;
	}
}

void
InfoWindow::drawDivider(float yVal)
{
	gos_VERTEX v[2];
	//	gos_SetRenderState( gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha );
	gos_SetRenderState(gos_State_Specular, FALSE);
	gos_SetRenderState(gos_State_AlphaTest, 0);
	gos_SetRenderState(gos_State_Texture, 0);
	gos_SetRenderState(gos_State_Filter, gos_FilterNone);
	memset(v, 0, sizeof(gos_VERTEX) * 2);
	for (size_t i = 0; i < 2; i++)
		v[i].rhw = .5f;
	v[0].x = DIVIDERLEFT;
	v[0].y = v[1].y = yVal;
	v[1].x = DIVIDERRIGHT;
	v[0].argb = v[1].argb = DIVIDERCOLOR;
	gos_DrawLines(v, 2);
}

void
InfoWindow::drawSkillBar(int32_t skill, float yVal, float height)
{
	float left = InfoWindow::SKILLLEFT;
	float right = InfoWindow::SKILLRIGHT;
	int32_t barCount = skill / InfoWindow::NUMBERSKILLBARS;
	int32_t redIncrement = 0;
	int32_t greenIncrement = 0;
	int32_t blueIncrement = 0;
	if (barCount)
	{
		redIncrement = (205 / barCount) << 16;
		greenIncrement = ((234 - 83) / barCount) << 8;
		blueIncrement = (255 - 146) / barCount;
	}
	uint32_t color = 0xff005392;
	RECT outSideRect = {
		left - SKILLSKIP + .5, yVal - .5, right + SKILLSKIP + .5, yVal + height + 1.5};
	drawRect(outSideRect, 0xff000000);
	RECT rect = {left + InfoWindow::SKILLSKIP, yVal + InfoWindow::SKILLSKIP + .5,
		left + InfoWindow::SKILLSKIP + SKILLUNITWIDTH, yVal + height - InfoWindow::SKILLSKIP + .5};
	drawEmptyRect(outSideRect, 0xff002f55, 0xff002f55);
	for (size_t i = 0; i < barCount; i++)
	{
		drawRect(rect, color);
		color += redIncrement;
		color += greenIncrement;
		color += blueIncrement;
		rect.left += SKILLUNITWIDTH + 1;
		rect.right = rect.left + SKILLUNITWIDTH;
	}
	wchar_t buffer[32];
	sprintf(buffer, "%ld", skill);
	componentFont.render(
		buffer, SKILLRIGHT + 2, yVal, SCROLLLEFT - SKILLRIGHT - 2, SKILLHEIGHT, 0xff005392, 0, 0);
}

void
InfoWindow::setScrollPos(int32_t where)
{
	if (where < 0)
		scrollPos = 0;
	else if (where > SCROLLMAX - SCROLLMIN)
		scrollPos = SCROLLMAX - SCROLLMIN;
	else
		scrollPos = where;
}

// end of file ( InfoWindow.cpp )
