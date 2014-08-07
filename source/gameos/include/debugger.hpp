//===========================================================================//
// File:	 Debugger.hpp													 //
// Contents: Debugger														 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

//
// Size of scroll back buffer for spew
//
#define DebuggerBufferSize 128*1024

extern gos_VERTEX pTextVertices[80];
extern ULONG TextVertex;
extern ULONG WantAA;
extern ULONG Want32;
extern ULONG Want32T;
extern ULONG WantRes;
extern ULONG WantRVRAM;
extern ULONG WantSW;
extern ULONG WantSW1;
extern ULONG WantHW;
extern ULONG FrameAdvance;							// Used to step or fast forward through scripts
extern ULONG OldFreeze;								// Old value of gFreezeLogic
extern ULONG OldRender;								// Old value of gStopRendering;
extern ULONG PerfCounterSelected;
extern int PerfYStart,PerfYEnd;
extern ULONG DoUpdateWindow;
extern ULONG DoingAdvance;
extern ULONG OldgStopSystem;
extern ULONG OldgGameLogicActive;
extern UCHAR MenuActive;
extern UCHAR SubMenuActive;
extern UCHAR	DebounceMenu;
extern ULONG OldStop;
extern ULONG OldRendering;
extern int AreaL, AreaR, AreaT, AreaB;
extern ULONG PolygonsInArea;
extern float DebuggerTexelOffset;
extern int DBWheelDelta;
extern ULONG gShowMemoryUseage;						// 2=Enable at next frame, 1=show data, 0=Do not show data
extern ULONG gShowWSUseage;							// 2=Enable at next frame, 1=show data, 0=Do not show data
extern ULONG gShowFTOLUseage;						// 2=Enable at next frame, 1=show data, 0=Do not show data
extern ULONG gShowALLUseage;
extern gos_VERTEX Graph[512];
extern ULONG gScreenBMP;

void	__stdcall WalkStack( ULONG* RoutineAddresses, ULONG NumberOfLevels, ULONG IgnoreLevels );
PSTR	__stdcall DecodeAddress( ULONG Address, UCHAR brief = true );
void	__stdcall DrawLines( int X1, int Y1, int X2, int Y2, ULONG Color );

typedef struct _MenuItem
{
	struct _MenuItem*	pNext;
	struct _MenuItem*	pSubMenu;							// Pointer to sub menu list, or NULL
	ULONG	(__stdcall *Callback)(PSTR Name, ULONG MenuFunction);
	void	(__stdcall *Routine)(void);								// Sub menu list pointer
	UCHAR	(__stdcall *Greyed)(void);
	UCHAR	(__stdcall *CheckMark)(void);							// 0 When no check mark routine, 1 when sub menu
	PSTR FullName;
	PSTR Name;
} MenuItem;



extern MenuItem*	pDebuggerMenu;
extern MenuItem*	pRasterizerMenu;
extern MenuItem*	pLibrariesMenu;
extern MenuItem*	pGameMenu;
//
extern MenuItem*	CurrentMenu;
extern MenuItem*	MenuHighlight;
extern MenuItem**	pCurrentMenu;
extern void __stdcall CleanUpDebugger(void);
extern void __stdcall InitDebuggerMenus(void);

#if defined(LAB_ONLY)
//
// Pie chart data
//
	extern gos_VERTEX PieData[103];	
#endif


//
// Array of textures in texture heap display
//
typedef struct _TextureHeap {
	struct _TextureHeap*	pNext;
	int						X1;
	int						Y1;
	int						X2;
	int						Y2;
	ULONG					Handle;
} TextureHeap;

extern TextureHeap* pTextureHeap;
extern ULONG SelectedHandle;
extern ULONG TextureToUnload;
UCHAR __stdcall CheckWindow(void);

//
// Main Debugger Screens
//

typedef enum EDbgScreen {
	DbgS_Stat,					// 0
	DbgS_Spew,					// 1
	DbgS_Texture,				// 2
	DbgS_Network,				// 3
	DbgS_Sound,					// 4
	DbgS_Control,				// 5
	DbgS_UsedTextures,			// 6
	DbgS_UnusedTextures,		// 7
	DbgS_Memory,				// 8
	DbgS_CPUStats,				// 9
	DbgS_PlayHistory,			// 10
	DbgS_SoundResource,			// 11
	DbgS_NetworkX,				// 12
	DbgS_File,					// 13
	DbgS_SoundSpatial,			// 14
	DbgS_LoadedTextures,		// 15
	DbgS_AllTextures,			// 16
	DbgS_UsedTextureDetail,		// 17
	DbgS_UnusedTextureDetail,	// 18
	DbgS_LoadedTextureDetail,	// 19
	DbgS_AllTextureDetail,		// 20
	DbgS_AllTextureNoUpload,	// 21
	DbgS_TextureLog,			// 22
} EDbgScreen;
extern  EDbgScreen DebugDisplay;


