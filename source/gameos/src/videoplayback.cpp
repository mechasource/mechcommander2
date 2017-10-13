/*******************************************************************************
 Copyright (c) 2011-2014, Jerker Back. All rights reserved.

 Permission to use, copy, modify, and distribute this software for any
 purpose with or without fee is hereby granted, provided that the following
 conditions are met (OSI approved BSD 2-clause license):

 1. Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
/*******************************************************************************
 videoplayback.cpp - gameos reference pseudo code

 MechCommander 2 source code

 2014-07-24 jerker_back, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

#include "stdafx.h"

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
gos_Video::gos_Video(char*, bool);
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
