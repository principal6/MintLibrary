#pragma once


namespace mint
{
#pragma region Free functions
    template<typename T>
    MINT_INLINE AffineMat<T> operator*(const T scalar, const AffineMat<T>& mat) noexcept
    {
        return mat * scalar;
    }

    template<typename T>
    MINT_INLINE AffineVec<T> operator*(const AffineVec<T>& rowVec, const AffineMat<T>& mat) noexcept
    {
        return AffineVec<T>(mat.getCol(0).dot(rowVec), mat.getCol(1).dot(rowVec), mat.getCol(2).dot(rowVec), mat.getCol(3).dot(rowVec));
    }

    template<typename T>
    MINT_INLINE AffineVec<T> operator*(const AffineMat<T>& mat, const AffineVec<T>& colVec) noexcept
    {
        return AffineVec<T>(mat.getRow(0).dot(colVec), mat.getRow(1).dot(colVec), mat.getRow(2).dot(colVec), mat.getRow(3).dot(colVec));
    }

    template<typename T>
    MINT_INLINE AffineMat<T> translationMatrix(const AffineVec<T>& translation) noexcept
    {
        T t[4];
        translation.get(t);
        return AffineMat<T>
        (
            1, 0, 0, t[0],
            0, 1, 0, t[1],
            0, 0, 1, t[2],
            0, 0, 0, 1
        );
    }
    
    template<typename T>
    MINT_INLINE AffineMat<T> scalarMatrix(const T scalar) noexcept
    {
        return AffineMat<T>
        (
            scalar,      0,      0,      0,
                 0, scalar,      0,      0,
                 0,      0, scalar,      0,
                 0,      0,      0,      1
        );
    }
    
    template<typename T>
    MINT_INLINE AffineMat<T> scalarMatrix(const AffineVec<T>& scalar) noexcept
    {
        T s[4];
        scalar.get(s);
        return AffineMat<T>
        (
            s[0],    0,    0,    0,
               0, s[1],    0,    0,
               0,    0, s[2],    0,
               0,    0,    0,    1
        );
    }

    template<typename T>
    MINT_INLINE AffineMat<T> rotationMatrixX(const T angle) noexcept
    {
        return AffineMat<T>
        (
            1.0f          , 0.0f          , 0.0f          , 0.0f,
            0.0f          , +::cos(angle) , -::sin(angle) , 0.0f,
            0.0f          , +::sin(angle) , +::cos(angle) , 0.0f,
            0.0f          , 0.0f          , 0.0f          , 1.0f
        );
    }

    template<typename T>
    MINT_INLINE AffineMat<T> rotationMatrixY(const T angle) noexcept
    {
        return AffineMat<T>
        (
            +::cos(angle) , 0.0f          , +::sin(angle) , 0.0f,
            0.0f          , 1.0f          , 0.0f          , 0.0f,
            -::sin(angle) , 0.0f          , +::cos(angle) , 0.0f,
            0.0f          , 0.0f          , 0.0f          , 1.0f
        );
    }

    template<typename T>
    MINT_INLINE AffineMat<T> rotationMatrixZ(const T angle) noexcept
    {
        return AffineMat<T>
        (
            +::cos(angle) , -::sin(angle) , 0.0f          , 0.0f,
            +::sin(angle) , +::cos(angle) , 0.0f          , 0.0f,
            0.0f          , 0.0f          , 1.0f          , 0.0f,
            0.0f          , 0.0f          , 0.0f          , 1.0f
        );
    }

    template<typename T>
    MINT_INLINE AffineMat<T> rotationMatrixRollPitchYaw(const T pitch, const T yaw, const T roll) noexcept
    {
        return rotationMatrixY(yaw) * rotationMatrixX(pitch) * rotationMatrixZ(roll);
    }

    template<typename T>
    MINT_INLINE AffineMat<T> rotationMatrixAxisAngle(const AffineVec<T>& axis, const T angle) noexcept
    {
        // (v * r)r(1 - cosθ) + vcosθ + (r X v)sinθ

        T r[4];
        normalize(axis).get(r);
        const T c = cosf(angle);
        const T s = sinf(angle);

        const T rx = r[0];
        const T ry = r[1];
        const T rz = r[2];
        AffineMat<T> result
        (
            (1 - c) * rx * rx  + c            , (1 - c) * ry * rx       - (rz * s), (1 - c) * rz * rx       + (ry * s), 0,
            (1 - c) * rx * ry       + (rz * s), (1 - c) * ry * ry  + c            , (1 - c) * rz * ry       - (rx * s), 0,
            (1 - c) * rx * rz       - (ry * s), (1 - c) * ry * rz       + (rx * s), (1 - c) * rz * rz  + c            , 0,
            0                                 , 0                                 , 0                                 , 1
        );
        return result;
    }

