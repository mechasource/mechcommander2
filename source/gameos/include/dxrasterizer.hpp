//===========================================================================//
// File:	 DXRasterizer.hpp												 //
// Contents: DirectDraw Manager routines									 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

// videocard.cpp functions
void __stdcall FindVideoCards(void);
void __stdcall CreateCopyBuffers(void);
void __stdcall RestoreAreas(void);
void __stdcall DestroyDirtyRectangles(void);
void __stdcall InvalidateVertexBuffers(void);
void __stdcall ReCreateVertexBuffers(void);

// IME displays directly to backbuffer, that's why it's in DXRasterizer.hpp
void __stdcall RenderIMEToBackBuffer(int POS_X, int POS_Y, float FONTHEIGHT);

// Variables for gamma correction
extern UCHAR GammaTable[256];
extern float GammaSetting;
extern UCHAR UseGammaCorrection;
extern ULONG GlobalWidth;
extern ULONG GlobalHeight;
extern UCHAR GlobalZBuffer;
extern UCHAR GlobalFullScreen;
extern ULONG ValidTextures;
extern UCHAR ModeChanged;
extern UCHAR DebuggerUsed;
extern DDSURFACEDESC2 BackBufferddsd;
extern ULONG AllowBrightness;
extern ULONG AllowContrast;

HRESULT __stdcall CheckEnumProc(DDPIXELFORMAT* ddpfPixelFormat, PVOID lpContext);
extern void __stdcall CopyBackBuffer(LPDIRECTDRAWSURFACE7 Dest, LPDIRECTDRAWSURFACE7 Source);

//
// Compatibility flags for different video cards
//
typedef enum __compatibilityconst {
	SceneRenderer=1,				// Scene based renderer (PowerVR, Kyro etc...)
	NoAlphaTest=1<<1,				// Disable alpha test (conflicts with other modes)
	NoTriLinear=1<<2,				// Do not attempt trilinear filtering
	NoTripleBuffer=1<<3,			// Do not attempt triple buffering
	DefaultBltNotFlip=1<<4,			// In full screen mode, use BLT
	NoFog=1<<5,						// Do not attempt fog
	TextureOffset=1<<6,				// Riva 128 needs a texture offset for fonts
	BrokenAlphaTest=1<<7,			// ATI Rage cards cannot do Alpha test without ALPHABLEND being enabled
	RenderEqualZ=1<<8,				// Render bit depth must equal Z bit depth (nVidia cards)
	NoParallelism=1<<9,				// Lock the back buffer after rendering (Voodoo3 - queueing up too many blits)
	BadMouse=1<<10,					// Card can never used GDI mouse
	LowEndCard=1<<11,				// This is a 'low end' card - the application can request this is not supported in hardware (Environment.DisableLowEndCard)
	NoMipMap=1<<12,					// Disable mipmaps
	Bad4444=1<<13,					// Disable 4444 textures  (Chromatic Mpact)
	BadAGP=1<<14,					// Disable AGP textures   (Chromatic Mpact)
	NoDisablePerspective=1<<15,		// Disable perspective correct disable.
	LimitTextureSize=1<<16,			// Limit texture size to 256*256 to correct bad caps
	ForceSquareTextures=1<<17,		// Do not allow non-square textures
	NoStretchTexture=1<<18,			// Force system memory stretch blit to square up textures
	AllowLowMemoryCard=1<<19,		// Allow card to bypass low texture memory test (Voodoo2)
	NoMultitexture=1<<20,			// Disable multitexture blending modes
	Disable1024x768=1<<21,			// Disable 1024*768 mode
	DisableTextureDeletes=1<<22,	// Try not to delete textures
	SlowDisplayModeChange=1<<23,    // Prevent video drivers that take a while before sending the WM_DISPLAYCHANGE msg from getting stuck in an endless loop of display mode changes
	DisableColorControl=1<<24,		// Disable gamma and color controls
	Limit500VidTextures=1<<25,		// Limit video memory textures to 500
	Bad8888=1<<26,					// Disable 32 bit textures
	Disable32BitRender=1<<27,		// Disable 32 bit rendering
};

//
// This is the current cards compatiblity flags
//
extern ULONG Compatibility3D;

//
// Used to store all known video cards
//
typedef struct CardInfo {
	ULONG VendorID;						// Vendor ID, ie: S3=0x5333
	ULONG DeviceID;						// Device ID, ie: ATI Rage=0x4750
	ULONG DriverH;						// High and Low dword of driver version number (Windows 9x)
	ULONG DriverL;
	ULONG NTDriverH;					// High and Low dword of driver version number (Windows 2000)
	ULONG NTDriverL;
	PSTR VendorName;					// Friendly name for Vendor
	PSTR CardName;						// Friendly name for card type
	ULONG CompatibilityFlags;			// Flags that may alter how GameOS behaves
} CardInfo;

extern CardInfo*	KnownCards;

