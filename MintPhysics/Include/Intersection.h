#pragma once


#ifndef _MINT_PHYSICS_INTERSECTION_H_
#define _MINT_PHYSICS_INTERSECTION_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/Vector.h>
#include <MintMath/Include/Float2.h>


namespace mint
{
	class Rect;
	class ByteColor;


	namespace Rendering
	{
		struct Shape;
		class ShapeRendererContext;
	}


	namespace Physics
	{
		using mint::Rendering::ShapeRendererContext;


		enum class CollisionShapeType
		{
			Point,
			Circle,
			AABB,
			Box,
			Convex,
		};

		class CollisionShape2D
		{
		public:
			CollisionShape2D() = default;
			CollisionShape2D(const Float2& center) : _center{ center } { __noop; }
			virtual ~CollisionShape2D() = default;

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset = Float2::kZero) const { __noop; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const abstract;
			virtual CollisionShapeType GetCollisionShapeType() const abstract;

		public:
			Float2 _center;
		};

		class PointCollisionShape2D : public CollisionShape2D
		{
		public:
			PointCollisionShape2D(const Float2& center);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset = Float2::kZero) const override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Point; }
		};
		
		class CircleCollisionShape2D : public CollisionShape2D
		{
		public:
			CircleCollisionShape2D(const Float2& center, const float radius);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset = Float2::kZero) const  override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Circle; }

		public:
			float _radius;
		};

		class AABBCollisionShape2D : public CollisionShape2D
		{
		public:
			AABBCollisionShape2D(const Float2& center, const Float2& halfSize);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset = Float2::kZero) const override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::AABB; }

		public:
			Float2 _halfSize;
		};
		
		class BoxCollisionShape2D : public CollisionShape2D
		{
		public:
			BoxCollisionShape2D(const Float2& center, const Float2& halfSize, const float angle);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset = Float2::kZero) const override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Box; }

		public:
			Float2 _halfSize;
			float _angle;
		};

		// CCW winding
		class ConvexCollisionShape2D : public CollisionShape2D
		{
		private:
			static ConvexCollisionShape2D MakeFromBoxShape2D(const BoxCollisionShape2D& shape);
			static ConvexCollisionShape2D MakeFromCircleShape2D(const CircleCollisionShape2D& shape);
		
		public:
			static ConvexCollisionShape2D MakeFromPoints(const Float2& center, const Vector<Float2>& points);
			static ConvexCollisionShape2D MakeFromShape2D(const CollisionShape2D& shape);
			static ConvexCollisionShape2D MakeFromRenderingShape(const Float2& center, const Rendering::Shape& renderingShape);
			static ConvexCollisionShape2D MakeMinkowskiDifferenceShape(const CollisionShape2D& a, const CollisionShape2D& b);
			static ConvexCollisionShape2D MakeMinkowskiDifferenceShape(const ConvexCollisionShape2D& a, const ConvexCollisionShape2D& b);

		public:
			ConvexCollisionShape2D(const Float2& center, const Vector<Float2>& vertices);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset = Float2::kZero) const override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Convex; }

		private:
			ConvexCollisionShape2D();

		private:
			static uint32 GrahamScan_FindStartPoint(const Vector<Float2>& points);
			static void GrahamScan_SortPoints(Vector<Float2>& inoutPoints);
			static void GrahamScan_Convexify(Vector<Float2>& inoutPoints);

		private:
			Vector<Float2> _vertices;
		};

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
		bool Intersect2D_GJK(const CollisionShape2D& shapeA, const CollisionShape2D& shapeB, uint32* const outLoopCount = nullptr);

		void Intersect2D_GJK_StepByStep(const uint32 maxStep, ShapeRendererContext& shapeRendererContext, const Float2& offset, const CollisionShape2D& shapeA, const CollisionShape2D& shapeB);
	}
}


#endif // !_MINT_PHYSICS_INTERSECTION_H_
