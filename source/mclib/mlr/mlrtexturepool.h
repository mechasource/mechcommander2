//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRTEXTUREPOOL_HPP
#define MLR_MLRTEXTUREPOOL_HPP

//#include "stuff/marray.h"
#include "mlr/mlrstate.h"
#include "mlr/gosimagepool.h"

namespace MidLevelRenderer
{

class MLRTexture;
class MLRState;
class GOSImage;
class GOSImagePool;

class MLRTexturePool // : public Stuff::RegisteredClass

{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialization
	//
public:
	static void __stdcall InitializeClass(void);
	static void TerminateClass(void);
	static ClassData* DefaultData;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors/Destructors
	//
protected:
	MLRTexturePool(std::iostream stream);

public:
	// insDep == nr of lower bits used for image instancing
	MLRTexturePool(GOSImagePool* image_pool, int32_t insDep = 3);
	~MLRTexturePool(void);

	static MLRTexturePool* Make(std::iostream stream);
	void Save(std::iostream stream);
	MLRTexture* Add(std::wstring_view textureName, int32_t instance = 0);
	MLRTexture* Add(GOSImage*);
	MLRTexture* Add(std::wstring_view imageName, /*gos_TextureFormat*/ uint32_t format, size_t size,
		/*gos_TextureHints*/ uint32_t hints)
	{
		return Add(imagePool->GetImage(imageName, format, size, hints));
	}

	// only removes the texture from the texture pool, it doesnt destroy the
	// texture
	void Remove(MLRTexture*);
	uint32_t LoadImages(void);
	MLRTexture* operator()(std::wstring_view name, int32_t = 0);
	MLRTexture* operator[](size_t index)
	{
		// Check_Object(this);
		_ASSERT(index - 1 < MLRState::TextureMask);
		return textureArray[index - 1];
	}

	MLRTexture* operator[](const MLRState* state)
	{
		// Check_Object(this);
		return textureArray[state->GetTextureHandle() - 1];
	}

	GOSImage* GetImage(std::wstring_view imageName)
	{
		// Check_Object(this);
		return imagePool->GetImage(imageName);
	}

	const GOSImagePool* GetGOSImagePool()
	{
		// Check_Object(this);
		return imagePool;
	}

	uint32_t GetLastHandle()
	{
		// Check_Object(this);
		return lastHandle;
	}

	void Stop(void);
	void Restart(void);

	uint32_t GetNumStoredTextures()
	{
		// Check_Object(this);
		return storedTextures;
	}

	int32_t GetInstanceDepth(void) const
	{
		// Check_Object(this);
		return instanceDepth;
	}

	static MLRTexturePool* Instance;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
public:
	void TestInstance(void) const { }

protected:
	bool unLoadedImages;
	int32_t instanceDepth; // bits used for image instancing
	int32_t instanceMax; // max for image instancing
	int32_t handleDepth; // bits used for image instancing
	int32_t handleMax; // max for image instancing
	uint32_t lastHandle;
	uint32_t storedTextures;

	std::array<MLRTexture*, MLRState::TextureMask + 1> textureArray;

	int32_t* freeHandle;
	int32_t firstFreeHandle;
	int32_t lastFreeHandle;

	GOSImagePool* imagePool;
};
} // namespace MidLevelRenderer
#endif
