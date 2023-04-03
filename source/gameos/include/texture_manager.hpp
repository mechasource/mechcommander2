//===========================================================================//
// File: Texture Manager.hpp                                                 //
// Contents: Texture Mananger                                                //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

class CTexInfo; // Per-texture class defined below

//
// List of video memory textures (so we don't delete on some cards)
//
typedef struct _VidMemHeap
{
	struct _VidMemHeap* pNext;
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 Surface;

} VidMemHeap;

extern VidMemHeap* pFreeVidMemTextures;

#if 1
#define TEXHANDLE_TO_PTR(handle) ((CTexInfo*)(handle))
#define TEXPTR_TO_HANDLE(ptr) ((uint32_t)(ptr))
#else
#define TEXHANDLE_TO_PTR(handle) (CTexInfo::TextureHandleToPtr(handle))
#define TEXPTR_TO_HANDLE(ptr) (ptr->TexturePointerToHandle(void))
#endif

extern int32_t HasMipMap; // Can do mipmapping
extern D3DDEVICEDESC7 CapsDirect3D; // Current 3D device caps

// Pixel conversion helper functions
uint32_t __stdcall GetPixelColor(uint32_t in);
uint32_t GetBackBufferColor(uint16_t in);
void UpdateBackBufferFormat(void);

void __stdcall DecodeBMPDimensions(PSTR filename, BOOLEAN* Data, uint32_t DataSize,
	uint32_t* pTexturewidth, uint32_t* pTextureheight);
void __stdcall DecodeJPGDimensions(PSTR filename, BOOLEAN* Data, uint32_t DataSize,
	uint32_t* pTexturewidth, uint32_t* pTextureheight);
#ifdef USEPNG
PVOID __stdcall DecodePNG(PSTR filename, BOOLEAN* Data, uint32_t DataSize, uint32_t* Texturewidth,
	uint32_t* Textureheight, BOOLEAN TextureLoad, PVOID pDestSurf = nullptr);
void __stdcall DecodePNGDimensions(PSTR filename, BOOLEAN* Data, uint32_t DataSize,
	uint32_t* pTexturewidth, uint32_t* pTextureheight);
#endif
void DecodeTGADimensions(PSTR filename, BOOLEAN* Data, uint32_t DataSize, uint32_t* pTexturewidth,
	uint32_t* pTextureheight);

PVOID
DecodeBMP(PSTR filename, BOOLEAN* Data, uint32_t DataSize, uint32_t* Texturewidth,
	uint32_t* Textureheight, BOOLEAN TextureLoad, PVOID pDestSurf = nullptr);
PVOID
DecodeJPG(PSTR filename, BOOLEAN* Data, uint32_t DataSize, uint32_t* Texturewidth,
	uint32_t* Textureheight, BOOLEAN TextureLoad, PVOID pDestSurf = nullptr);
PVOID
DecodeTGA(PSTR filename, BOOLEAN* Data, uint32_t DataSize, uint32_t* Texturewidth,
	uint32_t* Textureheight, BOOLEAN TextureLoad, PVOID pDestSurf = nullptr);

int32_t
MipLevelsRequired(uint16_t width, uint16_t height);

// GOS Internal textures
extern uint32_t FontTexture; // The font and mouse pointer
extern uint32_t ChessTexture; // Checkerboard pattern
extern uint32_t MipColorTexture; // The mipmap color texture

// Valid surface descriptions
extern DDSURFACEDESC2 TextureDesc[5][2][4]; // arranged by: [Format][want32bpp][compression]
extern int32_t TextureDescScore[5][2][4]; // validity of current entry in TextrueDesc

// Maximum number of textures that are supported (can be altered independently
// from display cards)
const uint32_t MaximumTextures = 3000;
extern uint32_t MaximumVidTextures; // limit the number simultaneously loaded

const uint32_t MaximumTextureLogs = 32;

