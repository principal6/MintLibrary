#include <MintMath/Include/Float4x4.h>

#include <MintMath/Include/Quaternion.h>
#include <MintMath/Include/VectorR.hpp>


namespace mint
{
    const Float4x4 Float4x4::kIdentity = Float4x4();

    Float4 Float4x4::mul(const Float4x4& m, const Float4& v) noexcept
    {
        return m.mul(v);
    }

    Float3 Float4x4::mul(const Float4x4& m, const Float3& v) noexcept
    {
        return m.mul(v);
    }

    Float4x4 Float4x4::mul(const Float4x4& l, const Float4x4& r) noexcept
    {
        return l.mul(r);
    }

    Float4x4 Float4x4::translationMatrix(const float x, const float y, const float z) noexcept
    {
        return Float4x4
        (
            1, 0, 0, x,
            0, 1, 0, y,
            0, 0, 1, z,
            0, 0, 0, 1
        );
    }

    Float4x4 Float4x4::translationMatrix(const Float3& translation) noexcept
    {
        return translationMatrix(translation._x, translation._y, translation._z);
    }

    Float4x4 Float4x4::scalingMatrix(const float x, const float y, const float z) noexcept
    {
        return Float4x4
        (
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1
        );
    }

    Float4x4 Float4x4::scalingMatrix(const Float3& scale) noexcept
    {
        return scalingMatrix(scale._x, scale._y, scale._z);
    }

    Float4x4 Float4x4::rotationMatrixX(const float angle) noexcept
    {
        return Float4x4
        (
            1.0f            , 0.0f            , 0.0f            , 0.0f,
            0.0f            , +cosf(angle)    , -sinf(angle)    , 0.0f,
            0.0f            , +sinf(angle)    , +cosf(angle)    , 0.0f,
            0.0f            , 0.0f            , 0.0f            , 1.0f
        );
    }

    Float4x4 Float4x4::rotationMatrixY(const float angle) noexcept
    {
        return Float4x4
        (
            +cosf(angle)    , 0.0f            , +sinf(angle)    , 0.0f,
            0.0f            , 1.0f            , 0.0f            , 0.0f,
            -sinf(angle)    , 0.0f            , +cosf(angle)    , 0.0f,
            0.0f            , 0.0f            , 0.0f            , 1.0f
        );
    }

    Float4x4 Float4x4::rotationMatrixZ(const float angle) noexcept
    {
        return Float4x4
        (
            +cosf(angle)    , -sinf(angle)    , 0.0f            , 0.0f,
            +sinf(angle)    , +cosf(angle)    , 0.0f            , 0.0f,
            0.0f            , 0.0f            , 1.0f            , 0.0f,
            0.0f            , 0.0f            , 0.0f            , 1.0f
        );
    }

    Float4x4 Float4x4::rotationMatrixRollPitchYaw(const float pitch, const float yaw, const float roll) noexcept
    {
        return rotationMatrixY(yaw) * rotationMatrixX(pitch) * rotationMatrixZ(roll);
    }

    Float4x4 Float4x4::rotationMatrixAxisAngle(const Float3& axis, const float angle) noexcept
    {
        // (v * r)r(1 - cosθ) + vcosθ + (r X v)sinθ

        const Float3 r = Float3::normalize(axis);
        const float c = cosf(angle);
        const float s = sinf(angle);

        const float rx = r._x;
        const float ry = r._y;
        const float rz = r._z;
        Float4x4 result
        (
            (1 - c) * rx * rx  + c            , (1 - c) * ry * rx       - (rz * s), (1 - c) * rz * rx       + (ry * s), 0,
            (1 - c) * rx * ry       + (rz * s), (1 - c) * ry * ry  + c            , (1 - c) * rz * ry       - (rx * s), 0,
            (1 - c) * rx * rz       - (ry * s), (1 - c) * ry * rz       + (rx * s), (1 - c) * rz * rz  + c            , 0,
            0                                 , 0                                 , 0                                 , 1
        );
        return result;
    }

    Float4x4 Float4x4::rotationMatrixFromAxes(const Float3& axisX, const Float3& axisY, const Float3& axisZ) noexcept
    {
        return axesToColumns(axisX, axisY, axisZ);
    }

    Float4x4 Float4x4::rotationMatrix(const QuaternionF& rotation) noexcept
    {
        Float3 axis;
        float angle;
        rotation.getAxisAngle(axis, angle);
        return rotationMatrixAxisAngle(axis, angle);
    }

    Float4x4 Float4x4::axesToColumns(const Float3& axisX, const Float3& axisY, const Float3& axisZ) noexcept
    {
        return Float4x4
        (
            axisX._x, axisY._x, axisZ._x, 0.0f,
            axisX._y, axisY._y, axisZ._y, 0.0f,
            axisX._z, axisY._z, axisZ._z, 0.0f,
                 0.0f,    0.0f,     0.0f, 1.0f
        );
    }

