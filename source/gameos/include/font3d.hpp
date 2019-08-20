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
	uint32_t dwWidth;
	uint32_t dwFontHeight;
	uint32_t dwHeight;
	uint32_t dwX[256];
	uint32_t dwY[256];
	uint32_t dwWidths[256];
	int32_t nA[256];
	int32_t nC[256];
	uint8_t bPixels[0];
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
	bool bItalic; // italic on or off when this font was created
	int32_t iWeight; // weight of font created
	int32_t iTextureCount;
	uint32_t dwFontHeight; // height in pixels of the resulatant font
	uint8_t bTexture[256]; // indicate which texture each character is on
	uint8_t bX[256]; // indicates x position of each character on it's texture
	uint8_t bY[256]; // indicates y position of each character on it's texture
	uint8_t bW[256]; // indicates width of each character in the texture
	signed char cA[256]; // indicates amount of space to add before each
		// character (may be -)
	signed char cC[256]; // indicates amount of space to add after each
		// character (may be -)
} D3DFontData1;

typedef struct
{
	uint32_t dwSize;
	uint8_t bPixels[0];
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
	uint32_t TextureSize; // Width and Height of texture
	float rhSize; // 1.0 / Size
	uint32_t TexturePitch; // Pitch of texture
	int32_t Width; // Width of font grid
	int32_t Height; // Height of font grid
	uint32_t Across; // Number of characters across one line
	uint32_t Aliased; // True if 4444 texture (may be aliased - else 1555 keyed)
	uint32_t FromTextureHandle; // True is from a texture handle
	uint8_t BlankPixels[256 - 32]; // Empty pixels before character
	uint8_t UsedPixels[256 - 32]; // Width of character
	uint8_t TopU[256 - 32];
	uint8_t TopV[256 - 32]; // Position of character
	uint8_t TopOffset[256 - 32]; // Offset from top (number of blank lines)
	uint8_t RealHeight[256 - 32]; // Height of character
	uint8_t TextureHandle[256 - 32]; // Which texture handle to use
	uint32_t NumberOfTextures; // Number of texture handles used (normally 1)
	HFONT hFontTTF; // handle to a GDI font
	uint32_t Texture[8]; // Texture handle array
} FontInfo;

void __stdcall Init3DFont(void);
void __stdcall Destroy3DFont(void);
