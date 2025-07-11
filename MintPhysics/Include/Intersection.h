#pragma once


#ifndef _MINT_PHYSICS_INTERSECTION_H_
#define _MINT_PHYSICS_INTERSECTION_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/Vector.h>
#include <MintMath/Include/Float2.h>


namespace mint
{
	class ByteColor;
	class Rect;
	struct Transform2D;

	namespace Rendering
	{
		class ShapeRenderer;
	}

	namespace Physics2D
	{
		class CollisionShape;
		class AABBCollisionShape;
	}
}

namespace mint
{
	namespace Physics2D
	{
		using mint::Rendering::ShapeRenderer;

		// Newest ---- Oldest
		//   A  -  B  -  C
		class GJKSimplex
		{
		public:
			GJKSimplex();
			GJKSimplex(const Float2& pointA);
			GJKSimplex(const Float2& pointB, const Float2& pointA);

		public:
			void AppendPoint(const Float2& pointA);

		public:
			void DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const;
			const Float2& GetClosestPoint() const;
			const uint8 GetValidPointCount() const { return _validPointCount; }
			const Float2& GetPointA() const { return _points[_validPointCount - 1]; }
			const Float2& GetPointB() const { return _points[_validPointCount - 2]; }
			const Float2& GetPointC() const { return _points[_validPointCount - 3]; }

		private:
			Float2 _points[3];
			uint8 _validPointCount;
		};

		struct GJKInfo
		{
			uint32 _maxLoopCount = kUint32Max;
			uint32 _loopCount = 0;
			Float2 _direction;
			GJKSimplex _simplex;
		};

		bool Intersect_Circle_Point(const Float2& circleCenter, const float circleRadius, const Float2& point);
		bool Intersect_AABB_Point(const Rect& aabb, const Float2& point);
		bool Intersect_AABB_AABB(const AABBCollisionShape& shapeA, const AABBCollisionShape& shapeB);
		bool Intersect_GJK(const CollisionShape& shapeA, const CollisionShape& shapeB, GJKInfo* const outGJKInfo = nullptr);

		Float2 ComputeEdgeNormal(const Float2& edgeVertex0, const Float2& edgeVertex1);
		Float2 ComputeClosestPointOnEdge(const Float2& point, const Float2& edgeVertex0, const Float2& edgeVertex1);
		struct EPAInfo
		{
			uint32 _maxIterationCount = kUint32Max;
			uint32 _iteration = 0;
			Vector<Float2> _points;
		};
		void ComputePenetration_EPA(const CollisionShape& shapeA, const CollisionShape& shapeB, const GJKInfo& gjkInfo, Float2& outNormal, float& outDistance, EPAInfo& epaInfo);
	}
}


#endif // !_MINT_PHYSICS_INTERSECTION_H_
