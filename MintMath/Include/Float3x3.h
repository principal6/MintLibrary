#pragma once


#ifndef MINT_FLOAT3X3_H
#define MINT_FLOAT3X3_H


#include <MintMath/Include/Float3.h>


namespace mint
{
    // 3x3 floating point matrix
    class Float3x3 final
    {
#pragma region Static Functions
    public:
        static Float3x3     rotationMatrixX(const float angle) noexcept;
        static Float3x3     rotationMatrixY(const float angle) noexcept;
        static Float3x3     rotationMatrixZ(const float angle) noexcept;
#pragma endregion

    public:
                            Float3x3();
        explicit            Float3x3(const float scalar);
        explicit            Float3x3(
                                const float _11, const float _12, const float _13,
                                const float _21, const float _22, const float _23,
                                const float _31, const float _32, const float _33);
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
        float               determinant() const noexcept;
        Float3x3            transpose() const noexcept;
        Float3x3            inverse() const noexcept;

    public:
        Float3              mul(const Float3& vec) const noexcept;

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
