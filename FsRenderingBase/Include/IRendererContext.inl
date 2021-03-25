#include "IRendererContext.h"
#pragma once


namespace fs
{
	namespace RenderingBase
	{
		inline const Color Color::kTransparent = Color(0, 0, 0, 0);
		inline const Color Color::kWhite = Color(255, 255, 255, 255);
		inline const Color Color::kBlack = Color(0, 0, 0, 255);

		
		FS_INLINE void fs::RenderingBase::ColorImage::setSize(const fs::Int2& size) noexcept
		{
			_size = size;
			_colorArray.resize(_size._x * _size._y);
		}

		FS_INLINE const fs::Int2& ColorImage::getSize() const noexcept
		{
			return _size;
		}

		FS_INLINE void ColorImage::fill(const Color& color) noexcept
		{
			for (auto& colorEntry : _colorArray)
			{
				colorEntry = color;
			}
		}

		FS_INLINE void ColorImage::fillRect(const fs::Int2& position, const fs::Int2& size, const Color& color) noexcept
		{
			if (size._x <= 0 || size._y <= 0)
			{
				return;
			}

			const int32 beginX = fs::max(position._x, 0);
			const int32 beginY = fs::max(position._y, 0);
			if (_size._x <= beginX || _size._y <= beginY)
			{
				return;
			}

			const int32 endX = fs::min(position._x + size._x, _size._x);
			const int32 endY = fs::min(position._y + size._y, _size._y);
			if (endX < 0 || endY < 0)
			{
				return;
			}

			for (int32 y = beginY; y < endY; ++y)
			{
				const int32 base = _size._x * y;
				for (int32 x = beginX; x < endX; ++x)
				{
					_colorArray[base + x] = color;
				}
			}
		}

		FS_INLINE void ColorImage::setPixel(const fs::Int2& at, const Color& color) noexcept
		{
			const int32 index = convertXyToIndex(at._x, at._y);
			_colorArray[index] = color;
		}

		FS_INLINE const Color& ColorImage::getPixel(const fs::Int2& at) noexcept
		{
			const int32 index = convertXyToIndex(at._x, at._y);
			return _colorArray[index];
		}

		FS_INLINE const byte* ColorImage::buildPixelRgbaArray() noexcept
		{
			static constexpr uint32 kByteCountPerPixel = 4;
			const uint32 pixelCount = static_cast<uint32>(_colorArray.size());
			_byteArray.resize(pixelCount * kByteCountPerPixel);
			for (uint32 pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex)
			{
				const Color& color = _colorArray[pixelIndex];
				_byteArray[pixelIndex * kByteCountPerPixel + 0] = color.rAsByte();
				_byteArray[pixelIndex * kByteCountPerPixel + 1] = color.gAsByte();
				_byteArray[pixelIndex * kByteCountPerPixel + 2] = color.bAsByte();
				_byteArray[pixelIndex * kByteCountPerPixel + 3] = color.aAsByte();
			}
			return _byteArray.data();
		}

		FS_INLINE const int32 ColorImage::convertXyToIndex(const uint32 x, const uint32 y) const noexcept
		{
			return fs::min(static_cast<int32>((_size._x * y) + x), static_cast<int32>(_colorArray.size() - 1));
		}


		inline IRendererContext::IRendererContext(fs::RenderingBase::GraphicDevice* const graphicDevice)
			: _graphicDevice{ graphicDevice }
			, _defaultColor{ Color::kWhite }
			, _viewportIndex{ 0.0f }
			, _useMultipleViewports{ false }
		{
			FS_ASSERT("김장원", nullptr != _graphicDevice, "GraphicDevice 가 nullptr 이면 안 됩니다!");
		}

		FS_INLINE void IRendererContext::setUseMultipleViewports() noexcept
		{
			_useMultipleViewports = true;
		}

		FS_INLINE const bool IRendererContext::getUseMultipleViewports() const noexcept
		{
			return _useMultipleViewports;
		}


		FS_INLINE const bool Color::isTransparent() const noexcept
		{
			return (_raw._w <= 0.0f);
		}

		FS_INLINE void IRendererContext::setPosition(const fs::Float4& position) noexcept
		{
			_position = position;
		}

		FS_INLINE void IRendererContext::setPositionZ(const float s) noexcept
		{
			_position._z = s;
		}

		FS_INLINE void IRendererContext::setColor(const fs::RenderingBase::Color& color) noexcept
		{
			_colorArray.clear();
			_defaultColor = color;
		}

		FS_INLINE void IRendererContext::setColor(const std::vector<fs::RenderingBase::Color>& colorArray) noexcept
		{
			_colorArray = colorArray;

			if (_colorArray.empty() == false)
			{
				_defaultColor = _colorArray.back();
			}
		}

		FS_INLINE void IRendererContext::setViewportIndex(const uint32 viewportIndex) noexcept
		{
			_viewportIndex = static_cast<float>(viewportIndex);
		}

		FS_INLINE const fs::Float4& IRendererContext::getColorInternal(const uint32 index) const noexcept
		{
			const uint32 colorCount = static_cast<uint32>(_colorArray.size());
			return (colorCount <= index) ? _defaultColor : _colorArray[index];
		}
	}
}
