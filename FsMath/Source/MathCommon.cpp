#include <FsMath/Include/MathCommon.h>

#include <cmath>


namespace fs
{
    namespace Math
    {
        const bool isNan(const float value) noexcept
        {
            return ::_isnan(value) != 0;
        }
    }
}
