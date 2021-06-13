#pragma once


#ifndef MINT_GRAPHIC_DEVICE_H
#define MINT_GRAPHIC_DEVICE_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/StaticArray.h>
#include <MintContainer/Include/ScopeString.h>
#include <MintContainer/Include/UniqueString.h>

#include <MintRenderingBase/Include/DxShaderHeaderMemory.h>
#include <MintRenderingBase/Include/DxShader.h>
#include <MintRenderingBase/Include/DxResource.h>
#include <MintRenderingBase/Include/LowLevelRenderer.h>
#include <MintRenderingBase/Include/RectangleRendererContext.h>
#include <MintRenderingBase/Include/ShapeRendererContext.h>
#include <MintRenderingBase/Include/FontRendererContext.h>

#include <MintRenderingBase/Include/GuiContext.h>

#include <MintRenderingBase/Include/Language/CppHlsl.h>

#include <MintMath/Include/Float4x4.h>

#include <Assets/CppHlsl/CppHlslStreamData.h>
#include <Assets/CppHlsl/CppHlslConstantBuffers.h>


namespace mint
{
    namespace Window
    {
        class IWindow;
    }

    namespace RenderingBase
    {
        using Microsoft::WRL::ComPtr;


        D3D11_RECT  rectToD3dRect(const Rect& rect) noexcept;


        class GraphicDevice final
        {
        public:
                                                                GraphicDevice();
                                                                ~GraphicDevice() = default;

        public:
            const bool                                          initialize(mint::Window::IWindow* const window);
            void                                                updateScreenSize();

        private:
            void                                                createDxDevice();
            const bool                                          loadFontData();

        private:
            const bool                                          createSwapChain(const mint::Int2& windowSize, const HWND windowHandle);
            const bool                                          initializeBackBuffer();
            const bool                                          initializeDepthStencilBufferAndView(const mint::Int2& windowSize);
            const bool                                          initializeDepthStencilStates();
            void                                                initializeShaderHeaderMemory();
            void                                                initializeShaders();
            void                                                initializeSamplerStates();
            void                                                initializeBlendStates();
            void                                                initializeFullScreenData(const mint::Int2& windowSize);
            void                                                setDefaultRenderTargetsAndDepthStencil();

        public:
            void                                                beginRendering();
            void                                                endRendering();

        public:
            void                                                useScissorRectanglesWithMultipleViewports() noexcept;
            void                                                useFullScreenViewport() noexcept;
            void                                                useWireFrameNoCullingRasterizer() noexcept;
            void                                                useWireFrameCullBackRasterizer() noexcept;
            void                                                useSolidCullBackRasterizer() noexcept;
            const D3D11_VIEWPORT&                               getFullScreenViewport() const noexcept;
            const D3D11_RECT&                                   getFullScreenScissorRectangle() const noexcept;

        public:
            mint::RenderingBase::DxShaderPool&                    getShaderPool() noexcept;
            mint::RenderingBase::DxResourcePool&                  getResourcePool() noexcept;
            mint::RenderingBase::RectangleRendererContext&        getRectangleRendererContext() noexcept;
            mint::RenderingBase::ShapeRendererContext&            getShapeRendererContext() noexcept;
            mint::RenderingBase::FontRendererContext&             getFontRendererContext() noexcept;
            mint::RenderingBase::ShapeFontRendererContext&        getShapeFontRendererContext() noexcept;
            mint::Gui::GuiContext&                                getGuiContext() noexcept;
            const mint::Language::CppHlsl&                        getCppHlslSteamData() const noexcept;
            const mint::Language::CppHlsl&                        getCppHlslConstantBuffers() const noexcept;

        public:
            void                                                initialize2DProjectionMatrix(const mint::Float2& windowSize) noexcept;
            void                                                setViewMatrix(const mint::Float4x4& viewMatrix) noexcept;
            void                                                setProjectionMatrix(const mint::Float4x4& projectionMatrix) noexcept;
            void                                                updateViewProjectionMatrix() noexcept;

        public:
            ID3D11Device*                                       getDxDevice() noexcept;
            ID3D11DeviceContext*                                getDxDeviceContext() noexcept;
            const mint::Int2&                                     getWindowSize() const noexcept;
            mint::Float2                                          getWindowSizeFloat2() const noexcept;
            mint::Window::IWindow*                                getWindow() noexcept;

        private:
            mint::Window::IWindow*                                _window;

        private:
            mint::RenderingBase::Color                            _clearColor;

    #pragma region DirectX
        private:
            ComPtr<IDXGISwapChain>                              _swapChain;
            ComPtr<ID3D11Device>                                _device;
            ComPtr<ID3D11DeviceContext>                         _deviceContext;

        private:
            ComPtr<ID3D11RenderTargetView>                      _backBufferRtv;
            ComPtr<ID3D11Texture2D>                             _depthStencilBuffer;
            ComPtr<ID3D11DepthStencilView>                      _depthStencilView;
            ComPtr<ID3D11DepthStencilState>                     _depthStencilStateLessEqual;

        private:
            ID3D11RasterizerState*                              _currentRasterizerFor3D;
            ComPtr<ID3D11RasterizerState>                       _rasterizerStateSolidCullBack;
            ComPtr<ID3D11RasterizerState>                       _rasterizerStateWireFrameNoCulling;
            ComPtr<ID3D11RasterizerState>                       _rasterizerStateWireFrameCullBack;
            ComPtr<ID3D11RasterizerState>                       _rasterizerStateScissorRectangles;
            D3D11_VIEWPORT                                      _fullScreenViewport;
            D3D11_RECT                                          _fullScreenScissorRectangle;

        private:
            DxShaderHeaderMemory                                _shaderHeaderMemory;
            DxShaderPool                                        _shaderPool;
            DxResourcePool                                      _resourcePool;

        private:
            mint::RenderingBase::CB_View                          _cbViewData;
            DxObjectId                                          _cbViewId;

        private:
            ComPtr<ID3D11SamplerState>                          _samplerState;
            ComPtr<ID3D11BlendState>                            _blendState;
    #pragma endregion

        private:
            mint::Language::CppHlsl                               _cppHlslStreamData;
            mint::Language::CppHlsl                               _cppHlslConstantBuffers;
            mint::Language::CppHlsl                               _cppHlslStructuredBuffers;

        private:
            mint::RenderingBase::RectangleRendererContext         _rectangleRendererContext;
            mint::RenderingBase::ShapeRendererContext             _shapeRendererContext;
            mint::RenderingBase::FontRendererContext              _fontRendererContext;
            mint::RenderingBase::ShapeFontRendererContext         _shapeFontRendererContext;
            bool                                                _needEndRenderingCall;

        private:
            mint::Gui::GuiContext                                 _guiContext;
        };
    }
}


#include <MintRenderingBase/Include/GraphicDevice.inl>


#endif // !MINT_GRAPHIC_DEVICE_H
