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
#pragma region CollisionShape2D - PointCollisionShape2D
		PointCollisionShape2D::PointCollisionShape2D(const Float2& center)
			: CollisionShape2D(center)
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
#pragma endregion

#pragma region CollisionShape2D - CircleCollisionShape2D
		CircleCollisionShape2D::CircleCollisionShape2D(const Float2& center, const float radius)
			: CollisionShape2D(center)
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
#pragma endregion

#pragma region CollisionShape2D - AABBCollisionShape2D
		AABBCollisionShape2D::AABBCollisionShape2D(const Float2& center, const Float2& halfSize)
			: CollisionShape2D(center)
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
			_center = transform2D._translation;
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
			if (direction._x > 0.0f)
			{
				return _center + Float2(_halfSize._x, (direction._y > 0.0f ? +_halfSize._y : -_halfSize._y));
			}
			return _center + Float2(-_halfSize._x, (direction._y > 0.0f ? +_halfSize._y : -_halfSize._y));
		}
#pragma endregion

#pragma region CollisionShape2D - BoxCollisionShape2D
		BoxCollisionShape2D::BoxCollisionShape2D(const Float2& center, const Float2& halfSize, const float angle)
			: CollisionShape2D(center)
			, _halfLengthedAxisX{ Float2::kZero }
			, _halfLengthedAxisY{ Float2::kZero }
		{
			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(angle);
			const Float2& x = rotationMatrix._row[0];
			const Float2& y = rotationMatrix._row[1];
			_halfLengthedAxisX = x * halfSize._x;
			_halfLengthedAxisY = y * halfSize._y;
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
#pragma endregion

#pragma region CollisionShape2D - ConvexCollisionShape2D
		ConvexCollisionShape2D::ConvexCollisionShape2D(const Float2& center, const Vector<Float2>& vertices)
			: CollisionShape2D(center)
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
			: CollisionShape2D(rhs._center)
			, _vertices{ rhs._vertices }
		{
			for (Float2& vertex : _vertices)
			{
				vertex = transform2D * vertex;
			}
		}

		ConvexCollisionShape2D::ConvexCollisionShape2D()
			: CollisionShape2D(Float2::kZero)
		{
			__noop;
		}

		ConvexCollisionShape2D ConvexCollisionShape2D::MakeFromPoints(const Float2& center, const Vector<Float2>& points)
		{
			ConvexCollisionShape2D shape(center, points);
			GrahamScan_Convexify(shape._vertices);
			return shape;
		}

		ConvexCollisionShape2D ConvexCollisionShape2D::MakeFromAABBShape2D(const AABBCollisionShape2D& shape)
		{
			ConvexCollisionShape2D result;
			result._center = shape._center;
			result._vertices.PushBack(Float2(-shape._halfSize._x, +shape._halfSize._y));
			result._vertices.PushBack(Float2(-shape._halfSize._x, -shape._halfSize._y));
			result._vertices.PushBack(Float2(+shape._halfSize._x, -shape._halfSize._y));
			result._vertices.PushBack(Float2(+shape._halfSize._x, +shape._halfSize._y));
			return result;
		}

		ConvexCollisionShape2D ConvexCollisionShape2D::MakeFromBoxShape2D(const BoxCollisionShape2D& shape)
		{
			const Float2& halfX = shape.GetHalfLengthedAxisX();
			const Float2& halfY = shape.GetHalfLengthedAxisY();

			ConvexCollisionShape2D result;
			result._center = shape._center;
			result._vertices.PushBack(+halfX + +halfY);
			result._vertices.PushBack(-halfX + +halfY);
			result._vertices.PushBack(-halfX + -halfY);
			result._vertices.PushBack(+halfX + -halfY);
			return result;
		}

		ConvexCollisionShape2D ConvexCollisionShape2D::MakeFromCircleShape2D(const CircleCollisionShape2D& shape)
		{
			ConvexCollisionShape2D result;
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

		ConvexCollisionShape2D ConvexCollisionShape2D::MakeFromShape2D(const CollisionShape2D& shape)
		{
			if (shape.GetCollisionShapeType() == CollisionShapeType::Circle)
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
				return ConvexCollisionShape2D(center, Vector<Float2>());
			}

			Vector<Float2> points;
			points.Resize(vertexCount);
			for (uint32 i = 0; i < vertexCount; ++i)
			{
				points[i] = renderingShape._vertices[i]._position.GetXY();
			}
			ConvexCollisionShape2D shape(center, points);
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

		void ConvexCollisionShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Transform2D& transform2D) const
		{
			shapeRendererContext.SetColor(color);

			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(transform2D._rotation);
			const uint32 vertexCount = _vertices.Size();
			const Float2 center = _center + transform2D._translation;
			for (uint32 vertexIndex = 1; vertexIndex < vertexCount; ++vertexIndex)
			{
				shapeRendererContext.DrawLine(center + rotationMatrix * _vertices[vertexIndex - 1], center + rotationMatrix * _vertices[vertexIndex], 1.0f);
			}
			shapeRendererContext.DrawLine(center + rotationMatrix * _vertices[vertexCount - 1], center + rotationMatrix * _vertices[0], 1.0f);
		}

		Float2 ConvexCollisionShape2D::ComputeSupportPoint(const Float2& direction) const
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
#pragma endregion
	}
}