//===========================================================================//
// File:	 Texture Manager.hpp											 //
// Contents: Texture Mananger												 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

class CTexInfo;	// Per-texture class defined below

//
// List of video memory textures (so we don't delete on some cards)
//
typedef struct _VidMemHeap
{
	struct _VidMemHeap*		pNext;
	DDSURFACEDESC2			ddsd;
	LPDIRECTDRAWSURFACE7	Surface;

} VidMemHeap;

extern VidMemHeap* pFreeVidMemTextures;



#if 1
	#define TEXHANDLE_TO_PTR(handle) ((CTexInfo *)(handle))
	#define TEXPTR_TO_HANDLE(ptr) ((ULONG)(ptr))
#else
	#define TEXHANDLE_TO_PTR(handle) (CTexInfo::TextureHandleToPtr(handle))
	#define TEXPTR_TO_HANDLE(ptr) (ptr->TexturePointerToHandle())
#endif

extern int						HasMipMap;					// Can do mipmapping
extern D3DDEVICEDESC7			CapsDirect3D;				// Current 3D device caps

// Pixel conversion helper functions
ULONG GetPixelColor( ULONG In );
ULONG GetBackBufferColor( USHORT In );
void UpdateBackBufferFormat();

void __stdcall DecodeBMPDimensions( PCSTR FileName, PUCHAR Data, ULONG DataSize, PULONG pTextureWidth, PULONG pTextureHeight );
void __stdcall DecodeJPGDimensions( PCSTR FileName, PUCHAR Data, ULONG DataSize, PULONG pTextureWidth, PULONG pTextureHeight );
#ifdef USEPNG
PVOID __stdcall DecodePNG( PCSTR FileName, PUCHAR Data, ULONG DataSize, PULONG TextureWidth, PULONG TextureHeight, bool TextureLoad, PVOID pDestSurf=NULL );
void __stdcall DecodePNGDimensions( PCSTR FileName, PUCHAR Data, ULONG DataSize, PULONG pTextureWidth, PULONG pTextureHeight );
#endif
void DecodeTGADimensions( PCSTR FileName, PUCHAR Data, ULONG DataSize, PULONG pTextureWidth, PULONG pTextureHeight );

PVOID DecodeBMP( PCSTR FileName, PUCHAR Data, ULONG DataSize, PULONG TextureWidth, PULONG TextureHeight, bool TextureLoad, PVOID pDestSurf=NULL );
PVOID DecodeJPG( PCSTR FileName, PUCHAR Data, ULONG DataSize, PULONG TextureWidth, PULONG TextureHeight, bool TextureLoad, PVOID pDestSurf=NULL );
PVOID DecodeTGA( PCSTR FileName, PUCHAR Data, ULONG DataSize, PULONG TextureWidth, PULONG TextureHeight, bool TextureLoad, PVOID pDestSurf=NULL );

int MipLevelsRequired( USHORT Width, USHORT Height );

// GOS Internal textures
extern ULONG FontTexture;						// The font and mouse pointer
extern ULONG ChessTexture;						// Checkerboard pattern
extern ULONG MipColorTexture;					// The mipmap color texture

// Valid surface descriptions
extern DDSURFACEDESC2 TextureDesc[5][2][4];		// arranged by:  [Format][want32bpp][compression]
extern int TextureDescScore[5][2][4];			// validity of current entry in TextrueDesc

// Maximum number of textures that are supported (can be altered independently from display cards)
const ULONG					MaximumTextures=3000;
extern ULONG				MaximumVidTextures;	// limit the number simultaneously loaded

const ULONG					MaximumTextureLogs=32;
// Texture flags
const USHORT					tFlag_InVidMem=1;	// Currently loaded in Video Memory or AGP
const USHORT					tFlag_InAGP=2;		// Allocation is in AGP ( tFlag_InVidMem set separately )
const USHORT					tFlag_Valid=4;		// This CTexInfo has been allocated
const USHORT					tFlag_Alpha=8;		// Texture format has ALPHA (either keyed or full channel)
const USHORT					tFlag_InSysMem=16;	// Texture has been converted into system memory format
const USHORT					tFlag_LockRW=32;	// Set when a texture is locked for writing
const USHORT					tFlag_Preload=64;	// Preload this texture at the end of the current frame
const USHORT					tFlag_Locked=128;	// Set when texture locked
const USHORT					tFlag_Filler=256;	// Indicates a debug texture used to fill vidmem
const USHORT					tFlag_Detect=512;	// GameOS detected the format
const int					tFlag_FormatShift = 10;	// Bits 10,11,12 used for format
const USHORT					FormatMask = (7<<tFlag_FormatShift);
const USHORT					tFlag_Special=8192;	// Special texture (font, chess, or color mipmap)

