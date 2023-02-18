#pragma once


namespace mint
{
	constexpr Float2 Float2::Max(const Float2& a, const Float2& b) noexcept
	{
		return Float2(mint::Max(a._x, b._x), mint::Max(a._y, b._y));
	}

	constexpr Float2 Float2::Min(const Float2& a, const Float2& b) noexcept
	{
		return Float2(mint::Min(a._x, b._x), mint::Min(a._y, b._y));
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

	inline constexpr Float2::Float2(const int32 x, const int32 y)
		: _x{ static_cast<float>(x) }
		, _y{ static_cast<float>(y) }
	{
		__noop;
	}
}
