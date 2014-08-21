/*************************************************************************************************\
MechIcon.h			: Interface for the MechIcon component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef MECHICON_H
#define MECHICON_H

#include <mclib.h>
#include "afont.h"
#include "mc2movie.h"

//*************************************************************************************************
class Mover;
class MechWarrior;

#define NUM_DEATH_INFOS 6
/**************************************************************************************************
CLASS DESCRIPTION
MechIcon:
**************************************************************************************************/
class PilotIcon
{

public:

	static const int32_t DEAD_PILOT_INDEX;

	PilotIcon();

	static void swapResolutions(bool bForce = 0);

	void setPilot( MechWarrior* pWarrior );
	void setTextureIndex(  int newIndex ) { pilotTextureIndex = newIndex; }
	void render( float left, float top, float right, float bottom );


private:

	ULONG pilotTextureIndex;
	
	static float			pilotIconX;
	static float			pilotIconY;

	static ULONG	s_pilotTextureHandle;
	static ULONG	s_pilotTextureWidth;

	friend class ForceGroupIcon;
	friend class MechIcon;
	friend class Mission;
};


class ForceGroupIcon
{
public:

		ForceGroupIcon();
		virtual ~ForceGroupIcon();

		virtual void update() = 0;
		virtual void render();
		void renderUnitIcon( float left, float top, float right, float bottom );
		virtual void renderUnitIconBack( float left, float top, float right, float bottom );
		void renderPilotIcon( float left, float top, float right, float bottom );
		virtual bool			init( Mover* pMover ){ return false; }
		void init();

		


		bool inRegion( int x, int y );

		void click( bool shiftDn );
		void rightClick();

		virtual void setDrawBack( bool bSet) { };

		static int __cdecl sort( PCVOID p1, PCVOID p2 );

		PCSTR getPilotName();

		static void init( FitIniFile& file, int which );
		void swapResolutions(bool bForce);
		static void resetResolution(bool bForce);

		void setLocationIndex( int i );
		void beginDeathAnimation( ){ deathAnimationTime = .0001f; } 
		bool deathAnimationOver(){ return deathAnimationTime > animationInfos[NUM_DEATH_INFOS-1].time; }
		bool isAnimatingDeath() { return deathAnimationTime ? 1 : 0; }

protected:

		static int32_t		damageColors[4][3];

		static ULONG	s_textureHandle[5];
		static bool				s_slotUsed[240];
		static TGAFileHeader*	s_textureMemory;

		static float			unitIconX;
		static float		 	unitIconY;
		static int				curScreenWidth;

		static StaticInfo*		jumpJetIcon;

		static aFont*			gosFontHandle;
		
		static gos_VERTEX	bmpLocation[17][5]; // in screen coords
		
		static RECT textArea[17];
		static RECT pilotRect[17];
		static RECT selectionRect[17];
		static RECT healthBar[17];
		static int32_t		pilotTextTop[17];

		static float	pilotLocation[17][4];
		Mover*		unit;

		static int32_t		ForceGroupColors[11];

		void drawBar( float barStatus );
		static void setIconVariables();


		friend class ForceGroupBar;
		friend void __stdcall TerminateGameEngine(void);

		bool	bDrawBack;

		int		damageIconIndex;
		int		backDamageIndex;
		int		locationIndex;

		float	deathAnimationTime;

		struct AnimationInfo
		{
			float	time;
			int32_t	color;
		};

		static AnimationInfo animationInfos[NUM_DEATH_INFOS];

		void drawDeathEffect();

		PilotIcon	pilotIcon;

		float		msgPlayTime;

		static		MC2MoviePtr 	bMovie;
		static		ULONG			pilotVideoTexture;
		static		MechWarrior*	pilotVideoPilot;
		
};

class MechIcon : public ForceGroupIcon
{
	public:

		MechIcon(){}
		~MechIcon(){}

		virtual bool			init( Mover* pMover );
		virtual void			update();

		static TGAFileHeader* s_MechTextures;

		void doDraw( PSTR newDamage, PSTR oldDamage, ULONG handle, ULONG where);
		virtual void setDrawBack( bool bSet);

		

		

	private:

		char	damage[8];
		char	backDamage[8];
		bool init( int32_t whichIndex );
		bool initTextures();

		
};

class VehicleIcon : public ForceGroupIcon
{
public:

	VehicleIcon(){}
	~VehicleIcon(){}

	virtual void renderUnitIconBack( float left, float top, float right, float bottom ){}
	virtual void	update();
	virtual bool			init( Mover* pMover );

	static TGAFileHeader* s_VehicleTextures;

private:

		char	damage[5];

};



//*************************************************************************************************
#endif  // end of file ( MechIcon.h )