// Texture flags
const uint16_t tFlag_InVidMem = 1; // Currently loaded in Video Memory or AGP
const uint16_t tFlag_InAGP = 2; // Allocation is in AGP ( tFlag_InVidMem set separately )
const uint16_t tFlag_Valid = 4; // This CTexInfo has been allocated
const uint16_t tFlag_Alpha = 8; // Texture format has ALPHA (either keyed or full channel)
const uint16_t tFlag_InSysMem = 16; // Texture has been converted into system memory format
const uint16_t tFlag_LockRW = 32; // Set when a texture is locked for writing
const uint16_t tFlag_Preload = 64; // Preload this texture at the end of the current frame
const uint16_t tFlag_Locked = 128; // Set when texture locked
const uint16_t tFlag_Filler = 256; // Indicates a debug texture used to fill vidmem
const uint16_t tFlag_Detect = 512; // GameOS detected the format
const int32_t tFlag_FormatShift = 10; // Bits 10,11,12 used for format
const uint16_t FormatMask = (7 << tFlag_FormatShift);
const uint16_t tFlag_Special = 8192; // Special texture (font, chess, or color mipmap)

enum class EGraphicsMemType
{
	gmt_All,
	gmt_Original,
	gmt_SysMem,
	gmt_VidMem
} EGraphicsMemType;

//
// CTexInfo
//
// Instances of this class represent textures created by the application.
//
class CTexInfo
{
private:
	uint16_t m_nwidth; // width in texels of original image
	uint16_t m_nheight; // height of original image
	uint16_t m_Flags; // tFlag_...
	uint16_t m_Hints; // gosHint_...
	LPDIRECTDRAWSURFACE7 m_pOriginalSurf; // Original 32bpp image (if any)
	LPDIRECTDRAWSURFACE7 m_pSysMemSurf; // Converted image in system memory
	LPDIRECTDRAWSURFACE7 m_pVidMemSurf; // Image in VRAM or AGP
	PSTR m_pName; // Name of texture (or file)
	uint16_t m_nLockedLevel; // Level currently locked
	uint16_t m_NextOffset; // Forward-only linked list (free or used)
	gos_RebuildFunction m_pRebuild; // Application function to repopulate surface
	PVOID m_pInstance; // Instance handle to pass to rebuild function
	uint32_t m_nLastFrameUsed; // Frame number when this texture was last used
	uint32_t m_nLastTimeUsed; // value of TimeNo when this texture was last used
	uint32_t m_nOriginalBytes; // Size of m_pOriginalSurf (even if not allocated)
	uint32_t m_nSysMemBytes; // Size of m_pSysMemSurf (even if not allocated)
	uint32_t m_nVidMemBytes; // Size of m_pVidMemSurf (even if not allocated)

