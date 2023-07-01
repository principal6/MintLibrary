#pragma once


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
		struct Shape;
		class ShapeRendererContext;
		class ByteColor;
	}


	namespace Physics
	{
		using mint::Rendering::ShapeRendererContext;
		using mint::Rendering::ByteColor;


		enum class ShapeType
		{
			Point,
			Circle,
			AABB,
			Box,
			Convex,
		};

		class Shape2D
		{
		public:
			Shape2D() = default;
			Shape2D(const Float2& center) : _center{ center } { __noop; }
			virtual ~Shape2D() = default;

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset = Float2::kZero) const { __noop; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const abstract;
			virtual ShapeType GetShapeType() const abstract;

		public:
			Float2 _center;
		};

		class PointShape2D : public Shape2D
		{
		public:
			PointShape2D(const Float2& center);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset = Float2::kZero) const override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual ShapeType GetShapeType() const override final { return ShapeType::Point; }
		};
		
		class CircleShape2D : public Shape2D
		{
		public:
			CircleShape2D(const Float2& center, const float radius);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset = Float2::kZero) const  override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual ShapeType GetShapeType() const override final { return ShapeType::Circle; }

		public:
			float _radius;
		};

		class AABBShape2D : public Shape2D
		{
		public:
			AABBShape2D(const Float2& center, const Float2& halfSize);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset = Float2::kZero) const override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual ShapeType GetShapeType() const override final { return ShapeType::AABB; }

		public:
			Float2 _halfSize;
		};
		
		class BoxShape2D : public Shape2D
		{
		public:
			BoxShape2D(const Float2& center, const Float2& halfSize, const float angle);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset = Float2::kZero) const override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual ShapeType GetShapeType() const override final { return ShapeType::Box; }

		public:
			Float2 _halfSize;
			float _angle;
		};

		// CCW winding
		class ConvexShape2D : public Shape2D
		{
		private:
			static ConvexShape2D MakeFromBoxShape2D(const BoxShape2D& shape);
			static ConvexShape2D MakeFromCircleShape2D(const CircleShape2D& shape);
		
		public:
			static ConvexShape2D MakeFromPoints(const Float2& center, const Vector<Float2>& points);
			static ConvexShape2D MakeFromShape2D(const Shape2D& shape);
			static ConvexShape2D MakeFromRenderingShape(const Float2& center, const Rendering::Shape& renderingShape);
			static ConvexShape2D MakeMinkowskiDifferenceShape(const Shape2D& a, const Shape2D& b);
			static ConvexShape2D MakeMinkowskiDifferenceShape(const ConvexShape2D& a, const ConvexShape2D& b);

		public:
			ConvexShape2D(const Float2& center, const Vector<Float2>& vertices);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset = Float2::kZero) const override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
			virtual ShapeType GetShapeType() const override final { return ShapeType::Convex; }

		private:
			ConvexShape2D();

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
		bool Intersect2D_GJK(const Shape2D& shapeA, const Shape2D& shapeB, uint32* const outLoopCount = nullptr);

		void Intersect2D_GJK_StepByStep(const uint32 maxStep, ShapeRendererContext& shapeRendererContext, const Float2& offset, const Shape2D& shapeA, const Shape2D& shapeB);
	}
}


#endif // !_MINT_PHYSICS_INTERSECTION_H_
