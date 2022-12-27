#include <MintRenderingBase/Include/GraphicDevice.h>

#include <d3dcompiler.h>
#include <functional>
#include <typeinfo>

#include <MintLibrary/Include/Algorithm.hpp>

#include <MintPlatform/Include/IWindow.h>
#include <MintPlatform/Include/WindowsWindow.h>

#include <MintContainer/Include/StackString.hpp>
#include <MintContainer/Include/HashMap.hpp>

#include <MintRenderingBase/Include/LowLevelRenderer.hpp>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")


namespace mint
{
	namespace Rendering
	{
#define MINT_CHECK_STATE(a, b) if (a == b) { return; } a = b;
#define MINT_CHECK_TWO_STATES(a, aa, b, bb) if ((a == aa) && (b == bb)) { return; } a = aa; b = bb;


		SafeResourceMapper::SafeResourceMapper(GraphicDevice& graphicDevice, ID3D11Resource* const resource, const uint32 subresource)
			: _graphicDevice{ graphicDevice }
			, _resource{ resource }
			, _subresource{ subresource }
			, _mappedSubresource{}
		{
			if (FAILED(_graphicDevice._deviceContext->Map(_resource, _subresource, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &_mappedSubresource)))
			{
				_mappedSubresource.pData = nullptr;
				_mappedSubresource.DepthPitch = 0;
				_mappedSubresource.RowPitch = 0;
			}
		}

		SafeResourceMapper::~SafeResourceMapper()
		{
			if (isValid() == true)
			{
				_graphicDevice._deviceContext->Unmap(_resource, _subresource);
			}
		}

		bool SafeResourceMapper::isValid() const noexcept
		{
			return _mappedSubresource.pData != nullptr;
		}

		void SafeResourceMapper::set(const void* const data, const uint32 size) noexcept
		{
			memcpy(_mappedSubresource.pData, data, size);
		}


		GraphicDevice::StateManager::StateManager(GraphicDevice& graphicDevice)
			: _graphicDevice{ graphicDevice }
			, _iaRenderingPrimitive{ RenderingPrimitive::INVALID }
			, _iaInputLayout{ nullptr }
			, _rsRasterizerState{ nullptr }
			, _rsViewport{}
			, _rsScissorRectangle{}
			, _vsShader{ nullptr }
			, _gsShader{ nullptr }
			, _psShader{ nullptr }
		{
			__noop;
		}

		void GraphicDevice::StateManager::setIAInputLayout(ID3D11InputLayout* const iaInputLayout) noexcept
		{
			MINT_CHECK_STATE(_iaInputLayout, iaInputLayout);

			_graphicDevice._deviceContext->IASetInputLayout(_iaInputLayout);
		}

		void GraphicDevice::StateManager::setIARenderingPrimitive(const RenderingPrimitive iaRenderingPrimitive) noexcept
		{
			MINT_CHECK_STATE(_iaRenderingPrimitive, iaRenderingPrimitive);

			switch (iaRenderingPrimitive)
			{
			case RenderingPrimitive::INVALID:
				MINT_NEVER;
				break;
			case RenderingPrimitive::LineList:
				_graphicDevice._deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
				break;
			case RenderingPrimitive::TriangleList:
				_graphicDevice._deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				break;
			default:
				break;
			}
		}

		void GraphicDevice::StateManager::setIAVertexBuffers(const int32 bindingStartSlot, const uint32 bufferCount, ID3D11Buffer* const* const buffers, const uint32* const strides, const uint32* const offsets) noexcept
		{
			_graphicDevice._deviceContext->IASetVertexBuffers(bindingStartSlot, bufferCount, buffers, strides, offsets);
		}

		void GraphicDevice::StateManager::setIAIndexBuffer(ID3D11Buffer* const buffer, const DXGI_FORMAT format, const uint32 offset) noexcept
		{
			_graphicDevice._deviceContext->IASetIndexBuffer(buffer, format, offset);
		}

		void GraphicDevice::StateManager::setRSRasterizerState(ID3D11RasterizerState* const rsRasterizerState) noexcept
		{
			MINT_CHECK_STATE(_rsRasterizerState, rsRasterizerState);

			_graphicDevice._deviceContext->RSSetState(_rsRasterizerState);
		}

		void GraphicDevice::StateManager::setRSViewport(const D3D11_VIEWPORT rsViewport) noexcept
		{
			MINT_CHECK_STATE(_rsViewport, rsViewport);

			_graphicDevice._deviceContext->RSSetViewports(1, &rsViewport);
		}

