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

	LoadScreenWrapper(void);

	virtual ~LoadScreenWrapper(void);

	void init( FitIniFile& file );

	virtual void update(void);
	virtual void render( int32_t xOffset, int32_t yOffset );

	virtual void begin(void);

	static LoadScreen*	enterScreen;
	static LoadScreen*	exitScreen;
	static void			changeRes(void);
	bool				waitForResChange;
	bool				bFirstTime;
};


class LoadScreen: public LogisticsScreen
{
public:

	LoadScreen(void);
	virtual ~LoadScreen(void);

	void init( FitIniFile& file, uint32_t neverFlush = 0 );
	virtual void update(void);
	virtual void render( int32_t xOffset, int32_t yOffset );

	virtual void begin(void);

	void resetExitAnims(void);
	void setupOutAnims(void);

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
	pint32_t		animIndices;


};


//*************************************************************************************************
#endif  // end of file ( LoadScreen.h )
