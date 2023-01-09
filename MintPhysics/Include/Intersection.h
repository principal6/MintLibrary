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
			virtual void debug_drawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color) { __noop; }
			virtual Float2 computeSupportPoint(const Float2& direction) const abstract;

		public:
			Float2 _center;
		};

		class CircleShape2D : public Shape2D
		{
		public:
			CircleShape2D(const Float2& center, const float radius);

		public:
			virtual void debug_drawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color) override final;
			virtual Float2 computeSupportPoint(const Float2& direction) const override final;

		public:
			float _radius;
		};

		class AABBShape2D : public Shape2D
		{
		public:
			AABBShape2D(const Float2& center, const Float2& halfSize);

		public:
			virtual void debug_drawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color) override final;
			virtual Float2 computeSupportPoint(const Float2& direction) const override final;
		
		public:
			Float2 _halfSize;
		};

		// CCW winding
		class ConvexShape2D : public Shape2D
		{
		public:
			static ConvexShape2D makeFromPoints(const Float2& center, const Vector<Float2>& points);
			static ConvexShape2D makeMinkowskiDifferenceShape(const ConvexShape2D& a, const ConvexShape2D& b);

		public:
			ConvexShape2D(const Float2& center, const Vector<Float2>& vertices);

		public:
			virtual void debug_drawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color) override final;
			virtual Float2 computeSupportPoint(const Float2& direction) const override final;

		private:
			ConvexShape2D();

		private:
			static uint32 GrahamScan_findStartPoint(const Vector<Float2>& points);
			static void GrahamScan_sortPoints(Vector<Float2>& inoutPoints);
			static void GrahamScan_convexify(Vector<Float2>& inoutPoints);

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
			void appendPoint(const Float2& pointA);
		
		public:
			const uint8 getValidPointCount() const { return _validPointCount; }
			const Float2& getPointA() const { return _points[_validPointCount - 1]; }
			const Float2& getPointB() const { return _points[_validPointCount - 2]; }
			const Float2& getPointC() const { return _points[_validPointCount - 3]; }

		private:
			Float2 _points[3];
			uint8 _validPointCount;
		};


		bool intersect2D_circle_point(const Float2& circleCenter, const float circleRadius, const Float2& point);
		bool intersect2D_aabb_point(const Rect& aabb, const Float2& point);
		bool intersect2D_GJK(const Shape2D& shapeA, const Shape2D& shapeB);
	}
}


#endif // !_MINT_PHYSICS_INTERSECTION_H_
