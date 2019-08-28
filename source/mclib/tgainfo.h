//---------------------------------------------------------------------------
//
// MechCommander 2 -- Copyright (c) 1998 FASA Interactive
//
// TGA file Specifics
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef TGAINFO_H
#define TGAINFO_H

#ifndef DSTD_H
#include "dstd.h"
#endif

#ifndef FILE_H
#include "file.h"
#endif

//---------------------------------------------------------------------------
enum TGAImageType
{
	NO_IMAGE = 0,
	UNC_PAL,
	UNC_TRUE,
	UNC_GRAY,
	RLE_PAL = 9,
	RLE_TRUE,
	RLE_GRAY
};

//---------------------------------------------------------------------------
#pragma pack(1)
typedef struct TGAFileHeader
{
	int8_t image_id_len;
	int8_t color_map;
	int8_t image_type;

	int16_t cm_first_entry;
	int16_t cm_length;
	int8_t cm_entry_size;

	int16_t x_origin;
	int16_t y_origin;
	int16_t width;
	int16_t height;
	int8_t pixel_depth;
	int8_t image_descriptor;
} TGAFileHeader;
#pragma pack()

void
tgaDecomp(uint8_t* dest, uint8_t* source, TGAFileHeader* tga_header);
void
loadTGATexture(std::unique_ptr<File> tgaFile, uint8_t* ourRAM, int32_t width, int32_t height);
void
loadTGAMask(std::unique_ptr<File> tgaFile, uint8_t* ourRAM, int32_t width, int32_t height);

void
flipTopToBottom(uint8_t* buffer, uint8_t depth, int32_t width, int32_t height);
//---------------------------------------------------------------------------
#endif
