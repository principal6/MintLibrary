#include <stdafx.h>
#include <MintRenderingBase/Include/GraphicDevice.h>

#include <d3dcompiler.h>
#include <functional>
#include <typeinfo>

#include <MintLibrary/Include/Algorithm.hpp>

#include <MintPlatform/Include/IWindow.h>
#include <MintPlatform/Include/WindowsWindow.h>

#include <MintContainer/Include/ScopeString.hpp>
#include <MintContainer/Include/HashMap.hpp>

#include <MintRenderingBase/Include/LowLevelRenderer.hpp>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")


namespace mint
{
    namespace RenderingBase
    {
        GraphicDevice::GraphicDevice()
            : _window{ nullptr }
            , _clearColor{ 0.875f, 0.875f, 0.875f, 1.0f }
            , _currentRasterizerFor3D{ nullptr }
            , _fullScreenViewport{}
            , _fullScreenScissorRectangle{}
            , _shaderPool{ this, &_shaderHeaderMemory, mint::RenderingBase::DxShaderVersion::v_5_0 }
            , _resourcePool{ this }
            , _rectangleRendererContext{ this }
            , _shapeRendererContext{ this }
            , _fontRendererContext{ this }
            , _shapeFontRendererContext{ this }
            , _needEndRenderingCall{ false }
            , _guiContext{ this }
        {
            __noop;
        }

        const bool GraphicDevice::initialize(mint::Window::IWindow* const window)
        {
            if (window == nullptr)
            {
                MINT_LOG_ERROR("김장원", "window 포인터가 nullptr 이면 안 됩니다!");
                return false;
            }

            _window = window;
            _clearColor = _window->getBackgroundColor();
            createDxDevice();
            if (loadFontData() == false)
            {
                return false;
            }

            _guiContext.initialize(kDefaultFont);

            return true;
        }

        void GraphicDevice::updateScreenSize()
        {
            _deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
            _deviceContext->OMSetDepthStencilState(nullptr, 0);

            _backBufferRtv->Release();

            _swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0);

            initializeBackBuffer();
            initializeDepthStencilBufferAndView(_window->getSize());
            initializeFullScreenData(_window->getSize());
            initialize2DProjectionMatrix(mint::Float2(_window->getSize()));

            setDefaultRenderTargetsAndDepthStencil();
        }

        void GraphicDevice::createDxDevice()
        {
            const mint::Window::WindowsWindow& windowsWindow = *static_cast<const mint::Window::WindowsWindow*>(_window);
            const mint::Int2& windowSize = windowsWindow.getSize();

            if (createSwapChain(windowSize, windowsWindow.getHandle()) == false)
            {
                return;
            }

            if (initializeBackBuffer() == false)
            {
                return;
            }

            if (initializeDepthStencilBufferAndView(windowSize) == false)
            {
                return;
            }
            initializeDepthStencilStates();

            initializeFullScreenData(windowSize);
            initializeShaderHeaderMemory();
            initializeShaders();
            initializeSamplerStates();
            initializeBlendStates();

            // Rasterizer states and viewport
            {
                D3D11_RASTERIZER_DESC rasterizerDescriptor;
                rasterizerDescriptor.AntialiasedLineEnable = TRUE;
                rasterizerDescriptor.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
                rasterizerDescriptor.DepthBias = 0;
                rasterizerDescriptor.DepthBiasClamp = 0.0f;
                rasterizerDescriptor.DepthClipEnable = TRUE;
                rasterizerDescriptor.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
                rasterizerDescriptor.FrontCounterClockwise = FALSE;
                rasterizerDescriptor.MultisampleEnable = TRUE;
                rasterizerDescriptor.ScissorEnable = FALSE;
                rasterizerDescriptor.SlopeScaledDepthBias = 0.0f;
                _device->CreateRasterizerState(&rasterizerDescriptor, _rasterizerStateWireFrameNoCulling.ReleaseAndGetAddressOf());

                rasterizerDescriptor.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
                _device->CreateRasterizerState(&rasterizerDescriptor, _rasterizerStateWireFrameCullBack.ReleaseAndGetAddressOf());

                rasterizerDescriptor.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
                _device->CreateRasterizerState(&rasterizerDescriptor, _rasterizerStateSolidCullBack.ReleaseAndGetAddressOf());

                rasterizerDescriptor.ScissorEnable = TRUE;
                _device->CreateRasterizerState(&rasterizerDescriptor, _rasterizerStateScissorRectangles.ReleaseAndGetAddressOf());

                _currentRasterizerFor3D = _rasterizerStateSolidCullBack.Get();
            }

            setDefaultRenderTargetsAndDepthStencil();
        }

