#pragma once


#ifndef _MINT_RENDERING_SPRITE_RENDERER_H_
#define _MINT_RENDERING_SPRITE_RENDERER_H_


#include <MintRenderingBase/Include/ShapeRenderer.h>


namespace mint
{
	namespace Rendering
	{
		class ImageRenderer : protected ShapeRenderer
		{
		public:
			ImageRenderer(GraphicsDevice& graphicsDevice, const uint32 psTextureSlot);
			ImageRenderer(GraphicsDevice& graphicsDevice, const uint32 psTextureSlot, const ByteColor& transparentColor);
			virtual ~ImageRenderer() = default;

		public:
			virtual void InitializeShaders() noexcept override;
			virtual void Render() noexcept override;
			virtual void Flush() noexcept override;

		public:
			void SetCoordinateSpace(const CoordinateSpace& coordinateSpace) noexcept;
			void DrawImage(const Float2& position, const Float2& size, const Float2& uv0, const Float2& uv1);

		protected:
			virtual const char* GetPixelShaderString() const noexcept override final;
			virtual const char* GetPixelShaderEntryPoint() const noexcept override final;

		private:
			uint32 _psTextureSlot;
			ByteColor _transparentColor;
		};
	}
}


#endif // !_MINT_RENDERING_SPRITE_RENDERER_H_
