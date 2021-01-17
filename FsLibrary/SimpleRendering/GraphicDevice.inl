#include "GraphicDevice.h"
namespace fs
{
	namespace SimpleRendering
	{
		FS_INLINE fs::SimpleRendering::DxShaderPool& GraphicDevice::getShaderPool() noexcept
		{
			return _shaderPool;
		}

		FS_INLINE fs::SimpleRendering::DxBufferPool& GraphicDevice::getBufferPool() noexcept
		{
			return _bufferPool;
		}

		FS_INLINE fs::SimpleRendering::RectangleRenderer& GraphicDevice::getRectangleRenderer() noexcept
		{
			return _rectangleRenderer; 
		}

		FS_INLINE fs::SimpleRendering::ShapeRenderer& GraphicDevice::getShapeRenderer() noexcept
		{
			return _shapeRenderer;
		}

		FS_INLINE fs::SimpleRendering::FontRenderer& GraphicDevice::getFontRenderer() noexcept
		{
			return _fontRenderer;
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