		void GraphicDevice::StateManager::setRSScissorRectangle(const D3D11_RECT rsScissorRectangle) noexcept
		{
			MINT_CHECK_STATE(_rsScissorRectangle, rsScissorRectangle);

			_graphicDevice._deviceContext->RSSetScissorRects(1, &rsScissorRectangle);
		}

		void GraphicDevice::StateManager::setVSShader(ID3D11VertexShader* const shader) noexcept
		{
			MINT_CHECK_STATE(_vsShader, shader);

			_graphicDevice._deviceContext->VSSetShader(shader, nullptr, 0);
		}

		void GraphicDevice::StateManager::setGSShader(ID3D11GeometryShader* const shader) noexcept
		{
			MINT_CHECK_STATE(_gsShader, shader);

			_graphicDevice._deviceContext->GSSetShader(shader, nullptr, 0);
		}

		void GraphicDevice::StateManager::setPSShader(ID3D11PixelShader* const shader) noexcept
		{
			MINT_CHECK_STATE(_psShader, shader);

			_graphicDevice._deviceContext->PSSetShader(shader, nullptr, 0);
		}

		void GraphicDevice::StateManager::setVSResources(const DxResource& resource) noexcept
		{
			Vector<GraphicObjectID>& shaderResources = _vsShaderResources;
			if (shaderResources.size() <= resource.getRegisterIndex())
			{
				shaderResources.resize(resource.getRegisterIndex() + 1);
			}
			if (resource.needToBind() == true)
			{
				shaderResources[resource.getRegisterIndex()] = GraphicObjectID();
			}
			MINT_CHECK_STATE(shaderResources[resource.getRegisterIndex()], resource.getID());

			_graphicDevice._deviceContext->VSSetShaderResources(resource.getRegisterIndex(), 1, resource.getResourceView());
		}

		void GraphicDevice::StateManager::setGSResources(const DxResource& resource) noexcept
		{
			Vector<GraphicObjectID>& shaderResources = _gsShaderResources;
			if (shaderResources.size() <= resource.getRegisterIndex())
			{
				shaderResources.resize(resource.getRegisterIndex() + 1);
			}
			if (resource.needToBind() == true)
			{
				shaderResources[resource.getRegisterIndex()] = GraphicObjectID();
			}
			MINT_CHECK_STATE(shaderResources[resource.getRegisterIndex()], resource.getID());

			_graphicDevice._deviceContext->GSSetShaderResources(resource.getRegisterIndex(), 1, resource.getResourceView());
		}

		void GraphicDevice::StateManager::setPSResources(const DxResource& resource) noexcept
		{
			Vector<GraphicObjectID>& shaderResources = _psShaderResources;
			if (shaderResources.size() <= resource.getRegisterIndex())
			{
				shaderResources.resize(resource.getRegisterIndex() + 1);
			}
			if (resource.needToBind() == true)
			{
				shaderResources[resource.getRegisterIndex()] = GraphicObjectID();
			}
			MINT_CHECK_STATE(shaderResources[resource.getRegisterIndex()], resource.getID());

			_graphicDevice._deviceContext->PSSetShaderResources(resource.getRegisterIndex(), 1, resource.getResourceView());
		}

		void GraphicDevice::StateManager::setVSConstantBuffers(const DxResource& constantBuffer)
		{
			Vector<GraphicObjectID>& constantBuffers = _vsConstantBuffers;
			if (constantBuffers.size() <= constantBuffer.getRegisterIndex())
			{
				constantBuffers.resize(constantBuffer.getRegisterIndex() + 1);
			}
			if (constantBuffer.needToBind() == true)
			{
				constantBuffers[constantBuffer.getRegisterIndex()] = GraphicObjectID();
			}
			MINT_CHECK_STATE(constantBuffers[constantBuffer.getRegisterIndex()], constantBuffer.getID());

			_graphicDevice._deviceContext->VSSetConstantBuffers(constantBuffer.getRegisterIndex(), 1, constantBuffer.getBuffer());
		}

		void GraphicDevice::StateManager::setGSConstantBuffers(const DxResource& constantBuffer)
		{
			Vector<GraphicObjectID>& constantBuffers = _gsConstantBuffers;
			if (constantBuffers.size() <= constantBuffer.getRegisterIndex())
			{
				constantBuffers.resize(constantBuffer.getRegisterIndex() + 1);
			}
			if (constantBuffer.needToBind() == true)
			{
				constantBuffers[constantBuffer.getRegisterIndex()] = GraphicObjectID();
			}
			MINT_CHECK_STATE(constantBuffers[constantBuffer.getRegisterIndex()], constantBuffer.getID());

			_graphicDevice._deviceContext->GSSetConstantBuffers(constantBuffer.getRegisterIndex(), 1, constantBuffer.getBuffer());
		}

