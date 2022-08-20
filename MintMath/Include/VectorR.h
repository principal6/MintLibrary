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
        bool                            equals(const T(&lhs)[N], const T(&rhs)[N], const T epsilon = 0) noexcept;
        
        template <int32 N, typename T>
        void                            copyVec(const T(&src)[N], T(&dest)[N]) noexcept;
        
        template <int32 N, typename T>
        T                               dot(const T(&lhs)[N], const T(&rhs)[N]) noexcept;
        
        template <typename T>
        void                            cross(const T(&lhs)[3], const T(&rhs)[3], T(&out)[3]) noexcept;
        
        template <typename T>
        void                            cross(const T(&lhs)[4], const T(&rhs)[4], T(&out)[4]) noexcept;

        template <int32 N, typename T>
        T                               normSq(const T(&vec)[N]) noexcept;

        template <int32 N, typename T>
        T                               norm(const T(&vec)[N]) noexcept;
        
        template <int32 N, typename T>
        void                            normalize(T(&inOut)[N]) noexcept;

        template <int32 N, typename T>
        void                            setZeroVec(T(&vec)[N]) noexcept;
        
        template <int32 N, typename T>
        void                            setNan(T(&vec)[N]) noexcept;

        template <int32 N, typename T>
        bool                            isNan(const T(&vec)[N]) noexcept;

        template <int32 N, typename T>
        void                            setAddVec(T(&lhs)[N], const T(&rhs)[N]) noexcept;
        
        template <int32 N, typename T>
        void                            setSubVec(T(&lhs)[N], const T(&rhs)[N]) noexcept;

        template <int32 N, typename T>
        void                            setMulVec(T(&vec)[N], const float scalar) noexcept;
        
        template <int32 N, typename T>
        void                            setDivVec(T(&vec)[N], const float scalar) noexcept;
    }
    

    template <int32 N, typename T>
    class VectorR
    {
        static_assert(N > 0, "N must be greater than 0!");

    public:
        static VectorR<N, T>            standardUnitVector(const int32 math_i) noexcept;
        static T                        dot(const VectorR& lhs, const VectorR& rhs) noexcept;
        static T                        distance(const VectorR& lhs, const VectorR& rhs) noexcept;
        static T                        angle(const VectorR& lhs, const VectorR& rhs) noexcept;
        static bool                     isOrthogonal(const VectorR& lhs, const VectorR& rhs) noexcept;
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
        bool                            operator==(const VectorR& rhs) const noexcept;
        bool                            operator!=(const VectorR& rhs) const noexcept;

    public:
        void                            setZero() noexcept;
        
    public:
        VectorR<N, T>&                  setComponent(const uint32 index, const T value) noexcept;
        T                               getComponent(const uint32 index) const noexcept;
        T&                              x() noexcept;
        T&                              y() noexcept;
        T&                              z() noexcept;
        T&                              w() noexcept;
        const T&                        x() const noexcept;
        const T&                        y() const noexcept;
        const T&                        z() const noexcept;
        const T&                        w() const noexcept;
        
    public:
        T                               maxComponent() const noexcept;
        T                               minComponent() const noexcept;

    public:
        T                               normSquared() const noexcept;
        T                               norm() const noexcept;
        VectorR<N, T>&                  setNormalized() noexcept;
        VectorR<N, T>                   normalize() const noexcept;
        bool                            isUnitVector() const noexcept;

    public:
        T                               dot(const VectorR& rhs) const noexcept;
        T                               distance(const VectorR& rhs) const noexcept;
        T                               angle(const VectorR& rhs) const noexcept;
        bool                            isOrthogonalTo(const VectorR& rhs) const noexcept;
        VectorR<N, T>                   projectOnto(const VectorR& rhs) const noexcept;

    public:
        VectorR<N - 1, T>               shrink() const noexcept;

    public:
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
    using VectorR3 = VectorR<3, T>;
        
    template <typename T>
    using VectorR4 = VectorR<4, T>;


    namespace VectorUtils
    {
        template <int32 N, typename T>
        void setNan(VectorR<N, T>& in) noexcept;

        template <int32 N, typename T>
        bool isNan(const VectorR<N, T>& in) noexcept;
    }
}


#endif // !MINT_VECTOR_R_H
