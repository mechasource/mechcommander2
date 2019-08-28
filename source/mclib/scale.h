//---------------------------------------------------------------------------//
// scale.h - This file contains definitions of the scaleFactor for scaled draws
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef SCALE_H
#define SCALE_H
//---------------------------------------------------------------------------
// Include files

int32_t
scaleDraw(uint8_t* shapeTable, uint32_t frameNum, int32_t sx, int32_t sy, bool reverse,
	uint8_t* fadeTable = nullptr, bool scaleUp = 0);
//---------------------------------------------------------------------------
#endif