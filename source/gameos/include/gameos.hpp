//===========================================================================//
// File:	 GameOS.hpp														 //
//																			 //
// This is the main header file for GameOS									 //
//																			 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//
// Pragmas needed to compile at Warning 4
//
//#pragma pack(push,4)
//#pragma warning( disable: 4725 )					// fdiv generates a warning
//#pragma warning( disable: 4127 )					// conditional expression is constant eg: Asserts
//will not work otherwise #pragma warning( disable: 4200 )
//// zero size array
//#pragma warning( disable: 4201 )					// Nameless struct or union
//#pragma warning( disable: 4514 )					// Unreferenced inline function

#pragma once

#ifndef _GAMEOS_HPP_
#define _GAMEOS_HPP_

#pragma warning(push)
#pragma warning(disable : 4820)

#ifndef MECH_IMPEXP
#define MECH_IMPEXP extern
#endif
#define MECH_CALL __stdcall

// for object files
#define LAB_ONLY 1
#define _DEBUG 1
#define _ARMOR 1

//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// **************************	  CODE ARMOR     ******************************
// //
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//
//
// Enter the visual C debugger
//
#define ENTER_DEBUGGER __debugbreak(); // _asm int32_t 3
//
// Check !=0 only in debug builds (can be continued)
//
#ifdef _ARMOR
#define gosASSERT(x)                                                          \
	do                                                                        \
	{                                                                         \
		if (!(x) && ErrorHandler(gos_ErrorVerify + gos_ErrorNoRegisters, #x)) \
			ENTER_DEBUGGER                                                    \
	}                                                                         \
	MSSUPPRESS_WARNING(4127)                                                    \
	while (0)
#else
#define gosASSERT(x) ((void)0)
#endif
//
// Report an error if the condition is false
//
#ifdef _ARMOR
#define gosREPORT(x, message)                                                       \
	do                                                                              \
	{                                                                               \
		if (!(x) && ErrorHandler(gos_ErrorMessage + gos_ErrorNoRegisters, message)) \
			ENTER_DEBUGGER                                                          \
	}                                                                               \
	MSSUPPRESS_WARNING(4127)                                                          \
	while (0)
#else
#define gosREPORT(x, message) ((void)0)
#endif
//
// Display a message and enter exception handler (can not be continued)
//

#define STOP(x)                     \
	do                              \
	{                               \
		if (InternalFunctionStop x) \
			ENTER_DEBUGGER          \
	}                               \
	MSSUPPRESS_WARNING(4127)          \
	while (0)

//
// Display a message and enter exception handler (can be continued)
//
#define PAUSE(x)                     \
	do                               \
	{                                \
		if (InternalFunctionPause x) \
			ENTER_DEBUGGER           \
	}                                \
	MSSUPPRESS_WARNING(4127)           \
	while (0)

//
// Displays a message to the debuglog or OutputDebugString  -  Three parameters
// are required.
//
// 1st parameter is the group of the message - different groups can be enabled
// or disabled by putting them in Environment.spew, 0 means all the time
//
//  eg: "Stuff_Math"
//
// 2nd parameter is a printf style string
//  eg: "%d beans used.",BeansUsed
//
#ifdef _ARMOR
#define SPEW(x) InternalFunctionSpew x
#else
#define SPEW(x) ((void)0)
#endif

// Handles to internal structures
typedef struct SoundResource* HGOSAUDIO;
typedef struct gos_Music* HGOSMUSIC;
typedef struct gos_Video* HGOSVIDEO;
typedef struct gosFileStream* HGOSFILE;
typedef struct _FontInfo* HGOSFONT3D;
typedef struct gosForceEffect* HGOSFORCEEFFECT;
typedef struct gos_Heap* HGOSHEAP;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// **********************  Environment Settings  ******************************
// //
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

typedef struct gosEnvironment	// size: 0x0130
{
	//
	// Application title and version number
	//
	PSTR applicationName; // "TestApp" or "Pong"
	PSTR version; // Default is "00.00.00.0000"
	PSTR registryVersion; // If present, will be used to create a sub folder in the registry (ie: v1.1, v1.2)
	PSTR directoryPath; // If program path end in this directory, move up levels
		// (ie: "\\Code\\AssassinEditor" or "\\Tools\\PixelWhIP"). "!" will force GameOS
		// NOT to  change directory at all (Including DEBUG/RELEASE/PROFILE....)
	PSTR defaultPlayerName; // used in lobby launch
	BOOLEAN allowMultipleApps; // Allows the game to be run more than once on a single system (network testing)
	// BOOLEAN _unused1[sizeof(void*) - 1];
	uint32_t MegMemoryRequired; // Megabytes of virtual memory required to run game (default is 64Meg)
	BOOLEAN dontClearRegistry; // When true, the registry is not cleared when the .exe is changed

	//
	// Current screen mode (application can check, but may change from frame to frame)
	//
	int32_t screenwidth; // 640
	int32_t screenheight; // 480
	int32_t bitDepth; // 16 or 32
	int32_t FullScreenDevice; // 0=Primary, 1=2nd video card (ie: 3Dfx) etc...
	int32_t Renderer; // 0=Try hardware, fallback to software, 1=RGB, 2=Refrast, 3=Blade
	BOOLEAN fullScreen; // Application start running full screen or in a window?
	BOOLEAN disableZBuffer; // When true no Z buffer surface will be created
	BOOLEAN AntiAlias; // When true full screen antialiasing will be enabled if possible
	BOOLEAN RenderToVram; // When true Blade applications will render directly to video memory (speed-up if no alpha is used)
	BOOLEAN Stencil; // When true an 8 bit stencil buffer will be enabled if possible
	BOOLEAN TripleBuffer; // When true, full screen modes will be triple buffered, else double buffered
	BOOLEAN MaxRefreshRate; // When true, full screen modes will use the maximum card refresh rate, else 60hz.
	BOOLEAN _unused2;
	int32_t DirtyRectangle; // Bit 0=Enabled, Bit 1=Save Z buffer rectangles too, Bit 2=Save directly in system memory (don't try video memory).
	int32_t DisableLowEndCard; // When set to 1 and video cards in VideoCard.cpp with the LowEndCard flag set will havehardware acceleration disabled
	uint32_t MinimumTextureMemory; // If this value is !=0 it specifies the minimum TEXTURE memory required otherwise hardware acceleration is disabled

	// Keys used by GameOS (Use 0 to disable them, or see keycodes later in this header)
	uint32_t Key_FullScreen; // Default is KEY_F4	(Enter/exit full screen mode)
	uint32_t Key_SwitchMonitors; // Default is KEY_F5	(Change video cards - nvidia/voodoo etc...)
	uint32_t Key_Exit; // Default is KEY_ESC	(Exit full screen mode or exit game)

	// Debugging settings
	PSTR debugLog; // File to dump info to, or "debugger" to the debug console
	PSTR spew; // List of groups to dump to debuglog - eg: GameOS_Texture, GameOS_DirectDraw
	BOOLEAN TimeStampSpew; // adds the current time stamp to all spews
	BOOLEAN _unused3[3];
	uint32_t MemoryManager; // 0=custom memory manager, 1=windows memory manager, 2=
	uint32_t memoryTraceLevel; // How many levels of stack to walk when tracing memory allocations

	// Game related functions
	float MaxTimeDelta; // Maximum time delta in seconds allowed between calls to
		// gos_GetElapsedTime(void); (Typical value = 1.0f)
	float MinimumTimeDelta; // If the time delta is greater than MaxTimeDelta, return this time to
		// the application. (Typical value = 1.0f/30.0f)

	// Sound-related application information
	BOOLEAN soundDisable; // false = disable all sound, true = enable all sound
	BOOLEAN soundHiFi; // true = 44Khz sound, false = 22KHz
	BOOLEAN _unused4[2];
	int32_t soundDevice; // 0 = primary/default, # = device enum
	int32_t soundChannels; // Default number of sound channels, for example 8
	int32_t soundForceCache; // 0 = never force a stream resource into cached
		// resource else # bytes under which to force
		// resource to cached if streamed
	BOOLEAN soundMixInHardware; // 0=no mixing sound in hardware (default),
		// 1=mix sound in hardware, if available.
		//
		// Network settings
		//
	BOOLEAN NetworkGame; // Is this game going to be network aware?
	BOOLEAN DirectPlayProtocol; // If this instance is the server, should we use
		// the DirectPlay protocol?
	BOOLEAN _unused5;

	union {
		BOOLEAN NetworkGUID[16]; // This is the DirectPlay GUID that must be unique for the game
		GUID GameGUID; // Note that this union allows access as GUID but byte order will changing depending on initialization
	}; // All initialization of this code should use GameGUID from now on

	uint32_t NetworkMaxPlayers; // The maximum number of players allowed in this game.
	BOOLEAN NetGameInfo[16]; // Information about the current network game (this can be enumerated
		// from other network games before you join them)
	PSTR(__stdcall* DecodeGameInfo)
	(PVOID Data); // GameOS will call this routine if present to decode the 16 bytes of network game information.
	PSTR ZoneMatchServerIP; // Typical value is ZoneMatch.zone.com
	int32_t ZoneAdvertisePort; // Port games will be advertised on.  This is the port ZoneMatch
		// connects to to receive updates to a game's state.  Don't confuse
		// this with the game's port, which would be the port that actual
		// clients who want to play the game would connect to.  Typical value
		// is/ APP_QUERY_PORT (27999)

	BOOLEAN NetServerMigration; // When true, the server player can quit and the game will continue
		// (another machine will become the server)

	// Controller settings
	BOOLEAN ButtonsAsKeys; // when true, gos_GetKey will return events for controller button presses
	BOOLEAN allowDoubleClicks; // make double-click messages available to the application (default is false)
	BOOLEAN _unused6;

	//
	// Raid Database settings
	//
	PSTR RaidFilePath; //	The path where the exception files for your project should be stored.
		// They will be referred to by hyperlink in the bug description.
	PSTR RaidCustomFields; //  A string containing key-value pairs of Raid Database column name and
		//  the value for that record. the column must be of type var-char with a
		//  width of 32 (standard raid list)
	PSTR RaidDataSource; //  The raid data source name (can be retrieved from your odbc control
		//  panel or the registry once an rdq has been opened on your machine.
	PSTR RaidDescTemplate; //	A string sets the default text that appears in the description of
		// the bug eg.

	// Description: \n
	// REPRO STEPS: \n etc.

	// These are legacy and will be deleted
	uint32_t NetworkMinPlayers; // The minimum number of player required to start a game
	uint32_t AllowJoinInProgress; // When true 'join in progress' is enabled for this network game
	uint32_t Texture_S_256; // Number of 256*256 Solid texture heap pages
	uint32_t Texture_S_128; // Number of 128*128 Solid texture heap pages
	uint32_t Texture_S_64; // Number of   64*64 Solid texture heap pages
	uint32_t Texture_S_32; // Number of   32*32 Solid texture heap pages
	uint32_t Texture_S_16; // Number of   16*16 Solid texture heap pages
	uint32_t Texture_K_256; // Number of 256*256 Keyed texture heap pages
	uint32_t Texture_K_128; // Number of 128*128 Keyed texture heap pages
	uint32_t Texture_K_64; // Number of   64*64 Keyed texture heap pages
	uint32_t Texture_K_32; // Number of   32*32 Keyed texture heap pages
	uint32_t Texture_K_16; // Number of   16*16 Keyed texture heap pages
	uint32_t Texture_A_256; // Number of 256*256 Alpha texture heap pages
	uint32_t Texture_A_128; // Number of 128*128 Alpha texture heap pages
	uint32_t Texture_A_64; // Number of   64*64 Alpha texture heap pages
	uint32_t Texture_A_32; // Number of   32*32 Alpha texture heap pages
	uint32_t Texture_A_16; // Number of   16*16 Alpha texture heap pages

	//
	// This flag suppresses the "No 3D Acceleration" message when not full
	// screen. MC2 needs this so that the initial MessageBox can display a
	// warning about sniffing you machine. At any time during the release
	// builds, the game will be full screen.
	BOOLEAN Suppress3DFullScreenWarning;
	BOOLEAN _unused7[3];

	// Now functions GameOS can call in the application

	// Returns custom data used in special places by GameOS
	PVOID(__cdecl* GetSpecialGameData)
	(int32_t data_type, ...);
	//
	// Returns a string containing game data
	//  This is called during error routines, so NO errors must be able to occur
	//  in this routine. Do not assume DirectX or your memory heap etc.. are
	//  valid.
	//
	PSTR(__stdcall* GetGameInformation)
	(void);
	//
	// Called ONCE only, after GameOS has been setup, just before main loop
	// starts
	//  All memory should be allocated and all structures should be cleared
	//
	void(__stdcall* InitializeGameEngine)(void);
	//
	// Called each main loop, game should move 'one tick' if DoTimedGameLogic is
	// not set
	//  Do not update the display, just do the game logic.
	//
	void(__stdcall* DoGameLogic)(void);
	//
	// Called each main loop, game should update it's sound and video renderers.
	//  This is the only time draw or sound functions within the game are valid.
	//  These are not allowed during DoGameLogic so renders may be skipped
	//  during heavy loads or during 'fast playback' of a logfile.
	//
	void(__stdcall* UpdateRenderers)(void);
	//
	// GameOS will call this once it comes out of the main loop, before it shuts
	// down
	//  No display updating should be attempted. This should be used to free all
	//  memory.
	//
	void(__stdcall* TerminateGameEngine)(void);
	//
	//
	//
	//
	// Applications can use thie to 'hook into' the gos_GetFile function.
	//
	// This allows you to 'catch' all the gos_GetFile calls. If you wish to have
	// a compressed file system, or manage your own content directories, this
	// API will allow you to catch all the access to the disk that GameOS does.
	// All texture reading and gosScript file access goes through this API. If
	// this hook function is 0 (default) then the calls will go to gos_GetFile.
	// If you put the address of your own function there, GameOS will jump to
	// that function. GameOS will check for gos_GetFIle being called again
	// during this function and allow it to read from the disk normally.
	//
	void(__stdcall* HookGetFile)(PSTR FileName, BOOLEAN** MemoryImage, uint32_t* Size);

	// Other file API calls available to hook

	BOOLEAN(__stdcall* HookDoesFileExist)(PSTR FileName);

} gosEnvironment;

//
// GameOS will call this function once, before it sets anything up.
//  NO errors must occur, or any GameOS functions called.
//  Only the environment structure should be setup.
//  The structure is cleared by GameOS, so any unused data or functions can be
//  left empty.
//
// The command line that was used to start the game is passed
//
void __stdcall GetGameOSEnvironment(PSTR commandline);
//
// Called by the Game to cause GameOS to quit and exit (at the end of the
// current frame - not immediately)
//
void __stdcall gos_TerminateApplication(void);
//
// In the Environment.TerminateGameEngine routine this function will tell you if
// you can prompt the user or if you must just terminate (for exmaple on log
// file playback)
//
BOOLEAN __stdcall gos_UserExit(void);
//
// While the application is inside the Environment.TerminateGameEngine routine
// it may execute this to make GameOS continue.
//
void __stdcall gos_AbortTermination(void);

//
// This API can be used to create 'modal' dialogs. It will run the GameOS main
// lopp (reading input devices, rendering etc...) WITHOUT running the gamelogic.
// This allows you to call this function in a loop from within your GameLogic to
// wait for a key, do a modal dialog etc.. Then continue with your game logic.
// This API returns true if the user closed the window or called
// gos_TerminateApplication. The application will exit when it finishs it's
// GameLogic loop It is passed the function to call in place of the original
// gamelogic. If this is passed 0, no game logic is called. If the original
// gamelogic loop should be used, pass Environment.DoGameLogic Returns true if
// the game was terminated
//
BOOLEAN __stdcall gos_RunMainLoop(void(__stdcall* DoGameLogic)(void) = 0);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// ******************************	VIDEO  API	  *****************************
// //
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// An enumeration of the various commands to the Video playback API.
enum gosVideo_Command : uint32_t
{
	gosVideo_SeekTime = 1, // Seek to a frame or timestamp in hVideo
	gosVideo_SetCoords = 2, // Set the destination coordinates of <hVideo> via <dwOriginX/Y>
	gosVideo_SetScale = 4, // Change the scale of <hVideo> on the fly via <fScaleOfX/Y>
	gosVideo_Volume = 8, // Set the volume of the multiVideo
	gosVideo_Panning = 16 // Set the pan of the multiVideo
};

//////////////////////////////////////////////////////////////////////////////////
// An enumeration of the various states the video playback can be in.
enum gosVideo_PlayMode : uint32_t
{
	gosVideo_PlayOnceHide, // The Video is currently playing (will hide when done)
	gosVideo_PlayOnceHold, // The Video is currently playing (will hold when done)
	gosVideo_Loop, // The Video is currently in continuous play mode
	gosVideo_Stop, // The Video is stopped
	gosVideo_Pause, // The Video has been paused
	gosVideo_Continue // SET ONLY: continue a paused Video
};

//////////////////////////////////////////////////////////////////////////////////
// This structure is used to send and receive information about a particular
// video resource. Each command (above) requires specific info to be initialized
// in the structure below in order to properly carry out that command. For this
// data, consult the comments listed under the appropriate command (listed
// above).
typedef struct _gosVideo_Info
{
	PSTR lpstrPath; // string specified path to data
	gosVideo_PlayMode ePlayMode; // the play mode (see above)
	gosVideo_PlayMode ePlayStatus; // the play mode (see above)
	uint32_t dwOriginX; // x coord on dest. surf for video
	uint32_t dwOriginY; // y coord on dest. surf for video
	float fScaleOfX; // ratio of displayed to orgininal width
	float fScaleOfY; // ratio of displayed to orgininal height
	float fDurationSec; // read-only duration of video (hundredth of a second)
	float fSoFarSec; // current play position (hundredth of a second)
	BOOLEAN* pdata; // RGB data
	uint32_t dwSurfacewidth; // read-only width of video surface
	uint32_t dwSurfaceheight; // read-only height of vidoe surface
	uint32_t dwPitch; // read-only pitch of video surface
	uint32_t width; // read-only width of video
	uint32_t height; // read-only height of vidoe
} gosVideo_ResourceInfo;

//////////////////////////////////////////////////////////////////////////////////
// Send a command to an existing video resource or create a new video resource.
// Consult the structures/enums listed above for more information.
//
void __stdcall gosVideo_CreateResource(HGOSVIDEO* handle, PSTR filename);
void __stdcall gosVideo_CreateResourceAsTexture(HGOSVIDEO* handle, uint32_t* hTex, PSTR filename);
void __stdcall gosVideo_DestroyResource(HGOSVIDEO* handle);
void __stdcall gosVideo_GetResourceInfo(HGOSVIDEO handle, gosVideo_ResourceInfo* gvi);

void __stdcall gosVideo_SetPlayMode(HGOSVIDEO handle, gosVideo_PlayMode gvpm);
void __stdcall gosVideo_Command(HGOSVIDEO handle, gosVideo_Command vc, float x, float y = 0.0f);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// ****************************	   SOUND API	 ******************************
// //
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//
// This define serves as a reminder that access to global sound properties such
// as overall volume and panning is handled on channel -1. Use the define below
// for legibility.
//
#define gosAudio_Mixer -1

////////////////////////////////////////////////////////////////////////////////////////////////////////
// These properties can be assigned to any channel via the
// gosAudio_AllocateChannelSliders function-- only allocate the sliders you
// intend to use on each channel-- this improves both hardware and software
// performance.
//
enum gosAudio_Properties : uint32_t
{
	gosAudio_Common = 1, //
	gosAudio_Volume =
		2, // Volume can be adjusted independently of mixer (0.0 = silence, 1.0 = full volume)
	gosAudio_Panning =
		4, // Panning can be adjusted independently of mixer (-1.0 = left, 0 = center, +1 = right)
	gosAudio_Frequency = 8, // Frequency can be adjust independently of mixer (< 1.0 = lower
	// freq, 1.0 = normal, 2 = 2x, etc.)
	gosAudio_SeekTime = 16, // RESERVED
	gosAudio_Position = 32, // * The 3D position can be set
	gosAudio_Velocity = 64, // * The 3D velocity can be set
	gosAudio_FrontOrientation = 128, // Mixer only: set the front and top orientation of "ears." The
	// front vector points in the direction
	gosAudio_TopOrientation =
		256, // of the listener's nose, and the top vector points out the top of the listener's
	// head. By default, the front vector is (0,0,1.0) and the top vector is (0,1.0,0).
	gosAudio_MinMaxDistance =
		512, // * Under minimum distance, volume is max; Over maximum distance, volume is zero
	gosAudio_Doppler = 1024, // Mixer only: 1.0f is real world, 2.0f is twice real world
	gosAudio_Rolloff = 2048, // Mixer only: 1.0f is real world, 2.0f is twice real world
	gosAudio_Distance = 4096, // Mixer only: position/velocity adjustment: 1.0f = same, 2.0f =
	// double position/velocity
	gosAudio_Reverb = 8192, // .0f to 2.0f ( < 1.0 = muffled, > 1.0 = exagerated)
	gosAudio_Decay = 16384, // .1f to 20.0f (in seconds)
	gosAudio_ConeAngles = 32768, // expressed as degrees. 1st parameters is inner (full volume),
	// second is outer (fully attenuated)
	gosAudio_ConeOrientation = 65536 // direction in which the cone points.
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// This enum is used to set or return the client's speaker configuration. Note
// that gosAudio_DegreeArc<x> may be bitwise OR'd into the speaker type if the
// speaker type is gosAudio_Stereo.
//
enum gosAudio_SpeakerConfig : uint32_t
{
	gosAudio_Headphones = 1, // The client system uses a pair of headphones for aural feedback
	gosAudio_Monaural = 2, // The client system has only one speaker attached
	gosAudio_Quadraphonic = 4, // The system is using a four-speaker system
	gosAudio_Stereo = 8, // This is a typical system configuration: two
	// speakers, left and right
	gosAudio_Surround = 16, // The client system has several (more than four) speakers wired in
	gosAudio_DegreeArc5 = 32, // For a stereo system, the user may specify the
	// arc over which the two speaker lie
	gosAudio_DegreeArc10 = 64,
	gosAudio_DegreeArc20 = 128,
	gosAudio_DegreeArc180 = 256
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sound Channels can be in one of the following states
//
enum gosAudio_PlayMode : uint32_t
{
	gosAudio_PlayOnce, // Play the channel's resource once and then end
	gosAudio_Loop, // Continually play the channel's resource, looping after
	// each iteration
	gosAudio_Pause, // Pause the sound, a Continue will resume the sound from
	// where it was paused
	gosAudio_Continue, // SET ONLY: continue a sound that was paused.
	gosAudio_Stop, // Silence the channel and stop processing the resource
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
// Resource Types
//
enum gosAudio_ResourceType : uint32_t
{
	gosAudio_CachedFile, // Pull a WAV file from disk, parse it, and copy the
	// raw sound data into system memory
	gosAudio_UserMemory, // Use a preloaded WAV already in the client's system
	// memory. Note: This memory must remain intact until
	// this resource is destroyed
	gosAudio_UserMemoryDecode, // Use a preloaded WAV already in the client's
	// system memory. Note: Unlike UserMemory,
	// UserMemoryDecode deciphers the waveformat
	// itself.
	gosAudio_UserMemoryPlayList, // Play a series of same-format WAVs back to
	// back, played from memory-mapped WAVs
	gosAudio_StreamedFile, // Leave the sound data on disk, streaming only the
	// data when needed. Note: only volume and pan affect
	// streamed files
	gosAudio_StreamedMusic, // Leave the sound data on disk, play a
	// non-PCM/ADPCM song.
	gosAudio_StreamedFP // Streamed from a file pointer
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// The format structure, for describing the format of sound resources
//
typedef struct _gosAudio_Format
{
	uint16_t wFormatTag; // Waveform-audio format type. 1=PCM, 2=Microsoft ADPCM.
	uint16_t nChannels; // 1=Mono, 2=Stereo.
	uint32_t nSamplesPerSec; // Sample rate, 11025Hz, 22050Hz or 44100Hz.
	uint32_t nAvgBytesPerSec; // Normally, nBlockAlign * nSamplesPerSec
	uint16_t nBlockAlign; // Normally, wBitsPerSample / 8 * nChannels
	uint16_t wBitsPerSample; // Bits per sample for the wFormatTag format type.
		// If wFormatTag is 1 (PCM), then wBitsPerSample
		// should be equal to 8 or 16.
	size_t cbSize; // Size, in bytes, of extra format information appended to
		// the end of the WAVEFORMATEX structure. For PCM's, this
		// should be set to 0. For ADPCM, this should be set to 32.
} gosAudio_Format;

////////////////////////////////////////////////////////////////////////////////////////////////////////
// For gosAudio_UserMemoryPlayList resources, fill in the structure below and
// pass it in. The information in the structure itself as well as the pointers
// within the arrays will be copied, so the structures can be discarded once the
// resource has been created
//
typedef struct _gosAudio_PlayList
{
	size_t m_dwListLength; // Number of sample in the playlist
	size_t* m_lpSoundData; // A array of pointers pointing to the start of each
		// memory-mapped WAV file.
	size_t* m_pdataLength; // The size of each corresponding sample in the playlist
} gosAudio_PlayList;

////////////////////////////////////////////////////////////////////////////////////////////////////////
// This structure is used to return important information about a particular
// resource.
//
typedef struct _gosAudio_ResourceInfo
{
	PSTR lpstrPath; // the path or name of the resource queried
	gosAudio_ResourceType eType; // see above
	gosAudio_Format sFormat; // 1 = PCM, 2 = ADPCM
	uint32_t sizeInBytes; // size of the data portion of the WAV
	float fDuration; // in seconds
} gosAudio_ResourceInfo;

////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetChannelInfo returns all the following information when inquiring about a
// channel. Note that the later portion is specific to the Mixer (channel -1)
//
typedef struct _gosAudio_ChannelInfo
{
	HGOSAUDIO hAudio;
	uint32_t dwProperties; // A bitwise flag field specifying which sliders have
		// been allocated to it.
	gosAudio_PlayMode ePlayMode; // The current playmode of the channel
	float fVolume; // The current volume (0.0 - 1.0) of the channel
	float fPanning; // The current panning (-1.0 - +1.0) of the channel
	float fFrequency; // The current frequency (>0 - +#) of the channel
	float fCompletionRatio; // 0-100.0.
	float fPosX, fPosY,
		fPosZ; // If the channel is 3d, the position of the emitter/listener
	float fVelX, fVelY,
		fVelZ; // If the channel is 3d, the velocity of the emitter/listener
	float fMinDistance, fMaxDistance; // MinDistance under which sound is at
		// full volume, MaxDistance after which
		// sound too far away to be heard
	float fConeInner,
		fConeOuter; // The dimensions of the sound cone, in degrees
	float fConeX, fConeY,
		fConeZ; // The rotations applied to the sound cone, in radians

	//////////////////////////////////////////////////////////////////////////////////
	//	Mixer (channel -1) related
	//
	uint32_t dwSpeakerConfig; // The speaker setup of the mixer
	float fFrontX, fFrontY,
		fFrontZ; // The forward-facing vector of the user's "ears"
	float ftopx, ftopy, fTopZ; // The top-pointing vector of the listener
	float fDoppler; // Typically 1.0, this can be manipulated for a desired effect
	float fRolloff; // Typically 1.0, this can be change to match a desired effect
	float fDistance; // Typically 1.0, this defines meters/unit within the sound
		// system
	float fReverb; // If available, the reverbation factor of all channels
	float fDecay; // If available, the decay factor of all channels
} gosAudio_ChannelInfo;

//////////////////////////////////////////////////////////////////////////////////
// Creates a resource to be played later
//
void __stdcall gosAudio_CreateResource(HGOSAUDIO* hgosaudio, gosAudio_ResourceType, PSTR file_name,
	gosAudio_Format* ga_wf = 0, PVOID data = 0, int32_t size = 0, BOOLEAN only2D = 0);
void __stdcall gosAudio_CreateResource(
	HGOSAUDIO* hgosaudio, PSTR identifier_name, HGOSFILE file, size_t offset, BOOLEAN only2D = 0);

//////////////////////////////////////////////////////////////////////////////////
// Destroy a resource; any sounds currently playing using the ResourceID will be
//  stopped.
//  Any memory the SoundAPI associated with the resource will be freed.
//
void __stdcall gosAudio_DestroyResource(HGOSAUDIO* hgosaudio);

//////////////////////////////////////////////////////////////////////////////////
// This prepares the channel for a specific type of sound playback. Optimally,
// allocate only the properties that will need modification. Use a bitwise'd
// group of gosAudio_Properties to set what is needed.
//
void __stdcall gosAudio_AllocateChannelSliders(int32_t Channel, uint32_t properties);

//////////////////////////////////////////////////////////////////////////////////
// Prepare a channel to play a resource of any type.
//
void __stdcall gosAudio_AssignResourceToChannel(int32_t Channel, HGOSAUDIO hgosaudio);

//////////////////////////////////////////////////////////////////////////////////
// Get and Set functions only operate if a channel has the property enabled
//  Channel number -1 used in SetVolume and SetPanning will alter the windows
//  master volume and balance
void __stdcall gosAudio_SetChannelSlider(
	int32_t Channel, gosAudio_Properties, float value1, float value2 = 0.0f, float value3 = 0.0f);
void __stdcall gosAudio_GetChannelSlider(
	int32_t Channel, gosAudio_Properties, float* value1, float* value2 = 0, float* value3 = 0);

//////////////////////////////////////////////////////////////////////////////////
// Set the speaker configuration. See enum above.
//
void __stdcall gosAudio_SetSpeakerConfig(uint32_t config);

//////////////////////////////////////////////////////////////////////////////////
// Play, Loop, Stop, Pause, or Continue a particular channel
//
void __stdcall gosAudio_SetChannelPlayMode(int32_t Channel, gosAudio_PlayMode ga_pm);
//////////////////////////////////////////////////////////////////////////////////
// Determine the current play mode of a channel
//
gosAudio_PlayMode __stdcall gosAudio_GetChannelPlayMode(int32_t Channel);

//////////////////////////////////////////////////////////////////////////////////
// Get the filename, type, frequency and number of channels for a sound resource
//
void __stdcall gosAudio_GetResourceInfo(HGOSAUDIO hSound, gosAudio_ResourceInfo* sri);
void __stdcall gosAudio_GetChannelInfo(int32_t Channel, gosAudio_ChannelInfo* sri);

//////////////////////////////////////////////////////////////////////////////////
// Reset the sound system if you wanna change the original sound device,
// or the KHz of the mixer....
//
void __stdcall gosAudio_Reset(void);

//////////////////////////////////////////////////////////////////////////////////
// Get a list of available sound devices. Returns <available> possibilities as
// strings in <name>
//
void __stdcall gosAudio_DeviceOptions(char names[8][128], int32_t* available);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// *************************	 LOCALIZATION API
// ***************************//
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////
// struct gosIME_Appearance
// This structure is used to control appearance of IME UI.
//
// symbolColor(*):				RGB colour of symbol.
//								If no symbol color is specified, the Henkan-kyu is grey when
// inactive and a rainbow sphere when active
// symbolColorText(*):			RGB colour of text in the symbol
// symbolheight:				the size (in pixels) of the IME symbol
// symbolTranslucence:			the translucency level of the IME symbol	(0-255 from
// invisible to opaque) symbolPlacement:				where the symbol is
// placed on the screen:
//
//    0 - to the right or left of the current print margins, centered vertically
//    top to bottom (default) 1 - the upper left corner of the screen 2 - the
//    upper right corner of the screen 3 - the lower right corner of the screen
//    4 - the lower left corner of the screen
//
// candColorBase:				RGB background color of the candidate list
// candColorBorder:				RGB border & highlight color of the candidate
// list candColorText:				RGB the text color of the candidate list
//								If it's 0, the selected text color at last gos_PositionIME()
// call is used
//
// compColorInput(*):			RGB colour for background of character currently being
// entered by the user compColorTargetConv(*):		RGB colour for background of
// portion of composition currently being converted compColorConverted(*):
// RGB colour for background for converted portion of composition string
// compColorTargetNotConv(*):	RGB colour for background for unconverted portion
// of composition string compColorInputErr(*):		RGB colour for background for
// erroneous portion of composition string
// compTranslucence:			The translucency of composition string (0-255 from
// invisible to opaque) compColorText:				RGB colour for the string
// (effective only in level 2)
//								If it's 0, the selected text color at last gos_PositionIME()
// call is used
// (* - transparency value is ignored )
//
typedef struct _gosIME_Appearance
{
	// symbol (Henkan-kyu)
	uint32_t symbolColor;
	uint32_t symbolColorText;
	BOOLEAN symbolheight;
	BOOLEAN symbolTranslucence;
	BOOLEAN symbolPlacement;
	BOOLEAN _padding1;

	// candidate list
	uint32_t candColorBase;
	uint32_t candColorBorder;
	uint32_t candColorText;

	// composition string
	uint32_t compColorInput;
	uint32_t compColorTargetConv;
	uint32_t compColorConverted;
	uint32_t compColorTargetNotConv;
	uint32_t compColorInputErr;
	BOOLEAN compTranslucence;
	BOOLEAN _padding2[3];
	uint32_t compColorText;
} gosIME_Appearance;

//
// Opens a DLL and returns a handle
//
uint32_t __stdcall gos_OpenResourceDLL(PSTR FileName);

//
// Use to close a resource DLL
//
void __stdcall gos_CloseResourceDLL(uint32_t handle);

//
// Returns a string from a resource (or a place holder if string not defined)
//
// This returns a pointer to an internal GameOS 4K buffer. It is only valid
// until the next call to gos_GetResourceString
//
PSTR __stdcall gos_GetResourceString(uint32_t handle, uint32_t id);

//
// Returns a PVOID to block of memory stored in the resource .dll. Resources
// that are stored in this fashion must be a "custom" resource and their
// resource type string must match that of the 2nd parameter.
//
// If the resource cannot be found, a nullptr is returned.
//
// If the resource is found, size is set to the size of resource in
// bytes and the resource data is copied into a freshly allocated memory block
// from the currently designated heap. It is the responsibility of the client
// application to release this memory when it is no longer needed.
//
PVOID __stdcall gos_GetResourceData(uint32_t handle, PSTR type, uint32_t id, size_t* size);

//
// If the user's OS supports IME, this function will Enable/Disable the user's
// ability to interface with the IME for in the GameOS application. If
// <enabledDisabled> is false, the user will be unable to bring up the input
// method editor's interface via ALT-~.
//
void _stdcall gos_EnableIME(BOOLEAN enabledDisabled);

//
// If the user's OS supports IME, and the IME is enabled (see above), the
// application can forcibly switch the IME to the active state. When active, the
// user's keypresses will be fed to the IME for clarification via the IME
// interface and candidate list. Note that this is the programatic equivalent of
// the user pressing ALT-~.
//
void _stdcall gos_ToggleIME(BOOLEAN activeInactive);

//
// If the user's OS supports IME, set text entry caret screen position (Used to
// enable the localization IME pop-up at the correct place):
//
void _stdcall gos_PositionIME(uint32_t x, uint32_t y);

//
// Finalize current composition string. An application should call this function
// when focused edit box is clicked
//
void _stdcall gos_FinalizeStringIME(void);

//
// Sets IME support level (2 or 3)
// 2 - Composition string is drawn by IME UI library. GOS application receives
// characters when composition string is finalized. 3 - Composition string is
// drawn by GOS application (default). GOS application receives characters and
// control keys whenever composition string changes.
//
void _stdcall gos_SetIMELevel(uint32_t dwImeLevel);

//
// Sets appearance of IME UI
//
void _stdcall gos_SetIMEAppearance(const gosIME_Appearance* pia);

//
// Gets appearance of IME UI
//
void _stdcall gos_GetIMEAppearance(gosIME_Appearance* pia);

//
// Tell the IME whether or not the IME should process keystrokes in Overwrite
// mode (bInsert = false) or in Insert mode (bInsert = true)
//
void _stdcall gos_SetIMEInsertMode(BOOLEAN bInsert);

//
// Returns a localized string describing a date as either int16_t form
// "02/03/2000" or Verbose form "Wednesday, May 3rd, 1999"
//
// If the Year, Month and Day are -1 the current time is used
//
PSTR __stdcall gos_GetFormattedDate(
	BOOLEAN Verbose, uint16_t Year = -1, uint16_t Month = -1, uint16_t Day = -1);

//
// Returns the year month and day
//
void __stdcall gos_GetUnformattedDate(uint16_t* Year, uint16_t* Month, uint16_t* Day);

//
// Returns a localized string describing a time "11:45:32 PM"
//
// If the Hours, Minutes and Seconds are -1 the current time is used
//
PSTR __stdcall gos_GetFormattedTime(uint16_t Hour = -1, uint16_t Minute = -1, uint16_t Second = -1);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// **************************	 3D FONT API	 ******************************
// //
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//
// Init font - reads a .TGA file and decodes it as a font
//
// Return value is a handle that can be passed to the font routines
//
// Rules for FONT.TGA files:
//
//  Fonts must be drawn on a regular grid, ie: 6*8, 8*8, 8*12, 16*16
//
//  The grid must start at the top left of the .TGA file and go all the
//  way to the edge. The next letters must start directly below the top
//  line of letters.
//
//  The first character must be a single pixel thick box, the same size
//  as the grid - this is how GOS measures grid size.
//
//  The 2nd character must be character ascii code 33 (!) all the other
//  characters must be incrementing ascii characters up to character 127.
//
//  The remainder of any .TGA file is used. See Ariel8.tga for an example
//
//  A proportional font need not be centered in the grid squares. GameOS
//  will calculate the leading blank pixels and width of each character.
//  It is important the characters should be centered vertically properly
//  and if the font will be displayed as a fixed pich font then the letters
//  should be centered. Letters should not use the right or bottom row of
//  the grid as these rows do not render accuratly across all cards. So
//  and 8*8 font would have 7*7 sized letters aligned in the top left.
//
//  The StartLine parameter is the Y position for the top of the font. There
//  maybe multiple fonts in a single texture.
//
//  CharCount is the ASCII value of the last character in the font
//  texture plus one.
//
HGOSFONT3D __stdcall gos_LoadFont(
	PSTR FontFile, uint32_t StartLine = 0, int32_t CharCount = 256, uint32_t TextureHandle = 0);

//
// This routine should be called to release storage and textures used by fonts.
//
void __stdcall gos_DeleteFont(HGOSFONT3D Fonthandle);

//
// FontHandle	- This must be a previously loaded font
// Foreground	- Foreground color (Background color should be done by drawing a
// quad) - Alpha must be FF for solid text
// Size			- If > 0, the desired scale of the font (in multiples of the point
// size). If < 0, the absolute point size (-8 = 8 Point Font) WordWrap		- If
// true words will be wraped within region, otherwise clipped Proportional	- If
// false words are displayed fixed pitch Bold         - If true text is bolded
// (Displays the text twice, shifted a pixel so is half the speed as normal
// text) - To display a lot of text use a bold font. Italic		- If true text
// is italiced WrapType		- 0=Left aligned, 1=Right aligned, 2=Centered,
// 3=Centered in region (X and Y)
//
// This API just updates internal variables, it's very fast to change any of
// these parameters
//

void __stdcall gos_TextSetAttributes(HGOSFONT3D FontHandle, uint32_t Foreground, float Size,
	BOOLEAN WordWrap, BOOLEAN Proportional, BOOLEAN Bold, BOOLEAN Italic, uint32_t WrapType = 0,
	BOOLEAN DisableEmbeddedCodes = 0);

//
// Set the current position of the cursor. (Screen pixels)
//
// This API just updates internal variables, it's very fast to change any of
// these parameters
//
void __stdcall gos_TextSetPosition(int32_t XPosition, int32_t YPosition);
//
// Returns the current print position
//
void __stdcall gos_TextGetPrintPosition(int32_t* pXPosition, int32_t* pYPosition);
//
// Set the region that the cursor will be clipped to (in screen pixels)
// The text will be clipped or wrap when the right edge is reached
//
// This API just updates internal variables, it's very fast to change any of
// these parameters
//
void __stdcall gos_TextSetRegion(int32_t Left, int32_t Top, int32_t Right, int32_t Bottom);

//
// Draws a solid rectangle in the region specified (will be clipped to the
// TextSetRegion)
//  This can be used to draw the 'background' of any text.
//  Remember! - To set the alpha component of the color to FF if you want a
//  solid background
//
void __stdcall gos_TextDrawBackground(
	int32_t Left, int32_t Top, int32_t Right, int32_t Bottom, uint32_t colour);

//
// Returns pixel height and width of string (height=height of charcters * number
// of /n found)
//  Make sure you have setup the Text attributes before calling this.
//
void __stdcall gos_TextStringLength(uint32_t* pwidth, uint32_t* pheight, PSTR message, ...);

//
// Draws string using current attributes and position
//
//  Special Codes -
//
//		\n					Return to left edge of current region, move down a
// line
//      /color=AARRGGBB		Sets the current color (AARRGGBB are in hex) - AA
//      = FF for solid text
//		/proportional=?		? is 1 or 1
//		/bold=?				? is 1 or 1
//		/italic=?			? is 1 or 1
//		//					/ character (/ not followed by a special code will be
// displayed as - // is only required to display strings like "//color"
//
void __stdcall gos_TextDraw(PSTR message, ...);
//
// Same as above, but an arglist can be passed
//
void __stdcall gos_TextDrawV(PSTR message, PSTR arglist);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// ****************************	   FILE API	   ********************************
// //
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//
//
// Read a whole file into memory.
//
// The whole file in read into memory, MemoryImage will point to the start of
// the file, Size will be the size of the file.
//
void __stdcall gos_GetFile(PSTR FileName, BOOLEAN** MemoryImage, size_t* pSize);

//
// Opens a memory mapped file - returns a handle that must be passed to the
// Close function below.
//
HANDLE __stdcall gos_OpenMemoryMappedFile(PSTR FileName, BOOLEAN** MemoryImage, size_t* pSize);

//
// Closes a memory mapped file
//
void __stdcall gos_CloseMemoryMappedFile(HANDLE handle);

//
// Opens and reads in the whole file in a background thread. The MemoryImage
// pointer will be nullptr until the file is read in completly.
//    When you are finished with the file, the Close function below must be
//    called.
//
uint32_t __stdcall gos_ReadFileInBackground(PSTR FileName, BOOLEAN** MemoryImage, uint32_t* Size,
	uint32_t Offset = 0, uint32_t MaxSize = 0xffffffff);

//
// Closes and releases the memory used by a file read in the background.
//
void __stdcall gos_CloseBackgroundFile(uint32_t handle);

/* =============================================================================*/
#if 1 // CONSIDERED_OBSOLETE

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// **************************	   MEMORY API	   ****************************
// //
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//
// IMPORTANT: to gather information about memory leaks in your application, you
// must have "GameOS_Memory" in your Environment.spew variable. Also, you can
// adjust the call stack trace info via the Environment.memoryTraceLevel
// variable. A good default level is 5. Also be aware that in Profile and
// Release compilation modes, no call stack information is gathered.
//
//////////////////////////////////////////////////////////////////////////////////

//
// By default, any gos_Malloc's or new's made by the client application will be
// dropped into this heap: the ParentClientHeap. Use ParentClientHeap to
// explicitly tell the Memory Manager to place allocated memory into client
// memory.
//
// extern HGOSHEAP ParentClientHeap;

//
// When including GamePlatform, new and delete will map directly to gos_Malloc
// and gos_Free
//
// new also has an optional second parameter which specifies the heap to
// allocate from.
//

//
// Not filling in the heap parameter on gos_Malloc's tells the Memory Manager to
// allocate memory in the heap currently on top of the heap stack.
//
PVOID __stdcall gos_Malloc(size_t bytes, HGOSHEAP Heap = 0);
void __stdcall gos_Free(PVOID ptr);

//
// Not filling in the heap parameter on new's tells the Memory Manager to
// allocate memory in the heap currently on top of the heap stack.

#ifdef __cplusplus
PVOID __cdecl
operator new(size_t size, HGOSHEAP Heap);
#endif

//
// Allows an application to create a named memory heap.
//
// The uint32_t returned is a heap number that can be the second parameter
// passed to new or gos_Malloc.
//
// If MaximumSize is specified, GameOS will _ASSERT when more than that number of
// bytes is allocated.
//
// The memory heaps and current sizes are visible in the GameOS debugger.
//
HGOSHEAP __stdcall gos_CreateMemoryHeap(
	PSTR HeapName, size_t MaximumSize /* = 0*/, HGOSHEAP parentHeap /* = ParentClientHeap*/);

//
// Allows the application to destroy a previously created memory heap.
// Note that, at shutdown, the default heap is destroyed automatically.
//
// o Destroying a heap destroys all its children as well
// o Setting shouldBeEmpty to false silences the deallocation of memory
//   still assigned to the heap or its children. We don't encourage this
//   as it can result in non-NULLed pointers within the client code.
//
void __stdcall gos_DestroyMemoryHeap(HGOSHEAP Heap, BOOLEAN shouldBeEmpty = 1);

//
// Sets the current heap being used by new and delete (and any gos_Mallocs that
// don't specifiy a heap)
//
// The 'default heap' for all client memory is caught by ClientHeap.
//
void __stdcall gos_PushCurrentHeap(HGOSHEAP Heap);

HGOSHEAP __stdcall gos_GetCurrentHeap(void);

void __stdcall gos_PopCurrentHeap(void);

//
// To verify that memory hasn't been corrupted during particularly dicey
// operations, the gos_WalkMemoryHeap function has been provided.
//
// If a client application tries to write beyond the bounds of the memory
// allocated to it by the Memory Manager, An exception will occur, notifying the
// user. Passing a nullptr (or 0) as pHeap will check ALL heaps.
//
// o NOTE: if vociferous is set to true, the client application will receive
// information on the blocks allocated
//         in the specified heap at the time of the call. These SPEWs will occur
//         only if Environment.memoryTraceLevel is
//
void __stdcall gos_WalkMemoryHeap(HGOSHEAP pHeap, BOOLEAN vociferous = 0);

#endif // MEMORY API

////////////////////////////////////////////////////////////////////////////////
/***************************	 KEYBOARD API	 ******************************/
////////////////////////////////////////////////////////////////////////////////

// A list of the "buttons" on a keyboard.
enum gosEnum_KeyIndex : uint32_t
{
	KEY_ESCAPE = 0x1B,
	KEY_1 = '1',
	KEY_2 = '2',
	KEY_3 = '3',
	KEY_4 = '4',
	KEY_5 = '5',
	KEY_6 = '6',
	KEY_7 = '7',
	KEY_8 = '8',
	KEY_9 = '9',
	KEY_0 = '0',
	KEY_MINUS = 0xBD, // - on main keyboard
	KEY_EQUALS = 0xBB,
	KEY_BACK = 0x08, // Backspace
	KEY_TAB = 0x09,
	KEY_Q = 'Q',
	KEY_W = 'W',
	KEY_E = 'E',
	KEY_R = 'R',
	KEY_T = 'T',
	KEY_Y = 'Y',
	KEY_U = 'U',
	KEY_I = 'I',
	KEY_O = 'O',
	KEY_P = 'P',
	KEY_LBRACKET = 0xDB,
	KEY_RBRACKET = 0xDD,
	KEY_RETURN = 0x0D, // Enter on main keyboard
	KEY_LCONTROL = 0x11, // Both controls (not left)
	KEY_A = 'A',
	KEY_S = 'S',
	KEY_D = 'D',
	KEY_F = 'F',
	KEY_G = 'G',
	KEY_H = 'H',
	KEY_J = 'J',
	KEY_K = 'K',
	KEY_L = 'L',
	KEY_SEMICOLON = 0xBA,
	KEY_APOSTROPHE = 0xDE,
	KEY_GRAVE = 0xC0, // accent grave
	KEY_LSHIFT = 0x10, // Both shifts (not left)
	KEY_BACKSLASH = 0xDC,
	KEY_Z = 'Z',
	KEY_X = 'X',
	KEY_C = 'C',
	KEY_V = 'V',
	KEY_B = 'B',
	KEY_N = 'N',
	KEY_M = 'M',
	KEY_COMMA = 0xBC,
	KEY_PERIOD = 0xBE, // . on main keyboard
	KEY_SLASH = 0xBF, // / on main keyboard
	KEY_RSHIFT = 0x10, // Both shifts, not just right
	KEY_MULTIPLY = 0x6A, // * on numeric keypad
	KEY_LMENU = 0x12, // Both Alt keys
	KEY_SPACE = 0x20,
	KEY_CAPITAL = 0x14,
	KEY_F1 = 0x70,
	KEY_F2 = 0x71,
	KEY_F3 = 0x72,
	KEY_F4 = 0x73,
	KEY_F5 = 0x74,
	KEY_F6 = 0x75,
	KEY_F7 = 0x76,
	KEY_F8 = 0x77,
	KEY_F9 = 0x78,
	KEY_F10 = 0x79,
	KEY_NUMPAD7 = 0x67,
	KEY_NUMPAD8 = 0x68,
	KEY_NUMPAD9 = 0x69,
	KEY_SUBTRACT = 0x6d, // - on numeric keypad
	KEY_NUMPAD4 = 0x64,
	KEY_NUMPAD5 = 0x65,
	KEY_NUMPAD6 = 0x66,
	KEY_ADD = 0x6b, // + on numeric keypad
	KEY_NUMPAD1 = 0x61,
	KEY_NUMPAD2 = 0x62,
	KEY_NUMPAD3 = 0x63,
	KEY_NUMPAD0 = 0x60,
	KEY_DECIMAL = 0x6e, // . on numeric keypad
	KEY_F11 = 0x7a,
	KEY_F12 = 0x7b,
	KEY_F13 = 0x7c, // (NECPC98)
	KEY_F14 = 0x7d, // (NECPC98)
	KEY_F15 = 0x7e, // (NECPC98)
	KEY_KANA = 0x15, // (Japanese keyboard)
	KEY_CONVERT = 0x1c, // (Japanese keyboard)
	KEY_NOCONVERT = 0x1d, // (Japanese keyboard)
	KEY_YEN = 0xdc, // (Japanese keyboard)
	KEY_NUMPADEQUALS = 0x92, // 	= on numeric keypad (NECPC98)
	KEY_CIRCUMFLEX = 0xde, // (Japanese keyboard)
	KEY_AT = 0xc0, // (NECPC98)
	KEY_COLON = 0xba, // (NECPC98)
	KEY_UNDERLINE = 0xdf, // (NECPC98)
	KEY_KANJI = 0x19, // (Japanese keyboard)
	KEY_STOP = 0x13, // (NECPC98)
	KEY_AX = 0x96, // (JapanAX)
	KEY_UNLABELED = 0x97, // (J3100)
	KEY_NUMPADENTER = 0x0D, // Enter on numeric keypad
	KEY_RCONTROL = 0x11, // Both controls (not right),
	KEY_NUMPADCOMMA = 0x6c, // , on numeric keypad (NECPC98)
	KEY_DIVIDE = 0x6F, // / on numeric keypad
	KEY_SYSRQ = 0x2C,
	KEY_RMENU = 0x12, // Both Alt keys
	KEY_HOME = 0x24, // Home on arrow keypad
	KEY_CENTER = 0x0c, // Key 5 on the numeric keypad
	KEY_UP = 0x26, // Up Arrow on arrow keypad
	KEY_PRIOR = 0x21, // Pg Up on arrow keypad
	KEY_LEFT = 0x25, // Left Arrow on arrow keypad
	KEY_RIGHT = 0x27, // Right Arrow on arrow keypad
	KEY_END = 0x23, // End on arrow keypad
	KEY_DOWN = 0x28, // Down Arrow on arrow keypad
	KEY_NEXT = 0x22, // PgDn on arrow keypad
	KEY_INSERT = 0x2D, // Insert on arrow keypad
	KEY_DELETE = 0x2E, // Delete on arrow keypad
	KEY_LWIN = 0x5B, // Left Windows key
	KEY_RWIN = 0x5C, // Right Windows key
	KEY_APPS = 0x5D, // AppMenu key
	KEY_NUMLOCK = 0x90, // NUMLOCK
	KEY_PAUSE = 0x13, // PAUSE Key
	KEY_LMOUSE = 1, // Left mouse button
	KEY_RMOUSE = 2, // Right mouse button
	KEY_MMOUSE = 3, // Middle mouse button
	KEY_MOUSEX1 = 4, // New mouse button 1
	KEY_MOUSEX2 = 5, // New mouse button 2
};

enum gosEnum_KeyStatus : uint32_t
{
	KEY_FREE,
	KEY_PRESSED,
	KEY_HELD,
	KEY_RELEASED,
};

enum gosEnum_KeyDeviceType : uint32_t
{
	KEYDEV_KEYBOARD = 0x00000000,
	KEYDEV_JOYSTICK = 0x01000000,
};

enum _KeyDevice_const : uint32_t
{
	KEYDEV_MASK = 0xff000000
};

//////////////////////////////////////////////////////////////////////////////////
// Get the status of key <index>. TRUE means it is pressed, while FALSE means
// it is unpressed.
//
// Mouse buttons are keys, 1,2,3,4 and 5  (1 is Left, 2 is right, 3 is middle, 4
// and 5 are on the 5 button mice)
//
gosEnum_KeyStatus __stdcall gos_GetKeyStatus(gosEnum_KeyIndex index);

//////////////////////////////////////////////////////////////////////////////////
// Get extended keys from keyboard.
//
// nullptr until a key is pressed
// Low byte is ASCII key values - or 0 when high byte is an extended keycode
// that is not ASCII High byte is a 'gosEnum_KeyIndex' code such as KEY_LEFT )
//
// For example cursor left is		KEY_LEFT<<8
// For example the enter key is		KEY_ENTER<<8 + 13
// For example the 'a' key is		KEY_A<<8 + 'a'
//
// There is an extra bit that may be added on some keyboards to signify an
// 'extended key'. This bit is require to display the correct key name.
// Applications should only use this bit for the gos_DescribeKey function. This
// extra bit is bit 16, i.e. 0x10000 - This bit SHOULD be masked off unless you
// are going to use the value for gos_DescribeKey.
//
// If Environment.ButtonsAsKeys is set, then gos_GetKey will also get button
// presses.  The high byte of the dword will indicate the type of device
// (KEYDEV_KEYBOARD or KEYDEV_JOYSTICK); the second byte will indicate the ID of
// the device (always 0 [or 1 for extended keys] for keyboard), ie joystick #1
// or #2, etc., and the third byte will indicate the button ID on the device.
// The low byte will always be zero (since these buttons never map to ASCII).
// For example, a return value of 0x01020300 is button 3 on joystick #2.
//
uint32_t __stdcall gos_GetKey(void);

//
// Returns the displayable name of a key press returned from gos_GetKey() eg:
// "A", "Cursor Left" or "Enter"
//
PSTR __stdcall gos_DescribeKey(uint32_t Key);

//////////////////////////////////////////////////////////////////////////////////
// Clear the client keyboard buffer
void __stdcall gos_KeyboardFlush(void);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// **************************	 JOYSTICK API	 ******************************
// //
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//
// The various outputs that can be examined by GameOS.
//
enum GOSJoystickAxis : uint32_t
{
	JOY_XAXIS = 0,
	JOY_YAXIS = 1,
	JOY_ZAXIS = 2,
	JOY_THROTTLE = 2,
	JOY_RXAXIS = 3,
	JOY_RYAXIS = 4,
	JOY_RZAXIS = 5,
	JOY_RUDDER = 5,
	JOY_SLIDER1 = 6,
	JOY_SLIDER2 = 7,
	JOY_HAT1 = 8,
	JOY_POV1 = 8,
	JOY_HAT2 = 9,
	JOY_POV2 = 9,
	JOY_HAT3 = 10,
	JOY_POV3 = 10,
	JOY_HAT4 = 11,
	JOY_POV4 = 11,
	JOY_MAX = 12
};

//
// Information returned about each joystick
//
typedef struct gosJoystick_Info
{
	PSTR lpstrName; // the brand and model name
	uint32_t bAxisValid : JOY_MAX; // yes/no bitflags.
	uint32_t nAxes : 4; // number of availab axes
	uint32_t nButtons : 5; // number of available buttons
	uint32_t nSliders : 4; // number of slidesr
	uint32_t nPOVs : 4; // number of point of view hats
	uint32_t bIsPolled : 1; // whether or not this stick is polled
	uint32_t bIsForceFeedback : 1;
	// 1 spare bit
} gosJoystick_Info;

//
// Used in defining a force feedback forces
//
enum gosForce : uint32_t
{
	gosForce_Constant,
	gosForce_Ramp,
	gosForce_Square,
	gosForce_Sine,
	gosForce_Triangle,
	gosForce_SawtoothUp,
	gosForce_SawtoothDown
};

#define gosFOREVER -1.0f

typedef struct
{
	float fAttackLevel;
	float fAttackTime;
	float fFadeLevel;
	float fFadeTime;
} gosJoystick_ForceEnvelope;

typedef struct _gosJoystick_ForceEffect
{
	gosForce eType;
	BOOLEAN bAffectedAxes[12];
	float fXOrigin, fYOrigin;
	float fMagnitude;
	float fDurationSec;
	float fScale;
	gosJoystick_ForceEnvelope* lpEnvelope;

	union {
		int32_t lPhase;
		float fStart;
	};
	union {
		float fOffset;
		float fEnd;
	};
	float fPeriodSec;

} gosJoystick_ForceEffect;

//
// Return the total number of joysticks detected when the application started.
//  This API may take up to 100ms to comlete, so should be called on a 'Refresh
//  button' in an options screen. When a joystick is unplugged during gameplay,
//  you need to call this API to have GameOS 're-detect' it. This will also
//  detect any new joysticks plugged into the computer since the application
//  started.
//
// If ReDetect is 0, the joysticks are not re-init'ed this can take up to two
// seconds on some hardware
//
uint32_t __stdcall gosJoystick_CountJoysticks(BOOLEAN ReDetect = 1);

//
// Return joystick <index>'s details in structure <gji>
//
void __stdcall gosJoystick_GetInfo(uint32_t index, gosJoystick_Info* gji);

//
// Turn the polling of joystick <index> on or off. howOften variable details the
// frequency of polling. A 'howOften' of .5 will poll twice per second, for
// example.	By default, it polls at 30Hz.
//
void __stdcall gosJoystick_SetPolling(uint32_t index, BOOLEAN yesNo, float howOften = 0.03333f);

//
// Return the current value of a particular joystick's particular axis. The
// value is mapped internally to range between -1.0f and 1.0f.
// except for POV which are at -1 when at rest, a %clockwise when engaged
//
float __stdcall gosJoystick_GetAxis(uint32_t index, GOSJoystickAxis axis);

//
// Return a value (FALSE unpressed, TRUE pressed) for the button <button> on
// joystick <index>.
//
BOOLEAN __stdcall gosJoystick_ButtonStatus(uint32_t index, uint32_t button);

//
// Play the precreated effect <ff>, iterating it <time> times.
//
void __stdcall gosJoystick_PlayEffect(HGOSFORCEEFFECT ff, uint32_t times = 1);

//
// Stop playing <ff>
//
void __stdcall gosJoystick_StopEffect(HGOSFORCEEFFECT ff);

//
// Create a force feedback effect <ff> on joystick <joy>, given the parameters
// specified in <fe>
//
void __stdcall gosJoystick_CreateEffect(
	HGOSFORCEEFFECT* ff, uint32_t joystick, gosJoystick_ForceEffect* fe);

//
// Load a feedback effect from an FFE file onto joystick <joy>
//
void __stdcall gosJoystick_LoadEffect(HGOSFORCEEFFECT* ff, uint32_t joystick, PSTR file);

//
// Update the force feedback effect <ff>, given the parameters specified in <fe>
//
void __stdcall gosJoystick_UpdateEffect(HGOSFORCEEFFECT ff, gosJoystick_ForceEffect* fe);

//
// Deallocate and stop resources dedicated to representing effect <fe>
//
void __stdcall gosJoystick_DestroyEffect(HGOSFORCEEFFECT* fe);

//
// Determine whether a feedback effect is still playing
//
BOOLEAN __stdcall gosJoystick_IsEffectPlaying(HGOSFORCEEFFECT fe);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// ****************************	MOUSE FUNCTIONS	*******************************
// //
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//
// The mouse position and buttons are read once a frame.
//
// Position returns values converted into position on window. Range is 0.0
// to 1.0  (0 to 639 on a 640 wide screen) When running in a window this may not
// match the sum of the deltas, as the windows mouse cursor can have
// acceleration
//
//
// Deltas returns values as a percentage of screen size. Range is -1.0 to 1.0
//
// ie: 0.5 means half a screen to the right or down
//
// The wheel position is only available as a delta value, Range is -1.0 to 1.0
// (999.0 means no wheel)
//
//
// ButtonsPressed are bitflags, 0=Left Button, 1=Right, 2=Middle, 3=X1, 4=X2 (X1
// and X2 are on 5 button mice)
//
// If you want to make sure you do not miss mouse clicks, check for the mouse
// buttons with
//
// You may pass zero as any parameter you do not wish to query
//
void __stdcall gos_GetMouseInfo(float* pXPosition, float* pYPosition, int32_t* pXDelta,
	int32_t* pYDelta, int32_t* pWheelDelta, uint32_t* pButtonsPressed);

//
// This function allows the application to move the windows mouse cursor.
//  It will take effect at the begining of the next frame. (ie: you can only
//  move the cursor once per frame)
//
void __stdcall gos_SetMousePosition(float XPosition, float YPosition);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// *****************************	TIME API	*******************************
// //
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//
// Return the current game time in seconds. This is updated only once every game
// loop. At the start of the game it is set to 0.0
//
// Environment.MaxTimeDelta can be set to limit the largest time span returned
// by this call.
//  For example, if this value was set to 0.25 then the largest time difference
//  between two 'game frames' will be 0.25 seconds. Any time greater than this
//  will be ignored and it will be reported to the game at only
//  'MinimumTimeDelta' has elapsed. This will smooth out CDROM spin up times, or
//  time spent inside the Visual C debugger.
//
//
// If 'RealTime' is set to 1 the time returned will NOT pause during
// ModalScripts. (And may be very different from normal gos_GetElapsedTime)
//
double __stdcall gos_GetElapsedTime(int32_t RealTime = 0);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// ****************************	   REGISTRY API	   ****************************
// //
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//
// Loads *szData bytes into *pData from the registry key specified.
// The data can be a uint32_t, Binary value or string
// If the key does not exist szData will be set to 0 and pData will not be
// altered If the *szData will be set to the actual number of bytes read
// Normally all registry data is saved and read from
// HKEY_CURRENT_USER\Software\Microsoft\Microsoft Games\App Name Data can be
// read from HKEY_LOCAL_MACHINE\Software\Microsoft\Microsoft Games\App Name, but
// not written (this would require a machine administrator) When 'BOOLEAN HKLM'
// on the read function is true it will read from LOCAL_MACHINE, otherwise
// CURRENT_USER. The write functions ONLY work to CURRENT_USER

void __stdcall gos_LoadDataFromRegistry(PSTR keyname, PVOID pdata, uint32_t* pcbdata, BOOLEAN ishklm = false);

//
// Saves szData bytes starting at pData into the registry key specified.
//
// If szData is 4 the data will be saved as a uint32_t, otherwise just binary
// data
//
void __stdcall gos_SaveDataToRegistry(PSTR keyname, PVOID pdata, uint32_t cbdata);

//
// Saves a string starting at pData, length szData into the registry key
// specified.
//
// This is the same as the gos_SaveDataToRegistry API, except it's saved as a
// readable string
//
void __stdcall gos_SaveStringToRegistry(PSTR keyname, PSTR pdata, uint32_t cbdata);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// *************************    NETWORK API	  ************************* //
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//

//
// Should be called before gos_StartNetworkGame - will see if the application
// was launched by
//  a lobby. This will join the game and be ready to send and recieve packets.
//  If the application was not launched by a lobby false will be returned. When
//  the game gets launched by a lobby server the command line will be "-lobby" -
//  the game should bypass any user interface and call gos_NetCheckLobby() as
//  soon as possible.
//
BOOLEAN __stdcall gos_NetCheckLobby(void);

//
// Enters GameOS 'modal scripts' that prompt for join/create network game.
//  Will only return if the user joins or creates a game and packets can be
//  sent, or will return false if the user cancels.
//
// GameOptionsScript is legacy and should be set to 0
// When Secure is true, encrypted packets are allowed and certain DPLAY messages
// will be encrypted and signed.
//
BOOLEAN __stdcall gos_NetStartGame(PSTR GameOptionsScript, BOOLEAN Secure = 0);

//
// Leaves the current network game.
//
void __stdcall gos_NetEndGame(void);

//
// Returns networking information
//
enum gosNetInfo : uint32_t
{
	gos_Networking, // (uint32_t)  Returns TRUE if in a networked game
	gos_AmITheServer, // (uint32_t)  Returns TRUE if you created the game
	// (FALSE=Joined)
	gos_NumberOfPlayers, // (uint32_t)  Returns the number of players in the
	// game
	gos_MyIPAddress, // (PSTR)  Returns string of IP address.  Can be 0.0.0.0 in
	// non-tcpip games
	gos_MyID, // (uint32_t)  Returns a PlayerID for this machines player NUMBER
	gos_ServerID, // (uint32_t)  Returns a PlayerID that represents the current
	// server
	gos_AllID, // (uint32_t)  Returns a PlayerID that represents 'All Players'
	gos_PlayerName, // (PSTR)  Returns a PSTR for the player NUMBER passed in
	// the parameter
	gos_PlayerID, // (uint32_t)  Returns a PlayerID for the player NUMBER passed
	// in the parameter
	gos_PlayerNumber, // (uint32_t)  Returns a NUMBER for the PlayerID passed in
	// the parameter (or 0xffffffff if not found)
	gos_LockedStatus, // (uint32_t)  Returns TRUE if the game is currently
	// locked
	gos_GameSecure, // (uint32_t)	Returns TRUE if the host created the game as a
	// secure game
	gos_GamePassword, // (PSTR)	Returns nullptr or a pointer to the ASCII
	// password used to create or join the game
	//
	// Networking status information
	//
	gos_PacketsSentLastFrame, // (uint32_t)  Returns number of packets sent in
	// the past frame to the player NUMBER passed
	gos_PacketsFromLastFrame, // (uint32_t)  Returns number of packets recieved
	// in the past Frame from the player NUMBER passed
	gos_BytesSentLastFrame, // (uint32_t)  Returns number of bytes sent in the
	// past Frame to the player NUMBER passed
	gos_BytesFromLastFrame, // (uint32_t)  Returns number of bytes recieved in
	// the past Frame from the player NUMBER passed
	gos_BandwithSentLastFrame, // (uint32_t)  Returns total number of bytes sent
	// in the past Frame to the player NUMBER passed
	// (including packet overhead)
	gos_BandwithFromLastFrame, // (uint32_t)  Returns total number of bytes
	// recieved in the past Frame from the player
	// NUMBER passed (including packet overhead)
	gos_PacketsLostLastFrame, // (uint32_t)  Returns number of packets lost from
	// the player NUMBER passed (based on packet
	// numbers and retries)
	gos_LatencyLastFrame, // (uint32_t)  Returns the average response time for
	// any guaranteed packets sent in the last frame to
	// the player NUMBER
	gos_GetMessageSendQueue, // (uint32_t)  Returns the number of messages still
	// in this machines send queue to the player NUMBER
	// passed
	gos_GetNumOutboudWindows, // (uint32_t)  Returns the number of outbound
	// window objects that exist
	gos_GetOutboundWindowSize, // (uint32_t)	Returns the current number of
	// outstanding packets sent to a given outbound
	// message window object
	gos_GetOB_NxtPkt2Send, // (uint32_t) Returns the next packet number to send
	// for a specified outbound window index
	gos_GetOB_LstPktRcvd, // (uint32_t) Returns the last packet received from a
	// specified outbound window index
	gos_GetOB_LstPktTheyRcvd, // (uint32_t) Returns the last packet that the
	// specified outbound window index received

	// Internal functions used for browsing, creating and joining games
	gos_InformationAboutGame, // (BOOLEAN*)  Returns a pointer to 16 bytes of
	// information from the game, Name=Game Name
	// (Games set this in Environment.NetGameInfo)
	gos_NumberOfGames, // (uint32_t)  Returns the number of Games found on
	// network
	gos_NameOfGame, // (PSTR)  Returns the name of the Game with the parameter
	// passed
	gos_IsGamePassworded, // (uint32_t)	Returns true if the game name passed
	// is passworded, Name=Game Name
	gos_IsGameSecure, // (uint32_t)	Returns true if the game name passed is
	// secure, Name=Game Name
	gos_IsGameLocked, // (uint32_t)	Returns true if the game name passed is
	// locked so no more players can enter, Name=Game Name
	gos_NumberOfPlayersInGame, // (uint32_t)  Returns number of players in a
	// game, Name=Game Name
	gos_MaxPlayersInGame, // (uint32_t)  Returns maximum number of players in a
	// game, Name=Game Name
	gos_NameOfPlayersInGame, // (PSTR)  Return players name - Parameter=Player
	// #, Name=Game Name.
	gos_NumberOfModems, // (uint32_t)  Returns the number of modems installed
	gos_NameOfModem, // (PSTR)  Returns the name of the modem number passed
	gos_NumberOfSerialPorts, // (uint32_t)  Returns the number of serial ports
	// installed
	gos_NameOfSerialPort, // (PSTR)  Returns the name of the serial port number
	// passed
};

PVOID __stdcall gos_NetInformation(gosNetInfo Info, uint32_t Parameter = 0, PSTR Name = 0);

//
// flags that can be used in NetPacket.flags
enum gosNetFlags : uint32_t
{
	gosNetFlag_guaranteed = 1, // Will arrive at destination in order and guaranteed.
	gosNetFlag_encrypt = 2, // Will encrypt the packet before sending (only valid in a secure game)
};

//
// These are the system messages that may be recieved by the application
//
const BOOLEAN gosNet_PlayerAdded =
	240; // FromID will contain the new PlayerID of the player.
	// gos_NetInformation(gos_NumberOfPlayers) will be greater than last game logic.
const BOOLEAN gosNet_PlayerDeleted =
	241; // FromID will contain the old PlayerID of the player.
	// gos_NetInformation will work with this ID only for the remainder of
	// the current GameLogic. (Packets sent to the player will be ignored)
const BOOLEAN gosNet_GameEnded = 242; // The server has ended the network game. This
	// is the last packet the game will see.
const BOOLEAN gosNet_Ping = 243; // Used to send a ping packet. You will eventually
	// receive back a ping packet containing four
	// uint32_t containing the milliseconds time at each
	// end of a 'double ping'.
const BOOLEAN gosNet_Heartbeat = 244; // Used to send a heartbeat packet to keep the
	// outbound window up to date.

// This is a network packet header for sent and recieved packets. This is not
// sent with each packet, it just contains packet information
#pragma pack(push, 1)
typedef struct _NetPacket
{
	uint32_t FromID; // Filled in by when packets are recieved
	uint32_t ToID; // Must be filled in to send a packet
	uint16_t flags; // Information flags (see gosNetFlag_*) - only valid on sending messages
	double TimeStamp; // Timestamp set by GameOS when the packet was recieved
	uint16_t Length; // Number of bytes of data.
	BOOLEAN Type; // Type of message (224-> System messages)
	PVOID pData; // Pointer to message data
} NetPacket;
#pragma pack(pop)

// Get the next network message, returns 0 when no more messages
NetPacket* __stdcall gos_NetGetMessage(void);

// Send a network message
BOOLEAN __stdcall gos_NetSendMessage(NetPacket* message);

// If this computer is the server these commands can be used
enum gos_NetCommands : uint32_t
{
	gos_Command_KillPlayer, // Remove the player with PlayerID == Data1 from the game
	gos_Command_LockGame, // Datat1=1 to lock the game, 0=to unlock the game - no more players can
	// join when a game is locked
	gos_Command_UpdateGameData, // Updates the 'game data' from/ Environment.NetGameInfo - so
	// players enumerating games can see the changes
	gos_Commend_UpdateMaxPlayers, // Updates the number of maximum players allowed in a game from
	// Environment.NetworkMaxPlayers
};

void __stdcall gos_NetServerCommands(gos_NetCommands Command, uint32_t Data1);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// *************************    UTILITY API	  ************************* //
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
// Seed the random number generator
//
void __stdcall gos_srand(uint32_t seed);
//
// Return a random number between 0 and 32767
//
int32_t __stdcall gos_rand(void);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// *************************	 3D Rasterizer API	  *************************
// //
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//
//
// Alpha blending can be quite confusing - so we provide a limited number of
// useful modes.
//
// OneZero has alpha blending disabled - the texture is drawn onto the
// background directly OneOne alpha blending is enabled, texture is added onto
// the background. AlphaInvAlpha means the alpha value*the vertex alpha does a
// blend between texture and background OneInvAlpha means the 1-alpha value*the
// vertex alpha effects how solid the background appears
//
enum gos_AlphaMode : uint32_t
{
	gos_Alpha_OneZero = 1, // Solid, no blending, 100% supported
	gos_Alpha_OneOne, // Used for saturation effects
	gos_Alpha_AlphaInvAlpha, // Most common blending mode used, 100% supported
	gos_Alpha_OneInvAlpha, // Wide support for this mode
	gos_Alpha_AlphaOne, // Alpha mode used in MW4
};

//
// Filter modes (Used for gos_State_Filter)
//
enum gos_FilterMode : uint32_t
{
	gos_FilterNone = 1, // Point sampling
	gos_FilterBiLinear, // BiLinear filtering
	gos_FilterTriLinear // The two closest mipmap levels are chosen and then
	// combined using a bilinear filter
};

//
// Texture addressing mode (Used for gos_State_TextureAddress)
//
enum gos_TextureAddressMode : uint32_t
{
	gos_TextureWrap = 1, // Wrap textures u,v's
	gos_TextureClamp, // Clamp u,v values to 0.0 and 1.0
};

//
// Texture shading mode (Used for gos_State_ShadeMode)
//
enum gos_ShadeAddressMode : uint32_t
{
	gos_ShadeFlat = 1, // The 1st vertices argb values use for all three vertices
	gos_ShadeGouraud, // All vertex argb colors are iterated across the triangle
};

//
// Texture blending mode (Used for gos_State_TextureMapBlend)
//
enum gos_TextureMapBlendMode : uint32_t
{
	gos_BlendDecal =
		1, // The vertex colors are ignored, the alpha and RGB are copied from the texture
	gos_BlendModulate, // When using an ALPHA or KEYED texture, the alpha comes from the texture.
	// When a SOLID texture, the alpha comes from the vertices.
	gos_BlendModulateAlpha, // The alpha at the vertices is multiplied with the alpha in the
	// texture. When a SOLID texture, the alpha comes from the vertices.
};

//
// Culling modes (Used for gos_State_Culling)
//
enum gos_CullMode : uint32_t
{
	gos_Cull_None = 1, // None - all triangles rendered
	gos_Cull_CW, // Clockwise polys not drawn
	gos_Cull_CCW // CounterClockWise polys not drawn
};

//
// Compare modes (Used for Stencil functions)
//
enum gos_CompareMode : uint32_t
{
	gos_Cmp_Never = 1, // Always fail the test
	gos_Cmp_Less = 2, // Accept when value is less than the reference
	gos_Cmp_Equal = 3, // Accept when value is equal to the reference
	gos_Cmp_LessEqual = 4, // Accept when value is less than or equal to the reference
	gos_Cmp_Greater = 5, // Accept when value is greater than the reference
	gos_Cmp_NotEqual = 6, // Accept when value is not equal to the reference
	gos_Cmp_GreaterEqual = 7, // Accept when value is greater than or equal to the reference
	gos_Cmp_Always = 8, // Always pass the test
};

//
// Functions for stencil operations
//
enum gos_StencilFunctions : uint32_t
{
	gos_Stencil_Keep = 1, // Do not update the stencil buffer
	gos_Stencil_Zero = 2, // Set stencil buffer to 0
	gos_Stencil_Replace = 3, // Set stencil buffer to reference value
	gos_Stencil_IncrSat = 4, // Increment and clamp at 255
	gos_Stencil_DecrSat = 5, // Decrement and clamp at 0
	gos_Stencil_Invert = 6, // Invert the stencil bits
	gos_Stencil_Incr = 7, // Increment the stencil value
	gos_Stencil_Decr = 8, // Decrement the stencil value
};

//
// Supported multitexture modes
//
enum gos_MutitextureModes : uint32_t
{
	gos_Multitexture_None = 0, // Multitexture is disabled
	gos_Multitexture_LightMap = 1, // The second texture will be modulate2X with the base texture
	gos_Multitexture_SpecularMap = 2, // The second texture will be ADDed with the base texture
	gos_Multitexture_DetailTexture = 3, // The second texture will be blended with the 1st based on
	// the alpha channel in the 1st texture
	gos_Multitexture_BumpEnvMap = 4, // Tex0=base (u0,v0), Tex1=bump (u0,v0), Tex2=envmap (u1,v1)
	gos_Multitexture_BumpDotMap = 5, // Tex0=base (u0,v0), Tex1=normalmap (u0,v0)
};

//
// Renderstate offsets in structure.
//
enum gos_RenderState : uint32_t
{
	gos_State_Texture = 1, // Default: 0						Current texture handle. 0=No texture
	gos_State_Texture2, // Default: 0						2nd Texture handle used in multitexture
	// rendering modes
	gos_State_Texture3, // Default: 0						3rd Texture handle used in bump environment map
	// rendering mode
	gos_State_Filter, // Default: gos_FilterNone			gos_FilterMode
	gos_State_ZCompare, // Default: 1						0=No z buffer test, 1=LessEqual test with z
	// buffer, 2=Less test
	gos_State_ZWrite, // Default: 1						true/false - write to zbuffer
	// enabled/disabled
	gos_State_AlphaTest, // Default: 0						true/false - Do not update screen or Z
	// with Alpha=0 pixels
	gos_State_Perspective, // Default: 1						true/false - Enable perspective
	// correct rendering
	gos_State_Specular, // Default: 0						true/false - Enable specular highlights
	gos_State_Dither, // Default: 0						true/false	(Improves gouraud shading)
	gos_State_Clipping, // Default: 0						1=D3D clipping, 2=Guard band clipping if available or
	// D3D Clipping if not
	gos_State_WireframeMode, // Default: 0						true/false - Enable wireframe mode
	gos_State_AlphaMode, // Default: gos_Alpha_OneZero		gos_AlphaMode
	gos_State_TextureAddress, // Default: gos_TextureWrap gos_TextureAddressMode
	gos_State_ShadeMode, // Default: gos_ShadeGouraud
	// gos_ShadeAddressMode (Note in FLAT, set all vertex colors to the same to allow clipping to
	// work correctly)
	gos_State_TextureMapBlend, // Default: gos_BlendModulateAlpha gos_TextureMapBlendMode
	gos_State_MipMapBias, // Default: 0						Number of larger mipmap levels to not
	// use (Max value=4 - would means 256*256 textures used the 16*16)
	gos_State_Fog, // Default: 0						Pointer to the fog color (when not 0 application
	// must use 'f' in vertex for fog density)
	gos_State_MonoEnable, // Default: 0						This is a hint to the software
	// rasterizer that the RGB colors of vertices are all the same
	// (intensity). Bit 8 enables 'pixel doubling' in Blade only.
	gos_State_Culling, // Default: gos_CullMode			Default gos_Cull_None, This renderstate
	// rejects polygons if they are facing away or towards the screen
	gos_State_StencilEnable, // Default: false					true/false  - Enable stencil
	// operations
	gos_State_StencilFunc, // Default: gos_Cmp_Never			Set to one of the gos_CompareMode
	// values
	gos_State_StencilRef, // Default: 0						Set to the pixel value to compare`
	gos_State_StencilMask, // Default: 0xffffffff				Used to mask the reference and pixel
	// before the compare
	gos_State_StencilZFail, // Default: gos_Stencil_Keep		Operation on the stencil buffer to
	// perform when the function passes, but the Z buffer test fails
	gos_State_StencilFail, // Default: gos_Stencil_Keep		Operation on the stencil buffer to
	// perform when the function fails
	gos_State_StencilPass, // Default: gos_Stencil_Keep		Operation on the stencil buffer to
	// perform when the function passes and so does the Z buffer test
	gos_State_Multitexture, // Default: gos_Multitexture_None	Select multitexture mode for
	// rendering (see gos_GetMachineInformation to see if modes are valid)
	gos_State_Ambient, // Default: 0xffffff				Set the Ambient lighting for the scene when
	// using hardware T&L
	gos_State_Lighting, // Default: 0 true / flase - Enables D3D lighting/ when using hardware T&L
	gos_State_NormalizeNormals, // Default: 0 true / false - Enables automatic normalization of
	// vertex normals when using hardware T&L
	gos_State_VertexBlend, // Default: 0 Number of blend weights in each vertex to blend (Only 0 or
	// 1 valid currently)
	gos_MaxState // Marker for last render state
};

//
// Vertex used by low level rasterizer
//
// You should us the following equation to turn x,y coords from 0.0-1.0 into
// valid values inside the current viewport. (Use GetViewport to get these
// values). The application can work these values out itself
// Environment.screenwidth and height should be used. Note that the screen size
// could change each frame, so make sure the application can deal with this.
//
// x = x*ViewportMulX + ViewportAddX
// y = y*ViewportMulY + ViewportAddY
//
typedef struct _gos_VERTEX
{
	float x, y; // Screen coords	- must be 0.0 to Environment.screenwidth/height (no clipping occurs unless gos_State_Clipping is true)
	float z; // 0.0 to 0.99999	- Used for visibility check in zbuffer (1.0 is not valid)
	float rhw; // 0.0 to 1.0		- reciprocal of homogeneous w - Used for perspective correct textures, fog and clipping
	uint32_t argb; // Vertex color and alpha (alpha of 255 means solid, 0=transparent)
	uint32_t frgb; // Specular color and fog
	float u, v; // Texture coordinates
} gos_VERTEX;
typedef gos_VERTEX* pgos_VERTEX;

//
// This vertex type is used for rendering with 2 textures at once, it is
// identical to the normal vertex structure, but with 2 u,v's
//
typedef struct _gos_VERTEX_2UV
{
	float x,
		y; // Screen coords	- must be 0.0 to Environment.screenwidth/height (no
		// clipping occurs unless gos_State_Clipping is true)
	float z; // 0.0 to 0.99999	- Used for visiblity check in zbuffer (1.0 is
		// not valid)
	float rhw; // 0.0 to 1.0		- reciprocal of homogeneous w - Used for
		// perspective correct textures, fog and clipping
	uint32_t argb; // Vertex color and alpha (alpha of 255 means solid,
		// 0=transparent)
	uint32_t frgb; // Specular color and fog
	float u1, v1; // Texture coordinates
	float u2, v2; // Texture coordinates
} gos_VERTEX_2UV;
typedef gos_VERTEX_2UV* pgos_VERTEX_2UV;

//
// This vertex type is used for rendering with 3 textures at once, it is
// identical to the normal vertex structure, but with 3 u,v's Expected use:
// u1,v1 = base texture  u2,v2 = bump map  u3,v3 = environment map
//
typedef struct _gos_VERTEX_3UV
{
	float x,
		y; // Screen coords	- must be 0.0 to Environment.screenwidth/height (no
		// clipping occurs unless gos_State_Clipping is true)
	float z; // 0.0 to 0.99999	- Used for visiblity check in zbuffer (1.0 is
		// not valid)
	float rhw; // 0.0 to 1.0		- reciprocal of homogeneous w - Used for
		// perspective correct textures, fog and clipping
	uint32_t argb; // Vertex color and alpha (alpha of 255 means solid,
		// 0=transparent)
	uint32_t frgb; // Specular color and fog
	float u1, v1; // Texture coordinates
	float u2, v2; // Texture coordinates
	float u3, v3; // Texture coordinates
} gos_VERTEX_3UV;
typedef gos_VERTEX_3UV* pgos_VERTEX_3UV;

//
// Setup a viewport, must be called before drawing
//
// Defines the region of the monitor the 3D objects will be rendered too.
// 0.0 0.0 1.0 1.0 would be whole screen with 0,0 in the top left
// 1.0 1.0 0.0 0.0 would be whole screen with 0,0 in bottom right
// -0.5 -0.5 0.5 0.5 would be whole screen with 0.0 in the middle
//
// If fillz is true the zbuffer will be cleared with the value Zbuffer (0.0
// to 1.0) If fillbg is true the background will be cleared to the color bgcolour
//
// Normal applications will fill the Z buffer with 1.0
//
void __stdcall gos_SetupViewport(BOOLEAN fillz, float zbuffer, BOOLEAN fillbg, uint32_t bgcolour,
	float top, float left, float bottom, float right, BOOLEAN clearstencil = false,
	uint32_t stencilvalue = 0);

//
// The values returned must be used to transform the x,y coords into the current
// viewport window. This is how the values should be used.
//
// x = x*ViewportMulX + ViewportAddX
// y = y*ViewportMulY + ViewportAddY
//
// If you call gos_SetupViewport you should call this function again. You should
// also call this function at least once per frame as when the window size
// changes, these values may change.
//
void __stdcall gos_GetViewport(
	float* pViewportMulX, float* pViewportMulY, float* pViewportAddX, float* pViewportAddY);

//
// Draw points, pass a pointer to an array of gos_VERTEX's - every vertex is a
// new point
//  (you must set the texture to 0 if you want to disable texture mapping on the
//  points)
//
MECH_IMPEXP HRESULT MECH_CALL
gos_DrawPoints(pgos_VERTEX Vertices, uint32_t NumVertices);

//
// Draw lines, pass a pointer to an array of gos_VERTEX's - every two vertices
// is a new line
//  (you must set the texture to 0 if you want to disable texture mapping on the
//  points)
//
MECH_IMPEXP HRESULT MECH_CALL
gos_DrawLines(pgos_VERTEX Vertices, uint32_t NumVertices);

//
// Draw triangles, pass a pointer to an array of gos_VERTEX's - every three
// vertices is a new triangle
//
MECH_IMPEXP HRESULT MECH_CALL
gos_DrawTriangles(pgos_VERTEX Vertices, uint32_t NumVertices);

//
// Draws a strip, pass a pointer to an array of gos_VERTEX's - every vertex
// after the 2nd is a new triangle
//
MECH_IMPEXP HRESULT MECH_CALL
gos_DrawStrips(pgos_VERTEX Vertices, uint32_t NumVertices);

//
// Draws a fan, pass a pointer to an array of gos_VERTEX's - every vertex after
// the 2nd is a new triangle
//
MECH_IMPEXP HRESULT MECH_CALL
gos_DrawFans(pgos_VERTEX Vertices, uint32_t NumVertices);

//
// Draw quads, pass a pointer to an array of gos_VERTEX's - every four vertices
// is a new quad
//
MECH_IMPEXP HRESULT MECH_CALL
gos_DrawQuads(pgos_VERTEX Vertices, uint32_t NumVertices);

//
// This API allows you to pass an array of indices and an array of vertices to
// be rendered.
//
// The x,y positions must be already transformed into the viewport using the
// constants returned by gos_GetViewport.
//
// The vertex colors in the array may be changed in some wireframe modes.
// Otherwise the data is not altered.
//
MECH_IMPEXP HRESULT MECH_CALL
gos_RenderIndexedArray1(pgos_VERTEX pVertexArray,
	uint32_t numbervertices, uint16_t* pwindices, uint32_t numberindices);
MECH_IMPEXP HRESULT MECH_CALL
gos_RenderIndexedArray2(pgos_VERTEX_2UV pVertexArray,
	uint32_t numbervertices, uint16_t* pwindices, uint32_t numberindices);
MECH_IMPEXP HRESULT MECH_CALL
gos_RenderIndexedArray3(pgos_VERTEX_3UV pVertexArray,
	uint32_t numbervertices, uint16_t* pwindices, uint32_t numberindices);

//
// Set a renderstate
//
// All renderstates have defaults, they stay set over multiple frames until
// changed.
//
void __stdcall gos_SetRenderState(gos_RenderState renderstate, int32_t value);

//
// Save and restore all the current renderstate settings. These are saved on a
// linked list, so can be nested. If they are not all pop'ed by the end of
// update renderers, an error will be generated
//
void __stdcall gos_PushRenderStates(void);
void __stdcall gos_PopRenderStates(void);

//
// Vertex definition for transform and lighting (Identical to D3DVERTEX)
//
typedef struct
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;

} gosD3DVERTEX;

//
// Vertex definition for transform and lighting with two u,v coordinates
//
typedef struct
{
	float x, y, z;
	float nx, ny, nz;
	float u1, v1, u2, v2;

} gosD3DVERTEX_2UV;

//
// Vertex definition for transform with no lighting
//
typedef struct
{
	float x, y, z;
	uint32_t unused; // This uint32_t is unused and is here to match the
		// D3DLVETEX type
	uint32_t argb;
	uint32_t frgb;
	float u, v;

} gosD3DLVERTEX;

//
// Vertex definition for transform with no lighting with two u,v coordinates
//
typedef struct
{
	float x, y, z;
	uint32_t argb;
	uint32_t frgb;
	float u1, v1, u2, v2;

} gosD3DLVERTEX_2UV;

//
// Vertex definition for transform, blending and lighting with two u,v
// coordinates
//
typedef struct
{
	float x, y, z;
	float blend;
	float nx, ny, nz;
	float u, v;

} gosD3DBLENDVERTEX;

//
// Vertex definition for transform, blending and lighting with two u,v
// coordinates
//
typedef struct gosD3DBLENDVERTEX_2UV
{
	float x, y, z;
	float blend;
	float nx, ny, nz;
	float u1, v1, u2, v2;

} gosD3DBLENDVERTEX_2UV;

//
// Used to specify type of vertex when creating vertex buffer or rendering an
// array of vertices
//
enum gosVERTEXTYPE : uint32_t
{
	VERTEX_gos_VERTEX = 1, // Old style untransformed and unlit vertex
	VERTEX_gos_VERTEX_2UV, // Old style untransformed and unlit vertex with TWO u,v coords
	VERTEX_gosD3DLVERTEX, // D3D transforms and clips by does not light
	VERTEX_gosD3DLVERTEX_2UV, // D3D transforms and clips by does not light with TWO u,v coords
	VERTEX_gosD3DVERTEX, // D3D transforms, clips and lights
	VERTEX_gosD3DVERTEX_2UV, // D3D transforms, clips and lights with TWO u,v coords
	VERTEX_gosD3DBLENDVERTEX, // D3D transforms, clips, blends and lights
	VERTEX_gosD3DBLENDVERTEX_2UV, // D3D transforms, clips, blends and lights with TWO u,v coords
	VERTEX_gos_VERTEX_3UV, // Old style untransformed and unlit vertex with TWO u,v coords
};

//
// Vector definition (Identical to D3DVECTOR)
//
#if CONSIDERED_OBSOLETE
typedef struct
{
	float x, y, z;

} gosVECTOR;
#endif

//
// Matrix definition (Identical to D3DMATRIX)
#if CONSIDERED_OBSOLETE
typedef struct gosMATRIX
{
	union {
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};

} gosMATRIX;
#endif
// #define gosMATRIX (struct _D3DMATRIX)
//
// colour definition (Identical to D3DCOLORVALUE)
#if CONSIDERED_OBSOLETE
typedef struct gosCOLOR
{
	float r, g, b, a;
} gosCOLOR;
#endif

//
// Material definition (Identical to D3DMATERIAL7)
#if CONSIDERED_OBSOLETE
typedef struct gosMATERIAL
{
	gosCOLOR diffuse;
	gosCOLOR ambient;
	gosCOLOR specular;
	gosCOLOR emissive;
	float power;
} gosMATERIAL;
#endif

//
// Used to specify type of light (Identical to D3DLIGHTTYPE)
#if CONSIDERED_OBSOLETE
enum class gosLIGHTTYPE : uint32_t
{
	LIGHT_POINT = 1,
	LIGHT_SPOT,
	LIGHT_DIRECTIONAL,
} gosLIGHTTYPE;
#endif

//
// Maximum distance for lights
//
#define LIGHT_RANGE_MAX ((float)sqrt(3.402823466e+38f))

//
// Light definition (Identical to D3DLIGHT7)
#if CONSIDERED_OBSOLETE
typedef struct gosLIGHT
{
	gosLIGHTTYPE dltType;
	gosCOLOR dcvDiffuse;
	gosCOLOR dcvSpecular;
	gosCOLOR dcvAmbient;
	D3DVECTOR dvPosition;
	D3DVECTOR dvDirection;
	float dvRange;
	float dvFalloff;
	float dvAttenuation0;
	float dvAttenuation1;
	float dvAttenuation2;
	float dvTheta;
	float dvPhi;
} gosLIGHT;
#endif

//
// Used to specify which matrix (Identical to D3DTRANSFORMSTATETYPE)
#if CONSIDERED_OBSOLETE
enum class gosTRANSFORMTYPE : uint32_t
{
	MATRIX_WORLD = 1,
	MATRIX_VIEW,
	MATRIX_PROJECTION,
	MATRIX_WORLD1, // 2nd world matrix used in vertex blending
} gosTRANSFORMTYPE;
#endif
#define gosTRANSFORMTYPE D3DTRANSFORMSTATETYPE
//
// Used to specify type of primitive being rendered in gos_RenderIndexedArray
// and gos_RenderVertexBuffer
//
enum gosPRIMITIVETYPE : uint32_t
{
	PRIMITIVE_POINTLIST = 1,
	PRIMITIVE_LINELIST = 2,
	PRIMITIVE_TRIANGLELIST = 3,
};

//
// Sets the region of screen that will be rendered to. (The default values set
// at the start of each frame are 0, 0, Environment.screenwidth,
// Environment.screenheight)
//
void __stdcall gos_SetViewport(uint32_t LeftX, uint32_t RightX, uint32_t width, uint32_t height,
	float MinZ = 0.0f, float MaxZ = 1.0f);

//
// Sets the WORLD/VIEW or PROJECTION matrix
//
void __stdcall gos_SetTransform(D3DTRANSFORMSTATETYPE TransformType, LPD3DMATRIX pMatrixData);

//
// Multiplies the current WORLD/VIEW or PROJECTION matrix by another
//
void __stdcall gos_MultiplyTransform(D3DTRANSFORMSTATETYPE TransformType, LPD3DMATRIX pMatrixData);

//
// Sets the parameters of a light (LightNumber can be any number)
//
void __stdcall gos_SetLight(uint32_t LightNumber, LPD3DLIGHT7 pLightData);

//
// Enables / Disables a light (Maximum of gos_GetMachineInformation(
// gos_Info_GetMaximumActiveLights ) lights enabled at once)
//
void __stdcall gos_LightEnable(uint32_t LightNumber, BOOLEAN Enable);

//
// Sets the parameters of the current material
//
void __stdcall gos_SetMaterial(LPD3DMATERIAL7 pMaterialData);

//
// Transforms and renders an indexed array of vertices
//
// NOTE: When primitivetype is a POINTLIST the pwindices and numberindices are
// ignored, it is treated as a list of points
//
void __stdcall gos_RenderIndexedArray0(PVOID pVertexArray, uint32_t numbervertices,
	uint16_t* pwindices, uint32_t numberindices, gosVERTEXTYPE vertextype,
	gosPRIMITIVETYPE primitivetype = PRIMITIVE_TRIANGLELIST);

//
// Returns a handle to a vertex buffer
//
// If the Optimized flag is set, when the Vertex Buffer is locked and unlocked
// the first time it will be optimized. It cannot be locked again after this.
// Only valid in game logic - not valid in update renderer.

// vertexbuffer.cpp
uint32_t __stdcall gos_CreateVertexBuffer(uint32_t numbervertices, BOOLEAN Optimized, gosVERTEXTYPE vertextype);

//
// Deletes the vertex buffer
//
// Only valid in game logic - not valid in update renderer.
//
void __stdcall gos_DeleteVertexBuffer(uint32_t vertexbufferhandle);

//
// Locks a vertex buffer and returns a pointer to the base address
//
// If the discardcontents flag is true, D3D may return a pointer to a new vertex
// buffer, the old contents will be discarded. If the nooverwrite flag is true,
// D3D will assume you are only ADDING new vertices, D3D may still be rendering
// old vertices in the buffer. Valid in game logic or the update renderer
//
PVOID __stdcall gos_LockVertexBuffer(uint32_t vertexbufferhandle, BOOLEAN discardcontents, BOOLEAN nooverwrite);

//
// Unlocks a vertex buffer
//
// Valid in game logic or the update renderer
//
void __stdcall gos_UnLockVertexBuffer(uint32_t vertexbufferhandle);

//
// Application should check this at the start of a frame. If the user has
// changed modes or video cards, this will be true. All vertex buffers will be
// marked INVALID and cannot be rendered with until locked and unlocked. (Their
// content restored).
//
// If you pass no parameter, or 0 it will tell you if ALL vertex buffers have
// been lost since the last frame. If you pass a specific handle to a vertex
// buffer it will tell you if that vertex buffer is currently lost (so you only
// have to restore vertex buffers when you need them)
//
BOOLEAN __stdcall gos_VertexBuffersLost(uint32_t vertexbufferhandle = 0);

//
// Renders a vertex buffer of untransformed vertices
//
// NOTE: When primitivetype is a POINTLIST the pwindices and numberindices are
// ignored, it is treated as a list of points
//
void __stdcall gos_RenderVertexBuffer(uint32_t vertexbufferhandle, uint32_t startvertex,
	uint32_t numbervertices, uint16_t* pwindices, uint32_t numberindices,
	gosPRIMITIVETYPE primitivetype = PRIMITIVE_TRIANGLELIST);

//
// This call is only valid during GameLogic (not inside update renderers).
//
// Applications should not assume the mode has changed, because the user could
// always override it, enter a window mode, an error caused software rendering
// etc...
//
// Applications should always use the current Environment variable for screen
// width and height. These are valid and will not change during UpdateRenderers.
//
// Applications can use gos_GetMachineInformation to query for device
// information.
//
// bitDepth can be 16 or 32 only
// Device can be 0 to gos_GetMachineInformation(gos_Info_NumberDevices)-1
//
// disableZBuffer, AntiAlias and RenderToVram can be used during different
// sections of the game, for example the interface to speed up rendering.
//
// GotoFullScreen if true will force the Game into full screen mode. If false it
// will stay in the mode it is already in (window/full screen).
//
// DirtyRectangle is Bit 0=Enabled, Bit 1=Save Z buffer rectangles too, Bit
// 2=Save directly in system memory (don't try video memory).
//
// GotoWindowMode if true will force the game into a window mode. If false it
// will stay in the mode it is already in. (Not valid when GotoFullScreen is
// also true)
//
// EnableStencil if true will try to enable an 8 bit stencil buffer. Use
// gos_GetMachineInformation(gos_Info_HasStencil) to see if you have a stencil
// buffer available
//
// Renderer is 0 for hardware or 3 for Blade
//
void __stdcall gos_SetScreenMode(uint32_t width, uint32_t height, uint32_t bitdepth = 16,
	uint32_t device = 0, BOOLEAN disablezbuffer = 0, BOOLEAN antialias = 0,
	BOOLEAN rendertovram = 0, BOOLEAN gotofullscreen = 0, int32_t dirtyrectangle = 0,
	BOOLEAN gotowindowmode = 0, BOOLEAN enablestencil = 0, uint32_t renderer = 0);

//
// This API sets the current gamma correction value. The default value is 1.0
// (no correction applied). All color values are effected by (value/255 ^
// (1.0/gamma)).
//
void __stdcall gos_SetGammaValue(float gamma); // Default 1.0
void __stdcall gos_SetBrightnessValue(uint32_t brightness); // value range 0-10,000, default 750 (See DirectX docs)
void __stdcall gos_SetContrastValue(uint32_t contrast); // value range 0-20,000, default 10,000 (See DirectX docs)

//
// When this is executed the WHOLE back buffer will be saved to the copy surface
//  (if you requested the Z buffer to be saved, it will also be copied).
//  You should call this when you have drawn the initial background scene.
//
void __stdcall gosDirtyRectangleSaveTarget(void);

//
// Specifies a region that has been rendered to on the back buffer.
//  Before the next BeginScene, GameOS will copy all these regions specified in
//  the previous frame from the copy buffer to the back buffer. In this way the
//  whole scene does not have to be re-rendered, only the areas that actually
//  are animating or moving. If you open the debugger (Control Break) or use
//  frame graphs the whole screen will be invalidated and it will be completly
//  restored from the copy of the back buffer. This may be slower if the copy is
//  in system memory, but at least all the debugger functions should still
//  operate as normal.
//
void __stdcall gosDirtyRectangeRestoreArea(uint32_t Left, uint32_t Top, uint32_t Right, uint32_t Bottom);

//
// The application should call this API at the start of UpdateRenderers.
//  If this returns TRUE it should re-render the whole scene and save the back
//  buffer again. This will happen if the saved surfaces were in video memory
//  and the mode has been changed. If it returns false the application can
//  assume all the dirty rectangles have been copied over and it is free to
//  continue rendering. On some hardware, where it is not possible to
//  save/restore the back buffer this may return true every frame.
//
BOOLEAN __stdcall gosDirtyRectangeLostTarget(void);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// *************************    Texture Manager API	  *************************
// //
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//
//
// Used when loading a texture - the source file will be loaded into a surface
// with this type (exact format may vary on each card)
//
//#if CONSIDERED_UNUSED
enum gos_TextureFormat
{
	gos_Texture_Detect =
		-1, // Looks at the texture alpha channel and makes it a solid, keyed or alpha
	gos_Texture_Solid = 0, // 565 or 555 (no transparency or translucency)
	gos_Texture_Keyed = 1, // 1555 (only a single bit of alpha)
	gos_Texture_Alpha =
		2, // 4444 or failing that 8888 (maximum alpha with at least 12 bits of RGB - ie: not 8332)
	gos_Texture_Bump = 3, // dUdV bump map (create from monochrome image)
	gos_Texture_Normal = 4, // texture map of normals (create from monochrome image)
};
//#endif

//
// Texture hints - These help improve the performance of the texture manager.
// The more hints you use the better the application will run. Leaving the hints
// as 0 (default) may result in poor performance.
//
// Each texture has specific hints - if you want the same texture image to have
// two different hints you must load it twice into memory.
//
enum gos_TextureHints : uint32_t
{
	gosHint_DisableMipmap = 1, // Do not mipmap this texture (HUD textures, fonts, animating textures maybe? etc...)
	gosHint_AGPMemory = 2, // Store in AGP memory if possible (allows processor and video card fast access, not as fast rendering as VideoMemory though)
	gosHint_VideoMemory = 4, // Store in "Local" memory on the video card, this is the fastest for the card to texture, but very slow for the processor to page or animate
	gosHint_ReloadFromDisk = 8, // GameOS will not store a uint32_t copy of the texture, it will reload it from disk on mode changes. (You cannot lock these textures)
	gosHint_PageLast = 16, // Frequently used textures should have this set. These textures will be paged ONLY if there are no other HighPriority texture that could be paged
	gosHint_PageFirst = 32, // Rarely used textures should have this set. These will be the first textures paged if paging needs to occur.
	gosHint_DontShrink = 64, // GOS may shrink textures on a low memory card - this flag will prevent it. (ie: for fonts or HUD textures)
	gosHint_Try32bpp = 128, // Use 8888 format on this texture if available
	gosHint_MipmapFilter0 = 256,
	gosHint_MipmapFilter1 = 512, // These bits can specify 8 different types of Mipmap filtering (0=Box filter - fast/accurate, 1=Point sample - very fast/ok
	gosHint_ROMFile = 1024, // For resource or ROM files (where GetFile was hooked) - doesn't free memory when used
	gosHint_UserMipMaps = 2048, // The user must load or write all mipmap levels (filter ignored) - invalid when used with gos_NewTextureFromMemory
	gosHint_PinkColorKey = 4096, // When the texture is loaded, any pink colored pixels (0xff00ff) will be set to ALPHA=0, all other colors to ALPHA=0xff
	gosHint_ForceAlpha = 8192, // When gos_Texture_Detect is specified as a format, an alpha texture (KEYED or ALPHA) will be returned. If a solid texture is loaded, the blue channel will be used as the alpha
	gosHint_Compress0 = 16384, // Bit 0 of compression
	gosHint_Compress1 = 32768, // Bit 1 of compression.  00=no compress, 01=some compress 10=very compressed
};

//
// Used when locking and unlocking a texture
//
typedef struct TEXTUREPTR
{
	uint32_t* pTexture;
	uint32_t width; // In pixels
	uint32_t height; // In pixels
	uintptr_t Pitch; // In uintptr_t (so can be added to pTexture to move down a line)	CHECK!!!
	gos_TextureFormat Type; // Internal format of texture (Solid, Keyed or Alpha) - Useful when you have used DETECT
} TEXTUREPTR;

typedef void(__stdcall* gos_RebuildFunction)(uint32_t, PVOID);

//
// Load a texture, returns a texture handle that can be passed as a renderstate
//
// Detect		- will look at the file's alpha channel, if no alpha found, 'Solid',
// within 10% variance of 0 and 255 'keyed', varied alpha means 'alpha' Solid
// - will load into a 555 or 565 texture (no transparency or translucency) Keyed
// - will load into the smallest alpha surface ie: 1555 Alpha		- will load
// into the largest alpha + at least 15 bit RGB surface ie: 4444 or 8888
//
// The file can only be a 24 or 32 bit compressed or uncompressed .TGA file
// currently
//
// Hints should be set to any combination of gos_TextureHints or 0 is a good
// default.
//

uint32_t __stdcall gos_NewTextureFromFile(gos_TextureFormat Format, PSTR FileName,
	uint32_t Hints = 0, gos_RebuildFunction pFunc = 0, PVOID pInstance = 0);

//
// Loads a texture from a memory image, returns a texture handle
//
// Detect		- will look at the file alpha channel, if no alpha found, 'Solid',
// within 10% variance of 0 and 255 'keyed', varied alpha means 'alpha' Solid
// - will load into a 555 or 565 texture (no transparency or translucency) Keyed
// - will load into the smallest alpha surface ie: 1555 Alpha		- will load
// into the largest alpha + at least 15 bit RGB surface ie: 4444 or 8888
//
// The filename is used to decode the image (if .TGA or .JPG) and for friendlier
// debugging
//
// GameOS does not require the texture memory after this call - it can be reused
// or freed by the application
//
// Hints should be set to any combination of gos_TextureHints or 0 is a good
// default.
//
uint32_t __stdcall gos_NewTextureFromMemory(gos_TextureFormat Format, PSTR FileName,
	BOOLEAN* pBitmap, uint32_t Size, uint32_t Hints = 0, gos_RebuildFunction pFunc = 0,
	PVOID pInstance = 0);

#define RECT_TEX(width, height) (((height) << 16) | (width))

//
//
// Creates and empty texture, returns a texture handle
//
// Only square textures can be created, so only a width is specified.
//
// Only textures with sizes already allocated in heaps can be created (ie: 512,
// 256, 128, 64, 32 or 16*16)
//
// Detect       - Not a valid option
// Solid		- will load into a 555 or 565 texture (no transparency or
// translucency) Keyed		- will load into the smallest alpha surface ie: 1555
// Alpha		- will load into the largest alpha + at least 15 bit RGB surface ie:
// 4444 or 8888
//
// The name passed is only used for better debugging - it can be nullptr.
//
// Hints should be set to any combination of gos_TextureHints or 0 is a good
// default.
//
// For square textures, place the width in heightwidth
// For Rectangular textures, use RECT_TEX(width,height) to pack the width and
// height values
//
uint32_t __stdcall gos_NewEmptyTexture(gos_TextureFormat Format, PSTR Name, uint32_t heightwidth,
	uint32_t Hints = 0, gos_RebuildFunction pFunc = 0, PVOID pInstance = 0);

//
// Destroy a texture handle (unload from memory)
//
void __stdcall gos_DestroyTexture(uint32_t handle);

//
// Locks a texture so you can modify it
//
// MipmapSize should be set to 0 - the TextureInfo will then be correct for the
// largest texture. If MipMapSize specifies a texture width that is valid, a
// pointer to that mipmap will be returned (ie: MipMapSize=128) If the ReadOnly
// flag is set the texture is not pixel format converted and reuploaded to video
// memory once unlocked
//
// Note that GameOS returns a pointer to an 8888 version of every texture. When
// you unlock it, it will be format converted and reuploaded in the current
// pixel format (555/4444 etc...)
//
void __stdcall gos_LockTexture(
	uint32_t handle, uint32_t MipMapSize, BOOLEAN ReadOnly, TEXTUREPTR* TextureInfo);

//
// Unlocks and updates a texture (may have to build and upload mipmaps)
//
void __stdcall gos_UnLockTexture(uint32_t handle);

//
// Converts from 32bpp source to subrect of n-bpp dest, bypassing intermediate
// 32bpp buffer
//
void __stdcall gos_ConvertTextureRect(uint32_t handle, uint32_t DestLeft, uint32_t DestTop,
	uint32_t* Source, uint32_t SourcePitch, uint32_t width, uint32_t height);

//
// Reinitialise texture manager - recreates the video memory texture heaps based
// on the Environment settings.
//
// Returns false if not enough texture memory (reduce heap sizes)
//
BOOLEAN __stdcall gos_RecreateTextureHeaps(void);

//
// Preloads a texture into texture memory - this occurs after the endscene/blit.
// So may occur in parallel with the next game logic loop.
//
void __stdcall gos_PreloadTexture(uint32_t handle);

void __stdcall gos_SetTextureName(uint32_t handle, PSTR name);
PSTR __stdcall gos_GetTextureName(uint32_t handle);

//
// Render to a texture using blade.
//
// Currently GameOS only supports rendering to the alpha bit of a KEYED texture
// (ie: 1555). When you start rendering, the texture will be filled with the
// value 0x0000. Where you render with blade, the value 0x8000 will be put in
// the texture. If you render a model from the lights point of view, you could
// now use this texture as a 'shadow map'.
//
//   . The texture handle passed must be a texture created with these flags:
//   gos_NewEmptyTexture( gos_Texture_Keyed,
//   gosHint_DisableMipmap|gosHint_DontShrink ) . Only one texture can be
//   rendered to at a time. . No rendering to the back buffer can occur between
//   these two calls. . A texture that has been already used in the current
//   frame cannot be rendered too. . No renderstates are allowed between these
//   two calls.
//
void __stdcall gos_StartRenderToTexture(uint32_t handle);
//
// When rendering to a texture is complete, this API must be called to process
// it.
//
// The 'ClearBorder' parameter will cause GameOS to clear a single pixel border
// around the texture. This allows the texture to be used in CLAMP mode with no
// artifacts.
//
void __stdcall gos_EndRenderToTexture(BOOLEAN ClearBorder = 0);

//
// Returns true if the TGA has a valid header.
//
BOOLEAN __stdcall gos_CheckValidTGA(BOOLEAN* Data, uint32_t DataSize);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// *************************  Debugging/Profilling API  ***********************
// //
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//
//
// Statistics API - adds variables to a list that can be graphed or watched
// during gameplay
//
//
enum gosType : uint32_t
{
	gos_DWORD = 1,
	gos_WORD,
	gos_BYTE,
	gos_int,
	gos_short,
	gos_char,
	gos_float,
	gos_double,
	gos_int64,
	gos_timedata,
	gos_cycledata
};
///
// flags used in AddStatistic
//
enum StatFlags : uint32_t
{
	Stat_AutoReset = 1, // After statistics read, zero memory variable
	Stat_Graph = 2, // Enable graphing of this variable
	Stat_1DP = 4, // Show one decimal place
	Stat_2DP = 8, // Show two decimal places
	Stat_3DP = 16, // Show three decimal places
	Stat_Total = 32, // Just display total - no minimum, maximum, average
	Stat_GraphMax = 64, // The graph has a fixed maximum, which is passed as the
	// initial value
};

//
// Adds a value to the list of variables being watched each frame
//
// Name      is the name of the variable ("Frame Rate")
// TypeName  is the name of the type of variable ("Hz")
// Type      is how the varibles is stored
// value     is a pointer to the value
// flags	 One or more of the StatFlags listed above
//
// Each statistic added requires about 4K of space allocated (for graphing
// values)
//
// gos_timedata  - value should point to an int64_t, filled with the time a
// function takes using the GetCycles().
//					It will be displayed as a percentage of total frame time
// gos_cycledata - Values should point to an gos_CycleData structure, filled
// with the StartCycleTiming and EndCycleTiming apis.
//					It will be displayed as processor information. Eventually cycles,
// cache, bus etc..
//
//
void __stdcall AddStatistic(PSTR Name, PSTR TypeName, gosType Type, PVOID value, uint32_t flags);
//
// This command adds text to the statistic display and can be used for blank
// lines (eg: " ") or titles (eg: "Ai Logic variables")
//
void __stdcall StatisticFormat(PSTR string);

//
// This API will return the value for a statistic. If the statistic is a timer
// the value returned will always be in milliseconds, never percentage. The
// 'Frame' parameter can be used to return old data from previous frames. The
// values 0 to -511 are valid.
//
float __stdcall gos_ReturnStatistic(PSTR Name, int32_t Frame = 0);

//
// Place a start and end Cycle timing around code to be optimized to display
// processor statistics in the debugger
//
void __stdcall StartCycleTiming(struct gos_CycleData* Time);
void __stdcall EndCycleTiming(struct gos_CycleData* Time);

//
// This API is only to be used with a 'gos_timedata' AddStatistic. When called
// will return the current time in cycles.
//
int64_t __stdcall GetCycles(void);

//
// Adds a menu item to the ^Break debugger.
//
// This allows games to enable or disable game features using the debugger.
//
// If no items have been added to this menu it will not appear. Otherwise the
// menu item will appear under the last drop down menu with the same title
// as the game. ie: Assassin.
//
// Name			is the name of the menu entry (if " " a blank line is added to the
// menu - blank lines at the bottom of a menu will all not be displayed)
// CheckMark	is a routine that should return true if the menu should have a
// check next to it (active) Activated	is a routine that is callled when the
// user select this menu item
// Greyed		is a routine that if it returns true the item will be greyed and will
// not be able to be selected
//
// Callback is a new paramater that will replace all the other routines
// (CheckMark,Activated and Greyed) if present
//
// It will be called with the name of the menu and a parameter requesting data.
// This 'MenuFunction' can be:
//
// Note, these enums may be expanded in the future, the application should
// return 0 to any other unknown Menu requests.
//
enum _gosMenuDebugger_const : uint32_t
{
	gosMenu_Name, // The name of the menu should be returned ie: return (uint32_t)"Tank Alpha";  (If
	// 0 is returned the default name the menu was created with will be printed)
	gosMenu_CheckMark, // True or False should be returned if the menu has a check mark next to it
	gosMenu_Greyed, // True or False should be returned if the menu is greyed out
	gosMenu_Activated, // This will be called when the menu is activated. If it returns true, the
	// menu will stay open.
};
//
//
//
void __stdcall AddDebuggerMenuItem(PSTR Name, BOOLEAN(__stdcall* CheckMark)(),
	void(__stdcall* Activated)(), BOOLEAN(__stdcall* Greyed)() = 0,
	uint32_t(__stdcall* Callback)(PSTR Name, uint32_t MenuFunction) = 0);

//
// Deletes a debugger menu item
//
void __stdcall DelDebuggerMenuItem(PSTR Name);

//
// This API can be used to query the status of the CheckMark, if the menu is
// Greyed or be used to trigger the menu as if the user had selected it.
//  Use the enum above to return gosMenu_CheckMark, gosMenu_Greyed or pass
//  gosMenu_Activated and the menu will be activated. This can be used to
//  trigger GameOS menu functions for example
//  CallDebuggerMenuItem("Debugger\\Frame graphs\\Background behind
//  graphs",gosMenu_Activated) will put a background behind the framegraphs if
//  called with gosMenu_Name as the function it will return true or false if the
//  menu exists. Other functions will be ignored if the menu does not exist.
//  Note Debugger\\, Rasterizer\\ and Libraries\\ must appear at the start of
//  the name, but for application menu items, no app name is needed.
//
uint32_t __stdcall CallDebuggerMenuItem(PSTR Name, uint32_t MenuFunction);

//
// This function enables or disables the math exception handler
//
// Default precision mode is 53  bits.
// Single precision mode is 24 bits.
//
// When exceptions are enabled, INVALID and ZERODIVIDE cause exceptions
//
// You can get the current states (for pushing and poping for example) with
// GetMathExceptions.
//
void __stdcall gos_MathExceptions(BOOLEAN enableexceptions, BOOLEAN singleprecision);
void __stdcall gos_GetMathExceptions(BOOLEAN* Exceptions, BOOLEAN* singleprecision);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// *************************   Machine Information API  ***********************
// //
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

enum gosSetting : uint32_t
{
	gos_Set_MouseCursor, // 0=Disable cursor (default), 1=Enable cursor (Hardware if possible)
	gos_Set_SkipRender, // When this is set to 1 there will be no rendering for the current frame.
	// Can be used to skip frames when behind.
	gos_Set_NextGameLogicTime, // value is the number of Milliseconds between the last GameLogic and
	// the next. GameOS will 'spin' until this time is met.
	gos_Set_TriggerBrowser, // value is a (PSTR) pointing to an address ie: "http:\\microsoft.com"
	gos_Set_LoseFocusBehavior, // 0=Slow to 10Hz (default), 1=Pause game, 2=Run at full speed, 3=run
	// at full speed, max 60 Hz
	gos_Set_IgnoreMaxUV, // 1=Disable the maximum U,V message _ASSERT. This is for apps in
	// development that have u,v's larger than the card can handle
	gos_Set_IgnoreVSYNC, // When 1 and in full sreen mode, GameOS will use BLT not FLIP. This means
	// the game is not limited by the refresh rate.
	gos_Set_CaptureScreen, // 1=Grab BMP screen shot, 0=Grab JPG screen shot
	gos_Set_MinMaxApp, // Force application to minimize (value==0) or maximize (value!=0)
	gos_Set_NumLockMode, // 0=force off, 1=force on, 2= don't care (default)
	gos_Set_CapsLockMode, // 0=force off, 1=force on, 2= don't care (default)
	gos_Set_ScrollLockMode // 0=force off, 1=force on, 2= don't care (default)
};

//
//
//
uint32_t __stdcall gos_EnableSetting(gosSetting Setting, uint32_t value);

enum MachineInfo : uint32_t
{
	gos_Info_HasAGPAvailable, // (uint32_t)  FALSE when no AGP memory available
	// (assume a low end card)
	gos_Info_HasGuardBandClipping, // (uint32_t)  TRUE when card be passed
	// vertices from at least -2047,-2047 to
	// 2047,2047 without errors
	gos_Info_TextureOffset, // (uint32_t)  TRUE when a -0.625 texel offset is
	// being applied to u,v coords to make text readable
	// - Only the Riva128 requires this. (Normally
	// 0.4375 is subtracted from the PIXEL x,y coords)
	gos_Info_TextureSize, // (uint32_t)  Returns currently selected texture
	// width/height in pixels (or 1 for no texture)

	gos_Info_GetProcessorInfo, // (PSTR)  Returns a user readable string that
	// shows the processor (ie: Intel Pentium III)
	gos_Info_GetNumberProcessors, // (uint32_t)	Returns the number of
	// processors in the system
	gos_Info_GetProcessorSpeed, // (uint32_t)	Returns the processor speed in
	// MHz, ie: 400  (This is approximate, never
	// check for ==400 or >200 - or anything
	// similar. use >180 for 200Mhz or above for
	// example)

	gos_Info_GetOSInfo, // (PSTR)	Returns a user readable string that shows the
	// operating system (ie: Windows 98 GOLD)
	gos_Info_GetMemorySize, // (uint32_t)  Returns the amount of memory the
	// machine has in bytes
	gos_Info_GetVirtualMemorySize, // (uint32_t)  Returns the amount of virutal
	// memory the machine has in bytes
	gos_Info_NumberDevices, // (uint32_t)  Number of devices GameOS can render
	// to (always at least 1)
	gos_Info_GetDeviceName, // (PSTR)  Returns the name of the device specified
	// in Param1 (Riva TNT, Voodoo 1 etc...)
	gos_Info_GetDeviceVendorID, // (uint32_t)  Returns the vendor of the device
	// specified in Param1 (0x121a=3Dfx for example
	// - See Videocard.cpp for complete list)
	gos_Info_GetDeviceDeviceID, // (uint32_t)  Returns the DeviceID of the
	// device specified in Param1 (2=Voodoo2 for
	// example - See Videocard.cpp for complete
	// list)
	gos_Info_GetDeviceVideoMemory, // (uint32_t)	Returns the amount of video
	// memory available (for front/back/z buffers
	// etc..) for device specified in Param1
	gos_Info_GetDeviceLocalMemory, // (uint32_t)	Returns the amount of local
	// video memory available (may be the same as
	// above) for device specified in Param1
	gos_Info_GetDeviceAGPMemory, // (uint32_t)	Returns the amount of AGP
	// video memory available (may be 0 on non-agp
	// cards) for device specified in Param1
	gos_Info_GetDeviceDriverVersionHi, // (uint32_t)  Returns the Hi uint32_t
	// version number of the Device specified
	// in param 0
	gos_Info_GetDeviceDriverVersionLo, // (uint32_t)  Returns the Lo uint32_t
	// version number of the Device specified
	// in param 0
	gos_Info_NumberValidModes, // (uint32_t)	Returns the number of valid modes
	// for device Param1, bitdepth Param2 (16 or 32
	// only)
	gos_Info_GetValidMode, // (uint32_t)	Returns a uint32_t containing
	// HIWORD=width, LOWORD=height of device 'Param1' for
	// bitdepth 'Param2', mode number Param3
	gos_Info_ValidMode, // (uint32_t)  Param1=Device, Param2=width,
	// Param3=height. Returns true if card supports mode
	// (ie: 1024*768)
	gos_Info_CanDeviceHardwareGamma, // (uint32_t)	Returns true if the gamma
	// of the display will be adjusted using
	// HARDWARE or SOFTWARE (software is much
	// slower)
	gos_Info_CanDeviceAntiAlias, // (uint32_t)  Returns true if the device
	// specified in Param1 can render antialiased
	gos_Info_CanDeviceRender32Bit, // (uint32_t)  Returns true if the device
	// specified in Param1 can render in 32 bit
	// mode
	gos_Info_CanDeviceRenderStencil, // (uint32_t)  Returns 1 if the device
	// specified in Param1 can render with a
	// stencil buffer, 2 if the device requires
	// 32 bit mode before stencils work

	gos_Info_CanMultitextureLightMap, // (uint32_t)  TRUE when single pass light
	// mapping is available
	gos_Info_CanMultitextureSpecularMap, // (uint32_t)  TRUE when single pass
	// specular mapping is available
	gos_Info_CanMultitextureDetail, // (uint32_t)  TRUE when single pass detail
	// texturing is available

	gos_Info_CanAlphaOneOne, // (uint32_t)	TRUE when the card can support
	// Alpha ONE:ONE blending
	gos_Info_CanAlphaTest, // (uint32_t)	TRUE when the card can support Alpha
	// testing (GameOS will enable AlphaInvAlpha
	// blending)
	gos_Info_CanClampTextures, // (uint32_t)	TRUE when texture address mode
	// CLAMP is available (GameOS will default to
	// wrap)
	gos_Info_CanModulateAlpha, // (uint32_t)	TRUE when ModulateAlpha is a
	// supported mode (GameOS will default to
	// Modulate)
	gos_Info_CanSpecular, // (uint32_t)	TRUE when specular is supported
	// (GameOS will ignore all specular otherwise)

	gos_Info_GetDoubleClickTime, // (uint32_t)  Number of milliseconds between
	// mouse clicks

	gos_Info_GetMaximumUVSize, // (uint32_t)  Returns the maximum U,V size for
	// the texture size passed as Param1 (if 0 is
	// returned, there is no maximum)
	gos_Info_IsMouseInSideWindow, // (uint32_t)  Returns 0 when the windows
	// mouse is OVER the game window, or in full
	// screen mode. Otherwise returns the absolute
	// number of pixels away from the window the
	// mouse is.

	gos_Info_GetPixelColor, // (uint32_t)	Returns a 32 bit ARGB value for the
	// pixel at screen location Param1=x, Param2=y.
	// Param3=0 for back buffer, 1 for CopyBuffer (Dirty
	// Rectangle mode). This API only valid during
	// GameLogic or before SetupViewPort.

	gos_Info_CanHardwareTandL, // (uint32_t)	TRUE when current mode is using
	// hardware T&L
	gos_Info_GetMaximumActiveLights, // (uint32_t)	Maximum number of active
	// lights allowed in current mode (Can be
	// -1 for infinite)
	gos_Info_GetVertexBlendMatrices, // (uint32_t)  Returns 0 or 1 currently.
	// The is the maximum number of vertex
	// blend weights the hardware can blend.

	gos_Info_GetApplicationStatus, // (uint32_t)  Bit 0 is true when the app is
	// active, Bit 1 is true when the app has
	// keyboard focus, Bit 2 is true when no
	// rendering can occur (ie: Control Alt
	// Delete Pressed)
	gos_Info_GetDesktopResolution, // (uint32_t)	Get the resolution of the
	// desktop (high word is vertical, low word
	// is horizontal)
	gos_Info_GetDesktopBitDepth, // (uint32_t)	Get the bitdepth of the desktop

	gos_Info_GetIMEStatus, // (uint32_t)  Bit 0 is true when the IME is open.
	gos_Info_GetKeyboardLayout, // (uint32_t)	Returns the current users keyboard
	// layout from windows

	gos_Info_GetMaximumTexturewidth, // (uint32_t)  Maximum width in texels of a
	// texture or mipmap level.
	gos_Info_GetMaximumTextureheight, // (uint32_t)  Maximum height in texels of
	// a texture or mipmap level.

	gos_Info_CanBumpEnvMap, // (uint32_t)	TRUE when current mode supports bump
	// environment maps
	gos_Info_CanBumpDotMap, // (uint32_t)  Returns 1 if the current mode
	// supports normal maps (dotproduct3)

	gos_Info_HardwareGammaControl, // (uint32_t)	TRUE if full screen and
	// hardware gamma control possible. Otherwise
	// slower software gamma emulation is used
	gos_Info_HardwareBrightnessControl, // (uint32_t)	TRUE if full screen and
	// hardware brightness control possible.
	// Otherwise invalid
	gos_Info_HardwareContrastControl, // (uint32_t)	TRUE if full screen and
	// hardware brightness control possible.
	// Otherwise invalid

	gos_Info_UsingHardwareRenderer, // (uint32_t)	TRUE when using a hardware
	// renderer, 0=Blade. (Environment.renderer
	// may have not been able to support
	// hardware)

	gos_Info_GetIMECaretStatus, // (uint32_t)  TRUE if an edit box should show
	// its caret. When it's FALSE, an edit box
	// shouldn't display caret.
};

//
// Returns machine information, valid for THE CURRENT FRAME ONLY
//
uint32_t __stdcall gos_GetMachineInformation(
	MachineInfo mi, int32_t Param1 = 0, int32_t Param2 = 0, int32_t Param3 = 0, int32_t Param4 = 0);

//
//
//
// Internal functions and types - Used by GameOS, not to be called or used by
// applications
//
enum GosErrorFlags
{
	gos_ErrorNoContinue 		= 1, // No CONTINUE is allowed from errors
	gos_ErrorNoRegisters 		= 2, // Register are not valid (takes address from where called)
	gos_ErrorAssert 		= 4, // Set to signify _ASSERT ( assumes gos_ErrorNoRegisters )
	gos_ErrorVerify 		= 8, // Set to signify verify ( assumes gos_ErrorNoRegisters )
	gos_ErrorException 		= 16, // Set to signify exception handler
	gos_ErrorDump 		= 32, // ErrorTitle must be valid
	gos_ErrorPop 		= 64, // Pop one more level of stack
	gos_ErrorPop2 		= 128, // Pop two more levels of stack
	gos_ErrorStop 		= 256, // Stop - display message
	gos_ErrorMessage 		= 512, // Display message without a STOP: or Verify:
	gos_ErrorFileLine 		= 1024, // Global variables gosErrorFile and gosErrorLine contain line to display (Pass a COMPLETE path\file name)
	gos_ErrorRetry 		= 2048, // Retry instead of continue button
	gos_ErrorGameInfoFirst 		= 4096, // Show the information from GetGameInformation routine immediately after File/Line information
	gos_ErrorAppendRoutine 		= 8192 // append ' in Routine()+xxx' to the error  (or address if no symbols)
};

#define Stat_Format (0x40000000) // Used for formatting in statistics
#define gos_SpewNoAddCR 1 // Don't add "\n" to spew output

extern PSTR gosErrorFile;
extern uint32_t gosErrorLine;
extern void __cdecl InternalFunctionSpew(PCSTR group, PCSTR message, ...);
extern void __cdecl InternalFunctionSpewV(int32_t flags, PCSTR group, PCSTR message, PSTR arglist);
extern int32_t __cdecl InternalFunctionStop(PCSTR message, ...);
extern int32_t __cdecl InternalFunctionPause(PCSTR message, ...);
extern void __stdcall gosLabRatStart(uint32_t handle);
extern void __stdcall gosLabRatEnd(uint32_t handle);
extern void __stdcall gosLabRatSet(uint32_t handle, uint32_t value);
extern void __stdcall gosLabRatSet(uint32_t handle, float value);
extern "C" int32_t __stdcall ErrorHandler(int32_t flags, PSTR errortext);
extern gosEnvironment Environment;

typedef struct gos_CycleData
{
	uint32_t Count;
	BOOLEAN InsideStartEnd;
	BOOLEAN _padding1[3];
	uint32_t Cycles;
	uint32_t StartProcessorCycles;
	int64_t TotalCounter[2];
	int64_t StartCounter[2];
} gos_CycleData;
#define SPEWALWAYS(x) InternalFunctionSpew x
//#pragma pack(pop)

// LOGGING FOR GosView.exe

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// GosView Logging ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//	To log entry/exit of a function or code block, place the LOG_BLOCK macro
//	at or near the start of the code block. GosView.exe splits names wherever
//	'::' or '\' are encountered and constructs a tree control based on these
//	so careful naming choices can assist during analysis.
//
//	e.g:
//
//	void AI::MechMove()
//	{
//		LOG_BLOCK( "MW4\\AI::MechMove" )
//		x += move.x;
//		y += move.y;
//		z += move.z;
//	}
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////// EVENT LOGGING MACROS ////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if defined(LAB_ONLY) // debug, armor, or profile

#define LOG_REF(refobj, reftype, refname) \
	static GosLogRef refobj(GosLogRef::reftype, refname, __FILE__, __LINE__)
#define LOG_BLOCK(funcname)                     \
	LOG_REF(gosLogFuncRef, logEnter, funcname); \
	GosLogFuncScope gosLogFuncScope(gosLogFuncRef);

#define LOG_START_CHECK GosEventLog::LogStartPoint(void);

#else // retail
#define LOG_BLOCK(funcname)
#define LOG_START_CHECK \
	{                   \
		;               \
	}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////// CLASSES FOR GOS MACRO USE ONLY //////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus

class GosLogRef
{
	uint32_t m_Count;
	/// void			(*m_pFunc)(GosLogRef &);
	/// void			*m_pObj;
	uint32_t m_id;

public:
	enum EventType : uint32_t
	{
		logEnter = 0, // return must be 1 more than enter
		logReturn = 1,
		logCount,
		logLevel,
		logAlloc,
		logFree,
		logNew,
		logDelete,
		log,
	};
	GosLogRef(EventType type, PSTR name, PSTR filename, int32_t lineno);
	operator uint32_t() { return m_id; }
	BOOLEAN ShouldLog()
	{
		m_Count++; // change to return (m_pFunc != nullptr);
		return 1;
	}
};

class GosEventLog
{
	static uint32_t* pLogBase;
	static uint32_t LogOffset; // Offset into log in ULONGS
	static uint32_t LogMod; // Mod in ULONGS
	static uint32_t NullLog[16];
	static BOOLEAN WaitingToStart;
	static BOOLEAN LoggingInProgress;

	// after stopping, the following members point to the frozen log
	static PSTR m_pLogStart1;
	static uint32_t m_nLogSize1; // size of first part in bytes
	static PSTR m_pLogStart2;
	static uint32_t m_nLogSize2; // size of second part in bytes

public:
	static void Log(uint32_t id)
	{
		uint32_t time;
		LogOffset &= LogMod;
		pLogBase[LogOffset++] = id;
		// _asm rdtsc
		// _asm mov time,eax
		time = (uint32_t)__rdtsc();
		pLogBase[LogOffset++] = time;
	}
	static void LogStart(void);
	static void LogStartPoint(void);
	static void LogStop(void);
	static void PushStop(void);
	static void PopStop(void);
	static PSTR LogAddr(int32_t part) { return part ? m_pLogStart2 : m_pLogStart1; }
	static uint32_t LogBytes(int32_t part) { return part ? m_nLogSize2 : m_nLogSize1; }
	static BOOLEAN Logging() { return LoggingInProgress; }
	static void Cleanup(void);
};

class GosLogFuncScope
{
private:
	uint32_t m_Caller;
	static uint32_t Current;

public:
	GosLogFuncScope(uint32_t newId)
	{
		m_Caller = Current;
		GosEventLog::Log(newId);
		Current = newId + 0x00100000; // so future logs will be the *return* to
			// this function scope
	}
	~GosLogFuncScope()
	{
		if (m_Caller)
		{
			Current = m_Caller;
			GosEventLog::Log(Current); // i.e. log the return to the function
		}
	}
};

#endif // __cplusplus
#pragma warning(pop)
#endif
