//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRCLIPPINGSTATE_HPP
#define MLR_MLRCLIPPINGSTATE_HPP

#include "stuff/vector4d.h"

namespace Stuff
{
class Vector4D;
} // namespace Stuff

namespace MidLevelRenderer
{

//##########################################################################
//####################    MLRClippingState    ##############################
//##########################################################################

class MLRClippingState
{
protected:
	uint32_t clippingState;

public:
	MLRClippingState(void)
	{
		clippingState = 0;
	};
	MLRClippingState(uint32_t state)
	{
		clippingState = state;
	};
	MLRClippingState(const MLRClippingState& state)
	{
		clippingState = state.clippingState;
	}

	//##########################################################################
	//	Attention !!! when changing the flags also change them in
	//	Stuff::Vector4D::MultiplySetClip the assembler block
	//
	//##########################################################################
	enum : uint32_t
	{
		TopClipBit = 0,
		BottomClipBit,
		LeftClipBit,
		RightClipBit,
		NearClipBit,
		FarClipBit,
		NextBit
	};

	enum : uint32_t
	{
		TopClipFlag = 1 << TopClipBit,
		BottomClipFlag = 1 << BottomClipBit,
		LeftClipFlag = 1 << LeftClipBit,
		RightClipFlag = 1 << RightClipBit,
		NearClipFlag = 1 << NearClipBit,
		FarClipFlag = 1 << FarClipBit,
		ClipMask =
			TopClipFlag | BottomClipFlag | LeftClipFlag | RightClipFlag | NearClipFlag | FarClipFlag
	};

	bool IsFarClipped(void)
	{
		// Check_Pointer(this);
		return (clippingState & FarClipFlag) != 0;
	}
	void SetFarClip(void)
	{
		// Check_Pointer(this);
		clippingState |= FarClipFlag;
	}
	void ClearFarClip(void)
	{
		// Check_Pointer(this);
		clippingState &= ~FarClipFlag;
	}
	bool IsNearClipped(void)
	{
		// Check_Pointer(this);
		return (clippingState & NearClipFlag) != 0;
	}
	void SetNearClip(void)
	{
		// Check_Pointer(this);
		clippingState |= NearClipFlag;
	}
	void ClearNearClip(void)
	{
		// Check_Pointer(this);
		clippingState &= ~NearClipFlag;
	}
	bool IsTopClipped(void)
	{
		// Check_Pointer(this);
		return clippingState & TopClipFlag;
	}
	void SetTopClip(void)
	{
		// Check_Pointer(this);
		clippingState |= TopClipFlag;
	}
	void ClearTopClip(void)
	{
		// Check_Pointer(this);
		clippingState &= ~TopClipFlag;
	}

	bool IsBottomClipped(void)
	{
		// Check_Pointer(this);
		return (clippingState & BottomClipFlag) != 0;
	}
	void SetBottomClip(void)
	{
		// Check_Pointer(this);
		clippingState |= BottomClipFlag;
	}
	void ClearBottomClip(void)
	{
		// Check_Pointer(this);
		clippingState &= ~BottomClipFlag;
	}
	bool IsLeftClipped(void)
	{
		// Check_Pointer(this);
		return (clippingState & LeftClipFlag) != 0;
	}
	void SetLeftClip(void)
	{
		// Check_Pointer(this);
		clippingState |= LeftClipFlag;
	}
	void ClearLeftClip(void)
	{
		// Check_Pointer(this);
		clippingState &= ~LeftClipFlag;
	}

	bool IsRightClipped(void)
	{
		// Check_Pointer(this);
		return (clippingState & RightClipFlag) != 0;
	}
	void SetRightClip(void)
	{
		// Check_Pointer(this);
		clippingState |= RightClipFlag;
	}
	void ClearRightClip(void)
	{
		// Check_Pointer(this);
		clippingState &= ~RightClipFlag;
	}

	void SetClip(uint32_t mask, uint32_t flag)
	{
		// Check_Pointer(this);
#if USE_INLINE_ASSEMBLER_CODE
		_asm
			{
				xor		ecx, ecx

				mov		ebx, mask

				test	ebx, 0ffffffffh

				seta	cl

				xor		eax, eax
				sub		eax, ecx

				and		flag, eax
			}
		clippingState |= flag;
#else
		if (mask != 0)
		{
			clippingState |= flag;
		}
#endif
	}

