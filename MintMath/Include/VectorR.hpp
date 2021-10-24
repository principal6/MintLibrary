#pragma once


#include <MintMath/Include/VectorR.h>

#include <MintMath/Include/MathCommon.h>

#include <MintMath/Include/Matrix.hpp>


namespace mint
{
    namespace Math
    {
        template<int32 N>
        inline VectorR<N>::VectorR()
            : _c{}
        {
            __noop;
        }

        template<int32 N>
        template <class ...Args>
        inline VectorR<N>::VectorR(Args... args)
            : _c{ args... }
        {
            __noop;
        }

        template<int32 N>
        inline VectorR<N>::VectorR(const std::initializer_list<double>& initializerList)
        {
            const int32 count = mint::min(static_cast<int32>(initializerList.size()), N);
            const double* const first = initializerList.begin();
            for (int32 index = 0; index < count; ++index)
            {
                _c[index] = *(first + index);
            }
        }

        template<int32 N>
        inline VectorR<N>::~VectorR()
        {
            __noop;
        }

        template<int32 N>
        inline VectorR<N> VectorR<N>::standardUnitVector(const int32 math_i) noexcept
        {
            VectorR<N> result;
            result.setComponent(math_i - 1, 1.0);
            return result;
        }

        template<int32 N>
        MINT_INLINE const double VectorR<N>::dot(const VectorR& lhs, const VectorR& rhs) noexcept
        {
            double result = 0.0;
            for (int32 index = 0; index < N; ++index)
            {
                result += (lhs._c[index] * rhs._c[index]);
            }
            return result;
        }

        template<int32 N>
        MINT_INLINE VectorR<3> VectorR<N>::cross(const VectorR<3>& lhs, const VectorR<3>& rhs) noexcept
        {
            return VectorR<3>
                (
                    {
                        lhs._c[1] * rhs._c[2] - lhs._c[2] * rhs._c[1],
                        lhs._c[2] * rhs._c[0] - lhs._c[0] * rhs._c[2],
                        lhs._c[0] * rhs._c[1] - lhs._c[1] * rhs._c[0]
                    }
            );
        }

        template<int32 N>
        MINT_INLINE const double VectorR<N>::distance(const VectorR& lhs, const VectorR& rhs) noexcept
        {
            return (rhs - lhs).norm();
        }

        template<int32 N>
        MINT_INLINE const double VectorR<N>::angle(const VectorR& lhs, const VectorR& rhs) noexcept
        {
            return ::acos(lhs.normalize().dot(rhs.normalize()));
        }

        template<int32 N>
        MINT_INLINE const bool VectorR<N>::isOrthogonal(const VectorR& lhs, const VectorR& rhs) noexcept
        {
            return mint::Math::equals(lhs.dot(rhs), 0.0);
        }

        template<int32 N>
        MINT_INLINE VectorR<N> VectorR<N>::projectUOntoV(const VectorR& u, const VectorR& v) noexcept
        {
            return (u.dot(v) / u.dot(u)) * u;
        }

        template<int32 N>
        MINT_INLINE VectorR<N>& VectorR<N>::operator*=(const double scalar) noexcept
        {
            for (int32 index = 0; index < N; ++index)
            {
                _c[index] *= scalar;
            }
            return *this;
        }

        template<int32 N>
        MINT_INLINE VectorR<N>& VectorR<N>::operator/=(const double scalar) noexcept
        {
            MINT_ASSERT("김장원", scalar != 0.0, "0 으로 나누려 합니다!");

            for (int32 index = 0; index < N; ++index)
            {
                _c[index] /= scalar;
            }
            return *this;
        }

        template<int32 N>
        MINT_INLINE VectorR<N>& VectorR<N>::operator+=(const VectorR& rhs) noexcept
        {
            for (int32 index = 0; index < N; ++index)
            {
                _c[index] += rhs._c[index];
            }
            return *this;
        }

        template<int32 N>
        MINT_INLINE VectorR<N>& VectorR<N>::operator-=(const VectorR& rhs) noexcept
        {
            for (int32 index = 0; index < N; ++index)
            {
                _c[index] -= rhs._c[index];
            }
            return *this;
        }

        template<int32 N>
        MINT_INLINE VectorR<N> VectorR<N>::operator*(const double scalar) const noexcept
        {
            VectorR result = *this;
            result *= scalar;
            return result;
        }

        template<int32 N>
        MINT_INLINE VectorR<N> VectorR<N>::operator/(const double scalar) const noexcept
        {
            VectorR result = *this;
            result /= scalar;
            return result;
        }

        template<int32 N>
        MINT_INLINE VectorR<N> VectorR<N>::operator+(const VectorR& rhs) const noexcept
        {
            VectorR result = *this;
            result += rhs;
            return result;
        }

