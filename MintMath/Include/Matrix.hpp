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
                if (false == areRowsEqual)
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
                if (row != destRow)
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
        const T determinant(const T(&mat)[2][2]) noexcept
        {
            const T a = mat[0][0];
            const T b = mat[0][1];
            const T c = mat[1][0];
            const T d = mat[1][1];
            return static_cast<T>(a * d - b * c);
        }

        template<typename T>
        const T determinant(const T(&mat)[3][3]) noexcept
        {
            const T a = mat[0][0];
            const T b = mat[0][1];
            const T c = mat[0][2];
            T minorA[2][2];
            T minorB[2][2];
            T minorC[2][2];
            minor(mat, 0, 0, minorA);
            minor(mat, 0, 1, minorB);
            minor(mat, 0, 2, minorC);
            return a * determinant(minorA) - b * determinant(minorB) + c * determinant(minorC);
        }

        template<typename T>
        const T determinant(const T(&mat)[4][4]) noexcept
        {
            const T a = mat[0][0];
            const T b = mat[0][1];
            const T c = mat[0][2];
            const T d = mat[0][3];
            T minorA[3][3];
            T minorB[3][3];
            T minorC[3][3];
            T minorD[3][3];
            minor(mat, 0, 0, minorA);
            minor(mat, 0, 1, minorB);
            minor(mat, 0, 2, minorC);
            minor(mat, 0, 3, minorD);
            return a * determinant(minorA) - b * determinant(minorB) + c * determinant(minorC) - d * determinant(minorD);
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
            MINT_LOG_ERROR("김장원", "Tried to calculate trace from a non-square matrix!");
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
        if (false == isSquareMatrix())
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
        if (false == isSquareMatrix())
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
        if (false == isSquareMatrix())
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
            if (true == isFirstTime)
            {
                identityMatrix.setIdentity();
                isFirstTime = false;
            }
            return identityMatrix;
        }
            
        template<typename T>
        const T determinant(const Matrix2x2<T>& in) noexcept
        {
            return Math::determinant(in._m);
        }

        template<typename T>
        const T determinant(const Matrix3x3<T>& in) noexcept
        {
            return Math::determinant(in._m);
        }

        template<typename T>
        const T determinant(const Matrix4x4<T>& in) noexcept
        {
            return Math::determinant(in._m);
        }

        template<typename T>
        Matrix4x4<T> cofactor(const Matrix4x4<T>& in) noexcept
        {
            return Matrix4x4<T>
            (
                {
                    +determinant(in.minor(0, 0)), -determinant(in.minor(0, 1)), +determinant(in.minor(0, 2)), -determinant(in.minor(0, 3)),
                    -determinant(in.minor(1, 0)), +determinant(in.minor(1, 1)), -determinant(in.minor(1, 2)), +determinant(in.minor(1, 3)),
                    +determinant(in.minor(2, 0)), -determinant(in.minor(2, 1)), +determinant(in.minor(2, 2)), -determinant(in.minor(2, 3)),
                    -determinant(in.minor(3, 0)), +determinant(in.minor(3, 1)), -determinant(in.minor(3, 2)), +determinant(in.minor(3, 3))
                }
            );
        }

        template<typename T>
        Matrix4x4<T> adjugate(const Matrix4x4<T>& in) noexcept
        {
            return cofactor(in).transpose();
        }

        template<typename T>
        Matrix4x4<T> inverse(const Matrix4x4<T>& in) noexcept
        {
            return adjugate(in) / determinant(in);
        }

        template<typename T>
        void decomposeSrt(const Matrix4x4<T>& in, Vector3<T>& outScale, Matrix4x4<T>& outRotationMatrix, Vector3<T>& outTranslation) noexcept
        {
            // TODO: avoid nan in outRotationMatrix


            // Srt Matrix
            // 
            // | s_x * r_11  s_y * r_12  s_z * r_13  t_x |
            // | s_x * r_21  s_y * r_22  s_z * r_23  t_y |
            // | s_x * r_31  s_y * r_32  s_z * r_33  t_z |
            // | 0           0           0           1   |

            const T _11 = in.getElement(0, 0);
            const T _12 = in.getElement(0, 1);
            const T _13 = in.getElement(0, 2);
            const T _21 = in.getElement(1, 0);
            const T _22 = in.getElement(1, 1);
            const T _23 = in.getElement(1, 2);
            const T _31 = in.getElement(2, 0);
            const T _32 = in.getElement(2, 1);
            const T _33 = in.getElement(2, 2);
                
            // s
            outScale.x() = ::sqrt((_11 * _11) + (_21 * _21) + (_31 * _31));
            outScale.y() = ::sqrt((_12 * _12) + (_22 * _22) + (_32 * _32));
            outScale.z() = ::sqrt((_13 * _13) + (_23 * _23) + (_33 * _33));

            // r
            outRotationMatrix.setElement(0, 0, _11 / outScale.x());
            outRotationMatrix.setElement(1, 0, _21 / outScale.x());
            outRotationMatrix.setElement(2, 0, _31 / outScale.x());

            outRotationMatrix.setElement(0, 1, _12 / outScale.y());
            outRotationMatrix.setElement(1, 1, _22 / outScale.y());
            outRotationMatrix.setElement(2, 1, _32 / outScale.y());

            outRotationMatrix.setElement(0, 2, _13 / outScale.z());
            outRotationMatrix.setElement(1, 2, _23 / outScale.z());
            outRotationMatrix.setElement(2, 2, _33 / outScale.z());

            const T kZero = getScalarZero<T>();
            const T kOne = getScalarOne<T>();
            outRotationMatrix.setElement(0, 3, kZero);
            outRotationMatrix.setElement(1, 3, kZero);
            outRotationMatrix.setElement(2, 3, kZero);
            outRotationMatrix.setElement(3, 0, kZero);
            outRotationMatrix.setElement(3, 1, kZero);
            outRotationMatrix.setElement(3, 2, kZero);
            outRotationMatrix.setElement(3, 3, kOne);

            const T _14 = in.getElement(0, 3);
            const T _24 = in.getElement(1, 3);
            const T _34 = in.getElement(2, 3);

            // t
            outTranslation.x() = _14;
            outTranslation.y() = _24;
            outTranslation.z() = _34;
        }

        template<typename T>
        Matrix4x4<T> axesToColumns(const Vector3<T>& axisX, const Vector3<T>& axisY, const Vector3<T>& axisZ) noexcept
        {
            return Matrix4x4<T>
            (
                {
                    axisX.x(), axisY.x(), axisZ.x(), 0,
                    axisX.y(), axisY.y(), axisZ.y(), 0,
                    axisX.z(), axisY.z(), axisZ.z(), 0,
                            0,         0,         0, 1
                }
            );
        }

        template<typename T>
        Matrix4x4<T> axesToRows(const Vector3<T>& axisX, const Vector3<T>& axisY, const Vector3<T>& axisZ) noexcept
        {
            return Matrix4x4<T>
            (
                {
                    axisX.x(), axisX.y(), axisX.z(), 0,
                    axisY.x(), axisY.y(), axisY.z(), 0,
                    axisZ.x(), axisZ.y(), axisZ.z(), 0,
                            0,         0,         0, 1
                }
            );
        }

        template<typename T>
        Matrix4x4<T> translationMatrix(const Vector3<T>& translation) noexcept
        {
            const T x = translation.x();
            const T y = translation.y();
            const T z = translation.z();
            return Matrix4x4<T>
            (
                {
                    1, 0, 0, x,
                    0, 1, 0, y,
                    0, 0, 1, z,
                    0, 0, 0, 1
                }
            );
        }

        template<typename T>
        Matrix4x4<T>& setTranslation(Matrix4x4<T>& in, const Vector3<T>& translation) noexcept
        {
            in.setElement(0, 3, translation.getComponent(0));
            in.setElement(1, 3, translation.getComponent(1));
            in.setElement(2, 3, translation.getComponent(2));
            return in;
        }

        template<typename T>
        Matrix4x4<T>& preTranslate(Matrix4x4<T>& in, const Vector3<T>& translation) noexcept
        {
            in.addElement(0, 3, translation.getComponent(0));
            in.addElement(1, 3, translation.getComponent(1));
            in.addElement(2, 3, translation.getComponent(2));
            return in;
        }

        template<typename T>
        Matrix4x4<T>& postTranslate(Matrix4x4<T>& in, const Vector3<T>& translation) noexcept
        {
            in.addElement(0, 3, in.getRow(0).shrink().dot(translation));
            in.addElement(1, 3, in.getRow(1).shrink().dot(translation));
            in.addElement(2, 3, in.getRow(2).shrink().dot(translation));
            in.addElement(3, 3, in.getRow(3).shrink().dot(translation));
            return in;
        }

        template<typename T>
        Vector3<T>& getTranslation(const Matrix4x4<T>& in) noexcept
        {
            return in.getColumn(0).shrink();
        }

        template<typename T>
        Matrix4x4<T> scalarMatrix(const Vector3<T>& scale) noexcept
        {
            const T x = scale.x();
            const T y = scale.y();
            const T z = scale.z();
            return Matrix4x4<T>
            (
                {
                    x, 0, 0, 0,
                    0, y, 0, 0,
                    0, 0, z, 0,
                    0, 0, 0, 1
                }
            );
        }

        template<typename T>
        Matrix4x4<T>& preScale(Matrix4x4<T>& in, const Vector3<T>& scale) noexcept
        {
            in.setRow(0, in.getRow(0) * scale.getComponent(0));
            in.setRow(1, in.getRow(1) * scale.getComponent(1));
            in.setRow(2, in.getRow(2) * scale.getComponent(2));
            return in;
        }

        template<typename T>
        Matrix4x4<T>& postScale(Matrix4x4<T>& in, const Vector3<T>& scale) noexcept
        {
            in.setColumn(0, in.getColumn(0) * scale.getComponent(0));
            in.setColumn(1, in.getColumn(1) * scale.getComponent(1));
            in.setColumn(2, in.getColumn(2) * scale.getComponent(2));
            return in;
        }

        template<typename T>
        Matrix4x4<T> rotationMatrixX(const T angle) noexcept
        {
            return Matrix4x4<T>
            (
                {
                    1             , 0             , 0             , 0,
                    0             , +::cos(angle) , -::sin(angle) , 0,
                    0             , +::sin(angle) , +::cos(angle) , 0,
                    0             , 0             , 0             , 1
                }
            );
        }

        template<typename T>
        Matrix4x4<T> rotationMatrixY(const T angle) noexcept
        {
            return Matrix4x4<T>
            (
                {
                    +::cos(angle) , 0             , +::sin(angle) , 0,
                    0             , 1             , 0             , 0,
                    -::sin(angle) , 0             , +::cos(angle) , 0,
                    0             , 0             , 0             , 1
                }
            );
        }

        template<typename T>
        Matrix4x4<T> rotationMatrixZ(const T angle) noexcept
        {
            return Matrix4x4<T>
            (
                {
                    +::cos(angle) , -::sin(angle) , 0             , 0,
                    +::sin(angle) , +::cos(angle) , 0             , 0,
                    0             , 0             , 1             , 0,
                    0             , 0             , 0             , 1
                }
            );
        }

        template<typename T>
        Matrix4x4<T> rotationMatrixRollPitchYaw(const T pitch, const T yaw, const T roll) noexcept
        {
            return rotationMatrixY(yaw) * rotationMatrixX(pitch) * rotationMatrixZ(roll);
        }

        template<typename T>
        Matrix4x4<T> rotationMatrixAxisAngle(const Vector3<T>& axis, const T angle) noexcept
        {
            // (v * r)r(1 - cosθ) + vcosθ + (r X v)sinθ

            const Vector3<T> r = axis.normalize();
            const T c = ::cos(angle);
            const T s = ::sin(angle);

            const T rx = r.x();
            const T ry = r.y();
            const T rz = r.z();
            Matrix4x4<T> result
            (
                {
                    (1 - c) * rx * rx  + c            , (1 - c) * ry * rx       - (rz * s), (1 - c) * rz * rx       + (ry * s), 0,
                    (1 - c) * rx * ry       + (rz * s), (1 - c) * ry * ry  + c            , (1 - c) * rz * ry       - (rx * s), 0,
                    (1 - c) * rx * rz       - (ry * s), (1 - c) * ry * rz       + (rx * s), (1 - c) * rz * rz  + c            , 0,
                    0                                 , 0                                 , 0                                 , 1
                }
            );
            return result;
        }

        template<typename T>
        Matrix4x4<T> rotationMatrixFromAxes(const Vector3<T>& axisX, const Vector3<T>& axisY, const Vector3<T>& axisZ) noexcept
        {
            return axesToColumns(axisX, axisY, axisZ);
        }

        template<typename T>
        Matrix4x4<T> rotationMatrix(const Quaternion<T>& rotation) noexcept
        {
            Vector3<T> axis;
            T angle;
            rotation.getAxisAngle(axis, angle);
            return rotationMatrixAxisAngle(axis, angle);
        }

        template<typename T>
        Matrix4x4<T> srtMatrix(const Vector3<T>& scale, const Quaternion<T>& rotation, const Vector3<T>& translation) noexcept
        {
            // SRT matrix for column vector is like below:
            // SRT = T * R * S
            // which is the same as below..
            Matrix4x4<T> matrix = rotationMatrix(rotation);
            matrix.preTranslate(translation.x(), translation.y(), translation.z());
            matrix.postScale(scale.x(), scale.y(), scale.z());
            return matrix;
        }
            
        template<typename T>
        Matrix4x4<T> projectionMatrixPerspective(const T fov, const T nearZ, const T farZ, const T ratio) noexcept
        {
            const T halfFov = static_cast<T>(fov * 0.5);
            const T a = static_cast<T>(1.0 / (::tan(halfFov) * ratio));
            const T b = static_cast<T>(1.0 / (::tan(halfFov)));
            const T c = (farZ) / (farZ - nearZ);
            const T d = -(farZ * nearZ) / (farZ - nearZ);
            const T e = static_cast<T>(1.0);
            return Matrix4x4<T>
            (
                {
                    a, 0, 0, 0,
                    0, b, 0, 0,
                    0, 0, c, d,
                    0, 0, e, 0
                }
            );
        }

        template<typename T>
        Matrix4x4<T> projectionMatrix2DFromTopLeft(const T pixelWidth, const T pixelHeight) noexcept
        {
            return Matrix4x4<T>
            (
                {
                    +static_cast<T>(2) / pixelWidth,  0                              , 0, -1,
                     0                             , -static_cast<T>(2) / pixelHeight, 0, +1,
                     0                             ,  0                              , 1,  0,
                     0                             ,  0                              , 0,  1
                }
            );
        }
    }
}
