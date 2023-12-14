#include <MintRenderingBase/Include/ShapeGenerator.h>
#include <MintMath/Include/Float2x2.h>
#include <MintMath/Include/Geometry.h>
#include <Assets/Include/CppHlsl/CppHlslStreamData.h>


namespace mint
{
	namespace Rendering
	{
#pragma region ShapeTransform
		ShapeTransform ShapeTransform::operator*(const ShapeTransform& rhs) const
		{
			// *this == parent, rhs == child
			ShapeTransform result;
			result._translation = _translation;
			result._translation += Float2x2::RotationMatrix(-_rotation).Mul(rhs._translation);
			result._rotation = _rotation + rhs._rotation;
			return result;
		}

		ShapeTransform& ShapeTransform::operator*=(const ShapeTransform& rhs)
		{
			// *this == parent, rhs == child
			_translation += Float2x2::RotationMatrix(-_rotation).Mul(rhs._translation);
			_rotation = _rotation + rhs._rotation;
			return *this;
		}
#pragma endregion


#pragma region ScopedShapeTransformer
		ScopedShapeTransformer::ScopedShapeTransformer(Shape& shape, const ShapeTransform& shapeTransform)
			: _shape{ shape }
			, _shapeTransform{ shapeTransform }
		{
			_shapeVertexOffset = _shape._vertices.Size();
		}

		ScopedShapeTransformer::~ScopedShapeTransformer()
		{
			const uint32 shapeVertexCount = _shape._vertices.Size();
			if (_shapeVertexOffset == shapeVertexCount)
			{
				return;
			}

			const Float2x2 rotationMatrix = Float2x2::RotationMatrix(-_shapeTransform._rotation);
			Float2 rotatedXY;
			for (uint32 i = _shapeVertexOffset; i < shapeVertexCount; ++i)
			{
				VS_INPUT_SHAPE& vertex = _shape._vertices[i];
				rotatedXY = rotationMatrix.Mul(Float2(vertex._position._x, vertex._position._y));
				vertex._position._x = rotatedXY._x;
				vertex._position._y = rotatedXY._y;

				vertex._position._x += _shapeTransform._translation._x;
				vertex._position._y += _shapeTransform._translation._y;
			}
		}
#pragma endregion


		MINT_INLINE float4 convertByteColorToFloat4(const ByteColor& byteColor)
		{
			return float4(byteColor.RAsFloat(), byteColor.GAsFloat(), byteColor.BAsFloat(), byteColor.AAsFloat());
		}

		void ShapeGenerator::GenerateCircle(float radius, uint8 sideCount, const ByteColor& byteColor, Shape& outShape, const ShapeTransform& shapeTransform)
		{
			MINT_ASSERT(radius > 0.0f, "radius must be greater than 0");
			MINT_ASSERT(sideCount > 2, "sideCount must be greater than 2");

			radius = Max(radius, 1.0f);
			sideCount = Max(sideCount, static_cast<uint8>(3));

			const uint32 vertexBase = outShape._vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = convertByteColorToFloat4(byteColor);
			v._position._w = 1.0f;

			v._position._x = shapeTransform._translation._x;
			v._position._y = shapeTransform._translation._y;
			outShape._vertices.PushBack(v);

			for (uint8 i = 0; i < sideCount; ++i)
			{
				const float theta = (static_cast<float>(i) / sideCount) * Math::kTwoPi;
				v._position._x = shapeTransform._translation._x + ::cos(theta) * radius;
				v._position._y = shapeTransform._translation._y + -::sin(theta) * radius;
				outShape._vertices.PushBack(v);
			}

			//     3
			//  4     2
			// 5   0   1
			//  6     8
			//     7
			for (uint8 i = 0; i < sideCount - 1; ++i)
			{
				outShape._indices.PushBack(vertexBase + 0);
				outShape._indices.PushBack(vertexBase + i + 1);
				outShape._indices.PushBack(vertexBase + i + 2);
			}
			outShape._indices.PushBack(vertexBase + 0);
			outShape._indices.PushBack(vertexBase + sideCount);
			outShape._indices.PushBack(vertexBase + 1);
		}

