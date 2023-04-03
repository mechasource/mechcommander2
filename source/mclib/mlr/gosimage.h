//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_GOSIMAGE_HPP
#define MLR_GOSIMAGE_HPP

#include "stuff/plug.h"
// #include "stuff/mstring.h"

namespace MidLevelRenderer
{

class GOSImage : public Stuff::Plug
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors/Destructors
	//
public:
	GOSImage(std::wstring_view imageName);
	GOSImage(uint32_t imageHandle);
	GOSImage(std::wstring_view, gos_TextureHints);
	~GOSImage(void);

#if CONSIDERED_OBSOLETE

	int32_t Getwidth(void);
	int32_t Getheight(void);

	std::wstring_view GetName(void)
	{
		// Check_Object(this);
		return imageName;
	}

	int32_t Ref(void)
	{
		// Check_Object(this);
		instance++;
		return instance;
	}

	int32_t DeRef(void)
	{
		// Check_Object(this);
		instance--;
		return instance;
	}

	int32_t GetRef(void)
	{
		// Check_Object(this);
		return instance;
	}

	bool IsLoaded(void)
	{
		// Check_Object(this);
		return ((flags & Loaded) != 0);
	}

	uint32_t GetHandle(void)
	{
		// Check_Object(this);
		uint32_t imageHandle = mcTextureManager->get_gosTextureHandle(mcTextureNodeIndex);
		if (imageHandle == 0xffffffff)
			imageHandle = 0;
		return imageHandle;
	}

	void SetHandle(uint32_t handle)
	{
		// EVERY call to this must change from gos_load to our load
		// Check_Object(this);
		mcTextureNodeIndex = handle;
	}

	enum
	{
		Loaded = 1,
		Locked = 2
	};

	void LockImage(void);
	void UnlockImage(void);
	uint8_t* GetImagePtr(void)
	{
		return (uint8_t*)ptr.pTexture;
	}
	int32_t GetPitch(void)
	{
		return ptr.Pitch;
	}

#endif

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
public:
	void TestInstance(void) const { }

	std::wstring imageName;
	int32_t flags;

protected:
	gos_TextureHints ipHints;
	int32_t instance;
	uint32_t mcTextureNodeIndex;
	TEXTUREPTR ptr;
};
} // namespace MidLevelRenderer
#endif