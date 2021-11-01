#include <MintMath/Include/Float3.h>

#include <MintCommon/Include/Logger.h>

#include <MintMath/Include/VectorR.hpp>


namespace mint
{
    const Float3 Float3::kUnitScale = Float3(1.0f, 1.0f, 1.0f);
    

    Float3& Float3::operator+=(const Float3& rhs)
    {
        Math::setAddVec(_c, rhs._c);
        return *this;
    }

    Float3& Float3::operator-=(const Float3& rhs)
    {
        Math::setSubVec(_c, rhs._c);
        return *this;
    }

    Float3& Float3::operator*=(const float s)
    {
        Math::setMulVec(_c, s);
        return *this;
    }

    Float3& Float3::operator/=(const float s)
    {
        Math::setDivVec(_c, s);
        return *this;
    }

    const Float3& Float3::operator+() const noexcept
    {
        return *this;
    }

    Float3 Float3::operator-() const noexcept
    {
        return Float3(-_x, -_y, -_z);
    }

    Float3 Float3::operator+(const Float3& rhs) const noexcept
    {
        return Float3(_x + rhs._x, _y + rhs._y, _z + rhs._z);
    }

    Float3 Float3::operator-(const Float3& rhs) const noexcept
    {
        return Float3(_x - rhs._x, _y - rhs._y, _z - rhs._z);
    }

    Float3 Float3::operator*(const float s) const noexcept
    {
        return Float3(_x * s, _y * s, _z * s);
    }

    Float3 Float3::operator/(const float s) const noexcept
    {
        return Float3(_x / s, _y / s, _z / s);
    }

    float& Float3::operator[](const uint32 index) noexcept
    {
        MINT_ASSERT("김장원", index < 3, "범위를 벗어난 접근입니다.");
        return _c[index];
    }

    const float& Float3::operator[](const uint32 index) const noexcept
    {
        MINT_ASSERT("김장원", index < 3, "범위를 벗어난 접근입니다.");
        return _c[index];
    }

    const bool Float3::operator==(const Float3& rhs) const noexcept
    {
        return Math::equals(_c, rhs._c);
    }

    const bool Float3::operator!=(const Float3& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    const float Float3::dot(const Float3& lhs, const Float3& rhs) noexcept
    {
        return Math::dot(lhs._c, rhs._c);
    }

    Float3 Float3::cross(const Float3& lhs, const Float3& rhs) noexcept
    {
        Float3 result;
        Math::cross(lhs._c, rhs._c, result._c);
        return result;
    }

    Float3 Float3::crossAndNormalize(const Float3& lhs, const Float3& rhs) noexcept
    {
        return normalize(cross(lhs, rhs));
    }

    Float3 Float3::normalize(const Float3& in) noexcept
    {
        Float3 result = in;
        result.normalize();
        return result;
    }

    void Float3::normalize() noexcept
    {
        Math::normalize(_c);
    }

    const float Float3::lengthSqaure() const noexcept
    {
        return Math::normSq(_c);
    }

    const float Float3::length() const noexcept
    {
        return Math::norm(_c);
    }

    void Float3::set(const float x, const float y, const float z) noexcept
    {
        _x = x;
        _y = y;
        _z = z;
    }

    Float2 Float3::xy() const noexcept
    {
        return Float2(_x, _y);
    }
}