	// List management globals
	static BOOLEAN Initialized; // Texture manager has been initialized
	static CTexInfo** PreloadList; // List of unique textures to preload
	static int32_t NumPreload; // Number of textures in PreloadList
	static CTexInfo* TexInfo; // Array of CTexInfo for all textures
	static CTexInfo* pFirstFreeTexture; // Free linked list (using m_NextOffest)
	static CTexInfo* pFirstUsedTexture; // Used linked list (using m_NextOffest)
	static uint32_t FrameNo; // Current Frame number (incremented by PreloadTextures())
	static uint32_t TimeNo; // Count of setrenderstate calls for LRU
	static uint32_t AGPUsed; // Bytes of AGP reserved (for debug limits)
	static int32_t NumDummyTextures; // number of valid textures in pDummyTex[]
	static LPDIRECTDRAWSURFACE7
		pDummyTex[1024]; // Video surfaces created to fill vidmem for debugger
	static int32_t ScaleShift; // 0 = no downscale, 1 = W/2,H/2, 2=W/4,H/4..
	static int32_t NumVidTextures; // The number of textures currently in video+AGP
	static CTexInfo
		MostRecentOriginal; // A recently freed "original" - ony m_pOriginalSurf is valid
	static CTexInfo MostRecentSysMem; // A recently freed "sysmem" - ony m_pSysMemSurf is valid

public:
	// Read-only operations
	uint16_t width(void)
	{
		return m_nwidth;
	}
	uint16_t height(void)
	{
		return m_nheight;
	}
	uint16_t Hints(void)
	{
		return m_Hints;
	}
	BOOLEAN HasAlpha(void)
	{
		return (m_Flags & tFlag_Alpha) != 0;
	}
	BOOLEAN IsLocked(void)
	{
		return (m_Flags & tFlag_Locked) != 0;
	}
	BOOLEAN InVidMem(void)
	{
		return (m_Flags & tFlag_InVidMem) != 0;
	}
	BOOLEAN InAGP(void)
	{
		return (m_Flags & tFlag_InAGP) != 0;
	}
	BOOLEAN InSysMem(void)
	{
		return (m_Flags & tFlag_InSysMem) != 0;
	}
	BOOLEAN Detected(void)
	{
		return (m_Flags & tFlag_Detect) != 0;
	}
	BOOLEAN MipmapDisabled(void)
	{
		return (m_Hints & gosHint_DisableMipmap) != 0;
	}
	BOOLEAN CanRebuild(void)
	{
		return m_pRebuild != nullptr;
	}
	int32_t MipFilter(void)
	{
		return (m_Hints / gosHint_MipmapFilter0) & 3;
	}
	uint32_t Area(void)
	{
		return uint32_t(m_nwidth * m_nheight);
	}
	int32_t MipMapLevels(void)
	{
		return (MipmapDisabled()) ? 1 : MipLevelsRequired(m_nwidth, m_nheight);
	}
	PSTR Name(void)
	{
		return m_pName;
	}
	PVOID pInstance(void)
	{
		return m_pInstance;
	}
	DDSURFACEDESC2* Description(void)
	{
		return &TextureDesc[Format()][(m_Hints & gosHint_Try32bpp)
				? 1
				: 0][(m_Hints & (gosHint_Compress0 | gosHint_Compress1)) / gosHint_Compress0];
	}
	static BOOLEAN ManagerInitialized(void)
	{
		return Initialized;
	}
	BOOLEAN ValidTexture(void)
	{
		return (CTexInfo::Initialized) && (this >= TexInfo) && (this - MaximumTextures < TexInfo) && (m_Flags & tFlag_Valid);
	}
	BOOLEAN SpecialTexture(void)
	{
		return (m_Flags & tFlag_Special) != 0;
	}
	void SetSpecial(void)
	{
		m_Flags |= tFlag_Special;
	}
	gos_RebuildFunction pRebuildFunc(void)
	{
		return m_pRebuild;
	}
	static CTexInfo* FirstTexture(void)
	{
		return pFirstUsedTexture;
	}
	CTexInfo* NextTexture(void)
	{
		return m_NextOffset ? (this + m_NextOffset) : nullptr;
	}
	gos_TextureFormat Format(void)
	{
		return (gos_TextureFormat)((m_Flags & FormatMask) >> tFlag_FormatShift);
	}
	BOOLEAN Used(void)
	{
		return m_nLastFrameUsed == FrameNo;
	}
	uint16_t Scaledheightwidth(BOOLEAN Wantheight);
	uint16_t Scaledwidth(void)
	{
		return Scaledheightwidth(false);
	}
	uint16_t Scaledheight(void)
	{
		return Scaledheightwidth(true);
	}
	static int32_t GlobalScaleShift(void)
	{
		return ScaleShift;
	}
	BOOLEAN SysMemSurfUsable(void);
	// Texture Update.cpp
	int32_t MemoryUsage(BOOLEAN Current = true,
		EGraphicsMemType MemType = gmt_All); // current or required bytes

	// Operations with effects on texture structures

	// Texture Manager.cpp
	// Maintenance of static members & texture manager
	static void InitializeTextureManager(void); // System startup
	static void DestroyTextureManager(void); // System shutdown
	static void PreloadTextures(void); // Once per frame, upload any "preload" textures to VRAM/AGP
	static BOOLEAN RecreateHeaps(void); // initializes texture types, ReleaseTextureHeaps()
	static void ReleaseTextures(BOOLEAN ReleaseSysmem); // releases vram and optionally sysmem copies