        template<int32 N>
        MINT_INLINE VectorR<N> VectorR<N>::operator-(const VectorR& rhs) const noexcept
        {
            VectorR result = *this;
            result -= rhs;
            return result;
        }

        template<int32 N>
        MINT_INLINE const VectorR<N>& VectorR<N>::operator+() const noexcept
        {
            return *this;
        }

        template<int32 N>
        MINT_INLINE VectorR<N> VectorR<N>::operator-() const noexcept
        {
            VectorR<N> result;
            for (int32 i = 0; i < N; ++i)
            {
                result._c[i] = -_c[i];
            }
            return result;
        }

        template<int32 N>
        MINT_INLINE double& VectorR<N>::operator[](const uint32 index) noexcept
        {
            MINT_ASSERT("김장원", index < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");
            return _c[index];
        }

        template<int32 N>
        MINT_INLINE const double& VectorR<N>::operator[](const uint32 index) const noexcept
        {
            MINT_ASSERT("김장원", index < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");
            return _c[index];
        }

        template<int32 N>
        MINT_INLINE const bool VectorR<N>::operator==(const VectorR& rhs) const noexcept
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

        template<int32 N>
        MINT_INLINE const bool VectorR<N>::operator!=(const VectorR& rhs) const noexcept
        {
            return !(rhs == *this);
        }

        template<int32 N>
        MINT_INLINE void VectorR<N>::setZero() noexcept
        {
            for (int32 index = 0; index < N; ++index)
            {
                _c[index] = 0.0;
            }
        }

        template<int32 N>
        MINT_INLINE void mint::Math::VectorR<N>::setComponent(const uint32 index, const double value) noexcept
        {
            if (index < static_cast<uint32>(N))
            {
                _c[index] = value;
            }
        }
        
        template<int32 N>
        MINT_INLINE const double VectorR<N>::getComponent(const uint32 index) const noexcept
        {
            MINT_ASSERT("김장원", index < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");
            return _c[index];
        }

        template<int32 N>
        MINT_INLINE const double VectorR<N>::maxComponent() const noexcept
        {
            double result = 1.0e-323;
            for (int32 index = 0; index < N; ++index)
            {
                if (result < _c[index])
                {
                    result = _c[index];
                }
            }
            return result;
        }

        template<int32 N>
        MINT_INLINE const double VectorR<N>::minComponent() const noexcept
        {
            double result = 1.0e+308;
            for (int32 index = 0; index < N; ++index)
            {
                if (_c[index] < result)
                {
                    result = _c[index];
                }
            }
            return result;
        }
        
        template<int32 N>
        MINT_INLINE const double VectorR<N>::normSquared() const noexcept
        {
            double result = 0.0;
            for (int32 index = 0; index < N; ++index)
            {
                result += (_c[index] * _c[index]);
            }
            return result;
        }

        template<int32 N>
        MINT_INLINE const double VectorR<N>::norm() const noexcept
        {
            return ::sqrt(normSquared());
        }
        
        template<int32 N>
        MINT_INLINE VectorR<N>& VectorR<N>::setNormalized() noexcept
        {
            const double norm_ = norm();
            *this /= norm_;
            return *this;
        }

        template<int32 N>
        MINT_INLINE VectorR<N> VectorR<N>::normalize() const noexcept
        {
            VectorR<N> result = *this;
            return result.setNormalized();
        }

        template<int32 N>
        MINT_INLINE const bool VectorR<N>::isUnitVector() const noexcept
        {
            return mint::Math::equals(normSquared(), 1.0);
        }

        template<int32 N>
        MINT_INLINE const double VectorR<N>::dot(const VectorR& rhs) const noexcept
        {
            return dot(*this, rhs);
        }

        template<>
        inline VectorR<3> VectorR<3>::cross(const VectorR<3>& rhs) const noexcept
        {
            return cross(*this, rhs);
        }

        template<int32 N>
        MINT_INLINE const double VectorR<N>::distance(const VectorR& rhs) const noexcept
        {
            return distance(*this, rhs);
        }

        template<int32 N>
        MINT_INLINE const double VectorR<N>::angle(const VectorR& rhs) const noexcept
        {
            return angle(*this, rhs);
        }

        template<int32 N>
        MINT_INLINE const bool VectorR<N>::isOrthogonalTo(const VectorR& rhs) const noexcept
        {
            return isOrthogonal(*this, rhs);
        }

        template<int32 N>
        MINT_INLINE VectorR<N> VectorR<N>::projectOnto(const VectorR& rhs) const noexcept
        {
            return projectUOntoV(*this, rhs);
        }


        template<int32 N>
        MINT_INLINE VectorR<N> operator*(const double scalar, const VectorR<N>& vector) noexcept
        {
            return (vector * scalar);
        }
    }
}
