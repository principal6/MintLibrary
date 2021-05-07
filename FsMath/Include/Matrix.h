#pragma once


#ifndef FS_MATRIX_H
#define FS_MATRIX_H


#include <FsCommon/Include/CommonDefinitions.h>

#include <FsMath/Include/VectorR.h>


namespace fs
{
    namespace Math
    {
        template <uint8 M, uint8 N>
        class Matrix
        {
        public:
                                Matrix();
                                ~Matrix();

        public:
            Matrix&             operator*=(const double scalar) noexcept;
            Matrix&             operator/=(const double scalar) noexcept;
            Matrix&             operator+=(const Matrix& rhs) noexcept;
            Matrix&             operator-=(const Matrix& rhs) noexcept;

        public:
            Matrix              operator*(const double scalar) noexcept;
            Matrix              operator/(const double scalar) noexcept;
            Matrix              operator+(const Matrix& rhs) noexcept;
            Matrix              operator-(const Matrix& rhs) noexcept;

        public:
            template <uint8 P>
            Matrix<M, P>        operator*(const Matrix<N, P>& rhs) const noexcept;

        public:
            void                setElement(const uint8 rowIndex, const uint8 columnIndex, const double value) noexcept;
            const double        getElement(const uint8 rowIndex, const uint8 columnIndex) const noexcept;
            VectorR<N>          getRow(const uint8 rowIndex) const noexcept;
            VectorR<M>          getColumn(const uint8 columnIndex) const noexcept;

        public:
            VectorR<N>          multiplyRowVector(const VectorR<M>& rowVector) const noexcept;
            VectorR<M>          multiplyColumnVector(const VectorR<N>& columnVector) const noexcept;

        private:
            double              _m[M][N];
        };
    }
}


#endif // !FS_MATRIX_H
