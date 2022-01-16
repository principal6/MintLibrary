#include <MintMath/Include/Float2.h>

#include <MintCommon/Include/Logger.h>

#include <MintMath/Include/VectorR.hpp>
#include <MintMath/Include/Int2.h>


namespace mint
{
    const Float2 Float2::kZero          = Float2(0.0f);
    const Float2 Float2::kOne           = Float2(+1.0f);
    const Float2 Float2::kNegativeOne   = Float2(-1.0f);
    const Float2 Float2::kMax           = Float2(Math::kFloatMax);
    const Float2 Float2::kNan           = Float2(Math::nan());

    Float2::Float2(const Int2& rhs)
        : Float2(static_cast<float>(rhs._x), static_cast<float>(rhs._y))
    {
        __noop;
    }

    Float2& Float2::operator+=(const Float2& rhs)
    {
        Math::setAddVec(_c, rhs._c);
        return *this;
    }

    Float2& Float2::operator-=(const Float2& rhs)
    {
        Math::setSubVec(_c, rhs._c);
        return *this;
    }

    Float2& Float2::operator*=(const float scalar)
    {
        Math::setMulVec(_c, scalar);
        return *this;
    }

    Float2& Float2::operator/=(const float scalar)
    {
        Math::setDivVec(_c, scalar);
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
        Float2 result = *this;
        Math::setAddVec(result._c, rhs._c);
        return result;
    }

    Float2 Float2::operator-(const Float2& rhs) const noexcept
    {
        Float2 result = *this;
        Math::setSubVec(result._c, rhs._c);
        return result;
    }

    Float2 Float2::operator*(const float scalar) const noexcept
    {
        Float2 result = *this;
        Math::setMulVec(result._c, scalar);
        return result;
    }

    Float2 Float2::operator/(const float scalar) const noexcept
    {
        Float2 result = *this;
        Math::setDivVec(result._c, scalar);
        return result;
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

    Float2 Float2::normalize(const Float2& in) noexcept
    {
        Float2 result = in;
        Math::normalize(result._c);
        return result;
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
        return max(_x, _y);
    }

    const float Float2::minElement() const noexcept
    {
        return min(_x, _y);
    }

    void Float2::set(const float x, const float y) noexcept
    {
        _x = x;
        _y = y;
    }

    void Float2::setZero() noexcept
    {
        Math::setZeroVec(_c);
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
