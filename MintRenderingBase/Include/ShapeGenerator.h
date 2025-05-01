#pragma once


#ifndef _MINT_RENDERING_BASE_SHAPE_GENERATOR_H_
#define _MINT_RENDERING_BASE_SHAPE_GENERATOR_H_


#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintMath/Include/Transform.h>


namespace mint
{
	namespace Rendering
	{
		class ScopedShapeTransformer
		{
		public:
			ScopedShapeTransformer(Vector<VS_INPUT_SHAPE>& vertices, const Transform2D& shapeTransform);
			~ScopedShapeTransformer();
		private:
			Vector<VS_INPUT_SHAPE>& _vertices;
			const Transform2D& _shapeTransform;
			uint32 _shapeVertexOffset;
		};

		class ShapeGenerator abstract final
		{
		public:
			static void GenerateCircle(float radius, uint8 sideCount, const Color& color, Vector<VS_INPUT_SHAPE>& vertices, Vector<IndexElementType>& indices, const Transform2D& shapeTransform);
			static void GenerateHalfCircle(float radius, uint8 sideCount, const Color& color, Vector<VS_INPUT_SHAPE>& vertices, Vector<IndexElementType>& indices, const Transform2D& shapeTransform);
			static void GenerateQuarterCircle(float radius, uint8 sideCount, const Color& color, Vector<VS_INPUT_SHAPE>& vertices, Vector<IndexElementType>& indices, const Transform2D& shapeTransform);
			static void GenerateRectangle(const Float2& size, const Color& color, Vector<VS_INPUT_SHAPE>& vertices, Vector<IndexElementType>& indices, const Transform2D& shapeTransform);
			static void GenerateRoundRectangle(const Float2& size, float roundness, uint8 roundSideCount, const Color& color, Vector<VS_INPUT_SHAPE>& vertices, Vector<IndexElementType>& indices, const Transform2D& shapeTransform);
			static void GenerateConvexShape(const Vector<Float2>& points, const Color& color, Vector<VS_INPUT_SHAPE>& vertices, Vector<IndexElementType>& indices, const Transform2D& shapeTransform);
			static void GenerateLine(const Float2& positionA, const Float2& positionB, float thickness, uint8 roundSideCount, const Color& color, Vector<VS_INPUT_SHAPE>& vertices, Vector<IndexElementType>& indices, const Transform2D& shapeTransform);

		public:
			static void GenerateCircle(float radius, uint8 sideCount, const Color& color, Shape& outShape, const Transform2D& shapeTransform = Transform2D::GetIdentity());
			static void GenerateHalfCircle(float radius, uint8 sideCount, const Color& color, Shape& outShape, const Transform2D& shapeTransform = Transform2D::GetIdentity());
			static void GenerateQuarterCircle(float radius, uint8 sideCount, const Color& color, Shape& outShape, const Transform2D& shapeTransform = Transform2D::GetIdentity());
			static void GenerateRectangle(const Float2& size, const Color& color, Shape& outShape, const Transform2D& shapeTransform = Transform2D::GetIdentity());
			static void GenerateRoundRectangle(const Float2& size, float roundness, uint8 roundSideCount, const Color& color, Shape& outShape, const Transform2D& shapeTransform = Transform2D::GetIdentity());
			static void GenerateConvexShape(const Vector<Float2>& points, const Color& color, Shape& outShape, const Transform2D& shapeTransform = Transform2D::GetIdentity());
			static void GenerateLine(const Float2& positionA, const Float2& positionB, float thickness, uint8 roundSideCount, const Color& color, Shape& outShape, const Transform2D& shapeTransform = Transform2D::GetIdentity());
			static void FillColor(Shape& inoutShape, const Color& color);

		public:
			static void GenerateTestShapeSet(Shape& outShape, const Transform2D& shapeTransform = Transform2D::GetIdentity());
		};
	}
}


#endif // !_MINT_RENDERING_BASE_SHAPE_GENERATOR_H_
