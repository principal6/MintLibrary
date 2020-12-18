#include <stdafx.h>
#include <SimpleRendering/GraphicDevice.h>

#include <d3dcompiler.h>
#include <functional>
#include <typeinfo>

#include <Algorithm.hpp>

#include <Platform/IWindow.h>
#include <Platform/WindowsWindow.h>

#include <Math/Float2.hpp>
#include <Math/Float3.hpp>
#include <Math/Float4.hpp>
#include <Math/Float4x4.hpp>

#include <Container/ScopeString.hpp>


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
		, _triangleVertexStride{ sizeof(fs::CppHlsl::VS_INPUT) }
		, _triangleVertexOffset{ 0 }
		, _cachedTriangleIndexCount{ 0 }
		, _triangleIndexOffset{ 0 }
		, _cppHlslParserStructs{ _cppHlslLexerStructs }
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
			SwapChainDesc.BufferDesc.Width = static_cast<UINT>(windowSize._x);
			SwapChainDesc.BufferDesc.Height = static_cast<UINT>(windowSize._y);
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
			TextFileReader textFileReader;
			textFileReader.open("FsLibrary/SimpleRendering/CppHlslStructs.h");
			_cppHlslLexerStructs.setSource(textFileReader.get());
			_cppHlslLexerStructs.execute();
			_cppHlslParserStructs.execute();


			CppHlsl::VS_INPUT vsInput;
			CppHlsl::VS_OUTPUT vsOutput;
			{
				std::string shaderHeaderContent;
				shaderHeaderContent.append(Language::CppHlslParser::serializeCppHlslTypeToHlslStruct(_cppHlslParserStructs.getTypeInfo("VS_INPUT"), false));
				shaderHeaderContent.append(Language::CppHlslParser::serializeCppHlslTypeToHlslStruct(_cppHlslParserStructs.getTypeInfo("VS_OUTPUT"), true));
				_shaderHeaderMemory.pushHeader("ShaderStructDefinitions", shaderHeaderContent.c_str());
			}
		}

		// Constant buffers
		{
			TextFileReader textFileReader;
			textFileReader.open("FsLibrary/SimpleRendering/CppHlslCbuffers.h");
			Language::CppHlslLexer cppHlslLexer{ textFileReader.get() };
			cppHlslLexer.execute();
			Language::CppHlslParser cppHlslParser{ cppHlslLexer };
			cppHlslParser.execute();


			fs::CppHlsl::CB_Transforms cbTransforms;
			cbTransforms._cbProjectionMatrix = fs::Float4x4::projectionMatrix2DFromTopLeft(static_cast<float>(windowSize._x), static_cast<float>(windowSize._y));
			{
				std::string shaderHeaderContent;
				const Language::CppHlslTypeInfo& typeInfo = cppHlslParser.getTypeInfo("CB_Transforms");
				shaderHeaderContent.append(Language::CppHlslParser::serializeCppHlslTypeToHlslCbuffer(typeInfo, 0));
				_shaderHeaderMemory.pushHeader("VsConstantBuffers", shaderHeaderContent.c_str());

				DxObjectId id = _bufferPool.pushBuffer(DxBufferType::ConstantBuffer, reinterpret_cast<const byte*>(&cbTransforms._cbProjectionMatrix), typeInfo.getSize());
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
			fs::CppHlsl::VS_INPUT vsInput;
			const Language::CppHlslTypeInfo& typeInfo = _cppHlslParserStructs.getTypeInfo("VS_INPUT");
			DxObjectId id = _shaderPool.pushVertexShader(kVertexShaderContent, "main", DxShaderVersion::v_4_0, &typeInfo);
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
			static constexpr const char kPixelShaderContentTest[]
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
						float ddx_ = ddx(input._texCoord.x);
						float ddy_ = ddy(input._texCoord.y);
						
						float4 sample  = texture0.Sample(sampler0, input._texCoord);
						float4 sample0 = texture0.Sample(sampler0, input._texCoord + float2(-ddx_, -ddy_));
						float4 sample1 = texture0.Sample(sampler0, input._texCoord + float2(0    , -ddy_)); // u
						float4 sample2 = texture0.Sample(sampler0, input._texCoord + float2(+ddx_, -ddy_));
						float4 sample3 = texture0.Sample(sampler0, input._texCoord + float2(-ddx_, 0    )); // l
						float4 sample4 = texture0.Sample(sampler0, input._texCoord + float2(+ddx_, 0    )); // r
						float4 sample5 = texture0.Sample(sampler0, input._texCoord + float2(-ddx_, +ddy_));
						float4 sample6 = texture0.Sample(sampler0, input._texCoord + float2(0    , +ddy_)); // d
						float4 sample7 = texture0.Sample(sampler0, input._texCoord + float2(+ddx_, +ddy_));

						float4 blended = sample * 0.25;
						blended += ((sample1 + sample3 + sample4 + sample6) / 4.0) * 0.5;
						blended += ((sample0 + sample2 + sample5 + sample7) / 4.0) * 0.5;
						if (0 < blended.r)
						{
							blended.r = 1;
						}
						result *= blended;
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
			viewport.Width = static_cast<FLOAT>(windowSize._x);
			viewport.Height = static_cast<FLOAT>(windowSize._y);
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = viewport.TopLeftY = 0.0f;
			_deviceContext->RSSetViewports(1, &viewport);
		}
	}

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
