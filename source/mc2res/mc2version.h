/*******************************************************************************
 Copyright (c) 2011-2014, Jerker Bäck. All rights reserved.

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
 mc2version.h - global unified scriptable version management

 MechCommander 2 source code

 2014-08-07 Jerker Bäck, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

#pragma once

#ifndef MC2VERSION_H
#define MC2VERSION_H

// wchar_t versionStamp[1024] = "00.06.01.0614";

#define MC2_VERSION_MAJOR 1
#define MC2_VERSION_MINOR 1
#define MC2_VERSION_RELEASE 1
#define MC2_VERSION_BUILD 1 // if 0 release or repository snapshot release
#define MC2_VERSION_BETA 1 // TRUE 1  or FALSE 0
#define MC2_VERSION_BUILD_COMMENT L"MechCommander 2 first patched beta build"

#endif
