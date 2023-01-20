﻿#include <MintPhysics/Include/Intersection.h>
#include <MintPhysics/Include/Intersection.hpp>
#include <MintRenderingBase/Include/ShapeRendererContext.h>
#include <MintLibrary/Include/Algorithm.hpp>


namespace mint
{
	namespace Physics
	{
		void CircleShape2D::debug_drawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color)
		{
			shapeRendererContext.setColor(color);

			const uint32 kSideCount = 32;
			const float kThetaUnit = Math::kTwoPi / kSideCount;
			for (uint32 sideIndex = 1; sideIndex < kSideCount; ++sideIndex)
			{
				const float thetaA = kThetaUnit * (sideIndex - 1);
				const float thetaB = kThetaUnit * sideIndex;
				const Float2 pointA = Float2(::cos(thetaA) * _radius, -::sin(thetaA) * _radius);
				const Float2 pointB = Float2(::cos(thetaB) * _radius, -::sin(thetaB) * _radius);
				shapeRendererContext.drawLine(_center + pointA, _center + pointB, 1.0f);
			}
		}

		Float2 CircleShape2D::computeSupportPoint(const Float2& direction) const
		{
			return _center + direction * _radius;
		}

		void AABBShape2D::debug_drawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color)
		{
			shapeRendererContext.setColor(color);

			const Float2 halfSizeX = Float2(_halfSize._x, 0.0f);
			const Float2 halfSizeY = Float2(0.0f, _halfSize._y);
			shapeRendererContext.drawLine(_center - halfSizeX + halfSizeY, _center + halfSizeX + halfSizeY, 1.0f);
			shapeRendererContext.drawLine(_center - halfSizeX - halfSizeY, _center + halfSizeX - halfSizeY, 1.0f);
			shapeRendererContext.drawLine(_center + halfSizeX + halfSizeY, _center + halfSizeX - halfSizeY, 1.0f);
			shapeRendererContext.drawLine(_center - halfSizeX + halfSizeY, _center - halfSizeX - halfSizeY, 1.0f);
		}

		Float2 AABBShape2D::computeSupportPoint(const Float2& direction) const
		{
			if (direction._x > 0.0f)
			{
				return _center + Float2(_halfSize._x, (direction._y > 0.0f ? +_halfSize._y : -_halfSize._y));
			}
			return _center + Float2(-_halfSize._x, (direction._y > 0.0f ? +_halfSize._y : -_halfSize._y));
		}

		ConvexShape2D ConvexShape2D::makeFromPoints(const Float2& center, const Vector<Float2>& points)
		{
			ConvexShape2D shape(center, points);
			GrahamScan_convexify(shape._vertices);
			return shape;
		}

		ConvexShape2D ConvexShape2D::makeMinkowskiDifferenceShape(const ConvexShape2D& a, const ConvexShape2D& b)
		{
			ConvexShape2D shape;
			shape._center = Float2(0, 0);

			shape._vertices.clear();
			for (uint32 i = 0; i < a._vertices.size(); i++)
			{
				for (uint32 j = 0; j < b._vertices.size(); j++)
				{
					shape._vertices.push_back(a._vertices[i] - b._vertices[j]);
				}
			}
			GrahamScan_convexify(shape._vertices);
			return shape;
		}

		void ConvexShape2D::debug_drawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color)
		{
			shapeRendererContext.setColor(color);

			const uint32 vertexCount = _vertices.size();
			for (uint32 vertexIndex = 1; vertexIndex < vertexCount; ++vertexIndex)
			{
				shapeRendererContext.drawLine(_vertices[vertexIndex - 1], _vertices[vertexIndex], 1.0f);
			}
		}

		Float2 ConvexShape2D::computeSupportPoint(const Float2& direction) const
		{
			if (_vertices.empty())
			{
				return Float2::kZero;
			}

			float maxDotProduct = -Math::kFloatMax;
			uint32 targetVertexIndex = 0;
			const uint32 vertexCount = _vertices.size();
			for (uint32 vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			{
				const float dotProduct = direction.dot(_vertices[vertexIndex]);
				if (dotProduct > maxDotProduct)
				{
					maxDotProduct = dotProduct;
					targetVertexIndex = vertexIndex;
				}
			}
			return _center + _vertices[targetVertexIndex];
		}
		
		uint32 ConvexShape2D::GrahamScan_findStartPoint(const Vector<Float2>& points)
		{
			Float2 min = Float2(10000.0f, -10000.0f);
			const uint32 pointCount = points.size();
			uint32 startPointIndex = 0;
			for (uint32 pointIndex = 0; pointIndex < pointCount; pointIndex++)
			{
				if (points[pointIndex]._y >= min._y)
				{
					if (points[pointIndex]._y == min._y)
					{
						if (points[pointIndex]._x < min._x)
						{
							min._x = points[pointIndex]._x;
							startPointIndex = pointIndex;
						}
					}
					else
					{
						min._y = points[pointIndex]._y;
						min._x = points[pointIndex]._x;
						startPointIndex = pointIndex;
					}
				}
			}
			return startPointIndex;
		}

		void ConvexShape2D::GrahamScan_sortPoints(Vector<Float2>& inoutPoints)
		{
			const uint32 startPointIndex = GrahamScan_findStartPoint(inoutPoints);
			const Float2& startPoint = inoutPoints[startPointIndex];
			struct AngleIndex
			{
				AngleIndex(const float theta, const uint32 index) : _theta{ theta }, _index{ index } { __noop; }
				float _theta = 0.0f;
				uint32 _index = 0;
			};
			struct AngleIndexComparator
			{
				bool operator()(const AngleIndex& lhs, const AngleIndex& rhs) const { return lhs._theta < rhs._theta; }
			};
			const uint32 pointCount = inoutPoints.size();
			Vector<AngleIndex> angleIndices;
			for (uint32 pointIndex = 0; pointIndex < pointCount; pointIndex++)
			{
				if (pointIndex == startPointIndex)
				{
					continue;
				}

				const float2 v = inoutPoints[pointIndex] - startPoint;
				const float theta = ::atan2f(-v._y, v._x);
				angleIndices.push_back(AngleIndex(theta, pointIndex));
			}
			quickSort(angleIndices, AngleIndexComparator());
			Vector<Float2> orderedPoints;
			orderedPoints.push_back(startPoint);
			for (const AngleIndex& angleIndex : angleIndices)
			{
				orderedPoints.push_back(inoutPoints[angleIndex._index]);
			}
			inoutPoints = orderedPoints;
		}

		void ConvexShape2D::GrahamScan_convexify(Vector<Float2>& inoutPoints)
		{
			if (inoutPoints.empty())
			{
				return;
			}

			GrahamScan_sortPoints(inoutPoints);

			Vector<uint32> convexPointIndices;
			convexPointIndices.reserve(inoutPoints.size());
			convexPointIndices.push_back(0);
			convexPointIndices.push_back(1);
			for (uint32 i = 2; i < inoutPoints.size(); i++)
			{
				const uint32 index_c = convexPointIndices[convexPointIndices.size() - 2];
				const uint32 index_b = convexPointIndices[convexPointIndices.size() - 1];
				const uint32 index_a = i;
				const Float2& c = inoutPoints[index_c];
				const Float2& b = inoutPoints[index_b];
				const Float2& a = inoutPoints[index_a];
				const Float3 cb = Float3(b - c);
				const Float3 ba = Float3(a - b);
				const Float3 ba_x_cb = ba.cross(cb);
				const bool is_counter_clockwise_or_straight = ba_x_cb._z >= 0.0f;
				if (is_counter_clockwise_or_straight)
				{
					convexPointIndices.push_back(index_a);
				}
				else
				{
					convexPointIndices.pop_back();
					--i;
				}
			}

			Vector<Float2> convexPoints;
			convexPoints.reserve(convexPointIndices.size());
			for (const uint32 convexPointIndex : convexPointIndices)
			{
				convexPoints.push_back(inoutPoints[convexPointIndex]);
			}
			inoutPoints = convexPoints;
		}

		GJKSimplex2D::GJKSimplex2D(const Float2& pointA)
			: _validPointCount{ 1 }
			, _points{ pointA }
		{
			__noop;
		}

		GJKSimplex2D::GJKSimplex2D(const Float2& pointB, const Float2& pointA)
			: _validPointCount{ 2 }
			, _points{ pointB, pointA }
		{
			__noop;
		}

		void GJKSimplex2D::appendPoint(const Float2& pointA)
		{
			_points[_validPointCount] = pointA;
			++_validPointCount;
		}

		MINT_INLINE Float2 GJK2D_getMinkowskiDifferenceVertex(const Shape2D& shapeA, const Shape2D& shapeB, const Float2& direction)
		{
			return shapeA.computeSupportPoint(direction) - shapeB.computeSupportPoint(-direction);
		}

		MINT_INLINE Float2 GJK2D_computePerpABToAC(const Float2& ab, const Float2& ac)
		{
			const Float2 normalizedAB = Float2::normalize(ab);
			const Float2 result = ac - normalizedAB * normalizedAB.dot(ac);
			return (result.lengthSqaure() == 0.0f ? result : Float2::normalize(result));
		}

		// returns true whenever it's sure that there's an intersection
		bool GJK2D_processSimplex(GJKSimplex2D& inoutSimplex, Float2& outDirection)
		{
			const Float2& a = inoutSimplex.getPointA();
			const Float2& b = inoutSimplex.getPointB();
			const Float2 ab = b - a;
			const Float2 ao = -a;
			const float ab_dot_ao = ab.dot(ao);
			if (inoutSimplex.getValidPointCount() == 2)
			{
				// 2-simplex (line)
				if (ab_dot_ao > 0.0f)
				{
					// origin is enclosed by the segment region AB
					outDirection = GJK2D_computePerpABToAC(ab, ao);
					return false;
				}
				else
				{
					// origin is outside the point region A
					inoutSimplex = GJKSimplex2D(a);
					outDirection = Float2::normalize(ao);
					return false;
				}
			}
			else if (inoutSimplex.getValidPointCount() == 3)
			{
				// 3-simplex (triangle)
				const Float2& c = inoutSimplex.getPointC();
				const Float2 ac = c - a;
				const float ac_dot_ao = ac.dot(ao);
				const Float2 perpDirection_ab_to_ao = GJK2D_computePerpABToAC(ab, ao);
				const Float2 perpDirection_ab_to_ac = GJK2D_computePerpABToAC(ab, ac);
				if (perpDirection_ab_to_ao.dot(perpDirection_ab_to_ac) > 0.0f)
				{
					// origin is inside the 3-simplex segment region AB
					const Float2 perpDirection_ac_to_ao = GJK2D_computePerpABToAC(ac, ao);
					const Float2 perpDirection_ac_to_ab = GJK2D_computePerpABToAC(ac, ab);
					if (perpDirection_ac_to_ao.dot(perpDirection_ac_to_ab) > 0.0f)
					{
						// origin is inside the 3-simplex segment region AC
						// thus, origin is enclosed by the 3-simplex
						// there is an intersection!!!
						return true;
					}
					else
					{
						// origin is outside the 3-simplex segment region AC
						inoutSimplex = GJKSimplex2D(c, a);
						outDirection = perpDirection_ac_to_ao;
						return false;
					}
				}
				else
				{
					// origin is outside the 3-simplex segment region AB
					inoutSimplex = GJKSimplex2D(b, a);
					outDirection = perpDirection_ab_to_ao;
					return false;
				}
			}

			// no 1-simplex case is possible!!!
			MINT_NEVER;
			return false;
		}

		bool intersect2D_GJK(const Shape2D& shapeA, const Shape2D& shapeB)
		{
			Float2 direction = Float2(1, 0);
			Float2 minkowskiDifferenceVertex = GJK2D_getMinkowskiDifferenceVertex(shapeA, shapeB, direction);

			GJKSimplex2D simplex{ minkowskiDifferenceVertex };
			// minkowskiDifferenceVertex to origin
			direction = -minkowskiDifferenceVertex;
			direction.normalize();

			while (true)
			{
				minkowskiDifferenceVertex = GJK2D_getMinkowskiDifferenceVertex(shapeA, shapeB, direction);
				if (minkowskiDifferenceVertex.dot(direction) < 0.0f)
				{
					// MinkowskiDifferenceVertex did not pass the origin
					// Thus, an intersection is not possible.
					return false;
				}

				simplex.appendPoint(minkowskiDifferenceVertex);
				if (GJK2D_processSimplex(simplex, direction))
				{
					return true;
				}
			}
		}
	}
}