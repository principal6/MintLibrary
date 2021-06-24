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
#include <MintRenderingBase/Include/ShapeRendererContext.h>
#include <MintRenderingBase/Include/FontRendererContext.h>

#include <MintRenderingBase/Include/GuiContext.h>

#include <MintRenderingBase/Include/CppHlsl/Interpreter.h>

#include <MintMath/Include/Float4x4.h>

#include <Assets/CppHlsl/CppHlslStreamData.h>
#include <Assets/CppHlsl/CppHlslConstantBuffers.h>
#include <Assets/CppHlsl/CppHlslStructuredBuffers.h>


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


        class SafeResourceMapper
        {
        public:
                                        SafeResourceMapper(GraphicDevice* const graphicDevice, ID3D11Resource* const resource, const uint32 subresource);
                                        ~SafeResourceMapper();

        public:
            const bool                  isValid() const noexcept;
            void                        set(const void* const data, const uint32 size) noexcept;

        private:
            GraphicDevice* const        _graphicDevice;
            ID3D11Resource* const       _resource;
            const uint32                _subresource;
            D3D11_MAPPED_SUBRESOURCE    _mappedSubresource;
        };


        class GraphicDevice final
        {
        public:
            class StateManager
            {
            public:
                                        StateManager(GraphicDevice* const graphicDevice);
                                        ~StateManager() = default;

            public: // IA
                void                    setIaInputLayout(ID3D11InputLayout* const iaInputLayout) noexcept;
                void                    setIaRenderingPrimitive(const RenderingPrimitive renderingPrimitive) noexcept;
                void                    setIaVertexBuffers(const int32 bindingStartSlot, const uint32 bufferCount, ID3D11Buffer* const* const buffers, const uint32* const strides, const uint32* const offsets) noexcept;
                void                    setIaIndexBuffer(ID3D11Buffer* const buffer, const DXGI_FORMAT format, const uint32 offset) noexcept;

            public: // RS
                void                    setRsRasterizerState(ID3D11RasterizerState* const rsRasterizerState) noexcept;
                void                    setRsViewport(const D3D11_VIEWPORT rsViewport) noexcept;
                void                    setRsScissorRectangle(const D3D11_RECT rsScissorRectangle) noexcept;

            public: // Shader
                void                    setVsShader(ID3D11VertexShader* const shader) noexcept;
                void                    setGsShader(ID3D11GeometryShader* const shader) noexcept;
                void                    setPsShader(ID3D11PixelShader* const shader) noexcept;

            public: // Resources
                void                    setVsResources(const DxResource& resource) noexcept;
                void                    setGsResources(const DxResource& resource) noexcept;
                void                    setPsResources(const DxResource& resource) noexcept;
            
            public: // Constant Buffers
                void                    setVsConstantBuffers(const DxResource& constantBuffer);
                void                    setGsConstantBuffers(const DxResource& constantBuffer);
                void                    setPsConstantBuffers(const DxResource& constantBuffer);
            
            private:
                GraphicDevice* const    _graphicDevice;

            private: // IA
                RenderingPrimitive      _iaRenderingPrimitive;
                ID3D11InputLayout*      _iaInputLayout;

            private: // RS
                ID3D11RasterizerState*  _rsRasterizerState;
                D3D11_VIEWPORT          _rsViewport;
                D3D11_RECT              _rsScissorRectangle;

            private: // Shaders
                ID3D11VertexShader*     _vsShader;
                ID3D11GeometryShader*   _gsShader;
                ID3D11PixelShader*      _psShader;

            private: // Resources
                mint::Vector<DxObjectId>    _vsShaderResources;
                mint::Vector<DxObjectId>    _gsShaderResources;
                mint::Vector<DxObjectId>    _psShaderResources;

            private: // Constant Buffers
                mint::Vector<DxObjectId>    _vsConstantBuffers;
                mint::Vector<DxObjectId>    _gsConstantBuffers;
                mint::Vector<DxObjectId>    _psConstantBuffers;
            };
            
            friend SafeResourceMapper;
            friend StateManager;

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
            void                                                draw(const uint32 vertexCount, const uint32 vertexOffset) noexcept;
            void                                                drawIndexed(const uint32 indexCount, const uint32 indexOffset, const uint32 vertexOffset) noexcept;
            void                                                endRendering();

        public:
            void                                                useScissorRectangles() noexcept;
            void                                                useFullScreenViewport() noexcept;
            void                                                useWireFrameNoCullingRasterizer() noexcept;
            void                                                useWireFrameCullBackRasterizer() noexcept;
            void                                                useSolidCullBackRasterizer() noexcept;
            const D3D11_VIEWPORT&                               getFullScreenViewport() const noexcept;
            const mint::Rect&                                   getFullScreenClipRect() const noexcept;

        public:
            mint::RenderingBase::DxShaderPool&                  getShaderPool() noexcept;
            mint::RenderingBase::DxResourcePool&                getResourcePool() noexcept;
            mint::RenderingBase::ShapeRendererContext&          getShapeRendererContext() noexcept;
            mint::RenderingBase::FontRendererContext&           getFontRendererContext() noexcept;
            mint::RenderingBase::ShapeFontRendererContext&      getShapeFontRendererContext() noexcept;
            mint::Gui::GuiContext&                              getGuiContext() noexcept;
            const mint::CppHlsl::Interpreter&                   getCppHlslSteamData() const noexcept;
            const mint::CppHlsl::Interpreter&                   getCppHlslConstantBuffers() const noexcept;
            StateManager&                                       getStateManager() noexcept;

        public: // Common buffers
            DxObjectId                                          getCommonCbTransformId() const noexcept;
            DxObjectId                                          getCommonSbTransformId() const noexcept;
            DxObjectId                                          getCommonSbMaterialId() const noexcept;

        public:
            void                                                initialize2DProjectionMatrix(const mint::Float2& windowSize) noexcept;
            void                                                setViewMatrix(const mint::Float4x4& viewMatrix) noexcept;
            void                                                setProjectionMatrix(const mint::Float4x4& projectionMatrix) noexcept;
            void                                                updateCbView() noexcept;

        public:
            ID3D11Device*                                       getDxDevice() noexcept;
            const mint::Int2&                                   getWindowSize() const noexcept;
            mint::Float2                                        getWindowSizeFloat2() const noexcept;
            mint::Window::IWindow*                              getWindow() noexcept;

        private:
            mint::Window::IWindow*                              _window;

        private:
            mint::RenderingBase::Color                          _clearColor;

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
            mint::Rect                                          _fullScreenClipRect;

        private:
            DxShaderHeaderMemory                                _shaderHeaderMemory;
            DxShaderPool                                        _shaderPool;
            DxResourcePool                                      _resourcePool;

        private:
            mint::RenderingBase::CB_View                        _cbViewData;
            DxObjectId                                          _cbViewId;

        private: // Common buffers
            DxObjectId                                          _cbTransformId;
            DxObjectId                                          _sbTransformId;
            DxObjectId                                          _sbMaterialId;

        private:
            ComPtr<ID3D11SamplerState>                          _samplerState;
            ComPtr<ID3D11BlendState>                            _blendState;
    #pragma endregion

        private:
            StateManager                                        _stateManager;

        private:
            mint::CppHlsl::Interpreter                          _cppHlslStreamData;
            mint::CppHlsl::Interpreter                          _cppHlslConstantBuffers;
            mint::CppHlsl::Interpreter                          _cppHlslStructuredBuffers;

        private:
            mint::RenderingBase::ShapeRendererContext           _shapeRendererContext;
            mint::RenderingBase::FontRendererContext            _fontRendererContext;
            mint::RenderingBase::ShapeFontRendererContext       _shapeFontRendererContext;
            bool                                                _needEndRenderingCall;

        private:
            mint::Gui::GuiContext                               _guiContext;
        };
    }
}


#include <MintRenderingBase/Include/GraphicDevice.inl>


#endif // !MINT_GRAPHIC_DEVICE_H
