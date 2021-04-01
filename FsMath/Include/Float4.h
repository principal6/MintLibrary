#pragma once


#ifndef FS_FLOAT4_H
#define FS_FLOAT4_H


#include <FsMath/Include/Float3.h>


namespace fs
{
    class alignas(16) Float4 final
    {
    public:
        static const Float4     kZero;

    public:
        constexpr               Float4();
        constexpr explicit      Float4(const float s);
        constexpr explicit      Float4(const float x, const float y, const float z, const float w);
        constexpr explicit      Float4(const Float3& rhs);
        constexpr               Float4(const Float4& rhs)           = default;
        constexpr               Float4(Float4&& rhs) noexcept       = default;
                                ~Float4()                           = default;

    public:
        Float4&                 operator=(const Float4& rhs)        = default;
        Float4&                 operator=(Float4&& rhs) noexcept    = default;

    public:
        Float4&                 operator+=(const Float4& rhs);
        Float4&                 operator-=(const Float4& rhs);
        Float4&                 operator*=(const float s);
        Float4&                 operator/=(const float s);

    public:
        const Float4&           operator+() const noexcept;
        Float4                  operator-() const noexcept;

    public:
        Float4                  operator+(const Float4& rhs) const noexcept;
        Float4                  operator-(const Float4& rhs) const noexcept;
        Float4                  operator*(const float s) const noexcept;
        Float4                  operator/(const float s) const noexcept;

    public:
        float&                  operator[](const uint32 index) noexcept;
        const float&            operator[](const uint32 index) const noexcept;

    public:
        constexpr const bool    operator==(const Float4& rhs) const noexcept;
        constexpr const bool    operator!=(const Float4& rhs) const noexcept;

    public:
        static const float      dotProductRaw(const float* const a, const float* const b);

        // for matrix multiplication
        static const float      dotProductRaw(const float (&a)[4], const float bX, const float bY, const float bZ, const float bW);

    public:
        static const float      dot(const Float4& lhs, const Float4& rhs) noexcept;

        // in 3D affine space
        static Float4           cross(const Float4& lhs, const Float4& rhs) noexcept;
        static Float4           crossNormalize(const Float4& lhs, const Float4& rhs) noexcept;
        static Float4           normalize(const Float4& float4) noexcept;
    
    public:
        void                    normalize() noexcept;
    
    public:
        void                    setXyz(const float x, const float y, const float z) noexcept;
        void                    setXyz(const Float3& rhs) noexcept;
        void                    setXyz(const Float4& rhs) noexcept;
        Float4                  getXyz0() const noexcept;
        Float3                  getXyz() const noexcept;

    public:
        const float             lengthSqaure() const noexcept;
        const float             length() const noexcept;

    public:
        void                    set(const float x, const float y, const float z, const float w) noexcept;
        void                    setZero() noexcept;
        void                    setNan() noexcept;

    public:
        const bool              isNan() const noexcept;
        
    public:
        union
        {
            struct
            {
                float           _x;
                float           _y;
                float           _z;
                float           _w;
            };
            float               _f[4];
        };
        
    };
}


#include <FsMath/Include/Float4.inl>


#endif // !FS_FLOAT4_H
