//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"

#include <mlr/gosimage.hpp>

using namespace MidLevelRenderer;

//#############################################################################
//############################    GOSImage    ###############################
//#############################################################################

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
GOSImage::GOSImage( PCSTR iName ) : Plug (DefaultData)
{
	imageName = iName;

	flags = 0;

	instance = 0;

	mcTextureNodeIndex = 0xffffffff;

	ptr.pTexture = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
GOSImage::GOSImage( uint32_t iHandle ) : Plug (DefaultData)
{
	char str[20];

	sprintf(str, "image%03d", iHandle);

	imageName = str;

	flags = Loaded;

	instance = 0;

	mcTextureNodeIndex = iHandle;

	ptr.pTexture = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
GOSImage::GOSImage(PCSTR name, gos_TextureHints hints) : Plug (DefaultData)
{
	imageName = name;

	flags = Loaded;

	instance = 0;

	ipHints = hints;

	mcTextureNodeIndex = mcTextureManager->loadTexture(name,gos_Texture_Detect,ipHints);

	ptr.pTexture = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
GOSImage::~GOSImage(void)
{
	imageName = "";

	if((flags & Locked) != 0)
	{
//		gos_UnLockTexture(imageHandle);
	}

	if((flags & Loaded) != 0)
	{
		if (mcTextureManager)
		{
			mcTextureManager->removeTexture(mcTextureNodeIndex);
			mcTextureNodeIndex = 0xffffffff;
		}
	}
	flags = 0;
}

#if _CONSIDERED_OBSOLETE

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t GOSImage::GetWidth()
{
	return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t GOSImage::GetHeight()
{
	return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void GOSImage::LockImage()
{
	if(!(flags & Locked))
	{
		flags |= Locked;
		uint32_t imageHandle = mcTextureManager->get_gosTextureHandle(mcTextureNodeIndex);

		if (imageHandle != 0xffffffff)
			gos_LockTexture(imageHandle, 0, false, &ptr);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void GOSImage::UnlockImage()
{
	if(flags & Locked)
	{
		flags &= ~Locked;
		Start_Timer(Unlock_Texture_Time);
		uint32_t imageHandle = mcTextureManager->get_gosTextureHandle(mcTextureNodeIndex);

		if (imageHandle != 0xffffffff)
			gos_UnLockTexture(imageHandle);

		Stop_Timer(Unlock_Texture_Time);

		ptr.pTexture = nullptr;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
puint8_t
	GOSImage::GetImagePtr()
{
	return (puint8_t )ptr.pTexture;
}

#endif
