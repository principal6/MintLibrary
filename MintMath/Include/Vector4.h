#pragma once


#ifndef MINT_MATH_VECTOR4_H
#define MINT_MATH_VECTOR4_H


#include <MintMath/Include/MathCommon.h>

#include <MintCommon/Include/Logger.h>

#include <immintrin.h>


namespace mint
{
    template<typename T>
    class Vector4
    {
        Vector4() = default;
    };


#pragma region Free functions
    const float         dot(const Vector4<float>& lhs, const Vector4<float>& rhs) noexcept;
    void                normalize(Vector4<float>& inOut) noexcept;
    Vector4<float>      cross(const Vector4<float>& lhs, const Vector4<float>& rhs) noexcept;

    const double        dot(const Vector4<double>& lhs, const Vector4<double>& rhs) noexcept;
    void                normalize(Vector4<double>& inOut) noexcept;
    Vector4<double>     cross(const Vector4<double>& lhs, const Vector4<double>& rhs) noexcept;
#pragma endregion


    template<>
    class Vector4<float>
    {
    public:
        static const    Vector4<float> kZero;
        static const    Vector4<float> kNan;
        static const    Vector4<float> k1000;
        static const    Vector4<float> k0100;
        static const    Vector4<float> k0010;
        static const    Vector4<float> k0001;

    public:
                        Vector4();
                        Vector4(const float scalar);
                        Vector4(const float x, const float y, const float z, const float w);
                        Vector4(const __m128 raw);
                        Vector4(const Vector4& rhs);
                        Vector4(Vector4&& rhs) noexcept;
                        ~Vector4() = default;
    
    public:
        Vector4&        operator=(const Vector4& rhs);
        Vector4&        operator=(Vector4&& rhs) noexcept;
    
    public:
        const bool      operator==(const Vector4& rhs) const noexcept;
        const bool      operator!=(const Vector4& rhs) const noexcept;
    
    public:
        Vector4&        operator+() noexcept;
        Vector4         operator-() const noexcept;

        Vector4         operator+(const Vector4& rhs) const noexcept;
        Vector4         operator-(const Vector4& rhs) const noexcept;
        Vector4         operator*(const float scalar) const noexcept;
        Vector4         operator/(const float scalar) const noexcept;
        
        Vector4&        operator+=(const Vector4& rhs) noexcept;
        Vector4&        operator-=(const Vector4& rhs) noexcept;
        Vector4&        operator*=(const float scalar) noexcept;
        Vector4&        operator/=(const float scalar) noexcept;
    
    public:
        void            load(const float x, const float y, const float z, const float w) noexcept;
        void            store(float(&vec)[4]) const noexcept;
        const __m128&   getRaw() const noexcept;
    
    public:
        const float     dot(const Vector4& rhs) const noexcept;
        const float     normSq() const noexcept;
        const float     norm() const noexcept;
        void            normalize() noexcept;
        Vector4<float>  cross(const Vector4<float>& rhs) const noexcept;

    private:
        __m128          _raw;
    };


    template<>
    class Vector4<double>
    {
    public:
                        Vector4();
                        Vector4(const double scalar);
                        Vector4(const double x, const double y, const double z, const double w);
                        Vector4(const __m256d raw);
                        Vector4(const Vector4& rhs);
                        Vector4(Vector4&& rhs) noexcept;
                        ~Vector4() = default;
    
    public:
        Vector4&        operator=(const Vector4& rhs);
        Vector4&        operator=(Vector4&& rhs) noexcept;
    
    public:
        const bool      operator==(const Vector4& rhs) const noexcept;
        const bool      operator!=(const Vector4& rhs) const noexcept;
    
    public:
        Vector4&        operator+() noexcept;
        Vector4         operator-() const noexcept;

        Vector4         operator+(const Vector4& rhs) const noexcept;
        Vector4         operator-(const Vector4& rhs) const noexcept;
        Vector4         operator*(const double scalar) const noexcept;
        Vector4         operator/(const double scalar) const noexcept;
        
        Vector4&        operator+=(const Vector4& rhs) noexcept;
        Vector4&        operator-=(const Vector4& rhs) noexcept;
        Vector4&        operator*=(const double scalar) noexcept;
        Vector4&        operator/=(const double scalar) noexcept;

    public:
        void            load(const double x, const double y, const double z, const double w) noexcept;
        void            store(double(&vec)[4]) const noexcept;
        const __m256d&  getRaw() const noexcept;
    
    public:
        const double    dot(const Vector4& rhs) const noexcept;
        const double    normSq() const noexcept;
        const double    norm() const noexcept;
        void            normalize() noexcept;
        Vector4<double> cross(const Vector4<double>& rhs) const noexcept;

    private:
        __m256d         _raw;
    };


    using Vector4F = Vector4<float>;
    using Vector4D = Vector4<double>;
}


#include <MintMath/Include/Vector4.inl>


#endif // !MINT_MATH_VECTOR4_H
