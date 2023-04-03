//-------------------------------------------------------------------------------------
// SimpleMath.inl -- Simplified C++ Math wrapper for DirectXMath
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//-------------------------------------------------------------------------------------

#pragma once

/****************************************************************************
*
* Rectangle
*
****************************************************************************/

//------------------------------------------------------------------------------
// Rectangle operations
//------------------------------------------------------------------------------
inline Vector2 Rectangle::Location() const noexcept
{
	return Vector2(float(x), float(y));
}

inline Vector2 Rectangle::Center() const noexcept
{
	return Vector2(float(x) + (float(width) / 2.f), float(y) + (float(height) / 2.f));
}

inline bool Rectangle::Contains(const Vector2& point) const noexcept
{
	return (float(x) <= point.x) && (point.x < float(x + width)) && (float(y) <= point.y) && (point.y < float(y + height));
}

inline void Rectangle::Inflate(int32_t horizAmount, int32_t vertAmount) noexcept
{
	x -= horizAmount;
	y -= vertAmount;
	width += horizAmount;
	height += vertAmount;
}

//------------------------------------------------------------------------------
// Static functions
//------------------------------------------------------------------------------

inline Rectangle Rectangle::Intersect(const Rectangle& ra, const Rectangle& rb) noexcept
{
	int32_t righta = ra.x + ra.width;
	int32_t rightb = rb.x + rb.width;

	int32_t bottoma = ra.y + ra.height;
	int32_t bottomb = rb.y + rb.height;

	int32_t maxX = ra.x > rb.x ? ra.x : rb.x;
	int32_t maxY = ra.y > rb.y ? ra.y : rb.y;

	int32_t minRight = righta < rightb ? righta : rightb;
	int32_t minBottom = bottoma < bottomb ? bottoma : bottomb;

	Rectangle result;

	if ((minRight > maxX) && (minBottom > maxY))
	{
		result.x = maxX;
		result.y = maxY;
		result.width = minRight - maxX;
		result.height = minBottom - maxY;
	}
	else
	{
		result.x = 0;
		result.y = 0;
		result.width = 0;
		result.height = 0;
	}

	return result;
}

inline RECT Rectangle::Intersect(const RECT& rcta, const RECT& rctb) noexcept
{
	int32_t maxX = rcta.left > rctb.left ? rcta.left : rctb.left;
	int32_t maxY = rcta.top > rctb.top ? rcta.top : rctb.top;

	int32_t minRight = rcta.right < rctb.right ? rcta.right : rctb.right;
	int32_t minBottom = rcta.bottom < rctb.bottom ? rcta.bottom : rctb.bottom;

	RECT result;

	if ((minRight > maxX) && (minBottom > maxY))
	{
		result.left = maxX;
		result.top = maxY;
		result.right = minRight;
		result.bottom = minBottom;
	}
	else
	{
		result.left = 0;
		result.top = 0;
		result.right = 0;
		result.bottom = 0;
	}

	return result;
}

inline Rectangle Rectangle::Union(const Rectangle& ra, const Rectangle& rb) noexcept
{
	int32_t righta = ra.x + ra.width;
	int32_t rightb = rb.x + rb.width;

	int32_t bottoma = ra.y + ra.height;
	int32_t bottomb = rb.y + rb.height;

	int32_t minX = ra.x < rb.x ? ra.x : rb.x;
	int32_t minY = ra.y < rb.y ? ra.y : rb.y;

	int32_t maxRight = righta > rightb ? righta : rightb;
	int32_t maxBottom = bottoma > bottomb ? bottoma : bottomb;

	Rectangle result;
	result.x = minX;
	result.y = minY;
	result.width = maxRight - minX;
	result.height = maxBottom - minY;
	return result;
}

inline RECT Rectangle::Union(const RECT& rcta, const RECT& rctb) noexcept
{
	RECT result;
	result.left = rcta.left < rctb.left ? rcta.left : rctb.left;
	result.top = rcta.top < rctb.top ? rcta.top : rctb.top;
	result.right = rcta.right > rctb.right ? rcta.right : rctb.right;
	result.bottom = rcta.bottom > rctb.bottom ? rcta.bottom : rctb.bottom;
	return result;
}

/****************************************************************************
 *
 * Vector2
 *
 ****************************************************************************/

//------------------------------------------------------------------------------
// Comparision operators
//------------------------------------------------------------------------------

inline bool Vector2::operator==(const Vector2& V) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = XMLoadFloat2(&V);
	return DirectX::XMVector2Equal(v1, v2);
}

inline bool Vector2::operator!=(const Vector2& V) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = XMLoadFloat2(&V);
	return DirectX::XMVector2NotEqual(v1, v2);
}

//------------------------------------------------------------------------------
// Assignment operators
//------------------------------------------------------------------------------

inline Vector2& Vector2::operator+=(const Vector2& V) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = XMLoadFloat2(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorAdd(v1, v2);
	XMStoreFloat2(this, X);
	return *this;
}

inline Vector2& Vector2::operator-=(const Vector2& V) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = XMLoadFloat2(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorSubtract(v1, v2);
	XMStoreFloat2(this, X);
	return *this;
}

inline Vector2& Vector2::operator*=(const Vector2& V) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = XMLoadFloat2(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorMultiply(v1, v2);
	XMStoreFloat2(this, X);
	return *this;
}

inline Vector2& Vector2::operator*=(float S) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR X = DirectX::XMVectorScale(v1, S);
	XMStoreFloat2(this, X);
	return *this;
}

inline Vector2& Vector2::operator/=(float S) noexcept
{
	assert(S != 0.0f);
	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR X = DirectX::XMVectorScale(v1, 1.f / S);
	XMStoreFloat2(this, X);
	return *this;
}

//------------------------------------------------------------------------------
// Binary operators
//------------------------------------------------------------------------------

inline Vector2 operator+(const Vector2& V1, const Vector2& V2) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&V1);
	DirectX::XMVECTOR v2 = XMLoadFloat2(&V2);
	DirectX::XMVECTOR X = DirectX::XMVectorAdd(v1, v2);
	Vector2 R;
	XMStoreFloat2(&R, X);
	return R;
}

inline Vector2 operator-(const Vector2& V1, const Vector2& V2) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&V1);
	DirectX::XMVECTOR v2 = XMLoadFloat2(&V2);
	DirectX::XMVECTOR X = DirectX::XMVectorSubtract(v1, v2);
	Vector2 R;
	XMStoreFloat2(&R, X);
	return R;
}

inline Vector2 operator*(const Vector2& V1, const Vector2& V2) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&V1);
	DirectX::XMVECTOR v2 = XMLoadFloat2(&V2);
	DirectX::XMVECTOR X = DirectX::XMVectorMultiply(v1, v2);
	Vector2 R;
	XMStoreFloat2(&R, X);
	return R;
}

inline Vector2 operator*(const Vector2& V, float S) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorScale(v1, S);
	Vector2 R;
	XMStoreFloat2(&R, X);
	return R;
}

inline Vector2 operator/(const Vector2& V1, const Vector2& V2) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&V1);
	DirectX::XMVECTOR v2 = XMLoadFloat2(&V2);
	DirectX::XMVECTOR X = DirectX::XMVectorDivide(v1, v2);
	Vector2 R;
	XMStoreFloat2(&R, X);
	return R;
}

inline Vector2 operator/(const Vector2& V, float S) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorScale(v1, 1.f / S);
	Vector2 R;
	XMStoreFloat2(&R, X);
	return R;
}

inline Vector2 operator*(float S, const Vector2& V) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorScale(v1, S);
	Vector2 R;
	XMStoreFloat2(&R, X);
	return R;
}

//------------------------------------------------------------------------------
// Vector operations
//------------------------------------------------------------------------------

inline bool Vector2::InBounds(const Vector2& Bounds) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = XMLoadFloat2(&Bounds);
	return DirectX::XMVector2InBounds(v1, v2);
}

inline float Vector2::Length() const noexcept
{

	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR X = DirectX::XMVector2Length(v1);
	return DirectX::XMVectorGetX(X);
}

inline float Vector2::LengthSquared() const noexcept
{

	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR X = DirectX::XMVector2LengthSq(v1);
	return DirectX::XMVectorGetX(X);
}

inline float Vector2::Dot(const Vector2& V) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = XMLoadFloat2(&V);
	DirectX::XMVECTOR X = DirectX::XMVector2Dot(v1, v2);
	return DirectX::XMVectorGetX(X);
}

inline void Vector2::Cross(const Vector2& V, Vector2& result) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = XMLoadFloat2(&V);
	DirectX::XMVECTOR R = DirectX::XMVector2Cross(v1, v2);
	XMStoreFloat2(&result, R);
}

inline Vector2 Vector2::Cross(const Vector2& V) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = XMLoadFloat2(&V);
	DirectX::XMVECTOR R = DirectX::XMVector2Cross(v1, v2);

	Vector2 result;
	XMStoreFloat2(&result, R);
	return result;
}

inline void Vector2::Normalize() noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR X = DirectX::XMVector2Normalize(v1);
	XMStoreFloat2(this, X);
}

inline void Vector2::Normalize(Vector2& result) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR X = DirectX::XMVector2Normalize(v1);
	XMStoreFloat2(&result, X);
}

inline void Vector2::Clamp(const Vector2& vmin, const Vector2& vmax) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = XMLoadFloat2(&vmin);
	DirectX::XMVECTOR v3 = XMLoadFloat2(&vmax);
	DirectX::XMVECTOR X = DirectX::XMVectorClamp(v1, v2, v3);
	XMStoreFloat2(this, X);
}

inline void Vector2::Clamp(const Vector2& vmin, const Vector2& vmax, Vector2& result) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(this);
	DirectX::XMVECTOR v2 = XMLoadFloat2(&vmin);
	DirectX::XMVECTOR v3 = XMLoadFloat2(&vmax);
	DirectX::XMVECTOR X = DirectX::XMVectorClamp(v1, v2, v3);
	XMStoreFloat2(&result, X);
}

//------------------------------------------------------------------------------
// Static functions
//------------------------------------------------------------------------------

inline float Vector2::Distance(const Vector2& v1, const Vector2& v2) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR V = DirectX::XMVectorSubtract(x2, x1);
	DirectX::XMVECTOR X = DirectX::XMVector2Length(V);
	return DirectX::XMVectorGetX(X);
}

inline float Vector2::DistanceSquared(const Vector2& v1, const Vector2& v2) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR V = DirectX::XMVectorSubtract(x2, x1);
	DirectX::XMVECTOR X = DirectX::XMVector2LengthSq(V);
	return DirectX::XMVectorGetX(X);
}

inline void Vector2::Min(const Vector2& v1, const Vector2& v2, Vector2& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorMin(x1, x2);
	XMStoreFloat2(&result, X);
}

inline Vector2 Vector2::Min(const Vector2& v1, const Vector2& v2) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorMin(x1, x2);

	Vector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void Vector2::Max(const Vector2& v1, const Vector2& v2, Vector2& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorMax(x1, x2);
	XMStoreFloat2(&result, X);
}

inline Vector2 Vector2::Max(const Vector2& v1, const Vector2& v2) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorMax(x1, x2);

	Vector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void Vector2::Lerp(const Vector2& v1, const Vector2& v2, float t, Vector2& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorLerp(x1, x2, t);
	XMStoreFloat2(&result, X);
}

inline Vector2 Vector2::Lerp(const Vector2& v1, const Vector2& v2, float t) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorLerp(x1, x2, t);

	Vector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void Vector2::SmoothStep(const Vector2& v1, const Vector2& v2, float t, Vector2& result) noexcept
{
	t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t); // Clamp value to 0 to 1
	t = t * t * (3.f - 2.f * t);
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorLerp(x1, x2, t);
	XMStoreFloat2(&result, X);
}

inline Vector2 Vector2::SmoothStep(const Vector2& v1, const Vector2& v2, float t) noexcept
{
	t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t); // Clamp value to 0 to 1
	t = t * t * (3.f - 2.f * t);
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorLerp(x1, x2, t);

	Vector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void Vector2::Barycentric(const Vector2& v1, const Vector2& v2, const Vector2& v3, float f, float g, Vector2& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR x3 = XMLoadFloat2(&v3);
	DirectX::XMVECTOR X = DirectX::XMVectorBaryCentric(x1, x2, x3, f, g);
	XMStoreFloat2(&result, X);
}

inline Vector2 Vector2::Barycentric(const Vector2& v1, const Vector2& v2, const Vector2& v3, float f, float g) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR x3 = XMLoadFloat2(&v3);
	DirectX::XMVECTOR X = DirectX::XMVectorBaryCentric(x1, x2, x3, f, g);

	Vector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void Vector2::CatmullRom(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector2& v4, float t, Vector2& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR x3 = XMLoadFloat2(&v3);
	DirectX::XMVECTOR x4 = XMLoadFloat2(&v4);
	DirectX::XMVECTOR X = DirectX::XMVectorCatmullRom(x1, x2, x3, x4, t);
	XMStoreFloat2(&result, X);
}

inline Vector2 Vector2::CatmullRom(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector2& v4, float t) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR x3 = XMLoadFloat2(&v3);
	DirectX::XMVECTOR x4 = XMLoadFloat2(&v4);
	DirectX::XMVECTOR X = DirectX::XMVectorCatmullRom(x1, x2, x3, x4, t);

	Vector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void Vector2::Hermite(const Vector2& v1, const Vector2& t1, const Vector2& v2, const Vector2& t2, float t, Vector2& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&t1);
	DirectX::XMVECTOR x3 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR x4 = XMLoadFloat2(&t2);
	DirectX::XMVECTOR X = DirectX::XMVectorHermite(x1, x2, x3, x4, t);
	XMStoreFloat2(&result, X);
}

