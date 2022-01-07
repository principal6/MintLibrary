#pragma once


namespace mint
{
    namespace Rendering
    {
        MINT_INLINE D3D11_RECT rectToD3dRect(const Rect& rect) noexcept
        {
            D3D11_RECT result;
            result.left   = static_cast<LONG>(rect.left());
            result.right  = static_cast<LONG>(rect.right());
            result.top    = static_cast<LONG>(rect.top());
            result.bottom = static_cast<LONG>(rect.bottom());
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

        MINT_INLINE FontRendererContext& GraphicDevice::getFontRendererContext() noexcept
        {
            return _fontRendererContext;
        }

        MINT_INLINE ShapeFontRendererContext& GraphicDevice::getShapeFontRendererContext() noexcept
        {
            return _shapeFontRendererContext;
        }

        MINT_INLINE Gui::GuiContext& GraphicDevice::getGuiContext() noexcept
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

        MINT_INLINE DxObjectID GraphicDevice::getCommonCbTransformID() const noexcept
        {
            return _cbTransformID;
        }

        MINT_INLINE DxObjectID GraphicDevice::getCommonSBTransformID() const noexcept
        {
            return _sbTransformID;
        }

        MINT_INLINE DxObjectID GraphicDevice::getCommonSBMaterialID() const noexcept
        {
            return _sbMaterialID;
        }

        MINT_INLINE ID3D11Device* GraphicDevice::getDxDevice() noexcept
        {
            return _device.Get();
        }
    }
}
