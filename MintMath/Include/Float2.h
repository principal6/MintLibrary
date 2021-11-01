#pragma once


#ifndef MINT_FLOAT2_H
#define MINT_FLOAT2_H


#include <MintMath/Include/MathCommon.h>


namespace mint
{
#pragma region Forward declaration
    class Int2;
#pragma endregion


    // No SIMD
    class alignas(8) Float2 final
    {
    public:
        static const Float2     kZero;
        static const Float2     kOne;
        static const Float2     kNegativeOne;
        static const Float2     kNan;

    public:
        constexpr               Float2();
        constexpr explicit      Float2(const float s);
        constexpr explicit      Float2(const float x, const float y);
        explicit                Float2(const Int2& rhs);
        constexpr               Float2(const Float2& rhs)           = default;
        constexpr               Float2(Float2&& rhs) noexcept       = default;
                                ~Float2()                           = default;

    public:
        Float2&                 operator=(const Float2& rhs)        = default;
        Float2&                 operator=(Float2&& rhs) noexcept    = default;

    public:
        Float2&                 operator+=(const Float2& rhs);        
        Float2&                 operator-=(const Float2& rhs);
        Float2&                 operator*=(const float s);
        Float2&                 operator/=(const float s);

    public:
        const Float2&           operator+() const noexcept;
        Float2                  operator-() const noexcept;

    public:
        Float2                  operator+(const Float2& rhs) const noexcept;
        Float2                  operator-(const Float2& rhs) const noexcept;
        Float2                  operator*(const float s) const noexcept;
        Float2                  operator/(const float s) const noexcept;

    public:
        float&                  operator[](const uint32 index) noexcept;
        const float&            operator[](const uint32 index) const noexcept;

    public:
        const bool              operator==(const Float2& rhs) const noexcept;
        const bool              operator!=(const Float2& rhs) const noexcept;

    public:
        static const float      dot(const Float2& lhs, const Float2& rhs) noexcept;
        static Float2           normalize(const Float2& float2) noexcept;
        static Float2           abs(const Float2& rhs) noexcept;

    public:
        const float             lengthSqaure() const noexcept;
        const float             length() const noexcept;
        const float             maxElement() const noexcept;
        const float             minElement() const noexcept;
        
    public:
        void                    set(const float x, const float y) noexcept;
        void                    setZero() noexcept;
        void                    setNan() noexcept;

    public:
        const bool              isNan() const noexcept;
        const bool              hasNegativeElement() const noexcept;

    public:
        union
        {
            struct
            {
                float           _x;
                float           _y;
            };
            float               _c[2];
        };
        
    };
}


#include <MintMath/Include/Float2.inl>


#endif // !MINT_FLOAT2_H
