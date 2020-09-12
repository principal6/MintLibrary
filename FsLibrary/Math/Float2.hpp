#pragma once


#include <stdafx.h>
#include <Math/Float2.h>

#include <Math/MathCommon.h>


namespace fs
{
	inline Float2::Float2()
		: Float2(0.0f)
	{
		__noop;
	}

	inline Float2::Float2(const float s)
		: Float2(s, s)
	{
		__noop;
	}

	inline Float2::Float2(const float x, const float y)
		: _f{ x, y } 
	{
		__noop;
	}

	inline Float2& Float2::operator+=(const Float2& rhs)
	{
		_f[0] += rhs._f[0];
		_f[1] += rhs._f[1];
		return *this;
	}

	inline Float2& Float2::operator-=(const Float2& rhs)
	{
		_f[0] -= rhs._f[0];
		_f[1] -= rhs._f[1];
		return *this;
	}

	inline Float2& Float2::operator*=(const float s)
	{
		_f[0] *= s;
		_f[1] *= s;
		return *this;
	}

	inline Float2& Float2::operator/=(const float s)
	{
		_f[0] /= s;
		_f[1] /= s;
		return *this;
	}

	inline const Float2 Float2::operator+(const Float2& rhs) const
	{
		return Float2(_f[0] + rhs._f[0], _f[1] + rhs._f[1]);
	}

	inline const Float2 Float2::operator-(const Float2& rhs) const
	{
		return Float2(_f[0] - rhs._f[0], _f[1] - rhs._f[1]);
	}

	inline const Float2 Float2::operator*(const float s) const
	{
		return Float2(_f[0] * s, _f[1] * s);
	}

	inline const Float2 Float2::operator/(const float s) const
	{
		return Float2(_f[0] / s, _f[1] / s);
	}

	inline float& Float2::operator[](const uint32 index) noexcept
	{
		FS_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
		return _f[index];
	}

	inline const float& Float2::operator[](const uint32 index) const noexcept
	{
		FS_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
		return _f[index];
	}

	inline const bool Float2::operator==(const Float2& rhs) const noexcept
	{
		return (_f[0] == rhs._f[0] && _f[1] == rhs._f[1]);
	}

	inline const bool Float2::operator!=(const Float2& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	inline const float Float2::dot(const Float2& lhs, const Float2& rhs) noexcept
	{
		return lhs._f[0] * rhs._f[0] + lhs._f[1] * rhs._f[1];
	}

	inline const Float2 Float2::normalize(const Float2& float2) noexcept
	{
		return (float2 / float2.length());
	}

	inline const float Float2::lengthSqaure() const noexcept
	{
		return dot(*this, *this);
	}

	inline const float Float2::length() const noexcept
	{
		return sqrt(lengthSqaure());
	}

	FS_INLINE void Float2::set(const float x, const float y)
	{
		_f[0] = x;
		_f[1] = y;
	}

	FS_INLINE void Float2::x(const float newX) noexcept
	{
		_f[0] = newX;
	}

	FS_INLINE const float Float2::x() const noexcept
	{
		return _f[0];
	}

	FS_INLINE void Float2::y(const float newY) noexcept
	{
		_f[1] = newY;
	}

	FS_INLINE const float Float2::y() const noexcept
	{
		return _f[1];
	}

}
