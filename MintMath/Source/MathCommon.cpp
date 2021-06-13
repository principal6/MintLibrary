#include <MintMath/Include/MathCommon.h>

#include <cmath>
#include <cfloat>


namespace mint
{
    namespace Math
    {
        const bool isNan(const float value) noexcept
        {
            return ::_isnan(value) != 0;
        }
    }
}
