#include <MintPhysics/Include/CollisionShape.h>
#include <MintContainer/Include/Vector.hpp>
#include <MintMath/Include/Float2x2.h>
#include <MintMath/Include/Geometry.h>
#include <MintMath/Include/Transform.h>
#include <MintRenderingBase/Include/ShapeRendererContext.h>


namespace mint
{
	namespace Physics
	{
#pragma region CollisionShape2D
		SharedPtr<CollisionShape2D> CollisionShape2D::MakeTransformed(const CollisionShape2D& shape, const Transform2D& transform2D)
		{
			switch (shape.GetCollisionShapeType())
			{
			case mint::Physics::CollisionShapeType::Point:
			{
				const PointCollisionShape2D& castedShape = static_cast<const PointCollisionShape2D&>(shape);
				return MakeShared<PointCollisionShape2D>(PointCollisionShape2D(castedShape._center + transform2D._translation));
			}
			case mint::Physics::CollisionShapeType::Circle:
			{
				const CircleCollisionShape2D& castedShape = static_cast<const CircleCollisionShape2D&>(shape);
				return MakeShared<CircleCollisionShape2D>(CircleCollisionShape2D(castedShape._center + transform2D._translation, castedShape._radius));
			}
			case mint::Physics::CollisionShapeType::AABB:
			{
				const AABBCollisionShape2D& castedShape = static_cast<const AABBCollisionShape2D&>(shape);
				return MakeShared<AABBCollisionShape2D>(AABBCollisionShape2D(castedShape._center + transform2D._translation, castedShape._halfSize));
			}
			case mint::Physics::CollisionShapeType::Box:
			{
				const BoxCollisionShape2D& castedShape = static_cast<const BoxCollisionShape2D&>(shape);
				const Float2x2 rotationMatrix{ Float2x2::RotationMatrix(transform2D._rotation) };
				const Float2 halfLengthedAxisX = rotationMatrix * castedShape.GetHalfLengthedAxisX();
				const Float2 halfLengthedAxisY = rotationMatrix * castedShape.GetHalfLengthedAxisY();
				return MakeShared<BoxCollisionShape2D>(BoxCollisionShape2D(castedShape._center + transform2D._translation, halfLengthedAxisX, halfLengthedAxisY));
			}
			case mint::Physics::CollisionShapeType::Convex:
			{
				const ConvexCollisionShape2D& castedShape = static_cast<const ConvexCollisionShape2D&>(shape);
				return MakeShared<ConvexCollisionShape2D>(ConvexCollisionShape2D(shape, transform2D));
			}
			default:
				break;
			}
			MINT_NEVER;
			return SharedPtr<CollisionShape2D>();
		}
#pragma endregion

#pragma region CollisionShape2D - PointCollisionShape2D
		PointCollisionShape2D::PointCollisionShape2D(const Float2& center)
			: CollisionShape2D()
			, _center{ center }
		{
			__noop;
		}

		void PointCollisionShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Transform2D& transform2D) const
		{
			shapeRendererContext.SetColor(color);
			shapeRendererContext.SetPosition(Float4(_center + transform2D._translation));
			shapeRendererContext.DrawCircle(2.0f);
		}

		Float2 PointCollisionShape2D::ComputeSupportPoint(const Float2& direction) const
		{
			return _center;
		}

		void PointCollisionShape2D::ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const
		{
			outVertexA = _center;
			outVertexB = _center;

			Float2 tangent{ -direction._y, direction._x };
			outVertexA -= tangent;
			outVertexB += tangent;
			MINT_ASSERT(tangent.Dot(direction) == 0.0f, "!!!");
		}
#pragma endregion

#pragma region CollisionShape2D - CircleCollisionShape2D
		CircleCollisionShape2D::CircleCollisionShape2D(const Float2& center, const float radius)
			: CollisionShape2D()
			, _center{ center }
			, _radius{ radius }
		{
			MINT_ASSERT(radius > 0.0f, "radius(%f) 가 0 이하입니다. 의도한 게 맞나요?", radius);
		}

