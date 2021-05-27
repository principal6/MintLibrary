#pragma once


#include <FsMath/Include/Matrix.h>

#include <FsMath/Include/VectorR.hpp>


namespace fs
{
    namespace Math
    {
        template<int32 M, int32 N>
        inline Matrix<M, N>::Matrix()
            : _m{}
        {
            __noop;
        }

        template<int32 M, int32 N>
        inline Matrix<M, N>::~Matrix()
        {
            __noop;
        }

        template<int32 M, int32 N>
        inline const bool Matrix<M, N>::operator==(const Matrix& rhs) const noexcept
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

        template<int32 M, int32 N>
        inline const bool Matrix<M, N>::operator!=(const Matrix& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        template<int32 M, int32 N>
        FS_INLINE Matrix<M, N>& Matrix<M, N>::operator*=(const double scalar) noexcept
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

        template<int32 M, int32 N>
        FS_INLINE Matrix<M, N>& Matrix<M, N>::operator/=(const double scalar) noexcept
        {
            FS_ASSERT("김장원", scalar != 0.0, "0 으로 나누려 합니다!");

            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] /= scalar;
                }
            }
            return *this;
        }

        template<int32 M, int32 N>
        FS_INLINE Matrix<M, N>& Matrix<M, N>::operator+=(const Matrix& rhs) noexcept
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

        template<int32 M, int32 N>
        FS_INLINE Matrix<M, N>& Matrix<M, N>::operator-=(const Matrix& rhs) noexcept
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

        template<int32 M, int32 N>
        inline Matrix<M, N>& Matrix<M, N>::operator*=(const Matrix<N, N>& rhs) noexcept
        {
            static_assert(M == N, "Power of non-square matrix!!!");

            for (int32 rowIndex = 0; rowIndex < N; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] = fs::Math::VectorR<N>::dot(getRow(rowIndex), rhs.getColumn(columnIndex));
                }
            }
            return *this;
        }

        template<int32 M, int32 N>
        FS_INLINE Matrix<M, N> Matrix<M, N>::operator*(const double scalar) noexcept
        {
            Matrix result = *this;
            result *= scalar;
            return result;
        }

        template<int32 M, int32 N>
        FS_INLINE Matrix<M, N> Matrix<M, N>::operator/(const double scalar) noexcept
        {
            Matrix result = *this;
            result /= scalar;
            return result;
        }

        template<int32 M, int32 N>
        FS_INLINE Matrix<M, N> Matrix<M, N>::operator+(const Matrix& rhs) noexcept
        {
            Matrix result = *this;
            result += rhs;
            return result;
        }

        template<int32 M, int32 N>
        FS_INLINE Matrix<M, N> Matrix<M, N>::operator-(const Matrix& rhs) noexcept
        {
            Matrix result = *this;
            result -= rhs;
            return result;
        }

        template<int32 M, int32 N>
        template<int32 P>
        FS_INLINE Matrix<M, P> Matrix<M, N>::operator*(const Matrix<N, P>& rhs) const noexcept
        {
            Matrix<M, P> result;
            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < P; ++columnIndex)
                {
                    result._m[rowIndex][columnIndex] = fs::Math::VectorR<N>::dot(getRow(rowIndex), rhs.getColumn(columnIndex));
                }
            }
            return result;
        }

        template<int32 M, int32 N>
        FS_INLINE void Matrix<M, N>::setElement(const uint32 rowIndex, const uint32 columnIndex, const double value) noexcept
        {
            if (rowIndex < static_cast<uint32>(M) && columnIndex < static_cast<uint32>(N))
            {
                _m[rowIndex][columnIndex] = value;
            }
        }

        template<int32 M, int32 N>
        FS_INLINE const double Matrix<M, N>::getElement(const uint32 rowIndex, const uint32 columnIndex) const noexcept
        {
            FS_ASSERT("김장원", (rowIndex < static_cast<uint32>(M) && columnIndex < static_cast<uint32>(N)), "범위를 벗어난 접근입니다!");
            return _m[rowIndex][columnIndex];
        }

        template<int32 M, int32 N>
        FS_INLINE void Matrix<M, N>::setRow(const uint32 rowIndex, const VectorR<N>& row) noexcept
        {
            if (rowIndex < static_cast<uint32>(M))
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] = row[columnIndex];
                }
            }
        }

        template<int32 M, int32 N>
        FS_INLINE VectorR<N> Matrix<M, N>::getRow(const uint32 rowIndex) const noexcept
        {
            FS_ASSERT("김장원", rowIndex < static_cast<uint32>(M), "범위를 벗어난 접근입니다!");

            VectorR<N> result;
            for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
            {
                result[columnIndex] = _m[rowIndex][columnIndex];
            }
            return result;
        }

        template<int32 M, int32 N>
        FS_INLINE void Matrix<M, N>::setColumn(const uint32 columnIndex, const VectorR<M>& column) noexcept
        {
            if (columnIndex < static_cast<uint32>(N))
            {
                for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
                {
                    _m[rowIndex][columnIndex] = column[rowIndex];
                }
            }
        }

        template<int32 M, int32 N>
        FS_INLINE VectorR<M> Matrix<M, N>::getColumn(const uint32 columnIndex) const noexcept
        {
            FS_ASSERT("김장원", columnIndex < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");

            VectorR<M> result;
            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                result[rowIndex] = _m[rowIndex][columnIndex];
            }
            return result;
        }

        template<int32 M, int32 N>
        FS_INLINE VectorR<N> Matrix<M, N>::multiplyRowVector(const VectorR<M>& rowVector) const noexcept
        {
            VectorR<N> result;
            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                result += getRow(rowIndex) * rowVector[rowIndex];
            }
            return result;
        }

        template<int32 M, int32 N>
        FS_INLINE VectorR<M> Matrix<M, N>::multiplyColumnVector(const VectorR<N>& columnVector) const noexcept
        {
            VectorR<M> result;
            for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
            {
                result += getColumn(columnIndex) * columnVector[columnIndex];
            }
            return result;
        }

        template<int32 M, int32 N>
        FS_INLINE Matrix<N, M> Matrix<M, N>::transpose() const noexcept
        {
            Matrix<N, M> result;
            for (int32 rowIndex = 0; rowIndex < M; rowIndex++)
            {
                for (int32 columnIndex = 0; columnIndex < N; columnIndex++)
                {
                    result.setElement(columnIndex, rowIndex, _m[rowIndex][columnIndex]);
                }
            }
            return result;
        }

        template<int32 M, int32 N>
        FS_INLINE const double Matrix<M, N>::trace() const noexcept
        {
            if (isSquareMatrix() == false)
            {
                FS_LOG_ERROR("김장원", "Tried to calculate trace from a non-square matrix!");
            }

            double trace = 0.0;
            const int32 safeSize = fs::min(M, N);
            for (int32 index = 0; index < safeSize; index++)
            {
                trace += _m[index][index];
            }
            return trace;
        }

        template<int32 M, int32 N>
        FS_INLINE constexpr const bool Matrix<M, N>::isSquareMatrix() const noexcept
        {
            return (M == N);
        }

        template<int32 M, int32 N>
        FS_INLINE const bool Matrix<M, N>::isDiagonalMatrix() const noexcept
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

        template<int32 M, int32 N>
        FS_INLINE const bool Matrix<M, N>::isScalarMatrix() const noexcept
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

        template<int32 M, int32 N>
        FS_INLINE const bool Matrix<M, N>::isIdentityMatrix() const noexcept
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

        template<int32 M, int32 N>
        FS_INLINE const bool Matrix<M, N>::isZeroMatrix() const noexcept
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

        template<int32 M, int32 N>
        FS_INLINE const bool Matrix<M, N>::isSymmetricMatrix() const noexcept
        {
            if (false == isSquareMatrix())
            {
                return false;
            }

            for (int32 rowIndex = 0; rowIndex < M; rowIndex++)
            {
                for (int32 columnIndex = 0; columnIndex < N; columnIndex++)
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

        template<int32 M, int32 N>
        FS_INLINE const bool Matrix<M, N>::isSkewSymmetricMatrix() const noexcept
        {
            if (false == isSquareMatrix())
            {
                return false;
            }

            for (int32 rowIndex = 0; rowIndex < M; rowIndex++)
            {
                for (int32 columnIndex = 0; columnIndex < N; columnIndex++)
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

        template<int32 M, int32 N>
        FS_INLINE const bool Matrix<M, N>::isUpperTriangularMatrix() const noexcept
        {
            if (false == isSquareMatrix())
            {
                return false;
            }

            for (int32 rowIndex = 1; rowIndex < M; rowIndex++)
            {
                for (int32 columnIndex = 0; columnIndex < rowIndex; columnIndex++)
                {
                    if (_m[rowIndex][columnIndex] != 0.0)
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        template<int32 M, int32 N>
        FS_INLINE const bool Matrix<M, N>::isIdempotentMatrix() const noexcept
        {
            Matrix<M, N> squared = *this;
            squared *= *this;
            return (squared == *this);
        }

        template<int32 M, int32 N>
        FS_INLINE void Matrix<M, N>::setIdentity() noexcept
        {
            if (isSquareMatrix() == false)
            {
                FS_LOG_ERROR("김장원", "Non-square matrix is set to identity!");
            }

            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] = (columnIndex == rowIndex) ? 1.0 : 0.0;
                }
            }
        }

        template<int32 M, int32 N>
        FS_INLINE void Matrix<M, N>::setZero() noexcept
        {
            for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] = 0.0;
                }
            }
        }


        template<int32 M, int32 N>
        FS_INLINE Matrix<M, N> operator*(const double scalar, const Matrix<M, N>& matrix) noexcept
        {
            return (matrix * scalar);
        }
        
        template<int32 M, int32 N>
        FS_INLINE VectorR<N> operator*(const VectorR<M>& rowVector, const Matrix<M, N>& matrix) noexcept
        {
            return matrix.multiplyRowVector(rowVector);
        }
        
        template<int32 M, int32 N>
        FS_INLINE VectorR<M> operator*(const Matrix<M, N>& matrix, const VectorR<N>& columnVector) noexcept
        {
            return matrix.multiplyColumnVector(columnVector);
        }
    }
}
