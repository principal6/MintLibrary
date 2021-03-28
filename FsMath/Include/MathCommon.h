#pragma once


#ifndef FS_MATH_COMMON_H
#define FS_MATH_COMMON_H


#include <CommonDefinitions.h>
#include <cmath>


namespace fs
{
    namespace Math
    {
        static constexpr float kPi              = 3.1415926535f;
        static constexpr float kPiOverTwo       = kPi * 0.5f;
        static constexpr float kPiOverFour      = kPiOverTwo * 0.5f;
        static constexpr float kPiOverEight     = kPiOverFour * 0.5f;
        static constexpr float kTwoPi           = kPi * 2.0f;
        static constexpr float kSqrtOfTwo       = 1.4142135624f;


        FS_INLINE constexpr uint32 pow2_ui32(const uint32 exponent)
        {
            return static_cast<uint32>(1 << exponent);
        }

        template<typename T>
        FS_INLINE constexpr const T& clamp(const T& value, const T& limitMin, const T& limitMax)
        {
            return max(min(value, limitMax), limitMin);
        }

        template<typename T>
        FS_INLINE constexpr const T& clamp(const T& value, T&& limitMin, T&& limitMax)
        {
            return max(min(value, limitMax), limitMin);
        }

        FS_INLINE constexpr const float saturate(const float value)
        {
            return clamp(value, 0.0f, 1.0f);
        }

        FS_INLINE constexpr const double saturate(const double value)
        {
            return clamp(value, 0.0, 1.0);
        }

        FS_INLINE constexpr const float nan() noexcept
        {
            return static_cast<float>(1e+300 * 1e+300) * 0.0f;
        }

        FS_INLINE const bool isNan(const float value) noexcept
        {
            return (value != value);
        }

        FS_INLINE const float toRadian(const float degree) noexcept
        {
            return (degree / 360.0f * kTwoPi);
        }

        FS_INLINE const float toDegree(const float radian) noexcept
        {
            return (radian / kTwoPi * 360.0f);
        }

        FS_INLINE const float limitAngleToPositiveTwoPi(const float radian) noexcept
        {
            float result = radian;
            while (+kTwoPi < result)
            {
                result -= kTwoPi;
            }
            return result;
        }

        FS_INLINE const float limitAngleToNegativeTwoPi(const float radian) noexcept
        {
            float result = radian;
            while (result < -kTwoPi)
            {
                result += kTwoPi;
            }
            return result;
        }

        FS_INLINE const float limitAngleToPositiveNegativeTwoPiRotation(const float radian) noexcept
        {
            if (+kTwoPi < radian)
            {
                return limitAngleToPositiveTwoPi(radian);
            }
            else if (radian < -kTwoPi)
            {
                return limitAngleToNegativeTwoPi(radian);
            }
            return radian;
        }

    }
}


#endif // !FS_MATH_COMMON_H
