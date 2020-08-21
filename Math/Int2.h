#pragma once


#ifndef FS_INT2_H
#define FS_INT2_H


#include <CommonDefinitions.h>


namespace fs
{
	// int32
	struct Int2
	{
	public:
		Int2() : _i{} { __noop; }
		Int2(const int32 x, const int32 y) : _i{ x, y } { __noop; }

	public:
		Int2& operator+=(const Int2& rhs) noexcept
		{
			_i[0] += rhs._i[0];
			_i[1] += rhs._i[1];
			return *this;
		}
		Int2& operator-=(const Int2& rhs) noexcept
		{
			_i[0] -= rhs._i[0];
			_i[1] -= rhs._i[1];
			return *this;
		}
		Int2& operator*=(const int32 s) noexcept
		{
			_i[0] *= s;
			_i[1] *= s;
			return *this;
		}
		Int2& operator/=(const int32 s) noexcept
		{
			_i[0] /= s;
			_i[1] /= s;
			return *this;
		}

	public:
		Int2			operator+(const Int2& rhs) const noexcept
		{
			return Int2(_i[0] + rhs._i[0], _i[1] + rhs._i[1]);
		}
		Int2			operator-(const Int2& rhs) const noexcept
		{
			return Int2(_i[0] - rhs._i[0], _i[1] - rhs._i[1]);
		}
		Int2			operator*(const int32 s) const noexcept
		{
			return Int2(_i[0] * s, _i[1] * s);
		}
		Int2			operator/(const int32 s) const noexcept
		{
			return Int2(_i[0] / s, _i[1] / s);
		}

	public:
		int32& operator[](const uint32 index) noexcept
		{
			FS_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
			return _i[index];
		}
		int32			operator[](const uint32 index) const noexcept
		{
			FS_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
			return _i[index];
		}

	public:
		bool			operator==(const Int2& rhs) const noexcept
		{
			return (_i[0] == rhs._i[0] && _i[1] == rhs._i[1]);
		}
		bool			operator!=(const Int2& rhs) const noexcept
		{
			return !(*this == rhs);
		}

	public:
		inline void		set(const int32 x, const int32 y) noexcept { _i[0] = x; _i[1] = y; }
		inline int32	x() const noexcept { return _i[0]; }
		inline int32	y() const noexcept { return _i[1]; }
		inline void		x(const int32 newX) noexcept { _i[0] = newX; }
		inline void		y(const int32 newY) noexcept { _i[1] = newY; }

	public:
		int32			_i[2];
	};
}


#endif // !FS_INT2_H
