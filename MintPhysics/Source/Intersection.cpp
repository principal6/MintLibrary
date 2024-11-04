#include <MintPhysics/Include/Intersection.h>
#include <MintPhysics/Include/Intersection.hpp>
#include <MintMath/Include/Geometry.h>
#include <MintRenderingBase/Include/ShapeRenderer.h>
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
			MINT_ASSERT(_validPointCount < 3, "!!!");
			_points[_validPointCount] = pointA;
			++_validPointCount;
		}

		void GJK2DSimplex::DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const
		{
			if (GetValidPointCount() == 0)
			{
				return;
			}

			const float kCircleRadius = 2.0f;
			const float kLineThickness = 1.0f;
			shapeRenderer.SetColor(color);

			if (GetValidPointCount() == 1)
			{
				shapeRenderer.SetPosition(Float4(GetPointA()));
				shapeRenderer.DrawCircle(kCircleRadius);
			}
			else if (GetValidPointCount() == 2)
			{
				shapeRenderer.SetPosition(Float4(GetPointA()));
				shapeRenderer.DrawCircle(kCircleRadius);
				shapeRenderer.SetPosition(Float4(GetPointB()));
				shapeRenderer.DrawCircle(kCircleRadius);
				shapeRenderer.DrawLine(GetPointA(), GetPointB(), kLineThickness);
			}
			else if (GetValidPointCount() == 3)
			{
				shapeRenderer.SetPosition(Float4(GetPointA()));
				shapeRenderer.DrawCircle(kCircleRadius);
				shapeRenderer.SetPosition(Float4(GetPointB()));
				shapeRenderer.DrawCircle(kCircleRadius);
				shapeRenderer.SetPosition(Float4(GetPointC()));
				shapeRenderer.DrawCircle(kCircleRadius);

				shapeRenderer.DrawLine(GetPointA(), GetPointB(), kLineThickness);
				shapeRenderer.DrawLine(GetPointA(), GetPointC(), kLineThickness);
				shapeRenderer.DrawLine(GetPointB(), GetPointC(), kLineThickness);
			}
		}

		const Float2& GJK2DSimplex::GetClosestPoint() const
		{
			if (GetValidPointCount() == 0)
			{
				MINT_ASSERT(false, "No valid points exist!");
				static const Float2 kInvalid;
				return kInvalid;
			}

			uint32 closestPointIndex = 0;
			float minLength = Math::kFloatMax;
			for (uint32 i = 0; i < _validPointCount; ++i)
			{
				float length = _points[i].Length();
				if (length < minLength)
				{
					closestPointIndex = i;
					minLength = length;
				}
			}
			return _points[closestPointIndex];
		}

		MINT_INLINE Float2 GJK2D_ComputeMinkowskiDifferencePoint(const CollisionShape2D& shapeA, const CollisionShape2D& shapeB, const Float2& direction)
		{
			return shapeA.ComputeSupportPoint(direction) - shapeB.ComputeSupportPoint(-direction);
		}

		MINT_INLINE Float2 GJK2D_ComputePerpABToAC(const Float2& ab, const Float2& ac)
		{
			const Float2 normalizedAB = Float2::Normalize(ab);
			const Float2 result = ac - normalizedAB * normalizedAB.Dot(ac);
			return (result.LengthSqaure() <= Math::kFloatEpsilon ? Float2::kZero : Float2::Normalize(result));
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
				if (ab == Float2::kZero)
				{
					// EDGE_CASE: ab are colinear!
					return true;
				}
				else if (ac == Float2::kZero)
				{
					// EDGE_CASE: ac are colinear!
					return true;
				}
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

		bool Intersect2D_GJK(const CollisionShape2D& shapeA, const CollisionShape2D& shapeB, GJK2DInfo* const outGJK2DInfo)
		{
			Float2 direction = Float2(1, 0);
			if (outGJK2DInfo != nullptr)
			{
				outGJK2DInfo->_loopCount = 0;
				outGJK2DInfo->_direction = direction;
			}

			Float2 minkowskiDifferencePoint = GJK2D_ComputeMinkowskiDifferencePoint(shapeA, shapeB, direction);
			GJK2DSimplex simplex{ minkowskiDifferencePoint };
			if (minkowskiDifferencePoint == Float2::kZero)
			{
				// EDGE_CASE: The origin is included in the Minkowski Sum, thus the two shapes intersect.
				if (outGJK2DInfo != nullptr)
				{
					outGJK2DInfo->_simplex = simplex;
				}
				return true;
			}

			// minkowskiDifferenceVertex to origin
			direction = -minkowskiDifferencePoint;
			direction.Normalize();

			bool result = false;
			while (true)
			{
				if (outGJK2DInfo != nullptr)
				{
					if (outGJK2DInfo->_loopCount >= outGJK2DInfo->_maxLoopCount)
					{
						break;
					}

					outGJK2DInfo->_direction = direction;
					++outGJK2DInfo->_loopCount;
				}

				minkowskiDifferencePoint = GJK2D_ComputeMinkowskiDifferencePoint(shapeA, shapeB, direction);

				const float signedDistance = minkowskiDifferencePoint.Dot(direction);
				if (signedDistance < 0.0f)
				{
					// MinkowskiDifferenceVertex did not pass the origin
					// Thus, an intersection is not possible.
					break;
				}
				else if (signedDistance == 0.0f)
				{
					// EDGE_CASE: The origin is included in the Minkowski Sum, thus the two shapes intersect.
					result = true;
					break;
				}

				simplex.AppendPoint(minkowskiDifferencePoint);
				if (GJK2D_ProcessSimplex(simplex, direction) == true)
				{
					result = true;
					break;
				}
			}

			if (outGJK2DInfo != nullptr)
			{
				outGJK2DInfo->_simplex = simplex;
			}
			return result;
		}

		Float2 ComputeEdgeNormal(const Float2& edgeVertex0, const Float2& edgeVertex1)
		{
			// V1 => V0 is CCW
			Float2 normal = edgeVertex0 - edgeVertex1;
			normal.Normalize();
			return Float2(-normal._y, normal._x);
		}

		Float2 ComputeClosestPointOnEdge(const Float2& point, const Float2& edgeVertex0, const Float2& edgeVertex1)
		{
			Float2 tangent = edgeVertex0 - edgeVertex1;
			const float edgeLength = tangent.Length();
			if (edgeLength == 0.0f)
			{
				return edgeVertex1;
			}

			tangent /= edgeLength;
			const float d = (point - edgeVertex1).Dot(tangent);
			return edgeVertex1 + tangent * Max(0.0f, Min(d, edgeLength));
		}

		void ComputePenetration_EPA(const CollisionShape2D& shapeA, const CollisionShape2D& shapeB, const GJK2DInfo& gjk2DInfo, Float2& outNormal, float& outDistance, EPA2DInfo& epa2DInfo)
		{
			epa2DInfo._iteration = 0;
			epa2DInfo._points.Clear();

			if (gjk2DInfo._simplex.GetValidPointCount() == 1)
			{
				const Float2& c = gjk2DInfo._simplex.GetPointA();
				Float2 direction = (c == Float2::kZero ? Float2(1, 1) : -c);
				direction.Normalize();
				const Float2 b = GJK2D_ComputeMinkowskiDifferencePoint(shapeA, shapeB, direction);
				Float2 edgeNormal = ComputeEdgeNormal(b, c);
				epa2DInfo._points.PushBack(c);
				epa2DInfo._points.PushBack(b);
				Float2 a = GJK2D_ComputeMinkowskiDifferencePoint(shapeA, shapeB, edgeNormal);
				epa2DInfo._points.PushBack(a);
			}
			else if (gjk2DInfo._simplex.GetValidPointCount() == 2)
			{
				const Float2& b = gjk2DInfo._simplex.GetPointA();
				const Float2& c = gjk2DInfo._simplex.GetPointB();
				Float2 edgeNormal = ComputeEdgeNormal(b, c);
				epa2DInfo._points.PushBack(c);
				epa2DInfo._points.PushBack(b);
				Float2 a = GJK2D_ComputeMinkowskiDifferencePoint(shapeA, shapeB, edgeNormal);
				epa2DInfo._points.PushBack(a);
			}
			else
			{
				const Float2& a = gjk2DInfo._simplex.GetPointA();
				const Float2& b = gjk2DInfo._simplex.GetPointB();
				const Float2& c = gjk2DInfo._simplex.GetPointC();
				epa2DInfo._points.PushBack(c);
				epa2DInfo._points.PushBack(b);
				epa2DInfo._points.PushBack(a);
			}
			GrahamScan_Convexify(epa2DInfo._points);

			while (true)
			{
				if (epa2DInfo._iteration >= epa2DInfo._maxIterationCount)
					break;

				++epa2DInfo._iteration;

				// a => b is CW
				Float2 closestEdgeNormal;
				Float2 closestPoint;
				uint32 indexB = 0;
				float distanceToEdge = Math::kFloatMax;
				{
					const uint32 pointCount = epa2DInfo._points.Size();
					for (uint32 i = 0; i < pointCount; ++i)
					{
						// v1 => v0 is CCW
						const Float2& v0 = epa2DInfo._points[(i == 0 ? pointCount - 1 : i - 1)];
						const Float2& v1 = epa2DInfo._points[i];
						const Float2 p = ComputeClosestPointOnEdge(Float2::kZero, v1, v0);
						const float distance = p.Length();
						if (distance < distanceToEdge)
						{
							indexB = i;
							closestPoint = p;
							closestEdgeNormal = ComputeEdgeNormal(v0, v1);
							distanceToEdge = distance;
						}
					}
				}

				Float2 support = GJK2D_ComputeMinkowskiDifferencePoint(shapeA, shapeB, closestEdgeNormal);
				const float distance = support.Dot(closestEdgeNormal);
				if (::abs(distance - distanceToEdge) < 1.0f)
				{
					outNormal = closestEdgeNormal;
					outDistance = distance;
					return;
				}
				else
				{
					epa2DInfo._points.Insert(support, indexB);
				}
			}
		}
	}
}