		void GraphicDevice::StateManager::setPSConstantBuffers(const DxResource& constantBuffer)
		{
			Vector<GraphicObjectID>& constantBuffers = _psConstantBuffers;
			if (constantBuffers.size() <= constantBuffer.getRegisterIndex())
			{
				constantBuffers.resize(constantBuffer.getRegisterIndex() + 1);
			}
			if (constantBuffer.needToBind() == true)
			{
				constantBuffers[constantBuffer.getRegisterIndex()] = GraphicObjectID();
			}
			MINT_CHECK_STATE(constantBuffers[constantBuffer.getRegisterIndex()], constantBuffer.getID());

			_graphicDevice._deviceContext->PSSetConstantBuffers(constantBuffer.getRegisterIndex(), 1, constantBuffer.getBuffer());
		}


		GraphicDevice& GraphicDevice::getInvalidInstance()
		{
			static Platform::WindowsWindow invalidWindow;
			static GraphicDevice invalidInstance(invalidWindow);
			return invalidInstance;
		}

		GraphicDevice::GraphicDevice(Platform::IWindow& window)
			: _window{ window }
			, _clearColor{ 0.875f, 0.875f, 0.875f, 1.0f }
			, _currentRasterizerFor3D{ nullptr }
			, _fullScreenViewport{}
			, _shaderPool{ *this, &_shaderHeaderMemory, DxShaderVersion::v_5_0 }
			, _resourcePool{ *this }
			, _stateManager{ *this }
			, _shapeRendererContext{ *this }
			, _needEndRenderingCall{ false }
			, _guiContext{ *this }
		{
			__noop;
		}

		bool GraphicDevice::initialize()
		{
			_clearColor = _window.getBackgroundColor();
			_lastWindowSize = _window.getSize();

			createDxDevice();

			if (loadFontData() == false)
			{
				return false;
			}

			_guiContext.initialize();

			return true;
		}

		void GraphicDevice::updateScreenSize()
		{
			if (_window.getSize() == _lastWindowSize)
			{
				return;
			}

			_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
			_deviceContext->OMSetDepthStencilState(nullptr, 0);

			_backBufferRtv->Release();

			_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0);

			initializeBackBuffer();
			initializeDepthStencilBufferAndView(_window.getSize());
			initializeFullScreenData(_window.getSize());
			initialize2DProjectionMatrix(Float2(_window.getSize()));

			setDefaultRenderTargetsAndDepthStencil();

			_lastWindowSize = _window.getSize();
		}

