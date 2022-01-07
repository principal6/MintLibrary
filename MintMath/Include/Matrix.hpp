#pragma once


#include <MintMath/Include/Matrix.h>

#include <MintCommon/Include/Logger.h>

#include <MintMath/Include/VectorR.hpp>
#include <MintMath/Include/Quaternion.h>

#include <initializer_list>


namespace mint
{
    namespace Math
    {
        template<int32 M, int32 N, typename T>
        const bool equals(const T(&lhs)[M][N], const T(&rhs)[M][N], const T epsilon) noexcept
        {
            for (int32 row = 0; row < M; ++row)
            {
                const bool areRowsEqual = Math::equals(lhs[row], rhs[row], epsilon);
                if (areRowsEqual == false)
                {
                    return false;
                }
            }
            return true;
        }

        template<int32 M, int32 N, typename T>
        void copyMat(const T(&src)[M][N], T(&dest)[M][N]) noexcept
        {
            for (int32 row = 0; row < M; ++row)
            {
                for (int32 col = 0; col < N; ++col)
                {
                    dest[row][col] = src[row][col];
                }
            }
        }

        template<int32 M, int32 N, typename T>
        void setAddMat(T(&lhs)[M][N], const T(&rhs)[M][N]) noexcept
        {
            for (int32 row = 0; row < M; ++row)
            {
                for (int32 col = 0; col < N; ++col)
                {
                    lhs[row][col] += rhs[row][col];
                }
            }
        }

        template<int32 M, int32 N, typename T>
        void setSubMat(T(&lhs)[M][N], const T(&rhs)[M][N]) noexcept
        {
            for (int32 row = 0; row < M; ++row)
            {
                for (int32 col = 0; col < N; ++col)
                {
                    lhs[row][col] -= rhs[row][col];
                }
            }
        }

        template<int32 M, int32 N, typename T>
        void setMulMat(T(&mat)[M][N], const T scalar) noexcept
        {
            for (int32 row = 0; row < M; ++row)
            {
                for (int32 col = 0; col < N; ++col)
                {
                    mat[row][col] *= scalar;
                }
            }
        }

        template<int32 M, int32 N, typename T>
        void setDivMat(T(&mat)[M][N], const T scalar) noexcept
        {
            MINT_ASSERT("김장원", scalar != 0.0, "0 으로 나누려 합니다!");
            for (int32 row = 0; row < M; ++row)
            {
                for (int32 col = 0; col < N; ++col)
                {
                    mat[row][col] /= scalar;
                }
            }
        }

        template<int32 M, int32 N, typename T>
        void setRow(const T(&src)[N], T(&dest)[M][N], const int32 destRow) noexcept
        {
            for (int32 col = 0; col < N; ++col)
            {
                dest[destRow][col] = src[col];
            }
        }

        template<int32 M, int32 N, typename T>
        void getRow(const T(&mat)[M][N], const int32 row, T(&out)[N]) noexcept
        {
            for (int32 col = 0; col < N; ++col)
            {
                out[col] = mat[row][col];
            }
        }

        template<int32 M, int32 N, typename T>
        void setCol(const T(&src)[M], T(&dest)[M][N], const int32 destCol) noexcept
        {
            for (int32 row = 0; row < M; ++row)
            {
                dest[row][destCol] = src[row];
            }
        }

        template<int32 M, int32 N, typename T>
        void getCol(const T(&mat)[M][N], const int32 col, T(&out)[M]) noexcept
        {
            for (int32 row = 0; row < M; ++row)
            {
                out[row] = mat[row][col];
            }
        }

        template<int32 M, int32 N, int32 P, typename T>
        void mul(const T(&lhs)[M][N], const T(&rhs)[N][P], T(&out)[M][P]) noexcept
        {
            T lhsRowVec[N];
            T rhsColVec[N];
            T rhsCopy[N][P];
            copyMat(rhs, rhsCopy);
            for (int32 row = 0; row < M; ++row)
            {
                Math::getRow(lhs, row, lhsRowVec);

                for (int32 col = 0; col < P; ++col)
                {
                    Math::getCol(rhsCopy, col, rhsColVec);
                    
                    out[row][col] = Math::dot(lhsRowVec, rhsColVec);
                }
            }
        }

