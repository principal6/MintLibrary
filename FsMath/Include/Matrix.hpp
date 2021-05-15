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
