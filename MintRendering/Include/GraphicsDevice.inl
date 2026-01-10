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

		MINT_INLINE const Language::CppHlsl::Interpreter& GraphicsDevice::GetCppHlslSteamData() const noexcept
		{
			return _cppHlslStreamData;
		}

		MINT_INLINE const Language::CppHlsl::Interpreter& GraphicsDevice::GetCppHlslConstantBuffers() const noexcept
		{
			return _cppHlslConstantBuffers;
		}

		MINT_INLINE GraphicsDevice::StateManager& GraphicsDevice::GetStateManager() noexcept
		{
			return _stateManager;
		}

		MINT_INLINE GraphicsObjectID GraphicsDevice::GetCommonCBTransformID() const noexcept
		{
			return _cbTransformID;
		}

		MINT_INLINE GraphicsObjectID GraphicsDevice::GetCommonCBMaterialID() const noexcept
		{
			return _cbMaterialID;
		}

		MINT_INLINE GraphicsObjectID GraphicsDevice::GetCommonSBTransformID() const noexcept
		{
			return _sbTransformID;
		}

		MINT_INLINE ID3D11Device* GraphicsDevice::GetDxDevice() noexcept
		{
			return _device.Get();
		}

		MINT_INLINE ID3D11DeviceContext* GraphicsDevice::GetDxDeviceContext() noexcept
		{
			return _deviceContext.Get();
		}

		MINT_INLINE const Int2& GraphicsDevice::GetWindowSize() const noexcept
		{
			return _cachedWindowSize;
		}
	}
}
