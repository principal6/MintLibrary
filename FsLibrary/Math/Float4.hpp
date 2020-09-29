#pragma once


#include <stdafx.h>
#include <Math/Float4.h>
#include <Math/MathCommon.h>
#include <Math/Float3.hpp>


namespace fs
{
	inline Float4::Float4()
		: Float4(0.0f)
	{
		__noop;
	}

	inline Float4::Float4(const float s)
		: Float4(s, s, s, s)
	{
		__noop;
	}

	inline Float4::Float4(const float x, const float y, const float z, const float w)
		: _f{ x, y, z, w } 
	{
		__noop;
	}

	inline Float4::Float4(const Float3& rhs)
		: _f{ rhs.x(), rhs.y(), rhs.z() }
	{
		__noop;
	}

	inline Float4& Float4::operator+=(const Float4& rhs)
	{
		_f[0] += rhs._f[0];
		_f[1] += rhs._f[1];
		_f[2] += rhs._f[2];
		_f[3] += rhs._f[3];
		return *this;
	}

	inline Float4& Float4::operator-=(const Float4& rhs)
	{
		_f[0] -= rhs._f[0];
		_f[1] -= rhs._f[1];
		_f[2] -= rhs._f[2];
		_f[3] -= rhs._f[3];
		return *this;
	}

	inline Float4& Float4::operator*=(const float s)
	{
		_f[0] *= s;
		_f[1] *= s;
		_f[2] *= s;
		_f[3] *= s;
		return *this;
	}

	inline Float4& Float4::operator/=(const float s)
	{
		_f[0] /= s;
		_f[1] /= s;
		_f[2] /= s;
		_f[3] /= s;
		return *this;
	}

	inline const Float4 Float4::operator+(const Float4& rhs) const
	{
		return Float4(_f[0] + rhs._f[0], _f[1] + rhs._f[1], _f[2] + rhs._f[2], _f[3] + rhs._f[3]);
	}

	inline const Float4 Float4::operator-(const Float4& rhs) const
	{
		return Float4(_f[0] - rhs._f[0], _f[1] - rhs._f[1], _f[2] - rhs._f[2], _f[3] - rhs._f[3]);
	}

	inline const Float4 Float4::operator*(const float s) const
	{
		return Float4(_f[0] * s, _f[1] * s, _f[2] * s, _f[3] * s);
	}

	inline const Float4 Float4::operator/(const float s) const
	{
		return Float4(_f[0] / s, _f[1] / s, _f[2] / s, _f[3] / s);
	}

	inline float& Float4::operator[](const uint32 index) noexcept
	{
		FS_ASSERT("김장원", index < 4, "범위를 벗어난 접근입니다.");
		return _f[index];
	}

	inline const float& Float4::operator[](const uint32 index) const noexcept
	{
		FS_ASSERT("김장원", index < 4, "범위를 벗어난 접근입니다.");
		return _f[index];
	}

	inline const bool Float4::operator==(const Float4& rhs) const noexcept
	{
		return (_f[0] == rhs._f[0] && _f[1] == rhs._f[1] && _f[2] == rhs._f[2] && _f[3] == rhs._f[3]);
	}

	inline const bool Float4::operator!=(const Float4& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	FS_INLINE const float Float4::dotProductRaw(const float(&a)[4], const float(&b)[4])
	{
		return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]) + (a[3] * b[3]);
	}

	FS_INLINE const float Float4::dotProductRaw(const float(&a)[4], const float bX, const float bY, const float bZ, const float bW)
	{
		return (a[0] * bX) + (a[1] * bY) + (a[2] * bZ) + (a[3] * bW);
	}

	inline const float Float4::dot(const Float4& lhs, const Float4& rhs) noexcept
	{
		return dotProductRaw(lhs._f, rhs._f);
	}

	inline const Float4 Float4::cross(const Float4& lhs, const Float4& rhs) noexcept
	{
		return Float4
		(
			lhs._f[1] * rhs._f[2]  -  lhs._f[2] * rhs._f[1],
			lhs._f[2] * rhs._f[0]  -  lhs._f[0] * rhs._f[2],
			lhs._f[0] * rhs._f[1]  -  lhs._f[1] * rhs._f[0],
			0.0f // a vector, not point
		);
	}

	inline const Float4 Float4::normalize(const Float4& float4) noexcept
	{
		return (float4 / float4.length());
	}

	inline const float Float4::lengthSqaure() const noexcept
	{
		return dot(*this, *this);
	}

	inline const float Float4::length() const noexcept
	{
		return sqrt(lengthSqaure());
	}

	FS_INLINE void Float4::set(const float x, const float y, const float z, const float w) noexcept
	{
		_f[0] = x;
		_f[1] = y; 
		_f[2] = z;
		_f[3] = w;
	}

	FS_INLINE void Float4::x(const float newX) noexcept
	{
		_f[0] = newX;
	}

	FS_INLINE const float Float4::x() const noexcept
	{
		return _f[0];
	}

	FS_INLINE void Float4::y(const float newY) noexcept
	{
		_f[1] = newY;
	}

	FS_INLINE const float Float4::y() const noexcept
	{
		return _f[1];
	}

	FS_INLINE void Float4::z(const float newZ) noexcept
	{
		_f[2] = newZ;
	}

	FS_INLINE const float Float4::z() const noexcept
	{
		return _f[2];
	}

	FS_INLINE void Float4::w(const float newW) noexcept
	{
		_f[3] = newW;
	}

	FS_INLINE const float Float4::w() const noexcept
	{
		return _f[3];
	}
}
