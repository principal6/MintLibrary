#pragma once


#ifndef FS_MATH_COMMON_H
#define FS_MATH_COMMON_H


#include <cmath>


namespace fs
{
	namespace Math
	{
		inline uint32 pow2_ui32(const uint32 exponent)
		{
			return static_cast<uint32>(1 << exponent);
		}
	}
}


#endif // !FS_MATH_COMMON_H
