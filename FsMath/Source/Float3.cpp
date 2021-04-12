#include <FsMath/Include/Float3.h>

#include <FsCommon/Include/Logger.h>


namespace fs
{
    const Float3 Float3::kUnitScale = Float3(1.0f, 1.0f, 1.0f);
    

    Float3& Float3::operator+=(const Float3& rhs)
    {
        _x += rhs._x;
        _y += rhs._y;
        _z += rhs._z;
        return *this;
    }

    Float3& Float3::operator-=(const Float3& rhs)
    {
        _x -= rhs._x;
        _y -= rhs._y;
        _z -= rhs._z;
        return *this;
    }

    Float3& Float3::operator*=(const float s)
    {
        _x *= s;
        _y *= s;
        _z *= s;
        return *this;
    }

    Float3& Float3::operator/=(const float s)
    {
        _x /= s;
        _y /= s;
        _z /= s;
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
        FS_ASSERT("김장원", index < 3, "범위를 벗어난 접근입니다.");
        return (&_x)[index];
    }

    const float& Float3::operator[](const uint32 index) const noexcept
    {
        FS_ASSERT("김장원", index < 3, "범위를 벗어난 접근입니다.");
        return (&_x)[index];
    }

    const bool Float3::operator==(const Float3& rhs) const noexcept
    {
        return (_x == rhs._x && _y == rhs._y && _z == rhs._z);
    }

    const bool Float3::operator!=(const Float3& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    const float Float3::dotProductRaw(const float* const a, const float* const b) noexcept
    {
        return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
    }

    const float Float3::dotProductRaw(const float(&a)[3], const float bX, const float bY, const float bZ) noexcept
    {
        return (a[0] * bX) + (a[1] * bY) + (a[2] * bZ);
    }

    const float Float3::dot(const Float3& lhs, const Float3& rhs) noexcept
    {
        return dotProductRaw(&lhs._x, &rhs._x);
    }

    Float3 Float3::cross(const Float3& lhs, const Float3& rhs) noexcept
    {
        return Float3
        (
            lhs._y * rhs._z - lhs._z * rhs._y,
            lhs._z * rhs._x - lhs._x * rhs._z,
            lhs._x * rhs._y - lhs._y * rhs._x
        );
    }

    Float3 Float3::crossAndNormalize(const Float3& lhs, const Float3& rhs) noexcept
    {
        return normalize(cross(lhs, rhs));
    }

    Float3 Float3::normalize(const Float3& float3) noexcept
    {
        return Float3(float3 / float3.length());
    }

    void Float3::normalize() noexcept
    {
        *this = Float3::normalize(*this);
    }

    const float Float3::lengthSqaure() const noexcept
    {
        return dot(*this, *this);
    }

    const float Float3::length() const noexcept
    {
        return sqrt(lengthSqaure());
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
