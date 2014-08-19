#pragma once
//===========================================================================//
// File:	 Sound Renderer.hpp												 //
// Contents: Direct Sound manager											 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "linkedlist.hpp"
#include "directx.hpp"
#include "sound_resource.hpp"
#include "sound_ds3dchannel.hpp"
#include "sound_ds3dmixer.hpp"

// This structure is used to initialize sound channels
//
// When GameOS is started, the init structure passed to GameOS by the application will contain a master frequency (11025,22050 or 44100), a number of channels (int) and a pointer to an array, a byte per channel containing flags listed above.
//
// Place to hold information about the sound renderer itself
//


//
// Used for sound device enumeration - Maximum of 8 devices
//
typedef struct SoundDeviceInfo {
	GUID				Guid;
	char				Description[128];
	char				Driver[128];
	char				Version[256];
	DSCAPS				Caps;
} SoundDeviceInfo;

#define MAX_SOUNDCHANNELS 64


typedef struct _srdata
{
	int m_numChannels;
	DS3DSoundChannel *
		m_Channel[MAX_SOUNDCHANNELS];
	DS3DSoundMixer *
		m_Mixer;
	LONG 
		m_RendererTimer;
	int
		m_isInitialized;
    UINT 
		m_Timer;
	UINT
		m_TimerRefCount;
	LONG 
		m_TimerServicing;             // reentrancy semaphore
	PSTR 
		m_workBuffer;

	SoundDeviceInfo m_DeviceArray[8];
	ULONG m_numDevices;
	ULONG m_PreferredDevice;
	bool m_reset;
	bool m_allPaused;
#if defined(LAB_ONLY)
	char m_playHistory[32][128];
	bool m_verboseDebugger;
	ULONG m_playHistoryItr;
#endif

} SoundRendererData;

extern HGOSHEAP SoundHeap;

void __stdcall SoundRendererInstall(int);
void __stdcall SoundRendererStartFrame();
void __stdcall SoundRendererEndFrame();
ULONG __stdcall SoundRendererUpdate( PVOID ThreadParam );
void __stdcall SoundRendererUninstall();
void __stdcall SoundRendererCreateTimer();
void __stdcall SoundRendererDestroyTimer();
void __stdcall SoundRendererPause();
void __stdcall SoundRendererContinue();
void __stdcall SoundRendererFF(double sec);
bool __stdcall IsValidSoundResource(HGOSAUDIO gosAudio);

void __stdcall TimeFunc( 
	UINT uTimerID, 
	UINT uMsg, 
	ULONG dwUser,
	ULONG dw1, 
	ULONG dw2 
);

void __stdcall SoundRendererNotify();

