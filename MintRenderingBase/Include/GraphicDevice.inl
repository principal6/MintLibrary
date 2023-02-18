#pragma once


namespace mint
{
	namespace Rendering
	{
		MINT_INLINE D3D11_RECT rectToD3dRect(const Rect& rect) noexcept
		{
			D3D11_RECT result;
			result.left = static_cast<LONG>(rect.Left());
			result.right = static_cast<LONG>(rect.Right());
			result.top = static_cast<LONG>(rect.Top());
			result.bottom = static_cast<LONG>(rect.Bottom());
			return result;
		}


		MINT_INLINE DxShaderPool& GraphicDevice::getShaderPool() noexcept
		{
			return _shaderPool;
		}

		MINT_INLINE DxResourcePool& GraphicDevice::getResourcePool() noexcept
		{
			return _resourcePool;
		}

		MINT_INLINE ShapeRendererContext& GraphicDevice::getShapeRendererContext() noexcept
		{
			return _shapeRendererContext;
		}

		MINT_INLINE GUI::GUIContext& GraphicDevice::getGUIContext() noexcept
		{
			return _guiContext;
		}

		MINT_INLINE const Language::CppHlsl::Interpreter& GraphicDevice::getCppHlslSteamData() const noexcept
		{
			return _cppHlslStreamData;
		}

		MINT_INLINE const Language::CppHlsl::Interpreter& GraphicDevice::getCppHlslConstantBuffers() const noexcept
		{
			return _cppHlslConstantBuffers;
		}

		MINT_INLINE GraphicDevice::StateManager& GraphicDevice::getStateManager() noexcept
		{
			return _stateManager;
		}

		MINT_INLINE GraphicObjectID GraphicDevice::getCommonCbTransformID() const noexcept
		{
			return _cbTransformID;
		}

		MINT_INLINE GraphicObjectID GraphicDevice::getCommonSBTransformID() const noexcept
		{
			return _sbTransformID;
		}

		MINT_INLINE GraphicObjectID GraphicDevice::getCommonSBMaterialID() const noexcept
		{
			return _sbMaterialID;
		}

		MINT_INLINE ID3D11Device* GraphicDevice::getDxDevice() noexcept
		{
			return _device.Get();
		}
	}
}
