#include <MintMath/Include/Float3x3.h>

#include <MintMath/Include/Float2x2.h>

#include <MintMath/Include/VectorR.hpp>


namespace mint
{
    Float3x3 Float3x3::rotationMatrixX(const float angle) noexcept
    {
        const float cosAngle = cos(angle);
        const float sinAngle = sin(angle);
        return Float3x3
        (
                +1.0f,      0.0f,      0.0f,
                 0.0f, +cosAngle, -sinAngle,
                 0.0f, +sinAngle, +cosAngle
        );
    }

    Float3x3 Float3x3::rotationMatrixY(const float angle) noexcept
    {
        const float cosAngle = cos(angle);
        const float sinAngle = sin(angle);
        return Float3x3
        (
            +cosAngle,      0.0f, +sinAngle,
                 0.0f,     +1.0f,      0.0f,
            -sinAngle,      0.0f, +cosAngle
        );
    }

    Float3x3 Float3x3::rotationMatrixZ(const float angle) noexcept
    {
        const float cosAngle = cos(angle);
        const float sinAngle = sin(angle);
        return Float3x3
        (
            +cosAngle, -sinAngle,      0.0f,
            +sinAngle, +cosAngle,      0.0f,
                 0.0f,      0.0f,     +1.0f
        );
    }

    Float3x3::Float3x3()
        : Float3x3(
            1.0f, 0.0f, 0.0f, 
            0.0f, 1.0f, 0.0f, 
            0.0f, 0.0f, 1.0f)
    {
        __noop;
    }

    Float3x3::Float3x3(const float s)
        : Float3x3(
            s, s, s,
            s, s, s, 
            s, s, s)
    {
        __noop;
    }

    Float3x3::Float3x3(
        const float m00, const float m01, const float m02, 
        const float m10, const float m11, const float m12, 
        const float m20, const float m21, const float m22) 
        : _m{ m00, m01, m02, m10, m11, m12, m20, m21, m22 }
    {
        __noop;
    }

    Float3x3 Float3x3::operator*(const Float3x3& rhs) const noexcept
    {
        Float3x3 result;
        Math::mul(_m, rhs._m, result._m);
        return result;
    }

    Float3x3 Float3x3::operator*(const float s) const noexcept
    {
        return Float3x3(
            _m[0][0] * s, _m[0][1] * s, _m[0][2] * s,
            _m[1][0] * s, _m[1][1] * s, _m[1][2] * s,
            _m[2][0] * s, _m[2][1] * s, _m[2][2] * s
        );
    }

    Float3x3 Float3x3::operator/(const float s) const noexcept
    {
        return Float3x3(
            _m[0][0] / s, _m[0][1] / s, _m[0][2] / s,
            _m[1][0] / s, _m[1][1] / s, _m[1][2] / s,
            _m[2][0] / s, _m[2][1] / s, _m[2][2] / s
        );
    }

    void Float3x3::setZero() noexcept
    {
        Math::setZeroMat(_m);
    }

    void Float3x3::setIdentity() noexcept
    {
        Math::setIdentity(_m);
    }

    Float2x2 Float3x3::minor(const uint32 row, const uint32 col) const noexcept
    {
        Float2x2 result;
        Math::minor(_m, row, col, result._m);
        return result;
    }

    const float Float3x3::determinant() const noexcept
    {
        return Math::determinant(_m);
    }

    Float3x3 Float3x3::transpose() const noexcept
    {
        Float3x3 result;
        Math::transpose(_m, result._m);
        return result;
    }

    Float3x3 Float3x3::cofactor() const noexcept
    {
        return Float3x3
        (
            +minor(0, 0).determinant(), -minor(0, 1).determinant(), +minor(0, 2).determinant(),
            -minor(1, 0).determinant(), +minor(1, 1).determinant(), -minor(1, 2).determinant(),
            +minor(2, 0).determinant(), -minor(2, 1).determinant(), +minor(2, 2).determinant()
        );
    }

    Float3x3 Float3x3::adjugate() const noexcept
    {
        return cofactor().transpose();
    }

    Float3x3 Float3x3::inverse() const noexcept
    {
        return adjugate() / determinant();
    }

    Float3 Float3x3::mul(const Float3& v) const noexcept
    {
        Float3 result;
        Math::mul(_m, v._c, result._c);
        return result;
    }
}
