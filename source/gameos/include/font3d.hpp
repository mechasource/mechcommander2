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

typedef struct D3DFontData
{
	uint32_t dwSig; // 0x46443344
	uint32_t width;
	uint32_t dwFontheight;
	uint32_t height;
	uint32_t dwX[256];
	uint32_t dwY[256];
	uint32_t widths[256];
	int32_t nA[256];
	int32_t nC[256];
	BOOLEAN bPixels[0];
} D3DFontData;

#pragma pack(push, 1)

//
// FontEdit 4.0+
//
typedef struct D3DFontData1
{
	uint32_t dwSig; // 0x46443344
	char szFaceName[64];
	int32_t iSize; // point size of font used to create this font
	BOOLEAN bItalic; // italic on or off when this font was created
	int32_t iWeight; // weight of font created
	int32_t iTextureCount;
	uint32_t dwFontheight; // height in pixels of the resulatant font
	BOOLEAN bTexture[256]; // indicate which texture each character is on
	BOOLEAN bX[256]; // indicates x position of each character on it's texture
	BOOLEAN bY[256]; // indicates y position of each character on it's texture
	BOOLEAN bW[256]; // indicates width of each character in the texture
	signed char cA[256]; // indicates amount of space to add before each
		// character (may be -)
	signed char cC[256]; // indicates amount of space to add after each
		// character (may be -)
} D3DFontData1;

typedef struct
{
	uint32_t size;
	BOOLEAN bPixels[0];
} D3DFontTexture;

#pragma pack(pop)
//#pragma warning( pop )

typedef struct _FontInfo
{
	uint32_t MagicNumber; // Valid font check
	struct _FontInfo* pNext; // Pointer to next font
	uint32_t ReferenceCount; // Reference count
	char FontFile[MAX_PATH]; // Path name of font texture
	uint32_t StartLine; // texture line where font starts
	int32_t CharCount; // number of chars in font (valid range 33 to 256)
	uint32_t TextureSize; // width and height of texture
	float rhSize; // 1.0 / Size
	uint32_t TexturePitch; // Pitch of texture
	int32_t width; // width of font grid
	int32_t height; // height of font grid
	uint32_t Across; // Number of characters across one line
	uint32_t Aliased; // True if 4444 texture (may be aliased - else 1555 keyed)
	uint32_t FromTextureHandle; // True is from a texture handle
	BOOLEAN BlankPixels[256 - 32]; // Empty pixels before character
	BOOLEAN UsedPixels[256 - 32]; // width of character
	BOOLEAN TopU[256 - 32];
	BOOLEAN TopV[256 - 32]; // Position of character
	BOOLEAN TopOffset[256 - 32]; // Offset from top (number of blank lines)
	BOOLEAN Realheight[256 - 32]; // height of character
	BOOLEAN TextureHandle[256 - 32]; // Which texture handle to use
	uint32_t NumberOfTextures; // Number of texture handles used (normally 1)
	HFONT hFontTTF; // handle to a GDI font
	uint32_t Texture[8]; // Texture handle array
} FontInfo;

void __stdcall Init3DFont(void);
void __stdcall Destroy3DFont(void);
