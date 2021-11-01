#include <MintMath/Include/Float2x2.h>

#include <MintMath/Include/VectorR.hpp>


namespace mint
{
    Float2x2 Float2x2::rotationMatrix(const float angle) noexcept
    {
        const float cosAngle = cos(angle);
        const float sinAngle = sin(angle);
        return Float2x2(cosAngle, -sinAngle, sinAngle, cosAngle);
    }

    Float2x2::Float2x2()
        : Float2x2(
            1, 0, 
            0, 1)
    {
        __noop;
    }

    Float2x2::Float2x2(const float s)
        : Float2x2(
            s, s, 
            s, s)
    {
        __noop;
    }

    Float2x2::Float2x2(
        const float m00, const float m01, 
        const float m10, const float m11)
        : _m{ m00, m01, m10, m11 }
    {
        __noop;
    }

    Float2x2 Float2x2::operator*(const float s) const noexcept
    {
        return Float2x2(
            _m[0][0] * s, _m[0][1] * s,
            _m[1][0] * s, _m[1][1] * s
        );
    }

    Float2x2 Float2x2::operator/(const float s) const noexcept
    {
        return Float2x2(
            _m[0][0] / s, _m[0][1] / s,
            _m[1][0] / s, _m[1][1] / s
        );
    }

    Float2x2 Float2x2::operator*(const Float2x2& rhs) const noexcept
    {
        return Float2x2(
            // row 0
            _m[0][0] * rhs._m[0][0] + _m[0][1] * rhs._m[1][0],
            _m[0][0] * rhs._m[0][1] + _m[0][1] * rhs._m[1][1],

            // row 1
            _m[1][0] * rhs._m[0][0] + _m[1][1] * rhs._m[1][0],
            _m[1][0] * rhs._m[0][1] + _m[1][1] * rhs._m[1][1]
        );
    }

    void Float2x2::set(const uint32 row, const uint32 col, const float newValue) noexcept
    {
        _m[mint::min(row - 1, mint::Float2x2::kMaxIndex)][mint::min(col - 1, mint::Float2x2::kMaxIndex)] = newValue;
    }

    const float Float2x2::get(const uint32 row, const uint32 col) const noexcept
    {
        return _m[mint::min(row - 1, mint::Float2x2::kMaxIndex)][mint::min(col - 1, mint::Float2x2::kMaxIndex)];
    }

    void Float2x2::setZero() noexcept
    {
        Math::setZero(_m[0]);
        Math::setZero(_m[1]);
    }

    void Float2x2::setIdentity() noexcept
    {
        Math::setIdentity(_m);
    }

    const float Float2x2::determinant() const noexcept
    {
        const float a = _m[0][0];
        const float b = _m[0][1];
        const float c = _m[1][0];
        const float d = _m[1][1];
        return a * d - b * c;
    }

    Float2x2 Float2x2::inverse() const noexcept
    {
        const float a = _m[0][0];
        const float b = _m[0][1];
        const float c = _m[1][0];
        const float d = _m[1][1];
        return Float2x2(
            d, -b,
            -c, a
        ) / determinant();
    }

    const bool Float2x2::isInvertible() const noexcept
    {
        return (0.0f != determinant());
    }

    Float2 Float2x2::mul(const Float2& v) const noexcept
    {
        Float2 result;
        Math::mul(_m, v._c, result._c);
        return result;
    }
}
