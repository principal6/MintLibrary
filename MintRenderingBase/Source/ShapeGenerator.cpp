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

		void ShapeGenerator::MakeCircle(float radius, uint8 sideCount, const ByteColor& byteColor, Shape& outShape)
		{
			MINT_ASSERT(radius > 0.0f, "radius must be greater than 0");
			MINT_ASSERT(sideCount > 2, "radius must be greater than 2");

			radius = Max(radius, 1.0f);
			sideCount = Max(sideCount, static_cast<uint8>(3));

			const float4 color{ convertByteColorToFloat4(byteColor) };
			VS_INPUT_SHAPE v;
			v._color = color;
			v._position._w = 1.0f;

			v._position._x = 0.0f;
			v._position._y = 0.0f;
			outShape._vertices.PushBack(v);

			for (uint8 i = 0; i < sideCount; ++i)
			{
				const float theta = (static_cast<float>(i) / sideCount) * Math::kTwoPi;
				v._position._x = ::cos(theta) * radius;
				v._position._y = -::sin(theta) * radius;
				outShape._vertices.PushBack(v);
			}

			//     3
			//  4     2
			// 5   0   1
			//  6     8
			//     7
			for (uint8 i = 0; i < sideCount - 1; ++i)
			{
				outShape._indices.PushBack(0);
				outShape._indices.PushBack(i + 1);
				outShape._indices.PushBack(i + 2);
			}
			outShape._indices.PushBack(0);
			outShape._indices.PushBack(sideCount);
			outShape._indices.PushBack(1);
		}
	}
}
