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
#include "SimpleMath.h"

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

namespace directxtk
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
} // namespace directxtk

/****************************************************************************
 *
 * Viewport
 *
 ****************************************************************************/

#if defined(__d3d11_h__) || defined(__d3d11_x_h__)
static_assert(sizeof(directxtk::SimpleMath::Viewport) == sizeof(D3D11_VIEWPORT), "Size mismatch");
static_assert(offsetof(directxtk::SimpleMath::Viewport, x) == offsetof(D3D11_VIEWPORT, TopLeftX), "Layout mismatch");
static_assert(offsetof(directxtk::SimpleMath::Viewport, y) == offsetof(D3D11_VIEWPORT, TopLeftY), "Layout mismatch");
static_assert(offsetof(directxtk::SimpleMath::Viewport, width) == offsetof(D3D11_VIEWPORT, Width), "Layout mismatch");
static_assert(offsetof(directxtk::SimpleMath::Viewport, height) == offsetof(D3D11_VIEWPORT, Height), "Layout mismatch");
static_assert(offsetof(directxtk::SimpleMath::Viewport, minDepth) == offsetof(D3D11_VIEWPORT, MinDepth), "Layout mismatch");
static_assert(offsetof(directxtk::SimpleMath::Viewport, maxDepth) == offsetof(D3D11_VIEWPORT, MaxDepth), "Layout mismatch");
#endif

#if defined(__d3d12_h__) || defined(__d3d12_x_h__) || defined(__XBOX_D3D12_X__)
static_assert(sizeof(directxtk::SimpleMath::Viewport) == sizeof(D3D12_VIEWPORT), "Size mismatch");
static_assert(offsetof(directxtk::SimpleMath::Viewport, x) == offsetof(D3D12_VIEWPORT, TopLeftX), "Layout mismatch");
static_assert(offsetof(directxtk::SimpleMath::Viewport, y) == offsetof(D3D12_VIEWPORT, TopLeftY), "Layout mismatch");
static_assert(offsetof(directxtk::SimpleMath::Viewport, width) == offsetof(D3D12_VIEWPORT, Width), "Layout mismatch");
static_assert(offsetof(directxtk::SimpleMath::Viewport, height) == offsetof(D3D12_VIEWPORT, Height), "Layout mismatch");
static_assert(offsetof(directxtk::SimpleMath::Viewport, minDepth) == offsetof(D3D12_VIEWPORT, MinDepth), "Layout mismatch");
static_assert(offsetof(directxtk::SimpleMath::Viewport, maxDepth) == offsetof(D3D12_VIEWPORT, MaxDepth), "Layout mismatch");
#endif

RECT directxtk::SimpleMath::Viewport::ComputeDisplayArea(DXGI_SCALING scaling, uint32_t backBufferWidth, uint32_t backBufferHeight, int32_t outputWidth, int32_t outputHeight) noexcept
{
	RECT rct = {};

	switch (int32_t(scaling))
	{
	case DXGI_SCALING_STRETCH:
		// Output fills the entire window area
		rct.top = 0;
		rct.left = 0;
		rct.right = outputWidth;
		rct.bottom = outputHeight;
		break;

	case 2 /*DXGI_SCALING_ASPECT_RATIO_STRETCH*/:
		// Output fills the window area but respects the original aspect ratio, using pillar boxing or letter boxing as required
		// Note: This scaling option is not supported for legacy Win32 windows swap chains
		{
			assert(backBufferHeight > 0);
			float aspectRatio = float(backBufferWidth) / float(backBufferHeight);

			// Horizontal fill
			float scaledWidth = float(outputWidth);
			float scaledHeight = float(outputWidth) / aspectRatio;
			if (scaledHeight >= float(outputHeight))
			{
				// Do vertical fill
				scaledWidth = float(outputHeight) * aspectRatio;
				scaledHeight = float(outputHeight);
			}

			float offsetX = (float(outputWidth) - scaledWidth) * 0.5f;
			float offsetY = (float(outputHeight) - scaledHeight) * 0.5f;

			rct.left = static_cast<int32_t>(offsetX);
			rct.top = static_cast<int32_t>(offsetY);
			rct.right = static_cast<int32_t>(offsetX + scaledWidth);
			rct.bottom = static_cast<int32_t>(offsetY + scaledHeight);

			// Clip to display window
			rct.left = std::max<int32_t>(0, rct.left);
			rct.top = std::max<int32_t>(0, rct.top);
			rct.right = std::min<int32_t>(outputWidth, rct.right);
			rct.bottom = std::min<int32_t>(outputHeight, rct.bottom);
		}
		break;

	case DXGI_SCALING_NONE:
	default:
		// Output is displayed in the upper left corner of the window area
		rct.top = 0;
		rct.left = 0;
		rct.right = std::min<int32_t>(static_cast<int32_t>(backBufferWidth), outputWidth);
		rct.bottom = std::min<int32_t>(static_cast<int32_t>(backBufferHeight), outputHeight);
		break;
	}

	return rct;
}

RECT directxtk::SimpleMath::Viewport::ComputeTitleSafeArea(uint32_t backBufferWidth, uint32_t backBufferHeight) noexcept
{
	float safew = (float(backBufferWidth) + 19.f) / 20.f;
	float safeh = (float(backBufferHeight) + 19.f) / 20.f;

	RECT rct;
	rct.left = static_cast<int32_t>(safew);
	rct.top = static_cast<int32_t>(safeh);
	rct.right = static_cast<int32_t>(float(backBufferWidth) - safew + 0.5f);
	rct.bottom = static_cast<int32_t>(float(backBufferHeight) - safeh + 0.5f);

	return rct;
}
