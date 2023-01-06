#pragma once


#include <MintPhysics/Include/Intersection.h>
#include <MintMath/Include/Float2.h>
#include <MintMath/Include/Rect.h>


namespace mint
{
	namespace Physics
	{
		MINT_INLINE bool intersect2D_circle_point(const Float2& circleCenter, const float circleRadius, const Float2& point)
		{
			const float distanceSq = (point - circleCenter).lengthSqaure();
			const float circleRadiusSq = circleRadius * circleRadius;
			return distanceSq <= circleRadiusSq;
		}

		MINT_INLINE bool intersect2D_aabb_point(const Rect& aabb, const Float2& point)
		{
			return aabb.contains(point);
		}
	}
}
