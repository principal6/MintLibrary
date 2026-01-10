#include <MintPhysics/Include/CollisionShape.h>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/SharedPtr.hpp>
#include <MintMath/Include/Float2x2.h>
#include <MintMath/Include/Geometry.h>
#include <MintMath/Include/Transform.h>
#include <MintRendering/Include/ShapeRenderer.h>


namespace mint
{
	namespace Physics2D
	{
#pragma region CollisionShape
		SharedPtr<CollisionShape> CollisionShape::MakeTransformed(const SharedPtr<CollisionShape>& shape, const Transform2D& transform2D)
		{
			if (transform2D.IsIdentity())
			{
				return shape;
			}

			switch (shape->GetCollisionShapeType())
			{
			case mint::Physics2D::CollisionShapeType::Point:
			{
				const PointCollisionShape& castedShape = static_cast<const PointCollisionShape&>(*shape);
				return MakeShared<PointCollisionShape>(PointCollisionShape(castedShape._center + transform2D._translation));
			}
			case mint::Physics2D::CollisionShapeType::Edge:
			{
				const EdgeCollisionShape& castedShape = static_cast<const EdgeCollisionShape&>(*shape);
				return MakeShared<EdgeCollisionShape>(EdgeCollisionShape(castedShape._vertexA, castedShape._vertexB, transform2D));
			}
			case mint::Physics2D::CollisionShapeType::Circle:
			{
				const CircleCollisionShape& castedShape = static_cast<const CircleCollisionShape&>(*shape);
				return MakeShared<CircleCollisionShape>(CircleCollisionShape(castedShape._center + transform2D._translation, castedShape._radius));
			}
			case mint::Physics2D::CollisionShapeType::AABB:
			{
				const AABBCollisionShape& castedShape = static_cast<const AABBCollisionShape&>(*shape);
				return MakeShared<AABBCollisionShape>(AABBCollisionShape(castedShape._center + transform2D._translation, castedShape._halfSize));
			}
			case mint::Physics2D::CollisionShapeType::Box:
			{
				const BoxCollisionShape& castedShape = static_cast<const BoxCollisionShape&>(*shape);
				const Float2x2 rotationMatrix{ Float2x2::RotationMatrix(transform2D._rotation) };
				const Float2 halfLengthedAxisX = rotationMatrix * castedShape.GetHalfLengthedAxisX();
				const Float2 halfLengthedAxisY = rotationMatrix * castedShape.GetHalfLengthedAxisY();
				return MakeShared<BoxCollisionShape>(BoxCollisionShape(castedShape._center + transform2D._translation, halfLengthedAxisX, halfLengthedAxisY));
			}
			case mint::Physics2D::CollisionShapeType::Convex:
			{
				const ConvexCollisionShape& castedShape = static_cast<const ConvexCollisionShape&>(*shape);
				return MakeShared<ConvexCollisionShape>(ConvexCollisionShape(castedShape, transform2D));
			}
			default:
				break;
			}
			MINT_NEVER;
			return SharedPtr<CollisionShape>();
		}
#pragma endregion

#pragma region CollisionShape - PointCollisionShape
		PointCollisionShape::PointCollisionShape(const Float2& center)
			: CollisionShape()
			, _center{ center }
		{
			__noop;
		}

		Float2 PointCollisionShape::ComputeSupportPoint(const Float2& direction) const
		{
			return _center;
		}

		void PointCollisionShape::ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const
		{
			outVertexA = _center;
			outVertexB = _center;

			Float2 tangent{ -direction._y, direction._x };
			outVertexA -= tangent;
			outVertexB += tangent;
			MINT_ASSERT(tangent.Dot(direction) == 0.0f, "!!!");
		}

		void PointCollisionShape::DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const
		{
			shapeRenderer.SetColor(color);
			shapeRenderer.DrawCircle(Float3(_center + transform2D._translation), 2.0f);
		}
#pragma endregion

#pragma region CollisionShape - EdgeCollisionShape
		EdgeCollisionShape::EdgeCollisionShape(const Float2& vertexA, const Float2& vertexB)
			: _vertexA{ vertexA }
			, _vertexB{ vertexB }
		{
			__noop;
		}

		EdgeCollisionShape::EdgeCollisionShape(const Float2& vertexA, const Float2& vertexB, const Transform2D& transform2D)
			: _vertexA{ transform2D * vertexA }
			, _vertexB{ transform2D * vertexB }
		{
			__noop;
		}

		Float2 EdgeCollisionShape::ComputeSupportPoint(const Float2& direction) const
		{
			Float2 directionBToA = _vertexA - _vertexB;
			directionBToA.Normalize();
			if (direction.Dot(directionBToA) > 0.0f)
			{
				return _vertexA;
			}
			return _vertexB;
		}

