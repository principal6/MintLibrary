#include "Rect.h"
#pragma once


namespace mint
{
    MINT_INLINE constexpr Rect Rect::fromPositionSize(const mint::Float2& positionFromLeftTop, const mint::Float2& size)
    {
        return Rect(positionFromLeftTop, size);
    }

    MINT_INLINE constexpr Rect Rect::fromLongs(const long left, const long right, const long top, const long bottom)
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

    inline constexpr Rect::Rect(const mint::Float2& positionFromLeftTop, const mint::Float2& size)
        : Rect(positionFromLeftTop._x, positionFromLeftTop._x + size._x, positionFromLeftTop._y, positionFromLeftTop._y + size._y)
    {
        __noop;
    }

    inline constexpr Rect::Rect(const float left, const float right, const float top, const float bottom)
        : _raw{ left, right, top, bottom }
    {
        __noop;
    }

    MINT_INLINE constexpr const bool Rect::operator==(const Rect& rhs) const noexcept
    {
        return _raw == rhs._raw;
    }

    MINT_INLINE constexpr const bool Rect::operator!=(const Rect& rhs) const noexcept
    {
        return _raw != rhs._raw;
    }

    MINT_INLINE constexpr const float Rect::left() const noexcept
    {
        return _raw._x;
    }

    MINT_INLINE constexpr const float Rect::right() const noexcept
    {
        return _raw._y;
    }

    MINT_INLINE constexpr const float Rect::top() const noexcept
    {
        return _raw._z;
    }

    MINT_INLINE constexpr const float Rect::bottom() const noexcept
    {
        return _raw._w;
    }

    MINT_INLINE constexpr const float Rect::horz() const noexcept
    {
        return left() + right();
    }

    MINT_INLINE constexpr const float Rect::vert() const noexcept
    {
        return top() + bottom();
    }

    MINT_INLINE float& Rect::left() noexcept
    {
        return _raw._x;
    }

    MINT_INLINE float& Rect::right() noexcept
    {
        return _raw._y;
    }

    MINT_INLINE float& Rect::top() noexcept
    {
        return _raw._z;
    }

    MINT_INLINE float& Rect::bottom() noexcept
    {
        return _raw._w;
    }

    MINT_INLINE constexpr void Rect::left(const float s) noexcept
    {
        _raw._x = s;
    }

    MINT_INLINE constexpr void Rect::right(const float s) noexcept
    {
        _raw._y = s;
    }

    MINT_INLINE constexpr void Rect::top(const float s) noexcept
    {
        _raw._z = s;
    }

    MINT_INLINE constexpr void Rect::bottom(const float s) noexcept
    {
        _raw._w = s;
    }

    MINT_INLINE constexpr const mint::Float2 Rect::center() const noexcept
    {
        return mint::Float2((left() + right()) * 0.5f, (top() + bottom()) * 0.5f);
    }

    MINT_INLINE constexpr const mint::Float2 Rect::size() const noexcept
    {
        return mint::Float2(right() - left(), bottom() - top());
    }

    MINT_INLINE constexpr const mint::Float2 Rect::position() const noexcept
    {
        return mint::Float2(left(), top());
    }

    MINT_INLINE constexpr void Rect::position(const mint::Float2& position) noexcept
    {
        left(position._x);
        top(position._y);
    }

    MINT_INLINE constexpr const bool Rect::contains(const mint::Float2& position) const noexcept
    {
        return (left() <= position._x && position._x <= right() && top() <= position._y && position._y <= bottom());
    }

    MINT_INLINE constexpr const bool Rect::contains(const Rect& rhs) const noexcept
    {
        return (left() <= rhs.left()) && (rhs.right() <= right()) && (top() >= rhs.top()) && (rhs.bottom() >= bottom());
    }

    MINT_INLINE const bool Rect::isNan() const noexcept
    {
        return _raw.isNan();
    }

    MINT_INLINE void Rect::setNan() noexcept
    {
        _raw.setNan();
    }
}
