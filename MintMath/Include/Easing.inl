#pragma once


namespace mint
{
	namespace Math
	{
		MINT_INLINE float EaseInQuad(const float x) noexcept
		{
			return x * x;
		}

		MINT_INLINE float EaseOutQuad(const float x) noexcept
		{
			const float term = (x - 1.0f);
			return 1.0f - (term * term);
		}

		MINT_INLINE float EaseInOutQuad(const float x) noexcept
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

		MINT_INLINE float Easing::Ease(const Method method, float x) noexcept
		{
			x = Math::Clamp(x, 0.0f, 1.0f);

			switch (method)
			{
			case Easing::Method::InQuad:
				return EaseInQuad(x);
			case Easing::Method::OutQuad:
				return EaseOutQuad(x);
			case Easing::Method::InOutQuad:
				return EaseInOutQuad(x);
			default:
				break;
			}

			return x;
		}
	}
}
