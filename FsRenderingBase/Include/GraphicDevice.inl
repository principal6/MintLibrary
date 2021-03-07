#include "GraphicDevice.h"
namespace fs
{
	namespace RenderingBase
	{
		FS_INLINE fs::RenderingBase::DxShaderPool& GraphicDevice::getShaderPool() noexcept
		{
			return _shaderPool;
		}

		FS_INLINE fs::RenderingBase::DxResourcePool& GraphicDevice::getResourcePool() noexcept
		{
			return _resourcePool;
		}

		FS_INLINE fs::RenderingBase::RectangleRendererContext& GraphicDevice::getRectangleRendererContext() noexcept
		{
			return _rectangleRendererContext; 
		}

		FS_INLINE fs::RenderingBase::ShapeRendererContext& GraphicDevice::getShapeRendererContext() noexcept
		{
			return _shapeRendererContext;
		}

		FS_INLINE fs::RenderingBase::FontRendererContext& GraphicDevice::getFontRendererContext() noexcept
		{
			return _fontRendererContext;
		}

		FS_INLINE fs::Gui::GuiContext& GraphicDevice::getGuiContext() noexcept
		{
			return _guiContext;
		}

		FS_INLINE const fs::Language::CppHlsl& GraphicDevice::getCppHlslSteamData() const noexcept
		{
			return _cppHlslStreamData;
		}

		FS_INLINE const fs::Language::CppHlsl& GraphicDevice::getCppHlslConstantBuffers() const noexcept
		{
			return _cppHlslConstantBuffers;
		}

		FS_INLINE fs::RenderingBase::CB_View& GraphicDevice::getCbViewData() noexcept
		{
			return _cbViewData;
		}

		FS_INLINE DxObjectId GraphicDevice::getCbViewId() const noexcept
		{
			return _cbViewId;
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
