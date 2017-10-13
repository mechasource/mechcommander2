//===========================================================================//
// File:	 DXRasterizer.hpp												 //
// Contents: DirectDraw Manager routines									 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _DXRASTERIZER_HPP_
#define _DXRASTERIZER_HPP_

// videocard.cpp functions
void __stdcall FindVideoCards(void);
void __stdcall CreateCopyBuffers(void);
void __stdcall RestoreAreas(void);
void __stdcall DestroyDirtyRectangles(void);
void __stdcall InvalidateVertexBuffers(void);
void __stdcall ReCreateVertexBuffers(void);

// IME displays directly to backbuffer, that's why it's in DXRasterizer.hpp
void __stdcall RenderIMEToBackBuffer(
	int32_t POS_X, int32_t POS_Y, float FONTHEIGHT);

// Variables for gamma correction
extern uint8_t GammaTable[256];
extern float GammaSetting;
extern uint8_t UseGammaCorrection;
extern uint32_t GlobalWidth;
extern uint32_t GlobalHeight;
extern uint8_t GlobalZBuffer;
extern uint8_t GlobalFullScreen;
extern uint32_t ValidTextures;
extern uint8_t ModeChanged;
extern uint8_t DebuggerUsed;
extern DDSURFACEDESC2 BackBufferddsd;
extern uint32_t AllowBrightness;
extern uint32_t AllowContrast;

HRESULT __stdcall CheckEnumProc(
	DDPIXELFORMAT* ddpfPixelFormat, PVOID lpContext);
extern void __stdcall CopyBackBuffer(
	LPDIRECTDRAWSURFACE7 Dest, LPDIRECTDRAWSURFACE7 Source);

//
// Compatibility flags for different video cards
//
typedef enum __compatibilityconst
{
	SceneRenderer  = 1,		 // Scene based renderer (PowerVR, Kyro etc...)
	NoAlphaTest	= 1 << 1, // Disable alpha test (conflicts with other modes)
	NoTriLinear	= 1 << 2, // Do not attempt trilinear filtering
	NoTripleBuffer = 1 << 3, // Do not attempt triple buffering
	DefaultBltNotFlip = 1 << 4, // In full screen mode, use BLT
	NoFog			  = 1 << 5, // Do not attempt fog
	TextureOffset	 = 1 << 6, // Riva 128 needs a texture offset for fonts
	BrokenAlphaTest   = 1 << 7, // ATI Rage cards cannot do Alpha test without
								// ALPHABLEND being enabled
	RenderEqualZ =
		1 << 8, // Render bit depth must equal Z bit depth (nVidia cards)
	NoParallelism = 1 << 9, // Lock the back buffer after rendering (Voodoo3 -
							// queueing up too many blits)
	BadMouse   = 1 << 10,   // Card can never used GDI mouse
	LowEndCard = 1 << 11,   // This is a 'low end' card - the application can
							// request this is not supported in hardware
							// (Environment.DisableLowEndCard)
	NoMipMap			 = 1 << 12, // Disable mipmaps
	Bad4444				 = 1 << 13, // Disable 4444 textures  (Chromatic Mpact)
	BadAGP				 = 1 << 14, // Disable AGP textures   (Chromatic Mpact)
	NoDisablePerspective = 1 << 15, // Disable perspective correct disable.
	LimitTextureSize =
		1 << 16, // Limit texture size to 256*256 to correct bad caps
	ForceSquareTextures = 1 << 17, // Do not allow non-square textures
	NoStretchTexture =
		1 << 18, // Force system memory stretch blit to square up textures
	AllowLowMemoryCard =
		1 << 19, // Allow card to bypass low texture memory test (Voodoo2)
	NoMultitexture		  = 1 << 20, // Disable multitexture blending modes
	Disable1024x768		  = 1 << 21, // Disable 1024*768 mode
	DisableTextureDeletes = 1 << 22, // Try not to delete textures
	SlowDisplayModeChange = 1 << 23, // Prevent video drivers that take a while
									 // before sending the WM_DISPLAYCHANGE msg
									 // from getting stuck in an endless loop of
									 // display mode changes
	DisableColorControl = 1 << 24,   // Disable gamma and color controls
	Limit500VidTextures = 1 << 25,   // Limit video memory textures to 500
	Bad8888				= 1 << 26,   // Disable 32 bit textures
	Disable32BitRender  = 1 << 27,   // Disable 32 bit rendering
};

//
// This is the current cards compatiblity flags
//
extern uint32_t Compatibility3D;

//
// Used to store all known video cards
//
typedef struct CardInfo
{
	uint32_t VendorID; // Vendor ID, ie: S3=0x5333
	uint32_t DeviceID; // Device ID, ie: ATI Rage=0x4750
	uint32_t
		DriverH; // High and Low dword of driver version number (Windows 9x)
	uint32_t DriverL;
	uint32_t
		NTDriverH; // High and Low dword of driver version number (Windows 2000)
	uint32_t NTDriverL;
	PSTR VendorName;			 // Friendly name for Vendor
	PSTR CardName;				 // Friendly name for card type
	uint32_t CompatibilityFlags; // Flags that may alter how GameOS behaves
} CardInfo;

extern CardInfo* KnownCards;

