#pragma once


#ifndef _MINT_RENDERING_SPRITE_RENDERER_H_
#define _MINT_RENDERING_SPRITE_RENDERER_H_


#include <MintRendering/Include/ShapeRenderer.h>


namespace mint
{
	namespace Rendering
	{
		class SpriteRenderer final : protected ShapeRenderer
		{
			friend GraphicsDevice;

		public:
			virtual ~SpriteRenderer() = default;

		public:
			virtual void Initialize() noexcept override final;

		public:
			MINT_INLINE GraphicsObjectID GetDefaultShaderPipelineID() const noexcept { return __super::GetDefaultShaderPipelineID(); }

		public:
			void SetMaterial(const GraphicsObjectID& materialID) noexcept;
			// position 은 내부 2D 좌표(화면 중앙이 원점, +Y 가 위)다.
			void DrawImage(const Float2& position, const Float2& size, const Float2& uv0, const Float2& uv1);
			// 화면 좌상단이 원점인 픽셀 좌표로 그린다.
			void DrawImage(const ScreenSpaceCoordinates& position, const Float2& size, const Float2& uv0, const Float2& uv1);

		private:
			SpriteRenderer(GraphicsDevice& graphicsDevice, LowLevelRenderer<VS_INPUT_SHAPE>& lowLevelRenderer, Vector<SB_Transform>& sbTransformData, const uint32 psTextureSlot, const ByteColor& transparentColor);

		private:
			virtual const char* GetPixelShaderName() const noexcept override final;
			virtual const char* GetPixelShaderString() const noexcept override final;
			virtual const char* GetPixelShaderEntryPoint() const noexcept override final;

		private:
			uint32 _psTextureSlot;
			ByteColor _transparentColor;
		};
	}
}


#endif // !_MINT_RENDERING_SPRITE_RENDERER_H_