    template<typename T>
    MINT_INLINE AffineMat<T> rotationMatrixFromAxes(const AffineVec<T>& axisX, const AffineVec<T>& axisY, const AffineVec<T>& axisZ) noexcept
    {
        return axesToColumns(axisX, axisY, axisZ);
    }

    template<typename T>
    MINT_INLINE AffineMat<T> rotationMatrix(const Quaternion<T>& rotation) noexcept
    {
        T axis[3];
        T angle;
        rotation.getAxisAngle(axis, angle);
        return rotationMatrixAxisAngle(AffineVec<T>(axis[0], axis[1], axis[2], 0), angle);
    }

    template<typename T>
    MINT_INLINE AffineMat<T> axesToColumns(const AffineVec<T>& axisX, const AffineVec<T>& axisY, const AffineVec<T>& axisZ) noexcept
    {
        T aX[4];
        T aY[4];
        T aZ[4];
        axisX.get(aX);
        axisY.get(aY);
        axisZ.get(aZ);
        return AffineMat<T>
        (
            aX[0], aY[0], aZ[0], 0,
            aX[1], aY[1], aZ[1], 0,
            aX[2], aY[2], aZ[2], 0,
                0,     0,     0, 1
        );
    }

    template<typename T>
    MINT_INLINE AffineMat<T> axesToRows(const AffineVec<T>& axisX, const AffineVec<T>& axisY, const AffineVec<T>& axisZ) noexcept
    {
        T aX[4];
        T aY[4];
        T aZ[4];
        axisX.get(aX);
        axisY.get(aY);
        axisZ.get(aZ);
        return AffineMat<T>
        (
            aX[0], aX[1], aX[2], 0,
            aY[0], aY[1], aY[2], 0,
            aZ[0], aZ[1], aZ[2], 0,
                0,     0,     0, 1
        );
    }

    template<typename T>
    MINT_INLINE AffineMat<T> srtMatrix(const AffineVec<T>& scale, const Quaternion<T>& rotation, const AffineVec<T>& translation) noexcept
    {
        // SRT matrix for column vector is like below:
        // SRT = T * R * S
        // which is the same as below..
        AffineMat<T> matrix = rotationMatrix(rotation);
        matrix.preTranslate(translation._x, translation._y, translation._z);
        matrix.postScale(scale._x, scale._y, scale._z);
        return matrix;
    }
#pragma endregion


    inline AffineMat<float>::AffineMat()
        : _rows{ AffineVecF(1, 0, 0, 0), AffineVecF(0, 1, 0, 0), AffineVecF(0, 0, 1, 0),AffineVecF(0, 0, 0, 1) }
    {
        __noop;
    }

    inline AffineMat<float>::AffineMat(const AffineVecF& row0, const AffineVecF& row1, const AffineVecF& row2, const AffineVecF& row3)
        : _rows{ row0, row1, row2, row3 }
    {
        __noop;
    }

    inline AffineMat<float>::AffineMat(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44)
        : _rows{ { _11, _12, _13, _14 }, { _21, _22, _23, _24 }, { _31, _32, _33, _34 }, { _41, _42, _43, _44 } }
    {
        __noop;
    }

