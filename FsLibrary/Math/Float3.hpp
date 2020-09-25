#pragma once


#include <stdafx.h>
#include <Math/Float3.h>

#include <Math/MathCommon.h>
#include <Math/Float2.hpp>


namespace fs
{
	inline Float3::Float3()
		: Float3(0.0f)
	{
		__noop;
	}

	inline Float3::Float3(const float s)
		: Float3(s, s, s)
	{ 
		__noop; 
	}

	inline Float3::Float3(const float x, const float y, const float z)
		: _f{ x, y, z } 
	{
		__noop;
	}

	inline Float3::Float3(const Float2& rhs)
		: _f{ rhs.x(), rhs.y(), 0.0f }
	{
		__noop;
	}

	inline Float3& Float3::operator+=(const Float3& rhs)
	{
		_f[0] += rhs._f[0];
		_f[1] += rhs._f[1];
		_f[2] += rhs._f[2];
		return *this;
	}

	inline Float3& Float3::operator-=(const Float3& rhs)
	{
		_f[0] -= rhs._f[0];
		_f[1] -= rhs._f[1];
		_f[2] -= rhs._f[2];
		return *this;
	}

	inline Float3& Float3::operator*=(const float s)
	{
		_f[0] *= s;
		_f[1] *= s;
		_f[2] *= s;
		return *this;
	}

	inline Float3& Float3::operator/=(const float s)
	{
		_f[0] /= s;
		_f[1] /= s;
		_f[2] /= s;
		return *this;
	}

	inline const Float3 Float3::operator+(const Float3& rhs) const
	{
		return Float3(_f[0] + rhs._f[0], _f[1] + rhs._f[1], _f[2] + rhs._f[2]);
	}

	inline const Float3 Float3::operator-(const Float3& rhs) const
	{
		return Float3(_f[0] - rhs._f[0], _f[1] - rhs._f[1], _f[2] - rhs._f[2]);
	}

	inline const Float3 Float3::operator*(const float s) const
	{
		return Float3(_f[0] * s, _f[1] * s, _f[2] * s);
	}

	inline const Float3 Float3::operator/(const float s) const
	{
		return Float3(_f[0] / s, _f[1] / s, _f[2] / s);
	}

	inline float& Float3::operator[](const uint32 index) noexcept
	{
		FS_ASSERT("김장원", index < 3, "범위를 벗어난 접근입니다.");
		return _f[index];
	}

	inline const float& Float3::operator[](const uint32 index) const noexcept
	{
		FS_ASSERT("김장원", index < 3, "범위를 벗어난 접근입니다.");
		return _f[index];
	}

	inline const bool Float3::operator==(const Float3& rhs) const noexcept
	{
		return (_f[0] == rhs._f[0] && _f[1] == rhs._f[1] && _f[2] == rhs._f[2]);
	}

	inline const bool Float3::operator!=(const Float3& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	FS_INLINE const float Float3::dotProductRaw(const float(&a)[3], const float(&b)[3]) noexcept
	{
		return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
	}

	FS_INLINE const float Float3::dotProductRaw(const float(&a)[3], const float bX, const float bY, const float bZ) noexcept
	{
		return (a[0] * bX) + (a[1] * bY) + (a[2] * bZ);
	}

	inline const float Float3::dot(const Float3& lhs, const Float3& rhs) noexcept
	{
		return dotProductRaw(lhs._f, rhs._f);
	}

	inline const Float3 Float3::cross(const Float3& lhs, const Float3& rhs) noexcept
	{
		return Float3
		(
			lhs._f[1] * rhs._f[2] - lhs._f[2] * rhs._f[1],
			lhs._f[2] * rhs._f[0] - lhs._f[0] * rhs._f[2],
			lhs._f[0] * rhs._f[1] - lhs._f[1] * rhs._f[0]
		);
	}

	inline const Float3 Float3::normalize(const Float3& float3) noexcept
	{
		return (float3 / float3.length());
	}

	inline const float Float3::lengthSqaure() const noexcept
	{
		return dot(*this, *this);
	}

	inline const float Float3::length() const noexcept
	{
		return sqrt(lengthSqaure());
	}

	FS_INLINE void Float3::set(const float x, const float y, const float z) noexcept
	{
		_f[0] = x;
		_f[1] = y;
		_f[2] = z;
	}

	FS_INLINE void Float3::x(const float newX) noexcept
	{
		_f[0] = newX;
	}

	FS_INLINE const float Float3::x() const noexcept
	{
		return _f[0];
	}

	FS_INLINE void Float3::y(const float newY) noexcept
	{
		_f[1] = newY;
	}

	FS_INLINE const float Float3::y() const noexcept
	{
		return _f[1];
	}

	FS_INLINE void Float3::z(const float newZ) noexcept
	{
		_f[2] = newZ;
	}

	FS_INLINE const float Float3::z() const noexcept
	{
		return _f[2];
	}

}