inline Vector2 Vector2::Hermite(const Vector2& v1, const Vector2& t1, const Vector2& v2, const Vector2& t2, float t) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat2(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat2(&t1);
	DirectX::XMVECTOR x3 = XMLoadFloat2(&v2);
	DirectX::XMVECTOR x4 = XMLoadFloat2(&t2);
	DirectX::XMVECTOR X = DirectX::XMVectorHermite(x1, x2, x3, x4, t);

	Vector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void Vector2::Reflect(const Vector2& ivec, const Vector2& nvec, Vector2& result) noexcept
{
	DirectX::XMVECTOR i = XMLoadFloat2(&ivec);
	DirectX::XMVECTOR n = XMLoadFloat2(&nvec);
	DirectX::XMVECTOR X = DirectX::XMVector2Reflect(i, n);
	XMStoreFloat2(&result, X);
}

inline Vector2 Vector2::Reflect(const Vector2& ivec, const Vector2& nvec) noexcept
{
	DirectX::XMVECTOR i = XMLoadFloat2(&ivec);
	DirectX::XMVECTOR n = XMLoadFloat2(&nvec);
	DirectX::XMVECTOR X = DirectX::XMVector2Reflect(i, n);

	Vector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void Vector2::Refract(const Vector2& ivec, const Vector2& nvec, float refractionIndex, Vector2& result) noexcept
{
	DirectX::XMVECTOR i = XMLoadFloat2(&ivec);
	DirectX::XMVECTOR n = XMLoadFloat2(&nvec);
	DirectX::XMVECTOR X = DirectX::XMVector2Refract(i, n, refractionIndex);
	XMStoreFloat2(&result, X);
}

inline Vector2 Vector2::Refract(const Vector2& ivec, const Vector2& nvec, float refractionIndex) noexcept
{
	DirectX::XMVECTOR i = XMLoadFloat2(&ivec);
	DirectX::XMVECTOR n = XMLoadFloat2(&nvec);
	DirectX::XMVECTOR X = DirectX::XMVector2Refract(i, n, refractionIndex);

	Vector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void Vector2::Transform(const Vector2& v, const Quaternion& quat, Vector2& result) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&v);
	DirectX::XMVECTOR q = XMLoadFloat4(&quat);
	DirectX::XMVECTOR X = DirectX::XMVector3Rotate(v1, q);
	XMStoreFloat2(&result, X);
}

inline Vector2 Vector2::Transform(const Vector2& v, const Quaternion& quat) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&v);
	DirectX::XMVECTOR q = XMLoadFloat4(&quat);
	DirectX::XMVECTOR X = DirectX::XMVector3Rotate(v1, q);

	Vector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void Vector2::Transform(const Vector2& v, const Matrix& m, Vector2& result) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&v);
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	DirectX::XMVECTOR X = XMVector2TransformCoord(v1, M);
	XMStoreFloat2(&result, X);
}

inline Vector2 Vector2::Transform(const Vector2& v, const Matrix& m) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&v);
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	DirectX::XMVECTOR X = XMVector2TransformCoord(v1, M);

	Vector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

_Use_decl_annotations_ inline void Vector2::Transform(const Vector2* varray, size_t count, const Matrix& m, Vector2* resultArray) noexcept
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	XMVector2TransformCoordStream(resultArray, sizeof(DirectX::XMFLOAT2), varray, sizeof(DirectX::XMFLOAT2), count, M);
}

inline void Vector2::Transform(const Vector2& v, const Matrix& m, Vector4& result) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&v);
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	DirectX::XMVECTOR X = XMVector2Transform(v1, M);
	XMStoreFloat4(&result, X);
}

_Use_decl_annotations_ inline void Vector2::Transform(const Vector2* varray, size_t count, const Matrix& m, Vector4* resultArray) noexcept
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	XMVector2TransformStream(resultArray, sizeof(DirectX::XMFLOAT4), varray, sizeof(DirectX::XMFLOAT2), count, M);
}

inline void Vector2::TransformNormal(const Vector2& v, const Matrix& m, Vector2& result) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&v);
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	DirectX::XMVECTOR X = XMVector2TransformNormal(v1, M);
	XMStoreFloat2(&result, X);
}

inline Vector2 Vector2::TransformNormal(const Vector2& v, const Matrix& m) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&v);
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	DirectX::XMVECTOR X = XMVector2TransformNormal(v1, M);

	Vector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

_Use_decl_annotations_ inline void Vector2::TransformNormal(const Vector2* varray, size_t count, const Matrix& m, Vector2* resultArray) noexcept
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	XMVector2TransformNormalStream(resultArray, sizeof(DirectX::XMFLOAT2), varray, sizeof(DirectX::XMFLOAT2), count, M);
}

/****************************************************************************
 *
 * Vector3
 *
 ****************************************************************************/

//------------------------------------------------------------------------------
// Comparision operators
//------------------------------------------------------------------------------

inline bool Vector3::operator==(const Vector3& V) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR v2 = XMLoadFloat3(&V);
	return DirectX::XMVector3Equal(v1, v2);
}

inline bool Vector3::operator!=(const Vector3& V) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR v2 = XMLoadFloat3(&V);
	return DirectX::XMVector3NotEqual(v1, v2);
}

//------------------------------------------------------------------------------
// Assignment operators
//------------------------------------------------------------------------------

inline Vector3& Vector3::operator+=(const Vector3& V) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR v2 = XMLoadFloat3(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorAdd(v1, v2);
	XMStoreFloat3(this, X);
	return *this;
}

inline Vector3& Vector3::operator-=(const Vector3& V) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR v2 = XMLoadFloat3(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorSubtract(v1, v2);
	XMStoreFloat3(this, X);
	return *this;
}

inline Vector3& Vector3::operator*=(const Vector3& V) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR v2 = XMLoadFloat3(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorMultiply(v1, v2);
	XMStoreFloat3(this, X);
	return *this;
}

inline Vector3& Vector3::operator*=(float S) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR X = DirectX::XMVectorScale(v1, S);
	XMStoreFloat3(this, X);
	return *this;
}

inline Vector3& Vector3::operator/=(float S) noexcept
{
	assert(S != 0.0f);
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR X = DirectX::XMVectorScale(v1, 1.f / S);
	XMStoreFloat3(this, X);
	return *this;
}

//------------------------------------------------------------------------------
// Urnary operators
//------------------------------------------------------------------------------

inline Vector3 Vector3::operator-() const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR X = DirectX::XMVectorNegate(v1);
	Vector3 R;
	XMStoreFloat3(&R, X);
	return R;
}

//------------------------------------------------------------------------------
// Binary operators
//------------------------------------------------------------------------------

inline Vector3 operator+(const Vector3& V1, const Vector3& V2) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&V1);
	DirectX::XMVECTOR v2 = XMLoadFloat3(&V2);
	DirectX::XMVECTOR X = DirectX::XMVectorAdd(v1, v2);
	Vector3 R;
	XMStoreFloat3(&R, X);
	return R;
}

inline Vector3 operator-(const Vector3& V1, const Vector3& V2) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&V1);
	DirectX::XMVECTOR v2 = XMLoadFloat3(&V2);
	DirectX::XMVECTOR X = DirectX::XMVectorSubtract(v1, v2);
	Vector3 R;
	XMStoreFloat3(&R, X);
	return R;
}

inline Vector3 operator*(const Vector3& V1, const Vector3& V2) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&V1);
	DirectX::XMVECTOR v2 = XMLoadFloat3(&V2);
	DirectX::XMVECTOR X = DirectX::XMVectorMultiply(v1, v2);
	Vector3 R;
	XMStoreFloat3(&R, X);
	return R;
}

inline Vector3 operator*(const Vector3& V, float S) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorScale(v1, S);
	Vector3 R;
	XMStoreFloat3(&R, X);
	return R;
}

inline Vector3 operator/(const Vector3& V1, const Vector3& V2) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&V1);
	DirectX::XMVECTOR v2 = XMLoadFloat3(&V2);
	DirectX::XMVECTOR X = DirectX::XMVectorDivide(v1, v2);
	Vector3 R;
	XMStoreFloat3(&R, X);
	return R;
}

inline Vector3 operator/(const Vector3& V, float S) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorScale(v1, 1.f / S);
	Vector3 R;
	XMStoreFloat3(&R, X);
	return R;
}

inline Vector3 operator*(float S, const Vector3& V) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorScale(v1, S);
	Vector3 R;
	XMStoreFloat3(&R, X);
	return R;
}

//------------------------------------------------------------------------------
// Vector operations
//------------------------------------------------------------------------------

inline bool Vector3::InBounds(const Vector3& Bounds) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR v2 = XMLoadFloat3(&Bounds);
	return DirectX::XMVector3InBounds(v1, v2);
}

inline float Vector3::Length() const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR X = DirectX::XMVector3Length(v1);
	return DirectX::XMVectorGetX(X);
}

inline float Vector3::LengthSquared() const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR X = DirectX::XMVector3LengthSq(v1);
	return DirectX::XMVectorGetX(X);
}

inline float Vector3::Dot(const Vector3& V) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR v2 = XMLoadFloat3(&V);
	DirectX::XMVECTOR X = DirectX::XMVector3Dot(v1, v2);
	return DirectX::XMVectorGetX(X);
}

inline void Vector3::Cross(const Vector3& V, Vector3& result) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR v2 = XMLoadFloat3(&V);
	DirectX::XMVECTOR R = DirectX::XMVector3Cross(v1, v2);
	XMStoreFloat3(&result, R);
}

inline Vector3 Vector3::Cross(const Vector3& V) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR v2 = XMLoadFloat3(&V);
	DirectX::XMVECTOR R = DirectX::XMVector3Cross(v1, v2);

	Vector3 result;
	XMStoreFloat3(&result, R);
	return result;
}

inline void Vector3::Normalize() noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR X = DirectX::XMVector3Normalize(v1);
	XMStoreFloat3(this, X);
}

inline void Vector3::Normalize(Vector3& result) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR X = DirectX::XMVector3Normalize(v1);
	XMStoreFloat3(&result, X);
}

inline void Vector3::Clamp(const Vector3& vmin, const Vector3& vmax) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR v2 = XMLoadFloat3(&vmin);
	DirectX::XMVECTOR v3 = XMLoadFloat3(&vmax);
	DirectX::XMVECTOR X = DirectX::XMVectorClamp(v1, v2, v3);
	XMStoreFloat3(this, X);
}

inline void Vector3::Clamp(const Vector3& vmin, const Vector3& vmax, Vector3& result) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(this);
	DirectX::XMVECTOR v2 = XMLoadFloat3(&vmin);
	DirectX::XMVECTOR v3 = XMLoadFloat3(&vmax);
	DirectX::XMVECTOR X = DirectX::XMVectorClamp(v1, v2, v3);
	XMStoreFloat3(&result, X);
}

//------------------------------------------------------------------------------
// Static functions
//------------------------------------------------------------------------------

inline float Vector3::Distance(const Vector3& v1, const Vector3& v2) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR V = DirectX::XMVectorSubtract(x2, x1);
	DirectX::XMVECTOR X = DirectX::XMVector3Length(V);
	return DirectX::XMVectorGetX(X);
}

inline float Vector3::DistanceSquared(const Vector3& v1, const Vector3& v2) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR V = DirectX::XMVectorSubtract(x2, x1);
	DirectX::XMVECTOR X = DirectX::XMVector3LengthSq(V);
	return DirectX::XMVectorGetX(X);
}

inline void Vector3::Min(const Vector3& v1, const Vector3& v2, Vector3& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorMin(x1, x2);
	XMStoreFloat3(&result, X);
}

inline Vector3 Vector3::Min(const Vector3& v1, const Vector3& v2) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorMin(x1, x2);

	Vector3 result;
	XMStoreFloat3(&result, X);
	return result;
}

inline void Vector3::Max(const Vector3& v1, const Vector3& v2, Vector3& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorMax(x1, x2);
	XMStoreFloat3(&result, X);
}

inline Vector3 Vector3::Max(const Vector3& v1, const Vector3& v2) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorMax(x1, x2);

	Vector3 result;
	XMStoreFloat3(&result, X);
	return result;
}

inline void Vector3::Lerp(const Vector3& v1, const Vector3& v2, float t, Vector3& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorLerp(x1, x2, t);
	XMStoreFloat3(&result, X);
}

inline Vector3 Vector3::Lerp(const Vector3& v1, const Vector3& v2, float t) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorLerp(x1, x2, t);

	Vector3 result;
	XMStoreFloat3(&result, X);
	return result;
}

inline void Vector3::SmoothStep(const Vector3& v1, const Vector3& v2, float t, Vector3& result) noexcept
{
	t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t); // Clamp value to 0 to 1
	t = t * t * (3.f - 2.f * t);
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorLerp(x1, x2, t);
	XMStoreFloat3(&result, X);
}

inline Vector3 Vector3::SmoothStep(const Vector3& v1, const Vector3& v2, float t) noexcept
{
	t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t); // Clamp value to 0 to 1
	t = t * t * (3.f - 2.f * t);
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorLerp(x1, x2, t);

	Vector3 result;
	XMStoreFloat3(&result, X);
	return result;
}

inline void Vector3::Barycentric(const Vector3& v1, const Vector3& v2, const Vector3& v3, float f, float g, Vector3& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR x3 = XMLoadFloat3(&v3);
	DirectX::XMVECTOR X = DirectX::XMVectorBaryCentric(x1, x2, x3, f, g);
	XMStoreFloat3(&result, X);
}

