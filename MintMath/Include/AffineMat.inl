#pragma once


namespace mint
{
#pragma region Helpers for inverse
    template<typename T>
    MINT_INLINE const T determinant(T _11, T _12, T _13, T _21, T _22, T _23, T _31, T _32, T _33) noexcept
    {
        return _11 * (_22 * _33 - _23 * _32)
            - _12 * (_21 * _33 - _23 * _31)
            + _13 * (_21 * _32 - _22 * _31);
    }

    template<typename T>
    MINT_INLINE const T determinantOfMinor(const T(&mat)[4][4], const int32 row, const int32 col) noexcept
    {
        // 0 => 1 2 3
        // 1 => 0 2 3
        // 2 => 0 1 3
        // 3 => 0 1 2
        const int32 r0 = (0 == row) ? 1 : 0;
        const int32 r1 = (0 == (row & 2)) ? 2 : 1;
        const int32 r2 = (3 == row) ? 2 : 3;
        const int32 c0 = (0 == col) ? 1 : 0;
        const int32 c1 = (0 == (col & 2)) ? 2 : 1;
        const int32 c2 = (3 == col) ? 2 : 3;
        return determinant(mat[r0][c0], mat[r0][c1], mat[r0][c2], mat[r1][c0], mat[r1][c1], mat[r1][c2], mat[r2][c0], mat[r2][c1], mat[r2][c2]);
    }

    template<typename T>
    MINT_INLINE const T determinant(const T(&mat)[4][4]) noexcept
    {
        return mat[0][0] * determinantOfMinor(mat, 0, 0) - mat[0][1] * determinantOfMinor(mat, 0, 1) 
             + mat[0][2] * determinantOfMinor(mat, 0, 2) - mat[0][3] * determinantOfMinor(mat, 0, 3);
    }

    // transpose of cofactor
    template<typename T>
    MINT_INLINE void adjugate(const T(&in)[4][4], T(&out)[4][4]) noexcept
    {
        out[0][0] = +determinantOfMinor(in, 0, 0); out[1][0] = -determinantOfMinor(in, 0, 1); out[2][0] = +determinantOfMinor(in, 0, 2); out[3][0] = -determinantOfMinor(in, 0, 3);
        out[0][1] = -determinantOfMinor(in, 1, 0); out[1][1] = +determinantOfMinor(in, 1, 1); out[2][1] = -determinantOfMinor(in, 1, 2); out[3][1] = +determinantOfMinor(in, 1, 3);
        out[0][2] = +determinantOfMinor(in, 2, 0); out[1][2] = -determinantOfMinor(in, 2, 1); out[2][2] = +determinantOfMinor(in, 2, 2); out[3][2] = -determinantOfMinor(in, 2, 3);
        out[0][3] = -determinantOfMinor(in, 3, 0); out[1][3] = +determinantOfMinor(in, 3, 1); out[2][3] = -determinantOfMinor(in, 3, 2); out[3][3] = +determinantOfMinor(in, 3, 3);
    }
#pragma endregion


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

    MINT_INLINE const bool AffineMat<float>::operator==(const AffineMat& rhs) const noexcept
    {
        return (_rows[0] == rhs._rows[0]) && (_rows[1] == rhs._rows[1]) && (_rows[2] == rhs._rows[2]) && (_rows[3] == rhs._rows[3]);
    }

    MINT_INLINE const bool AffineMat<float>::operator!=(const AffineMat& rhs) const noexcept
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

    MINT_INLINE AffineMat<float> AffineMat<float>::operator*(const float scalar) const noexcept
    {
        return AffineMat(_rows[0] * scalar, _rows[1] * scalar, _rows[2] * scalar, _rows[3] * scalar);
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

    MINT_INLINE AffineMat<float>& AffineMat<float>::operator*=(const float scalar) noexcept
    {
        _rows[0] *= scalar;
        _rows[1] *= scalar;
        _rows[2] *= scalar;
        _rows[3] *= scalar;
        return *this;
    }

    MINT_INLINE AffineMat<float>& AffineMat<float>::operator*=(const AffineMat& rhs) noexcept
    {
        *this = (*this * rhs);
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

    MINT_INLINE AffineMat<float> AffineMat<float>::inverse() const noexcept
    {
        float m[4][4];
        _rows[0].get(m[0]);
        _rows[1].get(m[1]);
        _rows[2].get(m[2]);
        _rows[3].get(m[3]);

        float adj[4][4];
        adjugate(m, adj);
        const float det = determinant(m);
        AffineMat inv = AffineMat(adj);
        inv /= det;
        return inv;
    }
}
