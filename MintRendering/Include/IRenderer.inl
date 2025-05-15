#pragma once


namespace mint
{
	namespace Rendering
	{
		MINT_INLINE void IRenderer::SetUseMultipleViewports() noexcept
		{
			_useMultipleViewports = true;
		}

		MINT_INLINE bool IRenderer::IsUsingMultipleViewports() const noexcept
		{
			return _useMultipleViewports;
		}

		MINT_INLINE void IRenderer::SetCoordinateSpace(const CoordinateSpace& coordinateSpace) noexcept
		{
			_coordinateSpace = coordinateSpace;
		}

		MINT_INLINE void IRenderer::SetColor(const ByteColor& color) noexcept
		{
			SetColor(Color(color.R(), color.G(), color.B(), color.A()));
		}

		MINT_INLINE void IRenderer::SetColor(const Color& color) noexcept
		{
			_color = color;
		}

		MINT_INLINE void IRenderer::SetClipRect(const Rect& clipRect) noexcept
		{
			_clipRect = clipRect;
		}

		MINT_INLINE const Rect& IRenderer::GetClipRect() const noexcept
		{
			return _clipRect;
		}
	}
}