inline Vector3 Vector3::Barycentric(const Vector3& v1, const Vector3& v2, const Vector3& v3, float f, float g) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR x3 = XMLoadFloat3(&v3);
	DirectX::XMVECTOR X = DirectX::XMVectorBaryCentric(x1, x2, x3, f, g);

	Vector3 result;
	XMStoreFloat3(&result, X);
	return result;
}

inline void Vector3::CatmullRom(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4, float t, Vector3& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR x3 = XMLoadFloat3(&v3);
	DirectX::XMVECTOR x4 = XMLoadFloat3(&v4);
	DirectX::XMVECTOR X = DirectX::XMVectorCatmullRom(x1, x2, x3, x4, t);
	XMStoreFloat3(&result, X);
}

inline Vector3 Vector3::CatmullRom(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4, float t) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR x3 = XMLoadFloat3(&v3);
	DirectX::XMVECTOR x4 = XMLoadFloat3(&v4);
	DirectX::XMVECTOR X = DirectX::XMVectorCatmullRom(x1, x2, x3, x4, t);

	Vector3 result;
	XMStoreFloat3(&result, X);
	return result;
}

inline void Vector3::Hermite(const Vector3& v1, const Vector3& t1, const Vector3& v2, const Vector3& t2, float t, Vector3& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&t1);
	DirectX::XMVECTOR x3 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR x4 = XMLoadFloat3(&t2);
	DirectX::XMVECTOR X = DirectX::XMVectorHermite(x1, x2, x3, x4, t);
	XMStoreFloat3(&result, X);
}

inline Vector3 Vector3::Hermite(const Vector3& v1, const Vector3& t1, const Vector3& v2, const Vector3& t2, float t) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat3(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat3(&t1);
	DirectX::XMVECTOR x3 = XMLoadFloat3(&v2);
	DirectX::XMVECTOR x4 = XMLoadFloat3(&t2);
	DirectX::XMVECTOR X = DirectX::XMVectorHermite(x1, x2, x3, x4, t);

	Vector3 result;
	XMStoreFloat3(&result, X);
	return result;
}

inline void Vector3::Reflect(const Vector3& ivec, const Vector3& nvec, Vector3& result) noexcept
{
	DirectX::XMVECTOR i = XMLoadFloat3(&ivec);
	DirectX::XMVECTOR n = XMLoadFloat3(&nvec);
	DirectX::XMVECTOR X = DirectX::XMVector3Reflect(i, n);
	XMStoreFloat3(&result, X);
}

inline Vector3 Vector3::Reflect(const Vector3& ivec, const Vector3& nvec) noexcept
{
	DirectX::XMVECTOR i = XMLoadFloat3(&ivec);
	DirectX::XMVECTOR n = XMLoadFloat3(&nvec);
	DirectX::XMVECTOR X = DirectX::XMVector3Reflect(i, n);

	Vector3 result;
	XMStoreFloat3(&result, X);
	return result;
}

inline void Vector3::Refract(const Vector3& ivec, const Vector3& nvec, float refractionIndex, Vector3& result) noexcept
{
	DirectX::XMVECTOR i = XMLoadFloat3(&ivec);
	DirectX::XMVECTOR n = XMLoadFloat3(&nvec);
	DirectX::XMVECTOR X = DirectX::XMVector3Refract(i, n, refractionIndex);
	XMStoreFloat3(&result, X);
}

inline Vector3 Vector3::Refract(const Vector3& ivec, const Vector3& nvec, float refractionIndex) noexcept
{
	DirectX::XMVECTOR i = XMLoadFloat3(&ivec);
	DirectX::XMVECTOR n = XMLoadFloat3(&nvec);
	DirectX::XMVECTOR X = DirectX::XMVector3Refract(i, n, refractionIndex);

	Vector3 result;
	XMStoreFloat3(&result, X);
	return result;
}

inline void Vector3::Transform(const Vector3& v, const Quaternion& quat, Vector3& result) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&v);
	DirectX::XMVECTOR q = XMLoadFloat4(&quat);
	DirectX::XMVECTOR X = DirectX::XMVector3Rotate(v1, q);
	XMStoreFloat3(&result, X);
}

inline Vector3 Vector3::Transform(const Vector3& v, const Quaternion& quat) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&v);
	DirectX::XMVECTOR q = XMLoadFloat4(&quat);
	DirectX::XMVECTOR X = DirectX::XMVector3Rotate(v1, q);

	Vector3 result;
	XMStoreFloat3(&result, X);
	return result;
}

inline void Vector3::Transform(const Vector3& v, const Matrix& m, Vector3& result) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&v);
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	DirectX::XMVECTOR X = XMVector3TransformCoord(v1, M);
	XMStoreFloat3(&result, X);
}

inline Vector3 Vector3::Transform(const Vector3& v, const Matrix& m) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&v);
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	DirectX::XMVECTOR X = XMVector3TransformCoord(v1, M);

	Vector3 result;
	XMStoreFloat3(&result, X);
	return result;
}

_Use_decl_annotations_ inline void Vector3::Transform(const Vector3* varray, size_t count, const Matrix& m, Vector3* resultArray) noexcept
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	XMVector3TransformCoordStream(resultArray, sizeof(DirectX::XMFLOAT3), varray, sizeof(DirectX::XMFLOAT3), count, M);
}

inline void Vector3::Transform(const Vector3& v, const Matrix& m, Vector4& result) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&v);
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	DirectX::XMVECTOR X = XMVector3Transform(v1, M);
	XMStoreFloat4(&result, X);
}

_Use_decl_annotations_ inline void Vector3::Transform(const Vector3* varray, size_t count, const Matrix& m, Vector4* resultArray) noexcept
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	XMVector3TransformStream(resultArray, sizeof(DirectX::XMFLOAT4), varray, sizeof(DirectX::XMFLOAT3), count, M);
}

inline void Vector3::TransformNormal(const Vector3& v, const Matrix& m, Vector3& result) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&v);
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	DirectX::XMVECTOR X = XMVector3TransformNormal(v1, M);
	XMStoreFloat3(&result, X);
}

inline Vector3 Vector3::TransformNormal(const Vector3& v, const Matrix& m) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&v);
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	DirectX::XMVECTOR X = XMVector3TransformNormal(v1, M);

	Vector3 result;
	XMStoreFloat3(&result, X);
	return result;
}

_Use_decl_annotations_ inline void Vector3::TransformNormal(const Vector3* varray, size_t count, const Matrix& m, Vector3* resultArray) noexcept
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	XMVector3TransformNormalStream(resultArray, sizeof(DirectX::XMFLOAT3), varray, sizeof(DirectX::XMFLOAT3), count, M);
}

/****************************************************************************
 *
 * Vector4
 *
 ****************************************************************************/

//------------------------------------------------------------------------------
// Comparision operators
//------------------------------------------------------------------------------

inline bool Vector4::operator==(const Vector4& V) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = XMLoadFloat4(&V);
	return DirectX::XMVector4Equal(v1, v2);
}

inline bool Vector4::operator!=(const Vector4& V) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = XMLoadFloat4(&V);
	return DirectX::XMVector4NotEqual(v1, v2);
}

//------------------------------------------------------------------------------
// Assignment operators
//------------------------------------------------------------------------------

inline Vector4& Vector4::operator+=(const Vector4& V) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = XMLoadFloat4(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorAdd(v1, v2);
	XMStoreFloat4(this, X);
	return *this;
}

inline Vector4& Vector4::operator-=(const Vector4& V) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = XMLoadFloat4(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorSubtract(v1, v2);
	XMStoreFloat4(this, X);
	return *this;
}

inline Vector4& Vector4::operator*=(const Vector4& V) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = XMLoadFloat4(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorMultiply(v1, v2);
	XMStoreFloat4(this, X);
	return *this;
}

inline Vector4& Vector4::operator*=(float S) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR X = DirectX::XMVectorScale(v1, S);
	XMStoreFloat4(this, X);
	return *this;
}

inline Vector4& Vector4::operator/=(float S) noexcept
{
	assert(S != 0.0f);
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR X = DirectX::XMVectorScale(v1, 1.f / S);
	XMStoreFloat4(this, X);
	return *this;
}

//------------------------------------------------------------------------------
// Urnary operators
//------------------------------------------------------------------------------

inline Vector4 Vector4::operator-() const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR X = DirectX::XMVectorNegate(v1);
	Vector4 R;
	XMStoreFloat4(&R, X);
	return R;
}

//------------------------------------------------------------------------------
// Binary operators
//------------------------------------------------------------------------------

inline Vector4 operator+(const Vector4& V1, const Vector4& V2) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(&V1);
	DirectX::XMVECTOR v2 = XMLoadFloat4(&V2);
	DirectX::XMVECTOR X = DirectX::XMVectorAdd(v1, v2);
	Vector4 R;
	XMStoreFloat4(&R, X);
	return R;
}

inline Vector4 operator-(const Vector4& V1, const Vector4& V2) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(&V1);
	DirectX::XMVECTOR v2 = XMLoadFloat4(&V2);
	DirectX::XMVECTOR X = DirectX::XMVectorSubtract(v1, v2);
	Vector4 R;
	XMStoreFloat4(&R, X);
	return R;
}

inline Vector4 operator*(const Vector4& V1, const Vector4& V2) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(&V1);
	DirectX::XMVECTOR v2 = XMLoadFloat4(&V2);
	DirectX::XMVECTOR X = DirectX::XMVectorMultiply(v1, v2);
	Vector4 R;
	XMStoreFloat4(&R, X);
	return R;
}

inline Vector4 operator*(const Vector4& V, float S) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorScale(v1, S);
	Vector4 R;
	XMStoreFloat4(&R, X);
	return R;
}

inline Vector4 operator/(const Vector4& V1, const Vector4& V2) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(&V1);
	DirectX::XMVECTOR v2 = XMLoadFloat4(&V2);
	DirectX::XMVECTOR X = DirectX::XMVectorDivide(v1, v2);
	Vector4 R;
	XMStoreFloat4(&R, X);
	return R;
}

inline Vector4 operator/(const Vector4& V, float S) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorScale(v1, 1.f / S);
	Vector4 R;
	XMStoreFloat4(&R, X);
	return R;
}

inline Vector4 operator*(float S, const Vector4& V) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(&V);
	DirectX::XMVECTOR X = DirectX::XMVectorScale(v1, S);
	Vector4 R;
	XMStoreFloat4(&R, X);
	return R;
}

//------------------------------------------------------------------------------
// Vector operations
//------------------------------------------------------------------------------

inline bool Vector4::InBounds(const Vector4& Bounds) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = XMLoadFloat4(&Bounds);
	return DirectX::XMVector4InBounds(v1, v2);
}

inline float Vector4::Length() const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR X = DirectX::XMVector4Length(v1);
	return DirectX::XMVectorGetX(X);
}

inline float Vector4::LengthSquared() const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR X = DirectX::XMVector4LengthSq(v1);
	return DirectX::XMVectorGetX(X);
}

inline float Vector4::Dot(const Vector4& V) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = XMLoadFloat4(&V);
	DirectX::XMVECTOR X = DirectX::XMVector4Dot(v1, v2);
	return DirectX::XMVectorGetX(X);
}

inline void Vector4::Cross(const Vector4& v1, const Vector4& v2, Vector4& result) const noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(this);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x3 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR R = DirectX::XMVector4Cross(x1, x2, x3);
	XMStoreFloat4(&result, R);
}

inline Vector4 Vector4::Cross(const Vector4& v1, const Vector4& v2) const noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(this);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x3 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR R = DirectX::XMVector4Cross(x1, x2, x3);

	Vector4 result;
	XMStoreFloat4(&result, R);
	return result;
}

inline void Vector4::Normalize() noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR X = DirectX::XMVector4Normalize(v1);
	XMStoreFloat4(this, X);
}

inline void Vector4::Normalize(Vector4& result) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR X = DirectX::XMVector4Normalize(v1);
	XMStoreFloat4(&result, X);
}

inline void Vector4::Clamp(const Vector4& vmin, const Vector4& vmax) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = XMLoadFloat4(&vmin);
	DirectX::XMVECTOR v3 = XMLoadFloat4(&vmax);
	DirectX::XMVECTOR X = DirectX::XMVectorClamp(v1, v2, v3);
	XMStoreFloat4(this, X);
}

inline void Vector4::Clamp(const Vector4& vmin, const Vector4& vmax, Vector4& result) const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(this);
	DirectX::XMVECTOR v2 = XMLoadFloat4(&vmin);
	DirectX::XMVECTOR v3 = XMLoadFloat4(&vmax);
	DirectX::XMVECTOR X = DirectX::XMVectorClamp(v1, v2, v3);
	XMStoreFloat4(&result, X);
}

//------------------------------------------------------------------------------
// Static functions
//------------------------------------------------------------------------------

inline float Vector4::Distance(const Vector4& v1, const Vector4& v2) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR V = DirectX::XMVectorSubtract(x2, x1);
	DirectX::XMVECTOR X = DirectX::XMVector4Length(V);
	return DirectX::XMVectorGetX(X);
}

inline float Vector4::DistanceSquared(const Vector4& v1, const Vector4& v2) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR V = DirectX::XMVectorSubtract(x2, x1);
	DirectX::XMVECTOR X = DirectX::XMVector4LengthSq(V);
	return DirectX::XMVectorGetX(X);
}

inline void Vector4::Min(const Vector4& v1, const Vector4& v2, Vector4& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorMin(x1, x2);
	XMStoreFloat4(&result, X);
}

inline Vector4 Vector4::Min(const Vector4& v1, const Vector4& v2) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorMin(x1, x2);

	Vector4 result;
	XMStoreFloat4(&result, X);
	return result;
}