//
// Framegraph information
//
typedef enum FrameGraphMode {
	Graph_Chart=0,
	Graph_30,
	Graph_Pie30,
	Graph_60,
	Graph_Pie60,
} FrameGraphMode;

extern FrameGraphMode GraphMode;
extern ULONG ShowColorInfo;

//
// True when the debugger window is visible on the display
//
extern UCHAR DebuggerActive;
extern UCHAR InDebugger;						// During debugger rendering
extern UCHAR ProcessMemorySize;				// When true will calculate each processes memory size in the exception handler (can take about 1 second!)
extern int ZoomMode;						// Zoom screen around cursor
extern int BaseZoom;
extern ULONG gForceNoFlip;					// Force blt instead of flip in fullscreen
extern ULONG gShowAverage;
extern ULONG gShowGraphsAsTime;
extern ULONG gShowGraphBackground;
extern ULONG gNoGraph;
extern ULONG WhichImage;					// Original / Special mode image?
//
// Scroll back buffer for Dumplog (128K)
//
extern char DebuggerBuffer[DebuggerBufferSize];
extern ULONG CurrentDebugSpewPos;		// Current position in buffer
extern ULONG CurrentDebugTopOfScreen;	// Current top of screen
//
// Alpha values to added to debugger window
//
extern ULONG DebuggerAlpha;
//
// Mouse position
//
extern int DBMouseX,DBMouseY;
extern ULONG DBMouseMoved;
extern int ExMouseX,ExMouseY;
extern ULONG DBButtons;

extern ULONG gEnableMulti;
extern ULONG gForceMono;
extern ULONG gForceFlat;
extern ULONG gForceBlend;
extern ULONG gForceGouraud;
extern ULONG gForcePerspective;
extern ULONG gForceNoPerspective;
extern ULONG gForceSpecular;
extern ULONG gForceNoSpecular;
extern ULONG gForceAlpha;
extern ULONG gShowVertexData;
extern ULONG gForceNoFog;
extern ULONG gForceNoTextures;
extern ULONG gForceBiLinear;
extern ULONG gForceTriLinear;
extern int	 gForceMipBias;
extern ULONG gForceNoFiltering;
extern ULONG gForceNoDithering;
extern ULONG gForceDithering;
extern ULONG gForceChessTexture;
extern ULONG gForceNoAlphaTest;
extern ULONG gForceAlphaTest;
extern ULONG ChessTexture;				// The chess texture
extern ULONG MipColorTexture;			// The mipmap colored texture
extern ULONG gForceAlphaBlending;
extern ULONG gUseGameSpew;
extern ULONG gControlsActive;					// Allow controls for the game during the debugger
extern ULONG gGameLogicActive;					// Allow game logic to run when debugger up
extern ULONG gStopSystem;						// Stop whole system in debugger
extern ULONG gFrameGraph;
extern ULONG gStopGameRendering;
extern ULONG gStopRendering;					// Stop rendering
extern ULONG gFreezeLogic;						// Stop rendering
extern ULONG gShowLFControls;
extern UCHAR NoDebuggerStats;						// When 0 Stats and spews are active during GameOS functions like debugger
extern ULONG gEnableRS;
extern ULONG gTextureOverrun;
extern ULONG gDisableLighting;
extern ULONG gDisableNormalizing;
extern ULONG gDisableHardwareTandL;
extern ULONG gDisableVertexBlending;

//
// Debugger window variables
//
extern int DbTopX,DbTopY;
extern int DbMaxX,DbMaxY,DbMinX;

extern ULONG TopStatistics;

//
// Debugger window constants
//
#define DbSizeX 489
#define DbSizeY 342


extern int CurrentX,CurrentY;		// Current pixel position
extern int DbChrX,DbChrY;			// Current character x,y
extern int StartX,StartY;			// Current start of line



void __stdcall UpdateDebugger(void);
void __stdcall InitDebugger(void);
void __stdcall EndRenderMode(void);
void __stdcall DestroyDebugger(void);
void __stdcall SpewToDebugger( PSTR Message );
int __stdcall GetMipmapUsed( ULONG Handle, pgos_VERTEX Pickv1, pgos_VERTEX Pickv2, pgos_VERTEX Pickv3 );
void __stdcall ShowFrameGraphs(void);
void __stdcall UpdateDebugMouse(void);
void __stdcall UpdateDebugWindow(void);
void __stdcall InitTextDisplay(void);
void __stdcall DrawText( ULONG Color, PSTR String );
void __stdcall DrawSquare( int TopX, int TopY, int Width, int Height, ULONG Color );
void __stdcall DrawChr( char Chr );














