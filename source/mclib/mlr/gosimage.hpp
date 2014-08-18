//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#if !defined(MLR_GOSIMAGE_HPP)
#define MLR_GOSIMAGE_HPP

#include <mlr/mlr.hpp>
#include <gameos.hpp>
#include "txmmgr.h"

namespace MidLevelRenderer {

	class GOSImage:
		public Stuff::Plug
	{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors/Destructors
	//
	public:
		GOSImage(PCSTR imageName);
		GOSImage(DWORD imageHandle);
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

		DWORD GetHandle(void)
		{ 
			Check_Object(this);
			DWORD imageHandle = mcTextureManager->get_gosTextureHandle(mcTextureNodeIndex);

			if (imageHandle == 0xffffffff)
				imageHandle = 0;

			return imageHandle; 
		}

		void SetHandle (DWORD handle)
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
		PUCHAR	GetImagePtr(void)
		{
			return (PUCHAR )ptr.pTexture;
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
		DWORD mcTextureNodeIndex;
		TEXTUREPTR ptr;
	};

}
#endif