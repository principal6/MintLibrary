﻿#pragma once


#ifndef _MINT_PHYSICS_INTERSECTION_H_
#define _MINT_PHYSICS_INTERSECTION_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/Vector.h>
#include <MintMath/Include/Float2.h>


namespace mint
{
	class Rect;

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
			GJK2DSimplex();
			GJK2DSimplex(const Float2& pointA);
			GJK2DSimplex(const Float2& pointB, const Float2& pointA);

		public:
			void AppendPoint(const Float2& pointA);

		public:
			const uint8 GetValidPointCount() const { return _validPointCount; }
			const Float2& GetPointA() const { return _points[_validPointCount - 1]; }
			const Float2& GetPointB() const { return _points[_validPointCount - 2]; }
			const Float2& GetPointC() const { return _points[_validPointCount - 3]; }

		private:
			Float2 _points[3];
			uint8 _validPointCount;
		};


		bool Intersect2D_Circle_Point(const Float2& circleCenter, const float circleRadius, const Float2& point);
		bool Intersect2D_AABB_Point(const Rect& aabb, const Float2& point);
		bool Intersect2D_AABB_AABB(const AABBCollisionShape2D& shapeA, const AABBCollisionShape2D& shapeB);
		bool Intersect2D_GJK(const CollisionShape2D& shapeA, const CollisionShape2D& shapeB, uint32* const outLoopCount = nullptr);

		void Intersect2D_GJK_StepByStep(const uint32 maxStep, ShapeRendererContext& shapeRendererContext, const Float2& offset, const CollisionShape2D& shapeA, const CollisionShape2D& shapeB);
	}
}


#endif // !_MINT_PHYSICS_INTERSECTION_H_