		void CircleCollisionShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Transform2D& transform2D) const
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
				shapeRendererContext.DrawLine(transform2D._translation + _center + pointA, transform2D._translation + _center + pointB, 1.0f);
			}
		}

		Float2 CircleCollisionShape2D::ComputeSupportPoint(const Float2& direction) const
		{
			return _center + direction * _radius;
		}

		void CircleCollisionShape2D::ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const
		{
			outVertexA = ComputeSupportPoint(direction);
			outVertexB = outVertexA;

			Float2 tangent{ -direction._y, direction._x };
			outVertexA -= tangent;
			outVertexB += tangent;
			MINT_ASSERT(tangent.Dot(direction) == 0.0f, "!!!");
		}
#pragma endregion

#pragma region CollisionShape2D - AABBCollisionShape2D
		AABBCollisionShape2D::AABBCollisionShape2D(const Float2& center, const Float2& halfSize)
			: CollisionShape2D()
			, _center{ center }
			, _halfSize{ halfSize }
		{
			__noop;
		}

		AABBCollisionShape2D::AABBCollisionShape2D(const CollisionShape2D& collisionShape2D)
			: CollisionShape2D(collisionShape2D)
		{
			if (collisionShape2D.GetCollisionShapeType() == CollisionShapeType::AABB)
			{
				const AABBCollisionShape2D& aabbCollisionShape2D = static_cast<const AABBCollisionShape2D&>(collisionShape2D);
				_center = aabbCollisionShape2D._center;
				_halfSize = aabbCollisionShape2D._halfSize;
			}
			else if (collisionShape2D.GetCollisionShapeType() == CollisionShapeType::Circle)
			{
				const CircleCollisionShape2D& circleCollisionShape2D = static_cast<const CircleCollisionShape2D&>(collisionShape2D);
				_center = circleCollisionShape2D._center;
				_halfSize = Float2(circleCollisionShape2D._radius, circleCollisionShape2D._radius);
			}
			else if (collisionShape2D.GetCollisionShapeType() == CollisionShapeType::Box)
			{
				const BoxCollisionShape2D& boxCollisionShape2D = static_cast<const BoxCollisionShape2D&>(collisionShape2D);
				_center = boxCollisionShape2D._center;
				const Float2 max = _center + boxCollisionShape2D.GetHalfLengthedAxisX() + boxCollisionShape2D.GetHalfLengthedAxisY();
				const Float2 min = _center - boxCollisionShape2D.GetHalfLengthedAxisX() - boxCollisionShape2D.GetHalfLengthedAxisY();

				const Float2 center = (max + min) * 0.5f;
				const Float2 size = max - min;
				_center = center;
				_halfSize = size * 0.5f;
			}
			else if (collisionShape2D.GetCollisionShapeType() == CollisionShapeType::Convex)
			{
				const ConvexCollisionShape2D& convexCollisionShape2D = static_cast<const ConvexCollisionShape2D&>(collisionShape2D);
				const Vector<Float2>& vertices = convexCollisionShape2D.GetVertices();
				Float2 min = +Float2::kMax;
				Float2 max = -Float2::kMax;
				for (const Float2& vertex : vertices)
				{
					min = Float2::Min(min, vertex);
					max = Float2::Max(max, vertex);
				}

				const Float2 center = (max + min) * 0.5f;
				const Float2 size = max - min;
				_center = center;
				_halfSize = size * 0.5f;
			}
			else
			{
				MINT_ASSERT(false, "Not implemented yet.");
			}
		}

		AABBCollisionShape2D::AABBCollisionShape2D(const AABBCollisionShape2D& aabbCollisionShape2D, const Transform2D& transform2D)
			: CollisionShape2D(aabbCollisionShape2D)
		{
			Set(aabbCollisionShape2D, transform2D);
		}

		void AABBCollisionShape2D::Set(const AABBCollisionShape2D& aabbCollisionShape2D, const Transform2D& transform2D)
		{
			_center = aabbCollisionShape2D._center + transform2D._translation;
			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(transform2D._rotation);
			const Float2& x = rotationMatrix._row[0];
			const Float2& y = rotationMatrix._row[1];
			const Float2 rotatedHalfSizeX = x * aabbCollisionShape2D._halfSize._x;
			const Float2 rotatedHalfSizeY = y * aabbCollisionShape2D._halfSize._y;
			const Float2 p0 = -rotatedHalfSizeX + +rotatedHalfSizeY;
			const Float2 p1 = -rotatedHalfSizeX + -rotatedHalfSizeY;
			const Float2 p2 = +rotatedHalfSizeX + -rotatedHalfSizeY;
			const Float2 p3 = +rotatedHalfSizeX + +rotatedHalfSizeY;
			const float minX = Min(Min(Min(p0._x, p1._x), p2._x), p3._x);
			const float minY = Min(Min(Min(p0._y, p1._y), p2._y), p3._y);
			const float maxX = Max(Max(Max(p0._x, p1._x), p2._x), p3._x);
			const float maxY = Max(Max(Max(p0._y, p1._y), p2._y), p3._y);
			_halfSize._x = maxX - minX;
			_halfSize._y = maxY - minY;
			_halfSize *= 0.5f;
		}

		void AABBCollisionShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Transform2D& transform2D) const
		{
			shapeRendererContext.SetColor(color);

			const Float2 halfSizeX = Float2(_halfSize._x, 0.0f);
			const Float2 halfSizeY = Float2(0.0f, _halfSize._y);
			shapeRendererContext.DrawLine(transform2D._translation + _center - halfSizeX + halfSizeY, transform2D._translation + _center + halfSizeX + halfSizeY, 1.0f);
			shapeRendererContext.DrawLine(transform2D._translation + _center - halfSizeX - halfSizeY, transform2D._translation + _center + halfSizeX - halfSizeY, 1.0f);
			shapeRendererContext.DrawLine(transform2D._translation + _center + halfSizeX + halfSizeY, transform2D._translation + _center + halfSizeX - halfSizeY, 1.0f);
			shapeRendererContext.DrawLine(transform2D._translation + _center - halfSizeX + halfSizeY, transform2D._translation + _center - halfSizeX - halfSizeY, 1.0f);
		}

		Float2 AABBCollisionShape2D::ComputeSupportPoint(const Float2& direction) const
		{
			if (direction._x >= 0.0f)
			{
				return _center + Float2(_halfSize._x, (direction._y >= 0.0f ? +_halfSize._y : -_halfSize._y));
			}
			return _center + Float2(-_halfSize._x, (direction._y >= 0.0f ? +_halfSize._y : -_halfSize._y));
		}

		void AABBCollisionShape2D::ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const
		{
			const float diagonalLength = ::sqrt(_halfSize._x * _halfSize._x + _halfSize._y * _halfSize._y);
			const Float2 diagonalLengthedDirection = direction * diagonalLength;
			if (diagonalLengthedDirection._y > _halfSize._y)
			{
				// Upper
				outVertexA = _center + Float2(+_halfSize._x, +_halfSize._y);
				outVertexB = _center + Float2(-_halfSize._x, +_halfSize._y);
			}
			else if (diagonalLengthedDirection._y < +_halfSize._y && diagonalLengthedDirection._y > -_halfSize._y)
			{
				// Side
				if (diagonalLengthedDirection._x > 0.0f)
				{
					// Right
					outVertexA = _center + Float2(+_halfSize._x, -_halfSize._y);
					outVertexB = _center + Float2(+_halfSize._x, +_halfSize._y);
				}
				else
				{
					// Left
					outVertexA = _center + Float2(-_halfSize._x, +_halfSize._y);
					outVertexB = _center + Float2(-_halfSize._x, -_halfSize._y);
				}
			}
			else
			{
				// Lower
				outVertexA = _center + Float2(-_halfSize._x, -_halfSize._y);
				outVertexB = _center + Float2(+_halfSize._x, -_halfSize._y);
			}
		}
