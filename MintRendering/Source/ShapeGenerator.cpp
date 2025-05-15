#include <MintRendering/Include/ShapeGenerator.h>
#include <MintMath/Include/Float2x2.h>
#include <MintMath/Include/Geometry.h>
#include <Assets/Include/CppHlsl/CppHlslStreamData.h>


namespace mint
{
	namespace Rendering
	{
#pragma region ScopedShapeTransformer
		ScopedShapeTransformer::ScopedShapeTransformer(Vector<VS_INPUT_SHAPE>& vertices, const Transform2D& shapeTransform)
			: _vertices{ vertices }
			, _shapeTransform{ shapeTransform }
		{
			_shapeVertexOffset = _vertices.Size();
		}

		ScopedShapeTransformer::~ScopedShapeTransformer()
		{
			const uint32 shapeVertexCount = _vertices.Size();
			if (_shapeVertexOffset == shapeVertexCount)
			{
				return;
			}

			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(_shapeTransform._rotation);
			Float2 rotatedXY;
			for (uint32 i = _shapeVertexOffset; i < shapeVertexCount; ++i)
			{
				VS_INPUT_SHAPE& vertex = _vertices[i];
				rotatedXY = rotationMatrix.Mul(Float2(vertex._position._x, vertex._position._y));
				vertex._position._x = rotatedXY._x;
				vertex._position._y = rotatedXY._y;

				vertex._position._x += _shapeTransform._translation._x;
				vertex._position._y += _shapeTransform._translation._y;
			}
		}
#pragma endregion


		void ShapeGenerator::GenerateCircle(float radius, uint8 sideCount, const Color& color, Vector<VS_INPUT_SHAPE>& vertices, Vector<IndexElementType>& indices, const Transform2D& shapeTransform)
		{
			MINT_ASSERT(radius > 0.0f, "radius must be greater than 0");
			MINT_ASSERT(sideCount > 2, "sideCount must be greater than 2");

			sideCount = Max(sideCount, static_cast<uint8>(3));

			const uint32 vertexBase = vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = color;
			v._position._w = 1.0f;

			v._position._x = shapeTransform._translation._x;
			v._position._y = shapeTransform._translation._y;
			vertices.PushBack(v);

			for (uint8 i = 0; i < sideCount; ++i)
			{
				const float theta = (static_cast<float>(i) / sideCount) * Math::kTwoPi;
				v._position._x = shapeTransform._translation._x + ::cos(theta) * radius;
				v._position._y = shapeTransform._translation._y + ::sin(theta) * radius;
				vertices.PushBack(v);
			}

			//     3
			//  4     2
			// 5   0   1
			//  6     8
			//     7
			for (uint8 i = 0; i < sideCount - 1; ++i)
			{
				indices.PushBack(vertexBase + 0);
				indices.PushBack(vertexBase + i + 1);
				indices.PushBack(vertexBase + i + 2);
			}
			indices.PushBack(vertexBase + 0);
			indices.PushBack(vertexBase + sideCount);
			indices.PushBack(vertexBase + 1);
		}

		void ShapeGenerator::GenerateHalfCircle(float radius, uint8 sideCount, const Color& color, Vector<VS_INPUT_SHAPE>& vertices, Vector<IndexElementType>& indices, const Transform2D& shapeTransform)
		{
			MINT_ASSERT(radius > 0.0f, "radius must be greater than 0");
			MINT_ASSERT(sideCount > 2, "sideCount must be greater than 2");

			sideCount = Max(sideCount, static_cast<uint8>(3));

			const uint32 vertexBase = vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = color;
			v._position._w = 1.0f;

			v._position._x = shapeTransform._translation._x;
			v._position._y = shapeTransform._translation._y;
			vertices.PushBack(v);

			for (uint8 i = 0; i <= sideCount; ++i)
			{
				const float theta = (static_cast<float>(i) / sideCount) * Math::kPi;
				v._position._x = shapeTransform._translation._x + ::cos(shapeTransform._rotation + theta) * radius;
				v._position._y = shapeTransform._translation._y + ::sin(shapeTransform._rotation + theta) * radius;
				vertices.PushBack(v);
			}

			//     3
			//  4     2
			// 5   0   1
			for (uint8 i = 0; i < sideCount; ++i)
			{
				indices.PushBack(vertexBase + 0);
				indices.PushBack(vertexBase + i + 1);
				indices.PushBack(vertexBase + i + 2);
			}
		}

