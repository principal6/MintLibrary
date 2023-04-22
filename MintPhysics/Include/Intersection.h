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
		class ShapeRendererContext;
		class ByteColor;
	}


	namespace Physics
	{
		using mint::Rendering::ShapeRendererContext;
		using mint::Rendering::ByteColor;


		class Shape2D
		{
		public:
			Shape2D() = default;
			Shape2D(const Float2& center) : _center{ center } { __noop; }
			virtual ~Shape2D() = default;

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) { __noop; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const abstract;

		public:
			Float2 _center;
		};

		class CircleShape2D : public Shape2D
		{
		public:
			CircleShape2D(const Float2& center, const float radius);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;

		public:
			float _radius;
		};

		class AABBShape2D : public Shape2D
		{
		public:
			AABBShape2D(const Float2& center, const Float2& halfSize);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
		
		public:
			Float2 _halfSize;
		};

		// CCW winding
		class ConvexShape2D : public Shape2D
		{
		public:
			static ConvexShape2D MakeFromPoints(const Float2& center, const Vector<Float2>& points);
			static ConvexShape2D MakeMinkowskiDifferenceShape(const ConvexShape2D& a, const ConvexShape2D& b);

		public:
			ConvexShape2D(const Float2& center, const Vector<Float2>& vertices);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;

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
		class GJKSimplex2D
		{
		public:
			GJKSimplex2D(const Float2& pointA);
			GJKSimplex2D(const Float2& pointB, const Float2& pointA);

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
		bool Intersect2D_GJK(const Shape2D& shapeA, const Shape2D& shapeB);
	}
}


#endif // !_MINT_PHYSICS_INTERSECTION_H_
