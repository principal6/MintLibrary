#pragma once


#include <MintPhysics/Include/Intersection.h>
#include <MintMath/Include/Rect.h>


namespace mint
{
	namespace Physics
	{
#pragma region Intersection Free Functions
		MINT_INLINE bool Intersect2D_Circle_Point(const Float2& circleCenter, const float circleRadius, const Float2& point)
		{
			const float distanceSq = (point - circleCenter).LengthSqaure();
			const float circleRadiusSq = circleRadius * circleRadius;
			return distanceSq <= circleRadiusSq;
		}

		MINT_INLINE bool Intersect2D_AABB_Point(const Rect& aabb, const Float2& point)
		{
			return aabb.Contains(point);
		}
#pragma endregion
	}
}