		void ShapeGenerator::GenerateQuarterCircle(float radius, uint8 sideCount, const Color& color, Vector<VS_INPUT_SHAPE>& vertices, Vector<IndexElementType>& indices, const Transform2D& shapeTransform)
		{
			MINT_ASSERT(radius > 0.0f, "radius must be greater than 0");
			MINT_ASSERT(sideCount > 0, "sideCount must be greater than 0");

			sideCount = Max(sideCount, static_cast<uint8>(1));

			const uint32 vertexBase = vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = color;
			v._position._w = 1.0f;

			v._position._x = shapeTransform._translation._x;
			v._position._y = shapeTransform._translation._y;
			vertices.PushBack(v);

			for (uint8 i = 0; i <= sideCount; ++i)
			{
				const float theta = (static_cast<float>(i) / sideCount) * Math::kPiOverTwo;
				v._position._x = shapeTransform._translation._x + ::cos(shapeTransform._rotation + theta) * radius;
				v._position._y = shapeTransform._translation._y + ::sin(shapeTransform._rotation + theta) * radius;
				vertices.PushBack(v);
			}

			// 3
			//    2
			// 0   1
			for (uint8 i = 0; i < sideCount; ++i)
			{
				indices.PushBack(vertexBase + 0);
				indices.PushBack(vertexBase + i + 1);
				indices.PushBack(vertexBase + i + 2);
			}
		}

		void ShapeGenerator::GenerateRectangle(const Float2& size, const Color& color, Vector<VS_INPUT_SHAPE>& vertices, Vector<IndexElementType>& indices, const Transform2D& shapeTransform)
		{
			//MINT_ASSERT(size._x > 0.0f, "size._x must be greater than 0");
			//MINT_ASSERT(size._y > 0.0f, "size._y must be greater than 0");

			const uint32 vertexBase = vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = color;
			v._position._w = 1.0f;

			const ScopedShapeTransformer scopedShapeTransformer{ vertices, shapeTransform };
			const Float2 halfSize = size * 0.5f;
			v._position._x = -halfSize._x;
			v._position._y = +halfSize._y;
			vertices.PushBack(v);

			v._position._x = -halfSize._x;
			v._position._y = -halfSize._y;
			vertices.PushBack(v);

			v._position._x = +halfSize._x;
			v._position._y = -halfSize._y;
			vertices.PushBack(v);

			v._position._x = +halfSize._x;
			v._position._y = +halfSize._y;
			vertices.PushBack(v);

			indices.PushBack(vertexBase + 0);
			indices.PushBack(vertexBase + 1);
			indices.PushBack(vertexBase + 2);

			indices.PushBack(vertexBase + 0);
			indices.PushBack(vertexBase + 2);
			indices.PushBack(vertexBase + 3);
		}

		void ShapeGenerator::GenerateRoundRectangle(const Float2& size, float roundness, uint8 roundSideCount, const Color& color, Vector<VS_INPUT_SHAPE>& vertices, Vector<IndexElementType>& indices, const Transform2D& shapeTransform)
		{
			MINT_ASSERT(size._x > 0.0f, "size._x must be greater than 0");
			MINT_ASSERT(size._y > 0.0f, "size._y must be greater than 0");
			MINT_ASSERT(roundness >= 0.0f, "roundness must be equal or greater than 0");
			MINT_ASSERT(roundSideCount > 0, "roundSideCount must be greater than 0");

			roundness = Math::Saturate(roundness);
			const float radius = roundness * size.GetMinElement() * 0.5f;
			if (radius == 0.0f)
			{
				GenerateRectangle(size, color, vertices, indices, shapeTransform);
				return;
			}
			roundSideCount = Max(roundSideCount, static_cast<uint8>(1));

			const uint32 vertexBase = vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = color;
			v._position._w = 1.0f;

			const float middleRectangleHeight = size._y - radius * 2.0f;
			const float topBottomRectangleWidth = size._x - radius * 2.0f;
			if (topBottomRectangleWidth == 0.0f && middleRectangleHeight == 0.0f)
			{
				GenerateCircle(radius, roundSideCount * 4, color, vertices, indices, shapeTransform);
				return;
			}

			const Float2 halfSize = size * 0.5f;
			if (middleRectangleHeight == 0.0f)
			{
				GenerateRectangle(Float2(size._x - radius * 2.0f, size._y), color, vertices, indices, shapeTransform);
				GenerateHalfCircle(radius, roundSideCount * 2, color, vertices, indices, shapeTransform * Transform2D(Math::kPiOverTwo, Float2(-halfSize._x + radius, 0.0f)));
				GenerateHalfCircle(radius, roundSideCount * 2, color, vertices, indices, shapeTransform * Transform2D(-Math::kPiOverTwo, Float2(halfSize._x - radius, 0.0f)));
				return;
			}

			// Middle
			GenerateRectangle(Float2(size._x, middleRectangleHeight), color, vertices, indices, shapeTransform);

			if (topBottomRectangleWidth > 0.0f)
			{
				// Top
				GenerateRectangle(Float2(topBottomRectangleWidth, radius), color, vertices, indices, shapeTransform * Transform2D(Float2(0.0f, (size._y - radius) * 0.5f)));

				// Bottom
				GenerateRectangle(Float2(topBottomRectangleWidth, radius), color, vertices, indices, shapeTransform * Transform2D(Float2(0.0f, -(size._y - radius) * 0.5f)));
			}

			// Corners
			GenerateQuarterCircle(radius, roundSideCount, color, vertices, indices, shapeTransform * Transform2D(Math::kPiOverTwo, Float2(-(halfSize._x - radius), (halfSize._y - radius))));
			GenerateQuarterCircle(radius, roundSideCount, color, vertices, indices, shapeTransform * Transform2D(Math::kPi, Float2(-(halfSize._x - radius), -(halfSize._y - radius))));
			GenerateQuarterCircle(radius, roundSideCount, color, vertices, indices, shapeTransform * Transform2D(-Math::kPiOverTwo, Float2((halfSize._x - radius), -(halfSize._y - radius))));
			GenerateQuarterCircle(radius, roundSideCount, color, vertices, indices, shapeTransform * Transform2D(0.0f, Float2((halfSize._x - radius), (halfSize._y - radius))));
		}

