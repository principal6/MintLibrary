#pragma once


#ifndef FS_VECTOR_R_H
#define FS_VECTOR_R_H


#include <FsCommon/Include/CommonDefinitions.h>

#include <initializer_list>


namespace fs
{
    namespace Math
    {
        template <uint8 N>
        class VectorR
        {
        public:
            static const double     dot(const VectorR& lhs, const VectorR& rhs) noexcept;

        public:
                                    VectorR();
                                    VectorR(const std::initializer_list<double>& initializerList);
                                    VectorR(const VectorR& rhs) = default;
                                    VectorR(VectorR&& rhs) noexcept = default;
                                    ~VectorR();
        
        public:
            VectorR&                operator=(const VectorR& rhs) = default;
            VectorR&                operator=(VectorR && rhs) noexcept = default;

        public:
            VectorR&                operator*=(const double scalar) noexcept;
            VectorR&                operator/=(const double scalar) noexcept;
            VectorR&                operator+=(const VectorR& rhs) noexcept;
            VectorR&                operator-=(const VectorR& rhs) noexcept;

        public:
            VectorR                 operator*(const double scalar) const noexcept;
            VectorR                 operator/(const double scalar) const noexcept;
            VectorR                 operator+(const VectorR& rhs) const noexcept;
            VectorR                 operator-(const VectorR& rhs) const noexcept;

        public:
            double&                 operator[](const uint8 index) noexcept;
            const double&           operator[](const uint8 index) const noexcept;

        public:
            void                    setZero() noexcept;
            void                    setComponent(const uint8 index, const double value) noexcept;
            const double            getComponent(const uint8 index) const noexcept;
            const double            maxComponent() const noexcept;
            const double            minComponent() const noexcept;

        public:
            const double            norm() const noexcept;
            void                    normalize() noexcept;

        public:
            const double            dot(const VectorR& rhs) const noexcept;

        private:
            double                  _c[N];
        };
    }
}


#endif // !FS_VECTOR_R_H
