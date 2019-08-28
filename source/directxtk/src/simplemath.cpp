//-------------------------------------------------------------------------------------
// SimpleMath.cpp -- Simplified C++ Math wrapper for DirectXMath
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//-------------------------------------------------------------------------------------

#include "stdinc.h"
#include "simplemath.h"

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

namespace DirectX
{
namespace SimpleMath
{
const Vector2 Vector2::Zero = {0.f, 0.f};
const Vector2 Vector2::One = {1.f, 1.f};
const Vector2 Vector2::UnitX = {1.f, 0.f};
const Vector2 Vector2::UnitY = {0.f, 1.f};

const Vector3 Vector3::Zero = {0.f, 0.f, 0.f};
const Vector3 Vector3::One = {1.f, 1.f, 1.f};
const Vector3 Vector3::UnitX = {1.f, 0.f, 0.f};
const Vector3 Vector3::UnitY = {0.f, 1.f, 0.f};
const Vector3 Vector3::UnitZ = {0.f, 0.f, 1.f};
const Vector3 Vector3::Up = {0.f, 1.f, 0.f};
const Vector3 Vector3::Down = {0.f, -1.f, 0.f};
const Vector3 Vector3::Right = {1.f, 0.f, 0.f};
const Vector3 Vector3::Left = {-1.f, 0.f, 0.f};
const Vector3 Vector3::Forward = {0.f, 0.f, -1.f};
const Vector3 Vector3::Backward = {0.f, 0.f, 1.f};

const Vector4 Vector4::Zero = {0.f, 0.f, 0.f, 0.f};
const Vector4 Vector4::One = {1.f, 1.f, 1.f, 1.f};
const Vector4 Vector4::UnitX = {1.f, 0.f, 0.f, 0.f};
const Vector4 Vector4::UnitY = {0.f, 1.f, 0.f, 0.f};
const Vector4 Vector4::UnitZ = {0.f, 0.f, 1.f, 0.f};
const Vector4 Vector4::UnitW = {0.f, 0.f, 0.f, 1.f};

const Matrix Matrix::Identity = {1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f};

const Quaternion Quaternion::Identity = {0.f, 0.f, 0.f, 1.f};
} // namespace SimpleMath
} // namespace DirectX

/****************************************************************************
 *
 * Viewport
 *
 ****************************************************************************/

#if defined(__d3d11_h__) || defined(__d3d11_x_h__)
static_assert(sizeof(DirectX::SimpleMath::Viewport) == sizeof(D3D11_VIEWPORT), "Size mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, x) == offsetof(D3D11_VIEWPORT, TopLeftX), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, y) == offsetof(D3D11_VIEWPORT, TopLeftY), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, width) == offsetof(D3D11_VIEWPORT, width), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, height) == offsetof(D3D11_VIEWPORT, height), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, minDepth) == offsetof(D3D11_VIEWPORT, MinDepth), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, maxDepth) == offsetof(D3D11_VIEWPORT, MaxDepth), "Layout mismatch");
#endif

#if defined(__d3d12_h__) || defined(__d3d12_x_h__)
static_assert(sizeof(DirectX::SimpleMath::Viewport) == sizeof(D3D12_VIEWPORT), "Size mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, x) == offsetof(D3D12_VIEWPORT, TopLeftX), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, y) == offsetof(D3D12_VIEWPORT, TopLeftY), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, width) == offsetof(D3D12_VIEWPORT, width), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, height) == offsetof(D3D12_VIEWPORT, height), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, minDepth) == offsetof(D3D12_VIEWPORT, MinDepth), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, maxDepth) == offsetof(D3D12_VIEWPORT, MaxDepth), "Layout mismatch");
#endif

RECT
DirectX::SimpleMath::Viewport::ComputeDisplayArea(DXGI_SCALING scaling, uint32_t backBufferwidth, uint32_t backBufferheight, int outputwidth, int outputheight)
{
	RECT rct;

	switch (int(scaling))
	{
	case DXGI_SCALING_STRETCH:
		// Output fills the entire window area
		rct.top = 0;
		rct.left = 0;
		rct.right = outputwidth;
		rct.bottom = outputheight;
		break;

	case 2 /*DXGI_SCALING_ASPECT_RATIO_STRETCH*/:
		// Output fills the window area but respects the original aspect ratio, using pillar boxing or letter boxing as required
		// Note: This scaling option is not supported for legacy Win32 windows swap chains
		{
			_ASSERT(backBufferheight > 0);
			float aspectRatio = float(backBufferwidth) / float(backBufferheight);

			// Horizontal fill
			float scaledwidth = float(outputwidth);
			float scaledheight = float(outputwidth) / aspectRatio;
			if (scaledheight >= outputheight)
			{
				// Do vertical fill
				scaledwidth = float(outputheight) * aspectRatio;
				scaledheight = float(outputheight);
			}

			float offsetX = (float(outputwidth) - scaledwidth) * 0.5f;
			float offsetY = (float(outputheight) - scaledheight) * 0.5f;

			rct.left = static_cast<long32_t>(offsetX);
			rct.top = static_cast<long32_t>(offsetY);
			rct.right = static_cast<long32_t>(offsetX + scaledwidth);
			rct.bottom = static_cast<long32_t>(offsetY + scaledheight);

			// Clip to display window
			rct.left = std::max<long32_t>(0, rct.left);
			rct.top = std::max<long32_t>(0, rct.top);
			rct.right = std::min<long32_t>(outputwidth, rct.right);
			rct.bottom = std::min<long32_t>(outputheight, rct.bottom);
		}
		break;

	case DXGI_SCALING_NONE:
	default:
		// Output is displayed in the upper left corner of the window area
		rct.top = 0;
		rct.left = 0;
		rct.right = std::min<long32_t>(static_cast<long32_t>(backBufferwidth), outputwidth);
		rct.bottom = std::min<long32_t>(static_cast<long32_t>(backBufferheight), outputheight);
		break;
	}

	return rct;
}

RECT
DirectX::SimpleMath::Viewport::ComputeTitleSafeArea(uint32_t backBufferwidth, uint32_t backBufferheight)
{
	float safew = (float(backBufferwidth) + 19.f) / 20.f;
	float safeh = (float(backBufferheight) + 19.f) / 20.f;

	RECT rct;
	rct.left = static_cast<long32_t>(safew);
	rct.top = static_cast<long32_t>(safeh);
	rct.right = static_cast<long32_t>(float(backBufferwidth) - safew + 0.5f);
	rct.bottom = static_cast<long32_t>(float(backBufferheight) - safeh + 0.5f);

	return rct;
}
