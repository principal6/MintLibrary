namespace fs
{
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

	inline constexpr Rect::Rect(const float left, const float right, const float top, const float bottom)
		: _raw{ left, right, top, bottom }
	{
		__noop;
	}

	inline constexpr Rect::Rect(const fs::Float2& positionFromLeftTop, const fs::Float2& size)
	{
		left(positionFromLeftTop._x);
		right(left() + size._x);
		top(positionFromLeftTop._y);
		bottom(top() + size._y);
	}

	FS_INLINE const bool Rect::operator==(const Rect& rhs) const noexcept
	{
		return _raw == rhs._raw;
	}

	FS_INLINE const bool Rect::operator!=(const Rect& rhs) const noexcept
	{
		return _raw != rhs._raw;
	}

	FS_INLINE constexpr const float Rect::left() const noexcept
	{
		return _raw._x;
	}

	FS_INLINE constexpr const float Rect::right() const noexcept
	{
		return _raw._y;
	}

	FS_INLINE constexpr const float Rect::top() const noexcept
	{
		return _raw._z;
	}

	FS_INLINE constexpr const float Rect::bottom() const noexcept
	{
		return _raw._w;
	}

	FS_INLINE constexpr void Rect::left(const float s) noexcept
	{
		_raw._x = s;
	}

	FS_INLINE constexpr void Rect::right(const float s) noexcept
	{
		_raw._y = s;
	}

	FS_INLINE constexpr void Rect::top(const float s) noexcept
	{
		_raw._z = s;
	}

	FS_INLINE constexpr void Rect::bottom(const float s) noexcept
	{
		_raw._w = s;
	}

	FS_INLINE constexpr const fs::Float2 Rect::center() const noexcept
	{
		return fs::Float2((left() + right()) * 0.5f, (top() + bottom()) * 0.5f);
	}

	FS_INLINE constexpr const fs::Float2 Rect::size() const noexcept
	{
		return fs::Float2(right() - left(), bottom() - top());
	}

	FS_INLINE constexpr const fs::Float2 Rect::position() const noexcept
	{
		return fs::Float2(left(), top());
	}

	FS_INLINE constexpr void Rect::position(const fs::Float2& position) noexcept
	{
		left(position._x);
		top(position._y);
	}

	FS_INLINE constexpr const bool Rect::contains(const fs::Float2& position) const noexcept
	{
		return (left() <= position._x && position._x <= right() && top() <= position._y && position._y <= bottom());
	}

	FS_INLINE const bool Rect::isNan() const noexcept
	{
		return _raw.isNan();
	}

	FS_INLINE void Rect::setNan() noexcept
	{
		_raw.setNan();
	}
}
