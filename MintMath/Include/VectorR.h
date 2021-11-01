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
        const bool                      equals(const T(&lhs)[N], const T(&rhs)[N], const T epsilon = 0) noexcept;
        
        template <int32 N, typename T>
        const T                         dot(const T(&lhs)[N], const T(&rhs)[N]) noexcept;

        template <int32 N, typename T>
        const T                         normSq(const T(&vec)[N]) noexcept;

        template <int32 N, typename T>
        const T                         norm(const T(&vec)[N]) noexcept;

        template <int32 N, typename T>
        void                            setZero(T(&vec)[N]) noexcept;
        
        template <int32 N, typename T>
        void                            setNan(T(&vec)[N]) noexcept;

        template <int32 N, typename T>
        const bool                      isNan(const T(&vec)[N]) noexcept;

        template <int32 N, typename T>
        void                            setAdd(T(&lhs)[N], const T(&rhs)[N]) noexcept;
        
        template <int32 N, typename T>
        void                            setSub(T(&lhs)[N], const T(&rhs)[N]) noexcept;

        template <int32 N, typename T>
        void                            setMul(T(&vec)[N], const float scalar) noexcept;
        
        template <int32 N, typename T>
        void                            setDiv(T(&vec)[N], const float scalar) noexcept;
    }
    

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
        constexpr                       VectorR();
                                        template <class ...Args>
        constexpr                       VectorR(Args ... args);
        constexpr                       VectorR(const std::initializer_list<T>& initializerList);
        constexpr                       VectorR(const VectorR& rhs) = default;
        constexpr                       VectorR(VectorR&& rhs) noexcept = default;
                                        ~VectorR() = default;
        
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
        
    public:
        VectorR<N, T>&                  setComponent(const uint32 index, const T value) noexcept;
        const T                         getComponent(const uint32 index) const noexcept;
        T&                              x() noexcept;
        T&                              y() noexcept;
        T&                              z() noexcept;
        T&                              w() noexcept;
        const T&                        x() const noexcept;
        const T&                        y() const noexcept;
        const T&                        z() const noexcept;
        const T&                        w() const noexcept;
        
    public:
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

    public:
        VectorR<N - 1, T>               shrink() const noexcept;

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


    namespace VectorUtils
    {
        template <int32 N, typename T>
        void setNan(VectorR<N, T>& in) noexcept;

        template <int32 N, typename T>
        const bool isNan(const VectorR<N, T>& in) noexcept;
    }
}


#endif // !MINT_VECTOR_R_H
