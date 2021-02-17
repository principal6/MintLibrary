﻿#include <stdafx.h>
#include <FsRenderingBase/Include/GraphicDevice.h>

#include <d3dcompiler.h>
#include <functional>
#include <typeinfo>

#include <FsLibrary/Algorithm.hpp>

#include <FsPlatform/Include/IWindow.h>
#include <FsPlatform/Include/WindowsWindow.h>

#include <FsContainer/Include/ScopeString.hpp>

#include <FsRenderingBase/Include/TriangleRenderer.hpp>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")


namespace fs
{
	namespace SimpleRendering
	{
		GraphicDevice::GraphicDevice()
			: _window{ nullptr }
			, _clearColor{ 0.875f, 0.875f, 0.875f, 1.0f }
			, _fullScreenViewport{}
			, _shaderPool{ this, &_shaderHeaderMemory, fs::SimpleRendering::DxShaderVersion::v_5_0 }
			, _resourcePool{ this }
			, _rectangleRendererContext{ this }
			, _shapeRendererContext{ this }
			, _fontRendererContext{ this }
			, _guiContext{ this }
		{
			__noop;
		}

		void GraphicDevice::initialize(fs::Window::IWindow* const window)
		{
			FS_ASSERT("김장원", window != nullptr, "window 에 대한 포인터가 nullptr 이면 안 됩니다!");
		
			_window = window;

			const fs::Float3& backgroundColor = _window->getBackgroundColor();
			_clearColor[0] = backgroundColor._x;
			_clearColor[1] = backgroundColor._y;
			_clearColor[2] = backgroundColor._z;
			
			createDxDevice();

#if defined FS_TEST_MEMORY_FONT_TEXTURE
			createFontTextureFromMemory();
#endif

			if (_fontRendererContext.loadFontData(kDefaultFont) == false)
			{
				_fontRendererContext.pushGlyphRange(fs::SimpleRendering::GlyphRange(0, 0x33DD));
				_fontRendererContext.pushGlyphRange(fs::SimpleRendering::GlyphRange(L'가', L'힣'));
				_fontRendererContext.bakeFontData(kDefaultFont, kDefaultFontSize, kDefaultFont, 2048, 1, 1);
				_fontRendererContext.loadFontData(kDefaultFont);
			}

			_guiContext.initialize(kDefaultFont);
		}

		void GraphicDevice::createDxDevice()
		{
			const fs::Window::WindowsWindow* const windowsWindow = static_cast<const fs::Window::WindowsWindow*>(_window);
			const Int2 windowSize = _window->getSize();

			// Create SwapChain
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
				swapChainDescriptor.OutputWindow = windowsWindow->getHandle();
				swapChainDescriptor.SampleDesc.Count = 1;
				swapChainDescriptor.SampleDesc.Quality = 0;
				swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
				swapChainDescriptor.Windowed = TRUE;

				D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
					&swapChainDescriptor, _swapChain.ReleaseAndGetAddressOf(), _device.ReleaseAndGetAddressOf(), nullptr, _deviceContext.ReleaseAndGetAddressOf());
			}