    Float4x4 Float4x4::axesToRows(const Float3& axisX, const Float3& axisY, const Float3& axisZ) noexcept
    {
        return Float4x4
        (
            axisX._x, axisX._y, axisX._z, 0.0f,
            axisY._x, axisY._y, axisY._z, 0.0f,
            axisZ._x, axisZ._y, axisZ._z, 0.0f,
                 0.0f,    0.0f,     0.0f, 1.0f
        );
    }
    
    Float4x4 Float4x4::srtMatrix(const Float3& scale, const QuaternionF& rotation, const Float3& translation) noexcept
    {
        // SRT matrix for column vector is like below:
        // SRT = T * R * S
        // which is the same as below..
        Float4x4 matrix = rotationMatrix(rotation);
        matrix.preTranslate(translation._x, translation._y, translation._z);
        matrix.postScale(scale._x, scale._y, scale._z);
        return matrix;
    }

    Float4x4 Float4x4::projectionMatrixPerspective(const float fov, const float nearZ, const float farZ, const float ratio) noexcept
    {
        const float halfFov = fov * 0.5f;
        const float a = 1.0f / (tanf(halfFov) * ratio);
        const float b = 1.0f / (tanf(halfFov));
        const float c = (farZ) / (farZ - nearZ);
        const float d = -(farZ * nearZ) / (farZ - nearZ);
        const float e = 1.0f;
        return Float4x4
        (
            a, 0, 0, 0,
            0, b, 0, 0,
            0, 0, c, d,
            0, 0, e, 0
        );
    }

    Float4x4 Float4x4::projectionMatrix2DFromTopLeft(const float pixelWidth, const float pixelHeight) noexcept
    {
        return Float4x4
        (
            +2.0f / pixelWidth,  0.0f              , 0.0f, -1.0f,
             0.0f             , -2.0f / pixelHeight, 0.0f, +1.0f,
             0.0f             ,  0.0f              , 1.0f,  0.0f,
             0.0f             ,  0.0f              , 0.0f,  1.0f
        );
    }

