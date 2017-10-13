//---------------------------------------------------------------------------
//
// MechCommander 2
//
// TGA file Specifics
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdafx.h"
//#include <string.h>

#ifndef TGAINFO_H
#include "tgainfo.h"
#endif

#include <estring.h>

#include <gameos.hpp>
//---------------------------------------------------------------------------
typedef struct _RGB
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} RGB;

//---------------------------------------------------------------------------
void tgaDecomp(puint8_t dest, puint8_t source, TGAFileHeader* tga_header)
{
	//---------------------------------------------------------------------
	// Display Image based on Format 10 (0x0a).  RLE true color.
	//----------------------------------------------
	// Parse RLE data -- Check for RLE/RAW byte
	puint8_t data_offset  = source;
	int32_t currentHeight = 0;
	int32_t currentWidth  = 0;
	while (currentHeight != tga_header->height)
	{
		while (currentWidth != tga_header->width)
		{
			byte rep_count = *data_offset;
			data_offset++;
			RGB rgb;
			//----------------------------------------------------------
			// Only an RLE packet if MSbit is 1
			if (rep_count >= 128)
			{
				rep_count ^= 0x80;
				rgb.b = *data_offset;
				data_offset++;
				rgb.g = *data_offset;
				data_offset++;
				rgb.r = *data_offset;
				data_offset++;
				//-------------------------
				// One more for Alpha Data
				// Use later for aliasing
				byte alpha = 0;
				if (tga_header->pixel_depth == 32)
				{
					alpha = *data_offset;
					data_offset++;
				}
				for (size_t i = 0; i < rep_count + 1; i++)
				{
					*dest = rgb.b;
					dest++;
					*dest = rgb.g;
					dest++;
					*dest = rgb.r;
					dest++;
					if (tga_header->pixel_depth == 32)
					{
						*dest = alpha;
						dest++;
					}
					else
					{
						//---------------------
						// Assume a color key.
						// r 0xff, g 0x00, b 0xff is alpha 0 (Hot Pink)
						// all else is alpha 0xff
						if ((rgb.r == 0xff) && (rgb.g == 0x0) &&
							(rgb.b == 0xff))
							*dest = 0x0;
						else
							*dest = 0xff;
						dest++;
					}
					currentWidth++;
				}
			}
			else
			{
				for (size_t i = 0; i < rep_count + 1; i++)
				{
					rgb.b = *data_offset;
					data_offset++;
					rgb.g = *data_offset;
					data_offset++;
					rgb.r = *data_offset;
					data_offset++;
					//-------------------------
					// One more for Alpha Data
					// Use later for aliasing
					byte alpha = 0;
					if (tga_header->pixel_depth == 32)
					{
						alpha = *data_offset;
						data_offset++;
					}
					*dest = rgb.b;
					dest++;
					*dest = rgb.g;
					dest++;
					*dest = rgb.r;
					dest++;
					if (tga_header->pixel_depth == 32)
					{
						*dest = alpha;
						dest++;
					}
					else
					{
						//---------------------
						// Assume a color key.
						// r 0xff, g 0x00, b 0xff is alpha 0 (Hot Pink)
						// all else is alpha 0xff
						if ((rgb.r == 0xff) && (rgb.g == 0x0) &&
							(rgb.b == 0xff))
							*dest = 0x0;
						else
							*dest = 0xff;
						dest++;
					}
					currentWidth++;
				}
			}
		}
		currentWidth = 0;
		currentHeight++;
	}
}

//---------------------------------------------------------------------------
void flipTopToBottom(
	puint8_t buffer, uint8_t depth, int32_t width, int32_t height)
{
	//-----------------------------------------------------------
	// ScanLine by Scanline
	puint8_t tmpBuffer  = (puint8_t)malloc(width * height * (depth >> 3));
	puint8_t tmpPointer = tmpBuffer;
	//----------------------------------
	// Point to last scanline.
	int32_t pixelWidth  = width * (depth >> 3);
	puint8_t workBuffer = buffer + ((height - 1) * pixelWidth);
	for (size_t i = 0; i < height; i++)
	{
		memcpy(tmpBuffer, workBuffer, pixelWidth);
		workBuffer -= pixelWidth;
		tmpBuffer += pixelWidth;
	}
	memcpy(buffer, tmpPointer, width * height * (depth >> 3));
	free(tmpPointer);
}

