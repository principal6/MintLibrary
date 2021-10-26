#pragma once


#ifndef MINT_VECTOR_R_H
#define MINT_VECTOR_R_H


#include <MintCommon/Include/CommonDefinitions.h>


namespace std
{
    template <class _Elem>
    class initializer_list;
}


namespace mint
{
    namespace Math
    {
        template <int32 N, typename T>
        class VectorR
        {
            static_assert(0 < N, "N must be greater than 0!");

        public:
            static VectorR<N, T>            standardUnitVector(const int32 math_i) noexcept;
            static const T                  dot(const VectorR& lhs, const VectorR& rhs) noexcept;
            static const T                  distance(const VectorR& lhs, const VectorR& rhs) noexcept;
            static const T                  angle(const VectorR& lhs, const VectorR& rhs) noexcept;
            static const bool               isOrthogonal(const VectorR& lhs, const VectorR& rhs) noexcept;
            static VectorR<N, T>            projectUOntoV(const VectorR& u, const VectorR& v) noexcept;

        public:
                                            VectorR();
                                            template <class ...Args>
                                            VectorR(Args ... args);
                                            VectorR(const std::initializer_list<T>& initializerList);
                                            VectorR(const VectorR& rhs) = default;
                                            VectorR(VectorR&& rhs) noexcept = default;
                                            ~VectorR();
        
        public:
            VectorR&                        operator=(const VectorR& rhs) = default;
            VectorR&                        operator=(VectorR && rhs) noexcept = default;

        public:
            VectorR&                        operator*=(const T scalar) noexcept;
            VectorR&                        operator/=(const T scalar) noexcept;
            VectorR&                        operator+=(const VectorR& rhs) noexcept;
            VectorR&                        operator-=(const VectorR& rhs) noexcept;
            
        public:
            VectorR                         operator*(const T scalar) const noexcept;
            VectorR                         operator/(const T scalar) const noexcept;
            VectorR                         operator+(const VectorR& rhs) const noexcept;
            VectorR                         operator-(const VectorR& rhs) const noexcept;
            
        public:
            const VectorR&                  operator+() const noexcept;
            VectorR                         operator-() const noexcept;

        public:
            T&                              operator[](const uint32 index) noexcept;
            const T&                        operator[](const uint32 index) const noexcept;

        public:
            const bool                      operator==(const VectorR& rhs) const noexcept;
            const bool                      operator!=(const VectorR& rhs) const noexcept;

        public:
            void                            setZero() noexcept;
            void                            setComponent(const uint32 index, const T value) noexcept;
            const T                         getComponent(const uint32 index) const noexcept;
            const T                         maxComponent() const noexcept;
            const T                         minComponent() const noexcept;

        public:
            const T                         normSquared() const noexcept;
            const T                         norm() const noexcept;
            VectorR<N, T>&                  setNormalized() noexcept;
            VectorR<N, T>                   normalize() const noexcept;
            const bool                      isUnitVector() const noexcept;

        public:
            const T                         dot(const VectorR& rhs) const noexcept;
            const T                         distance(const VectorR& rhs) const noexcept;
            const T                         angle(const VectorR& rhs) const noexcept;
            const bool                      isOrthogonalTo(const VectorR& rhs) const noexcept;
            VectorR<N, T>                   projectOnto(const VectorR& rhs) const noexcept;

        private:
            T                               _c[N];
        };


        template <typename T>
        VectorR<3, T>                       cross(const VectorR<3, T>& lhs, const VectorR<3, T>& rhs) noexcept;
        
        template <int32 N, typename T>
        VectorR<N, T>                       operator*(const T scalar, const VectorR<N, T>& vector) noexcept;


        template <int32 N>
        using VectorF = VectorR<N, float>;
        
        template <int32 N>
        using VectorD = VectorR<N, double>;


        template <typename T>
        using Vector2 = VectorR<2, T>;
        
        template <typename T>
        using Vector3 = VectorR<3, T>;
        
        template <typename T>
        using Vector4 = VectorR<4, T>;
    }
}


#endif // !MINT_VECTOR_R_H
