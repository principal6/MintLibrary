#pragma once


namespace mint
{
	MINT_INLINE constexpr Rect Rect::FromPositionSize(const Float2& positionFromLeftTop, const Float2& size)
	{
		return Rect(positionFromLeftTop, size);
	}

	MINT_INLINE constexpr Rect Rect::FromLongs(const long left, const long right, const long top, const long bottom)
	{
		return Rect(static_cast<float>(left), static_cast<float>(right), static_cast<float>(top), static_cast<float>(bottom));
	}


	inline constexpr Rect::Rect()
		: Rect(0.0f, 0.0f, 0.0f, 0.0f)
	{
		__noop;
	}

	inline constexpr Rect::Rect(const float uniformValue)
		: Rect(uniformValue, uniformValue, uniformValue, uniformValue)
	{
		__noop;
	}

	inline constexpr Rect::Rect(const Float2& positionFromLeftTop, const Float2& size)
		: Rect(positionFromLeftTop._x, positionFromLeftTop._x + size._x, positionFromLeftTop._y, positionFromLeftTop._y + size._y)
	{
		__noop;
	}

	inline constexpr Rect::Rect(const float left, const float right, const float top, const float bottom)
		: _raw{ left, right, top, bottom }
	{
		__noop;
	}

	MINT_INLINE constexpr bool Rect::operator==(const Rect& rhs) const noexcept
	{
		return _raw == rhs._raw;
	}

	MINT_INLINE constexpr bool Rect::operator!=(const Rect& rhs) const noexcept
	{
		return _raw != rhs._raw;
	}

	MINT_INLINE constexpr float Rect::Left() const noexcept
	{
		return _raw._x;
	}

	MINT_INLINE constexpr float Rect::Right() const noexcept
	{
		return _raw._y;
	}

	MINT_INLINE constexpr float Rect::Top() const noexcept
	{
		return _raw._z;
	}

	MINT_INLINE constexpr float Rect::Bottom() const noexcept
	{
		return _raw._w;
	}

	MINT_INLINE constexpr float Rect::Horz() const noexcept
	{
		return Left() + Right();
	}

	MINT_INLINE constexpr float Rect::Vert() const noexcept
	{
		return Top() + Bottom();
	}

	MINT_INLINE constexpr float Rect::Width() const noexcept
	{
		const float signedDifference = Right() - Left();
		return (signedDifference >= 0.0f ? signedDifference : -signedDifference);
	}

	MINT_INLINE constexpr float Rect::Height() const noexcept
	{
		const float signedDifference = Top() - Bottom();
		return (signedDifference >= 0.0f ? signedDifference : -signedDifference);
	}

	MINT_INLINE float& Rect::Left() noexcept
	{
		return _raw._x;
	}

	MINT_INLINE float& Rect::Right() noexcept
	{
		return _raw._y;
	}

	MINT_INLINE float& Rect::Top() noexcept
	{
		return _raw._z;
	}

	MINT_INLINE float& Rect::Bottom() noexcept
	{
		return _raw._w;
	}

	MINT_INLINE constexpr void Rect::Left(const float s) noexcept
	{
		_raw._x = s;
	}

	MINT_INLINE constexpr void Rect::Right(const float s) noexcept
	{
		_raw._y = s;
	}

	MINT_INLINE constexpr void Rect::Top(const float s) noexcept
	{
		_raw._z = s;
	}

	MINT_INLINE constexpr void Rect::Bottom(const float s) noexcept
	{
		_raw._w = s;
	}

	MINT_INLINE constexpr Float2 Rect::Center() const noexcept
	{
		return Float2((Left() + Right()) * 0.5f, (Top() + Bottom()) * 0.5f);
	}

	MINT_INLINE constexpr Float2 Rect::Size() const noexcept
	{
		return Float2(Right() - Left(), Bottom() - Top());
	}

	MINT_INLINE constexpr Float2 Rect::Position() const noexcept
	{
		return Float2(Left(), Top());
	}

	MINT_INLINE constexpr void Rect::Position(const Float2& position) noexcept
	{
		Left(position._x);
		Top(position._y);
	}

	MINT_INLINE void Rect::ClipBy(const Rect& outerRect) noexcept
	{
		Left(Max(Left(), outerRect.Left()));
		Right(Min(Right(), outerRect.Right()));
		Top(Max(Top(), outerRect.Top()));
		Bottom(Min(Bottom(), outerRect.Bottom()));

		Validate();
	}

	MINT_INLINE void Rect::MoveBy(const Float2& offset) noexcept
	{
		Left() += offset._x;
		Right() += offset._x;
		Top() += offset._y;
		Bottom() += offset._y;
	}

	MINT_INLINE void Rect::ExpandByQuantity(const Rect& quantity) noexcept
	{
		Left() -= quantity.Left();
		Right() += quantity.Right();
		Top() -= quantity.Top();
		Bottom() += quantity.Bottom();
	}

	MINT_INLINE void Rect::Expand(const Rect& rhs) noexcept
	{
		Top(Min(Top(), rhs.Top()));
		Left(Min(Left(), rhs.Left()));

		ExpandRightBottom(rhs);
	}

	MINT_INLINE void Rect::ExpandRightBottom(const Rect& rhs) noexcept
	{
		Right(Max(Right(), rhs.Right()));
		Bottom(Max(Bottom(), rhs.Bottom()));
	}

	MINT_INLINE void Rect::ShrinkByQuantity(const Rect& quantity) noexcept
	{
		Left() += quantity.Left();
		Right() -= quantity.Right();
		Top() += quantity.Top();
		Bottom() -= quantity.Bottom();

		Validate();
	}

	MINT_INLINE constexpr Float2 Rect::Bound(const Float2& position) const noexcept
	{
		return Float2(BoundHorz(position._x), BoundVert(position._y));
	}

	MINT_INLINE constexpr bool Rect::Contains(const Float2& position) const noexcept
	{
		if (Width() == 0.0f || Height() == 0.0f)
		{
			return false;
		}
		return (Left() <= position._x && position._x <= Right() && Top() <= position._y && position._y <= Bottom());
	}

	MINT_INLINE constexpr bool Rect::Contains(const Rect& rhs) const noexcept
	{
		return (Left() <= rhs.Left()) && (rhs.Right() <= Right()) && (Top() >= rhs.Top()) && (rhs.Bottom() >= Bottom());
	}

	MINT_INLINE bool Rect::IsNAN() const noexcept
	{
		return _raw.IsNAN();
	}

	MINT_INLINE void Rect::SetNAN() noexcept
	{
		_raw.SetNAN();
	}

	MINT_INLINE constexpr float Rect::BoundHorz(const float x) const noexcept
	{
		return Min(Max(Left(), x), Right());
	}

	MINT_INLINE constexpr float Rect::BoundVert(const float y) const noexcept
	{
		return Min(Max(Top(), y), Bottom());
	}

	MINT_INLINE void Rect::Validate() noexcept
	{
		// Rect Size 가 음수가 되지 않도록 방지!! (중요)
		Right(Max(Left(), Right()));
		Bottom(Max(Top(), Bottom()));
	}
}
