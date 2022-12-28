#include <MintRendering/Include/SpriteRenderer.h>


namespace mint
{
	namespace Rendering
	{
		SpriteRenderer::SpriteRenderer(GraphicDevice& graphicDevice)
			: ShapeRendererContext(graphicDevice)
		{
			initializeShaders();
		}

		void SpriteRenderer::flush() noexcept
		{
			__super::flush();
		}

		void SpriteRenderer::render() noexcept
		{
			__super::render();
		}

		void SpriteRenderer::drawRectangle(const Float2& position, const Float2& size, const Float2& uv0, const Float2& uv1)
		{
			const Float2 halfSize = size * 0.5f;
			setPosition(Float4(position + halfSize));
			_uv0 = uv0;
			_uv1 = uv1;
			drawRectangleInternal(Float2::kZero, halfSize, Color::kWhite, ShapeType::TexturedTriangle);
			pushShapeTransformToBuffer(0.0f);
		}
	}
}