		void ShapeGenerator::GenerateConvexShape(const Vector<Float2>& points, const Color& color, Vector<VS_INPUT_SHAPE>& vertices, Vector<IndexElementType>& indices, const Transform2D& shapeTransform)
		{
			Vector<Float2> rawVertices = points;
			GrahamScan_Convexify(rawVertices);

			const ScopedShapeTransformer scopedShapeTransformer{ vertices, shapeTransform };
			const uint32 vertexBase = vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = color;

			const uint32 rawVertexCount = rawVertices.Size();
			vertices.Reserve(vertices.Size() + rawVertexCount);
			for (uint32 i = 0; i < rawVertexCount; ++i)
			{
				v._position = Float4(rawVertices[i]);
				vertices.PushBack(v);
			}
			const uint32 triangleCount = rawVertexCount - 2;
			indices.Reserve(indices.Size() + triangleCount * 3);
			for (uint32 i = 0; i < triangleCount; i++)
			{
				indices.PushBack(vertexBase + 0);
				indices.PushBack(vertexBase + rawVertexCount - i - 2);
				indices.PushBack(vertexBase + rawVertexCount - i - 1);
			}
		}

		void ShapeGenerator::GenerateLine(const Float2& positionA, const Float2& positionB, float thickness, uint8 roundSideCount, const Color& color, Vector<VS_INPUT_SHAPE>& vertices, Vector<IndexElementType>& indices, const Transform2D& shapeTransform)
		{
			MINT_ASSERT(thickness >= 0.0f, "thickness must be equal to or greater than 0");
			MINT_ASSERT(roundSideCount > 1, "roundSideCount must be greater than 1");

			if (thickness == 0.0f)
			{
				return;
			}

			roundSideCount = Max(roundSideCount, static_cast<uint8>(2));

			const Float2 center = (positionA + positionB) * 0.5f;
			const Float2 aToB = positionB - positionA;
			const float length = aToB.Length();
			if (length <= 1.0f)
			{
				GenerateCircle(thickness, roundSideCount * 2, color, vertices, indices, center);
				return;
			}

			const float theta = ::atan2f(aToB._y, aToB._x);
			GenerateRoundRectangle(Float2(length, thickness), 1.0f, roundSideCount, color, vertices, indices, shapeTransform * Transform2D(theta, center));
		}

		void ShapeGenerator::GenerateCircle(float radius, uint8 sideCount, const Color& color, Shape& outShape, const Transform2D& shapeTransform)
		{
			GenerateCircle(radius, sideCount, color, outShape._vertices, outShape._indices, shapeTransform);
		}

		void ShapeGenerator::GenerateHalfCircle(float radius, uint8 sideCount, const Color& color, Shape& outShape, const Transform2D& shapeTransform)
		{
			GenerateHalfCircle(radius, sideCount, color, outShape._vertices, outShape._indices, shapeTransform);
		}

