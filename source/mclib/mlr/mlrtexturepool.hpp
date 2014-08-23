//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRTEXTUREPOOL_HPP
#define MLR_MLRTEXTUREPOOL_HPP

//#include <mlr/mlr.hpp>
//#include <mlr/gosimagepool.hpp>

namespace MidLevelRenderer {

	class MLRTexturePool:
		public Stuff::RegisteredClass

	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Initialization
		//
	public:
		static void __stdcall InitializeClass(void);
		static void TerminateClass();
		static ClassData* DefaultData;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors/Destructors
		//
	protected:
		MLRTexturePool(Stuff::MemoryStream* stream);

	public:
		//	insDep == nr of lower bits used for image instancing
		MLRTexturePool(GOSImagePool *image_pool, int insDep=3);
		~MLRTexturePool();

		static MLRTexturePool* Make(Stuff::MemoryStream *stream);
		void Save(Stuff::MemoryStream *stream);
		MLRTexture* Add(PCSTR textureName, int instance=0);
		MLRTexture*	Add(GOSImage*);
		MLRTexture* Add(PCSTR imageName, gos_TextureFormat format, int size, gos_TextureHints hints)
		{
			return Add(imagePool->GetImage(imageName, format, size, hints) );
		}

		// only removes the texture from the texture pool, it doesnt destroy the texture
		void Remove(MLRTexture*);
		uint32_t LoadImages();
		MLRTexture* operator() (PCSTR name, int=0);
		MLRTexture* operator[] (size_t index)
		{
			Check_Object(this);
			Verify(index-1 < MLRState::TextureMask);
			return textureArray[index-1];
		}

		MLRTexture* operator[] (const MLRState* state)
		{
			Check_Object(this);
			return textureArray[state->GetTextureHandle()-1];
		}

		GOSImage* GetImage(PCSTR imageName)
		{
			Check_Object(this);
			return imagePool->GetImage(imageName);
		}

		const GOSImagePool* GetGOSImagePool()
		{
			Check_Object(this);
			return imagePool;
		}

		uint32_t GetLastHandle()
		{
			Check_Object(this);
			return lastHandle;
		}

		void Stop (void);
		void Restart (void);

		uint32_t GetNumStoredTextures()
		{
			Check_Object(this);
			return storedTextures;
		}

		int GetInstanceDepth() const
		{
			Check_Object(this);
			return instanceDepth;
		}

		static MLRTexturePool* Instance;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Testing
		//
	public:
		void TestInstance() const {}

	protected:
		bool	unLoadedImages;
		int		instanceDepth;		// bits used for image instancing
		int		instanceMax;		// max for image instancing
		int		handleDepth;		// bits used for image instancing
		int		handleMax;			// max for image instancing
		int		lastHandle;
		int		storedTextures;

		Stuff::StaticArrayOf<MLRTexture*, MLRState::TextureMask+1> textureArray;

		int*	freeHandle;
		int		firstFreeHandle;
		int		lastFreeHandle;

		GOSImagePool*	imagePool;
	};

}
#endif
