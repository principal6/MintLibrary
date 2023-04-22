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


		class GJKShape2D
		{
		public:
			GJKShape2D() = default;
			GJKShape2D(const Float2& center) : _center{ center } { __noop; }
			virtual ~GJKShape2D() = default;

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) { __noop; }
			virtual Float2 ComputeSupportPoint(const Float2& direction) const abstract;

		public:
			Float2 _center;
		};

		class GJKPointShape2D : public GJKShape2D
		{
		public:
			GJKPointShape2D(const Float2& center);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
		};
		
		class GJKCircleShape2D : public GJKShape2D
		{
		public:
			GJKCircleShape2D(const Float2& center, const float radius);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;

		public:
			float _radius;
		};

		class GJKAABBShape2D : public GJKShape2D
		{
		public:
			GJKAABBShape2D(const Float2& center, const Float2& halfSize);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;
		
		public:
			Float2 _halfSize;
		};

		// CCW winding
		class GJKConvexShape2D : public GJKShape2D
		{
		public:
			static GJKConvexShape2D MakeFromPoints(const Float2& center, const Vector<Float2>& points);
			static GJKConvexShape2D MakeMinkowskiDifferenceShape(const GJKConvexShape2D& a, const GJKConvexShape2D& b);

		public:
			GJKConvexShape2D(const Float2& center, const Vector<Float2>& vertices);

		public:
			virtual void DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) override final;
			virtual Float2 ComputeSupportPoint(const Float2& direction) const override final;

		private:
			GJKConvexShape2D();

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
		bool Intersect2D_GJK(const GJKShape2D& shapeA, const GJKShape2D& shapeB);
	}
}


#endif // !_MINT_PHYSICS_INTERSECTION_H_
