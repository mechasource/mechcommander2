//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
// #include <stdio.h>
// #include <string.h>

#ifndef FILE_H
#include "file.h"
#endif

#include "vfx.h"
#include <gameos.hpp>

uint8_t
FindClosest(VFX_RGB* Palette, int32_t r, int32_t g, int32_t b);

//
//
// Number of colors listed in the AlphaPal.ini file (>255 are special colors)
//
//
#define NUM_ALPHACOLORS 280 // (One+last color listed)

//
// Table used for alpha effects
//  Low byte  = Background
//  High byte = Sprite
//
char AlphaTable[NUM_ALPHACOLORS * 256];
//
// Flag set to 1 when an alpha pixel is used
//
char SpecialColor[NUM_ALPHACOLORS];

enum
{
	R,
	G,
	B,
	SourceAlpha,
	DestAlpha
};

PSTR g_logistic_fadetable = &AlphaTable[256 * 256];
PSTR g_logistic_bluefade = &AlphaTable[267 * 256];
PSTR g_logistic_dlgfade = &AlphaTable[268 * 256];

void
InitAlphaLookup(VFX_RGB* Palette)
{
	PSTR pAlphaTable = AlphaTable;
	int32_t r, g, b, i;
	MechFile* IniFile;
	char Line[256];
	float AlphaIni[NUM_ALPHACOLORS][5];
	int32_t LineNumber;
	//
	// Clear the AlphaPal.ini array
	//
	memset(AlphaIni, 0, sizeof(AlphaIni));
	memset(SpecialColor, 0, sizeof(SpecialColor));
	//
	// Read in and parse the AlphaPal.ini file containing all the color
	// information
	//
	IniFile = new MechFile;
#ifdef _DEBUG
	int32_t AlphapalOpenResult =
#endif
		IniFile->open("AlphaPal.ini");
	gosASSERT(AlphapalOpenResult == NO_ERROR);
	LineNumber = 0;
	while (!IniFile->eof())
	{
		IniFile->readLine((puint8_t)&Line[0], 256);
		LineNumber++;
		if (Line[0] != '#' && Line[0] != ';' && Line[0] != 0xa && Line[0] != 0)
		{
			if (EOF == sscanf_s(Line, "%d", &i))
				break;
			gosASSERT(i >= 0 && i < NUM_ALPHACOLORS);
			SpecialColor[i] = 1;
			gosASSERT(EOF != sscanf_s(Line, "%f %f %f %f %f", &AlphaIni[i][R], &AlphaIni[i][G], &AlphaIni[i][B], &AlphaIni[i][SourceAlpha], &AlphaIni[i][DestAlpha]));
			gosASSERT((AlphaIni[i][SourceAlpha] != 0.0 || AlphaIni[i][DestAlpha] != 0.0) && (AlphaIni[i][R] != 255 && AlphaIni[i][G] != 255 && AlphaIni[i][B] != 255));
		}
	}
	IniFile->close();
	delete IniFile;
	IniFile = nullptr;
	//
	// Now generate the 64K alpha value lookup table
	//
	for (size_t source = 0; source < NUM_ALPHACOLORS; source++)
	{
		for (size_t dest = 0; dest < 256; dest++)
		{
			if (source == 255 || source == 0) // Color 255 and 0 - dest color
				// remains the same (totally
				// transparent)
			{
				*pAlphaTable++ = dest;
			}
			else if (SpecialColor[source] == 0) // If not specified, make a solid color
			{
				*pAlphaTable++ = source;
			}
			else if (dest < 10 || dest > 245)
			{
				*pAlphaTable++ = char(-1); // Is dest is ever a bad pixel, make white
			}
			else
			{
				if (AlphaIni[source][SourceAlpha] == 0.0 && AlphaIni[source][DestAlpha] == 0.0)
				{
					r = (int32_t)(
						((float)(Palette[dest].r << 2) * 255) / (255 - AlphaIni[source][R]));
					g = (int32_t)(
						((float)(Palette[dest].g << 2) * 255) / (255 - AlphaIni[source][G]));
					b = (int32_t)(
						((float)(Palette[dest].b << 2) * 255) / (255 - AlphaIni[source][B]));
				}
				else
				{
					r = (int32_t)(AlphaIni[source][SourceAlpha] * AlphaIni[source][R] + AlphaIni[source][DestAlpha] * (float)(Palette[dest].r << 2));
					g = (int32_t)(AlphaIni[source][SourceAlpha] * AlphaIni[source][G] + AlphaIni[source][DestAlpha] * (float)(Palette[dest].g << 2));
					b = (int32_t)(AlphaIni[source][SourceAlpha] * AlphaIni[source][B] + AlphaIni[source][DestAlpha] * (float)(Palette[dest].b << 2));
				}
				if (r < 0)
					r = 0;
				else if (r > 255)
					r = 255;
				if (g < 0)
					g = 0;
				else if (g > 255)
					g = 255;
				if (b < 0)
					b = 0;
				else if (b > 255)
					b = 255;
				*pAlphaTable++ = FindClosest(Palette, r >> 2, g >> 2, b >> 2);
			}
		}
	}
}

//
//
// 51% Green
// 39% Red
// 10% Blue
//
// Returns the closest matching color in a palette, dose not check windows
// colors
//
uint8_t
FindClosest(VFX_RGB* Palette, int32_t r, int32_t g, int32_t b)
{
	uint8_t Closest = 10;
	int32_t Distance = 255 * 255 * 255;
	int32_t tempR, tempG, tempB, tdist;
	for (size_t t1 = 10; t1 < 246; t1++)
	{
		tempR = r - Palette[t1].r;
		tempG = g - Palette[t1].g;
		tempB = b - Palette[t1].b;
		tdist = (39 * tempR * tempR) + (51 * tempG * tempG) + (10 * tempB * tempB);
		if (Distance > tdist)
		{
			Distance = tdist;
			Closest = t1;
			if (tdist == 0)
				break;
		}
	}
	return Closest;
}