typedef enum EGraphicsMemType { 
	gmt_All, gmt_Original, gmt_SysMem, gmt_VidMem
} EGraphicsMemType;

//
//	CTexInfo
//
//	Instances of this class represent textures created by the application.
//
class CTexInfo
{
private:
	USHORT					m_nWidth;			// Width in texels of original image
	USHORT					m_nHeight;			// Height of original image
	USHORT					m_Flags;			// tFlag_...
	USHORT					m_Hints;			// gosHint_...
	LPDIRECTDRAWSURFACE7	m_pOriginalSurf;	// Original 32bpp image (if any)
	LPDIRECTDRAWSURFACE7	m_pSysMemSurf;		// Converted image in system memory
	LPDIRECTDRAWSURFACE7	m_pVidMemSurf;		// Image in VRAM or AGP
	PSTR 					m_pName;			// Name of texture (or file)
	USHORT					m_nLockedLevel;		// Level currently locked
	USHORT					m_NextOffset;		// Forward-only linked list (free or used)
	gos_RebuildFunction		m_pRebuild;			// Application function to repopulate surface
	PVOID 					m_pInstance;		// Instance handle to pass to rebuild function
	ULONG					m_nLastFrameUsed;	// Frame number when this texture was last used
	ULONG					m_nLastTimeUsed;	// Value of TimeNo when this texture was last used
	ULONG					m_nOriginalBytes;	// Size of m_pOriginalSurf (even if not allocated)
	ULONG					m_nSysMemBytes;		// Size of m_pSysMemSurf (even if not allocated)
	ULONG					m_nVidMemBytes;		// Size of m_pVidMemSurf (even if not allocated)

// List management globals
static bool					Initialized;		// Texture manager has been initialized
static CTexInfo **			PreloadList;		// List of unique textures to preload
static int					NumPreload;			// Number of textures in PreloadList
static CTexInfo	*			TexInfo;			// Array of CTexInfo for all textures
static CTexInfo *			pFirstFreeTexture;	// Free linked list (using m_NextOffest)
static CTexInfo *			pFirstUsedTexture;	// Used linked list (using m_NextOffest)
static ULONG				FrameNo;			// Current Frame number (incremented by PreloadTextures())
static ULONG				TimeNo;				// Count of setrenderstate calls for LRU
static ULONG				AGPUsed;			// Bytes of AGP reserved (for debug limits)
static int					NumDummyTextures;	// number of valid textures in pDummyTex[]
static LPDIRECTDRAWSURFACE7 pDummyTex[1024];	// Video surfaces created to fill vidmem for debugger
static int					ScaleShift;			// 0 = no downscale, 1 = W/2,H/2, 2=W/4,H/4..
static int					NumVidTextures;		// The number of textures currently in video+AGP
static CTexInfo				MostRecentOriginal;	// A recently freed "original" - ony m_pOriginalSurf is valid
static CTexInfo				MostRecentSysMem;	// A recently freed "sysmem" - ony m_pSysMemSurf is valid

public:
	// Read-only operations
	USHORT					Width() { return m_nWidth; }
	USHORT					Height() { return m_nHeight; }
	USHORT					Hints() { return m_Hints; }
	bool					HasAlpha() { return (m_Flags & tFlag_Alpha)!=0; }
	bool					IsLocked() { return (m_Flags & tFlag_Locked)!=0; }
	bool					InVidMem() { return (m_Flags & tFlag_InVidMem)!=0; }
	bool					InAGP() { return (m_Flags & tFlag_InAGP)!=0; }
	bool					InSysMem() { return (m_Flags & tFlag_InSysMem)!=0; }
	bool					Detected() { return (m_Flags & tFlag_Detect)!=0; }
	bool					MipmapDisabled() { return (m_Hints & gosHint_DisableMipmap)!=0; }
	bool					CanRebuild() { return m_pRebuild!=NULL; }
	int						MipFilter() { return ( m_Hints / gosHint_MipmapFilter0 ) & 3; }
	ULONG					Area() { return ULONG(m_nWidth * m_nHeight); }
	int						MipMapLevels() { return (MipmapDisabled())?1:MipLevelsRequired(m_nWidth,m_nHeight); }
	PCSTR 					Name() { return m_pName; }
	PVOID 					pInstance() { return m_pInstance; }
	DDSURFACEDESC2 *		Description() { return &TextureDesc[Format()][(m_Hints&gosHint_Try32bpp)?1:0][(m_Hints&(gosHint_Compress0|gosHint_Compress1))/gosHint_Compress0]; }
static bool					ManagerInitialized() { return Initialized; }
	bool					ValidTexture() { return (CTexInfo::Initialized)&&(this>=TexInfo)&&(this-MaximumTextures<TexInfo)&&(m_Flags&tFlag_Valid); }
	bool					SpecialTexture() { return ( m_Flags & tFlag_Special ) != 0; }
	void					SetSpecial() { m_Flags |= tFlag_Special; }
	gos_RebuildFunction		pRebuildFunc() { return m_pRebuild; }
	static CTexInfo *		FirstTexture() { return pFirstUsedTexture; }
	CTexInfo *				NextTexture() { return m_NextOffset?(this+m_NextOffset):NULL; }
	gos_TextureFormat		Format() { return (gos_TextureFormat)((m_Flags&FormatMask)>>tFlag_FormatShift); }
	bool					Used() { return m_nLastFrameUsed == FrameNo; }
	USHORT					ScaledHeightWidth( bool WantHeight );
	USHORT					ScaledWidth() { return ScaledHeightWidth(false); }
	USHORT					ScaledHeight() { return ScaledHeightWidth(true); }
	static int				GlobalScaleShift() { return ScaleShift; }
	bool					SysMemSurfUsable();
// Texture Update.cpp
	int						MemoryUsage( bool Current=true, EGraphicsMemType MemType=gmt_All); // current or required bytes

