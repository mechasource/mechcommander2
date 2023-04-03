#pragma once
//==========================================================================//
// File:	 LocalizationManager.hpp										//
// Contents: Localizer routines												//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

struct hResource
{
	hResource* pNext;

	HINSTANCE hinstance;
	char Name[256];
	uint32_t ReferenceCount;
};

struct gos_CaretInfo
{
	uint32_t fontX;
	uint32_t fontY;
	float fontSize;
	HGOSFONT3D fontHandle;
	uint32_t fontcolourComp;
	uint32_t fontcolourCand;
	BOOLEAN fontBold;
	BOOLEAN fontItalic;
	BOOLEAN fontProportional;
	RECT margins;
};

void __stdcall TextDrawDBCS(PSTR message);

void InitLocalization(void);
void DestroyLocalization(void);

void LocalizationSetPurgeFlags(void);
void LocalizationPurge(void);
void LocalizationShutdown(void);
void RenderIME(void);

extern uint32_t gEnableLoc;
