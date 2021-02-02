namespace fs
{
	namespace SimpleRendering
	{
		FS_INLINE fs::SimpleRendering::DxShaderPool& GraphicDevice::getShaderPool() noexcept
		{
			return _shaderPool;
		}

		FS_INLINE fs::SimpleRendering::DxResourcePool& GraphicDevice::getResourcePool() noexcept
		{
			return _resourcePool;
		}

		FS_INLINE fs::SimpleRendering::RectangleRendererContext& GraphicDevice::getRectangleRendererContext() noexcept
		{
			return _rectangleRendererContext; 
		}

		FS_INLINE fs::SimpleRendering::ShapeRendererContext& GraphicDevice::getShapeRendererContext() noexcept
		{
			return _shapeRendererContext;
		}

		FS_INLINE fs::SimpleRendering::FontRendererContext& GraphicDevice::getFontRendererContext() noexcept
		{
			return _fontRendererContext;
		}

		FS_INLINE fs::Gui::GuiContext& GraphicDevice::getGuiContext() noexcept
		{
			return _guiContext;
		}

		FS_INLINE const fs::Language::CppHlsl& GraphicDevice::getCppHlslStructs() const noexcept
		{
			return _cppHlslStreamData;
		}

		FS_INLINE ID3D11Device* GraphicDevice::getDxDevice() noexcept
		{
			return _device.Get();
		}
		
		FS_INLINE ID3D11DeviceContext* GraphicDevice::getDxDeviceContext() noexcept
		{
			return _deviceContext.Get();
		}
	}
}
