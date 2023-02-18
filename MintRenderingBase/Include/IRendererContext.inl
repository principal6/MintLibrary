#pragma once


namespace mint
{
	namespace Rendering
	{
		MINT_INLINE void IRendererContext::SetUseMultipleViewports() noexcept
		{
			_useMultipleViewports = true;
		}

		MINT_INLINE bool IRendererContext::IsUsingMultipleViewports() const noexcept
		{
			return _useMultipleViewports;
		}

		MINT_INLINE void IRendererContext::SetPosition(const Float4& position) noexcept
		{
			_position = position;
		}

		MINT_INLINE void IRendererContext::SetPositionZ(const float s) noexcept
		{
			_position._z = s;
		}

		MINT_INLINE void IRendererContext::SetColor(const ByteColor& color) noexcept
		{
			SetColor(Color(color.R(), color.G(), color.B(), color.A()));
		}

		MINT_INLINE void IRendererContext::SetColor(const Color& color) noexcept
		{
			_colorArray.Clear();
			_defaultColor = color;
		}

		MINT_INLINE void IRendererContext::SetColor(const Vector<Color>& colorArray) noexcept
		{
			_colorArray = colorArray;

			if (_colorArray.IsEmpty() == false)
			{
				_defaultColor = _colorArray.Back();
			}
		}

		MINT_INLINE void IRendererContext::SetClipRect(const Rect& clipRect) noexcept
		{
			_clipRect = clipRect;
		}

		MINT_INLINE const Float4& IRendererContext::GetColorInternal(const uint32 index) const noexcept
		{
			const uint32 colorCount = static_cast<uint32>(_colorArray.Size());
			return (colorCount <= index) ? _defaultColor : _colorArray[index];
		}

		MINT_INLINE float IRendererContext::PackBits2_30AsFloat(const uint32 _2bits, const uint32 _30bits) noexcept
		{
			const uint32 packedUint = (_2bits << 30) | (_30bits);
			return *reinterpret_cast<const float*>(&packedUint);
		}

		MINT_INLINE float IRendererContext::PackBits4_28AsFloat(const uint32 _4bits, const uint32 _28bits) noexcept
		{
			const uint32 packedUint = (_4bits << 28) | (_28bits);
			return *reinterpret_cast<const float*>(&packedUint);
		}

		MINT_INLINE LowLevelRenderer<VS_INPUT_SHAPE>& IRendererContext::AccessLowLevelRenderer() noexcept
		{
			return *_lowLevelRenderer;
		}
	}
}