	bool IsClipped(uint32_t mask)
	{
		// Check_Pointer(this);
		return (clippingState & mask) != 0;
	}
	uint32_t GetClippingState(void)
	{
		// Check_Pointer(this);
		return (clippingState & ClipMask);
	}
	void SetClippingState(uint32_t state)
	{
		// Check_Pointer(this);
		clippingState = state & ClipMask;
	}
	uint32_t GetNumberOfSetBits(void)
	{
		// Check_Pointer(this);
		_ASSERT(clippingState <= ClipMask);
		return numberBitsLookUpTable[clippingState];
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Assignment operators
	//
public:
	MLRClippingState& operator=(const MLRClippingState& s)
	{
		// Check_Pointer(this);
		clippingState = s.clippingState;
		return *this;
	}

	MLRClippingState& operator&=(const MLRClippingState& s)
	{
		// Check_Pointer(this);
		clippingState &= s.clippingState;
		return *this;
	}

	MLRClippingState& operator|=(const MLRClippingState& s)
	{
		// Check_Pointer(this);
		clippingState |= s.clippingState;
		return *this;
	}

	bool operator==(const MLRClippingState& s)
	{
		// Check_Pointer(this);
		return (clippingState == s.clippingState);
	}

	bool operator==(const uint32_t& s)
	{
		// Check_Pointer(this);
		return (clippingState == s);
	}

	bool operator!=(const MLRClippingState& s)
	{
		// Check_Pointer(this);
		return (clippingState != s.clippingState);
	}

	bool operator!=(const uint32_t& s)
	{
		// Check_Pointer(this);
		return (clippingState != s);
	}

	void Save(std::iostream stream);
	void Load(std::iostream stream);

	inline void Clip4dVertex(Stuff::Vector4D* v4d);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
public:
	void TestInstance(void) { }

private:
	static uint32_t numberBitsLookUpTable[ClipMask + 1];
};

inline void
MLRClippingState::Clip4dVertex(Stuff::Vector4D* v4d)
{
#if USE_INLINE_ASSEMBLER_CODE
	uint32_t _ret = 0;
	_asm
		{
			mov     edi, v4d

			xor     ecx, ecx
			xor     edx, edx
			test    dword ptr [edi], 080000000h
			setne   cl
			sub     edx, ecx
			and     edx, 8 // RightClipFlag

			xor     ebx, ebx
			test    dword ptr [edi+4], 080000000h
			setne   cl
			sub     ebx, ecx
			and     ebx, 2 // BottomClipFlag

			or      edx, ebx

			xor     ebx, ebx
			test    dword ptr [edi+8], 080000000h
			setne   cl
			sub     ebx, ecx
			and     ebx, 16 // NearClipFlag

			or      edx, ebx

			fld     dword ptr [edi+0Ch]

			xor     ebx, ebx
			fcom    dword ptr [edi]
			fnstsw  ax
			test    ah, 1
			setne   cl
			sub     ebx, ecx
			and     ebx, 4 // LeftClipFlag

			or      edx, ebx

			xor     ebx, ebx
			fcom    dword ptr [edi+4]
			fnstsw  ax
			test    ah, 1
			setne   cl
			sub     ebx, ecx
			and     ebx, 1 // TopClipFlag

			or      edx, ebx

			xor     ebx, ebx
			fcomp   dword ptr [edi+8]
			fnstsw  ax
			test    ah, 41h
			setne   cl
			sub     ebx, ecx
			and     ebx, 32 // FarClipFlag

			or      edx, ebx

			mov     _ret, edx
		}
	clippingState = _ret;
#else
	clippingState = 0;
	if (v4d->w <= v4d->z)
	{
		SetFarClip();
	}
	if (v4d->z < 0.0f)
	{
		SetNearClip();
	}
	if (v4d->x < 0.0f)
	{
		SetRightClip();
	}
	if (v4d->w < v4d->x)
	{
		SetLeftClip();
	}
	if (v4d->y < 0.0f)
	{
		SetBottomClip();
	}
	if (v4d->w < v4d->y)
	{
		SetTopClip();
	}
#endif
}
} // namespace MidLevelRenderer
#endif
