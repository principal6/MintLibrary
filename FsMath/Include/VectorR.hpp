#pragma once


#include <FsMath/Include/VectorR.h>


namespace fs
{
    namespace Math
    {
        template<uint8 N>
        inline VectorR<N>::VectorR()
            : _c{}
        {
            __noop;
        }

        template<uint8 N>
        inline VectorR<N>::VectorR(const std::initializer_list<double>& initializerList)
        {
            const uint8 count = fs::min(static_cast<uint8>(initializerList.size()), N);
            const double* const first = initializerList.begin();
            for (uint8 index = 0; index < count; ++index)
            {
                _c[index] = *(first + index);
            }
        }

        template<uint8 N>
        inline VectorR<N>::~VectorR()
        {
            __noop;
        }

        template<uint8 N>
        FS_INLINE const double VectorR<N>::dot(const VectorR& lhs, const VectorR& rhs) noexcept
        {
            double result = 0.0;
            for (uint8 index = 0; index < N; ++index)
            {
                result += (lhs._c[index] * rhs._c[index]);
            }
            return result;
        }

        template<uint8 N>
        FS_INLINE VectorR<N>& VectorR<N>::operator*=(const double scalar) noexcept
        {
            for (uint8 index = 0; index < N; ++index)
            {
                _c[index] *= scalar;
            }
            return *this;
        }

        template<uint8 N>
        FS_INLINE VectorR<N>& VectorR<N>::operator/=(const double scalar) noexcept
        {
            FS_ASSERT("김장원", scalar != 0.0, "0 으로 나누려 합니다!");

            for (uint8 index = 0; index < N; ++index)
            {
                _c[index] /= scalar;
            }
            return *this;
        }

        template<uint8 N>
        FS_INLINE VectorR<N>& VectorR<N>::operator+=(const VectorR& rhs) noexcept
        {
            for (uint8 index = 0; index < N; ++index)
            {
                _c[index] += rhs._c[index];
            }
            return *this;
        }

        template<uint8 N>
        FS_INLINE VectorR<N>& VectorR<N>::operator-=(const VectorR& rhs) noexcept
        {
            for (uint8 index = 0; index < N; ++index)
            {
                _c[index] -= rhs._c[index];
            }
            return *this;
        }

        template<uint8 N>
        FS_INLINE VectorR<N> VectorR<N>::operator*(const double scalar) const noexcept
        {
            VectorR result = *this;
            result *= scalar;
            return result;
        }

        template<uint8 N>
        FS_INLINE VectorR<N> VectorR<N>::operator/(const double scalar) const noexcept
        {
            VectorR result = *this;
            result /= scalar;
            return result;
        }

        template<uint8 N>
        FS_INLINE VectorR<N> VectorR<N>::operator+(const VectorR& rhs) const noexcept
        {
            VectorR result = *this;
            result += rhs;
            return result;
        }

        template<uint8 N>
        FS_INLINE VectorR<N> VectorR<N>::operator-(const VectorR& rhs) const noexcept
        {
            VectorR result = *this;
            result -= rhs;
            return result;
        }

        template<uint8 N>
        FS_INLINE double& VectorR<N>::operator[](const uint8 index) noexcept
        {
            FS_ASSERT("김장원", index < N, "범위를 벗어난 접근입니다!");
            return _c[index];
        }

        template<uint8 N>
        FS_INLINE const double& VectorR<N>::operator[](const uint8 index) const noexcept
        {
            FS_ASSERT("김장원", index < N, "범위를 벗어난 접근입니다!");
            return _c[index];
        }

        template<uint8 N>
        FS_INLINE void VectorR<N>::setZero() noexcept
        {
            for (uint8 index = 0; index < N; ++index)
            {
                _c[index] = 0.0;
            }
        }

        template<uint8 N>
        FS_INLINE void fs::Math::VectorR<N>::setComponent(const uint8 index, const double value) noexcept
        {
            if (index < N)
            {
                _c[index] = value;
            }
        }
        
        template<uint8 N>
        FS_INLINE const double VectorR<N>::getComponent(const uint8 index) const noexcept
        {
            FS_ASSERT("김장원", index < N, "범위를 벗어난 접근입니다!");
            return _c[index];
        }

        template<uint8 N>
        FS_INLINE const double VectorR<N>::maxComponent() const noexcept
        {
            double result = 1.0e-323;
            for (uint8 index = 0; index < N; ++index)
            {
                if (result < _c[index])
                {
                    result = _c[index];
                }
            }
            return result;
        }

        template<uint8 N>
        FS_INLINE const double VectorR<N>::minComponent() const noexcept
        {
            double result = 1.0e+308;
            for (uint8 index = 0; index < N; ++index)
            {
                if (_c[index] < result)
                {
                    result = _c[index];
                }
            }
            return result;
        }
        
        template<uint8 N>
        FS_INLINE const double VectorR<N>::norm() const noexcept
        {
            double result = 0.0;
            for (uint8 i = 0; i < N; ++i)
            {
                result += (_c[i] * _c[i]);
            }
            return result;
        }
        
        template<uint8 N>
        FS_INLINE void VectorR<N>::normalize() noexcept
        {
            const double norm = norm();
            *this /= norm;
        }

        template<uint8 N>
        FS_INLINE const double VectorR<N>::dot(const VectorR& rhs) const noexcept
        {
            return dot(*this, rhs);
        }
    }
}
