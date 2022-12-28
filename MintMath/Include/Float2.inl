#pragma once


namespace mint
{
	constexpr Float2 Float2::max(const Float2& a, const Float2& b) noexcept
	{
		return Float2(mint::max(a._x, b._x), mint::max(a._y, b._y));
	}

	constexpr Float2 Float2::min(const Float2& a, const Float2& b) noexcept
	{
		return Float2(mint::min(a._x, b._x), mint::min(a._y, b._y));
	}

	inline constexpr Float2::Float2()
		: Float2(0.0f)
	{
		__noop;
	}

	inline constexpr Float2::Float2(const float scalar)
		: Float2(scalar, scalar)
	{
		__noop;
	}

	inline constexpr Float2::Float2(const float x, const float y)
		: _x{ x }
		, _y{ y }
	{
		__noop;
	}
}
