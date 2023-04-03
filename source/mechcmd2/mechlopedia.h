
#pragma once

#ifndef MECHLOPEDIA_H
#define MECHLOPEDIA_H
/*************************************************************************************************\
mechlopedia.h			: Interface for the mechlopedia component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef LOGISTICSSCREEN_H
#include "mechgui/logisticsscreen.h"
#endif

#ifndef SIMPLECAMERA_H
#include "simplecamera.h"
#endif

#ifndef ALISTBOX_H
#include "mechgui/alistbox.h"
#endif

#ifndef COMPONENTLISTBOX_H
#include "simplecomponentlistbox.h"
#endif

#include "resource.h"

class LogisticsChassis;
class LogisticsVehicle;

/**************************************************************************************************
CLASS DESCRIPTION
mechlopedia:
**************************************************************************************************/

class Mechlopedia : public LogisticsScreen
{
public:
	Mechlopedia(void);
	virtual ~Mechlopedia(void);

	int32_t init(void);
	virtual int32_t handleMessage(uint32_t, uint32_t);
	virtual void update(void);
	virtual void render(void);
	virtual void begin(void);

private:
	LogisticsScreen* subScreens[6];
	int32_t currentScreen;
	aListBox listBox;

	class SubScreen : public LogisticsScreen
	{
	public:
		void setListBox(aListBox* pLB)
		{
			groupListBox = pLB;
		}
		int32_t init(FitIniFile& file);
		virtual void update(void);
		virtual void select(aTextListItem* pEntry) { }
		virtual void end(void);

		virtual void setVehicle(bool bVehicle) { }

	protected:
		aListBox* groupListBox;
		aListBox descriptionListBox;
		SimpleCamera camera;
	};

	class MechScreen : public SubScreen
	{

	public:
		void init(void);
		virtual void update(void);
		virtual void render(void);
		virtual void begin(void);

		virtual void select(aTextListItem* pEntry);
		void setMech(LogisticsVariant* pChassis, bool bShowJump);
		// set this before you call begin
		virtual void setVehicle(bool bVehicle)
		{
			bIsVehicle = bVehicle;
		}
		void setVehicle(LogisticsVehicle* pVehicle);

	private:
		ComponentListBox compListBox;
		aListBox statsListBox;
		bool bIsVehicle;
	};

	class WeaponScreen : public SubScreen
	{

	public:
		void init(void);
		virtual void update(void);
		virtual void render(void);
		virtual void begin(void);

		virtual void select(aTextListItem* pEntry);
		void setWeapon(LogisticsComponent* pWeapon);

	private:
		aListBox statsListBox;
	};

	class PersonalityScreen : public SubScreen
	{
	public:
		void init(void);
		virtual void update(void);
		virtual void render(void);
		virtual void begin(void);

		virtual void select(aTextListItem* pEntry);

		void setIsHistory(bool bTrue)
		{
			bIsHistory = bTrue;
		}

	private:
		bool bIsHistory;
	};

	class BuildingScreen : public SubScreen
	{
	public:
		void init(void);
		virtual void update(void);
		virtual void render(void);
		virtual void begin(void);

		virtual void select(aTextListItem* pEntry);

	private:
		ComponentListBox compListBox;
	};

	// HELPER FUNCTIONS
};

/////////////////////////////////////////////////////
class MechlopediaListItem : public aAnimTextListItem
{
public:
	MechlopediaListItem(void);
	virtual void render(void);

	static void init();

private:
	aObject bmp;
	aAnimGroup bmpAnim;

	static MechlopediaListItem* s_templateItem;
};

#endif // end of file ( mechlopedia.h )
