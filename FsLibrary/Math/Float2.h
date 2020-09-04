#pragma once


#ifndef FS_FLOAT2_H
#define FS_FLOAT2_H


#include <CommonDefinitions.h>
#include <Math/MathCommon.h>


namespace fs
{
	class Float2 final
	{
	public:
						Float2() : _f{} { __noop; }
		explicit		Float2(const float s) : _f{ s, s } { __noop; }
						Float2(const float x, const float y) : _f{ x, y } { __noop; }
						Float2(const Float2& rhs) = default;
						Float2(Float2&& rhs) = default;
						~Float2() { __noop; }

	public:
		Float2&			operator=(const Float2& rhs) = default;
		Float2&			operator=(Float2&& rhs) = default;
		Float2&			operator+=(const Float2& rhs)
		{
			_f[0] += rhs._f[0];
			_f[1] += rhs._f[1];
			return *this;
		}
		Float2&			operator-=(const Float2& rhs)
		{
			_f[0] -= rhs._f[0];
			_f[1] -= rhs._f[1];
			return *this;
		}
		Float2&			operator*=(const float s)
		{
			_f[0] *= s;
			_f[1] *= s;
			return *this;
		}
		Float2&			operator/=(const float s)
		{
			_f[0] /= s;
			_f[1] /= s;
			return *this;
		}

	public:
		Float2			operator+(const Float2& rhs) const
		{
			return Float2(_f[0] + rhs._f[0], _f[1] + rhs._f[1]);
		}
		Float2			operator-(const Float2& rhs) const
		{
			return Float2(_f[0] - rhs._f[0], _f[1] - rhs._f[1]);
		}
		Float2			operator*(const float s) const
		{
			return Float2(_f[0] * s, _f[1] * s);
		}
		Float2			operator/(const float s) const
		{
			return Float2(_f[0] / s, _f[1] / s);
		}

	public:
		float&			operator[](const uint32 index) noexcept
		{
			FS_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
			return _f[index];
		}
		const float&	operator[](const uint32 index) const noexcept
		{
			FS_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
			return _f[index];
		}

	public:
		bool			operator==(const Float2& rhs) const noexcept
		{
			return (_f[0] == rhs._f[0] && _f[1] == rhs._f[1]);
		}
		bool			operator!=(const Float2& rhs) const noexcept
		{
			return !(*this == rhs);
		}

	public:
		static float	dot(const Float2& lhs, const Float2& rhs) noexcept
		{
			return lhs._f[0] * rhs._f[0] + lhs._f[1] * rhs._f[1];
		}

		static Float2	normalize(const Float2& float2) noexcept
		{
			return (float2 / float2.length());
		}

	public:
		inline float	lengthSqaure() const noexcept
		{
			return dot(*this, *this);
		}

		inline float	length() const noexcept
		{
			return sqrt(lengthSqaure());
		}

	public:
		inline void		set(const float x, const float y) { _f[0] = x; _f[1] = y; }
		inline void		x(const float newX) noexcept { _f[0] = newX; }
		inline void		y(const float newY) noexcept { _f[1] = newY; }
		inline float	x() const noexcept { return _f[0]; }
		inline float	y() const noexcept { return _f[1]; }

	public:
		float			_f[2];
	};
}


#endif // !FS_FLOAT2_H
