#include <MintRenderingBase/Include/ShapeGenerator.h>
#include <Assets/Include/CppHlsl/CppHlslStreamData.h>


namespace mint
{
	namespace Rendering
	{
		MINT_INLINE float4 convertByteColorToFloat4(const ByteColor& byteColor)
		{
			return float4(byteColor.RAsFloat(), byteColor.GAsFloat(), byteColor.BAsFloat(), byteColor.AAsFloat());
		}

		void ShapeGenerator::GenerateCircle(float radius, uint8 sideCount, const ByteColor& byteColor, Shape& outShape, const Float2& offset)
		{
			MINT_ASSERT(radius > 0.0f, "radius must be greater than 0");
			MINT_ASSERT(sideCount > 2, "sideCount must be greater than 2");

			radius = Max(radius, 1.0f);
			sideCount = Max(sideCount, static_cast<uint8>(3));

			const uint32 vertexBase = outShape._vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = convertByteColorToFloat4(byteColor);
			v._position._w = 1.0f;

			v._position._x = offset._x;
			v._position._y = offset._y;
			outShape._vertices.PushBack(v);

			for (uint8 i = 0; i < sideCount; ++i)
			{
				const float theta = (static_cast<float>(i) / sideCount) * Math::kTwoPi;
				v._position._x = offset._x + ::cos(theta) * radius;
				v._position._y = offset._y + -::sin(theta) * radius;
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

		void ShapeGenerator::GenerateHalfCircle(float radius, uint8 sideCount, float angle, const ByteColor& byteColor, Shape& outShape, const Float2& offset)
		{
			MINT_ASSERT(radius > 0.0f, "radius must be greater than 0");
			MINT_ASSERT(sideCount > 2, "sideCount must be greater than 2");

			radius = Max(radius, 1.0f);
			sideCount = Max(sideCount, static_cast<uint8>(3));

			const uint32 vertexBase = outShape._vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = convertByteColorToFloat4(byteColor);
			v._position._w = 1.0f;

			v._position._x = offset._x;
			v._position._y = offset._y;
			outShape._vertices.PushBack(v);

			for (uint8 i = 0; i <= sideCount; ++i)
			{
				const float theta = (static_cast<float>(i) / sideCount) * Math::kPi;
				v._position._x = offset._x + ::cos(angle + theta) * radius;
				v._position._y = offset._y + -::sin(angle + theta) * radius;
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
		
		void ShapeGenerator::GenerateQuarterCircle(float radius, uint8 sideCount, float angle, const ByteColor& byteColor, Shape& outShape, const Float2& offset)
		{
			MINT_ASSERT(radius > 0.0f, "radius must be greater than 0");
			MINT_ASSERT(sideCount > 0, "sideCount must be greater than 0");

			radius = Max(radius, 1.0f);
			sideCount = Max(sideCount, static_cast<uint8>(1));

			const uint32 vertexBase = outShape._vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = convertByteColorToFloat4(byteColor);
			v._position._w = 1.0f;

			v._position._x = offset._x;
			v._position._y = offset._y;
			outShape._vertices.PushBack(v);

			for (uint8 i = 0; i <= sideCount; ++i)
			{
				const float theta = (static_cast<float>(i) / sideCount) * Math::kPiOverTwo;
				v._position._x = offset._x + ::cos(angle + theta) * radius;
				v._position._y = offset._y + -::sin(angle + theta) * radius;
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

		void ShapeGenerator::GenerateRectangle(const Float2& size, const ByteColor& byteColor, Shape& outShape, const Float2& offset)
		{
			MINT_ASSERT(size._x > 0.0f, "size._x must be greater than 0");
			MINT_ASSERT(size._y > 0.0f, "size._y must be greater than 0");

			const uint32 vertexBase = outShape._vertices.Size();
			VS_INPUT_SHAPE v;
			v._color = convertByteColorToFloat4(byteColor);
			v._position._w = 1.0f;

			const Float2 halfSize = size * 0.5f;
			v._position._x = offset._x + halfSize._x;
			v._position._y = offset._y - halfSize._y;
			outShape._vertices.PushBack(v);

			v._position._x = offset._x - halfSize._x;
			v._position._y = offset._y - halfSize._y;
			outShape._vertices.PushBack(v);

			v._position._x = offset._x - halfSize._x;
			v._position._y = offset._y + halfSize._y;
			outShape._vertices.PushBack(v);

			v._position._x = offset._x + halfSize._x;
			v._position._y = offset._y + halfSize._y;
			outShape._vertices.PushBack(v);

			outShape._indices.PushBack(vertexBase + 0);
			outShape._indices.PushBack(vertexBase + 1);
			outShape._indices.PushBack(vertexBase + 2);

			outShape._indices.PushBack(vertexBase + 2);
			outShape._indices.PushBack(vertexBase + 3);
			outShape._indices.PushBack(vertexBase + 0);
		}

		void ShapeGenerator::GenerateRoundRectangle(const Float2& size, float roundness, uint8 roundSideCount, const ByteColor& byteColor, Shape& outShape, const Float2& offset)
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
				GenerateCircle(radius, roundSideCount * 4, byteColor, outShape, offset);
				return;
			}

			const Float2 halfSize = size * 0.5f;
			if (middleRectangleHeight == 0.0f)
			{
				GenerateRectangle(Float2(size._x - radius * 2.0f, size._y), byteColor, outShape, offset);
				GenerateHalfCircle(radius, roundSideCount * 2, Math::kPiOverTwo, byteColor, outShape, offset + Float2(-halfSize._x + radius, 0.0f));
				GenerateHalfCircle(radius, roundSideCount * 2, -Math::kPiOverTwo, byteColor, outShape, offset + Float2(halfSize._x - radius, 0.0f));
				return;
			}
			
			// Middle
			GenerateRectangle(Float2(size._x, middleRectangleHeight), byteColor, outShape, offset);

			// Top
			GenerateRectangle(Float2(topBottomRectangleWidth, radius), byteColor, outShape, offset + Float2(0.0f, -(size._y - radius) * 0.5f));
			
			// Bottom
			GenerateRectangle(Float2(topBottomRectangleWidth, radius), byteColor, outShape, offset + Float2(0.0f, (size._y - radius) * 0.5f));

			// Corners
			GenerateQuarterCircle(radius, roundSideCount, 0.0f, byteColor, outShape, offset + Float2((halfSize._x - radius), -(halfSize._y - radius)));
			GenerateQuarterCircle(radius, roundSideCount, Math::kPiOverTwo, byteColor, outShape, offset + Float2(-(halfSize._x - radius), -(halfSize._y - radius)));
			GenerateQuarterCircle(radius, roundSideCount, Math::kPi, byteColor, outShape, offset + Float2(-(halfSize._x - radius), (halfSize._y - radius)));
			GenerateQuarterCircle(radius, roundSideCount, -Math::kPiOverTwo, byteColor, outShape, offset + Float2((halfSize._x - radius), (halfSize._y - radius)));
		}
	}
}
