#include <MintMath/Include/Float3x3.h>

#include <MintMath/Include/VectorR.hpp>
#include <MintMath/Include/Matrix.hpp>


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

    Float3x3::Float3x3(const float scalar)
        : Float3x3(
            scalar, scalar, scalar,
            scalar, scalar, scalar, 
            scalar, scalar, scalar)
    {
        __noop;
    }

    Float3x3::Float3x3(
        const float _11, const float _12, const float _13, 
        const float _21, const float _22, const float _23, 
        const float _31, const float _32, const float _33) 
        : _m{ _11, _12, _13, _21, _22, _23, _31, _32, _33 }
    {
        __noop;
    }

    Float3x3 Float3x3::operator*(const Float3x3& rhs) const noexcept
    {
        Float3x3 result;
        Math::mul(_m, rhs._m, result._m);
        return result;
    }

    Float3x3 Float3x3::operator*(const float scalar) const noexcept
    {
        Float3x3 result = *this;
        Math::setMulMat(result._m, scalar);
        return result;
    }

    Float3x3 Float3x3::operator/(const float scalar) const noexcept
    {
        Float3x3 result = *this;
        Math::setDivMat(result._m, scalar);
        return result;
    }

    void Float3x3::setZero() noexcept
    {
        Math::setZeroMat(_m);
    }

    void Float3x3::setIdentity() noexcept
    {
        Math::setIdentity(_m);
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

    Float3x3 Float3x3::inverse() const noexcept
    {
        Float3x3 adj;
        Math::adjugate(_m, adj._m);
        return adj / Math::determinant(_m);
    }

    Float3 Float3x3::mul(const Float3& vec) const noexcept
    {
        Float3 result;
        Math::mul(_m, vec._c, result._c);
        return result;
    }
}