//
// Used to store information about video cards in machine
//
typedef struct DeviceInfo {
	GUID				DeviceGUID;
	DDDEVICEIDENTIFIER2	DDid;					// Use DDid.szDescription for the readable name
	ULONG				Empty;					// Bug in GetDeviceIdentifier - it overwrites the next 4 bytes!
	DDCAPS				DDCaps;
	D3DDEVICEDESC7		D3DCaps;
	char				FourCC[16*4];			// First 16 FourCC codes
	DDPIXELFORMAT		TextureFormats[16];		// First 16 texture formats (ends in 0)
	USHORT				Modes16[16*2];			// First 16, 16 bit screen modes above 640*480
	USHORT				Refresh16[16];			// Refresh rates for above modes
	USHORT				Modes32[16*2];			// First 16, 32 bit screen modes above 640*480
	USHORT				Refresh32[16];			// Refresh rates for above modes
	DDPIXELFORMAT		ZFormats[16];			// First 16 Z Buffer formats
	ULONG				TotalVid;
	ULONG				TotalLocalTex;
	ULONG				TotalTex;
	ULONG				MultitextureLightMap;			// Lightmap support
	ULONG				MultitextureSpecularMap;		// Specular support
	ULONG				MultitextureDetail;				// Detail support
	ULONG				MultitextureLightMapFilter;		// Lightmap support (1=Disable trilinear)
	ULONG				MultitextureSpecularMapFilter;	// Specular support (1=Disable trilinear)
	ULONG				MultitextureDetailFilter;		// Detail support (1=Disable trilinear)
	ULONG				CurrentCard;			// Which entry in the KnownCard array. (-1=Unknown)
	UCHAR				Failed60Hz;				// Set to only try full screen 60Hz once
	UCHAR				LinearMemory;			// Set if linear memory, not rectangular
	UCHAR				_padding1[2];
	int					MaxRefreshRate;			// Maximum refresh rate
	PSTR				CurrentVendor;			// If vendor known, point to name, else 0
} DeviceInfo;

extern DeviceInfo	DeviceArray[8];
extern int			HardwareRenderer;			// Set when using a hardware renderer
extern char DisplayInfoText[128];

extern ULONG TripleBuffer;						// Copied from Environment for current card (so can be disabled on 3Dfx)
extern ULONG StencilActive;
extern DDSURFACEDESC2 BBddsd;
extern DDSURFACEDESC2 ZBddsd;

extern LPDIRECTDRAW7			DDobject;
extern LPDIRECTDRAW7			CurrentDDobject;
extern LPDIRECTDRAWSURFACE7		FrontBufferSurface;
extern LPDIRECTDRAWSURFACE7		BackBufferSurface;
extern LPDIRECTDRAWSURFACE7		ZBufferSurface;
extern LPDIRECTDRAWSURFACE7		g_RefBackBuffer;
extern LPDIRECTDRAWSURFACE7		g_CopyBackBuffer;
extern LPDIRECTDRAWCLIPPER		g_lpDDClipper;
extern LPDIRECTDRAWSURFACE7		CopyTarget;
extern LPDIRECTDRAWSURFACE7		CopyZBuffer;
extern LPDIRECTDRAWGAMMACONTROL	GammaControlInterface;

extern LPDIRECT3D7				d3d7;
extern LPDIRECT3DDEVICE7		d3dDevice7;				// Current D3D Device
extern LPDIRECT3DDEVICE7		Refd3dDevice7;			// Reference D3D Device
extern LPDIRECT3DDEVICE7		Maind3dDevice7;			// Selected D3D Device
extern LPDIRECT3DDEVICE7		RenderDevice;
extern UCHAR					InsideBeginScene;
extern UCHAR					NeedToInitRenderStates;
extern int						HardwareRenderer;
extern ULONG					BGColor;
extern D3DDEVICEDESC7			CapsDirect3D;

extern D3DVIEWPORT7				viewData;
extern float					gosViewportMulX,gosViewportAddX,gosViewportMulY,gosViewportAddY;
extern ULONG					NumDevices;
extern ULONG					NumHWDevices;
extern ULONG					NumMonitors;
extern ULONG gosColorTable[];
extern ULONG DepthResults[8];
extern ULONG DepthPixels;

extern void __stdcall InitRenderer(void);
extern PSTR __stdcall GetDirectXVersion(void);

//
// MipMapping flags
//
extern ULONG HasBiLinear;
extern ULONG HasTriLinear;
extern int HasTLHAL;
//
// Card specific changes triggered by these values
//
extern ULONG HasVertexBlending;
extern ULONG HasClamp;
extern ULONG HasFog;
extern ULONG HasAlphaTest;
extern ULONG HasAlphaModes;
extern ULONG HasMultitextureLightmap;
extern ULONG HasMultitextureSpecularmap;
extern ULONG HasMultitextureDetailTexture;
extern ULONG HasMultitextureLightmapFilter;
extern ULONG HasMultitextureSpecularmapFilter;
extern ULONG HasMultitextureDetailTextureFilter;

void __stdcall DisplayBackBuffer(void);
void __stdcall End3DScene(void);

void __stdcall RenderWithReferenceRasterizer(ULONG Type);
void __stdcall DirectDrawInstall(void);
void __stdcall ShowFrameGraphs(void);
void __stdcall DirectDrawUninstall(void);
void __stdcall DirectDrawCreateDDObject(void);
BOOL __stdcall DirectDrawEnumerateCallback(GUID* lpGUID, PSTR, PSTR, PVOID lpContext);
HRESULT __stdcall DirectDrawEnumDisplayModesCallback(LPDDSURFACEDESC2 pddsd, PVOID lpContext);
void __stdcall DirectDrawCreateAllBuffers(void);
void __stdcall EnterWindowMode(void);
void __stdcall EnterFullScreenMode(void);
void __stdcall RenderIMEtoScreen(void);