			// Create back-buffer RTV
			{
				_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(_backBuffer.ReleaseAndGetAddressOf()));
				_device->CreateRenderTargetView(_backBuffer.Get(), nullptr, _backBufferRtv.ReleaseAndGetAddressOf());
			}

			// Create depth-stencil view
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

				_device->CreateTexture2D(&depthStencilBufferDescriptor, nullptr, _depthStencilBuffer.ReleaseAndGetAddressOf());
				_device->CreateDepthStencilView(_depthStencilBuffer.Get(), nullptr, _depthStencilView.ReleaseAndGetAddressOf());

				D3D11_DEPTH_STENCIL_DESC depthStencilDescriptor;
				depthStencilDescriptor.DepthEnable = TRUE;
				depthStencilDescriptor.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
				depthStencilDescriptor.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
				depthStencilDescriptor.StencilEnable = FALSE;
				_device->CreateDepthStencilState(&depthStencilDescriptor, _depthStencilStateLessEqual.ReleaseAndGetAddressOf());
			}

			// Set render targets
			_deviceContext->OMSetRenderTargets(1, _backBufferRtv.GetAddressOf(), _depthStencilView.Get());
			_deviceContext->OMSetDepthStencilState(_depthStencilStateLessEqual.Get(), 0);

			initializeShaderHeaderMemory();
			initializeShaders();
			initializeSamplerStates();
			initializeBlendStates();

			// Rasterizer states and viewport
			{
				D3D11_RASTERIZER_DESC rasterizerDescriptor;
				rasterizerDescriptor.AntialiasedLineEnable = TRUE;
				rasterizerDescriptor.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
				rasterizerDescriptor.DepthBias = 0;
				rasterizerDescriptor.DepthBiasClamp = 0.0f;
				rasterizerDescriptor.DepthClipEnable = TRUE;
				rasterizerDescriptor.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
				rasterizerDescriptor.FrontCounterClockwise = FALSE;
				rasterizerDescriptor.MultisampleEnable = TRUE;
				rasterizerDescriptor.ScissorEnable = FALSE;
				rasterizerDescriptor.SlopeScaledDepthBias = 0.0f;
				_device->CreateRasterizerState(&rasterizerDescriptor, _rasterizerStateDefault.ReleaseAndGetAddressOf());

				rasterizerDescriptor.ScissorEnable = TRUE;
				_device->CreateRasterizerState(&rasterizerDescriptor, _rasterizerStateScissorRectangles.ReleaseAndGetAddressOf());

				// Full-screen viewport
				{
					_fullScreenViewport.Width = static_cast<FLOAT>(windowSize._x);
					_fullScreenViewport.Height = static_cast<FLOAT>(windowSize._y);
					_fullScreenViewport.MinDepth = 0.0f;
					_fullScreenViewport.MaxDepth = 1.0f;
					_fullScreenViewport.TopLeftX = 0.0f;
					_fullScreenViewport.TopLeftY = 0.0f;
				}
			}
		}

		void GraphicDevice::initializeShaderHeaderMemory()
		{
			const Int2 windowSize = _window->getSize();

			// Stream data
			{
				_cppHlslStreamData.parseCppHlslFile("Assets/CppHlsl/CppHlslStreamData.h");
				_cppHlslStreamData.generateHlslString(fs::Language::CppHlslFileType::StreamData);
				_shaderHeaderMemory.pushHeader("ShaderStructDefinitions", _cppHlslStreamData.getHlslString());
			}

			// Constant buffers
			{
				fs::CppHlsl::CB_Transforms cbTransforms;
				cbTransforms._cbProjectionMatrix = fs::Float4x4::projectionMatrix2DFromTopLeft(static_cast<float>(windowSize._x), static_cast<float>(windowSize._y));
				{
					_cppHlslConstantBuffers.parseCppHlslFile("Assets/CppHlsl/CppHlslConstantBuffers.h");
					_cppHlslConstantBuffers.generateHlslString(fs::Language::CppHlslFileType::ConstantBuffers);
					_shaderHeaderMemory.pushHeader("ShaderConstantBuffers", _cppHlslConstantBuffers.getHlslString());
					
					const fs::Language::CppHlslTypeInfo& cppHlslTypeInfo = _cppHlslConstantBuffers.getTypeInfo(typeid(cbTransforms));
					DxObjectId id = _resourcePool.pushConstantBuffer(reinterpret_cast<const byte*>(&cbTransforms._cbProjectionMatrix), sizeof(cbTransforms));
					_resourcePool.getResource(id).bindToShader(DxShaderType::VertexShader, cppHlslTypeInfo.getRegisterIndex());
					_resourcePool.getResource(id).bindToShader(DxShaderType::PixelShader, cppHlslTypeInfo.getRegisterIndex());
				}
			}

			// Structured buffers
			{
				_cppHlslStructuredBuffers.parseCppHlslFile("Assets/CppHlsl/CppHlslStructuredBuffers.h");
				_cppHlslStructuredBuffers.generateHlslString(fs::Language::CppHlslFileType::StructuredBuffers);
				_shaderHeaderMemory.pushHeader("ShaderStructuredBufferDefinitions", _cppHlslStructuredBuffers.getHlslString());
			}
		}

		void GraphicDevice::initializeShaders()
		{
			_rectangleRendererContext.initializeShaders();
			_shapeRendererContext.initializeShaders();
			_fontRendererContext.initializeShaders();
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

#if defined FS_TEST_MEMORY_FONT_TEXTURE
		void GraphicDevice::createFontTextureFromMemory()
		{
			// ### Copy and paste to see this better ###
			// Replace ', ' with '_'
			// Replace '0b' with '.'
			// Replace  '0' with ' '
			static constexpr const byte kFontTextureRawBitData[]
			{
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b00000000, 0b00010000, 0b00101000, 0b00010010, 0b00010000, 0b00000000, 0b00000000, 0b00010000, 0b00001000, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010,
				0b00000000, 0b00010000, 0b00101000, 0b00100100, 0b00111100, 0b01100010, 0b00110000, 0b00010000, 0b00010000, 0b00010000, 0b01000100, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000100,
				0b00000000, 0b00010000, 0b00000000, 0b01111110, 0b01010000, 0b10100100, 0b01001000, 0b00000000, 0b00100000, 0b00001000, 0b00101000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00001000,
				0b00000000, 0b00010000, 0b00000000, 0b00100100, 0b00111100, 0b01101000, 0b00110010, 0b00000000, 0b00100000, 0b00001000, 0b11111110, 0b11111110, 0b00000000, 0b11111110, 0b00000000, 0b00010000,
				0b00000000, 0b00010000, 0b00000000, 0b01111110, 0b00010100, 0b00010110, 0b01001100, 0b00000000, 0b00100000, 0b00001000, 0b00101000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00100000,
				0b00000000, 0b00000000, 0b00000000, 0b00100100, 0b01111000, 0b00101010, 0b10001100, 0b00000000, 0b00010000, 0b00010000, 0b01000100, 0b00010000, 0b00001000, 0b00000000, 0b00011000, 0b01000000,
				0b00000000, 0b00010000, 0b00000000, 0b01001000, 0b00010000, 0b01001100, 0b01110010, 0b00000000, 0b00001000, 0b00100000, 0b00000000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b10000000,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b01111100, 0b00010000, 0b01111100, 0b01111100, 0b00001000, 0b11111110, 0b01111100, 0b11111110, 0b01111100, 0b01111100, 0b00000000, 0b00000000, 0b00001000, 0b00000000, 0b01000000, 0b01111100,
				0b10000110, 0b00110000, 0b10000010, 0b10000010, 0b00011000, 0b10000000, 0b10000010, 0b00000100, 0b10000010, 0b10000010, 0b00010000, 0b00010000, 0b00010000, 0b11111110, 0b00100000, 0b10000010,
				0b10001010, 0b00010000, 0b00000010, 0b00000010, 0b00101000, 0b10111100, 0b10000000, 0b00001000, 0b10000010, 0b10000010, 0b00000000, 0b00000000, 0b00100000, 0b00000000, 0b00010000, 0b00000010,
				0b10010010, 0b00010000, 0b00000100, 0b01111100, 0b01001000, 0b11000010, 0b10111100, 0b00010000, 0b01111100, 0b01111110, 0b00000000, 0b00000000, 0b01000000, 0b00000000, 0b00001000, 0b00011100,
				0b10100010, 0b00010000, 0b00111000, 0b00000010, 0b11111110, 0b00000010, 0b11000010, 0b00100000, 0b10000010, 0b00000010, 0b00010000, 0b00010000, 0b00100000, 0b11111110, 0b00010000, 0b00010000,
				0b11000010, 0b00010000, 0b01000000, 0b10000010, 0b00001000, 0b10000010, 0b10000010, 0b00100000, 0b10000010, 0b10000010, 0b00000000, 0b00100000, 0b00010000, 0b00000000, 0b00100000, 0b00000000,
				0b01111100, 0b00111000, 0b11111110, 0b01111100, 0b00011100, 0b01111100, 0b01111100, 0b00100000, 0b01111100, 0b01111100, 0b00000000, 0b00000000, 0b00001000, 0b00000000, 0b01000000, 0b00010000,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b00111000, 0b00010000, 0b11111100, 0b01111110, 0b11111100, 0b11111110, 0b11111110, 0b01111100, 0b10000010, 0b00111000, 0b00011100, 0b01000100, 0b10000000, 0b11000110, 0b11000010, 0b01111100,
				0b01000100, 0b00101000, 0b10000010, 0b10000000, 0b10000010, 0b10000000, 0b10000000, 0b10000000, 0b10000010, 0b00010000, 0b00001000, 0b01001000, 0b10000000, 0b10101010, 0b10100010, 0b10000010,
				0b10111010, 0b01000100, 0b10000010, 0b10000000, 0b10000010, 0b10000000, 0b10000000, 0b10000000, 0b10000010, 0b00010000, 0b00001000, 0b01010000, 0b10000000, 0b10010010, 0b10100010, 0b10000010,
				0b10101010, 0b01111100, 0b11111100, 0b10000000, 0b10000010, 0b11111110, 0b11111110, 0b10000110, 0b11111110, 0b00010000, 0b00001000, 0b01110000, 0b10000000, 0b10010010, 0b10010010, 0b10000010,
				0b10101110, 0b10000010, 0b10000010, 0b10000000, 0b10000010, 0b10000000, 0b10000000, 0b10000010, 0b10000010, 0b00010000, 0b01001000, 0b01010000, 0b10000000, 0b10000010, 0b10010010, 0b10000010,
				0b01010100, 0b10000010, 0b10000010, 0b10000000, 0b10000010, 0b10000000, 0b10000000, 0b10000010, 0b10000010, 0b00010000, 0b01001000, 0b01001000, 0b10000000, 0b10000010, 0b10001010, 0b10000010,
				0b00111010, 0b10000010, 0b11111100, 0b01111110, 0b11111100, 0b11111110, 0b10000000, 0b01111100, 0b10000010, 0b00111000, 0b00110000, 0b01000100, 0b11111110, 0b10000010, 0b10000110, 0b01111100,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b11111100, 0b01111100, 0b11111100, 0b01111100, 0b11111110, 0b10000010, 0b10000010, 0b10010010, 0b10000010, 0b10000010, 0b11111110, 0b00111000, 0b10000000, 0b00111000, 0b00010000, 0b00000000,
				0b10000010, 0b10000010, 0b10000010, 0b10000010, 0b00010000, 0b10000010, 0b10000010, 0b10010010, 0b01000100, 0b01000100, 0b00000100, 0b00100000, 0b01000000, 0b00001000, 0b00101000, 0b00000000,
				0b10000010, 0b10000010, 0b10000010, 0b10000000, 0b00010000, 0b10000010, 0b10000010, 0b10010010, 0b00101000, 0b00101000, 0b00001000, 0b00100000, 0b00100000, 0b00001000, 0b01000100, 0b00000000,
				0b11111100, 0b10000010, 0b11111100, 0b11111100, 0b00010000, 0b10000010, 0b10000010, 0b10010010, 0b00010000, 0b00010000, 0b00010000, 0b00100000, 0b00010000, 0b00001000, 0b00000000, 0b00000000,
				0b10000000, 0b10001010, 0b10000100, 0b00000010, 0b00010000, 0b10000010, 0b01000100, 0b10010010, 0b00101000, 0b00010000, 0b00100000, 0b00100000, 0b00001000, 0b00001000, 0b00000000, 0b00000000,
				0b10000000, 0b10000100, 0b10000010, 0b10000010, 0b00010000, 0b10000010, 0b00101000, 0b10010010, 0b01000100, 0b00010000, 0b01000000, 0b00100000, 0b00000100, 0b00001000, 0b00000000, 0b00000000,
				0b10000000, 0b01111010, 0b10000010, 0b01111100, 0b00010000, 0b01111100, 0b00010000, 0b01101100, 0b10000010, 0b00010000, 0b11111110, 0b00111000, 0b00000010, 0b00111000, 0b00000000, 0b01111100,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b00100000, 0b00000000, 0b01000000, 0b00000000, 0b00000010, 0b00000000, 0b00001100, 0b00000000, 0b01000000, 0b00010000, 0b00001000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b00010000, 0b00000000, 0b01000000, 0b00000000, 0b00000010, 0b00000000, 0b00010000, 0b00110100, 0b01000000, 0b00000000, 0b00000000, 0b01000100, 0b00010000, 0b00000000, 0b00000000, 0b00000000,
				0b00001000, 0b01110100, 0b01011000, 0b00111000, 0b00111010, 0b00111000, 0b00010000, 0b01001100, 0b01000000, 0b00010000, 0b00001000, 0b01001000, 0b00010000, 0b10101000, 0b10111000, 0b00111000,
				0b00000000, 0b10001100, 0b01100100, 0b01000100, 0b01000110, 0b01000100, 0b01111100, 0b01000100, 0b01010000, 0b00010000, 0b00001000, 0b01010000, 0b00010000, 0b01010100, 0b01000100, 0b01000100,
				0b00000000, 0b10000100, 0b01000100, 0b01000000, 0b01000110, 0b01111100, 0b00010000, 0b00111100, 0b01101000, 0b00010000, 0b00001000, 0b01110000, 0b00010000, 0b01010100, 0b01000100, 0b01000100,
				0b00000000, 0b10001100, 0b01000100, 0b01000100, 0b01000110, 0b01000000, 0b00010000, 0b00000100, 0b01001000, 0b00010000, 0b00001000, 0b01001000, 0b00010000, 0b01010100, 0b01000100, 0b01000100,
				0b00000000, 0b01110010, 0b01111000, 0b00111000, 0b00111010, 0b00111000, 0b00010000, 0b01000100, 0b01001100, 0b00011000, 0b00101000, 0b01000100, 0b00101100, 0b01010100, 0b01000100, 0b00111000,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00110000, 0b00111000, 0b00000000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00011000, 0b00010000, 0b00110000, 0b00000000, 0b00000000,
				0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100000, 0b00010000, 0b00001000, 0b00000000, 0b00000000,
				0b01011000, 0b00110100, 0b01011000, 0b00111100, 0b01111100, 0b01000100, 0b01000100, 0b01000100, 0b01000100, 0b10001000, 0b11111000, 0b00100000, 0b00010000, 0b00001000, 0b01100000, 0b00000000,
				0b01100100, 0b01001100, 0b01100100, 0b01000010, 0b00010000, 0b01000100, 0b01000100, 0b01010100, 0b00101000, 0b10001000, 0b00010000, 0b01100000, 0b00010000, 0b00001100, 0b10010010, 0b00000000,
				0b01100100, 0b01001100, 0b01000000, 0b00111000, 0b00010000, 0b01000100, 0b01000100, 0b01010100, 0b00010000, 0b10001000, 0b00100000, 0b00100000, 0b00010000, 0b00001000, 0b00001100, 0b00000000,
				0b01100100, 0b01001100, 0b01000000, 0b00000110, 0b00010100, 0b01000100, 0b00101000, 0b01010100, 0b00101000, 0b01111000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000000, 0b00000000,
				0b01011000, 0b00110100, 0b01000000, 0b01111100, 0b00001000, 0b00111010, 0b00010000, 0b00101000, 0b01000100, 0b00001000, 0b11111000, 0b00011000, 0b00010000, 0b00110000, 0b00000000, 0b00000000,
				0b01000000, 0b00000100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10001000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
				0b01000000, 0b00000100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
			};

			std::function<fs::Int2(const fs::Int2)> samplePixel = [&](const fs::Int2& pos)
			{
				const uint32 pixelIndex = static_cast<uint32>(static_cast<uint64>(pos._y) * kFontTextureWidth + pos._x);
				if (kFontTexturePixelCount <= pixelIndex)
				{
					return fs::Int2(0, 0);
				}

				const uint32 byteOffset = pixelIndex / kBitsPerByte;
				const uint32 bitShiftToLeft = kBitsPerByte - (pixelIndex % kBitsPerByte + 1);
				const uint8 bitMask = static_cast<uint8>(fs::Math::pow2_ui32(bitShiftToLeft));
				return (kFontTextureRawBitData[byteOffset] & bitMask) ? fs::Int2(1, 1) : fs::Int2(1, 0);
			};
		
			std::function<uint8(const fs::Int2)> samplePixelAlpha = [&](const fs::Int2& pos)
			{
				fs::Int2 sample = samplePixel(pos);
				return (1 == sample._y) ? 255 : 0;
			};

			std::function<uint8(const fs::Int2)> sampleKernelPixelAlpha = [&](const fs::Int2& pos)
			{
				static constexpr uint32 kKernelSize = 9;
				static constexpr float kKernel[9] = // Gaussian approximation
				{
					0.0625f, 0.125f, 0.0625f,
					 0.125f,  0.25f,  0.125f,
					0.0625f, 0.125f, 0.0625f,
				};

				fs::Int2 sampleArray[kKernelSize];
				{
					sampleArray[0] = samplePixel(fs::Int2(pos._x - 1, pos._y - 1));
					sampleArray[1] = samplePixel(fs::Int2(pos._x    , pos._y - 1)); // u
					sampleArray[2] = samplePixel(fs::Int2(pos._x + 1, pos._y - 1));

					sampleArray[3] = samplePixel(fs::Int2(pos._x - 1, pos._y    )); // l
					sampleArray[4] = samplePixel(fs::Int2(pos._x    , pos._y    )); // c
					if (sampleArray[4]._y == 1)
					{
						return static_cast <uint8>(255);
					}
					sampleArray[5] = samplePixel(fs::Int2(pos._x + 1, pos._y    )); // r

					sampleArray[6] = samplePixel(fs::Int2(pos._x - 1, pos._y + 1));
					sampleArray[7] = samplePixel(fs::Int2(pos._x    , pos._y + 1)); // d
					sampleArray[8] = samplePixel(fs::Int2(pos._x + 1, pos._y + 1));
				}

				float normalized = 0.0f;
				{
					float weightSum = 0.0f;
					for (uint32 i = 0; i < kKernelSize; ++i)
					{
						if (sampleArray[i]._x == 1)
						{
							normalized += kKernel[i] * sampleArray[i]._y;
							weightSum += kKernel[i];
						}
					}
					normalized /= weightSum;
				}
			
				// 외곽선 블러 줄이기
				normalized *= normalized;

				return static_cast<uint8>(normalized * 255);
			};

			std::function<fs::Int2(const uint32)> pixelIndexToPos = [&](const uint32 pixelIndex)
			{
				const int32 x = pixelIndex % kFontTextureWidth;
				const int32 y = pixelIndex / kFontTextureWidth;
				return fs::Int2(x, y);
			};

			_fontTextureRaw.resize(kFontTexturePixelCount * 4);
			fs::Int2 pixelPos;
			fs::Int2 sample;
			for (uint32 pixelIndex = 0; pixelIndex < kFontTexturePixelCount; ++pixelIndex)
			{
				pixelPos = pixelIndexToPos(pixelIndex);
				//const uint8 alpha = samplePixelAlpha(pixelPos);
				const uint8 alpha = sampleKernelPixelAlpha(pixelPos);
				const uint8 color = (0 != alpha) ? 255 : 0;

				_fontTextureRaw[static_cast<uint64>(pixelIndex) * 4 + 0] = color;
				_fontTextureRaw[static_cast<uint64>(pixelIndex) * 4 + 1] = color;
				_fontTextureRaw[static_cast<uint64>(pixelIndex) * 4 + 2] = color;
				_fontTextureRaw[static_cast<uint64>(pixelIndex) * 4 + 3] = alpha;
			}

			D3D11_TEXTURE2D_DESC texture2DDescriptor{};
			texture2DDescriptor.Width = kFontTextureWidth;
			texture2DDescriptor.Height = kFontTextureHeight;
			texture2DDescriptor.MipLevels = 1;
			texture2DDescriptor.ArraySize = 1;
			texture2DDescriptor.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			texture2DDescriptor.SampleDesc.Count = 1;
			texture2DDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
			texture2DDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
			texture2DDescriptor.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA subResource{};
			subResource.pSysMem = &_fontTextureRaw[0];
			subResource.SysMemPitch = texture2DDescriptor.Width * 4;
			subResource.SysMemSlicePitch = 0;

			ComPtr<ID3D11Texture2D> fontTexture;
			_device->CreateTexture2D(&texture2DDescriptor, &subResource, fontTexture.ReleaseAndGetAddressOf());

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = texture2DDescriptor.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = texture2DDescriptor.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;
			_device->CreateShaderResourceView(fontTexture.Get(), &srvDesc, _fontTextureSrv.ReleaseAndGetAddressOf());
		}
#endif

		void GraphicDevice::beginRendering()
		{
			_deviceContext->ClearRenderTargetView(_backBufferRtv.Get(), _clearColor);
			_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		
			_rectangleRendererContext.flushData();
			_shapeRendererContext.flushData();
			_fontRendererContext.flushData();
		}

		void GraphicDevice::endRendering()
		{
#if defined FS_TEST_MEMORY_FONT_TEXTURE
			_deviceContext->PSSetShaderResources(0, 1, _fontTextureSrv.GetAddressOf());
#endif

#pragma region Renderer Contexts
			useFullScreenViewport();

			_rectangleRendererContext.render();
			_shapeRendererContext.render();
			_guiContext.render();
			_fontRendererContext.render();
#pragma endregion

			_swapChain->Present(0, 0);
		}

		void GraphicDevice::useScissorRectanglesWithMultipleViewports() noexcept
		{
			_deviceContext->RSSetState(_rasterizerStateScissorRectangles.Get());
		}

		void GraphicDevice::useFullScreenViewport() noexcept
		{
			_deviceContext->RSSetState(_rasterizerStateDefault.Get());
			_deviceContext->RSSetViewports(1, &_fullScreenViewport);
		}

		const D3D11_VIEWPORT& GraphicDevice::getFullScreenViewport() const noexcept
		{
			return _fullScreenViewport;
		}
		
		const fs::Int2& GraphicDevice::getWindowSize() const noexcept
		{
			return (_window != nullptr) ? _window->getSize() : fs::Int2::kZero;
		}
		
		fs::Window::IWindow* GraphicDevice::getWindow() noexcept
		{
			return _window;
		}
	}
}