#pragma endregion

#pragma region CollisionShape2D - BoxCollisionShape2D
		BoxCollisionShape2D::BoxCollisionShape2D(const Float2& center, const Float2& halfSize, const float angle)
			: CollisionShape2D()
			, _center{ center }
			, _halfLengthedAxisX{ Float2::kZero }
			, _halfLengthedAxisY{ Float2::kZero }
		{
			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(angle);
			const Float2& x = rotationMatrix._row[0];
			const Float2& y = rotationMatrix._row[1];
			_halfLengthedAxisX = x * halfSize._x;
			_halfLengthedAxisY = y * halfSize._y;
		}

		BoxCollisionShape2D::BoxCollisionShape2D(const Float2& center, const Float2& halfLengthedAxisX, const Float2& halfLengthedAxisY)
			: CollisionShape2D()
			, _center{ center }
			, _halfLengthedAxisX{ halfLengthedAxisX }
			, _halfLengthedAxisY{ halfLengthedAxisY }
		{
			__noop;
		}

		void BoxCollisionShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Transform2D& transform2D) const
		{
			shapeRendererContext.SetColor(color);

			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(transform2D._rotation);
			const Float2 halfSizeX = rotationMatrix * GetHalfLengthedAxisX();
			const Float2 halfSizeY = rotationMatrix * GetHalfLengthedAxisY();
			const Float2 center = transform2D._translation + _center;
			shapeRendererContext.DrawLine(center + halfSizeX + halfSizeY, center - halfSizeX + halfSizeY, 1.0f);
			shapeRendererContext.DrawLine(center - halfSizeX + halfSizeY, center - halfSizeX - halfSizeY, 1.0f);
			shapeRendererContext.DrawLine(center - halfSizeX - halfSizeY, center + halfSizeX - halfSizeY, 1.0f);
			shapeRendererContext.DrawLine(center + halfSizeX - halfSizeY, center + halfSizeX + halfSizeY, 1.0f);
		}

		Float2 BoxCollisionShape2D::ComputeSupportPoint(const Float2& direction) const
		{
			const Float2& halfSizeX = GetHalfLengthedAxisX();
			const Float2& halfSizeY = GetHalfLengthedAxisY();
			if (direction.Dot(halfSizeY) >= 0.0f)
			{
				return _center + halfSizeY + (direction.Dot(halfSizeX) >= 0.0f ? halfSizeX : -halfSizeX);
			}
			else
			{
				return _center - halfSizeY + (direction.Dot(halfSizeX) >= 0.0f ? halfSizeX : -halfSizeX);
			}
		}

		void BoxCollisionShape2D::ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const
		{
			const Float2& halfSizeX = GetHalfLengthedAxisX();
			const Float2& halfSizeY = GetHalfLengthedAxisY();
			const float halfWidth = halfSizeX.Length();
			const float halfHeight = halfSizeY.Length();
			const float halfDiagonalLength = ::sqrt(halfWidth * halfWidth + halfHeight * halfHeight);
			const Float2 halfDiagonalLengthedDirection = direction * halfDiagonalLength;
			const float dotY = halfDiagonalLengthedDirection.Dot(halfSizeY);
			if (dotY > +halfHeight)
			{
				// Upper
				outVertexA = _center + halfSizeX + halfSizeY;
				outVertexB = _center - halfSizeX + halfSizeY;
			}
			else if (dotY <= -halfHeight && dotY >= -halfHeight)
			{
				// Side
				if (direction.Dot(halfSizeX) >= 0.0f)
				{
					// Right
					outVertexA = _center + halfSizeX - halfSizeY;
					outVertexB = _center + halfSizeX + halfSizeY;
				}
				else
				{
					// Left
					outVertexA = _center - halfSizeX + halfSizeY;
					outVertexB = _center - halfSizeX - halfSizeY;
				}
			}
			else
			{
				// Lower
				outVertexA = _center - halfSizeX - halfSizeY;
				outVertexB = _center + halfSizeX - halfSizeY;
			}
		}
