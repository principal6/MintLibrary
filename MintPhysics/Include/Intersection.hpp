#pragma once


#include <MintPhysics/Include/Intersection.h>
#include <MintMath/Include/Rect.h>
#include <MintContainer/Include/Vector.hpp>


namespace mint
{
	namespace Physics
	{
#pragma region Shape2D - PointShape2D
		inline PointShape2D::PointShape2D(const Float2& center)
			: Shape2D(center)
		{
			__noop;
		}
#pragma endregion

#pragma region Shape2D - CircleShape2D
		inline CircleShape2D::CircleShape2D(const Float2& center, const float radius)
			: Shape2D(center)
			, _radius{ radius }
		{
			MINT_ASSERT(radius > 0.0f, "radius(%f) 가 0 이하입니다. 의도한 게 맞나요?", radius);
		}
#pragma endregion

#pragma region Shape2D - AABBShape2D
		inline AABBShape2D::AABBShape2D(const Float2& center, const Float2& halfSize)
			: Shape2D(center)
			, _halfSize{ halfSize }
		{
			__noop;
		}
#pragma endregion

#pragma region Shape2D - ConvexShape2D
		inline ConvexShape2D::ConvexShape2D(const Float2& center, const Vector<Float2>& vertices)
			: Shape2D(center)
			, _vertices{ vertices }
		{
			__noop;
		}
		inline ConvexShape2D::ConvexShape2D()
			: Shape2D(Float2::kZero)
		{
			__noop;
		}
#pragma endregion


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