		// Operations with effects on texture structures

// Texture Manager.cpp
	// Maintenance of static members & texture manager
	static void				InitializeTextureManager(); // System startup
	static void				DestroyTextureManager(); // System shutdown
	static void				PreloadTextures(); // Once per frame, upload any "preload" textures to VRAM/AGP
	static bool				RecreateHeaps(); // initializes texture types, ReleaseTextureHeaps()
	static void				ReleaseTextures( bool ReleaseSysmem ); // releases vram and optionally sysmem copies

// Texture Create.cpp
	// Fundamental new/init/delete of a CtexInfo
	static CTexInfo *		Allocate();
	void					Initialize( gos_TextureFormat Format, PCSTR FileName, PUCHAR pBitmap, ULONG Size,
								USHORT Width, USHORT Height, ULONG Hints, gos_RebuildFunction pFunc, PVOID pInstance );
	void					Free();

// Texture Update.cpp	
	// Rebuild / Reload / Convert / Upload as required to get a usable texture handle
	ULONG					GetD3DTextureHandle(); // HW Rasterizer: VidMemSurf, SW Rasterizer: SysMemSurf
	// Altering the surface contents of a texture
	void					Lock( ULONG MipMapWidth, bool ReadOnly, TEXTUREPTR* TextureInfo );
	void					Unlock();
	void					InvalidateVidMem();	// mark vidmem as invalid and check if current texture
	void					UpdateRect( ULONG DestLeft, ULONG DestTop, ULONG *Source, ULONG SourcePitch, ULONG Width,
								ULONG Height );
	// Various setters
	void					SetPreload();
	void					SetName( PCSTR name );
	void					SetUsed() { m_nLastFrameUsed = FrameNo; m_nLastTimeUsed = TimeNo++; }
	// Active private routines used within the texture manager