		void ShapeGenerator::GenerateHalfCircle(float radius, uint8 sideCount, const ByteColor& byteColor, Shape& outShape, const ShapeTransform& shapeTransform)
		{
			MINT_ASSERT(radius > 0.0f, "radius must be greater than 0");
			MINT_ASSERT(sideCount > 2, "sideCount must be greater than 2");

			radius = Max(radius, 1.0f);
			sideCount = Max(sideCount, static_cast<uint8>(3));

			const uint32 vertexBase = outShape._vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = convertByteColorToFloat4(byteColor);
			v._position._w = 1.0f;

			v._position._x = shapeTransform._translation._x;
			v._position._y = shapeTransform._translation._y;
			outShape._vertices.PushBack(v);

			for (uint8 i = 0; i <= sideCount; ++i)
			{
				const float theta = (static_cast<float>(i) / sideCount) * Math::kPi;
				v._position._x = shapeTransform._translation._x + ::cos(shapeTransform._rotation + theta) * radius;
				v._position._y = shapeTransform._translation._y + -::sin(shapeTransform._rotation + theta) * radius;
				outShape._vertices.PushBack(v);
			}

			//     3
			//  4     2
			// 5   0   1
			for (uint8 i = 0; i < sideCount; ++i)
			{
				outShape._indices.PushBack(vertexBase + 0);
				outShape._indices.PushBack(vertexBase + i + 1);
				outShape._indices.PushBack(vertexBase + i + 2);
			}
		}

		void ShapeGenerator::GenerateQuarterCircle(float radius, uint8 sideCount, const ByteColor& byteColor, Shape& outShape, const ShapeTransform& shapeTransform)
		{
			MINT_ASSERT(radius > 0.0f, "radius must be greater than 0");
			MINT_ASSERT(sideCount > 0, "sideCount must be greater than 0");

			radius = Max(radius, 1.0f);
			sideCount = Max(sideCount, static_cast<uint8>(1));

			const uint32 vertexBase = outShape._vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = convertByteColorToFloat4(byteColor);
			v._position._w = 1.0f;

			v._position._x = shapeTransform._translation._x;
			v._position._y = shapeTransform._translation._y;
			outShape._vertices.PushBack(v);

			for (uint8 i = 0; i <= sideCount; ++i)
			{
				const float theta = (static_cast<float>(i) / sideCount) * Math::kPiOverTwo;
				v._position._x = shapeTransform._translation._x + ::cos(shapeTransform._rotation + theta) * radius;
				v._position._y = shapeTransform._translation._y + -::sin(shapeTransform._rotation + theta) * radius;
				outShape._vertices.PushBack(v);
			}

			// 3
			//    2
			// 0   1
			for (uint8 i = 0; i < sideCount; ++i)
			{
				outShape._indices.PushBack(vertexBase + 0);
				outShape._indices.PushBack(vertexBase + i + 1);
				outShape._indices.PushBack(vertexBase + i + 2);
			}
		}

		void ShapeGenerator::GenerateRectangle(const Float2& size, const ByteColor& byteColor, Shape& outShape, const ShapeTransform& shapeTransform)
		{
			MINT_ASSERT(size._x > 0.0f, "size._x must be greater than 0");
			MINT_ASSERT(size._y > 0.0f, "size._y must be greater than 0");

			const uint32 vertexBase = outShape._vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = convertByteColorToFloat4(byteColor);
			v._position._w = 1.0f;

			const ScopedShapeTransformer scopedShapeTransformer{ outShape, shapeTransform };
			const Float2 halfSize = size * 0.5f;
			v._position._x = +halfSize._x;
			v._position._y = -halfSize._y;
			outShape._vertices.PushBack(v);

			v._position._x = -halfSize._x;
			v._position._y = -halfSize._y;
			outShape._vertices.PushBack(v);

			v._position._x = -halfSize._x;
			v._position._y = +halfSize._y;
			outShape._vertices.PushBack(v);

			v._position._x = +halfSize._x;
			v._position._y = +halfSize._y;
			outShape._vertices.PushBack(v);

			outShape._indices.PushBack(vertexBase + 0);
			outShape._indices.PushBack(vertexBase + 1);
			outShape._indices.PushBack(vertexBase + 2);

			outShape._indices.PushBack(vertexBase + 2);
			outShape._indices.PushBack(vertexBase + 3);
			outShape._indices.PushBack(vertexBase + 0);
		}

