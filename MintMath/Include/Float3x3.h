#pragma once


#ifndef MINT_FLOAT3X3_H
#define MINT_FLOAT3X3_H


#include <MintMath/Include/Float3.h>


namespace mint
{
#pragma region Forward declaration
    class Float2x2;
    class Float4x4;
#pragma endregion


    // 3x3 floating point matrix
    class Float3x3 final
    {
        friend Float4x4;

#pragma region Static Functions
    public:
        static Float3x3     rotationMatrixX(const float angle) noexcept;
        static Float3x3     rotationMatrixY(const float angle) noexcept;
        static Float3x3     rotationMatrixZ(const float angle) noexcept;
#pragma endregion

    public:
                            Float3x3();
        explicit            Float3x3(const float s);
        explicit            Float3x3(
                                const float m00, const float m01, const float m02,
                                const float m10, const float m11, const float m12,
                                const float m20, const float m21, const float m22);
                            Float3x3(const Float3x3& rhs)       = default;
                            Float3x3(Float3x3&& rhs) noexcept   = default;
                            ~Float3x3()                         = default;

    public:
        Float3x3&           operator=(const Float3x3& rhs)      = default;
        Float3x3&           operator=(Float3x3&& rhs) noexcept  = default;

    public:
        Float3x3            operator*(const Float3x3& rhs) const noexcept;
        Float3x3            operator*(const float scalar) const noexcept;
        Float3x3            operator/(const float scalar) const noexcept;
        
    public:
        void                setZero() noexcept;
        void                setIdentity() noexcept;

    public:
        Float2x2            minor(const uint32 row, const uint32 col) const noexcept;
        const float         determinant() const noexcept;
        Float3x3            transpose() const noexcept;
        Float3x3            cofactor() const noexcept;
        Float3x3            adjugate() const noexcept;
        Float3x3            inverse() const noexcept;

    public:
        Float3              mul(const Float3& v) const noexcept;

    public:
        union
        {
            float           _m[3][3];
            Float3          _row[3];
            struct
            {
                float       _11, _12, _13;
                float       _21, _22, _23;
                float       _31, _32, _33;
            };
        };
    };
}


#endif // !MINT_FLOAT3X3_H
