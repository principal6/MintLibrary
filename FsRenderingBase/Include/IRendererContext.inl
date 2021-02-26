#include "IRendererContext.h"
namespace fs
{
	namespace RenderingBase
	{
		inline const Color Color::kTransparent = Color(0, 0, 0, 0);
		inline const Color Color::kWhite = Color(255, 255, 255, 255);
		inline const Color Color::kBlack = Color(0, 0, 0, 255);

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
