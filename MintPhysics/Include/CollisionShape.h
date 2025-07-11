#pragma once


#ifndef _MINT_PHYSICS_COLLISION_SHAPE_H_
#define _MINT_PHYSICS_COLLISION_SHAPE_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintMath/Include/Float2.h>
#include <MintMath/Include/Transform.h>


namespace mint
{
	class ByteColor;

	namespace Rendering
	{
		class ShapeRenderer;
		struct Shape;
	}

	namespace Physics2D
	{
		class ConvexCollisionShape;
	}
}

namespace mint
{
	namespace Physics2D
	{
		using mint::Rendering::ShapeRenderer;

		enum class CollisionShapeType
		{
			Point,
			Edge,
			Circle,
			AABB,
			Box,
			Convex,
			Composite,
		};

		class CollisionShape
		{
		public:
			static SharedPtr<CollisionShape> MakeTransformed(const SharedPtr<CollisionShape>& shape, const Transform2D& transform2D);

		public:
			CollisionShape() = default;
			virtual ~CollisionShape() = default;

		public:
			virtual CollisionShapeType GetCollisionShapeType() const abstract;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const abstract;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const abstract;
		
		public:
			virtual void DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const { __noop; }
		};

		class PointCollisionShape : public CollisionShape
		{
		public:
			PointCollisionShape(const Float2& center);

		public:
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Point; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const override final;
		
		public:
			virtual void DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const override final;

		public:
			Float2 _center;
		};

		class EdgeCollisionShape : public CollisionShape
		{
		public:
			EdgeCollisionShape(const Float2& vertexA, const Float2& vertexB);
			EdgeCollisionShape(const Float2& vertexA, const Float2& vertexB, const Transform2D& transform2D);

		public:
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Edge; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const override final;

		public:
			virtual void DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const override final;

		public:
			Float2 _vertexA;
			Float2 _vertexB;
		};

		class CircleCollisionShape : public CollisionShape
		{
		public:
			CircleCollisionShape(const Float2& center, const float radius);
			CircleCollisionShape(const float radius, const Transform2D& transform2D);

		public:
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Circle; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const override final;
		
		public:
			virtual void DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const  override final;

		public:
			Float2 _center;
			float _radius;
		};

		class AABBCollisionShape : public CollisionShape
		{
		public:
			AABBCollisionShape(const Float2& center, const Float2& halfSize);
			AABBCollisionShape(const CollisionShape& collisionShape);
			AABBCollisionShape(const AABBCollisionShape& aabbCollisionShape, const Transform2D& transform2D);

		public:
			void Set(const AABBCollisionShape& aabbCollisionShape, const Transform2D& transform2D);
			void SetExpanded(const AABBCollisionShape& aabbCollisionShape, const Transform2D& transform2D, const Float2& displacement);
			void SetExpandedByRadius(const AABBCollisionShape& aabbCollisionShape, const Transform2D& transform2D, const Float2& displacement);
			void SetExpandedByRadius(float radius, const Float2& center, const Float2& displacement);
		
		public:
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::AABB; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const override final;
		
		public:
			virtual void DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const override final;

		public:
			Float2 _center;
			Float2 _halfSize;
		};

		class BoxCollisionShape : public CollisionShape
		{
		public:
			BoxCollisionShape(const Float2& halfSize, const Transform2D& transform2D);
			BoxCollisionShape(const Float2& center, const Float2& halfLengthedAxisX, const Float2& halfLengthedAxisY);

		public:
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Box; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const override final;
			const Float2& GetHalfLengthedAxisX() const { return _halfLengthedAxisX; }
			const Float2& GetHalfLengthedAxisY() const { return _halfLengthedAxisY; }
		
		public:
			virtual void DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const override final;

		public:
			Float2 _center;

		private:
			Float2 _halfLengthedAxisX;
			Float2 _halfLengthedAxisY;
		};

		// CCW winding
		class ConvexCollisionShape : public CollisionShape
		{
		public:
			static ConvexCollisionShape MakeFromPoints(const Vector<Float2>& points);
			static ConvexCollisionShape MakeFromRenderingShape(const Float2& center, const Rendering::Shape& renderingShape);
			static ConvexCollisionShape MakeMinkowskiDifferenceShape(const CollisionShape& a, const CollisionShape& b);

		private:
			static ConvexCollisionShape MakeFromCollisionShape(const CollisionShape& shape);
			static ConvexCollisionShape MakeFromAABBShape(const AABBCollisionShape& shape);
			static ConvexCollisionShape MakeFromBoxShape(const BoxCollisionShape& shape);
			static ConvexCollisionShape MakeFromCircleShape(const CircleCollisionShape& shape);

		public:
			ConvexCollisionShape(const Vector<Float2>& vertices);
			ConvexCollisionShape(const Vector<Float2>& vertices, const Transform2D& transform2D);
			ConvexCollisionShape(const ConvexCollisionShape& rhs, const Transform2D& transform2D);

		public:
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Convex; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const override final;
			const Vector<Float2>& GetVertices() const { return _vertices; }
		
		public:
			virtual void DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const override final;

		private:
			ConvexCollisionShape();

		private:
			void ComputeSupportPointIndex(const Float2& direction, uint32& outIndex) const;

		private:
			Vector<Float2> _vertices;
		};

		class CompositeCollisionShape : public CollisionShape
		{
		public:
			struct ShapeInstance
			{
				SharedPtr<CollisionShape> _shape;
				Transform2D _transform2D;
			};

		public:
			CompositeCollisionShape(const Vector<ShapeInstance>& shapeInstances);
		
		public:
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Composite; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const override final;
			const Vector<ShapeInstance>& GetShapeInstances() const { return _shapeInstances; }

		public:
			virtual void DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const override final;

		private:
			Vector<ShapeInstance> _shapeInstances;
		};
	}
}


#endif // !_MINT_PHYSICS_COLLISION_SHAPE_H_