        template<int32 M, int32 N, typename T>
        void mul(const T(&mat)[M][N], const T(&vec)[N], T(&outVec)[M]) noexcept
        {
            T vecCopy[N];
            copyVec(vec, vecCopy);

            for (int32 row = 0; row < M; ++row)
            {
                outVec[row] = Math::dot(mat[row], vecCopy);
            }
        }

        template<int32 M, int32 N, typename T>
        void mul(const T(&vec)[M], const T(&mat)[M][N], T(&outVec)[N]) noexcept
        {
            T vecCopy[M];
            copyVec(vec, vecCopy);

            T matCol[M];
            for (int32 col = 0; col < N; ++col)
            {
                Math::getCol(mat, col, matCol);
                
                outVec[col] = Math::dot(vecCopy, matCol);
            }
        }

        template<int32 N, typename T>
        void setIdentity(T(&inOut)[N][N]) noexcept
        {
            for (int32 row = 0; row < N; ++row)
            {
                for (int32 col = 0; col < N; ++col)
                {
                    inOut[row][col] = static_cast<T>((row == col) ? 1 : 0);
                }
            }
        }

        template<int32 M, int32 N, typename T>
        void setZeroMat(T(&inOut)[M][N]) noexcept
        {
            for (int32 row = 0; row < M; ++row)
            {
                for (int32 col = 0; col < N; ++col)
                {
                    inOut[row][col] = static_cast<T>(0);
                }
            }
        }

        template<int32 M, int32 N, typename T>
        void transpose(const T(&in)[M][N], T(&out)[N][M]) noexcept
        {
            for (int32 row = 0; row < M; ++row)
            {
                for (int32 col = 0; col < N; ++col)
                {
                    out[col][row] = in[row][col];
                }
            }
        }

        template<int32 M, int32 N, typename T>
        void minor(const T(&in)[M][N], const int32 row, const int32 col, T(&out)[M - 1][N - 1]) noexcept
        {
            int32 destRow = 0;
            for (int32 srcRow = 0; srcRow < M; ++srcRow)
            {
                if (row != srcRow)
                {
                    int32 destCol = 0;
                    for (int32 srcCol = 0; srcCol < M; ++srcCol)
                    {
                        if (col != srcCol)
                        {
                            out[destRow][destCol] = in[srcRow][srcCol];

                            ++destCol;
                        }
                    }

                    ++destRow;
                }
            }
        }

        template<int32 N, typename T>
        const T trace(const T(&mat)[N][N]) noexcept
        {
            T result{};
            for (int32 row = 0; row < N; ++row)
            {
                for (int32 col = 0; col < N; ++col)
                {
                    result += mat[row][col];
                }
            }
            return result;
        }

        template<typename T>
        MINT_INLINE const T determinant(const T(&mat)[2][2]) noexcept
        {
            return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
        }
        
        template<typename T>
        MINT_INLINE const T determinant(T _11, T _12, T _21, T _22) noexcept
        {
            return _11 * _22 - _12 * _21;
        }

