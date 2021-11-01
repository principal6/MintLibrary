#include <MintMath/Include/Float2.h>

#include <MintCommon/Include/Logger.h>

#include <MintMath/Include/VectorR.hpp>
#include <MintMath/Include/Int2.h>


namespace mint
{
    const Float2 Float2::kZero          = mint::Float2(0.0f);
    const Float2 Float2::kOne           = mint::Float2(+1.0f);
    const Float2 Float2::kNegativeOne   = mint::Float2(-1.0f);
    const Float2 Float2::kNan           = mint::Float2(Math::nan());

    Float2::Float2(const Int2& rhs)
        : Float2(static_cast<float>(rhs._x), static_cast<float>(rhs._y))
    {
        __noop;
    }

    Float2& Float2::operator+=(const Float2& rhs)
    {
        Math::setAdd(_c, rhs._c);
        return *this;
    }

    Float2& Float2::operator-=(const Float2& rhs)
    {
        Math::setSub(_c, rhs._c);
        return *this;
    }

    Float2& Float2::operator*=(const float s)
    {
        Math::setMul(_c, s);
        return *this;
    }

    Float2& Float2::operator/=(const float s)
    {
        Math::setDiv(_c, s);
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
        MINT_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
        return _c[index];
    }

    const float& Float2::operator[](const uint32 index) const noexcept
    {
        MINT_ASSERT("김장원", index < 2, "범위를 벗어난 접근입니다.");
        return _c[index];
    }

    const bool Float2::operator==(const Float2& rhs) const noexcept
    {
        return Math::equals(_c, rhs._c);
    }

    const bool Float2::operator!=(const Float2& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    const float Float2::dot(const Float2& lhs, const Float2& rhs) noexcept
    {
        return Math::dot(lhs._c, rhs._c);
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
        return Math::normSq(_c);
    }

    const float Float2::length() const noexcept
    {
        return Math::norm(_c);
    }

    const float Float2::maxElement() const noexcept
    {
        return mint::max(_x, _y);
    }

    const float Float2::minElement() const noexcept
    {
        return mint::min(_x, _y);
    }

    void Float2::set(const float x, const float y) noexcept
    {
        _x = x;
        _y = y;
    }

    void Float2::setZero() noexcept
    {
        Math::setZero(_c);
    }

    void Float2::setNan() noexcept
    {
        Math::setNan(_c);
    }

    const bool Float2::isNan() const noexcept
    {
        return Math::isNan(_c);
    }

    const bool Float2::hasNegativeElement() const noexcept
    {
        return (_x < 0.0f) || (_y < 0.0f);
    }
}
