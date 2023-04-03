//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_GOSIMAGEPOOL_HPP
#define MLR_GOSIMAGEPOOL_HPP

// #include "stuff/mstring.h"
#include "stuff/hash.h"

namespace MidLevelRenderer
{

class GOSImage;

class GOSImagePool
#if defined(_ARMOR)
	: public Stuff::Signature
#endif
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors/Destructors
	//
public:
	GOSImagePool(void);
	virtual ~GOSImagePool(void);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Image handling
	//
public:
	GOSImage* GetImage(std::wstring_view imageName);
	GOSImage* GetImage(std::wstring_view imageName, /*gos_TextureFormat*/ uint32_t format, size_t size,
		/*gos_TextureHints*/ uint32_t hints);

	virtual bool LoadImage(GOSImage* image, int32_t = 0) = 0;
	void RemoveImage(GOSImage* image);
	void UnLoadImages(void);
	void GetTexturePath(std::wstring* pName) const
	{
		// Check_Object(this);
		*pName = texturePath;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
public:
	void TestInstance(void) const { }

protected:
	Stuff::HashOf<GOSImage*, std::wstring> imageHash;
	std::wstring texturePath;
};

class TGAFilePool : public GOSImagePool
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors/Destructors
	//
public:
	TGAFilePool(std::wstring_view path);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Image handling
	//
public:
	bool LoadImage(GOSImage* image, int32_t = 0);
};
} // namespace MidLevelRenderer
#endif
