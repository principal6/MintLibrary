#pragma once


#ifndef MINT_MATRIX_H
#define MINT_MATRIX_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintMath/Include/VectorR.h>


namespace std
{
    template <class _Elem>
    class initializer_list;
}


namespace mint
{
    namespace Math
    {
        template <int32 M, int32 N, typename T>
        class Matrix
        {
            static_assert(0 < M, "M must be greater than 0!");
            static_assert(0 < N, "N must be greater than 0!");

        public:
                                    Matrix();
                                    Matrix(const std::initializer_list<T>& initializerList);
                                    ~Matrix();

        public:
            const bool              operator==(const Matrix& rhs) const noexcept;
            const bool              operator!=(const Matrix& rhs) const noexcept;

        public:
            Matrix&                 operator*=(const T scalar) noexcept;
            Matrix&                 operator/=(const T scalar) noexcept;
            Matrix&                 operator+=(const Matrix& rhs) noexcept;
            Matrix&                 operator-=(const Matrix& rhs) noexcept;
            Matrix&                 operator*=(const Matrix<N, N, T>& rhs) noexcept;

        public:
            Matrix                  operator*(const T scalar) noexcept;
            Matrix                  operator/(const T scalar) noexcept;
            Matrix                  operator+(const Matrix& rhs) noexcept;
            Matrix                  operator-(const Matrix& rhs) noexcept;

        public:
            template <int32 P>
            Matrix<M, P, T>         operator*(const Matrix<N, P, T>& rhs) const noexcept;

        public:
            void                    setElement(const uint32 rowIndex, const uint32 columnIndex, const T value) noexcept;
            const T                 getElement(const uint32 rowIndex, const uint32 columnIndex) const noexcept;
            void                    setRow(const uint32 rowIndex, const VectorR<N, T>& row) noexcept;
            VectorR<N, T>           getRow(const uint32 rowIndex) const noexcept;
            void                    setColumn(const uint32 columnIndex, const VectorR<M, T>& column) noexcept;
            VectorR<M, T>           getColumn(const uint32 columnIndex) const noexcept;

        public:
            VectorR<N, T>           multiplyRowVector(const VectorR<M, T>& rowVector) const noexcept;
            VectorR<M, T>           multiplyColumnVector(const VectorR<N, T>& columnVector) const noexcept;

        public:
            Matrix<N, M, T>         transpose() const noexcept;
            const T                 trace() const noexcept;
            Matrix<M - 1, N - 1, T> minor(const uint32 row, const uint32 col) const noexcept;

        public:
            void                    factorizeLu(Matrix<N, N, T>& l, Matrix<N, N, T>& u) const noexcept;

        public:
            constexpr const bool    isSquareMatrix() const noexcept;
            const bool              isDiagonalMatrix() const noexcept;
            const bool              isScalarMatrix() const noexcept;
            const bool              isIdentityMatrix() const noexcept;
            const bool              isZeroMatrix() const noexcept;
            const bool              isSymmetricMatrix() const noexcept;
            const bool              isSkewSymmetricMatrix() const noexcept;
            const bool              isUpperTriangularMatrix() const noexcept;
            const bool              isIdempotentMatrix() const noexcept;

            void                    setIdentity() noexcept;
            void                    setZero() noexcept;

        private:
            T                       _m[M][N];
        };


        template <int32 M, int32 N, typename T>
        Matrix<M, N, T>             operator*(const T scalar, const Matrix<M, N, T>& matrix) noexcept;
        
        // Row vector multiplication
        template <int32 M, int32 N, typename T>
        VectorR<N, T>               operator*(const VectorR<M, T>& rowVector, const Matrix<M, N, T>& matrix) noexcept;

        // Column vector multiplication
        template <int32 M, int32 N, typename T>
        VectorR<M, T>               operator*(const Matrix<M, N, T>& matrix, const VectorR<N, T>& columnVector) noexcept;


        template <int32 M, int32 N>
        using MatrixF = Matrix<M, N, float>;

        template <int32 M, int32 N>
        using MatrixD = Matrix<M, N, double>;


#pragma region 2x2, 3x3, 4x4
        template <typename T>
        using Matrix2x2 = Matrix<2, 2, T>;

        template <typename T>
        using Matrix3x3 = Matrix<3, 3, T>;

        template <typename T>
        using Matrix4x4 = Matrix<4, 4, T>;


        namespace MatrixUtils
        {
            template<typename T>
            static constexpr T      getScalarZero() noexcept;

            template<>
            static constexpr float  getScalarZero() noexcept;
            
            template<>
            static constexpr double getScalarZero() noexcept;

            template<typename T>
            static constexpr T      getScalarOne() noexcept;

            template<>
            static constexpr float  getScalarOne() noexcept;
            
            template<>
            static constexpr double getScalarOne() noexcept;

            template<typename T>
            const T                 determinant(const Matrix2x2<T>& in) noexcept;

            template<typename T>
            const T                 determinant(const Matrix3x3<T>& in) noexcept;

            template<typename T>
            const T                 determinant(const Matrix4x4<T>& in) noexcept;

            template<typename T>
            Matrix4x4<T>            cofactor(const Matrix4x4<T>& in) noexcept;

            template<typename T>
            Matrix4x4<T>            adjugate(const Matrix4x4<T>& in) noexcept;

            template<typename T>
            Matrix4x4<T>            inverse(const Matrix4x4<T>& in) noexcept;

            template<typename T>
            void                    decomposeSrt(const Matrix4x4<T>& in, Vector3<T>& outScale, Matrix4x4<T>& outRotationMatrix, Vector3<T>& outTranslation) noexcept;
        }
#pragma endregion
    }
}


#endif // !MINT_MATRIX_H
