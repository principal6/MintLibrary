#pragma once


#ifndef MINT_MATRIX_H
#define MINT_MATRIX_H


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
    namespace Math
    {
        template <int32 N, typename T>
        class VectorR;


        template <int32 M, int32 N, typename T>
        class Matrix
        {
            static_assert(0 < M, "M must be greater than 0!");
            static_assert(0 < N, "N must be greater than 0!");

        public:
                                    Matrix();
                                    ~Matrix();

        public:
            const bool              operator==(const Matrix& rhs) const noexcept;
            const bool              operator!=(const Matrix& rhs) const noexcept;

        public:
            Matrix&                 operator*=(const double scalar) noexcept;
            Matrix&                 operator/=(const double scalar) noexcept;
            Matrix&                 operator+=(const Matrix& rhs) noexcept;
            Matrix&                 operator-=(const Matrix& rhs) noexcept;
            Matrix&                 operator*=(const Matrix<N, N, T>& rhs) noexcept;

        public:
            Matrix                  operator*(const double scalar) noexcept;
            Matrix                  operator/(const double scalar) noexcept;
            Matrix                  operator+(const Matrix& rhs) noexcept;
            Matrix                  operator-(const Matrix& rhs) noexcept;

        public:
            template <int32 P>
            Matrix<M, P, T>         operator*(const Matrix<N, P, T>& rhs) const noexcept;

        public:
            void                    setElement(const uint32 rowIndex, const uint32 columnIndex, const double value) noexcept;
            const double            getElement(const uint32 rowIndex, const uint32 columnIndex) const noexcept;
            void                    setRow(const uint32 rowIndex, const VectorR<N, T>& row) noexcept;
            VectorR<N, T>           getRow(const uint32 rowIndex) const noexcept;
            void                    setColumn(const uint32 columnIndex, const VectorR<M, T>& column) noexcept;
            VectorR<M, T>           getColumn(const uint32 columnIndex) const noexcept;

        public:
            VectorR<N, T>           multiplyRowVector(const VectorR<M, T>& rowVector) const noexcept;
            VectorR<M, T>           multiplyColumnVector(const VectorR<N, T>& columnVector) const noexcept;

        public:
            Matrix<N, M, T>         transpose() const noexcept;
            const double            trace() const noexcept;

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
            double                  _m[M][N];
        };


        template <int32 M, int32 N, typename T>
        Matrix<M, N, T>             operator*(const double scalar, const Matrix<M, N, T>& matrix) noexcept;
        
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
    }
}


#endif // !MINT_MATRIX_H