//---------------------------------------------------------------------------
void tgaCopy(puint8_t dest, puint8_t src, int32_t size)
{
	int32_t numCopied = 0;
	while (numCopied != size)
	{
		*dest = *src;
		dest++;
		src++;
		*dest = *src;
		dest++;
		src++;
		*dest = *src;
		dest++;
		src++;
		*dest = 0xff;
		dest++;
		numCopied++;
	}
}

//---------------------------------------------------------------------------
static cint32_t g_textureCache_BufferSize = 16384 /*64*64*sizeof(uint32_t)*/;
static uint8_t g_textureCache_Buffer[g_textureCache_BufferSize];
EString* g_textureCache_FilenameOfLastLoadedTexture =
	nullptr; /*This is an (EString *) instead of an EString because apparently
				gos memory management has a problem with global static
				allocation of EStrings.*/
static int32_t g_textureCache_WidthOfLastLoadedTexture  = 0; /*just to be sure*/
static int32_t g_textureCache_HeightOfLastLoadedTexture = 0; /*just to be sure*/
static int32_t g_textureCache_NumberOfConsecutiveLoads  = 0;
static bool g_textureCache_LastTextureIsCached			= false;

void loadTGATexture(
	FilePtr tgaFile, puint8_t ourRAM, int32_t width, int32_t height)
{
	if (!g_textureCache_FilenameOfLastLoadedTexture)
		g_textureCache_FilenameOfLastLoadedTexture = new EString;
	if (width * height * sizeof(uint32_t) <= g_textureCache_BufferSize)
	{
		if ((g_textureCache_FilenameOfLastLoadedTexture->Data()) &&
			(0 == strcmp(tgaFile->getFilename(),
					  g_textureCache_FilenameOfLastLoadedTexture->Data())) &&
			((cint32_t)width == g_textureCache_WidthOfLastLoadedTexture) &&
			((cint32_t)height == g_textureCache_HeightOfLastLoadedTexture))
		{
			if (g_textureCache_LastTextureIsCached)
			{
				g_textureCache_NumberOfConsecutiveLoads += 1;
				memcpy(ourRAM, g_textureCache_Buffer,
					width * height * sizeof(uint32_t));
				return;
			}
		}
	}
	puint8_t tgaBuffer = (puint8_t)malloc(tgaFile->fileSize());
	tgaFile->read(tgaBuffer, tgaFile->fileSize());
	//---------------------------------------
	// Parse out TGAHeader.
	TGAFileHeader* header = (TGAFileHeader*)tgaBuffer;
	gosASSERT(header->width == width);
	gosASSERT(header->height == height);
	switch (header->image_type)
	{
	case UNC_PAL:
	{
		//------------------------------------------------
		// If palette, use entries to create 24Bit image.
	}
	break;
	case UNC_TRUE:
	{
		//------------------------------------------------
		// This is just a bitmap.  Copy it into ourRAM.
		puint8_t image = tgaBuffer + sizeof(TGAFileHeader);
		if (header->pixel_depth == 32)
			memcpy(ourRAM, image, width * height * 4);
		else
			tgaCopy(ourRAM, image, width * height);
		//------------------------------------------------------------------------
		// Must check image_descriptor to see if we need to un upside down
		// image.
		bool left = (header->image_descriptor & 16) != 0;
		bool top  = (header->image_descriptor & 32) != 0;
		if (!top && !left)
		{
			//--------------------------------
			// Image is Upside down.
			flipTopToBottom(ourRAM, 32, width, height);
		}
		else if (!top && left)
		{
			// flipTopToBottom(ourRAM,header->pixel_depth,width,height);
			// flipRightToLeft(ourRAM,header->pixel_depth,width,height);
		}
		else if (top && left)
		{
			// flipRightToLeft(ourRAM,header->pixel_depth,width,height);
		}
	}
	break;
	case RLE_PAL:
	{
		//------------------------------------------------
		// If palette, use entries to create 24Bit image.
	}
	break;
	case RLE_TRUE:
	{
		puint8_t image = tgaBuffer + sizeof(TGAFileHeader);
		tgaDecomp(ourRAM, image, header);
		//------------------------------------------------------------------------
		// Must check image_descriptor to see if we need to un upside down
		// image.
		bool left = (header->image_descriptor & 16) != 0;
		bool top  = (header->image_descriptor & 32) != 0;
		if (!top && !left)
		{
			//--------------------------------
			// Image is Upside down.
			flipTopToBottom(ourRAM, 32, width, height);
		}
		else if (!top && left)
		{
			// flipTopToBottom(ourRAM,header->pixel_depth,width,height);
			// flipRightToLeft(ourRAM,header->pixel_depth,width,height);
		}
		else if (top && left)
		{
			// flipRightToLeft(ourRAM,header->pixel_depth,width,height);
		}
	}
	break;
	}
	free(tgaBuffer);
	if (width * height * sizeof(uint32_t) <= g_textureCache_BufferSize)
	{
		if ((g_textureCache_FilenameOfLastLoadedTexture->Data()) &&
			(0 == strcmp(tgaFile->getFilename(),
					  g_textureCache_FilenameOfLastLoadedTexture->Data())) &&
			((cint32_t)width == g_textureCache_WidthOfLastLoadedTexture) &&
			((cint32_t)height == g_textureCache_HeightOfLastLoadedTexture))
		{
			g_textureCache_NumberOfConsecutiveLoads += 1;
			if (2 == g_textureCache_NumberOfConsecutiveLoads)
			{
				memcpy(g_textureCache_Buffer, ourRAM,
					width * height * sizeof(uint32_t));
				(*g_textureCache_FilenameOfLastLoadedTexture) =
					tgaFile->getFilename();
				g_textureCache_WidthOfLastLoadedTexture  = width;
				g_textureCache_HeightOfLastLoadedTexture = height;
				g_textureCache_LastTextureIsCached		 = true;
			}
		}
		else
		{
			(*g_textureCache_FilenameOfLastLoadedTexture) =
				tgaFile->getFilename();
			g_textureCache_WidthOfLastLoadedTexture  = width;
			g_textureCache_HeightOfLastLoadedTexture = height;
			g_textureCache_NumberOfConsecutiveLoads  = 1;
			g_textureCache_LastTextureIsCached		 = false;
		}
	}
}

