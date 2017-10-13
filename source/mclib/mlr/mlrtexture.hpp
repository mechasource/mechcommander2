//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRTEXTURE_HPP
#define MLR_MLRTEXTURE_HPP

#include <stuff/memorystream.hpp>
#include <stuff/mstring.hpp>
#include <stuff/affinematrix.hpp>

namespace Stuff
{
class AffineMatrix4D;
}

namespace MidLevelRenderer
{

class MLRTexturePool;
class GOSImage;

class MLRTexture
#if defined(_ARMOR)
	: public Stuff::Signature
#endif
{
	friend class MLRTexturePool;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors/Destructors
	//
  protected:
	MLRTexture(Stuff::MemoryStream* stream);

  public:
	MLRTexture(MLRTexturePool* pool, PCSTR name, int32_t instance,
		int32_t handle, int32_t hint = 0);
	MLRTexture(MLRTexturePool* pool, GOSImage* image, int32_t handle,
		int32_t hint = 0);
	MLRTexture(const MLRTexture&);
	~MLRTexture(void);

	static MLRTexture* Make(Stuff::MemoryStream* stream);

	void Save(Stuff::MemoryStream* stream);

	GOSImage* GetImage(pint32_t h = nullptr)
	{
		// Check_Object(this);
		if (h)
		{
			*h = hint;
		}
		return image;
	}

	PCSTR GetTextureName(void)
	{
		// Check_Object(this);
		return textureName;
	}

	int32_t GetTextureHandle(void)
	{
		// Check_Object(this);
		return textureHandle;
	}

	int32_t GetImageNumber(void);
	int32_t GetInstanceNumber(void);

	int32_t GetTextureInstance()
	{
		// Check_Object(this);
		return instance;
	}

	bool GetAnimateTexture(void)
	{
		// Check_Object(this);
		return !textureMatrixIsIdentity;
	}

	void SetAnimateTexture(bool yesNo)
	{
		// Check_Object(this);
		if (yesNo == true)
		{
			textureMatrixIsIdentity = false;
		}
		else
		{
			textureMatrixIsIdentity = true;
			textureMatrix			= Stuff::AffineMatrix4D::Identity;
		}
	}

	Stuff::AffineMatrix4D& GetTextureMatrix()
	{
		// Check_Object(this);
		return textureMatrix;
	}

	void SetHint(int32_t h)
	{
		// Check_Object(this);
		hint = h;
	}

	int32_t GetHint()
	{
		// Check_Object(this);
		return hint;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
  public:
	void TestInstance(void) const;

  protected:
	Stuff::MString textureName;
	int32_t textureNameHashValue;
	int32_t instance;
	int32_t textureHandle;
	int32_t hint;
	bool textureMatrixIsIdentity;
	Stuff::AffineMatrix4D textureMatrix;
	GOSImage* image;
	MLRTexturePool* thePool;
};
}
#endif
