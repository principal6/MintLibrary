#include <MintPhysics/Include/Intersection.h>
#include <MintPhysics/Include/Intersection.hpp>
#include <MintRenderingBase/Include/ShapeRendererContext.h>


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
