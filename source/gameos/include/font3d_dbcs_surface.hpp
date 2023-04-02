//===========================================================================//
// File:	 Surface.hpp													 //
// Contents: DirectDraw Surface object										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

class DBCSSurface
{
public:
	int32_t m_height;
	int32_t m_width;
	int32_t m_offsety;
	LPDIRECTDRAWSURFACE7 m_lpdds7;
	BOOLEAN* m_lpDynData;

	DBCSSurface(uint32_t width, uint32_t height);
	~DBCSSurface(void);
	PVOID GetDataPointer(void);
	void Clear(uint32_t color);
};
