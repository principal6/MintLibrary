#include <MintPhysics/Include/Intersection.h>
#include <MintPhysics/Include/Intersection.hpp>
#include <MintRenderingBase/Include/ShapeRendererContext.h>
#include <MintRenderingBase/Include/ShapeGenerator.h>
#include <MintPhysics/Include/CollisionShape.h>


namespace mint
{
	namespace Physics
	{
		GJK2DSimplex::GJK2DSimplex()
			: _validPointCount{ 0 }
		{
			__noop;
		}

		GJK2DSimplex::GJK2DSimplex(const Float2& pointA)
			: _validPointCount{ 1 }
			, _points{ pointA }
		{
			__noop;
		}

		GJK2DSimplex::GJK2DSimplex(const Float2& pointB, const Float2& pointA)
			: _validPointCount{ 2 }
			, _points{ pointB, pointA }
		{
			__noop;
		}

		void GJK2DSimplex::AppendPoint(const Float2& pointA)
		{
			_points[_validPointCount] = pointA;
			++_validPointCount;
		}

		MINT_INLINE Float2 GJK2D_GetMinkowskiDifferenceVertex(const CollisionShape2D& shapeA, const CollisionShape2D& shapeB, const Float2& direction)
		{
			return shapeA.ComputeSupportPoint(direction) - shapeB.ComputeSupportPoint(-direction);
		}

		MINT_INLINE Float2 GJK2D_ComputePerpABToAC(const Float2& ab, const Float2& ac)
		{
			const Float2 normalizedAB = Float2::Normalize(ab);
			const Float2 result = ac - normalizedAB * normalizedAB.Dot(ac);
			return (result.LengthSqaure() <= Math::kFloatEpsilon ? Float2::kZero : Float2::Normalize(result));
		}

		MINT_INLINE bool Intersect2D_GJK_Retrun(const bool result, const uint32 loopCount, uint32* const outLoopCount)
		{
			if (outLoopCount != nullptr)
			{
				*outLoopCount = loopCount;
			}
			return result;
		}
		// returns true whenever it's sure that there's an intersection
		bool GJK2D_ProcessSimplex(GJK2DSimplex& inoutSimplex, Float2& outDirection)
		{
			const Float2& a = inoutSimplex.GetPointA();
			const Float2& b = inoutSimplex.GetPointB();
			const Float2 ab = b - a;
			const Float2 ao = -a;
			const float ab_dot_ao = ab.Dot(ao);
			if (inoutSimplex.GetValidPointCount() == 2)
			{
				// 2-simplex (line)
				if (ab_dot_ao > 0.0f)
				{
					// origin is enclosed by the segment region AB
					outDirection = GJK2D_ComputePerpABToAC(ab, ao);
					if (outDirection == Float2::kZero)
					{
						// EDGE_CASE: ab and ao are colineaer!
						return true;
					}
					return false;
				}
				else
				{
					// origin is outside the point region A
					inoutSimplex = GJK2DSimplex(a);
					outDirection = Float2::Normalize(ao);
					return false;
				}
			}
			else if (inoutSimplex.GetValidPointCount() == 3)
			{
				// 3-simplex (triangle)
				const Float2& c = inoutSimplex.GetPointC();
				const Float2 ac = c - a;
				const float ac_dot_ao = ac.Dot(ao);
				const Float2 perpDirection_ab_to_ao = GJK2D_ComputePerpABToAC(ab, ao);
				const Float2 perpDirection_ab_to_ac = GJK2D_ComputePerpABToAC(ab, ac);
				if (perpDirection_ab_to_ao.Dot(perpDirection_ab_to_ac) > 0.0f)
				{
					// origin is inside the 3-simplex segment region AB
					const Float2 perpDirection_ac_to_ao = GJK2D_ComputePerpABToAC(ac, ao);
					const Float2 perpDirection_ac_to_ab = GJK2D_ComputePerpABToAC(ac, ab);
					if (perpDirection_ac_to_ao.Dot(perpDirection_ac_to_ab) > 0.0f)
					{
						// origin is inside the 3-simplex segment region AC
						// thus, origin is enclosed by the 3-simplex
						// there is an intersection!!!
						return true;
					}
					else
					{
						// origin is outside the 3-simplex segment region AC
						inoutSimplex = GJK2DSimplex(c, a);
						outDirection = perpDirection_ac_to_ao;
						return false;
					}
				}
				else
				{
					// origin is outside the 3-simplex segment region AB
					inoutSimplex = GJK2DSimplex(b, a);
					outDirection = perpDirection_ab_to_ao;
					return false;
				}
			}

			// no 1-simplex case is possible!!!
			MINT_NEVER;
			return false;
		}

		bool Intersect2D_AABB_AABB(const AABBCollisionShape2D& shapeA, const AABBCollisionShape2D& shapeB)
		{
			Float2 minA = shapeA._center - shapeA._halfSize;
			Float2 maxA = shapeA._center + shapeA._halfSize;
			Float2 minB = shapeB._center - shapeB._halfSize;
			Float2 maxB = shapeB._center + shapeB._halfSize;
			if (maxA._x < minB._x || maxB._x < minA._x || maxA._y < minB._y || maxB._y < minA._y)
			{
				return false;
			}
			return true;
		}

		bool Intersect2D_GJK(const CollisionShape2D& shapeA, const CollisionShape2D& shapeB, uint32* const outLoopCount)
		{
			uint32 loopCount = 0;
			Float2 direction = Float2(1, 0);
			Float2 minkowskiDifferenceVertex = GJK2D_GetMinkowskiDifferenceVertex(shapeA, shapeB, direction);
			if (minkowskiDifferenceVertex == Float2::kZero)
			{
				// EDGE_CASE: The origin is included in the Minkowski Sum, thus the two shapes intersect.
				return Intersect2D_GJK_Retrun(true, loopCount, outLoopCount);
			}

			GJK2DSimplex simplex{ minkowskiDifferenceVertex };
			// minkowskiDifferenceVertex to origin
			direction = -minkowskiDifferenceVertex;
			direction.Normalize();

			while (true)
			{
				++loopCount;

				minkowskiDifferenceVertex = GJK2D_GetMinkowskiDifferenceVertex(shapeA, shapeB, direction);

				const float signedDistance = minkowskiDifferenceVertex.Dot(direction);
				if (signedDistance < 0.0f)
				{
					// MinkowskiDifferenceVertex did not pass the origin
					// Thus, an intersection is not possible.
					return Intersect2D_GJK_Retrun(false, loopCount, outLoopCount);
				}
				else if (signedDistance == 0.0f)
				{
					// EDGE_CASE: The origin is included in the Minkowski Sum, thus the two shapes intersect.
					return Intersect2D_GJK_Retrun(true, loopCount, outLoopCount);
				}

				simplex.AppendPoint(minkowskiDifferenceVertex);
				if (GJK2D_ProcessSimplex(simplex, direction))
				{
					return Intersect2D_GJK_Retrun(true, loopCount, outLoopCount);
				}
			}
		}
	}
}