inline void Vector4::Max(const Vector4& v1, const Vector4& v2, Vector4& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorMax(x1, x2);
	XMStoreFloat4(&result, X);
}

inline Vector4 Vector4::Max(const Vector4& v1, const Vector4& v2) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorMax(x1, x2);

	Vector4 result;
	XMStoreFloat4(&result, X);
	return result;
}

inline void Vector4::Lerp(const Vector4& v1, const Vector4& v2, float t, Vector4& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorLerp(x1, x2, t);
	XMStoreFloat4(&result, X);
}

inline Vector4 Vector4::Lerp(const Vector4& v1, const Vector4& v2, float t) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorLerp(x1, x2, t);

	Vector4 result;
	XMStoreFloat4(&result, X);
	return result;
}

inline void Vector4::SmoothStep(const Vector4& v1, const Vector4& v2, float t, Vector4& result) noexcept
{
	t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t); // Clamp value to 0 to 1
	t = t * t * (3.f - 2.f * t);
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorLerp(x1, x2, t);
	XMStoreFloat4(&result, X);
}

inline Vector4 Vector4::SmoothStep(const Vector4& v1, const Vector4& v2, float t) noexcept
{
	t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t); // Clamp value to 0 to 1
	t = t * t * (3.f - 2.f * t);
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR X = DirectX::XMVectorLerp(x1, x2, t);

	Vector4 result;
	XMStoreFloat4(&result, X);
	return result;
}

inline void Vector4::Barycentric(const Vector4& v1, const Vector4& v2, const Vector4& v3, float f, float g, Vector4& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR x3 = XMLoadFloat4(&v3);
	DirectX::XMVECTOR X = DirectX::XMVectorBaryCentric(x1, x2, x3, f, g);
	XMStoreFloat4(&result, X);
}

inline Vector4 Vector4::Barycentric(const Vector4& v1, const Vector4& v2, const Vector4& v3, float f, float g) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR x3 = XMLoadFloat4(&v3);
	DirectX::XMVECTOR X = DirectX::XMVectorBaryCentric(x1, x2, x3, f, g);

	Vector4 result;
	XMStoreFloat4(&result, X);
	return result;
}

inline void Vector4::CatmullRom(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4, float t, Vector4& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR x3 = XMLoadFloat4(&v3);
	DirectX::XMVECTOR x4 = XMLoadFloat4(&v4);
	DirectX::XMVECTOR X = DirectX::XMVectorCatmullRom(x1, x2, x3, x4, t);
	XMStoreFloat4(&result, X);
}

inline Vector4 Vector4::CatmullRom(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4, float t) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR x3 = XMLoadFloat4(&v3);
	DirectX::XMVECTOR x4 = XMLoadFloat4(&v4);
	DirectX::XMVECTOR X = DirectX::XMVectorCatmullRom(x1, x2, x3, x4, t);

	Vector4 result;
	XMStoreFloat4(&result, X);
	return result;
}

inline void Vector4::Hermite(const Vector4& v1, const Vector4& t1, const Vector4& v2, const Vector4& t2, float t, Vector4& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&t1);
	DirectX::XMVECTOR x3 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR x4 = XMLoadFloat4(&t2);
	DirectX::XMVECTOR X = DirectX::XMVectorHermite(x1, x2, x3, x4, t);
	XMStoreFloat4(&result, X);
}

inline Vector4 Vector4::Hermite(const Vector4& v1, const Vector4& t1, const Vector4& v2, const Vector4& t2, float t) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR x2 = XMLoadFloat4(&t1);
	DirectX::XMVECTOR x3 = XMLoadFloat4(&v2);
	DirectX::XMVECTOR x4 = XMLoadFloat4(&t2);
	DirectX::XMVECTOR X = DirectX::XMVectorHermite(x1, x2, x3, x4, t);

	Vector4 result;
	XMStoreFloat4(&result, X);
	return result;
}

inline void Vector4::Reflect(const Vector4& ivec, const Vector4& nvec, Vector4& result) noexcept
{
	DirectX::XMVECTOR i = XMLoadFloat4(&ivec);
	DirectX::XMVECTOR n = XMLoadFloat4(&nvec);
	DirectX::XMVECTOR X = DirectX::XMVector4Reflect(i, n);
	XMStoreFloat4(&result, X);
}

inline Vector4 Vector4::Reflect(const Vector4& ivec, const Vector4& nvec) noexcept
{
	DirectX::XMVECTOR i = XMLoadFloat4(&ivec);
	DirectX::XMVECTOR n = XMLoadFloat4(&nvec);
	DirectX::XMVECTOR X = DirectX::XMVector4Reflect(i, n);

	Vector4 result;
	XMStoreFloat4(&result, X);
	return result;
}

inline void Vector4::Refract(const Vector4& ivec, const Vector4& nvec, float refractionIndex, Vector4& result) noexcept
{
	DirectX::XMVECTOR i = XMLoadFloat4(&ivec);
	DirectX::XMVECTOR n = XMLoadFloat4(&nvec);
	DirectX::XMVECTOR X = DirectX::XMVector4Refract(i, n, refractionIndex);
	XMStoreFloat4(&result, X);
}

inline Vector4 Vector4::Refract(const Vector4& ivec, const Vector4& nvec, float refractionIndex) noexcept
{
	DirectX::XMVECTOR i = XMLoadFloat4(&ivec);
	DirectX::XMVECTOR n = XMLoadFloat4(&nvec);
	DirectX::XMVECTOR X = DirectX::XMVector4Refract(i, n, refractionIndex);

	Vector4 result;
	XMStoreFloat4(&result, X);
	return result;
}

inline void Vector4::Transform(const Vector2& v, const Quaternion& quat, Vector4& result) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&v);
	DirectX::XMVECTOR q = XMLoadFloat4(&quat);
	DirectX::XMVECTOR X = DirectX::XMVector3Rotate(v1, q);
	X = DirectX::XMVectorSelect(DirectX::g_XMIdentityR3, X, DirectX::g_XMSelect1110); // result.w = 1.f
	XMStoreFloat4(&result, X);
}

inline Vector4 Vector4::Transform(const Vector2& v, const Quaternion& quat) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat2(&v);
	DirectX::XMVECTOR q = XMLoadFloat4(&quat);
	DirectX::XMVECTOR X = DirectX::XMVector3Rotate(v1, q);
	X = DirectX::XMVectorSelect(DirectX::g_XMIdentityR3, X, DirectX::g_XMSelect1110); // result.w = 1.f

	Vector4 result;
	XMStoreFloat4(&result, X);
	return result;
}

inline void Vector4::Transform(const Vector3& v, const Quaternion& quat, Vector4& result) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&v);
	DirectX::XMVECTOR q = XMLoadFloat4(&quat);
	DirectX::XMVECTOR X = DirectX::XMVector3Rotate(v1, q);
	X = DirectX::XMVectorSelect(DirectX::g_XMIdentityR3, X, DirectX::g_XMSelect1110); // result.w = 1.f
	XMStoreFloat4(&result, X);
}

inline Vector4 Vector4::Transform(const Vector3& v, const Quaternion& quat) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat3(&v);
	DirectX::XMVECTOR q = XMLoadFloat4(&quat);
	DirectX::XMVECTOR X = DirectX::XMVector3Rotate(v1, q);
	X = DirectX::XMVectorSelect(DirectX::g_XMIdentityR3, X, DirectX::g_XMSelect1110); // result.w = 1.f

	Vector4 result;
	XMStoreFloat4(&result, X);
	return result;
}

inline void Vector4::Transform(const Vector4& v, const Quaternion& quat, Vector4& result) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(&v);
	DirectX::XMVECTOR q = XMLoadFloat4(&quat);
	DirectX::XMVECTOR X = DirectX::XMVector3Rotate(v1, q);
	X = DirectX::XMVectorSelect(v1, X, DirectX::g_XMSelect1110); // result.w = v.w
	XMStoreFloat4(&result, X);
}

inline Vector4 Vector4::Transform(const Vector4& v, const Quaternion& quat) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(&v);
	DirectX::XMVECTOR q = XMLoadFloat4(&quat);
	DirectX::XMVECTOR X = DirectX::XMVector3Rotate(v1, q);
	X = DirectX::XMVectorSelect(v1, X, DirectX::g_XMSelect1110); // result.w = v.w

	Vector4 result;
	XMStoreFloat4(&result, X);
	return result;
}

inline void Vector4::Transform(const Vector4& v, const Matrix& m, Vector4& result) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(&v);
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	DirectX::XMVECTOR X = XMVector4Transform(v1, M);
	XMStoreFloat4(&result, X);
}

inline Vector4 Vector4::Transform(const Vector4& v, const Matrix& m) noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(&v);
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	DirectX::XMVECTOR X = XMVector4Transform(v1, M);

	Vector4 result;
	XMStoreFloat4(&result, X);
	return result;
}

_Use_decl_annotations_ inline void Vector4::Transform(const Vector4* varray, size_t count, const Matrix& m, Vector4* resultArray) noexcept
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
	XMVector4TransformStream(resultArray, sizeof(DirectX::XMFLOAT4), varray, sizeof(DirectX::XMFLOAT4), count, M);
}

/****************************************************************************
 *
 * Matrix
 *
 ****************************************************************************/

//------------------------------------------------------------------------------
// Comparision operators
//------------------------------------------------------------------------------

inline bool Matrix::operator==(const Matrix& M) const noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	DirectX::XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._11));
	DirectX::XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._21));
	DirectX::XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._31));
	DirectX::XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._41));

	return (DirectX::XMVector4Equal(x1, y1)
			   && DirectX::XMVector4Equal(x2, y2)
			   && DirectX::XMVector4Equal(x3, y3)
			   && DirectX::XMVector4Equal(x4, y4))
		!= 0;
}

inline bool Matrix::operator!=(const Matrix& M) const noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	DirectX::XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._11));
	DirectX::XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._21));
	DirectX::XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._31));
	DirectX::XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._41));

	return (DirectX::XMVector4NotEqual(x1, y1)
			   || DirectX::XMVector4NotEqual(x2, y2)
			   || DirectX::XMVector4NotEqual(x3, y3)
			   || DirectX::XMVector4NotEqual(x4, y4))
		!= 0;
}

//------------------------------------------------------------------------------
// Assignment operators
//------------------------------------------------------------------------------

inline Matrix::Matrix(const DirectX::XMFLOAT3X3& M) noexcept
{
	_11 = M._11;
	_12 = M._12;
	_13 = M._13;
	_14 = 0.f;
	_21 = M._21;
	_22 = M._22;
	_23 = M._23;
	_24 = 0.f;
	_31 = M._31;
	_32 = M._32;
	_33 = M._33;
	_34 = 0.f;
	_41 = 0.f;
	_42 = 0.f;
	_43 = 0.f;
	_44 = 1.f;
}

inline Matrix::Matrix(const DirectX::XMFLOAT4X3& M) noexcept
{
	_11 = M._11;
	_12 = M._12;
	_13 = M._13;
	_14 = 0.f;
	_21 = M._21;
	_22 = M._22;
	_23 = M._23;
	_24 = 0.f;
	_31 = M._31;
	_32 = M._32;
	_33 = M._33;
	_34 = 0.f;
	_41 = M._41;
	_42 = M._42;
	_43 = M._43;
	_44 = 1.f;
}

inline Matrix& Matrix::operator=(const DirectX::XMFLOAT3X3& M) noexcept
{
	_11 = M._11;
	_12 = M._12;
	_13 = M._13;
	_14 = 0.f;
	_21 = M._21;
	_22 = M._22;
	_23 = M._23;
	_24 = 0.f;
	_31 = M._31;
	_32 = M._32;
	_33 = M._33;
	_34 = 0.f;
	_41 = 0.f;
	_42 = 0.f;
	_43 = 0.f;
	_44 = 1.f;
	return *this;
}

inline Matrix& Matrix::operator=(const DirectX::XMFLOAT4X3& M) noexcept
{
	_11 = M._11;
	_12 = M._12;
	_13 = M._13;
	_14 = 0.f;
	_21 = M._21;
	_22 = M._22;
	_23 = M._23;
	_24 = 0.f;
	_31 = M._31;
	_32 = M._32;
	_33 = M._33;
	_34 = 0.f;
	_41 = M._41;
	_42 = M._42;
	_43 = M._43;
	_44 = 1.f;
	return *this;
}

inline Matrix& Matrix::operator+=(const Matrix& M) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	DirectX::XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._11));
	DirectX::XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._21));
	DirectX::XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._31));
	DirectX::XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._41));

	x1 = DirectX::XMVectorAdd(x1, y1);
	x2 = DirectX::XMVectorAdd(x2, y2);
	x3 = DirectX::XMVectorAdd(x3, y3);
	x4 = DirectX::XMVectorAdd(x4, y4);

	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_11), x1);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_21), x2);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_31), x3);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_41), x4);
	return *this;
}

inline Matrix& Matrix::operator-=(const Matrix& M) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	DirectX::XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._11));
	DirectX::XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._21));
	DirectX::XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._31));
	DirectX::XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._41));

	x1 = DirectX::XMVectorSubtract(x1, y1);
	x2 = DirectX::XMVectorSubtract(x2, y2);
	x3 = DirectX::XMVectorSubtract(x3, y3);
	x4 = DirectX::XMVectorSubtract(x4, y4);

	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_11), x1);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_21), x2);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_31), x3);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_41), x4);
	return *this;
}

