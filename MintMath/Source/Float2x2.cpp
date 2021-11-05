#include <MintMath/Include/Float2x2.h>

#include <MintMath/Include/VectorR.hpp>


namespace mint
{
    Float2x2 Float2x2::rotationMatrix(const float angle) noexcept
    {
        const float cosAngle = cos(angle);
        const float sinAngle = sin(angle);
        return Float2x2
        (
            cosAngle, -sinAngle,
            sinAngle,  cosAngle
        );
    }

    Float2x2::Float2x2()
        : Float2x2(
            1, 0, 
            0, 1)
    {
        __noop;
    }

    Float2x2::Float2x2(const float scalar)
        : Float2x2(
            scalar, scalar, 
            scalar, scalar)
    {
        __noop;
    }

    Float2x2::Float2x2(
        const float _11, const float _12, 
        const float _21, const float _22)
        : _m{ _11, _12, _21, _22 }
    {
        __noop;
    }

    Float2x2 Float2x2::operator*(const float scalar) const noexcept
    {
        Float2x2 result = *this;
        Math::setMulMat(result._m, scalar);
        return result;
    }

    Float2x2 Float2x2::operator/(const float scalar) const noexcept
    {
        Float2x2 result = *this;
        Math::setDivMat(result._m, scalar);
        return result;
    }

    Float2x2 Float2x2::operator*(const Float2x2& rhs) const noexcept
    {
        Float2x2 result;
        Math::mul(_m, rhs._m, result._m);
        return result;
    }

    void Float2x2::set(const uint32 row, const uint32 col, const float newValue) noexcept
    {
        _m[min(row - 1, Float2x2::kMaxIndex)][min(col - 1, Float2x2::kMaxIndex)] = newValue;
    }

    const float Float2x2::get(const uint32 row, const uint32 col) const noexcept
    {
        return _m[min(row - 1, Float2x2::kMaxIndex)][min(col - 1, Float2x2::kMaxIndex)];
    }

    void Float2x2::setZero() noexcept
    {
        Math::setZeroMat(_m);
    }

    void Float2x2::setIdentity() noexcept
    {
        Math::setIdentity(_m);
    }

    const float Float2x2::determinant() const noexcept
    {
        return Math::determinant(_m);
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

    Float2 Float2x2::mul(const Float2& vec) const noexcept
    {
        Float2 result;
        Math::mul(_m, vec._c, result._c);
        return result;
    }
}
