#pragma once


#ifndef _MINT_RENDERING_SPRITE_RENDERER_H_
#define _MINT_RENDERING_SPRITE_RENDERER_H_


#include <MintRenderingBase/Include/ShapeRendererContext.h>


namespace mint
{
	namespace Rendering
	{
		class ImageRenderer : protected ShapeRendererContext
		{
		public:
			ImageRenderer(GraphicDevice& graphicDevice);
			virtual ~ImageRenderer() = default;

		public:
			virtual void initializeShaders() noexcept override;
			virtual void flush() noexcept override;
			virtual void render() noexcept override;

		public:
			void drawImage(const Float2& position, const Float2& size, const Float2& uv0, const Float2& uv1);
		};
	}
}


#endif // !_MINT_RENDERING_SPRITE_RENDERER_H_
