#include <MintPhysics/Include/Intersection.h>
#include <MintPhysics/Include/Intersection.hpp>
#include <MintRenderingBase/Include/ShapeRendererContext.h>
#include <MintRenderingBase/Include/ShapeGenerator.h>
#include <MintLibrary/Include/Algorithm.hpp>
#include <MintMath/Include/Float2x2.h>


namespace mint
{
	namespace Physics
	{
		void PointShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) const
		{
			shapeRendererContext.SetColor(color);
			shapeRendererContext.SetPosition(Float4(_center + offset));
			shapeRendererContext.DrawCircle(2.0f);
		}

		Float2 PointShape2D::ComputeSupportPoint(const Float2& direction) const
		{
			return _center;
		}

		void CircleShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) const
		{
			shapeRendererContext.SetColor(color);

			const uint32 kSideCount = 32;
			const float kThetaUnit = Math::kTwoPi / kSideCount;
			for (uint32 sideIndex = 1; sideIndex <= kSideCount; ++sideIndex)
			{
				const float thetaA = kThetaUnit * (sideIndex - 1);
				const float thetaB = kThetaUnit * sideIndex;
				const Float2 pointA = Float2(::cos(thetaA) * _radius, -::sin(thetaA) * _radius);
				const Float2 pointB = Float2(::cos(thetaB) * _radius, -::sin(thetaB) * _radius);
				shapeRendererContext.DrawLine(offset + _center + pointA, offset + _center + pointB, 1.0f);
			}
		}

		Float2 CircleShape2D::ComputeSupportPoint(const Float2& direction) const
		{
			return _center + direction * _radius;
		}

		void AABBShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) const
		{
			shapeRendererContext.SetColor(color);

			const Float2 halfSizeX = Float2(_halfSize._x, 0.0f);
			const Float2 halfSizeY = Float2(0.0f, _halfSize._y);
			shapeRendererContext.DrawLine(offset + _center - halfSizeX + halfSizeY, offset + _center + halfSizeX + halfSizeY, 1.0f);
			shapeRendererContext.DrawLine(offset + _center - halfSizeX - halfSizeY, offset + _center + halfSizeX - halfSizeY, 1.0f);
			shapeRendererContext.DrawLine(offset + _center + halfSizeX + halfSizeY, offset + _center + halfSizeX - halfSizeY, 1.0f);
			shapeRendererContext.DrawLine(offset + _center - halfSizeX + halfSizeY, offset + _center - halfSizeX - halfSizeY, 1.0f);
		}

		Float2 AABBShape2D::ComputeSupportPoint(const Float2& direction) const
		{
			if (direction._x > 0.0f)
			{
				return _center + Float2(_halfSize._x, (direction._y > 0.0f ? +_halfSize._y : -_halfSize._y));
			}
			return _center + Float2(-_halfSize._x, (direction._y > 0.0f ? +_halfSize._y : -_halfSize._y));
		}

		void BoxShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) const
		{
			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(_angle);
			const Float2& i = rotationMatrix._row[0];
			const Float2& j = rotationMatrix._row[1];

			shapeRendererContext.SetColor(color);

			const Float2 common = offset + _center;
			shapeRendererContext.DrawLine(common + i * _halfSize._x + j * _halfSize._y, common - i * _halfSize._x + j * _halfSize._y, 1.0f);
			shapeRendererContext.DrawLine(common - i * _halfSize._x + j * _halfSize._y, common - i * _halfSize._x - j * _halfSize._y, 1.0f);
			shapeRendererContext.DrawLine(common - i * _halfSize._x - j * _halfSize._y, common + i * _halfSize._x - j * _halfSize._y, 1.0f);
			shapeRendererContext.DrawLine(common + i * _halfSize._x - j * _halfSize._y, common + i * _halfSize._x + j * _halfSize._y, 1.0f);
		}

		Float2 BoxShape2D::ComputeSupportPoint(const Float2& direction) const
		{
			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(_angle);
			const Float2& i = rotationMatrix._row[0];
			const Float2& j = rotationMatrix._row[1];
			if (direction.Dot(j) >= 0.0f)
			{
				return _center + j * _halfSize._y + (direction.Dot(i) >= 0.0f ? i * _halfSize._x : -i * _halfSize._x);
			}
			else
			{
				return _center - j * _halfSize._y + (direction.Dot(i) >= 0.0f ? i * _halfSize._x : -i * _halfSize._x);
			}
		}

		ConvexShape2D ConvexShape2D::MakeFromPoints(const Float2& center, const Vector<Float2>& points)
		{
			ConvexShape2D shape(center, points);
			GrahamScan_Convexify(shape._vertices);
			return shape;
		}

		ConvexShape2D ConvexShape2D::MakeFromBoxShape2D(const BoxShape2D& shape)
		{
			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(shape._angle);
			const Float2& i = rotationMatrix._row[0];
			const Float2& j = rotationMatrix._row[1];

			ConvexShape2D result;
			result._center = shape._center;
			result._vertices.PushBack(+i * shape._halfSize._x + +j * shape._halfSize._y);
			result._vertices.PushBack(-i * shape._halfSize._x + +j * shape._halfSize._y);
			result._vertices.PushBack(-i * shape._halfSize._x + -j * shape._halfSize._y);
			result._vertices.PushBack(+i * shape._halfSize._x + -j * shape._halfSize._y);
			return result;
		}

		ConvexShape2D ConvexShape2D::MakeFromCircleShape2D(const CircleShape2D& shape)
		{
			ConvexShape2D result;
			result._center = shape._center;
			for (uint32 i = 0; i <= 32; i++)
			{
				float theta = (static_cast<float>(i) / 32.0f) * Math::kTwoPi;
				const float x = ::cos(theta) * shape._radius;
				const float y = ::sin(theta) * shape._radius;
				result._vertices.PushBack(Float2(x, y));
			}
			return result;
		}

		ConvexShape2D ConvexShape2D::MakeFromShape2D(const Shape2D& shape)
		{
			if (shape.GetShapeType() == ShapeType::Circle)
			{
				return MakeFromCircleShape2D(static_cast<const CircleShape2D&>(shape));
			}
			else if (shape.GetShapeType() == ShapeType::Box)
			{
				return MakeFromBoxShape2D(static_cast<const BoxShape2D&>(shape));
			}
			else
			{
				MINT_NEVER;
			}
			return ConvexShape2D();
		}

		ConvexShape2D ConvexShape2D::MakeFromRenderingShape(const Float2& center, const Rendering::Shape& renderingShape)
		{
			const uint32 vertexCount = renderingShape._vertices.Size();
			if (vertexCount == 0)
			{
				return ConvexShape2D(center, Vector<Float2>());
			}

			Vector<Float2> points;
			points.Resize(vertexCount);
			for (uint32 i = 0; i < vertexCount; ++i)
			{
				points[i] = renderingShape._vertices[i]._position.GetXY();
			}
			ConvexShape2D shape(center, points);
			GrahamScan_Convexify(shape._vertices);
			return shape;
		}

		ConvexShape2D ConvexShape2D::MakeMinkowskiDifferenceShape(const Shape2D& a, const Shape2D& b)
		{
			return MakeMinkowskiDifferenceShape(MakeFromShape2D(a), MakeFromShape2D(b));
		}

		ConvexShape2D ConvexShape2D::MakeMinkowskiDifferenceShape(const ConvexShape2D& a, const ConvexShape2D& b)
		{
			ConvexShape2D shape;
			shape._center = Float2(0, 0);

			shape._vertices.Clear();
			for (uint32 i = 0; i < a._vertices.Size(); i++)
			{
				for (uint32 j = 0; j < b._vertices.Size(); j++)
				{
					shape._vertices.PushBack(a._vertices[i] - b._vertices[j]);
				}
			}
			GrahamScan_Convexify(shape._vertices);
			return shape;
		}

		void ConvexShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) const
		{
			shapeRendererContext.SetColor(color);

			const uint32 vertexCount = _vertices.Size();
			for (uint32 vertexIndex = 1; vertexIndex < vertexCount; ++vertexIndex)
			{
				shapeRendererContext.DrawLine(_center + offset + _vertices[vertexIndex - 1], _center + offset + _vertices[vertexIndex], 1.0f);
			}
			shapeRendererContext.DrawLine(_center + offset + _vertices[vertexCount - 1], _center + offset + _vertices[0], 1.0f);
		}

		Float2 ConvexShape2D::ComputeSupportPoint(const Float2& direction) const
		{
			if (_vertices.IsEmpty())
			{
				return Float2::kZero;
			}

			float maxDotProduct = -Math::kFloatMax;
			uint32 targetVertexIndex = 0;
			const uint32 vertexCount = _vertices.Size();
			for (uint32 vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			{
				const float dotProduct = direction.Dot(_vertices[vertexIndex]);
				if (dotProduct > maxDotProduct)
				{
					maxDotProduct = dotProduct;
					targetVertexIndex = vertexIndex;
				}
			}
			return _center + _vertices[targetVertexIndex];
		}

		uint32 ConvexShape2D::GrahamScan_FindStartPoint(const Vector<Float2>& points)
		{
			Float2 min = Float2(10000.0f, -10000.0f);
			const uint32 pointCount = points.Size();
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

		void ConvexShape2D::GrahamScan_SortPoints(Vector<Float2>& inoutPoints)
		{
			const uint32 startPointIndex = GrahamScan_FindStartPoint(inoutPoints);
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
			const uint32 pointCount = inoutPoints.Size();
			Vector<AngleIndex> angleIndices;
			for (uint32 pointIndex = 0; pointIndex < pointCount; pointIndex++)
			{
				if (pointIndex == startPointIndex)
				{
					continue;
				}

				const float2 v = inoutPoints[pointIndex] - startPoint;
				const float theta = ::atan2f(-v._y, v._x);
				angleIndices.PushBack(AngleIndex(theta, pointIndex));
			}
			QuickSort(angleIndices, AngleIndexComparator());
			Vector<Float2> orderedPoints;
			orderedPoints.PushBack(startPoint);
			for (const AngleIndex& angleIndex : angleIndices)
			{
				orderedPoints.PushBack(inoutPoints[angleIndex._index]);
			}
			inoutPoints = orderedPoints;
		}

		void ConvexShape2D::GrahamScan_Convexify(Vector<Float2>& inoutPoints)
		{
			if (inoutPoints.IsEmpty())
			{
				return;
			}

			GrahamScan_SortPoints(inoutPoints);

			Vector<uint32> convexPointIndices;
			convexPointIndices.Reserve(inoutPoints.Size());
			convexPointIndices.PushBack(0);
			convexPointIndices.PushBack(1);
			for (uint32 i = 2; i < inoutPoints.Size(); i++)
			{
				const uint32 index_c = convexPointIndices[convexPointIndices.Size() - 2];
				const uint32 index_b = convexPointIndices[convexPointIndices.Size() - 1];
				const uint32 index_a = i;
				const Float2& c = inoutPoints[index_c];
				const Float2& b = inoutPoints[index_b];
				const Float2& a = inoutPoints[index_a];
				const Float3 cb = Float3(b - c);
				const Float3 ba = Float3(a - b);
				const Float3 ba_x_cb = ba.Cross(cb);
				const bool is_counter_clockwise = ba_x_cb._z > 0.0f;
				if (is_counter_clockwise)
				{
					convexPointIndices.PushBack(index_a);
				}
				else
				{
					convexPointIndices.PopBack();
					--i;
				}
			}

			Vector<Float2> convexPoints;
			convexPoints.Reserve(convexPointIndices.Size());
			for (const uint32 convexPointIndex : convexPointIndices)
			{
				convexPoints.PushBack(inoutPoints[convexPointIndex]);
			}
			inoutPoints = convexPoints;
		}

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

		MINT_INLINE Float2 GJK2D_getMinkowskiDifferenceVertex(const Shape2D& shapeA, const Shape2D& shapeB, const Float2& direction)
		{
			return shapeA.ComputeSupportPoint(direction) - shapeB.ComputeSupportPoint(-direction);
		}

		MINT_INLINE Float2 GJK2D_computePerpABToAC(const Float2& ab, const Float2& ac)
		{
			const Float2 normalizedAB = Float2::Normalize(ab);
			const Float2 result = ac - normalizedAB * normalizedAB.Dot(ac);
			return (result.LengthSqaure() == 0.0f ? result : Float2::Normalize(result));
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
		bool GJK2D_processSimplex(GJK2DSimplex& inoutSimplex, Float2& outDirection)
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
					outDirection = GJK2D_computePerpABToAC(ab, ao);
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
				const Float2 perpDirection_ab_to_ao = GJK2D_computePerpABToAC(ab, ao);
				const Float2 perpDirection_ab_to_ac = GJK2D_computePerpABToAC(ab, ac);
				if (perpDirection_ab_to_ao.Dot(perpDirection_ab_to_ac) > 0.0f)
				{
					// origin is inside the 3-simplex segment region AB
					const Float2 perpDirection_ac_to_ao = GJK2D_computePerpABToAC(ac, ao);
					const Float2 perpDirection_ac_to_ab = GJK2D_computePerpABToAC(ac, ab);
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

		bool Intersect2D_GJK(const Shape2D& shapeA, const Shape2D& shapeB, uint32* const outLoopCount)
		{
			uint32 loopCount = 0;
			Float2 direction = Float2(1, 0);
			Float2 minkowskiDifferenceVertex = GJK2D_getMinkowskiDifferenceVertex(shapeA, shapeB, direction);
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

				minkowskiDifferenceVertex = GJK2D_getMinkowskiDifferenceVertex(shapeA, shapeB, direction);
				
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
				if (GJK2D_processSimplex(simplex, direction))
				{
					return Intersect2D_GJK_Retrun(true, loopCount, outLoopCount);
				}
			}
		}

		bool Debug_Intersect2D_GJK(const Shape2D& shapeA, const Shape2D& shapeB, const uint32 maxStep, GJK2DSimplex& simplex, Float2& direction)
		{
			uint32 step = 0;
			direction = Float2(1, 0);
			Float2 minkowskiDifferenceVertex = GJK2D_getMinkowskiDifferenceVertex(shapeA, shapeB, direction);
			if (minkowskiDifferenceVertex == Float2::kZero)
			{
				// EDGE_CASE: The origin is included in the Minkowski Sum, thus the two shapes intersect.
				return true;
			}

			simplex = GJK2DSimplex(minkowskiDifferenceVertex);

			++step;
			if (step >= maxStep)
				return false;

			// minkowskiDifferenceVertex to origin
			direction = -minkowskiDifferenceVertex;
			direction.Normalize();

			while (true)
			{
				minkowskiDifferenceVertex = GJK2D_getMinkowskiDifferenceVertex(shapeA, shapeB, direction);
				const float signedDistance = minkowskiDifferenceVertex.Dot(direction);
				if (signedDistance < 0.0f)
				{
					// MinkowskiDifferenceVertex did not pass the origin
					// Thus, an intersection is not possible.
					return false;
				}
				else if (signedDistance == 0.0f)
				{
					// EDGE_CASE: The origin is included in the Minkowski Sum, thus the two shapes intersect.
					return true;
				}

				simplex.AppendPoint(minkowskiDifferenceVertex);

				++step;

				if (step >= maxStep)
					break;

				if (GJK2D_processSimplex(simplex, direction))
				{
					return true;
				}

				++step;

				if (step >= maxStep)
					break;
			}
			return false;
		}

		void Intersect2D_GJK_StepByStep(const uint32 maxStep, ShapeRendererContext& shapeRendererContext, const Float2& offset, const Shape2D& shapeA, const Shape2D& shapeB)
		{
			Physics::ConvexShape2D minkowskiDifferenceShape = Physics::ConvexShape2D::MakeMinkowskiDifferenceShape(shapeA, shapeB);
			minkowskiDifferenceShape.DebugDrawShape(shapeRendererContext, ByteColor(0, 127, 255), offset);

			const float kAxisLength = 128.0f;
			const Float2 origin = offset + shapeB._center - shapeA._center;
			shapeRendererContext.SetColor(Rendering::Color(0.0f, 0.0f, 0.0f));
			shapeRendererContext.DrawLine(origin - Float2(kAxisLength, 0.0f), origin + Float2(kAxisLength, 0.0f), 1.0f);
			shapeRendererContext.DrawLine(origin - Float2(0.0f, kAxisLength), origin + Float2(0.0f, kAxisLength), 1.0f);

			Physics::GJK2DSimplex simplex;
			Float2 direction;
			const bool intersects = Physics::Debug_Intersect2D_GJK(shapeA, shapeB, maxStep, simplex, direction);

			shapeA.DebugDrawShape(shapeRendererContext, (intersects ? ByteColor(0, 255, 0) : ByteColor(255, 0, 255)), offset);
			shapeB.DebugDrawShape(shapeRendererContext, (intersects ? ByteColor(0, 255, 0) : ByteColor(0, 0, 255)), offset);

			if (simplex.GetValidPointCount() == 1)
			{
				shapeRendererContext.SetColor(Rendering::Color(1.0f, 0.25f, 0.25f));
				shapeRendererContext.SetPosition(Float4(origin + simplex.GetPointA()));
				shapeRendererContext.DrawCircle(2.0f);
			}
			else if (simplex.GetValidPointCount() == 2)
			{
				const Float2 b = origin + simplex.GetPointB();
				const Float2 a = origin + simplex.GetPointA();

				shapeRendererContext.SetColor(Rendering::Color(0.75f, 0.0f, 0.0f));
				shapeRendererContext.SetPosition(Float4(b));
				shapeRendererContext.DrawCircle(2.0f);

				shapeRendererContext.DrawLine(a, b, 1.0f);

				shapeRendererContext.SetColor(Rendering::Color(1.0f, 0.25f, 0.25f));
				shapeRendererContext.SetPosition(Float4(a));
				shapeRendererContext.DrawCircle(2.0f);
			}
			else if (simplex.GetValidPointCount() == 3)
			{
				const Float2 c = origin + simplex.GetPointC();
				const Float2 b = origin + simplex.GetPointB();
				const Float2 a = origin + simplex.GetPointA();

				shapeRendererContext.SetColor(Rendering::Color(0.75f, 0.0f, 0.0f));
				shapeRendererContext.SetPosition(Float4(c));
				shapeRendererContext.DrawCircle(2.0f);

				shapeRendererContext.SetPosition(Float4(b));
				shapeRendererContext.DrawCircle(2.0f);

				shapeRendererContext.DrawLine(a, b, 1.0f);
				shapeRendererContext.DrawLine(a, c, 1.0f);
				shapeRendererContext.DrawLine(b, c, 1.0f);

				shapeRendererContext.SetColor(Rendering::Color(1.0f, 0.25f, 0.25f));
				shapeRendererContext.SetPosition(Float4(a));
				shapeRendererContext.DrawCircle(2.0f);
			}
		}
	}
}
