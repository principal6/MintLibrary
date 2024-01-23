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
		class ShapeRendererContext;
		struct Shape;
	}

	namespace Physics
	{
		class ConvexCollisionShape2D;
	}
}

namespace mint
{
	namespace Physics
	{
		using mint::Rendering::ShapeRendererContext;

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

		class CollisionShape2D
		{
		public:
			static SharedPtr<CollisionShape2D> MakeTransformed(const SharedPtr<CollisionShape2D>& shape, const Transform2D& transform2D);

		public:
			CollisionShape2D() = default;
			virtual ~CollisionShape2D() = default;

		public:
			virtual CollisionShapeType GetCollisionShapeType() const abstract;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const abstract;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const abstract;
		
		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Transform2D& transform2D) const { __noop; }
		};

		class PointCollisionShape2D : public CollisionShape2D
		{
		public:
			PointCollisionShape2D(const Float2& center);

		public:
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Point; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const override final;
		
		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Transform2D& transform2D) const override final;

		public:
			Float2 _center;
		};

		class EdgeCollisionShape2D : public CollisionShape2D
		{
		public:
			EdgeCollisionShape2D(const Float2& vertexA, const Float2& vertexB);
			EdgeCollisionShape2D(const Float2& vertexA, const Float2& vertexB, const Transform2D& transform2D);

		public:
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Edge; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const override final;

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Transform2D& transform2D) const override final;

		public:
			Float2 _vertexA;
			Float2 _vertexB;
		};

		class CircleCollisionShape2D : public CollisionShape2D
		{
		public:
			CircleCollisionShape2D(const Float2& center, const float radius);
			CircleCollisionShape2D(const float radius, const Transform2D& transform2D);

		public:
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Circle; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const override final;
		
		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Transform2D& transform2D) const  override final;

		public:
			Float2 _center;
			float _radius;
		};

		class AABBCollisionShape2D : public CollisionShape2D
		{
		public:
			AABBCollisionShape2D(const Float2& center, const Float2& halfSize);
			AABBCollisionShape2D(const CollisionShape2D& collisionShape2D);
			AABBCollisionShape2D(const AABBCollisionShape2D& aabbCollisionShape2D, const Transform2D& transform2D);

		public:
			void Set(const AABBCollisionShape2D& aabbCollisionShape2D, const Transform2D& transform2D);
			void SetExpanded(const AABBCollisionShape2D& aabbCollisionShape2D, const Transform2D& transform2D, const Float2& displacement);
			void SetExpandedByRadius(const AABBCollisionShape2D& aabbCollisionShape2D, const Transform2D& transform2D, const Float2& displacement);
			void SetExpandedByRadius(float radius, const Float2& center, const Float2& displacement);
		
		public:
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::AABB; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const override final;
		
		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Transform2D& transform2D) const override final;

		public:
			Float2 _center;
			Float2 _halfSize;
		};

		class BoxCollisionShape2D : public CollisionShape2D
		{
		public:
			BoxCollisionShape2D(const Float2& center, const Float2& halfSize, const float angle);
			BoxCollisionShape2D(const Float2& center, const Float2& halfLengthedAxisX, const Float2& halfLengthedAxisY);

		public:
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Box; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const override final;
			const Float2& GetHalfLengthedAxisX() const { return _halfLengthedAxisX; }
			const Float2& GetHalfLengthedAxisY() const { return _halfLengthedAxisY; }
		
		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Transform2D& transform2D) const override final;

		public:
			Float2 _center;

		private:
			Float2 _halfLengthedAxisX;
			Float2 _halfLengthedAxisY;
		};

		// CCW winding
		class ConvexCollisionShape2D : public CollisionShape2D
		{
		public:
			static ConvexCollisionShape2D MakeFromPoints(const Vector<Float2>& points);
			static ConvexCollisionShape2D MakeFromRenderingShape(const Float2& center, const Rendering::Shape& renderingShape);
			static ConvexCollisionShape2D MakeMinkowskiDifferenceShape(const CollisionShape2D& a, const CollisionShape2D& b);

		private:
			static ConvexCollisionShape2D MakeFromCollisionShape2D(const CollisionShape2D& shape);
			static ConvexCollisionShape2D MakeFromAABBShape2D(const AABBCollisionShape2D& shape);
			static ConvexCollisionShape2D MakeFromBoxShape2D(const BoxCollisionShape2D& shape);
			static ConvexCollisionShape2D MakeFromCircleShape2D(const CircleCollisionShape2D& shape);

		public:
			ConvexCollisionShape2D(const Vector<Float2>& vertices);
			ConvexCollisionShape2D(const Vector<Float2>& vertices, const Transform2D& transform2D);
			ConvexCollisionShape2D(const ConvexCollisionShape2D& rhs, const Transform2D& transform2D);

		public:
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Convex; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const override final;
			const Vector<Float2>& GetVertices() const { return _vertices; }
		
		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Transform2D& transform2D) const override final;

		private:
			ConvexCollisionShape2D();

		private:
			void ComputeSupportPointIndex(const Float2& direction, uint32& outIndex) const;

		private:
			Vector<Float2> _vertices;
		};

		class CompositeCollisionShape2D : public CollisionShape2D
		{
		public:
			struct ShapeInstance
			{
				SharedPtr<CollisionShape2D> _shape;
				Transform2D _transform2D;
			};

		public:
			CompositeCollisionShape2D(const Vector<ShapeInstance>& shapeInstances);
		
		public:
			virtual CollisionShapeType GetCollisionShapeType() const override final { return CollisionShapeType::Composite; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual void ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const override final;
			const Vector<ShapeInstance>& GetShapeInstances() const { return _shapeInstances; }

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Transform2D& transform2D) const override final;

		private:
			Vector<ShapeInstance> _shapeInstances;
		};
	}
}


#endif // !_MINT_PHYSICS_COLLISION_SHAPE_H_
