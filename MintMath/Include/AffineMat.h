#pragma once


#ifndef MINT_MATH_MATRIX4X4_H
#define MINT_MATH_MATRIX4X4_H


#include <MintMath/Include/AffineVec.h>


namespace mint
{
    template<typename T>
    class AffineMat
    {
        AffineMat() = default;
    };

    
    template<typename T>
    AffineMat<T> translationMatrix(const AffineVec<T>& translation) noexcept;

    template<typename T>
    AffineMat<T> scalarMatrix(const T scalar) noexcept;

    template<typename T>
    AffineMat<T> scalarMatrix(const AffineVec<T>& scalar) noexcept; // actually diagonal ...


    template<>
    class AffineMat<float>
    {
    public:
                            AffineMat();
                            AffineMat(const AffineVecF& row0, const AffineVecF& row1, const AffineVecF& row2, const AffineVecF& row3);
                            AffineMat(
                                  float _11, float _12, float _13, float _14
                                , float _21, float _22, float _23, float _24
                                , float _31, float _32, float _33, float _34
                                , float _41, float _42, float _43, float _44);
                            AffineMat(const float(&mat)[4][4]);
                            AffineMat(const AffineMat& rhs) = default;
                            AffineMat(AffineMat&& rhs) noexcept = default;
                            ~AffineMat() = default;

    public:
        AffineMat&          operator=(const AffineMat& rhs) = default;
        AffineMat&          operator=(AffineMat&& rhs) noexcept = default;
    
    public:
        const bool          operator==(const AffineMat& rhs) const noexcept;
        const bool          operator!=(const AffineMat& rhs) const noexcept;

    public:
        AffineMat&          operator+() noexcept;
        AffineMat           operator-() const noexcept;

        AffineMat           operator+(const AffineMat& rhs) const noexcept;
        AffineMat           operator-(const AffineMat& rhs) const noexcept;
        AffineMat           operator*(const float scalar) const noexcept;
        AffineMat           operator*(const AffineMat& rhs) const noexcept;
        AffineMat           operator/(const float scalar) const noexcept;

        AffineMat&          operator+=(const AffineMat& rhs) noexcept;
        AffineMat&          operator-=(const AffineMat& rhs) noexcept;
        AffineMat&          operator*=(const float scalar) noexcept;
        AffineMat&          operator*=(const AffineMat& rhs) noexcept;
        AffineMat&          operator/=(const float scalar) noexcept;

    public:
        void                set(
              float _11, float _12, float _13, float _14
            , float _21, float _22, float _23, float _24
            , float _31, float _32, float _33, float _34
            , float _41, float _42, float _43, float _44) noexcept;
        void                setZero() noexcept;
        void                setIdentity() noexcept;
        void                setRow(const int32 row, const AffineVecF& vec) noexcept;
        void                setCol(const int32 col, const AffineVecF& vec) noexcept;
        void                setElement(const int32 row, const int32 col, const float scalar) noexcept;

    public:
        void                get(float(&mat)[4][4]) const noexcept;
        const AffineVecF&   getRow(const int32 row) const noexcept;
        AffineVecF          getCol(const int32 col) const noexcept;

    public:
        AffineMat           inverse() const noexcept;

    private:
        AffineVecF          _rows[4];
    };


    // TODO: AffineMat<double>
}


#include <MintMath/Include/AffineMat.inl>


#endif // !MINT_MATH_MATRIX4X4_H
