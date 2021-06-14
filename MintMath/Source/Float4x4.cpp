#include <MintMath/Include/Float4x4.h>

#include <MintMath/Include/Float3x3.h>

#include <cstring>


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

    Float4x4 Float4x4::translationMatrix(const mint::Float3& translation) noexcept
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

    Float4x4 Float4x4::scalingMatrix(const mint::Float3& scale) noexcept
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

    Float4x4 Float4x4::rotationMatrixAxisAngle(const mint::Float3& axis, const float angle) noexcept
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

    Float4x4 Float4x4::rotationMatrixFromAxes(const mint::Float3& axisX, const mint::Float3& axisY, const mint::Float3& axisZ) noexcept
    {
        return axesToColumns(axisX, axisY, axisZ);
    }

    Float4x4 Float4x4::rotationMatrix(const mint::Quaternion& rotation) noexcept
    {
        mint::Float3 axis;
        float angle;
        rotation.getAxisAngle(axis, angle);
        return rotationMatrixAxisAngle(axis, angle);
    }

    Float4x4 Float4x4::axesToColumns(const mint::Float3& axisX, const mint::Float3& axisY, const mint::Float3& axisZ) noexcept
    {
        return Float4x4
        (
            axisX._x, axisY._x, axisZ._x, 0.0f,
            axisX._y, axisY._y, axisZ._y, 0.0f,
            axisX._z, axisY._z, axisZ._z, 0.0f,
                 0.0f,    0.0f,     0.0f, 1.0f
        );
    }

    Float4x4 Float4x4::axesToRows(const mint::Float3& axisX, const mint::Float3& axisY, const mint::Float3& axisZ) noexcept
    {
        return Float4x4
        (
            axisX._x, axisX._y, axisX._z, 0.0f,
            axisY._x, axisY._y, axisY._z, 0.0f,
            axisZ._x, axisZ._y, axisZ._z, 0.0f,
                 0.0f,    0.0f,     0.0f, 1.0f
        );
    }
    
    Float4x4 Float4x4::srtMatrix(const mint::Float3& scale, const mint::Quaternion& rotation, const mint::Float3& translation) noexcept
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
        // row 0
        mint::Float4 row = _row[0];
        _row[0]._x = Float4::dotProductRaw(row._f, rhs._m[0][0], rhs._m[1][0], rhs._m[2][0], rhs._m[3][0]);
        _row[0]._y = Float4::dotProductRaw(row._f, rhs._m[0][1], rhs._m[1][1], rhs._m[2][1], rhs._m[3][1]);
        _row[0]._z = Float4::dotProductRaw(row._f, rhs._m[0][2], rhs._m[1][2], rhs._m[2][2], rhs._m[3][2]);
        _row[0]._w = Float4::dotProductRaw(row._f, rhs._m[0][3], rhs._m[1][3], rhs._m[2][3], rhs._m[3][3]);

        // row 1
        row = _row[1];
        _row[1]._x = Float4::dotProductRaw(row._f, rhs._m[0][0], rhs._m[1][0], rhs._m[2][0], rhs._m[3][0]);
        _row[1]._y = Float4::dotProductRaw(row._f, rhs._m[0][1], rhs._m[1][1], rhs._m[2][1], rhs._m[3][1]);
        _row[1]._z = Float4::dotProductRaw(row._f, rhs._m[0][2], rhs._m[1][2], rhs._m[2][2], rhs._m[3][2]);
        _row[1]._w = Float4::dotProductRaw(row._f, rhs._m[0][3], rhs._m[1][3], rhs._m[2][3], rhs._m[3][3]);

        // row 2
        row = _row[2];
        _row[2]._x = Float4::dotProductRaw(row._f, rhs._m[0][0], rhs._m[1][0], rhs._m[2][0], rhs._m[3][0]);
        _row[2]._y = Float4::dotProductRaw(row._f, rhs._m[0][1], rhs._m[1][1], rhs._m[2][1], rhs._m[3][1]);
        _row[2]._z = Float4::dotProductRaw(row._f, rhs._m[0][2], rhs._m[1][2], rhs._m[2][2], rhs._m[3][2]);
        _row[2]._w = Float4::dotProductRaw(row._f, rhs._m[0][3], rhs._m[1][3], rhs._m[2][3], rhs._m[3][3]);

        // row 3
        row = _row[3];
        _row[3]._x = Float4::dotProductRaw(row._f, rhs._m[0][0], rhs._m[1][0], rhs._m[2][0], rhs._m[3][0]);
        _row[3]._y = Float4::dotProductRaw(row._f, rhs._m[0][1], rhs._m[1][1], rhs._m[2][1], rhs._m[3][1]);
        _row[3]._z = Float4::dotProductRaw(row._f, rhs._m[0][2], rhs._m[1][2], rhs._m[2][2], rhs._m[3][2]);
        _row[3]._w = Float4::dotProductRaw(row._f, rhs._m[0][3], rhs._m[1][3], rhs._m[2][3], rhs._m[3][3]);

        return *this;
    }

    Float4x4 Float4x4::operator+(const Float4x4& rhs) const noexcept
    {
        return Float4x4(
            _m[0][0] + rhs._m[0][0], _m[0][1] + rhs._m[0][1], _m[0][2] + rhs._m[0][2], _m[0][3] + rhs._m[0][3],
            _m[1][0] + rhs._m[1][0], _m[1][1] + rhs._m[1][1], _m[1][2] + rhs._m[1][2], _m[1][3] + rhs._m[1][3],
            _m[2][0] + rhs._m[2][0], _m[2][1] + rhs._m[2][1], _m[2][2] + rhs._m[2][2], _m[2][3] + rhs._m[2][3],
            _m[3][0] + rhs._m[3][0], _m[3][1] + rhs._m[3][1], _m[3][2] + rhs._m[3][2], _m[3][3] + rhs._m[3][3]
        );
    }

    Float4x4 Float4x4::operator-(const Float4x4& rhs) const noexcept
    {
        return Float4x4(
            _m[0][0] - rhs._m[0][0], _m[0][1] - rhs._m[0][1], _m[0][2] - rhs._m[0][2], _m[0][3] - rhs._m[0][3],
            _m[1][0] - rhs._m[1][0], _m[1][1] - rhs._m[1][1], _m[1][2] - rhs._m[1][2], _m[1][3] - rhs._m[1][3],
            _m[2][0] - rhs._m[2][0], _m[2][1] - rhs._m[2][1], _m[2][2] - rhs._m[2][2], _m[2][3] - rhs._m[2][3],
            _m[3][0] - rhs._m[3][0], _m[3][1] - rhs._m[3][1], _m[3][2] - rhs._m[3][2], _m[3][3] - rhs._m[3][3]
        );
    }

    Float4x4 Float4x4::operator*(const float s) const noexcept
    {
        return Float4x4(
            _m[0][0] * s, _m[0][1] * s, _m[0][2] * s, _m[0][3] * s,
            _m[1][0] * s, _m[1][1] * s, _m[1][2] * s, _m[1][3] * s,
            _m[2][0] * s, _m[2][1] * s, _m[2][2] * s, _m[2][3] * s,
            _m[3][0] * s, _m[3][1] * s, _m[3][2] * s, _m[3][3] * s
        );
    }

    Float4x4 Float4x4::operator/(const float s) const noexcept
    {
        return Float4x4(
            _m[0][0] / s, _m[0][1] / s, _m[0][2] / s, _m[0][3] / s,
            _m[1][0] / s, _m[1][1] / s, _m[1][2] / s, _m[1][3] / s,
            _m[2][0] / s, _m[2][1] / s, _m[2][2] / s, _m[2][3] / s,
            _m[3][0] / s, _m[3][1] / s, _m[3][2] / s, _m[3][3] / s
        );
    }

    Float4x4 Float4x4::operator*(const Float4x4& rhs) const noexcept
    {
        return mul(*this, rhs);
    }

    Float4 Float4x4::operator*(const Float4& v) const noexcept
    {
        return mul(*this, v);
    }

    Float3 Float4x4::operator*(const Float3& v) const noexcept
    {
        return mul(*this, v);
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
        ::memset(_m, 0, sizeof(float) * 16);
    }

    void Float4x4::setIdentity() noexcept
    {
        _m[0][0] = _m[1][1] = _m[2][2] = _m[3][3] = 1.0f;

        _m[0][1] = _m[0][2] = _m[0][3] = _m[1][0] = _m[1][2] = _m[1][3]
            = _m[2][0] = _m[2][1] = _m[2][3] = _m[3][0] = _m[3][1] = _m[3][2] = 0.0f;
    }

    Float4x4& Float4x4::power(const uint32 exponent) noexcept
    {
        if (exponent == 0)
        {
            setIdentity();
            return *this;
        }

        const mint::Float4x4 original = *this;
        for (uint32 iter = 0; iter < exponent; ++iter)
        {
            *this *= original;
        }
        return *this;
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

    void Float4x4::preScale(const mint::Float3& scale) noexcept
    {
        preScale(scale._x, scale._y, scale._z);
    }

    void Float4x4::postScale(const mint::Float3& scale) noexcept
    {
        postScale(scale._x, scale._y, scale._z);
    }

    void Float4x4::setTranslation(const float x, const float y, const float z) noexcept
    {
        _14 = x;
        _24 = y;
        _34 = z;
    }

    void Float4x4::setTranslation(const mint::Float3& translation) noexcept
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
        mint::Float4 row = _row[0];
        _row[0]._w += row._x * x + row._y * y + row._z * z;
        row = _row[1];
        _row[1]._w += row._x * x + row._y * y + row._z * z;
        row = _row[2];
        _row[2]._w += row._x * x + row._y * y + row._z * z;
        row = _row[3];
        _row[3]._w += row._x * x + row._y * y + row._z * z;
    }

    void Float4x4::preTranslate(const mint::Float3& translation) noexcept
    {
        preTranslate(translation._x, translation._y, translation._z);
    }

    void Float4x4::postTranslate(const mint::Float3& translation) noexcept
    {
        postTranslate(translation._x, translation._y, translation._z);
    }

    mint::Float3 Float4x4::getTranslation() const noexcept
    {
        return mint::Float3(_14, _24, _34);
    }

    void Float4x4::decomposeSrt(mint::Float3& outScale, mint::Float4x4& outRotationMatrix, mint::Float3& outTranslation) const noexcept
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

    Float3x3 Float4x4::minor(const uint32 row, const uint32 col) const noexcept
    {
        Float3x3 result;

        uint32 smallRow{};
        for (uint32 myRow = 0; myRow < 4; ++myRow)
        {
            if (myRow == row) continue;

            uint32 smallCol{};
            for (uint32 myCol = 0; myCol < 4; ++myCol)
            {
                if (myCol == col) continue;

                result._m[smallRow][smallCol] = _m[myRow][myCol];

                ++smallCol;
            }

            ++smallRow;
        }

        return result;
    }

    const float Float4x4::determinant() const noexcept
    {
        const float a = _m[0][0];
        const float b = _m[0][1];
        const float c = _m[0][2];
        const float d = _m[0][3];
        return a * minor(0, 0).determinant() - b * minor(0, 1).determinant() + c * minor(0, 2).determinant() - d * minor(0, 3).determinant();
    }

    Float4x4 Float4x4::transpose() const noexcept
    {
        return Float4x4
        (
            _m[0][0], _m[1][0], _m[2][0], _m[3][0],
            _m[0][1], _m[1][1], _m[2][1], _m[3][1],
            _m[0][2], _m[1][2], _m[2][2], _m[3][2],
            _m[0][3], _m[1][3], _m[2][3], _m[3][3]
        );
    }

    Float4x4 Float4x4::cofactor() const noexcept
    {
        return Float4x4
        (
            +minor(0, 0).determinant(), -minor(0, 1).determinant(), +minor(0, 2).determinant(), -minor(0, 3).determinant(),
            -minor(1, 0).determinant(), +minor(1, 1).determinant(), -minor(1, 2).determinant(), +minor(1, 3).determinant(),
            +minor(2, 0).determinant(), -minor(2, 1).determinant(), +minor(2, 2).determinant(), -minor(2, 3).determinant(),
            -minor(3, 0).determinant(), +minor(3, 1).determinant(), -minor(3, 2).determinant(), +minor(3, 3).determinant()
        );
    }

    Float4x4 Float4x4::adjugate() const noexcept
    {
        return cofactor().transpose();
    }

    Float4x4 Float4x4::inverse() const noexcept
    {
        return adjugate() / determinant();
    }

    Float4x4 Float4x4::mul(const Float4x4& rhs) const noexcept
    {
        return Float4x4
        (
            // row 0
            Float4::dotProductRaw(_row[0]._f, rhs._m[0][0], rhs._m[1][0], rhs._m[2][0], rhs._m[3][0]),
            Float4::dotProductRaw(_row[0]._f, rhs._m[0][1], rhs._m[1][1], rhs._m[2][1], rhs._m[3][1]),
            Float4::dotProductRaw(_row[0]._f, rhs._m[0][2], rhs._m[1][2], rhs._m[2][2], rhs._m[3][2]),
            Float4::dotProductRaw(_row[0]._f, rhs._m[0][3], rhs._m[1][3], rhs._m[2][3], rhs._m[3][3]),

            // row 1
            Float4::dotProductRaw(_row[1]._f, rhs._m[0][0], rhs._m[1][0], rhs._m[2][0], rhs._m[3][0]),
            Float4::dotProductRaw(_row[1]._f, rhs._m[0][1], rhs._m[1][1], rhs._m[2][1], rhs._m[3][1]),
            Float4::dotProductRaw(_row[1]._f, rhs._m[0][2], rhs._m[1][2], rhs._m[2][2], rhs._m[3][2]),
            Float4::dotProductRaw(_row[1]._f, rhs._m[0][3], rhs._m[1][3], rhs._m[2][3], rhs._m[3][3]),

            // row 2
            Float4::dotProductRaw(_row[2]._f, rhs._m[0][0], rhs._m[1][0], rhs._m[2][0], rhs._m[3][0]),
            Float4::dotProductRaw(_row[2]._f, rhs._m[0][1], rhs._m[1][1], rhs._m[2][1], rhs._m[3][1]),
            Float4::dotProductRaw(_row[2]._f, rhs._m[0][2], rhs._m[1][2], rhs._m[2][2], rhs._m[3][2]),
            Float4::dotProductRaw(_row[2]._f, rhs._m[0][3], rhs._m[1][3], rhs._m[2][3], rhs._m[3][3]),

            // row 3
            Float4::dotProductRaw(_row[3]._f, rhs._m[0][0], rhs._m[1][0], rhs._m[2][0], rhs._m[3][0]),
            Float4::dotProductRaw(_row[3]._f, rhs._m[0][1], rhs._m[1][1], rhs._m[2][1], rhs._m[3][1]),
            Float4::dotProductRaw(_row[3]._f, rhs._m[0][2], rhs._m[1][2], rhs._m[2][2], rhs._m[3][2]),
            Float4::dotProductRaw(_row[3]._f, rhs._m[0][3], rhs._m[1][3], rhs._m[2][3], rhs._m[3][3])
        );
    }

    void Float4x4::mulAssignReverse(const Float4x4& lhs) noexcept
    {
        static constexpr uint32 kRowCount = 4;
        static constexpr uint32 kColumnCount = 4;

        mint::Float4x4 copy = *this;
        for (uint32 rowIndex = 0; rowIndex < kRowCount; rowIndex++)
        {
            for (uint32 columnIndex = 0; columnIndex < kColumnCount; columnIndex++)
            {
                _row[rowIndex][columnIndex] = Float4::dotProductRaw(lhs._row[rowIndex]._f, copy._m[0][columnIndex], copy._m[1][columnIndex], copy._m[2][columnIndex], copy._m[3][columnIndex]);
            }
        }
    }

    Float4 Float4x4::mul(const Float4& v) const noexcept
    {
        return Float4
        (
            // x'
            Float4::dot(_row[0], v),

            // y'
            Float4::dot(_row[1], v),

            // z'
            Float4::dot(_row[2], v),

            // w'
            Float4::dot(_row[3], v)
        );
    }

    Float3 Float4x4::mul(const Float3& v) const noexcept
    {
        return Float3
        (
            // x'
            Float3::dotProductRaw(_row[0]._f, &v[0]),
            
            // y'
            Float3::dotProductRaw(_row[1]._f, &v[0]),

            // z'
            Float3::dotProductRaw(_row[2]._f, &v[0])
        );
    }

}
