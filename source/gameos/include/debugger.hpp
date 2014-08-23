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
extern uint32_t TextVertex;
extern uint32_t WantAA;
extern uint32_t Want32;
extern uint32_t Want32T;
extern uint32_t WantRes;
extern uint32_t WantRVRAM;
extern uint32_t WantSW;
extern uint32_t WantSW1;
extern uint32_t WantHW;
extern uint32_t FrameAdvance;							// Used to step or fast forward through scripts
extern uint32_t OldFreeze;								// Old value of gFreezeLogic
extern uint32_t OldRender;								// Old value of gStopRendering;
extern uint32_t PerfCounterSelected;
extern int32_t PerfYStart,PerfYEnd;
extern uint32_t DoUpdateWindow;
extern uint32_t DoingAdvance;
extern uint32_t OldgStopSystem;
extern uint32_t OldgGameLogicActive;
extern uint8_t MenuActive;
extern uint8_t SubMenuActive;
extern uint8_t	DebounceMenu;
extern uint32_t OldStop;
extern uint32_t OldRendering;
extern int32_t AreaL, AreaR, AreaT, AreaB;
extern uint32_t PolygonsInArea;
extern float DebuggerTexelOffset;
extern int32_t DBWheelDelta;
extern uint32_t gShowMemoryUseage;						// 2=Enable at next frame, 1=show data, 0=Do not show data
extern uint32_t gShowWSUseage;							// 2=Enable at next frame, 1=show data, 0=Do not show data
extern uint32_t gShowFTOLUseage;						// 2=Enable at next frame, 1=show data, 0=Do not show data
extern uint32_t gShowALLUseage;
extern gos_VERTEX Graph[512];
extern uint32_t gScreenBMP;

void	__stdcall WalkStack( uint32_t* RoutineAddresses, uint32_t NumberOfLevels, uint32_t IgnoreLevels );
PSTR	__stdcall DecodeAddress( uint32_t Address, uint8_t brief = true );
void	__stdcall DrawLines( int32_t X1, int32_t Y1, int32_t X2, int32_t Y2, uint32_t Color );

typedef struct _MenuItem
{
	struct _MenuItem*	pNext;
	struct _MenuItem*	pSubMenu;							// Pointer to sub menu list, or NULL
	uint32_t	(__stdcall *Callback)(PSTR Name, uint32_t MenuFunction);
	void	(__stdcall *Routine)(void);								// Sub menu list pointer
	uint8_t	(__stdcall *Greyed)(void);
	uint8_t	(__stdcall *CheckMark)(void);							// 0 When no check mark routine, 1 when sub menu
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
	int32_t						X1;
	int32_t						Y1;
	int32_t						X2;
	int32_t						Y2;
	uint32_t					Handle;
} TextureHeap;

extern TextureHeap* pTextureHeap;
extern uint32_t SelectedHandle;
extern uint32_t TextureToUnload;
uint8_t __stdcall CheckWindow(void);

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
extern uint32_t ShowColorInfo;

//
// True when the debugger window is visible on the display
//
extern uint8_t DebuggerActive;
extern uint8_t InDebugger;						// During debugger rendering
extern uint8_t ProcessMemorySize;				// When true will calculate each processes memory size in the exception handler (can take about 1 second!)
extern int32_t ZoomMode;						// Zoom screen around cursor
extern int32_t BaseZoom;
extern uint32_t gForceNoFlip;					// Force blt instead of flip in fullscreen
extern uint32_t gShowAverage;
extern uint32_t gShowGraphsAsTime;
extern uint32_t gShowGraphBackground;
extern uint32_t gNoGraph;
extern uint32_t WhichImage;					// Original / Special mode image?
//
// Scroll back buffer for Dumplog (128K)
//
extern char DebuggerBuffer[DebuggerBufferSize];
extern uint32_t CurrentDebugSpewPos;		// Current position in buffer
extern uint32_t CurrentDebugTopOfScreen;	// Current top of screen
//
// Alpha values to added to debugger window
//
extern uint32_t DebuggerAlpha;
//
// Mouse position
//
extern int32_t DBMouseX,DBMouseY;
extern uint32_t DBMouseMoved;
extern int32_t ExMouseX,ExMouseY;
extern uint32_t DBButtons;

extern uint32_t gEnableMulti;
extern uint32_t gForceMono;
extern uint32_t gForceFlat;
extern uint32_t gForceBlend;
extern uint32_t gForceGouraud;
extern uint32_t gForcePerspective;
extern uint32_t gForceNoPerspective;
extern uint32_t gForceSpecular;
extern uint32_t gForceNoSpecular;
extern uint32_t gForceAlpha;
extern uint32_t gShowVertexData;
extern uint32_t gForceNoFog;
extern uint32_t gForceNoTextures;
extern uint32_t gForceBiLinear;
extern uint32_t gForceTriLinear;
extern int32_t	 gForceMipBias;
extern uint32_t gForceNoFiltering;
extern uint32_t gForceNoDithering;
extern uint32_t gForceDithering;
extern uint32_t gForceChessTexture;
extern uint32_t gForceNoAlphaTest;
extern uint32_t gForceAlphaTest;
extern uint32_t ChessTexture;				// The chess texture
extern uint32_t MipColorTexture;			// The mipmap colored texture
extern uint32_t gForceAlphaBlending;
extern uint32_t gUseGameSpew;
extern uint32_t gControlsActive;					// Allow controls for the game during the debugger
extern uint32_t gGameLogicActive;					// Allow game logic to run when debugger up
extern uint32_t gStopSystem;						// Stop whole system in debugger
extern uint32_t gFrameGraph;
extern uint32_t gStopGameRendering;
extern uint32_t gStopRendering;					// Stop rendering
extern uint32_t gFreezeLogic;						// Stop rendering
extern uint32_t gShowLFControls;
extern uint8_t NoDebuggerStats;						// When 0 Stats and spews are active during GameOS functions like debugger
extern uint32_t gEnableRS;
extern uint32_t gTextureOverrun;
extern uint32_t gDisableLighting;
extern uint32_t gDisableNormalizing;
extern uint32_t gDisableHardwareTandL;
extern uint32_t gDisableVertexBlending;

//
// Debugger window variables
//
extern int32_t DbTopX,DbTopY;
extern int32_t DbMaxX,DbMaxY,DbMinX;

extern uint32_t TopStatistics;

//
// Debugger window constants
//
#define DbSizeX 489
#define DbSizeY 342


extern int32_t CurrentX,CurrentY;		// Current pixel position
extern int32_t DbChrX,DbChrY;			// Current character x,y
extern int32_t StartX,StartY;			// Current start of line



void __stdcall UpdateDebugger(void);
void __stdcall InitDebugger(void);
void __stdcall EndRenderMode(void);
void __stdcall DestroyDebugger(void);
void __stdcall SpewToDebugger( PSTR Message );
int32_t __stdcall GetMipmapUsed( uint32_t Handle, pgos_VERTEX Pickv1, pgos_VERTEX Pickv2, pgos_VERTEX Pickv3 );
void __stdcall ShowFrameGraphs(void);
void __stdcall UpdateDebugMouse(void);
void __stdcall UpdateDebugWindow(void);
void __stdcall InitTextDisplay(void);
void __stdcall DrawText( uint32_t Color, PSTR String );
void __stdcall DrawSquare( int32_t TopX, int32_t TopY, int32_t Width, int32_t Height, uint32_t Color );
void __stdcall DrawChr( char Chr );














