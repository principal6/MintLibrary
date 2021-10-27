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
        template<typename T>
        const T getScalarZero() noexcept
        {
            MINT_NEVER;
            return T();
        }

        template<>
        const float getScalarZero() noexcept
        {
            return 0.0f;
        }

        template<>
        const double getScalarZero() noexcept
        {
            return 0.0;
        }

        template<typename T>
        const T getScalarOne() noexcept
        {
            MINT_NEVER;
            return T();
        }

        template<>
        const float getScalarOne() noexcept
        {
            return 1.0f;
        }

        template<>
        const double getScalarOne() noexcept
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
                    _m[rowIndex][columnIndex] = (columnIndex == rowIndex) ? getScalarOne<T>() : getScalarZero<T>();
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
                    _m[rowIndex][columnIndex] = 0;
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

            template<typename T>
            Matrix4x4<T> axesToColumns(const Vector3<T>& axisX, const Vector3<T>& axisY, const Vector3<T>& axisZ) noexcept
            {
                return Matrix4x4<T>
                (
                    {
                        axisX._c[0], axisY._c[0], axisZ._c[0], 0,
                        axisX._c[1], axisY._c[1], axisZ._c[1], 0,
                        axisX._c[2], axisY._c[2], axisZ._c[2], 0,
                                  0,           0,           0, 1
                    }
                );
            }

            template<typename T>
            Matrix4x4<T> axesToRows(const Vector3<T>& axisX, const Vector3<T>& axisY, const Vector3<T>& axisZ) noexcept
            {
                return Matrix4x4<T>
                (
                    {
                        axisX._c[0], axisX._c[1], axisX._c[2], 0,
                        axisY._c[0], axisY._c[1], axisY._c[2], 0,
                        axisZ._c[0], axisZ._c[1], axisZ._c[2], 0,
                                  0,           0,           0, 1
                    }
                );
            }

            template<typename T>
            Matrix4x4<T> translationMatrix(const Vector3<T>& translation) noexcept
            {
                const T x = translation._c[0];
                const T y = translation._c[1];
                const T z = translation._c[2];
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
                const T x = scale._c[0];
                const T y = scale._c[1];
                const T z = scale._c[2];
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

                const T rx = r._c[0];
                const T ry = r._c[1];
                const T rz = r._c[2];
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
                matrix.preTranslate(translation._x, translation._y, translation._z);
                matrix.postScale(scale._x, scale._y, scale._z);
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
}
