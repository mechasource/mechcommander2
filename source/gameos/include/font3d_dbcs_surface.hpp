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

		int m_height;
		int m_width;
		int m_offsety;
		LPDIRECTDRAWSURFACE7 m_lpdds7;
		puint8_t m_lpDynData;

		DBCSSurface(ULONG width, ULONG height);
		~DBCSSurface();
		PVOID GetDataPointer(void);
		void Clear(ULONG color);
};