		void EdgeCollisionShape::ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const
		{
			// This is a sided edge!
			outVertexA = _vertexA;
			outVertexB = _vertexB;
		}

		void EdgeCollisionShape::DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const
		{
			shapeRenderer.SetColor(color);
			shapeRenderer.DrawLine(transform2D * _vertexA, transform2D * _vertexB, 4.0f);
		}
#pragma endregion

#pragma region CollisionShape - CircleCollisionShape
		CircleCollisionShape::CircleCollisionShape(const Float2& center, const float radius)
			: CollisionShape()
			, _center{ center }
			, _radius{ radius }
		{
			MINT_ASSERT(radius > 0.0f, "radius(%f) is 0 or less. IsPhase it really intended?", radius);
		}

		CircleCollisionShape::CircleCollisionShape(const float radius, const Transform2D& transform2D)
			: CircleCollisionShape(transform2D._translation, radius)
		{
			__noop;
		}

		Float2 CircleCollisionShape::ComputeSupportPoint(const Float2& direction) const
		{
			return _center + direction * _radius;
		}

		void CircleCollisionShape::ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const
		{
			outVertexA = ComputeSupportPoint(direction);
			outVertexB = outVertexA;

			Float2 tangent{ -direction._y, direction._x };
			outVertexA -= tangent;
			outVertexB += tangent;
			MINT_ASSERT(tangent.Dot(direction) == 0.0f, "!!!");
		}

		void CircleCollisionShape::DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const
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

#pragma region CollisionShape - AABBCollisionShape
		AABBCollisionShape::AABBCollisionShape(const Float2& center, const Float2& halfSize)
			: CollisionShape()
			, _center{ center }
			, _halfSize{ halfSize }
		{
			__noop;
		}

