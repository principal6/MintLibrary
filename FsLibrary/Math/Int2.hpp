#pragma once


#include <stdafx.h>
#include <Math/Int2.h>


namespace fs
{
	inline Int2::Int2()
		: Int2(0)
	{
		__noop;
	}

	inline Int2::Int2(const int32 s)
		: Int2(s, s)
	{
		__noop;
	}

	inline Int2::Int2(const int32 x, const int32 y)
		: _i{ x, y }
	{
		__noop;
	}

	inline Int2& Int2::operator+=(const Int2& rhs) noexcept
	{
		_i[0] += rhs._i[0];
		_i[1] += rhs._i[1];
		return *this;
	}

	inline Int2& Int2::operator-=(const Int2& rhs) noexcept
	{
		_i[0] -= rhs._i[0];
		_i[1] -= rhs._i[1];
		return *this;
	}

	inline Int2& Int2::operator*=(const int32 s) noexcept
	{
		_i[0] *= s;
		_i[1] *= s;
		return *this;
	}

	inline Int2& Int2::operator/=(const int32 s) noexcept
	{
		_i[0] /= s;
		_i[1] /= s;
		return *this;
	}

	inline const Int2 Int2::operator+(const Int2& rhs) const noexcept
	{
		return Int2(_i[0] + rhs._i[0], _i[1] + rhs._i[1]);
	}

	inline const Int2 Int2::operator-(const Int2& rhs) const noexcept
	{
		return Int2(_i[0] - rhs._i[0], _i[1] - rhs._i[1]);
	}

	inline const Int2 Int2::operator*(const int32 s) const noexcept
	{
		return Int2(_i[0] * s, _i[1] * s);
	}

	inline const Int2 Int2::operator/(const int32 s) const noexcept
	{
		return Int2(_i[0] / s, _i[1] / s);
	}

	inline int32& Int2::operator[](const uint32 index) noexcept
	{
		FS_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
		return _i[index];
	}

	inline const int32& Int2::operator[](const uint32 index) const noexcept
	{
		FS_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
		return _i[index];
	}

	inline const bool Int2::operator==(const Int2& rhs) const noexcept
	{
		return (_i[0] == rhs._i[0] && _i[1] == rhs._i[1]);
	}

	inline const bool Int2::operator!=(const Int2& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	inline void Int2::set(const int32 x, const int32 y) noexcept
	{
		_i[0] = x;
		_i[1] = y;
	}

	inline void Int2::x(const int32 newX) noexcept
	{
		_i[0] = newX;
	}

	inline const int32 Int2::x() const noexcept
	{
		return _i[0];
	}

	inline void Int2::y(const int32 newY) noexcept
	{
		_i[1] = newY;
	}

	inline const int32 Int2::y() const noexcept
	{
		return _i[1];
	}
}
