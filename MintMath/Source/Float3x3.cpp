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

    Float3 Float3x3::mul(const Float3& v) const noexcept
    {
        Float3 result;
        Math::mul(_m, v._c, result._c);
        return result;
    }
}
