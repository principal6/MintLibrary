#pragma once


#ifndef _MINT_RENDERING_BASE_SHAPE_GENERATOR_H_
#define _MINT_RENDERING_BASE_SHAPE_GENERATOR_H_


#include <MintRenderingBase/Include/RenderingBaseCommon.h>


namespace mint
{
	namespace Rendering
	{
		struct VS_INPUT_SHAPE;


		struct Shape
		{
			Vector<VS_INPUT_SHAPE> _vertices;
			Vector<IndexElementType> _indices;
		};

		class ShapeGenerator abstract final
		{
		public:
			static void GenerateCircle(float radius, uint8 sideCount, const ByteColor& byteColor, Shape& outShape, const Float2& offset = Float2::kZero);
			static void GenerateHalfCircle(float radius, uint8 sideCount, float angle, const ByteColor& byteColor, Shape& outShape, const Float2& offset = Float2::kZero);
			static void GenerateQuarterCircle(float radius, uint8 sideCount, float angle, const ByteColor& byteColor, Shape& outShape, const Float2& offset = Float2::kZero);
			static void GenerateRectangle(const Float2& size, const ByteColor& byteColor, Shape& outShape, const Float2& offset = Float2::kZero);
			static void GenerateRoundRectangle(const Float2& size, float roundness, uint8 roundSideCount, const ByteColor& byteColor, Shape& outShape, const Float2& offset = Float2::kZero);
		};
	}
}


#endif // !_MINT_RENDERING_BASE_SHAPE_GENERATOR_H_
