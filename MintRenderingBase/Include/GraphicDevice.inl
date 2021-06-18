#include "GraphicDevice.h"
#pragma once


namespace mint
{
    namespace RenderingBase
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


        MINT_INLINE mint::RenderingBase::DxShaderPool& GraphicDevice::getShaderPool() noexcept
        {
            return _shaderPool;
        }

        MINT_INLINE mint::RenderingBase::DxResourcePool& GraphicDevice::getResourcePool() noexcept
        {
            return _resourcePool;
        }

        MINT_INLINE mint::RenderingBase::ShapeRendererContext& GraphicDevice::getShapeRendererContext() noexcept
        {
            return _shapeRendererContext;
        }

        MINT_INLINE mint::RenderingBase::FontRendererContext& GraphicDevice::getFontRendererContext() noexcept
        {
            return _fontRendererContext;
        }

        MINT_INLINE mint::RenderingBase::ShapeFontRendererContext& GraphicDevice::getShapeFontRendererContext() noexcept
        {
            return _shapeFontRendererContext;
        }

        MINT_INLINE mint::Gui::GuiContext& GraphicDevice::getGuiContext() noexcept
        {
            return _guiContext;
        }

        MINT_INLINE const mint::CppHlsl::Interpreter& GraphicDevice::getCppHlslSteamData() const noexcept
        {
            return _cppHlslStreamData;
        }

        MINT_INLINE const mint::CppHlsl::Interpreter& GraphicDevice::getCppHlslConstantBuffers() const noexcept
        {
            return _cppHlslConstantBuffers;
        }

        MINT_INLINE DxObjectId GraphicDevice::getCommonCbTransformId() const noexcept
        {
            return _cbTransformId;
        }

        MINT_INLINE DxObjectId GraphicDevice::getCommonSbTransformId() const noexcept
        {
            return _sbTransformId;
        }

        MINT_INLINE DxObjectId GraphicDevice::getCommonSbMaterialId() const noexcept
        {
            return _sbMaterialId;
        }

        MINT_INLINE ID3D11Device* GraphicDevice::getDxDevice() noexcept
        {
            return _device.Get();
        }
        
        MINT_INLINE ID3D11DeviceContext* GraphicDevice::getDxDeviceContext() noexcept
        {
            return _deviceContext.Get();
        }
    }
}
