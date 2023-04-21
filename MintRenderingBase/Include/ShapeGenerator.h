#pragma once


#ifndef _MINT_RENDERING_BASE_SHAPE_GENERATOR_H_
#define _MINT_RENDERING_BASE_SHAPE_GENERATOR_H_


#include <MintRenderingBase/Include/RenderingBaseCommon.h>


namespace mint
{
	namespace Rendering
	{
		struct VS_INPUT_SHAPE;


		struct ShapeTransform
		{
			ShapeTransform() : ShapeTransform(0.0f, Float2::kZero) { __noop; }
			ShapeTransform(float rotation, const Float2& translation) : _rotation{ rotation }, _translation{ translation } { __noop; }

			float _rotation;
			Float2 _translation;
		};

		struct Shape
		{
			Vector<VS_INPUT_SHAPE> _vertices;
			Vector<IndexElementType> _indices;
		};

		class ScopedShapeTransformer
		{
		public:
			ScopedShapeTransformer(Shape& shape, const ShapeTransform& shapeTransform);
			~ScopedShapeTransformer();
		private:
			Shape& _shape;
			const ShapeTransform& _shapeTransform;
			uint32 _shapeVertexOffset;
		};

		class ShapeGenerator abstract final
		{
		public:
			static void GenerateCircle(float radius, uint8 sideCount, const ByteColor& byteColor, Shape& outShape, const Float2& offset = Float2::kZero);
			static void GenerateHalfCircle(float radius, uint8 sideCount, float angle, const ByteColor& byteColor, Shape& outShape, const Float2& offset = Float2::kZero);
			static void GenerateQuarterCircle(float radius, uint8 sideCount, float angle, const ByteColor& byteColor, Shape& outShape, const Float2& offset = Float2::kZero);
			static void GenerateRectangle(const Float2& size, const ByteColor& byteColor, Shape& outShape, const Float2& offset = Float2::kZero);
			static void GenerateRoundRectangle(const Float2& size, float roundness, uint8 roundSideCount, const ByteColor& byteColor, Shape& outShape, const Float2& offset = Float2::kZero);
			static void GenerateLine(const Float2& positionA, const Float2& positionB, float thickness, uint8 roundSideCount, const ByteColor& byteColor, Shape& outShape);
		};
	}
}


#endif // !_MINT_RENDERING_BASE_SHAPE_GENERATOR_H_
