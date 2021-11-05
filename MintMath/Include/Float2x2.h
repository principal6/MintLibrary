#pragma once


#ifndef MINT_FLOAT2X2_H
#define MINT_FLOAT2X2_H


#include <MintMath/Include/Float2.h>


namespace mint
{
    // 2x2 floating point matrix
    class Float2x2 final
    {
        static constexpr uint32     kMaxIndex = 1;

#pragma region Static Functions
    public:
        static Float2x2             rotationMatrix(const float angle) noexcept;
#pragma endregion


    public:
                                    Float2x2();
        explicit                    Float2x2(const float s);
        explicit                    Float2x2(const float m00, const float m01, const float m10, const float m11);
                                    Float2x2(const Float2x2& rhs)       = default;
                                    Float2x2(Float2x2&& rhs) noexcept   = default;
                                    ~Float2x2()                         = default;
        
    public:
        Float2x2&                   operator=(const Float2x2& rhs)      = default;
        Float2x2&                   operator=(Float2x2&& rhs) noexcept  = default;
    public:
        Float2x2                    operator*(const float scalar) const noexcept;
        Float2x2                    operator/(const float scalar) const noexcept;

    public:
        Float2x2                    operator*(const Float2x2& rhs) const noexcept;

    public:
        void                        set(const uint32 row, const uint32 col, const float newValue) noexcept;
        const float                 get(const uint32 row, const uint32 col) const noexcept;
        void                        setZero() noexcept;
        void                        setIdentity() noexcept;

    public:
        const float                 determinant() const noexcept;
        Float2x2                    inverse() const noexcept;
        const bool                  isInvertible() const noexcept;

    public:
        Float2                      mul(const Float2& v) const noexcept;

    public:
        union
        {
            float                   _m[2][2];
            Float2                  _row[2];
            struct
            {
                float               _11, _12;
                float               _21, _22;
            };
        };
    };
}


#endif // !MINT_FLOAT2X2_H