//
// Used to store information about video cards in machine
//
typedef struct DeviceInfo
{
	GUID DeviceGUID;
	DDDEVICEIDENTIFIER2 DDid; // Use DDid.szDescription for the readable name
	uint32_t
		Empty; // Bug in GetDeviceIdentifier - it overwrites the next 4 bytes!
	DDCAPS DDCaps;
	D3DDEVICEDESC7 D3DCaps;
	char FourCC[16 * 4];			  // First 16 FourCC codes
	DDPIXELFORMAT TextureFormats[16]; // First 16 texture formats (ends in 0)
	uint16_t Modes16[16 * 2];   // First 16, 16 bit screen modes above 640*480
	uint16_t Refresh16[16];		// Refresh rates for above modes
	uint16_t Modes32[16 * 2];   // First 16, 32 bit screen modes above 640*480
	uint16_t Refresh32[16];		// Refresh rates for above modes
	DDPIXELFORMAT ZFormats[16]; // First 16 Z Buffer formats
	uint32_t TotalVid;
	uint32_t TotalLocalTex;
	uint32_t TotalTex;
	uint32_t MultitextureLightMap;	// Lightmap support
	uint32_t MultitextureSpecularMap; // Specular support
	uint32_t MultitextureDetail;	  // Detail support
	uint32_t
		MultitextureLightMapFilter; // Lightmap support (1=Disable trilinear)
	uint32_t
		MultitextureSpecularMapFilter; // Specular support (1=Disable trilinear)
	uint32_t MultitextureDetailFilter; // Detail support (1=Disable trilinear)
	uint32_t CurrentCard; // Which entry in the KnownCard array. (-1=Unknown)
	uint8_t Failed60Hz;   // Set to only try full screen 60Hz once
	uint8_t LinearMemory; // Set if linear memory, not rectangular
	uint8_t _padding1[2];
	int32_t MaxRefreshRate; // Maximum refresh rate
	PSTR CurrentVendor;		// If vendor known, point to name, else 0
} DeviceInfo;

extern DeviceInfo DeviceArray[8];
extern int32_t HardwareRenderer; // Set when using a hardware renderer
extern char DisplayInfoText[128];

extern uint32_t TripleBuffer; // Copied from Environment for current card (so
							  // can be disabled on 3Dfx)
extern uint32_t StencilActive;
extern DDSURFACEDESC2 BBddsd;
extern DDSURFACEDESC2 ZBddsd;

extern LPDIRECTDRAW7 DDobject;
extern LPDIRECTDRAW7 CurrentDDobject;
extern LPDIRECTDRAWSURFACE7 FrontBufferSurface;
extern LPDIRECTDRAWSURFACE7 BackBufferSurface;
extern LPDIRECTDRAWSURFACE7 ZBufferSurface;
extern LPDIRECTDRAWSURFACE7 g_RefBackBuffer;
extern LPDIRECTDRAWSURFACE7 g_CopyBackBuffer;
extern LPDIRECTDRAWCLIPPER g_lpDDClipper;
extern LPDIRECTDRAWSURFACE7 CopyTarget;
extern LPDIRECTDRAWSURFACE7 CopyZBuffer;
extern LPDIRECTDRAWGAMMACONTROL GammaControlInterface;

extern LPDIRECT3D7 d3d7;
extern LPDIRECT3DDEVICE7 d3dDevice7;	 // Current D3D Device
extern LPDIRECT3DDEVICE7 Refd3dDevice7;  // Reference D3D Device
extern LPDIRECT3DDEVICE7 Maind3dDevice7; // Selected D3D Device
extern LPDIRECT3DDEVICE7 RenderDevice;
extern uint8_t InsideBeginScene;
extern uint8_t NeedToInitRenderStates;
extern int32_t HardwareRenderer;
extern uint32_t BGColor;
extern D3DDEVICEDESC7 CapsDirect3D;

extern D3DVIEWPORT7 viewData;
extern float gosViewportMulX, gosViewportAddX, gosViewportMulY, gosViewportAddY;
extern uint32_t NumDevices;
extern uint32_t NumHWDevices;
extern uint32_t NumMonitors;
extern uint32_t gosColorTable[];
extern uint32_t DepthResults[8];
extern uint32_t DepthPixels;

extern void __stdcall InitRenderer(void);
extern PSTR __stdcall GetDirectXVersion(void);

//
// MipMapping flags
//
extern uint32_t HasBiLinear;
extern uint32_t HasTriLinear;
extern int32_t HasTLHAL;
//
// Card specific changes triggered by these values
//
extern uint32_t HasVertexBlending;
extern uint32_t HasClamp;
extern uint32_t HasFog;
extern uint32_t HasAlphaTest;
extern uint32_t HasAlphaModes;
extern uint32_t HasMultitextureLightmap;
extern uint32_t HasMultitextureSpecularmap;
extern uint32_t HasMultitextureDetailTexture;
extern uint32_t HasMultitextureLightmapFilter;
extern uint32_t HasMultitextureSpecularmapFilter;
extern uint32_t HasMultitextureDetailTextureFilter;

void __stdcall DisplayBackBuffer(void);
void __stdcall End3DScene(void);

void __stdcall RenderWithReferenceRasterizer(uint32_t Type);
void __stdcall DirectDrawInstall(void);
void __stdcall ShowFrameGraphs(void);
void __stdcall DirectDrawUninstall(void);
void __stdcall DirectDrawCreateDDObject(void);
BOOL __stdcall DirectDrawEnumerateCallback(
	GUID* lpGUID, PSTR, PSTR, PVOID lpContext);
HRESULT __stdcall DirectDrawEnumDisplayModesCallback(
	LPDDSURFACEDESC2 pddsd, PVOID lpContext);
void __stdcall DirectDrawCreateAllBuffers(void);
void __stdcall EnterWindowMode(void);
void __stdcall EnterFullScreenMode(void);
void __stdcall RenderIMEtoScreen(void);

#endif
