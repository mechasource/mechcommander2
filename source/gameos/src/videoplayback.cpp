/*******************************************************************************
 This file consist of reversed and interpreted code from object source in the
 x86 debug build gameos.lib. The code in this file can not be used in any way
 other than serve as an information reference to the original released source of
 Mechcommander2. The code is a work of progress and there is no guarantee it is
 complete, accurate or useful in any way. The purpose is instead to make it
 possible to safely remove any dependencies of gameos.lib from Mechcommander2.
 All code is logically copyrighted by Microsoft
*******************************************************************************/
/*******************************************************************************
 videoplayback.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Bäck, created

*******************************************************************************/

#include "stdinc.h"

// -----------------------------------------------------------------------------
// Global data exported from this module

// global referenced data not listed in headers

// local data

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers

// global implemented functions not listed in headers

// local functions

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers

VideoManagerInstall(void);
VideoManagerRestore(void);
VideoManagerRelease(void);
VideoManagerUpdate(void);
VideoManagerPause(void);
VideoManagerContinue(void);
VideoManagerUninstall(void);
gosVideo_CreateResource(gos_Video**, char*);
gosVideo_CreateResourceAsTexture(gos_Video**, ulong*, char*);
gosVideo_DestroyResource(gos_Video**);
gosVideo_GetResourceInfo(gos_Video*, _gosVideo_Info*);
gosVideo_Display(gos_Video*);
gosVideo_GetVideoPlayMode(gos_Video*);
gos_Video::OpenMMStream(char const*, IDirectDraw*, IMultiMediaStream**);
gos_Video::gos_Video(char*, BOOLEAN);
gos_Video::~gos_Video(void);
gos_Video::Update(void);
gos_Video::Stop(void);
gos_Video::Pause(void);
gos_Video::Continue(void);
gos_Video::SetLocation(ulong, ulong);
gosVideo_Command(gos_Video*, gosVideo_Command, float, float);
gosVideo_SetPlayMode(gos_Video*, gosVideo_PlayMode);
gos_Video::FF(double);
VideoManagerFF(double);
gos_Video::Release(void);
gos_Video::Restore(void);
