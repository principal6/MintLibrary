﻿#pragma once


#ifndef _MINT_MATH_FLOAT2_H_
#define _MINT_MATH_FLOAT2_H_


#include <MintCommon/Include/CommonDefinitions.h>

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
        static const Float2     kMax;
        static const Float2     kNan;

    public:
        static constexpr Float2 max(const Float2& a, const Float2& b) noexcept;
        static constexpr Float2 min(const Float2& a, const Float2& b) noexcept;

    public:
        constexpr               Float2();
        constexpr explicit      Float2(const float scalar);
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
        Float2&                 operator*=(const float scalar);
        Float2&                 operator/=(const float scalar);

    public:
        const Float2&           operator+() const noexcept;
        Float2                  operator-() const noexcept;

    public:
        Float2                  operator+(const Float2& rhs) const noexcept;
        Float2                  operator-(const Float2& rhs) const noexcept;
        Float2                  operator*(const float scalar) const noexcept;
        Float2                  operator/(const float scalar) const noexcept;

    public:
        float&                  operator[](const uint32 index) noexcept;
        const float&            operator[](const uint32 index) const noexcept;

    public:
        bool                    operator==(const Float2& rhs) const noexcept;
        bool                    operator!=(const Float2& rhs) const noexcept;

    public:
        static float            dot(const Float2& lhs, const Float2& rhs) noexcept;
        static Float2           normalize(const Float2& in) noexcept;
        static Float2           abs(const Float2& rhs) noexcept;

    public:
        float                   lengthSqaure() const noexcept;
        float                   length() const noexcept;
        float                   maxElement() const noexcept;
        float                   minElement() const noexcept;
        
    public:
        void                    set(const float x, const float y) noexcept;
        void                    setZero() noexcept;
        void                    setNan() noexcept;

    public:
        bool                    isNan() const noexcept;
        bool                    hasNegativeElement() const noexcept;

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

    Float2                      operator*(const float scalar, const Float2& vector) noexcept;
}


#include <MintMath/Include/Float2.inl>


#endif // !_MINT_MATH_FLOAT2_H_