		void ShapeGenerator::GenerateQuarterCircle(float radius, uint8 sideCount, const Color& color, Shape& outShape, const Transform2D& shapeTransform)
		{
			GenerateQuarterCircle(radius, sideCount, color, outShape._vertices, outShape._indices, shapeTransform);
		}

		void ShapeGenerator::GenerateRectangle(const Float2& size, const Color& color, Shape& outShape, const Transform2D& shapeTransform)
		{
			GenerateRectangle(size, color, outShape._vertices, outShape._indices, shapeTransform);
		}

		void ShapeGenerator::GenerateRoundRectangle(const Float2& size, float roundness, uint8 roundSideCount, const Color& color, Shape& outShape, const Transform2D& shapeTransform)
		{
			GenerateRoundRectangle(size, roundness, roundSideCount, color, outShape._vertices, outShape._indices, shapeTransform);
		}

		void ShapeGenerator::GenerateConvexShape(const Vector<Float2>& points, const Color& color, Shape& outShape, const Transform2D& shapeTransform)
		{
			GenerateConvexShape(points, color, outShape._vertices, outShape._indices, shapeTransform);
		}

		void ShapeGenerator::GenerateLine(const Float2& positionA, const Float2& positionB, float thickness, uint8 roundSideCount, const Color& color, Shape& outShape, const Transform2D& shapeTransform)
		{
			GenerateLine(positionA, positionB, thickness, roundSideCount, color, outShape._vertices, outShape._indices, shapeTransform);
		}

		void ShapeGenerator::FillColor(Shape& inoutShape, const Color& color)
		{
			for (auto& vertex : inoutShape._vertices)
			{
				vertex._color = color;
			}
		}

		void ShapeGenerator::GenerateTestShapeSet(Shape& outShape, const Transform2D& shapeTransform)
		{
			ShapeGenerator::GenerateCircle(0.25f, 16, ByteColor(255, 0, 0, 127), outShape, shapeTransform * Transform2D(Float2(-2, 1)));

			ShapeGenerator::GenerateHalfCircle(0.25f, 16, ByteColor(255, 127, 0, 63), outShape, shapeTransform * Transform2D(Float2(-1, 1)));

			ShapeGenerator::GenerateQuarterCircle(0.25f, 16, ByteColor(255, 196, 0, 63), outShape, shapeTransform * Transform2D(Float2(0, 1)));

			ShapeGenerator::GenerateRectangle(Float2(1.0f, 0.5f), ByteColor(255, 255, 0), outShape, shapeTransform * Transform2D(Float2(1, 1)));

			ShapeGenerator::GenerateRoundRectangle(Float2(0.75f, 0.5f), 0.5f, 2, ByteColor(0, 255, 0), outShape, shapeTransform * Transform2D(Float2(-2, 0)));

			ShapeGenerator::GenerateRoundRectangle(Float2(0.75f, 0.5f), 1.0f, 8, ByteColor(0, 255, 0), outShape, shapeTransform * Transform2D(Float2(-1, 0)));

			Transform2D shapeTransformCache = shapeTransform * Transform2D(0.25f, Float2(0, 0));
			ShapeGenerator::GenerateRoundRectangle(Float2(1.0f, 0.5f), 0.5f, 2, ByteColor(0, 255, 0), outShape, shapeTransformCache);
			shapeTransformCache *= Transform2D(0.0f, Float2(1.0f, 0.0f));
			ShapeGenerator::GenerateRoundRectangle(Float2(1.0f, 0.5f), 0.5f, 2, ByteColor(0, 255, 0), outShape, shapeTransformCache);

			Vector<Float2> convexPoints;
			convexPoints.PushBack(Float2(0.0f, -0.25f));
			convexPoints.PushBack(Float2(0.4f, 0.0f));
			convexPoints.PushBack(Float2(0.25f, 0.4f));
			convexPoints.PushBack(Float2(-0.25f, 0.4f));
			convexPoints.PushBack(Float2(-0.4f, 0.0f));
			ShapeGenerator::GenerateConvexShape(convexPoints, ByteColor(0, 196, 255), outShape, shapeTransform * Transform2D(Float2(2, 0)));

			ShapeGenerator::GenerateLine(Float2(-2, -1), Float2(1.0f, -1.25f), 0.25f, 4, ByteColor(0, 127, 255), outShape, shapeTransform);
		}
	}
}
