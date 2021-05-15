#pragma once


#ifndef FS_MATRIX_H
#define FS_MATRIX_H


#include <FsCommon/Include/CommonDefinitions.h>

#include <FsMath/Include/VectorR.h>


namespace fs
{
    namespace Math
    {
        template <int32 M, int32 N>
        class Matrix
        {
            static_assert(0 < M, "M must be greater than 0!");
            static_assert(0 < N, "N must be greater than 0!");

        public:
                                    Matrix();
                                    ~Matrix();

        public:
            Matrix&                 operator*=(const double scalar) noexcept;
            Matrix&                 operator/=(const double scalar) noexcept;
            Matrix&                 operator+=(const Matrix& rhs) noexcept;
            Matrix&                 operator-=(const Matrix& rhs) noexcept;

        public:
            Matrix                  operator*(const double scalar) noexcept;
            Matrix                  operator/(const double scalar) noexcept;
            Matrix                  operator+(const Matrix& rhs) noexcept;
            Matrix                  operator-(const Matrix& rhs) noexcept;

        public:
            template <int32 P>
            Matrix<M, P>            operator*(const Matrix<N, P>& rhs) const noexcept;

        public:
            void                    setElement(const uint32 rowIndex, const uint32 columnIndex, const double value) noexcept;
            const double            getElement(const uint32 rowIndex, const uint32 columnIndex) const noexcept;
            void                    setRow(const uint32 rowIndex, const VectorR<N>& row) noexcept;
            VectorR<N>              getRow(const uint32 rowIndex) const noexcept;
            void                    setColumn(const uint32 columnIndex, const VectorR<M>& column) noexcept;
            VectorR<M>              getColumn(const uint32 columnIndex) const noexcept;

        public:
            VectorR<N>              multiplyRowVector(const VectorR<M>& rowVector) const noexcept;
            VectorR<M>              multiplyColumnVector(const VectorR<N>& columnVector) const noexcept;

        public:
            constexpr const bool    isSquareMatrix() const noexcept;
            const bool              isDiagonalMatrix() const noexcept;
            const bool              isScalarMatrix() const noexcept;
            const bool              isIdentityMatrix() const noexcept;
            const bool              isZeroMatrix() const noexcept;

            void                    setIdentity() noexcept;
            void                    setZero() noexcept;

        private:
            double                  _m[M][N];
        };


        template <int32 M, int32 N>
        Matrix<M, N>                operator*(const double scalar, const Matrix<M, N>& matrix) noexcept;
        
        // Row vector multiplication
        template <int32 M, int32 N>
        VectorR<N>                  operator*(const VectorR<M>& rowVector, const Matrix<M, N>& matrix) noexcept;

        // Column vector multiplication
        template <int32 M, int32 N>
        VectorR<M>                  operator*(const Matrix<M, N>& matrix, const VectorR<N>& columnVector) noexcept;
    }
}


#endif // !FS_MATRIX_H
