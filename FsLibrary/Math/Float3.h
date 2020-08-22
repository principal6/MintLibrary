#pragma once


#ifndef FS_FLOAT3_H
#define FS_FLOAT3_H


#include <CommonDefinitions.h>
#include <Math/MathCommon.h>


namespace fs
{
	class Float3
	{
	public:
						Float3() : _f{} { __noop; }
		explicit		Float3(const float s) : _f{ s, s, s } { __noop; }
						Float3(const float x, const float y, const float z) : _f{ x, y, z } { __noop; }
						Float3(const Float3& rhs) = default;
						Float3(Float3&& rhs) = default;
		virtual			~Float3() { __noop; }

	public:
		Float3&			operator=(const Float3& rhs) = default;
		Float3&			operator=(Float3&& rhs) = default;
		Float3&			operator+=(const Float3& rhs)
		{
			_f[0] += rhs._f[0];
			_f[1] += rhs._f[1];
			_f[2] += rhs._f[2];
			return *this;
		}
		Float3&			operator-=(const Float3& rhs)
		{
			_f[0] -= rhs._f[0];
			_f[1] -= rhs._f[1];
			_f[2] -= rhs._f[2];
			return *this;
		}
		Float3&			operator*=(const float s)
		{
			_f[0] *= s;
			_f[1] *= s;
			_f[2] *= s;
			return *this;
		}
		Float3&			operator/=(const float s)
		{
			_f[0] /= s;
			_f[1] /= s;
			_f[2] /= s;
			return *this;
		}

	public:
		Float3			operator+(const Float3& rhs) const
		{
			return Float3(_f[0] + rhs._f[0], _f[1] + rhs._f[1], _f[2] + rhs._f[2]);
		}
		Float3			operator-(const Float3& rhs) const
		{
			return Float3(_f[0] - rhs._f[0], _f[1] - rhs._f[1], _f[2] - rhs._f[2]);
		}
		Float3			operator*(const float s) const
		{
			return Float3(_f[0] * s, _f[1] * s, _f[2] * s);
		}
		Float3			operator/(const float s) const
		{
			return Float3(_f[0] / s, _f[1] / s, _f[2] / s);
		}

	public:
		float&			operator[](const uint32 index) noexcept
		{
			FS_ASSERT("김장원", index < 3, "범위를 벗어난 접근입니다.");
			return _f[index];
		}
		const float&	operator[](const uint32 index) const noexcept
		{
			FS_ASSERT("김장원", index < 3, "범위를 벗어난 접근입니다.");
			return _f[index];
		}

	public:
		bool			operator==(const Float3& rhs) const noexcept
		{
			return (_f[0] == rhs._f[0] && _f[1] == rhs._f[1] && _f[2] == rhs._f[2]);
		}
		bool			operator!=(const Float3& rhs) const noexcept
		{
			return !(*this == rhs);
		}

	public:
		static float	dot(const Float3& lhs, const Float3& rhs) noexcept
		{
			return lhs._f[0] * rhs._f[0] + lhs._f[1] * rhs._f[1] + lhs._f[2] * rhs._f[2];
		}

		static Float3	cross(const Float3& lhs, const Float3& rhs) noexcept
		{
			return Float3
			(
				lhs._f[1] * rhs._f[2] - lhs._f[2] * rhs._f[1],
				lhs._f[2] * rhs._f[0] - lhs._f[0] * rhs._f[2],
				lhs._f[0] * rhs._f[1] - lhs._f[1] * rhs._f[0]
			);
		}

		static Float3	normalize(const Float3& float3) noexcept
		{
			return (float3 / float3.length());
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
		inline void		set(const float x, const float y, const float z) { _f[0] = x; _f[1] = y; _f[2] = z; }
		inline void		x(const float newX) noexcept { _f[0] = newX; }
		inline void		y(const float newY) noexcept { _f[1] = newY; }
		inline void		z(const float newZ) noexcept { _f[2] = newZ; }
		inline float	x() const noexcept { return _f[0]; }
		inline float	y() const noexcept { return _f[1]; }
		inline float	z() const noexcept { return _f[2]; }

	public:
		float			_f[3];
	};
}


#endif // !FS_FLOAT3_H