inline Matrix& Matrix::operator*=(const Matrix& M) noexcept
{
	DirectX::XMMATRIX M1 = DirectX::XMLoadFloat4x4(this);
	DirectX::XMMATRIX M2 = DirectX::XMLoadFloat4x4(&M);
	DirectX::XMMATRIX X = XMMatrixMultiply(M1, M2);
	XMStoreFloat4x4(this, X);
	return *this;
}

inline Matrix& Matrix::operator*=(float S) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	x1 = DirectX::XMVectorScale(x1, S);
	x2 = DirectX::XMVectorScale(x2, S);
	x3 = DirectX::XMVectorScale(x3, S);
	x4 = DirectX::XMVectorScale(x4, S);

	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_11), x1);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_21), x2);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_31), x3);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_41), x4);
	return *this;
}

inline Matrix& Matrix::operator/=(float S) noexcept
{
	assert(S != 0.f);
	DirectX::XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	float rs = 1.f / S;

	x1 = DirectX::XMVectorScale(x1, rs);
	x2 = DirectX::XMVectorScale(x2, rs);
	x3 = DirectX::XMVectorScale(x3, rs);
	x4 = DirectX::XMVectorScale(x4, rs);

	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_11), x1);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_21), x2);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_31), x3);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_41), x4);
	return *this;
}

inline Matrix& Matrix::operator/=(const Matrix& M) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	DirectX::XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._11));
	DirectX::XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._21));
	DirectX::XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._31));
	DirectX::XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._41));

	x1 = DirectX::XMVectorDivide(x1, y1);
	x2 = DirectX::XMVectorDivide(x2, y2);
	x3 = DirectX::XMVectorDivide(x3, y3);
	x4 = DirectX::XMVectorDivide(x4, y4);

	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_11), x1);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_21), x2);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_31), x3);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&_41), x4);
	return *this;
}

//------------------------------------------------------------------------------
// Urnary operators
//------------------------------------------------------------------------------

inline Matrix Matrix::operator-() const noexcept
{
	DirectX::XMVECTOR v1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_11));
	DirectX::XMVECTOR v2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_21));
	DirectX::XMVECTOR v3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_31));
	DirectX::XMVECTOR v4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&_41));

	v1 = DirectX::XMVectorNegate(v1);
	v2 = DirectX::XMVectorNegate(v2);
	v3 = DirectX::XMVectorNegate(v3);
	v4 = DirectX::XMVectorNegate(v4);

	Matrix R;
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._11), v1);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._21), v2);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._31), v3);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._41), v4);
	return R;
}

//------------------------------------------------------------------------------
// Binary operators
//------------------------------------------------------------------------------

inline Matrix operator+(const Matrix& M1, const Matrix& M2) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._11));
	DirectX::XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._21));
	DirectX::XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._31));
	DirectX::XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._41));

	DirectX::XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._11));
	DirectX::XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._21));
	DirectX::XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._31));
	DirectX::XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._41));

	x1 = DirectX::XMVectorAdd(x1, y1);
	x2 = DirectX::XMVectorAdd(x2, y2);
	x3 = DirectX::XMVectorAdd(x3, y3);
	x4 = DirectX::XMVectorAdd(x4, y4);

	Matrix R;
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._11), x1);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._21), x2);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._31), x3);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._41), x4);
	return R;
}

inline Matrix operator-(const Matrix& M1, const Matrix& M2) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._11));
	DirectX::XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._21));
	DirectX::XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._31));
	DirectX::XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._41));

	DirectX::XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._11));
	DirectX::XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._21));
	DirectX::XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._31));
	DirectX::XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._41));

	x1 = DirectX::XMVectorSubtract(x1, y1);
	x2 = DirectX::XMVectorSubtract(x2, y2);
	x3 = DirectX::XMVectorSubtract(x3, y3);
	x4 = DirectX::XMVectorSubtract(x4, y4);

	Matrix R;
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._11), x1);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._21), x2);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._31), x3);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._41), x4);
	return R;
}

inline Matrix operator*(const Matrix& M1, const Matrix& M2) noexcept
{
	DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(&M1);
	DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(&M2);
	DirectX::XMMATRIX X = XMMatrixMultiply(m1, m2);

	Matrix R;
	XMStoreFloat4x4(&R, X);
	return R;
}

inline Matrix operator*(const Matrix& M, float S) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._11));
	DirectX::XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._21));
	DirectX::XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._31));
	DirectX::XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._41));

	x1 = DirectX::XMVectorScale(x1, S);
	x2 = DirectX::XMVectorScale(x2, S);
	x3 = DirectX::XMVectorScale(x3, S);
	x4 = DirectX::XMVectorScale(x4, S);

	Matrix R;
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._11), x1);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._21), x2);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._31), x3);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._41), x4);
	return R;
}

inline Matrix operator/(const Matrix& M, float S) noexcept
{
	assert(S != 0.f);

	DirectX::XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._11));
	DirectX::XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._21));
	DirectX::XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._31));
	DirectX::XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._41));

	float rs = 1.f / S;

	x1 = DirectX::XMVectorScale(x1, rs);
	x2 = DirectX::XMVectorScale(x2, rs);
	x3 = DirectX::XMVectorScale(x3, rs);
	x4 = DirectX::XMVectorScale(x4, rs);

	Matrix R;
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._11), x1);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._21), x2);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._31), x3);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._41), x4);
	return R;
}

inline Matrix operator/(const Matrix& M1, const Matrix& M2) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._11));
	DirectX::XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._21));
	DirectX::XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._31));
	DirectX::XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._41));

	DirectX::XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._11));
	DirectX::XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._21));
	DirectX::XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._31));
	DirectX::XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._41));

	x1 = DirectX::XMVectorDivide(x1, y1);
	x2 = DirectX::XMVectorDivide(x2, y2);
	x3 = DirectX::XMVectorDivide(x3, y3);
	x4 = DirectX::XMVectorDivide(x4, y4);

	Matrix R;
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._11), x1);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._21), x2);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._31), x3);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._41), x4);
	return R;
}

inline Matrix operator*(float S, const Matrix& M) noexcept
{

	DirectX::XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._11));
	DirectX::XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._21));
	DirectX::XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._31));
	DirectX::XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M._41));

	x1 = DirectX::XMVectorScale(x1, S);
	x2 = DirectX::XMVectorScale(x2, S);
	x3 = DirectX::XMVectorScale(x3, S);
	x4 = DirectX::XMVectorScale(x4, S);

	Matrix R;
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._11), x1);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._21), x2);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._31), x3);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&R._41), x4);
	return R;
}

//------------------------------------------------------------------------------
// Matrix operations
//------------------------------------------------------------------------------

inline bool Matrix::Decompose(Vector3& scale, Quaternion& rotation, Vector3& translation) noexcept
{

	DirectX::XMVECTOR s, r, t;

	if (!XMMatrixDecompose(&s, &r, &t, *this))
		return false;

	XMStoreFloat3(&scale, s);
	XMStoreFloat4(&rotation, r);
	XMStoreFloat3(&translation, t);

	return true;
}

inline Matrix Matrix::Transpose() const noexcept
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(this);
	Matrix R;
	XMStoreFloat4x4(&R, XMMatrixTranspose(M));
	return R;
}

inline void Matrix::Transpose(Matrix& result) const noexcept
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(this);
	XMStoreFloat4x4(&result, XMMatrixTranspose(M));
}

inline Matrix Matrix::Invert() const noexcept
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(this);
	Matrix R;
	DirectX::XMVECTOR det;
	XMStoreFloat4x4(&R, XMMatrixInverse(&det, M));
	return R;
}

inline void Matrix::Invert(Matrix& result) const noexcept
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(this);
	DirectX::XMVECTOR det;
	XMStoreFloat4x4(&result, XMMatrixInverse(&det, M));
}

inline float Matrix::Determinant() const noexcept
{
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(this);
	return DirectX::XMVectorGetX(XMMatrixDeterminant(M));
}

//------------------------------------------------------------------------------
// Static functions
//------------------------------------------------------------------------------

_Use_decl_annotations_ inline Matrix Matrix::CreateBillboard(
	const Vector3& object,
	const Vector3& cameraPosition,
	const Vector3& cameraUp,
	const Vector3* cameraForward) noexcept
{
	DirectX::XMVECTOR O = XMLoadFloat3(&object);
	DirectX::XMVECTOR C = XMLoadFloat3(&cameraPosition);
	DirectX::XMVECTOR Z = DirectX::XMVectorSubtract(O, C);

	DirectX::XMVECTOR N = DirectX::XMVector3LengthSq(Z);
	if (XMVector3Less(N, DirectX::g_XMEpsilon))
	{
		if (cameraForward)
		{
			DirectX::XMVECTOR F = XMLoadFloat3(cameraForward);
			Z = DirectX::XMVectorNegate(F);
		}
		else
			Z = DirectX::g_XMNegIdentityR2;
	}
	else
	{
		Z = DirectX::XMVector3Normalize(Z);
	}

	DirectX::XMVECTOR up = XMLoadFloat3(&cameraUp);
	DirectX::XMVECTOR X = DirectX::XMVector3Cross(up, Z);
	X = DirectX::XMVector3Normalize(X);

	DirectX::XMVECTOR Y = DirectX::XMVector3Cross(Z, X);

	DirectX::XMMATRIX M;
	M.r[0] = X;
	M.r[1] = Y;
	M.r[2] = Z;
	M.r[3] = DirectX::XMVectorSetW(O, 1.f);

	Matrix R;
	XMStoreFloat4x4(&R, M);
	return R;
}

_Use_decl_annotations_ inline Matrix Matrix::CreateConstrainedBillboard(
	const Vector3& object,
	const Vector3& cameraPosition,
	const Vector3& rotateAxis,
	const Vector3* cameraForward,
	const Vector3* objectForward) noexcept
{

	static const DirectX::XMVECTORF32 s_minAngle = {{{0.99825467075f, 0.99825467075f, 0.99825467075f, 0.99825467075f}}}; // 1.0 - XMConvertToRadians( 0.1f );

	DirectX::XMVECTOR O = XMLoadFloat3(&object);
	DirectX::XMVECTOR C = XMLoadFloat3(&cameraPosition);
	DirectX::XMVECTOR faceDir = DirectX::XMVectorSubtract(O, C);

	DirectX::XMVECTOR N = DirectX::XMVector3LengthSq(faceDir);
	if (XMVector3Less(N, DirectX::g_XMEpsilon))
	{
		if (cameraForward)
		{
			DirectX::XMVECTOR F = XMLoadFloat3(cameraForward);
			faceDir = DirectX::XMVectorNegate(F);
		}
		else
			faceDir = DirectX::g_XMNegIdentityR2;
	}
	else
	{
		faceDir = DirectX::XMVector3Normalize(faceDir);
	}

	DirectX::XMVECTOR Y = XMLoadFloat3(&rotateAxis);
	DirectX::XMVECTOR X, Z;

	DirectX::XMVECTOR dot = DirectX::XMVectorAbs(DirectX::XMVector3Dot(Y, faceDir));
	if (XMVector3Greater(dot, s_minAngle))
	{
		if (objectForward)
		{
			Z = XMLoadFloat3(objectForward);
			dot = DirectX::XMVectorAbs(DirectX::XMVector3Dot(Y, Z));
			if (XMVector3Greater(dot, s_minAngle))
			{
				dot = DirectX::XMVectorAbs(DirectX::XMVector3Dot(Y, DirectX::g_XMNegIdentityR2));
				Z = (XMVector3Greater(dot, s_minAngle)) ? DirectX::g_XMIdentityR0 : DirectX::g_XMNegIdentityR2;
			}
		}
		else
		{
			dot = DirectX::XMVectorAbs(DirectX::XMVector3Dot(Y, DirectX::g_XMNegIdentityR2));
			Z = (XMVector3Greater(dot, s_minAngle)) ? DirectX::g_XMIdentityR0 : DirectX::g_XMNegIdentityR2;
		}

		X = DirectX::XMVector3Cross(Y, Z);
		X = DirectX::XMVector3Normalize(X);

		Z = DirectX::XMVector3Cross(X, Y);
		Z = DirectX::XMVector3Normalize(Z);
	}
	else
	{
		X = DirectX::XMVector3Cross(Y, faceDir);
		X = DirectX::XMVector3Normalize(X);

		Z = DirectX::XMVector3Cross(X, Y);
		Z = DirectX::XMVector3Normalize(Z);
	}

	DirectX::XMMATRIX M;
	M.r[0] = X;
	M.r[1] = Y;
	M.r[2] = Z;
	M.r[3] = DirectX::XMVectorSetW(O, 1.f);

	Matrix R;
	XMStoreFloat4x4(&R, M);
	return R;
}

inline Matrix Matrix::CreateTranslation(const Vector3& position) noexcept
{
	Matrix R;
	XMStoreFloat4x4(&R, DirectX::XMMatrixTranslation(position.x, position.y, position.z));
	return R;
}

inline Matrix Matrix::CreateTranslation(float x, float y, float z) noexcept
{
	Matrix R;
	XMStoreFloat4x4(&R, DirectX::XMMatrixTranslation(x, y, z));
	return R;
}

inline Matrix Matrix::CreateScale(const Vector3& scales) noexcept
{
	Matrix R;
	XMStoreFloat4x4(&R, DirectX::XMMatrixScaling(scales.x, scales.y, scales.z));
	return R;
}

inline Matrix Matrix::CreateScale(float xs, float ys, float zs) noexcept
{
	Matrix R;
	XMStoreFloat4x4(&R, DirectX::XMMatrixScaling(xs, ys, zs));
	return R;
}

inline Matrix Matrix::CreateScale(float scale) noexcept
{
	Matrix R;
	XMStoreFloat4x4(&R, DirectX::XMMatrixScaling(scale, scale, scale));
	return R;
}