#pragma endregion

#pragma region CollisionShape2D - ConvexCollisionShape2D
		ConvexCollisionShape2D::ConvexCollisionShape2D(const Vector<Float2>& vertices)
			: CollisionShape2D()
			, _vertices{ vertices }
		{
			__noop;
		}

		ConvexCollisionShape2D::ConvexCollisionShape2D(const CollisionShape2D& rhs, const Transform2D& transform2D)
		{
			*this = MakeFromShape2D(rhs);

			for (Float2& vertex : _vertices)
			{
				vertex = transform2D * vertex;
			}
		}

		ConvexCollisionShape2D::ConvexCollisionShape2D(const ConvexCollisionShape2D& rhs, const Transform2D& transform2D)
			: CollisionShape2D()
			, _vertices{ rhs._vertices }
		{
			for (Float2& vertex : _vertices)
			{
				vertex = transform2D * vertex;
			}
		}

		ConvexCollisionShape2D::ConvexCollisionShape2D()
			: CollisionShape2D()
		{
			__noop;
		}

		ConvexCollisionShape2D ConvexCollisionShape2D::MakeFromPoints(const Vector<Float2>& points)
		{
			ConvexCollisionShape2D shape(points);
			GrahamScan_Convexify(shape._vertices);
			return shape;
		}

		ConvexCollisionShape2D ConvexCollisionShape2D::MakeFromAABBShape2D(const AABBCollisionShape2D& shape)
		{
			ConvexCollisionShape2D result;
			result._vertices.PushBack(shape._center + Float2(-shape._halfSize._x, +shape._halfSize._y));
			result._vertices.PushBack(shape._center + Float2(-shape._halfSize._x, -shape._halfSize._y));
			result._vertices.PushBack(shape._center + Float2(+shape._halfSize._x, -shape._halfSize._y));
			result._vertices.PushBack(shape._center + Float2(+shape._halfSize._x, +shape._halfSize._y));
			return result;
		}

		ConvexCollisionShape2D ConvexCollisionShape2D::MakeFromBoxShape2D(const BoxCollisionShape2D& shape)
		{
			const Float2& halfX = shape.GetHalfLengthedAxisX();
			const Float2& halfY = shape.GetHalfLengthedAxisY();

			ConvexCollisionShape2D result;
			result._vertices.PushBack(shape._center + +halfX + +halfY);
			result._vertices.PushBack(shape._center + -halfX + +halfY);
			result._vertices.PushBack(shape._center + -halfX + -halfY);
			result._vertices.PushBack(shape._center + +halfX + -halfY);
			return result;
		}

		ConvexCollisionShape2D ConvexCollisionShape2D::MakeFromCircleShape2D(const CircleCollisionShape2D& shape)
		{
			ConvexCollisionShape2D result;
			for (uint32 i = 0; i <= 32; i++)
			{
				float theta = (static_cast<float>(i) / 32.0f) * Math::kTwoPi;
				const float x = ::cos(theta) * shape._radius;
				const float y = ::sin(theta) * shape._radius;
				result._vertices.PushBack(shape._center + Float2(x, y));
			}
			return result;
		}

		ConvexCollisionShape2D ConvexCollisionShape2D::MakeFromShape2D(const CollisionShape2D& shape)
		{
			if (shape.GetCollisionShapeType() == CollisionShapeType::Convex)
			{
				return static_cast<const ConvexCollisionShape2D&>(shape);
			}
			else if (shape.GetCollisionShapeType() == CollisionShapeType::Circle)
			{
				return MakeFromCircleShape2D(static_cast<const CircleCollisionShape2D&>(shape));
			}
			else if (shape.GetCollisionShapeType() == CollisionShapeType::Box)
			{
				return MakeFromBoxShape2D(static_cast<const BoxCollisionShape2D&>(shape));
			}
			else if (shape.GetCollisionShapeType() == CollisionShapeType::AABB)
			{
				return MakeFromAABBShape2D(static_cast<const AABBCollisionShape2D&>(shape));
			}
			else
			{
				MINT_NEVER;
			}
			return ConvexCollisionShape2D();
		}

		ConvexCollisionShape2D ConvexCollisionShape2D::MakeFromRenderingShape(const Float2& center, const Rendering::Shape& renderingShape)
		{
			const uint32 vertexCount = renderingShape._vertices.Size();
			if (vertexCount == 0)
			{
				return ConvexCollisionShape2D(Vector<Float2>());
			}

			Vector<Float2> points;
			points.Resize(vertexCount);
			for (uint32 i = 0; i < vertexCount; ++i)
			{
				points[i] = center + renderingShape._vertices[i]._position.GetXY();
			}
			ConvexCollisionShape2D shape(points);
			GrahamScan_Convexify(shape._vertices);
			return shape;
		}

		ConvexCollisionShape2D ConvexCollisionShape2D::MakeMinkowskiDifferenceShape(const CollisionShape2D& a, const CollisionShape2D& b)
		{
			return MakeMinkowskiDifferenceShape(MakeFromShape2D(a), MakeFromShape2D(b));
		}

		ConvexCollisionShape2D ConvexCollisionShape2D::MakeMinkowskiDifferenceShape(const ConvexCollisionShape2D& a, const ConvexCollisionShape2D& b)
		{
			ConvexCollisionShape2D shape;
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

		void ConvexCollisionShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Transform2D& transform2D) const
		{
			shapeRendererContext.SetColor(color);

			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(transform2D._rotation);
			const uint32 vertexCount = _vertices.Size();
			const Float2 center = transform2D._translation;
			for (uint32 vertexIndex = 1; vertexIndex < vertexCount; ++vertexIndex)
			{
				shapeRendererContext.DrawLine(center + rotationMatrix * _vertices[vertexIndex - 1], center + rotationMatrix * _vertices[vertexIndex], 1.0f);
			}
			shapeRendererContext.DrawLine(center + rotationMatrix * _vertices[vertexCount - 1], center + rotationMatrix * _vertices[0], 1.0f);

			shapeRendererContext.SetPosition(Float4(center));
			shapeRendererContext.DrawCircle(4.0f);
		}

		Float2 ConvexCollisionShape2D::ComputeSupportPoint(const Float2& direction) const
		{
			if (_vertices.IsEmpty())
			{
				return Float2::kZero;
			}

			uint32 targetVertexIndex = 0;
			ComputeSupportPointIndex(direction, targetVertexIndex);
			return _vertices[targetVertexIndex];
		}

		void ConvexCollisionShape2D::ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const
		{
			uint32 targetVertexIndex = 0;
			ComputeSupportPointIndex(direction, targetVertexIndex);

			const Float2& targetVertex = _vertices[targetVertexIndex];
			const Float2* sideVertexCW = nullptr;
			const Float2* sideVertexCCW = nullptr;
			if (targetVertexIndex == 0)
			{
				sideVertexCW = &_vertices.Back();
				sideVertexCCW = &_vertices[targetVertexIndex + 1];
			}
			else if (targetVertexIndex == _vertices.Size() - 1)
			{
				sideVertexCW = &_vertices[targetVertexIndex - 1];
				sideVertexCCW = &_vertices[0];
			}
			else
			{
				sideVertexCW = &_vertices[targetVertexIndex - 1];
				sideVertexCCW = &_vertices[targetVertexIndex + 1];
			}

			Float2 v0 = (*sideVertexCW - targetVertex);
			v0.Normalize();
			Float2 v1 = (*sideVertexCCW - targetVertex);
			v1.Normalize();
			const float dot0 = direction.Dot(v0);
			const float dot1 = direction.Dot(v1);
			if (dot0 > dot1)
			{
				outVertexA = *sideVertexCW;
				outVertexB = targetVertex;
			}
			else
			{
				outVertexA = targetVertex;
				outVertexB = *sideVertexCCW;
			}
		}

		void ConvexCollisionShape2D::ComputeSupportPointIndex(const Float2& direction, uint32& outIndex) const
		{
			float maxDotProduct = -Math::kFloatMax;
			const uint32 vertexCount = _vertices.Size();
			for (uint32 vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			{
				const float dotProduct = direction.Dot(_vertices[vertexIndex]);
				if (dotProduct > maxDotProduct)
				{
					maxDotProduct = dotProduct;
					outIndex = vertexIndex;
				}
			}
		}
#pragma endregion
	}
}