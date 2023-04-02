/*******************************************************************************
 This file consist of reversed and interpreted code from object source in the
 x86 debug build gameos.lib. The code in this file can not be used in any way
 other than serve as an information reference to the original released source of
 Mechcommander2. The code is a work of progress and there is no guarantee it is
 complete, accurate or useful in any way. The purpose is instead to make it
 possible to safely remove any dependencies of gameos.lib from Mechcommander2.
 All code is logically copyrighted to Microsoft
*******************************************************************************/
/*******************************************************************************
 font3d.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

#include "stdinc.h"

// -----------------------------------------------------------------------------
// Global data exported from this module

// global referenced data not listed in headers
int FontX;
int FontY;
float FontMaxX;
float FontMaxY;
gos_VERTEX FontVertices;
float FontMinX;
float FontMinY;
uint32_t FontIndex;

// local data

float flt_30; // weak
float flt_34; // weak
int dword_40; // weak
float flt_50; // weak
float flt_54; // weak
int dword_60; // weak
float flt_70; // weak
float flt_74; // weak
int dword_80[996]; // weak

uint32_t FontClipping;
float TexOffset; // weak
BOOLEAN FontNoCache; // weak
BOOLEAN FontStartOfLine; // weak
BOOLEAN OddSize; // weak
BOOLEAN FontWordWrap; // weak
struct _FontInfo* Fonts;
struct _FontInfo* FontHandle;
uint32_t Fontcolour; // weak
float FontSize; // weak
uint32_t FontWrapType; // weak
BOOLEAN FontProportional; // weak
BOOLEAN FontBold; // weak
BOOLEAN FontItalic; // weak
BOOLEAN FontDisableCodes; // weak
uint32_t MarginBuffer; // weak
int Largestheight; // idb
float MagicOffset = 0.4375; // weak
int32_t CurrentFontTexture = 4294967295; // idb
float FontScale = 1.0; // weak
char `gos_TextDrawV '::`4' ::textBufferArray[8191]; // idb
char byte_68F3; // weak

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers
void __stdcall gos_TextSetAttributes(HGOSFONT3D FontHandle, uint32_t Foreground, float Size,
	BOOLEAN WordWrap, BOOLEAN Proportional, BOOLEAN Bold, BOOLEAN Italic, uint32_t WrapType,
	BOOLEAN DisableEmbeddedCodes);
void __stdcall gos_TextSetPosition(int32_t XPosition, int32_t YPosition);
void __stdcall gos_TextGetPrintPosition(int32_t* pXPosition, int32_t* pYPosition);
void __stdcall gos_TextSetRegion(int32_t Left, int32_t Top, int32_t Right, int32_t Bottom);
void __stdcall gos_TextStringLength(uint32_t* pwidth, uint32_t* pheight, PSTR message, ...);
void __stdcall gos_TextDrawBackground(
	int32_t Left, int32_t Top, int32_t Right, int32_t Bottom, uint32_t colour);
void __stdcall SetupFontRenderstates(void);
void __stdcall gos_TextDraw(PSTR message, ...);
void __stdcall gos_TextDrawV(PSTR message, PSTR arglist);

// global implemented functions not listed in headers
void __stdcall FontDrawQuad(BOOLEAN);
void __stdcall GetChrSize(int32_t*, int32_t*, char);
size_t __stdcall IgnoreEmbedded(BOOLEAN*);
int32_t __stdcall GetNextWordLen(BOOLEAN*);
int32_t __stdcall GetNextLineLen(BOOLEAN*, BOOLEAN**);
void __stdcall gos_ChrDraw(char c);
void __stdcall HandleTags(char**);

// local functions

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers
