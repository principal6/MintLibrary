#pragma once


#ifndef FS_VECTOR_R_H
#define FS_VECTOR_R_H


#include <FsCommon/Include/CommonDefinitions.h>

#include <initializer_list>


namespace fs
{
    namespace Math
    {
        template <int32 N>
        class VectorR
        {
            static_assert(0 < N, "N must be greater than 0!");

        public:
            static const double     dot(const VectorR& lhs, const VectorR& rhs) noexcept;
            //VectorR                 operator*(const double scalar, const VectorR& vector) noexcept;

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
            double&                 operator[](const uint32 index) noexcept;
            const double&           operator[](const uint32 index) const noexcept;

        public:
            void                    setZero() noexcept;
            void                    setComponent(const uint32 index, const double value) noexcept;
            const double            getComponent(const uint32 index) const noexcept;
            const double            maxComponent() const noexcept;
            const double            minComponent() const noexcept;

        public:
            const double            normSquared() const noexcept;
            const double            norm() const noexcept;
            VectorR<N>&             setNormalized() noexcept;
            VectorR<N>              normalize() const noexcept;
            const bool              isUnitVector() const noexcept;

        public:
            const double            dot(const VectorR& rhs) const noexcept;
            VectorR<3>              cross(const VectorR<3>& rhs) const noexcept;
            const double            distance(const VectorR& rhs) const noexcept;
            const double            angle(const VectorR& rhs) const noexcept;
            const bool              isOrthogonalTo(const VectorR& rhs) const noexcept;
            VectorR<N>              projectOnto(const VectorR& rhs) const noexcept;

        private:
            double                  _c[N];
        };


        template <int32 N>
        VectorR<N>                  operator*(const double scalar, const VectorR<N>& vector) noexcept;
    }
}


#endif // !FS_VECTOR_R_H
