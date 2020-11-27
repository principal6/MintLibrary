#pragma once


#include <stdafx.h>
#include <Math/Int2.h>


namespace fs
{
	FS_INLINE Int2::Int2()
		: Int2(0)
	{
		__noop;
	}

	FS_INLINE Int2::Int2(const int32 s)
		: Int2(s, s)
	{
		__noop;
	}

	FS_INLINE Int2::Int2(const int32 x, const int32 y)
		: _x{ x }
		, _y{ y }
	{
		__noop;
	}

	FS_INLINE Int2& Int2::operator+=(const Int2& rhs) noexcept
	{
		_x += rhs._x;
		_y += rhs._y;
		return *this;
	}

	FS_INLINE Int2& Int2::operator-=(const Int2& rhs) noexcept
	{
		_x -= rhs._x;
		_y -= rhs._y;
		return *this;
	}

	FS_INLINE Int2& Int2::operator*=(const int32 s) noexcept
	{
		_x *= s;
		_y *= s;
		return *this;
	}

	FS_INLINE Int2& Int2::operator/=(const int32 s) noexcept
	{
		_x /= s;
		_y /= s;
		return *this;
	}

	FS_INLINE Int2 Int2::operator+(const Int2& rhs) const noexcept
	{
		return Int2(_x + rhs._x, _y + rhs._y);
	}

	FS_INLINE Int2 Int2::operator-(const Int2& rhs) const noexcept
	{
		return Int2(_x - rhs._x, _y - rhs._y);
	}

	FS_INLINE Int2 Int2::operator*(const int32 s) const noexcept
	{
		return Int2(_x * s, _y * s);
	}

	FS_INLINE Int2 Int2::operator/(const int32 s) const noexcept
	{
		return Int2(_x / s, _y / s);
	}

	FS_INLINE int32& Int2::operator[](const uint32 index) noexcept
	{
		FS_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
		return (&_x)[index];
	}

	FS_INLINE const int32& Int2::operator[](const uint32 index) const noexcept
	{
		FS_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
		return (&_x)[index];
	}

	FS_INLINE const bool Int2::operator==(const Int2& rhs) const noexcept
	{
		return (_x == rhs._x && _y == rhs._y);
	}

	FS_INLINE const bool Int2::operator!=(const Int2& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	FS_INLINE void Int2::set(const int32 x, const int32 y) noexcept
	{
		_x = x;
		_y = y;
	}

	/*
	FS_INLINE void Int2::x(const int32 newX) noexcept
	{
		_x = newX;
	}

	FS_INLINE const int32 Int2::x() const noexcept
	{
		return _x;
	}

	FS_INLINE void Int2::y(const int32 newY) noexcept
	{
		_y = newY;
	}

	FS_INLINE const int32 Int2::y() const noexcept
	{
		return _y;
	}
	*/
}
