//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/mlrtexturepool.h"

namespace MidLevelRenderer {

//#############################################################################
//############################    MLRTexture    ###############################
//#############################################################################

MLRTexturePool* MLRTexturePool::Instance;
MLRTexturePool::ClassData* MLRTexturePool::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRTexturePool::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(
		MLRTexturePoolClassID, "MidLevelRenderer::MLRTexturePool", RegisteredClass::DefaultData);
	Register_Object(DefaultData);
	MLRTexturePool::Instance = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRTexturePool::TerminateClass()
{
	if (MLRTexturePool::Instance)
	{
		Unregister_Object(MLRTexturePool::Instance);
		delete MLRTexturePool::Instance;
	}
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRTexturePool::MLRTexturePool(std::iostream stream) :
	RegisteredClass(DefaultData)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	unLoadedImages = false;
	STOP(("Not implemented"));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRTexturePool::Stop(void)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	int32_t i;
	for (i = 0; i < MLRState::TextureMask; i++)
	{
		if (textureArray[i] != nullptr)
		{
			Unregister_Object(textureArray[i]);
			delete textureArray[i];
			textureArray[i] = nullptr;
		}
	}
	Unregister_Pointer(freeHandle);
	delete[] freeHandle;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRTexturePool::Restart(void)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	freeHandle = new int32_t[handleMax];
	Register_Pointer(freeHandle);
	lastHandle = 0;
	firstFreeHandle = 0;
	lastFreeHandle = 0;
	storedTextures = 0;
	for (size_t i = 0; i < MLRState::TextureMask + 1; i++)
	{
		textureArray[i] = nullptr;
	}
	unLoadedImages = false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRTexturePool::MLRTexturePool(GOSImagePool* image_pool, int32_t insDep) :
	RegisteredClass(DefaultData)
{
	Check_Object(image_pool);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	instanceDepth = insDep;
	instanceMax = 1 << insDep;
	handleDepth = MLRState::TextureNumberBits - insDep;
	handleMax = 1 << handleDepth;
	freeHandle = new int32_t[handleMax];
	Register_Pointer(freeHandle);
	lastHandle = 0;
	firstFreeHandle = 0;
	lastFreeHandle = 0;
	storedTextures = 0;
	imagePool = image_pool;
	for (size_t i = 0; i < MLRState::TextureMask + 1; i++)
	{
		textureArray[i] = nullptr;
	}
	unLoadedImages = false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRTexturePool::~MLRTexturePool()
{
	int32_t i;
	for (i = 0; i < MLRState::TextureMask; i++)
	{
		if (textureArray[i] != nullptr)
		{
			Unregister_Object(textureArray[i]);
			delete textureArray[i];
			textureArray[i] = nullptr;
		}
	}
	Unregister_Object(imagePool);
	delete imagePool;
	Unregister_Pointer(freeHandle);
	delete[] freeHandle;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRTexturePool*
MLRTexturePool::Make(std::iostream stream)
{
	Check_Object(stream);
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLRTexturePool* pool = new MLRTexturePool(stream);
	gos_PopCurrentHeap();
	return pool;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRTexturePool::Save(std::iostream stream)
{
	STOP(("Not implemented"));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRTexture*
MLRTexturePool::Add(const std::wstring_view& tn, int32_t instance)
{
	const std::wstring_view& textureName(tn);
	int32_t i, j, textureNameHashValue = textureName.GetHashValue();
	for (i = 0; i < lastHandle; i++)
	{
		int32_t first = i << instanceDepth;
		bool yo = false;
		for (j = first; j < first + instanceMax; j++)
		{
			if (textureArray[j] && textureArray[j]->textureNameHashValue == textureNameHashValue)
			{
				yo = 1;
			}
		}
		if (yo == false)
		{
			continue;
		}
		for (j = first; j < first + instanceMax; j++)
		{
			if (textureArray[j] && textureArray[j]->instance == instance)
			{
				return textureArray[j];
			}
		}
		for (j = first; j < first + instanceMax; j++)
		{
			if (!textureArray[j])
			{
#ifdef _GAMEOS_HPP_
				gos_PushCurrentHeap(Heap);
#endif
				textureArray[j] = new MLRTexture(this, textureName, instance, j + 1);
				Register_Object(textureArray[j]);
#ifdef _GAMEOS_HPP_
				gos_PopCurrentHeap();
#endif
				storedTextures++;
				unLoadedImages = true;
				return textureArray[j];
			}
		}
		STOP(("Asked for too much image instances !"));
	}
	int32_t newHandle;
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	if (firstFreeHandle < lastFreeHandle)
	{
		newHandle = (freeHandle[firstFreeHandle & (handleMax - 1)]) << instanceDepth;
		textureArray[newHandle] = new MLRTexture(this, textureName, instance, newHandle + 1);
		storedTextures++;
		firstFreeHandle++;
	}
	else
	{
		_ASSERT(((lastHandle << instanceDepth) + 1) < MLRState::TextureMask);
		newHandle = lastHandle << instanceDepth;
		textureArray[newHandle] = new MLRTexture(this, textureName, instance, newHandle + 1);
		storedTextures++;
		lastHandle++;
	}
	Register_Object(textureArray[newHandle]);
	gos_PopCurrentHeap();
	unLoadedImages = true;
	return textureArray[newHandle];
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRTexture*
MLRTexturePool::Add(GOSImage* image)
{
	const std::wstring_view& textureName;
	textureName = image->GetName();
	int32_t i, j, textureNameHashValue = textureName.GetHashValue();
	for (i = 0; i < lastHandle; i++)
	{
		int32_t first = i << instanceDepth;
		for (j = first; j < first + instanceMax; j++)
		{
			if (textureArray[j] && textureArray[j]->textureNameHashValue == textureNameHashValue)
			{
				_ASSERT(image == textureArray[j]->GetImage());
				return textureArray[j];
				//				STOP(("Image allready in texture pool !"));
			}
		}
	}
	int32_t newHandle;
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	if (firstFreeHandle < lastFreeHandle)
	{
		newHandle = (freeHandle[firstFreeHandle & (handleMax - 1)]) << instanceDepth;
		textureArray[newHandle] = new MLRTexture(this, image, newHandle + 1);
		storedTextures++;
		firstFreeHandle++;
	}
	else
	{
		_ASSERT(((lastHandle << instanceDepth) + 1) < MLRState::TextureMask);
		newHandle = lastHandle << instanceDepth;
		textureArray[newHandle] = new MLRTexture(this, image, newHandle + 1);
		storedTextures++;
		lastHandle++;
	}
	Register_Object(textureArray[newHandle]);
	gos_PopCurrentHeap();
	return textureArray[newHandle];
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRTexturePool::Remove(MLRTexture* tex)
{
	textureArray[tex->textureHandle - 1] = nullptr;
	storedTextures--;
	int32_t i, first = (tex->textureHandle - 1) & ~(instanceMax - 1);
	for (i = first; i < first + instanceMax; i++)
	{
		if (textureArray[i] != nullptr)
		{
			break;
		}
	}
	if (i >= first + instanceMax)
	{
		imagePool->RemoveImage(tex->image);
		tex->image = nullptr;
		freeHandle[lastFreeHandle & (handleMax - 1)] = (tex->textureHandle - 1) >> instanceDepth;
		lastFreeHandle++;
	}
	tex->textureHandle = 0;
	unLoadedImages = true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRTexture*
MLRTexturePool::operator()(const std::wstring_view& tn, int32_t instance)
{
	// Check_Object(this);
	const std::wstring_view& textureName = tn;
	int32_t i, j, textureNameHashValue = textureName.GetHashValue();
	for (i = 0; i < lastHandle; i++)
	{
		int32_t first = i << instanceDepth;
		for (j = first; j < first + instanceMax; j++)
		{
			if (textureArray[j])
			{
				if (textureArray[j]->textureNameHashValue == textureNameHashValue)
				{
					if (textureArray[j]->instance == instance)
					{
						return textureArray[j];
					}
				}
				else
				{
					break;
				}
			}
		}
	}
	return nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
uint32_t
MLRTexturePool::LoadImages()
{
	Check_Object(imagePool);
	if (unLoadedImages == false)
	{
		return lastHandle;
	}
	//
	// Statistic timing function
	//
	for (uint32_t i = 0; i < MLRState::TextureMask + 1; i++)
	{
		if (textureArray[i])
		{
			int32_t hint;
			GOSImage* image = textureArray[i]->GetImage(&hint);
			if (image && !image->IsLoaded())
			{
				Check_Object(image);
				if (!imagePool->LoadImage(image, hint))
				{
					STOP(("Cannot load texture: %s!", textureArray[i]->textureName));
				}
			}
		}
	}
	//
	// End timing function
	//
	unLoadedImages = false;
	return lastHandle;
}

} // namespace MidLevelRenderer