		void ShapeGenerator::GenerateRoundRectangle(const Float2& size, float roundness, uint8 roundSideCount, const ByteColor& byteColor, Shape& outShape, const ShapeTransform& shapeTransform)
		{
			MINT_ASSERT(size._x > 0.0f, "size._x must be greater than 0");
			MINT_ASSERT(size._y > 0.0f, "size._y must be greater than 0");
			MINT_ASSERT(roundness >= 0.0f, "roundness must be equal or greater than 0");
			MINT_ASSERT(roundSideCount > 0, "roundSideCount must be greater than 0");

			roundness = Math::Saturate(roundness);
			const float radius = roundness * size.GetMinElement() * 0.5f;
			if (radius == 0.0f)
			{
				GenerateRectangle(size, byteColor, outShape);
				return;
			}
			roundSideCount = Max(roundSideCount, static_cast<uint8>(1));

			const uint32 vertexBase = outShape._vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = convertByteColorToFloat4(byteColor);
			v._position._w = 1.0f;

			const float middleRectangleHeight = size._y - radius * 2.0f;
			const float topBottomRectangleWidth = size._x - radius * 2.0f;
			if (topBottomRectangleWidth == 0.0f && middleRectangleHeight == 0.0f)
			{
				GenerateCircle(radius, roundSideCount * 4, byteColor, outShape, shapeTransform);
				return;
			}

			const Float2 halfSize = size * 0.5f;
			if (middleRectangleHeight == 0.0f)
			{
				GenerateRectangle(Float2(size._x - radius * 2.0f, size._y), byteColor, outShape, shapeTransform);
				GenerateHalfCircle(radius, roundSideCount * 2, byteColor, outShape, shapeTransform * ShapeTransform(Math::kPiOverTwo, Float2(-halfSize._x + radius, 0.0f)));
				GenerateHalfCircle(radius, roundSideCount * 2, byteColor, outShape, shapeTransform * ShapeTransform(-Math::kPiOverTwo, Float2(halfSize._x - radius, 0.0f)));
				return;
			}

			// Middle
			GenerateRectangle(Float2(size._x, middleRectangleHeight), byteColor, outShape, shapeTransform);

			// Top
			GenerateRectangle(Float2(topBottomRectangleWidth, radius), byteColor, outShape, shapeTransform * ShapeTransform(Float2(0.0f, -(size._y - radius) * 0.5f)));

			// Bottom
			GenerateRectangle(Float2(topBottomRectangleWidth, radius), byteColor, outShape, shapeTransform * ShapeTransform(Float2(0.0f, (size._y - radius) * 0.5f)));

			// Corners
			GenerateQuarterCircle(radius, roundSideCount, byteColor, outShape, shapeTransform * ShapeTransform(0.0f, Float2((halfSize._x - radius), -(halfSize._y - radius))));
			GenerateQuarterCircle(radius, roundSideCount, byteColor, outShape, shapeTransform * ShapeTransform(Math::kPiOverTwo, Float2(-(halfSize._x - radius), -(halfSize._y - radius))));
			GenerateQuarterCircle(radius, roundSideCount, byteColor, outShape, shapeTransform * ShapeTransform(Math::kPi, Float2(-(halfSize._x - radius), (halfSize._y - radius))));
			GenerateQuarterCircle(radius, roundSideCount, byteColor, outShape, shapeTransform * ShapeTransform(-Math::kPiOverTwo, Float2((halfSize._x - radius), (halfSize._y - radius))));
		}

