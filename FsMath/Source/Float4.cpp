#pragma once


#include <stdafx.h>
#include <FsMath/Include/Float4.h>

#include <FsMath/Include/MathCommon.h>
#include <FsMath/Include/Float3.h>


namespace fs
{
	Float4::Float4()
		: Float4(0.0f)
	{
		__noop;
	}

	Float4::Float4(const float s)
		: Float4(s, s, s, s)
	{
		__noop;
	}

	Float4::Float4(const float x, const float y, const float z, const float w)
		: _x{ x }
		, _y{ y }
		, _z{ z }
		, _w{ w }
	{
		__noop;
	}

	Float4::Float4(const Float3& rhs)
		: Float4(rhs._x, rhs._y, rhs._z, 1.0f)
	{
		__noop;
	}

	Float4& Float4::operator+=(const Float4& rhs)
	{
		_x += rhs._x;
		_y += rhs._y;
		_z += rhs._z;
		_w += rhs._w;
		return *this;
	}

	Float4& Float4::operator-=(const Float4& rhs)
	{
		_x -= rhs._x;
		_y -= rhs._y;
		_z -= rhs._z;
		_w -= rhs._w;
		return *this;
	}

	Float4& Float4::operator*=(const float s)
	{
		_x *= s;
		_y *= s;
		_z *= s;
		_w *= s;
		return *this;
	}

	Float4& Float4::operator/=(const float s)
	{
		_x /= s;
		_y /= s;
		_z /= s;
		_w /= s;
		return *this;
	}

	Float4 Float4::operator+(const Float4& rhs) const noexcept
	{
		return Float4(_x + rhs._x, _y + rhs._y, _z + rhs._z, _w + rhs._w);
	}

	Float4 Float4::operator-(const Float4& rhs) const noexcept
	{
		return Float4(_x - rhs._x, _y - rhs._y, _z - rhs._z, _w - rhs._w);
	}

	Float4 Float4::operator*(const float s) const noexcept
	{
		return Float4(_x * s, _y * s, _z * s, _w * s);
	}

	Float4 Float4::operator/(const float s) const noexcept
	{
		return Float4(_x / s, _y / s, _z / s, _w / s);
	}

	float& Float4::operator[](const uint32 index) noexcept
	{
		FS_ASSERT("김장원", index < 4, "범위를 벗어난 접근입니다.");
		return (&_x)[index];
	}

	const float& Float4::operator[](const uint32 index) const noexcept
	{
		FS_ASSERT("김장원", index < 4, "범위를 벗어난 접근입니다.");
		return (&_x)[index];
	}

	const bool Float4::operator==(const Float4& rhs) const noexcept
	{
		return (_x == rhs._x && _y == rhs._y && _z == rhs._z && _w == rhs._w);
	}

	const bool Float4::operator!=(const Float4& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	const float Float4::dotProductRaw(const float* const a, const float* const b)
	{
		return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]) + (a[3] * b[3]);
	}

	const float Float4::dotProductRaw(const float(&a)[4], const float bX, const float bY, const float bZ, const float bW)
	{
		return (a[0] * bX) + (a[1] * bY) + (a[2] * bZ) + (a[3] * bW);
	}

	const float Float4::dot(const Float4& lhs, const Float4& rhs) noexcept
	{
		return dotProductRaw(&lhs._x, &rhs._x);
	}

	Float4 Float4::cross(const Float4& lhs, const Float4& rhs) noexcept
	{
		return Float4
		(
			lhs._y * rhs._z  -  lhs._z * rhs._y,
			lhs._z * rhs._x  -  lhs._x * rhs._z,
			lhs._x * rhs._y  -  lhs._y * rhs._x,
			0.0f // a vector, not point
		);
	}

	Float4 Float4::normalize(const Float4& float4) noexcept
	{
		return (float4 / float4.length());
	}

	const float Float4::lengthSqaure() const noexcept
	{
		return dot(*this, *this);
	}

	const float Float4::length() const noexcept
	{
		return sqrt(lengthSqaure());
	}

	void Float4::set(const float x, const float y, const float z, const float w) noexcept
	{
		_x = x;
		_y = y; 
		_z = z;
		_w = w;
	}

	/*
	void Float4::x(const float newX) noexcept
	{
		_x = newX;
	}

	const float Float4::x() const noexcept
	{
		return _x;
	}

	void Float4::y(const float newY) noexcept
	{
		_y = newY;
	}

	const float Float4::y() const noexcept
	{
		return _y;
	}

	void Float4::z(const float newZ) noexcept
	{
		_z = newZ;
	}

	const float Float4::z() const noexcept
	{
		return _z;
	}

	void Float4::w(const float newW) noexcept
	{
		_w = newW;
	}

	const float Float4::w() const noexcept
	{
		return _w;
	}
	*/
}
