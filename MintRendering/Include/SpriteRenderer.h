#pragma once


#ifndef _MINT_RENDERING_SPRITE_RENDERER_H_
#define _MINT_RENDERING_SPRITE_RENDERER_H_


#include <MintRenderingBase/Include/ShapeRendererContext.h>


namespace mint
{
	namespace Rendering
	{
		class SpriteRenderer : protected ShapeRendererContext
		{
		public:
			SpriteRenderer(GraphicDevice& graphicDevice);
			virtual ~SpriteRenderer() = default;

		public:
			virtual void flush() noexcept override;
			virtual void render() noexcept override;

		public:
			void drawRectangle(const Float2& position, const Float2& size, const Float2& uv0, const Float2& uv1);
		};
	}
}


#endif // !_MINT_RENDERING_SPRITE_RENDERER_H_
