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
			ShapeTransform(const Float2& translation) : ShapeTransform(0.0f, translation) { __noop; }
			ShapeTransform(float rotation, const Float2& translation) : _rotation{ rotation }, _translation{ translation } { __noop; }

			ShapeTransform operator*(const ShapeTransform& rhs) const;
			ShapeTransform& operator*=(const ShapeTransform& rhs);

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
			static void GenerateCircle(float radius, uint8 sideCount, const ByteColor& byteColor, Shape& outShape, const ShapeTransform& shapeTransform = ShapeTransform());
			static void GenerateHalfCircle(float radius, uint8 sideCount, const ByteColor& byteColor, Shape& outShape, const ShapeTransform& shapeTransform = ShapeTransform());
			static void GenerateQuarterCircle(float radius, uint8 sideCount, const ByteColor& byteColor, Shape& outShape, const ShapeTransform& shapeTransform = ShapeTransform());
			static void GenerateRectangle(const Float2& size, const ByteColor& byteColor, Shape& outShape, const ShapeTransform& shapeTransform = ShapeTransform());
			static void GenerateRoundRectangle(const Float2& size, float roundness, uint8 roundSideCount, const ByteColor& byteColor, Shape& outShape, const ShapeTransform& shapeTransform = ShapeTransform());
			static void GenerateLine(const Float2& positionA, const Float2& positionB, float thickness, uint8 roundSideCount, const ByteColor& byteColor, Shape& outShape, const ShapeTransform& shapeTransform = ShapeTransform());
			static void GenerateTestShapeSet(Shape& outShape, const ShapeTransform& shapeTransform = ShapeTransform());
			static void FillColor(Shape& inoutShape, const ByteColor& byteColor);
		};
	}
}


#endif // !_MINT_RENDERING_BASE_SHAPE_GENERATOR_H_
