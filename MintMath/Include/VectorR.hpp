#pragma once


#include <MintMath/Include/VectorR.h>

#include <MintMath/Include/MathCommon.h>

#include <MintMath/Include/Matrix.hpp>

#include <initializer_list>


namespace mint
{
    namespace Math
    {
        template<int32 N, typename T>
        MINT_INLINE bool equals(const T(&lhs)[N], const T(&rhs)[N], const T epsilon) noexcept
        {
            for (int32 i = 0; i < N; ++i)
            {
                const T absDiff = ::abs(lhs[i] - rhs[i]);
                if (epsilon < absDiff)
                {
                    return false;
                }
            }
            return true;
        }

        template<int32 N, typename T>
        MINT_INLINE void copyVec(const T(&src)[N], T(&dest)[N]) noexcept
        {
            for (int32 i = 0; i < N; ++i)
            {
                dest[i] = src[i];
            }
        }

        template<int32 N, typename T>
        MINT_INLINE T dot(const T(&lhs)[N], const T(&rhs)[N]) noexcept
        {
            T result{};
            for (int32 i = 0; i < N; ++i)
            {
                result += lhs[i] * rhs[i];
            }
            return result;
        }

        template<typename T>
        MINT_INLINE void cross(const T(&lhs)[3], const T(&rhs)[3], T(&out)[3]) noexcept
        {
            out[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1];
            out[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2];
            out[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0];
        }

        template<typename T>
        void cross(const T(&lhs)[4], const T(&rhs)[4], T(&out)[4]) noexcept
        {
            out[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1];
            out[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2];
            out[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0];
            out[3] = static_cast<T>(0); // It is a vector, not point!
        }

        template<int32 N, typename T>
        MINT_INLINE T normSq(const T(&vec)[N]) noexcept
        {
            return dot(vec, vec);
        }

        template<int32 N, typename T>
        MINT_INLINE T norm(const T(&vec)[N]) noexcept
        {
            return ::sqrt(normSq(vec));
        }

        template<int32 N, typename T>
        MINT_INLINE void normalize(T(&inOut)[N]) noexcept
        {
            const T norm_ = norm(inOut);
            Math::setDivVec(inOut, norm_);
        }

        template<int32 N, typename T>
        MINT_INLINE void setZeroVec(T(&vec)[N]) noexcept
        {
            for (int32 i = 0; i < N; ++i)
            {
                vec[i] = 0;
            }
        }

        template<int32 N, typename T>
        MINT_INLINE void setNan(T(&vec)[N]) noexcept
        {
            for (int32 i = 0; i < N; ++i)
            {
                vec[i] = Math::nan();
            }
        }

        template<int32 N, typename T>
        MINT_INLINE bool isNan(const T(&vec)[N]) noexcept
        {
            for (int32 i = 0; i < N; ++i)
            {
                if (Math::isNan(vec[i]))
                {
                    return true;
                }
            }
            return false;
        }

        template<int32 N, typename T>
        MINT_INLINE void setAddVec(T(&lhs)[N], const T(&rhs)[N]) noexcept
        {
            for (int32 i = 0; i < N; ++i)
            {
                lhs[i] += rhs[i];
            }
        }

        template<int32 N, typename T>
        MINT_INLINE void setSubVec(T(&lhs)[N], const T(&rhs)[N]) noexcept
        {
            for (int32 i = 0; i < N; ++i)
            {
                lhs[i] -= rhs[i];
            }
        }
        
        template<int32 N, typename T>
        MINT_INLINE void setMulVec(T(&vec)[N], const float scalar) noexcept
        {
            for (int32 i = 0; i < N; ++i)
            {
                vec[i] *= scalar;
            }
        }

        template<int32 N, typename T>
        MINT_INLINE void setDivVec(T(&vec)[N], const float scalar) noexcept
        {
            MINT_ASSERT(scalar != 0.0, "0 으로 나누려 합니다!");
            for (int32 i = 0; i < N; ++i)
            {
                vec[i] /= scalar;
            }
        }
    }
    