inline Matrix Matrix::CreateRotationX(float radians) noexcept
{
	Matrix R;
	XMStoreFloat4x4(&R, DirectX::XMMatrixRotationX(radians));
	return R;
}

inline Matrix Matrix::CreateRotationY(float radians) noexcept
{
	Matrix R;
	XMStoreFloat4x4(&R, DirectX::XMMatrixRotationY(radians));
	return R;
}

inline Matrix Matrix::CreateRotationZ(float radians) noexcept
{
	Matrix R;
	XMStoreFloat4x4(&R, DirectX::XMMatrixRotationZ(radians));
	return R;
}

inline Matrix Matrix::CreateFromAxisAngle(const Vector3& axis, float angle) noexcept
{
	Matrix R;
	DirectX::XMVECTOR a = XMLoadFloat3(&axis);
	XMStoreFloat4x4(&R, DirectX::XMMatrixRotationAxis(a, angle));
	return R;
}

inline Matrix Matrix::CreatePerspectiveFieldOfView(float fov, float aspectRatio, float nearPlane, float farPlane) noexcept
{
	Matrix R;
	XMStoreFloat4x4(&R, DirectX::XMMatrixPerspectiveFovRH(fov, aspectRatio, nearPlane, farPlane));
	return R;
}

inline Matrix Matrix::CreatePerspective(float width, float height, float nearPlane, float farPlane) noexcept
{
	Matrix R;
	XMStoreFloat4x4(&R, DirectX::XMMatrixPerspectiveRH(width, height, nearPlane, farPlane));
	return R;
}

inline Matrix Matrix::CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlane, float farPlane) noexcept
{
	Matrix R;
	XMStoreFloat4x4(&R, DirectX::XMMatrixPerspectiveOffCenterRH(left, right, bottom, top, nearPlane, farPlane));
	return R;
}

inline Matrix Matrix::CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane) noexcept
{
	Matrix R;
	XMStoreFloat4x4(&R, DirectX::XMMatrixOrthographicRH(width, height, zNearPlane, zFarPlane));
	return R;
}

inline Matrix Matrix::CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane) noexcept
{
	Matrix R;
	XMStoreFloat4x4(&R, DirectX::XMMatrixOrthographicOffCenterRH(left, right, bottom, top, zNearPlane, zFarPlane));
	return R;
}

inline Matrix Matrix::CreateLookAt(const Vector3& eye, const Vector3& target, const Vector3& up) noexcept
{
	Matrix R;
	DirectX::XMVECTOR eyev = XMLoadFloat3(&eye);
	DirectX::XMVECTOR targetv = XMLoadFloat3(&target);
	DirectX::XMVECTOR upv = XMLoadFloat3(&up);
	XMStoreFloat4x4(&R, DirectX::XMMatrixLookAtRH(eyev, targetv, upv));
	return R;
}

inline Matrix Matrix::CreateWorld(const Vector3& position, const Vector3& forward, const Vector3& up) noexcept
{
	DirectX::XMVECTOR zaxis = DirectX::XMVector3Normalize(DirectX::XMVectorNegate(XMLoadFloat3(&forward)));
	DirectX::XMVECTOR yaxis = XMLoadFloat3(&up);
	DirectX::XMVECTOR xaxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(yaxis, zaxis));
	yaxis = DirectX::XMVector3Cross(zaxis, xaxis);

	Matrix R;
	XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&R._11), xaxis);
	XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&R._21), yaxis);
	XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&R._31), zaxis);
	R._14 = R._24 = R._34 = 0.f;
	R._41 = position.x;
	R._42 = position.y;
	R._43 = position.z;
	R._44 = 1.f;
	return R;
}

inline Matrix Matrix::CreateFromQuaternion(const Quaternion& rotation) noexcept
{
	Matrix R;
	DirectX::XMVECTOR quatv = XMLoadFloat4(&rotation);
	XMStoreFloat4x4(&R, DirectX::XMMatrixRotationQuaternion(quatv));
	return R;
}

inline Matrix Matrix::CreateFromYawPitchRoll(float yaw, float pitch, float roll) noexcept
{
	Matrix R;
	XMStoreFloat4x4(&R, DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll));
	return R;
}

inline Matrix Matrix::CreateShadow(const Vector3& lightDir, const Plane& plane) noexcept
{
	Matrix R;
	DirectX::XMVECTOR light = XMLoadFloat3(&lightDir);
	DirectX::XMVECTOR planev = XMLoadFloat4(&plane);
	XMStoreFloat4x4(&R, DirectX::XMMatrixShadow(planev, light));
	return R;
}

inline Matrix Matrix::CreateReflection(const Plane& plane) noexcept
{
	Matrix R;
	DirectX::XMVECTOR planev = XMLoadFloat4(&plane);
	XMStoreFloat4x4(&R, DirectX::XMMatrixReflect(planev));
	return R;
}

inline void Matrix::Lerp(const Matrix& M1, const Matrix& M2, float t, Matrix& result) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._11));
	DirectX::XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._21));
	DirectX::XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._31));
	DirectX::XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._41));

	DirectX::XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._11));
	DirectX::XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._21));
	DirectX::XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._31));
	DirectX::XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._41));

	x1 = DirectX::XMVectorLerp(x1, y1, t);
	x2 = DirectX::XMVectorLerp(x2, y2, t);
	x3 = DirectX::XMVectorLerp(x3, y3, t);
	x4 = DirectX::XMVectorLerp(x4, y4, t);

	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&result._11), x1);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&result._21), x2);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&result._31), x3);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&result._41), x4);
}

inline Matrix Matrix::Lerp(const Matrix& M1, const Matrix& M2, float t) noexcept
{
	DirectX::XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._11));
	DirectX::XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._21));
	DirectX::XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._31));
	DirectX::XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M1._41));

	DirectX::XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._11));
	DirectX::XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._21));
	DirectX::XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._31));
	DirectX::XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&M2._41));

	x1 = DirectX::XMVectorLerp(x1, y1, t);
	x2 = DirectX::XMVectorLerp(x2, y2, t);
	x3 = DirectX::XMVectorLerp(x3, y3, t);
	x4 = DirectX::XMVectorLerp(x4, y4, t);

	Matrix result;
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&result._11), x1);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&result._21), x2);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&result._31), x3);
	XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&result._41), x4);
	return result;
}

inline void Matrix::Transform(const Matrix& M, const Quaternion& rotation, Matrix& result) noexcept
{
	DirectX::XMVECTOR quatv = XMLoadFloat4(&rotation);

	DirectX::XMMATRIX M0 = DirectX::XMLoadFloat4x4(&M);
	DirectX::XMMATRIX M1 = DirectX::XMMatrixRotationQuaternion(quatv);

	XMStoreFloat4x4(&result, XMMatrixMultiply(M0, M1));
}

inline Matrix Matrix::Transform(const Matrix& M, const Quaternion& rotation) noexcept
{
	DirectX::XMVECTOR quatv = XMLoadFloat4(&rotation);

	DirectX::XMMATRIX M0 = DirectX::XMLoadFloat4x4(&M);
	DirectX::XMMATRIX M1 = DirectX::XMMatrixRotationQuaternion(quatv);

	Matrix result;
	XMStoreFloat4x4(&result, XMMatrixMultiply(M0, M1));
	return result;
}

/****************************************************************************
 *
 * Plane
 *
 ****************************************************************************/

inline Plane::Plane(const Vector3& point1, const Vector3& point2, const Vector3& point3) noexcept
{
	DirectX::XMVECTOR P0 = XMLoadFloat3(&point1);
	DirectX::XMVECTOR P1 = XMLoadFloat3(&point2);
	DirectX::XMVECTOR P2 = XMLoadFloat3(&point3);
	XMStoreFloat4(this, DirectX::XMPlaneFromPoints(P0, P1, P2));
}

inline Plane::Plane(const Vector3& point, const Vector3& normal) noexcept
{
	DirectX::XMVECTOR P = XMLoadFloat3(&point);
	DirectX::XMVECTOR N = XMLoadFloat3(&normal);
	XMStoreFloat4(this, DirectX::XMPlaneFromPointNormal(P, N));
}

//------------------------------------------------------------------------------
// Comparision operators
//------------------------------------------------------------------------------

inline bool Plane::operator==(const Plane& p) const noexcept
{
	DirectX::XMVECTOR p1 = XMLoadFloat4(this);
	DirectX::XMVECTOR p2 = XMLoadFloat4(&p);
	return DirectX::XMPlaneEqual(p1, p2);
}

inline bool Plane::operator!=(const Plane& p) const noexcept
{
	DirectX::XMVECTOR p1 = XMLoadFloat4(this);
	DirectX::XMVECTOR p2 = XMLoadFloat4(&p);
	return DirectX::XMPlaneNotEqual(p1, p2);
}

//------------------------------------------------------------------------------
// Plane operations
//------------------------------------------------------------------------------

inline void Plane::Normalize() noexcept
{
	DirectX::XMVECTOR p = XMLoadFloat4(this);
	XMStoreFloat4(this, DirectX::XMPlaneNormalize(p));
}

inline void Plane::Normalize(Plane& result) const noexcept
{
	DirectX::XMVECTOR p = XMLoadFloat4(this);
	XMStoreFloat4(&result, DirectX::XMPlaneNormalize(p));
}

inline float Plane::Dot(const Vector4& v) const noexcept
{
	DirectX::XMVECTOR p = XMLoadFloat4(this);
	DirectX::XMVECTOR v0 = XMLoadFloat4(&v);
	return DirectX::XMVectorGetX(DirectX::XMPlaneDot(p, v0));
}

inline float Plane::DotCoordinate(const Vector3& position) const noexcept
{
	DirectX::XMVECTOR p = XMLoadFloat4(this);
	DirectX::XMVECTOR v0 = XMLoadFloat3(&position);
	return DirectX::XMVectorGetX(DirectX::XMPlaneDotCoord(p, v0));
}

inline float Plane::DotNormal(const Vector3& normal) const noexcept
{
	DirectX::XMVECTOR p = XMLoadFloat4(this);
	DirectX::XMVECTOR n0 = XMLoadFloat3(&normal);
	return DirectX::XMVectorGetX(DirectX::XMPlaneDotNormal(p, n0));
}

//------------------------------------------------------------------------------
// Static functions
//------------------------------------------------------------------------------

inline void Plane::Transform(const Plane& plane, const Matrix& M, Plane& result) noexcept
{
	DirectX::XMVECTOR p = XMLoadFloat4(&plane);
	DirectX::XMMATRIX m0 = DirectX::XMLoadFloat4x4(&M);
	XMStoreFloat4(&result, XMPlaneTransform(p, m0));
}

inline Plane Plane::Transform(const Plane& plane, const Matrix& M) noexcept
{
	DirectX::XMVECTOR p = XMLoadFloat4(&plane);
	DirectX::XMMATRIX m0 = DirectX::XMLoadFloat4x4(&M);

	Plane result;
	XMStoreFloat4(&result, XMPlaneTransform(p, m0));
	return result;
}

inline void Plane::Transform(const Plane& plane, const Quaternion& rotation, Plane& result) noexcept
{
	DirectX::XMVECTOR p = XMLoadFloat4(&plane);
	DirectX::XMVECTOR q = XMLoadFloat4(&rotation);
	DirectX::XMVECTOR X = DirectX::XMVector3Rotate(p, q);
	X = DirectX::XMVectorSelect(p, X, DirectX::g_XMSelect1110); // result.d = plane.d
	XMStoreFloat4(&result, X);
}

inline Plane Plane::Transform(const Plane& plane, const Quaternion& rotation) noexcept
{
	DirectX::XMVECTOR p = XMLoadFloat4(&plane);
	DirectX::XMVECTOR q = XMLoadFloat4(&rotation);
	DirectX::XMVECTOR X = DirectX::XMVector3Rotate(p, q);
	X = DirectX::XMVectorSelect(p, X, DirectX::g_XMSelect1110); // result.d = plane.d

	Plane result;
	XMStoreFloat4(&result, X);
	return result;
}

/****************************************************************************
 *
 * Quaternion
 *
 ****************************************************************************/

//------------------------------------------------------------------------------
// Comparision operators
//------------------------------------------------------------------------------

inline bool Quaternion::operator==(const Quaternion& q) const noexcept
{
	DirectX::XMVECTOR q1 = XMLoadFloat4(this);
	DirectX::XMVECTOR q2 = XMLoadFloat4(&q);
	return DirectX::XMQuaternionEqual(q1, q2);
}

inline bool Quaternion::operator!=(const Quaternion& q) const noexcept
{
	DirectX::XMVECTOR q1 = XMLoadFloat4(this);
	DirectX::XMVECTOR q2 = XMLoadFloat4(&q);
	return DirectX::XMQuaternionNotEqual(q1, q2);
}

//------------------------------------------------------------------------------
// Assignment operators
//------------------------------------------------------------------------------

inline Quaternion& Quaternion::operator+=(const Quaternion& q) noexcept
{
	DirectX::XMVECTOR q1 = XMLoadFloat4(this);
	DirectX::XMVECTOR q2 = XMLoadFloat4(&q);
	XMStoreFloat4(this, DirectX::XMVectorAdd(q1, q2));
	return *this;
}

inline Quaternion& Quaternion::operator-=(const Quaternion& q) noexcept
{
	DirectX::XMVECTOR q1 = XMLoadFloat4(this);
	DirectX::XMVECTOR q2 = XMLoadFloat4(&q);
	XMStoreFloat4(this, DirectX::XMVectorSubtract(q1, q2));
	return *this;
}

