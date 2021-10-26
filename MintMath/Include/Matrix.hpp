#pragma once


#include <MintMath/Include/Matrix.h>

#include <MintCommon/Include/Logger.h>

#include <MintMath/Include/VectorR.hpp>

#include <initializer_list>


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
        MINT_INLINE Matrix<M, N, T>& Matrix<M, N, T>::operator*=(const T scalar) noexcept
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
        MINT_INLINE Matrix<M, N, T>& Matrix<M, N, T>::operator/=(const T scalar) noexcept
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
        MINT_INLINE void Matrix<M, N, T>::setElement(const uint32 rowIndex, const uint32 columnIndex, const T value) noexcept
        {
            if (rowIndex < static_cast<uint32>(M) && columnIndex < static_cast<uint32>(N))
            {
                _m[rowIndex][columnIndex] = value;
            }
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
        MINT_INLINE const T Matrix<M, N, T>::trace() const noexcept
        {
            if (isSquareMatrix() == false)
            {
                MINT_LOG_ERROR("김장원", "Tried to calculate trace from a non-square matrix!");
            }

            T trace = 0.0;
            const int32 safeSize = min(M, N);
            for (int32 index = 0; index < safeSize; ++index)
            {
                trace += _m[index][index];
            }
            return trace;
        }

        template<int32 M, int32 N, typename T>
        MINT_INLINE Matrix<M - 1, N - 1, T> Matrix<M, N, T>::minor(const uint32 row, const uint32 col) const noexcept
        {
            Matrix<M - 1, N - 1, T> result;
            uint32 targetRow{ 0 };
            for (uint32 sourceRow = 0; sourceRow < M; ++sourceRow)
            {
                if (sourceRow == row) continue;

                uint32 targetCol{ 0 };
                for (uint32 sourceCol = 0; sourceCol < N; ++sourceCol)
                {
                    if (sourceCol == col) continue;

                    result.setElement(targetRow, targetCol, _m[sourceRow][sourceCol]);
                    ++targetCol;
                }
                ++targetRow;
            }
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
            
            const T scale = _m[0][0];
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
                return T();
            }
            
            template<typename T>
            constexpr float getScalarOne() noexcept
            {
                MINT_NEVER;
                return 1.0f;
            }
            
            template<typename T>
            constexpr double getScalarOne() noexcept
            {
                return 1.0;
            }

            template<typename T>
            const T determinant(const Matrix2x2<T>& in) noexcept
            {
                const T a = in.getElement(0, 0);
                const T b = in.getElement(0, 1);
                const T c = in.getElement(1, 0);
                const T d = in.getElement(1, 1);
                return static_cast<T>(a * d - b * c);
            }

            template<typename T>
            const T determinant(const Matrix3x3<T>& in) noexcept
            {
                const T a = in.getElement(0, 0);
                const T b = in.getElement(0, 1);
                const T c = in.getElement(0, 2);
                return a * determinant(in.minor(0, 0)) - b * determinant(in.minor(0, 1)) + c * determinant(in.minor(0, 2));
            }

            template<typename T>
            const T determinant(const Matrix4x4<T>& in) noexcept
            {
                const T a = in.getElement(0, 0);
                const T b = in.getElement(0, 1);
                const T c = in.getElement(0, 2);
                const T d = in.getElement(0, 3);
                return a * determinant(in.minor(0, 0)) - b * determinant(in.minor(0, 1)) + c * determinant(in.minor(0, 2)) - d * determinant(in.minor(0, 3));
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
                outScale._c[0] = ::sqrt((_11 * _11) + (_21 * _21) + (_31 * _31));
                outScale._c[1] = ::sqrt((_12 * _12) + (_22 * _22) + (_32 * _32));
                outScale._c[2] = ::sqrt((_13 * _13) + (_23 * _23) + (_33 * _33));

                // r
                outRotationMatrix.setElement(0, 0, _11 / outScale._c[0]);
                outRotationMatrix.setElement(1, 0, _21 / outScale._c[0]);
                outRotationMatrix.setElement(2, 0, _31 / outScale._c[0]);

                outRotationMatrix.setElement(0, 1, _12 / outScale._c[1]);
                outRotationMatrix.setElement(1, 1, _22 / outScale._c[1]);
                outRotationMatrix.setElement(2, 1, _32 / outScale._c[1]);

                outRotationMatrix.setElement(0, 2, _13 / outScale._c[2]);
                outRotationMatrix.setElement(1, 2, _23 / outScale._c[2]);
                outRotationMatrix.setElement(2, 2, _33 / outScale._c[2]);

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
                outTranslation._c[0] = _14;
                outTranslation._c[1] = _24;
                outTranslation._c[2] = _34;
            }
        }
    }
}
