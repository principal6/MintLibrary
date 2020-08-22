#pragma once


#ifndef FS_FLOAT4_H
#define FS_FLOAT4_H


#include <CommonDefinitions.h>
#include <Math/MathCommon.h>


namespace fs
{
	class Float4
	{
	public:
						Float4() : _f{} { __noop; }
		explicit		Float4(const float s) : _f{ s, s, s, s } { __noop; }
						Float4(const float x, const float y, const float z, const float w) : _f{ x, y, z, w } { __noop; }
						Float4(const Float4& rhs) = default;
						Float4(Float4&& rhs) = default;
		virtual			~Float4() { __noop; }

	public:
		Float4&			operator=(const Float4& rhs) = default;
		Float4&			operator=(Float4&& rhs) = default;
		Float4&			operator+=(const Float4& rhs)
		{
			_f[0] += rhs._f[0];
			_f[1] += rhs._f[1];
			_f[2] += rhs._f[2];
			_f[3] += rhs._f[3];
			return *this;
		}
		Float4&			operator-=(const Float4& rhs)
		{
			_f[0] -= rhs._f[0];
			_f[1] -= rhs._f[1];
			_f[2] -= rhs._f[2];
			_f[3] -= rhs._f[3];
			return *this;
		}
		Float4&			operator*=(const float s)
		{
			_f[0] *= s;
			_f[1] *= s;
			_f[2] *= s;
			_f[3] *= s;
			return *this;
		}
		Float4&			operator/=(const float s)
		{
			_f[0] /= s;
			_f[1] /= s;
			_f[2] /= s;
			_f[3] /= s;
			return *this;
		}

	public:
		Float4			operator+(const Float4& rhs) const
		{
			return Float4(_f[0] + rhs._f[0], _f[1] + rhs._f[1], _f[2] + rhs._f[2], _f[3] + rhs._f[3]);
		}
		Float4			operator-(const Float4& rhs) const
		{
			return Float4(_f[0] - rhs._f[0], _f[1] - rhs._f[1], _f[2] - rhs._f[2], _f[3] - rhs._f[3]);
		}
		Float4			operator*(const float s) const
		{
			return Float4(_f[0] * s, _f[1] * s, _f[2] * s, _f[3] * s);
		}
		Float4			operator/(const float s) const
		{
			return Float4(_f[0] / s, _f[1] / s, _f[2] / s, _f[3] / s);
		}

	public:
		float&			operator[](const uint32 index) noexcept
		{
			FS_ASSERT("김장원", index < 4, "범위를 벗어난 접근입니다.");
			return _f[index];
		}
		float			operator[](const uint32 index) const noexcept
		{
			FS_ASSERT("김장원", index < 4, "범위를 벗어난 접근입니다.");
			return _f[index];
		}

	public:
		bool			operator==(const Float4& rhs) const noexcept
		{
			return (_f[0] == rhs._f[0] && _f[1] == rhs._f[1] && _f[2] == rhs._f[2] && _f[3] == rhs._f[3]);
		}
		bool			operator!=(const Float4& rhs) const noexcept
		{
			return !(*this == rhs);
		}

	public:
		static float	dot(const Float4& lhs, const Float4& rhs) noexcept
		{
			return lhs._f[0] * rhs._f[0] + lhs._f[1] * rhs._f[1] + lhs._f[2] * rhs._f[2];
		}

		static Float4	cross(const Float4& lhs, const Float4& rhs) noexcept
		{
			return Float4
			(
				lhs._f[1] * rhs._f[2] - lhs._f[2] * rhs._f[1],
				lhs._f[2] * rhs._f[0] - lhs._f[0] * rhs._f[2],
				lhs._f[0] * rhs._f[1] - lhs._f[1] * rhs._f[0],
				0.0f
			);
		}

		static Float4	normalize(const Float4& float4) noexcept
		{
			return (float4 / float4.length());
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
		inline void		set(const float x, const float y, const float z, const float w) { _f[0] = x; _f[1] = y; _f[2] = z; _f[3] = w; }
		inline void		x(const float newX) noexcept { _f[0] = newX; }
		inline void		y(const float newY) noexcept { _f[1] = newY; }
		inline void		z(const float newZ) noexcept { _f[2] = newZ; }
		inline void		w(const float newW) noexcept { _f[3] = newW; }
		inline float	x() const noexcept { return _f[0]; }
		inline float	y() const noexcept { return _f[1]; }
		inline float	z() const noexcept { return _f[2]; }
		inline float	w() const noexcept { return _f[3]; }

	public:
		float			_f[4];
	};
}


#endif // !FS_FLOAT4_H