        template<typename T>
        MINT_INLINE const T determinant(const T(&mat)[3][3]) noexcept
        {
            return mat[0][0] * (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1])
                 - mat[0][1] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0])
                 + mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);
        }

        template<typename T>
        MINT_INLINE const T determinant(T _11, T _12, T _13, T _21, T _22, T _23, T _31, T _32, T _33) noexcept
        {
            return _11 * (_22 * _33 - _23 * _32)
                 - _12 * (_21 * _33 - _23 * _31)
                 + _13 * (_21 * _32 - _22 * _31);
        }

        template<typename T>
        MINT_INLINE const T determinantOfMinor(const T(&mat)[3][3], const int32 row, const int32 col) noexcept
        {
            // 0 => 1 2
            // 1 => 0 2
            // 2 => 0 1
            const int32 r0 = (row ^ 1) & 1;
            const int32 r1 = (r0 ^ 1) & 1;
            const int32 c0 = (col ^ 1) & 1;
            const int32 c1 = (c0 ^ 1) & 1;
            return determinant(mat[r0][c0], mat[r0][c1], mat[r1][c0], mat[r1][c1]);
        }

        template<typename T>
        MINT_INLINE const T determinantOfMinor(const T(&mat)[4][4], const int32 row, const int32 col) noexcept
        {
            // 0 => 1 2 3
            // 1 => 0 2 3
            // 2 => 0 1 3
            // 3 => 0 1 2
            const int32 r0 = (row == 0) ? 1 : 0;
            const int32 r1 = ((row & 2) == 0) ? 2 : 1;
            const int32 r2 = (row == 3) ? 2 : 3;
            const int32 c0 = (col == 0) ? 1 : 0;
            const int32 c1 = ((col & 2) == 0) ? 2 : 1;
            const int32 c2 = (col == 3) ? 2 : 3;
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
        MINT_INLINE void adjugate(const T(&in)[3][3], T(&out)[3][3]) noexcept
        {
            out[0][0] = +determinantOfMinor(in, 0, 0); out[1][0] = -determinantOfMinor(in, 0, 1); out[2][0] = +determinantOfMinor(in, 0, 2);
            out[0][1] = -determinantOfMinor(in, 1, 0); out[1][1] = +determinantOfMinor(in, 1, 1); out[2][1] = -determinantOfMinor(in, 1, 2);
            out[0][2] = +determinantOfMinor(in, 2, 0); out[1][2] = -determinantOfMinor(in, 2, 1); out[2][2] = +determinantOfMinor(in, 2, 2);
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
    }


    template<typename T>
    constexpr T getScalarZero() noexcept
    {
        MINT_NEVER;
        return T();
    }

    template<>
    constexpr float getScalarZero() noexcept
    {
        return 0.0f;
    }

    template<>
    constexpr double getScalarZero() noexcept
    {
        return 0.0;
    }

    template<typename T>
    constexpr T getScalarOne() noexcept
    {
        MINT_NEVER;
        return T();
    }

    template<>
    constexpr float getScalarOne() noexcept
    {
        return 1.0f;
    }

    template<>
    constexpr double getScalarOne() noexcept
    {
        return 1.0;
    }


    template<int32 M, int32 N, typename T>
    inline Matrix<M, N, T>::Matrix()
        : _m{}
    {
        __noop;
    }

    template<int32 M, int32 N, typename T>
    inline Matrix<M, N, T>::Matrix(const std::initializer_list<T>& initializerList)
    {
        const int32 count = min(static_cast<int32>(initializerList.size()), M * N);
        const T* const first = initializerList.begin();
        for (int32 index = 0; index < count; ++index)
        {
            const int32 col = index % M;
            const int32 row = index / M;
            _m[row][col] = *(first + index);
        }
    }

    template<int32 M, int32 N, typename T>
    inline Matrix<M, N, T>::~Matrix()
    {
        __noop;
    }

    template<int32 M, int32 N, typename T>
    inline const bool Matrix<M, N, T>::operator==(const Matrix& rhs) const noexcept
    {
        return Math::equals(_m, rhs._m);
    }

    template<int32 M, int32 N, typename T>
    inline const bool Matrix<M, N, T>::operator!=(const Matrix& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE Matrix<M, N, T>& Matrix<M, N, T>::operator*=(const T scalar) noexcept
    {
        Math::setMulMat(_m, scalar);
        return *this;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE Matrix<M, N, T>& Matrix<M, N, T>::operator/=(const T scalar) noexcept
    {
        Math::setDivMat(_m, scalar);
        return *this;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE Matrix<M, N, T>& Matrix<M, N, T>::operator+=(const Matrix& rhs) noexcept
    {
        Math::setAddMat(_m, rhs._m);
        return *this;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE Matrix<M, N, T>& Matrix<M, N, T>::operator-=(const Matrix& rhs) noexcept
    {
        Math::setSubMat(_m, rhs._m);
        return *this;
    }

    template<int32 M, int32 N, typename T>
    inline Matrix<M, N, T>& Matrix<M, N, T>::operator*=(const Matrix<N, N, T>& rhs) noexcept
    {
        static_assert(M == N, "Power of non-square matrix!!!");
        Math::mul(_m, rhs._m, _m);
        return *this;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE Matrix<M, N, T> Matrix<M, N, T>::operator*(const T scalar) noexcept
    {
        Matrix result = *this;
        result *= scalar;
        return result;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE Matrix<M, N, T> Matrix<M, N, T>::operator/(const T scalar) noexcept
    {
        Matrix result = *this;
        result /= scalar;
        return result;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE Matrix<M, N, T> Matrix<M, N, T>::operator+(const Matrix& rhs) noexcept
    {
        Matrix result = *this;
        result += rhs;
        return result;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE Matrix<M, N, T> Matrix<M, N, T>::operator-(const Matrix& rhs) noexcept
    {
        Matrix result = *this;
        result -= rhs;
        return result;
    }

    template<int32 M, int32 N, typename T>
    template<int32 P>
    MINT_INLINE Matrix<M, P, T> Matrix<M, N, T>::operator*(const Matrix<N, P, T>& rhs) const noexcept
    {
        Matrix<M, P, T> result;
        Math::mul(_m, rhs._m, result._m);
        return result;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE void Matrix<M, N, T>::setElement(const uint32 rowIndex, const uint32 columnIndex, const T value) noexcept
    {
        MINT_ASSERT("김장원", (rowIndex < static_cast<uint32>(M) && columnIndex < static_cast<uint32>(N)), "범위를 벗어난 접근입니다!");
        _m[rowIndex][columnIndex] = value;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE void Matrix<M, N, T>::addElement(const uint32 rowIndex, const uint32 columnIndex, const T value) noexcept
    {
        MINT_ASSERT("김장원", (rowIndex < static_cast<uint32>(M) && columnIndex < static_cast<uint32>(N)), "범위를 벗어난 접근입니다!");
        _m[rowIndex][columnIndex] += value;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE void Matrix<M, N, T>::mulElement(const uint32 rowIndex, const uint32 columnIndex, const T value) noexcept
    {
        MINT_ASSERT("김장원", (rowIndex < static_cast<uint32>(M) && columnIndex < static_cast<uint32>(N)), "범위를 벗어난 접근입니다!");
        _m[rowIndex][columnIndex] *= value;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE const T Matrix<M, N, T>::getElement(const uint32 rowIndex, const uint32 columnIndex) const noexcept
    {
        MINT_ASSERT("김장원", (rowIndex < static_cast<uint32>(M) && columnIndex < static_cast<uint32>(N)), "범위를 벗어난 접근입니다!");
        return _m[rowIndex][columnIndex];
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE void Matrix<M, N, T>::setRow(const uint32 rowIndex, const VectorR<N, T>& row) noexcept
    {
        if (rowIndex < static_cast<uint32>(M))
        {
            Math::setRow(row._c, _m, rowIndex);
        }
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE VectorR<N, T> Matrix<M, N, T>::getRow(const uint32 rowIndex) const noexcept
    {
        MINT_ASSERT("김장원", rowIndex < static_cast<uint32>(M), "범위를 벗어난 접근입니다!");

        VectorR<N, T> result;
        Math::getRow(_m, rowIndex, result._c);
        return result;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE void Matrix<M, N, T>::setColumn(const uint32 columnIndex, const VectorR<M, T>& column) noexcept
    {
        if (columnIndex < static_cast<uint32>(N))
        {
            Math::setCol(column._c, _m, static_cast<int32>(columnIndex));
        }
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE VectorR<M, T> Matrix<M, N, T>::getColumn(const uint32 columnIndex) const noexcept
    {
        MINT_ASSERT("김장원", columnIndex < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");

        VectorR<M, T> result;
        Math::getCol(_m, columnIndex, result._c);
        return result;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE VectorR<N, T> Matrix<M, N, T>::multiplyRowVector(const VectorR<M, T>& rowVector) const noexcept
    {
        VectorR<N, T> result;
        Math::mul(rowVector._c, _m, result._c);
        return result;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE VectorR<M, T> Matrix<M, N, T>::multiplyColumnVector(const VectorR<N, T>& columnVector) const noexcept
    {
        VectorR<M, T> result;
        Math::mul(_m, columnVector._c, result._c);
        return result;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE Matrix<N, M, T> Matrix<M, N, T>::transpose() const noexcept
    {
        Matrix<N, M, T> result;
        Math::transpose(_m, result._m);
        return result;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE const T Matrix<M, N, T>::trace() const noexcept
    {
        if (isSquareMatrix() == false)
        {
            MINT_LOG_ERROR("김장원", "Tried to compute trace from a non-square matrix!");
        }

        return Math::trace(_m);
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE Matrix<M - 1, N - 1, T> Matrix<M, N, T>::minor(const uint32 row, const uint32 col) const noexcept
    {
        Matrix<M - 1, N - 1, T> result;
        Math::minor(_m, row, col, result._m);
        return result;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE constexpr const bool Matrix<M, N, T>::isSquareMatrix() const noexcept
    {
        return (M == N);
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE const bool Matrix<M, N, T>::isDiagonalMatrix() const noexcept
    {
        if (isSquareMatrix() == false)
        {
            return false;
        }

        for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
        {
            for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
            {
                if (columnIndex != rowIndex)
                {
                    if (_m[rowIndex][columnIndex] != 0)
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE const bool Matrix<M, N, T>::isScalarMatrix() const noexcept
    {
        if (isSquareMatrix() == false)
        {
            return false;
        }
            
        const T scale = _m[0][0];
        for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
        {
            for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
            {
                if (_m[rowIndex][columnIndex] != ((columnIndex == rowIndex) ? scale : 0))
                {
                    return false;
                }
            }
        }
        return true;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE const bool Matrix<M, N, T>::isIdentityMatrix() const noexcept
    {
        if (isSquareMatrix() == false)
        {
            return false;
        }

        for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
        {
            for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
            {
                if (_m[rowIndex][columnIndex] != ((columnIndex == rowIndex) ? 1 : 0))
                {
                    return false;
                }
            }
        }
        return true;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE const bool Matrix<M, N, T>::isZeroMatrix() const noexcept
    {
        for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
        {
            for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
            {
                if (_m[rowIndex][columnIndex] != 0)
                {
                    return false;
                }
            }
        }
        return true;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE const bool Matrix<M, N, T>::isSymmetricMatrix() const noexcept
    {
        if (isSquareMatrix() == false)
        {
            return false;
        }

        for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
        {
            for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
            {
                if (rowIndex == columnIndex)
                {
                    continue;
                }

                if (_m[rowIndex][columnIndex] != _m[columnIndex][rowIndex])
                {
                    return false;
                }
            }
        }
        return true;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE const bool Matrix<M, N, T>::isSkewSymmetricMatrix() const noexcept
    {
        if (isSquareMatrix() == false)
        {
            return false;
        }

        for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
        {
            for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
            {
                if (rowIndex == columnIndex)
                {
                    continue;
                }

                if (_m[rowIndex][columnIndex] != -_m[columnIndex][rowIndex])
                {
                    return false;
                }
            }
        }
        return true;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE const bool Matrix<M, N, T>::isUpperTriangularMatrix() const noexcept
    {
        if (isSquareMatrix() == false)
        {
            return false;
        }

        for (int32 rowIndex = 1; rowIndex < M; ++rowIndex)
        {
            for (int32 columnIndex = 0; columnIndex < rowIndex; ++columnIndex)
            {
                if (_m[rowIndex][columnIndex] != 0)
                {
                    return false;
                }
            }
        }

        return true;
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE const bool Matrix<M, N, T>::isIdempotentMatrix() const noexcept
    {
        Matrix<M, N, T> squared = *this;
        squared *= *this;
        return (squared == *this);
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE void Matrix<M, N, T>::setIdentity() noexcept
    {
        if (isSquareMatrix() == false)
        {
            MINT_LOG_ERROR("김장원", "Non-square matrix is set to identity!");
        }

        Math::setIdentity(_m);
    }

    template<int32 M, int32 N, typename T>
    MINT_INLINE void Matrix<M, N, T>::setZero() noexcept
    {
        Math::setZeroMat(_m);
    }


    template<int32 M, int32 N, typename T>
    MINT_INLINE Matrix<M, N, T> operator*(const T scalar, const Matrix<M, N, T>& matrix) noexcept
    {
        return (matrix * scalar);
    }
        
    template<int32 M, int32 N, typename T>
    MINT_INLINE VectorR<N, T> operator*(const VectorR<M, T>& rowVector, const Matrix<M, N, T>& matrix) noexcept
    {
        return matrix.multiplyRowVector(rowVector);
    }
        
    template<int32 M, int32 N, typename T>
    MINT_INLINE VectorR<M, T> operator*(const Matrix<M, N, T>& matrix, const VectorR<N, T>& columnVector) noexcept
    {
        return matrix.multiplyColumnVector(columnVector);
    }


    namespace MatrixUtils
    {
        template<int32 N, typename T>
        const Matrix<N, N, T>& identity() noexcept
        {
            static bool isFirstTime = true;
            static Matrix<N, N, T> identityMatrix;
            if (isFirstTime)
            {
                identityMatrix.setIdentity();
                isFirstTime = false;
            }
            return identityMatrix;
        }
    }
}
