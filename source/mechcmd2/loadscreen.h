/*************************************************************************************************\
LoadScreen.h			: Interface for the LoadScreen component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef LOADSCREEN_H
#define LOADSCREEN_H

//#include <mechgui/logisticsscreen.h>
//#include <mechgui/aanim.h>

struct TGAFileHeader;
class FitIniFile;

struct tagRECT;
struct _DDSURFACEDESC2;

//*************************************************************************************************
class LoadScreen;
/**************************************************************************************************
CLASS DESCRIPTION
LoadScreen:
**************************************************************************************************/
class LoadScreenWrapper : public LogisticsScreen
{
public:

	LoadScreenWrapper();

	virtual ~LoadScreenWrapper();

	void init( FitIniFile& file );

	virtual void update();
	virtual void render( int xOffset, int yOffset );

	virtual void begin();

	static LoadScreen*	enterScreen;
	static LoadScreen*	exitScreen;
	static void			changeRes();
	bool				waitForResChange;
	bool				bFirstTime;
};


class LoadScreen: public LogisticsScreen
{
public:

	LoadScreen();
	virtual ~LoadScreen();

	void init( FitIniFile& file, ULONG neverFlush = 0 );
	virtual void update();
	virtual void render( int xOffset, int yOffset );

	virtual void begin();

	void resetExitAnims();
	void setupOutAnims();

	static void changeRes(FitIniFile& file);



private:

	LoadScreen( const LoadScreen& src );
	LoadScreen& operator=( const LoadScreen& oadScreen );


	static TGAFileHeader*	progressTextureMemory;
	static TGAFileHeader*	progressBackground;
	static TGAFileHeader*	mergedTexture;
	static TGAFileHeader*	waitingForPlayersMemory;

	static int32_t xProgressLoc;
	static int32_t yProgressLoc;
	static int32_t xWaitLoc;
	static int32_t yWaitLoc;

	friend void ProgressTimer( tagRECT& WinRect, _DDSURFACEDESC2&  );
	friend class Mission;

	//Must needs be static cause we create a new one when the res changes!!
	// This will erase the hardmouse status and we always use async mouse then!
	static bool	turnOffAsyncMouse;

	aAnimation	outAnims[5];
	aAnimation	inAnims[5];
	aText		text;
	int*		animIndices;


};


//*************************************************************************************************
#endif  // end of file ( LoadScreen.h )
