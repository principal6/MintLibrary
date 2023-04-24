#pragma once


#include <MintPhysics/Include/Intersection.h>
#include <MintMath/Include/Rect.h>
#include <MintContainer/Include/Vector.hpp>


namespace mint
{
	namespace Physics
	{
#pragma region GJKShape2D - GJKPointShape2D
		inline GJKPointShape2D::GJKPointShape2D(const Float2& center)
			: GJKShape2D(center)
		{
			__noop;
		}
#pragma endregion

#pragma region GJKShape2D - GJKCircleShape2D
		inline GJKCircleShape2D::GJKCircleShape2D(const Float2& center, const float radius)
			: GJKShape2D(center)
			, _radius{ radius }
		{
			MINT_ASSERT(radius > 0.0f, "radius(%f) 가 0 이하입니다. 의도한 게 맞나요?", radius);
		}
#pragma endregion

#pragma region GJKShape2D - GJKAABBShape2D
		inline GJKAABBShape2D::GJKAABBShape2D(const Float2& center, const Float2& halfSize)
			: GJKShape2D(center)
			, _halfSize{ halfSize }
		{
			__noop;
		}
#pragma endregion
		
#pragma region GJKShape2D - GJKBoxShape2D
		inline GJKBoxShape2D::GJKBoxShape2D(const Float2& center, const Float2& halfSize, const float angle)
			: GJKShape2D(center)
			, _halfSize{ halfSize }
			, _angle{ angle }
		{
			__noop;
		}
#pragma endregion

#pragma region GJKShape2D - GJKConvexShape2D
		inline GJKConvexShape2D::GJKConvexShape2D(const Float2& center, const Vector<Float2>& vertices)
			: GJKShape2D(center)
			, _vertices{ vertices }
		{
			__noop;
		}
		inline GJKConvexShape2D::GJKConvexShape2D()
			: GJKShape2D(Float2::kZero)
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
