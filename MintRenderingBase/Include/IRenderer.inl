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

		MINT_INLINE void IRenderer::SetPosition(const Float4& position) noexcept
		{
			_position = position;
		}

		MINT_INLINE void IRenderer::SetPositionZ(const float s) noexcept
		{
			_position._z = s;
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

		MINT_INLINE float IRenderer::PackBits2_30AsFloat(const uint32 _2bits, const uint32 _30bits) noexcept
		{
			const uint32 packedUint = (_2bits << 30) | (_30bits);
			return *reinterpret_cast<const float*>(&packedUint);
		}

		MINT_INLINE float IRenderer::PackBits4_28AsFloat(const uint32 _4bits, const uint32 _28bits) noexcept
		{
			const uint32 packedUint = (_4bits << 28) | (_28bits);
			return *reinterpret_cast<const float*>(&packedUint);
		}
	}
}
