#include <MintMath/Include/MathCommon.h>

#include <cfloat>


namespace mint
{
	namespace Math
	{
		bool IsNAN(const float value) noexcept
		{
			return ::_isnan(value) != 0;
		}
	}
}
