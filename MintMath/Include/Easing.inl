#pragma once


namespace mint
{
    namespace Math
    {
        MINT_INLINE float easeInQuad(const float x) noexcept
        {
            return x * x;
        }

        MINT_INLINE float easeOutQuad(const float x) noexcept
        {
            const float term = (x - 1.0f);
            return 1.0f - (term * term);
        }

        MINT_INLINE float easeInOutQuad(const float x) noexcept
        {
            if (x < 0.5f)
            {
                return 2.0f * x * x;
            }
            else
            {
                const float term = (x - 1.0f);
                return 1.0f - 2.0f * (term * term);
            }
        }

        MINT_INLINE float Easing::ease(const Method method, float x) noexcept
        {
            x = Math::clamp(x, 0.0f, 1.0f);

            switch (method)
            {
            case Easing::Method::InQuad:
                return easeInQuad(x);
            case Easing::Method::OutQuad:
                return easeOutQuad(x);
            case Easing::Method::InOutQuad:
                return easeInOutQuad(x);
            default:
                break;
            }

            return x;
        }
    }
}