    Float4x4::Float4x4()
        : Float4x4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        )
    {
        __noop;
    }

    Float4x4::Float4x4(const float s)
        : Float4x4(
            s, s, s, s,
            s, s, s, s,
            s, s, s, s,
            s, s, s, s
        )
    {
        __noop;
    }

    Float4x4::Float4x4(
        const float m00, const float m01, const float m02, const float m03,
        const float m10, const float m11, const float m12, const float m13,
        const float m20, const float m21, const float m22, const float m23,
        const float m30, const float m31, const float m32, const float m33)
        : _m{
            m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33
        }
    {
        __noop;
    }

    Float4x4& Float4x4::operator*=(const Float4x4& rhs) noexcept
    {
        Math::mul(_m, rhs._m, _m);
        return *this;
    }

    Float4x4 Float4x4::operator+(const Float4x4& rhs) const noexcept
    {
        Float4x4 result = *this;
        Math::setAddMat(result._m, rhs._m);
        return result;
    }

    Float4x4 Float4x4::operator-(const Float4x4& rhs) const noexcept
    {
        Float4x4 result = *this;
        Math::setSubMat(result._m, rhs._m);
        return result;
    }

    Float4x4 Float4x4::operator*(const float scalar) const noexcept
    {
        Float4x4 result = *this;
        Math::setMulMat(result._m, scalar);
        return result;
    }

    Float4x4 Float4x4::operator/(const float scalar) const noexcept
    {
        Float4x4 result = *this;
        Math::setDivMat(result._m, scalar);
        return result;
    }

    Float4x4 Float4x4::operator*(const Float4x4& rhs) const noexcept
    {
        return mul(*this, rhs);
    }

    Float4 Float4x4::operator*(const Float4& vec) const noexcept
    {
        return mul(*this, vec);
    }

    Float3 Float4x4::operator*(const Float3& vec) const noexcept
    {
        return mul(*this, vec);
    }

    void Float4x4::set(
        float m00, float m01, float m02, float m03, 
        float m10, float m11, float m12, float m13, 
        float m20, float m21, float m22, float m23, 
        float m30, float m31, float m32, float m33) noexcept
    {
        // row 0
        _m[0][0] = m00;
        _m[0][1] = m01;
        _m[0][2] = m02;
        _m[0][3] = m03;

        // row 1
        _m[1][0] = m10;
        _m[1][1] = m11;
        _m[1][2] = m12;
        _m[1][3] = m13;

        // row 2
        _m[2][0] = m20;
        _m[2][1] = m21;
        _m[2][2] = m22;
        _m[2][3] = m23;

        // row 3
        _m[3][0] = m30;
        _m[3][1] = m31;
        _m[3][2] = m32;
        _m[3][3] = m33;
    }

    void Float4x4::setZero() noexcept
    {
        Math::setZeroMat(_m);
    }

    void Float4x4::setIdentity() noexcept
    {
        Math::setIdentity(_m);
    }

    void Float4x4::preScale(const float x, const float y, const float z) noexcept
    {
        _row[0] *= x;
        _row[1] *= y;
        _row[2] *= z;
    }

    void Float4x4::postScale(const float x, const float y, const float z) noexcept
    {
        _11 *= x;
        _21 *= x;
        _31 *= x;
        _41 *= x;

        _12 *= y;
        _22 *= y;
        _32 *= y;
        _42 *= y;

        _13 *= z;
        _23 *= z;
        _33 *= z;
        _43 *= z;
    }

    void Float4x4::preScale(const Float3& scale) noexcept
    {
        preScale(scale._x, scale._y, scale._z);
    }

    void Float4x4::postScale(const Float3& scale) noexcept
    {
        postScale(scale._x, scale._y, scale._z);
    }

    void Float4x4::setTranslation(const float x, const float y, const float z) noexcept
    {
        _14 = x;
        _24 = y;
        _34 = z;
    }

    void Float4x4::setTranslation(const Float3& translation) noexcept
    {
        _14 = translation._x;
        _24 = translation._y;
        _34 = translation._z;
    }

    void Float4x4::preTranslate(const float x, const float y, const float z) noexcept
    {
        _14 += x;
        _24 += y;
        _34 += z;
    }

    void Float4x4::postTranslate(const float x, const float y, const float z) noexcept
    {
        Float4 row = _row[0];
        _row[0]._w += row._x * x + row._y * y + row._z * z;
        row = _row[1];
        _row[1]._w += row._x * x + row._y * y + row._z * z;
        row = _row[2];
        _row[2]._w += row._x * x + row._y * y + row._z * z;
        row = _row[3];
        _row[3]._w += row._x * x + row._y * y + row._z * z;
    }

    void Float4x4::preTranslate(const Float3& translation) noexcept
    {
        preTranslate(translation._x, translation._y, translation._z);
    }

    void Float4x4::postTranslate(const Float3& translation) noexcept
    {
        postTranslate(translation._x, translation._y, translation._z);
    }

    Float3 Float4x4::getTranslation() const noexcept
    {
        return Float3(_14, _24, _34);
    }

    void Float4x4::decomposeSrt(Float3& outScale, Float4x4& outRotationMatrix, Float3& outTranslation) const noexcept
    {
        // TODO: avoid nan in outRotationMatrix
        

        // Srt Matrix
        // 
        // | s_x * r_11  s_y * r_12  s_z * r_13  t_x |
        // | s_x * r_21  s_y * r_22  s_z * r_23  t_y |
        // | s_x * r_31  s_y * r_32  s_z * r_33  t_z |
        // | 0           0           0           1   |
        
        // s
        outScale._x = ::sqrtf((_11 * _11) + (_21 * _21) + (_31 * _31));
        outScale._y = ::sqrtf((_12 * _12) + (_22 * _22) + (_32 * _32));
        outScale._z = ::sqrtf((_13 * _13) + (_23 * _23) + (_33 * _33));

        // r
        outRotationMatrix._11 = _11 / outScale._x;
        outRotationMatrix._21 = _21 / outScale._x;
        outRotationMatrix._31 = _31 / outScale._x;

        outRotationMatrix._12 = _12 / outScale._y;
        outRotationMatrix._22 = _22 / outScale._y;
        outRotationMatrix._32 = _32 / outScale._y;

        outRotationMatrix._13 = _13 / outScale._z;
        outRotationMatrix._23 = _23 / outScale._z;
        outRotationMatrix._33 = _33 / outScale._z;

        outRotationMatrix._14 = outRotationMatrix._24 = outRotationMatrix._34 = outRotationMatrix._41 = outRotationMatrix._42 = outRotationMatrix._43 = 0.0f;
        outRotationMatrix._44 = 1.0f;

        // t
        outTranslation._x = _14;
        outTranslation._y = _24;
        outTranslation._z = _34;
    }

    const float Float4x4::determinant() const noexcept
    {
        return Math::determinant(_m);
    }

    Float4x4 Float4x4::transpose() const noexcept
    {
        Float4x4 result;
        Math::transpose(_m, result._m);
        return result;
    }

    Float4x4 Float4x4::inverse() const noexcept
    {
        Float4x4 adj;
        Math::adjugate(_m, adj._m);
        return adj / Math::determinant(_m);
    }

    Float4x4 Float4x4::mul(const Float4x4& rhs) const noexcept
    {
        Float4x4 result;
        Math::mul(_m, rhs._m, result._m);
        return result;
    }

    void Float4x4::mulAssignReverse(const Float4x4& lhs) noexcept
    {
        Float4x4 rhsCopy = *this;
        Math::mul(lhs._m, rhsCopy._m, _m);
    }

    Float4 Float4x4::mul(const Float4& v) const noexcept
    {
        Float4 result;
        Math::mul(_m, v._c, result._c);
        return result;
    }

    Float3 Float4x4::mul(const Float3& v) const noexcept
    {
        Float4 v4 = Float4(v._x, v._y, v._z, 0.0f);
        Float4 r4;
        Math::mul(_m, v4._c, r4._c);
        return r4.getXyz();
    }
}