	// Texture Create.cpp
	// Fundamental new/init/delete of a CtexInfo
	static CTexInfo* Allocate(void);
	void Initialize(gos_TextureFormat Format, PSTR filename, BOOLEAN* pBitmap, uint32_t Size,
		uint16_t width, uint16_t height, uint32_t Hints, gos_RebuildFunction pFunc,
		PVOID pInstance);
	void Free(void);

	// Texture Update.cpp
	// Rebuild / Reload / Convert / Upload as required to get a usable texture
	// handle
	uint32_t GetD3DTextureHandle(void); // HW Rasterizer: VidMemSurf, SW Rasterizer: SysMemSurf
	// Altering the surface contents of a texture
	void Lock(uint32_t MipMapwidth, BOOLEAN ReadOnly, TEXTUREPTR* TextureInfo);
	void Unlock(void);
	void InvalidateVidMem(void); // mark vidmem as invalid and check if current texture
	void UpdateRect(uint32_t DestLeft, uint32_t DestTop, uint32_t* Source, uint32_t SourcePitch,
		uint32_t width, uint32_t height);
	// Various setters
	void SetPreload(void);
	void SetName(PSTR name);
	void SetUsed(void)
	{
		m_nLastFrameUsed = FrameNo;
		m_nLastTimeUsed = TimeNo++;
	}
	// Active private routines used within the texture manager

	// Notes on Get...( BOOLEAN populate, uint16_t MipMapwidth=0 )
	// Get..() Allocates, and/or populates the different versions of the texture
	// Each function allocates the surface if required and if populate is true
	// and the surface contents are not already valid, copies/converts the
	// contents from the prior level(s) GetOriginalSurf(true) will use the
	// reload or rebuild operation as required to populate the surface If
	// MipMapwidth is non-zero, returns specific level Notes on Allocate...()
	// Assumes surface does not already exist
	// Allocates all mip levels required for the texture (factoring in flags)
	// Does not touch validity flags (e.g. tFlag_InVidmem)
	// Notes on Populate...()
	// Assumes source surface exists and is valid
	// Assumes dest surface exists but is not valid
	// Sets appropriate validity flag
	// Notes on Free...()
	// Assumes surface exists
	// Clears appropriate validity flag and clears pointer to surface

	// Texture Original.cpp
	// Original surface
public:
	LPDIRECTDRAWSURFACE7 GetOriginalSurf(BOOLEAN populate, uint16_t MipMapwidth = 0);

private:
	void AllocateOriginal(void); // Allocates original 32bpp surface
	void PopulateOriginal(void); // Rebuilds or Reloads (assumes surface exists but is not valid)
	void Reload(
		BOOLEAN* pData, uint32_t Size, BOOLEAN Detect); // reloads 32bpp surface from file or pData
	void Rebuild(void); // Calls rebuild function of app to re-populate the surface
	void FreeOriginal(void); // Frees the original surface (assumes it exists and is ok to free)
	void DecodeImageLevel(
		PSTR filename, BOOLEAN* pSourceData, uint32_t Size, LPDIRECTDRAWSURFACE7 pSurface);

