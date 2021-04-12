#include <FsMath/Include/Float2.h>

#include <FsCommon/Include/Logger.h>

#include <FsMath/Include/Int2.h>


namespace fs
{
    const Float2 Float2::kZero  = fs::Float2(0.0f);
    const Float2 Float2::kOne   = fs::Float2(1.0f);
    const Float2 Float2::kNan   = fs::Float2(fs::Math::nan());

    Float2::Float2(const Int2& rhs)
        : Float2(static_cast<float>(rhs._x), static_cast<float>(rhs._y))
    {
        __noop;
    }

    Float2& Float2::operator+=(const Float2& rhs)
    {
        _x += rhs._x;
        _y += rhs._y;
        return *this;
    }

    Float2& Float2::operator-=(const Float2& rhs)
    {
        _x -= rhs._x;
        _y -= rhs._y;
        return *this;
    }

    Float2& Float2::operator*=(const float s)
    {
        _x *= s;
        _y *= s;
        return *this;
    }

    Float2& Float2::operator/=(const float s)
    {
        _x /= s;
        _y /= s;
        return *this;
    }

    const Float2& Float2::operator+() const noexcept
    {
        return *this;
    }

    Float2 Float2::operator-() const noexcept
    {
        return Float2(-_x, -_y);
    }

    Float2 Float2::operator+(const Float2& rhs) const noexcept
    {
        return Float2(_x + rhs._x, _y + rhs._y);
    }

    Float2 Float2::operator-(const Float2& rhs) const noexcept
    {
        return Float2(_x - rhs._x, _y - rhs._y);
    }

    Float2 Float2::operator*(const float s) const noexcept
    {
        return Float2(_x * s, _y * s);
    }

    Float2 Float2::operator/(const float s) const noexcept
    {
        return Float2(_x / s, _y / s);
    }

    float& Float2::operator[](const uint32 index) noexcept
    {
        FS_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
        return (&_x)[index];
    }

    const float& Float2::operator[](const uint32 index) const noexcept
    {
        FS_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
        return (&_x)[index];
    }

    const bool Float2::operator==(const Float2& rhs) const noexcept
    {
        return (_x == rhs._x && _y == rhs._y);
    }

    const bool Float2::operator!=(const Float2& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    const float Float2::dot(const Float2& lhs, const Float2& rhs) noexcept
    {
        return lhs._x * rhs._x + lhs._y * rhs._y;
    }

    Float2 Float2::normalize(const Float2& float2) noexcept
    {
        return Float2(float2 / float2.length());
    }

    Float2 Float2::abs(const Float2& rhs) noexcept
    {
        return Float2(::abs(rhs._x), ::abs(rhs._y));
    }

    const float Float2::lengthSqaure() const noexcept
    {
        return dot(*this, *this);
    }

    const float Float2::length() const noexcept
    {
        return sqrt(lengthSqaure());
    }

    const float Float2::maxElement() const noexcept
    {
        return fs::max(_x, _y);
    }

    const float Float2::minElement() const noexcept
    {
        return fs::min(_x, _y);
    }

    void Float2::set(const float x, const float y) noexcept
    {
        _x = x;
        _y = y;
    }

    void Float2::setZero() noexcept
    {
        _x = _y = 0.0f;
    }

    void Float2::setNan() noexcept
    {
        _x = _y = fs::Math::nan();
    }

    const bool Float2::isNan() const noexcept
    {
        return (fs::Math::isNan(_x) || fs::Math::isNan(_y));
    }
}
