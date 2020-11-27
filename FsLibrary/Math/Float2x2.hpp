#pragma once


#include <stdafx.h>
#include <Math/Float2x2.h>


namespace fs
{
	FS_INLINE Float2x2::Float2x2()
		: Float2x2(
			1.0f, 0.0f, 
			0.0f, 1.0f)
	{
		__noop;
	}

	FS_INLINE Float2x2::Float2x2(const float s)
		: Float2x2(
			s, s, 
			s, s)
	{
		__noop;
	}

	FS_INLINE Float2x2::Float2x2(
		const float m00, const float m01, 
		const float m10, const float m11)
		: _m{ m00, m01, m10, m11 }
	{
		__noop;
	}

	FS_INLINE Float2x2 Float2x2::operator*(const float s) const noexcept
	{
		return Float2x2(
			_m[0][0] * s, _m[0][1] * s,
			_m[1][0] * s, _m[1][1] * s
		);
	}

	FS_INLINE Float2x2 Float2x2::operator/(const float s) const noexcept
	{
		return Float2x2(
			_m[0][0] / s, _m[0][1] / s,
			_m[1][0] / s, _m[1][1] / s
		);
	}

	FS_INLINE Float2x2 Float2x2::operator*(const Float2x2& rhs) const noexcept
	{
		return Float2x2(
			// row 0
			_m[0][0] * rhs._m[0][0] + _m[0][1] * rhs._m[1][0],
			_m[0][0] * rhs._m[0][1] + _m[0][1] * rhs._m[1][1],

			// row 1
			_m[1][0] * rhs._m[0][0] + _m[1][1] * rhs._m[1][0],
			_m[1][0] * rhs._m[0][1] + _m[1][1] * rhs._m[1][1]
		);
	}

	FS_INLINE void Float2x2::set(const uint32 row, const uint32 col, const float newValue) noexcept
	{
		_m[fs::min(row - 1, fs::Float2x2::kMaxIndex)][fs::min(col - 1, fs::Float2x2::kMaxIndex)] = newValue;
	}

	FS_INLINE const float Float2x2::get(const uint32 row, const uint32 col) const noexcept
	{
		return _m[fs::min(row - 1, fs::Float2x2::kMaxIndex)][fs::min(col - 1, fs::Float2x2::kMaxIndex)];
	}

	FS_INLINE void Float2x2::setZero() noexcept
	{
		_m[0][0] = _m[0][1] = _m[1][0] = _m[1][1] = 0.0f;
	}

	FS_INLINE void Float2x2::setIdentity() noexcept
	{
		_m[0][1] = _m[1][0] = 0.0f;
		_m[0][0] = _m[1][1] = 1.0f;
	}

	FS_INLINE const float Float2x2::determinant() const noexcept
	{
		const float a = _m[0][0];
		const float b = _m[0][1];
		const float c = _m[1][0];
		const float d = _m[1][1];
		return a * d - b * c;
	}

	FS_INLINE Float2x2 Float2x2::inverse() const noexcept
	{
		const float a = _m[0][0];
		const float b = _m[0][1];
		const float c = _m[1][0];
		const float d = _m[1][1];
		return Float2x2(
			d, -b,
			-c, a
		) / determinant();
	}
}
