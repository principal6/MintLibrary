#pragma once


#include <MintMath/Include/Matrix.h>

#include <MintMath/Include/VectorR.hpp>


namespace mint
{
    namespace Math
    {
        template<int32 M, int32 N, typename T>
        inline Matrix<M, N, T>::Matrix()
            : _m{}
        {
            __noop;
        }

        template<int32 M, int32 N, typename T>
        inline Matrix<M, N, T>::~Matrix()
        {
            __noop;
        }

        template<int32 M, int32 N, typename T>
        inline const bool Matrix<M, N, T>::operator==(const Matrix& rhs) const noexcept
        {
            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    if (_m[rowIndex][columnIndex] != rhs._m[rowIndex][columnIndex])
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        template<int32 M, int32 N, typename T>
        inline const bool Matrix<M, N, T>::operator!=(const Matrix& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE Matrix<M, N, T>& Matrix<M, N, T>::operator*=(const double scalar) noexcept
        {
            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] *= scalar;
                }
            }
            return *this;
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE Matrix<M, N, T>& Matrix<M, N, T>::operator/=(const double scalar) noexcept
        {
            MINT_ASSERT("김장원", scalar != 0.0, "0 으로 나누려 합니다!");

            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] /= scalar;
                }
            }
            return *this;
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE Matrix<M, N, T>& Matrix<M, N, T>::operator+=(const Matrix& rhs) noexcept
        {
            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] += rhs._m[rowIndex][columnIndex];
                }
            }
            return *this;
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE Matrix<M, N, T>& Matrix<M, N, T>::operator-=(const Matrix& rhs) noexcept
        {
            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] -= rhs._m[rowIndex][columnIndex];
                }
            }
            return *this;
        }

        template<int32 M, int32 N, typename T>
        inline Matrix<M, N, T>& Matrix<M, N, T>::operator*=(const Matrix<N, N, T>& rhs) noexcept
        {
            static_assert(M == N, "Power of non-square matrix!!!");

            for (int32 rowIndex = 0; rowIndex < N; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] = VectorR<N, T>::dot(getRow(rowIndex), rhs.getColumn(columnIndex));
                }
            }
            return *this;
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE Matrix<M, N, T> Matrix<M, N, T>::operator*(const double scalar) noexcept
        {
            Matrix result = *this;
            result *= scalar;
            return result;
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE Matrix<M, N, T> Matrix<M, N, T>::operator/(const double scalar) noexcept
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
            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < P; ++columnIndex)
                {
                    result._m[rowIndex][columnIndex] = VectorR<N, T>::dot(getRow(rowIndex), rhs.getColumn(columnIndex));
                }
            }
            return result;
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE void Matrix<M, N, T>::setElement(const uint32 rowIndex, const uint32 columnIndex, const double value) noexcept
        {
            if (rowIndex < static_cast<uint32>(M) && columnIndex < static_cast<uint32>(N))
            {
                _m[rowIndex][columnIndex] = value;
            }
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE const double Matrix<M, N, T>::getElement(const uint32 rowIndex, const uint32 columnIndex) const noexcept
        {
            MINT_ASSERT("김장원", (rowIndex < static_cast<uint32>(M) && columnIndex < static_cast<uint32>(N)), "범위를 벗어난 접근입니다!");
            return _m[rowIndex][columnIndex];
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE void Matrix<M, N, T>::setRow(const uint32 rowIndex, const VectorR<N, T>& row) noexcept
        {
            if (rowIndex < static_cast<uint32>(M))
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] = row[columnIndex];
                }
            }
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE VectorR<N, T> Matrix<M, N, T>::getRow(const uint32 rowIndex) const noexcept
        {
            MINT_ASSERT("김장원", rowIndex < static_cast<uint32>(M), "범위를 벗어난 접근입니다!");

            VectorR<N, T> result;
            for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
            {
                result[columnIndex] = _m[rowIndex][columnIndex];
            }
            return result;
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE void Matrix<M, N, T>::setColumn(const uint32 columnIndex, const VectorR<M, T>& column) noexcept
        {
            if (columnIndex < static_cast<uint32>(N))
            {
                for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
                {
                    _m[rowIndex][columnIndex] = column[rowIndex];
                }
            }
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE VectorR<M, T> Matrix<M, N, T>::getColumn(const uint32 columnIndex) const noexcept
        {
            MINT_ASSERT("김장원", columnIndex < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");

            VectorR<M, T> result;
            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                result[rowIndex] = _m[rowIndex][columnIndex];
            }
            return result;
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE VectorR<N, T> Matrix<M, N, T>::multiplyRowVector(const VectorR<M, T>& rowVector) const noexcept
        {
            VectorR<N, T> result;
            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                result += getRow(rowIndex) * rowVector[rowIndex];
            }
            return result;
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE VectorR<M, T> Matrix<M, N, T>::multiplyColumnVector(const VectorR<N, T>& columnVector) const noexcept
        {
            VectorR<M, T> result;
            for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
            {
                result += getColumn(columnIndex) * columnVector[columnIndex];
            }
            return result;
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE Matrix<N, M, T> Matrix<M, N, T>::transpose() const noexcept
        {
            Matrix<N, M, T> result;
            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    result.setElement(columnIndex, rowIndex, _m[rowIndex][columnIndex]);
                }
            }
            return result;
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE const double Matrix<M, N, T>::trace() const noexcept
        {
            if (isSquareMatrix() == false)
            {
                MINT_LOG_ERROR("김장원", "Tried to calculate trace from a non-square matrix!");
            }

            double trace = 0.0;
            const int32 safeSize = min(M, N);
            for (int32 index = 0; index < safeSize; ++index)
            {
                trace += _m[index][index];
            }
            return trace;
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
                        if (_m[rowIndex][columnIndex] != 0.0)
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
            
            const double scale = _m[0][0];
            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    if (_m[rowIndex][columnIndex] != ((columnIndex == rowIndex) ? scale : 0.0))
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
                    if (_m[rowIndex][columnIndex] != ((columnIndex == rowIndex) ? 1.0 : 0.0))
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
                    if (_m[rowIndex][columnIndex] != 0.0)
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
                    if (_m[rowIndex][columnIndex] != 0.0)
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

            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] = (columnIndex == rowIndex) ? 1.0 : 0.0;
                }
            }
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE void Matrix<M, N, T>::setZero() noexcept
        {
            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] = 0.0;
                }
            }
        }


        template<int32 M, int32 N, typename T>
        MINT_INLINE Matrix<M, N, T> operator*(const double scalar, const Matrix<M, N, T>& matrix) noexcept
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
    }
}
