//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include <mlr/gosimagepool.hpp>

using namespace MidLevelRenderer;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
GOSImagePool::GOSImagePool() : imageHash(4099, nullptr, true)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	texturePath = "";
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
GOSImagePool::~GOSImagePool()
{
	HashIteratorOf<GOSImage*, std::wstring> images(&imageHash);
	images.DeletePlugs();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void GOSImagePool::UnLoadImages(void)
{
	HashIteratorOf<GOSImage*, std::wstring> images(&imageHash);
	GOSImage* image = images.ReadAndNext();
	while (image)
	{
		RemoveImage(image);
		image = images.ReadAndNext();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
GOSImage* GOSImagePool::GetImage(PCSTR image_name)
{
	// Check_Object(this);
	std::wstring imageName = image_name;
	_ASSERT(imageName.GetLength() > 0);
	//
	//---------------------------
	// Get the image for the name
	//---------------------------
	//
	GOSImage* image;
	if ((image = imageHash.Find(imageName)) == nullptr)
	{
#ifdef _GAMEOS_HPP_
		gos_PushCurrentHeap(Heap);
#endif
		image = new GOSImage(image_name);
		Register_Object(image);
		gos_PopCurrentHeap();
		imageHash.AddValue(image, image->imageName);
	}
	Check_Object(image);
	return image;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
GOSImage* GOSImagePool::GetImage(
	PCSTR image_name, gos_TextureFormat format, int32_t size, gos_TextureHints hints)
{
	// Check_Object(this);
	std::wstring imageName = image_name;
	_ASSERT(imageName.GetLength() > 0);
	//
	//---------------------------
	// Get the image for the name
	//---------------------------
	//
	GOSImage* image;
	if ((image = imageHash.Find(imageName)) == nullptr)
	{
#ifdef _GAMEOS_HPP_
		gos_PushCurrentHeap(Heap);
#endif
		image = new GOSImage(image_name, hints);
		// Register_Object(image);
		gos_PopCurrentHeap();
		imageHash.AddValue(image, image->imageName);
	}
#ifdef _ARMOR
	else
	{
	}
#endif
	// Check_Object(image);

	return image;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void GOSImagePool::RemoveImage(GOSImage* image)
{
	// Check_Object(this);
	// Unregister_Object(image);
	delete image;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
TGAFilePool::TGAFilePool(PCSTR path) { texturePath = path; }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool TGAFilePool::LoadImage(GOSImage* image, int32_t hint)
{
	if ((image->flags & GOSImage::Loaded) != 0)
		return true;
	std::wstring file_name = texturePath;
	file_name += image->imageName;
	file_name += ".tga";
	PSTR fFileName = file_name;
	if (((fFileName[0] != 'F') || (fFileName[0] != 'f')) &&
		((fFileName[1] != 'X') || (fFileName[1] != 'x')))
		hint |= gosHint_DisableMipmap;
	uint32_t nodeIndex = mcTextureManager->loadTexture(file_name, gos_Texture_Detect, hint);
	image->SetHandle(nodeIndex);
	image->flags |= GOSImage::Loaded;
	return ((image->flags & GOSImage::Loaded) != 0);
}
