#pragma once
//==========================================================================//
// File:	 Font3D.hpp														//
// Contents: Font3D drawing routines										//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//
// Support for .d3f fonts files
//
//#pragma warning( push )
//#pragma warning( disable : 4200 ) 
//
// Original FontEdit program
//
typedef struct {
	ULONG dwSig;				// 0x46443344
	ULONG dwWidth;
	ULONG dwFontHeight;
	ULONG dwHeight;
	ULONG dwX[256];
	ULONG dwY[256];
	ULONG dwWidths[256];
	int nA[256];
	int nC[256];
	UCHAR bPixels[0];
} D3DFontData;

#pragma pack(push,1)

//
// FontEdit 4.0+
//
typedef struct 
{
	ULONG dwSig;				// 0x46443344
	char szFaceName[64];
	int iSize;					// point size of font used to create this font
	bool bItalic;				// italic on or off when this font was created
	int iWeight;				// weight of font created
	int iTextureCount; 
	ULONG dwFontHeight;			// height in pixels of the resulatant font
	UCHAR bTexture[256];			// indicate which texture each character is on
	UCHAR bX[256];				// indicates x position of each character on it's texture
	UCHAR bY[256];				// indicates y position of each character on it's texture
	UCHAR bW[256];				// indicates width of each character in the texture
	signed char cA[256];		// indicates amount of space to add before each character (may be -)
	signed char cC[256];		// indicates amount of space to add after each character (may be -)
} D3DFontData1;

typedef struct
{
	ULONG dwSize;
	UCHAR bPixels[0];
} D3DFontTexture;

#pragma pack( pop )
//#pragma warning( pop )




typedef struct _FontInfo
{
	ULONG		MagicNumber;			// Valid font check
	_FontInfo*	pNext;					// Pointer to next font
	ULONG		ReferenceCount;			// Reference count
	char		FontFile[MAX_PATH];		// Path name of font texture
	ULONG		StartLine;				// texture line where font starts
	int			CharCount;				// number of chars in font (valid range 33 to 256)
	ULONG		TextureSize;			// Width and Height of texture
	float		rhSize;					// 1.0 / Size
	ULONG		TexturePitch;			// Pitch of texture
	int			Width;					// Width of font grid
	int			Height;					// Height of font grid
	ULONG		Across;					// Number of characters across one line
	ULONG		Aliased;				// True if 4444 texture (may be aliased - else 1555 keyed)
	ULONG		FromTextureHandle;		// True is from a texture handle
	UCHAR		BlankPixels[256-32];	// Empty pixels before character
	UCHAR		UsedPixels[256-32];		// Width of character
	UCHAR		TopU[256-32];
	UCHAR		TopV[256-32];			// Position of character
	UCHAR		TopOffset[256-32];		// Offset from top (number of blank lines)
	UCHAR		RealHeight[256-32];		// Height of character
	UCHAR		TextureHandle[256-32];	// Which texture handle to use
	ULONG		NumberOfTextures;		// Number of texture handles used (normally 1)
	HFONT		hFontTTF;				// handle to a GDI font
	ULONG		Texture[8];				// Texture handle array
} FontInfo;

void __stdcall Init3DFont();
void __stdcall Destroy3DFont();