	// Texture SysMem.cpp
	// System Memory Surface
public:
	LPDIRECTDRAWSURFACE7 GetSysMemSurf(BOOLEAN populate, uint16_t MipMapwidth = 0);

private:
	void AllocateSysMem(void); // Allocates (probably 16bpp) system memory surface
	void PopulateSysMem(void); // (Assumes valid original) Converts the Original Level(s)
	void ConvertLevel(LPDIRECTDRAWSURFACE7 dest, LPDIRECTDRAWSURFACE7 source);
	// ...Converts one level from original to sysmem
	void FreeSysMem(void); // Frees the SysMem surface/chain (assumes it exists
		// and is ok to free)
	// Mipmap.cpp
	void GenerateMipMaps(void); // Filters top level of SysMem chain to all lower levels
	// Texture VidMem.cpp
	// Video Memory or AGP Surface
	LPDIRECTDRAWSURFACE7 GetVidMemSurf(BOOLEAN populate, uint16_t MipMapwidth = 0);
	BOOLEAN CreateTexture(void); // Called by AllocateVidMem(void) to attempt texture creation
	void AllocateVidMem(void); // Uses various techniques to allocate video memory
	void PopulateVidMem(void); // Uploads level(s) from SysMem to VidMem/AGP
	void UploadLevel(LPDIRECTDRAWSURFACE7 dest, LPDIRECTDRAWSURFACE7 source);

public:
	void FreeVidMem(void); // Frees the Vidmem Surface(s) (assumes it exists and
		// is ok to free)
private:
	// Private helper function(s)
	void SetFormat(gos_TextureFormat Format)
	{
		m_Flags = (uint16_t)((m_Flags & ~FormatMask) | ((uint16_t)(Format) << tFlag_FormatShift));
	}
	LPDIRECTDRAWSURFACE7 FindTextureLevel(
		LPDIRECTDRAWSURFACE7 pSurf, uint16_t MipMapwidth); // Helper function
public:
	static CTexInfo* TextureHandleToPtr(uint32_t handle);
	uint32_t TexturePointerToHandle(void);

	static void NewAGPLimit(void); // called by debugger when AGP limit changes
	static void NewLocalLimit(void); // called by debugger when local limit changes
	static void FreeDummyTextures(void); // Free the artificially created vidmem textures

public:
	static void NextFrame(void)
	{
		FrameNo++;
	}

private:
	static void AnalyzeTextureFormats(void);
	// Texture Create.cpp
	static void CheckChain(void);

	static void PurgeTextures(void); // removes all app textures and does flip's to let them go

#if defined(LAB_ONLY)
	static char LogString[MaximumTextureLogs][256];
	static int32_t LogCount; // 0..MaximumTextureLogs
	static int32_t LogNext; // index of next string to write to in circular log
	static BOOLEAN Logging;
	void LogTextureEvent(PSTR description);

public:
	static PSTR GetTextureEvent(int32_t offset)
	{
		return (offset >= LogCount)
			? (nullptr)
			: (LogString[(LogNext + MaximumTextureLogs - 1 - offset) % MaximumTextureLogs]);
	}
	static BOOLEAN LogEnable(BOOLEAN enable = Logging)
	{
		Logging = enable;
		return Logging;
	}
#else
	void LogTextureEvent(PSTR /*description*/)
	{
		;
	}

public:
	static PSTR GetTextureEvent(int32_t /*offset*/)
	{
		return nullptr;
	}
	static BOOLEAN LogEnable(BOOLEAN /*enable = false*/)
	{
		return false;
	}
#endif
};

// Texture management API
inline void __stdcall CheckPreloadTextures(void)
{
	CTexInfo::PreloadTextures();
}
inline void __stdcall ReleaseTextureHeap(BOOLEAN releasesysmem)
{
	CTexInfo::ReleaseTextures(releasesysmem);
}
inline void __stdcall InitTextureManager(void)
{
	CTexInfo::InitializeTextureManager();
}
inline void __stdcall DestroyTextureManager(void)
{
	CTexInfo::DestroyTextureManager();
}

uint32_t __stdcall GetTexturesUsed(void);

// Statistics
extern uint32_t TexturesUsed;
extern uint32_t TextureKUsed;
extern uint32_t TextureVKUsed;
extern uint32_t AlphaTextureKUsed;
extern uint32_t TexturesSwapped;
extern uint32_t TextureBytesPaged;
extern uint32_t TextureBytesPaged1;
extern uint32_t TexturesDestroyed;
extern uint32_t TotalTexMemoryAllocated;
extern uint32_t TexturesChanged;
extern uint32_t VidMemUsed; // Number of textures actually allocated in video memory
extern uint32_t TexturesBumped;
extern float TextureAGPMegs;
extern float TextureLocalMegs;
extern uint32_t TexturesLoaded;
extern uint32_t TexturesCreated;
