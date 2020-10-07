#include <stdafx.h>
#include <SimpleRendering/GraphicDevice.h>

#include <Algorithm.hpp>
#include <d3dcompiler.h>
#include <Platform/IWindow.h>
#include <Platform/WindowsWindow.h>
#include <Math/Float2.hpp>
#include <Math/Float3.hpp>
#include <Math/Float4.hpp>
#include <Math/Float4x4.hpp>
#include <Container/ScopeString.hpp>
#include <typeinfo>
#include <SimpleRendering\DxHelper.h>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")


namespace fs
{
	GraphicDevice::GraphicDevice()
		: _window{ nullptr }
		, _clearColor{ 0.0f, 0.75f, 1.0f, 1.0f }
		, _shaderPool{ this, &_shaderHeaderMemory }
		, _bufferPool{ this }
		, _cachedTriangleVertexCount{ 0 }
		, _triangleVertexStride{ sizeof(VS_INPUT) }
		, _triangleVertexOffset{ 0 }
		, _cachedTriangleIndexCount{ 0 }
		, _triangleIndexOffset{ 0 }
		, _rectangleRenderer{ this }
	{
		__noop;
	}

	void GraphicDevice::initialize(const fs::Window::IWindow* const window)
	{
		FS_ASSERT("김장원", window != nullptr, "window 에 대한 포인터가 nullptr 이면 안 됩니다!");
		
		_window = window;

		createDxDevice();
		createFontTextureFromMemory();
	}

	void GraphicDevice::createDxDevice()
	{
		const fs::Window::WindowsWindow* const windowsWindow = static_cast<const fs::Window::WindowsWindow*>(_window);
		const Int2 windowSize = windowsWindow->size();

		// Create SwapChain
		{
			DXGI_SWAP_CHAIN_DESC SwapChainDesc{};
			SwapChainDesc.BufferCount = 1;
			SwapChainDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			SwapChainDesc.BufferDesc.Width = static_cast<UINT>(windowSize.x());
			SwapChainDesc.BufferDesc.Height = static_cast<UINT>(windowSize.y());
			SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
			SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
			SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
			SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			SwapChainDesc.Flags = 0;
			SwapChainDesc.OutputWindow = windowsWindow->getHandle();
			SwapChainDesc.SampleDesc.Count = 1;
			SwapChainDesc.SampleDesc.Quality = 0;
			SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
			SwapChainDesc.Windowed = TRUE;

			D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
				&SwapChainDesc, _swapChain.ReleaseAndGetAddressOf(), _device.ReleaseAndGetAddressOf(), nullptr, _deviceContext.ReleaseAndGetAddressOf());
		}

