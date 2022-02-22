#include <MintMath/Include/Float4.h>

#include <MintMath/Include/VectorR.hpp>
#include <MintMath/Include/Float4x4.h>


namespace mint
{
    const Float4 Float4::kZero = Float4(0.0f);

    float& Float4::operator[](const uint32 index) noexcept
    {
        MINT_ASSERT(index < 4, "범위를 벗어난 접근입니다.");
        return _c[index];
    }

    const float& Float4::operator[](const uint32 index) const noexcept
    {
        MINT_ASSERT(index < 4, "범위를 벗어난 접근입니다.");
        return _c[index];
    }

    const float Float4::dot(const Float4& lhs, const Float4& rhs) noexcept
    {
        return Math::dot(lhs._c, rhs._c);
    }

    Float4 Float4::cross(const Float4& lhs, const Float4& rhs) noexcept
    {
        Float4 result;
        Math::cross(lhs._c, rhs._c, result._c);
        return result;
    }

    Float4 Float4::crossNormalize(const Float4& lhs, const Float4& rhs) noexcept
    {
        return normalize(cross(lhs, rhs));
    }

    Float4 Float4::normalize(const Float4& in) noexcept
    {
        Float4 result = in;
        Math::normalize(result._c);
        return result;
    }

    Float4& Float4::transform(const Float4x4& matrix) noexcept
    {
        *this = matrix * (*this);
        return *this;
    }

    void Float4::normalize() noexcept
    {
        Math::normalize(_c);
    }

    void Float4::setXyz(const float x, const float y, const float z) noexcept
    {
        _x = x;
        _y = y;
        _z = z;
    }

    void Float4::setXyz(const Float3& rhs) noexcept
    {
        _x = rhs._x;
        _y = rhs._y;
        _z = rhs._z;
    }

    void Float4::setXyz(const Float4& rhs) noexcept
    {
        _x = rhs._x;
        _y = rhs._y;
        _z = rhs._z;
    }

    Float4 Float4::getXyz0() const noexcept
    {
        return Float4(_x, _y, _z, 0.0f);
    }

    Float4 Float4::getXyz1() const noexcept
    {
        return Float4(_x, _y, _z, 1.0f);
    }

    Float3 Float4::getXyz() const noexcept
    {
        return Float3(_x, _y, _z);
    }

    const float Float4::lengthSqaure() const noexcept
    {
        return Math::normSq(_c);
    }

    const float Float4::length() const noexcept
    {
        return Math::norm(_c);
    }

    void Float4::set(const float x, const float y, const float z, const float w) noexcept
    {
        _x = x;
        _y = y; 
        _z = z;
        _w = w;
    }

    void Float4::setZero() noexcept
    {
        Math::setZeroVec(_c);
    }

    const bool Float4::isNan() const noexcept
    {
        return Math::isNan(_c);
    }

    void Float4::setNan() noexcept
    {
        Math::setNan(_c);
    }
}
