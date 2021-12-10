#pragma once


#ifndef MINT_MATH_MATRIX4X4_H
#define MINT_MATH_MATRIX4X4_H


#include <MintMath/Include/AffineVec.h>

#include <MintMath/Include/Matrix.h>
#include <MintMath/Include/Matrix.hpp>


namespace mint
{
    template<typename T>
    class AffineMat
    {
        AffineMat() = default;
    };


#pragma region Free functions
    template<typename T>
    AffineMat<T>    operator*(const T scalar, const AffineMat<T>& mat) noexcept;

    // Row vector * Matrix Multiplication
    template<typename T>
    AffineVec<T>    operator*(const AffineVec<T>& rowVec, const AffineMat<T>& mat) noexcept;

    // Matrix * Column vector Multiplication
    template <typename T>
    AffineVec<T>    operator*(const AffineMat<T>& mat, const AffineVec<T>& colVec) noexcept;

    template<typename T>
    AffineMat<T>    translationMatrix(const AffineVec<T>& translation) noexcept;

    template<typename T>
    AffineMat<T>    scalarMatrix(const T scalar) noexcept;

    template<typename T>
    AffineMat<T>    scalarMatrix(const AffineVec<T>& scalar) noexcept; // actually diagonal ...

    template<typename T>
    AffineMat<T>    rotationMatrixX(const T angle) noexcept;

    template<typename T>
    AffineMat<T>    rotationMatrixY(const T angle) noexcept;

    template<typename T>
    AffineMat<T>    rotationMatrixZ(const T angle) noexcept;

    template<typename T>
    AffineMat<T>    rotationMatrixRollPitchYaw(const T pitch, const T yaw, const T roll) noexcept;

    // Rodrigues' rotation formula
    template<typename T>
    AffineMat<T>    rotationMatrixAxisAngle(const AffineVec<T>& axis, const T angle) noexcept;

    template<typename T>
    AffineMat<T>    rotationMatrixFromAxes(const AffineVec<T>& axisX, const AffineVec<T>& axisY, const AffineVec<T>& axisZ) noexcept;

    template<typename T>
    AffineMat<T>    rotationMatrix(const Quaternion<T>& rotation) noexcept;

    template<typename T>
    AffineMat<T>    axesToColumns(const AffineVec<T>& axisX, const AffineVec<T>& axisY, const AffineVec<T>& axisZ) noexcept;

    // Interprets vector from new basis.
    template<typename T>
    AffineMat<T>    axesToRows(const AffineVec<T>& axisX, const AffineVec<T>& axisY, const AffineVec<T>& axisZ) noexcept;

    template<typename T>
    AffineMat<T>    srtMatrix(const AffineVec<T>& scale, const Quaternion<T>&rotation, const AffineVec<T>& translation) noexcept;
#pragma endregion


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
        AffineMat           operator*(const AffineMat& rhs) const noexcept;
        AffineMat           operator*(const float scalar) const noexcept;
        AffineMat           operator/(const float scalar) const noexcept;

        AffineMat&          operator+=(const AffineMat& rhs) noexcept;
        AffineMat&          operator-=(const AffineMat& rhs) noexcept;
        AffineMat&          operator*=(const AffineMat& rhs) noexcept;
        AffineMat&          operator*=(const float scalar) noexcept;
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
        void                preScale(const AffineVecF& scale) noexcept;
        void                postScale(const AffineVecF& scale) noexcept;
        void                preTranslate(const AffineVecF& translation) noexcept;
        void                postTranslate(const AffineVecF& translation) noexcept;
    
    public:
        void                setTranslation(const AffineVecF& translation) noexcept { setCol(3, translation); }
        AffineVecF          getTranslation() const noexcept { return getCol(3); }

    public:
        void                decomposeSRT(AffineVecF& outScale, AffineMat& outRotationMatrix, AffineVecF& outTranslation) const noexcept;

    public:
        AffineMat           inverse() const noexcept;

    private:
        AffineVecF          _rows[4];
    };

    
    // TODO: AffineMat<double>
}


#include <MintMath/Include/AffineMat.inl>


#endif // !MINT_MATH_MATRIX4X4_H