    template<int32 N, typename T>
    inline VectorR<N, T> VectorR<N, T>::standardUnitVector(const int32 math_i) noexcept
    {
        VectorR<N, T> result;
        result.setComponent(math_i - 1, 1.0);
        return result;
    }

    template<int32 N, typename T>
    MINT_INLINE T VectorR<N, T>::dot(const VectorR& lhs, const VectorR& rhs) noexcept
    {
        return Math::dot(lhs._c, rhs._c);
    }

    template<int32 N, typename T>
    MINT_INLINE T VectorR<N, T>::distance(const VectorR& lhs, const VectorR& rhs) noexcept
    {
        return (rhs - lhs).norm();
    }

    template<int32 N, typename T>
    MINT_INLINE T VectorR<N, T>::angle(const VectorR& lhs, const VectorR& rhs) noexcept
    {
        return ::acos(lhs.normalize().dot(rhs.normalize()));
    }

    template<int32 N, typename T>
    MINT_INLINE bool VectorR<N, T>::isOrthogonal(const VectorR& lhs, const VectorR& rhs) noexcept
    {
        return Math::equals(lhs.dot(rhs), 0.0);
    }

    template<int32 N, typename T>
    MINT_INLINE VectorR<N, T> VectorR<N, T>::projectUOntoV(const VectorR& u, const VectorR& v) noexcept
    {
        return (u.dot(v) / u.dot(u)) * u;
    }

    template<int32 N, typename T>
    inline constexpr VectorR<N, T>::VectorR()
        : _c{}
    {
        __noop;
    }

    template<int32 N, typename T>
    template <class ...Args>
    inline constexpr VectorR<N, T>::VectorR(Args... args)
        : _c{ args... }
    {
        __noop;
    }

    template<int32 N, typename T>
    inline constexpr VectorR<N, T>::VectorR(const std::initializer_list<T>& initializerList)
    {
        const int32 count = min(static_cast<int32>(initializerList.size()), N);
        const T* const first = initializerList.begin();
        for (int32 index = 0; index < count; ++index)
        {
            _c[index] = *(first + index);
        }
    }

    template<int32 N, typename T>
    MINT_INLINE VectorR<N, T>& VectorR<N, T>::operator*=(const T scalar) noexcept
    {
        for (int32 index = 0; index < N; ++index)
        {
            _c[index] *= scalar;
        }
        return *this;
    }

    template<int32 N, typename T>
    MINT_INLINE VectorR<N, T>& VectorR<N, T>::operator/=(const T scalar) noexcept
    {
        MINT_ASSERT(scalar != 0.0, "0 으로 나누려 합니다!");

        for (int32 index = 0; index < N; ++index)
        {
            _c[index] /= scalar;
        }
        return *this;
    }

    template<int32 N, typename T>
    MINT_INLINE VectorR<N, T>& VectorR<N, T>::operator+=(const VectorR& rhs) noexcept
    {
        for (int32 index = 0; index < N; ++index)
        {
            _c[index] += rhs._c[index];
        }
        return *this;
    }

    template<int32 N, typename T>
    MINT_INLINE VectorR<N, T>& VectorR<N, T>::operator-=(const VectorR& rhs) noexcept
    {
        for (int32 index = 0; index < N; ++index)
        {
            _c[index] -= rhs._c[index];
        }
        return *this;
    }

    template<int32 N, typename T>
    MINT_INLINE VectorR<N, T> VectorR<N, T>::operator*(const T scalar) const noexcept
    {
        VectorR result = *this;
        result *= scalar;
        return result;
    }

    template<int32 N, typename T>
    MINT_INLINE VectorR<N, T> VectorR<N, T>::operator/(const T scalar) const noexcept
    {
        VectorR result = *this;
        result /= scalar;
        return result;
    }

    template<int32 N, typename T>
    MINT_INLINE VectorR<N, T> VectorR<N, T>::operator+(const VectorR& rhs) const noexcept
    {
        VectorR result = *this;
        result += rhs;
        return result;
    }

