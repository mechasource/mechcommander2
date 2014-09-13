//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_GOSIMAGEPOOL_HPP
#define MLR_GOSIMAGEPOOL_HPP

#include <stuff/mstring.hpp>
#include <stuff/hash.hpp>

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
		GOSImage* GetImage(PCSTR imageName);
		GOSImage* GetImage(PCSTR imageName, /*gos_TextureFormat*/ uint32_t format, size_t size, /*gos_TextureHints*/ uint32_t hints);

		virtual bool LoadImage(GOSImage* image, int32_t = 0) = 0;
		void RemoveImage(GOSImage* image);
		void UnLoadImages(void);
		void GetTexturePath(Stuff::MString* pName) const
		{
			Check_Object(this);
			*pName = texturePath;
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Testing
		//
	public:
		void TestInstance(void) const
		{
		}

	protected:
		Stuff::HashOf<GOSImage*, Stuff::MString> imageHash;
		Stuff::MString texturePath;
	};

	class TGAFilePool:
		public GOSImagePool
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors/Destructors
		//
	public:
		TGAFilePool(PCSTR path);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Image handling
		//
	public:
		bool LoadImage(GOSImage* image, int32_t = 0);
	};
}
#endif