        const bool GraphicDevice::loadFontData()
        {
            if (_fontRendererContext.existsFontData(kDefaultFont) == false)
            {
                _fontRendererContext.pushGlyphRange(mint::RenderingBase::GlyphRange(0, 0x33DD));
                _fontRendererContext.pushGlyphRange(mint::RenderingBase::GlyphRange(L'가', L'힣'));
                if (_fontRendererContext.bakeFontData(kDefaultFont, kDefaultFontSize, kDefaultFont, 2048, 1, 1) == false)
                {
                    MINT_LOG_ERROR("김장원", "폰트 데이터를 Bake 하는 데 실패했습니다!");
                    return false;
                }
            }

            if (_fontRendererContext.loadFontData(kDefaultFont) == false)
            {
                MINT_LOG_ERROR("김장원", "폰트 데이터를 로드하지 못했습니다!");
                return false;
            }

            _shapeFontRendererContext.initializeFontData(_fontRendererContext.getFontData());

            return true;
        }

        const bool GraphicDevice::createSwapChain(const mint::Int2& windowSize, const HWND windowHandle)
        {
            DXGI_SWAP_CHAIN_DESC swapChainDescriptor{};
            swapChainDescriptor.BufferCount = 1;
            swapChainDescriptor.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
            swapChainDescriptor.BufferDesc.Width = static_cast<UINT>(windowSize._x);
            swapChainDescriptor.BufferDesc.Height = static_cast<UINT>(windowSize._y);
            swapChainDescriptor.BufferDesc.RefreshRate.Denominator = 1;
            swapChainDescriptor.BufferDesc.RefreshRate.Numerator = 60;
            swapChainDescriptor.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
            swapChainDescriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
            swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDescriptor.Flags = 0;
            swapChainDescriptor.OutputWindow = windowHandle;
            swapChainDescriptor.SampleDesc.Count = 1;
            swapChainDescriptor.SampleDesc.Quality = 0;
            swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
            swapChainDescriptor.Windowed = TRUE;

            if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
                &swapChainDescriptor, _swapChain.ReleaseAndGetAddressOf(), _device.ReleaseAndGetAddressOf(), nullptr, _deviceContext.ReleaseAndGetAddressOf())))
            {
                MINT_LOG_ERROR("김장원", "SwapChain 생성에 실패했습니다!");
                return false;
            }
            return true;
        }

        const bool GraphicDevice::initializeBackBuffer()
        {
            ComPtr<ID3D11Texture2D> backBuffer;
            _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.ReleaseAndGetAddressOf()));
            
            if (FAILED(_device->CreateRenderTargetView(backBuffer.Get(), nullptr, _backBufferRtv.ReleaseAndGetAddressOf())))
            {
                MINT_LOG_ERROR("김장원", "BackBuffer 초기화에 실패했습니다!");
                return false;
            }
            return true;
        }

        const bool GraphicDevice::initializeDepthStencilBufferAndView(const mint::Int2& windowSize)
        {
            D3D11_TEXTURE2D_DESC depthStencilBufferDescriptor;
            depthStencilBufferDescriptor.Width = static_cast<UINT>(windowSize._x);
            depthStencilBufferDescriptor.Height = static_cast<UINT>(windowSize._y);
            depthStencilBufferDescriptor.MipLevels = 1;
            depthStencilBufferDescriptor.ArraySize = 1;
            depthStencilBufferDescriptor.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depthStencilBufferDescriptor.SampleDesc.Count = 1;
            depthStencilBufferDescriptor.SampleDesc.Quality = 0;
            depthStencilBufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
            depthStencilBufferDescriptor.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            depthStencilBufferDescriptor.CPUAccessFlags = 0;
            depthStencilBufferDescriptor.MiscFlags = 0;

            if (FAILED(_device->CreateTexture2D(&depthStencilBufferDescriptor, nullptr, _depthStencilBuffer.ReleaseAndGetAddressOf())))
            {
                MINT_LOG_ERROR("김장원", "DepthStencil 텍스쳐 생성에 실패했습니다.");
                return false;
            }
            
            if (FAILED(_device->CreateDepthStencilView(_depthStencilBuffer.Get(), nullptr, _depthStencilView.ReleaseAndGetAddressOf())))
            {
                MINT_LOG_ERROR("김장원", "DepthStencil 뷰 생성에 실패했습니다.");
                return false;
            }

            return true;
        }

        const bool GraphicDevice::initializeDepthStencilStates()
        {
            D3D11_DEPTH_STENCIL_DESC depthStencilDescriptor;
            depthStencilDescriptor.DepthEnable = TRUE;
            depthStencilDescriptor.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
            depthStencilDescriptor.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilDescriptor.StencilEnable = FALSE;
            if (FAILED(_device->CreateDepthStencilState(&depthStencilDescriptor, _depthStencilStateLessEqual.ReleaseAndGetAddressOf())))
            {
                MINT_LOG_ERROR("김장원", "DepthStencil State 생성에 실패했습니다.");
                return false;
            }

            return true;
        }

        void GraphicDevice::initializeShaderHeaderMemory()
        {
            const Int2 windowSize = _window->getSize();

            // Stream data
            {
                _cppHlslStreamData.parseCppHlslFile("Assets/CppHlsl/CppHlslStreamData.h");
                _cppHlslStreamData.generateHlslString(mint::CppHlsl::CppHlslFileType::StreamData);
                _shaderHeaderMemory.pushHeader("ShaderStructDefinitions", _cppHlslStreamData.getHlslString());
            }

            // Constant buffers
            {
                _cppHlslConstantBuffers.parseCppHlslFile("Assets/CppHlsl/CppHlslConstantBuffers.h");
                _cppHlslConstantBuffers.generateHlslString(mint::CppHlsl::CppHlslFileType::ConstantBuffers);
                _shaderHeaderMemory.pushHeader("ShaderConstantBuffers", _cppHlslConstantBuffers.getHlslString());

                {
                    const mint::CppHlsl::TypeMetaData& typeMetaData = _cppHlslConstantBuffers.getTypeMetaData(typeid(_cbViewData));
                    _cbViewId = _resourcePool.pushConstantBuffer(&_cbViewData, sizeof(_cbViewData), typeMetaData.getRegisterIndex());
                    
                    mint::RenderingBase::DxResource& cbView = _resourcePool.getResource(_cbViewId);
                    cbView.bindToShader(DxShaderType::VertexShader, cbView.getRegisterIndex());
                    cbView.bindToShader(DxShaderType::GeometryShader, cbView.getRegisterIndex());
                    cbView.bindToShader(DxShaderType::PixelShader, cbView.getRegisterIndex());
                }

                {
                    mint::RenderingBase::CB_Transform cbTransformData;
                    const mint::CppHlsl::TypeMetaData& typeMetaData = _cppHlslConstantBuffers.getTypeMetaData(typeid(cbTransformData));
                    _cbTransformId = _resourcePool.pushConstantBuffer(&cbTransformData, sizeof(cbTransformData), typeMetaData.getRegisterIndex());
                }

                initialize2DProjectionMatrix(mint::Float2(windowSize));
            }

            // Structured buffers
            {
                _cppHlslStructuredBuffers.parseCppHlslFile("Assets/CppHlsl/CppHlslStructuredBuffers.h");
                _cppHlslStructuredBuffers.generateHlslString(mint::CppHlsl::CppHlslFileType::StructuredBuffers);
                _shaderHeaderMemory.pushHeader("ShaderStructuredBufferDefinitions", _cppHlslStructuredBuffers.getHlslString());

                {
                    mint::RenderingBase::SB_Transform sbTransformData;
                    const mint::CppHlsl::TypeMetaData& typeMetaData = _cppHlslStructuredBuffers.getTypeMetaData(typeid(sbTransformData));
                    _sbTransformId = _resourcePool.pushStructuredBuffer(&sbTransformData, sizeof(sbTransformData), 1, typeMetaData.getRegisterIndex());
                }

                {
                    mint::RenderingBase::SB_Material sbMaterialData;
                    const mint::CppHlsl::TypeMetaData& typeMetaData = _cppHlslStructuredBuffers.getTypeMetaData(typeid(sbMaterialData));
                    _sbMaterialId = _resourcePool.pushStructuredBuffer(&sbMaterialData, sizeof(sbMaterialData), 1, typeMetaData.getRegisterIndex());
                }
            }
        }

        void GraphicDevice::initializeShaders()
        {
            _rectangleRendererContext.initializeShaders();
            _shapeRendererContext.initializeShaders();
            _fontRendererContext.initializeShaders();
            _shapeFontRendererContext.initializeShaders();
        }

        void GraphicDevice::initializeSamplerStates()
        {
            {
                D3D11_SAMPLER_DESC samplerDescriptor{};
                samplerDescriptor.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                samplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
                samplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
                samplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
                samplerDescriptor.MipLODBias = 0.0f;
                samplerDescriptor.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
                samplerDescriptor.MinLOD = 0.0f;
                samplerDescriptor.MaxLOD = 0.0f;
                _device->CreateSamplerState(&samplerDescriptor, _samplerState.ReleaseAndGetAddressOf());
                _deviceContext->PSSetSamplers(0, 1, _samplerState.GetAddressOf());
            }
        }

        void GraphicDevice::initializeBlendStates()
        {
            {
                D3D11_BLEND_DESC blendDescriptor{};
                blendDescriptor.AlphaToCoverageEnable = false;
                blendDescriptor.RenderTarget[0].BlendEnable = true;
                blendDescriptor.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
                blendDescriptor.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
                blendDescriptor.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
                blendDescriptor.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
                blendDescriptor.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
                blendDescriptor.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
                blendDescriptor.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
                _device->CreateBlendState(&blendDescriptor, _blendState.ReleaseAndGetAddressOf());

                const float kBlendFactor[4]{ 0, 0, 0, 0 };
                _deviceContext->OMSetBlendState(_blendState.Get(), kBlendFactor, 0xFFFFFFFF);
            }
        }

        void GraphicDevice::initializeFullScreenData(const mint::Int2& windowSize)
        {
            _fullScreenViewport.Width = static_cast<FLOAT>(windowSize._x);
            _fullScreenViewport.Height = static_cast<FLOAT>(windowSize._y);
            _fullScreenViewport.MinDepth = 0.0f;
            _fullScreenViewport.MaxDepth = 1.0f;
            _fullScreenViewport.TopLeftX = 0.0f;
            _fullScreenViewport.TopLeftY = 0.0f;

            _fullScreenScissorRectangle.left = 0;
            _fullScreenScissorRectangle.right = 0;
            _fullScreenScissorRectangle.right = windowSize._x;
            _fullScreenScissorRectangle.bottom = windowSize._y;
        }

        void GraphicDevice::setDefaultRenderTargetsAndDepthStencil()
        {
            _deviceContext->OMSetRenderTargets(1, _backBufferRtv.GetAddressOf(), _depthStencilView.Get());
            _deviceContext->OMSetDepthStencilState(_depthStencilStateLessEqual.Get(), 0);
        }

        void GraphicDevice::beginRendering()
        {
            if (true == _needEndRenderingCall)
            {
                MINT_LOG_ERROR("김장원", "beginRendering() 을 두 번 연달아 호출할 수 없습니다. 먼저 endRendering() 을 호출해 주세요!");
                return;
            }

            _needEndRenderingCall = true;

            _deviceContext->ClearRenderTargetView(_backBufferRtv.Get(), _clearColor);
            _deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

            _rectangleRendererContext.flush();
            _shapeRendererContext.flush();
            _fontRendererContext.flush();
            _shapeFontRendererContext.flush();
        }

        void GraphicDevice::endRendering()
        {
            if (false == _needEndRenderingCall)
            {
                MINT_LOG_ERROR("김장원", "endRendering() 을 두 번 연달아 호출할 수 없습니다. 먼저 beginRendering() 을 호출해 주세요!");
                return;
            }

#pragma region Renderer Contexts
            useFullScreenViewport();

            _rectangleRendererContext.renderAndFlush();
            _shapeRendererContext.renderAndFlush();
            _fontRendererContext.renderAndFlush();
            _shapeFontRendererContext.renderAndFlush();
            _guiContext.render();
#pragma endregion

            _swapChain->Present(0, 0);

            _needEndRenderingCall = false;
        }

        void GraphicDevice::useScissorRectanglesWithMultipleViewports() noexcept
        {
            _deviceContext->RSSetState(_rasterizerStateScissorRectangles.Get());
        }

        void GraphicDevice::useFullScreenViewport() noexcept
        {
            _deviceContext->RSSetState(_currentRasterizerFor3D);
            _deviceContext->RSSetViewports(1, &_fullScreenViewport);
        }

        void GraphicDevice::useWireFrameNoCullingRasterizer() noexcept
        {
            _currentRasterizerFor3D = _rasterizerStateWireFrameNoCulling.Get();
        }

        void GraphicDevice::useWireFrameCullBackRasterizer() noexcept
        {
            _currentRasterizerFor3D = _rasterizerStateWireFrameCullBack.Get();
        }

        void GraphicDevice::useSolidCullBackRasterizer() noexcept
        {
            _currentRasterizerFor3D = _rasterizerStateSolidCullBack.Get();
        }

        const D3D11_VIEWPORT& GraphicDevice::getFullScreenViewport() const noexcept
        {
            return _fullScreenViewport;
        }

        const D3D11_RECT& GraphicDevice::getFullScreenScissorRectangle() const noexcept
        {
            return _fullScreenScissorRectangle;
        }
        
        void GraphicDevice::initialize2DProjectionMatrix(const mint::Float2& windowSize) noexcept
        {
            _cbViewData._cb2DProjectionMatrix = mint::Float4x4::projectionMatrix2DFromTopLeft(windowSize._x, windowSize._y);

            DxResource& cbView = _resourcePool.getResource(_cbViewId);
            cbView.updateBuffer(&_cbViewData, 1);
        }

        void GraphicDevice::setViewMatrix(const mint::Float4x4& viewMatrix) noexcept
        {
            _cbViewData._cbViewMatrix = viewMatrix;
            
            DxResource& cbView = _resourcePool.getResource(_cbViewId);
            cbView.updateBuffer(&_cbViewData, 1);
        }

        void GraphicDevice::setProjectionMatrix(const mint::Float4x4& projectionMatrix) noexcept
        {
            _cbViewData._cb3DProjectionMatrix = projectionMatrix;

            DxResource& cbView = _resourcePool.getResource(_cbViewId);
            cbView.updateBuffer(&_cbViewData, 1);
        }

        void GraphicDevice::updateViewProjectionMatrix() noexcept
        {
            _cbViewData._cbViewProjectionMatrix = _cbViewData._cb3DProjectionMatrix * _cbViewData._cbViewMatrix;

            DxResource& cbView = _resourcePool.getResource(_cbViewId);
            cbView.updateBuffer(&_cbViewData, 1);
        }

        const mint::Int2& GraphicDevice::getWindowSize() const noexcept
        {
            return (_window != nullptr) ? _window->getSize() : mint::Int2::kZero;
        }

        mint::Float2 GraphicDevice::getWindowSizeFloat2() const noexcept
        {
            return mint::Float2(getWindowSize());
        }
        
        mint::Window::IWindow* GraphicDevice::getWindow() noexcept
        {
            return _window;
        }
    }
}