	// Notes on Get...( bool populate, USHORT MipMapWidth=0 )
	//		Get..() Allocates, and/or populates the different versions of the texture
	//		Each function allocates the surface if required and if populate is true and the surface
	//		contents are not already valid, copies/converts the contents from the prior level(s)
	//		GetOriginalSurf(true) will use the reload or rebuild operation as required to populate the surface
	//		If MipMapWidth is non-zero, returns specific level
	// Notes on Allocate...()
	//		Assumes surface does not already exist
	//		Allocates all mip levels required for the texture (factoring in flags)
	//		Does not touch validity flags (e.g. tFlag_InVidmem)
	// Notes on Populate...()
	//		Assumes source surface exists and is valid
	//		Assumes dest surface exists but is not valid
	//		Sets appropriate validity flag
	// Notes on Free...()
	//		Assumes surface exists
	//		Clears appropriate validity flag and clears pointer to surface

// Texture Original.cpp
	// Original surface
public:
	LPDIRECTDRAWSURFACE7	GetOriginalSurf( bool populate, USHORT MipMapWidth=0 );
private:
	void					AllocateOriginal();	// Allocates original 32bpp surface
	void					PopulateOriginal();	// Rebuilds or Reloads (assumes surface exists but is not valid)
	void					Reload( PUCHAR pData, ULONG Size, bool Detect ); // reloads 32bpp surface from file or pData
	void					Rebuild();			// Calls rebuild function of app to re-populate the surface
	void					FreeOriginal();		// Frees the original surface (assumes it exists and is ok to free)
	void					DecodeImageLevel( PCSTR FileName, PUCHAR pSourceData, ULONG Size, LPDIRECTDRAWSURFACE7 pSurface );
// Texture SysMem.cpp
	// System Memory Surface
public:
	LPDIRECTDRAWSURFACE7	GetSysMemSurf( bool populate, USHORT MipMapWidth=0 );
private:
	void					AllocateSysMem();	// Allocates (probably 16bpp) system memory surface
	void					PopulateSysMem();	// (Assumes valid original) Converts the Original Level(s)
	void					ConvertLevel( LPDIRECTDRAWSURFACE7 dest, LPDIRECTDRAWSURFACE7 source );
												// ...Converts one level from original to sysmem
	void					FreeSysMem();		// Frees the SysMem surface/chain (assumes it exists and is ok to free)
// Mipmap.cpp
	void					GenerateMipMaps();	// Filters top level of SysMem chain to all lower levels
// Texture VidMem.cpp
	// Video Memory or AGP Surface
	LPDIRECTDRAWSURFACE7	GetVidMemSurf( bool populate, USHORT MipMapWidth=0 );
	bool					CreateTexture();	// Called by AllocateVidMem() to attempt texture creation
	void					AllocateVidMem();	// Uses various techniques to allocate video memory
	void					PopulateVidMem();	// Uploads level(s) from SysMem to VidMem/AGP
	void					UploadLevel( LPDIRECTDRAWSURFACE7 dest, LPDIRECTDRAWSURFACE7 source );
public:
	void					FreeVidMem();		// Frees the Vidmem Surface(s) (assumes it exists and is ok to free)
private:

	// Private helper function(s)
	void					SetFormat( gos_TextureFormat Format ){m_Flags= (USHORT)((m_Flags & ~FormatMask )
																	| ( (USHORT)(Format) << tFlag_FormatShift));}
	LPDIRECTDRAWSURFACE7 	FindTextureLevel( LPDIRECTDRAWSURFACE7 pSurf, USHORT MipMapWidth ); // Helper function
public:
	static CTexInfo *		TextureHandleToPtr( ULONG Handle );
	ULONG					TexturePointerToHandle();

	static void				NewAGPLimit();		// called by debugger when AGP limit changes
	static void				NewLocalLimit();	// called by debugger when local limit changes
	static void				FreeDummyTextures();// Free the artificially created vidmem textures

public:
	static void				NextFrame() { FrameNo++; }
private:
	static void				AnalyzeTextureFormats();
// Texture Create.cpp
	static void				CheckChain();

	static void				PurgeTextures();	// removes all app textures and does flip's to let them go

#if defined(LAB_ONLY)
	static char				LogString[MaximumTextureLogs][256];
	static int				LogCount;			// 0..MaximumTextureLogs
	static int				LogNext;			// index of next string to write to in circular log
	static bool				Logging;
	void					LogTextureEvent( PSTR description );
public:
	static PSTR 			GetTextureEvent( int offset ) { return (offset>=LogCount)?(NULL):(LogString[(LogNext+MaximumTextureLogs-1-offset)%MaximumTextureLogs]); }
	static bool				LogEnable( bool enable = Logging ) { Logging = enable; return Logging; }
#else
	void					LogTextureEvent( PSTR /*description*/ ) { ; }
public:
	static PSTR 			GetTextureEvent( int /*offset*/ ) { return NULL; }
	static bool				LogEnable( bool /*enable = false*/ ) { return false; }
#endif

};

// Texture management API
inline void __stdcall CheckPreloadTextures(void) { CTexInfo::PreloadTextures(); }
inline void __stdcall ReleaseTextureHeap( bool ReleaseSysmem ) { CTexInfo::ReleaseTextures(ReleaseSysmem); }
inline void __stdcall InitTextureManager(void) { CTexInfo::InitializeTextureManager(); }
inline void __stdcall DestroyTextureManager(void) { CTexInfo::DestroyTextureManager(); }

ULONG __stdcall GetTexturesUsed(void);

// Statistics
extern ULONG TexturesUsed;
extern ULONG TextureKUsed;
extern ULONG TextureVKUsed;
extern ULONG AlphaTextureKUsed;
extern ULONG TexturesSwapped;
extern ULONG TextureBytesPaged;
extern ULONG TextureBytesPaged1;
extern ULONG TexturesDestroyed;
extern ULONG TotalTexMemoryAllocated;
extern ULONG TexturesChanged;
extern ULONG VidMemUsed;				// Number of textures actually allocated in video memory
extern ULONG TexturesBumped;
extern float TextureAGPMegs;
extern float TextureLocalMegs;
extern ULONG TexturesLoaded;
extern ULONG TexturesCreated;