void loadTGAMask(
	FilePtr tgaFile, puint8_t ourRAM, int32_t width, int32_t height)
{
	puint8_t tgaBuffer = (puint8_t)malloc(tgaFile->fileSize());
	tgaFile->read(tgaBuffer, tgaFile->fileSize());
	//---------------------------------------
	// Parse out TGAHeader.
	TGAFileHeader* header = (TGAFileHeader*)tgaBuffer;
	gosASSERT(header->width == width);
	gosASSERT(header->height == height);
	gosASSERT(header->image_type != UNC_TRUE);
	gosASSERT(header->image_type != RLE_TRUE);
	gosASSERT(header->image_type != RLE_PAL);
	switch (header->image_type)
	{
	case UNC_PAL:
	case UNC_GRAY:
	{
		//------------------------------------------------
		// This is just a bitmap.  Copy it into ourRAM.
		puint8_t image = tgaBuffer + sizeof(TGAFileHeader);
		if (header->color_map)
			image += header->cm_length * (header->cm_entry_size >> 3);
		memcpy(ourRAM, image, width * height);
		//------------------------------------------------------------------------
		// Must check image_descriptor to see if we need to un upside down
		// image.
		bool left = (header->image_descriptor & 16) != 0;
		bool top  = (header->image_descriptor & 32) != 0;
		if (!top && !left)
		{
			//--------------------------------
			// Image is Upside down.
			flipTopToBottom(ourRAM, header->pixel_depth, width, height);
		}
		else if (!top && left)
		{
			flipTopToBottom(ourRAM, header->pixel_depth, width, height);
			// flipRightToLeft(ourRAM,header->pixel_depth,width,height);
		}
		else if (top && left)
		{
			// flipRightToLeft(ourRAM,header->pixel_depth,width,height);
		}
	}
	break;
	case RLE_PAL:
	{
	}
	break;
	}
	free(tgaBuffer);
}

//---------------------------------------------------------------------------
