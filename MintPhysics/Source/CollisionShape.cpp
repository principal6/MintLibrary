#include <MintPhysics/Include/CollisionShape.h>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/SharedPtr.hpp>
#include <MintMath/Include/Float2x2.h>
#include <MintMath/Include/Geometry.h>
#include <MintMath/Include/Transform.h>
#include <MintRenderingBase/Include/ShapeRenderer.h>


namespace mint
{
	namespace Physics
	{
#pragma region CollisionShape2D
		SharedPtr<CollisionShape2D> CollisionShape2D::MakeTransformed(const SharedPtr<CollisionShape2D>& shape, const Transform2D& transform2D)
		{
			if (transform2D.IsIdentity())
			{
				return shape;
			}

			switch (shape->GetCollisionShapeType())
			{
			case mint::Physics::CollisionShapeType::Point:
			{
				const PointCollisionShape2D& castedShape = static_cast<const PointCollisionShape2D&>(*shape);
				return MakeShared<PointCollisionShape2D>(PointCollisionShape2D(castedShape._center + transform2D._translation));
			}
			case mint::Physics::CollisionShapeType::Edge:
			{
				const EdgeCollisionShape2D& castedShape = static_cast<const EdgeCollisionShape2D&>(*shape);
				return MakeShared<EdgeCollisionShape2D>(EdgeCollisionShape2D(castedShape._vertexA, castedShape._vertexB, transform2D));
			}
			case mint::Physics::CollisionShapeType::Circle:
			{
				const CircleCollisionShape2D& castedShape = static_cast<const CircleCollisionShape2D&>(*shape);
				return MakeShared<CircleCollisionShape2D>(CircleCollisionShape2D(castedShape._center + transform2D._translation, castedShape._radius));
			}
			case mint::Physics::CollisionShapeType::AABB:
			{
				const AABBCollisionShape2D& castedShape = static_cast<const AABBCollisionShape2D&>(*shape);
				return MakeShared<AABBCollisionShape2D>(AABBCollisionShape2D(castedShape._center + transform2D._translation, castedShape._halfSize));
			}
			case mint::Physics::CollisionShapeType::Box:
			{
				const BoxCollisionShape2D& castedShape = static_cast<const BoxCollisionShape2D&>(*shape);
				const Float2x2 rotationMatrix{ Float2x2::RotationMatrix(transform2D._rotation) };
				const Float2 halfLengthedAxisX = rotationMatrix * castedShape.GetHalfLengthedAxisX();
				const Float2 halfLengthedAxisY = rotationMatrix * castedShape.GetHalfLengthedAxisY();
				return MakeShared<BoxCollisionShape2D>(BoxCollisionShape2D(castedShape._center + transform2D._translation, halfLengthedAxisX, halfLengthedAxisY));
			}
			case mint::Physics::CollisionShapeType::Convex:
			{
				const ConvexCollisionShape2D& castedShape = static_cast<const ConvexCollisionShape2D&>(*shape);
				return MakeShared<ConvexCollisionShape2D>(ConvexCollisionShape2D(castedShape, transform2D));
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

		void PointCollisionShape2D::DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const
		{
			shapeRenderer.SetColor(color);
			shapeRenderer.SetPosition(Float4(_center + transform2D._translation));
			shapeRenderer.DrawCircle(2.0f);
		}
#pragma endregion

#pragma region CollisionShape2D - EdgeCollisionShape2D
		EdgeCollisionShape2D::EdgeCollisionShape2D(const Float2& vertexA, const Float2& vertexB)
			: _vertexA{ vertexA }
			, _vertexB{ vertexB }
		{
			__noop;
		}

		EdgeCollisionShape2D::EdgeCollisionShape2D(const Float2& vertexA, const Float2& vertexB, const Transform2D& transform2D)
			: _vertexA{ transform2D * vertexA }
			, _vertexB{ transform2D * vertexB }
		{
			__noop;
		}

		Float2 EdgeCollisionShape2D::ComputeSupportPoint(const Float2& direction) const
		{
			Float2 directionBToA = _vertexA - _vertexB;
			directionBToA.Normalize();
			if (direction.Dot(directionBToA) > 0.0f)
			{
				return _vertexA;
			}
			return _vertexB;
		}

		void EdgeCollisionShape2D::ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const
		{
			// This is a sided edge!
			outVertexA = _vertexA;
			outVertexB = _vertexB;
		}

		void EdgeCollisionShape2D::DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const
		{
			shapeRenderer.SetColor(color);
			shapeRenderer.DrawLine(transform2D * _vertexA, transform2D * _vertexB, 4.0f);
		}
#pragma endregion

#pragma region CollisionShape2D - CircleCollisionShape2D
		CircleCollisionShape2D::CircleCollisionShape2D(const Float2& center, const float radius)
			: CollisionShape2D()
			, _center{ center }
			, _radius{ radius }
		{
			MINT_ASSERT(radius > 0.0f, "radius(%f) is 0 or less. Is it really intended?", radius);
		}

		CircleCollisionShape2D::CircleCollisionShape2D(const float radius, const Transform2D& transform2D)
			: CircleCollisionShape2D(transform2D._translation, radius)
		{
			__noop;
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

		void CircleCollisionShape2D::DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const
		{
			shapeRenderer.SetColor(color);

			const uint32 kSideCount = 32;
			const float kThetaUnit = Math::kTwoPi / kSideCount;
			for (uint32 sideIndex = 1; sideIndex <= kSideCount; ++sideIndex)
			{
				const float thetaA = kThetaUnit * (sideIndex - 1);
				const float thetaB = kThetaUnit * sideIndex;
				const Float2 pointA = Float2(::cos(thetaA) * _radius, -::sin(thetaA) * _radius);
				const Float2 pointB = Float2(::cos(thetaB) * _radius, -::sin(thetaB) * _radius);
				shapeRenderer.DrawLine(transform2D._translation + _center + pointA, transform2D._translation + _center + pointB, 1.0f);
			}
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
			if (collisionShape2D.GetCollisionShapeType() == CollisionShapeType::Edge)
			{
				const EdgeCollisionShape2D& edgeCollisionShape2D = static_cast<const EdgeCollisionShape2D&>(collisionShape2D);
				_center = (edgeCollisionShape2D._vertexA + edgeCollisionShape2D._vertexB) * 0.5f;
				_halfSize = (edgeCollisionShape2D._vertexA - edgeCollisionShape2D._vertexB) * 0.5f;
				_halfSize._x = Max(::abs(_halfSize._x), 1.0f);
				_halfSize._y = Max(::abs(_halfSize._y), 1.0f);
			}
			else if (collisionShape2D.GetCollisionShapeType() == CollisionShapeType::AABB)
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

		void AABBCollisionShape2D::SetExpanded(const AABBCollisionShape2D& aabbCollisionShape2D, const Transform2D& transform2D, const Float2& displacement)
		{
			Set(aabbCollisionShape2D, transform2D);

			const Float2& position0 = aabbCollisionShape2D._center + transform2D._translation;
			const Float2 position1 = position0 + displacement;
			Float2 min = _center - _halfSize;
			Float2 max = _center + _halfSize;
			if (displacement._x > 0.0f)
			{
				max._x += displacement._x;
			}
			else
			{
				min._x += displacement._x;
			}
			if (displacement._y > 0.0f)
			{
				max._y += displacement._y;
			}
			else
			{
				min._y += displacement._y;
			}
			_center = (min + max) * 0.5f;
			_halfSize = (max - min) * 0.5f;
		}

		void AABBCollisionShape2D::SetExpandedByRadius(const AABBCollisionShape2D& aabbCollisionShape2D, const Transform2D& transform2D, const Float2& displacement)
		{
			const float radiusSq = aabbCollisionShape2D._halfSize._x * aabbCollisionShape2D._halfSize._x + aabbCollisionShape2D._halfSize._y * aabbCollisionShape2D._halfSize._y;
			const float radius = ::sqrt(radiusSq);
			const Float2 position0 = aabbCollisionShape2D._center + transform2D._translation;
			SetExpandedByRadius(radius, position0, displacement);
		}

		void AABBCollisionShape2D::SetExpandedByRadius(float radius, const Float2& center, const Float2& displacement)
		{
			const Float2& position0 = center;
			const Float2 position1 = position0 + displacement;
			Float2 min;
			Float2 max;
			if (position0._x < position1._x)
			{
				min._x = position0._x;
				max._x = position1._x;
			}
			else
			{
				min._x = position1._x;
				max._x = position0._x;
			}
			if (position0._y < position1._y)
			{
				min._y = position0._y;
				max._y = position1._y;
			}
			else
			{
				min._y = position1._y;
				max._y = position0._y;
			}

			_center = min;
			_center += max;
			_center *= 0.5f;

			_halfSize = max;
			_halfSize -= min;
			_halfSize *= 0.5f;
			_halfSize._x += radius;
			_halfSize._y += radius;
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

		void AABBCollisionShape2D::DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const
		{
			shapeRenderer.SetColor(color);

			const Float2 halfSizeX = Float2(_halfSize._x, 0.0f);
			const Float2 halfSizeY = Float2(0.0f, _halfSize._y);
			shapeRenderer.DrawLine(transform2D._translation + _center - halfSizeX + halfSizeY, transform2D._translation + _center + halfSizeX + halfSizeY, 1.0f);
			shapeRenderer.DrawLine(transform2D._translation + _center - halfSizeX - halfSizeY, transform2D._translation + _center + halfSizeX - halfSizeY, 1.0f);
			shapeRenderer.DrawLine(transform2D._translation + _center + halfSizeX + halfSizeY, transform2D._translation + _center + halfSizeX - halfSizeY, 1.0f);
			shapeRenderer.DrawLine(transform2D._translation + _center - halfSizeX + halfSizeY, transform2D._translation + _center - halfSizeX - halfSizeY, 1.0f);
		}
#pragma endregion

#pragma region CollisionShape2D - BoxCollisionShape2D
		BoxCollisionShape2D::BoxCollisionShape2D(const Float2& halfSize, const Transform2D& transform2D)
			: CollisionShape2D()
			, _center{ transform2D._translation }
			, _halfLengthedAxisX{ Float2::kZero }
			, _halfLengthedAxisY{ Float2::kZero }
		{
			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(transform2D._rotation);
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

		void BoxCollisionShape2D::DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const
		{
			shapeRenderer.SetColor(color);

			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(transform2D._rotation);
			const Float2 halfSizeX = rotationMatrix * GetHalfLengthedAxisX();
			const Float2 halfSizeY = rotationMatrix * GetHalfLengthedAxisY();
			const Float2 center = transform2D._translation + _center;
			shapeRenderer.DrawLine(center + halfSizeX + halfSizeY, center - halfSizeX + halfSizeY, 1.0f);
			shapeRenderer.DrawLine(center - halfSizeX + halfSizeY, center - halfSizeX - halfSizeY, 1.0f);
			shapeRenderer.DrawLine(center - halfSizeX - halfSizeY, center + halfSizeX - halfSizeY, 1.0f);
			shapeRenderer.DrawLine(center + halfSizeX - halfSizeY, center + halfSizeX + halfSizeY, 1.0f);
		}
#pragma endregion

#pragma region CollisionShape2D - ConvexCollisionShape2D
		ConvexCollisionShape2D::ConvexCollisionShape2D(const Vector<Float2>& vertices)
			: CollisionShape2D()
			, _vertices{ vertices }
		{
			__noop;
		}

		ConvexCollisionShape2D::ConvexCollisionShape2D(const Vector<Float2>& vertices, const Transform2D& transform2D)
			: CollisionShape2D()
			, _vertices{ vertices }
		{
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
			constexpr const uint32 kSampleCount = 128;
			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(Math::kTwoPi / static_cast<float>(kSampleCount));
			ConvexCollisionShape2D shape;
			shape._vertices.Clear();
			Float2 direction = Float2(1, 0);
			for (uint32 i = 0; i < kSampleCount; ++i)
			{
				shape._vertices.PushBack(a.ComputeSupportPoint(direction) - b.ComputeSupportPoint(-direction));
				direction = rotationMatrix * direction;
			}
			GrahamScan_Convexify(shape._vertices);
			return shape;
		}

		ConvexCollisionShape2D ConvexCollisionShape2D::MakeFromCollisionShape2D(const CollisionShape2D& shape)
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

		void ConvexCollisionShape2D::DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const
		{
			shapeRenderer.SetColor(color);

			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(transform2D._rotation);
			const uint32 vertexCount = _vertices.Size();
			const Float2 center = transform2D._translation;
			for (uint32 vertexIndex = 1; vertexIndex < vertexCount; ++vertexIndex)
			{
				shapeRenderer.DrawLine(center + rotationMatrix * _vertices[vertexIndex - 1], center + rotationMatrix * _vertices[vertexIndex], 1.0f);
			}
			shapeRenderer.DrawLine(center + rotationMatrix * _vertices[vertexCount - 1], center + rotationMatrix * _vertices[0], 1.0f);

			shapeRenderer.SetPosition(Float4(center));
			shapeRenderer.DrawCircle(4.0f);
		}
#pragma endregion

#pragma region CollisionShape2D - CompositeCollisionShape2D
		CompositeCollisionShape2D::CompositeCollisionShape2D(const Vector<ShapeInstance>& shapeInstances)
			: _shapeInstances{ shapeInstances }
		{
			__noop;
		}

		Float2 CompositeCollisionShape2D::ComputeSupportPoint(const Float2& direction) const
		{
			MINT_NEVER;
			return Float2::kZero;
		}

		void CompositeCollisionShape2D::ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const
		{
			MINT_NEVER;
		}

		void CompositeCollisionShape2D::DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const
		{
			for (const ShapeInstance& shapeInstance : _shapeInstances)
			{
				shapeInstance._shape->DebugDrawShape(shapeRenderer, color, transform2D * shapeInstance._transform2D);
			}
		}
#pragma endregion
	}
}