#pragma once


#ifndef FS_MATH_COMMON_H
#define FS_MATH_COMMON_H


#include <cmath>


namespace fs
{
	namespace Math
	{
		static constexpr float kPi = 3.1415926535f;
		static constexpr float kPiOverTwo = kPi * 0.5f;
		static constexpr float kPiOverFour = kPiOverTwo * 0.5f;

		constexpr uint32 pow2_ui32(const uint32 exponent)
		{
			return static_cast<uint32>(1 << exponent);
		}

		template<typename T>
		constexpr T clamp(const T value, const T min_, const T max_)
		{
			return max(min(value, max_), min_);
		}
	}
}


#endif // !FS_MATH_COMMON_H
