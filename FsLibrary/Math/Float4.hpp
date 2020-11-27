#pragma once


#include <stdafx.h>
#include <Math/Float4.h>
#include <Math/MathCommon.h>
#include <Math/Float3.hpp>


namespace fs
{
	FS_INLINE Float4::Float4()
		: Float4(0.0f)
	{
		__noop;
	}

	FS_INLINE Float4::Float4(const float s)
		: Float4(s, s, s, s)
	{
		__noop;
	}

	FS_INLINE Float4::Float4(const float x, const float y, const float z, const float w)
		: _x{ x }
		, _y{ y }
		, _z{ z }
		, _w{ w }
	{
		__noop;
	}

	FS_INLINE Float4::Float4(const Float3& rhs)
		: Float4(rhs._x, rhs._y, rhs._z, 1.0f)
	{
		__noop;
	}

	FS_INLINE Float4& Float4::operator+=(const Float4& rhs)
	{
		_x += rhs._x;
		_y += rhs._y;
		_z += rhs._z;
		_w += rhs._w;
		return *this;
	}

	FS_INLINE Float4& Float4::operator-=(const Float4& rhs)
	{
		_x -= rhs._x;
		_y -= rhs._y;
		_z -= rhs._z;
		_w -= rhs._w;
		return *this;
	}

	FS_INLINE Float4& Float4::operator*=(const float s)
	{
		_x *= s;
		_y *= s;
		_z *= s;
		_w *= s;
		return *this;
	}

	FS_INLINE Float4& Float4::operator/=(const float s)
	{
		_x /= s;
		_y /= s;
		_z /= s;
		_w /= s;
		return *this;
	}

	FS_INLINE Float4 Float4::operator+(const Float4& rhs) const noexcept
	{
		return Float4(_x + rhs._x, _y + rhs._y, _z + rhs._z, _w + rhs._w);
	}

	FS_INLINE Float4 Float4::operator-(const Float4& rhs) const noexcept
	{
		return Float4(_x - rhs._x, _y - rhs._y, _z - rhs._z, _w - rhs._w);
	}

	FS_INLINE Float4 Float4::operator*(const float s) const noexcept
	{
		return Float4(_x * s, _y * s, _z * s, _w * s);
	}

	FS_INLINE Float4 Float4::operator/(const float s) const noexcept
	{
		return Float4(_x / s, _y / s, _z / s, _w / s);
	}

	FS_INLINE float& Float4::operator[](const uint32 index) noexcept
	{
		FS_ASSERT("김장원", index < 4, "범위를 벗어난 접근입니다.");
		return (&_x)[index];
	}

	FS_INLINE const float& Float4::operator[](const uint32 index) const noexcept
	{
		FS_ASSERT("김장원", index < 4, "범위를 벗어난 접근입니다.");
		return (&_x)[index];
	}

	FS_INLINE const bool Float4::operator==(const Float4& rhs) const noexcept
	{
		return (_x == rhs._x && _y == rhs._y && _z == rhs._z && _w == rhs._w);
	}

	FS_INLINE const bool Float4::operator!=(const Float4& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	FS_INLINE const float Float4::dotProductRaw(const float* const a, const float* const b)
	{
		return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]) + (a[3] * b[3]);
	}

	FS_INLINE const float Float4::dotProductRaw(const float(&a)[4], const float bX, const float bY, const float bZ, const float bW)
	{
		return (a[0] * bX) + (a[1] * bY) + (a[2] * bZ) + (a[3] * bW);
	}

	FS_INLINE const float Float4::dot(const Float4& lhs, const Float4& rhs) noexcept
	{
		return dotProductRaw(&lhs._x, &rhs._x);
	}

	FS_INLINE Float4 Float4::cross(const Float4& lhs, const Float4& rhs) noexcept
	{
		return Float4
		(
			lhs._y * rhs._z  -  lhs._z * rhs._y,
			lhs._z * rhs._x  -  lhs._x * rhs._z,
			lhs._x * rhs._y  -  lhs._y * rhs._x,
			0.0f // a vector, not point
		);
	}

	FS_INLINE Float4 Float4::normalize(const Float4& float4) noexcept
	{
		return (float4 / float4.length());
	}

	FS_INLINE const float Float4::lengthSqaure() const noexcept
	{
		return dot(*this, *this);
	}

	FS_INLINE const float Float4::length() const noexcept
	{
		return sqrt(lengthSqaure());
	}

	FS_INLINE void Float4::set(const float x, const float y, const float z, const float w) noexcept
	{
		_x = x;
		_y = y; 
		_z = z;
		_w = w;
	}

	/*
	FS_INLINE void Float4::x(const float newX) noexcept
	{
		_x = newX;
	}

	FS_INLINE const float Float4::x() const noexcept
	{
		return _x;
	}

	FS_INLINE void Float4::y(const float newY) noexcept
	{
		_y = newY;
	}

	FS_INLINE const float Float4::y() const noexcept
	{
		return _y;
	}

	FS_INLINE void Float4::z(const float newZ) noexcept
	{
		_z = newZ;
	}

	FS_INLINE const float Float4::z() const noexcept
	{
		return _z;
	}

	FS_INLINE void Float4::w(const float newW) noexcept
	{
		_w = newW;
	}

	FS_INLINE const float Float4::w() const noexcept
	{
		return _w;
	}
	*/
}