		void ShapeGenerator::GenerateConvexShape(const Vector<Float2>& points, const ByteColor& byteColor, Shape& outShape, const ShapeTransform& shapeTransform)
		{
			Vector<Float2> rawVertices = points;
			GrahamScan_Convexify(rawVertices);

			const ScopedShapeTransformer scopedShapeTransformer{ outShape, shapeTransform };
			const uint32 vertexBase = outShape._vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = Color(byteColor);

			const uint32 rawVertexCount = rawVertices.Size();
			outShape._vertices.Reserve(outShape._vertices.Size() + rawVertexCount);
			for (uint32 i = 0; i < rawVertexCount; ++i)
			{
				v._position = Float4(rawVertices[i]);
				outShape._vertices.PushBack(v);
			}
			const uint32 triangleCount = rawVertexCount - 2;
			outShape._indices.Reserve(outShape._indices.Size() + triangleCount * 3);
			for (uint32 i = 0; i < triangleCount; i++)
			{
				outShape._indices.PushBack(vertexBase + 0);
				outShape._indices.PushBack(vertexBase + rawVertexCount - i - 2);
				outShape._indices.PushBack(vertexBase + rawVertexCount - i - 1);
			}
		}

		void ShapeGenerator::GenerateLine(const Float2& positionA, const Float2& positionB, float thickness, uint8 roundSideCount, const ByteColor& byteColor, Shape& outShape, const ShapeTransform& shapeTransform)
		{
			MINT_ASSERT(thickness > 0.0f, "thickness must be equal or greater than 0");
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
				GenerateCircle(thickness, roundSideCount * 2, byteColor, outShape, center);
				return;
			}

			const float theta = ::atan2f(-aToB._y, aToB._x);
			GenerateRoundRectangle(Float2(length, thickness), 1.0f, roundSideCount, byteColor, outShape, shapeTransform * ShapeTransform(theta, center));
		}

		void ShapeGenerator::FillColor(Shape& inoutShape, const ByteColor& byteColor)
		{
			const float4 color = convertByteColorToFloat4(byteColor);
			for (auto& vertex : inoutShape._vertices)
			{
				vertex._color = color;
			}
		}

		void ShapeGenerator::GenerateTestShapeSet(Shape& outShape, const ShapeTransform& shapeTransform)
		{
			ShapeGenerator::GenerateCircle(16.0f, 16, ByteColor(255, 0, 0, 127), outShape, shapeTransform * ShapeTransform(Float2(80, 80)));

			ShapeGenerator::GenerateHalfCircle(16.0f, 16, ByteColor(255, 127, 0, 63), outShape, shapeTransform * ShapeTransform(Float2(160, 80)));

			ShapeGenerator::GenerateQuarterCircle(16.0f, 16, ByteColor(255, 196, 0, 63), outShape, shapeTransform * ShapeTransform(Float2(240, 80)));

			ShapeGenerator::GenerateRectangle(Float2(64.0f, 32.0f), ByteColor(255, 255, 0), outShape, shapeTransform * ShapeTransform(Float2(320, 80)));

			ShapeGenerator::GenerateRoundRectangle(Float2(64.0f, 32.0f), 0.5f, 2, ByteColor(0, 255, 0), outShape, shapeTransform * ShapeTransform(Float2(80, 160)));

			ShapeTransform shapeTransformCache = shapeTransform * ShapeTransform(0.25f, Float2(160.0f, 160.0f));
			ShapeGenerator::GenerateRoundRectangle(Float2(64.0f, 32.0f), 0.5f, 2, ByteColor(0, 255, 0), outShape, shapeTransformCache);
			shapeTransformCache *= ShapeTransform(0.0f, Float2(80.0f, 0.0f));
			ShapeGenerator::GenerateRoundRectangle(Float2(64.0f, 32.0f), 0.5f, 2, ByteColor(0, 255, 0), outShape, shapeTransformCache);

			Vector<Float2> convexPoints;
			convexPoints.PushBack(Float2(0, -25));
			convexPoints.PushBack(Float2(40, 0));
			convexPoints.PushBack(Float2(25, 40));
			convexPoints.PushBack(Float2(-25, 40));
			convexPoints.PushBack(Float2(-40, 0));
			ShapeGenerator::GenerateConvexShape(convexPoints, ByteColor(0, 196, 255), outShape, shapeTransform * ShapeTransform(Float2(320, 160)));

			ShapeGenerator::GenerateLine(Float2(80.0f, 160.0f), Float2(320.0f, 180.0f), 8.0f, 4, ByteColor(0, 127, 255), outShape, shapeTransform);
		}
	}
}