inline Quaternion& Quaternion::operator*=(const Quaternion& q) noexcept
{
	DirectX::XMVECTOR q1 = XMLoadFloat4(this);
	DirectX::XMVECTOR q2 = XMLoadFloat4(&q);
	XMStoreFloat4(this, DirectX::XMQuaternionMultiply(q1, q2));
	return *this;
}

inline Quaternion& Quaternion::operator*=(float S) noexcept
{
	DirectX::XMVECTOR q = XMLoadFloat4(this);
	XMStoreFloat4(this, DirectX::XMVectorScale(q, S));
	return *this;
}

inline Quaternion& Quaternion::operator/=(const Quaternion& q) noexcept
{
	DirectX::XMVECTOR q1 = XMLoadFloat4(this);
	DirectX::XMVECTOR q2 = XMLoadFloat4(&q);
	q2 = DirectX::XMQuaternionInverse(q2);
	XMStoreFloat4(this, DirectX::XMQuaternionMultiply(q1, q2));
	return *this;
}

//------------------------------------------------------------------------------
// Urnary operators
//------------------------------------------------------------------------------

inline Quaternion Quaternion::operator-() const noexcept
{
	DirectX::XMVECTOR q = XMLoadFloat4(this);

	Quaternion R;
	XMStoreFloat4(&R, DirectX::XMVectorNegate(q));
	return R;
}

//------------------------------------------------------------------------------
// Binary operators
//------------------------------------------------------------------------------

inline Quaternion operator+(const Quaternion& Q1, const Quaternion& Q2) noexcept
{
	DirectX::XMVECTOR q1 = XMLoadFloat4(&Q1);
	DirectX::XMVECTOR q2 = XMLoadFloat4(&Q2);

	Quaternion R;
	XMStoreFloat4(&R, DirectX::XMVectorAdd(q1, q2));
	return R;
}

inline Quaternion operator-(const Quaternion& Q1, const Quaternion& Q2) noexcept
{
	DirectX::XMVECTOR q1 = XMLoadFloat4(&Q1);
	DirectX::XMVECTOR q2 = XMLoadFloat4(&Q2);

	Quaternion R;
	XMStoreFloat4(&R, DirectX::XMVectorSubtract(q1, q2));
	return R;
}

inline Quaternion operator*(const Quaternion& Q1, const Quaternion& Q2) noexcept
{
	DirectX::XMVECTOR q1 = XMLoadFloat4(&Q1);
	DirectX::XMVECTOR q2 = XMLoadFloat4(&Q2);

	Quaternion R;
	XMStoreFloat4(&R, DirectX::XMQuaternionMultiply(q1, q2));
	return R;
}

inline Quaternion operator*(const Quaternion& Q, float S) noexcept
{
	DirectX::XMVECTOR q = XMLoadFloat4(&Q);

	Quaternion R;
	XMStoreFloat4(&R, DirectX::XMVectorScale(q, S));
	return R;
}

inline Quaternion operator/(const Quaternion& Q1, const Quaternion& Q2) noexcept
{
	DirectX::XMVECTOR q1 = XMLoadFloat4(&Q1);
	DirectX::XMVECTOR q2 = XMLoadFloat4(&Q2);
	q2 = DirectX::XMQuaternionInverse(q2);

	Quaternion R;
	XMStoreFloat4(&R, DirectX::XMQuaternionMultiply(q1, q2));
	return R;
}

inline Quaternion operator*(float S, const Quaternion& Q) noexcept
{
	DirectX::XMVECTOR q1 = XMLoadFloat4(&Q);

	Quaternion R;
	XMStoreFloat4(&R, DirectX::XMVectorScale(q1, S));
	return R;
}

//------------------------------------------------------------------------------
// Quaternion operations
//------------------------------------------------------------------------------

inline float Quaternion::Length() const noexcept
{
	DirectX::XMVECTOR q = XMLoadFloat4(this);
	return DirectX::XMVectorGetX(DirectX::XMQuaternionLength(q));
}

inline float Quaternion::LengthSquared() const noexcept
{
	DirectX::XMVECTOR q = XMLoadFloat4(this);
	return DirectX::XMVectorGetX(DirectX::XMQuaternionLengthSq(q));
}

inline void Quaternion::Normalize() noexcept
{
	DirectX::XMVECTOR q = XMLoadFloat4(this);
	XMStoreFloat4(this, DirectX::XMQuaternionNormalize(q));
}

inline void Quaternion::Normalize(Quaternion& result) const noexcept
{
	DirectX::XMVECTOR q = XMLoadFloat4(this);
	XMStoreFloat4(&result, DirectX::XMQuaternionNormalize(q));
}

inline void Quaternion::Conjugate() noexcept
{
	DirectX::XMVECTOR q = XMLoadFloat4(this);
	XMStoreFloat4(this, DirectX::XMQuaternionConjugate(q));
}

inline void Quaternion::Conjugate(Quaternion& result) const noexcept
{
	DirectX::XMVECTOR q = XMLoadFloat4(this);
	XMStoreFloat4(&result, DirectX::XMQuaternionConjugate(q));
}

inline void Quaternion::Inverse(Quaternion& result) const noexcept
{
	DirectX::XMVECTOR q = XMLoadFloat4(this);
	XMStoreFloat4(&result, DirectX::XMQuaternionInverse(q));
}

inline float Quaternion::Dot(const Quaternion& q) const noexcept
{
	DirectX::XMVECTOR q1 = XMLoadFloat4(this);
	DirectX::XMVECTOR q2 = XMLoadFloat4(&q);
	return DirectX::XMVectorGetX(DirectX::XMQuaternionDot(q1, q2));
}

//------------------------------------------------------------------------------
// Static functions
//------------------------------------------------------------------------------

inline Quaternion Quaternion::CreateFromAxisAngle(const Vector3& axis, float angle) noexcept
{
	DirectX::XMVECTOR a = XMLoadFloat3(&axis);

	Quaternion R;
	XMStoreFloat4(&R, DirectX::XMQuaternionRotationAxis(a, angle));
	return R;
}

inline Quaternion Quaternion::CreateFromYawPitchRoll(float yaw, float pitch, float roll) noexcept
{
	Quaternion R;
	XMStoreFloat4(&R, DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
	return R;
}

inline Quaternion Quaternion::CreateFromRotationMatrix(const Matrix& M) noexcept
{
	DirectX::XMMATRIX M0 = DirectX::XMLoadFloat4x4(&M);

	Quaternion R;
	XMStoreFloat4(&R, XMQuaternionRotationMatrix(M0));
	return R;
}

inline void Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result) noexcept
{
	DirectX::XMVECTOR Q0 = XMLoadFloat4(&q1);
	DirectX::XMVECTOR Q1 = XMLoadFloat4(&q2);

	DirectX::XMVECTOR dot = DirectX::XMVector4Dot(Q0, Q1);

	DirectX::XMVECTOR R;
	if (DirectX::XMVector4GreaterOrEqual(dot, DirectX::XMVectorZero()))
	{
		R = DirectX::XMVectorLerp(Q0, Q1, t);
	}
	else
	{
		DirectX::XMVECTOR tv = DirectX::XMVectorReplicate(t);
		DirectX::XMVECTOR t1v = DirectX::XMVectorReplicate(1.f - t);
		DirectX::XMVECTOR X0 = DirectX::XMVectorMultiply(Q0, t1v);
		DirectX::XMVECTOR X1 = DirectX::XMVectorMultiply(Q1, tv);
		R = DirectX::XMVectorSubtract(X0, X1);
	}

	XMStoreFloat4(&result, DirectX::XMQuaternionNormalize(R));
}

inline Quaternion Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float t) noexcept
{
	DirectX::XMVECTOR Q0 = XMLoadFloat4(&q1);
	DirectX::XMVECTOR Q1 = XMLoadFloat4(&q2);

	DirectX::XMVECTOR dot = DirectX::XMVector4Dot(Q0, Q1);

	DirectX::XMVECTOR R;
	if (DirectX::XMVector4GreaterOrEqual(dot, DirectX::XMVectorZero()))
	{
		R = DirectX::XMVectorLerp(Q0, Q1, t);
	}
	else
	{
		DirectX::XMVECTOR tv = DirectX::XMVectorReplicate(t);
		DirectX::XMVECTOR t1v = DirectX::XMVectorReplicate(1.f - t);
		DirectX::XMVECTOR X0 = DirectX::XMVectorMultiply(Q0, t1v);
		DirectX::XMVECTOR X1 = DirectX::XMVectorMultiply(Q1, tv);
		R = DirectX::XMVectorSubtract(X0, X1);
	}

	Quaternion result;
	XMStoreFloat4(&result, DirectX::XMQuaternionNormalize(R));
	return result;
}

inline void Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result) noexcept
{
	DirectX::XMVECTOR Q0 = XMLoadFloat4(&q1);
	DirectX::XMVECTOR Q1 = XMLoadFloat4(&q2);
	XMStoreFloat4(&result, DirectX::XMQuaternionSlerp(Q0, Q1, t));
}

inline Quaternion Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t) noexcept
{
	DirectX::XMVECTOR Q0 = XMLoadFloat4(&q1);
	DirectX::XMVECTOR Q1 = XMLoadFloat4(&q2);

	Quaternion result;
	XMStoreFloat4(&result, DirectX::XMQuaternionSlerp(Q0, Q1, t));
	return result;
}

inline void Quaternion::Concatenate(const Quaternion& q1, const Quaternion& q2, Quaternion& result) noexcept
{
	DirectX::XMVECTOR Q0 = XMLoadFloat4(&q1);
	DirectX::XMVECTOR Q1 = XMLoadFloat4(&q2);
	XMStoreFloat4(&result, DirectX::XMQuaternionMultiply(Q1, Q0));
}

inline Quaternion Quaternion::Concatenate(const Quaternion& q1, const Quaternion& q2) noexcept
{
	DirectX::XMVECTOR Q0 = XMLoadFloat4(&q1);
	DirectX::XMVECTOR Q1 = XMLoadFloat4(&q2);

	Quaternion result;
	XMStoreFloat4(&result, DirectX::XMQuaternionMultiply(Q1, Q0));
	return result;
}

/****************************************************************************
 *
 * Color
 *
 ****************************************************************************/

inline Color::Color(const DirectX::PackedVector::XMCOLOR& Packed) noexcept
{
	XMStoreFloat4(this, DirectX::PackedVector::XMLoadColor(&Packed));
}

inline Color::Color(const DirectX::PackedVector::XMUBYTEN4& Packed) noexcept
{
	XMStoreFloat4(this, DirectX::PackedVector::XMLoadUByteN4(&Packed));
}

//------------------------------------------------------------------------------
// Comparision operators
//------------------------------------------------------------------------------
inline bool Color::operator==(const Color& c) const noexcept
{
	DirectX::XMVECTOR c1 = XMLoadFloat4(this);
	DirectX::XMVECTOR c2 = XMLoadFloat4(&c);
	return DirectX::XMColorEqual(c1, c2);
}

inline bool Color::operator!=(const Color& c) const noexcept
{
	DirectX::XMVECTOR c1 = XMLoadFloat4(this);
	DirectX::XMVECTOR c2 = XMLoadFloat4(&c);
	return DirectX::XMColorNotEqual(c1, c2);
}

//------------------------------------------------------------------------------
// Assignment operators
//------------------------------------------------------------------------------

inline Color& Color::operator=(const DirectX::PackedVector::XMCOLOR& Packed) noexcept
{
	XMStoreFloat4(this, DirectX::PackedVector::XMLoadColor(&Packed));
	return *this;
}

inline Color& Color::operator=(const DirectX::PackedVector::XMUBYTEN4& Packed) noexcept
{
	XMStoreFloat4(this, DirectX::PackedVector::XMLoadUByteN4(&Packed));
	return *this;
}

inline Color& Color::operator+=(const Color& c) noexcept
{
	DirectX::XMVECTOR c1 = XMLoadFloat4(this);
	DirectX::XMVECTOR c2 = XMLoadFloat4(&c);
	XMStoreFloat4(this, DirectX::XMVectorAdd(c1, c2));
	return *this;
}

inline Color& Color::operator-=(const Color& c) noexcept
{
	DirectX::XMVECTOR c1 = XMLoadFloat4(this);
	DirectX::XMVECTOR c2 = XMLoadFloat4(&c);
	XMStoreFloat4(this, DirectX::XMVectorSubtract(c1, c2));
	return *this;
}

inline Color& Color::operator*=(const Color& c) noexcept
{
	DirectX::XMVECTOR c1 = XMLoadFloat4(this);
	DirectX::XMVECTOR c2 = XMLoadFloat4(&c);
	XMStoreFloat4(this, DirectX::XMVectorMultiply(c1, c2));
	return *this;
}

inline Color& Color::operator*=(float S) noexcept
{
	DirectX::XMVECTOR c = XMLoadFloat4(this);
	XMStoreFloat4(this, DirectX::XMVectorScale(c, S));
	return *this;
}

inline Color& Color::operator/=(const Color& c) noexcept
{
	DirectX::XMVECTOR c1 = XMLoadFloat4(this);
	DirectX::XMVECTOR c2 = XMLoadFloat4(&c);
	XMStoreFloat4(this, DirectX::XMVectorDivide(c1, c2));
	return *this;
}

//------------------------------------------------------------------------------
// Urnary operators
//------------------------------------------------------------------------------

inline Color Color::operator-() const noexcept
{
	DirectX::XMVECTOR c = XMLoadFloat4(this);
	Color R;
	XMStoreFloat4(&R, DirectX::XMVectorNegate(c));
	return R;
}

//------------------------------------------------------------------------------
// Binary operators
//------------------------------------------------------------------------------

