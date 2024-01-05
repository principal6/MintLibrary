#pragma once


namespace mint
{
	inline constexpr Float3::Float3()
		: Float3(0.0f)
	{
		__noop;
	}

	inline constexpr Float3::Float3(const float s)
		: Float3(s, s, s)
	{
		__noop;
	}

	inline constexpr Float3::Float3(const float x, const float y, const float z)
		: _x{ x }
		, _y{ y }
		, _z{ z }
	{
		__noop;
	}

	inline constexpr Float3::Float3(const Float2& xy)
		: Float3(xy, 0.0f)
	{
		__noop;
	}

	inline constexpr Float3::Float3(const Float2& xy, const float z)
		: Float3(xy._x, xy._y, z)
	{
		__noop;
	}
}
