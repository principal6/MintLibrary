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
			static void MakeCircle(float radius, uint8 sideCount, const ByteColor& byteColor, Shape& outShape);
		};
	}
}


#endif // !_MINT_RENDERING_BASE_SHAPE_GENERATOR_H_
