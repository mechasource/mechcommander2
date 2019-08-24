#pragma once
//===========================================================================//
// File:	 Font3D_DBCS_Storage.hpp										 //
// Contents: Representation of DBCS in 3D environment						 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "Font3D_DBCS_Surface.hpp"

// right hand
typedef struct _quad
{
	gos_VERTEX v[4];
	_quad* pNext;
	uint32_t texture;
} quad;

typedef struct _paneTextures
{
	_paneTextures* pNext;
	uint32_t Handle;
	uint32_t Size;
} paneTextures;

class gos_DBCS
{
protected:
	int32_t m_RefCount; // used for whether to always cache

public:
	uint32_t m_height, m_width, m_latticeX, m_latticeY, m_color;

	quad* m_pListOfQuads;

	DBCSSurface* m_surf;

	paneTextures* m_pTextures;

	gos_DBCS* m_pNext;
	gos_DBCS* m_pPrev;
	float m_FontX;
	float m_FontY;
	int32_t m_beginFontX;
	int32_t m_beginFontY;
	float m_doneFontX;
	float m_doneFontY;
	float m_LastFontX;
	float m_LastFontY;
	char m_msg[8192];
	bool m_bRendered;
	bool embeddedColorChange;
	float m_FontSize;
	bool m_FontBold;
	uint32_t m_FontColor;

public:
	gos_DBCS(PSTR msg, int32_t w, int32_t h, bool cc);
	~gos_DBCS(void);
	void Render(void);
	void Update(void);
	DBCSSurface* GetSurface() { return m_surf; }
	int32_t GetWidth() { return m_width; }
	int32_t GetHeight() { return m_height; }
	void PrepareTextures(void);
	void Texture(DBCSSurface* surf);
	void Translate(float x, float y, float z);
	void Color(uint32_t c);
	void AlphaMode(enum gos_AlphaMode mode);
	void MakeQuads(void);
	bool NeedClip(float x, float y);
	void ShiftCoordinates(float x, float y);
	void Fill(float r, float g, float b, float a);

	uint32_t AddTexture(uint32_t Size);
	void AddQuad(uint32_t X, uint32_t Y, uint32_t Width, uint32_t Height, uint32_t Texture,
		uint32_t U, uint32_t V, uint32_t TextureSize);

	void AddRef(void) { m_RefCount++; }
	void Release(void)
	{
		m_RefCount--;
		if (m_RefCount < 0)
			m_RefCount = 0;
	}
	int32_t RefCount(void) { return m_RefCount; }
};
