#pragma once
//==========================================================================//
// File:	 LocalizationManager.hpp										//
// Contents: Localizer routines												//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
		



struct hResource
{
	hResource*	pNext;
	
	HINSTANCE	hInstance;
	char		Name[256];
	ULONG		ReferenceCount;

} ;

struct gos_CaretInfo
{
	ULONG		fontX;
	ULONG		fontY;
	float		fontSize;
	HGOSFONT3D	fontHandle;
	ULONG		fontColorComp;
	ULONG		fontColorCand;
	bool		fontBold;
	bool		fontItalic;
	bool		fontProportional;
	RECT		margins ;
} ;

void TextDrawDBCS(  PSTR Message );

void InitLocalization();
void DestroyLocalization();

void LocalizationSetPurgeFlags();
void LocalizationPurge();
void LocalizationShutdown();
void RenderIME();

extern ULONG gEnableLoc;
