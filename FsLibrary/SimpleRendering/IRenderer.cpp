#include "stdafx.h"
#include <FsLibrary/SimpleRendering/IRenderer.h>


namespace fs
{
	namespace SimpleRendering
	{
		IRenderer::IRenderer(fs::SimpleRendering::GraphicDevice* const graphicDevice)
			: _graphicDevice{ graphicDevice }
			, _defaultColor{ 1.0f, 1.0f, 1.0f, 1.0f }
		{
			FS_ASSERT("김장원", nullptr != _graphicDevice, "GraphicDevice 가 nullptr 이면 안 됩니다!");
		}

		void IRenderer::setColor(const fs::Float4& color) noexcept
		{
			_colorArray.clear();
			_defaultColor = color;
		}

		void IRenderer::setColor(const std::vector<fs::Float4>& colorArray) noexcept
		{
			_colorArray = colorArray;

			if (_colorArray.empty() == false)
			{
				_defaultColor = _colorArray.back();
			}
		}
	}
}
