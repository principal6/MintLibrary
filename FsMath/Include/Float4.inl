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

    FS_INLINE Float4& Float4::operator+=(const Float4& rhs)
    {
        _x += rhs._x;
        _y += rhs._y;
        _z += rhs._z;
        _w += rhs._w;
        return *this;
    }

    FS_INLINE Float4& Float4::operator-=(const Float4& rhs)
    {
        _x -= rhs._x;
        _y -= rhs._y;
        _z -= rhs._z;
        _w -= rhs._w;
        return *this;
    }

    FS_INLINE Float4& Float4::operator*=(const float s)
    {
        _x *= s;
        _y *= s;
        _z *= s;
        _w *= s;
        return *this;
    }

    FS_INLINE Float4& Float4::operator/=(const float s)
    {
        _x /= s;
        _y /= s;
        _z /= s;
        _w /= s;
        return *this;
    }

    FS_INLINE const Float4& Float4::operator+() const noexcept
    {
        return *this;
    }

    FS_INLINE Float4 Float4::operator-() const noexcept
    {
        return Float4(-_x, -_y, -_z, -_w);
    }

    FS_INLINE Float4 Float4::operator+(const Float4& rhs) const noexcept
    {
        return Float4(_x + rhs._x, _y + rhs._y, _z + rhs._z, _w + rhs._w);
    }

    FS_INLINE Float4 Float4::operator-(const Float4& rhs) const noexcept
    {
        return Float4(_x - rhs._x, _y - rhs._y, _z - rhs._z, _w - rhs._w);
    }

    FS_INLINE Float4 Float4::operator*(const float s) const noexcept
    {
        return Float4(_x * s, _y * s, _z * s, _w * s);
    }

    FS_INLINE Float4 Float4::operator/(const float s) const noexcept
    {
        return Float4(_x / s, _y / s, _z / s, _w / s);
    }

    FS_INLINE constexpr const bool Float4::operator==(const Float4& rhs) const noexcept
    {
        return (_x == rhs._x && _y == rhs._y && _z == rhs._z && _w == rhs._w);
    }

    FS_INLINE constexpr const bool Float4::operator!=(const Float4& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    FS_INLINE const float Float4::maxComponent() const noexcept
    {
        return fs::max(fs::max(_x, _y), fs::max(_z, _w));
    }

    FS_INLINE const float Float4::minComponent() const noexcept
    {
        return fs::min(fs::min(_x, _y), fs::min(_z, _w));
    }
}
