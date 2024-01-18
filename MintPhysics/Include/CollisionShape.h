#pragma once


#ifndef _MINT_PHYSICS_COLLISION_SHAPE_H_
#define _MINT_PHYSICS_COLLISION_SHAPE_H_


#include <MintContainer/Include/Vector.h>
#include <MintMath/Include/Float2.h>


namespace mint
{
	class ByteColor;
	struct Transform2D;

	namespace Rendering
	{
		struct Shape;
		class ShapeRendererContext;
	}
}

namespace mint
{
	namespace Physics
	{
		class ConvexCollisionShape2D;
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
			AABBCollisionShape2D(const ConvexCollisionShape2D& convexCollisionShape2D);

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
			ConvexCollisionShape2D(const CollisionShape2D& rhs, const Transform2D& transform2D);
			ConvexCollisionShape2D(const ConvexCollisionShape2D& rhs, const Transform2D& transform2D);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset = Float2::kZero) const override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Convex; }
			const Vector<Float2>& GetVertices() const { return _vertices; }

		private:
			ConvexCollisionShape2D();

		private:
			Vector<Float2> _vertices;
		};
	}
}


#endif // !_MINT_PHYSICS_COLLISION_SHAPE_H_
