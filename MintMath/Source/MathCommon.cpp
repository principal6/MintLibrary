#include <MintMath/Include/MathCommon.h>

#include <cfloat>


namespace mint
{
    namespace Math
    {
        bool isNan(const float value) noexcept
        {
            return ::_isnan(value) != 0;
        }
    }
}
