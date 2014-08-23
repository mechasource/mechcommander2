//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_GOSIMAGE_HPP
#define MLR_GOSIMAGE_HPP

#include <stuff/plug.hpp>
#include <stuff/mstring.hpp>

namespace MidLevelRenderer {

	class GOSImage:
		public Stuff::Plug
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors/Destructors
		//
	public:
		GOSImage(PCSTR imageName);
		GOSImage(ULONG imageHandle);
		GOSImage(PCSTR, gos_TextureHints);
		~GOSImage(void);

#if _CONSIDERED_OBSOLETE

		int		GetWidth(void);
		int		GetHeight(void);

		PCSTR	GetName(void)
		{ 
			Check_Object(this);
			return imageName;
		}

		int Ref(void)
		{
			Check_Object(this);
			instance++;
			return instance;
		}

		int DeRef(void)
		{
			Check_Object(this);
			instance--;
			return instance;
		}

		int GetRef(void)
		{ 
			Check_Object(this); return instance; 
		}

		bool IsLoaded(void)
		{
			Check_Object(this); 
			return ( (flags & Loaded) != 0);
		}

		ULONG GetHandle(void)
		{ 
			Check_Object(this);
			ULONG imageHandle = mcTextureManager->get_gosTextureHandle(mcTextureNodeIndex);

			if (imageHandle == 0xffffffff)
				imageHandle = 0;

			return imageHandle; 
		}

		void SetHandle (ULONG handle)
		{
			//EVERY call to this must change from gos_load to our load
			Check_Object(this);  
			mcTextureNodeIndex = handle;
		}

		enum {
			Loaded = 1,
			Locked = 2
		};

		void	LockImage(void);
		void	UnlockImage(void);
		puint8_t	GetImagePtr(void)
		{
			return (puint8_t )ptr.pTexture;
		}
		int		GetPitch(void)
		{ 
			return ptr.Pitch;
		}

#endif

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Testing
		//
	public:
		void TestInstance(void) const
		{
		}

		Stuff::MString imageName;
		int flags;
	protected:
		gos_TextureHints ipHints;
		int instance;
		ULONG mcTextureNodeIndex;
		TEXTUREPTR ptr;
	};

}
#endif