		void GraphicDevice::createDxDevice()
		{
			const Platform::WindowsWindow& windowsWindow = static_cast<const Platform::WindowsWindow&>(_window);
			const Int2& windowSize = windowsWindow.getSize();

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
				rasterizerDescriptor.FrontCounterClockwise = TRUE; // 오른손 좌표계
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

		bool GraphicDevice::loadFontData()
		{
			static const Path kDefaultFontPath = Path::makeAssetPath("noto_sans_kr_medium");
			FontLoader fontLoader;
			if (FontLoader::doesExistFont(kDefaultFontPath) == false)
			{
				fontLoader.pushGlyphRange(GlyphRange(0, 0x33DD));
				fontLoader.pushGlyphRange(GlyphRange(L'가', L'힣'));
				if (fontLoader.bakeFontData(kDefaultFontPath, kDefaultFontSize, kDefaultFontPath, 2048, 1, 1) == false)
				{
					MINT_LOG_ERROR("폰트 데이터를 Bake 하는 데 실패했습니다!");
					return false;
				}
			}

			if (fontLoader.loadFont(kDefaultFontPath, *this) == false)
			{
				MINT_LOG_ERROR("폰트 데이터를 로드하지 못했습니다!");
				return false;
			}

			_shapeRendererContext.initializeFontData(fontLoader.getFontData());

			return true;
		}

		bool GraphicDevice::createSwapChain(const Int2& windowSize, const HWND windowHandle)
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
				MINT_LOG_ERROR("SwapChain 생성에 실패했습니다!");
				return false;
			}
			return true;
		}

		bool GraphicDevice::initializeBackBuffer()
		{
			ComPtr<ID3D11Texture2D> backBuffer;
			_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.ReleaseAndGetAddressOf()));

			if (FAILED(_device->CreateRenderTargetView(backBuffer.Get(), nullptr, _backBufferRtv.ReleaseAndGetAddressOf())))
			{
				MINT_LOG_ERROR("BackBuffer 초기화에 실패했습니다!");
				return false;
			}
			return true;
		}

		bool GraphicDevice::initializeDepthStencilBufferAndView(const Int2& windowSize)
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
				MINT_LOG_ERROR("DepthStencil 텍스쳐 생성에 실패했습니다.");
				return false;
			}

			if (FAILED(_device->CreateDepthStencilView(_depthStencilBuffer.Get(), nullptr, _depthStencilView.ReleaseAndGetAddressOf())))
			{
				MINT_LOG_ERROR("DepthStencil 뷰 생성에 실패했습니다.");
				return false;
			}

			return true;
		}

		bool GraphicDevice::initializeDepthStencilStates()
		{
			D3D11_DEPTH_STENCIL_DESC depthStencilDescriptor;
			depthStencilDescriptor.DepthEnable = TRUE;
			depthStencilDescriptor.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
			depthStencilDescriptor.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDescriptor.StencilEnable = FALSE;
			if (FAILED(_device->CreateDepthStencilState(&depthStencilDescriptor, _depthStencilStateLessEqual.ReleaseAndGetAddressOf())))
			{
				MINT_LOG_ERROR("DepthStencil State 생성에 실패했습니다.");
				return false;
			}

			return true;
		}

		void GraphicDevice::initializeShaderHeaderMemory()
		{
			const Int2 windowSize = _window.getSize();

			// Stream data
			{
				_cppHlslStreamData.parseCppHlslFile(Path::makeIncludeAssetPath("CppHlsl/CppHlslStreamData.h"));
				_cppHlslStreamData.generateHlslString(Language::CppHlsl::CppHlslFileType::StreamData);
				_shaderHeaderMemory.pushHeader("ShaderStructDefinitions", _cppHlslStreamData.getHlslString());
			}

			using namespace Language;

			// Constant buffers
			{
				_cppHlslConstantBuffers.parseCppHlslFile(Path::makeIncludeAssetPath("CppHlsl/CppHlslConstantBuffers.h"));
				_cppHlslConstantBuffers.generateHlslString(Language::CppHlsl::CppHlslFileType::ConstantBuffers);
				_shaderHeaderMemory.pushHeader("ShaderConstantBuffers", _cppHlslConstantBuffers.getHlslString());

				{
					const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _cppHlslConstantBuffers.getTypeMetaData(typeid(_cbViewData));
					_cbViewID = _resourcePool.pushConstantBuffer(&_cbViewData, sizeof(_cbViewData), typeMetaData._customData.getRegisterIndex());

					DxResource& cbView = _resourcePool.getResource(_cbViewID);
					cbView.bindToShader(GraphicShaderType::VertexShader, cbView.getRegisterIndex());
					cbView.bindToShader(GraphicShaderType::GeometryShader, cbView.getRegisterIndex());
					cbView.bindToShader(GraphicShaderType::PixelShader, cbView.getRegisterIndex());
				}

				{
					CB_Transform cbTransformData;
					const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _cppHlslConstantBuffers.getTypeMetaData(typeid(cbTransformData));
					_cbTransformID = _resourcePool.pushConstantBuffer(&cbTransformData, sizeof(cbTransformData), typeMetaData._customData.getRegisterIndex());
				}

				initialize2DProjectionMatrix(Float2(windowSize));
			}

			// Structured buffers
			{
				_cppHlslStructuredBuffers.parseCppHlslFile(Path::makeIncludeAssetPath("CppHlsl/CppHlslStructuredBuffers.h"));
				_cppHlslStructuredBuffers.generateHlslString(Language::CppHlsl::CppHlslFileType::StructuredBuffers);
				_shaderHeaderMemory.pushHeader("ShaderStructuredBufferDefinitions", _cppHlslStructuredBuffers.getHlslString());

				{
					SB_Transform sbTransformData;
					const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _cppHlslStructuredBuffers.getTypeMetaData(typeid(sbTransformData));
					_sbTransformID = _resourcePool.pushStructuredBuffer(&sbTransformData, sizeof(sbTransformData), 1, typeMetaData._customData.getRegisterIndex());
				}

				{
					SB_Material sbMaterialData;
					const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _cppHlslStructuredBuffers.getTypeMetaData(typeid(sbMaterialData));
					_sbMaterialID = _resourcePool.pushStructuredBuffer(&sbMaterialData, sizeof(sbMaterialData), 1, typeMetaData._customData.getRegisterIndex());
				}
			}
		}

		void GraphicDevice::initializeShaders()
		{
			_shapeRendererContext.initializeShaders();
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

		void GraphicDevice::initializeFullScreenData(const Int2& windowSize)
		{
			_fullScreenViewport.Width = static_cast<FLOAT>(windowSize._x);
			_fullScreenViewport.Height = static_cast<FLOAT>(windowSize._y);
			_fullScreenViewport.MinDepth = 0.0f;
			_fullScreenViewport.MaxDepth = 1.0f;
			_fullScreenViewport.TopLeftX = 0.0f;
			_fullScreenViewport.TopLeftY = 0.0f;

			_fullScreenClipRect.left(0);
			_fullScreenClipRect.right(static_cast<float>(windowSize._x));
			_fullScreenClipRect.top(0);
			_fullScreenClipRect.bottom(static_cast<float>(windowSize._y));
		}

		void GraphicDevice::setDefaultRenderTargetsAndDepthStencil()
		{
			_deviceContext->OMSetRenderTargets(1, _backBufferRtv.GetAddressOf(), _depthStencilView.Get());
			_deviceContext->OMSetDepthStencilState(_depthStencilStateLessEqual.Get(), 0);
		}

		void GraphicDevice::beginRendering()
		{
			if (_needEndRenderingCall)
			{
				MINT_LOG_ERROR("beginRendering() 을 두 번 연달아 호출할 수 없습니다. 먼저 endRendering() 을 호출해 주세요!");
				return;
			}

			_needEndRenderingCall = true;

			_deviceContext->ClearRenderTargetView(_backBufferRtv.Get(), _clearColor);
			_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			_shapeRendererContext.flush();
		}

		void GraphicDevice::draw(const uint32 vertexCount, const uint32 vertexOffset) noexcept
		{
			_deviceContext->Draw(vertexCount, vertexOffset);
		}

		void GraphicDevice::drawIndexed(const uint32 indexCount, const uint32 indexOffset, const uint32 vertexOffset) noexcept
		{
			_deviceContext->DrawIndexed(indexCount, indexOffset, vertexOffset);
		}

		void GraphicDevice::endRendering()
		{
			if (_needEndRenderingCall == false)
			{
				MINT_LOG_ERROR("endRendering() 을 두 번 연달아 호출할 수 없습니다. 먼저 beginRendering() 을 호출해 주세요!");
				return;
			}

#pragma region Renderer Contexts
			useFullScreenViewport();

			_shapeRendererContext.render();
			_shapeRendererContext.flush();
			_guiContext.render();
#pragma endregion

			_swapChain->Present(0, 0);

			_needEndRenderingCall = false;
		}

		void GraphicDevice::useScissorRectangles() noexcept
		{
			_stateManager.setRSRasterizerState(_rasterizerStateScissorRectangles.Get());
			_stateManager.setRSViewport(_fullScreenViewport);
		}

		void GraphicDevice::useFullScreenViewport() noexcept
		{
			_stateManager.setRSRasterizerState(_currentRasterizerFor3D);
			_stateManager.setRSViewport(_fullScreenViewport);
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

		const Rect& GraphicDevice::getFullScreenClipRect() const noexcept
		{
			return _fullScreenClipRect;
		}

		void GraphicDevice::initialize2DProjectionMatrix(const Float2& windowSize) noexcept
		{
			_cbViewData._cb2DProjectionMatrix = Float4x4::projectionMatrix2DFromTopLeft(windowSize._x, windowSize._y);

			DxResource& cbView = _resourcePool.getResource(_cbViewID);
			cbView.updateBuffer(&_cbViewData, 1);
		}

		void GraphicDevice::setViewProjectionMatrix(const Float4x4& viewMatrix, const Float4x4& projectionMatrix) noexcept
		{
			_cbViewData._cbViewMatrix = viewMatrix;
			_cbViewData._cb3DProjectionMatrix = projectionMatrix;
			_cbViewData._cbViewProjectionMatrix = _cbViewData._cb3DProjectionMatrix * _cbViewData._cbViewMatrix;

			DxResource& cbView = _resourcePool.getResource(_cbViewID);
			cbView.updateBuffer(&_cbViewData, 1);
		}

		const Int2& GraphicDevice::getWindowSize() const noexcept
		{
			return _window.getSize();
		}

		Float2 GraphicDevice::getWindowSizeFloat2() const noexcept
		{
			return Float2(getWindowSize());
		}

		Platform::IWindow& GraphicDevice::accessWindow() noexcept
		{
			return _window;
		}

		const Platform::IWindow& GraphicDevice::getWindow() const noexcept
		{
			return _window;
		}
	}
}