    inline AffineMat<float>::AffineMat(const float(&mat)[4][4])
        : AffineMat(mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0], mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1], mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2], mat[3][3])
    {
        __noop;
    }

    MINT_INLINE bool AffineMat<float>::operator==(const AffineMat& rhs) const noexcept
    {
        return (_rows[0] == rhs._rows[0]) && (_rows[1] == rhs._rows[1]) && (_rows[2] == rhs._rows[2]) && (_rows[3] == rhs._rows[3]);
    }

    MINT_INLINE bool AffineMat<float>::operator!=(const AffineMat& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    MINT_INLINE AffineMat<float>& AffineMat<float>::operator+() noexcept
    {
        return *this;
    }

    MINT_INLINE AffineMat<float> AffineMat<float>::operator-() const noexcept
    {
        return AffineMat(-_rows[0], -_rows[1], -_rows[2], -_rows[3]);
    }

    MINT_INLINE AffineMat<float> AffineMat<float>::operator+(const AffineMat& rhs) const noexcept
    {
        return AffineMat(_rows[0] + rhs._rows[0], _rows[1] + rhs._rows[1], _rows[2] + rhs._rows[2], _rows[3] + rhs._rows[3]);
    }

    MINT_INLINE AffineMat<float> AffineMat<float>::operator-(const AffineMat& rhs) const noexcept
    {
        return AffineMat(_rows[0] - rhs._rows[0], _rows[1] - rhs._rows[1], _rows[2] - rhs._rows[2], _rows[3] - rhs._rows[3]);
    }

    MINT_INLINE AffineMat<float> AffineMat<float>::operator*(const AffineMat& rhs) const noexcept
    {
        return AffineMat
            (
                _rows[0].dot(rhs.getCol(0)), _rows[0].dot(rhs.getCol(1)), _rows[0].dot(rhs.getCol(2)), _rows[0].dot(rhs.getCol(3)),
                _rows[1].dot(rhs.getCol(0)), _rows[1].dot(rhs.getCol(1)), _rows[1].dot(rhs.getCol(2)), _rows[1].dot(rhs.getCol(3)),
                _rows[2].dot(rhs.getCol(0)), _rows[2].dot(rhs.getCol(1)), _rows[2].dot(rhs.getCol(2)), _rows[2].dot(rhs.getCol(3)),
                _rows[3].dot(rhs.getCol(0)), _rows[3].dot(rhs.getCol(1)), _rows[3].dot(rhs.getCol(2)), _rows[3].dot(rhs.getCol(3))
            );
    }

    MINT_INLINE AffineMat<float> AffineMat<float>::operator*(const float scalar) const noexcept
    {
        return AffineMat(_rows[0] * scalar, _rows[1] * scalar, _rows[2] * scalar, _rows[3] * scalar);
    }

    MINT_INLINE AffineMat<float> AffineMat<float>::operator/(const float scalar) const noexcept
    {
        return AffineMat(_rows[0] / scalar, _rows[1] / scalar, _rows[2] / scalar, _rows[3] / scalar);
    }

    MINT_INLINE AffineMat<float>& AffineMat<float>::operator+=(const AffineMat& rhs) noexcept
    {
        _rows[0] += rhs._rows[0];
        _rows[1] += rhs._rows[1];
        _rows[2] += rhs._rows[2];
        _rows[3] += rhs._rows[3];
        return *this;
    }

    MINT_INLINE AffineMat<float>& AffineMat<float>::operator-=(const AffineMat& rhs) noexcept
    {
        _rows[0] -= rhs._rows[0];
        _rows[1] -= rhs._rows[1];
        _rows[2] -= rhs._rows[2];
        _rows[3] -= rhs._rows[3];
        return *this;
    }

    MINT_INLINE AffineMat<float>& AffineMat<float>::operator*=(const AffineMat& rhs) noexcept
    {
        *this = (*this * rhs);
        return *this;
    }

    MINT_INLINE AffineMat<float>& AffineMat<float>::operator*=(const float scalar) noexcept
    {
        _rows[0] *= scalar;
        _rows[1] *= scalar;
        _rows[2] *= scalar;
        _rows[3] *= scalar;
        return *this;
    }

    MINT_INLINE AffineMat<float>& AffineMat<float>::operator/=(const float scalar) noexcept
    {
        _rows[0] /= scalar;
        _rows[1] /= scalar;
        _rows[2] /= scalar;
        _rows[3] /= scalar;
        return *this;
    }

    MINT_INLINE void AffineMat<float>::set(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44) noexcept
    {
        _rows[0].set(_11, _12, _13, _14);
        _rows[1].set(_21, _22, _23, _24);
        _rows[2].set(_31, _32, _33, _34);
        _rows[3].set(_41, _42, _43, _44);
    }

    MINT_INLINE void AffineMat<float>::setZero() noexcept
    {
        _rows[0].set(0, 0, 0, 0);
        _rows[1].set(0, 0, 0, 0);
        _rows[2].set(0, 0, 0, 0);
        _rows[3].set(0, 0, 0, 0);
    }

    MINT_INLINE void AffineMat<float>::setIdentity() noexcept
    {
        _rows[0].set(1, 0, 0, 0);
        _rows[1].set(0, 1, 0, 0);
        _rows[2].set(0, 0, 1, 0);
        _rows[3].set(0, 0, 0, 1);
    }
    
    MINT_INLINE void AffineMat<float>::setRow(const int32 row, const AffineVecF& vec) noexcept
    {
        _rows[row] = vec;
    }
    
    MINT_INLINE void AffineMat<float>::setCol(const int32 col, const AffineVecF& vec) noexcept
    {
        float v[4];
        vec.get(v);
        _rows[0].setComponent(col, v[0]);
        _rows[1].setComponent(col, v[1]);
        _rows[2].setComponent(col, v[2]);
        _rows[3].setComponent(col, v[3]);
    }

    MINT_INLINE void AffineMat<float>::setElement(const int32 row, const int32 col, const float scalar) noexcept
    {
        _rows[row].setComponent(col, scalar);
    }
    
    MINT_INLINE void AffineMat<float>::get(float(&mat)[4][4]) const noexcept
    {
        _rows[0].get(mat[0]);
        _rows[1].get(mat[1]);
        _rows[2].get(mat[2]);
        _rows[3].get(mat[3]);
    }

    MINT_INLINE const AffineVecF& AffineMat<float>::getRow(const int32 row) const noexcept
    {
        return _rows[row];
    }

    MINT_INLINE AffineVecF AffineMat<float>::getCol(const int32 col) const noexcept
    {
        return AffineVecF(_rows[0].getComponent(col), _rows[1].getComponent(col), _rows[2].getComponent(col), _rows[3].getComponent(col));
    }

    MINT_INLINE void AffineMat<float>::preScale(const AffineVecF& scale) noexcept
    {
        float s[4];
        scale.get(s);
        _rows[0] *= s[0];
        _rows[1] *= s[1];
        _rows[2] *= s[2];
    }

    MINT_INLINE void AffineMat<float>::postScale(const AffineVecF& scale) noexcept
    {
        AffineVecF scaleW0 = scale;
        scaleW0.setComponent(3, 0.0f);
        _rows[0] *= scaleW0;
        _rows[1] *= scaleW0;
        _rows[2] *= scaleW0;
        _rows[3] *= scaleW0;
    }

    MINT_INLINE void AffineMat<float>::preTranslate(const AffineVecF& translation) noexcept
    {
        float t[4];
        translation.get(t);
        _rows[0].addComponent(3, t[0]);
        _rows[1].addComponent(3, t[1]);
        _rows[2].addComponent(3, t[2]);
    }

    MINT_INLINE void AffineMat<float>::postTranslate(const AffineVecF& translation) noexcept
    {
        AffineVecF rowCopy = _rows[0];
        rowCopy.setComponent(3, 0.0f);
        _rows[0].addComponent(3, rowCopy.dot(rowCopy));
        
        rowCopy = _rows[1];
        rowCopy.setComponent(3, 0.0f);
        _rows[1].addComponent(3, rowCopy.dot(rowCopy));
        
        rowCopy = _rows[2];
        rowCopy.setComponent(3, 0.0f);
        _rows[2].addComponent(3, rowCopy.dot(rowCopy));
        
        rowCopy = _rows[3];
        rowCopy.setComponent(3, 0.0f);
        _rows[3].addComponent(3, rowCopy.dot(rowCopy));
    }

    MINT_INLINE void AffineMat<float>::decomposeSRT(AffineVecF& outScale, AffineMat& outRotationMatrix, AffineVecF& outTranslation) const noexcept
    {
        // TODO: avoid nan in outRotationMatrix

        float m[4][4];
        get(m);

        // SRT Matrix
        // 
        // | s_x * r_11  s_y * r_12  s_z * r_13  t_x |
        // | s_x * r_21  s_y * r_22  s_z * r_23  t_y |
        // | s_x * r_31  s_y * r_32  s_z * r_33  t_z |
        // | 0           0           0           1   |

        // s
        const float sx = ::sqrt((m[0][0] * m[0][0]) + (m[1][0] * m[1][0]) + (m[2][0] * m[2][0]));
        const float sy = ::sqrt((m[0][1] * m[0][1]) + (m[1][1] * m[1][1]) + (m[2][1] * m[2][1]));
        const float sz = ::sqrt((m[0][2] * m[0][2]) + (m[1][2] * m[1][2]) + (m[2][2] * m[2][2]));
        outScale.set(sx, sy, sz, 1.0f);

        // r
        outRotationMatrix.setIdentity();
        outRotationMatrix.setRow(0, _rows[0] / outScale);
        outRotationMatrix.setRow(1, _rows[1] / outScale);
        outRotationMatrix.setRow(2, _rows[2] / outScale);
        outRotationMatrix.setElement(0, 3, 0.0f);
        outRotationMatrix.setElement(1, 3, 0.0f);
        outRotationMatrix.setElement(2, 3, 0.0f);

        // t
        outTranslation.set(m[0][3], m[1][3], m[2][3], 1.0f);

        // s !!
        outScale.setComponent(3, 0.0f);
    }

    MINT_INLINE AffineMat<float> AffineMat<float>::inverse() const noexcept
    {
        float m[4][4];
        _rows[0].get(m[0]);
        _rows[1].get(m[1]);
        _rows[2].get(m[2]);
        _rows[3].get(m[3]);

        float adj[4][4];
        Math::adjugate(m, adj);
        const float det = Math::determinant(m);
        AffineMat inv = AffineMat(adj);
        inv /= det;
        return inv;
    }
}
