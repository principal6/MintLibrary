#include <stdafx.h>
#include <FsLibrary/SimpleRendering/IRendererContext.h>


namespace fs
{
	namespace SimpleRendering
	{
		const Color Color::kTransparent = Color(0, 0, 0, 0);
		const Color Color::kWhite = Color(255, 255, 255, 255);
		const Color Color::kBlack = Color(0, 0, 0, 255);


		IRendererContext::IRendererContext(fs::SimpleRendering::GraphicDevice* const graphicDevice)
			: _graphicDevice{ graphicDevice }
			, _defaultColor{ 1.0f, 1.0f, 1.0f, 1.0f }
		{
			FS_ASSERT("김장원", nullptr != _graphicDevice, "GraphicDevice 가 nullptr 이면 안 됩니다!");
		}

		void IRendererContext::setColor(const fs::SimpleRendering::Color& color) noexcept
		{
			_colorArray.clear();
			_defaultColor = color;
		}

		void IRendererContext::setColor(const std::vector<fs::SimpleRendering::Color>& colorArray) noexcept
		{
			_colorArray = colorArray;

			if (_colorArray.empty() == false)
			{
				_defaultColor = _colorArray.back();
			}
		}

		const bool Color::isTransparent() const noexcept
		{
			return (_raw._w <= 0.0f);
		}
	}
}