inline Color operator+(const Color& C1, const Color& C2) noexcept
{
	DirectX::XMVECTOR c1 = XMLoadFloat4(&C1);
	DirectX::XMVECTOR c2 = XMLoadFloat4(&C2);
	Color R;
	XMStoreFloat4(&R, DirectX::XMVectorAdd(c1, c2));
	return R;
}

inline Color operator-(const Color& C1, const Color& C2) noexcept
{
	DirectX::XMVECTOR c1 = XMLoadFloat4(&C1);
	DirectX::XMVECTOR c2 = XMLoadFloat4(&C2);
	Color R;
	XMStoreFloat4(&R, DirectX::XMVectorSubtract(c1, c2));
	return R;
}

inline Color operator*(const Color& C1, const Color& C2) noexcept
{
	DirectX::XMVECTOR c1 = XMLoadFloat4(&C1);
	DirectX::XMVECTOR c2 = XMLoadFloat4(&C2);
	Color R;
	XMStoreFloat4(&R, DirectX::XMVectorMultiply(c1, c2));
	return R;
}

inline Color operator*(const Color& C, float S) noexcept
{
	DirectX::XMVECTOR c = XMLoadFloat4(&C);
	Color R;
	XMStoreFloat4(&R, DirectX::XMVectorScale(c, S));
	return R;
}

inline Color operator/(const Color& C1, const Color& C2) noexcept
{
	DirectX::XMVECTOR c1 = XMLoadFloat4(&C1);
	DirectX::XMVECTOR c2 = XMLoadFloat4(&C2);
	Color R;
	XMStoreFloat4(&R, DirectX::XMVectorDivide(c1, c2));
	return R;
}

inline Color operator*(float S, const Color& C) noexcept
{
	DirectX::XMVECTOR c1 = XMLoadFloat4(&C);
	Color R;
	XMStoreFloat4(&R, DirectX::XMVectorScale(c1, S));
	return R;
}

//------------------------------------------------------------------------------
// Color operations
//------------------------------------------------------------------------------

inline DirectX::PackedVector::XMCOLOR Color::BGRA() const noexcept
{
	DirectX::XMVECTOR clr = XMLoadFloat4(this);
	DirectX::PackedVector::XMCOLOR Packed;
	DirectX::PackedVector::XMStoreColor(&Packed, clr);
	return Packed;
}

inline DirectX::PackedVector::XMUBYTEN4 Color::RGBA() const noexcept
{
	DirectX::XMVECTOR clr = XMLoadFloat4(this);
	DirectX::PackedVector::XMUBYTEN4 Packed;
	DirectX::PackedVector::XMStoreUByteN4(&Packed, clr);
	return Packed;
}

inline Vector3 Color::ToVector3() const noexcept
{
	return Vector3(x, y, z);
}

inline Vector4 Color::ToVector4() const noexcept
{
	return Vector4(x, y, z, w);
}

inline void Color::Negate() noexcept
{
	DirectX::XMVECTOR c = XMLoadFloat4(this);
	XMStoreFloat4(this, DirectX::XMColorNegative(c));
}

inline void Color::Negate(Color& result) const noexcept
{
	DirectX::XMVECTOR c = XMLoadFloat4(this);
	XMStoreFloat4(&result, DirectX::XMColorNegative(c));
}

inline void Color::Saturate() noexcept
{
	DirectX::XMVECTOR c = XMLoadFloat4(this);
	XMStoreFloat4(this, DirectX::XMVectorSaturate(c));
}

inline void Color::Saturate(Color& result) const noexcept
{
	DirectX::XMVECTOR c = XMLoadFloat4(this);
	XMStoreFloat4(&result, DirectX::XMVectorSaturate(c));
}

inline void Color::Premultiply() noexcept
{
	DirectX::XMVECTOR c = XMLoadFloat4(this);
	DirectX::XMVECTOR a = DirectX::XMVectorSplatW(c);
	a = DirectX::XMVectorSelect(DirectX::g_XMIdentityR3, a, DirectX::g_XMSelect1110);
	XMStoreFloat4(this, DirectX::XMVectorMultiply(c, a));
}

inline void Color::Premultiply(Color& result) const noexcept
{
	DirectX::XMVECTOR c = XMLoadFloat4(this);
	DirectX::XMVECTOR a = DirectX::XMVectorSplatW(c);
	a = DirectX::XMVectorSelect(DirectX::g_XMIdentityR3, a, DirectX::g_XMSelect1110);
	XMStoreFloat4(&result, DirectX::XMVectorMultiply(c, a));
}

inline void Color::AdjustSaturation(float sat) noexcept
{
	DirectX::XMVECTOR c = XMLoadFloat4(this);
	XMStoreFloat4(this, DirectX::XMColorAdjustSaturation(c, sat));
}

inline void Color::AdjustSaturation(float sat, Color& result) const noexcept
{
	DirectX::XMVECTOR c = XMLoadFloat4(this);
	XMStoreFloat4(&result, DirectX::XMColorAdjustSaturation(c, sat));
}

inline void Color::AdjustContrast(float contrast) noexcept
{
	DirectX::XMVECTOR c = XMLoadFloat4(this);
	XMStoreFloat4(this, DirectX::XMColorAdjustContrast(c, contrast));
}

inline void Color::AdjustContrast(float contrast, Color& result) const noexcept
{
	DirectX::XMVECTOR c = XMLoadFloat4(this);
	XMStoreFloat4(&result, DirectX::XMColorAdjustContrast(c, contrast));
}

//------------------------------------------------------------------------------
// Static functions
//------------------------------------------------------------------------------

inline void Color::Modulate(const Color& c1, const Color& c2, Color& result) noexcept
{
	DirectX::XMVECTOR C0 = XMLoadFloat4(&c1);
	DirectX::XMVECTOR C1 = XMLoadFloat4(&c2);
	XMStoreFloat4(&result, DirectX::XMColorModulate(C0, C1));
}

inline Color Color::Modulate(const Color& c1, const Color& c2) noexcept
{
	DirectX::XMVECTOR C0 = XMLoadFloat4(&c1);
	DirectX::XMVECTOR C1 = XMLoadFloat4(&c2);

	Color result;
	XMStoreFloat4(&result, DirectX::XMColorModulate(C0, C1));
	return result;
}

inline void Color::Lerp(const Color& c1, const Color& c2, float t, Color& result) noexcept
{
	DirectX::XMVECTOR C0 = XMLoadFloat4(&c1);
	DirectX::XMVECTOR C1 = XMLoadFloat4(&c2);
	XMStoreFloat4(&result, DirectX::XMVectorLerp(C0, C1, t));
}

inline Color Color::Lerp(const Color& c1, const Color& c2, float t) noexcept
{
	DirectX::XMVECTOR C0 = XMLoadFloat4(&c1);
	DirectX::XMVECTOR C1 = XMLoadFloat4(&c2);

	Color result;
	XMStoreFloat4(&result, DirectX::XMVectorLerp(C0, C1, t));
	return result;
}

/****************************************************************************
 *
 * Ray
 *
 ****************************************************************************/

//-----------------------------------------------------------------------------
// Comparision operators
//------------------------------------------------------------------------------
inline bool Ray::operator==(const Ray& r) const noexcept
{
	DirectX::XMVECTOR r1p = XMLoadFloat3(&position);
	DirectX::XMVECTOR r2p = XMLoadFloat3(&r.position);
	DirectX::XMVECTOR r1d = XMLoadFloat3(&direction);
	DirectX::XMVECTOR r2d = XMLoadFloat3(&r.direction);
	return DirectX::XMVector3Equal(r1p, r2p) && DirectX::XMVector3Equal(r1d, r2d);
}

inline bool Ray::operator!=(const Ray& r) const noexcept
{
	DirectX::XMVECTOR r1p = XMLoadFloat3(&position);
	DirectX::XMVECTOR r2p = XMLoadFloat3(&r.position);
	DirectX::XMVECTOR r1d = XMLoadFloat3(&direction);
	DirectX::XMVECTOR r2d = XMLoadFloat3(&r.direction);
	return DirectX::XMVector3NotEqual(r1p, r2p) && DirectX::XMVector3NotEqual(r1d, r2d);
}

//-----------------------------------------------------------------------------
// Ray operators
//------------------------------------------------------------------------------

inline bool Ray::Intersects(const DirectX::BoundingSphere& sphere, _Out_ float& Dist) const noexcept
{
	return sphere.Intersects(position, direction, Dist);
}

inline bool Ray::Intersects(const DirectX::BoundingBox& box, _Out_ float& Dist) const noexcept
{
	return box.Intersects(position, direction, Dist);
}

inline bool Ray::Intersects(const Vector3& tri0, const Vector3& tri1, const Vector3& tri2, _Out_ float& Dist) const noexcept
{
	return DirectX::TriangleTests::Intersects(position, direction, tri0, tri1, tri2, Dist);
}

inline bool Ray::Intersects(const Plane& plane, _Out_ float& Dist) const noexcept
{
	DirectX::XMVECTOR p = XMLoadFloat4(&plane);
	DirectX::XMVECTOR dir = XMLoadFloat3(&direction);

	DirectX::XMVECTOR nd = DirectX::XMPlaneDotNormal(p, dir);

	if (DirectX::XMVector3LessOrEqual(DirectX::XMVectorAbs(nd), DirectX::g_RayEpsilon))
	{
		Dist = 0.f;
		return false;
	}
	else
	{
		// t = -(dot(n,origin) + D) / dot(n,dir)
		DirectX::XMVECTOR pos = XMLoadFloat3(&position);
		DirectX::XMVECTOR v = DirectX::XMPlaneDotNormal(p, pos);
		v = DirectX::XMVectorAdd(v, DirectX::XMVectorSplatW(p));
		v = DirectX::XMVectorDivide(v, nd);
		float dist = -DirectX::XMVectorGetX(v);
		if (dist < 0)
		{
			Dist = 0.f;
			return false;
		}
		else
		{
			Dist = dist;
			return true;
		}
	}
}

/****************************************************************************
 *
 * Viewport
 *
 ****************************************************************************/

//------------------------------------------------------------------------------
// Comparison operators
//------------------------------------------------------------------------------

inline bool Viewport::operator==(const Viewport& vp) const noexcept
{
	return (x == vp.x && y == vp.y
		&& width == vp.width && height == vp.height
		&& minDepth == vp.minDepth && maxDepth == vp.maxDepth);
}

inline bool Viewport::operator!=(const Viewport& vp) const noexcept
{
	return (x != vp.x || y != vp.y
		|| width != vp.width || height != vp.height
		|| minDepth != vp.minDepth || maxDepth != vp.maxDepth);
}

//------------------------------------------------------------------------------
// Assignment operators
//------------------------------------------------------------------------------

inline Viewport& Viewport::operator=(const RECT& rct) noexcept
{
	x = float(rct.left);
	y = float(rct.top);
	width = float(rct.right - rct.left);
	height = float(rct.bottom - rct.top);
	minDepth = 0.f;
	maxDepth = 1.f;
	return *this;
}

#if defined(__d3d11_h__) || defined(__d3d11_x_h__)
inline Viewport& Viewport::operator=(const D3D11_VIEWPORT& vp) noexcept
{
	x = vp.TopLeftX;
	y = vp.TopLeftY;
	width = vp.Width;
	height = vp.Height;
	minDepth = vp.MinDepth;
	maxDepth = vp.MaxDepth;
	return *this;
}
#endif

#if defined(__d3d12_h__) || defined(__d3d12_x_h__) || defined(__XBOX_D3D12_X__)
inline Viewport& Viewport::operator=(const D3D12_VIEWPORT& vp) noexcept
{
	x = vp.TopLeftX;
	y = vp.TopLeftY;
	width = vp.Width;
	height = vp.Height;
	minDepth = vp.MinDepth;
	maxDepth = vp.MaxDepth;
	return *this;
}
#endif

//------------------------------------------------------------------------------
// Viewport operations
//------------------------------------------------------------------------------

inline float Viewport::AspectRatio() const noexcept
{
	if (width == 0.f || height == 0.f)
		return 0.f;

	return (width / height);
}

inline Vector3 Viewport::Project(const Vector3& p, const Matrix& proj, const Matrix& view, const Matrix& world) const noexcept
{
	DirectX::XMVECTOR v = XMLoadFloat3(&p);
	DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&proj);
	v = XMVector3Project(v, x, y, width, height, minDepth, maxDepth, projection, view, world);
	Vector3 result;
	XMStoreFloat3(&result, v);
	return result;
}

inline void Viewport::Project(const Vector3& p, const Matrix& proj, const Matrix& view, const Matrix& world, Vector3& result) const noexcept
{
	DirectX::XMVECTOR v = XMLoadFloat3(&p);
	DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&proj);
	v = XMVector3Project(v, x, y, width, height, minDepth, maxDepth, projection, view, world);
	XMStoreFloat3(&result, v);
}

inline Vector3 Viewport::Unproject(const Vector3& p, const Matrix& proj, const Matrix& view, const Matrix& world) const noexcept
{
	DirectX::XMVECTOR v = XMLoadFloat3(&p);
	DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&proj);
	v = XMVector3Unproject(v, x, y, width, height, minDepth, maxDepth, projection, view, world);
	Vector3 result;
	XMStoreFloat3(&result, v);
	return result;
}

inline void Viewport::Unproject(const Vector3& p, const Matrix& proj, const Matrix& view, const Matrix& world, Vector3& result) const noexcept
{
	DirectX::XMVECTOR v = XMLoadFloat3(&p);
	DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&proj);
	v = XMVector3Unproject(v, x, y, width, height, minDepth, maxDepth, projection, view, world);
	XMStoreFloat3(&result, v);
}
