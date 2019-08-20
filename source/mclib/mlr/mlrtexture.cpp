//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include <mlr/mlrtexture.hpp>

using namespace MidLevelRenderer;

//#############################################################################
//############################    MLRTexture    ###############################
//#############################################################################

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRTexture::MLRTexture(std::iostream stream)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	// Check_Pointer(this);
	Check_Object(stream);
	hint = 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRTexture::MLRTexture(
	MLRTexturePool* tp, PCSTR texName, int32_t _instance, int32_t handle, int32_t _hint)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	thePool = tp;
	textureHandle = handle;
	textureName = texName;
	textureNameHashValue = textureName.GetHashValue();
	instance = _instance;
	textureMatrix = AffineMatrix4D::Identity;
	textureMatrixIsIdentity = true;
	image = thePool->GetImage(textureName);
	hint = _hint;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRTexture::MLRTexture(MLRTexturePool* tp, GOSImage* _image, int32_t handle, int32_t _hint)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	thePool = tp;
	textureHandle = handle;
	image = _image;
	textureName = image->GetName();
	textureNameHashValue = textureName.GetHashValue();
	instance = 0;
	textureMatrix = AffineMatrix4D::Identity;
	textureMatrixIsIdentity = true;
	hint = _hint;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRTexture::TestInstance(void) const
{
	_ASSERT((*thePool)[textureHandle]);
	Check_Object(image);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRTexture::MLRTexture(const MLRTexture& tex)
{
	Check_Object(&tex);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRTexture::~MLRTexture()
{
	if (nullptr == thePool->GetImage(image->GetName()))
	{
		Unregister_Object(image);
		delete image;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRTexture*
MLRTexture::Make(std::iostream stream)
{
	Check_Object(stream);
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLRTexture* texture = new MLRTexture(stream);
	gos_PopCurrentHeap();
	return texture;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRTexture::Save(std::iostream stream)
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t
MLRTexture::GetImageNumber()
{
	// Check_Object(this);
	return ((textureHandle - 1) >> (thePool->GetInstanceDepth()));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t
MLRTexture::GetInstanceNumber()
{
	// Check_Object(this);
	return ((textureHandle - 1) & ~((1 << thePool->GetInstanceDepth()) - 1));
}
