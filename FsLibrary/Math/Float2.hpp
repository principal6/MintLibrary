#pragma once


#include <stdafx.h>
#include <Math/Float2.h>

#include <Math/MathCommon.h>


namespace fs
{
	FS_INLINE Float2::Float2()
		: Float2(0.0f)
	{
		__noop;
	}

	FS_INLINE Float2::Float2(const float s)
		: Float2(s, s)
	{
		__noop;
	}

	FS_INLINE Float2::Float2(const float x, const float y)
		: _x{ x }
		, _y{ y }
	{
		__noop;
	}

	FS_INLINE Float2& Float2::operator+=(const Float2& rhs)
	{
		_x += rhs._x;
		_y += rhs._y;
		return *this;
	}

	FS_INLINE Float2& Float2::operator-=(const Float2& rhs)
	{
		_x -= rhs._x;
		_y -= rhs._y;
		return *this;
	}

	FS_INLINE Float2& Float2::operator*=(const float s)
	{
		_x *= s;
		_y *= s;
		return *this;
	}

	FS_INLINE Float2& Float2::operator/=(const float s)
	{
		_x /= s;
		_y /= s;
		return *this;
	}

	FS_INLINE Float2 Float2::operator+(const Float2& rhs) const noexcept
	{
		return Float2(_x + rhs._x, _y + rhs._y);
	}

	FS_INLINE Float2 Float2::operator-(const Float2& rhs) const noexcept
	{
		return Float2(_x - rhs._x, _y - rhs._y);
	}

	FS_INLINE Float2 Float2::operator*(const float s) const noexcept
	{
		return Float2(_x * s, _y * s);
	}

	FS_INLINE Float2 Float2::operator/(const float s) const noexcept
	{
		return Float2(_x / s, _y / s);
	}

	FS_INLINE float& Float2::operator[](const uint32 index) noexcept
	{
		FS_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
		return (&_x)[index];
	}

	FS_INLINE const float& Float2::operator[](const uint32 index) const noexcept
	{
		FS_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
		return (&_x)[index];
	}

	FS_INLINE const bool Float2::operator==(const Float2& rhs) const noexcept
	{
		return (_x == rhs._x && _y == rhs._y);
	}

	FS_INLINE const bool Float2::operator!=(const Float2& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	FS_INLINE const float Float2::dot(const Float2& lhs, const Float2& rhs) noexcept
	{
		return lhs._x * rhs._x + lhs._y * rhs._y;
	}

	FS_INLINE Float2 Float2::normalize(const Float2& float2) noexcept
	{
		return Float2(float2 / float2.length());
	}

	FS_INLINE const float Float2::lengthSqaure() const noexcept
	{
		return dot(*this, *this);
	}

	FS_INLINE const float Float2::length() const noexcept
	{
		return sqrt(lengthSqaure());
	}

	FS_INLINE void Float2::set(const float x, const float y)
	{
		_x = x;
		_y = y;
	}

	/*
	FS_INLINE void Float2::x(const float newX) noexcept
	{
		_x = newX;
	}

	FS_INLINE const float Float2::x() const noexcept
	{
		return _x;
	}

	FS_INLINE void Float2::y(const float newY) noexcept
	{
		_y = newY;
	}

	FS_INLINE const float Float2::y() const noexcept
	{
		return _y;
	}
	*/
}
