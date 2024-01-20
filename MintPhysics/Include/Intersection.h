#pragma once


#ifndef _MINT_PHYSICS_INTERSECTION_H_
#define _MINT_PHYSICS_INTERSECTION_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/Vector.h>
#include <MintMath/Include/Float2.h>


namespace mint
{
	class Rect;
	struct Transform2D;

	namespace Rendering
	{
		class ShapeRendererContext;
	}

	namespace Physics
	{
		class CollisionShape2D;
		class AABBCollisionShape2D;
	}
}

namespace mint
{
	namespace Physics
	{
		using mint::Rendering::ShapeRendererContext;

		// Newest ---- Oldest
		//   A  -  B  -  C
		class GJK2DSimplex
		{
		public:
			struct Point
			{
				union
				{
					struct
					{
						Float2 _position;
						Float2 _shapeAPoint;
						Float2 _shapeBPoint;
					};
					Float2 _positions[3]{};
				};
			};

		public:
			GJK2DSimplex();
			GJK2DSimplex(const Point& pointA);
			GJK2DSimplex(const Point& pointB, const Point& pointA);

		public:
			void AppendPoint(const Point& pointA);

		public:
			void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Transform2D& transform2D) const;
			const Point& GetClosestPoint() const;
			const uint8 GetValidPointCount() const { return _validPointCount; }
			const Point& GetPointA() const { return _points[_validPointCount - 1]; }
			const Point& GetPointB() const { return _points[_validPointCount - 2]; }
			const Point& GetPointC() const { return _points[_validPointCount - 3]; }

		private:
			Point _points[3];
			uint8 _validPointCount;
		};

		struct GJK2DInfo
		{
			uint32 _maxLoopCount = kUint32Max;
			uint32 _loopCount = 0;
			Float2 _direction;
			GJK2DSimplex _simplex;
		};

		bool Intersect2D_Circle_Point(const Float2& circleCenter, const float circleRadius, const Float2& point);
		bool Intersect2D_AABB_Point(const Rect& aabb, const Float2& point);
		bool Intersect2D_AABB_AABB(const AABBCollisionShape2D& shapeA, const AABBCollisionShape2D& shapeB);
		bool Intersect2D_GJK(const CollisionShape2D& shapeA, const CollisionShape2D& shapeB, GJK2DInfo* const outGJK2DInfo = nullptr);
	}
}


#endif // !_MINT_PHYSICS_INTERSECTION_H_