		// Create back-buffer RTV
		{
			_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(_backBuffer.ReleaseAndGetAddressOf()));
			_device->CreateRenderTargetView(_backBuffer.Get(), nullptr, _backBufferRtv.ReleaseAndGetAddressOf());
			_deviceContext->OMSetRenderTargets(1, _backBufferRtv.GetAddressOf(), nullptr);
		}

		// Shader header
		{
			VS_INPUT vsInput;
			VS_OUTPUT vsOutput;
			{
				fs::DynamicStringA shaderHeaderContent;
				shaderHeaderContent.append(fs::convertReflectiveClassToHlslStruct(&vsInput, false));
				shaderHeaderContent.append(fs::convertReflectiveClassToHlslStruct(&vsOutput, true));
				_shaderHeaderMemory.pushHeader("ShaderStructDefinitions", shaderHeaderContent.c_str());
			}
		}

		// Constant buffers
		{
			CB_Transforms cbTransforms;
			cbTransforms._cbProjectionMatrix = fs::Float4x4::projectionMatrix2DFromTopLeft(static_cast<float>(windowSize.x()), static_cast<float>(windowSize.y()));
			{
				fs::DynamicStringA shaderHeaderContent;
				shaderHeaderContent.append(fs::convertReflectiveClassToHlslConstantBuffer(&cbTransforms, 0));
				_shaderHeaderMemory.pushHeader("VsConstantBuffers", shaderHeaderContent.c_str());

				DxObjectId id = _bufferPool.pushBuffer(DxBufferType::ConstantBuffer, cbTransforms.compactOffsetPtr(), cbTransforms.compactSize());
				_bufferPool.getBuffer(id).bindToShader(DxShaderType::VertexShader, 0);
			}
		}

		// Compile vertex shader and create input layer
		{
			static constexpr const char kVertexShaderContent[]
			{
				R"(
				#include <ShaderStructDefinitions>
				#include <VsConstantBuffers>

				VS_OUTPUT main(VS_INPUT input)
				{
					VS_OUTPUT result;
					result._position	= mul(float4(input._position.xyz, 1.0), _cbProjectionMatrix);
					result._color		= input._color;
					result._texCoord	= input._texCoord;
					result._flag		= input._flag;
					return result;
				}
				)"
			};
			DxObjectId id = _shaderPool.pushVertexShader(kVertexShaderContent, "main", DxShaderVersion::v_4_0, &VS_INPUT());
			_shaderPool.getShader(DxShaderType::VertexShader, id).bind();
		}

		// Compile pixel shader
		{
			static constexpr const char kPixelShaderContent[]
			{
				R"(
				#include <ShaderStructDefinitions>

				sampler		sampler0;
				Texture2D	texture0;
				
				float4 main(VS_OUTPUT input) : SV_Target
				{
					float4 result = input._color;
					if (input._flag == 1)
					{
						result = texture0.Sample(sampler0, input._texCoord);
					}
					else if (input._flag == 2)
					{
						result *= texture0.Sample(sampler0, input._texCoord);
					}
					return result;
				}
				)"
			};
			DxObjectId id = _shaderPool.pushNonVertexShader(kPixelShaderContent, "main", DxShaderVersion::v_4_0, DxShaderType::PixelShader);
			_shaderPool.getShader(DxShaderType::PixelShader, id).bind();
		}

		// Create texture sampler state
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

		// Blend state
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

		// Viewport
		{
			D3D11_VIEWPORT viewport{};
			viewport.Width = static_cast<FLOAT>(windowSize.x());
			viewport.Height = static_cast<FLOAT>(windowSize.y());
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = viewport.TopLeftY = 0.0f;
			_deviceContext->RSSetViewports(1, &viewport);
		}
	}

	void GraphicDevice::createFontTextureFromMemory()
	{
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        ,
		//.        _.   1    _.  1 1   _.   1  1 _.   1    _.        _.        _.   1    _.    1   _.  1     _.        _.        _.        _.        _.        _.      1 ,
		//.        _.   1    _.  1 1   _.  1  1  _.  1111  _. 11   1 _.  11    _.   1    _.   1    _.   1    _. 1   1  _.   1    _.        _.        _.        _.     1  ,
		//.        _.   1    _.        _. 111111 _. 1 1    _.1 1  1  _. 1  1   _.        _.  1     _.    1   _.  1 1   _.   1    _.        _.        _.        _.    1   ,
		//.        _.   1    _.        _.  1  1  _.  1111  _. 11 1   _.  11  1 _.        _.  1     _.    1   _.1111111 _.1111111 _.        _.1111111 _.        _.   1    ,
		//.        _.   1    _.        _. 111111 _.   1 1  _.   1 11 _. 1  11  _.        _.  1     _.    1   _.  1 1   _.   1    _.        _.        _.        _.  1     ,
		//.        _.        _.        _.  1  1  _. 1111   _.  1 1 1 _.1   11  _.        _.   1    _.   1    _. 1   1  _.   1    _.    1   _.        _.   11   _. 1      ,
		//.        _.   1    _.        _. 1  1   _.   1    _. 1  11  _. 111  1 _.        _.    1   _.  1     _.        _.        _.   1    _.        _.        _.1       ,
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        ,
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        ,
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        ,
		//. 11111  _.   1    _. 11111  _. 11111  _.    1   _.1111111 _. 11111  _.1111111 _. 11111  _. 11111  _.        _.        _.    1   _.        _. 1      _. 11111  ,
		//.1    11 _.  11    _.1     1 _.1     1 _.   11   _.1       _.1     1 _.     1  _.1     1 _.1     1 _.   1    _.   1    _.   1    _.1111111 _.  1     _.1     1 ,
		//.1   1 1 _.   1    _.      1 _.      1 _.  1 1   _.1 1111  _.1       _.    1   _.1     1 _.1     1 _.        _.        _.  1     _.        _.   1    _.      1 ,
		//.1  1  1 _.   1    _.     1  _. 11111  _. 1  1   _.11    1 _.1 1111  _.   1    _. 11111  _. 111111 _.        _.        _. 1      _.        _.    1   _.   111  ,
		//.1 1   1 _.   1    _.   11   _.      1 _.1111111 _.      1 _.11    1 _.  1     _.1     1 _.      1 _.   1    _.   1    _.  1     _.1111111 _.   1    _.   1    ,
		//.11    1 _.   1    _. 1      _.1     1 _.    1   _.1     1 _.1     1 _.  1     _.1     1 _.1     1 _.        _.  1     _.   1    _.        _.  1     _.        ,
		//. 11111  _.  111   _.1111111 _. 11111  _.   111  _. 11111  _. 11111  _.  1     _. 11111  _. 11111  _.        _.        _.    1   _.        _. 1      _.   1    ,
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        ,
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        ,
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        ,
		//.  111   _.   1    _.111111  _. 111111 _.111111  _.1111111 _.1111111 _. 11111  _.1     1 _.  111   _.   111  _. 1   1  _.1       _.11   11 _.11    1 _. 11111  ,
		//. 1   1  _.  1 1   _.1     1 _.1       _.1     1 _.1       _.1       _.1       _.1     1 _.   1    _.    1   _. 1  1   _.1       _.1 1 1 1 _.1 1   1 _.1     1 ,
		//.1 111 1 _. 1   1  _.1     1 _.1       _.1     1 _.1       _.1       _.1       _.1     1 _.   1    _.    1   _. 1 1    _.1       _.1  1  1 _.1 1   1 _.1     1 ,
		//.1 1 1 1 _. 11111  _.111111  _.1       _.1     1 _.1111111 _.1111111 _.1    11 _.1111111 _.   1    _.    1   _. 111    _.1       _.1  1  1 _.1  1  1 _.1     1 ,
		//.1 1 111 _.1     1 _.1     1 _.1       _.1     1 _.1       _.1       _.1     1 _.1     1 _.   1    _. 1  1   _. 1 1    _.1       _.1     1 _.1  1  1 _.1     1 ,
		//. 1 1 1  _.1     1 _.1     1 _.1       _.1     1 _.1       _.1       _.1     1 _.1     1 _.   1    _. 1  1   _. 1  1   _.1       _.1     1 _.1   1 1 _.1     1 ,
		//.  111 1 _.1     1 _.111111  _. 111111 _.111111  _.1111111 _.1       _. 11111  _.1     1 _.  111   _.  11    _. 1   1  _.1111111 _.1     1 _.1    11 _. 11111  ,
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        ,
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        ,
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        ,
		//.111111  _. 11111  _.111111  _. 11111  _.1111111 _.1     1 _.1     1 _.1  1  1 _.1     1 _.1     1 _.1111111 _.  111   _.1       _.  111   _.   1    _.        ,
		//.1     1 _.1     1 _.1     1 _.1     1 _.   1    _.1     1 _.1     1 _.1  1  1 _. 1   1  _. 1   1  _.     1  _.  1     _. 1      _.    1   _.  1 1   _.        ,
		//.1     1 _.1     1 _.1     1 _.1       _.   1    _.1     1 _.1     1 _.1  1  1 _.  1 1   _.  1 1   _.    1   _.  1     _.  1     _.    1   _. 1   1  _.        ,
		//.111111  _.1     1 _.111111  _.111111  _.   1    _.1     1 _.1     1 _.1  1  1 _.   1    _.   1    _.   1    _.  1     _.   1    _.    1   _.        _.        ,
		//.1       _.1   1 1 _.1    1  _.      1 _.   1    _.1     1 _. 1   1  _.1  1  1 _.  1 1   _.   1    _.  1     _.  1     _.    1   _.    1   _.        _.        ,
		//.1       _.1    1  _.1     1 _.1     1 _.   1    _.1     1 _.  1 1   _.1  1  1 _. 1   1  _.   1    _. 1      _.  1     _.     1  _.    1   _.        _.        ,
		//.1       _. 1111 1 _.1     1 _. 11111  _.   1    _. 11111  _.   1    _. 11 11  _.1     1 _.   1    _.1111111 _.  111   _.      1 _.  111   _.        _. 11111  ,
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        ,
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        ,
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        ,
		//.  1     _.        _. 1      _.        _.      1 _.        _.    11  _.        _. 1      _.   1    _.    1   _.        _.        _.        _.        _.        ,
		//.   1    _.        _. 1      _.        _.      1 _.        _.   1    _.  11 1  _. 1      _.        _.        _. 1   1  _.   1    _.        _.        _.        ,
		//.    1   _. 111 1  _. 1 11   _.  111   _.  111 1 _.  111   _.   1    _. 1  11  _. 1      _.   1    _.    1   _. 1  1   _.   1    _.1 1 1   _.1 111   _.  111   ,
		//.        _.1   11  _. 11  1  _. 1   1  _. 1   11 _. 1   1  _. 11111  _. 1   1  _. 1 1    _.   1    _.    1   _. 1 1    _.   1    _. 1 1 1  _. 1   1  _. 1   1  ,
		//.        _.1    1  _. 1   1  _. 1      _. 1   11 _. 11111  _.   1    _.  1111  _. 11 1   _.   1    _.    1   _. 111    _.   1    _. 1 1 1  _. 1   1  _. 1   1  ,
		//.        _.1   11  _. 1   1  _. 1   1  _. 1   11 _. 1      _.   1    _.     1  _. 1  1   _.   1    _.    1   _. 1  1   _.   1    _. 1 1 1  _. 1   1  _. 1   1  ,
		//.        _. 111  1 _. 1111   _.  111   _.  111 1 _.  111   _.   1    _. 1   1  _. 1  11  _.   11   _.  1 1   _. 1   1  _.  1 11  _. 1 1 1  _. 1   1  _.  111   ,
		//.        _.        _.        _.        _.        _.        _.  11    _.  111   _.        _.        _.   1    _.        _.        _.        _.        _.        ,
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        ,
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        ,
		//.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.        _.   11   _.   1    _.  11    _.        _.        ,
		//.        _.        _.        _.        _.   1    _.        _.        _.        _.        _.        _.        _.  1     _.   1    _.    1   _.        _.        ,
		//. 1 11   _.  11 1  _. 1 11   _.  1111  _. 11111  _. 1   1  _. 1   1  _. 1   1  _. 1   1  _.1   1   _.11111   _.  1     _.   1    _.    1   _. 11     _.        ,
		//. 11  1  _. 1  11  _. 11  1  _. 1    1 _.   1    _. 1   1  _. 1   1  _. 1 1 1  _.  1 1   _.1   1   _.   1    _. 11     _.   1    _.    11  _.1  1  1 _.        ,
		//. 11  1  _. 1  11  _. 1      _.  111   _.   1    _. 1   1  _. 1   1  _. 1 1 1  _.   1    _.1   1   _.  1     _.  1     _.   1    _.    1   _.    11  _.        ,
		//. 11  1  _. 1  11  _. 1      _.     11 _.   1 1  _. 1   1  _.  1 1   _. 1 1 1  _.  1 1   _. 1111   _. 1      _.  1     _.   1    _.    1   _.        _.        ,
		//. 1 11   _.  11 1  _. 1      _. 11111  _.    1   _.  111 1 _.   1    _.  1 1   _. 1   1  _.    1   _.11111   _.   11   _.   1    _.  11    _.        _.        ,
		//. 1      _.     1  _.        _.        _.        _.        _.        _.        _.        _.1   1   _.        _.        _.        _.        _.        _.        ,
		//. 1      _.     1  _.        _.        _.        _.        _.        _.        _.        _. 111    _.        _.        _.        _.        _.        _.        ,


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
			0b10100010, 0b00010000, 0b00011000, 0b00000010, 0b11111110, 0b00000010, 0b11000010, 0b00100000, 0b10000010, 0b00000010, 0b00010000, 0b00010000, 0b00100000, 0b11111110, 0b00010000, 0b00010000,
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

		_fontTextureRaw.resize(kFontTexturePixelCount * 4);
		for (uint32 pixelIndex = 0; pixelIndex < kFontTexturePixelCount; ++pixelIndex)
		{
			const uint32 byteOffset = pixelIndex / kBitsPerByte;
			const uint32 bitShiftToLeft = kBitsPerByte - (pixelIndex % kBitsPerByte + 1);

			const uint8 bitMask = static_cast<uint8>(fs::Math::pow2_ui32(bitShiftToLeft));
			const uint8 color = (kFontTextureRawBitData[byteOffset] & bitMask) ? 255 : 0;
			_fontTextureRaw[static_cast<uint64>(pixelIndex) * 4 + 0] = color;
			_fontTextureRaw[static_cast<uint64>(pixelIndex) * 4 + 1] = color;
			_fontTextureRaw[static_cast<uint64>(pixelIndex) * 4 + 2] = color;
			_fontTextureRaw[static_cast<uint64>(pixelIndex) * 4 + 3] = color;
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

	void GraphicDevice::beginRendering()
	{
		_deviceContext->ClearRenderTargetView(_backBufferRtv.Get(), _clearColor);

		_triangleVertexArray.clear();
		_triangleIndexArray.clear();
	}

	void GraphicDevice::endRendering()
	{
#pragma region TODO: Render font
		_deviceContext->PSSetShaderResources(0, 1, _fontTextureSrv.GetAddressOf());
#pragma endregion

#pragma region Render triangles
		prepareTriangleDataBuffer();

		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_deviceContext->IASetVertexBuffers(0, 1, _triangleVertexBuffer.GetAddressOf(), &_triangleVertexStride, &_triangleVertexOffset);
		_deviceContext->IASetIndexBuffer(_triangleIndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R16_UINT, _triangleIndexOffset);
		_deviceContext->DrawIndexed(_cachedTriangleIndexCount, _triangleIndexOffset, _triangleIndexOffset);
#pragma endregion

		_swapChain->Present(0, 0);
	}

	void GraphicDevice::prepareTriangleDataBuffer()
	{
		const uint32 triangleVertexCount = static_cast<uint32>(_triangleVertexArray.size());
		if (_triangleVertexBuffer.Get() == nullptr || _cachedTriangleVertexCount < triangleVertexCount)
		{
			D3D11_BUFFER_DESC bufferDescriptor{};
			bufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
			bufferDescriptor.ByteWidth = static_cast<UINT>(triangleVertexCount * _triangleVertexStride);
			bufferDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			bufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
			bufferDescriptor.MiscFlags = 0;
			_device->CreateBuffer(&bufferDescriptor, nullptr, _triangleVertexBuffer.ReleaseAndGetAddressOf());

			_cachedTriangleVertexCount = triangleVertexCount;
		}

		if (false == _triangleVertexArray.empty())
		{
			D3D11_MAPPED_SUBRESOURCE mappedVertexResource{};
			if (_deviceContext->Map(_triangleVertexBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexResource) == S_OK)
			{
				memcpy(mappedVertexResource.pData, &_triangleVertexArray[0], static_cast<size_t>(triangleVertexCount) * _triangleVertexStride);

				_deviceContext->Unmap(_triangleVertexBuffer.Get(), 0);
			}
		}

		const uint32 triangleIndexCount = static_cast<uint32>(_triangleIndexArray.size());
		if (_triangleIndexBuffer.Get() == nullptr || _cachedTriangleIndexCount < triangleIndexCount)
		{
			D3D11_BUFFER_DESC bufferDescriptor{};
			bufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
			bufferDescriptor.ByteWidth = static_cast<UINT>(triangleIndexCount * sizeof(TriangleIndexType));
			bufferDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
			bufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
			bufferDescriptor.MiscFlags = 0;
			_device->CreateBuffer(&bufferDescriptor, nullptr, _triangleIndexBuffer.ReleaseAndGetAddressOf());

			_cachedTriangleIndexCount = triangleIndexCount;
		}

		if (false == _triangleIndexArray.empty())
		{
			D3D11_MAPPED_SUBRESOURCE mappedIndexResource{};
			if (_deviceContext->Map(_triangleIndexBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedIndexResource) == S_OK)
			{
				memcpy(mappedIndexResource.pData, &_triangleIndexArray[0], static_cast<size_t>(triangleIndexCount) * sizeof(TriangleIndexType));

				_deviceContext->Unmap(_triangleIndexBuffer.Get(), 0);
			}
		}
	}

}
