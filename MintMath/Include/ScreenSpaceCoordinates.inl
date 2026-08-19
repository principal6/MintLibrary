#pragma once


namespace mint
{
	MINT_INLINE ScreenSpaceCoordinates ScreenSpaceCoordinates::FromInternal(const Float2& internalPosition, const Float2& screenSize) noexcept
	{
		return ScreenSpaceCoordinates(
			internalPosition._x + screenSize._x * 0.5f,
			screenSize._y * 0.5f - internalPosition._y);
	}

	inline constexpr ScreenSpaceCoordinates::ScreenSpaceCoordinates(const float x, const float y)
		: _pixels{ x, y }
	{
		__noop;
	}

	inline constexpr ScreenSpaceCoordinates::ScreenSpaceCoordinates(const Float2& pixels)
		: _pixels{ pixels }
	{
		__noop;
	}

	MINT_INLINE Float2 ScreenSpaceCoordinates::ToInternal(const Float2& screenSize) const noexcept
	{
		return Float2(
			_pixels._x - screenSize._x * 0.5f,
			screenSize._y * 0.5f - _pixels._y);
	}
}
