#pragma once


#include <MintPhysics/Include/Intersection.h>
#include <MintMath/Include/Rect.h>
#include <MintContainer/Include/Vector.hpp>


namespace mint
{
	namespace Physics
	{
#pragma region CollisionShape2D - PointCollisionShape2D
		inline PointCollisionShape2D::PointCollisionShape2D(const Float2& center)
			: CollisionShape2D(center)
		{
			__noop;
		}
#pragma endregion

#pragma region CollisionShape2D - CircleCollisionShape2D
		inline CircleCollisionShape2D::CircleCollisionShape2D(const Float2& center, const float radius)
			: CollisionShape2D(center)
			, _radius{ radius }
		{
			MINT_ASSERT(radius > 0.0f, "radius(%f) 가 0 이하입니다. 의도한 게 맞나요?", radius);
		}
#pragma endregion

#pragma region CollisionShape2D - AABBCollisionShape2D
		inline AABBCollisionShape2D::AABBCollisionShape2D(const Float2& center, const Float2& halfSize)
			: CollisionShape2D(center)
			, _halfSize{ halfSize }
		{
			__noop;
		}
#pragma endregion
		
#pragma region CollisionShape2D - BoxCollisionShape2D
		inline BoxCollisionShape2D::BoxCollisionShape2D(const Float2& center, const Float2& halfSize, const float angle)
			: CollisionShape2D(center)
			, _halfSize{ halfSize }
			, _angle{ angle }
		{
			__noop;
		}
#pragma endregion

#pragma region CollisionShape2D - ConvexCollisionShape2D
		inline ConvexCollisionShape2D::ConvexCollisionShape2D(const Float2& center, const Vector<Float2>& vertices)
			: CollisionShape2D(center)
			, _vertices{ vertices }
		{
			__noop;
		}
		inline ConvexCollisionShape2D::ConvexCollisionShape2D()
			: CollisionShape2D(Float2::kZero)
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
