#pragma once


namespace fs
{
	inline constexpr Float4::Float4()
		: Float4(0.0f)
	{
		__noop;
	}

	inline constexpr Float4::Float4(const float s)
		: Float4(s, s, s, s)
	{
		__noop;
	}

	inline constexpr Float4::Float4(const float x, const float y, const float z, const float w)
		: _x{ x }
		, _y{ y }
		, _z{ z }
		, _w{ w }
	{
		__noop;
	}

	inline constexpr Float4::Float4(const Float3& rhs)
		: Float4(rhs._x, rhs._y, rhs._z, 1.0f)
	{
		__noop;
	}
}
