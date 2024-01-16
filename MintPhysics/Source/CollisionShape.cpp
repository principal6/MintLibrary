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

		void PointCollisionShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) const
		{
			shapeRendererContext.SetColor(color);
			shapeRendererContext.SetPosition(Float4(_center + offset));
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

		void CircleCollisionShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) const
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

		void AABBCollisionShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) const
		{
			shapeRendererContext.SetColor(color);

			const Float2 halfSizeX = Float2(_halfSize._x, 0.0f);
			const Float2 halfSizeY = Float2(0.0f, _halfSize._y);
			shapeRendererContext.DrawLine(offset + _center - halfSizeX + halfSizeY, offset + _center + halfSizeX + halfSizeY, 1.0f);
			shapeRendererContext.DrawLine(offset + _center - halfSizeX - halfSizeY, offset + _center + halfSizeX - halfSizeY, 1.0f);
			shapeRendererContext.DrawLine(offset + _center + halfSizeX + halfSizeY, offset + _center + halfSizeX - halfSizeY, 1.0f);
			shapeRendererContext.DrawLine(offset + _center - halfSizeX + halfSizeY, offset + _center - halfSizeX - halfSizeY, 1.0f);
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
			, _halfSize{ halfSize }
			, _angle{ angle }
		{
			__noop;
		}

		void BoxCollisionShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) const
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

		Float2 BoxCollisionShape2D::ComputeSupportPoint(const Float2& direction) const
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

		ConvexCollisionShape2D ConvexCollisionShape2D::MakeFromBoxShape2D(const BoxCollisionShape2D& shape)
		{
			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(shape._angle);
			const Float2& i = rotationMatrix._row[0];
			const Float2& j = rotationMatrix._row[1];

			ConvexCollisionShape2D result;
			result._center = shape._center;
			result._vertices.PushBack(+i * shape._halfSize._x + +j * shape._halfSize._y);
			result._vertices.PushBack(-i * shape._halfSize._x + +j * shape._halfSize._y);
			result._vertices.PushBack(-i * shape._halfSize._x + -j * shape._halfSize._y);
			result._vertices.PushBack(+i * shape._halfSize._x + -j * shape._halfSize._y);
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

		void ConvexCollisionShape2D::DebugDrawShape(ShapeRendererContext& shapeRendererContext, const ByteColor& color, const Float2& offset) const
		{
			shapeRendererContext.SetColor(color);

			const uint32 vertexCount = _vertices.Size();
			for (uint32 vertexIndex = 1; vertexIndex < vertexCount; ++vertexIndex)
			{
				shapeRendererContext.DrawLine(_center + offset + _vertices[vertexIndex - 1], _center + offset + _vertices[vertexIndex], 1.0f);
			}
			shapeRendererContext.DrawLine(_center + offset + _vertices[vertexCount - 1], _center + offset + _vertices[0], 1.0f);
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