#include <MintMath/Include/Float4.h>

#include <MintCommon/Include/Logger.h>

#include <MintMath/Include/Float4x4.h>


namespace mint
{
    const Float4 Float4::kZero = Float4(0.0f);

    float& Float4::operator[](const uint32 index) noexcept
    {
        MINT_ASSERT("김장원", index < 4, "범위를 벗어난 접근입니다.");
        return (&_x)[index];
    }

    const float& Float4::operator[](const uint32 index) const noexcept
    {
        MINT_ASSERT("김장원", index < 4, "범위를 벗어난 접근입니다.");
        return (&_x)[index];
    }

    const float Float4::dotProductRaw(const float* const a, const float* const b)
    {
        return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]) + (a[3] * b[3]);
    }

    const float Float4::dotProductRaw(const float(&a)[4], const float bX, const float bY, const float bZ, const float bW)
    {
        return (a[0] * bX) + (a[1] * bY) + (a[2] * bZ) + (a[3] * bW);
    }

    const float Float4::dot(const Float4& lhs, const Float4& rhs) noexcept
    {
        return dotProductRaw(&lhs._x, &rhs._x);
    }

    Float4 Float4::cross(const Float4& lhs, const Float4& rhs) noexcept
    {
        return Float4
        (
            lhs._y * rhs._z  -  lhs._z * rhs._y,
            lhs._z * rhs._x  -  lhs._x * rhs._z,
            lhs._x * rhs._y  -  lhs._y * rhs._x,
            0.0f // a vector, not point
        );
    }

    Float4 Float4::crossNormalize(const Float4& lhs, const Float4& rhs) noexcept
    {
        return normalize(cross(lhs, rhs));
    }

    Float4 Float4::normalize(const Float4& float4) noexcept
    {
        return (float4 / float4.length());
    }

    Float4& Float4::transform(const Float4x4& matrix) noexcept
    {
        *this = matrix * (*this);
        return *this;
    }

    void Float4::normalize() noexcept
    {
        *this = Float4::normalize(*this);
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
        return dot(*this, *this);
    }

    const float Float4::length() const noexcept
    {
        return sqrt(lengthSqaure());
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
        _x = _y = _z = _w = 0.0f;
    }

    const bool Float4::isNan() const noexcept
    {
        return (mint::Math::isNan(_x) || mint::Math::isNan(_y) || mint::Math::isNan(_z) || mint::Math::isNan(_w));
    }

    void Float4::setNan() noexcept
    {
        _x = _y = _z = _w = mint::Math::nan();
    }
}
