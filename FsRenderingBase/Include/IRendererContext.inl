#include "IRendererContext.h"
namespace fs
{
	namespace RenderingBase
	{
		inline const Color Color::kTransparent = Color(0, 0, 0, 0);
		inline const Color Color::kWhite = Color(255, 255, 255, 255);
		inline const Color Color::kBlack = Color(0, 0, 0, 255);


		FS_INLINE void PixelRgba::setSize(const uint32 width, const uint32 height) noexcept
		{
			_width = width;
			_byteArray.resize(width * height * kByteCountPerPixel);
		}

		FS_INLINE void PixelRgba::setSize(const uint32 width, const uint32 height, const Color& initializationColor) noexcept
		{
			_width = width;
			const uint32 pixelCount = width * height;
			_byteArray.resize(pixelCount * kByteCountPerPixel);
			const byte r = initializationColor.rAsByte();
			const byte g = initializationColor.gAsByte();
			const byte b = initializationColor.bAsByte();
			const byte a = initializationColor.aAsByte();
			for (uint32 pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex)
			{
				_byteArray[pixelIndex * kByteCountPerPixel + 0] = r;
				_byteArray[pixelIndex * kByteCountPerPixel + 1] = g;
				_byteArray[pixelIndex * kByteCountPerPixel + 2] = b;
				_byteArray[pixelIndex * kByteCountPerPixel + 3] = a;
			}
		}

		FS_INLINE void PixelRgba::setColor(const uint32 x, const uint32 y, const Color& color) noexcept
		{
			const uint32 baseIndex = convertXyToIndex(x, y);
			_byteArray[baseIndex + 0] = color.rAsByte();
			_byteArray[baseIndex + 1] = color.gAsByte();
			_byteArray[baseIndex + 2] = color.bAsByte();
			_byteArray[baseIndex + 3] = color.aAsByte();
		}

		FS_INLINE void PixelRgba::setR(const uint32 x, const uint32 y, const byte value) noexcept
		{
			_byteArray[convertXyToIndex(x, y) + 0] = value;
		}

		FS_INLINE void PixelRgba::setG(const uint32 x, const uint32 y, const byte value) noexcept
		{
			_byteArray[convertXyToIndex(x, y) + 1] = value;
		}

		FS_INLINE void PixelRgba::setB(const uint32 x, const uint32 y, const byte value) noexcept
		{
			_byteArray[convertXyToIndex(x, y) + 2] = value;
		}

		FS_INLINE void PixelRgba::setA(const uint32 x, const uint32 y, const byte value) noexcept
		{
			_byteArray[convertXyToIndex(x, y) + 3] = value;
		}

		FS_INLINE const uint32 PixelRgba::getByteCount() const noexcept
		{
			return static_cast<uint32>(_byteArray.size());
		}

		FS_INLINE const byte* PixelRgba::getByteRawPointer() const noexcept
		{
			return _byteArray.data();
		}

		FS_INLINE const uint32 PixelRgba::convertXyToIndex(const uint32 x, const uint32 y) const noexcept
		{
			return fs::min(static_cast<uint32>((_width * kByteCountPerPixel * y) + kByteCountPerPixel * x), static_cast<uint32>(_byteArray.size()));
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
