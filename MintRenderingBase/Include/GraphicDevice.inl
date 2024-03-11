#pragma once


namespace mint
{
	namespace Rendering
	{
		MINT_INLINE D3D11_RECT RectToD3dRect(const Rect& rect) noexcept
		{
			D3D11_RECT result;
			result.left = static_cast<LONG>(rect.Left());
			result.right = static_cast<LONG>(rect.Right());
			result.top = static_cast<LONG>(rect.Top());
			result.bottom = static_cast<LONG>(rect.Bottom());
			return result;
		}


		MINT_INLINE ShaderPool& GraphicDevice::GetShaderPool() noexcept
		{
			return _shaderPool;
		}

		MINT_INLINE GraphicResourcePool& GraphicDevice::GetResourcePool() noexcept
		{
			return _resourcePool;
		}

		MINT_INLINE ShapeRendererContext& GraphicDevice::GetShapeRendererContext() noexcept
		{
			return _shapeRendererContext;
		}
		
		MINT_INLINE const Language::CppHlsl::Interpreter& GraphicDevice::GetCppHlslSteamData() const noexcept
		{
			return _cppHlslStreamData;
		}

		MINT_INLINE const Language::CppHlsl::Interpreter& GraphicDevice::GetCppHlslConstantBuffers() const noexcept
		{
			return _cppHlslConstantBuffers;
		}

		MINT_INLINE GraphicDevice::StateManager& GraphicDevice::GetStateManager() noexcept
		{
			return _stateManager;
		}

		MINT_INLINE GraphicObjectID GraphicDevice::GetCommonCBTransformID() const noexcept
		{
			return _cbTransformID;
		}

		MINT_INLINE GraphicObjectID GraphicDevice::GetCommonSBTransformID() const noexcept
		{
			return _sbTransformID;
		}

		MINT_INLINE GraphicObjectID GraphicDevice::GetCommonSBMaterialID() const noexcept
		{
			return _sbMaterialID;
		}

		MINT_INLINE ID3D11Device* GraphicDevice::GetDxDevice() noexcept
		{
			return _device.Get();
		}

		MINT_INLINE ID3D11DeviceContext* GraphicDevice::GetDxDeviceContext() noexcept
		{
			return _deviceContext.Get();
		}
	}
}
