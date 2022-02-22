#pragma once


namespace mint
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

    inline constexpr Float4::Float4(const Float2& rhs)
        : Float4(rhs._x, rhs._y, 0.0f, 0.0f)
    {
        __noop;
    }

    inline constexpr Float4::Float4(const Float3& rhs)
        : Float4(rhs._x, rhs._y, rhs._z, 0.0f)
    {
        __noop;
    }

    MINT_INLINE Float4& Float4::operator+=(const Float4& rhs)
    {
        _x += rhs._x;
        _y += rhs._y;
        _z += rhs._z;
        _w += rhs._w;
        return *this;
    }

    MINT_INLINE Float4& Float4::operator-=(const Float4& rhs)
    {
        _x -= rhs._x;
        _y -= rhs._y;
        _z -= rhs._z;
        _w -= rhs._w;
        return *this;
    }

    MINT_INLINE Float4& Float4::operator*=(const float scalar)
    {
        _x *= scalar;
        _y *= scalar;
        _z *= scalar;
        _w *= scalar;
        return *this;
    }

    MINT_INLINE Float4& Float4::operator/=(const float scalar)
    {
        _x /= scalar;
        _y /= scalar;
        _z /= scalar;
        _w /= scalar;
        return *this;
    }

    MINT_INLINE const Float4& Float4::operator+() const noexcept
    {
        return *this;
    }

    MINT_INLINE Float4 Float4::operator-() const noexcept
    {
        return Float4(-_x, -_y, -_z, -_w);
    }

    MINT_INLINE Float4 Float4::operator+(const Float4& rhs) const noexcept
    {
        return Float4(_x + rhs._x, _y + rhs._y, _z + rhs._z, _w + rhs._w);
    }

    MINT_INLINE Float4 Float4::operator-(const Float4& rhs) const noexcept
    {
        return Float4(_x - rhs._x, _y - rhs._y, _z - rhs._z, _w - rhs._w);
    }

    MINT_INLINE Float4 Float4::operator*(const float scalar) const noexcept
    {
        return Float4(_x * scalar, _y * scalar, _z * scalar, _w * scalar);
    }

    MINT_INLINE Float4 Float4::operator/(const float scalar) const noexcept
    {
        return Float4(_x / scalar, _y / scalar, _z / scalar, _w / scalar);
    }

    MINT_INLINE constexpr const bool Float4::operator==(const Float4& rhs) const noexcept
    {
        return (_x == rhs._x && _y == rhs._y && _z == rhs._z && _w == rhs._w);
    }

    MINT_INLINE constexpr const bool Float4::operator!=(const Float4& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    MINT_INLINE const float Float4::maxComponent() const noexcept
    {
        return max(max(_x, _y), max(_z, _w));
    }

    MINT_INLINE const float Float4::minComponent() const noexcept
    {
        return min(min(_x, _y), min(_z, _w));
    }
}
