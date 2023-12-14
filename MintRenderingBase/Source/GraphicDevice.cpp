#include <MintRenderingBase/Include/GraphicDevice.h>

#include <d3dcompiler.h>
#include <functional>
#include <typeinfo>

#include <MintPlatform/Include/Window.h>

#include <MintContainer/Include/StackString.hpp>
#include <MintContainer/Include/HashMap.hpp>
#include <MintContainer/Include/Algorithm.hpp>

#include <MintRenderingBase/Include/LowLevelRenderer.hpp>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")


namespace mint
{
	namespace Rendering
	{
#define MINT_COMPARE_SET_OR_RETURN(a, b) if (a == b) { return; } a = b;
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
			if (IsValid() == true)
			{
				_graphicDevice._deviceContext->Unmap(_resource, _subresource);
			}
		}

		bool SafeResourceMapper::IsValid() const noexcept
		{
			return _mappedSubresource.pData != nullptr;
		}

		void SafeResourceMapper::Set(const void* const data, const uint32 size) noexcept
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

		void GraphicDevice::StateManager::SetIAInputLayout(ID3D11InputLayout* const iaInputLayout) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_iaInputLayout, iaInputLayout);

			_graphicDevice._deviceContext->IASetInputLayout(_iaInputLayout);
		}

		void GraphicDevice::StateManager::SetIARenderingPrimitive(const RenderingPrimitive iaRenderingPrimitive) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_iaRenderingPrimitive, iaRenderingPrimitive);

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

		void GraphicDevice::StateManager::SetIAVertexBuffers(const int32 bindingStartSlot, const uint32 bufferCount, ID3D11Buffer* const* const buffers, const uint32* const strides, const uint32* const offsets) noexcept
		{
			_graphicDevice._deviceContext->IASetVertexBuffers(bindingStartSlot, bufferCount, buffers, strides, offsets);
		}

		void GraphicDevice::StateManager::SetIAIndexBuffer(ID3D11Buffer* const buffer, const DXGI_FORMAT format, const uint32 offset) noexcept
		{
			_graphicDevice._deviceContext->IASetIndexBuffer(buffer, format, offset);
		}

		void GraphicDevice::StateManager::SetRSRasterizerState(ID3D11RasterizerState* const rsRasterizerState) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_rsRasterizerState, rsRasterizerState);

			_graphicDevice._deviceContext->RSSetState(_rsRasterizerState);
		}

		void GraphicDevice::StateManager::SetRSViewport(const D3D11_VIEWPORT rsViewport) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_rsViewport, rsViewport);

			_graphicDevice._deviceContext->RSSetViewports(1, &rsViewport);
		}

		void GraphicDevice::StateManager::SetRSScissorRectangle(const D3D11_RECT rsScissorRectangle) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_rsScissorRectangle, rsScissorRectangle);

			_graphicDevice._deviceContext->RSSetScissorRects(1, &rsScissorRectangle);
		}

		void GraphicDevice::StateManager::SetVSShader(ID3D11VertexShader* const shader) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_vsShader, shader);

			_graphicDevice._deviceContext->VSSetShader(shader, nullptr, 0);
		}

		void GraphicDevice::StateManager::SetGSShader(ID3D11GeometryShader* const shader) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_gsShader, shader);

			_graphicDevice._deviceContext->GSSetShader(shader, nullptr, 0);
		}

		void GraphicDevice::StateManager::SetPSShader(ID3D11PixelShader* const shader) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_psShader, shader);

			_graphicDevice._deviceContext->PSSetShader(shader, nullptr, 0);
		}

		void GraphicDevice::StateManager::SetShaderResources(GraphicShaderType graphicShaderType, const DxResource* resource, uint32 bindingSlot) noexcept
		{
			Vector<GraphicObjectID>* shaderResourceIDs = nullptr;
			if (graphicShaderType == GraphicShaderType::VertexShader)
			{
				shaderResourceIDs = &_vsShaderResourceIDs;
			}
			else if (graphicShaderType == GraphicShaderType::GeometryShader)
			{
				shaderResourceIDs = &_gsShaderResourceIDs;
			}
			else if (graphicShaderType == GraphicShaderType::PixelShader)
			{
				shaderResourceIDs = &_psShaderResourceIDs;
			}

			if (resource == nullptr)
			{
				MINT_ASSERT(bindingSlot < shaderResourceIDs->Size(), "반드시 호출자에서 보장해 주세요.");

				(*shaderResourceIDs)[bindingSlot] = GraphicObjectID();

				ID3D11ShaderResourceView* const nullSRVs[1]{ nullptr };
				if (graphicShaderType == GraphicShaderType::VertexShader)
				{
					_graphicDevice._deviceContext->VSSetShaderResources(bindingSlot, 1, nullSRVs);
				}
				else if (graphicShaderType == GraphicShaderType::GeometryShader)
				{
					_graphicDevice._deviceContext->GSSetShaderResources(bindingSlot, 1, nullSRVs);
				}
				else if (graphicShaderType == GraphicShaderType::PixelShader)
				{
					_graphicDevice._deviceContext->PSSetShaderResources(bindingSlot, 1, nullSRVs);
				}
				return;
			}

			if (bindingSlot == kUint32Max)
			{
				bindingSlot = resource->GetRegisterIndex();
			}

			if (shaderResourceIDs->Size() <= bindingSlot)
			{
				shaderResourceIDs->Resize(bindingSlot + 1);
			}
			if (resource->NeedsToBind() == true)
			{
				(*shaderResourceIDs)[bindingSlot] = GraphicObjectID();
			}
			MINT_COMPARE_SET_OR_RETURN((*shaderResourceIDs)[bindingSlot], resource->GetID());

			if (graphicShaderType == GraphicShaderType::VertexShader)
			{
				_graphicDevice._deviceContext->VSSetShaderResources(bindingSlot, 1, resource->GetResourceView());
			}
			else if (graphicShaderType == GraphicShaderType::GeometryShader)
			{
				_graphicDevice._deviceContext->GSSetShaderResources(bindingSlot, 1, resource->GetResourceView());
			}
			else if (graphicShaderType == GraphicShaderType::PixelShader)
			{
				_graphicDevice._deviceContext->PSSetShaderResources(bindingSlot, 1, resource->GetResourceView());
			}
		}

		void GraphicDevice::StateManager::SetConstantBuffers(GraphicShaderType graphicShaderType, const DxResource* constantBuffer, uint32 bindingSlot)
		{
			Vector<GraphicObjectID>* constantBufferIDs = nullptr;
			if (graphicShaderType == GraphicShaderType::VertexShader)
			{
				constantBufferIDs = &_vsConstantBufferIDs;
			}
			else if (graphicShaderType == GraphicShaderType::GeometryShader)
			{
				constantBufferIDs = &_gsConstantBufferIDs;
			}
			else if (graphicShaderType == GraphicShaderType::PixelShader)
			{
				constantBufferIDs = &_psConstantBufferIDs;
			}

			if (constantBuffer == nullptr)
			{
				MINT_ASSERT(bindingSlot < constantBufferIDs->Size(), "반드시 호출자에서 보장해 주세요.");

				(*constantBufferIDs)[bindingSlot] = GraphicObjectID();

				ID3D11Buffer* const nullBuffers[1]{ nullptr };
				if (graphicShaderType == GraphicShaderType::VertexShader)
				{
					_graphicDevice._deviceContext->VSSetConstantBuffers(bindingSlot, 1, nullBuffers);
				}
				else if (graphicShaderType == GraphicShaderType::GeometryShader)
				{
					_graphicDevice._deviceContext->GSSetConstantBuffers(bindingSlot, 1, nullBuffers);
				}
				else if (graphicShaderType == GraphicShaderType::PixelShader)
				{
					_graphicDevice._deviceContext->PSSetConstantBuffers(bindingSlot, 1, nullBuffers);
				}
				return;
			}

			if (bindingSlot == kUint32Max)
			{
				bindingSlot = constantBuffer->GetRegisterIndex();
			}

			if (constantBufferIDs->Size() <= bindingSlot)
			{
				constantBufferIDs->Resize(bindingSlot + 1);
			}
			if (constantBuffer->NeedsToBind() == true)
			{
				(*constantBufferIDs)[bindingSlot] = GraphicObjectID();
			}
			MINT_COMPARE_SET_OR_RETURN((*constantBufferIDs)[bindingSlot], constantBuffer->GetID());

			if (graphicShaderType == GraphicShaderType::VertexShader)
			{
				_graphicDevice._deviceContext->VSSetConstantBuffers(bindingSlot, 1, constantBuffer->GetBuffer());
			}
			else if (graphicShaderType == GraphicShaderType::GeometryShader)
			{
				_graphicDevice._deviceContext->GSSetConstantBuffers(bindingSlot, 1, constantBuffer->GetBuffer());
			}
			else if (graphicShaderType == GraphicShaderType::PixelShader)
			{
				_graphicDevice._deviceContext->PSSetConstantBuffers(bindingSlot, 1, constantBuffer->GetBuffer());
			}
		}


		GraphicDevice& GraphicDevice::GetInvalidInstance()
		{
			static Window invalidWindow;
			static GraphicDevice invalidInstance{ invalidWindow, false };
			return invalidInstance;
		}

		GraphicDevice::GraphicDevice(Window& window, bool usesMSAA)
			: _window{ window }
			, _usesMSAA{ usesMSAA }
			, _clearColor{ 0.875f, 0.875f, 0.875f, 1.0f }
			, _currentRasterizerFor3D{ nullptr }
			, _fullScreenViewport{}
			, _shaderPool{ *this, &_shaderHeaderMemory, DxShaderVersion::v_5_0 }
			, _resourcePool{ *this }
			, _stateManager{ *this }
			, _shapeRendererContext{ *this }
			, _needEndRenderingCall{ false }
		{
			__noop;
		}

		bool GraphicDevice::Initialize()
		{
			_clearColor = _window.GetBackgroundColor();
			_lastWindowSize = _window.GetSize();

			CreateDxDevice();

			if (LoadFontData() == false)
			{
				return false;
			}
			return true;
		}

		void GraphicDevice::UpdateScreenSize()
		{
			if (_window.GetSize() == _lastWindowSize)
			{
				return;
			}

			_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
			_deviceContext->OMSetDepthStencilState(nullptr, 0);

			_backBufferRtv.Reset();

			if (FAILED(_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0)))
			{
				return;
			}

			InitializeBackBuffer();
			InitializeDepthStencilBufferAndView(_window.GetSize());
			InitializeFullScreenData(_window.GetSize());
			Initialize2DProjectionMatrix();

			SetDefaultRenderTargetsAndDepthStencil();

			_lastWindowSize = _window.GetSize();
		}

		void GraphicDevice::CreateDxDevice()
		{
			const Int2& windowSize = _window.GetSize();

			if (CreateSwapChain(windowSize, _window.GetHandle()) == false)
			{
				return;
			}

			if (InitializeBackBuffer() == false)
			{
				return;
			}

			if (InitializeDepthStencilBufferAndView(windowSize) == false)
			{
				return;
			}
			InitializeDepthStencilStates();

			InitializeFullScreenData(windowSize);
			InitializeShaderHeaderMemory();
			InitializeShaders();
			InitializeSamplerStates();
			InitializeBlendStates();

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

			SetDefaultRenderTargetsAndDepthStencil();
		}

		bool GraphicDevice::LoadFontData()
		{
			static const Path kDefaultFontPath = Path::MakeAssetPath("noto_sans_kr_medium");
			FontLoader fontLoader;
			if (FontLoader::ExistsFont(kDefaultFontPath) == false)
			{
				fontLoader.PushGlyphRange(GlyphRange(0, 0x33DD));
				fontLoader.PushGlyphRange(GlyphRange(L'가', L'힣'));
				if (fontLoader.BakeFontData(kDefaultFontPath, kDefaultFontSize, kDefaultFontPath, 2048, 1, 1) == false)
				{
					MINT_LOG_ERROR("폰트 데이터를 Bake 하는 데 실패했습니다!");
					return false;
				}
			}

			if (fontLoader.LoadFont(kDefaultFontPath, *this) == false)
			{
				MINT_LOG_ERROR("폰트 데이터를 로드하지 못했습니다!");
				return false;
			}

			_shapeRendererContext.InitializeFontData(fontLoader.GetFontData());

			return true;
		}

		bool GraphicDevice::CreateSwapChain(const Int2& windowSize, const HWND windowHandle)
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
			swapChainDescriptor.SampleDesc.Count = (_usesMSAA ? 4 : 1);
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

		bool GraphicDevice::InitializeBackBuffer()
		{
			ComPtr<ID3D11Texture2D> backBuffer;
			if (FAILED(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.ReleaseAndGetAddressOf()))))
			{
				MINT_LOG_ERROR("BackBuffer 를 얻어오는 데 실패했습니다!");
				return false;
			}

			if (FAILED(_device->CreateRenderTargetView(backBuffer.Get(), nullptr, _backBufferRtv.ReleaseAndGetAddressOf())))
			{
				MINT_LOG_ERROR("BackBuffer 초기화에 실패했습니다!");
				return false;
			}
			return true;
		}

		bool GraphicDevice::InitializeDepthStencilBufferAndView(const Int2& windowSize)
		{
			D3D11_TEXTURE2D_DESC depthStencilBufferDescriptor;
			depthStencilBufferDescriptor.Width = static_cast<UINT>(windowSize._x);
			depthStencilBufferDescriptor.Height = static_cast<UINT>(windowSize._y);
			depthStencilBufferDescriptor.MipLevels = 1;
			depthStencilBufferDescriptor.ArraySize = 1;
			depthStencilBufferDescriptor.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilBufferDescriptor.SampleDesc.Count = (_usesMSAA ? 4 : 1);
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

		bool GraphicDevice::InitializeDepthStencilStates()
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

		void GraphicDevice::InitializeShaderHeaderMemory()
		{
			const Int2 windowSize = _window.GetSize();

			// Stream data
			{
				_cppHlslStreamData.ParseCppHlslFile(Path::MakeIncludeAssetPath("CppHlsl/CppHlslStreamData.h"));
				_cppHlslStreamData.GenerateHlslString(Language::CppHlsl::CppHlslFileType::StreamData);
				_shaderHeaderMemory.pushHeader("ShaderStructDefinitions", _cppHlslStreamData.GetHlslString());
			}

			using namespace Language;

			// Constant buffers
			{
				_cppHlslConstantBuffers.ParseCppHlslFile(Path::MakeIncludeAssetPath("CppHlsl/CppHlslConstantBuffers.h"));
				_cppHlslConstantBuffers.GenerateHlslString(Language::CppHlsl::CppHlslFileType::ConstantBuffers);
				_shaderHeaderMemory.pushHeader("ShaderConstantBuffers", _cppHlslConstantBuffers.GetHlslString());

				{
					const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _cppHlslConstantBuffers.GetTypeMetaData(typeid(_cbViewData));
					_cbViewID = _resourcePool.AddConstantBuffer(&_cbViewData, sizeof(_cbViewData), typeMetaData._customData.GetRegisterIndex());

					DxResource& cbView = _resourcePool.GetResource(_cbViewID);
					cbView.BindToShader(GraphicShaderType::VertexShader, cbView.GetRegisterIndex());
					cbView.BindToShader(GraphicShaderType::GeometryShader, cbView.GetRegisterIndex());
					cbView.BindToShader(GraphicShaderType::PixelShader, cbView.GetRegisterIndex());
				}

				{
					CB_Transform cbTransformData;
					const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _cppHlslConstantBuffers.GetTypeMetaData(typeid(cbTransformData));
					_cbTransformID = _resourcePool.AddConstantBuffer(&cbTransformData, sizeof(cbTransformData), typeMetaData._customData.GetRegisterIndex());
				}

				Initialize2DProjectionMatrix();
			}

			// Structured buffers
			{
				_cppHlslStructuredBuffers.ParseCppHlslFile(Path::MakeIncludeAssetPath("CppHlsl/CppHlslStructuredBuffers.h"));
				_cppHlslStructuredBuffers.GenerateHlslString(Language::CppHlsl::CppHlslFileType::StructuredBuffers);
				_shaderHeaderMemory.pushHeader("ShaderStructuredBufferDefinitions", _cppHlslStructuredBuffers.GetHlslString());

				{
					SB_Transform sbTransformData;
					const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _cppHlslStructuredBuffers.GetTypeMetaData(typeid(sbTransformData));
					_sbTransformID = _resourcePool.AddStructuredBuffer(&sbTransformData, sizeof(sbTransformData), 1, typeMetaData._customData.GetRegisterIndex());
				}

				{
					SB_Material sbMaterialData;
					const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _cppHlslStructuredBuffers.GetTypeMetaData(typeid(sbMaterialData));
					_sbMaterialID = _resourcePool.AddStructuredBuffer(&sbMaterialData, sizeof(sbMaterialData), 1, typeMetaData._customData.GetRegisterIndex());
				}
			}
		}

		void GraphicDevice::InitializeShaders()
		{
			_shapeRendererContext.InitializeShaders();
		}

		void GraphicDevice::InitializeSamplerStates()
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
				_device->CreateSamplerState(&samplerDescriptor, _linearSamplerState.ReleaseAndGetAddressOf());
				_deviceContext->PSSetSamplers(0, 1, _linearSamplerState.GetAddressOf());
			}
			{
				D3D11_SAMPLER_DESC samplerDescriptor{};
				samplerDescriptor.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
				samplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDescriptor.MipLODBias = 0.0f;
				samplerDescriptor.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
				samplerDescriptor.MinLOD = 0.0f;
				samplerDescriptor.MaxLOD = 0.0f;
				_device->CreateSamplerState(&samplerDescriptor, _pointSamplerState.ReleaseAndGetAddressOf());
				_deviceContext->PSSetSamplers(1, 1, _pointSamplerState.GetAddressOf());
			}
		}

		void GraphicDevice::InitializeBlendStates()
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

		void GraphicDevice::InitializeFullScreenData(const Int2& windowSize)
		{
			_fullScreenViewport.Width = static_cast<FLOAT>(windowSize._x);
			_fullScreenViewport.Height = static_cast<FLOAT>(windowSize._y);
			_fullScreenViewport.MinDepth = 0.0f;
			_fullScreenViewport.MaxDepth = 1.0f;
			_fullScreenViewport.TopLeftX = 0.0f;
			_fullScreenViewport.TopLeftY = 0.0f;

			_fullScreenClipRect.Left(0);
			_fullScreenClipRect.Right(static_cast<float>(windowSize._x));
			_fullScreenClipRect.Top(0);
			_fullScreenClipRect.Bottom(static_cast<float>(windowSize._y));
		}

		void GraphicDevice::SetDefaultRenderTargetsAndDepthStencil()
		{
			_deviceContext->OMSetRenderTargets(1, _backBufferRtv.GetAddressOf(), _depthStencilView.Get());
			_deviceContext->OMSetDepthStencilState(_depthStencilStateLessEqual.Get(), 0);
		}

		void GraphicDevice::BeginRendering()
		{
			if (_needEndRenderingCall)
			{
				MINT_LOG_ERROR("BeginRendering() 을 두 번 연달아 호출할 수 없습니다. 먼저 EndRendering() 을 호출해 주세요!");
				return;
			}

			if (_window.IsResized())
			{
				UpdateScreenSize();
			}

			_needEndRenderingCall = true;

			_deviceContext->ClearRenderTargetView(_backBufferRtv.Get(), _clearColor);
			_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			_shapeRendererContext.Flush();
		}

		void GraphicDevice::Draw(const uint32 vertexCount, const uint32 vertexOffset) noexcept
		{
			_deviceContext->Draw(vertexCount, vertexOffset);
		}

		void GraphicDevice::DrawIndexed(const uint32 indexCount, const uint32 indexOffset, const uint32 vertexOffset) noexcept
		{
			_deviceContext->DrawIndexed(indexCount, indexOffset, vertexOffset);
		}

		void GraphicDevice::EndRendering()
		{
			if (_needEndRenderingCall == false)
			{
				MINT_LOG_ERROR("EndRendering() 을 두 번 연달아 호출할 수 없습니다. 먼저 BeginRendering() 을 호출해 주세요!");
				return;
			}

#pragma region Renderer Contexts
			UseFullScreenViewport();

			_shapeRendererContext.Render();
			_shapeRendererContext.Flush();
#pragma endregion

			_swapChain->Present(0, 0);

			_needEndRenderingCall = false;
		}

		void GraphicDevice::UseScissorRectangles() noexcept
		{
			_stateManager.SetRSRasterizerState(_rasterizerStateScissorRectangles.Get());
			_stateManager.SetRSViewport(_fullScreenViewport);
		}

		void GraphicDevice::UseFullScreenViewport() noexcept
		{
			_stateManager.SetRSRasterizerState(_currentRasterizerFor3D);
			_stateManager.SetRSViewport(_fullScreenViewport);
		}

		void GraphicDevice::UseWireFrameNoCullingRasterizer() noexcept
		{
			_currentRasterizerFor3D = _rasterizerStateWireFrameNoCulling.Get();
		}

		void GraphicDevice::UseWireFrameCullBackRasterizer() noexcept
		{
			_currentRasterizerFor3D = _rasterizerStateWireFrameCullBack.Get();
		}

		void GraphicDevice::UseSolidCullBackRasterizer() noexcept
		{
			_currentRasterizerFor3D = _rasterizerStateSolidCullBack.Get();
		}

		const Rect& GraphicDevice::GetFullScreenClipRect() const noexcept
		{
			return _fullScreenClipRect;
		}

		void GraphicDevice::Initialize2DProjectionMatrix() noexcept
		{
			const Float2 windowSize{ GetWindowSize() };
			_cbViewData._cb2DProjectionMatrix = Float4x4::ProjectionMatrix2DFromTopLeft(windowSize._x, windowSize._y);
			_cbViewData._cbViewProjectionMatrix = _cbViewData._cb2DProjectionMatrix * _cbViewData._cbViewMatrix;

			DxResource& cbView = _resourcePool.GetResource(_cbViewID);
			cbView.UpdateBuffer(&_cbViewData, 1);
		}

		void GraphicDevice::SetViewProjectionMatrix(const Float4x4& viewMatrix, const Float4x4& ProjectionMatrix) noexcept
		{
			_cbViewData._cbViewMatrix = viewMatrix;
			_cbViewData._cb3DProjectionMatrix = ProjectionMatrix;
			_cbViewData._cbViewProjectionMatrix = _cbViewData._cb3DProjectionMatrix * _cbViewData._cbViewMatrix;

			DxResource& cbView = _resourcePool.GetResource(_cbViewID);
			cbView.UpdateBuffer(&_cbViewData, 1);
		}

		const Int2& GraphicDevice::GetWindowSize() const noexcept
		{
			return _window.GetSize();
		}

		Window& GraphicDevice::AccessWindow() noexcept
		{
			return _window;
		}

		const Window& GraphicDevice::GetWindow() const noexcept
		{
			return _window;
		}
	}
}
