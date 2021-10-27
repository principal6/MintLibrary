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
        inline VectorR<N, T>::VectorR()
            : _c{}
        {
            __noop;
        }

        template<int32 N, typename T>
        template <class ...Args>
        inline VectorR<N, T>::VectorR(Args... args)
            : _c{ args... }
        {
            __noop;
        }

        template<int32 N, typename T>
        inline VectorR<N, T>::VectorR(const std::initializer_list<T>& initializerList)
        {
            const int32 count = min(static_cast<int32>(initializerList.size()), N);
            const T* const first = initializerList.begin();
            for (int32 index = 0; index < count; ++index)
            {
                _c[index] = *(first + index);
            }
        }

        template<int32 N, typename T>
        inline VectorR<N, T>::~VectorR()
        {
            __noop;
        }

        template<int32 N, typename T>
        inline VectorR<N, T> VectorR<N, T>::standardUnitVector(const int32 math_i) noexcept
        {
            VectorR<N, T> result;
            result.setComponent(math_i - 1, 1.0);
            return result;
        }

        template<int32 N, typename T>
        MINT_INLINE const T VectorR<N, T>::dot(const VectorR& lhs, const VectorR& rhs) noexcept
        {
            T result = 0.0;
            for (int32 index = 0; index < N; ++index)
            {
                result += (lhs._c[index] * rhs._c[index]);
            }
            return result;
        }

        template<int32 N, typename T>
        MINT_INLINE const T VectorR<N, T>::distance(const VectorR& lhs, const VectorR& rhs) noexcept
        {
            return (rhs - lhs).norm();
        }

        template<int32 N, typename T>
        MINT_INLINE const T VectorR<N, T>::angle(const VectorR& lhs, const VectorR& rhs) noexcept
        {
            return ::acos(lhs.normalize().dot(rhs.normalize()));
        }

        template<int32 N, typename T>
        MINT_INLINE const bool VectorR<N, T>::isOrthogonal(const VectorR& lhs, const VectorR& rhs) noexcept
        {
            return equals(lhs.dot(rhs), 0.0);
        }

        template<int32 N, typename T>
        MINT_INLINE VectorR<N, T> VectorR<N, T>::projectUOntoV(const VectorR& u, const VectorR& v) noexcept
        {
            return (u.dot(v) / u.dot(u)) * u;
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
            MINT_ASSERT("김장원", scalar != 0.0, "0 으로 나누려 합니다!");

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
            MINT_ASSERT("김장원", index < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");
            return _c[index];
        }

        template<int32 N, typename T>
        MINT_INLINE const T& VectorR<N, T>::operator[](const uint32 index) const noexcept
        {
            MINT_ASSERT("김장원", index < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");
            return _c[index];
        }

        template<int32 N, typename T>
        MINT_INLINE const bool VectorR<N, T>::operator==(const VectorR& rhs) const noexcept
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
        MINT_INLINE const bool VectorR<N, T>::operator!=(const VectorR& rhs) const noexcept
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
        MINT_INLINE const T VectorR<N, T>::getComponent(const uint32 index) const noexcept
        {
            MINT_ASSERT("김장원", index < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");
            return _c[index];
        }

        template<int32 N, typename T>
        MINT_INLINE const T VectorR<N, T>::maxComponent() const noexcept
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
        MINT_INLINE const T VectorR<N, T>::minComponent() const noexcept
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
        MINT_INLINE const T VectorR<N, T>::normSquared() const noexcept
        {
            return dot(*this);
        }

        template<int32 N, typename T>
        MINT_INLINE const T VectorR<N, T>::norm() const noexcept
        {
            return ::sqrt(normSquared());
        }
        
        template<int32 N, typename T>
        MINT_INLINE VectorR<N, T>& VectorR<N, T>::setNormalized() noexcept
        {
            const T norm_ = norm();
            *this /= norm_;
            return *this;
        }

        template<int32 N, typename T>
        MINT_INLINE VectorR<N, T> VectorR<N, T>::normalize() const noexcept
        {
            VectorR<N, T> result = *this;
            return result.setNormalized();
        }

        template<int32 N, typename T>
        MINT_INLINE const bool VectorR<N, T>::isUnitVector() const noexcept
        {
            return equals(normSquared(), 1.0);
        }

        template<int32 N, typename T>
        MINT_INLINE const T VectorR<N, T>::dot(const VectorR& rhs) const noexcept
        {
            return dot(*this, rhs);
        }

        template<int32 N, typename T>
        MINT_INLINE const T VectorR<N, T>::distance(const VectorR& rhs) const noexcept
        {
            return distance(*this, rhs);
        }

        template<int32 N, typename T>
        MINT_INLINE const T VectorR<N, T>::angle(const VectorR& rhs) const noexcept
        {
            return angle(*this, rhs);
        }

        template<int32 N, typename T>
        MINT_INLINE const bool VectorR<N, T>::isOrthogonalTo(const VectorR& rhs) const noexcept
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
            MINT_ASSERT("김장원", 1 < N, "N must be greater than 1!!!");
            
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
            const bool isNan(const VectorR<N, T>& in) noexcept
            {
                for (uint32 i = 0; i < N; ++i)
                {
                    if (true == Math::isNan(in.getComponent(i)))
                    {
                        return true;
                    }
                }
                return false;
            }
        }
    }
}
