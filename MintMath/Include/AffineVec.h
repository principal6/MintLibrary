#pragma once


#ifndef MINT_MATH_VECTOR4_H
#define MINT_MATH_VECTOR4_H


#include <MintMath/Include/MathCommon.h>

#include <MintCommon/Include/Logger.h>

#include <immintrin.h>


namespace mint
{
    template<typename T>
    class AffineVec
    {
        AffineVec() = default;
    };


#pragma region Free functions
    template<typename T>
    AffineVec<T>        operator*(const T scalar, const AffineVec<T>& vec) noexcept;

    template<typename T>
    AffineVec<T>        normalize(const AffineVec<T>& in) noexcept;

    template<typename T>
    void                normalize(AffineVec<T>& inOut) noexcept;

    const float         dot(const AffineVec<float>& lhs, const AffineVec<float>& rhs) noexcept;
    AffineVec<float>    cross(const AffineVec<float>& lhs, const AffineVec<float>& rhs) noexcept;

    const double        dot(const AffineVec<double>& lhs, const AffineVec<double>& rhs) noexcept;
    AffineVec<double>   cross(const AffineVec<double>& lhs, const AffineVec<double>& rhs) noexcept;
#pragma endregion


    template<>
    class AffineVec<float>
    {
    public:
        static const        AffineVec<float> kZero;
        static const        AffineVec<float> kNan;
        static const        AffineVec<float> k1000;
        static const        AffineVec<float> k0100;
        static const        AffineVec<float> k0010;
        static const        AffineVec<float> k0001;

    public:
                            AffineVec();
                            AffineVec(const float scalar);
                            AffineVec(const float x, const float y, const float z, const float w);
                            AffineVec(const __m128 raw);
                            AffineVec(const AffineVec& rhs);
                            AffineVec(AffineVec&& rhs) noexcept;
                            ~AffineVec() = default;
    
    public:
        AffineVec&          operator=(const AffineVec& rhs);
        AffineVec&          operator=(AffineVec&& rhs) noexcept;
    
    public:
        const bool          operator==(const AffineVec& rhs) const noexcept;
        const bool          operator!=(const AffineVec& rhs) const noexcept;
    
    public:
        AffineVec&          operator+() noexcept;
        AffineVec           operator-() const noexcept;

        AffineVec           operator+(const AffineVec& rhs) const noexcept;
        AffineVec           operator-(const AffineVec& rhs) const noexcept;
        AffineVec           operator*(const AffineVec& rhs) const noexcept;
        AffineVec           operator/(const AffineVec& rhs) const noexcept;
        AffineVec           operator*(const float scalar) const noexcept;
        AffineVec           operator/(const float scalar) const noexcept;
    
        AffineVec&          operator+=(const AffineVec& rhs) noexcept;
        AffineVec&          operator-=(const AffineVec& rhs) noexcept;
        AffineVec&          operator*=(const AffineVec& rhs) noexcept;
        AffineVec&          operator/=(const AffineVec& rhs) noexcept;
        AffineVec&          operator*=(const float scalar) noexcept;
        AffineVec&          operator/=(const float scalar) noexcept;
    
    public:
        void                set(const float x, const float y, const float z, const float w) noexcept;
        void                setComponent(const int32 i, const float scalar) noexcept;
        void                addComponent(const int32 i, const float scalar) noexcept;
    
    public:
        void                get(float(&vec)[4]) const noexcept;
        const float         getComponent(const int32 i) const noexcept;
        const __m128&       getRaw() const noexcept;
    
    public:
        const float         dot(const AffineVec& rhs) const noexcept;
        const float         normSq() const noexcept;
        const float         norm() const noexcept;
        void                normalize() noexcept;
        AffineVec<float>    cross(const AffineVec<float>& rhs) const noexcept;

    private:
        __m128              _raw;
    };


    template<>
    class AffineVec<double>
    {
    public:
                            AffineVec();
                            AffineVec(const double scalar);
                            AffineVec(const double x, const double y, const double z, const double w);
                            AffineVec(const __m256d raw);
                            AffineVec(const AffineVec& rhs);
                            AffineVec(AffineVec&& rhs) noexcept;
                            ~AffineVec() = default;
    
    public:
        AffineVec&          operator=(const AffineVec& rhs);
        AffineVec&          operator=(AffineVec&& rhs) noexcept;
    
    public:
        const bool          operator==(const AffineVec& rhs) const noexcept;
        const bool          operator!=(const AffineVec& rhs) const noexcept;
    
    public:
        AffineVec&          operator+() noexcept;
        AffineVec           operator-() const noexcept;

        AffineVec           operator+(const AffineVec& rhs) const noexcept;
        AffineVec           operator-(const AffineVec& rhs) const noexcept;
        AffineVec           operator*(const AffineVec& rhs) const noexcept;
        AffineVec           operator/(const AffineVec& rhs) const noexcept;
        AffineVec           operator*(const double scalar) const noexcept;
        AffineVec           operator/(const double scalar) const noexcept;
        
        AffineVec&          operator+=(const AffineVec& rhs) noexcept;
        AffineVec&          operator-=(const AffineVec& rhs) noexcept;
        AffineVec&          operator*=(const AffineVec& rhs) noexcept;
        AffineVec&          operator/=(const AffineVec& rhs) noexcept;
        AffineVec&          operator*=(const double scalar) noexcept;
        AffineVec&          operator/=(const double scalar) noexcept;

    public:
        void                set(const double x, const double y, const double z, const double w) noexcept;
        void                setComponent(const int32 i, const double scalar) noexcept;
        void                addComponent(const int32 i, const double scalar) noexcept;

    public:
        void                get(double(&vec)[4]) const noexcept;
        const double        getComponent(const int32 i) const noexcept;
        const __m256d&      getRaw() const noexcept;
    
    public:
        const double        dot(const AffineVec& rhs) const noexcept;
        const double        normSq() const noexcept;
        const double        norm() const noexcept;
        void                normalize() noexcept;
        AffineVec<double>   cross(const AffineVec<double>& rhs) const noexcept;

    private:
        __m256d             _raw;
    };


    using AffineVecF = AffineVec<float>;
    using AffineVecD = AffineVec<double>;
}


#include <MintMath/Include/AffineVec.inl>


#endif // !MINT_MATH_VECTOR4_H