		AABBCollisionShape::AABBCollisionShape(const CollisionShape& collisionShape)
			: CollisionShape(collisionShape)
		{
			if (collisionShape.GetCollisionShapeType() == CollisionShapeType::Edge)
			{
				const EdgeCollisionShape& edgeCollisionShape = static_cast<const EdgeCollisionShape&>(collisionShape);
				_center = (edgeCollisionShape._vertexA + edgeCollisionShape._vertexB) * 0.5f;
				_halfSize = (edgeCollisionShape._vertexA - edgeCollisionShape._vertexB) * 0.5f;
				_halfSize._x = Max(::abs(_halfSize._x), 1.0f);
				_halfSize._y = Max(::abs(_halfSize._y), 1.0f);
			}
			else if (collisionShape.GetCollisionShapeType() == CollisionShapeType::AABB)
			{
				const AABBCollisionShape& aabbCollisionShape = static_cast<const AABBCollisionShape&>(collisionShape);
				_center = aabbCollisionShape._center;
				_halfSize = aabbCollisionShape._halfSize;
			}
			else if (collisionShape.GetCollisionShapeType() == CollisionShapeType::Circle)
			{
				const CircleCollisionShape& circleCollisionShape = static_cast<const CircleCollisionShape&>(collisionShape);
				_center = circleCollisionShape._center;
				_halfSize = Float2(circleCollisionShape._radius, circleCollisionShape._radius);
			}
			else if (collisionShape.GetCollisionShapeType() == CollisionShapeType::Box)
			{
				const BoxCollisionShape& boxCollisionShape = static_cast<const BoxCollisionShape&>(collisionShape);
				_center = boxCollisionShape._center;
				const Float2 max = _center + boxCollisionShape.GetHalfLengthedAxisX() + boxCollisionShape.GetHalfLengthedAxisY();
				const Float2 min = _center - boxCollisionShape.GetHalfLengthedAxisX() - boxCollisionShape.GetHalfLengthedAxisY();

				const Float2 center = (max + min) * 0.5f;
				const Float2 size = max - min;
				_center = center;
				_halfSize = size * 0.5f;
			}
			else if (collisionShape.GetCollisionShapeType() == CollisionShapeType::Convex)
			{
				const ConvexCollisionShape& convexCollisionShape = static_cast<const ConvexCollisionShape&>(collisionShape);
				const Vector<Float2>& vertices = convexCollisionShape.GetVertices();
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

		AABBCollisionShape::AABBCollisionShape(const AABBCollisionShape& aabbCollisionShape, const Transform2D& transform2D)
			: CollisionShape(aabbCollisionShape)
		{
			Set(aabbCollisionShape, transform2D);
		}

		void AABBCollisionShape::Set(const AABBCollisionShape& aabbCollisionShape, const Transform2D& transform2D)
		{
			_center = aabbCollisionShape._center + transform2D._translation;
			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(transform2D._rotation);
			const Float2& x = rotationMatrix._row[0];
			const Float2& y = rotationMatrix._row[1];
			const Float2 rotatedHalfSizeX = x * aabbCollisionShape._halfSize._x;
			const Float2 rotatedHalfSizeY = y * aabbCollisionShape._halfSize._y;
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

		void AABBCollisionShape::SetExpanded(const AABBCollisionShape& aabbCollisionShape, const Transform2D& transform2D, const Float2& displacement)
		{
			Set(aabbCollisionShape, transform2D);

			const Float2& position0 = aabbCollisionShape._center + transform2D._translation;
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

		void AABBCollisionShape::SetExpandedByRadius(const AABBCollisionShape& aabbCollisionShape, const Transform2D& transform2D, const Float2& displacement)
		{
			const float radiusSq = aabbCollisionShape._halfSize._x * aabbCollisionShape._halfSize._x + aabbCollisionShape._halfSize._y * aabbCollisionShape._halfSize._y;
			const float radius = ::sqrt(radiusSq);
			const Float2 position0 = aabbCollisionShape._center + transform2D._translation;
			SetExpandedByRadius(radius, position0, displacement);
		}

		void AABBCollisionShape::SetExpandedByRadius(float radius, const Float2& center, const Float2& displacement)
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

		Float2 AABBCollisionShape::ComputeSupportPoint(const Float2& direction) const
		{
			if (direction._x >= 0.0f)
			{
				return _center + Float2(_halfSize._x, (direction._y >= 0.0f ? +_halfSize._y : -_halfSize._y));
			}
			return _center + Float2(-_halfSize._x, (direction._y >= 0.0f ? +_halfSize._y : -_halfSize._y));
		}

		void AABBCollisionShape::ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const
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

		void AABBCollisionShape::DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const
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

#pragma region CollisionShape - BoxCollisionShape
		BoxCollisionShape::BoxCollisionShape(const Float2& halfSize, const Transform2D& transform2D)
			: CollisionShape()
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

		BoxCollisionShape::BoxCollisionShape(const Float2& center, const Float2& halfLengthedAxisX, const Float2& halfLengthedAxisY)
			: CollisionShape()
			, _center{ center }
			, _halfLengthedAxisX{ halfLengthedAxisX }
			, _halfLengthedAxisY{ halfLengthedAxisY }
		{
			__noop;
		}

		Float2 BoxCollisionShape::ComputeSupportPoint(const Float2& direction) const
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

		void BoxCollisionShape::ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const
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

		void BoxCollisionShape::DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const
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

#pragma region CollisionShape - ConvexCollisionShape
		ConvexCollisionShape::ConvexCollisionShape(const Vector<Float2>& vertices)
			: CollisionShape()
			, _vertices{ vertices }
		{
			__noop;
		}

		ConvexCollisionShape::ConvexCollisionShape(const Vector<Float2>& vertices, const Transform2D& transform2D)
			: CollisionShape()
			, _vertices{ vertices }
		{
			for (Float2& vertex : _vertices)
			{
				vertex = transform2D * vertex;
			}
		}

		ConvexCollisionShape::ConvexCollisionShape(const ConvexCollisionShape& rhs, const Transform2D& transform2D)
			: CollisionShape()
			, _vertices{ rhs._vertices }
		{
			for (Float2& vertex : _vertices)
			{
				vertex = transform2D * vertex;
			}
		}

		ConvexCollisionShape::ConvexCollisionShape()
			: CollisionShape()
		{
			__noop;
		}

		ConvexCollisionShape ConvexCollisionShape::MakeFromPoints(const Vector<Float2>& points)
		{
			ConvexCollisionShape shape(points);
			GrahamScan_Convexify(shape._vertices);
			return shape;
		}

		ConvexCollisionShape ConvexCollisionShape::MakeFromRenderingShape(const Float2& center, const Rendering::Shape& renderingShape)
		{
			const uint32 vertexCount = renderingShape._vertices.Size();
			if (vertexCount == 0)
			{
				return ConvexCollisionShape(Vector<Float2>());
			}

			Vector<Float2> points;
			points.Resize(vertexCount);
			for (uint32 i = 0; i < vertexCount; ++i)
			{
				points[i] = center + renderingShape._vertices[i]._position.GetXY();
			}
			ConvexCollisionShape shape(points);
			GrahamScan_Convexify(shape._vertices);
			return shape;
		}

		ConvexCollisionShape ConvexCollisionShape::MakeMinkowskiDifferenceShape(const CollisionShape& a, const CollisionShape& b)
		{
			constexpr const uint32 kSampleCount = 128;
			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(Math::kTwoPi / static_cast<float>(kSampleCount));
			ConvexCollisionShape shape;
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

		ConvexCollisionShape ConvexCollisionShape::MakeFromCollisionShape(const CollisionShape& shape)
		{
			if (shape.GetCollisionShapeType() == CollisionShapeType::Convex)
			{
				return static_cast<const ConvexCollisionShape&>(shape);
			}
			else if (shape.GetCollisionShapeType() == CollisionShapeType::Circle)
			{
				return MakeFromCircleShape(static_cast<const CircleCollisionShape&>(shape));
			}
			else if (shape.GetCollisionShapeType() == CollisionShapeType::Box)
			{
				return MakeFromBoxShape(static_cast<const BoxCollisionShape&>(shape));
			}
			else if (shape.GetCollisionShapeType() == CollisionShapeType::AABB)
			{
				return MakeFromAABBShape(static_cast<const AABBCollisionShape&>(shape));
			}
			else
			{
				MINT_NEVER;
			}
			return ConvexCollisionShape();
		}

		ConvexCollisionShape ConvexCollisionShape::MakeFromAABBShape(const AABBCollisionShape& shape)
		{
			ConvexCollisionShape result;
			result._vertices.PushBack(shape._center + Float2(-shape._halfSize._x, +shape._halfSize._y));
			result._vertices.PushBack(shape._center + Float2(-shape._halfSize._x, -shape._halfSize._y));
			result._vertices.PushBack(shape._center + Float2(+shape._halfSize._x, -shape._halfSize._y));
			result._vertices.PushBack(shape._center + Float2(+shape._halfSize._x, +shape._halfSize._y));
			return result;
		}

		ConvexCollisionShape ConvexCollisionShape::MakeFromBoxShape(const BoxCollisionShape& shape)
		{
			const Float2& halfX = shape.GetHalfLengthedAxisX();
			const Float2& halfY = shape.GetHalfLengthedAxisY();

			ConvexCollisionShape result;
			result._vertices.PushBack(shape._center + +halfX + +halfY);
			result._vertices.PushBack(shape._center + -halfX + +halfY);
			result._vertices.PushBack(shape._center + -halfX + -halfY);
			result._vertices.PushBack(shape._center + +halfX + -halfY);
			return result;
		}

		ConvexCollisionShape ConvexCollisionShape::MakeFromCircleShape(const CircleCollisionShape& shape)
		{
			ConvexCollisionShape result;
			for (uint32 i = 0; i <= 32; i++)
			{
				float theta = (static_cast<float>(i) / 32.0f) * Math::kTwoPi;
				const float x = ::cos(theta) * shape._radius;
				const float y = ::sin(theta) * shape._radius;
				result._vertices.PushBack(shape._center + Float2(x, y));
			}
			return result;
		}

		Float2 ConvexCollisionShape::ComputeSupportPoint(const Float2& direction) const
		{
			if (_vertices.IsEmpty())
			{
				return Float2::kZero;
			}

			uint32 targetVertexIndex = 0;
			ComputeSupportPointIndex(direction, targetVertexIndex);
			return _vertices[targetVertexIndex];
		}

		void ConvexCollisionShape::ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const
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

		void ConvexCollisionShape::ComputeSupportPointIndex(const Float2& direction, uint32& outIndex) const
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

		void ConvexCollisionShape::DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const
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

			shapeRenderer.DrawCircle(Float3(center), 4.0f);
		}
#pragma endregion

#pragma region CollisionShape - CompositeCollisionShape
		CompositeCollisionShape::CompositeCollisionShape(const Vector<ShapeInstance>& shapeInstances)
			: _shapeInstances{ shapeInstances }
		{
			__noop;
		}

		Float2 CompositeCollisionShape::ComputeSupportPoint(const Float2& direction) const
		{
			MINT_NEVER;
			return Float2::kZero;
		}

		void CompositeCollisionShape::ComputeSupportEdge(const Float2& direction, Float2& outVertexA, Float2& outVertexB) const
		{
			MINT_NEVER;
		}

		void CompositeCollisionShape::DebugDrawShape(ShapeRenderer& shapeRenderer, const ByteColor& color, const Transform2D& transform2D) const
		{
			for (const ShapeInstance& shapeInstance : _shapeInstances)
			{
				shapeInstance._shape->DebugDrawShape(shapeRenderer, color, transform2D * shapeInstance._transform2D);
			}
		}
#pragma endregion
	}
}