    template<int32 N, typename T>
    MINT_INLINE VectorR<N, T> VectorR<N, T>::operator-(const VectorR& rhs) const noexcept
    {
        VectorR result = *this;
        result -= rhs;
        return result;
    }

    template<int32 N, typename T>
    MINT_INLINE const VectorR<N, T>& VectorR<N, T>::operator+() const noexcept
    {
        return *this;
    }

    template<int32 N, typename T>
    MINT_INLINE VectorR<N, T> VectorR<N, T>::operator-() const noexcept
    {
        VectorR<N, T> result;
        for (int32 i = 0; i < N; ++i)
        {
            result._c[i] = -_c[i];
        }
        return result;
    }

    template<int32 N, typename T>
    MINT_INLINE T& VectorR<N, T>::operator[](const uint32 index) noexcept
    {
        MINT_ASSERT(index < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");
        return _c[index];
    }

    template<int32 N, typename T>
    MINT_INLINE const T& VectorR<N, T>::operator[](const uint32 index) const noexcept
    {
        MINT_ASSERT(index < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");
        return _c[index];
    }

    template<int32 N, typename T>
    MINT_INLINE bool VectorR<N, T>::operator==(const VectorR& rhs) const noexcept
    {
        for (int32 i = 0; i < N; ++i)
        {
            if (rhs._c[i] != _c[i])
            {
                return false;
            }
        }
        return true;
    }

    template<int32 N, typename T>
    MINT_INLINE bool VectorR<N, T>::operator!=(const VectorR& rhs) const noexcept
    {
        return !(rhs == *this);
    }

    template<int32 N, typename T>
    MINT_INLINE void VectorR<N, T>::setZero() noexcept
    {
        for (int32 index = 0; index < N; ++index)
        {
            _c[index] = 0.0;
        }
    }

    template<int32 N, typename T>
    MINT_INLINE VectorR<N, T>& VectorR<N, T>::setComponent(const uint32 index, const T value) noexcept
    {
        if (index < static_cast<uint32>(N))
        {
            _c[index] = value;
        }
        return *this;
    }
        
    template<int32 N, typename T>
    MINT_INLINE T VectorR<N, T>::getComponent(const uint32 index) const noexcept
    {
        MINT_ASSERT(index < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");
        return _c[index];
    }

    template<int32 N, typename T>
    MINT_INLINE T& VectorR<N, T>::x() noexcept
    {
        return _c[0];
    }

    template<int32 N, typename T>
    MINT_INLINE T& VectorR<N, T>::y() noexcept
    {
        static_assert(N >= 2, "Vector dimension is not enough!");
        return _c[1];
    }

    template<int32 N, typename T>
    MINT_INLINE T& VectorR<N, T>::z() noexcept
    {
        static_assert(N >= 3, "Vector dimension is not enough!");
        return _c[2];
    }

    template<int32 N, typename T>
    MINT_INLINE T& VectorR<N, T>::w() noexcept
    {
        static_assert(N >= 4, "Vector dimension is not enough!");
        return _c[3];
    }

    template<int32 N, typename T>
    MINT_INLINE const T& VectorR<N, T>::x() const noexcept
    {
        return _c[0];
    }

    template<int32 N, typename T>
    MINT_INLINE const T& VectorR<N, T>::y() const noexcept
    {
        static_assert(N >= 2, "Vector dimension is not enough!");
        return _c[1];
    }

    template<int32 N, typename T>
    MINT_INLINE const T& VectorR<N, T>::z() const noexcept
    {
        static_assert(N >= 3, "Vector dimension is not enough!");
        return _c[2];
    }

    template<int32 N, typename T>
    MINT_INLINE const T& VectorR<N, T>::w() const noexcept
    {
        static_assert(N >= 4, "Vector dimension is not enough!");
        return _c[3];
    }

    template<int32 N, typename T>
    MINT_INLINE T VectorR<N, T>::maxComponent() const noexcept
    {
        T result = 1.0e-323;
        for (int32 index = 0; index < N; ++index)
        {
            if (result < _c[index])
            {
                result = _c[index];
            }
        }
        return result;
    }

    template<int32 N, typename T>
    MINT_INLINE T VectorR<N, T>::minComponent() const noexcept
    {
        T result = 1.0e+308;
        for (int32 index = 0; index < N; ++index)
        {
            if (_c[index] < result)
            {
                result = _c[index];
            }
        }
        return result;
    }
        
    template<int32 N, typename T>
    MINT_INLINE T VectorR<N, T>::normSquared() const noexcept
    {
        return Math::normSq(_c);
    }

    template<int32 N, typename T>
    MINT_INLINE T VectorR<N, T>::norm() const noexcept
    {
        return Math::norm(_c);
    }
        
    template<int32 N, typename T>
    MINT_INLINE VectorR<N, T>& VectorR<N, T>::setNormalized() noexcept
    {
        const T norm_ = norm();
        if (norm_ != 0)
        {
            *this /= norm_;
        }
        return *this;
    }

    template<int32 N, typename T>
    MINT_INLINE VectorR<N, T> VectorR<N, T>::normalize() const noexcept
    {
        VectorR<N, T> result = *this;
        return result.setNormalized();
    }

    template<int32 N, typename T>
    MINT_INLINE bool VectorR<N, T>::isUnitVector() const noexcept
    {
        return Math::equals(normSquared(), static_cast<T>(1.0));
    }

    template<int32 N, typename T>
    MINT_INLINE T VectorR<N, T>::dot(const VectorR& rhs) const noexcept
    {
        return dot(*this, rhs);
    }

    template<int32 N, typename T>
    MINT_INLINE T VectorR<N, T>::distance(const VectorR& rhs) const noexcept
    {
        return distance(*this, rhs);
    }

    template<int32 N, typename T>
    MINT_INLINE T VectorR<N, T>::angle(const VectorR& rhs) const noexcept
    {
        return angle(*this, rhs);
    }

    template<int32 N, typename T>
    MINT_INLINE bool VectorR<N, T>::isOrthogonalTo(const VectorR& rhs) const noexcept
    {
        return isOrthogonal(*this, rhs);
    }

    template<int32 N, typename T>
    MINT_INLINE VectorR<N, T> VectorR<N, T>::projectOnto(const VectorR& rhs) const noexcept
    {
        return projectUOntoV(*this, rhs);
    }

    template<int32 N, typename T>
    MINT_INLINE VectorR<N - 1, T> VectorR<N, T>::shrink() const noexcept
    {
        MINT_ASSERT(N > 1, "N must be greater than 1!!!");
            
        VectorR<N - 1, T> result;
        for (uint32 i = 0; i < N - 1; ++i)
        {
            result.setComponent(0, _c[i]);
        }
        return result;
    }


    template<typename T>
    VectorR<3, T> cross(const VectorR<3, T>& lhs, const VectorR<3, T>& rhs) noexcept
    {
        return VectorR<3, T>
        (
            {
                lhs.getComponent(1) * rhs.getComponent(2) - lhs.getComponent(2) * rhs.getComponent(1),
                lhs.getComponent(2) * rhs.getComponent(0) - lhs.getComponent(0) * rhs.getComponent(2),
                lhs.getComponent(0) * rhs.getComponent(1) - lhs.getComponent(1) * rhs.getComponent(0)
            }
        );
    }

    template<int32 N, typename T>
    MINT_INLINE VectorR<N, T> operator*(const T scalar, const VectorR<N, T>& vector) noexcept
    {
        return (vector * scalar);
    }


    namespace VectorUtils
    {
        template<int32 N, typename T>
        void setNan(VectorR<N, T>& in) noexcept
        {
            for (uint32 i = 0; i < N; ++i)
            {
                in.setComponent(i, Math::nan());
            }
        }

        template<int32 N, typename T>
        bool isNan(const VectorR<N, T>& in) noexcept
        {
            for (uint32 i = 0; i < N; ++i)
            {
                if (Math::isNan(in.getComponent(i)))
                {
                    return true;
                }
            }
            return false;
        }
    }
}
