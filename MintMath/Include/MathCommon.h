#pragma once


#ifndef _MINT_MATH_COMMON_H_
#define _MINT_MATH_COMMON_H_


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
		static constexpr float kPi = 3.1415926535f;
		static constexpr float kPiOverTwo = kPi * 0.5f;
		static constexpr float kPiOverFour = kPiOverTwo * 0.5f;
		static constexpr float kPiOverEight = kPiOverFour * 0.5f;
		static constexpr float kTwoPi = kPi * 2.0f;
		static constexpr float kSqrtOfTwo = 1.4142135624f;
		static constexpr float kFloatMax = 3.402823466e+38f;
		static constexpr float kFloatEpsilon = 0.0000152587890625f;
		static constexpr double kDoubleEpsilon = 0.0000152587890625;

		MINT_INLINE constexpr uint32 Pow2_Uint32(const uint32 exponent)
		{
			return static_cast<uint32>(1 << exponent);
		}

		template<typename T>
		MINT_INLINE constexpr const T& Clamp(const T& value, const T& limitMin, const T& limitMax)
		{
			return Max(Min(value, limitMax), limitMin);
		}

		template<typename T>
		MINT_INLINE constexpr const T& Clamp(const T& value, T&& limitMin, T&& limitMax)
		{
			return Max(Min(value, limitMax), limitMin);
		}

		MINT_INLINE constexpr float Saturate(const float value)
		{
			return Clamp(value, 0.0f, 1.0f);
		}

		MINT_INLINE constexpr double Saturate(const double value)
		{
			return Clamp(value, 0.0, 1.0);
		}

		MINT_INLINE constexpr float getNAN() noexcept
		{
			return static_cast<float>(1e+300 * 1e+300) * 0.0f;
		}

		bool IsNAN(const float value) noexcept;

		MINT_INLINE float ToRadian(const float degree) noexcept
		{
			return (degree / 360.0f * kTwoPi);
		}

		MINT_INLINE float ToDegree(const float radian) noexcept
		{
			return (radian / kTwoPi * 360.0f);
		}

		MINT_INLINE float LimitAngleToPositiveTwoPi(const float radian) noexcept
		{
			float result = radian;
			while (+kTwoPi < result)
			{
				result -= kTwoPi;
			}
			return result;
		}

		MINT_INLINE float LimitAngleToNegativeTwoPi(const float radian) noexcept
		{
			float result = radian;
			while (result < -kTwoPi)
			{
				result += kTwoPi;
			}
			return result;
		}

		MINT_INLINE float LimitAngleToPositiveNegativeTwoPiRotation(const float radian) noexcept
		{
			if (+kTwoPi < radian)
			{
				return LimitAngleToPositiveTwoPi(radian);
			}
			else if (radian < -kTwoPi)
			{
				return LimitAngleToNegativeTwoPi(radian);
			}
			return radian;
		}

		MINT_INLINE bool Equals(const float a, const float b, const float epsilon = kFloatEpsilon)
		{
			return (::abs(a - b) < epsilon);
		}

		MINT_INLINE bool Equals(const double a, const double b, const double epsilon = kDoubleEpsilon)
		{
			return (::abs(a - b) < epsilon);
		}

		template<typename T>
		MINT_INLINE T Lerp(const T& a, const T& b, const float t) noexcept
		{
			return a * (1.0f - t) + b * t;
		}
	}
}


#endif // !_MINT_MATH_COMMON_H_
