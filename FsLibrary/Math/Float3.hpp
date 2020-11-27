#pragma once


#include <stdafx.h>
#include <Math/Float3.h>

#include <Math/MathCommon.h>
#include <Math/Float2.hpp>


namespace fs
{
	FS_INLINE Float3::Float3()
		: Float3(0.0f)
	{
		__noop;
	}

	FS_INLINE Float3::Float3(const float s)
		: Float3(s, s, s)
	{ 
		__noop; 
	}

	FS_INLINE Float3::Float3(const float x, const float y, const float z)
		: _x{ x }
		, _y{ y }
		, _z{ z }
	{
		__noop;
	}

	FS_INLINE Float3::Float3(const Float2& rhs)
		: Float3(rhs._x, rhs._y, 0.0f)
	{
		__noop;
	}

	FS_INLINE Float3& Float3::operator+=(const Float3& rhs)
	{
		_x += rhs._x;
		_y += rhs._y;
		_z += rhs._z;
		return *this;
	}

	FS_INLINE Float3& Float3::operator-=(const Float3& rhs)
	{
		_x -= rhs._x;
		_y -= rhs._y;
		_z -= rhs._z;
		return *this;
	}

	FS_INLINE Float3& Float3::operator*=(const float s)
	{
		_x *= s;
		_y *= s;
		_z *= s;
		return *this;
	}

	FS_INLINE Float3& Float3::operator/=(const float s)
	{
		_x /= s;
		_y /= s;
		_z /= s;
		return *this;
	}

	FS_INLINE Float3 Float3::operator+(const Float3& rhs) const noexcept
	{
		return Float3(_x + rhs._x, _y + rhs._y, _z + rhs._z);
	}

	FS_INLINE Float3 Float3::operator-(const Float3& rhs) const noexcept
	{
		return Float3(_x - rhs._x, _y - rhs._y, _z - rhs._z);
	}

	FS_INLINE Float3 Float3::operator*(const float s) const noexcept
	{
		return Float3(_x * s, _y * s, _z * s);
	}

	FS_INLINE Float3 Float3::operator/(const float s) const noexcept
	{
		return Float3(_x / s, _y / s, _z / s);
	}

	FS_INLINE float& Float3::operator[](const uint32 index) noexcept
	{
		FS_ASSERT("김장원", index < 3, "범위를 벗어난 접근입니다.");
		return (&_x)[index];
	}

	FS_INLINE const float& Float3::operator[](const uint32 index) const noexcept
	{
		FS_ASSERT("김장원", index < 3, "범위를 벗어난 접근입니다.");
		return (&_x)[index];
	}

	FS_INLINE const bool Float3::operator==(const Float3& rhs) const noexcept
	{
		return (_x == rhs._x && _y == rhs._y && _z == rhs._z);
	}

	FS_INLINE const bool Float3::operator!=(const Float3& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	FS_INLINE const float Float3::dotProductRaw(const float* const a, const float* const b) noexcept
	{
		return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
	}

	FS_INLINE const float Float3::dotProductRaw(const float(&a)[3], const float bX, const float bY, const float bZ) noexcept
	{
		return (a[0] * bX) + (a[1] * bY) + (a[2] * bZ);
	}

	FS_INLINE const float Float3::dot(const Float3& lhs, const Float3& rhs) noexcept
	{
		return dotProductRaw(&lhs._x, &rhs._x);
	}

	FS_INLINE Float3 Float3::cross(const Float3& lhs, const Float3& rhs) noexcept
	{
		return Float3
		(
			lhs._y * rhs._z - lhs._z * rhs._y,
			lhs._z * rhs._x - lhs._x * rhs._z,
			lhs._x * rhs._y - lhs._y * rhs._x
		);
	}

	FS_INLINE Float3 Float3::normalize(const Float3& float3) noexcept
	{
		return Float3(float3 / float3.length());
	}

	FS_INLINE const float Float3::lengthSqaure() const noexcept
	{
		return dot(*this, *this);
	}

	FS_INLINE const float Float3::length() const noexcept
	{
		return sqrt(lengthSqaure());
	}

	FS_INLINE void Float3::set(const float x, const float y, const float z) noexcept
	{
		_x = x;
		_y = y;
		_z = z;
	}
	
	/*
	FS_INLINE void Float3::x(const float newX) noexcept
	{
		_x = newX;
	}

	FS_INLINE const float Float3::x() const noexcept
	{
		return _x;
	}

	FS_INLINE void Float3::y(const float newY) noexcept
	{
		_y = newY;
	}

	FS_INLINE const float Float3::y() const noexcept
	{
		return _y;
	}

	FS_INLINE void Float3::z(const float newZ) noexcept
	{
		_z = newZ;
	}

	FS_INLINE const float Float3::z() const noexcept
	{
		return _z;
	}
	*/
}
