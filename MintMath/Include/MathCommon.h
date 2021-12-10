#pragma once


#ifndef MINT_MATH_COMMON_H
#define MINT_MATH_COMMON_H


#include <cmath>

#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
    enum class Axis
    {
        X,
        Y,
        Z
    };


    namespace Math
    {
        static constexpr float kPi              = 3.1415926535f;
        static constexpr float kPiOverTwo       = kPi * 0.5f;
        static constexpr float kPiOverFour      = kPiOverTwo * 0.5f;
        static constexpr float kPiOverEight     = kPiOverFour * 0.5f;
        static constexpr float kTwoPi           = kPi * 2.0f;
        static constexpr float kSqrtOfTwo       = 1.4142135624f;
        static constexpr float kFloatMax        = 3.402823466e+38f;
        static constexpr float kFloatEpsilon    = 0.0000152587890625f;
        static constexpr double kDoubleEpsilon  = 0.0000152587890625;

        MINT_INLINE constexpr uint32 pow2_ui32(const uint32 exponent)
        {
            return static_cast<uint32>(1 << exponent);
        }

        template<typename T>
        MINT_INLINE constexpr const T& clamp(const T& value, const T& limitMin, const T& limitMax)
        {
            return max(min(value, limitMax), limitMin);
        }

        template<typename T>
        MINT_INLINE constexpr const T& clamp(const T& value, T&& limitMin, T&& limitMax)
        {
            return max(min(value, limitMax), limitMin);
        }

        MINT_INLINE constexpr const float saturate(const float value)
        {
            return clamp(value, 0.0f, 1.0f);
        }

        MINT_INLINE constexpr const double saturate(const double value)
        {
            return clamp(value, 0.0, 1.0);
        }

        MINT_INLINE constexpr const float nan() noexcept
        {
            return static_cast<float>(1e+300 * 1e+300) * 0.0f;
        }

        const bool isNan(const float value) noexcept;

        MINT_INLINE const float toRadian(const float degree) noexcept
        {
            return (degree / 360.0f * kTwoPi);
        }

        MINT_INLINE const float toDegree(const float radian) noexcept
        {
            return (radian / kTwoPi * 360.0f);
        }

        MINT_INLINE const float limitAngleToPositiveTwoPi(const float radian) noexcept
        {
            float result = radian;
            while (+kTwoPi < result)
            {
                result -= kTwoPi;
            }
            return result;
        }

        MINT_INLINE const float limitAngleToNegativeTwoPi(const float radian) noexcept
        {
            float result = radian;
            while (result < -kTwoPi)
            {
                result += kTwoPi;
            }
            return result;
        }

        MINT_INLINE const float limitAngleToPositiveNegativeTwoPiRotation(const float radian) noexcept
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

        MINT_INLINE const bool equals(const float a, const float b, const float epsilon = kFloatEpsilon)
        {
            return (::abs(a - b) < epsilon);
        }

        MINT_INLINE const bool equals(const double a, const double b, const double epsilon = kDoubleEpsilon)
        {
            return (::abs(a - b) < epsilon);
        }
    }
}


#endif // !MINT_MATH_COMMON_H
