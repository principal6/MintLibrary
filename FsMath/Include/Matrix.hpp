#pragma once


#include <FsMath/Include/Matrix.h>

#include <FsMath/Include/VectorR.hpp>


namespace fs
{
    namespace Math
    {
        template<uint8 M, uint8 N>
        inline Matrix<M, N>::Matrix()
            : _m{}
        {
            __noop;
        }

        template<uint8 M, uint8 N>
        inline Matrix<M, N>::~Matrix()
        {
            __noop;
        }

        template<uint8 M, uint8 N>
        FS_INLINE Matrix<M, N>& Matrix<M, N>::operator*=(const double scalar) noexcept
        {
            for (uint8 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (uint8 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] *= scalar;
                }
            }
            return *this;
        }

        template<uint8 M, uint8 N>
        FS_INLINE Matrix<M, N>& Matrix<M, N>::operator/=(const double scalar) noexcept
        {
            FS_ASSERT("김장원", scalar != 0.0, "0 으로 나누려 합니다!");

            for (uint8 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (uint8 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] /= scalar;
                }
            }
            return *this;
        }

        template<uint8 M, uint8 N>
        FS_INLINE Matrix<M, N>& Matrix<M, N>::operator+=(const Matrix& rhs) noexcept
        {
            for (uint8 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (uint8 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] += rhs._m[rowIndex][columnIndex];
                }
            }
            return *this;
        }

        template<uint8 M, uint8 N>
        FS_INLINE Matrix<M, N>& Matrix<M, N>::operator-=(const Matrix& rhs) noexcept
        {
            for (uint8 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (uint8 columnIndex = 0; columnIndex < N; ++columnIndex)
                {
                    _m[rowIndex][columnIndex] -= rhs._m[rowIndex][columnIndex];
                }
            }
            return *this;
        }

        template<uint8 M, uint8 N>
        FS_INLINE Matrix<M, N> Matrix<M, N>::operator*(const double scalar) noexcept
        {
            Matrix result = *this;
            result *= scalar;
            return result;
        }

        template<uint8 M, uint8 N>
        FS_INLINE Matrix<M, N> Matrix<M, N>::operator/(const double scalar) noexcept
        {
            Matrix result = *this;
            result /= scalar;
            return result;
        }

        template<uint8 M, uint8 N>
        FS_INLINE Matrix<M, N> Matrix<M, N>::operator+(const Matrix& rhs) noexcept
        {
            Matrix result = *this;
            result += rhs;
            return result;
        }

        template<uint8 M, uint8 N>
        FS_INLINE Matrix<M, N> Matrix<M, N>::operator-(const Matrix& rhs) noexcept
        {
            Matrix result = *this;
            result -= rhs;
            return result;
        }

        template<uint8 M, uint8 N>
        template<uint8 P>
        FS_INLINE Matrix<M, P> Matrix<M, N>::operator*(const Matrix<N, P>& rhs) const noexcept
        {
            Matrix<M, P> result;
            for (uint8 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                for (uint8 columnIndex = 0; columnIndex < P; ++columnIndex)
                {
                    result._m[rowIndex][columnIndex] = fs::Math::VectorR<N>::dot(getRow(rowIndex), rhs.getColumn(columnIndex));
                }
            }
            return result;
        }

        template<uint8 M, uint8 N>
        FS_INLINE void Matrix<M, N>::setElement(const uint8 rowIndex, const uint8 columnIndex, const double value) noexcept
        {
            if (rowIndex < M && columnIndex < N)
            {
                _m[rowIndex][columnIndex] = value;
            }
        }

        template<uint8 M, uint8 N>
        FS_INLINE const double Matrix<M, N>::getElement(const uint8 rowIndex, const uint8 columnIndex) const noexcept
        {
            FS_ASSERT("김장원", (rowIndex < M && columnIndex < N), "범위를 벗어난 접근입니다!");
            return _m[rowIndex][columnIndex];
        }

        template<uint8 M, uint8 N>
        FS_INLINE VectorR<N> Matrix<M, N>::getRow(const uint8 rowIndex) const noexcept
        {
            FS_ASSERT("김장원", rowIndex < M, "범위를 벗어난 접근입니다!");

            VectorR<N> result;
            for (uint8 columnIndex = 0; columnIndex < N; ++columnIndex)
            {
                result[columnIndex] = _m[rowIndex][columnIndex];
            }
            return result;
        }

        template<uint8 M, uint8 N>
        FS_INLINE VectorR<M> Matrix<M, N>::getColumn(const uint8 columnIndex) const noexcept
        {
            FS_ASSERT("김장원", columnIndex < N, "범위를 벗어난 접근입니다!");

            VectorR<M> result;
            for (uint8 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                result[rowIndex] = _m[rowIndex][columnIndex];
            }
            return result;
        }

        template<uint8 M, uint8 N>
        FS_INLINE VectorR<N> Matrix<M, N>::multiplyRowVector(const VectorR<M>& rowVector) const noexcept
        {
            VectorR<N> result;
            for (uint8 rowIndex = 0; rowIndex < M; ++rowIndex)
            {
                result += getRow(rowIndex) * rowVector[rowIndex];
            }
            return result;
        }

        template<uint8 M, uint8 N>
        FS_INLINE VectorR<M> Matrix<M, N>::multiplyColumnVector(const VectorR<N>& columnVector) const noexcept
        {
            VectorR<M> result;
            for (uint8 columnIndex = 0; columnIndex < N; ++columnIndex)
            {
                result += getColumn(columnIndex) * columnVector[columnIndex];
            }
            return result;
        }
    }
}
